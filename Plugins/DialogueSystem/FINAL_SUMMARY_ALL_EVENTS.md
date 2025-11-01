# ? ��������� ������: ��� DIALOGUE EVENTS ���������!

## ?? ��� �������:

### ? ��������� ������� ��� 3 �������:

1. **GameEvent.Dialogue.Started** � ��� ������ �������
2. **GameEvent.Dialogue.Completed** � ��� ���������� �������
3. **GameEvent.Dialogue.ChoiceSelected** � ��� ������ ��������

---

## ?? ��������� �����:

### 1?? DialogueSubsystem.cpp

**���������:**
- ? ������� `GameEvent.Dialogue.Started` � `StartDialogue()`
- ? ������� `GameEvent.Dialogue.Completed` � `HandleDialogueEnded()`
- ? �������� �� `OnChoiceSelected` � `StartDialogue()`
- ? ����� `HandleChoiceSelected()` � �������� `GameEvent.Dialogue.ChoiceSelected`
- ? ������� �� `OnChoiceSelected` � `ReleaseRunner()`

### 2?? DialogueSubsystem.h

**���������:**
- ? ���������� `void HandleChoiceSelected(int32 ChoiceIndex, UDialogueNode* ChoiceNode);`

### 3?? DialogueSystemRuntime.Build.cs

**��� ��������� �����:**
- ? ����������� `"GameEventBus"`

---

## ?? ������� �������:

### 1. Talk to NPC
```
EventTag: GameEvent.Dialogue.Started
NpcId: Lianne
Count: 1
```

### 2. Complete Dialogue
```
EventTag: GameEvent.Dialogue.Completed
NpcId: Merchant
Count: 1
```

### 3. Make Specific Choice
```
EventTag: GameEvent.Dialogue.ChoiceSelected
DialogNodeId: choice_help_alice
Count: 1
```

### 4. Make Any Choice
```
EventTag: GameEvent.Dialogue.ChoiceSelected
Count: 3
```

---

## ?? ������:

| ������� | ������ |
|---------|--------|
| GameEvent.Dialogue.Started | ? **100%** |
| GameEvent.Dialogue.Completed | ? **100%** |
| GameEvent.Dialogue.ChoiceSelected | ? **100%** |

---

## ?? ��� ������:

1. **Rebuild Solution**
2. **������� Editor**
3. **������� ������**
4. **�����������!**

---

## ?? ������������:

- **������:** `Plugins/DialogueSystem/ALL_DIALOGUE_EVENTS_COMPLETE.md`
- **���������� �������:** `Plugins/QuestSystem/QUEST_CONDITIONS_UPDATED.md`

---

**��� DIALOGUE EVENTS ��������!** ?????
