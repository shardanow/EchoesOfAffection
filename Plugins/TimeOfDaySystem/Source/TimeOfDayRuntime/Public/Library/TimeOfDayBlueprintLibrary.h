// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/TimeOfDayTypes.h"
#include "TimeOfDayBlueprintLibrary.generated.h"

class UTimeOfDaySubsystem;

/**
 * Blueprint Function Library providing convenient access to Time of Day functionality
 */
UCLASS()
class TIMEOFDAYRUNTIME_API UTimeOfDayBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ========== Subsystem Access ==========

	/** Get the TimeOfDaySubsystem from the world context */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static UTimeOfDaySubsystem* GetTimeOfDaySubsystem(const UObject* WorldContextObject);

	// ========== Quick Access Functions ==========

	/** Get the current game time */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static FTimeOfDayTime GetCurrentTime(const UObject* WorldContextObject);

	/** Get the current day phase */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static ETimeOfDayPhase GetCurrentPhase(const UObject* WorldContextObject);

	/** Get normalized time of day (0-1) */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static float GetNormalizedTimeOfDay(const UObject* WorldContextObject);

	/** Get the current season */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static ETimeOfDaySeason GetCurrentSeason(const UObject* WorldContextObject);

	/** Check if it's currently daytime */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static bool IsDaytime(const UObject* WorldContextObject);

	/** Check if it's currently nighttime */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static bool IsNighttime(const UObject* WorldContextObject);

	/** Get formatted time string */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static FString GetFormattedTime(const UObject* WorldContextObject, bool bIncludeSeconds = false);

	/** Get formatted time string in 12-hour format with AM/PM */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static FString GetFormattedTime12Hour(const UObject* WorldContextObject, bool bIncludeSeconds = false);

	/** Get formatted date string */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static FString GetFormattedDate(const UObject* WorldContextObject);

	/** Get formatted date string with specified format */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static FString GetFormattedDateWithFormat(const UObject* WorldContextObject, EDateFormat Format);

	// ========== Time Control Functions ==========

	/** Set the current time */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static void SetCurrentTime(const UObject* WorldContextObject, const FTimeOfDayTime& NewTime);

	/** Advance time by the specified amount */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static void AdvanceTime(const UObject* WorldContextObject, float GameSeconds);

	/** Set time scale */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static void SetTimeScale(const UObject* WorldContextObject, float NewTimeScale);

	/** Get time scale */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static float GetTimeScale(const UObject* WorldContextObject);

	/** Pause time */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static void PauseTime(const UObject* WorldContextObject);

	/** Resume time */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static void ResumeTime(const UObject* WorldContextObject);

	/** Check if time is paused */
	UFUNCTION(BlueprintPure, Category = "Time of Day", meta = (WorldContext = "WorldContextObject"))
	static bool IsTimePaused(const UObject* WorldContextObject);

	// ========== Utility Functions ==========

	/** Create a game time from components */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static FTimeOfDayTime MakeGameTime(int32 Year, ETimeOfDaySeason Season, int32 DayOfSeason, int32 Hour, int32 Minute, int32 Second);

	/** Break a game time into components */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static void BreakGameTime(const FTimeOfDayTime& GameTime, int32& Year, ETimeOfDaySeason& Season, int32& DayOfSeason, 
		ETimeOfDayWeekday& DayOfWeek, int32& Hour, int32& Minute, int32& Second);

	/** Compare two game times */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static bool IsTimeAfter(const FTimeOfDayTime& TimeA, const FTimeOfDayTime& TimeB);

	/** Get time difference in game seconds */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static float GetTimeDifference(const FTimeOfDayTime& TimeA, const FTimeOfDayTime& TimeB);

	/** Convert hours to game seconds */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static float HoursToSeconds(float Hours);

	/** Convert days to game seconds */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static float DaysToSeconds(float Days);

	/** Get phase name as text */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static FText GetPhaseDisplayName(ETimeOfDayPhase Phase);

	/** Get season name as text */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static FText GetSeasonDisplayName(ETimeOfDaySeason Season);

	/** Get weekday name as text */
	UFUNCTION(BlueprintPure, Category = "Time of Day")
	static FText GetWeekdayDisplayName(ETimeOfDayWeekday Weekday);
};

