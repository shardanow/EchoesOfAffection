// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/TimeOfDayTypes.h"
#include "Core/TimeOfDayInterface.h"
#include "Data/TimeOfDaySettings.h"
#include "TimeOfDaySubsystem.generated.h"

// Delegate declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinuteChanged, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHourChanged, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChanged, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDayPhaseChanged, ETimeOfDayPhase, NewPhase, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSeasonChanged, ETimeOfDaySeason, NewSeason, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnYearChanged, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeScaleChanged, float, NewTimeScale);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimePausedChanged, bool, bIsPaused, const FTimeOfDayTime&, CurrentTime);

// Sun/Moon event delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSunriseEvent, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSunsetEvent, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoonriseEvent, float, MoonPhase, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoonsetEvent, float, MoonPhase, const FTimeOfDayTime&, CurrentTime);

/**
 * World Subsystem managing the game time and day/night cycle
 * Implements event-driven architecture with delegates for time changes
 * 
 * This is the central authority for all time-related operations in the game world.
 * It handles:
 * - Time progression and scaling
 * - Day/night cycle phases
 * - Season and calendar management
 * - Event broadcasting to listeners
 * - Save/load of time state
 */
UCLASS()
class TIMEOFDAYRUNTIME_API UTimeOfDaySubsystem : public UTickableWorldSubsystem, public ITimeOfDayManager
{
	GENERATED_BODY()

public:
	UTimeOfDaySubsystem();

	// ========== USubsystem Interface ==========
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ========== UTickableWorldSubsystem Interface ==========
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

	// ========== ITimeOfDayManager Interface Implementation ==========
	virtual FTimeOfDayTime GetCurrentTime_Implementation() const override;
	virtual ETimeOfDayPhase GetCurrentPhase_Implementation() const override;
	virtual float GetNormalizedTimeOfDay_Implementation() const override;
	virtual ETimeOfDaySeason GetCurrentSeason_Implementation() const override;
	virtual void SetCurrentTime_Implementation(const FTimeOfDayTime& NewTime) override;
	virtual void AdvanceTime_Implementation(float GameSeconds) override;
	virtual void SetTimeScale_Implementation(float NewTimeScale) override;
	virtual float GetTimeScale_Implementation() const override;
	virtual void PauseTime_Implementation() override;
	virtual void ResumeTime_Implementation() override;
	virtual bool IsTimePaused_Implementation() const override;
	virtual void RegisterListener_Implementation(const TScriptInterface<ITimeOfDayListener>& Listener) override;
	virtual void UnregisterListener_Implementation(const TScriptInterface<ITimeOfDayListener>& Listener) override;

	// ========== Blueprint Callable Functions ==========

	/** Initialize the subsystem with a settings asset */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	void InitializeWithSettings(UTimeOfDaySettings* Settings);

	/** Get the phase definition for the current time */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	FDayPhaseDefinition GetCurrentPhaseDefinition() const;

	/** Check if it's currently daytime (between dawn and dusk) */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	bool IsDaytime() const;

	/** Check if it's currently nighttime */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	bool IsNighttime() const;

	/** Get formatted time string (HH:MM) */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	FString GetFormattedTime(bool bIncludeSeconds = false) const;

	/** Get formatted time string in 12-hour format with AM/PM (e.g. "3:45 PM") */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	FString GetFormattedTime12Hour(bool bIncludeSeconds = false) const;

	/** Get formatted date string */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	FString GetFormattedDate() const;

	/** Get formatted date string with specified format */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	FString GetFormattedDateWithFormat(EDateFormat Format) const;

	/** Calculate time difference in game seconds between two times */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	float CalculateTimeDifference(const FTimeOfDayTime& TimeA, const FTimeOfDayTime& TimeB) const;

	// ========== Sun/Daylight Functions ==========

	/** Get sun position for current time (0.0 = sunrise, 0.5 = noon, 1.0 = sunset, -1.0 = night) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	float GetSunPosition() const;

	/** Get sun altitude angle in degrees (0 = horizon, 90 = zenith, negative = below horizon) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	float GetSunAltitude() const;

	/** Check if it's currently daylight hours (between sunrise and sunset) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	bool IsDaylightHours() const;

	/** Get sunrise time for current season (in minutes since midnight) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	int32 GetSunriseMinutes() const;

	/** Get sunset time for current season (in minutes since midnight) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	int32 GetSunsetMinutes() const;

	/** Get day length for current season (in minutes) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	int32 GetDayLengthMinutes() const;

	/** 
	 * Get sun rotation for Directional Light (Pitch and Yaw)
	 * ? INTERPOLATED: Uses seconds for smooth movement (no jerky updates)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	FRotator GetSunRotation() const;

	/** 
	 * Get moon rotation for Directional Light (Pitch and Yaw) - opposite of sun 
	 * ? INTERPOLATED: Uses seconds for smooth movement (no jerky updates)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	FRotator GetMoonRotation() const;

	/** Get moon phase (0.0 = New Moon, 0.5 = Full Moon, 1.0 = New Moon) based on day of month */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day|Daylight")
	float GetMoonPhase() const;

