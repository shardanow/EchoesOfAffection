// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystems/TimeOfDaySubsystem.h"
#include "Engine/World.h"
#include "TimeOfDayGameEventHelper.h"

// ADD THIS: Include ScheduleSubsystem for integration
#if WITH_EDITOR || WITH_SERVER_CODE || WITH_GAME
#include "Subsystems/ScheduleSubsystem.h"
#endif

UTimeOfDaySubsystem::UTimeOfDaySubsystem()
	: CurrentPhase(ETimeOfDayPhase::Morning)
	, AccumulatedTime(0.0f)
	, PreviousMinute(0)
	, PreviousHour(0)
	, PreviousDay(0)
	, PreviousPhase(ETimeOfDayPhase::Morning)
	, PreviousSeason(ETimeOfDaySeason::Spring)
	, PreviousYear(1)
	, bPreviousWasSunUp(false)
	, bPreviousWasMoonUp(false)
{
}

void UTimeOfDaySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Try to load default settings
	const FString SettingsPath = TEXT("/TimeOfDaySystem/DA_TimeOfDaySettings.DA_TimeOfDaySettings");
	Settings = Cast<UTimeOfDaySettings>(StaticLoadObject(UTimeOfDaySettings::StaticClass(), nullptr, *SettingsPath));

	if (Settings)
	{
		UE_LOG(LogTemp, Log, TEXT("[OK] Loaded TimeOfDaySettings from: %s"), *SettingsPath);
		InitializeWithSettings(Settings);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[WARN] TimeOfDaySettings not found at: %s"), *SettingsPath);
		UE_LOG(LogTemp, Warning, TEXT("[WARN] Using default C++ values. Initial time will be: 06:00 (Hour=6)"));
		UE_LOG(LogTemp, Warning, TEXT("[INFO] Create a DataAsset at Content/TimeOfDaySystem/DA_TimeOfDaySettings to configure initial time!"));
		
		// Use default values if no settings asset is found
		CurrentTime = FTimeOfDayTime();
		TimeConversionSettings = FTimeConversionSettings();
		CalendarSettings = FCalendarSettings();
		UpdatePhase();
	}

	// Initialize previous values
	PreviousMinute = CurrentTime.Minute;
	PreviousHour = CurrentTime.Hour;
	PreviousDay = CurrentTime.GetTotalDays();
	PreviousPhase = CurrentPhase;
	PreviousSeason = CurrentTime.Season;
	PreviousYear = CurrentTime.Year;

	// Initialize celestial states
	const int32 CurrentMinutes = CurrentTime.Hour * 60 + CurrentTime.Minute;
	const int32 SunriseMinutes = GetSunriseMinutes();
	const int32 SunsetMinutes = GetSunsetMinutes();
	bPreviousWasSunUp = (CurrentMinutes >= SunriseMinutes && CurrentMinutes < SunsetMinutes);
	bPreviousWasMoonUp = !bPreviousWasSunUp;

	UE_LOG(LogTemp, Log, TEXT("TimeOfDaySubsystem initialized. Current time: %s"), *CurrentTime.ToString());
	UE_LOG(LogTemp, Log, TEXT("  Hour: %d, Minute: %d, Sunrise: %d, Sunset: %d"), CurrentTime.Hour, CurrentTime.Minute, SunriseMinutes, SunsetMinutes);
}

void UTimeOfDaySubsystem::Deinitialize()
{
	RegisteredListeners.Empty();
	Super::Deinitialize();
}

void UTimeOfDaySubsystem::Tick(float DeltaTime)
{
	if (!Settings || !Settings->bAutoAdvanceTime || TimeConversionSettings.bIsPaused)
	{
		return;
	}

	// Convert real time to game time
	const float GameDeltaSeconds = TimeConversionSettings.ConvertRealToGameDelta(DeltaTime);
	
	// Update with rate limiting based on UpdateFrequency
	if (Settings->UpdateFrequency > 0.0f)
	{
		AccumulatedTime += GameDeltaSeconds;
		const float UpdateInterval = 1.0f / Settings->UpdateFrequency;

		if (AccumulatedTime >= UpdateInterval)
		{
			UpdateTime(AccumulatedTime);
			AccumulatedTime = 0.0f;
		}
	}
	else
	{
		UpdateTime(GameDeltaSeconds);
	}
}

TStatId UTimeOfDaySubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeOfDaySubsystem, STATGROUP_Tickables);
}

bool UTimeOfDaySubsystem::IsTickable() const
{
	return !IsTemplate() && Settings && Settings->bAutoAdvanceTime;
}

// ========== ITimeOfDayManager Interface Implementation ==========

