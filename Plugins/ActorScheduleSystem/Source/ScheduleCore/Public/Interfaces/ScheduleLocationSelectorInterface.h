// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ScheduleTypes.h"
#include "ScheduleLocationSelectorInterface.generated.h"

/**
 * Interface for location selectors that determine where actions should occur
 * Implement this to create custom location selection strategies (roads, POIs, zones, etc.)
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UScheduleLocationSelectorInterface : public UInterface
{
	GENERATED_BODY()
};

class SCHEDULECORE_API IScheduleLocationSelectorInterface
{
	GENERATED_BODY()

public:
	/**
	 * Select a location for action execution
	 * @param Context - The selection context with actor and world data
	 * @param OutLocation - The selected location
	 * @return True if a valid location was selected
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	bool SelectLocation(const FScheduleLocationContext& Context, FScheduleLocation& OutLocation);
	virtual bool SelectLocation_Implementation(const FScheduleLocationContext& Context, FScheduleLocation& OutLocation) { return false; }

	/**
	 * Get multiple potential locations (for batch evaluation or visualization)
	 * @param Context - The selection context
	 * @param MaxLocations - Maximum number of locations to return
	 * @param OutLocations - Array of potential locations
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	void GetPotentialLocations(const FScheduleLocationContext& Context, int32 MaxLocations, TArray<FScheduleLocation>& OutLocations);
	virtual void GetPotentialLocations_Implementation(const FScheduleLocationContext& Context, int32 MaxLocations, TArray<FScheduleLocation>& OutLocations) {}

	/**
	 * Get a human-readable description of this selector
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	FText GetSelectorDescription() const;
	virtual FText GetSelectorDescription_Implementation() const { return FText::FromString(TEXT("Custom Selector")); }
};
