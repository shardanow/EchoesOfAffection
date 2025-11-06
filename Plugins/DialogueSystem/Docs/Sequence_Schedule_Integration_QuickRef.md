# Sequence + Schedule Integration - Quick Reference

## Problem
Actors with active schedules were being **teleported back** after sequence playback ended, breaking natural NPC movement.

## Solution
`DialogueEffect_PlaySequence` now checks if actor has **active schedule** before restoring transform.

## How It Works

### Before Fix (v1.13.3)
```
1. Dialogue starts with Lianne
2. Sequence plays (Alice walks in background)
3. Sequence saves Alice's transform: X=1068.533
4. Alice walks to X=1200.000
5. Sequence ends
6. System RESTORES Alice's transform ? X=1068.533 ?? TELEPORT!
```

### After Fix (v1.13.4)
```
1. Dialogue starts with Lianne
2. Sequence plays (Alice walks in background)
3. Sequence saves Alice's transform: X=1068.533
4. Alice walks to X=1200.000
5. Sequence ends
6. System CHECKS: Is Alice's schedule active? YES
7. System SKIPS transform restoration ? Alice stays at X=1200.000 ?
```

## Detection Logic

```cpp
// Check if actor has ScheduleComponent (soft dependency)
for (UActorComponent* Comp : Actor->GetComponents())
{
    if (Comp->GetClass()->GetName().Contains(TEXT("ScheduleComponent")))
    {
  bool bIsPaused = true;
        bool bHasActiveAction = false;
   
        // Get state via reflection
        Comp->ProcessEvent(FindFunction("IsSchedulePaused"), &bIsPaused);
   Comp->ProcessEvent(FindFunction("HasActiveAction"), &bHasActiveAction);
        
        // Don't restore if schedule is active AND not paused
    if (bHasActiveAction && !bIsPaused)
        {
 bShouldRestore = false; // Skip!
        }
    }
}
```

## When Transform IS Restored

? **Dialogue participants** (Lianne talking to player)  
? **Sequence-only actors** (no ScheduleComponent)  
? **Actors with paused schedule** (frozen during dialogue)  

## When Transform IS NOT Restored

? **Background NPCs with active schedules** (Alice walking)  
? **NPCs moving during sequence** (not in dialogue)  

## Logs to Watch

```
[LogDialogueSequence] Skipping transform restore for actor: BP_Alice_C_1 
 (has active schedule: YES, paused: NO)
    
[LogDialogueSequence] Restored transform for actor: BP_Lianne_C_1 
    to X=1937.514 Y=-4104.943 Z=96.650
```

## Testing Checklist

1. ? Start dialogue with NPC A
2. ? Play sequence showing NPC B walking in background
3. ? End dialogue
4. ? **Expected**: NPC B continues from current position
5. ? **Expected**: NPC A restored to dialogue start position

## Related Docs

- `Sequence_Schedule_Integration_Fix.md` - Full technical details
- `Sequence_Auto_Stop_Fix.md` - Auto-stop mechanism
- `ActorSchedule_Architecture.md` - Schedule system integration

## Version
- **Added**: v1.13.4
- **Module**: DialogueSystemCore
- **File**: `DialogueEffect_PlaySequence.cpp`
