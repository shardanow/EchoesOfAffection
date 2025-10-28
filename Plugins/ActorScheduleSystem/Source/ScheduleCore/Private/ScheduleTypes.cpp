// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScheduleTypes.h"

bool FScheduleTimeCondition::EvaluateTime(int32 CurrentHour, int32 CurrentMinute, int32 CurrentDayOfWeek) const
{
	// Check day of week if specified
	if (DaysOfWeek.Num() > 0 && !DaysOfWeek.Contains(0)) // 0 means all days
	{
		if (!DaysOfWeek.Contains(CurrentDayOfWeek))
		{
			UE_LOG(LogTemp, VeryVerbose, TEXT("       ? Day %d not in allowed days"), CurrentDayOfWeek);
			return false;
		}
	}

	switch (TimeType)
	{
	case EScheduleTimeType::TimeOfDay:
		{
			bool bMatches = CurrentHour == StartHour && CurrentMinute == StartMinute;
			UE_LOG(LogTemp, VeryVerbose, TEXT("       TimeOfDay: %02d:%02d == %02d:%02d ? %s"),
				CurrentHour, CurrentMinute, StartHour, StartMinute,
				bMatches ? TEXT("YES") : TEXT("NO"));
			return bMatches;
		}

	case EScheduleTimeType::TimeRange:
		{
			const int32 CurrentTotalMinutes = CurrentHour * 60 + CurrentMinute;
			const int32 StartTotalMinutes = StartHour * 60 + StartMinute;
			const int32 EndTotalMinutes = EndHour * 60 + EndMinute;

			bool bMatches;
			if (StartTotalMinutes <= EndTotalMinutes)
			{
				bMatches = CurrentTotalMinutes >= StartTotalMinutes && CurrentTotalMinutes <= EndTotalMinutes;
			}
			else // Range crosses midnight
			{
				bMatches = CurrentTotalMinutes >= StartTotalMinutes || CurrentTotalMinutes <= EndTotalMinutes;
			}
			
			UE_LOG(LogTemp, VeryVerbose, TEXT(" TimeRange: %02d:%02d (%d) in [%02d:%02d (%d) - %02d:%02d (%d)] ? %s"),
				CurrentHour, CurrentMinute, CurrentTotalMinutes,
				StartHour, StartMinute, StartTotalMinutes,
				EndHour, EndMinute, EndTotalMinutes,
				bMatches ? TEXT("YES") : TEXT("NO"));
			
			return bMatches;
		}

	case EScheduleTimeType::Duration:
	case EScheduleTimeType::GameTime:
		// These require external timing context
		UE_LOG(LogTemp, VeryVerbose, TEXT("       Duration/GameTime: always true (needs context)"));
		return true;

	default:
		UE_LOG(LogTemp, VeryVerbose, TEXT("       ? Unknown time type"));
		return false;
	}
}
