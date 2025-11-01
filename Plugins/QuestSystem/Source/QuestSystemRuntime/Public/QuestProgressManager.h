// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestTypes.h"
#include "QuestStructures.h"
#include "QuestAsset.h"
#include "QuestSaveGame.h"
#include "QuestProgressManager.generated.h"

class UQuestEventBus;
class UQuestAssetLoader;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuestStateChangedSignature, FName, QuestId, EQuestState, OldState, EQuestState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStartedSignature, FName, QuestId, UQuestAsset*, QuestAsset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestCompletedSignature, FName, QuestId, UQuestAsset*, QuestAsset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestFailedSignature, FName, QuestId, UQuestAsset*, QuestAsset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveUpdatedSignature, FName, QuestId, FName, ObjectiveId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveCompletedSignature, FName, QuestId, FName, ObjectiveId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuestPhaseChangedSignature, FName, QuestId, FName, OldPhaseId, FName, NewPhaseId);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnQuestStartedCallback, bool, bSuccess, UQuestAsset*, QuestAsset);

/**
 * Quest Progress Manager - Manages quest state and progression
 * Single Responsibility: Quest lifecycle and progress tracking
 */
UCLASS(BlueprintType)
class QUESTSYSTEMRUNTIME_API UQuestProgressManager : public UObject
{
	GENERATED_BODY()

public:
	UQuestProgressManager();

	/** Initialize with dependencies */
	void Initialize(UQuestEventBus* InEventBus, UQuestAssetLoader* InAssetLoader);
	void Shutdown();

	// === Quest Lifecycle ===

	/**
	 * Start quest asynchronously
	 * @param QuestId - Quest to start
	 * @param OnStarted - Callback when quest starts (or fails)
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	void StartQuestAsync(FName QuestId, FOnQuestStartedCallback OnStarted);

	/**
	 * Complete active quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool CompleteQuest(FName QuestId);

	/**
	 * Fail active quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool FailQuest(FName QuestId);

	/**
	 * Abandon active quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool AbandonQuest(FName QuestId);

	/**
	 * Retry failed quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool RetryQuest(FName QuestId);

	// === State Queries ===

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	EQuestState GetQuestState(FName QuestId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	bool IsQuestActive(FName QuestId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	bool IsQuestCompleted(FName QuestId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	TArray<FQuestSaveData> GetActiveQuests() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	TArray<FQuestSaveData> GetCompletedQuests() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	FQuestSaveData GetQuestSaveData(FName QuestId) const;

	/**
	 * Get quest asset
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Assets")
	UQuestAsset* GetQuestAsset(FName QuestId) const;

	// === Objective Management ===

	/**
	 * Update objective progress
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool UpdateObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount);

	/**
	 * Set objective progress to specific value
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool SetObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Progress);

	/**
	 * Instantly complete objective
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool CompleteObjective(FName QuestId, FName ObjectiveId);

	/**
	 * Get objective progress
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	void GetObjectiveProgress(FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted) const;

	// === Phase Management ===

	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool AdvanceToNextPhase(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest|Progress")
	bool SetQuestPhase(FName QuestId, FName PhaseId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	FName GetCurrentPhaseId(FName QuestId) const;

	// === Dependencies ===

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Progress")
	bool CanStartQuest(FName QuestId, TArray<FText>& OutFailReasons) const;

	// === Events ===

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnQuestStateChangedSignature OnQuestStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnQuestStartedSignature OnQuestStarted;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnQuestCompletedSignature OnQuestCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnQuestFailedSignature OnQuestFailed;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnObjectiveUpdatedSignature OnObjectiveUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnObjectiveCompletedSignature OnObjectiveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Quest|Events")
	FOnQuestPhaseChangedSignature OnQuestPhaseChanged;

	// === Save Data Access ===

	void SetSaveGame(UQuestSaveGame* InSaveGame) { CurrentSaveGame = InSaveGame; }
	UQuestSaveGame* GetSaveGame() const { return CurrentSaveGame; }

protected:
	// Dependencies
	UPROPERTY()
	TObjectPtr<UQuestEventBus> EventBus;

	UPROPERTY()
	TObjectPtr<UQuestAssetLoader> AssetLoader;

	// Save data
	UPROPERTY()
	TObjectPtr<UQuestSaveGame> CurrentSaveGame;

	// Quest timers
	TMap<FName, FTimerHandle> QuestTimers;

	// Internal methods
	void OnQuestAssetLoaded(UQuestAsset* QuestAsset, FName QuestId, FOnQuestStartedCallback OnStarted);
	bool StartQuestInternal(FName QuestId, UQuestAsset* QuestAsset);
	bool CheckDependencies(const UQuestAsset* QuestAsset, TArray<FText>& OutFailReasons) const;
	bool CheckProgressGates(const FQuestObjective& Objective) const;
	bool EvaluateCondition(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const;

	void StartQuestTimer(FName QuestId, const UQuestAsset* QuestAsset);
	void OnQuestTimerExpired(FName QuestId);

	FQuestSaveData& GetOrCreateQuestSaveData(FName QuestId);
	FPhaseSaveData& GetOrCreatePhaseSaveData(FQuestSaveData& QuestData, FName PhaseId);
	FObjectiveSaveData& GetOrCreateObjectiveSaveData(FPhaseSaveData& PhaseData, FName ObjectiveId);
};
