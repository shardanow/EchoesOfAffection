# ? DIALOGUE EVENTS ADDED TO QUEST SYSTEM!

## ?? ��������:

��� ������ ������� � NPC **�� �������������** ������� `GameEvent.Dialogue.Started`, ������� ������ ���� "Talk to NPC" **�� ��������**!

### ���� ����������:

```
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
```

**��:**
- ? **���** `[GameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started'`
- ? ������ �� �������� �������� ��� ��������� � NPC!

---

## ? �������:

### 1?? ��������� ������� ������� � DialogueSubsystem

**����:** `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Subsystems/DialogueSubsystem.cpp`

**���������:**

```cpp
// After dialogue started, emit event to GameEventBus
OnAnyDialogueStarted.Broadcast(ActiveDialogue, Player, NPC);

// ?? EMIT GameEvent for quest system
UGameEventBusLibrary::EmitDialogueEvent(
 this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
    NPC, // NPC actor
    Player, // Player actor
 DialogueAsset->DialogueId // Dialogue ID as StringParam
);

UE_LOG(LogDialogueSubsystem, Log, TEXT("Started dialogue '%s'"), *DialogueAsset->DialogueId.ToString());
```

---

### 2?? �������� include

```cpp
#include "GameEventBusLibrary.h" // For emitting dialogue events
```

---

### 3?? ��������� ����������� ������

**����:** `Plugins/DialogueSystem/Source/DialogueSystemRuntime/DialogueSystemRuntime.Build.cs`

```csharp
PublicDependencyModuleNames.AddRange(
    new string[]
    {
    "Core",
  "CoreUObject",
        "Engine",
        "GameplayTags",
    "Json",
 "JsonUtilities",
        "EnhancedInput",
        "DialogueSystemCore",
        "GameEventBus" // ? ���������!
    }
);
```

---

### 4?? ���������� ��������

**����:**
```cpp
return FDialogueSessionSavingData(); // ? ��������!
```

**�����:**
```cpp
return FDialogueSessionSaveData(); // ? ����������!
```

---

## ?? ��� ����� �������:

### ��� 1: Rebuild Project

**�������� Unreal Editor** � ���������:

```
Build ? Rebuild Solution
```

��� � ��������� ������:

```powershell
& "D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\Build.bat" EchoesOfAffection Win64 Development "D:\Game Dev\Projects\UEProjects\EchoesOfAffection\EchoesOfAffection.uproject" -WaitMutex
```

### ��� 2: �������� ����� � �������� "Talk to NPC"

```
Objective: "Talk to Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    NpcId: Lianne  ? ID NPC �� Interactable Component!
    Count: 1
```

**�����:** `NpcId` ������ ��������� � **ID NPC** � `InteractableComponent`!

### ��� 3: ��������� ���� � ���������� � NPC

---

## ?? ��������� ���� ����� �����������:

```
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Started', NPC=BP_Lianne_C_1
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne' | IntParam=0
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started'
[LogQuestBridge] ?? ForwardToQuestSystem: Converting GameEventPayload to QuestEventPayload...
[LogQuestEventBus] ?? Checking objective 'O_TalkToLianne' (Progress: 0/1, Conditions: 1)
[LogQuestEventBus]   ?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
[LogQuestEventBus] ? ALL CHECKS PASSED - CONDITION MATCHES!
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

---

## ?? ������ �������� ������ ����:

? **"Talk to specific NPC"** � `GameEvent.Dialogue.Started` + `NpcId`

�������:
```
Objective: "Talk to Lianne"
  EventTag: GameEvent.Dialogue.Started
  NpcId: Lianne
  Count: 1

Objective: "Talk to Alice"
  EventTag: GameEvent.Dialogue.Started
  NpcId: Alice
  Count: 1

Objective: "Talk to 3 villagers"
  EventTag: GameEvent.Dialogue.Started
  RequiredTags: NPC.Type.Villager  ? ����� NPC � �����!
  Count: 3
```

---

## ?? �����: NpcId ������ ���������!

� Data Asset ������:
```
NpcId: Lianne
```

� InteractableComponent NPC:
```
InteractionData.ActionText: "Talk"
NPC ID (������ DialogueComponent): "Lianne"  ? ������ ���������!
```

---

## ?? ������ ������� ��������:

| ������� | EventTag | ������ |
|---------|----------|--------|
| Dialogue Started | `GameEvent.Dialogue.Started` | ? **��������!** |
| Dialogue Ended | `GameEvent.Dialogue.Completed` | ? **��������!** |
| Choice Selected | `GameEvent.Dialogue.ChoiceSelected` | ? **��������!** |

---

## ?? ��� dialogue events ��������� �����������!

**������ ������ ��������� �� �����** � ��� ��� ������� ������ ����������� � GameEventBus! ?

---

**DIALOGUE EVENTS ���������!** ?????
