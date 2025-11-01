# ?? ������ ���������� �� Objective Conditions (������� �������)

## ?? ��� ����� Objective Condition?

**Objective Condition** � ��� ������� ������� ���������� **����� � ���** objective (������) ������ �������� ��������.

������ Objective ����� ����� **��������� �������** (Conditions), � �������� ������������� ����� **���� �� ���� �������** ���������.

---

## ?? �������� Condition

������ ������� ������� ��:

### 1?? EventTag (�����������!)

**������� ����** � ���������� **��� �������** ������� ����� �����������:

```
EventTag: GameEvent.Inventory.ItemAcquired
```

**��� EventTag ������� �� ��������!**

### 2?? ������� (�����������)

�������������� ������� �������� **��� ������** ������ ���������:

- `ItemId` � ���������� �������
- `NpcId` � ���������� NPC
- `AreaId` � ���������� ����
- `RequiredTags` � ������ ����
- `Count` � ������� ��� ������ ���������

---

## ?? ��� ��������� �������

### ?? 1. Inventory Events (������� ���������)

#### ? Item Acquired � ��������� �������

**EventTag:** `GameEvent.Inventory.ItemAcquired`

**����� �����������:** ����� ��������/������� �������

**������:**
```
Objective: "Collect 5 apples"
  Condition:
  EventTag: GameEvent.Inventory.ItemAcquired
    ItemId: RedApple
    Count: 5
```

**��� �����������:**
- ? `EventTag` ���������
- ? `ItemId` ��������� (���� ������)
- ? �������� +1 �� ������ ����������� �������

---

#### ? Item Used � ������������ �������

**EventTag:** `GameEvent.Inventory.ItemUsed`

**����� �����������:** ����� �����������/����/�������� �������

**������:**
```
Objective: "Eat 2 apples"
  Condition:
    EventTag: GameEvent.Inventory.ItemUsed
    ItemId: RedApple
    Count: 2
```

**��� �����������:**
- ? `EventTag` ���������
- ? `ItemId` ��������� (���� ������)
- ? �������� +1 �� ������ �������������

---

#### ? Item Crafted � ��������� �������

**EventTag:** `GameEvent.Inventory.ItemCrafted`

**����� �����������:** ����� �������� �������

**������:**
```
Objective: "Craft 3 healing potions"
  Condition:
  EventTag: GameEvent.Inventory.ItemCrafted
    ItemId: HealingPotion
    Count: 3
```

---

#### ? Item Gifted � �������� ������� NPC

**EventTag:** `GameEvent.Inventory.ItemGifted`

**����� �����������:** ����� ������� ������� NPC

**������:**
```
Objective: "Give flowers to Alice"
  Condition:
    EventTag: GameEvent.Inventory.ItemGifted
    ItemId: RedRose
    NpcId: Alice
    Count: 1
```

**��� �����������:**
- ? `EventTag` ���������
- ? `ItemId` ���������
- ? **`NpcId` ���������** (�������� ����������!)

---

#### ? Item Dropped � ��������� �������

**EventTag:** `GameEvent.Inventory.ItemDropped`

**����� �����������:** ����� �������� �������

**������:**
```
Objective: "Drop 5 trash items"
  Condition:
    EventTag: GameEvent.Inventory.ItemDropped
    RequiredTags: Item.Type.Trash
    Count: 5
```

---

### ?? 2. Dialogue Events (������� ��������)

#### ? NPC Talked � ���������� � NPC

**EventTag:** `GameEvent.Dialogue.Started`

**����� �����������:** ����� ����� ������ � NPC

**������:**
```
Objective: "Talk to Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    NpcId: Lianne
    Count: 1
```

**��� �����������:**
- ? `EventTag` ���������
- ? **`NpcId` ���������** (�������� � ��� �������!)

---

#### ? Dialog Choice � ������� ������� � �������

**EventTag:** `GameEvent.Dialogue.ChoiceSelected`

**����� �����������:** ����� ������ ���������� ������� ������

