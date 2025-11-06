# ActorScheduleSystem ? DialogueSystem Integration Summary

## ?? Overview

**Version:** 1.14.0  
**Date:** 2025-11-05  
**Status:** ? Production Ready

This document summarizes the **complete integration** between ActorScheduleSystem and DialogueSystem through GameEventBus, enabling NPCs to automatically pause their schedules during dialogues and sequences.

---

## ?? Key Features

### ? Core Functionality
- **Automatic schedule pause** when NPC is involved in dialogue
- **Automatic resume** when dialogue/sequence ends
- **Optional "freeze all NPCs"** mode during any dialogue
- **Sequence participant tracking** via `AdditionalActors`
- **Zero hard dependencies** between systems
- **Event-driven architecture** via GameEventBus

### ?? Configuration Options
- `bPauseDuringAnyDialogue` - Pause schedule during ANY dialogue (not just when NPC is participant)
- Default behavior: Only pause if NPC is dialogue participant (Player, NPC, or sequence actor)

---

## ??? Architecture

### Event Flow Diagram

```
???????????????????????
?  DialogueRunner     ?
??
?  StartDialogue()  ?
?  ?? ProcessNode()?
??? EmitEvent     ?
???????????????????????
  ?
           ? FGameEventPayload
           ? ?? EventTag: "GameEvent.Dialogue.Started"
           ? ?? InstigatorActor: Player
         ? ?? TargetActor: NPC
           ? ?? AdditionalActors: [Sequence participants]
         ?
           ?
??????????????????????????????
? GameEventBusSubsystem ?  ? Central Hub (no logic)
?      ?
? TMap<Tag, Delegates>       ?
??????????????????????????????
   ?
           ? Broadcast to subscribers
           ?
 ?????????????????????????????????????????
      ?          ?     ?           ?
  ?          ?        ?             ?
???????????? ???????????? ????????????? ????????????
?Schedule  ? ?Quest     ? ?Relation   ? ?[Future]  ?
?Component ? ?System    ? ?System     ? ?Combat    ?
?       ? ?          ? ?           ? ?System    ?
?OnDialogue? ?OnDialogue? ?OnDialogue ? ?OnDialogue?
?Started() ? ?Started() ? ?Started()  ? ?Started() ?
???????????? ???????????? ????????????? ????????????
     ?
     ? Decision Logic
     ?
     ?? if (bPauseDuringAnyDialogue == true)
     ?     ?? PauseSchedule() ? Freeze ALL NPCs
     ?
     ?? else if (IsParticipant)
        ?? Is Instigator? (Player)
           ?? Is Target? (NPC)
      ?? Is in AdditionalActors? (Sequence)
       ?? PauseSchedule() ? Only this NPC
         ?? Stop AI movement
      ?? Stop CharacterMovementComponent
      ?? Wait for Dialogue.Ended event
```

---

## ?? Implementation Details

### 1. DialogueRunner (Emitter)

**File:** `Plugins/DialogueSystem/Source/DialogueSystemCore/Private/Core/DialogueRunner.cpp`

```cpp
void UDialogueRunner::EmitDialogueStartedEvent(FName DialogueId, const TArray<UObject*>& Participants)
{
#if WITH_GAMEEVENTBUS
    UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(GetWorld());
    if (!EventBus) return;

    // Create event payload
    FGameEventPayload Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started"));
    Payload.StringParam = DialogueId;
 
    // Set participants as instigator/target (Player and NPC only)
    if (Participants.Num() > 0)
        Payload.InstigatorActor = Cast<AActor>(Participants[0]); // Player
    if (Participants.Num() > 1)
   Payload.TargetActor = Cast<AActor>(Participants[1]); // NPC

    // ? Add AdditionalParticipants from context (sequence actors)
    if (CurrentContext && CurrentContext->GetParticipants())
    {
        TArray<UObject*> AdditionalObjs = CurrentContext->GetParticipants()->GetAdditionalParticipants();
        for (UObject* Obj : AdditionalObjs)
        {
         if (AActor* Actor = Cast<AActor>(Obj))
    Payload.AdditionalActors.AddUnique(Actor);
        }
    }

 // Emit event
    EventBus->EmitEvent(Payload, true);
#endif
}
```

**Key Points:**
- ? Uses compile-time flag `WITH_GAMEEVENTBUS`
- ? No direct dependency on ScheduleSystem
- ? GameplayTags for event identification
- ? Supports sequence participants via `AdditionalActors`

---

### 2. ScheduleComponent (Listener)

**File:** `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Private/Components/ScheduleComponent.cpp`

#### Subscription (BeginPlay)

