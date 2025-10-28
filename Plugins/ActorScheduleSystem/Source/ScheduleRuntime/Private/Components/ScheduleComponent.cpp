// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/ScheduleComponent.h"
#include "Subsystems/ScheduleSubsystem.h"
#include "Data/ScheduleData.h"
#include "Data/ScheduleEntryData.h"
#include "Actions/ScheduleActionBase.h"
#include "Interfaces/ScheduleActionInterface.h"
#include "Interfaces/ScheduleLocationSelectorInterface.h"
#include "Selectors/ScheduleLocationSelectorBase.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UScheduleComponent::UScheduleComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Event-driven, no tick needed
	bWantsInitializeComponent = true;
}

void UScheduleComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		ScheduleSubsystem = World->GetSubsystem<UScheduleSubsystem>();
	}
}

void UScheduleComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Cancel any active actions
	if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
	{
		// Use Execute_ wrapper for Blueprint interface compatibility
		IScheduleActionInterface::Execute_CancelAction(CurrentEntry->Action, CurrentActionHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UScheduleComponent::OnRegister()
{
	Super::OnRegister();

	// Register with subsystem when component is registered
	if (UWorld* World = GetWorld())
	{
		if (UScheduleSubsystem* Subsystem = World->GetSubsystem<UScheduleSubsystem>())
		{
			Subsystem->RegisterScheduleComponent(this);
		}
	}
}

void UScheduleComponent::OnUnregister()
{
	// Unregister from subsystem
	if (ScheduleSubsystem)
	{
		ScheduleSubsystem->UnregisterScheduleComponent(this);
	}

	Super::OnUnregister();
}

void UScheduleComponent::EvaluateSchedule()
{
	if (!bScheduleEnabled || bIsPaused || !ScheduleData)
	{
		return;
	}

	// Build evaluation context
	FScheduleEvaluationContext Context = BuildEvaluationContext();

	// Find matching entry
	UScheduleEntryData* MatchingEntry = FindMatchingEntry(Context);

	UE_LOG(LogTemp, Log, TEXT("📋 EvaluateSchedule for %s:"), *GetOwner()->GetName());
	UE_LOG(LogTemp, Log, TEXT("   MatchingEntry: %s"), MatchingEntry ? *MatchingEntry->GetDisplayText().ToString() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("   CurrentEntry: %s"), CurrentEntry ? *CurrentEntry->GetDisplayText().ToString() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("   CurrentActionHandle.IsValid(): %s"), CurrentActionHandle.IsValid() ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Log, TEXT("   Same entry check: %s"), (MatchingEntry == CurrentEntry && CurrentActionHandle.IsValid()) ? TEXT("TRUE - CONTINUING") : TEXT("FALSE - WILL PROCEED"));

	// If same entry is already running, continue
	if (MatchingEntry == CurrentEntry && CurrentActionHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("   ✅ Same entry already running, skipping execution"));
		return;
	}

	// Cancel current action if switching to new entry
	if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
	{
		if (CurrentEntry->bCanBeInterrupted)
		{
			UE_LOG(LogTemp, Warning, TEXT("   ⚠️ Canceling current action (can be interrupted)"));
			// Use Execute_ wrapper for Blueprint interface compatibility
			IScheduleActionInterface::Execute_CancelAction(CurrentEntry->Action, CurrentActionHandle);
			CurrentActionHandle = FScheduleActionHandle();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("   ⚠️ Cannot interrupt current action"));
			return; // Can't interrupt current action
		}
	}

	// Execute new entry if found
	if (MatchingEntry)
	{
		ExecuteEntry(MatchingEntry);
	}
	else if (ScheduleData->FallbackAction && !CurrentActionHandle.IsValid())
	{
		// Execute fallback action if no entry matches
		UE_LOG(LogTemp, Log, TEXT("   📌 Executing fallback action"));
		FScheduleExecutionContext ExecContext = BuildExecutionContext(FScheduleLocation());
		// Use Execute_ wrapper for Blueprint interface compatibility
		CurrentActionHandle = IScheduleActionInterface::Execute_ExecuteAction(ScheduleData->FallbackAction, ExecContext);
		CurrentEntry = nullptr;
	}
}

