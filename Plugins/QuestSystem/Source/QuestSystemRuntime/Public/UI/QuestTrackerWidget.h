// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestTypes.h"
#include "QuestTrackerWidget.generated.h"

// Forward declarations
class UQuestSubsystem;
class UQuestAsset;
class UTextBlock;
class UProgressBar;
class UVerticalBox;

/**
 * Quest Tracker Widget - Ready-to-use HUD widget for tracking active quest
 * 
 * Usage in Blueprint:
 * 1. Create Widget Blueprint based on UQuestTrackerWidget
 * 2. Add named widgets in Designer:
 *    - TextBlock: "TextQuestTitle"
 *    - TextBlock: "TextObjective"
 *    - ProgressBar: "ProgressBar" (optional)
 * 3. Done! Widget will auto-update when quest progresses
 */
UCLASS(BlueprintType, Blueprintable)
class QUESTSYSTEMRUNTIME_API UQuestTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UQuestTrackerWidget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface

	/**
	 * Set tracked quest manually
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Tracker")
	void SetTrackedQuest(FName QuestId);

	/**
	 * Clear tracked quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Tracker")
	void ClearTrackedQuest();

	/**
	 * Get current tracked quest ID
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Tracker")
	FName GetTrackedQuestId() const { return TrackedQuestId; }

protected:
	// === Widget Bindings (auto-bind by name) ===

	/** Quest title text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextQuestTitle;

	/** Current objective text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TextObjective;

	/** Optional progress bar */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> ProgressBar;

	/** Optional container for multiple objectives */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> ObjectiveList;

	// === Configuration ===

	/** Auto-track first active quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Tracker")
	bool bAutoTrackFirstQuest = true;

	/** Show progress bar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Tracker")
	bool bShowProgressBar = true;

	/** Update interval (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Tracker")
	float UpdateInterval = 0.5f;

	// === Blueprint Events ===

	/**
	 * Called when tracked quest changes
	 * Override in Blueprint to customize appearance
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest Tracker")
	void OnTrackedQuestChanged(FName NewQuestId, UQuestAsset* QuestAsset);

	/**
	 * Called when quest progress updates
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest Tracker")
	void OnQuestProgressUpdated(FName QuestId, float CompletionPercent);

	/**
	 * Called when tracked quest completes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Quest Tracker")
	void OnTrackedQuestCompleted(FName QuestId);

protected:
	/** Currently tracked quest */
	UPROPERTY(BlueprintReadOnly, Category = "Quest Tracker")
	FName TrackedQuestId;

	/** Cached subsystem */
	UPROPERTY()
	TObjectPtr<UQuestSubsystem> QuestSubsystem;

	/** Update timer */
	FTimerHandle UpdateTimerHandle;

	// === Internal Methods ===

	/** Subscribe to quest events */
	void SubscribeToEvents();

	/** Unsubscribe from quest events */
	void UnsubscribeFromEvents();

	/** Refresh display */
	void RefreshDisplay();

	/** Update progress bar */
	void UpdateProgressBar();

	/** Event handlers */
	UFUNCTION()
	void OnQuestStartedHandler(FName QuestId, UQuestAsset* QuestAsset);

	UFUNCTION()
	void OnObjectiveUpdatedHandler(FName QuestId, FName ObjectiveId);

	UFUNCTION()
	void OnQuestCompletedHandler(FName QuestId, UQuestAsset* QuestAsset);

	/** Get quest subsystem */
	UQuestSubsystem* GetQuestSubsystem() const;
};