```cpp
void UScheduleComponent::SubscribeToDialogueEvents()
{
#if WITH_GAMEEVENTBUS
    UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(GetWorld());
    if (!EventBus) return;

    // Subscribe to Dialogue.Started event
    FGameplayTag DialogueStartedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started"));
    DialogueStartedHandle = EventBus->SubscribeToEventNative(DialogueStartedTag,
        FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnDialogueStarted));

    // Subscribe to Dialogue.Ended event
    FGameplayTag DialogueEndedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Ended"));
  DialogueEndedHandle = EventBus->SubscribeToEventNative(DialogueEndedTag,
    FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnDialogueEnded));

    // Subscribe to Sequence.Started event
    FGameplayTag SequenceStartedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.Started"));
  SequenceStartedHandle = EventBus->SubscribeToEventNative(SequenceStartedTag,
        FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnSequenceStarted));

    // Subscribe to Sequence.Ended event
  FGameplayTag SequenceEndedTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.Ended"));
    SequenceEndedHandle = EventBus->SubscribeToEventNative(SequenceEndedTag,
        FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UScheduleComponent::OnSequenceEnded));
#endif
}
```

#### Event Handler

```cpp
void UScheduleComponent::OnDialogueStarted(const FGameEventPayload& Payload)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // ? Option 1: Pause ALL NPCs during ANY dialogue
    if (bPauseDuringAnyDialogue)
    {
      UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] bPauseDuringAnyDialogue=TRUE - pausing schedule"));
        PauseSchedule();
 return;
    }

    // ? Option 2: Only pause if this actor is a participant (DEFAULT behavior)
    bool bIsInvolvedInDialogue = 
        (Payload.TargetActor == Owner) ||// NPC in dialogue
        (Payload.InstigatorActor == Owner) ||      // Player (rare)
        Payload.AdditionalActors.Contains(Owner);  // Sequence participant

    if (bIsInvolvedInDialogue)
    {
        UE_LOG(LogScheduleComponent, Warning, TEXT("[EVENT] Dialogue started - pausing schedule"));
     PauseSchedule();
    }
}
```

**Key Points:**
- ? Two modes: "Freeze all" vs "Only participants"
- ? Checks `AdditionalActors` for sequence participants
- ? Falls back gracefully if GameEventBus not available

---

### 3. PauseSchedule Implementation

**File:** `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Private/Components/ScheduleComponent.cpp`

```cpp
void UScheduleComponent::PauseSchedule()
{
    bIsPaused = true;

    // Pause current action
    if (CurrentActionHandle.IsValid() && CurrentEntry && CurrentEntry->Action)
    {
        IScheduleActionInterface::Execute_PauseAction(CurrentEntry->Action, CurrentActionHandle);
    }

    // ? AGGRESSIVE STOP: Force stop movement immediately
    AActor* Owner = GetOwner();
    if (Owner)
    {
        // Stop AI movement
    if (APawn* Pawn = Cast<APawn>(Owner))
{
            if (AAIController* AIController = Cast<AAIController>(Pawn->GetController()))
      {
        AIController->StopMovement();
   }
        }

        // Stop character movement component
        if (ACharacter* Character = Cast<ACharacter>(Owner))
        {
            if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
     {
       MovementComp->StopMovementImmediately();
}
        }
    }
}
```

---

## ?? Build Configuration

### DialogueSystemCore.Build.cs

```csharp
public class DialogueSystemCore : ModuleRules
{
    public DialogueSystemCore(ReadOnlyTargetRules Target) : base(Target)
    {
     // ... existing modules ...

        // ? Soft dependency on GameEventBus
        if (Target.bBuildEditor || IsPluginAvailable(Target, "GameEventBus"))
        {
            PublicDependencyModuleNames.Add("GameEventBus");
 PublicDefinitions.Add("WITH_GAMEEVENTBUS=1");
        }
        else
 {
     PublicDefinitions.Add("WITH_GAMEEVENTBUS=0");
 }
    }

    private bool IsPluginAvailable(ReadOnlyTargetRules Target, string PluginName)
    {
string PluginsDir = Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins", PluginName);
     return Directory.Exists(PluginsDir);
    }
}
```

### ScheduleRuntime.Build.cs

```csharp
public class ScheduleRuntime : ModuleRules
{
    public ScheduleRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        // ... existing modules ...

        // ? Soft dependency on GameEventBus
        if (Target.bBuildEditor || IsPluginAvailable(Target, "GameEventBus"))
        {
            PublicDependencyModuleNames.Add("GameEventBus");
          PublicDefinitions.Add("WITH_GAMEEVENTBUS=1");
        }
    else
        {
          PublicDefinitions.Add("WITH_GAMEEVENTBUS=0");
        }
    }

    private bool IsPluginAvailable(ReadOnlyTargetRules Target, string PluginName)
    {
        string PluginsDir = Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins", PluginName);
        return Directory.Exists(PluginsDir);
  }
}
```

