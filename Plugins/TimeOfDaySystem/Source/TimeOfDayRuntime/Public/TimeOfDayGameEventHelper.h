// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TimeOfDayGameEventHelper.generated.h"

class UObject;
enum class ETimeOfDaySeason : uint8;

/**
 * Helper library for TimeOfDay System GameEventBus integration
 * This file contains utility functions to emit events from TimeOfDaySubsystem
 */
UCLASS()
class TIMEOFDAYRUNTIME_API UTimeOfDayGameEventHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Emit HourChanged event through GameEventBus (if available)
	 * Call this when game hour changes
	 */
	UFUNCTION(BlueprintCallable, Category = "Time|Events")
	static void EmitHourChangedEvent(UObject* WorldContext, int32 NewHour);

	/**
	 * Emit DayChanged event through GameEventBus (if available)
	 * Call this when game day changes
	 */
	UFUNCTION(BlueprintCallable, Category = "Time|Events")
	static void EmitDayChangedEvent(UObject* WorldContext, int32 NewDay);

	/**
	 * Emit SeasonChanged event through GameEventBus (if available)
	 * Call this when season changes
	 */
	UFUNCTION(BlueprintCallable, Category = "Time|Events")
	static void EmitSeasonChangedEvent(UObject* WorldContext, int32 NewSeason);

	/**
	 * Emit Sunrise event through GameEventBus (if available)
	 */
	UFUNCTION(BlueprintCallable, Category = "Time|Events")
	static void EmitSunriseEvent(UObject* WorldContext);

	/**
	 * Emit Sunset event through GameEventBus (if available)
	 */
	UFUNCTION(BlueprintCallable, Category = "Time|Events")
	static void EmitSunsetEvent(UObject* WorldContext);
};