FTimeOfDayTime UTimeOfDaySubsystem::GetCurrentTime_Implementation() const
{
	return CurrentTime;
}

ETimeOfDayPhase UTimeOfDaySubsystem::GetCurrentPhase_Implementation() const
{
	return CurrentPhase;
}

float UTimeOfDaySubsystem::GetNormalizedTimeOfDay_Implementation() const
{
	return CurrentTime.GetNormalizedTimeOfDay();
}

ETimeOfDaySeason UTimeOfDaySubsystem::GetCurrentSeason_Implementation() const
{
	return CurrentTime.Season;
}

void UTimeOfDaySubsystem::SetCurrentTime_Implementation(const FTimeOfDayTime& NewTime)
{
	CurrentTime = NewTime;
	UpdatePhase();
	ProcessTimeChanges();
}

void UTimeOfDaySubsystem::AdvanceTime_Implementation(float GameSeconds)
{
	UpdateTime(GameSeconds);
}

void UTimeOfDaySubsystem::SetTimeScale_Implementation(float NewTimeScale)
{
	if (NewTimeScale != TimeConversionSettings.TimeScale)
	{
		TimeConversionSettings.TimeScale = FMath::Max(0.0f, NewTimeScale);
		NotifyListeners_TimeScale(TimeConversionSettings.TimeScale);
		OnTimeScaleChanged.Broadcast(TimeConversionSettings.TimeScale);
	}
}

float UTimeOfDaySubsystem::GetTimeScale_Implementation() const
{
	return TimeConversionSettings.TimeScale;
}

void UTimeOfDaySubsystem::PauseTime_Implementation()
{
	if (!TimeConversionSettings.bIsPaused)
	{
		TimeConversionSettings.bIsPaused = true;
		OnTimePausedChanged.Broadcast(true, CurrentTime);
	}
}

void UTimeOfDaySubsystem::ResumeTime_Implementation()
{
	if (TimeConversionSettings.bIsPaused)
	{
		TimeConversionSettings.bIsPaused = false;
		OnTimePausedChanged.Broadcast(false, CurrentTime);
	}
}

bool UTimeOfDaySubsystem::IsTimePaused_Implementation() const
{
	return TimeConversionSettings.bIsPaused;
}

void UTimeOfDaySubsystem::RegisterListener_Implementation(const TScriptInterface<ITimeOfDayListener>& Listener)
{
	if (Listener.GetObject() && !RegisteredListeners.Contains(Listener))
	{
		RegisteredListeners.Add(Listener);
	}
}

void UTimeOfDaySubsystem::UnregisterListener_Implementation(const TScriptInterface<ITimeOfDayListener>& Listener)
{
	RegisteredListeners.Remove(Listener);
}

// ========== Blueprint Callable Functions ==========

void UTimeOfDaySubsystem::InitializeWithSettings(UTimeOfDaySettings* InSettings)
{
	if (!InSettings)
	{
		return;
	}

	Settings = InSettings;
	CurrentTime = InSettings->InitialGameTime;
	TimeConversionSettings = InSettings->TimeConversion;
	CalendarSettings = InSettings->Calendar;
	
	// CRITICAL: TotalElapsedSeconds must be calculated from InitialGameTime!
	// Otherwise UpdateTime() will reset the time to 00:00
	const int32 InitialSeconds = (CurrentTime.Hour * 3600) + (CurrentTime.Minute * 60) + CurrentTime.Second;
	CurrentTime.TotalElapsedSeconds = static_cast<double>(InitialSeconds);
	
	UE_LOG(LogTemp, Log, TEXT("InitializeWithSettings: Hour=%d, Minute=%d, Second=%d"), CurrentTime.Hour, CurrentTime.Minute, CurrentTime.Second);
	UE_LOG(LogTemp, Log, TEXT("  TotalElapsedSeconds set to: %.2f"), CurrentTime.TotalElapsedSeconds);
	
	UpdatePhase();

	UE_LOG(LogTemp, Log, TEXT("TimeOfDaySubsystem initialized with settings. Time: %s"), *CurrentTime.ToString());
}

FDayPhaseDefinition UTimeOfDaySubsystem::GetCurrentPhaseDefinition() const
{
	if (Settings)
	{
		return Settings->GetPhaseForTime(CurrentTime.Hour, CurrentTime.Minute);
	}
	return FDayPhaseDefinition();
}

bool UTimeOfDaySubsystem::IsDaytime() const
{
	return CurrentPhase != ETimeOfDayPhase::Night && CurrentPhase != ETimeOfDayPhase::Evening;
}

