# �����: ��� ��������� ������ ������� ����������

## ?? ������ ����� �����

�� ��������������� ����� �����:

```
[2025.10.27-14.13.47:222][287]LogTemp: Schedule Subsystem Initialized
[2025.10.27-14.13.47:222][287]LogTemp: TimeOfDaySubsystem initialized. Current time: Year 1, Winter Day 31, 06:00:00
[2025.10.27-14.13.47:337][287]LogTemp: TimeOfDaySubsystem initialized with settings. Time: Year 1, Winter Day 31, 10:00:00
```

### ? ��� ��������:

1. **ScheduleSubsystem** - ��������������� ?
2. **TimeOfDaySubsystem** - ��������������� ?

### ? ��� �� ��������:

1. **��� ���������� ����� ���������** ?
2. **��� ����������� ����������� ����������** ?
3. **��� ����������� �� ��������� �������** ?

---

## ?? ��������: ������� �� �������!

TimeOfDaySystem ��������, ScheduleSystem ��������, �� **��� �� �������� ���� � ������**.

### ������ ������ �� �����������?

ScheduleSystem **�� �����**, ��� ����� ����������, ������ ���:
- TimeOfDay ������ �����: `06:00` ? `10:00`
- ScheduleSystem **�� ��������** ��� �����������
- ���������� **�� �����������**
- ������ **�� �����������**

---

## ? �������: �������� ��������� ����������

### ��� 1: ������� GameState Blueprint

1. � Content Browser ������� ��� GameState Blueprint
2. ���� ��� ��� - �������� �����: `Blueprint Class` ? `GameStateBase`

### ��� 2: �������� ���������

1. �������� GameState Blueprint
2. Click **Add Component**
3. ������� `TimeOfDayScheduleIntegration`
4. �������� ���
5. **Compile** � **Save**

### ��� 3: ���������� GameState � GameMode

���� ������� ����� GameState:
1. �������� ��� GameMode Blueprint
2. � ���������� ������� `Game State Class`
3. ���������� ��� ����� GameState
4. Compile � Save

### ��� 4: ������������� ����

����� ����������� �� ������ �������:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   - Ready to register schedule components

? TimeOfDayScheduleIntegration: Successfully connected!
   - Schedule system will be notified on every hour change
   - Initial time: 10:00 (Weekday: 1)
```

---

## ?? ��� �� ������ ������ ����� �����������

### ��� ������� ����:

```
[LogTemp] ? Schedule Subsystem Initialized
[LogTemp]    - Initial time: 08:00 (Day 1)
[LogTemp]    - Ready to register schedule components

[LogTemp] ? TimeOfDayScheduleIntegration: Successfully connected!
[LogTemp]    - Schedule system will be notified on every hour change
[LogTemp]    - Initial time: 10:00 (Weekday: 1)
```

### ����� NPC ���������� � ����:

```
[LogTemp] ?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_1 (Total registered: 1)
```

### ������ ������� ���:

```
[LogTemp] ? Schedule System: Time changed to 11:00 (Day 1)
[LogTemp]    - Evaluating 1 registered components
```

### ��� ���������� ������ (� Verbose ������):

```
[LogTemp]    ? Evaluated 1 schedule components
[LogTemp] ?? Schedule: Actor 'BP_NPC_Shopkeeper_C_1' - Starting action 'Open Shop' (09:00-18:00)
```

---

## ?? ��������: �������� �� ����������?

### ���� 1: �������� �����

��������� ���� � ������� � Output Log:
- `TimeOfDayScheduleIntegration: Successfully connected!`

**����?** ? ? ���������� ��������  
**���?** ? ? ��������� �� �������� ��� �� �������

### ���� 2: ��������� �������

��������� 1 ������� ��� � �������:
- `Schedule System: Time changed to XX:XX`

**����?** ? ? ����� ���������� � ������� ����������  
**���?** ? ? ���������� �� ��������

### ���� 3: ����������� NPC

���������� ����� � `ScheduleComponent` �� ������, �������:
- `Registered schedule component for actor: XXX`

**����?** ? ? ���������� ��������������  
**���?** ? ? ��������� �� �������� ��� �� �������

---

## ?? �������������� ��������

### �������� ��������� ����

� ������� ���� (������ `~`):
```
Log LogTemp Verbose
```

### �������� ���������� � runtime

� Blueprint ��� C++:
```cpp
UScheduleSubsystem* Subsystem = GetWorld()->GetSubsystem<UScheduleSubsystem>();

int32 TotalActors, ActiveActions, PausedActions;
Subsystem->GetScheduleStats(TotalActors, ActiveActions, PausedActions);

UE_LOG(LogTemp, Log, TEXT("?? Total actors: %d, Active: %d, Paused: %d"), 
    TotalActors, ActiveActions, PausedActions);
```

### ���������, ��������������� �� ���������� NPC

```cpp
// � C++ ��� ����� Blueprint
if (ScheduleComponent && ScheduleComponent->IsRegisteredWithSubsystem())
{
    UE_LOG(LogTemp, Log, TEXT("? Component is registered"));
}
else
{
    UE_LOG(LogTemp, Warning, TEXT("? Component is NOT registered!"));
}
```

---

## ?? ������������

��� ����� ��������� ����������:

- **QUICK_CHECK.md** - ������� �������� (1 ������)
- **DIAGNOSTICS.md** - ������ ����������� (��� �����)
- **TIME_INTEGRATION.md** - ��������� ���������� �������
- **QUICKSTART.md** - ������� ����� � ���������
- **ContentGuide.md** - �������� ����������

---

## ?? �������� ���-����

����� ���������� `TimeOfDayScheduleIntegration` � GameState:

- [ ] ���� "Schedule Subsystem Initialized"
- [ ] ���� "TimeOfDayScheduleIntegration: Successfully connected!"
- [ ] ���� "Registered schedule component for actor: XXX"
- [ ] ���� "Schedule System: Time changed to XX:XX" (������ ���)

**���� ��� 4 ������ ?** ? **������� �������� ���������!** ??

---

## ?? �����!

**����� ������ ������:** ������ �������� `TimeOfDayScheduleIntegration` � GameState.

**��� ����� ����������:**
- TimeOfDay �������� ?
- ScheduleSystem �������� ?
- �� ��� **�� �������** ?
- ������ **�� �����������** ?

**� ���� �����������:**
- TimeOfDay �������� ?
- ScheduleSystem �������� ?
- ���������� �������� ?
- ������ ����������� ?

---

## ?? ��������� ����

1. ? �������� `TimeOfDayScheduleIntegration` � GameState
2. ? ������������� ����
3. ? ��������� ���� (��. ������ "��� �� ������ ������")
4. ? ������� NPC � `ScheduleComponent`
5. ? ��������� `ScheduleData` � �����������
6. ? ��������� ��������� �������
7. ? ��������� ���������� �����

**������!** ������� ��������� �������������. ??
