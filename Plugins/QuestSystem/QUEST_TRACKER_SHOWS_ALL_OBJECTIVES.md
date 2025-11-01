# ?? QuestTrackerWidget ������ ���������� ��� �������� objectives!

## ?? ��������:

**������ ��������� ������ ������ objective �� ������!**

### ������ ���������:

```
�����: "������ ������"
? Objective 1: ������� 5 ����� (0/5)  ? ������������
? Objective 2: ������ 2 ������ (0/2)  ? �� �����!
```

����� ������ ����������:
```
�����: "������ ������"
? Objective 1: ������� 5 ����� (5/5)  ? ��������, �����
? Objective 2: ������ 2 ������ (0/2)  ? ������ ������������
```

**��������:** ����� �� ����� ��� ���� ��� objectives!

### �������:

������ ��� � `RefreshDisplay()`:

```cpp
// Find first incomplete objective
for (const FQuestObjective& Objective : Phase.Objectives)
{
  if (!bIsCompleted && !Objective.bIsHidden)
    {
        ObjectiveText = Objective.Title;
        break;  // ? ����! ������ ������!
    }
}
```

---

## ? �������:

��������� `RefreshDisplay()` ��� ����������� **���� �������� objectives**:

```cpp
// Collect ALL active (incomplete) objectives
TArray<FString> ActiveObjectives;
for (const FQuestObjective& Objective : Phase.Objectives)
{
    if (!bIsCompleted && !Objective.bIsHidden)
    {
      ActiveObjectives.Add(ObjectiveStr);
    }
}

// Join all objectives with newlines
FString CombinedText = FString::Join(ActiveObjectives, TEXT("\n"));
TextObjective->SetText(FText::FromString(CombinedText));
```

### ����� ���������:

```
�����: "������ ������"
? Objective 1: ������� 5 ����� (0/5)
? Objective 2: ������ 2 ������ (0/2)
```

����� ������ ��������:
```
�����: "������ ������"
? Objective 1: ������� 5 ����� (5/5)  ? ������������ ��� �����������
? Objective 2: ������ 2 ������ (0/2)
```

---

## ?? ��� ��� ��������:

### ������ 1: ����� � ����� objectives

```
????????????????????????
  ������ ������
????????????????????????
������� 5 ����� (3/5)
������ 2 ������ (0/2)
????????????????????????
[????????????] 40%
```

### ������ 2: ���� objective ��������

```
????????????????????????
  ������ ������
????????????????????????
������ 2 ������ (1/2)
????????????????????????
[????????????] 80%
```

### ������ 3: ��� objectives ���������

```
????????????????????????
  ������ ������
????????????????????????
All objectives complete!
????????????????????????
[????????????] 100%
```

---

## ?? �������������� ���������:

### 1. **������� ������� objectives**
```cpp
if (Objective.bIsHidden)
{
    continue; // �� ���������� �������
}
```

### 2. **�������������� ���������**
- **TargetCount > 1:** `"������� ������ (3/5)"`
- **TargetCount = 1:** `"���������� � NPC"` (��� �����)

### 3. **�����������**
```
[LogTemp] QuestTrackerWidget: Showing 2 active objectives
```

---

## ?? ��� ��������������:

### ��� 1: Intermediate ��� QuestSystem ��� ������

### ��� 2: ������������ ������
- � Visual Studio: Build ? Rebuild Solution
- ��� �������� Unreal Editor � �������� `.uproject` ������

### ��� 3: �������� ����� � 2+ objectives

� Data Asset `DA_Quest_Test`:

```
Phase: P_MultiObjective
  bRequireAllObjectives: TRUE ?

  Objective 1: O_CollectApples
    Title: "Collect 5 apples"
    TargetCount: 5
    Condition: EventTag=GameEvent.Inventory.ItemAcquired, ItemId=RedApple

  Objective 2: O_EatApples
    Title: "Eat 2 apples"
 TargetCount: 2
    Condition: EventTag=GameEvent.Inventory.ItemUsed, ItemId=RedApple
```

### ��� 4: ��������� ���� � ������� �����

### ��� 5: ��������� Quest Tracker Widget

**������ ������� ��� objectives ������������:**

```
Collect 5 apples (0/5)
Eat 2 apples (0/2)
```

### ��� 6: ��������� 3 ������

```
Collect 5 apples (3/5)  ? �������� ���������!
Eat 2 apples (0/2)
```

### ��� 7: ��������� ������ objective (5 �����)

```
Eat 2 apples (0/2)  ? ������ ������ �������
```

### ��� 8: ������� 2 ������

```
All objectives complete!  ? ����� ��������!
```

---

## ?? �������������:

### ? �������� �:
- **�������������� objectives** (2+)
- **���������� objectives** (1)
- **�������� objectives** (`bIsHidden = true`)
- **������������� objectives** (`bIsOptional = true`)
- **������� ������ �������** (ItemAcquired, ItemUsed, ItemCrafted, etc.)

### ? Progress Bar:
- ���������� **������� ��������** ���� objectives
- ��������� **��������� ��������** ������� objective
- ����������� � �������� �������

---

## ?? ����:

### ����:
```
? ����������� ������ ������ objective
? ����� �� ����� ��� ���� ��� ������
? ���������� "��������" ����� ���������� �������
```

### �����:
```
? ������������ ��� �������� objectives
? ����� ����� ������ ������ �����
? �������� ������� objective ����� �����
? ������������� ����������� ��� ���������
```

**������ ������ ��������� ������������!** ?????