bool UTimeOfDaySubsystem::IsNighttime() const
{
	return CurrentPhase == ETimeOfDayPhase::Night || CurrentPhase == ETimeOfDayPhase::Evening;
}

FString UTimeOfDaySubsystem::GetFormattedTime(bool bIncludeSeconds) const
{
	// Check if widget wants 12-hour format (we'll add this parameter later)
	// For now, always use 24-hour format
	// TODO: Add bUse12HourFormat parameter to this method
	
	if (bIncludeSeconds)
	{
		return FString::Printf(TEXT("%02d:%02d:%02d"), CurrentTime.Hour, CurrentTime.Minute, CurrentTime.Second);
	}
	return FString::Printf(TEXT("%02d:%02d"), CurrentTime.Hour, CurrentTime.Minute);
}

FString UTimeOfDaySubsystem::GetFormattedTime12Hour(bool bIncludeSeconds) const
{
	// Convert to 12-hour format
	int32 Hour12 = CurrentTime.Hour % 12;
	if (Hour12 == 0)
	{
		Hour12 = 12; // Midnight/Noon shows as 12
	}
	
	const TCHAR* AMPM = (CurrentTime.Hour < 12) ? TEXT("AM") : TEXT("PM");
	
	// Debug log
	//UE_LOG(LogTemp, Log, TEXT("GetFormattedTime12Hour: Hour24=%d, Hour12=%d, AMPM=%s"), CurrentTime.Hour, Hour12, AMPM);
	
	if (bIncludeSeconds)
	{
		return FString::Printf(TEXT("%d:%02d:%02d %s"), Hour12, CurrentTime.Minute, CurrentTime.Second, AMPM);
	}
	return FString::Printf(TEXT("%d:%02d %s"), Hour12, CurrentTime.Minute, AMPM);
}

FString UTimeOfDaySubsystem::GetFormattedDate() const
{
	// Default format: Full textual
	return GetFormattedDateWithFormat(EDateFormat::Full);
}

FString UTimeOfDaySubsystem::GetFormattedDateWithFormat(EDateFormat Format) const
{
	// Calculate month and day of month from DayOfSeason
	const int32 MonthIndexInSeason = (CurrentTime.DayOfSeason - 1) / 30; // 0-2 within season
	const int32 DayOfMonth = ((CurrentTime.DayOfSeason - 1) % 30) + 1; // 1-30
	
	// Map seasons to calendar months (CORRECT mapping):
	// Spring (0) = months 3, 4, 5 (March, April, May)
	// Summer (1) = months 6, 7, 8 (June, July, August)
	// Autumn (2) = months 9, 10, 11 (September, October, November)
	// Winter (3) = months 12, 1, 2 (December, January, February)
	
	int32 CalendarMonth; // 1-12
	
	switch (CurrentTime.Season)
	{
		case ETimeOfDaySeason::Spring:
			// March (3), April (4), May (5)
			CalendarMonth = 3 + MonthIndexInSeason;
			break;
			
		case ETimeOfDaySeason::Summer:
			// June (6), July (7), August (8)
			CalendarMonth = 6 + MonthIndexInSeason;
			break;
			
		case ETimeOfDaySeason::Autumn:
			// September (9), October (10), November (11)
			CalendarMonth = 9 + MonthIndexInSeason;
			break;
			
		case ETimeOfDaySeason::Winter:
			// December (12), January (1), February (2)
			if (MonthIndexInSeason == 0)
			{
				CalendarMonth = 12; // December
			}
			else
			{
				CalendarMonth = MonthIndexInSeason; // 1 = January, 2 = February
			}
			break;
			
		default:
			CalendarMonth = 1;
			break;
	}
	
	// Month array index (0-11) for month names
	const int32 MonthArrayIndex = CalendarMonth - 1;
	
	// Year for short formats (last 2 digits)
	const int32 YearShort = CurrentTime.Year % 100;
	
	switch (Format)
	{
		case EDateFormat::DotShort:
			// DD.MM.YY - Example: "15.03.01"
			return FString::Printf(TEXT("%02d.%02d.%02d"), DayOfMonth, CalendarMonth, YearShort);
			
		case EDateFormat::DotEuropean:
			// DD.MM.YYYY - Example: "15.03.2001"
			return FString::Printf(TEXT("%02d.%02d.%04d"), DayOfMonth, CalendarMonth, CurrentTime.Year);
			
		case EDateFormat::SlashEuropean:
			// DD/MM/YYYY - Example: "15/03/2001"
			return FString::Printf(TEXT("%02d/%02d/%04d"), DayOfMonth, CalendarMonth, CurrentTime.Year);
			
		case EDateFormat::SlashAmerican:
			// MM/DD/YYYY - Example: "03/15/2001"
			return FString::Printf(TEXT("%02d/%02d/%04d"), CalendarMonth, DayOfMonth, CurrentTime.Year);
			
		case EDateFormat::ISO:
			// YYYY-MM-DD - Example: "2001-03-15" (ISO 8601)
			return FString::Printf(TEXT("%04d-%02d-%02d"), CurrentTime.Year, CalendarMonth, DayOfMonth);
			
		case EDateFormat::Full:
		default:
		{
			// DD Month, Year Y (Weekday) - Example: "15 March, Year 1 (Monday)"
			
			// Month names array (calendar order: Jan-Dec)
			static const TCHAR* DefaultMonthNames[12] = {
				TEXT("January"),// Month 1 (index 0)
				TEXT("February"),  // Month 2 (index 1)
				TEXT("March"),     // Month 3 (index 2) - Spring starts
				TEXT("April"),     // Month 4 (index 3)
				TEXT("May"),       // Month 5 (index 4)
				TEXT("June"),      // Month 6 (index 5) - Summer starts
				TEXT("July"),      // Month 7 (index 6)
				TEXT("August"),    // Month 8 (index 7)
				TEXT("September"), // Month 9 (index 8) - Autumn starts
				TEXT("October"),   // Month 10 (index 9)
				TEXT("November"),  // Month 11 (index 10)
				TEXT("December")   // Month 12 (index 11) - Winter starts
			};
			
			// Use custom month names if available, otherwise use default
			FString MonthName;
			if (Settings && Settings->MonthNames.Num() == 12 && MonthArrayIndex >= 0 && MonthArrayIndex < 12)
			{
				MonthName = Settings->MonthNames[MonthArrayIndex];
			}
			else if (MonthArrayIndex >= 0 && MonthArrayIndex < 12)
			{
				MonthName = DefaultMonthNames[MonthArrayIndex];
			}
			else
			{
				MonthName = TEXT("Unknown");
			}
			
			// Format: "15 March, Year 1 (Monday)"
			return FString::Printf(TEXT("%d %s, Year %d (%s)"),
				DayOfMonth,
				*MonthName,
				CurrentTime.Year,
				*StaticEnum<ETimeOfDayWeekday>()->GetDisplayNameTextByValue((int64)CurrentTime.DayOfWeek).ToString());
		}
	}
}

