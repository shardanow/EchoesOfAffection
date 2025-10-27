// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Core/TimeOfDayTypes.h"
#include "TimeOfDayInterface.generated.h"

/**
 * Interface for objects that can respond to time changes
 * Implements the Observer pattern for time events
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UTimeOfDayListener : public UInterface
{
	GENERATED_BODY()
};

class TIMEOFDAYCORE_API ITimeOfDayListener
{
	GENERATED_BODY()

public:
	/** Called when a new minute starts */
	UFUNCTION(BlueprintNativeEvent, Category = "Time of Day")
	void OnMinuteChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when a new hour starts */
	UFUNCTION(BlueprintNativeEvent, Category = "Time of Day")
	void OnHourChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when a new day starts */
	UFUNCTION(BlueprintNativeEvent, Category = "Time of Day")
	void OnDayChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when the day phase changes */
	UFUNCTION(BlueprintNativeEvent, Category = "Time of Day")
	void OnDayPhaseChanged(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime);

	/** Called when a new season starts */
	UFUNCTION(BlueprintNativeEvent, Category = "Time of Day")
	void OnSeasonChanged(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime);

	/** Called when a new year starts */
	UFUNCTION(BlueprintNativeEvent, Category = "Time of Day")
	void OnYearChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when time scale changes */
	UFUNCTION(BlueprintNativeEvent, Category = "Time of Day")
	void OnTimeScaleChanged(float NewTimeScale);
};

/**
 * Interface for the Time of Day Manager
 * Provides access to time information and control
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UTimeOfDayManager : public UInterface
{
	GENERATED_BODY()
};

class TIMEOFDAYCORE_API ITimeOfDayManager
{
	GENERATED_BODY()

public:
	/** Get the current game time */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	FTimeOfDayTime GetCurrentTime() const;

	/** Get the current day phase */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	ETimeOfDayPhase GetCurrentPhase() const;

	/** Get normalized time of day (0.0 = midnight, 0.5 = noon, 1.0 = midnight) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	float GetNormalizedTimeOfDay() const;

	/** Get the current season */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	ETimeOfDaySeason GetCurrentSeason() const;

	/** Set the current time (jumps immediately to the specified time) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	void SetCurrentTime(const FTimeOfDayTime& NewTime);

	/** Advance time by the specified amount of game seconds */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	void AdvanceTime(float GameSeconds);

	/** Set the time scale multiplier */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	void SetTimeScale(float NewTimeScale);

	/** Get the current time scale multiplier */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	float GetTimeScale() const;

	/** Pause time progression */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	void PauseTime();

	/** Resume time progression */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	void ResumeTime();

	/** Check if time is currently paused */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	bool IsTimePaused() const;

	/** Register a listener for time events */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	void RegisterListener(const TScriptInterface<ITimeOfDayListener>& Listener);

	/** Unregister a listener for time events */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time of Day")
	void UnregisterListener(const TScriptInterface<ITimeOfDayListener>& Listener);
};


