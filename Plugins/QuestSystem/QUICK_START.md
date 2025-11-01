# ?? Quest System - Quick Start Guide

## ? ������� ����� �� 5 �����

### ��� 1: ������� ����� (1 ���)

```
Content Browser ? Right Click ? Quest System ? Quest Asset
���: DA_Quest_MyFirstQuest
```

**���������:**
```
QuestId: Q_MyFirstQuest
QuestName: "My First Quest"
QuestDescription: "Learn how quests work!"
```

### ��� 2: �������� Objective (2 ���)

```
Objectives ? Add Element
?? ObjectiveId: O_TalkToNPC
?? Description: "Talk to Lianne"
?? RequiredCount: 1
?? Conditions ? Add Element
     ?? EventTag: GameEvent.Dialogue.Started
     ?? NpcId: Lianne
```

### ��� 3: ��������� NPC (1 ���)

```
BP_Lianne (� ��������� ������):
?? Details ? Tags ? Gameplay Tags: NPC.Lianne  ? �����������!
```

### ��� 4: �������� ����� NPC (1 ���)

```
BP_Lianne:
?? Add Component ? QuestGiverComponent
     ?? bAutoAssignNpcId: true
     ?? AvailableQuests ? Add Element: DA_Quest_MyFirstQuest
```

### ��� 5: ����! (30 ���)

```
1. Play (PIE)
2. ������� � Lianne (������ ! ��� �������)
3. �������� E - ������ �����
4. ���������� � Lianne
5. ? ����� ��������!
```

---

## ?? ��������������� ���� �������

### 1?? ������� ��������

```
DA_Quest_CollectApples:
  ?? Objective: "Collect 5 apples"
       ?? Condition:
       ?? EventTag: GameEvent.Inventory.ItemAcquired
            ?? ItemId: RedApple
   ?? RequiredCount: 5
```

### 2?? ���������� � NPC

```
DA_Quest_TalkToLianne:
  ?? Objective: "Talk to Lianne"
     ?? Condition:
      ?? EventTag: GameEvent.Dialogue.Started
 ?? NpcId: Lianne
      ?? RequiredCount: 1
```

### 3?? ������������ �������

```
DA_Quest_UsePotion:
  ?? Objective: "Use a health potion"
       ?? Condition:
     ?? EventTag: GameEvent.Inventory.ItemUsed
   ?? ItemId: HealthPotion
            ?? RequiredCount: 1
```

### 4?? ������� �������

```
DA_Quest_SecondQuest:
  ?? Dependencies: [Q_MyFirstQuest]  ? ������� ����� ���������!
  ?? Objective: "Complete follow-up task"
```

### 5?? ������� ���� �������

```
DA_Quest_MakeChoice:
  ?? Objective: "Reach dialogue node"
       ?? Condition:
        ?? EventTag: GameEvent.Dialogue.NodeReached
            ?? NpcId: Lianne
         ?? RequiredCount: 1
```

### 6?? ��������� ������ �����

```
DA_Quest_MetaQuest:
  ?? Objective: "Complete any quest"
       ?? Condition:
    ?? EventTag: GameEvent.Quest.Completed
          ?? RequiredCount: 1
```

---

## ?? ������� �������

### �� ������� ���������

| ���� | EventTag | ������ |
|------|----------|--------|
| ��������� ������ | `GameEvent.Inventory.ItemAcquired` | `ItemId: RedApple` |
| ������������ ����� | `GameEvent.Inventory.ItemUsed` | `ItemId: HealthPotion` |
| ��������� ��� | `GameEvent.Inventory.ItemCrafted` | `ItemId: IronSword` |
| �������� ������ | `GameEvent.Inventory.ItemGifted` | `ItemId: Rose` |

### �� ������� �������

| ���� | EventTag | ������ |
|------|----------|--------|
| ���������� � Lianne | `GameEvent.Dialogue.Started` | `NpcId: Lianne` |
| ������� ���� | `GameEvent.Dialogue.NodeReached` | `NpcId: Alice` |
| ������� ����� | `GameEvent.Dialogue.ChoiceMade` | `NpcId: Merchant` |

### �� ������� ������

| ���� | EventTag | ������ |
|------|----------|--------|
| ������ ����� | `GameEvent.Quest.Started` | `QuestId: Q_Tutorial` |
| ��������� ����� | `GameEvent.Quest.Completed` | `QuestId: Q_FirstQuest` |
| ��������� objective | `GameEvent.Quest.ObjectiveCompleted` | - |

### �� ������� �������

| ���� | EventTag | ������ |
|------|----------|--------|
| ��������� ���� | `GameEvent.Time.HourChanged` | `RequiredCount: 5` |
| ��������� ��� | `GameEvent.Time.DayChanged` | `RequiredCount: 2` |
| ��������� �������� | `GameEvent.Time.Sunrise` | `RequiredCount: 1` |

---

## ?? ��������� NPC

### ������� �: � GameplayTags (�������������!)

```
BP_Lianne:
  ?? Gameplay Tags: NPC.Lianne  ? ���������� "Lianne"
  ?? QuestGiverComponent:
       ?? bAutoAssignNpcId: true
       ?? AvailableQuests: [DA_Quest_TalkToLianne]
       ?? QuestAvailableIcon: Icon_YellowExclamation
       ?? QuestInProgressIcon: Icon_GrayQuestion
  ?? QuestCompleteIcon: Icon_GoldCheck
```

