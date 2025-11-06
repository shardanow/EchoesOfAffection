# Automatic Sequence Schedule Bridge (Simple Implementation)

## Problem

**Manual binding** is complex. We want **automatic** pause of sequence actors **without Blueprint setup**.

## Solution: Auto-Binding Component

Create a **simple C++ component** that automatically binds to sequence events.

---

## Implementation

### 1. Create Bridge Component

**File:** `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Public/Components/SequenceScheduleBridgeComponent.h`

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SequenceScheduleBridgeComponent.generated.h"

// Forward declarations
class UDialogueEffect_PlaySequence;
class UDialogueRunner;

/**
 * AUTO-BRIDGE: Automatically connects sequence participants to ActorSchedule
 * 
 * Usage:
 * 1. Add this component to Player or GameMode
 * 2. It will automatically bind to ALL PlaySequence effects
 * 3. Sequence actors will auto-pause their schedule during cutscenes
 * 
 * NO MANUAL SETUP REQUIRED!
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API USequenceScheduleBridgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USequenceScheduleBridgeComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Handle sequence participants gathered */
	UFUNCTION()
	void OnSequenceParticipantsGathered(const TArray<AActor*>& Participants);

	/** Bind to dialogue events */
	void BindToDialogueEvents();

	/** Unbind from dialogue events */
	void UnbindFromDialogueEvents();

	/** Stored participants for cleanup */
	UPROPERTY(Transient)
	TArray<AActor*> CurrentSequenceParticipants;
};
```

### 2. Implement Component

**File:** `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Components/SequenceScheduleBridgeComponent.cpp`

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/SequenceScheduleBridgeComponent.h"
#include "Core/DialogueRunner.h"
#include "Effects/DialogueEffect_PlaySequence.h"
#include "Subsystems/DialogueSubsystem.h"

// GameEventBus integration (soft dependency)
#if WITH_GAMEEVENTBUS
#include "GameEventBusSubsystem.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogSequenceBridge, Log, All);

USequenceScheduleBridgeComponent::USequenceScheduleBridgeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USequenceScheduleBridgeComponent::BeginPlay()
{
	Super::BeginPlay();
	BindToDialogueEvents();
}

void USequenceScheduleBridgeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromDialogueEvents();
	Super::EndPlay(EndPlayReason);
}

void USequenceScheduleBridgeComponent::BindToDialogueEvents()
{
	// Subscribe to DialogueSubsystem events
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UDialogueSubsystem* DialogueSubsystem = World->GetSubsystem<UDialogueSubsystem>();
	if (!DialogueSubsystem)
	{
		UE_LOG(LogSequenceBridge, Warning, TEXT("DialogueSubsystem not found"));
		return;
	}

	// Bind to "Dialogue About To Start" event
	// This is fired BEFORE dialogue starts, so we can bind to the runner's effects
	DialogueSubsystem->OnDialogueAboutToStart.AddDynamic(this, &USequenceScheduleBridgeComponent::OnDialogueAboutToStart);

	UE_LOG(LogSequenceBridge, Log, TEXT("SequenceScheduleBridge initialized and listening"));
}

void USequenceScheduleBridgeComponent::UnbindFromDialogueEvents()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UDialogueSubsystem* DialogueSubsystem = World->GetSubsystem<UDialogueSubsystem>();
	if (DialogueSubsystem)
	{
		DialogueSubsystem->OnDialogueAboutToStart.RemoveAll(this);
	}
}

void USequenceScheduleBridgeComponent::OnDialogueAboutToStart(UDialogueRunner* Runner, AActor* Player, AActor* NPC)
{
	if (!Runner)
	{
		return;
	}

	// Find all PlaySequence effects in this dialogue
	UDialogueDataAsset* DialogueAsset = Runner->GetLoadedDialogue();
	if (!DialogueAsset)
	{
		return;
	}

	// Iterate through all nodes
	for (UDialogueNode* Node : DialogueAsset->Nodes)
	{
		if (!Node)
		{
			continue;
		}

		// Check effects
		for (UDialogueEffect* Effect : Node->Effects)
		{
			UDialogueEffect_PlaySequence* SeqEffect = Cast<UDialogueEffect_PlaySequence>(Effect);
			if (!SeqEffect)
			{
				continue;
			}

			// Bind to this effect's event
			SeqEffect->OnSequenceParticipantsGathered.AddDynamic(this, &USequenceScheduleBridgeComponent::OnSequenceParticipantsGathered);
			UE_LOG(LogSequenceBridge, Log, TEXT("Bound to PlaySequence effect in node: %s"), *Node->NodeId.ToString());
		}
	}
}

void USequenceScheduleBridgeComponent::OnSequenceParticipantsGathered(const TArray<AActor*>& Participants)
{
	UE_LOG(LogSequenceBridge, Warning, TEXT("Sequence participants gathered: %d actors"), Participants.Num());

	// Store participants
	CurrentSequenceParticipants = Participants;

#if WITH_GAMEEVENTBUS
	// Emit GameEventBus event with participants
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		UE_LOG(LogSequenceBridge, Warning, TEXT("GameEventBus not available"));
		return;
	}

	// Create additional actors payload
	UDialogueAdditionalActors* AdditionalActors = NewObject<UDialogueAdditionalActors>();
	AdditionalActors->Actors = Participants;

	// Emit separate event for sequence participants
	// ScheduleComponent will listen to this and pause schedules
	FGameEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.ParticipantsGathered"));
	Payload.PayloadObject = AdditionalActors;

	EventBus->EmitEvent(Payload, true);

	UE_LOG(LogSequenceBridge, Log, TEXT("Emitted GameEvent.Sequence.ParticipantsGathered"));
#else
	UE_LOG(LogSequenceBridge, Error, TEXT("GameEventBus not compiled - cannot pause actor schedules!"));
#endif
}
```

