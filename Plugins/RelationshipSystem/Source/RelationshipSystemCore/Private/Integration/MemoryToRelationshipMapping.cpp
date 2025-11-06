// Copyright Epic Games, Inc. All Rights Reserved.

#include "Integration/MemoryToRelationshipMapping.h"

bool UMemoryToRelationshipMapping::FindActionForMemoryType(
	const FString& MemoryType, float Importance, FGameplayTag& OutActionTag) const
{
	for (const FMemoryActionMapping& Mapping : MemoryTypeActions)
	{
		if (Mapping.MemoryTypeName.Equals(MemoryType, ESearchCase::IgnoreCase))
		{

			if (Importance >= Mapping.MinImportance)
			{
				OutActionTag = Mapping.ActionTag;
				return true;
			}
		}
	}
	
	return false;
}

TArray<FEmotionDimensionModifier> UMemoryToRelationshipMapping::GetModifiersForEmotion(
	const FString& EmotionName) const
{
	for (const FEmotionMapping& Mapping : EmotionMappings)
	{
		if (Mapping.EmotionName.Equals(EmotionName, ESearchCase::IgnoreCase))
		{
			return Mapping.DimensionModifiers;
		}
	}
	
	return TArray<FEmotionDimensionModifier>();
}

bool UMemoryToRelationshipMapping::ShouldMemoryAffectRelationship(
	float Importance, float Freshness) const
{
	return Importance >= GlobalMinImportance && Freshness >= MinFreshness;
}
