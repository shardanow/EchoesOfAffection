// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/TimeOfDayListenerComponent.h"
#include "Subsystems/TimeOfDaySubsystem.h"
#include "Engine/World.h"

UTimeOfDayListenerComponent::UTimeOfDayListenerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTimeOfDayListenerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register with the TimeOfDaySubsystem using BP_ method to avoid interface assertion
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			TimeSubsystem->BP_RegisterListener(this);  // ? Using BP_ version
			UE_LOG(LogTemp, Log, TEXT("TimeOfDayListenerComponent registered for actor: %s"), *GetOwner()->GetName());
		}
	}
}

void UTimeOfDayListenerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from the TimeOfDaySubsystem using BP_ method to avoid interface assertion
	if (UWorld* World = GetWorld())
	{
		if (UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>())
		{
			TimeSubsystem->BP_UnregisterListener(this);  // ? Using BP_ version
		}
	}

	Super::EndPlay(EndPlayReason);
}

// ========== ITimeOfDayListener Interface Implementation ==========

void UTimeOfDayListenerComponent::OnMinuteChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	if (bReceiveMinuteEvents)
	{
		BP_OnMinuteChanged(CurrentTime);
	}
}

void UTimeOfDayListenerComponent::OnHourChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	if (bReceiveHourEvents)
	{
		BP_OnHourChanged(CurrentTime);
	}
}

void UTimeOfDayListenerComponent::OnDayChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	if (bReceiveDayEvents)
	{
		BP_OnDayChanged(CurrentTime);
	}
}

void UTimeOfDayListenerComponent::OnDayPhaseChanged_Implementation(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime)
{
	if (bReceivePhaseEvents)
	{
		BP_OnDayPhaseChanged(NewPhase, CurrentTime);
	}
}

void UTimeOfDayListenerComponent::OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime)
{
	if (bReceiveSeasonEvents)
	{
		BP_OnSeasonChanged(NewSeason, CurrentTime);
	}
}

void UTimeOfDayListenerComponent::OnYearChanged_Implementation(const FTimeOfDayTime& CurrentTime)
{
	if (bReceiveYearEvents)
	{
		BP_OnYearChanged(CurrentTime);
	}
}

void UTimeOfDayListenerComponent::OnTimeScaleChanged_Implementation(float NewTimeScale)
{
	if (bReceiveTimeScaleEvents)
	{
		BP_OnTimeScaleChanged(NewTimeScale);
	}
}

