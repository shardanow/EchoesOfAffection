// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "QuestCheatManager.generated.h"

/**
 * Quest Cheat Manager - Debug commands for testing
 * 
 * Usage in your PlayerController:
 * 1. Override GetCheatManager()
 * 2. Add UQuestCheatManager to CheatClass
 * 
 * Or use directly:
 * - Add to existing CheatManager as parent class
 * 
 * Console Commands:
 * - StartQuest <QuestID>
 * - CompleteQuest <QuestID>
 * - FailQuest <QuestID>
 * - ResetQuests
 * - ShowQuestSaves
 * - SaveQuests
 * - LoadQuests
 * - EnableAutoSave <Interval>
 * - DisableAutoSave
 */
UCLASS(Within=PlayerController, Blueprintable)
class QUESTSYSTEMRUNTIME_API UQuestCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	/** Start quest by ID */
	UFUNCTION(Exec, Category = "Quest")
	void StartQuest(FName QuestId);

	/** Complete active quest */
	UFUNCTION(Exec, Category = "Quest")
	void CompleteQuest(FName QuestId);

	/** Fail active quest */
	UFUNCTION(Exec, Category = "Quest")
	void FailQuest(FName QuestId);

	/** Abandon active quest */
	UFUNCTION(Exec, Category = "Quest")
	void AbandonQuest(FName QuestId);

	/** Complete specific objective */
	UFUNCTION(Exec, Category = "Quest")
	void CompleteObjective(FName QuestId, FName ObjectiveId);

	/** Update objective progress */
	UFUNCTION(Exec, Category = "Quest")
	void UpdateObjective(FName QuestId, FName ObjectiveId, int32 Amount);

	/** Show all active quests */
	UFUNCTION(Exec, Category = "Quest")
	void ShowActiveQuests();

	/** Show all completed quests */
	UFUNCTION(Exec, Category = "Quest")
	void ShowCompletedQuests();

	/** Show detailed quest info */
	UFUNCTION(Exec, Category = "Quest")
	void ShowQuestInfo(FName QuestId);

	/** Delete all quest saves and restart */
	UFUNCTION(Exec, Category = "Quest")
	void ResetQuests();

	/** Show save file info */
	UFUNCTION(Exec, Category = "Quest")
	void ShowQuestSaves();

	/** Save quest progress manually */
	UFUNCTION(Exec, Category = "Quest")
	void SaveQuests();

	/** Load quest progress manually */
	UFUNCTION(Exec, Category = "Quest")
	void LoadQuests();

	/** Enable auto-save with interval */
	UFUNCTION(Exec, Category = "Quest")
	void EnableAutoSave(float IntervalSeconds = 60.0f);

	/** Disable auto-save */
	UFUNCTION(Exec, Category = "Quest")
	void DisableAutoSave();

	/** Emit test NPC talked event */
	UFUNCTION(Exec, Category = "Quest")
	void EmitNpcTalked(FName NpcId);

	/** Emit test item acquired event */
	UFUNCTION(Exec, Category = "Quest")
	void EmitItemAcquired(FName ItemId, int32 Count = 1);

protected:
	/** Get Quest Subsystem */
	class UQuestSubsystem* GetQuestSubsystem() const;

	/** Print colored message to screen */
	void PrintScreen(const FString& Message, FColor Color = FColor::Yellow, float Duration = 5.0f);
};
