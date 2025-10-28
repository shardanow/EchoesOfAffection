// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Selectors/ScheduleLocationSelectorBase.h"
#include "ScheduleLocationSelector_ActorTag.generated.h"

/** How to select from multiple valid locations */
UENUM(BlueprintType)
enum ELocationSelectionMode
{
	/** Select the closest location */
	Closest UMETA(DisplayName = "Closest"),
	/** Select a random location */
	Random UMETA(DisplayName = "Random"),
	/** Select the first found location */
	First UMETA(DisplayName = "First"),
	/** Select the farthest location */
	Farthest UMETA(DisplayName = "Farthest")
};

/**
 * Selector that finds actors with specific gameplay tags
 * Useful for finding POIs, shops, quest locations, etc.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Actor Tag Selector"))
class SCHEDULERUNTIME_API UScheduleLocationSelector_ActorTag : public UScheduleLocationSelectorBase
{
	GENERATED_BODY()

public:
	/** Class filter - only consider actors of this class (and children) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector")
	TSubclassOf<AActor> ActorClassFilter;

	/** How to select from multiple valid actors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector")
	TEnumAsByte<ELocationSelectionMode> SelectionMode;

	/** Random seed for randomized selection */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selector")
	int32 RandomSeed = 0;

protected:
	virtual bool SelectLocationInternal_Implementation(const FScheduleLocationContext& Context, FScheduleLocation& OutLocation) override;
	virtual void GetPotentialLocationsInternal_Implementation(const FScheduleLocationContext& Context, int32 MaxLocations, TArray<FScheduleLocation>& OutLocations) override;

private:
	/** Find all actors matching criteria */
	TArray<AActor*> FindMatchingActors(const FScheduleLocationContext& Context) const;
};
