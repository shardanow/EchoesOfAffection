// Copyright Epic Games, Inc. All Rights Reserved.

#include "Serialization/DialogueJsonSerializer.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueState.h"
#include "Core/DialogueParticipants.h"
#include "Core/DialogueVariableStore.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void FDialogueJsonSerializer::SaveToJson(
    const UDialogueSessionContext* Context,
    FString& OutJson
) const
{
    if (!Context)
    {
   UE_LOG(LogTemp, Error, TEXT("JsonSerializer::SaveToJson - Null context"));
     OutJson = TEXT("{}");
        return;
  }

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
 
    // Save custom variables
    TSharedPtr<FJsonObject> VarsObj = MakeShareable(new FJsonObject());
    UDialogueVariableStore* Variables = Context->GetVariables();
    if (Variables)
    {
     TArray<FName> Keys = Variables->GetAllVariableKeys();
        for (const FName& Key : Keys)
        {
       VarsObj->SetStringField(Key.ToString(), Variables->GetCustomVariable(Key));
 }
  }
    JsonObject->SetObjectField(TEXT("CustomVariables"), VarsObj);
    
    // Save visited nodes
 TArray<TSharedPtr<FJsonValue>> VisitedArray;
    UDialogueState* State = Context->GetState();
  if (State)
    {
        for (const FName& NodeId : State->GetVisitedNodes())
        {
   VisitedArray.Add(MakeShareable(new FJsonValueString(NodeId.ToString())));
        }
    }
    JsonObject->SetArrayField(TEXT("VisitedNodes"), VisitedArray);
    
    // Save conversation history
    TArray<TSharedPtr<FJsonValue>> HistoryArray;
    if (State)
    {
        const TArray<FConversationEntry>& History = State->GetConversationHistory();
     for (const FConversationEntry& Entry : History)
        {
  TSharedPtr<FJsonObject> EntryObj = MakeShareable(new FJsonObject());
            EntryObj->SetStringField(TEXT("Speaker"), Entry.SpeakerName.ToString());
   EntryObj->SetStringField(TEXT("Text"), Entry.DialogueText.ToString());
     EntryObj->SetStringField(TEXT("NodeId"), Entry.NodeId.ToString());
  EntryObj->SetStringField(TEXT("Timestamp"), Entry.Timestamp.ToString());
      HistoryArray.Add(MakeShareable(new FJsonValueObject(EntryObj)));
      }
    }
    JsonObject->SetArrayField(TEXT("History"), HistoryArray);
    
    // Save active tags
    TArray<TSharedPtr<FJsonValue>> TagsArray;
    if (Variables)
    {
     for (const FGameplayTag& Tag : Variables->GetActiveTags())
        {
      TagsArray.Add(MakeShareable(new FJsonValueString(Tag.ToString())));
}
    }
    JsonObject->SetArrayField(TEXT("ActiveTags"), TagsArray);
  
    // Save other fields
    UDialogueParticipants* Participants = Context->GetParticipants();
    if (Participants)
  {
        JsonObject->SetNumberField(TEXT("BaseAffinity"), Participants->GetBaseAffinityLevel());
  }
    
    if (State)
    {
  JsonObject->SetStringField(TEXT("SessionStart"), State->GetSessionStartTime().ToString());
    }
    
    UDialogueNode* CurrentNodePtr = Context->GetCurrentNode();
  if (CurrentNodePtr)
    {
     JsonObject->SetStringField(TEXT("CurrentNode"), CurrentNodePtr->NodeId.ToString());
    }
    
    // Serialize to string
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    UE_LOG(LogTemp, Verbose, TEXT("JsonSerializer: Saved to JSON (Length: %d bytes)"), OutJson.Len());
}

bool FDialogueJsonSerializer::LoadFromJson(
    UDialogueSessionContext* Context,
    const FString& Json
) const
{
    if (!Context)
    {
  UE_LOG(LogTemp, Error, TEXT("JsonSerializer::LoadFromJson - Null context"));
   return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Json);
    
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
UE_LOG(LogTemp, Error, TEXT("JsonSerializer::LoadFromJson - Failed to deserialize JSON"));
        return false;
    }
    
    // Load custom variables
    UDialogueVariableStore* Variables = Context->GetVariables();
    if (const TSharedPtr<FJsonObject>* VarsObj; JsonObject->TryGetObjectField(TEXT("CustomVariables"), VarsObj))
    {
   if (Variables)
        {
     Variables->ClearVariables();
   for (const auto& Pair : (*VarsObj)->Values)
      {
      Variables->SetCustomVariable(FName(*Pair.Key), Pair.Value->AsString());
            }
        }
 }
    
    // Load visited nodes
 UDialogueState* State = Context->GetState();
    const TArray<TSharedPtr<FJsonValue>>* VisitedArray;
    if (JsonObject->TryGetArrayField(TEXT("VisitedNodes"), VisitedArray))
    {
   if (State)
     {
       State->ClearVisitedNodes();
  for (const auto& Value : *VisitedArray)
 {
        State->MarkNodeVisited(FName(*Value->AsString()));
    }
        }
    }
    
    // Load conversation history
    const TArray<TSharedPtr<FJsonValue>>* HistoryArray;
    if (JsonObject->TryGetArrayField(TEXT("History"), HistoryArray))
{
        if (State)
        {
       State->ClearHistory();
       for (const auto& Value : *HistoryArray)
            {
     const TSharedPtr<FJsonObject>* EntryObj;
          if (Value->TryGetObject(EntryObj))
         {
FText SpeakerName = FText::FromString((*EntryObj)->GetStringField(TEXT("Speaker")));
    FText DialogueText = FText::FromString((*EntryObj)->GetStringField(TEXT("Text")));
    FName NodeId = FName(*(*EntryObj)->GetStringField(TEXT("NodeId")));
      State->AddToHistory(SpeakerName, DialogueText, NodeId);
}
    }
        }
    }
    
    // Load active tags
    const TArray<TSharedPtr<FJsonValue>>* TagsArray;
 if (JsonObject->TryGetArrayField(TEXT("ActiveTags"), TagsArray))
    {
   if (Variables)
    {
        Variables->ClearTags();
   for (const auto& Value : *TagsArray)
 {
           FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*Value->AsString()), false);
      if (Tag.IsValid())
        {
    Variables->AddTag(Tag);
       }
 }
 }
    }
    
    // Load base affinity
    UDialogueParticipants* Participants = Context->GetParticipants();
    if (Participants && JsonObject->HasField(TEXT("BaseAffinity")))
    {
     float Affinity = JsonObject->GetNumberField(TEXT("BaseAffinity"));
   Participants->SetBaseAffinityLevel(Affinity);
    }
  
    // Note: CurrentNode will be restored by DialogueRunner
    
    UE_LOG(LogTemp, Log, TEXT("JsonSerializer: Loaded from JSON (Length: %d bytes)"), Json.Len());
    return true;
}