### 3. Subscribe ScheduleComponent to New Event

Update `ScheduleComponent::SubscribeToDialogueEvents()`:

```cpp
void UScheduleComponent::SubscribeToDialogueEvents()
{
#if WITH_GAMEEVENTBUS
    // ...existing code...

    // NEW v1.13.2: Subscribe to Sequence.ParticipantsGathered event
    FGameplayTag SequenceParticipantsTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.ParticipantsGathered"));
    SequenceParticipantsHandle = EventBus->SubscribeToEventNative(SequenceParticipantsTag,
        FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnSequenceParticipantsGathered));

    UE_LOG(LogScheduleComponent, Warning, TEXT("[SUBSCRIBE]   Subscribed to 'Sequence.ParticipantsGathered'"));
#endif
}

void UScheduleComponent::OnSequenceParticipantsGathered(const FGameEventPayload& Payload)
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    // Check if this actor is in the participants list
    if (Payload.PayloadObject.IsValid())
    {
        if (UDialogueAdditionalActors* AdditionalActors = Cast<UDialogueAdditionalActors>(Payload.PayloadObject.Get()))
      {
          if (AdditionalActors->ContainsActor(Owner))
            {
       UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] Actor %s is sequence participant - pausing schedule"), *Owner->GetName());
PauseSchedule();
}
        }
    }
}
```

---

## Usage (SUPER SIMPLE!)

### 1. Add Component to Player

**In Player Blueprint:**
1. Open `bp_MyAnimeCharacter`
2. Add Component: **Sequence Schedule Bridge Component**
3. **Done!** ??

### 2. Add GameplayTag

**In `Config/DefaultGameplayTags.ini`:**

```ini
+GameplayTagList=(Tag="GameEvent.Sequence.ParticipantsGathered",DevComment="[GameEventBus] Sequence participants gathered")
```

### 3. Test

1. **Start PIE**
2. **Start dialogue** with sequence
3. **Alice freezes automatically!** ?

---

## How It Works

```
[Dialogue Starts]
    |
    v
[SequenceScheduleBridge subscribes to DialogueSubsystem]
    |
    v
[Dialogue contains PlaySequence effect]
    |
    v
[Bridge binds to OnSequenceParticipantsGathered event]
    |
    v
[Sequence executes ? GatherSequenceParticipants()]
    |
    v
[OnSequenceParticipantsGathered.Broadcast(Alice)]
    |
v
[Bridge receives event ? Emits GameEvent.Sequence.ParticipantsGathered]
    |
    v
[ScheduleComponent receives GameEvent ? Pauses Alice's schedule]
    |
    v
[? Alice is frozen during cutscene!]
```

---

## Benefits

? **ZERO Blueprint Setup** — Just add component!  
? **Automatic Binding** — No manual event wiring  
? **Clean Separation** — No hard dependencies  
? **Reusable** — Works for ANY dialogue with sequences  
? **Maintainable** — Single place to update logic  

---

## Alternative: Per-Dialogue Binding

If you want **per-dialogue control**, create Blueprint version:

1. Create Blueprint child of `SequenceScheduleBridgeComponent`
2. Override `OnSequenceParticipantsGathered`
3. Add custom logic (filters, logging, etc.)

---

**File:** `Plugins/DialogueSystem/Docs/Sequence_Auto_Bridge.md`  
**Version:** v1.13.2  
**Status:** Design Doc (not implemented yet)

**To implement:** Create the files above and add to project!
