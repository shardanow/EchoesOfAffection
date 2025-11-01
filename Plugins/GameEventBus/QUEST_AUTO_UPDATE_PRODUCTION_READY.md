# ? Quest Auto-Update System - Production Implementation

## ?? ������� ������ � �������������!

������� ��������������� ���������� ������� ����� ������� **��������� �����������** � ������ ��� production.

---

## ?? ����������� �������

### ���������� �������

```
GameEventBus (����������� ���� �������)
    ?
QuestEventBridge (���� ����� ���������)
    ?
QuestEventBus (����������� ��� ������� �������)
    ?
QuestEventBus::BroadcastEvent() [����� �����]
    ?
�������������� ���������� Quest Objectives
```

### �������� ���������

| ��������� | ��������� |
|-----------|-----------|
| `QuestEventBus` | �������� ����� `BroadcastEvent()` ��� ��������������� routing ������� |
| `QuestEventBus` | ��������� ������ `DoesConditionMatchEvent()` � `CalculateProgressAmount()` |
| `QuestEventBus` | ��������� ������ �� `QuestProgressManager` |
| `QuestSubsystem` | �������� ����� `EventBus->Initialize(ProgressManager)` |
| `QuestProgressManager` | �������� ��������� ����� `GetQuestAsset()` |
| `QuestEventBridge` | **���˨�** ��������� ���� - ������ ������� ��������� ������� |

---

## ?? ��� ��� ��������

### ����� ����������

1. **������� ������������** (��������, ����� ��������� ������)
```cpp
InventoryComponent->AddItem("Apple", 1);
  ?
InventoryGameEventHelper->EmitItemAcquired("Apple", 1, ...)
  ?
GameEventBus->EmitEvent(Payload with "Inventory.Event.ItemAcquired")
```

2. **QuestEventBridge �������������** (�������� �� "Inventory.Event.*")
```cpp
QuestEventBridge::OnGameEvent(GamePayload)
  ?
QuestEventBridge::ForwardToQuestSystem(GamePayload)
  ?
QuestSubsystem->EmitQuestEvent(QuestPayload)
```

3. **QuestEventBus ������������ �������������**
```cpp
QuestEventBus::EmitEvent(QuestPayload)
  ?
QuestEventBus::BroadcastEvent(QuestPayload) [����� ���!]
  ?
��� ������� ��������� ������:
  ?? �������� ������� ����
  ?? ��� ������ Objective � ����:
  ?   ?? ���������� ���� ��� ���������
  ?   ?? ��� ������� Condition:
  ?       ?? DoesConditionMatchEvent()?
  ?       ?? ���� ��: CalculateProgressAmount()
  ?       ?? UpdateObjectiveProgress(QuestId, ObjectiveId, Amount)
  ?? ����� ������������� �������!
```

### ������ �������� �������

```cpp
bool QuestEventBus::DoesConditionMatchEvent(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload)
{
    // 1) EventTag ������ ���������
    if (!Condition.EventTag.MatchesTag(Payload.EventTag))
        return false;

  // 2) �������� �������� (���� ������):
    
  // ItemId filter
    if (!Condition.ItemId.IsNone() && Condition.ItemId != Payload.StringParam)
  return false;
    
    // NpcId filter
    if (!Condition.NpcId.IsNone() && Condition.NpcId != Payload.StringParam)
        return false;
    
    // AreaId filter
    if (!Condition.AreaId.IsNone() && Condition.AreaId != Payload.StringParam)
        return false;
    
    // ActorClass filter
    if (Condition.ActorClass.IsValid() && !TargetActor->IsA(Condition.ActorClass))
  return false;
    
  // RequiredTags filter
    if (Condition.RequiredTags.Num() > 0 && !Payload.TagsParam.HasAny(Condition.RequiredTags))
        return false;
  
    // ? ��� ������� ������!
    return true;
}
```

---

## ?? ��� ������� ����� ���������

### 1. ������ Quest Data Asset

`Content/Quests/DA_CollectApples.uasset`

**Quest Settings:**
- **Quest ID:** `CollectApples`
- **Title:** `������ ������`
- **Description:** `������ 5 ����� ��� �������`

### 2. ������ Phase

**Phase Settings:**
- **Phase ID:** `Phase_CollectApples`
- **Title:** `���� �����`

### 3. ������ Objective � ����������� Conditions

