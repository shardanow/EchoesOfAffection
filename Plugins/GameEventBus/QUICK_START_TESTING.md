# ?? ������� �����: ������������ ��������������� ���������� �������

## ? ������: ������ � ������������!

������� **��������� ����** ��� ��������������� ���������� ������� ��� ����� ���������.

---

## ?? ���� ��� �����

### 1. ����������� ������

**Visual Studio:**
```
Build > Build Solution
```

���:
```
Rebuild ������ QuestSystemRuntime plugin
```

### 2. ������ Data Assets (���� ��� �� ��������)

#### A. Item Data Asset

**����:** `Content/Game/Core/Subsystems/Inventory/Items/`

**��� �����:** `DA_Item_Apple`

**���������:**
```
Item ID: "Apple"  ? �����! ��� ��������
Item Name: "Red Apple"
Item Description: "A juicy red apple"
Max Stack Size: 99
? Is Stackable
? Can Be Picked Up
? Can Be Dropped
```

#### B. Quest Data Asset

**����:** `Content/Game/Quests/`

**��� �����:** `DA_Quest_CollectApples`

**���������:**
```
Quest ID: "CollectApples"
Title: "������ ������"
Description: "������ 5 ����� ��� �������"

Objectives[0]:
  Objective ID: "Collect_Apple"  ? �����! ������: Collect_{ItemID}
  Description: "Collect apples"
  Target Count: 5
  ? Is Hidden
  ? Is Optional
```

#### C. World Item Blueprint

**����:** `Content/Game/Items/`

**���:** `BP_Item_Apple`

**Parent Class:** `WorldItemActor`

**���������:**
```
World Item Actor:
  Item Data: DA_Item_Apple
  Stack Size: 1
  ? Can Be Picked Up
  ? Auto Create Interactable
```

������ **Static Mesh** ������ � ������� **Transform**.

#### D. Quest Giver NPC

����� ��� ������ `BP_QuestGiver` / `BP_Lianne`:

```
Quest Giver Component:
  Available Quests:
    [0] = DA_Quest_CollectApples
```

---

### 3. �������� � Level

1. �������� **BP_Item_Apple** � ��� (������� 5 ����)
2. �������� **BP_QuestGiver** (NPC ������ �����)
3. ������� ��� � ������ ���� **Quest Tracker Widget** �� ������

---

### 4. ������� ���� (PIE)

**Alt + P** ��� **Play in Editor**

---

### 5. ������ �����������

**������ �������:** `~`

```
GameEventBus.SetVerbose 1
GameEventBus.EnableHistory 1
```

---

### 6. ����� �����

������� � NPC ? ����� `E` ? ������ ����� "������ ������"

��� ����� �������:
```
Quest.Start CollectApples
```

---

### 7. ������ ������

������� � ������ ? ����� `E`

---

### 8. ������� ���������

#### A. ��� ������� ������ ��������:

```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | Apple | Count: 1
LogQuestBridge: TEMP FIX: ItemAcquired event - Item='Apple' Count=1
LogQuestBridge: TEMP FIX: Checking 1 active quests
LogQuestBridge: TEMP FIX: Successfully updated quest 'CollectApples' objective 'Collect_Apple' (+1)
LogQuest: Objective progress updated: CollectApples / Collect_Apple (1/5)
```

#### B. Quest Tracker Widget ������ ����������:

```
????????????????????????????????
?  ������ ������   ?
?        ?
?  Collect apples (1/5)  ? ����������!
?             ?
?  ??????????????????? 20%    ?
????????????????????????????????
```

#### C. ������� ������� �������:

```
GameEventBus.PrintHistory
```

������ ���� �����:
```
[0] Inventory.Event.ItemAcquired | Apple | 1
```

---

### 9. ������ ��� 5 �����

�������� ��� 7 ���� �� ������� ��� ������.

