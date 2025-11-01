# ? ��� DIALOGUE EVENTS ���������!

## ?? ��� ���������:

### 1?? GameEvent.Dialogue.Started ?

**�����:** ��� ������ �������  
**���:** `DialogueSubsystem::StartDialogue()`

```cpp
// After dialogue started
OnAnyDialogueStarted.Broadcast(ActiveDialogue, Player, NPC);

UGameEventBusLibrary::EmitDialogueEvent(
    this,
  FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
    NPC, // NPC actor
    Player, // Player actor
 DialogueAsset->DialogueId // Dialogue ID
);
```

---

### 2?? GameEvent.Dialogue.Completed ?

**�����:** ��� ���������� �������  
**���:** `DialogueSubsystem::HandleDialogueEnded()`

```cpp
// Before cleanup
OnAnyDialogueEnded.Broadcast(CurrentPlayer, CurrentNPC);

// Emit completion event
if (CurrentNPC && CurrentPlayer)
{
    UGameEventBusLibrary::EmitDialogueEvent(
    this,
     FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Completed")),
        CurrentNPC,
   CurrentPlayer,
        DialogueId
    );
}
```

---

### 3?? GameEvent.Dialogue.ChoiceSelected ?

**�����:** ��� ������ �������� ������  
**���:** `DialogueSubsystem::HandleChoiceSelected()` (����� �����!)

**��������� �������� � StartDialogue:**
```cpp
ActiveDialogue->OnChoiceSelected.AddDynamic(this, &UDialogueSubsystem::HandleChoiceSelected);
```

**����� ����������:**
```cpp
void UDialogueSubsystem::HandleChoiceSelected(int32 ChoiceIndex, UDialogueNode* ChoiceNode)
{
  if (CurrentNPC && CurrentPlayer && ChoiceNode)
    {
        UGameEventBusLibrary::EmitDialogueEvent(
            this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.ChoiceSelected")),
   CurrentNPC,
     CurrentPlayer,
        ChoiceNode->NodeId // Choice NodeId as StringParam
        );
    }
}
```

**������� � ReleaseRunner:**
```cpp
Runner->OnChoiceSelected.RemoveDynamic(this, &UDialogueSubsystem::HandleChoiceSelected);
```

---

## ?? ��������� �����:

### 1. DialogueSubsystem.cpp

- ? ��������� ������� `GameEvent.Dialogue.Started` � `StartDialogue()`
- ? ��������� ������� `GameEvent.Dialogue.Completed` � `HandleDialogueEnded()`
- ? ��������� �������� �� `OnChoiceSelected` � `StartDialogue()`
- ? �������� ����� `HandleChoiceSelected()` � �������� `GameEvent.Dialogue.ChoiceSelected`
- ? ��������� ������� �� `OnChoiceSelected` � `ReleaseRunner()`

### 2. DialogueSubsystem.h

- ? ��������� ���������� `void HandleChoiceSelected(int32 ChoiceIndex, UDialogueNode* ChoiceNode);`

### 3. DialogueSystemRuntime.Build.cs

- ? ��������� ����������� `"GameEventBus"`

---

## ?? ������� �������:

### Talk to NPC
```
Objective: "Talk to Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    NpcId: Lianne
    Count: 1
```

### Complete Dialogue
```
Objective: "Finish conversation with merchant"
  Condition:
    EventTag: GameEvent.Dialogue.Completed
    NpcId: Merchant
    Count: 1
```

### Make Specific Choice
```
Objective: "Agree to help Alice"
  Condition:
    EventTag: GameEvent.Dialogue.ChoiceSelected
    DialogNodeId: choice_help_alice  ? NodeId ���������� ��������!
    Count: 1
```

### Make Any Choice in Dialogue
```
Objective: "Make 3 choices in any dialogue"
  Condition:
    EventTag: GameEvent.Dialogue.ChoiceSelected
    Count: 3
```

---

## ?? ��������� ����:

### ��� ������ �������:
```
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Started'
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='lianne_greeting'
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

### ��� ���������� �������:
```
[LogDialogueSubsystem] Dialogue ended callback
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Completed'
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Completed' | StringParam='lianne_greeting'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

### ��� ������ ��������:
```
[LogDialogueSubsystem] Choice selected - NodeId='choice_help', Index=0
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.ChoiceSelected'
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.ChoiceSelected' | StringParam='choice_help'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

---

## ?? ��� ������:

1. **Rebuild Solution** (������� Editor ? Build ? Rebuild)
2. **������� Editor**
3. **������� ������** � ������ ���������
4. **��������� ����** � �������������� ��� �������
5. **��������� ����** � ������ ���� ������� ���� ��� �������!

---

## ?? ��������� ������:

| ������� | EventTag | ������ | ���������� |
|---------|----------|--------|------------|
| Started | `GameEvent.Dialogue.Started` | ? **100%** | ��������! |
| Completed | `GameEvent.Dialogue.Completed` | ? **100%** | ��������! |
| ChoiceSelected | `GameEvent.Dialogue.ChoiceSelected` | ? **100%** | ��������! |

---

## ?? ��������� �������:

### GameEvent.Dialogue.Started
- **NPC Actor:** CurrentNPC
- **Player Actor:** CurrentPlayer
- **StringParam:** DialogueAsset->DialogueId
- **������� ������:** `NpcId` (ID NPC)

### GameEvent.Dialogue.Completed
- **NPC Actor:** CurrentNPC
- **Player Actor:** CurrentPlayer
- **StringParam:** DialogueId
- **������� ������:** `NpcId` (ID NPC)

### GameEvent.Dialogue.ChoiceSelected
- **NPC Actor:** CurrentNPC
- **Player Actor:** CurrentPlayer
- **StringParam:** ChoiceNode->NodeId
- **������� ������:** `DialogNodeId` (ID ���������� choice node)

---

## ?? �����:

### NpcId ������ ���������:

**� ������:**
```
NpcId: Lianne
```

**� DialogueComponent NPC:**
```
NPC ID: "Lianne"  ? ���������!
```

### DialogNodeId ��� �������:

**� ������:**
```
DialogNodeId: choice_help_alice
```

**� Dialogue Data Asset:**
```
Choice Node:
  NodeId: choice_help_alice  ? ���������!
```

---

**��� DIALOGUE EVENTS ��������!** ?????
