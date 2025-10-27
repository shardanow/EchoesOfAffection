// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/TimeClockWidget.h"
#include "Subsystems/TimeOfDaySubsystem.h"
#include "Components/TextBlock.h"

UTimeClockWidget::UTimeClockWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTimeClockWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			TimeSubsystem->BP_RegisterListener(this);  // ? Using BP_ version
			UpdateClock();
		}
	}
}

void UTimeClockWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			TimeSubsystem->BP_UnregisterListener(this);  // ? Using BP_ version
		}
	}

	Super::NativeDestruct();
}

void UTimeClockWidget::OnMinuteChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	UpdateClock();
}

void UTimeClockWidget::OnHourChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	UpdateClock();
}

void UTimeClockWidget::OnDayChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	UpdateClock();
}

void UTimeClockWidget::OnDayPhaseChanged_Implementation(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime)
{
	UpdateColor(NewPhase);
}

void UTimeClockWidget::OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime)
{
}

void UTimeClockWidget::OnYearChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
}

void UTimeClockWidget::OnTimeScaleChanged_Implementation(float NewTimeScale)
{
}

void UTimeClockWidget::UpdateClock()
{
	if (!ClockText)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			// Use subsystem's formatted time instead of direct FTimeOfDayTime access
			const FString FormattedTime = TimeSubsystem->GetFormattedTime(TimeFormat.Contains(TEXT("{Second")));
			ClockText->SetText(FText::FromString(FormattedTime));

			if (bColorChangesWithDayPhase)
			{
				UpdateColor(TimeSubsystem->BP_GetCurrentPhase());  // ? Using BP_ version
			}
		}
	}
}

FString UTimeClockWidget::FormatTime(const FTimeOfDayTime& Time) const
{
	// This method is deprecated - use subsystem's GetFormattedTime instead
	// Kept for compatibility
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			return TimeSubsystem->GetFormattedTime(TimeFormat.Contains(TEXT("{Second")));
		}
	}
	
	return TEXT("--:--");
}

void UTimeClockWidget::UpdateColor(ETimeOfDayPhase Phase)
{
	if (!ClockText || !bColorChangesWithDayPhase)
	{
		return;
	}

	const bool bIsNight = (Phase == ETimeOfDayPhase::Night || Phase == ETimeOfDayPhase::Evening);
	const FLinearColor TargetColor = bIsNight ? NightColor : DayColor;
	
	ClockText->SetColorAndOpacity(FSlateColor(TargetColor));
}

