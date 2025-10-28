// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ScheduleData.generated.h"

class UScheduleEntryData;

/**
 * Primary data asset defining a complete schedule for an actor
 * Contains multiple schedule entries that define the actor's daily routine
 * Designers configure this in the editor and assign to actors via ScheduleComponent
 */
UCLASS(BlueprintType, Blueprintable)
class SCHEDULECORE_API UScheduleData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Display name for this schedule */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule")
	FText ScheduleName;

	/** Detailed description of this schedule */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule", meta = (MultiLine = true))
	FText Description;

	/** Gameplay tags for this schedule (e.g., Schedule.Type.Civilian, Schedule.Location.Town) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule")
	FGameplayTagContainer ScheduleTags;

	/** All schedule entries in this schedule */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule")
	TArray<TObjectPtr<UScheduleEntryData>> Entries;

	/** Default behavior when no entry conditions are met */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Schedule|Fallback")
	TObjectPtr<class UScheduleActionBase> FallbackAction;

	/** Whether to automatically sort entries by priority */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Settings")
	bool bAutoSortByPriority = true;

	/** Whether to allow multiple entries to execute simultaneously */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Settings")
	bool bAllowConcurrentEntries = false;

	/** Maximum number of concurrent entries (if allowed) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Schedule|Settings", meta = (EditCondition = "bAllowConcurrentEntries", ClampMin = "1"))
	int32 MaxConcurrentEntries = 1;

public:
	/**
	 * Get all entries sorted by priority (if auto-sort is enabled)
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	TArray<UScheduleEntryData*> GetSortedEntries() const;

	/**
	 * Find entries with specific tags
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	TArray<UScheduleEntryData*> FindEntriesByTags(const FGameplayTagContainer& Tags, bool bExactMatch = false) const;

	/** UObject interface */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
