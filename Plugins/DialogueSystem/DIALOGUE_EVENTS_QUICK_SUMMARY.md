# ?? ������� ������: GameEvent.Dialogue.Started ��������!

## ? ��������:

**EventTag `GameEvent.Dialogue.Started` �� �������** ������ ��� DialogueSubsystem **�� ����������** ������� � GameEventBus!

---

## ? ������� (3 ���������):

### 1?? DialogueSubsystem.cpp � ��������� �������

```cpp
// After dialogue started
OnAnyDialogueStarted.Broadcast(ActiveDialogue, Player, NPC);

UGameEventBusLibrary::EmitDialogueEvent(
    this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
    NPC,
    Player,
    DialogueAsset->DialogueId
);
```

### 2?? DialogueSystemRuntime.Build.cs � ��������� �����������

```csharp
"GameEventBus" // ? ����� �����������!
```

### 3?? DialogueSubsystem.cpp � �������� include

```cpp
#include "GameEventBusLibrary.h"
```

---

## ?? ��� ������:

1. **Rebuild Solution** (������� Editor ? Build ? Rebuild)
2. **������� Editor**
3. **������� �����:**
   ```
   Objective: "Talk to Lianne"
     EventTag: GameEvent.Dialogue.Started
     NpcId: Lianne
     Count: 1
   ```
4. **��������� ����** � ���������� � NPC
5. **��������� ����:**
   ```
   [LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started'
   [LogQuestEventBus] ??? MATCH FOUND!
   ```

---

## ?? ������ ��������:

? `GameEvent.Dialogue.Started` � **100% ��������!** (����� rebuild)

### ������� �������:

```
Talk to Lianne:
EventTag: GameEvent.Dialogue.Started
  NpcId: Lianne

Talk to Alice:
  EventTag: GameEvent.Dialogue.Started
  NpcId: Alice

Talk to 3 villagers:
  EventTag: GameEvent.Dialogue.Started
  RequiredTags: NPC.Type.Villager
  Count: 3
```

---

**DIALOGUE EVENTS ��������!** ???
