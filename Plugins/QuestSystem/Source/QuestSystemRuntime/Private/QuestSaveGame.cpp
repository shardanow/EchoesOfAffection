// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestSaveGame.h"

UQuestSaveGame::UQuestSaveGame()
{
	SaveSlotName = TEXT("QuestSaveSlot");
	UserIndex = 0;
	SaveVersion = 1;
}

FQuestSaveData UQuestSaveGame::GetQuestData(FName QuestId) const
{
	for (const FQuestSaveData& Data : QuestProgress)
	{
		if (Data.QuestId == QuestId)
		{
			return Data;
		}
	}
	return FQuestSaveData();
}

void UQuestSaveGame::SetQuestData(const FQuestSaveData& QuestData)
{
	// Find existing and update, or add new
	for (int32 i = 0; i < QuestProgress.Num(); ++i)
	{
		if (QuestProgress[i].QuestId == QuestData.QuestId)
		{
			QuestProgress[i] = QuestData;
			return;
		}
	}
	
	// Not found, add new
	QuestProgress.Add(QuestData);
}

bool UQuestSaveGame::HasQuestData(FName QuestId) const
{
	for (const FQuestSaveData& Data : QuestProgress)
	{
		if (Data.QuestId == QuestId)
		{
			return true;
		}
	}
	return false;
}

void UQuestSaveGame::RemoveQuestData(FName QuestId)
{
	QuestProgress.RemoveAll([QuestId](const FQuestSaveData& Data)
	{
		return Data.QuestId == QuestId;
	});
}

TArray<FQuestSaveData> UQuestSaveGame::GetQuestsByState(EQuestState State) const
{
	TArray<FQuestSaveData> Result;
	for (const FQuestSaveData& Data : QuestProgress)
	{
		if (Data.State == State)
		{
			Result.Add(Data);
		}
	}
	return Result;
}
