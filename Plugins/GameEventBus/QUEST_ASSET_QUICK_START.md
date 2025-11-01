# ?? Quick Start: �������� Quest Asset ��� ����-����������

## ��� 1: ������ Quest Data Asset

1. � Content Browser:
   - **Right Click** ? **Miscellaneous** ? **Data Asset**
   - ������ **QuestAsset**
   - ������: `DA_CollectApples`

2. ������ `DA_CollectApples`

## ��� 2: ������� �������� ������

**General Settings:**
```
Quest ID: CollectApples
Title: "������ ������"
Description: "������ 5 ����� ��� ������� �����"
```

**Quest Settings:**
```
Start Policy:
  Policy Type: Manual  (������ ����� �������)

Failure Policy:
  Failure Type: None
```

## ��� 3: ������ Phase

**Phases** ? **Add Element [0]:**
```
Phase ID: Phase_Collect
Title: "���� �����"
Description: "����� � ������ 5 �����"
bRequireAllObjectives: true
```

## ��� 4: ������ Objective

**Phases[0] ? Objectives** ? **Add Element [0]:**

```
Objective ID: Collect_Apple_Objective
Title: "������ ������"
Description: "������ 5 ����� (0/5)"

Style: Collect

Logic: And

Counters:
  Target Count: 5
  Current Count: 0

bIsOptional: false
bIsHidden: false
bTrackOnHUD: true
```

## ��� 5: ?? �������� ������ - ������� Condition!

**Objectives[0] ? Conditions** ? **Add Element [0]:**

```
EventType: OnItemAcquired

EventTag: Inventory.Event.ItemAcquired  ? ��������! ������ ��������� � ����������� ��������

ItemId: Apple  ? ������: ������ ������

Count: 1  ? �� ������������ (�������� �� �������)

NpcId: None
AreaId: None
ActorClass: None
RequiredTags: []
```

## ��� 6: ������� � �����������

### � ���������:

1. **Play (PIE)**
2. ������ �������: **`~`**
3. ������ ����:
```
GameEventBus.SetVerbose 1
```
4. ������� �����:
```
Quest.Start CollectApples
```
5. ������� ������ (����� `E` �� ������)

### ��������� ��������� � �����:

```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | Apple | Count: 1
LogQuestEventBus: Processing event 'Inventory.Event.ItemAcquired' for 1 active quests
LogQuestEventBus: Event 'Inventory.Event.ItemAcquired' matched objective 'Collect_Apple_Objective' in quest 'CollectApples' - Adding 1 progress
LogQuestProgressManager: Objective progress updated: CollectApples / Collect_Apple_Objective (1/5)
```

### Quest Tracker Widget ������ ��������:

```
???????????????????????????????????
?  ������ ������            ?
?         ?
?  Collect apples (1/5)        ?  ? ���������� �������������!
?   ?
?  ????????????????????? 20%      ?
???????????????????????????????????
```

---

## ?? ������ �������

### ? ����������� ������� EventTag!

```
EventTag: Inventory.Event.ItemAcquired
```

��� ����� ������� �� ����� ��������!

### ? ItemId = ������ (�����������)

���� **ItemId = None** ? ����� ������� **���** ��������!

���� **ItemId = "Apple"** ? ������ ������.

### ? Target Count = ����

```
Target Count: 5  ? ����� ������� 5 �����
```

### ? Logic = ������ ����������

- **And** = ��� conditions ������ ���� ���������
- **Or** = ����� condition ��������� objective
- **Count** = N �� M conditions

---

## ?? �������� �������

```bash
# ��������� �����
Quest.Start CollectApples

# ��������� ������ ������
Quest.Info CollectApples

# �������� ������ ������� (��������� �������)
Inventory.AddItem Apple 1

# ��������� �������� objective
Quest.Objective.Info CollectApples Collect_Apple_Objective

# ��������� ����� ������� (��� �����)
Quest.Complete CollectApples

# ���������� ��� �������� ������
Quest.List Active
```

---

## ?? Checklist ��� ��������

- [ ] Quest ID ��������
- [ ] Phase �������
- [ ] Objective �������
- [ ] **EventTag ��������!** (`Inventory.Event.ItemAcquired`)
- [ ] ItemId = `Apple` (��� ����������)
- [ ] Target Count = `5`
- [ ] ����� ������� ����� `Quest.Start CollectApples`
- [ ] ��������� ������
- [ ] �������� ��������� �������������!

---

## ?? ��������� ����

����� ���� ��� ������� ����� ��������, ��������:

1. **����� � ����������� ���������:**
   - ������ 3 ������ **�** 2 �����
   - Logic = And
   - 2 Conditions

2. **����� � ��������������:**
   - ������� **�����** �����
   - Logic = Or
   - 3 Conditions (Apple, Pear, Orange)

3. **���������� �����:**
   - EventTag: `Dialogue.Event.NodeReached`
   - DialogNodeId: `Anna_Intro_End`

4. **����� �� ��������:**
   - EventTag: `Combat.Event.EnemyKilled`
   - ActorClass: `BP_Wolf`

---

## ?? Troubleshooting

### ����� �� �����������?

1. ������� ����:
```
LogQuestEventBus: Processing event...
```

���� ��� ? EventTag ������������!

2. ������� ItemId:
```
ItemId � Condition ������ ��������� � ID ��������!
```

3. ������� ����� �������:
```
Quest.Info CollectApples
? State: Active
```

### ����������� ��� ���� ���������?

**ItemId = None** ? ����������� ��� ����!

������� ItemId = `Apple`

---

## ? ������!

������ � ���� ���� ������� Quest Asset � �������������� �����������!

**������ �������� ������ ? ����� ����������� ���!** ???
