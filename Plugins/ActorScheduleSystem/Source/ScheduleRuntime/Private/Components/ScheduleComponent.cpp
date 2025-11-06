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
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// GameEventBus integration (soft dependency)
#if WITH_GAMEEVENTBUS
#include "GameEventBusSubsystem.h" // includes UDialogueAdditionalActors
#endif

// DialogueSystem integration (soft dependency via reflection - NO HARD LINK!)
// We use FindComponentByClass + Execute_GetParticipantId to avoid circular dependency
class UDialogueComponent; // Forward declaration only!
class IDialogueParticipant; // Forward declaration only!

DEFINE_LOG_CATEGORY_STATIC(LogScheduleComponent, Log, All);

UScheduleComponent::UScheduleComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Event-driven, no tick needed
	bWantsInitializeComponent = true;
	
	// ✅ NEW v1.14: Default behavior - only pause if involved
	bPauseDuringAnyDialogue = false;
}

void UScheduleComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		ScheduleSubsystem = World->GetSubsystem<UScheduleSubsystem>();
	}

	// ✅ Subscribe to Dialogue events from GameEventBus (if available)
	SubscribeToDialogueEvents();
}

void UScheduleComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ✅ Unsubscribe from Dialogue events
	UnsubscribeFromDialogueEvents();

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

	UE_LOG(LogTemp, Log, TEXT("[SCHEDULE] EvaluateSchedule for %s:"), *GetOwner()->GetName());
	UE_LOG(LogTemp, Log, TEXT("   MatchingEntry: %s"), MatchingEntry ? *MatchingEntry->GetDisplayText().ToString() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("   CurrentEntry: %s"), CurrentEntry ? *CurrentEntry->GetDisplayText().ToString() : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("   CurrentActionHandle.IsValid(): %s"), CurrentActionHandle.IsValid() ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Log, TEXT("   Same entry check: %s"), (MatchingEntry == CurrentEntry && CurrentActionHandle.IsValid()) ? TEXT("TRUE - CONTINUING") : TEXT("FALSE - WILL PROCEED"));

	// If same entry is already running, continue
	if (MatchingEntry == CurrentEntry && CurrentActionHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("   [OK] Same entry already running, skipping execution"));
		return;
	}

	// Cancel current action if switching to new entry
	if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
	{
		if (CurrentEntry->bCanBeInterrupted)
		{
			UE_LOG(LogTemp, Warning, TEXT("   [WARN] Canceling current action (can be interrupted)"));
			// Use Execute_ wrapper for Blueprint interface compatibility
			IScheduleActionInterface::Execute_CancelAction(CurrentEntry->Action, CurrentActionHandle);
			CurrentActionHandle = FScheduleActionHandle();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("   [WARN] Cannot interrupt current action"));
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
		UE_LOG(LogTemp, Log, TEXT("   [FALLBACK] Executing fallback action"));
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

	UE_LOG(LogTemp, Verbose, TEXT("   [WARN] No matching entry found for %s at %02d:%02d"), 
		*GetOwner()->GetName(), Context.CurrentHour, Context.CurrentMinute);

	return nullptr;
}