float UTimeOfDaySubsystem::CalculateTimeDifference(const FTimeOfDayTime& TimeA, const FTimeOfDayTime& TimeB) const
{
	return FMath::Abs(TimeA.TotalElapsedSeconds - TimeB.TotalElapsedSeconds);
}

// ========== Sun/Daylight Functions ==========

float UTimeOfDaySubsystem::GetSunPosition() const
{
	if (!Settings)
	{
		// Fallback: use simple normalized time
		return CurrentTime.GetNormalizedTimeOfDay();
	}

	return CurrentTime.GetSunPosition(Settings->Daylight);
}

float UTimeOfDaySubsystem::GetSunAltitude() const
{
	if (!Settings)
	{
		// Fallback: simple arc based on hour
		const float NormalizedTime = CurrentTime.GetNormalizedTimeOfDay();
		const float Radians = NormalizedTime * PI * 2.0f;
		return FMath::Sin(Radians) * 90.0f;
	}

	return CurrentTime.GetSunAltitude(Settings->Daylight);
}

bool UTimeOfDaySubsystem::IsDaylightHours() const
{
	if (!Settings)
	{
		// Fallback: 6:00 - 18:00
		return CurrentTime.Hour >= 6 && CurrentTime.Hour < 18;
	}

	return CurrentTime.IsDaylightHours(Settings->Daylight);
}

int32 UTimeOfDaySubsystem::GetSunriseMinutes() const
{
	if (!Settings)
	{
		return 6 * 60; // 6:00 AM
	}

	return Settings->Daylight.GetSunriseMinutes(CurrentTime.Season);
}

int32 UTimeOfDaySubsystem::GetSunsetMinutes() const
{
	if (!Settings)
	{
		return 18 * 60; // 6:00 PM
	}

	return Settings->Daylight.GetSunsetMinutes(CurrentTime.Season);
}

int32 UTimeOfDaySubsystem::GetDayLengthMinutes() const
{
	return GetSunsetMinutes() - GetSunriseMinutes();
}

