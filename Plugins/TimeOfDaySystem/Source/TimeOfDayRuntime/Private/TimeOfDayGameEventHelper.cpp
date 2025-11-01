// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeOfDayGameEventHelper.h"

// Conditional GameEventBus integration
#if __has_include("GameEventBusSubsystem.h")
	#include "GameEventBusSubsystem.h"
	#include "GameEventBusLibrary.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

void UTimeOfDayGameEventHelper::EmitHourChangedEvent(UObject* WorldContext, int32 NewHour)
{
	if (!WorldContext)
	{
		return;
	}

#if GAMEEVENTBUS_AVAILABLE
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContext);
	if (!EventBus)
	{
		return;
	}

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.HourChanged"));
	if (EventTag.IsValid())
	{
		FGameEventPayload Payload(EventTag);
		Payload.IntParam = NewHour;
		EventBus->EmitEvent(Payload, true); // true = log event
	}
#endif
}

void UTimeOfDayGameEventHelper::EmitDayChangedEvent(UObject* WorldContext, int32 NewDay)
{
	if (!WorldContext)
	{
		return;
	}

#if GAMEEVENTBUS_AVAILABLE
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContext);
	if (!EventBus)
	{
		return;
	}

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.DayChanged"));
	if (EventTag.IsValid())
	{
		FGameEventPayload Payload(EventTag);
		Payload.IntParam = NewDay;
		EventBus->EmitEvent(Payload, true);
	}
#endif
}

void UTimeOfDayGameEventHelper::EmitSeasonChangedEvent(UObject* WorldContext, int32 NewSeason)
{
	if (!WorldContext)
	{
		return;
	}

#if GAMEEVENTBUS_AVAILABLE
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContext);
	if (!EventBus)
	{
		return;
	}

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.SeasonChanged"));
	if (EventTag.IsValid())
	{
		// Map season enum to string
		const TCHAR* SeasonNames[] = { TEXT("Spring"), TEXT("Summer"), TEXT("Autumn"), TEXT("Winter") };
		FString SeasonName = (NewSeason >= 0 && NewSeason < 4) ? SeasonNames[NewSeason] : TEXT("Unknown");

		FGameEventPayload Payload(EventTag);
		Payload.StringParam = FName(*SeasonName);
		Payload.IntParam = NewSeason;
		EventBus->EmitEvent(Payload, true);
	}
#endif
}

void UTimeOfDayGameEventHelper::EmitSunriseEvent(UObject* WorldContext)
{
	if (!WorldContext)
	{
		return;
	}

#if GAMEEVENTBUS_AVAILABLE
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContext);
	if (!EventBus)
	{
		return;
	}

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.Sunrise"));
	if (EventTag.IsValid())
	{
		FGameEventPayload Payload(EventTag);
		EventBus->EmitEvent(Payload, true);
	}
#endif
}

void UTimeOfDayGameEventHelper::EmitSunsetEvent(UObject* WorldContext)
{
	if (!WorldContext)
	{
		return;
	}

#if GAMEEVENTBUS_AVAILABLE
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContext);
	if (!EventBus)
	{
		return;
	}

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.Sunset"));
	if (EventTag.IsValid())
	{
		FGameEventPayload Payload(EventTag);
		EventBus->EmitEvent(Payload, true);
	}
#endif
}
