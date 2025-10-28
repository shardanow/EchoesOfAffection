# Feature: Loop Child Actions in Composite Action

## ��������
��������� ���������������� ������������ ������ �������� ������� � `UScheduleAction_Composite`.

## ����� ��������

### `bLoopChildActions` (bool)
- **��������**: �������� ������������ �������� ������� ����� ���������� ����������
- **�������� �� ���������**: `false`
- **���������**: Action
- **������� �����������**: ������ ��� ����������������� ���������� (`bExecuteSequentially = true`)

### `MaxLoops` (int32)
- **��������**: ������������ ���������� ���������� ����� (0 = ����������)
- **�������� �� ���������**: `0` (����������)
- **���������**: Action
- **������� �����������**: ������ ���� `bLoopChildActions = true`
- **�����������**: ������� 0

### `LoopCount` (int32) - ���������� ����
- **��������**: ������� �������� ���������� ����������� ������
- **���**: ���������� ���� � `FScheduleChildActionData`

## �������������

### ������ 1: ����������� ������������ (������� ����)

```cpp
// ������� Composite Action � ���������
Composite Action: "������� �������"
??? bExecuteSequentially = true
??? bLoopChildActions = true
??? MaxLoops = 0  // ����������
??? Child Actions:
    1. MoveToLocation (������)
    2. PlayAnimation (������)
  3. Wait (30 ������)
    4. MoveToLocation (�����)
    5. Wait (10 ������)
```

**���������**: NPC ����� ���������� ��������� ����: ���� � ������ ? �������� ? ����� ? ���� �� ����� ? ����� ? ����� � ������...

### ������ 2: ������������ ������������

```cpp
// NPC ������ 3 ���� ������ ���������� �������
Composite Action: "������� 3 �����"
??? bExecuteSequentially = true
??? bLoopChildActions = true
??? MaxLoops = 3  // ����� 3 ����
??? Child Actions:
    1. MoveToLocation (����� 1)
    2. Wait (5 ������)
    3. MoveToLocation (����� 2)
    4. Wait (5 ������)
    5. MoveToLocation (����� 3)
    6. Wait (5 ������)
```

**���������**: NPC ������� ������� ����� 3 ����, ����� ����� ����������.

### ������ 3: ��� ������������ (�� ���������)

```cpp
Composite Action: "�������� ������"
??? bExecuteSequentially = true
??? bLoopChildActions = false  // ��������� ���� ���
??? Child Actions:
    1. MoveToLocation (�����)
    2. PlayAnimation (���� ����)
    3. MoveToLocation (������)
    4. PlayAnimation (���������)
```

**���������**: NPC �������� ������������������ ���� ��� � ��������.

## ������ ������

### ������������������ ���������� � �������������:

1. **�����**: ����������� ������ �������� �����
2. **��������**: ����� ���������� �������� ������ ����������� ���������
3. **����� ������������������**: 
   - ���� `bLoopChildActions = false` ? Composite �����������
   - ���� `bLoopChildActions = true`:
     - ������������� `LoopCount`
     - ����������� �������: `(MaxLoops == 0) || (LoopCount < MaxLoops)`
     - **���� true**: ������������ `CurrentActionIndex = 0`, ��������� `ChildHandles`, ����������� ������ ����� �����
  - **���� false**: Composite ����������� (��������� `MaxLoops`)

### �����������

```cpp
// ��� ������������
UE_LOG(LogScheduleComposite, Log, TEXT("Looping child actions (Loop %d/%d)"), LoopCount, MaxLoops);

// ��� ���������� ������
UE_LOG(LogScheduleComposite, Log, TEXT("Max loops (%d) reached for composite"), MaxLoops);
```

## �����������

1. **������ ��� ����������������� ����������**: ������������ �������� ������ ��� `bExecuteSequentially = true`
2. **������������ ���������� �� ��������������**: ���� `bExecuteSequentially = false`, �������� `bLoopChildActions` ������������
3. **������ ��������� ����**: ����� `CancelAction()` ���������� ������������� ��� �������� ������ � ���������� ����

## ���������� � �����������

### ������: ������� ���� � �������������

```cpp
Schedule Entry: "������� �����"
??? TimeCondition: 08:00-17:00 (9 �����)
??? RecurrenceType: Daily
??? Action: Composite (Loop)
    ??? bLoopChildActions = true
    ??? MaxLoops = 0  // ���������� �� ����� ���������� ����
    ??? Child Actions:
        1. MoveToLocation (������� �����)
      2. PlayAnimation (������)
3. Wait (60 ������)
        4. MoveToLocation (�������)
      5. Wait (30 ������)
```

**���������**: 
- � 08:00 �� 17:00 NPC ����� ���������� ��������� ���� ������
- � 17:00 ��������� ���� ����������, ���������� ������� �����
- �� ��������� ���� � 08:00 ���� �������� ������ (��������� `RecurrenceType: Daily`)

## ������������������

- **��������� �������**: �����������, ������ �������� ������� � ����� �������
- **������**: ���� �������������� `int32` (LoopCount) �� ������ �������� Composite Action
- **�������**: ������������ ��� �� ������ `CheckInterval`, ��� � ��� ������������

## �������

### �������� �������� ����� (C++)
```cpp
if (FScheduleChildActionData* Data = ChildActionData.Find(ActionHandle.HandleID))
{
  int32 CurrentLoop = Data->LoopCount;
    int32 CurrentAction = Data->CurrentActionIndex;
    
    UE_LOG(LogTemp, Log, TEXT("Loop %d, Action %d/%d"), 
        CurrentLoop, CurrentAction, ChildActions.Num());
}
```

### Visual Logger
�������� ��������� `LogScheduleComposite` ��� ������������ ������������.

## �������� ������������ ������

������������ `Composite Action` **�� ������� ���������**:
- `bLoopChildActions` �� ��������� `false` � ��������� �� ���������
- ��� ������ �������� ����� �������� ��� ������

## Changelog

### v1.1 - 2025-01-XX
- ? ��������� �������� `bLoopChildActions`
- ? ��������� �������� `MaxLoops`
- ? �������� ������� `LoopCount` � `FScheduleChildActionData`
- ? ����������� ������ ������������ � `CheckAndAdvanceSequential()`
- ? ��������� �������� � `GetActionDescription_Implementation()`
- ? ��������� ����������� ������

## ��. �����
- [README.md](../README.md) - ����� ������������ ������� ����������
- [ScheduleAction_Composite.h](../Source/ScheduleRuntime/Public/Actions/ScheduleAction_Composite.h) - ������������ ����
- [ScheduleAction_Composite.cpp](../Source/ScheduleRuntime/Private/Actions/ScheduleAction_Composite.cpp) - ����������
