// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimeOfDayTypes.generated.h"

/**
 * Enumeration representing the seasons in the game world
 */
UENUM(BlueprintType)
enum class ETimeOfDaySeason : uint8
{
	Spring UMETA(DisplayName = "Spring"),
	Summer UMETA(DisplayName = "Summer"),
	Autumn UMETA(DisplayName = "Autumn"),
	Winter UMETA(DisplayName = "Winter")
};

/**
 * Enumeration representing the phases of the day
 */
UENUM(BlueprintType)
enum class ETimeOfDayPhase : uint8
{
	Night UMETA(DisplayName = "Night"),
	Dawn UMETA(DisplayName = "Dawn"),
	Morning UMETA(DisplayName = "Morning"),
	Noon UMETA(DisplayName = "Noon"),
	Afternoon UMETA(DisplayName = "Afternoon"),
	Dusk UMETA(DisplayName = "Dusk"),
	Evening UMETA(DisplayName = "Evening")
};

/**
 * Enumeration representing days of the week
 */
UENUM(BlueprintType)
enum class ETimeOfDayWeekday : uint8
{
	Monday UMETA(DisplayName = "Monday"),
	Tuesday UMETA(DisplayName = "Tuesday"),
	Wednesday UMETA(DisplayName = "Wednesday"),
	Thursday UMETA(DisplayName = "Thursday"),
	Friday UMETA(DisplayName = "Friday"),
	Saturday UMETA(DisplayName = "Saturday"),
	Sunday UMETA(DisplayName = "Sunday")
};

/**
 * Enumeration representing different date format styles
 */
UENUM(BlueprintType)
enum class EDateFormat : uint8
{
	/** DD Month YYYY (Monday) - Example: "15 March, Year 1 (Monday)" */
	Full UMETA(DisplayName = "Full (15 March, Year 1 (Monday))"),
	
	/** DD.MM.YY - Example: "15.03.01" */
	DotShort UMETA(DisplayName = "DD.MM.YY (15.03.01)"),
	
	/** DD/MM/YYYY - Example: "15/03/2001" */
	SlashEuropean UMETA(DisplayName = "DD/MM/YYYY (15/03/2001)"),
	
	/** MM/DD/YYYY - Example: "03/15/2001" */
	SlashAmerican UMETA(DisplayName = "MM/DD/YYYY (03/15/2001)"),
	
	/** YYYY-MM-DD - Example: "2001-03-15" (ISO 8601) */
	ISO UMETA(DisplayName = "YYYY-MM-DD (2001-03-15)"),
	
	/** DD.MM.YYYY - Example: "15.03.2001" */
	DotEuropean UMETA(DisplayName = "DD.MM.YYYY (15.03.2001)")
};

// Forward declaration
struct FDaylightSettings;

/**
 * Structure representing a specific game time
 * Contains all components needed to represent a moment in game time
 */
USTRUCT(BlueprintType)
struct TIMEOFDAYCORE_API FTimeOfDayTime
{
	GENERATED_BODY()

	/** Current year */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 Year = 1;

	/** Current season (0-3) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	ETimeOfDaySeason Season = ETimeOfDaySeason::Spring;

	/** Current day within the season */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 DayOfSeason = 1;

	/** Current day of the week (0-6) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	ETimeOfDayWeekday DayOfWeek = ETimeOfDayWeekday::Monday;

	/** Current hour (0-23) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0", ClampMax = "23"))
	int32 Hour = 6;

	/** Current minute (0-59) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0", ClampMax = "59"))
	int32 Minute = 0;

	/** Current second (0-59) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0", ClampMax = "59"))
	int32 Second = 0;

	/** Total elapsed game time in seconds since game start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	double TotalElapsedSeconds = 0.0;

	FTimeOfDayTime()
		: Year(1)
		, Season(ETimeOfDaySeason::Winter)    // ? FIXED! Start with Winter (January)
		, DayOfSeason(31)         // ? FIXED! January 1st = Winter day 31
		, DayOfWeek(ETimeOfDayWeekday::Monday)
		, Hour(6)
		, Minute(0)
		, Second(0)
		, TotalElapsedSeconds(0.0)
	{
	}

	/** Get total minutes since midnight */
	FORCEINLINE int32 GetMinutesSinceMidnight() const
	{
		return Hour * 60 + Minute;
	}