UScheduleEntryData* UScheduleComponent::FindMatchingEntry(const FScheduleEvaluationContext& Context) const
{
	if (!ScheduleData)
	{
		UE_LOG(LogTemp, VeryVerbose, TEXT("   No ScheduleData assigned to %s"), *GetOwner()->GetName());
		return nullptr;
	}

	// Get entries sorted by priority
	TArray<UScheduleEntryData*> SortedEntries = ScheduleData->GetSortedEntries();

	UE_LOG(LogTemp, VeryVerbose, TEXT("   Checking %d entries for %s"), SortedEntries.Num(), *GetOwner()->GetName());

	// Find first matching entry (highest priority)
	for (UScheduleEntryData* Entry : SortedEntries)
	{
		if (Entry)
		{
			bool bMatches = Entry->EvaluateConditions(Context);
			
			UE_LOG(LogTemp, VeryVerbose, TEXT("     Entry '%s' (Priority %d): %s"),
				*Entry->GetDisplayText().ToString(),
				Entry->Priority,
				bMatches ? TEXT("MATCH") : TEXT("no match"));
			
			if (bMatches)
			{
				return Entry;
			}
		}
	}

	UE_LOG(LogTemp, Verbose, TEXT(" ⚠️  No matching entry found for %s at %02d:%02d"), 
		*GetOwner()->GetName(), Context.CurrentHour, Context.CurrentMinute);

	return nullptr;
}

void UScheduleComponent::ExecuteEntry(UScheduleEntryData* Entry)
{
	if (!Entry)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ ExecuteEntry called with null Entry for %s"), *GetOwner()->GetName());
		return;
	}
	
	if (!Entry->Action)
	{
		UE_LOG(LogTemp, Warning, TEXT("⚠️ Entry '%s' has no Action assigned!"), *Entry->GetDisplayText().ToString());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("🎯 Starting action '%s' for %s (Priority: %d)"),
		*Entry->GetDisplayText().ToString(),
		*GetOwner()->GetName(),
		Entry->Priority);

	CurrentEntry = Entry;

	// Select location if selector is provided
	FScheduleLocation SelectedLocation;
	if (Entry->LocationSelector)
	{
		UE_LOG(LogTemp, Log, TEXT("   📍 LocationSelector assigned: %s (Class: %s)"), 
			*Entry->LocationSelector->GetName(),
			*Entry->LocationSelector->GetClass()->GetName());

		// Check if the selector implements the interface
		if (!Entry->LocationSelector->GetClass()->ImplementsInterface(UScheduleLocationSelectorInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("   ❌ LocationSelector does NOT implement IScheduleLocationSelectorInterface!"));
			return;
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("   ✅ LocationSelector implements IScheduleLocationSelectorInterface"));
		}

		FScheduleLocationContext LocationContext;
		LocationContext.World = GetWorld();
		LocationContext.RequestingActor = GetOwner();
		
		UE_LOG(LogTemp, Log, TEXT("   📍 Calling SelectLocation..."));
		UE_LOG(LogTemp, Log, TEXT("      World: %s"), LocationContext.World ? *LocationContext.World->GetName() : TEXT("NULL"));
		if (LocationContext.RequestingActor.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("      RequestingActor: %s"), *LocationContext.RequestingActor->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("      RequestingActor: NULL"));
		}
		
		// Use Execute_ wrapper for Blueprint interface compatibility
		bool bLocationSelected = IScheduleLocationSelectorInterface::Execute_SelectLocation(Entry->LocationSelector, LocationContext, SelectedLocation);
		
		UE_LOG(LogTemp, Log, TEXT("   📍 SelectLocation returned: %s"), bLocationSelected ? TEXT("TRUE") : TEXT("FALSE"));
		
		if (!bLocationSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("⚠️ Failed to select location for schedule entry: %s"), 
				*Entry->GetDisplayText().ToString());
			UE_LOG(LogTemp, Warning, TEXT("   Selector: %s"), *Entry->LocationSelector->GetName());
			UE_LOG(LogTemp, Warning, TEXT("   Selector Class: %s"), *Entry->LocationSelector->GetClass()->GetName());
			return;
		}
		
		UE_LOG(LogTemp, Log, TEXT("   📍 Selected location: %s"), *SelectedLocation.Location.ToString());
		if (SelectedLocation.ReferenceActor.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("      ReferenceActor: %s"), *SelectedLocation.ReferenceActor->GetName());
		}
		if (SelectedLocation.LocationTags.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("      LocationTags: %s"), *SelectedLocation.LocationTags.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("   📍 No LocationSelector assigned - using default location"));
	}

	// Build execution context
	FScheduleExecutionContext ExecContext = BuildExecutionContext(SelectedLocation);

	// Execute action
	if (Entry->Action->GetClass()->ImplementsInterface(UScheduleActionInterface::StaticClass()))
	{
		CurrentActionHandle = IScheduleActionInterface::Execute_ExecuteAction(Entry->Action, ExecContext);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("   ❌ Action does not implement IScheduleActionInterface!"));
		return;
	}

	if (CurrentActionHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("   ✅ Action started successfully"));
		OnActionStarted.Broadcast(CurrentActionHandle);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("   ⚠️ Action returned invalid handle!"));
	}
}

