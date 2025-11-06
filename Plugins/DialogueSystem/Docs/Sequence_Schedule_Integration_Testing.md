# Testing: Sequence + Schedule Integration Fix (v1.13.4)

## Test Scenario

**Setup**: Two NPCs with schedules walking around
- **Lianne** (Dialogue Target) - Schedule: Walk around town
- **Alice** (Background NPC) - Schedule: Walk around town
- **Sequence**: "S_EllenWalksIn" showing Alice walking in background

## Expected Behavior

### BEFORE Fix (v1.13.3)
```
1. Talk to Lianne ? Lianne pauses, Alice keeps moving
2. Sequence plays ? Shows Alice walking from X=1068 to X=1200
3. Sequence ends ? ?? Alice TELEPORTS back to X=1068
4. Dialogue ends ? Alice resumes from X=1068 (wrong!)
```

### AFTER Fix (v1.13.4)
```
1. Talk to Lianne ? Lianne pauses, Alice keeps moving
2. Sequence plays ? Shows Alice walking from X=1100 to X=1200
3. Sequence ends ? ? Alice STAYS at X=1200
4. Dialogue ends ? Alice resumes from X=1200 (correct!)
```

## Test Steps

### 1. Prepare Test Environment

**NPCs Setup**:
- Create/Open `BP_Lianne` blueprint
  - Add `DialogueComponent`
  - Add `ScheduleComponent` with "Walk Around" schedule
  - Place in level

- Create/Open `BP_Alice` blueprint
  - Add `ScheduleComponent` with "Walk Around" schedule
  - Place in level

**Sequence Setup**:
- Open/Create Level Sequence "S_EllenWalksIn"
  - Add **Possessable** binding for Alice
  - Add Transform track: Move Alice from start to destination
  - Duration: ~5 seconds

**Dialogue Setup**:
- Create dialogue asset "DLG_Lianne_Test"
  - Node 1 (NPC): "Hello!"
  - Node 2: `PlaySequence(/Game/Sequences/S_EllenWalksIn)`
  - Node 3 (NPC): "Did you see Alice walk by?"
  - Node 4 (End)

### 2. Run Test

**Steps**:
1. ?? Play in Editor
2. ?? Wait for Alice to walk to position X=~1100
3. ?? Press E to talk to Lianne
4. ?? Watch sequence play
5. ? End dialogue
6. ?? Check Alice's position

**Expected Result**:
```
? Alice continues walking from where sequence left her (~X=1200)
? Alice does NOT teleport back to sequence start (~X=1100)
```

### 3. Check Logs

Look for these log messages:

**Sequence Start**:
```
[LogDialogueSequence] Saved transform for actor: BP_Alice_C_1 
    at X=1100.000 Y=-3879.322 Z=96.650
[LogDialogueSequence] Saved transform for actor: BP_Lianne_C_1 
    at X=1937.514 Y=-4104.943 Z=96.650
```

**Sequence End (KEY LOGS)**:
```
[LogDialogueSequence] Actor BP_Alice_C_1 has ScheduleComponent 
    - will restore (active: YES, paused: NO)  ? Should be YES, NO
[LogDialogueSequence] Skipping transform restore for actor: BP_Alice_C_1 
    (has active schedule: YES, paused: NO)  ? KEY! This means fix works
    
[LogDialogueSequence] Actor BP_Lianne_C_1 has ScheduleComponent 
    - will restore (active: YES, paused: YES)  ? Should be YES, YES
[LogDialogueSequence] Restored transform for actor: BP_Lianne_C_1 
    to X=1937.514 Y=-4104.943 Z=96.650  ? Lianne restored
```

## Troubleshooting

### Problem: Alice still teleports back

**Possible causes**:
1. ? Alice doesn't have `ScheduleComponent`
   - **Fix**: Add ScheduleComponent to BP_Alice

2. ? Alice's schedule is paused
 - **Fix**: Make sure Alice is NOT dialogue participant
   - Check logs: `bIsInvolvedInDialogue: NO`

3. ? Sequence uses **Spawnable** instead of **Possessable**
   - **Fix**: Convert Alice binding to Possessable
   - See `Sequencer_Possessable_vs_Spawnable.md`

4. ? Module not recompiled after fix
   - **Fix**: Rebuild DialogueSystemCore module

### Problem: No logs appear

**Possible causes**:
1. ? Log category disabled
   - **Fix**: Enable `LogDialogueSequence` in Editor Settings
   - Set to "Log" or "Verbose"

2. ? Sequence not using PlaySequence effect
   - **Fix**: Check dialogue script has `PlaySequence(...)` command

## Validation Checklist

Before marking as fixed:

- [ ] Alice continues from sequence end position
- [ ] Lianne returns to dialogue start position
- [ ] No teleporting/snapping/warping
- [ ] Schedules resume after dialogue ends
- [ ] Logs show "Skipping transform restore for actor: BP_Alice"
- [ ] Logs show "Restored transform for actor: BP_Lianne"

## Edge Cases to Test

### Test 1: No Sequence (Simple Dialogue)
- Expected: Both actors return to dialogue start positions ?

### Test 2: Sequence with Only Lianne
- Expected: Lianne restored, no Alice logs ?

### Test 3: Sequence with Both, No Schedules
- Expected: Both actors restored (old behavior) ?

### Test 4: Player Interrupts Sequence
- Expected: Actors keep current positions ?

## Performance Testing

**Metrics to watch**:
- Reflection overhead: ~0.1ms per actor (negligible)
- Memory usage: No significant change
- Frame time: No measurable impact

**Test**:
```
1. Spawn 10 NPCs with schedules
2. Play sequence involving all 10
3. Check frame time before/after
4. Expected: <0.5ms total overhead
```

## Reporting Issues

If test fails, include:
1. **Logs** from LogDialogueSequence
2. **Logs** from LogScheduleComponent
3. **Screenshot** of actor positions
4. **Blueprint** settings (ScheduleComponent, DialogueComponent)
5. **Sequence** settings (Possessable/Spawnable type)

Report to: [GitHub Issues](https://github.com/shardanaw/EchoesOfAffection/issues)

## Success Criteria

? **Pass**: Alice walks naturally, no teleporting  
? **Pass**: Lianne returns to dialogue position  
? **Pass**: Schedules resume correctly  
? **Pass**: Logs confirm schedule detection  

? **Fail**: Any actor teleports unexpectedly  
? **Fail**: Schedules don't resume  
? **Fail**: No logs from DialogueSequence  

---

**Test completed**: ____________  
**Result**: ? PASS ? FAIL  
**Notes**: _____________________________________
