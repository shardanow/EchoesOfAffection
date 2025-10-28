# Quick Example: Looping Actions

## ������� ������ - ����������� ������� ����

### ������
������� NPC ��������, ������� � 08:00 �� 17:00 ��������� ��������� ����:
1. ���� � ������
2. �������� 30 ������
3. ���� �� �����
4. ����� ������ (5 ������)
5. ������������ � ������

### �������

#### 1. �������� ������� �������

```cpp
// � ������ ���������� LocationMarker'�:
LocationMarker_Machine (Tag: Location.Workplace.Machine)
LocationMarker_Storage (Tag: Location.Workplace.Storage)
```

#### 2. �������� Composite Action

� Content Browser: **Right-click ? Miscellaneous ? Data Asset ? ScheduleAction_Composite**

��������: `DA_Action_WorkLoop`

**���������:**
- `Action Name`: "������� ����"
- `bExecuteSequentially`: ? true
- `bLoopChildActions`: ? true
- `MaxLoops`: 0 (����������)
- `CheckInterval`: 0.5

**Child Actions** (�������� �� �������):

1. **MoveToLocation** (� ������)
   - Action Name: "���� � ������"
   - Location Selector: ActorTag
     - Required Tags: `Location.Workplace.Machine`
     - Selection Mode: Closest
   - Acceptance Radius: 100

2. **PlayAnimation** (������)
   - Action Name: "��������"
   - Animation: `Anim_Working` (���� ��������)
   - Duration: 30.0

3. **MoveToLocation** (�� �����)
   - Action Name: "���� �� �����"
   - Location Selector: ActorTag
     - Required Tags: `Location.Workplace.Storage`
     - Selection Mode: Closest
   - Acceptance Radius: 100

4. **Wait** (����� ������)
   - Action Name: "����� ������"
   - Wait Duration: 5.0

> **���������**: ����� ���� 4 ���� ������������� �������� � ���� 1!

#### 3. �������� Schedule Entry

� Content Browser: **Right-click ? Miscellaneous ? Data Asset ? ScheduleEntryData**

��������: `DA_Entry_WorkShift`

**���������:**
- `Entry Name`: "������� �����"
- `Priority`: 80
- `Recurrence Type`: Daily
- `Time Condition`:
  - Time Type: TimeRange
  - Start Hour: 8
  - Start Minute: 0
  - End Hour: 17
  - End Minute: 0
  - Days Of Week: [1, 2, 3, 4, 5] (��-��)
- `Action`: ? �������� `DA_Action_WorkLoop`

#### 4. �������� Schedule Data

� Content Browser: **Right-click ? Miscellaneous ? Data Asset ? ScheduleData**

��������: `DA_Schedule_Worker`

**���������:**
- `Schedule Name`: "���������� ��������"
- `Entries`: �������� `DA_Entry_WorkShift`

#### 5. �������� ��������� �� NPC

1. �������� ������ NPC � ������
2. **Add Component** ? `Schedule Component`
3. � ����������:
   - `Schedule Data`: ? �������� `DA_Schedule_Worker`
   - `bScheduleEnabled`: ? true

#### 6. ��������� ���������� �������

���������, ��� � ��� ���� `TimeOfDayScheduleIntegration` ��������� � ������ ��� GameState.

### ���������

?? **��������� NPC:**
- **�� 08:00**: Idle (��� ������ ����������)
- **08:00-17:00**: ���������� ��������� ����:
  - ���� � ������ ? �������� 30 ��� ? ���� �� ����� ? ���� 5 ��� ? [����� � ������]
- **����� 17:00**: ���������� �������� ��������, NPC ��������� � ���������� Entry

### �����������

#### ���������� ���������� ������
```
MaxLoops: 5  // ��������� ����� 5 ������ �� �����
```

#### �������� ������� ������ N ������
����������� ��������� Composite:
```
Composite (�������, MaxLoops = 0):
  ?? Composite (������� ����, MaxLoops = 3)
  ?  ?? [������ ? ������ ? ����� ? �����]
  ?? Wait (������� 60 ������)
```

## �������

### �������� ������ �����

1. **�������� Visual Logger** (Ctrl+L � PIE)
2. �������� ������ NPC
3. � ��������� `LogScheduleComposite` �� �������:
   ```
   Starting composite action with 4 children (Sequential: YES)
   Starting first child action
   Child action 0 completed, advancing to next
   Starting next child action 1
 ...
   Child action 3 completed, advancing to next
   Looping child actions (Loop 1/-1)  // -1 = infinite
   Restarting first child action
   ```

### �������� ��������

? **NPC �� ���������**
- ���������, ��� � NPC ���� AIController
- ���������, ��� NavMesh �������� � ������
- ���������, ��� LocationMarker'� ����� ���������� ����

? **���� �� �����������**
- ���������: `bLoopChildActions = true`
- ���������: `bExecuteSequentially = true`

? **���� ��������������� ������ �������**
- ��������� `MaxLoops` (0 = ����������)
- ��������� ����� ���� � Schedule Entry

## ��. �����
- [������ ������������ �� ������������](FEATURE_Loop_Child_Actions.md)
- [README.md](../README.md)
- [QUICKSTART.md](../QUICKSTART.md)