FScheduleEvaluationContext UScheduleComponent::BuildEvaluationContext() const
{
	FScheduleEvaluationContext Context;
	Context.World = GetWorld();
	Context.ScheduledActor = GetOwner();
	
	// Get time from subsystem
	if (ScheduleSubsystem)
	{
		int32 Hour, Minute, DayOfWeek;
		ScheduleSubsystem->GetCurrentTime(Hour, Minute, DayOfWeek);
		Context.CurrentHour = Hour;
		Context.CurrentMinute = Minute;
		Context.CurrentDayOfWeek = DayOfWeek;
		
		UE_LOG(LogTemp, VeryVerbose, TEXT("   Evaluating schedule for %s at %02d:%02d (Day %d)"),
			*GetOwner()->GetName(), Hour, Minute, DayOfWeek);
	}

	Context.ActiveTags = ActorScheduleTags;

	return Context;
}

FScheduleExecutionContext UScheduleComponent::BuildExecutionContext(const FScheduleLocation& Location) const
{
	FScheduleExecutionContext Context;
	Context.World = GetWorld();
	Context.ExecutingActor = GetOwner();
	Context.TargetLocation = Location;

	return Context;
}

void UScheduleComponent::InterruptCurrentAction(bool bCanResume)
{
	if (!CurrentActionHandle.IsValid() || !CurrentEntry || !CurrentEntry->Action)
	{
		return;
	}

	if (bCanResume && CurrentEntry->bResumeAfterInterruption)
	{
		// Store for resume
		InterruptedEntry = CurrentEntry;
		InterruptedActionHandle = CurrentActionHandle;
		// Use Execute_ wrapper for Blueprint interface compatibility
		IScheduleActionInterface::Execute_PauseAction(CurrentEntry->Action, CurrentActionHandle);
	}
	else
	{
		// Use Execute_ wrapper for Blueprint interface compatibility
		IScheduleActionInterface::Execute_CancelAction(CurrentEntry->Action, CurrentActionHandle);
	}

	OnActionInterrupted.Broadcast(CurrentActionHandle);
	CurrentActionHandle = FScheduleActionHandle();
	CurrentEntry = nullptr;
}

void UScheduleComponent::ResumeInterruptedAction()
{
	if (!InterruptedActionHandle.IsValid() || !InterruptedEntry || !InterruptedEntry->Action)
	{
		return;
	}

	// Use Execute_ wrapper for Blueprint interface compatibility
	IScheduleActionInterface::Execute_ResumeAction(InterruptedEntry->Action, InterruptedActionHandle);
	CurrentEntry = InterruptedEntry;
	CurrentActionHandle = InterruptedActionHandle;

	InterruptedEntry = nullptr;
	InterruptedActionHandle = FScheduleActionHandle();

	OnActionStarted.Broadcast(CurrentActionHandle);
}

void UScheduleComponent::CancelCurrentAction()
{
	if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
	{
		// Use Execute_ wrapper for Blueprint interface compatibility
		IScheduleActionInterface::Execute_CancelAction(CurrentEntry->Action, CurrentActionHandle);
		OnActionInterrupted.Broadcast(CurrentActionHandle);
	}

	CurrentActionHandle = FScheduleActionHandle();
	CurrentEntry = nullptr;
}

void UScheduleComponent::PauseSchedule()
{
	bIsPaused = true;

	if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
	{
		// Use Execute_ wrapper for Blueprint interface compatibility
		IScheduleActionInterface::Execute_PauseAction(CurrentEntry->Action, CurrentActionHandle);
	}
}

void UScheduleComponent::ResumeSchedule()
{
	bIsPaused = false;

	if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
	{
		// Use Execute_ wrapper for Blueprint interface compatibility
		IScheduleActionInterface::Execute_ResumeAction(CurrentEntry->Action, CurrentActionHandle);
	}

	// Re-evaluate to catch up with time
	EvaluateSchedule();
}

void UScheduleComponent::SetScheduleData(UScheduleData* NewScheduleData, bool bCancelCurrent)
{
	if (bCancelCurrent)
	{
		CancelCurrentAction();
	}

	ScheduleData = NewScheduleData;

	// Trigger evaluation with new data
	EvaluateSchedule();
}