	/** Get the settings asset (useful for external systems to access time configuration) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day")
	UTimeOfDaySettings* GetSettings() const { return Settings; }

	// ========== Blueprint-Friendly Wrapper Functions (הכÿ סמגלוסעטלמסעט) ==========

	/** Register a listener - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (DisplayName = "Register Listener"))
	void BP_RegisterListener(TScriptInterface<ITimeOfDayListener> Listener);

	/** Unregister a listener - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (DisplayName = "Unregister Listener"))
	void BP_UnregisterListener(TScriptInterface<ITimeOfDayListener> Listener);

	/** Get current time - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day", meta = (DisplayName = "Get Current Time"))
	FTimeOfDayTime BP_GetCurrentTime() const;

	/** Get current phase - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day", meta = (DisplayName = "Get Current Phase"))
	ETimeOfDayPhase BP_GetCurrentPhase() const;

	/** Get normalized time - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day", meta = (DisplayName = "Get Normalized Time"))
	float BP_GetNormalizedTimeOfDay() const;

	/** Get current season - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day", meta = (DisplayName = "Get Current Season"))
	ETimeOfDaySeason BP_GetCurrentSeason() const;

	/** Set current time - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (DisplayName = "Set Current Time"))
	void BP_SetCurrentTime(const FTimeOfDayTime& NewTime);

	/** Advance time - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (DisplayName = "Advance Time"))
	void BP_AdvanceTime(float GameSeconds);

	/** Set time scale - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (DisplayName = "Set Time Scale"))
	void BP_SetTimeScale(float NewTimeScale);

	/** Get time scale - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (DisplayName = "Get Time Scale"))
	float BP_GetTimeScale() const;

	/** Pause time - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (DisplayName = "Pause Time"))
	void BP_PauseTime();

	/** Resume time - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, Category = "Time of Day", meta = (DisplayName = "Resume Time"))
	void BP_ResumeTime();

	/** Check if paused - Blueprint friendly version */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Time of Day", meta = (DisplayName = "Is Time Paused"))
	bool BP_IsTimePaused() const;

	// ========== Multicast Delegates ==========

	/** Broadcast when a new minute starts */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events")
	FOnMinuteChanged OnMinuteChanged;

	/** Broadcast when a new hour starts */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events")
	FOnHourChanged OnHourChanged;

	/** Broadcast when a new day starts */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events")
	FOnDayChanged OnDayChanged;

	/** Broadcast when the day phase changes */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events")
	FOnDayPhaseChanged OnDayPhaseChanged;

	/** Broadcast when a new season starts */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events")
	FOnSeasonChanged OnSeasonChanged;

	/** Broadcast when a new year starts */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events")
	FOnYearChanged OnYearChanged;

	/** Broadcast when time scale changes */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events")
	FOnTimeScaleChanged OnTimeScaleChanged;

	/** Broadcast when time pause state changes */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events")
	FOnTimePausedChanged OnTimePausedChanged;

	/** Broadcast when the sun rises (at sunrise time) */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events|Celestial")
	FOnSunriseEvent OnSunrise;

	/** Broadcast when the sun sets (at sunset time) */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events|Celestial")
	FOnSunsetEvent OnSunset;

	/** Broadcast when the moon rises (at sunset time) */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events|Celestial")
	FOnMoonriseEvent OnMoonrise;

	/** Broadcast when the moon sets (at sunrise time) */
	UPROPERTY(BlueprintAssignable, Category = "Time of Day Events|Celestial")
	FOnMoonsetEvent OnMoonset;
	
protected:
	// ========== Internal State ==========

	/** Current game time */
	UPROPERTY()
	FTimeOfDayTime CurrentTime;

	/** Current day phase */
	UPROPERTY()
	ETimeOfDayPhase CurrentPhase;

	/** Time conversion settings */
	UPROPERTY()
	FTimeConversionSettings TimeConversionSettings;

	/** Calendar settings */
	UPROPERTY()
	FCalendarSettings CalendarSettings;

	/** Reference to the settings data asset */
	UPROPERTY()
	TObjectPtr<UTimeOfDaySettings> Settings;

	/** Registered listeners for time events */
	UPROPERTY()
	TArray<TScriptInterface<ITimeOfDayListener>> RegisteredListeners;

	/** Accumulated time for tick management */
	float AccumulatedTime;

	/** Previous values for change detection */
	int32 PreviousMinute;
	int32 PreviousHour;
	int32 PreviousDay;
	ETimeOfDayPhase PreviousPhase;
	ETimeOfDaySeason PreviousSeason;
	int32 PreviousYear;

	/** Previous sun/moon states for event detection */
	bool bPreviousWasSunUp;
	bool bPreviousWasMoonUp;

	// ========== Internal Methods ==========

	/** Update game time by delta seconds */
	void UpdateTime(float DeltaGameSeconds);

	/** Process time changes and broadcast events */
	void ProcessTimeChanges();

	/** Update current phase based on time */
	void UpdatePhase();

	/** Notify all listeners about a time event */
	void NotifyListeners_Minute();
	void NotifyListeners_Hour();
	void NotifyListeners_Day();
	void NotifyListeners_Phase(ETimeOfDayPhase NewPhase);
	void NotifyListeners_Season(ETimeOfDaySeason NewSeason);
	void NotifyListeners_Year();
	void NotifyListeners_TimeScale(float NewScale);

	/** ? NEW: Notify Schedule System about time changes */
	void NotifyScheduleSystem();

	/** Calculate weekday from total days */
	ETimeOfDayWeekday CalculateWeekday(int32 TotalDays) const;

	/** Calculate season and day from total days */
	void CalculateSeasonAndDay(int32 TotalDays, ETimeOfDaySeason& OutSeason, int32& OutDayOfSeason) const;

	/** Check for sunrise/sunset/moonrise/moonset events */
	void CheckCelestialEvents();
};