void UScheduleComponent::ExecuteEntry(UScheduleEntryData* Entry)
{
	if (!Entry)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WARN] ExecuteEntry called with null Entry for %s"), *GetOwner()->GetName());
		return;
	}
	
	if (!Entry->Action)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WARN] Entry '%s' has no Action assigned!"), *Entry->GetDisplayText().ToString());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[ACTION] Starting action '%s' for %s (Priority: %d)"),
		*Entry->GetDisplayText().ToString(),
		*GetOwner()->GetName(),
		Entry->Priority);

	CurrentEntry = Entry;

	// Select location if selector is provided
	FScheduleLocation SelectedLocation;
	if (Entry->LocationSelector)
	{
		UE_LOG(LogTemp, Log, TEXT("   [LOCATION] LocationSelector assigned: %s (Class: %s)"), 
			*Entry->LocationSelector->GetName(),
			*Entry->LocationSelector->GetClass()->GetName());

		// Check if the selector implements the interface
		if (!Entry->LocationSelector->GetClass()->ImplementsInterface(UScheduleLocationSelectorInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("   [ERROR] LocationSelector does NOT implement IScheduleLocationSelectorInterface!"));
			return;
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("   [OK] LocationSelector implements IScheduleLocationSelectorInterface"));
		}

		FScheduleLocationContext LocationContext;
		LocationContext.World = GetWorld();
		LocationContext.RequestingActor = GetOwner();
		
		UE_LOG(LogTemp, Log, TEXT("   [LOCATION] Calling SelectLocation..."));
		UE_LOG(LogTemp, Log, TEXT("    World: %s"), LocationContext.World ? *LocationContext.World->GetName() : TEXT("NULL"));
		if (LocationContext.RequestingActor.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("    RequestingActor: %s"), *LocationContext.RequestingActor->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("      RequestingActor: NULL"));
		}
		
		// Use Execute_ wrapper for Blueprint interface compatibility
		bool bLocationSelected = IScheduleLocationSelectorInterface::Execute_SelectLocation(Entry->LocationSelector, LocationContext, SelectedLocation);
		
		UE_LOG(LogTemp, Log, TEXT("   [LOCATION] SelectLocation returned: %s"), bLocationSelected ? TEXT("TRUE") : TEXT("FALSE"));
		
		if (!bLocationSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("[WARN] Failed to select location for schedule entry: %s"), 
				*Entry->GetDisplayText().ToString());
			UE_LOG(LogTemp, Warning, TEXT("   Selector: %s"), *Entry->LocationSelector->GetName());
			UE_LOG(LogTemp, Warning, TEXT("   Selector Class: %s"), *Entry->LocationSelector->GetClass()->GetName());
			return;
		}
		
		UE_LOG(LogTemp, Log, TEXT("   [LOCATION] Selected location: %s"), *SelectedLocation.Location.ToString());
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
		UE_LOG(LogTemp, Log, TEXT("   [LOCATION] No LocationSelector assigned - using default location"));
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
		UE_LOG(LogTemp, Error, TEXT(" [ERROR] Action does not implement IScheduleActionInterface!"));
		return;
	}

	if (CurrentActionHandle.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("   [OK] Action started successfully"));
		OnActionStarted.Broadcast(CurrentActionHandle);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("   [WARN] Action returned invalid handle!"));
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

	UE_LOG(LogScheduleComponent, Warning, TEXT("[PAUSE] Pausing schedule for actor: %s"), 
		GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));

	if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
	{
		// Pause current action
		IScheduleActionInterface::Execute_PauseAction(CurrentEntry->Action, CurrentActionHandle);
		
		UE_LOG(LogScheduleComponent, Warning, TEXT("[PAUSE]   Current action paused: %s"), 
		CurrentEntry ? *CurrentEntry->GetDisplayText().ToString() : TEXT("NULL"));
	}

	// ✅ AGGRESSIVE STOP: Force stop movement immediately
	AActor* Owner = GetOwner();
	if (Owner)
	{
		// Stop AI movement
		if (APawn* Pawn = Cast<APawn>(Owner))
		{
			if (AAIController* AIController = Cast<AAIController>(Pawn->GetController()))
			{
				AIController->StopMovement();
				UE_LOG(LogScheduleComponent, Warning, TEXT("[PAUSE]   Stopped AI movement"));
			}
		}

		// Stop character movement component
		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
			{
				MovementComp->StopMovementImmediately();
				UE_LOG(LogScheduleComponent, Warning, TEXT("[PAUSE]   Stopped CharacterMovementComponent"));
			}
		}
	}
}

