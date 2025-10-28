// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/ScheduleEntryData.h"
#include "Conditions/ScheduleConditionBase.h"
#include "Interfaces/ScheduleConditionInterface.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

bool UScheduleEntryData::EvaluateConditions(const FScheduleEvaluationContext& Context) const
{
	if (!bEnabled)
	{
		return false;
	}

	// Evaluate time condition
	if (!TimeCondition.EvaluateTime(Context.CurrentHour, Context.CurrentMinute, Context.CurrentDayOfWeek))
	{
		return false;
	}

	// Evaluate all additional conditions (AND logic)
	for (const UScheduleConditionBase* Condition : Conditions)
	{
		// Use Execute_ wrapper for Blueprint interface compatibility
		if (Condition && !IScheduleConditionInterface::Execute_EvaluateCondition(Condition, Context))
		{
			return false;
		}
	}

	return true;
}

FText UScheduleEntryData::GetDisplayText() const
{
	if (!EntryName.IsEmpty())
	{
		return EntryName;
	}

	return FText::Format(
		NSLOCTEXT("ScheduleSystem", "DefaultEntryName", "Schedule Entry {0}"),
		FText::AsNumber(Priority)
	);
}

FPrimaryAssetId UScheduleEntryData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("ScheduleEntry"), GetFName());
}

#if WITH_EDITOR
EDataValidationResult UScheduleEntryData::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if (!Action)
	{
		Context.AddError(FText::FromString(TEXT("Schedule entry must have an Action assigned")));
		Result = EDataValidationResult::Invalid;
	}

	if (RecurrenceType == EScheduleRecurrence::Custom && CustomRecurrenceInterval <= 0.0f)
	{
		Context.AddError(FText::FromString(TEXT("Custom recurrence interval must be greater than 0")));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif
