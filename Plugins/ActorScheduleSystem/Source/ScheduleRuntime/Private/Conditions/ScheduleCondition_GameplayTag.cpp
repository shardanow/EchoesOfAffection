// Copyright Epic Games, Inc. All Rights Reserved.

#include "Conditions/ScheduleCondition_GameplayTag.h"

bool UScheduleCondition_GameplayTag::EvaluateConditionInternal_Implementation(const FScheduleEvaluationContext& Context) const
{
	if (RequiredTags.IsEmpty())
	{
		return true;
	}

	if (bRequireAll)
	{
		return Context.ActiveTags.HasAll(RequiredTags);
	}
	else
	{
		return Context.ActiveTags.HasAny(RequiredTags);
	}
}

FText UScheduleCondition_GameplayTag::GetConditionDescription_Implementation() const
{
	if (!ConditionName.IsEmpty())
	{
		return Super::GetConditionDescription_Implementation();
	}

	if (RequiredTags.IsEmpty())
	{
		return NSLOCTEXT("ScheduleSystem", "NoTagCondition", "No Tags Required");
	}

	FString TagString;
	for (const FGameplayTag& Tag : RequiredTags)
	{
		if (!TagString.IsEmpty())
		{
			TagString += TEXT(", ");
		}
		TagString += Tag.ToString();
	}

	if (bRequireAll)
	{
		return FText::Format(
			NSLOCTEXT("ScheduleSystem", "RequireAllTags", "Has All Tags: {0}"),
			FText::FromString(TagString)
		);
	}
	else
	{
		return FText::Format(
			NSLOCTEXT("ScheduleSystem", "RequireAnyTag", "Has Any Tag: {0}"),
			FText::FromString(TagString)
		);
	}
}
