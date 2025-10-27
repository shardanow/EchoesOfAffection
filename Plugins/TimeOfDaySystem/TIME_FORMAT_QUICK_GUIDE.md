# ? ������� ���������� - �������������� �������

**������**: 1.3  
**������**: ? ������ � �������������

---

## ?? 12-������� ������ � AM/PM

### � Widget Blueprint:

1. �������� ��� `TimeOfDayWidget` Blueprint
2. ���������� **bUse12HourFormat = true**
3. ������! ����� ����� ������������ ��� "3:45 PM"

### � C++ ����:

```cpp
UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();

// 12-������� ������
FString Time = TimeSystem->GetFormattedTime12Hour(false);  // "3:45 PM"
```

### � Blueprint:

```
Get Time of Day Subsystem ? Get Formatted Time 12 Hour
```

---

## ?? �������� �������

### ����������� (�� ���������):

```
1 January, Year 1 (Monday)
15 March, Year 1 (Wednesday)
30 December, Year 1 (Sunday)
```

### ��������� ��������:

**��� 1:** �������� `DA_TimeOfDay_Default`

**��� 2:** � ��������� **Calendar** ������� **Month Names**

**��� 3:** �������� 12 ��������:

#### ������ 1: �������
```
0: ������
1: �������
2: ����
3: ������
4: ���
5: ����
6: ����
7: ������
8: ��������
9: �������
10: ������
11: �������
```

#### ������ 2: �������
```
0: Wintermoon
1: Frostfall
2: Stormwind
3: Springtide
4: Blossomrise
5: Sunhigh
6: Harvest
7: Goldleaf
8: Amberfall
9: Frostscar
10: Darknight
11: Starfall
```

**���������:**
```
15 ����, Year 1 (Wednesday)
15 Stormwind, Year 1 (Wednesday)
```

---

## ?? ������� �������������

### ������ 1: UI Clock Widget

```
Event Tick
 ?
    ?
Get Formatted Time 12 Hour (World Context = Self)
    ?
    ?
Set Text (ClockText) ? "11:23 AM"
```

### ������ 2: Date Display

```
Event Tick
    ?
    ?
Get Formatted Date (World Context = Self)
    ?
    ?
Set Text (DateText) ? "15 March, Year 1 (Monday)"
```

### ������ 3: Full Time & Date Widget

```
TimeText: "3:45 PM"
DateText: "15 March, Year 1 (Monday)"
SeasonText: "Spring"
PhaseText: "Afternoon"
```

---

## ?? ��������� � DataAsset

### DA_TimeOfDay_Default

```
Calendar:
  ?? Days Per Season: 90 (�������������: 3 ������ ? 30 ����)
  ?? Seasons Per Year: 4
  ?? Days Per Week: 7
  ?? Month Names: [12 ��������] (�����������)

Initial Game Time:
  ?? Year: 1
  ?? Season: Spring
  ?? Day of Season: 15  ? ���� 15 = �������� ������ 1 (February)
  ?? Hour: 10
  ?? Minute: 30
  ?? Second: 0
```

**������������:**
- `DayOfSeason 1-30` = ������ ����� ������
- `DayOfSeason 31-60` = ������ ����� ������
- `DayOfSeason 61-90` = ������ ����� ������

---

## ?? ������� ��������

### �����:

| ������ | ������ | ��� �������� |
|--------|--------|--------------|
| 24-������� | `15:45` | `GetFormattedTime(false)` |
| 24-������� � ���. | `15:45:23` | `GetFormattedTime(true)` |
| 12-������� | `3:45 PM` | `GetFormattedTime12Hour(false)` |
| 12-������� � ���. | `3:45:23 PM` | `GetFormattedTime12Hour(true)` |

### ����:

| ������� | ������ | ��� |
|---------|--------|-----|
| ���� ������ | `15` | 1-30 |
| �������� ������ | `March` | String (���������������) |
| ��� | `Year 1` | int32 |
| ���� ������ | `Monday` | Enum |

---

## ? ������!

������ � ��� ����:
- ? 12-������� ������ � AM/PM
- ? ������� ������� (12 �������)
- ? ��������� �������� �������
- ? �������� ������ ����

**������ ���������� `bUse12HourFormat = true` � �������� �������� �������!**

---

**������ ����������:** `TIME_FORMATTING_UPDATE.md`  
**������ ������������:** `README.md`

**Happy Formatting!** ???
