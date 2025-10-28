// Copyright Epic Games, Inc. All Rights Reserved.

#include "Conditions/ScheduleConditionBase.h"

bool UScheduleConditionBase::EvaluateCondition_Implementation(const FScheduleEvaluationContext& Context) const
{
	bool bResult = EvaluateConditionInternal(Context);
	return bInvertCondition ? !bResult : bResult;
}

FText UScheduleConditionBase::GetConditionDescription_Implementation() const
{
	if (!ConditionName.IsEmpty())
	{
		if (bInvertCondition)
		{
			return FText::Format(
				NSLOCTEXT("ScheduleSystem", "InvertedCondition", "NOT ({0})"),
				ConditionName
			);
		}
		return ConditionName;
	}

	return NSLOCTEXT("ScheduleSystem", "UnnamedCondition", "Custom Condition");
}