	/** Get normalized time of day (0.0 = midnight, 0.5 = noon, 1.0 = midnight) */
	FORCEINLINE float GetNormalizedTimeOfDay() const
	{
		return (Hour * 3600.0f + Minute * 60.0f + Second) / 86400.0f;
	}

	/** Get total days since game start */
	FORCEINLINE int32 GetTotalDays() const
	{
		return static_cast<int32>(TotalElapsedSeconds / 86400.0);
	}

	/** Get sun position for directional light rotation (requires daylight settings) */
	float GetSunPosition(const FDaylightSettings& DaylightSettings) const;

	/** Get sun altitude angle in degrees (0 = horizon, 90 = zenith) */
	float GetSunAltitude(const FDaylightSettings& DaylightSettings) const;

	/** Check if it's currently daytime based on seasonal sunrise/sunset */
	bool IsDaylightHours(const FDaylightSettings& DaylightSettings) const;

	/** Equality operator */
	bool operator==(const FTimeOfDayTime& Other) const
	{
		return Year == Other.Year
			&& Season == Other.Season
			&& DayOfSeason == Other.DayOfSeason
			&& Hour == Other.Hour
			&& Minute == Other.Minute
			&& Second == Other.Second;
	}

	/** Inequality operator */
	bool operator!=(const FTimeOfDayTime& Other) const
	{
		return !(*this == Other);
	}

	/** Convert to formatted string */
	FString ToString() const
	{
		const FString SeasonName = StaticEnum<ETimeOfDaySeason>()->GetDisplayNameTextByValue((int64)Season).ToString();
		return FString::Printf(TEXT("Year %d, %s Day %d, %02d:%02d:%02d"),
			Year,
			*SeasonName,
			DayOfSeason,
			Hour,
			Minute,
			Second);
	}
};

/**
 * Structure defining the time range for a day phase
 */
USTRUCT(BlueprintType)
struct TIMEOFDAYCORE_API FDayPhaseDefinition
{
	GENERATED_BODY()

	/** Phase identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	ETimeOfDayPhase Phase = ETimeOfDayPhase::Morning;

	/** Start hour of this phase (0-23) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase", meta = (ClampMin = "0", ClampMax = "23"))
	int32 StartHour = 6;

	/** Start minute of this phase (0-59) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase", meta = (ClampMin = "0", ClampMax = "59"))
	int32 StartMinute = 0;

	/** Display name for this phase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	FText DisplayName;

	/** Optional description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	FText Description;

	FDayPhaseDefinition()
		: Phase(ETimeOfDayPhase::Morning)
		, StartHour(6)
		, StartMinute(0)
	{
	}

	FDayPhaseDefinition(ETimeOfDayPhase InPhase, int32 InHour, int32 InMinute)
		: Phase(InPhase)
		, StartHour(InHour)
		, StartMinute(InMinute)
	{
	}

	/** Get start time in minutes since midnight */
	FORCEINLINE int32 GetStartMinutes() const
	{
		return StartHour * 60 + StartMinute;
	}
};

/**
 * Structure representing time conversion settings
 */
USTRUCT(BlueprintType)
struct TIMEOFDAYCORE_API FTimeConversionSettings
{
	GENERATED_BODY()

	/** How many real seconds equal one game day (default 1200 = 20 minutes) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Conversion", meta = (ClampMin = "1.0"))
	float RealSecondsPerGameDay = 1200.0f;

	/** Current time scale multiplier (1.0 = normal speed, 2.0 = 2x speed, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Conversion", meta = (ClampMin = "0.0"))
	float TimeScale = 1.0f;

	/** Whether time is currently paused */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Conversion")
	bool bIsPaused = false;

	FTimeConversionSettings()
		: RealSecondsPerGameDay(1200.0f)
		, TimeScale(1.0f)
		, bIsPaused(false)
	{
	}

