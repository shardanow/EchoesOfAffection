# AdditionalPersonas Fix - Quick Test Guide

## ?? Problem
Alice is listed in Lianne's dialogue `AdditionalPersonas`, but she doesn't pause during the dialogue.

## ? Solution
Added **automatic resolution** of `PersonaId` ? `AActor*` in `DialogueSubsystem::StartDialogue()`.

---

## ?? Changes Made

### 1. **DialogueSubsystem.h**
```cpp
protected:
    /**
     * Resolve PersonaId to AActor by searching for DialogueComponent with matching CharacterId
     * @param PersonaId - The persona ID to search for (e.g., "Alice")
     * @param WorldContext - World to search in
     * @return Found actor, or nullptr
     */
    AActor* ResolvePersonaToActor(FName PersonaId, UWorld* WorldContext);
```

### 2. **DialogueSubsystem.cpp**
**Added** `ResolvePersonaToActor()` implementation:
- Iterates through all actors with `DialogueComponent`
- Compares `CharacterId` with `PersonaId`
- Returns matching actor

**Modified** `StartDialogue_Implementation()`:
- Resolves all `AdditionalPersonas` from `DialogueDataAsset`
- Adds resolved actors to `Participants` array (index 2+)
- Passes extended array to `DialogueRunner::StartDialogue()`

### 3. **DialogueRunner.cpp**
**Modified** `StartDialogue()`:
- Takes all participants from array
- Adds participants with index 2+ to `Context->Participants->AdditionalParticipants`

**Note:** NO changes to `EmitDialogueStartedEvent()` — already includes `AdditionalParticipants` in payload (v1.13.4)

---

## ?? How to Test

### Step 1: Setup Data Asset
1. Open `DA_Dialogue_Lianne_Greeting` (or create new)
2. **Persona** section ? **Additional Personas** ? Add entry
3. Set **PersonaId** = `Alice`
4. Save

### Step 2: Setup NPCs
1. Open `BP_Alice` blueprint
2. Find **DialogueComponent**
3. Set **Character Id** = `Alice` ? **CRITICAL: Must match PersonaId!**
4. Save

5. Ensure **ScheduleComponent** exists on both Alice and Lianne
6. **DO NOT** enable `bPauseDuringAnyDialogue` (test default behavior)

### Step 3: Run Test
1. Compile project (Ctrl+Alt+F11)
2. Launch PIE
3. Start dialogue with Lianne
4. **Expected Result:**
   - ? Lianne pauses (TargetActor)
   - ? Alice pauses (AdditionalParticipant) ? **NEW!**
   - ? NO `bPauseDuringAnyDialogue` flag needed

---

## ?? Log Verification

### Expected Logs (on dialogue start):
```
[LogDialogueSubsystem] ?? Resolving 1 AdditionalPersonas...
[LogDialogueSubsystem] ?? ResolvePersonaToActor: Searching for PersonaId='Alice'
[LogDialogueSubsystem] ? Resolved PersonaId='Alice' ? Actor='BP_Alice_C_1'
[LogDialogueSubsystem] ? Added AdditionalParticipant to Participants array: 'BP_Alice_C_1' (PersonaId='Alice')

[LogDialogueRunner] ?? Adding 1 AdditionalParticipants from Participants array...
[LogDialogueRunner] ? Added AdditionalParticipant: 'BP_Alice_C_1'

[LogDialogueRunner] [GAMEEVENTBUS] EMITTING: GameEvent.Dialogue.Started
[LogDialogueRunner] [GAMEEVENTBUS]   Player (Instigator): bp_MyAnimeCharacter_C_0
[LogDialogueRunner] [GAMEEVENTBUS]   NPC (Target): BP_Lianne_C_1
[LogDialogueRunner] [GAMEEVENTBUS]   AdditionalParticipants: 1

[LogScheduleComponent] [EVENT] OnDialogueStarted received for actor: BP_Alice_C_1
[LogScheduleComponent] [EVENT]   DialogueId: lianne_greeting
[LogScheduleComponent] [EVENT]   InstigatorActor: bp_MyAnimeCharacter_C_0
[LogScheduleComponent] [EVENT]   TargetActor: BP_Lianne_C_1
[LogScheduleComponent] [EVENT]   bIsInvolvedInDialogue: YES ?
[LogScheduleComponent] [EVENT] Dialogue started - pausing schedule for actor: BP_Alice_C_1
[LogScheduleComponent] [PAUSE] Pausing schedule for actor: BP_Alice_C_1
```