void UScheduleComponent::ResumeSchedule()
{
    UE_LOG(LogScheduleComponent, Warning, TEXT("[RESUME] Resuming schedule for actor: %s (was paused: %s)"), 
		GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"),
        bIsPaused ? TEXT("YES") : TEXT("NO"));

    bIsPaused = false;

    if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
    {
      // Use Execute_ wrapper for Blueprint interface compatibility
        IScheduleActionInterface::Execute_ResumeAction(CurrentEntry->Action, CurrentActionHandle);
 
 UE_LOG(LogScheduleComponent, Warning, TEXT("[RESUME]   Current action resumed: %s"), 
        CurrentEntry ? *CurrentEntry->GetDisplayText().ToString() : TEXT("NULL"));
    
        // ✅ FIX: DON'T re-evaluate immediately if we have an active action!
        // Let the action complete naturally, THEN schedule will re-evaluate
        UE_LOG(LogScheduleComponent, Warning, TEXT("[RESUME]   Action still active - skipping re-evaluation"));
UE_LOG(LogScheduleComponent, Warning, TEXT("[RESUME]   Done! (action continues)"));
    }
    else
    {
        // No active action - re-evaluate to find new task
        UE_LOG(LogScheduleComponent, Warning, TEXT("[RESUME]   No active action - re-evaluating schedule..."));
   EvaluateSchedule();
   UE_LOG(LogScheduleComponent, Warning, TEXT("[RESUME]   Done! (new action started)"));
    }
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

//==============================================================================
// GameEventBus Integration (Dialogue System soft coupling)
//==============================================================================

void UScheduleComponent::SubscribeToDialogueEvents()
{
#if WITH_GAMEEVENTBUS
    UWorld* World = GetWorld();
  if (!World)
    {
        UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE] Cannot subscribe: World is null"));
        return;
 }

    UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
    if (!EventBus)
    {
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE] GameEventBus not available - dialogue integration disabled"));
        return;
    }

    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE] ============================================"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE] Subscribing to Dialogue & Sequence events for actor: %s"), 
        GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));

    // Subscribe to Dialogue.Started event
 FGameplayTag DialogueStartedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Tag 'GameEvent.Dialogue.Started' valid: %s"), 
        DialogueStartedTag.IsValid() ? TEXT("YES") : TEXT("NO"));
    
    DialogueStartedHandle = EventBus->SubscribeToEventNative(DialogueStartedTag,
   FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnDialogueStarted));
    
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Subscribed to 'Dialogue.Started' (Handle valid: %s)"),
        DialogueStartedHandle.IsValid() ? TEXT("YES") : TEXT("NO"));

    // Subscribe to Dialogue.Ended event
    FGameplayTag DialogueEndedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Ended"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Tag 'GameEvent.Dialogue.Ended' valid: %s"), 
        DialogueEndedTag.IsValid() ? TEXT("YES") : TEXT("NO"));
    
    DialogueEndedHandle = EventBus->SubscribeToEventNative(DialogueEndedTag,
        FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnDialogueEnded));

    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Subscribed to 'Dialogue.Ended' (Handle valid: %s)"),
        DialogueEndedHandle.IsValid() ? TEXT("YES") : TEXT("NO"));
    
    // ✅ NEW v1.13.3: Subscribe to Sequence.Started event
    FGameplayTag SequenceStartedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.Started"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Tag 'GameEvent.Sequence.Started' valid: %s"), 
        SequenceStartedTag.IsValid() ? TEXT("YES") : TEXT("NO"));
    
    SequenceStartedHandle = EventBus->SubscribeToEventNative(SequenceStartedTag,
   FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnSequenceStarted));
    
  UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Subscribed to 'Sequence.Started' (Handle valid: %s)"),
        SequenceStartedHandle.IsValid() ? TEXT("YES") : TEXT("NO"));
    
    // ✅ NEW v1.13.3: Subscribe to Sequence.Ended event
    FGameplayTag SequenceEndedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.Ended"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Tag 'GameEvent.Sequence.Ended' valid: %s"), 
 SequenceEndedTag.IsValid() ? TEXT("YES") : TEXT("NO"));
    
    SequenceEndedHandle = EventBus->SubscribeToEventNative(SequenceEndedTag,
     FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnSequenceEnded));
  
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Subscribed to 'Sequence.Ended' (Handle valid: %s)"),
        SequenceEndedHandle.IsValid() ? TEXT("YES") : TEXT("NO"));
    
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE] Subscription complete!"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE] ============================================"));
#else
    UE_LOG(LogScheduleComponent, Verbose, TEXT("GameEventBus not compiled - dialogue integration disabled"));
#endif
}

void UScheduleComponent::UnsubscribeFromDialogueEvents()
{
#if WITH_GAMEEVENTBUS
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		return;
	}

	// Unsubscribe from dialogue events
	if (DialogueStartedHandle.IsValid())
	{
		FGameplayTag DialogueStartedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started"));
		EventBus->UnsubscribeNative(DialogueStartedTag, DialogueStartedHandle);
		DialogueStartedHandle.Reset();
	}

	if (DialogueEndedHandle.IsValid())
	{
		FGameplayTag DialogueEndedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Ended"));
		EventBus->UnsubscribeNative(DialogueEndedTag, DialogueEndedHandle);
		DialogueEndedHandle.Reset();
	}
	
	// ✅ NEW v1.13.3: Unsubscribe from sequence events
	if (SequenceStartedHandle.IsValid())
	{
		FGameplayTag SequenceStartedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.Started"));
		EventBus->UnsubscribeNative(SequenceStartedTag, SequenceStartedHandle);
		SequenceStartedHandle.Reset();
	}

	if (SequenceEndedHandle.IsValid())
	{
		FGameplayTag SequenceEndedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.Ended"));
		EventBus->UnsubscribeNative(SequenceEndedTag, SequenceEndedHandle);
		SequenceEndedHandle.Reset();
	}
