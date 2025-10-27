// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/TimeOfDayTypes.h"

// ========== FTimeOfDayTime Sun/Daylight Methods ==========

float FTimeOfDayTime::GetSunPosition(const FDaylightSettings& DaylightSettings) const
{
	const int32 CurrentMinutes = Hour * 60 + Minute;
	return DaylightSettings.GetNormalizedSunPosition(CurrentMinutes, Season);
}

float FTimeOfDayTime::GetSunAltitude(const FDaylightSettings& DaylightSettings) const
{
	const int32 CurrentMinutes = Hour * 60 + Minute;
	return DaylightSettings.GetSunAltitude(CurrentMinutes, Season);
}

bool FTimeOfDayTime::IsDaylightHours(const FDaylightSettings& DaylightSettings) const
{
	const int32 CurrentMinutes = Hour * 60 + Minute;
	const int32 SunriseMinutes = DaylightSettings.GetSunriseMinutes(Season);
	const int32 SunsetMinutes = DaylightSettings.GetSunsetMinutes(Season);
	
	return CurrentMinutes >= SunriseMinutes && CurrentMinutes < SunsetMinutes;
}