**������:**
```
Objective: "Agree to help Alice"
  Condition:
    EventTag: GameEvent.Dialogue.ChoiceSelected
    DialogNodeId: Choice_HelpAlice
    RequiredTags: Dialogue.Choice.Agree
    Count: 1
```

**��� �����������:**
- ? `EventTag` ���������
- ? `DialogNodeId` ��������� (���� ������)
- ? `RequiredTags` ����������� (��������, ��� ������)

---

#### ? Dialogue Completed � ��������� ������

**EventTag:** `GameEvent.Dialogue.Completed`

**����� �����������:** ������ ��������

**������:**
```
Objective: "Finish conversation with merchant"
  Condition:
    EventTag: GameEvent.Dialogue.Completed
    NpcId: Merchant
    Count: 1
```

---

### ?? 3. Quest Events (������� �������)

#### ? Quest Started � ������ �����

**EventTag:** `GameEvent.Quest.Started`

**����� �����������:** ����� ����� ������ �����

**������:**
```
Objective: "Start 3 side quests"
  Condition:
    EventTag: GameEvent.Quest.Started
    RequiredTags: Quest.Type.SideQuest
    Count: 3
```

---

#### ? Quest Completed � ��������� �����

**EventTag:** `GameEvent.Quest.Completed`

**����� �����������:** ����� �������� ������ �����

**������:**
```
Objective: "Complete 'Collect Apples' quest"
  Condition:
    EventTag: GameEvent.Quest.Completed
    NpcId: Q_CollectApples  // ID ������ � NpcId
    Count: 1
```

---

### ??? 4. Location Events (������� �������)

#### ? Enter Area � ����� � ����

**EventTag:** `GameEvent.Location.EnterArea`

**����� �����������:** ����� ����� � �������-����

**������:**
```
Objective: "Visit the library"
  Condition:
    EventTag: GameEvent.Location.EnterArea
    AreaId: Library
    Count: 1
```

**��� �����������:**
- ? `EventTag` ���������
- ? **`AreaId` ���������** (ID ����!)

---

#### ? Leave Area � �������� ����

**EventTag:** `GameEvent.Location.LeaveArea`

**����� �����������:** ����� ����� �� �������-����

**������:**
```
Objective: "Leave the dungeon"
  Condition:
    EventTag: GameEvent.Location.LeaveArea
    AreaId: Dungeon
    Count: 1
```

---

### ? 5. Time Events (������� �������)

#### ? Time Reached � ��������� �������

**EventTag:** `GameEvent.Time.Reached`

**����� �����������:** ��������� ����������� ����� �����

**������:**
```
Objective: "Wait until midnight"
  Condition:
 EventTag: GameEvent.Time.Reached
    TimeRangeStart: 0  // 00:00
    TimeRangeEnd: 1    // 01:00
    Count: 1
```

**����:**
- `TimeRangeStart` � ������ ��������� (���� 0-24)
- `TimeRangeEnd` � ����� ��������� (���� 0-24)

---

#### ? Day Changed � ����� ���

**EventTag:** `GameEvent.Time.DayChanged`

**����� �����������:** �������� ����� ����

**������:**
```
Objective: "Wait 3 days"
  Condition:
    EventTag: GameEvent.Time.DayChanged
    Count: 3
```

---

### ??? 6. Weather Events (������� ������)

#### ? Weather Changed � ����� ������

**EventTag:** `GameEvent.Weather.Changed`

**����� �����������:** ������ ����������

**������:**
```
Objective: "Wait for rain"
  Condition:
    EventTag: GameEvent.Weather.Changed
    WeatherTags: Weather.Rain
    Count: 1
```

**����:**
- `WeatherTags` � ������ ���� ������

---

### ?? 7. Affection Events (������� ���������)

#### ? Affection Changed � ��������� �������������

**EventTag:** `GameEvent.Affection.Changed`

**����� �����������:** ������������� � NPC ����������

