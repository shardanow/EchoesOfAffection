# ? AdditionalPersonas Implementation CANCELLED

## ?? Critical Architecture Violation

**Problem:** Attempting to pass resolved AActor* pointers from DialogueSubsystem (Runtime) to DialogueRunner (Core) creates:
1. ? Circular module dependency (Core cannot depend on Runtime)
2. ? Lifetime management issues (pointers become invalid)
3. ? Memory access violations (crash with `this = 0x48`)

---

## ? CORRECT Architecture (Not Yet Implemented)

### Option A: Event-Based Resolution (RECOMMENDED)

```
???????????????????????
? DialogueDataAsset   ?
? AdditionalPersonas: ?
? { "Alice": { ... }} ?
???????????????????????
           ?
           ?
?????????????????????????
? DialogueRunner    ?
? EmitDialogueStarted   ?
?              ?
? Payload.StringArrayParam: ?
? ["Alice", "Bob", ...]     ?
?????????????????????????
  ?
       ?
?????????????????????????
? GameEventBusSubsystem ?
?????????????????????????
           ?
        ?
?????????????????????????
? ScheduleComponent     ?
? (Each NPC)      ?
?             ?
? OnDialogueStarted()   ?
? { ?
?   MyName = GetOwner()->GetName(); ?
?   if (StringArrayParam.Contains(MyName)) ?
?     PauseSchedule();  ?
? }           ?
?????????????????????????
```

**Benefits:**
- ? No hard dependencies
- ? No pointer lifetime issues
- ? Each component resolves itself
- ? Fully event-driven

---

### Option B: Use CharacterId Instead of Actor Names

```cpp
// DialogueRunner::EmitDialogueStartedEvent()
Payload.StringArrayParam.Empty();
for (const TPair<FName, FDialoguePersonaData>& Pair : LoadedDialogue->AdditionalPersonas)
{
    Payload.StringArrayParam.Add(Pair.Value.PersonaId.ToString());  // "Alice", "Bob", etc.
}

// ScheduleComponent::OnDialogueStarted()
UDialogueComponent* DialogueComp = GetOwner()->FindComponentByClass<UDialogueComponent>();
if (DialogueComp)
{
    FName MyCharacterId = IDialogueParticipant::Execute_GetParticipantId(DialogueComp);
    
    for (const FString& PersonaIdStr : Payload.StringArrayParam)
    {
   if (MyCharacterId == FName(*PersonaIdStr))
        {
         PauseSchedule();
          break;
     }
    }
}
```

**Benefits:**
- ? Uses existing CharacterId system
- ? No actor name dependency
- ? More robust (names can change, IDs should not)

---

## ?? What We WILL NOT DO

### ? Pass AActor* Pointers Between Modules
**Reason:** Creates circular dependency, lifetime issues, crashes

### ? Store Actor References in DialogueContext
**Reason:** Core module cannot depend on Runtime module objects

### ? Use TWeakObjectPtr in Payload
**Reason:** Still has lifetime management complexity, not event-driven

---

## ?? Current Status

### Files Reverted:
- `DialogueRunner.cpp` - Restored from Git (removed AdditionalParticipants code)
- `DialogueSubsystem.cpp` - Removed resolution + Participants array modification

### Files Unchanged:
- `DialogueSubsystem.h` - `ResolvePersonaToActor()` declaration (not used)
- `ScheduleComponent.cpp` - Already checks `AdditionalActors` in payload

---

## ?? Next Steps (When Implementing Correctly)

### Step 1: Modify `FGameEventPayload`
Add field for PersonaIds:
```cpp
// GameEventPayload.h
UPROPERTY(BlueprintReadWrite)
TArray<FString> StringArrayParam;  // For AdditionalPersonas
```

### Step 2: Populate in DialogueRunner
```cpp
// DialogueRunner::EmitDialogueStartedEvent()
if (LoadedDialogue && LoadedDialogue->AdditionalPersonas.Num() > 0)
{
    for (const TPair<FName, FDialoguePersonaData>& Pair : LoadedDialogue->AdditionalPersonas)
    {
   Payload.StringArrayParam.Add(Pair.Value.PersonaId.ToString());
    }
}
```

### Step 3: Check in ScheduleComponent
```cpp
// ScheduleComponent::OnDialogueStarted()
UDialogueComponent* DialogueComp = GetOwner()->FindComponentByClass<UDialogueComponent>();
if (DialogueComp)
{
    FName MyCharacterId = IDialogueParticipant::Execute_GetParticipantId(DialogueComp);
    
    // Check if my CharacterId is in AdditionalPersonas list
    for (const FString& PersonaIdStr : Payload.StringArrayParam)
    {
if (MyCharacterId == FName(*PersonaIdStr))
        {
        // I'm an AdditionalParticipant!
  bIsInvolvedInDialogue = true;
       break;
        }
  }
}

if (bIsInvolvedInDialogue)
{
    PauseSchedule();
}
```

---

## ? Benefits of Correct Approach

| Aspect | Pointer-Based (?) | String-Based (?) |
|--------|-------------------|------------------|
| Module Coupling | Tight (circular) | Loose (event-driven) |
| Lifetime Management | Complex | Simple |
| Crash Risk | High | Low |
| Testability | Hard | Easy |
| Scalability | Poor | Excellent |
| Performance | Fast lookup | Slightly slower (string comparison) |

**Performance Note:** String comparison is negligible overhead (happens once per dialogue start, ~5-10 NPCs max)

---

## ?? Documentation Created

- `AdditionalPersonas_Fix_Test_Guide.md` - Original (incorrect) approach
- `AdditionalPersonas_Crash_Fix.md` - Crash analysis
- `Schedule_Dialogue_Integration_SUMMARY.md` - Current working state (without AdditionalPersonas)
- **THIS FILE** - Why implementation was cancelled and correct approach

---

## ?? Lessons Learned

1. **Never pass Runtime objects to Core modules** - always use interfaces or identifiers
2. **Event-driven > Direct coupling** - use GameEventBus for cross-system communication
3. **Strings/Names > Pointers** - for event payloads, use serializable data
4. **Test early** - architectural violations often manifest as crashes

---

**Status:** Implementation Cancelled  
**Reason:** Architectural Violation (Circular Dependency)  
**Recommended Approach:** String-based PersonaId transmission via GameEventBus  
**Last Updated:** 2025-11-05 20:40

