// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/TimeOfDayInterface.h"
#include "Core/TimeOfDayTypes.h"
#include "TimeOfDayWidget.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
class UTimeOfDaySubsystem;

/**
 * Base widget for displaying time of day information
 * Can be subclassed in Blueprint to create custom time displays
 */
UCLASS(Abstract)
class TIMEOFDAYUI_API UTimeOfDayWidget : public UUserWidget, public ITimeOfDayListener
{
	GENERATED_BODY()

public:
	UTimeOfDayWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// ========== ITimeOfDayListener Interface ==========
	
	virtual void OnMinuteChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnHourChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnDayChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnDayPhaseChanged_Implementation(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime) override;
	virtual void OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime) override;
	virtual void OnYearChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnTimeScaleChanged_Implementation(float NewTimeScale) override;

	// ========== Blueprint Events ==========

	/** Called when the widget needs to refresh its display */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "Update Display"))
	void BP_UpdateDisplay(const FTimeOfDayTime& CurrentTime, ETimeOfDayPhase CurrentPhase);

	/** Called when the day phase changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Phase Changed"))
	void BP_OnPhaseChanged(ETimeOfDayPhase NewPhase);

	/** Called when the season changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Season Changed"))
	void BP_OnSeasonChanged(ETimeOfDaySeason NewSeason);

	// ========== Configuration ==========

	/** Whether to auto-update the display every minute */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	bool bAutoUpdate = true;

	/** Whether to show seconds in the time display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	bool bShowSeconds = false;

	/** Whether to use 12-hour format (AM/PM) instead of 24-hour */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	bool bUse12HourFormat = false;

	/** Date format to use for display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	EDateFormat DateFormat = EDateFormat::Full;

	/** Whether to show the date */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	bool bShowDate = true;

	/** Whether to show the season */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	bool bShowSeason = true;

	/** Whether to show the day phase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
	bool bShowPhase = true;

	// ========== Widget Bindings (Optional - bind in Blueprint) ==========

	/** Text block for displaying the time */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeText;

	/** Text block for displaying the date */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DateText;

	/** Text block for displaying the season */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SeasonText;

	/** Text block for displaying the phase */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PhaseText;

	/** Progress bar showing day progress (0 = midnight, 1 = next midnight) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> DayProgressBar;

	/** Image that can be used to show phase icon */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> PhaseIcon;

	// ========== Public Functions ==========

	/** Manually refresh the display */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	void RefreshDisplay();

	/** Get formatted time string based on widget settings */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	FString GetFormattedTimeString(const FTimeOfDayTime& Time) const;

	/** Get formatted date string */
	UFUNCTION(BlueprintCallable, Category = "Time of Day")
	FString GetFormattedDateString(const FTimeOfDayTime& Time) const;

protected:
	/** Update all widget elements with current time data */
	void UpdateWidgetElements(const FTimeOfDayTime& CurrentTime, ETimeOfDayPhase CurrentPhase);

	/** Internal method to update widgets using subsystem (avoids FTimeOfDayTime forward-declaration issues) */
	void UpdateWidgetElementsSimplified(UTimeOfDaySubsystem* TimeSubsystem, ETimeOfDayPhase CurrentPhase);

	/** Get AM/PM suffix for 12-hour format */
	FString GetAMPMSuffix(int32 Hour) const;

	/** Convert 24-hour to 12-hour format */
	int32 Convert24To12Hour(int32 Hour) const;
};

