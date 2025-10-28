// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/ScheduleLocationSelectorInterface.h"
#include "ScheduleLocationSelectorBase.generated.h"

/**
 * Base class for location selectors
 * Extend this to create custom location selection strategies (roads, POIs, zones, etc.)
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class SCHEDULECORE_API UScheduleLocationSelectorBase : public UObject, public IScheduleLocationSelectorInterface
{
	GENERATED_BODY()

public:
	/** Human-readable name for this selector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector")
	FText SelectorName;

	/** Required location tags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector")
	FGameplayTagContainer RequiredLocationTags;

	/** Optional location tags (preferred but not required) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector")
	FGameplayTagContainer OptionalLocationTags;

	/** Maximum search distance from actor (0 = unlimited) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector", meta = (ClampMin = "0"))
	float MaxSearchDistance = 0.0f;

public:
	/** IScheduleLocationSelectorInterface implementation */
	virtual bool SelectLocation_Implementation(const FScheduleLocationContext& Context, FScheduleLocation& OutLocation) override;
	virtual void GetPotentialLocations_Implementation(const FScheduleLocationContext& Context, int32 MaxLocations, TArray<FScheduleLocation>& OutLocations) override;
	virtual FText GetSelectorDescription_Implementation() const override;

protected:
	/**
	 * Override this in child classes to implement location selection logic
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Selector", meta = (DisplayName = "Select Location"))
	bool SelectLocationInternal(const FScheduleLocationContext& Context, FScheduleLocation& OutLocation);
	virtual bool SelectLocationInternal_Implementation(const FScheduleLocationContext& Context, FScheduleLocation& OutLocation) { return false; }

	/**
	 * Override to provide multiple potential locations
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Selector", meta = (DisplayName = "Get Potential Locations"))
	void GetPotentialLocationsInternal(const FScheduleLocationContext& Context, int32 MaxLocations, TArray<FScheduleLocation>& OutLocations);
	virtual void GetPotentialLocationsInternal_Implementation(const FScheduleLocationContext& Context, int32 MaxLocations, TArray<FScheduleLocation>& OutLocations) {}
};
