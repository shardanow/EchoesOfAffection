// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuestTypes.h"
#include "QuestAsset.h"
#include "QuestProgressManager.h"
#include "QuestSubsystem.generated.h"

class UQuestProgressManager;
class UQuestRewardSystem;
class UQuestAssetLoader;
class UQuestEventBus;
class UQuestSaveManager;
class UQuestSaveGame;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnQuestStartedDelegate, bool, bSuccess, UQuestAsset*, QuestAsset);

/**
 * Quest Subsystem v2.0 - Coordinator Pattern
 * 
 * Responsibilities:
 * - Initialize and coordinate sub-systems
 * - Provide unified Blueprint API
 * - Route requests to appropriate managers
 * - Orchestrate save/load
 * 
 * Architecture:
 * - QuestAssetLoader: Async asset loading
 * - QuestProgressManager: Quest state & progression
 * - QuestRewardSystem: Reward distribution
 * - QuestSaveManager: Persistence
 * - QuestEventBus: Event routing
 */
UCLASS()
class QUESTSYSTEMRUNTIME_API UQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UQuestSubsystem();

	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	// === SUBSYSTEM ACCESS (Blueprint-friendly) ===

	/**
	 * Get Progress Manager - manages quest state and progression
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Systems")
	UQuestProgressManager* GetProgressManager() const { return ProgressManager; }

	/**
	 * Get Reward System - handles reward distribution
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Systems")
	UQuestRewardSystem* GetRewardSystem() const { return RewardSystem; }

	/**
	 * Get Asset Loader - handles async quest loading
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Systems")
	UQuestAssetLoader* GetAssetLoader() const { return AssetLoader; }

	/**
	 * Get Event Bus - handles event routing
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Systems")
	UQuestEventBus* GetEventBus() const { return EventBus; }

	/**
	 * Get Save Manager - handles persistence
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Systems")
	UQuestSaveManager* GetSaveManager() const { return SaveManager; }

	// === CONVENIENT API (Delegates to sub-systems) ===

	/**
	 * Start quest asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartQuestAsync(FName QuestId, FOnQuestStartedDelegate OnStarted);

	/**
	 * Complete quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool CompleteQuest(FName QuestId);

	/**
	 * Update objective progress
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool UpdateObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount = 1);

	/**
	 * Get objective progress
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	void GetObjectiveProgress(FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted) const;

	/**
	 * Check if quest is active
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	bool IsQuestActive(FName QuestId) const;

	/**
	 * Check if quest is completed
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	bool IsQuestCompleted(FName QuestId) const;

	/**
	 * Get active quests
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	TArray<FQuestSaveData> GetActiveQuests() const;

	/**
	 * Get completed quests
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	TArray<FQuestSaveData> GetCompletedQuests() const;

	/**
	 * Get quest asset
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	UQuestAsset* GetQuestAsset(FName QuestId) const;

	/**
	 * Get quest state
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	EQuestState GetQuestState(FName QuestId) const;

	/**
	 * Get quest save data
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	FQuestSaveData GetQuestSaveData(FName QuestId) const;

	/**
	 * Check if quest can be started
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool CanStartQuest(FName QuestId, TArray<FText>& OutFailReasons) const;

	/**
	 * Fail quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool FailQuest(FName QuestId);

	/**
	 * Abandon quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AbandonQuest(FName QuestId);

	/**
	 * Complete objective directly
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool CompleteObjective(FName QuestId, FName ObjectiveId);

	/**
	 * Get current phase ID
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	FName GetCurrentPhaseId(FName QuestId) const;

	/**
	 * Emit quest event
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest", meta = (AutoCreateRefTerm = "Payload"))
	void EmitQuestEvent(const FQuestEventPayload& Payload);

	/** Process phase completion rewards */
	void ProcessPhaseRewards(FName QuestId, FName PhaseId, const FQuestRewardSet& Rewards);

	// === SAVE/LOAD ORCHESTRATION ===

	/**
	 * Save quest progress asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void SaveAsync();

	/**
	 * Load quest progress asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void LoadAsync();

	/**
	 * Enable auto-save
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void EnableAutoSave(float IntervalSeconds = 60.0f);

	// === STATIC HELPER ===

	/**
	 * Get quest subsystem from world context
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static UQuestSubsystem* Get(const UObject* WorldContextObject);

	// === DIAGNOSTIC COMMANDS ===

	/**
	 * Console command: Print all active quests and their objectives
	 */
	UFUNCTION(Exec, Category = "Quest|Debug")
	void QuestDebugPrint();

	/**
	 * Console command: List all registered event listeners in QuestEventBus
	 */
	UFUNCTION(Exec, Category = "Quest|Debug")
	void QuestDebugListeners();

	/**
	 * Console command: Test event emission and see what triggers
	 */
	UFUNCTION(Exec, Category = "Quest|Debug")
	void QuestTestEvent(const FString& EventTag, const FString& StringParam, int32 IntParam);

protected:
	// === SUB-SYSTEMS (Single Responsibility) ===

	/** Manages quest state, progress, phases */
	UPROPERTY()
	TObjectPtr<UQuestProgressManager> ProgressManager;

	/** Handles reward distribution */
	UPROPERTY()
	TObjectPtr<UQuestRewardSystem> RewardSystem;

	/** Handles async asset loading */
	UPROPERTY()
	TObjectPtr<UQuestAssetLoader> AssetLoader;

	/** Routes events between systems */
	UPROPERTY()
	TObjectPtr<UQuestEventBus> EventBus;

	/** Manages save/load operations */
	UPROPERTY()
	TObjectPtr<UQuestSaveManager> SaveManager;

	/** Current save game instance */
	UPROPERTY()
	TObjectPtr<UQuestSaveGame> CurrentSaveGame;

	/** Bridge to GameEventBus (optional - only if plugin enabled) */
	UPROPERTY()
	TObjectPtr<UObject> GameEventBridge;

	// === INITIALIZATION ===

	void InitializeSubsystems();
	void ConnectSubsystems();
	void LoadInitialData();
	void InitializeGameEventBridge();

	// === INTERNAL EVENT HANDLERS ===

	UFUNCTION()
	void OnQuestCompletedInternal(FName QuestId, UQuestAsset* QuestAsset);

	UFUNCTION()
	void OnObjectiveCompletedInternal(FName QuestId, FName ObjectiveId);

	UFUNCTION()
	void OnPhaseChangedInternal(FName QuestId, FName OldPhaseId, FName NewPhaseId);

	UFUNCTION()
	void OnQuestStartedInternal(bool bSuccess, UQuestAsset* QuestAsset);

private:
	// Map to store pending callbacks for quest start operations
	TMap<FName, FOnQuestStartedDelegate> PendingQuestCallbacks;
};
