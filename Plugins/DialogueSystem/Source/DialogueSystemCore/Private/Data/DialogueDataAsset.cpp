// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/DialogueDataAsset.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueContext.h"
#include "Conditions/DialogueConditionEvaluator.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

void UDialogueDataAsset::PostLoad()
{
    Super::PostLoad();
    RebuildIndex();
}

void UDialogueDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    
    if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UDialogueDataAsset, Nodes) ||
        PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UDialogueDataAsset, StartNode))
    {
        RebuildIndex();
    }
}

UDialogueNode* UDialogueDataAsset::FindNode(FName NodeId) const
{
    if (const int32* IndexPtr = NodeIndex.Find(NodeId))
    {
        if (Nodes.IsValidIndex(*IndexPtr))
        {
            return Nodes[*IndexPtr];
        }
    }
    return nullptr;
}

TArray<FDialogueEdgeData> UDialogueDataAsset::GetOutgoingEdges(FName FromNodeId) const
{
    TArray<FDialogueEdgeData> Result;
    
    for (const TObjectPtr<UDialogueNode>& Node : Nodes)
    {
        if (Node && Node->NodeId == FromNodeId)
        {
            for (const FDialogueConnection& Connection : Node->Connections)
            {
                FDialogueEdgeData Edge;
                Edge.FromNodeId = FromNodeId;
                Edge.ToNodeId = Connection.TargetNodeId;
                Edge.EdgeCondition = Connection.Condition ? Connection.Condition->GetDescription() : FString();
                Result.Add(Edge);
            }
            break;
        }
    }
    
    return Result;
}

FName UDialogueDataAsset::GetStartNode(const UDialogueSessionContext* Context) const
{
    return StartNode;
}

bool UDialogueDataAsset::ValidateGraph(TArray<FString>& OutErrors) const
{
    bool bValid = true;
    OutErrors.Empty();
    
    // Check for empty dialogue
    if (Nodes.Num() == 0)
    {
        OutErrors.Add(TEXT("Dialogue has no nodes"));
        bValid = false;
    }
    
    // Check for invalid start node
    if (StartNode.IsNone())
    {
        OutErrors.Add(TEXT("No default start node set"));
        bValid = false;
    }
    else
    {
        bool bStartNodeExists = false;
        for (const TObjectPtr<UDialogueNode>& Node : Nodes)
        {
            if (Node && Node->NodeId == StartNode)
            {
                bStartNodeExists = true;
                break;
            }
        }
        if (!bStartNodeExists)
        {
            OutErrors.Add(FString::Printf(TEXT("Start node does not exist")));
            bValid = false;
        }
    }
    
    // Check for duplicate node IDs
    TSet<FName> SeenNodeIds;
    for (const TObjectPtr<UDialogueNode>& Node : Nodes)
    {
        if (!Node)
        {
            OutErrors.Add(TEXT("Null node found in Nodes array"));
            bValid = false;
            continue;
        }
        
        if (SeenNodeIds.Contains(Node->NodeId))
        {
            OutErrors.Add(FString::Printf(TEXT("Duplicate node ID found")));
            bValid = false;
        }
        SeenNodeIds.Add(Node->NodeId);
    }
    
    // Check for connections pointing to non-existent nodes
    for (const TObjectPtr<UDialogueNode>& Node : Nodes)
    {
        if (!Node)
        {
            continue;
        }
        
        for (const FDialogueConnection& Connection : Node->Connections)
        {
            bool bTargetExists = false;
            
            for (const TObjectPtr<UDialogueNode>& TargetNode : Nodes)
            {
                if (TargetNode && TargetNode->NodeId == Connection.TargetNodeId)
                {
                    bTargetExists = true;
                    break;
                }
            }
            
            if (!bTargetExists)
            {
                OutErrors.Add(FString::Printf(TEXT("Node has connection to non-existent target node")));
                bValid = false;
            }
        }
    }
    
    // Check for unreachable nodes (except start)
    TSet<FName> ReachableNodes;
    TArray<FName> NodesToCheck;
    NodesToCheck.Add(StartNode);
    
    while (NodesToCheck.Num() > 0)
    {
        FName CurrentNodeId = NodesToCheck.Pop();
        if (ReachableNodes.Contains(CurrentNodeId))
        {
            continue;
        }
        
        ReachableNodes.Add(CurrentNodeId);
        
        // Find the node
        for (const TObjectPtr<UDialogueNode>& Node : Nodes)
        {
            if (Node && Node->NodeId == CurrentNodeId)
            {
                // Add connected nodes
                for (const FDialogueConnection& Connection : Node->Connections)
                {
                    if (!ReachableNodes.Contains(Connection.TargetNodeId))
                    {
                        NodesToCheck.Add(Connection.TargetNodeId);
                    }
                }
                break;
            }
        }
    }
    
    for (const TObjectPtr<UDialogueNode>& Node : Nodes)
    {
        if (Node && !ReachableNodes.Contains(Node->NodeId) && Node->NodeId != StartNode)
        {
            OutErrors.Add(FString::Printf(TEXT("Unreachable node found")));
            // This is a warning, not an error
        }
    }
    
    return bValid;
}

