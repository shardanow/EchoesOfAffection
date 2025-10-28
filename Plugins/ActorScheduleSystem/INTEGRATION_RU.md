# ���������� Time System - ������� �������

## ? ������! ���������� �������� �������������

**TimeOfDaySystem** � **ActorScheduleSystem** ��� ������������� � �������� ������ �� �������!

---

## ?? ��� ����� ������� (3 ����)

### 1. �������� ����������

```
Content Browser ? Right-Click ? Data Asset ? ScheduleData
Name: DA_NPC_Shopkeeper
```

�������� ������ (Entries):
- 08:00 - ������� �������
- 08:00-20:00 - ��������
- 20:00 - ������� �������
- 20:00-08:00 - �����

### 2. �������� ��������� � NPC

```
Select NPC Actor
Add Component ? Schedule Component
Set Schedule Data ? DA_NPC_Shopkeeper
```

### 3. �������!

```
Press Play (PIE)
```

**��� � ��!** Time System ������������� ��������� ���������� ������ ������� ������.

---

## ?? ��� ��� �������� ��� �������

```
TimeOfDaySystem (������ ������)
  ?
NotifyTimeChanged(Hour, Minute, DayOfWeek)
    ?
ScheduleSubsystem (batch evaluation)
    ?
All NPCs with ScheduleComponent
    ?
Execute matching schedule entries
```

**Zero per-actor ticks!** ��� �������� event-driven.

---

## ?? ������� ������: ��������

### ���������� ��������

```
DA_Schedule_Merchant:

Entry 1: "Open Shop" (08:00)
  Priority: 90
  Action: Composite
    ? Move To (Tag: Location.ShopDoor)
    ? Play Animation (AM_Unlock)
  ? Move To (Tag: Location.Counter)

Entry 2: "Serve Customers" (08:00-20:00)
  Priority: 80
  Action: Composite
    ? Play Animation (AM_Idle_Working)
    ? Wait (Forever)
  Location: Tag (Location.Counter)

Entry 3: "Close Shop" (20:00)
  Priority: 90
  Action: Composite
    ? Play Animation (AM_Lock)
    ? Move To (Tag: Location.Home)

Entry 4: "Sleep" (20:00-08:00)
  Priority: 70
  Action: Play Animation (AM_Sleeping)
Location: Tag (Location.Bed)
```

### ������� �������

�������� empty actors � ������:
- `Location.ShopDoor`
- `Location.Counter`
- `Location.Home`
- `Location.Bed`

### ���������

- � 08:00 �������� ���� � �������� � ��������� �����
- � 08:00 �� 20:00 ����� �� ���������
- � 20:00 ��������� ������� � ���� �����
- � 20:00 �� 08:00 ����

**�� �������������!**

---

## ?? ���������� ��������

### �������� �����

```cpp
// C++
UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();
FTimeOfDayTime NewTime;
NewTime.Hour = 14;
NewTime.Minute = 30;
TimeSystem->SetCurrentTime(NewTime);
// ��� ���������� ������������� ���������������
```

```blueprint
// Blueprint
Get TimeOfDay Subsystem ? Set Current Time (Hour: 14, Minute: 30)
```

### �����/���������

```cpp
// �����
TimeSystem->PauseTime();

// ����������
TimeSystem->ResumeTime();

// �������� (1 real sec = 2 game minutes)
TimeSystem->SetTimeScale(120.0f);
```

---

## ?? ��������� ������������������

### Project Settings ? Actor Schedule System

```
Performance:
  Max Concurrent Scheduled Actors: 500
  bUse Distance Based Culling: ?
  Culling Distance: 5000.0

Debug:
  bEnable Debug Visualization: ? (for testing)
  bLog Schedule Events: ? (for debugging)
```

### TimeOfDaySettings Data Asset

```
Time Conversion:
  Time Scale: 60.0 (1 real sec = 1 game minute)
  bAuto Advance Time: ?

Performance:
  Update Frequency: 30.0 Hz
```

---

## ?? ���������� �������

### NPC �� ������� ����������?

1. ? **Time System ��������?**
   - Output Log ? "TimeOfDaySubsystem initialized"

2. ? **Component ��������?**
   - Actor ? Details ? Schedule Component?

3. ? **Schedule Data ��������?**
   - Schedule Component ? Schedule Data

4. ? **bScheduleEnabled = true?**
   - Schedule Component ? bSchedule Enabled ?

5. ? **����� ������������� ��������?**
   - Entry: 08:00-17:00, Monday-Friday
   - Current: 14:00 Saturday ?

### NPC �� ���������?

1. ? **AIController ��������?**
   - Actor ? AI Controller Class

2. ? **NavMesh ����������?**
   - Show ? Navigation (P key)

3. ? **Target reachable?**
   - Acceptance Radius �� ������� ���������

---

## ?? ��������� ������������

- **`TIME_INTEGRATION.md`** - ������ ����������� �� ����������
- **`INSTALL.md`** - ��������� � ������ ������
- **`QUICKSTART.md`** - ������� ���������
- **`README.md`** - ������ ������������
- **`BlueprintExamples.md`** - 10+ ������� ��������

---

## ? �����

? **����������**: ��������������  
? **������������������**: < 0.5ms ��� 100 NPC  
? **�������� �������������**: 3 ����  
? **��������**: ������ ��������� Blueprint  

**���������� ����� ������� ����! ??**
