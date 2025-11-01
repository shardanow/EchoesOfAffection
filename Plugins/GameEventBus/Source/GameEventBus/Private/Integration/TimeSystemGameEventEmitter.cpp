// Copyright Epic Games, Inc. All Rights Reserved.

#include "Integration/TimeSystemGameEventEmitter.h"
#include "GameEventBusSubsystem.h"
#include "GameEventBusLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogTimeEvents, Log, All);

UTimeSystemGameEventEmitter::UTimeSystemGameEventEmitter()
{
	PrimaryComponentTick.bCanEverTick = false;
	bEmitEvents = true;
	bEmitHourlyEvents = true;
	bEmitDailyEvents = true;
	bEmitWeatherEvents = true;
	bLogEvents = false;
}

void UTimeSystemGameEventEmitter::BeginPlay()
{
	Super::BeginPlay();

	if (bEmitEvents)
	{
		// Start checking for time changes every second
		GetWorld()->GetTimerManager().SetTimer(
			CheckTimerHandle,
			this,
			&UTimeSystemGameEventEmitter::CheckForTimeChanges,
			1.0f,
			true
		);

		UE_LOG(LogTimeEvents, Log, TEXT("TimeSystemGameEventEmitter: Started"));
	}
}

void UTimeSystemGameEventEmitter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UTimeSystemGameEventEmitter::SetEventEmissionEnabled(bool bEnabled)
{
	bEmitEvents = bEnabled;

	if (bEnabled && !CheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(
			CheckTimerHandle,
			this,
			&UTimeSystemGameEventEmitter::CheckForTimeChanges,
			1.0f,
			true
		);
		UE_LOG(LogTimeEvents, Log, TEXT("TimeSystemGameEventEmitter: Enabled"));
	}
	else if (!bEnabled && CheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckTimerHandle);
		CheckTimerHandle.Invalidate();
		UE_LOG(LogTimeEvents, Log, TEXT("TimeSystemGameEventEmitter: Disabled"));
	}
}

void UTimeSystemGameEventEmitter::CheckForTimeChanges()
{
	// This is a TEMPLATE implementation
	// In real project, you'd get time from your Time System (TimeManager, TimeOfDaySubsystem, etc)
	
	// PLACEHOLDER: Get time from your time system
	// int32 CurrentHour = YourTimeSystem->GetCurrentHour();
	// int32 CurrentDay = YourTimeSystem->GetCurrentDay();
	
	// For demonstration, we'll use dummy values
	// You should replace this with actual time system integration
	
	// Example: If you have a TimeOfDaySubsystem
	// UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetGameInstance()->GetSubsystem<UTimeOfDaySubsystem>();
	// if (TimeSystem)
	// {
	//     CurrentHour = TimeSystem->GetCurrentHour();
	//  CurrentDay = TimeSystem->GetCurrentDay();
	// }

	// IMPORTANT: Replace this with your actual time system!
	// This is just a placeholder showing the pattern
}

void UTimeSystemGameEventEmitter::OnHourChanged_Implementation(int32 NewHour)
{
	if (!bEmitEvents || !bEmitHourlyEvents) return;

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.HourChanged"), false);
	if (EventTag.IsValid())
	{
		EmitTimeEvent(EventTag, NewHour, LastDay);
		
		if (bLogEvents)
		{
			UE_LOG(LogTimeEvents, Log, TEXT("Hour Changed: %d"), NewHour);
		}
	}
}

void UTimeSystemGameEventEmitter::OnDayChanged_Implementation(int32 NewDay)
{
	if (!bEmitEvents || !bEmitDailyEvents) return;

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.DayChanged"), false);
	if (EventTag.IsValid())
	{
		EmitTimeEvent(EventTag, LastHour, NewDay);
		
		if (bLogEvents)
		{
			UE_LOG(LogTimeEvents, Log, TEXT("Day Changed: %d"), NewDay);
		}
	}
}

void UTimeSystemGameEventEmitter::OnWeatherChanged_Implementation(const FGameplayTagContainer& WeatherTags)
{
	if (!bEmitEvents || !bEmitWeatherEvents) return;

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Weather.Event.Changed"), false);
	if (EventTag.IsValid())
	{
		UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
		if (EventBus)
		{
			FGameEventPayload Payload(EventTag);
			Payload.AdditionalTags = WeatherTags;
			Payload.IntParam = LastHour;
			Payload.FloatParam = static_cast<float>(LastDay);

			EventBus->EmitEvent(Payload, bLogEvents);
		}
	}
}

void UTimeSystemGameEventEmitter::EmitTimeEvent(const FGameplayTag& EventTag, int32 Hour, int32 Day) const
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
	if (!EventBus) return;

	FGameEventPayload Payload = UGameEventBusLibrary::MakeTimePayload(EventTag, Hour, Day);
	EventBus->EmitEvent(Payload, bLogEvents);
}
