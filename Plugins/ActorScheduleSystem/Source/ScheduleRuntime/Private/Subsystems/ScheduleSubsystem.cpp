// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystems/ScheduleSubsystem.h"
#include "Components/ScheduleComponent.h"
#include "Data/ScheduleData.h"
#include "Engine/World.h"

UScheduleSubsystem::UScheduleSubsystem()
{
}

void UScheduleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("? Schedule Subsystem Initialized"));
	UE_LOG(LogTemp, Log, TEXT("   - Initial time: %02d:%02d (Day %d)"), CurrentHour, CurrentMinute, CurrentDayOfWeek);
	UE_LOG(LogTemp, Log, TEXT("   - Ready to register schedule components"));
}

void UScheduleSubsystem::Deinitialize()
{
	RegisteredComponents.Empty();
	Super::Deinitialize();
}

TStatId UScheduleSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UScheduleSubsystem, STATGROUP_Tickables);
}

void UScheduleSubsystem::Tick(float DeltaTime)
{
	if (bIsPaused)
	{
		return;
	}

	AccumulatedDeltaTime += DeltaTime;

	// Minimal maintenance tick - actual evaluation happens on time change events
	// Clean up invalid components
	RegisteredComponents.RemoveAll([](const UScheduleComponent* Component)
	{
		return !IsValid(Component);
	});
	
	// ? Call Super::Tick ONCE at the end
	Super::Tick(DeltaTime);
}

void UScheduleSubsystem::RegisterScheduleComponent(UScheduleComponent* Component)
{
	if (!Component || RegisteredComponents.Contains(Component))
	{
		return;
	}

	RegisteredComponents.Add(Component);

	UE_LOG(LogTemp, Log, TEXT("?? Registered schedule component for actor: %s (Total registered: %d)"), 
		Component->GetOwner() ? *Component->GetOwner()->GetName() : TEXT("None"),
		RegisteredComponents.Num());

	// Trigger initial evaluation for this component
	Component->EvaluateSchedule();
}

void UScheduleSubsystem::UnregisterScheduleComponent(UScheduleComponent* Component)
{
	if (Component)
	{
		RegisteredComponents.Remove(Component);
	}
}

void UScheduleSubsystem::NotifyTimeChanged(int32 NewHour, int32 NewMinute, int32 NewDayOfWeek)
{
	// Only evaluate if time actually changed
	if (NewHour == LastEvaluationHour && NewMinute == LastEvaluationMinute)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("? Schedule System: Time changed to %02d:%02d (Day %d)"), NewHour, NewMinute, NewDayOfWeek);
	UE_LOG(LogTemp, Log, TEXT("   - Evaluating %d registered components"), RegisteredComponents.Num());

	CurrentHour = NewHour;
	CurrentMinute = NewMinute;
	CurrentDayOfWeek = NewDayOfWeek;

	LastEvaluationHour = NewHour;
	LastEvaluationMinute = NewMinute;

	// Trigger batch evaluation
	EvaluateSchedules();
}

void UScheduleSubsystem::RefreshAllSchedules()
{
	EvaluateSchedules();
}

void UScheduleSubsystem::EvaluateSchedules()
{
	if (bIsPaused)
	{
		UE_LOG(LogTemp, Verbose, TEXT("   ??  Schedule evaluation paused"));
		return;
	}

	int32 EvaluatedCount = 0;
	
	// Batch evaluate all registered components
	for (UScheduleComponent* Component : RegisteredComponents)
	{
		if (IsValid(Component))
		{
			Component->EvaluateSchedule();
			EvaluatedCount++;
		}
	}
	
	if (EvaluatedCount > 0)
	{
		UE_LOG(LogTemp, Verbose, TEXT("   ? Evaluated %d schedule components"), EvaluatedCount);
	}
}

TArray<AActor*> UScheduleSubsystem::GetActorsWithScheduleTags(const FGameplayTagContainer& Tags, bool bExactMatch) const
{
	TArray<AActor*> MatchingActors;

	for (const UScheduleComponent* Component : RegisteredComponents)
	{
		if (!IsValid(Component) || !Component->GetScheduleData())
		{
			continue;
		}

		const FGameplayTagContainer& ScheduleTags = Component->GetScheduleData()->ScheduleTags;

		if (bExactMatch)
		{
			if (ScheduleTags.HasAllExact(Tags))
			{
				MatchingActors.Add(Component->GetOwner());
			}
		}
		else
		{
			if (ScheduleTags.HasAny(Tags))
			{
				MatchingActors.Add(Component->GetOwner());
			}
		}
	}

	return MatchingActors;
}

void UScheduleSubsystem::PauseAllSchedules()
{
	bIsPaused = true;

	for (UScheduleComponent* Component : RegisteredComponents)
	{
		if (IsValid(Component))
		{
			Component->PauseSchedule();
		}
	}
}

void UScheduleSubsystem::ResumeAllSchedules()
{
	bIsPaused = false;

	for (UScheduleComponent* Component : RegisteredComponents)
	{
		if (IsValid(Component))
		{
			Component->ResumeSchedule();
		}
	}
}

void UScheduleSubsystem::GetScheduleStats(int32& OutTotalActors, int32& OutActiveActions, int32& OutPausedActions) const
{
	OutTotalActors = RegisteredComponents.Num();
	OutActiveActions = 0;
	OutPausedActions = 0;

	for (const UScheduleComponent* Component : RegisteredComponents)
	{
		if (IsValid(Component))
		{
			if (Component->IsSchedulePaused())
			{
				OutPausedActions++;
			}
			else if (Component->HasActiveAction())
			{
				OutActiveActions++;
			}
		}
	}
}

void UScheduleSubsystem::BroadcastScheduleEvent(const FGameplayTag& EventType, AActor* Actor, const FScheduleActionHandle& Handle)
{
	FScheduleEventData EventData;
	EventData.EventType = EventType;
	EventData.RelatedActor = Actor;
	EventData.ActionHandle = Handle;

	OnScheduleEvent.Broadcast(EventData);
}
