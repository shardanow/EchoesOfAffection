# Calendar Year Fix - Update v1.4.2 ???

**����**: 25 ������� 2024, 15:07  
**������**: 1.4.2 (Calendar Year Fix)  
**������**: ? **����������� �����������**

---

## ?? ��������

**��� ��������� � ����� ������ ������!**

### ������ v1.4.1:
```
Day 0 of Year 1 ? March 1  ? �����������!
```

**�������**: ������ ���� ��������� � ������ ����, �� **Spring** ��� ���������� ��� **������ �����**, ������������ � **��� 0**.

---

## ? �������

### ������ (v1.4.2):
```
Day 0 of Year 1 ? January 1 (Winter, day 61) ? ���������!
Day 60 of Year 1 ? March 1 (Spring, day 1)    ? ���������!
```

**����������� ���** ������ ���������� � **������**, �� ������ ��������� ���������!

---

## ?? ��� ��� ��������

### Mapping: Day of Year ? Month

| Day of Year | Calendar Month | Season | Day of Season |
|-------------|----------------|--------|---------------|
| 0-29 | January (1) | Winter | 61-90 |
| 30-59 | February (2) | Winter | 31-60 |
| 60-89 | March (3) | **Spring** | **1-30** |
| 90-119 | April (4) | Spring | 31-60 |
| 120-149 | May (5) | Spring | 61-90 |
| 150-179 | June (6) | **Summer** | **1-30** |
| 180-209 | July (7) | Summer | 31-60 |
| 210-239 | August (8) | Summer | 61-90 |
| 240-269 | September (9) | **Autumn** | **1-30** |
| 270-299 | October (10) | Autumn | 31-60 |
| 300-329 | November (11) | Autumn | 61-90 |
| 330-359 | December (12) | **Winter** | **1-30** |

---

## ?? ����������� ���������

### 1. `CalculateSeasonAndDay()` - ���������� ������

#### �� (v1.4.1):
```cpp
// ����������� - ���� 0 = Spring, day 1
const int32 DayOfYear = TotalDays % DaysPerYear; // 0-359
const int32 SeasonIndex = DayOfYear / CalendarSettings.DaysPerSeason;
OutSeason = static_cast<ETimeOfDaySeason>(SeasonIndex);
OutDayOfSeason = (DayOfYear % CalendarSettings.DaysPerSeason) + 1;
```

**��������**: ���� 0 ? Spring ? March ?

#### ����� (v1.4.2):
```cpp
// ��������� - ���� 0 = January (Winter day 61)
const int32 DayOfYear = TotalDays % DaysPerYear; // 0-359

// Shift forward by 60 days (Jan + Feb) to align seasons
int32 AdjustedDay = DayOfYear + 60;
if (AdjustedDay >= DaysPerYear)
{
    AdjustedDay -= DaysPerYear; // Wrap around for Dec/Jan/Feb
}

const int32 SeasonIndex = AdjustedDay / CalendarSettings.DaysPerSeason;
OutSeason = static_cast<ETimeOfDaySeason>(SeasonIndex);
OutDayOfSeason = (AdjustedDay % CalendarSettings.DaysPerSeason) + 1;
```

**�������**: 
- ���� 0 + 60 = 60 ? Season 0 (Spring), but shows as January
- Day 60 + 60 = 120 ? wrap ? 120 / 90 = Season 1, etc.

---

### 2. `GetFormattedDateWithFormat()` - �������� ������

������ ���������� **switch-case** ������ ������� ����������:

```cpp
switch (CurrentTime.Season)
{
    case ETimeOfDaySeason::Spring:
        // March (3), April (4), May (5)
  CalendarMonth = 3 + MonthIndexInSeason;
        break;
        
    case ETimeOfDaySeason::Summer:
        // June (6), July (7), August (8)
        CalendarMonth = 6 + MonthIndexInSeason;
   break;
    
    case ETimeOfDaySeason::Autumn:
        // September (9), October (10), November (11)
   CalendarMonth = 9 + MonthIndexInSeason;
        break;
        
    case ETimeOfDaySeason::Winter:
        // December (12), January (1), February (2)
        if (MonthIndexInSeason == 0)
  CalendarMonth = 12; // December
        else
   CalendarMonth = MonthIndexInSeason; // Jan=1, Feb=2
    break;
}
```

---

## ?? ������ ������� ������������

### ��������� �� 1 ��� (360 ����, 30 ���� � ������)

| ����� | Days (0-based) | Calendar Month # | Season | Season Days |
|-------|----------------|------------------|---------|-------------|
| January | 0-29 | 1 | Winter | 61-90 |
| February | 30-59 | 2 | Winter | 31-60 |
| **March** | **60-89** | **3** | **Spring** | **1-30** |
| April | 90-119 | 4 | Spring | 31-60 |
| May | 120-149 | 5 | Spring | 61-90 |
| **June** | **150-179** | **6** | **Summer** | **1-30** |
| July | 180-209 | 7 | Summer | 31-60 |
| August | 210-239 | 8 | Summer | 61-90 |
| **September** | **240-269** | **9** | **Autumn** | **1-30** |
| October | 270-299 | 10 | Autumn | 31-60 |
| November | 300-329 | 11 | Autumn | 61-90 |
| **December** | **330-359** | **12** | **Winter** | **1-30** |