�������� ������ �����������:
```
Collect apples (1/5) ? 
Collect apples (2/5) ? 
Collect apples (3/5) ? 
Collect apples (4/5) ? 
Collect apples (5/5) ? COMPLETE!
```

��� ����� ���������� ������ ����� ������ ������������� �����������!

---

## ?? ������� ���� �� ��������

### �������� 1: ������� �� �����������

**������� ���:**
```
GameEventBus.PrintHistory
```

���� �����:
- ������� ��� `DA_Item_Apple->ItemID = "Apple"`
- ������� ��� `BP_Item_Apple->ItemData` ��������� �� `DA_Item_Apple`
- ������� ��� GameEventBus plugin �������:
  - `Edit ? Plugins ? Search "GameEventBus"`
  - ? Enabled

---

### �������� 2: ������� �����������, �� ����� �� �����������

**������� ObjectiveID:**

���� ������� ��������� �������� (� ������� ����������):

1. `Collect_Apple` ? **�������������!**
2. `ItemAcquired_Apple`
3. `CollectApples_Objective`
4. `Apple`
5. `CollectionObjective`

**�������� ������������ ���� �� ���� ��������� � Quest Data Asset!**

**���� ����������� ���� ������:**

������ �����������:
```
GameEventBus.SetVerbose 1
```

� ������� ��� - ������� ����� ObjectiveID ������� �������� ��������.

---

### �������� 3: ����� �� �����������

�������:
- Quest Asset ��������� ��������
- Quest Giver Component �������� ����� � `Available Quests`
- ����� ��������������� � NPC ���������

�������� ��������� �������:
```
Quest.Start CollectApples
```

---

### �������� 4: GameplayTag �� ������

**������� ����:** `Config/DefaultGameplayTags.ini`

������ ���� ������:
```ini
+GameplayTagList=(Tag="Inventory.Event.ItemAcquired",DevComment="")
```

**����������� ��������!** (���� ����������� ������ ��� ������)

���:
**Tools ? Refresh Gameplay Tags**

---

## ?? �������������� ������� ObjectiveID

| ������ | ������ | ����� ������������ |
|--------|--------|-------------------|
| `Collect_{ItemID}` | `Collect_Apple` | **������!** �������� |
| `ItemAcquired_{ItemID}` | `ItemAcquired_Apple` | ����� event |
| `{QuestID}_Objective` | `CollectApples_Objective` | ������������� ���� |
| `{ItemID}` | `Apple` | ������� ������� |
| `CollectionObjective` | `CollectionObjective` | ����� ���� |

---

## ?? ��������� �����������

- ? �������� ��� `Inventory.Event.ItemAcquired`
- ? �� �������� ��� ������ �������:
  - Dialogue.Event.*
  - Time.Event.*
  - Location.Event.*
  - Combat.Event.*

��� ������ ����� ������� ����� ����� �������� ����������� ������!

---

## ? ������� ����������

- [ ] ��� �������������
- [ ] DA_Item_Apple ������ (ItemID = "Apple")
- [ ] DA_Quest_CollectApples ������ (ObjectiveID = "Collect_Apple")
- [ ] BP_Item_Apple ��������
- [ ] 5+ ����� ��������� � ������
- [ ] Quest Giver �������� � �������
- [ ] Quest Tracker Widget �������� �� UI
- [ ] GameEventBus plugin �������
- [ ] �������� �����������

---

## ?? ������!

������ ������� ������������� ��������� ������ ��� ����� ���������!

**����� � �������������!** ???

---

## ?? �������������� ���������

- `CRITICAL_BUG_FOUND.md` - ��������� �������� ��������
- `TEMP_FIX_APPLIED.md` - ����������� ������������ �����
- `DIAGNOSIS_SUMMARY.md` - ������ ����������� � ������
- `HOW_TO_CREATE_QUEST_WITH_EVENTS.md` - ������������ ����������

**���� ��������� �������� - ����� ��� �����!**
