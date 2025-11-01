// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestTypes.h"
#include "QuestAsset.h"
#include "QuestSaveGame.h"
#include "QuestLogComponent.generated.h"

// Forward declarations
class UQuestSubsystem;

/**
 * Quest Log Component - Player-side quest tracking
 * Attach to Player Controller, Player State, or Pawn
 */
UCLASS( ClassGroup=(Quest), meta=(BlueprintSpawnableComponent) )
class QUESTSYSTEMRUNTIME_API UQuestLogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestLogComponent();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

	// Quest Operations (delegates to subsystem)

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool StartQuest(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AbandonQuest(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool IsQuestActive(FName QuestId) const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool IsQuestCompleted(FName QuestId) const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	EQuestState GetQuestState(FName QuestId) const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FQuestSaveData> GetActiveQuests() const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FQuestSaveData> GetCompletedQuests() const;

	UFUNCTION(BlueprintCallable, Category = "Quest")
	UQuestAsset* GetQuestAsset(FName QuestId) const;

	// Objective queries

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void GetObjectiveProgress(FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted) const;

	// Tracked quest management

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SetTrackedQuest(FName QuestId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	FName GetTrackedQuest() const { return TrackedQuestId; }

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void ClearTrackedQuest();

	// UI Helpers

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	FText GetCurrentObjectiveText(FName QuestId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	TArray<FText> GetActiveObjectiveTexts(FName QuestId) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest")
	float GetQuestCompletionPercent(FName QuestId) const;

	// Event subscription

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void SubscribeToQuestEvents();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void UnsubscribeFromQuestEvents();

protected:
	/** Currently tracked quest for HUD */
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	FName TrackedQuestId;

	/** Cached subsystem reference */
	UPROPERTY()
	TObjectPtr<UQuestSubsystem> QuestSubsystem;

	/** Is subscribed to events */
	bool bIsSubscribed = false;

	// Event handlers
	UFUNCTION()
	void OnQuestStartedHandler(FName QuestId, UQuestAsset* QuestAsset);

	UFUNCTION()
	void OnQuestCompletedHandler(FName QuestId, UQuestAsset* QuestAsset);

	UFUNCTION()
	void OnQuestFailedHandler(FName QuestId, UQuestAsset* QuestAsset);

	UFUNCTION()
	void OnObjectiveUpdatedHandler(FName QuestId, FName ObjectiveId);

	UFUNCTION()
	void OnObjectiveCompletedHandler(FName QuestId, FName ObjectiveId);

	// Get subsystem
	UQuestSubsystem* GetQuestSubsystem() const;
};
