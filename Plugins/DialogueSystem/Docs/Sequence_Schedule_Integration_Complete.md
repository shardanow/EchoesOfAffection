# Sequence + Schedule + Dialogue Integration - Complete Architecture

## Overview

This document explains how **DialogueSystem**, **ActorScheduleSystem**, and **Level Sequencer** work together to create natural, cinematic NPC behavior in an open-world dating sim.

## Core Concept

**NPCs move around on schedules** (morning routines, work, etc.), but when:
- Player talks to them ? **Pause schedule**
- Cutscene plays ? **Pause participants**
- Dialogue ends ? **Resume from current position**

## System Components

### 1. DialogueSystem
- **Purpose**: Manage conversations between player and NPCs
- **Key Class**: `UDialogueRunner`
- **Events**: `GameEvent.Dialogue.Started`, `GameEvent.Dialogue.Ended`

### 2. ActorScheduleSystem
- **Purpose**: Make NPCs follow daily routines (move, idle, work)
- **Key Class**: `UScheduleComponent`
- **Events**: Listens to dialogue/sequence events and pauses/resumes

### 3. Level Sequencer
- **Purpose**: Play cinematic sequences (camera movements, actor animations)
- **Key Class**: `UDialogueEffect_PlaySequence`
- **Events**: `GameEvent.Sequence.Started`, `GameEvent.Sequence.Ended`

## Integration Architecture

```
      PLAYER TALKS TO LIANNE
 |
     v
    +-------------+-------------+
    |      |
    v       v
DIALOGUE SYSTEM          SCHEDULE SYSTEM
  - Start dialogue        - Listen to GameEvent.Dialogue.Started
  - Emit event       - Check if actor is Instigator/Target
  - Block input      - Pause Lianne's schedule
  - Show UI   - Stop AI movement
             - Alice keeps moving (not involved)
    |
    v
SEQUENCE PLAYS (optional)
  - Play Level Sequence: "S_EllenWalksIn"
  - Emit GameEvent.Sequence.Started
  - Save transforms: Lianne (X=1937), Alice (X=1068)
  - Move Alice to X=1200 (keyframe animation)
  
 |
           v
  SCHEDULE SYSTEM (again)
    - Listen to GameEvent.Sequence.Started
    - Check AdditionalActors array
    - Pause Alice's schedule (she's in sequence)
    - Alice stops AI movement, sequencer takes over
    
           |
   v
   SEQUENCE ENDS
     - Emit GameEvent.Sequence.Ended
     - Check each actor:
         • Lianne: Schedule paused? YES ? Restore to X=1937 ?
         • Alice: Schedule paused? YES ? But active? NO ? Skip restore ?
   - Alice stays at X=1200 (natural!)
     
       |
           v
   DIALOGUE ENDS
     - Emit GameEvent.Dialogue.Ended
     - Resume Lianne's schedule
     - Lianne continues from X=1937 (where dialogue happened)
     
     |
           v
   ALICE CONTINUES
     - Sequence ended earlier ? Schedule auto-resumed
     - Alice continues from X=1200 (where sequence left her)
     - Natural open-world behavior! ?
```

## Event Flow Diagram

```
TIME    LIANNE (Dialogue Target)    ALICE (Background NPC)
----    --------------------------        ----------------------
 0s     [Schedule Active: Walk]  [Schedule Active: Walk]
     Position: X=1937       Position: X=1068
        
 1s     GameEvent.Dialogue.Started ?    (Not involved, keeps moving)
        Schedule PAUSED ? Position: X=1100
        
 2sSequence starts ?        GameEvent.Sequence.Started ?
        Transform SAVED (X=1937)         Transform SAVED (X=1100)
   Schedule PAUSED ?
          
 3s     (Frozen at X=1937)               Sequence moves to X=1200
 4s     (Frozen at X=1937)   (Moved by sequence)
 5s     Sequence ends ?            Sequence ends ?
     Schedule still PAUSED      bShouldRestore = false
     Transform RESTORED ?        Transform NOT restored ?
        Position: X=1937           Position: X=1200
        Schedule RESUMED ?
       
 6s     Dialogue ends ?   (Moving on schedule)
      Schedule RESUMED ?        Position: X=1220
        Continues from X=1937 ?         Continues from X=1200 ?
```

## Key Design Decisions

### Why Check Schedule State Before Restoring?

**Problem**: Sequence always restored transforms for ALL possessables  
**Impact**: Alice teleported from X=1200 ? X=1100 (where sequence started)  
**Solution**: Only restore if schedule is **paused** (actor was frozen)

### Why Use Soft Dependency (Reflection)?

DialogueSystem should NOT depend on ActorScheduleSystem module.

```cpp
// ? BAD: Hard dependency
#include "Components/ScheduleComponent.h"
UScheduleComponent* ScheduleComp = Actor->FindComponentByClass<UScheduleComponent>();

// ? GOOD: Soft dependency
UActorComponent* ScheduleComp = /* find by name */;
if (ScheduleComp->GetClass()->GetName().Contains(TEXT("ScheduleComponent")))
{
    // Use reflection to call methods
}
```

### Why Event-Driven Architecture?

**Before (v1.12)**: Direct function calls, tight coupling  
**After (v1.13+)**: Events through GameEventBus

**Benefits**:
- ? Loose coupling
- ? Testable
- ? Extensible (other systems can listen)
- ? No compile dependencies