FRotator UTimeOfDaySubsystem::GetSunRotation() const
{
	if (!Settings)
	{
		// Fallback: simple 24-hour rotation
		const float NormalizedTime = CurrentTime.GetNormalizedTimeOfDay();
		const float Pitch = -FMath::Sin(NormalizedTime * PI * 2.0f) * 90.0f;
		const float Yaw = 90.0f - (NormalizedTime * 360.0f);
		return FRotator(Pitch, Yaw, 0.0f);
	}

	// NEW: Include seconds for smooth interpolation
	// Calculate exact time in minutes (with fractional seconds)
	const float CurrentMinutesExact = (CurrentTime.Hour * 60.0f) + CurrentTime.Minute + (CurrentTime.Second / 60.0f);
	
	const int32 SunriseMinutes = GetSunriseMinutes();
	const int32 SunsetMinutes = GetSunsetMinutes();
	
	// Calculate total day and night durations
	const int32 DayLength = SunsetMinutes - SunriseMinutes;
	const int32 NightLength = 1440 - DayLength; // 24 hours - day length
	
	// Calculate normalized position over 24 hours (0.0 = sunrise, 1.0 = next sunrise)
	float NormalizedPosition24h;
	
	if (CurrentMinutesExact >= SunriseMinutes && CurrentMinutesExact < SunsetMinutes)
	{
		// DAYTIME: Sunrise to Sunset
		// Map to 0.0 (sunrise) to 0.5 (sunset)
		const float MinutesSinceSunrise = CurrentMinutesExact - SunriseMinutes;
		NormalizedPosition24h = 0.5f * (MinutesSinceSunrise / static_cast<float>(DayLength));
	}
	else
	{
		// NIGHTTIME: Sunset to Sunrise
		// Map to 0.5 (sunset) to 1.0 (next sunrise)
		float MinutesSinceSunset;
		
		if (CurrentMinutesExact >= SunsetMinutes)
		{
			// After sunset, same day
			MinutesSinceSunset = CurrentMinutesExact - SunsetMinutes;
		}
		else
		{
			// After midnight, before sunrise
			MinutesSinceSunset = (1440 - SunsetMinutes) + CurrentMinutesExact;
		}
		
		NormalizedPosition24h = 0.5f + 0.5f * (MinutesSinceSunset / static_cast<float>(NightLength));
	}
	
	// Convert to sun elevation using smooth sine wave
	const float Radians = NormalizedPosition24h * PI * 2.0f;
	const float Pitch = -FMath::Sin(Radians) * 90.0f;
	
	// Yaw: Complete rotation East to South to West to North to East
	const float Yaw = 90.0f - (NormalizedPosition24h * 360.0f);
	
	return FRotator(Pitch, Yaw, 0.0f);
}

FRotator UTimeOfDaySubsystem::GetMoonRotation() const
{
	// Moon is opposite to the sun (180° offset)
	const FRotator SunRotation = GetSunRotation();
	
	// Inverse the rotation
	const float MoonPitch = -SunRotation.Pitch;
	const float MoonYaw = SunRotation.Yaw + 180.0f;
	
	return FRotator(MoonPitch, MoonYaw, 0.0f);
}

float UTimeOfDaySubsystem::GetMoonPhase() const
{
	// Calculate moon phase based on day of month (30-day cycle)
	// Get current day of month from DayOfSeason
	const int32 MonthIndexInSeason = (CurrentTime.DayOfSeason - 1) / 30; // 0-2 within season
	const int32 DayOfMonth = ((CurrentTime.DayOfSeason - 1) % 30) + 1; // 1-30
	
	// Moon phase cycles every 30 days (simplified lunar cycle)
	// 0.0 = New Moon (day 1, 30)
	// 0.25 = First Quarter (day 8)
	// 0.5 = Full Moon (day 15)
	// 0.75 = Last Quarter (day 23)
	// 1.0 = New Moon again
	
	const float Phase = static_cast<float>(DayOfMonth - 1) / 30.0f; // 0.0 - 1.0
	
	return Phase;
}

// ========== Blueprint-Friendly Wrapper Functions ==========

void UTimeOfDaySubsystem::BP_RegisterListener(TScriptInterface<ITimeOfDayListener> Listener)
{
	RegisterListener_Implementation(Listener);
}

void UTimeOfDaySubsystem::BP_UnregisterListener(TScriptInterface<ITimeOfDayListener> Listener)
{
	UnregisterListener_Implementation(Listener);
}

FTimeOfDayTime UTimeOfDaySubsystem::BP_GetCurrentTime() const
{
	return GetCurrentTime_Implementation();
}

ETimeOfDayPhase UTimeOfDaySubsystem::BP_GetCurrentPhase() const
{
	return GetCurrentPhase_Implementation();
}

