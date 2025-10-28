# CHANGELOG - Loop Child Actions Feature

## v1.1 - 2025-01-XX

### Added - ������������ �������� ������� � Composite Action

#### ����� �������� � `UScheduleAction_Composite`:

1. **`bLoopChildActions`** (bool)
   - �������� ������������ �������� �������
   - �� ���������: `false`
   - �������� ������ ��� `bExecuteSequentially = true`

2. **`MaxLoops`** (int32)
   - ������������ ���������� ������ (0 = ����������)
   - �� ���������: `0`
   - ������������ ������ ���� `bLoopChildActions = true`

3. **`LoopCount`** (int32) - � `FScheduleChildActionData`
   - ���������� ������� ����������� ������

#### ���������� �����:

**Header:**
- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Public/Actions/ScheduleAction_Composite.h`
  - ��������� ����� UPROPERTY
  - ��������� ���� `LoopCount` � `FScheduleChildActionData`

**Implementation:**
- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Private/Actions/ScheduleAction_Composite.cpp`
  - �������������� `CheckAndAdvanceSequential()` ��� ��������� ������������
  - ��������� `GetActionDescription_Implementation()` ��� ����������� ���������� � ������
  - ��������� ����������� ��� ������������

#### ����� ������������:

1. `Plugins/ActorScheduleSystem/Docs/FEATURE_Loop_Child_Actions.md`
   - ������ �������� ����������������
   - ������� �������������
   - ����������� ������

2. `Plugins/ActorScheduleSystem/Docs/EXAMPLE_Looping_Actions.md`
   - ��������� ������ �������� ������������ �������� ����������
   - ������� � troubleshooting

3. `Plugins/ActorScheduleSystem/README.md`
   - ��������� ������ "Built-in Components"
   - ��������� ������ "Looping Child Actions" � Advanced Features

#### �������������:

- ? ������ �������� �������������
- ? ������������ Composite Actions ���������� �������� ��� ���������
- ? ������� breaking changes

#### �������������:

```cpp
// ����������� ���� (�� ������)
Composite Action:
?? bExecuteSequentially = true
?? bLoopChildActions = true
?? MaxLoops = 0  // Infinite
?? Child Actions: [Action1, Action2, Action3]

// ������������ ������������ (3 ����)
Composite Action:
?? bExecuteSequentially = true
?? bLoopChildActions = true
?? MaxLoops = 3
?? Child Actions: [Action1, Action2, Action3]
```

#### �����������:

����� ���-��������� � ��������� `LogScheduleComposite`:
- `Looping child actions (Loop N/M)` - ��� ������ ������ �����
- `Restarting first child action` - ��� �������� ������� ������
- `Max loops (N) reached for composite` - ��� ���������� ������ ������

#### Use Cases:

1. **������� �����** - NPC ��������� ���� ������ ���� ����
2. **���������� ��������** - ����������� ����� �����
3. **������������� ��������** - ����� ���������� � N ����������

#### Known Limitations:

1. �������� ������ ��� ����������������� ���������� (`bExecuteSequentially = true`)
2. ��� ������������ ������� �� ��������������
3. ������ ��������� ������� ���� ����������

#### Performance:

- ��������� �������: ����������� (���� int32 � ��������� ��������)
- ������: +4 ����� �� �������� Composite Action
- CPU: ��� �������������� �������� ��� ����������

---

## ��������

������� � �����������: https://github.com/shardanow/EchoesOfAffection
