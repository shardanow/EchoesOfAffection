// Copyright Epic Games, Inc. All Rights Reserved.

#include "Serialization/DialogueSnapshotSerializer.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueState.h"
#include "Core/DialogueParticipants.h"
#include "Core/DialogueVariableStore.h"

FDialogueStateSnapshot FDialogueSnapshotSerializer::CreateSnapshot(
    const UDialogueSessionContext* Context,
    uint8 DialogueState,
    const TArray<UDialogueNode*>& NodeHistory
) const
{
    if (!Context)
    {
    UE_LOG(LogTemp, Error, TEXT("SnapshotSerializer::CreateSnapshot - Null context"));
     return FDialogueStateSnapshot();
    }

    FDialogueStateSnapshot Snapshot;
    
    // Capture current node
    UDialogueState* State = Context->GetState();
    if (State && State->GetCurrentNode())
    {
        Snapshot.CurrentNodeId = State->GetCurrentNode()->NodeId;
    }
    else
    {
        Snapshot.CurrentNodeId = NAME_None;
    }
    
    // Capture node history
    Snapshot.NodeHistoryIds.Reserve(NodeHistory.Num());
    for (const UDialogueNode* Node : NodeHistory)
    {
   if (Node)
        {
      Snapshot.NodeHistoryIds.Add(Node->NodeId);
    }
    }
    
    // Capture custom variables from Variables component
    UDialogueVariableStore* Variables = Context->GetVariables();
    if (Variables)
    {
        TArray<FName> Keys = Variables->GetAllVariableKeys();
        for (const FName& Key : Keys)
    {
            Snapshot.CustomVariables.Add(Key, Variables->GetCustomVariable(Key));
        }
        
        // Capture active tags
        Snapshot.ActiveTags = Variables->GetActiveTags();
    }
    
    // Capture visited nodes
    if (State)
    {
        Snapshot.VisitedNodes = State->GetVisitedNodes();
      
        // Capture conversation history
        Snapshot.ConversationHistory = State->GetConversationHistory();
    }
    
    // Capture base affinity level
    UDialogueParticipants* Participants = Context->GetParticipants();
    if (Participants)
{
 Snapshot.BaseAffinityLevel = Participants->GetBaseAffinityLevel();
    }

    // Set metadata
    Snapshot.Timestamp = FDateTime::UtcNow();
    Snapshot.DialogueState = DialogueState;
    Snapshot.bIsValid = true;
    
    UE_LOG(LogTemp, Verbose, TEXT("SnapshotSerializer: Created snapshot (Size: ~%d bytes)"), 
        Snapshot.GetMemorySize());
    
    return Snapshot;
}

bool FDialogueSnapshotSerializer::RestoreFromSnapshot(
    UDialogueSessionContext* Context,
    const FDialogueStateSnapshot& Snapshot
) const
{
    if (!Context)
    {
        UE_LOG(LogTemp, Error, TEXT("SnapshotSerializer::RestoreFromSnapshot - Null context"));
        return false;
    }

    if (!Snapshot.IsValid())
    {
      UE_LOG(LogTemp, Warning, TEXT("SnapshotSerializer::RestoreFromSnapshot - Invalid snapshot"));
        return false;
    }
    
    // Restore visited nodes
 UDialogueState* State = Context->GetState();
    if (State)
    {
      State->ClearVisitedNodes();
        for (const FName& NodeId : Snapshot.VisitedNodes)
        {
         State->MarkNodeVisited(NodeId);
        }
     
        // Restore conversation history
        State->ClearHistory();
 for (const FConversationEntry& Entry : Snapshot.ConversationHistory)
        {
       State->AddToHistory(Entry.SpeakerName, Entry.DialogueText, Entry.NodeId);
   }
    }
    
    // Restore custom variables
UDialogueVariableStore* Variables = Context->GetVariables();
    if (Variables)
    {
        Variables->ClearVariables();
        for (const auto& Pair : Snapshot.CustomVariables)
   {
          Variables->SetCustomVariable(Pair.Key, Pair.Value);
    }
        
        // Restore active tags
        Variables->ClearTags();
        for (const FGameplayTag& Tag : Snapshot.ActiveTags)
      {
      Variables->AddTag(Tag);
        }
}
    
    // Restore base affinity level
    UDialogueParticipants* Participants = Context->GetParticipants();
    if (Participants)
    {
        Participants->SetBaseAffinityLevel(Snapshot.BaseAffinityLevel);
    }
    
    // Note: CurrentNode and NodeHistory are restored by DialogueRunner
    
    UE_LOG(LogTemp, Log, TEXT("SnapshotSerializer: Restored from snapshot (Age: %.3fs)"),
        (FDateTime::UtcNow() - Snapshot.Timestamp).GetTotalSeconds());
    
    return true;
}