**������:**
```
Objective: "Reach 50% affection with Alice"
  Condition:
    EventTag: GameEvent.Affection.Changed
    NpcId: Alice
    ThresholdValue: 0.5  // 50%
    Count: 1
```

**����:**
- `NpcId` � ID NPC
- `ThresholdValue` � ����������� �������� (0.0 - 1.0)

---

### ?? 8. Needs Events (������� ������������)

#### ? Need Changed � ��������� �����������

**EventTag:** `GameEvent.Need.Changed`

**����� �����������:** ����������� (�����, �������, � �.�.) ����������

**������:**
```
Objective: "Restore hunger to 80%"
  Condition:
    EventTag: GameEvent.Need.Changed
    RequiredTags: Need.Hunger
 ThresholdValue: 0.8  // 80%
    Count: 1
```

**����:**
- `RequiredTags` � ��� ����������� (Need.Hunger, Need.Energy)
- `ThresholdValue` � ������� �������� (0.0 - 1.0)

---

## ?? ��� �������� �������� �������?

����� ������� ����������, ������� ���������:

### 1?? EventTag (�����������)

```cpp
if (Condition.EventTag != Payload.EventTag)
{
    return false; // �� ��������� � ���������
}
```

### 2?? ItemId (���� ������)

```cpp
if (!Condition.ItemId.IsNone() && Condition.ItemId != Payload.StringParam)
{
    return false; // �� ��� ������� � ���������
}
```

### 3?? NpcId (���� ������)

```cpp
if (!Condition.NpcId.IsNone() && Condition.NpcId != Payload.StringParam)
{
    return false; // �� ��� NPC � ���������
}
```

### 4?? RequiredTags (���� �������)

```cpp
if (Condition.RequiredTags.Num() > 0)
{
if (!Payload.TagsParam.HasAll(Condition.RequiredTags))
    {
        return false; // ���� �� ��������� � ���������
    }
}
```

### ? ��� �������� ������ � ��������� ��������!

```cpp
Manager->UpdateObjectiveProgress(QuestId, ObjectiveId, ProgressAmount);
```

---

## ?? ������� ������� �������

### ������ 1: ������� �������

```
Quest: "Gather Resources"

Phase 1: "Collect Materials"
  Objective 1: "Collect 10 wood"
    Condition:
    EventTag: GameEvent.Inventory.ItemAcquired
      ItemId: Wood
Count: 10

  Objective 2: "Collect 5 stones"
    Condition:
      EventTag: GameEvent.Inventory.ItemAcquired
      ItemId: Stone
  Count: 5
```

---

### ������ 2: ���������� � NPC

```
Quest: "Village Gossip"

Phase 1: "Talk to Villagers"
  Objective 1: "Talk to Alice"
    Condition:
    EventTag: GameEvent.Dialogue.Started
      NpcId: Alice
      Count: 1

  Objective 2: "Talk to Bob"
    Condition:
      EventTag: GameEvent.Dialogue.Started
    NpcId: Bob
      Count: 1

  Objective 3: "Talk to Charlie"
    Condition:
      EventTag: GameEvent.Dialogue.Started
      NpcId: Charlie
   Count: 1
```

---

### ������ 3: �������� ��������

```
Quest: "Birthday Gifts"

Phase 1: "Give Presents"
  Objective 1: "Give flowers to Alice"
    Condition:
      EventTag: GameEvent.Inventory.ItemGifted
      ItemId: RedRose
      NpcId: Alice
      Count: 1

  Objective 2: "Give book to Lianne"
  Condition:
      EventTag: GameEvent.Inventory.ItemGifted
      ItemId: OldBook
    NpcId: Lianne
      Count: 1
```

---

### ������ 4: ��������������� �������

```
Quest: "Master Crafter"

Phase 1: "Craft Items"
  Objective 1: "Craft any 5 items"
    Condition [0]:
      EventTag: GameEvent.Inventory.ItemCrafted
      ItemId: [�����]  // �� ��������� ItemId!
    Count: 5

Phase 2: "Gift Crafted Items"
  Objective 1: "Gift 3 crafted items to NPCs"
    Condition [0]:
      EventTag: GameEvent.Inventory.ItemGifted
      RequiredTags: Item.Crafted  // ������ �����������!
  Count: 3
```