### Error Logs (if PersonaId not found):
```
[LogDialogueSubsystem] ?? Resolving 1 AdditionalPersonas...
[LogDialogueSubsystem] ?? ResolvePersonaToActor: Searching for PersonaId='Alice'
[LogDialogueSubsystem] ? ResolvePersonaToActor: No actor found for PersonaId='Alice'
[LogDialogueSubsystem] ? Failed to resolve PersonaId='Alice' to actor
```

**Solution:** Check `DialogueComponent.CharacterId` on Alice actor — must match `PersonaId` exactly!

---

## ?? Troubleshooting

### Problem: Alice doesn't pause

**Check:**
1. Does Alice have `ScheduleComponent`? ? Add if missing
2. Does Alice have `DialogueComponent`? ? Add if missing
3. Is `CharacterId` set correctly? ? Must match `PersonaId` in DA
4. Is Alice actually in scene? ? Check World Outliner
5. Is schedule enabled? ? `bScheduleEnabled = true`

**Enable verbose logging:**
```ini
[Core.Log]
LogDialogueSubsystem=VeryVerbose
LogDialogueRunner=VeryVerbose
LogScheduleComponent=VeryVerbose
```

### Problem: Compilation error - "ResolvePersonaToActor not found"

**Solution:**
```
1. Close Unreal Editor
2. Delete Intermediate/ and Binaries/ folders
3. Right-click .uproject ? Generate Visual Studio project files
4. Open .sln ? Build ? Rebuild Solution
```

### Problem: "No actor found for PersonaId"

**Common causes:**
- `CharacterId` is empty ? Set it
- `CharacterId` is different ? Make it match PersonaId
- Alice not spawned in level ? Add to world
- DialogueComponent missing ? Add component

---

## ?? Architecture Summary

```
???????????????????????
? DialogueDataAsset   ?
? AdditionalPersonas: ?
? { "Alice": { ... }} ?
???????????????????????
    ?
  ?
?????????????????????????
? DialogueSubsystem     ?
? StartDialogue()  ?
?                 ?
? 1. Resolve "Alice"    ?
?  via CharacterId    ?
? 2. Add to Participants?
?????????????????????????
           ?
           ?
?????????????????????????
? DialogueRunner        ?
? StartDialogue()       ?
?          ?
? Add Participants[2+]  ?
? to AdditionalParts... ?
?????????????????????????
 ?
           ?
?????????????????????????
? EmitDialogueStarted   ?
? Payload.Additional... ?
?????????????????????????
     ?
           ?
?????????????????????????
? ScheduleComponent     ?
? (Alice)      ?
?    ?
? if AdditionalActors   ?
?  .Contains(Self)  ?
?   ? PauseSchedule()   ?
?????????????????????????
```

---

## ? Success Criteria

- [x] Code compiles without errors
- [x] Alice pauses when Lianne dialogue starts
- [x] Alice resumes when dialogue ends
- [x] Logs show PersonaId resolution
- [x] Logs show AdditionalParticipants = 1
- [x] Logs show "bIsInvolvedInDialogue: YES" for Alice
- [x] No `bPauseDuringAnyDialogue` flag needed

---

**Last Updated:** 2025-11-05  
**Status:** ? Ready for Testing  
**Version:** DialogueSystem v1.14.0
