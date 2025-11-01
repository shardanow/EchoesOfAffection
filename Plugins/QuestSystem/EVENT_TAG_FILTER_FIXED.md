# ?? ��������� ������ ����� � EventTag!

## ?? ��������:

� ���� `EventTag` ��������� `FObjectiveCondition` **��� ���������� ������** ������� ��������� ������ ���� `Quest.Event.*`!

### ������ ���:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (Categories = "Quest.Event"))
FGameplayTag EventTag;
```

**���������:** Dropdown ��������� ������:
- `Quest.Event.Item.Acquired` ? (������, �� ������������)
- `Quest.Event.Item.Used` ? (������, �� ������������)

**�� ���������:**
- `GameEvent.Inventory.ItemAcquired` ? (������!)
- `GameEvent.Inventory.ItemUsed` ? (������!)

---

## ? �������:

������ ������ � `Quest.Event` �� `GameEvent`:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (Categories = "GameEvent"))
FGameplayTag EventTag;
```

**������ dropdown ����������:**
- ? `GameEvent.Inventory.ItemAcquired`
- ? `GameEvent.Inventory.ItemUsed`
- ? `GameEvent.Inventory.ItemCrafted`
- ? `GameEvent.Inventory.ItemGifted`
- ? `GameEvent.Inventory.ItemDropped`
- ? `GameEvent.Quest.*`
- ? `GameEvent.Dialogue.*`
- ? ��� ��������� `GameEvent.*` ����!

---

## ?? ��� ����� �������:

### ��� 1: Intermediate ��� QuestSystem ��� ������

### ��� 2: ������������ ������

**�������� Unreal Editor** � ���������:

```
Build ? Rebuild Solution
```

��� ������ �������� `.uproject` ������ � Visual Studio ���������� �������������.

### ��� 3: �������� Data Asset ������

����� ����������� Editor:

1. **��������** `DA_Quest_Apples`
2. **�������** Objectives ? Conditions ? `EventTag`
3. **������ � dropdown** ������ ���� ����� ���� `GameEvent.Inventory.*`!
4. **��������:**
   - Objective 1: `GameEvent.Inventory.ItemAcquired`
   - Objective 2: `GameEvent.Inventory.ItemUsed`
5. **���������** (Ctrl+S)
6. **��������� ����**

---

## ?? ���� ���� �� ����� �� �����:

���� ����� ���������� ���� �� ��� �� ��������� � dropdown:

### ������� 1: �������� ���� � Project Settings

1. Edit ? Project Settings
2. Gameplay Tags
3. Manage Gameplay Tags
4. Add New Gameplay Tag ? `GameEvent.Inventory.ItemAcquired`
5. ��������� ��� ��������� �����
6. ������������� Editor

### ������� 2: ������� ���� �������

����� **����������** ��� ����� � ���� `EventTag`:

1. �������� �� ���� `EventTag`
2. **�����������:** `GameEvent.Inventory.ItemAcquired`
3. Enter
4. ���������

**Unreal ������ ����� ��� ���� ���� ��� ��� � dropdown!**

---

## ?? ����� �����������:

### ���� �������:

```
[QuestEventBus] ?? Condition: EventTag='GameEvent.Inventory.ItemAcquired', ItemId='RedApple'
[QuestEventBus] ? EventTag Match? YES (Exact comparison)
[QuestEventBus] ? ALL CHECKS PASSED - CONDITION MATCHES!
[QuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

**������ ����������!** ???
