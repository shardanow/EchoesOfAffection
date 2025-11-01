// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestTypes.h"
#include "QuestSaveGame.h"
#include "QuestJournalWidget.generated.h"

class UQuestAsset;
class UQuestSubsystem;

/**
 * Base class for Quest Journal UI
 * Extend in Blueprint to create custom journal
 */
UCLASS(Abstract, Blueprintable)
class QUESTSYSTEMRUNTIME_API UQuestJournalWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UQuestJournalWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface

	// Public API

	UFUNCTION(BlueprintCallable, Category = "Quest Journal")
	void RefreshQuestList();

	UFUNCTION(BlueprintCallable, Category = "Quest Journal")
	void SetSelectedQuest(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Quest Journal")
	void FilterByState(EQuestState State);

	UFUNCTION(BlueprintCallable, Category = "Quest Journal")
	void FilterByCategory(FGameplayTag CategoryTag);

	UFUNCTION(BlueprintCallable, Category = "Quest Journal")
	void ClearFilters();

	// Blueprint Implementable Events

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest Journal")
	void OnQuestListUpdated(const TArray<FQuestSaveData>& Quests);

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest Journal")
	void OnQuestSelected(FName QuestId, UQuestAsset* QuestAsset);

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest Journal")
	void OnQuestProgressChanged(FName QuestId);

	protected:
	/** Currently selected quest */
	UPROPERTY(BlueprintReadOnly, Category = "Quest Journal")
	FName SelectedQuestId;

	/** Current filter state */
	UPROPERTY(BlueprintReadOnly, Category = "Quest Journal")
	EQuestState FilterState = EQuestState::Active;

	/** Current filter category */
	UPROPERTY(BlueprintReadOnly, Category = "Quest Journal")
	FGameplayTag FilterCategory;

	/** Cached quest list */
	UPROPERTY(BlueprintReadOnly, Category = "Quest Journal")
	TArray<FQuestSaveData> CachedQuests;

	/** Quest subsystem reference */
	UPROPERTY()
	TObjectPtr<UQuestSubsystem> QuestSubsystem;

	// Event handlers
	UFUNCTION()
	void OnQuestStartedHandler(FName QuestId, UQuestAsset* QuestAsset);

	UFUNCTION()
	void OnQuestCompletedHandler(FName QuestId, UQuestAsset* QuestAsset);

	UFUNCTION()
	void OnObjectiveUpdatedHandler(FName QuestId, FName ObjectiveId);

	// Helper methods
	UQuestSubsystem* GetQuestSubsystem() const;
	void SubscribeToEvents();
	void UnsubscribeFromEvents();
	TArray<FQuestSaveData> GetFilteredQuests() const;
};