**Key Points:**
- ? Compile-time feature flags (`WITH_GAMEEVENTBUS`)
- ? Automatic plugin detection
- ? No hard dependencies
- ? Works with or without GameEventBus

---

## ?? Usage Guide

### 1. Default Behavior (Only Participants Pause)

**No configuration needed!** By default, only NPCs involved in dialogue will pause:

```
Player talks to NPC_Lianne:
  ? NPC_Lianne pauses schedule
  ? NPC_Alice continues schedule (not involved)
```

### 2. "Freeze All NPCs" Mode

**Blueprint:**
1. Select NPC actor in World Outliner
2. Find `ScheduleComponent` in Details panel
3. Enable **"Pause During Any Dialogue"** checkbox

**C++:**
```cpp
ScheduleComponent->bPauseDuringAnyDialogue = true;
```

**Result:**
```
Player talks to NPC_Lianne:
  ? NPC_Lianne pauses schedule (participant)
  ? NPC_Alice pauses schedule (bPauseDuringAnyDialogue=true)
  ? NPC_Bob pauses schedule (bPauseDuringAnyDialogue=true)
```

### 3. Sequence Participants

**Automatically handled!** If NPC is in a sequence:

```cpp
// DialogueEffect_PlaySequence adds actors to AdditionalActors
Payload.AdditionalActors.Add(SequenceActor);
```

**Result:**
```
Dialogue has sequence with NPC_Guard:
  ? NPC_Lianne pauses (target)
  ? NPC_Guard pauses (AdditionalActors)
  ? NPC_Alice continues (not involved)
```

---

## ?? Testing & Debugging

### Log Categories

Enable these in `DefaultEngine.ini`:
```ini
[Core.Log]
LogScheduleComponent=VeryVerbose
LogDialogueRunner=VeryVerbose
LogGameEventBus=VeryVerbose
```

### Expected Logs (Successful Pause)

```
[DialogueRunner] EMITTING: GameEvent.Dialogue.Started
[DialogueRunner]   DialogueId: lianne_greeting
[DialogueRunner]   Player (Instigator): bp_MyAnimeCharacter_C_0
[DialogueRunner] NPC (Target): BP_Lianne_C_1

[ScheduleComponent] [EVENT] OnDialogueStarted received for actor: BP_Lianne_C_1
[ScheduleComponent] [EVENT]   DialogueId: lianne_greeting
[ScheduleComponent] [EVENT]   bIsInvolvedInDialogue: YES
[ScheduleComponent] [EVENT] Dialogue started - pausing schedule
[ScheduleComponent] [PAUSE] Pausing schedule for actor: BP_Lianne_C_1
[ScheduleComponent] [PAUSE]   Stopped AI movement
[ScheduleComponent] [PAUSE]   Stopped CharacterMovementComponent
```

### Expected Logs (No Pause - Not Involved)

```
[ScheduleComponent] [EVENT] OnDialogueStarted received for actor: BP_Alice_C_1
[ScheduleComponent] [EVENT]   DialogueId: lianne_greeting
[ScheduleComponent] [EVENT]   bIsInvolvedInDialogue: NO
```

### Expected Logs (Freeze All Mode)

```
[ScheduleComponent] [EVENT] OnDialogueStarted received for actor: BP_Alice_C_1
[ScheduleComponent] [EVENT]   bPauseDuringAnyDialogue=TRUE - pausing schedule
[ScheduleComponent] [PAUSE] Pausing schedule for actor: BP_Alice_C_1
```

---

## ? Architecture Quality

### Dependency Analysis

| System | Depends On | Type |
|--------|-----------|------|
| DialogueSystem | GameEventBus | Soft (compile-time) |
| ScheduleSystem | GameEventBus | Soft (compile-time) |
| DialogueSystem | ScheduleSystem | ? None |
| ScheduleSystem | DialogueSystem | ? None |

**Result:** ? Zero circular dependencies, fully decoupled

### Design Patterns Used

| Pattern | Implementation | Benefit |
|---------|---------------|---------|
| **Event Bus** | GameEventBusSubsystem | Decoupling |
| **Observer** | SubscribeToEventNative() | Loose coupling |
| **Strategy** | bPauseDuringAnyDialogue | Configurable behavior |
| **Compile-time Polymorphism** | `#if WITH_GAMEEVENTBUS` | Optional features |
| **GameplayTags** | Event identification | Type-safe strings |

