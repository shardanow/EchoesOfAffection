# ?? ������������� �����: ����������� ������� ����������

## ?? ��� ������

> "��� �� ����� ������ � ������ �� ��� ������� ���������� ����������� ���������, ���� ���������� ���������� ������� � ������ �� ���������� �����������?"

---

## ? ������� �����

**�������� � Output Log (Window ? Developer Tools ? Output Log)**

### ��� �� ������ ������:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   
? TimeOfDaySubsystem initialized. Current time: Year 1, Winter Day 31, 10:00:00

?? Registered schedule component for actor: BP_NPC_1 (Total: 1)

? Schedule System: Time changed to 11:00 (Day 1)
   - Evaluating 1 registered components
```

### ? ��� �� ������ ������:

```
? Schedule Subsystem Initialized         (����)
? TimeOfDaySubsystem initialized...      (����)
? Registered schedule component: ...     (���!)
? Time changed to XX:XX         (���!)
```

---

## ?? ������ ��������

### ? ��� ��������:

| ��������� | ������ |
|-----------|--------|
| ScheduleSubsystem | ? ��������������� |
| TimeOfDaySubsystem | ? ��������������� |
| **���������� �������** | ? **��� ��������!** |

### ? ��� �� ��������:

| �������� | ������� |
|----------|---------|
| ��� ����������� NPC | ��� ������� � `ScheduleComponent` |
| ��� ��������� ������� | ����� �� �������� ��� ��� NPC ��� �������� |

---

## ?? ������ ���������: ���������� ��� ��������!

### ���������� ����� TimeOfDay � Schedule **��������** � ���!

� ����� `TimeOfDaySubsystem.cpp` (������ 305-330, 259):

```cpp
void UTimeOfDaySubsystem::NotifyScheduleSystem()
{
    if (UWorld* World = GetWorld())
    {
      UScheduleSubsystem* ScheduleSystem = World->GetSubsystem<UScheduleSubsystem>();
        
        if (ScheduleSystem && ScheduleSystem->GetWorld())
        {
            ScheduleSystem->NotifyTimeChanged(CurrentTime.Hour, CurrentTime.Minute, DayOfWeek);
        }
    }
}

// ���������� ������������� ������ ������:
if (CurrentTime.Minute != PreviousMinute)
{
    NotifyScheduleSystem(); // ? �����!
}
```

**������� �������������� ����������� �� �����!**

---

## ?? ������ ��� ����� "Time changed"?

### ��������� ������� #1: ��� ������� � ������������

���� ��� ������������������ �����������, ��� �� ���������.

**�������:**
1. �������� NPC Blueprint
2. �������� `ScheduleComponent`
3. �������� `ScheduleData` asset
4. ��������� ��� � ���������
5. ���������� NPC �� ������

### ��������� ������� #2: ����� �� ��������

��������� `DA_TimeOfDaySettings`:
- `Auto Advance Time` = ? True
- `Time Scale` > 0 (��������, 60 ��� �������� �������)
- ����� �� �� �����

---

## ?? ������ ���-���� �����������

### 1?? ������������� ���������

**����� � �����:**
```
Schedule Subsystem Initialized
TimeOfDaySubsystem initialized
```

**��� ������:** ? ����

---

### 2?? ����������� NPC

**����� � �����:**
```
?? Registered schedule component for actor: BP_NPC_1
```

**��� ������:** ? ���

**��������:**
- �������� `ScheduleComponent` � ������
- ��������� `ScheduleData`
- ���������� ����� �� ������ (Drag & Drop � viewport)
- ������������� ����

---

### 3?? ��������� �������

**����� � ����� (������ ������ �������� �������):**
```
? Schedule System: Time changed to 09:00 (Day 1)
   - Evaluating 1 registered components
```

**��� ������:** ? ��� (������ ��� ��� ������������������ �����������)

**��������:**
- ������� ��������� ����� 2??
- ��������� ��������� �������� �������
- ���������, ��� ����� ������������� ���� (`Auto Advance Time = true`)

---

## ?? ��������� ����������

### ��� 1: ������� NPC � �����������

1. **Content Browser** ? **Blueprint Class** ? �������� **Actor** ��� **Character**
2. ��������: `BP_NPC_Shopkeeper`
3. �������� Blueprint

### ��� 2: �������� Schedule Component

1. � **Components** panel ? **Add Component**
2. ������� **Schedule Component**
3. ��������

### ��� 3: ������� Schedule Data

1. **Content Browser** ? **Right Click** ? **Miscellaneous** ? **Data Asset**
2. �������� �����: **Schedule Data**
3. ��������: `DA_Shopkeeper_Schedule`
4. �������� asset

### ��� 4: ��������� ����������

� `DA_Shopkeeper_Schedule`:

1. **Schedule Entries** ? **Add Element**
2. ��������� ������ ������:
   - **Start Hour**: 9
   - **Start Minute**: 0
   - **End Hour**: 18
   - **End Minute**: 0
   - **Action**: �������� ������� `ScheduleAction` (��. ������������)
   - **Active Days**: �������� ��� (��������, ��� �����)

### ��� 5: ��������� Schedule Data

� `BP_NPC_Shopkeeper`:
1. �������� **Schedule Component**
2. � **Details** panel ������� **Schedule Data**
3. ����������: `DA_Shopkeeper_Schedule`
4. **Compile** + **Save**

### ��� 6: ���������� �� ������

1. Drag & Drop `BP_NPC_Shopkeeper` � viewport
2. ���������� ��� �����
3. **Save** �������

### ��� 7: ���������

1. **Play (PIE)**
2. �������� **Output Log**
3. �������:

```
?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_0 (Total registered: 1)
```

? **�����!** NPC ���������������.

### ��� 8: ��������� ��������� �������

��������� 1 ������� ������ (� ����������� �� `Time Scale`).

**������ �������:**
```
? Schedule System: Time changed to 09:01 (Day 1)
   - Evaluating 1 registered components
```

? **������ �����!** ���������� ��������!

---

## ?? �������� ������

**���� ������ ��� ��� ������:**

1. ? `Schedule Subsystem Initialized`
2. ? `TimeOfDaySubsystem initialized`
3. ? `Registered schedule component for actor: XXX`
4. ? `Schedule System: Time changed to XX:XX`

**? ������� �������� ���������!** ??

---

## ? ������� ��������: �������� �� ����������?

### ���� � console (~ ������):

```cpp
// �������� ���������� ����������
Log LogTemp Log
```

����� ��������� 1 ������� ������.

**���� ������** `Time changed to XX:XX` ? **���������� ��������!** ?

---

## ?? ������������

- **CORRECTION_Integration_Already_Exists.md** - ������ ��������� ��� ����������
- **DIAGNOSTICS.md** - ������ �����������
- **QUICK_CHECK.md** - ������� ��������
- **CHECKLIST.md** - ��������� ���-����
- **ContentGuide.md** - �������� ����������
- **QUICKSTART.md** - ������� �����

---

## ?? ����

### ���� ������� ��������:

**��� ������� � `ScheduleComponent`** ? ��� ����������� ? ��� �����

### �������:

1. ������� NPC � `ScheduleComponent`
2. ������� � ��������� `ScheduleData`
3. ���������� �� ������
4. ��������� ����

**���������� ��� �������� �������������!** ������� �������������� ����������� �� �����!

**�����!** ??
