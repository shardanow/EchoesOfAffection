// Copyright Epic Games, Inc. All Rights Reserved.

#include "Serialization/DialogueSaveDataSerializer.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueState.h"
#include "Core/DialogueParticipants.h"
#include "Core/DialogueVariableStore.h"
#include "SaveGame/DialogueSaveData.h"
#include "GameFramework/Actor.h"

FDialogueSessionSaveData FDialogueSaveDataSerializer::ToSaveData(
    const UDialogueSessionContext* Context,
    FName DialogueId
) const
{
  FDialogueSessionSaveData SaveData;
  
    if (!Context)
    {
     UE_LOG(LogTemp, Error, TEXT("SaveDataSerializer::ToSaveData - Null context"));
  return SaveData;
    }

    SaveData.DialogueId = DialogueId;
    
    // Save current node ID
    UDialogueState* State = Context->GetState();
    if (State && State->GetCurrentNode())
    {
        SaveData.CurrentNodeId = State->GetCurrentNode()->NodeId;
    }
    else
    {
  SaveData.CurrentNodeId = NAME_None;
    }
    
    // Save visited nodes
    if (State)
    {
        SaveData.VisitedNodes = State->GetVisitedNodes();
    
        // Save session start time
      SaveData.SessionStartTime = State->GetSessionStartTime();
   
        // Convert conversation history to save format
        const TArray<FConversationEntry>& History = State->GetConversationHistory();
   for (const FConversationEntry& Entry : History)
        {
     FDialogueHistoryEntry HistoryEntry;
        HistoryEntry.SpeakerName = Entry.SpeakerName;
    HistoryEntry.DialogueText = Entry.DialogueText;
    HistoryEntry.NodeId = Entry.NodeId;
     HistoryEntry.Timestamp = Entry.Timestamp;
            SaveData.ConversationHistory.Add(HistoryEntry);
 }
  }
    
    // Save custom variables
  UDialogueVariableStore* Variables = Context->GetVariables();
    if (Variables)
    {
        TArray<FName> Keys = Variables->GetAllVariableKeys();
        for (const FName& Key : Keys)
   {
         SaveData.CustomVariables.Add(Key, Variables->GetCustomVariable(Key));
        }
        
        // Save active tags
 SaveData.ActiveTags = Variables->GetActiveTags();
    }
    
    // Save base affinity level
UDialogueParticipants* Participants = Context->GetParticipants();
    if (Participants)
    {
        SaveData.BaseAffinityLevel = Participants->GetBaseAffinityLevel();
  }
    
  // Save actor paths
    AActor* PlayerActor = Context->GetPlayer();
    AActor* NPCActor = Context->GetNPC();
    
    if (PlayerActor)
    {
    SaveData.PlayerPath = FSoftObjectPath(PlayerActor);
    }
    if (NPCActor)
    {
 SaveData.NPCPath = FSoftObjectPath(NPCActor);
    }
 
 UE_LOG(LogTemp, Verbose, TEXT("SaveDataSerializer: Saved to FDialogueSessionSaveData (Dialogue: %s, Nodes: %d)"),
  *DialogueId.ToString(), SaveData.VisitedNodes.Num());
    
    return SaveData;
}

bool FDialogueSaveDataSerializer::FromSaveData(
    UDialogueSessionContext* Context,
    const FDialogueSessionSaveData& SaveData,
    AActor* InPlayer,
    AActor* InNPC
) const
{
    if (!Context)
    {
   UE_LOG(LogTemp, Error, TEXT("SaveDataSerializer::FromSaveData - Null context"));
     return false;
    }

    if (!SaveData.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("SaveDataSerializer::FromSaveData - Invalid save data"));
return false;
    }
    
 // Restore player and NPC references
 Context->SetPlayer(InPlayer);
    Context->SetNPC(InNPC);
 
    // Restore visited nodes
    UDialogueState* State = Context->GetState();
    if (State)
    {
   State->ClearVisitedNodes();
        for (const FName& NodeId : SaveData.VisitedNodes)
        {
 State->MarkNodeVisited(NodeId);
        }
        
    // Restore conversation history
        State->ClearHistory();
        for (const FDialogueHistoryEntry& HistoryEntry : SaveData.ConversationHistory)
        {
    State->AddToHistory(HistoryEntry.SpeakerName, HistoryEntry.DialogueText, HistoryEntry.NodeId);
   }
    }
    
    // Restore custom variables
    UDialogueVariableStore* Variables = Context->GetVariables();
    if (Variables)
    {
      Variables->ClearVariables();
        for (const auto& Pair : SaveData.CustomVariables)
  {
   Variables->SetCustomVariable(Pair.Key, Pair.Value);
        }
        
        // Restore active tags
     Variables->ClearTags();
     for (const FGameplayTag& Tag : SaveData.ActiveTags)
        {
     Variables->AddTag(Tag);
        }
    }
    
    // Restore base affinity level
    UDialogueParticipants* Participants = Context->GetParticipants();
    if (Participants)
    {
   Participants->SetBaseAffinityLevel(SaveData.BaseAffinityLevel);
    }
  
    // Note: CurrentNode will be restored by DialogueRunner
    
  UE_LOG(LogTemp, Log, TEXT("SaveDataSerializer: Loaded from FDialogueSessionSaveData (Dialogue: %s, Variables: %d)"),
        *SaveData.DialogueId.ToString(), SaveData.CustomVariables.Num());
 
    return true;
}
