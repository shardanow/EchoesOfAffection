// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/TimeOfDayWidget.h"
#include "Core/TimeOfDayTypes.h"
#include "Subsystems/TimeOfDaySubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

UTimeOfDayWidget::UTimeOfDayWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoUpdate = true;
	bShowSeconds = false;
	bUse12HourFormat = false;
	bShowDate = true;
	bShowSeason = true;
	bShowPhase = true;
}

void UTimeOfDayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Register with TimeOfDaySubsystem using BP_ method to avoid interface assertion
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			TimeSubsystem->BP_RegisterListener(this);  // ? Using BP_ version
			
			// Initial refresh
			RefreshDisplay();
		}
	}
}

void UTimeOfDayWidget::NativeDestruct()
{
	// Unregister from TimeOfDaySubsystem using BP_ method to avoid interface assertion
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			TimeSubsystem->BP_UnregisterListener(this);  // ? Using BP_ version
		}
	}

	Super::NativeDestruct();
}

// ========== ITimeOfDayListener Interface ==========

void UTimeOfDayWidget::OnMinuteChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	if (bAutoUpdate)
	{
		RefreshDisplay();
	}
}

void UTimeOfDayWidget::OnHourChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	if (bAutoUpdate)
	{
		RefreshDisplay();
	}
}

void UTimeOfDayWidget::OnDayChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	if (bAutoUpdate)
	{
		RefreshDisplay();
	}
}

void UTimeOfDayWidget::OnDayPhaseChanged_Implementation(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime)
{
	BP_OnPhaseChanged(NewPhase);
	
	if (bAutoUpdate)
	{
		RefreshDisplay();
	}
}

void UTimeOfDayWidget::OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime)
{
	BP_OnSeasonChanged(NewSeason);
	
	if (bAutoUpdate)
	{
		RefreshDisplay();
	}
}

void UTimeOfDayWidget::OnYearChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	if (bAutoUpdate)
	{
		RefreshDisplay();
	}
}

void UTimeOfDayWidget::OnTimeScaleChanged_Implementation(float NewTimeScale)
{
	// Can be overridden in Blueprint if needed
}

// ========== Public Functions ==========

void UTimeOfDayWidget::RefreshDisplay()
{
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			const ETimeOfDayPhase CurrentPhase = TimeSubsystem->BP_GetCurrentPhase();  // ? Using BP_ version
			
			// Call simplified update that doesn't use FTimeOfDayTime directly
			UpdateWidgetElementsSimplified(TimeSubsystem, CurrentPhase);
			
			// Note: BP_UpdateDisplay will need to be reworked or removed due to FTimeOfDayTime forward declaration issues
			// For now, skip it - Blueprint implementations can override UpdateWidgetElements instead
		}
	}
}

FString UTimeOfDayWidget::GetFormattedTimeString(const FTimeOfDayTime& Time) const
{
	// Use subsystem to avoid forward-declaration issues
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			if (bUse12HourFormat)
			{
				return TimeSubsystem->GetFormattedTime12Hour(bShowSeconds);
			}
			else
			{
				return TimeSubsystem->GetFormattedTime(bShowSeconds);
			}
		}
	}
	
	// Fallback
	return TEXT("--:--");
}

FString UTimeOfDayWidget::GetFormattedDateString(const FTimeOfDayTime& Time) const
{
	// Get values through subsystem to avoid forward-declaration issues
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			return TimeSubsystem->GetFormattedDateWithFormat(DateFormat);
		}
	}
	
	// Fallback
	return TEXT("Unknown Date");
}

// ========== Protected Functions ==========

void UTimeOfDayWidget::UpdateWidgetElements(const FTimeOfDayTime& CurrentTime, ETimeOfDayPhase CurrentPhase)
{
	// This method is kept for interface compatibility but uses subsystem internally
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			UpdateWidgetElementsSimplified(TimeSubsystem, CurrentPhase);
		}
	}
}

void UTimeOfDayWidget::UpdateWidgetElementsSimplified(UTimeOfDaySubsystem* TimeSubsystem, ETimeOfDayPhase CurrentPhase)
{
	if (!TimeSubsystem)
	{
		return;
	}

	// Update time text with 12/24-hour format support
	if (TimeText)
	{
		FString TimeString;
		if (bUse12HourFormat)
		{
			TimeString = TimeSubsystem->GetFormattedTime12Hour(bShowSeconds);
		}
		else
		{
			TimeString = TimeSubsystem->GetFormattedTime(bShowSeconds);
		}
		TimeText->SetText(FText::FromString(TimeString));
	}

	// Update date text
	if (DateText && bShowDate)
	{
		DateText->SetText(FText::FromString(TimeSubsystem->GetFormattedDateWithFormat(DateFormat)));
	}

	// Update season text
	if (SeasonText && bShowSeason)
	{
		const ETimeOfDaySeason CurrentSeason = TimeSubsystem->BP_GetCurrentSeason();  // ? Using BP_ version
		const FText SeasonDisplayText = StaticEnum<ETimeOfDaySeason>()->GetDisplayNameTextByValue((int64)CurrentSeason);
		SeasonText->SetText(SeasonDisplayText);
	}

	// Update phase text
	if (PhaseText && bShowPhase)
	{
		const FText PhaseDisplayText = StaticEnum<ETimeOfDayPhase>()->GetDisplayNameTextByValue((int64)CurrentPhase);
		PhaseText->SetText(PhaseDisplayText);
	}

	// Update day progress bar
	if (DayProgressBar)
	{
		const float Progress = TimeSubsystem->BP_GetNormalizedTimeOfDay();  // ? Using BP_ version
		DayProgressBar->SetPercent(Progress);
	}
}

FString UTimeOfDayWidget::GetAMPMSuffix(int32 Hour) const
{
	return (Hour < 12) ? TEXT("AM") : TEXT("PM");
}

int32 UTimeOfDayWidget::Convert24To12Hour(int32 Hour) const
{
	if (Hour == 0)
	{
		return 12; // Midnight
	}
	else if (Hour > 12)
	{
		return Hour - 12;
	}
	return Hour;
}