**Objective Settings:**
- **Objective ID:** `Collect_Apple_Objective`
- **Title:** `������ ������ (0/5)`
- **Style:** `Collect`
- **Counters:**
  - **Target Count:** `5`
  - **Current Count:** `0`

### 4. **�������� ������**: ������� Condition

**������ Condition:**
```
Conditions[0]:
  EventType: OnItemAcquired
  EventTag: "Inventory.Event.ItemAcquired"  ? ��������!
  ItemId: "Apple"? ������: ������ ������
  Count: 1      ? �� ������������ (������ �� �������)
```

---

## ??? �������������� EventTags

| EventTag | ����� �������� | StringParam | IntParam | ������� |
|----------|----------------|-------------|----------|---------|
| `Inventory.Event.ItemAcquired` | �������� ������� | ItemID | Count | ItemId, RequiredTags |
| `Inventory.Event.ItemUsed` | ����������� ������� | ItemID | 1 | ItemId, RequiredTags |
| `Dialogue.Event.NodeReached` | ��������� ������-��� | DialogNodeID | - | DialogNodeId |
| `Dialogue.Event.ChoiceSelected` | ������ ������� | DialogID | - | DialogNodeId, RequiredTags |
| `Time.Event.HourChanged` | ��������� ��� | - | Hour | TimeRangeStart, TimeRangeEnd |
| `Location.Event.Entered` | ����� � ���� | AreaID | - | AreaId, RequiredTags |
| `Combat.Event.EnemyKilled` | ���� ���� | - | 1 | ActorClass, RequiredTags |
| `NPC.Event.AffectionChanged` | ���������� ������������� | NpcID | NewValue | NpcId, ThresholdValue |

---

## ?? ������� �������

### ������ 1: ����� �� ���� ���������

```
Quest: "������ 5 �����"

Objective:
  ObjectiveId: "Collect_Apple"
  Counters:
    TargetCount: 5
  
  Conditions[0]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Apple"
```

**���������:**
- ����� ��������� 1 ������ ? +1 �������� (1/5)
- ����� ��������� 3 ������ ? +3 �������� (4/5)
- ����� ��������� 1 ������ ? +1 �������� (5/5) ? **Objective Complete!**

---

### ������ 2: ����� �� ������

```
Quest: "�������� � �����"

Objective:
  ObjectiveId: "Talk_To_Anna"
  Counters:
    TargetCount: 1
  
  Conditions[0]:
    EventTag: "Dialogue.Event.NodeReached"
    DialogNodeId: "Anna_Intro_End"
```

**���������:**
- ����� �������� ������ ? �� ���������
- ����� ������� �� ���� "Anna_Intro_End" ? **Objective Complete!**

---

### ������ 3: ����� �� �������� ������

```
Quest: "���� 10 ������"

Objective:
  ObjectiveId: "Kill_Wolves"
  Counters:
    TargetCount: 10
  
  Conditions[0]:
    EventTag: "Combat.Event.EnemyKilled"
    ActorClass: "/Game/Enemies/BP_Wolf.BP_Wolf_C"
```

**���������:**
- ����� ������� ����� ? +1 (1/10)
- ����� ������� ������� ? �� ��������� (������ �����)
- ����� ������� ��� 9 ������ ? **Objective Complete!**

---

### ������ 4: ����� � ����������� ��������� (AND)

```
Quest: "������ ������ � �����"

Objective:
  ObjectiveId: "Collect_Fruits"
  Logic: And  ? ��� ������� ������ ���� ���������
  
  Conditions[0]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Apple"
    Count: 3
  
  Conditions[1]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Pear"
    Count: 2
```

**���������:**
- ������ 3 ������ ? Condition[0] ���������, �� Objective �� ���������
- ������ 2 ����� ? Condition[1] ��������� ? **Objective Complete!** (��� ������� ���������)

---

### ������ 5: ����� � �������������� (OR)

```
Quest: "������� �����"

Objective:
  ObjectiveId: "Bring_Any_Fruit"
  Logic: Or  ? ����� ������� ��������� objective
  
  Conditions[0]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Apple"
  
  Conditions[1]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Pear"

  Conditions[2]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Orange"
```

**���������:**
- �������� ������ ? **Objective Complete!** (OR ������)
- ��� �������� ����� ? **Objective Complete!**
- ��� �������� �������� ? **Objective Complete!**