### SOLID Principles

| Principle | How Applied |
|-----------|-------------|
| **Single Responsibility** | Each system handles one domain |
| **Open/Closed** | Extensible via new event subscriptions |
| **Liskov Substitution** | Interface-based actions (IScheduleActionInterface) |
| **Interface Segregation** | Minimal event payload structure |
| **Dependency Inversion** | Depend on GameEventBus abstraction |

---

## ?? Performance Characteristics

### Event Bus Overhead

| Operation | Complexity | Notes |
|-----------|-----------|-------|
| **EmitEvent** | O(n) | n = subscribers for this tag |
| **Subscribe** | O(1) | TMap insert |
| **Unsubscribe** | O(1) | TMap remove |
| **Tag Lookup** | O(1) | TMap find |

### Memory Usage

```cpp
// Per ScheduleComponent instance
sizeof(FDelegateHandle) ? 4 = 32 bytes (4 event handles)

// Per event emission
sizeof(FGameEventPayload) = ~128 bytes (transient)
```

**Conclusion:** ? Negligible overhead for typical game scenarios

---

## ?? Future Enhancements (Optional)

### 1. Priority-based Pause
```cpp
// Skip pause if higher-priority action is running
if (CurrentEntry->Priority > DialoguePriority)
    return; // Don't interrupt
```

### 2. Pause Radius
```cpp
// Pause NPCs within X meters of dialogue
UPROPERTY(EditAnywhere)
float PauseRadius = 500.0f;
```

### 3. Event Batching
```cpp
// Batch multiple dialogue starts within same frame
TArray<FGameEventPayload> PendingEvents;
void FlushEvents(); // Called end of frame
```

**Note:** Current implementation is already production-ready; these are optional optimizations.

---

## ?? Related Documentation

- **[ActorSchedule_Architecture.md](ActorSchedule_Architecture.md)** - Schedule system overview
- **[Sequence_Schedule_Integration_Complete.md](../DialogueSystem/Docs/Sequence_Schedule_Integration_Complete.md)** - Sequence integration details
- **[GameEventBus API Reference](../GameEventBus/Docs/API_Reference.md)** - Event bus documentation

---

## ?? Best Practices

### DO ?
- Use `bPauseDuringAnyDialogue` for cinematic moments
- Keep default behavior (only participants pause) for open-world NPCs
- Log event flow during development
- Test with multiple NPCs in scene

### DON'T ?
- Don't add hard dependencies between systems
- Don't poll for dialogue state (use events)
- Don't forget to unsubscribe in `EndPlay()`
- Don't emit events from Tick() (use state changes)

---

## ?? Troubleshooting

### Problem: NPC doesn't pause during dialogue

**Check:**
1. Is `GameEventBus` plugin enabled?
2. Is `ScheduleComponent` subscribed? (Check logs for `[SUBSCRIBE]`)
3. Is NPC actually a participant? (Check `bIsInvolvedInDialogue` log)
4. Is `bScheduleEnabled = true`?

**Solution:**
```cpp
// Enable verbose logging
LogScheduleComponent=VeryVerbose
LogDialogueRunner=VeryVerbose
```

### Problem: All NPCs freeze (but shouldn't)

**Check:**
- Is `bPauseDuringAnyDialogue = true` accidentally set?

**Solution:**
```cpp
// Disable per-NPC or globally
ScheduleComponent->bPauseDuringAnyDialogue = false;
```

### Problem: Schedule doesn't resume after dialogue

**Check:**
1. Is `Dialogue.Ended` event emitted? (Check logs)
2. Is `ResumeSchedule()` called?
3. Is `bIsPaused` still true?

**Solution:**
```cpp
// Debug log in OnDialogueEnded
UE_LOG(LogScheduleComponent, Warning, TEXT("Resuming schedule..."));
```

---

## ?? Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.14.0 | 2025-11-05 | Added `bPauseDuringAnyDialogue` option |
| 1.13.4 | 2025-11-04 | Fixed AdditionalParticipants support |
| 1.13.3 | 2025-11-03 | Added Sequence.Started/Ended events |
| 1.13.2 | 2025-11-02 | Improved participant detection |
| 1.13.0 | 2025-11-01 | Initial GameEventBus integration |

---

## ?? Credits

**Architecture:** Event-driven design with soft dependencies  
**Implementation:** C++ with Blueprint exposure  
**Testing:** Multi-NPC scenario validation
**Documentation:** Complete integration summary

---

**Last Updated:** 2025-11-05  
**Status:** ? Production Ready  
**Maintainer:** [Your Name]
