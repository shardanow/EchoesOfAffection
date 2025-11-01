// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "QuestTypes.h"
#include "QuestSaveGame.generated.h"

/**
 * Objective Save Data - Progress for single objective
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FObjectiveSaveData
{
	GENERATED_BODY()

	/** Objective ID */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FName ObjectiveId;

	/** Current progress count */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	int32 CurrentCount = 0;

	/** Is objective completed */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	bool bIsCompleted = false;

	/** Timestamp when objective was started */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FDateTime StartTime;

	/** Timestamp when objective was completed */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FDateTime CompletionTime;

	/** Custom data for specific objective types */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	TMap<FName, FString> CustomData;

	FObjectiveSaveData() = default;
};

/**
 * Phase Save Data - Progress for single quest phase
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FPhaseSaveData
{
	GENERATED_BODY()

	/** Phase ID */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FName PhaseId;

	/** Is phase active */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	bool bIsActive = false;

	/** Is phase completed */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	bool bIsCompleted = false;

	/** Objective progress */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	TArray<FObjectiveSaveData> Objectives;

	/** Timestamp when phase was started */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FDateTime StartTime;

	/** Timestamp when phase was completed */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FDateTime CompletionTime;

	FPhaseSaveData() = default;
};

/**
 * Quest Save Data - Complete progress for single quest
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestSaveData
{
	GENERATED_BODY()

	/** Quest ID */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FName QuestId;

	/** Current quest state */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	EQuestState State = EQuestState::NotStarted;

	/** Current active phase ID */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FName CurrentPhaseId;

	/** Phase progress data */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	TArray<FPhaseSaveData> Phases;

	/** Timestamp when quest was started */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FDateTime StartTime;

	/** Timestamp when quest was completed/failed */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FDateTime EndTime;

	/** Time elapsed on quest (for time-limited quests) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	float ElapsedTime = 0.0f;

	/** Number of times quest was completed (for repeatable quests) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	int32 CompletionCount = 0;

	/** Custom quest variables */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	TMap<FName, FString> QuestVariables;

	/** Chosen branch paths (for branching quests) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	TArray<FName> ChosenBranches;

	FQuestSaveData() = default;
};

/**
 * Quest Save Game - Main save game object for quest system
 */
UCLASS(BlueprintType)
class QUESTSYSTEMRUNTIME_API UQuestSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UQuestSaveGame();

	/** Save slot name */
	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FString SaveSlotName;

	/** Save index */
	UPROPERTY(BlueprintReadWrite, Category = "Save")
	int32 UserIndex = 0;

	/** All quest progress data */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	TArray<FQuestSaveData> QuestProgress;

	/** Global quest-related tags (unlocked content, flags, etc) */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	TSet<FName> GlobalQuestTags;

	/** Global quest variables */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	TMap<FName, FString> GlobalVariables;

	/** Last save timestamp */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	FDateTime LastSaveTime;

	/** Save format version for migrations */
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
	int32 SaveVersion = 1;

	// Helper methods

	/** Get quest save data by ID */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	FQuestSaveData GetQuestData(FName QuestId) const;

	/** Set quest save data */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void SetQuestData(const FQuestSaveData& QuestData);

	/** Check if quest exists in save */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	bool HasQuestData(FName QuestId) const;

	/** Remove quest data */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void RemoveQuestData(FName QuestId);

	/** Get all quests with specific state */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	TArray<FQuestSaveData> GetQuestsByState(EQuestState State) const;
};
