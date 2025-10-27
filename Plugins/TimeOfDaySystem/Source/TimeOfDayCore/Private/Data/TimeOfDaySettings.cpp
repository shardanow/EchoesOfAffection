// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/TimeOfDaySettings.h"

UTimeOfDaySettings::UTimeOfDaySettings()
{
	// Initialize with sensible defaults
	TimeConversion.RealSecondsPerGameDay = 1200.0f; // 20 minutes real time = 1 day
	TimeConversion.TimeScale = 1.0f;
	TimeConversion.bIsPaused = false;

	Calendar.DaysPerSeason = 90;  // 3 months ? 30 days = 90 days per season
	Calendar.SeasonsPerYear = 4;
	Calendar.DaysPerWeek = 7;

	// IMPORTANT: Default to January 1st (start of calendar year)
	// January 1st = Winter season, day 31
	// (Winter includes: December 1-30, January 31-60, February 61-90)
	InitialGameTime.Year = 1;
	InitialGameTime.Season = ETimeOfDaySeason::Winter;  // ? FIXED! Was Spring
	InitialGameTime.DayOfSeason = 31;   // ? FIXED! Was 1 (January 1st = Winter day 31)
	InitialGameTime.DayOfWeek = ETimeOfDayWeekday::Monday;
	InitialGameTime.Hour = 6;
	InitialGameTime.Minute = 0;
	InitialGameTime.Second = 0;

	// Set up default day phases
	DayPhases.Empty();
	DayPhases.Add(FDayPhaseDefinition(ETimeOfDayPhase::Night, 0, 0));      // 00:00 - Night
	DayPhases.Add(FDayPhaseDefinition(ETimeOfDayPhase::Dawn, 5, 0));       // 05:00 - Dawn
	DayPhases.Add(FDayPhaseDefinition(ETimeOfDayPhase::Morning, 7, 0));    // 07:00 - Morning
	DayPhases.Add(FDayPhaseDefinition(ETimeOfDayPhase::Noon, 11, 0));      // 11:00 - Noon
	DayPhases.Add(FDayPhaseDefinition(ETimeOfDayPhase::Afternoon, 14, 0)); // 14:00 - Afternoon
	DayPhases.Add(FDayPhaseDefinition(ETimeOfDayPhase::Dusk, 18, 0));      // 18:00 - Dusk
	DayPhases.Add(FDayPhaseDefinition(ETimeOfDayPhase::Evening, 20, 0));   // 20:00 - Evening

	bAutoAdvanceTime = true;
	UpdateFrequency = 10.0f;
	bPersistTimeState = true;
}

FDayPhaseDefinition UTimeOfDaySettings::GetPhaseForTime(int32 Hour, int32 Minute) const
{
	if (DayPhases.Num() == 0)
	{
		return FDayPhaseDefinition();
	}

	const int32 CurrentMinutes = Hour * 60 + Minute;
	
	// Find the appropriate phase
	FDayPhaseDefinition CurrentPhase = DayPhases[0];
	
	for (const FDayPhaseDefinition& Phase : DayPhases)
	{
		const int32 PhaseMinutes = Phase.GetStartMinutes();
		if (CurrentMinutes >= PhaseMinutes)
		{
			CurrentPhase = Phase;
		}
		else
		{
			break;
		}
	}
	
	return CurrentPhase;
}

ETimeOfDayPhase UTimeOfDaySettings::GetPhaseEnumForTime(int32 Hour, int32 Minute) const
{
	return GetPhaseForTime(Hour, Minute).Phase;
}

void UTimeOfDaySettings::ValidatePhases()
{
	// Sort phases by start time
	DayPhases.Sort([](const FDayPhaseDefinition& A, const FDayPhaseDefinition& B)
	{
		return A.GetStartMinutes() < B.GetStartMinutes();
	});

	// Ensure we have at least one phase
	if (DayPhases.Num() == 0)
	{
		DayPhases.Add(FDayPhaseDefinition(ETimeOfDayPhase::Morning, 0, 0));
	}

	// Ensure first phase starts at midnight
	if (DayPhases[0].GetStartMinutes() != 0)
	{
		DayPhases.Insert(FDayPhaseDefinition(DayPhases[0].Phase, 0, 0), 0);
	}
}

#if WITH_EDITOR
void UTimeOfDaySettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UTimeOfDaySettings, DayPhases))
	{
		ValidatePhases();
	}
}
#endif