float UTimeOfDaySubsystem::BP_GetNormalizedTimeOfDay() const
{
	return GetNormalizedTimeOfDay_Implementation();
}

ETimeOfDaySeason UTimeOfDaySubsystem::BP_GetCurrentSeason() const
{
	return GetCurrentSeason_Implementation();
}

void UTimeOfDaySubsystem::BP_SetCurrentTime(const FTimeOfDayTime& NewTime)
{
	SetCurrentTime_Implementation(NewTime);
}

void UTimeOfDaySubsystem::BP_AdvanceTime(float GameSeconds)
{
	AdvanceTime_Implementation(GameSeconds);
}

void UTimeOfDaySubsystem::BP_SetTimeScale(float NewTimeScale)
{
	SetTimeScale_Implementation(NewTimeScale);
}

float UTimeOfDaySubsystem::BP_GetTimeScale() const
{
	return GetTimeScale_Implementation();
}

void UTimeOfDaySubsystem::BP_PauseTime()
{
	PauseTime_Implementation();
}

void UTimeOfDaySubsystem::BP_ResumeTime()
{
	ResumeTime_Implementation();
}

bool UTimeOfDaySubsystem::BP_IsTimePaused() const
{
	return IsTimePaused_Implementation();
}

// ========== Internal Methods ==========

void UTimeOfDaySubsystem::UpdateTime(float DeltaGameSeconds)
{
	// Store previous values for change detection
	PreviousMinute = CurrentTime.Minute;
	PreviousHour = CurrentTime.Hour;
	PreviousDay = CurrentTime.GetTotalDays();
	PreviousSeason = CurrentTime.Season;
	PreviousYear = CurrentTime.Year;
	PreviousPhase = CurrentPhase;

	// Update total elapsed time
	CurrentTime.TotalElapsedSeconds += DeltaGameSeconds;

	// DEBUG: Log time update
	//UE_LOG(LogTemp, Log, TEXT("UpdateTime: DeltaGameSeconds=%.4f, TotalElapsedSeconds=%.2f"), DeltaGameSeconds, CurrentTime.TotalElapsedSeconds);

	// Calculate new time components
	const int32 TotalSeconds = static_cast<int32>(CurrentTime.TotalElapsedSeconds);
	
	CurrentTime.Second = TotalSeconds % 60;
	const int32 TotalMinutes = TotalSeconds / 60;
	
	CurrentTime.Minute = TotalMinutes % 60;
	const int32 TotalHours = TotalMinutes / 60;
	
	CurrentTime.Hour = TotalHours % 24;
	const int32 TotalDays = TotalHours / 24;

	// DEBUG: Log calculated time
	//UE_LOG(LogTemp, Log, TEXT("  Calculated: Hour=%d, Minute=%d, Second=%d"), CurrentTime.Hour, CurrentTime.Minute, CurrentTime.Second);

	// Calculate day of week
	CurrentTime.DayOfWeek = CalculateWeekday(TotalDays);

	// Calculate season and day within season
	CalculateSeasonAndDay(TotalDays, CurrentTime.Season, CurrentTime.DayOfSeason);

	// Calculate year
	const int32 DaysPerYear = CalendarSettings.GetDaysPerYear();
	CurrentTime.Year = (TotalDays / DaysPerYear) + 1;

	// Update phase
	UpdatePhase();

	// Process and broadcast changes
	ProcessTimeChanges();
}

void UTimeOfDaySubsystem::ProcessTimeChanges()
{
	// Check for minute change
	if (CurrentTime.Minute != PreviousMinute)
	{
		NotifyListeners_Minute();
		OnMinuteChanged.Broadcast(CurrentTime);

		// NEW: Notify Schedule System about time change
		NotifyScheduleSystem();

		// Check for sunrise/sunset/moonrise/moonset events (minute-based precision)
		CheckCelestialEvents();
	}

	// Check for hour change
	if (CurrentTime.Hour != PreviousHour)
	{
		NotifyListeners_Hour();
		OnHourChanged.Broadcast(CurrentTime);
		
		// NEW: Emit GameEventBus event
		UTimeOfDayGameEventHelper::EmitHourChangedEvent(this, CurrentTime.Hour);
	}

	// Check for day change
	const int32 CurrentDay = CurrentTime.GetTotalDays();
	if (CurrentDay != PreviousDay)
	{
		NotifyListeners_Day();
		OnDayChanged.Broadcast(CurrentTime);
		
		// NEW: Emit GameEventBus event
		UTimeOfDayGameEventHelper::EmitDayChangedEvent(this, CurrentDay);
	}

	// Check for phase change
	if (CurrentPhase != PreviousPhase)
	{
		NotifyListeners_Phase(CurrentPhase);
		OnDayPhaseChanged.Broadcast(CurrentPhase, CurrentTime);
	}

	// Check for season change
	if (CurrentTime.Season != PreviousSeason)
	{
		NotifyListeners_Season(CurrentTime.Season);
		OnSeasonChanged.Broadcast(CurrentTime.Season, CurrentTime);
		
		// NEW: Emit GameEventBus event
		UTimeOfDayGameEventHelper::EmitSeasonChangedEvent(this, static_cast<int32>(CurrentTime.Season));
	}

	// Check for year change
	if (CurrentTime.Year != PreviousYear)
	{
		NotifyListeners_Year();
		OnYearChanged.Broadcast(CurrentTime);
	}
}

