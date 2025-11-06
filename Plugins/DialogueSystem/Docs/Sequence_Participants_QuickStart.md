# Sequence Participants Integration (v1.13.2) - Quick Start

## Problem Solved

**Actors in Level Sequences** (Possessables like Alice) were **continuing their schedule** during dialogue, causing them to run away mid-cutscene.

## Solution: Event-Driven Integration (NO Hard Dependencies!)

`DialogueEffect_PlaySequence` broadcasts an event with **all sequence participants** (Possessable actors). External systems (Blueprint or C++) can listen to this event and pause ActorSchedule.

---

## Architecture

```
DialogueEffect_PlaySequence
  ??> GatherSequenceParticipants()  // Find Possessable actors
    ??> OnSequenceParticipantsGathered.Broadcast(Actors)  // Fire event

Blueprint Bridge (YOU CREATE THIS!)
    ??> Listen to OnSequenceParticipantsGathered
    ??> Create UDialogueAdditionalActors
    ??> Pass to DialogueRunner via GameEventBus
```

---

## Step 1: Create Bridge Blueprint

### Option A: Event Graph Bridge (Recommended)

1. Open your **Dialogue Data Asset** or **Dialogue Effect**
2. Find `PlaySequence` effect instance
3. Bind to **On Sequence Participants Gathered** event
4. In event graph:

```blueprint
Event OnSequenceParticipantsGathered
    |
    ??> Get Game Instance
  ??> Get Subsystem (GameEventBusSubsystem)
         ??> Get Current Payload (from DialogueRunner)
    ??> Create Dialogue Additional Actors object
        ??> Set Actors array = Participants
   ??> Set PayloadObject = Additional Actors object
             ??> Update payload in event system
```

**Visual Blueprint:**
```
[OnSequenceParticipantsGathered]
        |
        v
[Create DialogueAdditionalActors Object]
        |
        v
  [Set Actors = Participants]
        |
 v
[GameEventBus -> Update Payload]
```

### Option B: C++ Bridge (Advanced)

```cpp
// In your GameMode or custom subsystem
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Find all DialogueEffect_PlaySequence instances and bind
 // This is advanced - Blueprint is simpler!
}
```

---

## Step 2: Update DialogueRunner Event Emission

**In Blueprint:**

Create a custom Blueprint function library:

```cpp
// MyDialogueHelpers.h
UCLASS()
class UMyDialogueHelpers : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Emit Dialogue Started event with additional actors
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    static void EmitDialogueStartedWithSequenceActors(
        UObject* WorldContextObject,
        FName DialogueId,
        AActor* Player,
        AActor* NPC,
        const TArray<AActor*>& SequenceActors);
};
```

```cpp
// MyDialogueHelpers.cpp
void UMyDialogueHelpers::EmitDialogueStartedWithSequenceActors(
    UObject* WorldContextObject,
    FName DialogueId,
    AActor* Player,
    AActor* NPC,
  const TArray<AActor*>& SequenceActors)
{
    UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
    if (!EventBus)
    {
        return;
    }

    // Create payload
    FGameEventPayload Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started"));
    Payload.StringParam = DialogueId;
    Payload.InstigatorActor = Player;
    Payload.TargetActor = NPC;

    // Create additional actors object
    if (SequenceActors.Num() > 0)
    {
        UDialogueAdditionalActors* AdditionalActors = NewObject<UDialogueAdditionalActors>();
        AdditionalActors->Actors = SequenceActors;
 Payload.PayloadObject = AdditionalActors;
    }

    // Emit event
    EventBus->EmitEvent(Payload, true);
}
```

**In Blueprint:** Call this function in your dialogue start event.

---

## Step 3: Verify Integration

### Test Steps

1. **Start PIE** (Play In Editor)
2. **Start dialogue** with sequence
3. **Watch Output Log:**

```
[DialogueEffect_PlaySequence] Found sequence participant: BP_Alice_C_1
[DialogueEffect_PlaySequence] Broadcasting OnSequenceParticipantsGathered with 1 actors
[ScheduleComponent] [EVENT] Found in AdditionalActors (sequence participant)
[ScheduleComponent] Dialogue started - pausing schedule for actor: BP_Alice_C_1
[ScheduleComponent] [PAUSE] Stopped AI movement
```

? **Alice should now be FROZEN during dialogue!**

---

## Alternative: Automatic Integration (Simple!)

**If you control DialogueRunner startup**, you can bind automatically:

```cpp
// In DialogueSubsystem::StartDialogue_Implementation()

// After creating DialogueRunner:
if (ActiveDialogue)
{
    // Find PlaySequence effects in dialogue
    for (UDialogueNode* Node : DialogueAsset->Nodes)
    {
        for (UDialogueEffect* Effect : Node->Effects)
        {
        if (UDialogueEffect_PlaySequence* SeqEffect = Cast<UDialogueEffect_PlaySequence>(Effect))
   {
    // Bind to event
     SeqEffect->OnSequenceParticipantsGathered.AddDynamic(this, &UDialogueSubsystem::OnSequenceParticipantsGathered);
            }
        }
 }
}
```

Then handle the event:

```cpp
void UDialogueSubsystem::OnSequenceParticipantsGathered(const TArray<AActor*>& Participants)
{
    UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
    if (!EventBus)
    {
     return;
    }

    // Create additional actors object
    UDialogueAdditionalActors* AdditionalActors = NewObject<UDialogueAdditionalActors>();
    AdditionalActors->Actors = Participants;

    // Update the current dialogue event payload
    // (This requires extending DialogueRunner to store last emitted payload)
    // For now, emit a separate event:
    FGameEventPayload Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.ParticipantsGathered"));
    Payload.InstigatorActor = CurrentPlayer;
    Payload.TargetActor = CurrentNPC;
    Payload.PayloadObject = AdditionalActors;

  EventBus->EmitEvent(Payload, true);
}
```

---

## Benefits

? **No Hard Dependencies** — `DialogueEffect_PlaySequence` doesn't know about `GameEventBus`  
? **Flexible Integration** — Choose Blueprint or C++ bridge  
? **Event-Driven** — Clean separation of concerns  
? **Testable** — Can mock event handlers
? **Extensible** — Other systems can also listen to sequence participants  

---

## Troubleshooting

### Alice Still Moving?

1. **Check Output Log** for `"Broadcasting OnSequenceParticipantsGathered"`
2. **Verify Event Binding** in Blueprint
3. **Check GameEventBus** is receiving `PayloadObject`
4. **Verify ScheduleComponent** finds actor in `AdditionalActors`

### No Output Logs?

- **Enable verbose logging:**
  ```
  LogDialogueSequence Verbose
  LogScheduleComponent Verbose
  ```

### Event Not Firing?

- **Verify sequence has Possessable actors** (not Spawnables!)
- **Check sequence asset is loaded** before `Execute()`

---

## Summary

**Version:** v1.13.2  
**Feature:** Sequence Participants Integration  
**Pattern:** Event-Driven Bridge (no hard dependencies)  
**Status:** ? Compiled & Ready

**Next Step:** Implement Blueprint/C++ bridge in your project!

---

**File:** `Plugins/DialogueSystem/Docs/Sequence_Participants_QuickStart.md`  
**Author:** GitHub Copilot Assistant  
**Date:** 2025-11-05
