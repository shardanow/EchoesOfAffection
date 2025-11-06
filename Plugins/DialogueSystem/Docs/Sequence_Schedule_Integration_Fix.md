# Sequence + Schedule Integration Fix

## Problem

When playing a Level Sequence during dialogue, the `DialogueEffect_PlaySequence` was **restoring transforms** for ALL possessable actors in the sequence, including actors with active schedules (like Alice).

### Symptoms
- Alice was teleported back to her position when the sequence started
- Her schedule movement was interrupted unexpectedly
- Only Lianne was paused (as dialogue participant), but Alice was also affected

### Root Cause
The sequence effect saved and restored transforms for:
1. **Dialogue participants** (Lianne) - ? Correct, they should be frozen during dialogue
2. **Sequence participants** (Alice) - ? **Incorrect**, they should continue their schedule

Alice was NOT a dialogue participant (she wasn't talking), but she WAS a sequence participant (she appeared in the cutscene as a Possessable).

## Solution

Modified `DialogueEffect_PlaySequence::CleanupSequence()` to **check if actor has active schedule** before restoring transform:

```cpp
// NEW: Check if actor has ScheduleComponent (soft dependency)
for (UActorComponent* Comp : Actor->GetComponents())
{
    if (Comp && Comp->GetClass()->GetName().Contains(TEXT("ScheduleComponent")))
    {
      // Use reflection to check if schedule is paused
        bool bIsPaused = true;
        bool bHasActiveAction = false;
        
        // Get state via reflection (soft dependency)
        UFunction* IsPausedFunc = Comp->FindFunction(TEXT("IsSchedulePaused"));
        UFunction* HasActiveFunc = Comp->FindFunction(TEXT("HasActiveAction"));
        
        if (IsPausedFunc) Comp->ProcessEvent(IsPausedFunc, &bIsPaused);
      if (HasActiveFunc) Comp->ProcessEvent(HasActiveFunc, &bHasActiveAction);
        
        // Don't restore if schedule is active AND not paused
    if (bHasActiveAction && !bIsPaused)
      {
     bShouldRestore = false; // Skip transform restoration
        }
   break;
    }
}
```

### How It Works

1. **Sequence starts** ? Saves transforms for ALL possessables (Lianne + Alice)
2. **Dialogue starts** ? Pauses Lianne's schedule (she's talking)
3. **Alice continues moving** ? Her schedule is NOT paused (she's not in dialogue)
4. **Sequence ends** ? Restoration logic:
   - **Lianne**: Schedule is paused ? ? Restore transform (she was frozen)
   - **Alice**: Schedule is active + not paused ? ? **Skip** restore (she's moving)

## Benefits

? **Actors with active schedules** are NOT teleported back after sequence  
? **Soft dependency** - uses reflection, no hard coupling to ScheduleSystem  
? **Dialogue participants** still properly frozen/restored  
? **Backward compatible** - doesn't break sequences without schedules  

## Testing

1. Start dialogue with Lianne
2. Sequence plays showing Alice walking
3. Dialogue ends
4. **Expected**: Alice continues her schedule from current position
5. **Before fix**: Alice teleports back to sequence start position
6. **After fix**: Alice keeps moving naturally ?

## Related Files

- `Plugins/DialogueSystem/Source/DialogueSystemCore/Private/Effects/DialogueEffect_PlaySequence.cpp` - Main fix
- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Public/Components/ScheduleComponent.h` - Schedule API
- `Plugins/DialogueSystem/Docs/Sequence_Schedule_Integration_Summary.md` - Original integration docs

## Notes

- Uses **reflection** (`FindFunction` + `ProcessEvent`) for soft coupling
- Falls back to **restore transform** if ScheduleComponent not found (safe default)
- Adds detailed logging for debugging:
  ```
  [LogDialogueSequence] Skipping transform restore for actor: BP_Alice_C_1 (has active schedule: YES, paused: NO)
  [LogDialogueSequence] Restored transform for actor: BP_Lianne_C_1 to X=1937.514 Y=-4104.943 Z=96.650
  ```

## Version History

- **v1.13.4** - Added schedule check before transform restoration
- **v1.13.3** - Added GameEvent.Sequence.Started/Ended events
- **v1.13.0** - Initial sequence + schedule integration