// NEW: Helper method to notify Schedule System
void UTimeOfDaySubsystem::NotifyScheduleSystem()
{
	// SAFE: Check if ScheduleSubsystem exists and is ready
	if (UWorld* World = GetWorld())
	{
		// Try to get ScheduleSubsystem - returns nullptr if plugin not loaded
		UScheduleSubsystem* ScheduleSystem = World->GetSubsystem<UScheduleSubsystem>();
		
		// FIX: Extra safety check - subsystem exists and world is valid
		if (ScheduleSystem && ScheduleSystem->GetWorld())
		{
			// Convert ETimeOfDayWeekday to int32 (0=Sunday, 1=Monday, etc.)
			const int32 DayOfWeek = static_cast<int32>(CurrentTime.DayOfWeek);
			
			// Notify with current time
			ScheduleSystem->NotifyTimeChanged(CurrentTime.Hour, CurrentTime.Minute, DayOfWeek);
			
			// Optional: Log for debugging (comment out in production)
			// UE_LOG(LogTemp, VeryVerbose, TEXT("[SCHEDULE] Notified Schedule System: %02d:%02d, Day %d"), 
			//     CurrentTime.Hour, CurrentTime.Minute, DayOfWeek);
		}
	}
}

void UTimeOfDaySubsystem::UpdatePhase()
{
	if (Settings)
	{
		CurrentPhase = Settings->GetPhaseEnumForTime(CurrentTime.Hour, CurrentTime.Minute);
	}
}

// ========== Listener Notification Methods ==========

void UTimeOfDaySubsystem::NotifyListeners_Minute()
{
	for (const TScriptInterface<ITimeOfDayListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			ITimeOfDayListener::Execute_OnMinuteChanged(Listener.GetObject(), CurrentTime);
		}
	}
}

void UTimeOfDaySubsystem::NotifyListeners_Hour()
{
	for (const TScriptInterface<ITimeOfDayListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			ITimeOfDayListener::Execute_OnHourChanged(Listener.GetObject(), CurrentTime);
		}
	}
}

void UTimeOfDaySubsystem::NotifyListeners_Day()
{
	for (const TScriptInterface<ITimeOfDayListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			ITimeOfDayListener::Execute_OnDayChanged(Listener.GetObject(), CurrentTime);
		}
	}
}

void UTimeOfDaySubsystem::NotifyListeners_Phase(ETimeOfDayPhase NewPhase)
{
	for (const TScriptInterface<ITimeOfDayListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			ITimeOfDayListener::Execute_OnDayPhaseChanged(Listener.GetObject(), NewPhase, CurrentTime);
		}
	}
}

void UTimeOfDaySubsystem::NotifyListeners_Season(ETimeOfDaySeason NewSeason)
{
	for (const TScriptInterface<ITimeOfDayListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			ITimeOfDayListener::Execute_OnSeasonChanged(Listener.GetObject(), NewSeason, CurrentTime);
		}
	}
}

void UTimeOfDaySubsystem::NotifyListeners_Year()
{
	for (const TScriptInterface<ITimeOfDayListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			ITimeOfDayListener::Execute_OnYearChanged(Listener.GetObject(), CurrentTime);
		}
	}
}

void UTimeOfDaySubsystem::NotifyListeners_TimeScale(float NewScale)
{
	for (const TScriptInterface<ITimeOfDayListener>& Listener : RegisteredListeners)
	{
		if (Listener.GetObject())
		{
			ITimeOfDayListener::Execute_OnTimeScaleChanged(Listener.GetObject(), NewScale);
		}
	}
}

// ========== Helper Calculations ==========

ETimeOfDayWeekday UTimeOfDaySubsystem::CalculateWeekday(int32 TotalDays) const
{
	const int32 DayIndex = TotalDays % CalendarSettings.DaysPerWeek;
	return static_cast<ETimeOfDayWeekday>(DayIndex);
}