## Implementation Details

### 1. Sequence Participant Gathering

```cpp
TArray<AActor*> UDialogueEffect_PlaySequence::GatherSequenceParticipants()
{
    TArray<AActor*> Participants;
    
    // Find all Possessable actors in sequence
    for (int32 i = 0; i < MovieScene->GetPossessableCount(); ++i)
  {
   FMovieScenePossessable& Possessable = MovieScene->GetPossessable(i);
      AActor* Actor = /* find in world by name */;
        Participants.Add(Actor);
    }
    
 return Participants; // [BP_Alice_C_1, BP_Lianne_C_1, ...]
}
```

### 2. Event Emission

```cpp
void UDialogueEffect_PlaySequence::EmitSequenceStartedEvent()
{
    FGameEventPayload Payload;
    Payload.EventTag = "GameEvent.Sequence.Started";
    Payload.InstigatorActor = Player;
    Payload.TargetActor = NPC;
    Payload.AdditionalActors = GatherSequenceParticipants(); // ?? KEY!
    
    EventBus->EmitEvent(Payload);
}
```

### 3. Schedule Component Listener

```cpp
void UScheduleComponent::OnSequenceStarted(const FGameEventPayload& Payload)
{
    // Check if this actor is in the sequence
    for (AActor* Actor : Payload.AdditionalActors)
    {
        if (Actor == GetOwner())
        {
            // This actor is in sequence ? pause schedule
         PauseSchedule();
     break;
        }
    }
}
```

### 4. Smart Transform Restoration

```cpp
void UDialogueEffect_PlaySequence::CleanupSequence()
{
 for (auto& Pair : OriginalActorTransforms)
  {
    AActor* Actor = Pair.Key.Get();
    bool bShouldRestore = true;
    
     // Find ScheduleComponent (soft dependency)
        for (UActorComponent* Comp : Actor->GetComponents())
        {
      if (Comp->GetClass()->GetName().Contains("ScheduleComponent"))
         {
         bool bIsPaused = /* reflection call */;
      bool bHasActiveAction = /* reflection call */;
        
      // Don't restore if schedule is active and not paused
       if (bHasActiveAction && !bIsPaused)
            {
 bShouldRestore = false; // ?? KEY!
             }
                break;
            }
        }
        
        if (bShouldRestore)
        {
    Actor->SetActorTransform(OriginalTransform);
        }
    }
}
```

## Use Cases

### Use Case 1: Simple Dialogue (No Sequence)
```
1. Player talks to Lianne
2. Lianne's schedule pauses ? frozen at X=1937
3. Dialogue ends
4. Lianne's schedule resumes ? continues from X=1937 ?
```

### Use Case 2: Dialogue with Sequence (Lianne Only)
```
1. Player talks to Lianne
2. Lianne's schedule pauses ? frozen at X=1937
3. Sequence plays ? moves Lianne to X=2000
4. Sequence ends ? Lianne restored to X=1937 ?
5. Dialogue ends ? Lianne continues from X=1937 ?
```

### Use Case 3: Dialogue with Sequence (Lianne + Alice)
```
1. Player talks to Lianne
2. Lianne pauses, Alice keeps moving
3. Alice walks to X=1100
4. Sequence starts ? Alice paused at X=1100, saved
5. Sequence moves Alice to X=1200
6. Sequence ends:
   - Lianne: paused ? restore to X=1937 ?
   - Alice: active + not paused ? DON'T restore, stay at X=1200 ?
7. Alice resumes schedule from X=1200 ?
8. Dialogue ends ? Lianne resumes from X=1937 ?
```

## Benefits

? **Natural NPC behavior** - No teleporting after cutscenes  
? **Open world feel** - NPCs keep moving in background  
? **Cinematic quality** - Full control during sequences  
? **Event-driven** - Loose coupling between systems  
? **Testable** - Each system can be tested independently  
? **Extensible** - Easy to add new systems (quests, etc.)  

## Known Limitations

?? **Possessable vs Spawnable**: Only works with Possessables  
   - Spawnables are destroyed after sequence ? no schedule to resume  
   - See `Sequencer_Possessable_vs_Spawnable.md`

?? **Reflection overhead**: Minimal but exists  
   - Could be optimized with caching in future

?? **Name-based actor matching**: Simplified implementation  
   - Real project should use GUID-based binding resolution

## Version History

- **v1.13.4** - Smart transform restoration (this fix)
- **v1.13.3** - Sequence participant events
- **v1.13.2** - GameEventBus integration
- **v1.13.0** - Initial schedule + sequence integration
- **v1.12** - Sequence support
- **v1.11** - Input blocker improvements

## Related Documentation

- `Sequence_Schedule_Integration_Fix.md` - This fix in detail
- `Sequence_Schedule_Integration_QuickRef.md` - Quick reference
- `Sequence_Auto_Stop_Fix.md` - Auto-stop on dialogue end
- `ActorSchedule_Architecture.md` - Schedule system design
- `Sequencer_Possessable_vs_Spawnable.md` - Actor type differences

## Author Notes

This integration was built for **Echoes of Affection**, an open-world dating sim where:
- NPCs have daily routines (work, eat, sleep)
- Player can interrupt them for dialogue
- Cutscenes show NPCs in background
- Everything should feel natural and alive

The key insight was: **"If an actor is moving on their own schedule, don't teleport them back after a cutscene!"**

---

**Happy dating! ??**
