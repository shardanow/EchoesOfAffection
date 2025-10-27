// Copyright Epic Games, Inc. All Rights Reserved.

#include "Library/TimeOfDayBlueprintLibrary.h"
#include "Subsystems/TimeOfDaySubsystem.h"
#include "Engine/World.h"

// ========== Subsystem Access ==========

UTimeOfDaySubsystem* UTimeOfDayBlueprintLibrary::GetTimeOfDaySubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return World->GetSubsystem<UTimeOfDaySubsystem>();
	}

	return nullptr;
}

// ========== Quick Access Functions ==========

FTimeOfDayTime UTimeOfDayBlueprintLibrary::GetCurrentTime(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->BP_GetCurrentTime();  // ? FIXED! Use BP_ wrapper
	}
	return FTimeOfDayTime();
}

ETimeOfDayPhase UTimeOfDayBlueprintLibrary::GetCurrentPhase(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->BP_GetCurrentPhase();  // ? FIXED! Use BP_ wrapper
	}
	return ETimeOfDayPhase::Morning;
}

float UTimeOfDayBlueprintLibrary::GetNormalizedTimeOfDay(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->BP_GetNormalizedTimeOfDay();  // ? FIXED! Use BP_ wrapper
	}
	return 0.5f;
}

ETimeOfDaySeason UTimeOfDayBlueprintLibrary::GetCurrentSeason(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->BP_GetCurrentSeason();  // ? FIXED! Use BP_ wrapper
	}
	return ETimeOfDaySeason::Spring;
}

bool UTimeOfDayBlueprintLibrary::IsDaytime(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->IsDaytime();
	}
	return true;
}

bool UTimeOfDayBlueprintLibrary::IsNighttime(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->IsNighttime();
	}
	return false;
}

FString UTimeOfDayBlueprintLibrary::GetFormattedTime(const UObject* WorldContextObject, bool bIncludeSeconds)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->GetFormattedTime(bIncludeSeconds);
	}
	return TEXT("00:00");
}

FString UTimeOfDayBlueprintLibrary::GetFormattedTime12Hour(const UObject* WorldContextObject, bool bIncludeSeconds)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->GetFormattedTime12Hour(bIncludeSeconds);
	}
	return TEXT("12:00 AM");
}

FString UTimeOfDayBlueprintLibrary::GetFormattedDate(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->GetFormattedDate();
	}
	return TEXT("Unknown");
}

FString UTimeOfDayBlueprintLibrary::GetFormattedDateWithFormat(const UObject* WorldContextObject, EDateFormat Format)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->GetFormattedDateWithFormat(Format);
	}
	return TEXT("Unknown");
}

// ========== Time Control Functions ==========

void UTimeOfDayBlueprintLibrary::SetCurrentTime(const UObject* WorldContextObject, const FTimeOfDayTime& NewTime)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		Subsystem->BP_SetCurrentTime(NewTime);  // ? FIXED! Use BP_ wrapper
	}
}

void UTimeOfDayBlueprintLibrary::AdvanceTime(const UObject* WorldContextObject, float GameSeconds)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		Subsystem->BP_AdvanceTime(GameSeconds);  // ? FIXED! Use BP_ wrapper
	}
}

void UTimeOfDayBlueprintLibrary::SetTimeScale(const UObject* WorldContextObject, float NewTimeScale)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		Subsystem->BP_SetTimeScale(NewTimeScale);  // ? FIXED! Use BP_ wrapper
	}
}

float UTimeOfDayBlueprintLibrary::GetTimeScale(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->BP_GetTimeScale();  // ? FIXED! Use BP_ wrapper
	}
	return 1.0f;
}

void UTimeOfDayBlueprintLibrary::PauseTime(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		Subsystem->BP_PauseTime();  // ? FIXED! Use BP_ wrapper
	}
}

void UTimeOfDayBlueprintLibrary::ResumeTime(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		Subsystem->BP_ResumeTime();  // ? FIXED! Use BP_ wrapper
	}
}

bool UTimeOfDayBlueprintLibrary::IsTimePaused(const UObject* WorldContextObject)
{
	if (UTimeOfDaySubsystem* Subsystem = GetTimeOfDaySubsystem(WorldContextObject))
	{
		return Subsystem->BP_IsTimePaused();  // ? FIXED! Use BP_ wrapper
	}
	return false;
}

// ========== Utility Functions ==========

FTimeOfDayTime UTimeOfDayBlueprintLibrary::MakeGameTime(int32 Year, ETimeOfDaySeason Season, int32 DayOfSeason, 
	int32 Hour, int32 Minute, int32 Second)
{
	FTimeOfDayTime Time;
	Time.Year = Year;
	Time.Season = Season;
	Time.DayOfSeason = DayOfSeason;
	Time.Hour = Hour;
	Time.Minute = Minute;
	Time.Second = Second;
	return Time;
}

void UTimeOfDayBlueprintLibrary::BreakGameTime(const FTimeOfDayTime& GameTime, int32& Year, ETimeOfDaySeason& Season, 
	int32& DayOfSeason, ETimeOfDayWeekday& DayOfWeek, int32& Hour, int32& Minute, int32& Second)
{
	Year = GameTime.Year;
	Season = GameTime.Season;
	DayOfSeason = GameTime.DayOfSeason;
	DayOfWeek = GameTime.DayOfWeek;
	Hour = GameTime.Hour;
	Minute = GameTime.Minute;
	Second = GameTime.Second;
}

bool UTimeOfDayBlueprintLibrary::IsTimeAfter(const FTimeOfDayTime& TimeA, const FTimeOfDayTime& TimeB)
{
	return TimeA.TotalElapsedSeconds > TimeB.TotalElapsedSeconds;
}

float UTimeOfDayBlueprintLibrary::GetTimeDifference(const FTimeOfDayTime& TimeA, const FTimeOfDayTime& TimeB)
{
	return FMath::Abs(TimeA.TotalElapsedSeconds - TimeB.TotalElapsedSeconds);
}

float UTimeOfDayBlueprintLibrary::HoursToSeconds(float Hours)
{
	return Hours * 3600.0f;
}

float UTimeOfDayBlueprintLibrary::DaysToSeconds(float Days)
{
	return Days * 86400.0f;
}

FText UTimeOfDayBlueprintLibrary::GetPhaseDisplayName(ETimeOfDayPhase Phase)
{
	return StaticEnum<ETimeOfDayPhase>()->GetDisplayNameTextByValue((int64)Phase);
}

FText UTimeOfDayBlueprintLibrary::GetSeasonDisplayName(ETimeOfDaySeason Season)
{
	return StaticEnum<ETimeOfDaySeason>()->GetDisplayNameTextByValue((int64)Season);
}

FText UTimeOfDayBlueprintLibrary::GetWeekdayDisplayName(ETimeOfDayWeekday Weekday)
{
	return StaticEnum<ETimeOfDayWeekday>()->GetDisplayNameTextByValue((int64)Weekday);
}

