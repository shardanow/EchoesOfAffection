# �����������: ������ ������� � Composite Action

## ��� ���� ����������

**��������**: ��� ������������� ������������ �������� (��������, "�������� ? �������� ? �������� ? ��������") ��� �������� MoveToLocation ������������ **���� � �� ��** �������, ��������� ���� ��� �� ������ ScheduleEntry.

**�������**: ��������� ��������� ������������ LocationSelector � ������ �������� MoveToLocation.

## ��� ������ ��������� ������ �������

### ��� 1: �������� ��� ScheduleEntry

� Content Browser ������� ��� ScheduleEntry (��������, `Morning_Activity`).

### ��� 2: ������� LocationSelector � Entry

**�����**: ���� �� ������, ����� ������ �������� ���� � ������ ������, **�� ����������** LocationSelector �� ������ Entry!

### ��� 3: ��������� LocationSelector ��� ������� MoveToLocation

�������� ��� Composite Action � ��� ������� ��������� MoveToLocation:

1. �������� �������� MoveToLocation
2. � Details ������� ������ **Movement**
3. ��������� �������� **Location Selector**
4. ������� **+** ��� �������� ������ ���������
5. �������� ��� (��������, `ScheduleLocationSelector_ActorTag`)
6. ��������� **Required Tags** ��� ����� ����������� ��������

### ������ ������������

**Morning Activity**:
- LocationSelector: **�� ��������**
- Action: Composite (Sequential)
  - **Child 0**: MoveToLocation
    - LocationSelector: ActorTag
      - Required Tags: `Location.Fields`
  - **Child 1**: Wait (Duration: 5 seconds)
  - **Child 2**: MoveToLocation
    - LocationSelector: ActorTag
      - Required Tags: `Location.Town`
  - **Child 3**: Wait (Duration: 3 seconds)
  - **Child 4**: MoveToLocation
    - LocationSelector: ActorTag
      - Required Tags: `Location.Home`
  - **Child 5**: Wait (Duration: 2 seconds)

## ���������

������ �������� �����:
1. ���� � ������� � ����� `Location.Fields`
2. ����� 5 ������
3. ���� � ������� � ����� `Location.Town`
4. ����� 3 �������
5. ���� � ������� � ����� `Location.Home`
6. ����� 2 �������
7. ��������� ����������

## �������� � �����

����� ������� ���� �� ������� � �����:

```
MoveToLocation: Using own LocationSelector: ScheduleLocationSelector_ActorTag_0
MoveToLocation: Selected location: X=1311.576 Y=-6383.472 Z=-0.000
   ReferenceActor: BP_LocationMarker_Fields
   LocationTags: (GameplayTags=((TagName="Location.Fields")))
```

� ��� ��������� ��������:

```
MoveToLocation: Using own LocationSelector: ScheduleLocationSelector_ActorTag_1
MoveToLocation: Selected location: X=2456.123 Y=-4521.789 Z=-0.000
   ReferenceActor: BP_LocationMarker_Town
   LocationTags: (GameplayTags=((TagName="Location.Town")))
```

## �������������

������ ������ (���� LocationSelector �� Entry) �� ��� ��������! ���� ��� �����, ����� ��� �������� ���� � ����� �����, ������:
- ��������� LocationSelector �� ������ ScheduleEntry
- �� ���������� LocationSelector � �������� MoveToLocation

## ����� ���������

- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Public/Actions/ScheduleAction_MoveToLocation.h`
- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Private/Actions/ScheduleAction_MoveToLocation.cpp`
