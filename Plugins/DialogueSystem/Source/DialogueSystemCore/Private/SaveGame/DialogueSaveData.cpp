// Copyright Epic Games, Inc. All Rights Reserved.

#include "SaveGame/DialogueSaveData.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SaveGame.h"
const FString UDialogueSaveGame::SaveSlotName = TEXT("DialogueSaveSlot");
const int32 UDialogueSaveGame::UserIndex = 0;

UDialogueSaveGame::UDialogueSaveGame()
    : LastSaveTime(FDateTime::Now())
    , SaveVersion(1)
{
}

void UDialogueSaveGame::SaveActiveSession(const FDialogueSessionSaveData& SessionData)
{
    ActiveSession = SessionData;
    LastSaveTime = FDateTime::Now();
}

void UDialogueSaveGame::ClearActiveSession()
{
    ActiveSession = FDialogueSessionSaveData();
}

void UDialogueSaveGame::MarkDialogueCompleted(FName DialogueId)
{
    if (!DialogueId.IsNone())
    {
        CompletedDialogues.AddUnique(DialogueId);
    }
}

bool UDialogueSaveGame::WasDialogueCompleted(FName DialogueId) const
{
    return CompletedDialogues.Contains(DialogueId);
}

void UDialogueSaveGame::IncrementNodeVisit(FName DialogueId, FName NodeId)
{
    if (DialogueId.IsNone() || NodeId.IsNone())
    {
        return;
    }

    FDialogueNodeVisitData* VisitData = GetOrCreateNodeVisitData(DialogueId);
    if (VisitData)
    {
        int32& Count = VisitData->NodeVisitCounts.FindOrAdd(NodeId, 0);
        Count++;
    }
}

int32 UDialogueSaveGame::GetNodeVisitCount(FName DialogueId, FName NodeId) const
{
    const FDialogueNodeVisitData* VisitData = FindNodeVisitData(DialogueId);
    if (VisitData)
    {
        if (const int32* Count = VisitData->NodeVisitCounts.Find(NodeId))
        {
            return *Count;
        }
    }
    return 0;
}

void UDialogueSaveGame::SetGlobalMemory(FName Key, bool Value)
{
    if (!Key.IsNone())
    {
        GlobalMemoryFlags.Add(Key, Value);
    }
}

bool UDialogueSaveGame::GetGlobalMemory(FName Key, bool DefaultValue) const
{
    if (const bool* Value = GlobalMemoryFlags.Find(Key))
    {
        return *Value;
    }
    return DefaultValue;
}

void UDialogueSaveGame::SetGlobalVariable(FName Key, const FString& Value)
{
    if (!Key.IsNone())
    {
        GlobalVariables.Add(Key, Value);
    }
}

FString UDialogueSaveGame::GetGlobalVariable(FName Key, const FString& DefaultValue) const
{
    if (const FString* Value = GlobalVariables.Find(Key))
    {
        return *Value;
    }
    return DefaultValue;
}

FDialogueNodeVisitData* UDialogueSaveGame::GetOrCreateNodeVisitData(FName DialogueId)
{
    for (FDialogueNodeVisitData& Data : NodeVisitData)
    {
        if (Data.DialogueId == DialogueId)
        {
            return &Data;
        }
    }
    FDialogueNodeVisitData NewData(DialogueId);
    NodeVisitData.Add(NewData);
    return &NodeVisitData.Last();
}

const FDialogueNodeVisitData* UDialogueSaveGame::FindNodeVisitData(FName DialogueId) const
{
    for (const FDialogueNodeVisitData& Data : NodeVisitData)
    {
        if (Data.DialogueId == DialogueId)
        {
            return &Data;
        }
    }
    return nullptr;
}

UDialogueSaveGame* UDialogueSaveGame::LoadOrCreateSaveGame(UObject* WorldContextObject)
{
    UDialogueSaveGame* SaveGameObject = nullptr;
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
    {
        SaveGameObject = Cast<UDialogueSaveGame>(
            UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex)
        );
    }
    if (!SaveGameObject)
    {
        SaveGameObject = Cast<UDialogueSaveGame>(
            UGameplayStatics::CreateSaveGameObject(UDialogueSaveGame::StaticClass())
        );
    }

    return SaveGameObject;
}

bool UDialogueSaveGame::SaveDialogueSaveGame(UObject* WorldContextObject, UDialogueSaveGame* SaveGameObject)
{
    if (!SaveGameObject)
    {
        return false;
    }

    SaveGameObject->LastSaveTime = FDateTime::Now();

    return UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveSlotName, UserIndex);
}

bool UDialogueSaveGame::DeleteDialogueSaveGame(UObject* WorldContextObject)
{
    return UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex);
}
