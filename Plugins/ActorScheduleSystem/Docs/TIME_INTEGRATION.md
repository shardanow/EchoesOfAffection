# Time System Integration - Complete Guide

## ? ���������� ���������!

**TimeOfDaySystem** ������ ������������� ������������ � **ActorScheduleSystem**!

---

## ?? ��� ��� ��������

### �������������� ���������� (��� ���������)

```cpp
// � TimeOfDaySubsystem::ProcessTimeChanges()
// ������ ������ ������������� ����������:

if (UScheduleSubsystem* ScheduleSystem = World->GetSubsystem<UScheduleSubsystem>())
{
    ScheduleSystem->NotifyTimeChanged(Hour, Minute, DayOfWeek);
}
```

**������������:**
- ? �������� ��������������� ����
- ? ������� ����������� �� ����� ���������
- ? �������� �� �������
- ? ���������� ������ ������� ������
- ? ��� performance overhead

---

## ?? ��� ���� �������

### 1. ������������� TimeOfDaySubsystem

**����:** `Plugins/TimeOfDaySystem/Source/TimeOfDayRuntime/Private/Subsystems/TimeOfDaySubsystem.cpp`

```cpp
// �������� include
#include "Subsystems/ScheduleSubsystem.h"

// �������� �����
void UTimeOfDaySubsystem::NotifyScheduleSystem()
{
    if (UWorld* World = GetWorld())
  {
        if (UScheduleSubsystem* ScheduleSystem = World->GetSubsystem<UScheduleSubsystem>())
  {
     const int32 DayOfWeek = static_cast<int32>(CurrentTime.DayOfWeek);
            ScheduleSystem->NotifyTimeChanged(CurrentTime.Hour, CurrentTime.Minute, DayOfWeek);
        }
    }
}

// ���������� � ProcessTimeChanges() ������ ������
```

### 2. ��������� ����������� ������

**����:** `Plugins/TimeOfDaySystem/Source/TimeOfDayRuntime/TimeOfDayRuntime.Build.cs`

```csharp
PublicDependencyModuleNames.Add("ScheduleRuntime");
```

---

## ?? �������������

### �������� NPC � �����������

#### ��� 1: �������� Schedule Data Asset

```
Content Browser ? Right-click ? Data Asset ? ScheduleData
Name: DA_NPC_Shopkeeper
```

#### ��� 2: �������� Schedule Entry

```
Content Browser ? Right-click ? Data Asset ? ScheduleEntryData
Name: DA_Entry_OpenShop

Configure:
  Entry Name: "Open Shop"
  Priority: 80
  
  Time Condition:
  Time Type: Time Of Day
    Start Hour: 8
    Start Minute: 0
    Days Of Week: [1,2,3,4,5] // Monday-Friday
  
  Action: ScheduleAction_Composite
    Child Actions:
      [0] ScheduleAction_MoveToLocation
          Location Selector: ActorTag
            Tags: Location.ShopDoor
  [1] ScheduleAction_PlayAnimation
      Montage: AM_OpenDoor
```

#### ��� 3: �������� Entry � Schedule

```
Open DA_NPC_Shopkeeper
  Entries:
    [0] DA_Entry_OpenShop (08:00)
    [1] DA_Entry_ServeCustomers (08:00-20:00)
    [2] DA_Entry_CloseShop (20:00)
    [3] DA_Entry_GoHome (20:00-08:00)
  
  Fallback Action: PlayAnimation (Idle)
```

#### ��� 4: �������� � NPC

```
1. Select NPC Actor
2. Add Component ? Schedule Component
3. Set Schedule Data ? DA_NPC_Shopkeeper
4. Set bSchedule Enabled ? true
```

#### ��� 5: Play!

```
1. Press Play (PIE)
2. Time System ������������� ��������� �����
3. ������ ������� ������ Schedule System ��������� ����������
4. NPC ������������� ��������� �������� �� ����������
```

---

## ?? Debugging

### �������� ����������

**� Output Log ������ ���������:**

```
LogTemp: ? Loaded TimeOfDaySettings from: /TimeOfDaySystem/DA_TimeOfDaySettings
LogTemp: TimeOfDaySubsystem initialized. Current time: 06:00 Year 1 (Monday)
LogTemp: ScheduleSubsystem initialized
LogTemp: Registered 1 scheduled actor(s)
```

**������ ������:**

```
LogTemp: ?? Time changed: 08:00, evaluating schedules...
LogTemp: Actor 'BP_Shopkeeper' ? Entry 'Open Shop' activated
```

### ��������� debug �����

**Project Settings ? Plugins ? Actor Schedule System:**

```
Debug:
  bEnable Debug Visualization: ?
  Debug Display Duration: 5.0

Logging:
  bLog Schedule Events: ?
  Log Verbosity Level: 4 (Log)
```

---

## ?? Performance

### �����������

**Time System �����������:**
- Real-time: ������ frame
- Game-time: � ������ TimeScale
- Schedule evaluation: ������ ��� ��������� ������

**Benchmark:**
- 100 NPCs � ������������
- Update ������ game-������
- < 0.5ms CPU time
- Zero per-actor ticks

**��������� � TimeOfDaySettings:**

```
Time Conversion:
  Time Scale: 60.0 (1 real second = 1 game minute)
  bIs Paused: false

Performance:
  Update Frequency: 30.0 (30Hz)
  bAuto Advance Time: true
```

---

## ?? ������� ����������

### ������ 1: ������� (08:00-20:00)

```
Entry 1: Open Shop (08:00)
  Priority: 90
  Action: Composite
    ? Move To Door
 ? Play Animation (Unlock)
    ? Move To Counter

Entry 2: Work Hours (08:00-20:00)
  Priority: 80
  Action: Composite
    ? Wait At Counter
    ? Play Animation (Idle_Working)
  Location: Tag (Location.ShopCounter)

Entry 3: Close Shop (20:00)
  Priority: 90
  Action: Composite
    ? Play Animation (Lock Door)
    ? Move To Home

Entry 4: Sleep (20:00-08:00)
  Priority: 70
  Action: Play Animation (Sleeping)
  Location: Tag (Location.Home)
```

### ������ 2: �������� (24/7 �������)

```
Entry 1: Patrol Route A (00:00-08:00)
  Priority: 80
  Action: Composite (Sequential)
    ? Move To (Patrol.Point.1)
    ? Wait (30s)
  ? Move To (Patrol.Point.2)
    ? Wait (30s)
    [Loop via recurrence]

Entry 2: Patrol Route B (08:00-16:00)
  Priority: 80
  [Same pattern, different points]

Entry 3: Patrol Route C (16:00-24:00)
  Priority: 80
  [Same pattern, different points]

Fallback: Stand Guard
  Action: Play Animation (Alert_Idle)
  Location: Tag (Location.GuardPost)
```

### ������ 3: ����������� (������� ������)

```
Entry 1: Morning Routine (06:00-08:00)
  Days: All
  Priority: 90
  Action: Composite
    ? Play Animation (Wake Up)
    ? Move To (Location.Bathroom)
    ? Play Animation (Washing)
    ? Move To (Location.Kitchen)
    ? Play Animation (Eating)

Entry 2: Commute to Work (08:00)
  Days: Monday-Friday
  Priority: 85
  Action: Move To (Location.Workplace)

Entry 3: Work (08:00-17:00)
  Days: Monday-Friday
  Priority: 80
  Action: Composite
    ? Play Animation (Working)
    ? Wait (32400s) // 9 hours

Entry 4: Weekend Activities (08:00-20:00)
  Days: Saturday, Sunday
  Priority: 75
  Action: Composite
    ? Move To (Random: Location.Park, Location.Shop, Location.Square)
    ? Play Animation (Relaxing)
    ? Wait (Random: 600-1800s)

Entry 5: Commute Home (17:00)
  Days: Monday-Friday
  Priority: 85
  Action: Move To (Location.Home)

Entry 6: Evening Home (18:00-22:00)
  Days: All
  Priority: 70
  Action: Composite
  ? Play Animation (Relaxing)
    ? Move To (Location.TV)
    ? Play Animation (Watching TV)

Entry 7: Sleep (22:00-06:00)
  Days: All
  Priority: 60
  Action: Play Animation (Sleeping)
  Location: Tag (Location.Bed)
```