### ������� �: ��� ����� (fallback)

```
BP_Lianne_C_1:  ? ��� ������ �����!
  ?? QuestGiverComponent:
       ?? (�� �� �� �����)

���������: "BP_Lianne_C_1" ? "Lianne" (��������)
```

---

## ?? UI ����������

### Quest Tracker Widget

**������������� ����������� �������� ������:**

```cpp
// �������� � HUD Blueprint
UQuestTrackerWidget* QuestTracker = CreateWidget<UQuestTrackerWidget>(this, QuestTrackerClass);
QuestTracker->AddToViewport();

// Widget �������������:
// ? ������������� �� OnQuestProgressChanged
// ? ��������� ������ objectives
// ? ���������� �������� (X/Y)
// ? ����������, ����� ��� �������
```

### Quest Giver ������

**��� ������� NPC (Widget Component):**

```cpp
// Blueprint Tick
void UpdateQuestIcon()
{
    UQuestGiverComponent* QuestGiver = NPC->FindComponentByClass<UQuestGiverComponent>();
    UTexture2D* Icon = QuestGiver->GetQuestIndicatorIcon(PlayerController);
    
 if (Icon)
    {
        IconWidget->SetBrushFromTexture(Icon);
     IconWidget->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
 IconWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}
```

**������:**
- `!` (�����) - ����� ����� ��������
- `?` (�����) - ����� �������
- `?` (�������) - ����� ����� �����

---

## ?? �������

### �������� ����

```ini
// Config/DefaultEngine.ini
[Core.Log]
LogGameEventBus=Verbose
LogQuestEventBus=Verbose
LogQuestBridge=Verbose
LogInventoryEvents=Verbose
LogDialogueSubsystem=Verbose
```

### ��������� � �����

**? ���������� �����:**

```
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] ?? Called! EventTag='GameEvent.Dialogue.Started', NPC=BP_Lianne_C_1
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne'
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started' | StringParam='Lianne'
[LogQuestEventBus] ?? Checking objective 'O_TalkToLianne'
[LogQuestEventBus] ? CONDITION MATCH! StringParam='Lianne' == NpcId='Lianne'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

**? �������� ������:**

```
// ������ 1: EventTag �� ���������
[LogQuestEventBus] ? EventTag mismatch
�������: ��������� ������ ���������� EventTag

// ������ 2: NPC ID �� ���������
[LogQuestEventBus] ? NPC ID mismatch: Expected 'Lianne', got 'lianne_greeting'
�������: �������� GameplayTag "NPC.Lianne"

// ������ 3: ������� �� ��������
(��� ����� GameEventBus)
�������: ���������, ��� GameEventBus plugin �������
```

---

## ?? ������ ������

| �������� | ������� | ������� |
|----------|---------|---------|
| ����� �� ���������� | Dependencies �� ��������� | ��������� ����������� |
| ������� �� ����������� | EventTag �� ��������� | ������ ����������! |
| NPC ID ������������ | ��� GameplayTag | �������� `NPC.{Name}` |
| UI �� ����������� | ������ �� �������� | ��������� �������� |
| ������� �� �������� | ������ �������� | �������� GameEventBus |
| ������ �� ���������� | QuestGiver �� �������� | �������� ��������� |

---

## ?? ������� ����� ������

```
? Quest Asset ������
   ?? QuestId ��������
   ?? QuestName ��������
   ?? Objectives ���������

? Objectives ���������
   ?? ObjectiveId ��������
   ?? Description ���������
   ?? RequiredCount > 0
   ?? Conditions ���������

? Conditions ���������
   ?? EventTag ������ �� ������ (������ ����������!)
   ?? ������� ��������� (ItemId/NpcId/QuestId)

? NPC ��������
   ?? GameplayTag ��������: NPC.{Name}
   ?? QuestGiverComponent ��������
    ?? AvailableQuests �������� �����

? ������� ��������
   ?? GameEventBus ?
   ?? QuestSystem ?
   ?? InventorySystem ? (���� ������������)
   ?? DialogueSystem ? (���� ������������)

? ���� �������� (��� �������)
   ?? DefaultEngine.ini ��������
```

---

## ?? ��������� ����

### ���������� � �������:
1. **NPC_ID_COMPLETE_GUIDE.md** - �� ��� NPC ID
2. **QUEST_CONDITIONS_COMPLETE_REFERENCE.md** - ��� ���� �������
3. **QUEST_GIVER_COMPONENT_EXPLAINED.md** - UI ���������
4. **VISUAL_FLOW_DIAGRAM.md** - ���������� �����
5. **README_FINAL.md** - ������ ������������

### ������� �������:
- `DA_Quest_CollectApples` - ������� ��������
- `DA_Quest_TalkToLianne` - ���������� � NPC
- `DA_Quest_ChainedQuest` - ������� �������

---

## ?? �������� ������

1. **������ ����������� GameplayTags** ��� NPC - ��� ������
2. **EventTag ������ ����� ���������** - ��������� �� ������!
3. **RequiredCount > 0** - ����� objective ����� ����������
4. **���������� � ������** - Verbose ����� ��� ����
5. **���� ����� �� ���** - ������� �������, ����� �������

---

**������! ������ �������� ���� ������ �����! ??**

**����� �� ��������:** ~5 �����  
**���������:** ˸����  
**���������:** ���������� �����! ?