FString UDialogueDataAsset::ExportToJson() const
{
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    
    // Metadata
    RootObject->SetStringField(TEXT("DialogueId"), DialogueId.ToString());
    RootObject->SetStringField(TEXT("DisplayName"), DisplayName.ToString());
    RootObject->SetStringField(TEXT("Description"), Description.ToString());
    RootObject->SetNumberField(TEXT("Version"), Version);
    RootObject->SetStringField(TEXT("StartNode"), StartNode.ToString());
    
    // Nodes array
    TArray<TSharedPtr<FJsonValue>> NodesArray;
    for (const TObjectPtr<UDialogueNode>& Node : Nodes)
    {
        if (!Node)
        {
            continue;
        }
        
        TSharedPtr<FJsonObject> NodeObject = MakeShareable(new FJsonObject());
        NodeObject->SetStringField(TEXT("NodeId"), Node->NodeId.ToString());
        NodeObject->SetStringField(TEXT("Type"), UEnum::GetValueAsString(Node->NodeType));
        NodeObject->SetStringField(TEXT("Text"), Node->DialogueText.ToString());
        NodeObject->SetStringField(TEXT("SpeakerId"), Node->SpeakerId.ToString());
        
        // Connections
        TArray<TSharedPtr<FJsonValue>> ConnectionsArray;
        for (const FDialogueConnection& Connection : Node->Connections)
        {
            TSharedPtr<FJsonObject> ConnectionObject = MakeShareable(new FJsonObject());
            ConnectionObject->SetStringField(TEXT("TargetNodeId"), Connection.TargetNodeId.ToString());
            ConnectionObject->SetStringField(TEXT("Condition"), TEXT(""));
            if (Connection.Condition)
            {
                // TODO: Add condition serialization
            }
            ConnectionsArray.Add(MakeShareable(new FJsonValueObject(ConnectionObject)));
        }
        NodeObject->SetArrayField(TEXT("Connections"), ConnectionsArray);
        
        NodesArray.Add(MakeShareable(new FJsonValueObject(NodeObject)));
    }
    RootObject->SetArrayField(TEXT("Nodes"), NodesArray);
    
    // Serialize to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
    
    return OutputString;
}

bool UDialogueDataAsset::ImportFromJson(const FString& Json)
{
    // JSON import not fully implemented yet - would need to create UDialogueNode objects
    // This is a placeholder
    return false;
}

void UDialogueDataAsset::RebuildIndex()
{
    NodeIndex.Empty();
    
    for (int32 i = 0; i < Nodes.Num(); ++i)
    {
        if (Nodes[i])
        {
            NodeIndex.Add(Nodes[i]->NodeId, i);
        }
    }
}