#endif
}

#if WITH_GAMEEVENTBUS
void UScheduleComponent::OnDialogueStarted(const FGameEventPayload& Payload)
{
    // Check if this actor is involved in the dialogue
    AActor* Owner = GetOwner();
    if (!Owner)
 {
        return;
    }

    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] OnDialogueStarted received for actor: %s"), *Owner->GetName());
    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   DialogueId: %s"), *Payload.StringParam.ToString());
    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   InstigatorActor: %s"), 
        Payload.InstigatorActor.IsValid() ? *Payload.InstigatorActor->GetName() : TEXT("NULL"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   TargetActor: %s"), 
        Payload.TargetActor.IsValid() ? *Payload.TargetActor->GetName() : TEXT("NULL"));

  // ✅ v1.14: Option 1 - Pause ALL NPCs during ANY dialogue
    if (bPauseDuringAnyDialogue)
    {
        UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] bPauseDuringAnyDialogue=TRUE - pausing schedule"));
        PauseSchedule();
        return;
    }

 // ✅ v1.14: Option 2 - Only pause if this actor is a participant (DEFAULT behavior)
    // - Direct participant: InstigatorActor (Player) or TargetActor (NPC)
    // - Additional participant: Listed in AdditionalActors (sequence actors, etc.)
    bool bIsInvolvedInDialogue = (Payload.TargetActor == Owner) || (Payload.InstigatorActor == Owner);

    // ✅ v1.13.2: Check AdditionalActors array (sequence participants, etc.)
    if (!bIsInvolvedInDialogue && Payload.AdditionalActors.Num() > 0)
    {
        bIsInvolvedInDialogue = Payload.AdditionalActors.Contains(Owner);
        if (bIsInvolvedInDialogue)
 {
            UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   Found in AdditionalActors (sequence/additional participant)"));
      }
    }

    // ✅ NEW v1.14: Check AdditionalPersonaIds array (DialogueDataAsset AdditionalPersonas)
    if (!bIsInvolvedInDialogue && Payload.AdditionalPersonaIds.Num() > 0)
    {
        // Get DialogueComponent from owner (via reflection - NO HARD DEPENDENCY!)
    // Use FindObject to get class without #include
 UClass* DialogueComponentClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.DialogueComponent"));
        if (DialogueComponentClass)
{
   UActorComponent* DialogueComp = Owner->GetComponentByClass(DialogueComponentClass);
   if (DialogueComp)
 {
         // Get CharacterId via UFunction call (reflection)
       UFunction* GetParticipantIdFunc = DialogueComp->FindFunction(FName("GetParticipantId"));
          if (GetParticipantIdFunc)
       {
      FName MyCharacterId = NAME_None;
        DialogueComp->ProcessEvent(GetParticipantIdFunc, &MyCharacterId);
         
  // Check if my CharacterId matches any AdditionalPersonaId
     for (const FString& PersonaIdStr : Payload.AdditionalPersonaIds)
          {
             if (MyCharacterId == FName(*PersonaIdStr))
{
     bIsInvolvedInDialogue = true;
 UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   ✅ Found in AdditionalPersonaIds (PersonaId='%s')"), 
      *PersonaIdStr);
 break;
 }
     }
  }
      }
     }
    }

    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   bIsInvolvedInDialogue: %s"), 
  bIsInvolvedInDialogue ? TEXT("YES") : TEXT("NO"));

    if (bIsInvolvedInDialogue)
    {
        UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] Dialogue started - pausing schedule for actor: %s (DialogueId: %s)"),
           *Owner->GetName(), *Payload.StringParam.ToString());

        // Pause schedule
 PauseSchedule();
    }
}