void UTimeOfDaySubsystem::CalculateSeasonAndDay(int32 TotalDays, ETimeOfDaySeason& OutSeason, int32& OutDayOfSeason) const
{
	const int32 DaysPerYear = CalendarSettings.GetDaysPerYear(); // 360 days (30*12)
	const int32 DayOfYear = TotalDays % DaysPerYear; // 0-359
	
	// CORRECT mapping:
	// Year starts with January (DayOfYear 0)
	// But seasons need to align with real-world months:
	// - January (0-29), February (30-59) = Winter (last 2 months)
	// - March (60-89), April (90-119), May (120-149) = Spring
	// - June (150-179), July (180-209), August (210-239) = Summer
	// - September (240-269), October (270-299), November (300-329) = Autumn
	// - December (330-359) = Winter (first month)
	
	if (DayOfYear < 60)
	{
		// January (0-29) = Winter day 31-60
		// February (30-59) = Winter day 61-90
		OutSeason = ETimeOfDaySeason::Winter;
		OutDayOfSeason = 31 + DayOfYear; // 31-90
	}
	else if (DayOfYear < 150)
	{
		// March (60-89), April (90-119), May (120-149) = Spring (1-90)
		OutSeason = ETimeOfDaySeason::Spring;
		OutDayOfSeason = (DayOfYear - 60) + 1; // 1-90
	}
	else if (DayOfYear < 240)
	{
		// June (150-179), July (180-209), August (210-239) = Summer (1-90)
		OutSeason = ETimeOfDaySeason::Summer;
		OutDayOfSeason = (DayOfYear - 150) + 1; // 1-90
	}
	else if (DayOfYear < 330)
	{
		// September (240-269), October (270-299), November (300-329) = Autumn (1-90)
		OutSeason = ETimeOfDaySeason::Autumn;
		OutDayOfSeason = (DayOfYear - 240) + 1; // 1-90
	}
	else
	{
		// December (330-359) = Winter day 1-30
		OutSeason = ETimeOfDaySeason::Winter;
		OutDayOfSeason = (DayOfYear - 330) + 1; // 1-30
	}
}

void UTimeOfDaySubsystem::CheckCelestialEvents()
{
	// Get current state
	const int32 CurrentMinutes = CurrentTime.Hour * 60 + CurrentTime.Minute;
	const int32 SunriseMinutes = GetSunriseMinutes();
	const int32 SunsetMinutes = GetSunsetMinutes();
	
	const bool bIsSunUp = (CurrentMinutes >= SunriseMinutes && CurrentMinutes < SunsetMinutes);
	const bool bIsMoonUp = !bIsSunUp; // Moon is up when sun is down
	
	// Check for SUNRISE (sun transitions from down to up)
	if (bIsSunUp && !bPreviousWasSunUp)
	{
		OnSunrise.Broadcast(CurrentTime);
		UE_LOG(LogTemp, Log, TEXT("[SUNRISE] Sunrise at %s"), *GetFormattedTime());
		
		// NEW: Emit GameEventBus event
		UTimeOfDayGameEventHelper::EmitSunriseEvent(this);
	}
	
	// Check for SUNSET (sun transitions from up to down)
	if (!bIsSunUp && bPreviousWasSunUp)
	{
		OnSunset.Broadcast(CurrentTime);
		UE_LOG(LogTemp, Log, TEXT("[SUNSET] Sunset at %s"), *GetFormattedTime());
		
		// NEW: Emit GameEventBus event
		UTimeOfDayGameEventHelper::EmitSunsetEvent(this);
	}
	
	// Check for MOONRISE (moon transitions from down to up)
	// Moon rises when sun sets
	if (bIsMoonUp && !bPreviousWasMoonUp)
	{
		const float MoonPhase = GetMoonPhase();
		OnMoonrise.Broadcast(MoonPhase, CurrentTime);
		UE_LOG(LogTemp, Log, TEXT("[MOONRISE] Moonrise at %s (Phase: %.2f)"), *GetFormattedTime(), MoonPhase);
	}
	
	// Check for MOONSET (moon transitions from up to down)
	// Moon sets when sun rises
	if (!bIsMoonUp && bPreviousWasMoonUp)
	{
		const float MoonPhase = GetMoonPhase();
		OnMoonset.Broadcast(MoonPhase, CurrentTime);
		UE_LOG(LogTemp, Log, TEXT("[MOONSET] Moonset at %s (Phase: %.2f)"), *GetFormattedTime(), MoonPhase);
	}
	
	// Update previous states
	bPreviousWasSunUp = bIsSunUp;
	bPreviousWasMoonUp = bIsMoonUp;
}