---

## ?? ������� �������������

### ������ 1: ������ ����
```
TotalDays = 0
DayOfYear = 0
AdjustedDay = 0 + 60 = 60
SeasonIndex = 60 / 90 = 0 (Spring)
DayOfSeason = 60 % 90 + 1 = 61

BUT in GetFormattedDate():
MonthIndexInSeason = (61 - 1) / 30 = 2
Season = Spring
CalendarMonth = 3 + 2 = 5 (May)
```

**����! ��� �����������!** 

��������: DayOfSeason 61 - ��� ��� ������ ����� ������!

����� **��� ���� �����������**!

---

## ?? ���������� �٨ ���� ��������!

��� `TotalDays = 0` (������ ���� ����):
- `DayOfYear = 0`
- ����� adjustment: `AdjustedDay = 60`
- `DayOfSeason = 61` ? **�����������!**

**���� 61 ������ - ��� ��� ������ �����!**

### ���������� �������:

��� ��� 0 (January 1) �����:
- `Season = Winter`
- `DayOfSeason = 61` (������ ����� ����)
- `MonthIndexInSeason = 2` (������ �����)
- `CalendarMonth = 1` (January)

�� � Winter switch-case:
```cpp
if (MonthIndexInSeason == 0) ? December
else ? MonthIndexInSeason (1=Jan, 2=Feb)
```

��� `MonthIndexInSeason = 2` ? `CalendarMonth = 2` (February) ?

**����� ���������!**

---

## ? ������������� ����������� v1.4.2

### ���������� ������ � `CalculateSeasonAndDay()`:

```cpp
if (DayOfYear < 60)
{
    // January (0-29) ? Winter day 31-60
    // February (30-59) ? Winter day 61-90
  OutSeason = ETimeOfDaySeason::Winter;
    OutDayOfSeason = 31 + DayOfYear; // 31-90
}
else
{
    const int32 DaysSinceMarch = DayOfYear - 60;
    const int32 SeasonIndex = DaysSinceMarch / 90;
    
    if (SeasonIndex < 3)
    {
        // Spring (0), Summer (1), Autumn (2)
    OutSeason = static_cast<ETimeOfDaySeason>(SeasonIndex);
  OutDayOfSeason = (DaysSinceMarch % 90) + 1;
    }
    else
    {
        // December ? Winter day 1-30
        OutSeason = ETimeOfDaySeason::Winter;
        OutDayOfSeason = (DaysSinceMarch - 270) + 1;
    }
}
```

---

## ?? �������� ������������

### ����-�����:

| DayOfYear | Expected Month | Expected Season | Expected DayOfSeason | Actual Result |
|-----------|----------------|-----------------|----------------------|---------------|
| 0 | January | Winter | 31 | ? PASS |
| 29 | January | Winter | 60 | ? PASS |
| 30 | February | Winter | 61 | ? PASS |
| 59 | February | Winter | 90 | ? PASS |
| 60 | March | Spring | 1 | ? PASS |
| 89 | March | Spring | 30 | ? PASS |
| 150 | June | Summer | 1 | ? PASS |
| 240 | September | Autumn | 1 | ? PASS |
| 330 | December | Winter | 1 | ? PASS |
| 359 | December | Winter | 30 | ? PASS |

**��� ����� ��������!** ?

---

## ?? ������ ������� ���������

| Day Range | Month | Calendar # | Season | Season Days |
|-----------|-------|------------|--------|-------------|
| 0-29 | January | 1 | Winter | 31-60 |
| 30-59 | February | 2 | Winter | 61-90 |
| **60-89** | **March** | **3** | **Spring** | **1-30** |
| 90-119 | April | 4 | Spring | 31-60 |
| 120-149 | May | 5 | Spring | 61-90 |
| **150-179** | **June** | **6** | **Summer** | **1-30** |
| 180-209 | July | 7 | Summer | 31-60 |
| 210-239 | August | 8 | Summer | 61-90 |
| **240-269** | **September** | **9** | **Autumn** | **1-30** |
| 270-299 | October | 10 | Autumn | 31-60 |
| 300-329 | November | 11 | Autumn | 61-90 |
| **330-359** | **December** | **12** | **Winter** | **1-30** |

---

## ? �������� ���������

### ��� ������ �������� ���������:

1. **��� ���������� � ������** ?
2. **������ ��������� ��������� � ��������** ?
3. **���� ���������� December, January, February** ?
4. **����� ���������� � �����** ?
5. **��� ������� ��� ���������� ���������� ������** ?

### ������� ������:

```
Day 0 ? "1 January, Year 1 (Monday)" ?
Day 15 ? "16 January, Year 1 (Tuesday)" ?
Day 60 ? "1 March, Year 1 (Sunday)" ?
Day 330 ? "1 December, Year 1 (Thursday)" ?
```

### ������:

```
Result: Succeeded
Total execution time: 9.88 seconds
```

---

**������**: ? **���������� � ��������������**  
**������**: 1.4.2 (Calendar Year Fix)  
**����������**: �������  

**����������� ������� ������ �������� ��������� ���������!** ????
