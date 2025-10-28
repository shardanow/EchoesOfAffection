// Copyright Epic Games, Inc. All Rights Reserved.

#include "Selectors/ScheduleLocationSelectorBase.h"

bool UScheduleLocationSelectorBase::SelectLocation_Implementation(const FScheduleLocationContext& Context, FScheduleLocation& OutLocation)
{
	// Build context with our settings
	FScheduleLocationContext AugmentedContext = Context;
	AugmentedContext.RequiredTags.AppendTags(RequiredLocationTags);
	AugmentedContext.OptionalTags.AppendTags(OptionalLocationTags);
	
	if (MaxSearchDistance > 0.0f && AugmentedContext.MaxDistance == 0.0f)
	{
		AugmentedContext.MaxDistance = MaxSearchDistance;
	}

	return SelectLocationInternal(AugmentedContext, OutLocation);
}

void UScheduleLocationSelectorBase::GetPotentialLocations_Implementation(const FScheduleLocationContext& Context, int32 MaxLocations, TArray<FScheduleLocation>& OutLocations)
{
	// Build context with our settings
	FScheduleLocationContext AugmentedContext = Context;
	AugmentedContext.RequiredTags.AppendTags(RequiredLocationTags);
	AugmentedContext.OptionalTags.AppendTags(OptionalLocationTags);
	
	if (MaxSearchDistance > 0.0f && AugmentedContext.MaxDistance == 0.0f)
	{
		AugmentedContext.MaxDistance = MaxSearchDistance;
	}

	GetPotentialLocationsInternal(AugmentedContext, MaxLocations, OutLocations);
}

FText UScheduleLocationSelectorBase::GetSelectorDescription_Implementation() const
{
	if (!SelectorName.IsEmpty())
	{
		return SelectorName;
	}

	return NSLOCTEXT("ScheduleSystem", "UnnamedSelector", "Custom Location Selector");
}
