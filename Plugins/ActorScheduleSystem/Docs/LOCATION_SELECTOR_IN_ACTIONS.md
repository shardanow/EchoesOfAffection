# Location Selector � Actions

## ��������

��� ������������� ����������� �������� (Composite Action) ��� �������� �������� ���������� **���� � �� ��** ������� �� `FScheduleExecutionContext`, ������� ���������� ���� ��� �� ������ `ScheduleEntry`.

���� ��� �����, ����� ������ �������� �������� ��������� � **������** �������� � ������� ������, ������ ������ �� �������.

## �������

������ `UScheduleAction_MoveToLocation` ������������ ����������� `LocationSelector`, ������� �������������� ������� �� Context.

## ��� ������������

### 1. ���� Location Selector ��� ���� (������ ������)

���� ��� �������� ���� � ����� �������, ��������� `LocationSelector` �� ������ `ScheduleEntry`:

```
ScheduleEntry
  ?? LocationSelector: ActorTag("Location.Fields")
  ?? Action: Composite
     ?? MoveToLocation (��� LocationSelector)
?? Wait (5 ���)
       ?? MoveToLocation (��� LocationSelector)
```

���������: ��� MoveToLocation ���� � ����� ������� � ����� "Location.Fields".

### 2. ������ Location Selectors (����� ������)

���� ������ �������� ������ ���� � ������ ��������, **�� ���������� LocationSelector �� ScheduleEntry**, � ��������� ��� �� ������ �������� ��������:

```
ScheduleEntry
  ?? LocationSelector: �� ��������!
  ?? Action: Composite
       ?? MoveToLocation
       ?    ?? LocationSelector: ActorTag("Location.Fields")
       ?? Wait (5 ���)
       ?? MoveToLocation
    ?    ?? LocationSelector: ActorTag("Location.Town")
    ?? Wait (3 ���)
       ?? MoveToLocation
       ?? LocationSelector: ActorTag("Location.Home")
```

���������:
1. �������� � ������� � ����� "Location.Fields"
2. �������� 5 ������
3. �������� � ������� � ����� "Location.Town"
4. �������� 3 �������
5. �������� � ������� � ����� "Location.Home"

## �����������

� ����� �� �������:

```
MoveToLocation: Using own LocationSelector: ScheduleLocationSelector_ActorTag_0
MoveToLocation: Selected location: X=1311.576 Y=-6383.472 Z=-0.000
   ReferenceActor: BP_LocationMarker_C_2
   LocationTags: (GameplayTags=((TagName="Location.Fields")))
```

���, ���� ������������ Context:

```
MoveToLocation: Using location from Context
   Context LocationTags: (GameplayTags=((TagName="Location.Fields")))
```

## ��������� ������ �������

1. **����������� LocationSelector** � MoveToLocation ? ���������� ����� �������
2. **������� �� Context** ? ������������ �������, ��������� �� ������ ScheduleEntry
3. **��� �������** ? ������, �������� �� �����������

## ����������

���� ���������� ����� �������� � ������ ���������, ������� ����� �������� ��������� ������������ LocationSelector � �������.
