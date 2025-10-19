// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueContext.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void UDialogueSessionContext::BeginDestroy()
{
    Super::BeginDestroy();
}

FString UDialogueSessionContext::GetCustomVariable(FName VariableName, const FString& DefaultValue) const
{
    if (const FString* Value = CustomVariables.Find(VariableName))
    {
        return *Value;
    }
    return DefaultValue;
}

void UDialogueSessionContext::SetCustomVariable(FName VariableName, const FString& Value)
{
    CustomVariables.Add(VariableName, Value);
}

void UDialogueSessionContext::AddTag(FGameplayTag Tag)
{
    ActiveTags.AddTag(Tag);
}

void UDialogueSessionContext::RemoveTag(FGameplayTag Tag)
{
    ActiveTags.RemoveTag(Tag);
}

bool UDialogueSessionContext::HasTag(FGameplayTag Tag) const
{
    return ActiveTags.HasTag(Tag);
}

void UDialogueSessionContext::AddToHistory(const FText& SpeakerName, const FText& DialogueText, FName NodeId)
{
    FConversationEntry Entry;
    Entry.SpeakerName = SpeakerName;
    Entry.DialogueText = DialogueText;
    Entry.NodeId = NodeId;
    Entry.Timestamp = FDateTime::Now();
    
    ConversationHistory.Add(Entry);
}

void UDialogueSessionContext::MarkNodeVisited(FName NodeId)
{
    VisitedNodes.AddUnique(NodeId);
}

bool UDialogueSessionContext::WasNodeVisited(FName NodeId) const
{
    return VisitedNodes.Contains(NodeId);
}

void UDialogueSessionContext::SaveToJson(FString& OutJson) const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    
    // Save custom variables
    TSharedPtr<FJsonObject> VarsObj = MakeShareable(new FJsonObject());
    for (const auto& Pair : CustomVariables)
    {
        VarsObj->SetStringField(Pair.Key.ToString(), Pair.Value);
    }
    JsonObject->SetObjectField(TEXT("CustomVariables"), VarsObj);
    
    // Save visited nodes
    TArray<TSharedPtr<FJsonValue>> VisitedArray;
    for (const FName& NodeId : VisitedNodes)
    {
        VisitedArray.Add(MakeShareable(new FJsonValueString(NodeId.ToString())));
    }
    JsonObject->SetArrayField(TEXT("VisitedNodes"), VisitedArray);
    
    // Serialize to string
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
}

bool UDialogueSessionContext::LoadFromJson(const FString& Json)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        return false;
    }
    
    // Load custom variables
    if (const TSharedPtr<FJsonObject>* VarsObj; JsonObject->TryGetObjectField(TEXT("CustomVariables"), VarsObj))
    {
        for (const auto& Pair : (*VarsObj)->Values)
        {
            CustomVariables.Add(FName(*Pair.Key), Pair.Value->AsString());
        }
    }
    
    // Load visited nodes
    const TArray<TSharedPtr<FJsonValue>>* VisitedArray;
    if (JsonObject->TryGetArrayField(TEXT("VisitedNodes"), VisitedArray))
    {
        for (const auto& Value : *VisitedArray)
        {
            VisitedNodes.Add(FName(*Value->AsString()));
        }
    }
    
    return true;
}
