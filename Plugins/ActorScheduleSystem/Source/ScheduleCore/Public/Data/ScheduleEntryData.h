// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ScheduleTypes.h"
#include "ScheduleEntryData.generated.h"

class UScheduleConditionBase;
class UScheduleActionBase;
class UScheduleLocationSelectorBase;

/**
 * Data asset defining a single schedule entry (time + conditions + action)
 * This is the atomic unit of scheduling - designers configure these in the editor
 */
UCLASS(BlueprintType, Blueprintable)
class SCHEDULECORE_API UScheduleEntryData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Display name for this schedule entry */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule")
	FText EntryName;

	/** Detailed description of what this entry does */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule", meta = (MultiLine = true))
	FText Description;

	/** Priority for execution when multiple entries are valid (higher = first) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule", meta = (ClampMin = "0", ClampMax = "100"))
	int32 Priority = 50;

	/** Gameplay tags for this entry (used for filtering, querying, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule")
	FGameplayTagContainer EntryTags;

	/** Whether this entry is currently enabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule")
	bool bEnabled = true;

	/** How this entry recurs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Timing")
	EScheduleRecurrence RecurrenceType = EScheduleRecurrence::Daily;

	/** Custom recurrence interval in seconds (for Custom recurrence type) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Timing", meta = (EditCondition = "RecurrenceType == EScheduleRecurrence::Custom", ClampMin = "1"))
	float CustomRecurrenceInterval = 3600.0f;

	/** Time-based condition (when this entry should execute) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Timing")
	FScheduleTimeCondition TimeCondition;

	/** Additional conditions that must be met (AND logic) */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Schedule|Conditions")
	TArray<TObjectPtr<UScheduleConditionBase>> Conditions;

	/** Location selector for this entry (where the action occurs) */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Schedule|Location")
	TObjectPtr<UScheduleLocationSelectorBase> LocationSelector;

	/** The action to execute when conditions are met */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Schedule|Action")
	TObjectPtr<UScheduleActionBase> Action;

	/** Whether this action can be interrupted by player interaction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Execution")
	bool bCanBeInterrupted = true;

	/** Whether to resume this action after interruption */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Execution", meta = (EditCondition = "bCanBeInterrupted"))
	bool bResumeAfterInterruption = false;

	/** Maximum duration for this action (0 = unlimited) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Execution", meta = (ClampMin = "0", Units = "Seconds"))
	float MaxDuration = 0.0f;

public:
	/**
	 * Evaluate if this entry's conditions are currently met
	 * @param Context - Evaluation context
	 * @return True if all conditions are satisfied
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	bool EvaluateConditions(const FScheduleEvaluationContext& Context) const;

	/**
	 * Get the display text for editor/debug purposes
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	FText GetDisplayText() const;

	/** UObject interface */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
