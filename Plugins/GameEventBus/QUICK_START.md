# ? ������! GameEventBus ������������ �� ��� �������!

## ?? ��� �������

### 1. **Inventory System** � ������� ������� ���������! ?

**����� ���������:**
- `InventoryComponentGameEventHelper.h` (helper ��� �������)
- `InventoryComponentGameEventHelper.cpp` (����������)

**��������� � `InventoryComponent.cpp`:**
- ? `AddItem()` ? ��������� `Inventory.Event.ItemAcquired`
- ? `UseItem()` ? ��������� `Inventory.Event.ItemUsed`
- ? `GiftItem()` ? ��������� `Inventory.Event.ItemGifted`

**��� ��� ��������!** ������ ��������� ���� ��������� ��� ������, ������� ����� ������������� �������������.

---

### 2. **TimeOfDay System** � ������� ������� ���������! ?

**����� ���������:**
- `TimeOfDayGameEventHelper.h` (helper ��� �������)
- `TimeOfDayGameEventHelper.cpp` (����������)

**��������� � `TimeOfDaySubsystem.cpp`:**
- ? `ProcessTimeChanges()` ? ��������� `Time.Event.HourChanged`
- ? `ProcessTimeChanges()` ? ��������� `Time.Event.DayChanged`
- ? `ProcessTimeChanges()` ? ��������� `Time.Event.SeasonChanged`
- ? `CheckCelestialEvents()` ? ��������� `Time.Event.Sunrise`
- ? `CheckCelestialEvents()` ? ��������� `Time.Event.Sunset`

**��� ��� ��������!** ����� ���������� ? ������� ����������� �������������!

---

### 3. **Quest System** � ������������� ������� �������! ?

**��� ���������:**
- ? `QuestEventBridge` ������������� �� ��� ������� ���������
- ? `QuestEventBridge` ������������� �� ��� ������� �������
- ? ������ ������������� ����������� ��� ��������� ���������
- ? ������ ������������� ����������� ��� ��������� �������
- ? �� �������� ����� Data Assets

---

### 4. **Dialogue System** � ����� ����������� �������! ?

**��� ������:**
- ? `DialogueEffect_EmitGameEvent` �������� � Data Assets
- ? ����� ��������� ������ �� ��������
- ? ����� ����������� ��������� �������

---

## ?? ��� ������ ������?

### ������� 1: �� ��� ��������! (����������)

**Inventory + TimeOfDay + Quest System** ������ �� �������!

#### ������ 1: ����� "������� 5 �����"

**Quest Data Asset:**
```
Quest: "Collect Apples"
  Objective:
    Event Tag: "Inventory.Event.ItemAcquired"
    String Filter: "Apple"  // ItemID �� ItemDataAsset
    Target Count: 5
```

����� ����� ������� 5 ����� ? ����� ���������� **�������������**! ??

#### ������ 2: ����� "��������� � 14:00"

**Quest Data Asset:**
```
Quest: "Meet at 2 PM"
  Objective:
    Event Tag: "Time.Event.HourChanged"
    Int Filter: 14  // Hour = 14 (2 PM)
    Target Count: 1
```

����� ����� ������ 14:00 ? ����� ���������� **�������������**! ??

#### ������ 3: ����� "������ ����� ������"

**Quest Data Asset:**
```
Quest: "Autumn Harvest"
  Start Conditions:
    - Event Tag: "Time.Event.SeasonChanged"
      String Filter: "Autumn"
  
  Objectives:
 - Event Tag: "Inventory.Event.ItemAcquired"
      String Filter: "Mushroom"
      Count: 10
```

����� ��������� ? ����� ���������� ? ������ ����� ? ����� ��������! ??

---

## ?? ������� �������������

### ������ 1: ��������������� ����� "������� ������ �� ������"

**Quest Data Asset:**
```
Quest: "Bring Apples Before Sunset"
  Objectives:
    1. Event Listener:
       - Event Tag: "Inventory.Event.ItemAcquired"
         String Filter: "Apple"
         Count: 5
  
  Fail Conditions:
    - Event Tag: "Time.Event.Sunset"
      Action: Fail Quest
```

**��� ����������:**
1. ����� �����������
2. ����� �������� ������
3. ���� ������ 5 �� ������ ? ����� �������� ?
4. ���� ����� �������� ������ ? ����� �������� ?

---

### ������ 2: ����� "������� 3 ���"

**Quest Data Asset:**
```
Quest: "Wait for Harvest"
  Objective:
    Event Tag: "Time.Event.DayChanged"
    Target Count: 3
```

**��� ����������:**
1. ���� 1 ? 1/3
2. ���� 2 ? 2/3
3. ���� 3 ? 3/3 ? ����� ��������! ?

---

## ??? ���������� ������� ��� ������������

### �������� ������� �������
```
GameEventBus.EnableHistory 1
```

### �������� ��������� 10 �������
```
GameEventBus.PrintHistory
```

### �������� ����������
```
GameEventBus.PrintStats
```

### ������ ������:
```
LogGameEventBus: Event [Inventory.Event.ItemAcquired] | ItemId: Apple | Count: 5
LogGameEventBus: Event [Time.Event.HourChanged] | Hour: 14
LogGameEventBus: Event [Time.Event.Sunrise]
LogQuest: Quest objective updated: CollectApples (5/5)
LogQuest: Quest completed: CollectApples
```

---

## ? ��������� �������

- [x] GameEventBus ������ ������ � ����������
- [x] Inventory System ��������� ������� �������������
- [x] TimeOfDay System ��������� ������� �������������
- [x] Quest System �������� �� ������� �������������
- [x] Dialogue System ����� ����������� ������� ����� Data Assets
- [x] Build ������� (Result: Succeeded)
- [x] ������������ ���������
- [x] ������� �������

---

## ?? ���� ����� ������� ������:

1. **������� Quest Data Asset** � Event Listener objective
2. **������� ItemDataAsset** ��� ��������� (� `ItemID`)
3. **��������� ����** �:
   - ������� ������� ? ����� ���������
   - ��������� ����������� ����� ? ����� ���������
4. **��������� ��������** ������

**�� ��������� �������� �������������!** ??

---

## ?? ����� ������?

- ������ `Plugins/GameEventBus/INTEGRATION_COMPLETE.md` ��� �������� Inventory
- ������ `Plugins/GameEventBus/TIMEOFDAY_INTEGRATION_COMPLETE.md` ��� �������� TimeOfDay
- ������ `Plugins/GameEventBus/Documentation/INTEGRATION_EXAMPLES.md` ��� ��������� ��������
- ������ `Plugins/GameEventBus/Documentation/GameplayTags.md` ��� ������ ���� �����
- ������� ����: `LogGameEventBus`, `LogQuest`, `LogDialogue`

**����� � ����������!** ???
