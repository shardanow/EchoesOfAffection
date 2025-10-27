// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/TimeOfDayTypes.h"
#include "TimeOfDaySettings.generated.h"

/**
 * Data Asset containing all configuration for the Time of Day System
 * Allows designers to configure time behavior without touching code
 */
UCLASS(BlueprintType)
class TIMEOFDAYCORE_API UTimeOfDaySettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UTimeOfDaySettings();

	// ========== Time Conversion Settings ==========

	/** Time conversion and scaling settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Time Conversion")
	FTimeConversionSettings TimeConversion;

	// ========== Calendar Settings ==========

	/** Calendar configuration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Calendar")
	FCalendarSettings Calendar;

	/** Starting game time when a new game begins */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Calendar")
	FTimeOfDayTime InitialGameTime;

	/** Custom month names (12 names for 12 months). If empty, uses default names. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Calendar", meta = (EditCondition = "MonthNames.Num() == 0 || MonthNames.Num() == 12"))
	TArray<FString> MonthNames;

	// ========== Daylight Settings ==========

	/** Seasonal daylight configuration (sunrise/sunset times) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Daylight")
	FDaylightSettings Daylight;

	// ========== Day Phase Settings ==========

	/** Definitions for each phase of the day */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Day Phases")
	TArray<FDayPhaseDefinition> DayPhases;

	// ========== Advanced Settings ==========

	/** Whether to automatically advance time (set to false for turn-based games) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced")
	bool bAutoAdvanceTime = true;

	/** Update frequency in Hz (how many times per second to tick the time system) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced", meta = (ClampMin = "1.0", ClampMax = "60.0"))
	float UpdateFrequency = 10.0f;

	/** Whether to save/load time state with save games */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced")
	bool bPersistTimeState = true;

	// ========== Helper Functions ==========

	/** Get the phase definition for a given time */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	FDayPhaseDefinition GetPhaseForTime(int32 Hour, int32 Minute) const;

	/** Get the current phase enum for a given time */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	ETimeOfDayPhase GetPhaseEnumForTime(int32 Hour, int32 Minute) const;

	/** Validate and sort day phases */
	void ValidatePhases();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};