	/** Get the number of game seconds that pass per real second */
	FORCEINLINE float GetGameSecondsPerRealSecond() const
	{
		if (bIsPaused || TimeScale <= 0.0f)
		{
			return 0.0f;
		}
		return (86400.0f / RealSecondsPerGameDay) * TimeScale;
	}

	/** Convert real time delta to game time delta */
	FORCEINLINE float ConvertRealToGameDelta(float RealDeltaSeconds) const
	{
		return RealDeltaSeconds * GetGameSecondsPerRealSecond();
	}
};

/**
 * Structure representing calendar configuration
 */
USTRUCT(BlueprintType)
struct TIMEOFDAYCORE_API FCalendarSettings
{
	GENERATED_BODY()

	/** Number of days in each season (3 months ? 30 days = 90) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calendar", meta = (ClampMin = "1"))
	int32 DaysPerSeason = 90;

	/** Number of seasons in a year (typically 4) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calendar", meta = (ClampMin = "1"))
	int32 SeasonsPerYear = 4;

	/** Number of days in a week */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calendar", meta = (ClampMin = "1"))
	int32 DaysPerWeek = 7;

	/** Names of months (optional, for display purposes) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calendar")
	TArray<FText> MonthNames;

	FCalendarSettings()
		: DaysPerSeason(90)
		, SeasonsPerYear(4)
		, DaysPerWeek(7)
	{
	}

	/** Get total days in a year (360 days = 12 months ? 30 days) */
	FORCEINLINE int32 GetDaysPerYear() const
	{
		return DaysPerSeason * SeasonsPerYear;  // 90 ? 4 = 360 days
	}
};

/**
 * Structure representing daylight settings for seasonal changes
 * Controls sunrise/sunset times and day length based on season
 */
USTRUCT(BlueprintType)
struct TIMEOFDAYCORE_API FDaylightSettings
{
	GENERATED_BODY()

	/** Enable seasonal daylight changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight")
	bool bEnableSeasonalDaylight = true;

	// ========== Summer Settings ========= =
	
	/** Sunrise time in summer (hour, 0-23) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight|Summer", meta = (ClampMin = "0", ClampMax = "23", EditCondition = "bEnableSeasonalDaylight"))
	int32 SummerSunriseHour = 5;

	/** Sunrise time in summer (minute, 0-59) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight|Summer", meta = (ClampMin = "0", ClampMax = "59", EditCondition = "bEnableSeasonalDaylight"))
	int32 SummerSunriseMinute = 0;

	/** Sunset time in summer (hour, 0-23) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight|Summer", meta = (ClampMin = "0", ClampMax = "23", EditCondition = "bEnableSeasonalDaylight"))
	int32 SummerSunsetHour = 21;

	/** Sunset time in summer (minute, 0-59) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight|Summer", meta = (ClampMin = "0", ClampMax = "59", EditCondition = "bEnableSeasonalDaylight"))
	int32 SummerSunsetMinute = 0;

	// ========== Winter Settings ========= =
	
	/** Sunrise time in winter (hour, 0-23) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight|Winter", meta = (ClampMin = "0", ClampMax = "23", EditCondition = "bEnableSeasonalDaylight"))
	int32 WinterSunriseHour = 8;

	/** Sunrise time in winter (minute, 0-59) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight|Winter", meta = (ClampMin = "0", ClampMax = "59", EditCondition = "bEnableSeasonalDaylight"))
	int32 WinterSunriseMinute = 0;

	/** Sunset time in winter (hour, 0-23) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight|Winter", meta = (ClampMin = "0", ClampMax = "23", EditCondition = "bEnableSeasonalDaylight"))
	int32 WinterSunsetHour = 16;

	/** Sunset time in winter (minute, 0-59) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Daylight|Winter", meta = (ClampMin = "0", ClampMax = "59", EditCondition = "bEnableSeasonalDaylight"))
	int32 WinterSunsetMinute = 0;

	FDaylightSettings()
		: bEnableSeasonalDaylight(true)
		, SummerSunriseHour(5)
		, SummerSunriseMinute(0)
		, SummerSunsetHour(21)
		, SummerSunsetMinute(0)
		, WinterSunriseHour(8)
		, WinterSunriseMinute(0)
		, WinterSunsetHour(16)
		, WinterSunsetMinute(0)
	{
	}

	/** Get sunrise time in minutes for a specific season */
	FORCEINLINE int32 GetSunriseMinutes(ETimeOfDaySeason Season) const
	{
		if (!bEnableSeasonalDaylight)
		{
			// Default: 6:00 AM
			return 6 * 60;
		}

		switch (Season)
		{
			case ETimeOfDaySeason::Summer:
				return SummerSunriseHour * 60 + SummerSunriseMinute;
			
			case ETimeOfDaySeason::Winter:
				return WinterSunriseHour * 60 + WinterSunriseMinute;
			
			case ETimeOfDaySeason::Spring:
			case ETimeOfDaySeason::Autumn:
				// Interpolate between summer and winter
				{
					const int32 SummerMinutes = SummerSunriseHour * 60 + SummerSunriseMinute;
					const int32 WinterMinutes = WinterSunriseHour * 60 + WinterSunriseMinute;
					return (SummerMinutes + WinterMinutes) / 2;
				}
			
			default:
				return 6 * 60; // Fallback
		}
	}

