// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/ScheduleData.h"
#include "Data/ScheduleEntryData.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

TArray<UScheduleEntryData*> UScheduleData::GetSortedEntries() const
{
	TArray<UScheduleEntryData*> SortedEntries = Entries;

	if (bAutoSortByPriority)
	{
		SortedEntries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B)
		{
			// Handle null entries - push them to the end
			if (!A && !B) return false;
			if (!A) return false; // A is null, B comes first
			if (!B) return true;  // B is null, A comes first
			
			return A->Priority > B->Priority; // Higher priority first
		});
	}

	return SortedEntries;
}

TArray<UScheduleEntryData*> UScheduleData::FindEntriesByTags(const FGameplayTagContainer& Tags, bool bExactMatch) const
{
	TArray<UScheduleEntryData*> MatchingEntries;

	for (UScheduleEntryData* Entry : Entries)
	{
		if (!Entry) continue;

		if (bExactMatch)
		{
			if (Entry->EntryTags.HasAllExact(Tags))
			{
				MatchingEntries.Add(Entry);
			}
		}
		else
		{
			if (Entry->EntryTags.HasAny(Tags))
			{
				MatchingEntries.Add(Entry);
			}
		}
	}

	return MatchingEntries;
}

FPrimaryAssetId UScheduleData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Schedule"), GetFName());
}

#if WITH_EDITOR
EDataValidationResult UScheduleData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if (Entries.Num() == 0)
	{
		Context.AddWarning(FText::FromString(TEXT("Schedule has no entries")));
		Result = EDataValidationResult::NotValidated;
	}

	for (int32 i = 0; i < Entries.Num(); ++i)
	{
		if (!Entries[i])
		{
			Context.AddError(FText::Format(
				FText::FromString(TEXT("Entry at index {0} is null")),
				FText::AsNumber(i)
			));
			Result = EDataValidationResult::Invalid;
		}
	}

	if (bAllowConcurrentEntries && MaxConcurrentEntries < 1)
	{
		Context.AddError(FText::FromString(TEXT("Max concurrent entries must be at least 1")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

void UScheduleData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Auto-sort on priority change
	if (bAutoSortByPriority && PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UScheduleData, Entries))
	{
		Entries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B)
		{
			// Handle null entries - push them to the end
			if (!A && !B) return false;
			if (!A) return false; // A is null, B comes first
			if (!B) return true;  // B is null, A comes first
			
			return A->Priority > B->Priority;
		});
	}
}
#endif
