// Copyright Epic Games, Inc. All Rights Reserved.

#include "Integration/TimeSystemGameEventEmitter.h"
#include "GameEventBusSubsystem.h"
#include "GameEventBusLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

// Conditional include - only if TimeOfDaySystem is available
#if __has_include("Subsystems/TimeOfDaySubsystem.h")
	#include "Subsystems/TimeOfDaySubsystem.h"
	#include "Core/TimeOfDayTypes.h"
	#define TIMEOFDAYSYSTEM_AVAILABLE 1
#else
	#define TIMEOFDAYSYSTEM_AVAILABLE 0
#endif

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
#if TIMEOFDAYSYSTEM_AVAILABLE
		// Подписываемся на события TimeOfDaySubsystem
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				UTimeOfDaySubsystem* TimeSystem = GameInstance->GetSubsystem<UTimeOfDaySubsystem>();
				if (TimeSystem)
				{
					// Подписываемся на события времени
					TimeSystem->OnHourChanged.AddDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDayHourChanged);
					TimeSystem->OnDayChanged.AddDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDayDayChanged);
					TimeSystem->OnMinuteChanged.AddDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDayMinuteChanged);
					TimeSystem->OnDayPhaseChanged.AddDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDayPhaseChanged);
					TimeSystem->OnSeasonChanged.AddDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDaySeasonChanged);

					UE_LOG(LogTimeEvents, Log, TEXT("TimeSystemGameEventEmitter: Connected to TimeOfDaySubsystem"));
				}
				else
				{
					UE_LOG(LogTimeEvents, Warning, TEXT("TimeSystemGameEventEmitter: TimeOfDaySubsystem not found!"));
				}
			}
		}
#else
		// Fallback: используем таймер для проверки изменений
		GetWorld()->GetTimerManager().SetTimer(
			CheckTimerHandle,
			this,
			&UTimeSystemGameEventEmitter::CheckForTimeChanges,
			1.0f,
			true
		);

		UE_LOG(LogTimeEvents, Log, TEXT("TimeSystemGameEventEmitter: Started (fallback timer mode)"));
#endif
	}
}

void UTimeSystemGameEventEmitter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if TIMEOFDAYSYSTEM_AVAILABLE
	// Отписываемся от событий TimeOfDaySubsystem
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			UTimeOfDaySubsystem* TimeSystem = GameInstance->GetSubsystem<UTimeOfDaySubsystem>();
			if (TimeSystem)
			{
				TimeSystem->OnHourChanged.RemoveDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDayHourChanged);
				TimeSystem->OnDayChanged.RemoveDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDayDayChanged);
				TimeSystem->OnMinuteChanged.RemoveDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDayMinuteChanged);
				TimeSystem->OnDayPhaseChanged.RemoveDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDayPhaseChanged);
				TimeSystem->OnSeasonChanged.RemoveDynamic(this, &UTimeSystemGameEventEmitter::OnTimeOfDaySeasonChanged);

				UE_LOG(LogTimeEvents, Verbose, TEXT("TimeSystemGameEventEmitter: Disconnected from TimeOfDaySubsystem"));
			}
		}
	}
#endif

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

#if TIMEOFDAYSYSTEM_AVAILABLE
void UTimeSystemGameEventEmitter::OnTimeOfDayHourChanged(const FTimeOfDayTime& CurrentTime)
{
	if (!bEmitEvents || !bEmitHourlyEvents)
	{
		return;
	}

	OnHourChanged_Implementation(CurrentTime.Hour);
	
	LastHour = CurrentTime.Hour;
	LastDay = CurrentTime.Day;
}

void UTimeSystemGameEventEmitter::OnTimeOfDayDayChanged(const FTimeOfDayTime& CurrentTime)
{
	if (!bEmitEvents || !bEmitDailyEvents)
	{
		return;
	}

	OnDayChanged_Implementation(CurrentTime.Day);
	
	LastHour = CurrentTime.Hour;
	LastDay = CurrentTime.Day;
}

void UTimeSystemGameEventEmitter::OnTimeOfDayMinuteChanged(const FTimeOfDayTime& CurrentTime)
{
	// Можно добавить эмиссию событий каждую минуту если нужно
	// Пока просто обновляем кэш
}

void UTimeSystemGameEventEmitter::OnTimeOfDayPhaseChanged(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime)
{
	if (!bEmitEvents)
	{
		return;
	}

	// Эмитим событие смены фазы дня
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.PhaseChanged"), false);
	if (EventTag.IsValid())
	{
		UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
		if (EventBus)
		{
			FGameEventPayload Payload(EventTag);
			Payload.IntParam = static_cast<int32>(NewPhase);
			Payload.FloatParam = CurrentTime.GetNormalizedTimeOfDay();
			Payload.StringParam = FName(*UEnum::GetValueAsString(NewPhase));

			EventBus->EmitEvent(Payload, bLogEvents);

			if (bLogEvents)
			{
				UE_LOG(LogTimeEvents, Log, TEXT("Phase Changed: %s at %02d:%02d"),
					*UEnum::GetValueAsString(NewPhase), CurrentTime.Hour, CurrentTime.Minute);
			}
		}
	}
}

void UTimeSystemGameEventEmitter::OnTimeOfDaySeasonChanged(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime)
{
	if (!bEmitEvents)
	{
		return;
	}

	// Эмитим событие смены сезона
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.SeasonChanged"), false);
	if (EventTag.IsValid())
	{
		UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
		if (EventBus)
		{
			FGameEventPayload Payload(EventTag);
			Payload.IntParam = static_cast<int32>(NewSeason);
			Payload.StringParam = FName(*UEnum::GetValueAsString(NewSeason));
			Payload.FloatParam = static_cast<float>(CurrentTime.Day);

			EventBus->EmitEvent(Payload, bLogEvents);

			if (bLogEvents)
			{
				UE_LOG(LogTimeEvents, Log, TEXT("Season Changed: %s on Day %d"),
					*UEnum::GetValueAsString(NewSeason), CurrentTime.Day);
			}
		}
	}
}
#else
// Заглушки для случая когда TimeOfDaySystem недоступен
void UTimeSystemGameEventEmitter::OnTimeOfDayHourChanged(const FTimeOfDayTime& CurrentTime)
{
	// Пустая реализация - TimeOfDaySystem недоступен
}

void UTimeSystemGameEventEmitter::OnTimeOfDayDayChanged(const FTimeOfDayTime& CurrentTime)
{
	// Пустая реализация - TimeOfDaySystem недоступен
}

void UTimeSystemGameEventEmitter::OnTimeOfDayMinuteChanged(const FTimeOfDayTime& CurrentTime)
{
	// Пустая реализация - TimeOfDaySystem недоступен
}

void UTimeSystemGameEventEmitter::OnTimeOfDayPhaseChanged(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime)
{
	// Пустая реализация - TimeOfDaySystem недоступен
}

void UTimeSystemGameEventEmitter::OnTimeOfDaySeasonChanged(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime)
{
	// Пустая реализация - TimeOfDaySystem недоступен
}
#endif

void UTimeSystemGameEventEmitter::CheckForTimeChanges()
{
	// Fallback implementation when TimeOfDaySystem is not available
	// This is just a placeholder - you would integrate with your custom time system here
	
#if !TIMEOFDAYSYSTEM_AVAILABLE
	UE_LOG(LogTimeEvents, VeryVerbose, TEXT("TimeSystemGameEventEmitter: Checking for time changes (fallback mode)"));
	// TODO: Add custom time system integration here
#endif
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
