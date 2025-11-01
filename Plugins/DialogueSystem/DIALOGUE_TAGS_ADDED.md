# ? FIXED: Missing Dialogue GameplayTags

## ?? **Problem Found**

**Error logs:**
```
Requested Gameplay Tag GameEvent.Dialogue.ChoiceSelected was not found
Requested Gameplay Tag GameEvent.Dialogue.Completed was not found
```

**Root cause:** `DialogueSubsystem.cpp` emits events with these tags, but they were **NOT registered** in `DefaultGameplayTags.ini`.

---

## ? **Solution: Added Missing Tags**

### **Added to `Config/DefaultGameplayTags.ini`:**

```ini
+GameplayTagList=(Tag="GameEvent.Dialogue.ChoiceSelected",DevComment="[GameEventBus] Player selected a dialogue choice")
+GameplayTagList=(Tag="GameEvent.Dialogue.Completed",DevComment="[GameEventBus] Dialogue was completed/ended")
```

---

## ?? **Complete Dialogue Event Tags**

| Tag | When Emitted | Emitted By |
|-----|--------------|------------|
| `GameEvent.Dialogue.Started` | Dialogue begins | `DialogueSubsystem::StartDialogue` |
| `GameEvent.Dialogue.ChoiceSelected` | Player picks choice | `DialogueSubsystem::HandleChoiceSelected` |
| `GameEvent.Dialogue.Completed` | Dialogue ends | `DialogueSubsystem::HandleDialogueEnded` |
| `GameEvent.Dialogue.NodeReached` | Specific node entered | *(Future use)* |

---

## ?? **Where Tags Are Used**

### **1. DialogueSubsystem.cpp — StartDialogue**
```cpp
UGameEventBusLibrary::EmitDialogueEvent(
    this,
FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")), // ? EXISTS
    NpcId,
 NAME_None,
    Player,
  NPC
);
```

### **2. DialogueSubsystem.cpp — HandleChoiceSelected**
```cpp
UGameEventBusLibrary::EmitDialogueEvent(
    this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.ChoiceSelected")), // ? WAS MISSING ? ? NOW ADDED
    NAME_None,
    ChoiceNode->NodeId,
    CurrentPlayer,
    CurrentNPC
);
```

### **3. DialogueSubsystem.cpp — HandleDialogueEnded**
```cpp
UGameEventBusLibrary::EmitDialogueEvent(
    this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Completed")), // ? WAS MISSING ? ? NOW ADDED
  DialogueId,
    NAME_None,
    CurrentPlayer,
    CurrentNPC
);
```

---

## ?? **Quest System Can Now Use These Events**

### **Example: Quest Objective Conditions**

```cpp
// Data Asset configuration
Objective: "Talk to Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Started  // ? Works
    NpcId: Lianne
    Count: 1

Objective: "Choose to help Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.ChoiceSelected  // ? NOW WORKS!
    NodeId: node_player_agree
    Count: 1

Objective: "Complete dialogue with Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Completed  // ? NOW WORKS!
    NpcId: Lianne
    Count: 1
```

---

## ?? **Before vs After**

### **? Before (Missing Tags)**
```
[PIE] Player clicks choice
   ?
DialogueSubsystem::HandleChoiceSelected()
   ?
FGameplayTag::RequestGameplayTag("GameEvent.Dialogue.ChoiceSelected")
   ?
? ERROR: Tag not found!
   ?
?? Ensure failed, event NOT emitted
```

### **? After (Tags Added)**
```
[PIE] Player clicks choice
   ?
DialogueSubsystem::HandleChoiceSelected()
   ?
FGameplayTag::RequestGameplayTag("GameEvent.Dialogue.ChoiceSelected")
   ?
? Tag found!
   ?
UGameEventBusLibrary::EmitDialogueEvent()
   ?
GameEventBus ? QuestEventBridge ? QuestSystem
   ?
?? Quest objective updates!
```

---

## ?? **How to Test**

### **Step 1: Restart Unreal Editor**
**IMPORTANT:** GameplayTags are loaded **at editor startup**. You MUST restart UE after editing `DefaultGameplayTags.ini`.

```
1. Close Unreal Editor
2. Reopen .uproject
3. Wait for GameplayTags to reload
```

### **Step 2: Play in Editor**
```
1. Start PIE
2. Talk to NPC
3. Select a dialogue choice
4. End dialogue
```

### **Step 3: Check Logs**

**You should see:**
```
? NO ERRORS about missing tags!

[QuestBridge] Received game event: 'GameEvent.Dialogue.Started'
[QuestBridge] Received game event: 'GameEvent.Dialogue.ChoiceSelected'
[QuestBridge] Received game event: 'GameEvent.Dialogue.Completed'
```

**No more:**
```
? Requested Gameplay Tag GameEvent.Dialogue.ChoiceSelected was not found
? Requested Gameplay Tag GameEvent.Dialogue.Completed was not found
```

---

## ?? **Files Changed**

| File | Change |
|------|--------|
| `Config/DefaultGameplayTags.ini` | ? Added `GameEvent.Dialogue.ChoiceSelected` |
| `Config/DefaultGameplayTags.ini` | ? Added `GameEvent.Dialogue.Completed` |

---

## ?? **Related Systems**

These tags are used by:
- ? **DialogueSubsystem** — Emits events
- ? **GameEventBus** — Routes events
- ? **QuestEventBridge** — Converts to quest events
- ? **QuestSystem** — Can use as objective conditions

---

## ? **Status**

- ? **Tags Added:** Complete
- ? **DialogueSubsystem:** Already emitting events
- ? **QuestSystem:** Can listen to these events
- ?? **Action Required:** **RESTART EDITOR** to load tags

---

**Last Updated:** 2025-11-01  
**Issue:** Missing dialogue event tags  
**Status:** ? FIXED  
**Next:** Restart editor and test! ??