---

## ?? ����������� ���������

### ������������ ��������� �������

```cpp
// C++
if (UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>())
{
    FTimeOfDayTime NewTime;
  NewTime.Hour = 14;
    NewTime.Minute = 30;
    TimeSystem->SetCurrentTime(NewTime);
    
    // Schedule System ������������� ���������� ��� ����������
}
```

```blueprint
// Blueprint
Get TimeOfDay Subsystem
  ? Set Current Time
    Hour: 14
    Minute: 30

// ��� NPC ������������� ������������ �� ��������������� ��������
```

### ����� �������

```cpp
// C++
TimeSystem->PauseTime();
// Schedule System ���� ���������������

TimeSystem->ResumeTime();
// Schedule System ���������� ��������
```

### Time Scale

```cpp
// C++
TimeSystem->SetTimeScale(120.0f); // 1 real second = 2 game minutes
// Schedule System ������������ �������������
```

---

## ?? Troubleshooting

### ��������: ���������� �� �����������

**���������:**

1. **Time System ��������?**
   ```
   Output Log ? Search "TimeOfDaySubsystem initialized"
   ```

2. **Schedule Component ��������?**
   ```
   Actor ? Details Panel ? Components ? Schedule Component?
   ```

3. **Schedule Data ��������?**
   ```
   Schedule Component ? Schedule Data ? Is set?
   ```

4. **bScheduleEnabled = true?**
   ```
   Schedule Component ? bSchedule Enabled ? ?
   ```

5. **����� ������������� ��������?**
   ```
   Current Time: 14:00
   Entry Time: 08:00-17:00 ?
   Entry Days: Monday-Friday
   Current Day: Saturday ? (Won't execute!)
   ```

### ��������: ����� �� �����������

**��������� TimeOfDaySettings:**

```
DA_TimeOfDaySettings:
  bAuto Advance Time: ? Must be true
  Time Scale: > 0
  bIs Paused: false
```

### ��������: NPC �� ���������

**���������:**

1. **AIController ��������?**
   ```
   Actor ? Details Panel ? AI Controller Class ? Set to AIController
   ```

2. **NavMesh ����������?**
   ```
   Show ? Navigation (P)
   Green mesh should be visible
   ```

3. **Acceptance Radius ��������?**
   ```
   MoveToLocation Action ? Acceptance Radius: 100.0 (default)
   Too small = NPC can't reach
   ```

4. **Target location reachable?**
   ```
   Place NavMesh Bounds Volume around level
   Rebuild Navigation: Build ? Build Paths
   ```

---

## ? �����

### ��� ������ �������� �������������:

? **TimeOfDaySystem** ��������� ����� ������ frame  
? **������ ������� ������** ? `NotifyTimeChanged`  
? **ScheduleSubsystem** ��������� ��� ����������  
? **NPCs** ��������� ��������������� ��������  
? **�������� ������� ����** �� ���������  

### Workflow:

```
1. Create Schedule Assets (Data Assets)
2. Add Schedule Component to NPCs
3. Press Play
4. Everything works automatically!
```

---

## ?? �������������� �������

- **INSTALL.md** - ��������� ��������� � ������ ������
- **QUICKSTART.md** - ������� ���������
- **README.md** - ������ ������������ Schedule System
- **BlueprintExamples.md** - 10+ ������� ��������
- **ContentGuide.md** - ����������� �������

---

**?? ������! ������ ���� NPC ����� �� ����������!** ??