---

### ������ 5: ��������� ������

```
Quest: "Night Owl"

Phase 1: "Visit Library at Night"
  Objective 1: "Enter library between 20:00 and 06:00"
    Condition:
      EventTag: GameEvent.Location.EnterArea
    AreaId: Library
      TimeRangeStart: 20  // 20:00
   TimeRangeEnd: 6     // 06:00
      Count: 1
```

---

## ?? ������ ������

### ? ������ 1: ������ EventTag

```
Condition:
  EventTag: [�����]  ? ������!
  ItemId: RedApple
```

**�����������:**
```
Condition:
  EventTag: GameEvent.Inventory.ItemAcquired  ? �����������!
  ItemId: RedApple
```

---

###? ������ 2: ������������ ���

```
Condition:
  EventTag: Quest.Event.Item.Acquired  ? ������ ���! �� ��������!
  ItemId: RedApple
```

**�����������:**
```
Condition:
EventTag: GameEvent.Inventory.ItemAcquired  ? ����������!
  ItemId: RedApple
```

---

### ? ������ 3: ���������� ItemId � NpcId

```
Objective: "Talk to Alice"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    ItemId: Alice  ? ������! ��� NPC, �� �������!
```

**�����������:**
```
Objective: "Talk to Alice"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    NpcId: Alice  ? ���������!
```

---

## ?? �����: ��� ����� ������?

### ? �������� ������:

| ��� | EventTag | ��� ����������� |
|-----|----------|-----------------|
| Inventory | `GameEvent.Inventory.ItemAcquired` | ������ ��������� |
| Inventory | `GameEvent.Inventory.ItemUsed` | ������������� ��������� |
| Inventory | `GameEvent.Inventory.ItemCrafted` | ����� ��������� |
| Inventory | `GameEvent.Inventory.ItemGifted` | ������� ��������� NPC |
| Inventory | `GameEvent.Inventory.ItemDropped` | ������ ��������� |
| Dialogue | `GameEvent.Dialogue.Started` | **�������� � NPC** ? |
| Dialogue | `GameEvent.Dialogue.ChoiceSelected` | ����� � ������� |
| Dialogue | `GameEvent.Dialogue.Completed` | ���������� ������� |
| Quest | `GameEvent.Quest.Started` | ������ ������ |
| Quest | `GameEvent.Quest.Completed` | ���������� ������ |

### ?? ������� ����������:

| ��� | EventTag | ������ |
|-----|----------|--------|
| Location | `GameEvent.Location.EnterArea` | ? ����� �������-��������� |
| Location | `GameEvent.Location.LeaveArea` | ? ����� �������-��������� |
| Time | `GameEvent.Time.Reached` | ?? �������� (���� ������� �������) |
| Time | `GameEvent.Time.DayChanged` | ?? �������� (���� ������� �������) |
| Weather | `GameEvent.Weather.Changed` | ? ����� ������� ������ |
| Affection | `GameEvent.Affection.Changed` | ? ����� ������� ��������� |
| Needs | `GameEvent.Need.Changed` | ? ����! (NeedsSubsystem) |

---

## ?? ������������

### ��� ���������� �������:

1. **������** ���������� `EventTag`!
2. ����������� ������� (`ItemId`, `NpcId`) ��� ��������
3. ���������� ���� � ��� ����� ��� �� ���������
4. ���������� ������ objective ��������

### ��� �������������:

1. ��� ����� ������� � ���������� ���� � `DefaultGameplayTags.ini`
2. ���������� `Emit*` ������� � `QuestEventBus` ��� `GameEventBus`
3. ���������� ��� `StringParam` / `IntParam` ��������� ���������
4. ����������� ����������� ��� �������

---

**������ ��������� �������������!** ?????