void UScheduleComponent::OnDialogueEnded(const FGameEventPayload& Payload)
{
    // Check if this actor was involved in the dialogue
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] OnDialogueEnded received for actor: %s"), *Owner->GetName());
    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   InstigatorActor: %s"), 
        Payload.InstigatorActor.IsValid() ? *Payload.InstigatorActor->GetName() : TEXT("NULL"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   TargetActor: %s"), 
        Payload.TargetActor.IsValid() ? *Payload.TargetActor->GetName() : TEXT("NULL"));
    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   bIsPaused: %s"), bIsPaused ? TEXT("YES") : TEXT("NO"));

    // ✅ v1.14: If bPauseDuringAnyDialogue=true, always resume if paused
    if (bPauseDuringAnyDialogue && bIsPaused)
    {
        UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] bPauseDuringAnyDialogue=TRUE - resuming schedule"));
        ResumeSchedule();
        return;
    }

    // ✅ v1.14: Only resume if this actor was a participant (DEFAULT behavior)
    bool bWasInvolvedInDialogue = (Payload.TargetActor == Owner) || (Payload.InstigatorActor == Owner);

    // Check AdditionalActors
   if (!bWasInvolvedInDialogue && Payload.AdditionalActors.Num() > 0)
    {
        bWasInvolvedInDialogue = Payload.AdditionalActors.Contains(Owner);
    }

    // ✅ NEW v1.14: Check AdditionalPersonaIds
    if (!bWasInvolvedInDialogue && Payload.AdditionalPersonaIds.Num() > 0)
  {
        UClass* DialogueComponentClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.DialogueComponent"));
        if (DialogueComponentClass)
  {
       UActorComponent* DialogueComp = Owner->GetComponentByClass(DialogueComponentClass);
    if (DialogueComp)
{
       UFunction* GetParticipantIdFunc = DialogueComp->FindFunction(FName("GetParticipantId"));
      if (GetParticipantIdFunc)
 {
     FName MyCharacterId = NAME_None;
     DialogueComp->ProcessEvent(GetParticipantIdFunc, &MyCharacterId);
       
  for (const FString& PersonaIdStr : Payload.AdditionalPersonaIds)
 {
           if (MyCharacterId == FName(*PersonaIdStr))
          {
        bWasInvolvedInDialogue = true;
         break;
     }
     }
   }
  }
       }
    }

    UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT]   bWasInvolvedInDialogue: %s"), 
        bWasInvolvedInDialogue ? TEXT("YES") : TEXT("NO"));

  if (bWasInvolvedInDialogue && bIsPaused)
    {
        UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] Dialogue ended - resuming schedule for actor: %s from position: %s"),
      *Owner->GetName(), *Owner->GetActorLocation().ToString());

     // Resume schedule from CURRENT POSITION (не восстанавливаем позицию!)
     ResumeSchedule();
    }
    else if (bWasInvolvedInDialogue && !bIsPaused)
    {
        UE_LOG(LogScheduleComponent, Error, TEXT("[EVENT] ERROR: Actor was involved but NOT paused! This should not happen!"));
    }
}

void UScheduleComponent::OnSequenceStarted(const FGameEventPayload& Payload)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UE_LOG(LogScheduleComponent, Warning, TEXT("[SEQUENCE] OnSequenceStarted received for actor: %s"), *Owner->GetName());
	UE_LOG(LogScheduleComponent, Warning, TEXT("[SEQUENCE]   Sequence: %s"), *Payload.StringParam.ToString());
	UE_LOG(LogScheduleComponent, Warning, TEXT("[SEQUENCE]   Sequence Participants: %d"), Payload.AdditionalActors.Num());

	// Check if this actor is a sequence participant
	bool bIsParticipant = Payload.AdditionalActors.Contains(Owner);

	UE_LOG(LogScheduleComponent, Warning, TEXT("[SEQUENCE]   bIsParticipant: %s"), 
		bIsParticipant ? TEXT("YES") : TEXT("NO"));

	if (bIsParticipant)
	{
		UE_LOG(LogScheduleComponent, Warning, TEXT("[SEQUENCE] Actor is sequence participant - pausing schedule"));
		PauseSchedule();
	}
}

void UScheduleComponent::OnSequenceEnded(const FGameEventPayload& Payload)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UE_LOG(LogScheduleComponent, Warning, TEXT("[SEQUENCE] OnSequenceEnded received for actor: %s"), *Owner->GetName());

	// Check if this actor was a sequence participant
	bool bWasParticipant = Payload.AdditionalActors.Contains(Owner);

	UE_LOG(LogScheduleComponent, Warning, TEXT("[SEQUENCE]   bWasParticipant: %s"), 
		bWasParticipant ? TEXT("YES") : TEXT("NO"));

	if (bWasParticipant && bIsPaused)
	{
		UE_LOG(LogScheduleComponent, Warning, TEXT("[SEQUENCE] Sequence ended - resuming schedule for actor: %s"), 
			*Owner->GetName());
		ResumeSchedule();
	}
}
#endif // WITH_GAMEEVENTBUS