	/** Get sunset time in minutes for a specific season */
	FORCEINLINE int32 GetSunsetMinutes(ETimeOfDaySeason Season) const
	{
		if (!bEnableSeasonalDaylight)
		{
			// Default: 18:00 (6 PM)
			return 18 * 60;
		}

		switch (Season)
		{
			case ETimeOfDaySeason::Summer:
				return SummerSunsetHour * 60 + SummerSunsetMinute;
			
			case ETimeOfDaySeason::Winter:
				return WinterSunsetHour * 60 + WinterSunsetMinute;
			
			case ETimeOfDaySeason::Spring:
			case ETimeOfDaySeason::Autumn:
				// Interpolate between summer and winter
				{
					const int32 SummerMinutes = SummerSunsetHour * 60 + SummerSunsetMinute;
					const int32 WinterMinutes = WinterSunsetHour * 60 + WinterSunsetMinute;
					return (SummerMinutes + WinterMinutes) / 2;
				}
			
			default:
				return 18 * 60; // Fallback
		}
	}

	/** Get day length in minutes for a specific season */
	FORCEINLINE int32 GetDayLengthMinutes(ETimeOfDaySeason Season) const
	{
		return GetSunsetMinutes(Season) - GetSunriseMinutes(Season);
	}

	/** Get normalized sun position (0.0 = sunrise, 0.5 = noon, 1.0 = sunset) for current time */
	FORCEINLINE float GetNormalizedSunPosition(int32 CurrentMinutes, ETimeOfDaySeason Season) const
	{
		const int32 SunriseMinutes = GetSunriseMinutes(Season);
		const int32 SunsetMinutes = GetSunsetMinutes(Season);

		// Before sunrise or after sunset = night
		if (CurrentMinutes < SunriseMinutes || CurrentMinutes >= SunsetMinutes)
		{
			return -1.0f; // Night time
		}

		// During day: interpolate from 0.0 (sunrise) to 1.0 (sunset)
		const int32 DayLength = SunsetMinutes - SunriseMinutes;
		const int32 MinutesSinceSunrise = CurrentMinutes - SunriseMinutes;
		
		return static_cast<float>(MinutesSinceSunrise) / static_cast<float>(DayLength);
	}

	/** Get sun altitude angle in degrees (0 = horizon, 90 = zenith) */
	FORCEINLINE float GetSunAltitude(int32 CurrentMinutes, ETimeOfDaySeason Season) const
	{
		const float NormalizedPos = GetNormalizedSunPosition(CurrentMinutes, Season);
		
		if (NormalizedPos < 0.0f)
		{
			return -10.0f; // Below horizon at night
		}

		// Simple arc: 0° at sunrise/sunset, 90° at noon
		// Using sine wave for smooth transition
		const float Radians = NormalizedPos * PI;
		return FMath::Sin(Radians) * 90.0f;
	}
};













