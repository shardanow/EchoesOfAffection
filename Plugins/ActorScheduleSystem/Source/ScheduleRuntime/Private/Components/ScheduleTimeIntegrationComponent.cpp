// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/ScheduleTimeIntegrationComponent.h"
#include "Subsystems/ScheduleSubsystem.h"
#include "Engine/World.h"

UScheduleTimeIntegrationComponent::UScheduleTimeIntegrationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	// Default event names that common time systems might use
	TimeChangeEventNames.Add(TEXT("OnMinuteChanged"));
	TimeChangeEventNames.Add(TEXT("OnHourChanged"));
	TimeChangeEventNames.Add(TEXT("OnTimeUpdated"));
}

void UScheduleTimeIntegrationComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		ScheduleSubsystem = World->GetSubsystem<UScheduleSubsystem>();
	}

	if (bIntegrationEnabled)
	{
		SubscribeToTimeEvents();
	}
}

void UScheduleTimeIntegrationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnsubscribeFromTimeEvents();
	Super::EndPlay(EndPlayReason);
}

void UScheduleTimeIntegrationComponent::NotifyTimeChanged(int32 Hour, int32 Minute, int32 DayOfWeek)
{
	if (!bIntegrationEnabled || !ScheduleSubsystem)
	{
		return;
	}

	// Prevent duplicate notifications
	if (Hour == LastNotifiedHour && Minute == LastNotifiedMinute)
	{
		return;
	}

	LastNotifiedHour = Hour;
	LastNotifiedMinute = Minute;

	// Forward to schedule subsystem
	ScheduleSubsystem->NotifyTimeChanged(Hour, Minute, DayOfWeek);

	UE_LOG(LogTemp, Verbose, TEXT("Schedule Time Integration: Time changed to %02d:%02d, Day %d"), 
		Hour, Minute, DayOfWeek);
}

void UScheduleTimeIntegrationComponent::SubscribeToTimeEvents()
{
	// This is a placeholder for event binding
	// In a real implementation, you would bind to delegates from your time system
	// Example:
	// if (UTimeOfDaySystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySystem>())
	// {
	//     TimeSystem->OnMinuteChanged.AddDynamic(this, &UScheduleTimeIntegrationComponent::HandleTimeChanged);
	// }

	UE_LOG(LogTemp, Log, TEXT("Schedule Time Integration: Subscribed to time events (implement binding in your project)"));
}

void UScheduleTimeIntegrationComponent::UnsubscribeFromTimeEvents()
{
	// Unbind from time system events
	// Example:
	// if (UTimeOfDaySystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySystem>())
	// {
	//     TimeSystem->OnMinuteChanged.RemoveDynamic(this, &UScheduleTimeIntegrationComponent::HandleTimeChanged);
	// }

	UE_LOG(LogTemp, Verbose, TEXT("Schedule Time Integration: Unsubscribed from time events"));
}

void UScheduleTimeIntegrationComponent::FetchCurrentTimeFromSystem_Implementation(int32& OutHour, int32& OutMinute, int32& OutDayOfWeek)
{
	// Default implementation - override in Blueprint or derived class to fetch from your time system
	// Example:
	// if (UTimeOfDaySystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySystem>())
	// {
	//     OutHour = TimeSystem->GetCurrentHour();
	//   OutMinute = TimeSystem->GetCurrentMinute();
	//     OutDayOfWeek = TimeSystem->GetCurrentDayOfWeek();
	// }

	OutHour = 12;
	OutMinute = 0;
	OutDayOfWeek = 1;
}
