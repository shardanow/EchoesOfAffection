# ?? ��������� �������� ��� QuestTrackerWidget

## ?? ��������:

**Progress bar �� �������� ��� ����� ���������!**

### �������:

������ ���������� `UpdateProgressBar()` ������� **������ ����������� objectives**:

```cpp
if (bIsCompleted)
{
    CompletedObjectives++;  // +1 ������ ���� ��������� ���������
}

float Percent = (float)CompletedObjectives / (float)TotalObjectives;
```

**���������:**
- 0/5 ����� ? 0% ?
- 1/5 ����� ? 0% ?
- 2/5 ����� ? 0% ?
- ...
- 5/5 ����� ? 100% ?

�������� ��� **�������** ��������� � 0% �� 100% ������ ��� ������ ����������!

---

## ? �������:

��������� ����� `UpdateProgressBar()` ��� ����� **���������� ���������**:

```cpp
// Calculate partial progress for this objective
float ObjectiveProgress = 0.0f;
if (TargetCount > 0)
{
    ObjectiveProgress = FMath::Clamp((float)CurrentCount / (float)TargetCount, 0.0f, 1.0f);
}
```

**������:**
- 0/5 ����� ? 0%
- 1/5 ����� ? 20% ?
- 2/5 ����� ? 40% ?
- 3/5 ����� ? 60% ?
- 4/5 ����� ? 80% ?
- 5/5 ����� ? 100% ?

---

## ?? �������������� ���������:

1. **������ ������� ����:** Progress bar ���������� �������� **������ ��� ������� ���� ������**, � �� ���� ��� �����.

2. **�����������:** ��������� ���� ��� �������:
   ```
   [LogTemp] QuestTrackerWidget: Objective 'O_CollectApples' progress: 40.00% (2/5)
   [LogTemp] QuestTrackerWidget: Progress bar updated - 40.00% (1 objectives)
   ```

3. **��������:** ������������ `FMath::Clamp` ��� �������� ��� ������� ������ � ��������� 0.0-1.0.

---

## ?? ��� ��������������:

### ��� 1: Intermediate ��� QuestSystem ��� ������

### ��� 2: ������������ ������
- � Visual Studio: Build ? Rebuild Solution
- ��� �������� Unreal Editor � �������� `.uproject` ������

### ��� 3: **�����! ��������� Data Asset ������**

�������� `DA_Quest_Apples` � **�������� `TargetCount` � 1 �� 5**:

```
Phases ? P_CollectApples ? Objectives ? O_CollectApples:
  TargetCount: 5  ? ���������!
```

��������� Data Asset.

### ��� 4: ��������� ���� (PIE)

### ��� 5: ��������� ������ �� ������

**������ �������:**

```
1 ������ ? Progress bar: 20%
2 ������ ? Progress bar: 40%
3 ������ ? Progress bar: 60%
4 ������ ? Progress bar: 80%
5 ����� ? Progress bar: 100% + Quest Complete!
```

---

## ?? Output Log ��� ������� �����:

```
[LogTemp] [QuestEventBus] ??? MATCH FOUND! Adding 1 progress
[LogTemp] QuestProgressManager: Updated objective progress: 1/5
[LogTemp] QuestTrackerWidget: Objective 'O_CollectApples' progress: 20.00% (1/5)
[LogTemp] QuestTrackerWidget: Progress bar updated - 20.00% (1 objectives)
```

**�������� ��� ������ ����������� ������ � ������ �������!** ???

---

## ?? �������� �����������:

1. ? ���� � ���� ? `GameEvent.*`
2. ? ���� � ������� ? ���������
3. ? QuestEventBridge ? �������� �� `GameEvent.*`
4. ? Initialize() ? �������� `UFUNCTION()`
5. ? **Progress bar ? ���������� ��������� ��������** ? �����!

**�Ѩ �������� ��������!** ?????