---

## ?? ������ ������

### 1. EventTag ����������!

```diff
? �����������:
Conditions[0]:
  EventType: OnItemAcquired
  ItemId: "Apple"
  # EventTag �� ��������!

? ���������:
Conditions[0]:
  EventType: OnItemAcquired
  EventTag: "Inventory.Event.ItemAcquired"  ? �����������!
  ItemId: "Apple"
```

### 2. ������� �����������

���� ItemId �� ������ ? ������� ��������� ��� **������** ��������!

```cpp
Conditions[0]:
  EventTag: "Inventory.Event.ItemAcquired"
  # ItemId �� ��������

? ������ ��� ������: ������, �����, �����, ����...
```

### 3. TargetCount vs Count � Condition

- **Objective.Counters.TargetCount** = ������� ����� ������� **�����**
- **Condition.Count** = �� ������������ (��� �������������)

�������� ������ �� `Payload.IntParam` (���������� � �������).

### 4. Multiple Conditions � ����� Objective

��� **Logic = And:**
- ��� conditions ������ ���� ���������

��� **Logic = Or:**
- ����� condition ��������� objective

��� **Logic = Count:**
- ����� ��������� N conditions �� ������

---

## ?? ������������

### Console Commands

```
# ��������� �����
Quest.Start CollectApples

# ��������� ������
Quest.Info CollectApples

# �������� ������� (��������� �������)
Inventory.AddItem Apple 1

# ��������� ��������
Quest.Objective.Info CollectApples Collect_Apple_Objective
```

### ��������� ����

```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | Apple | Count: 1
LogQuestEventBus: Processing event 'Inventory.Event.ItemAcquired' for 1 active quests
LogQuestEventBus: Event 'Inventory.Event.ItemAcquired' matched objective 'Collect_Apple_Objective' in quest 'CollectApples' - Adding 1 progress
LogQuestProgressManager: Objective progress updated: CollectApples / Collect_Apple_Objective (1/5)
```

---

## ?? Production-Ready Features

### ? ��� ��������

- ? �������������� ���������� ������� ��� ��������
- ? ���������� �� ItemId, NpcId, AreaId, ActorClass
- ? ��������� RequiredTags
- ? AND/OR/Count ������ ��� Objectives
- ? ������������� Conditions �� Objective
- ? �������������� ������ ��������� �� IntParam
- ? ��������� ���� ������: Inventory, Dialogue, Time, Location, Combat
- ? ��������������: ����������� ������ �������� ������
- ? ������� ����������� objectives

### ?? ������������������

- **������:** ������������ ������ �������� ������
- **����:** ���������� completed objectives
- **����������:** ������� ����� EventTag + ���������
- **�������������:** �� 100+ �������� ������� ��� �������

---

## ?? API Reference

### QuestEventBus

```cpp
// ������������� (���������� ������������� QuestSubsystem)
void Initialize(UQuestProgressManager* InProgressManager);

// ���� ������� (��������� �� GameEventBridge)
void EmitEvent(const FQuestEventPayload& Payload);

// �������������� ������������� (���������, ���������� �� EmitEvent)
void BroadcastEvent(const FQuestEventPayload& Payload);

// �������� ���������� condition � ��������
bool DoesConditionMatchEvent(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const;

// ������ ��������� �� �������
int32 CalculateProgressAmount(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const;
```

---

## ?? Troubleshooting

### ����� �� �����������?

1. **������� EventTag:**
```
EventTag ������ ���� �������� � ��������� � ��������!
```

2. **������� �������:**
```
ItemId/NpcId/AreaId ������ ��������� � Payload.StringParam
```

3. **������� ����:**
```
LogQuestEventBus: Processing event...
LogQuestEventBus: Event matched objective...
```

4. **������� ����� �������:**
```
Quest.Info <QuestId>
State ������ ����: Active
```

### Objective ����������� ��������� ���?

������� ��� Condition.ItemId **��������**!

����� ����� ItemAcquired ����� ����������:
```cpp
Condition.ItemId = None ? ����������� ��� ���� ���������!
```

---

## ?? ����

**������� ��������� ������ � �������������!**

������ ������ ������������� ����������� ��� �������� ��� ������� ����.

**������ ������ Quest Asset � ����������� Conditions ? �� ��������!**

**Enjoy automatic quest updates! ?????**
