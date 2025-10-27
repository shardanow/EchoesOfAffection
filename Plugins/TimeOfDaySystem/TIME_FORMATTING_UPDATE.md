# Time of Day System - Time Formatting Updates

**����**: 25 ������� 2024, 14:30  
**������**: 1.3 (Time Formatting Update)  
**������**: ? **������� ���������**

---

## ?? ��� ���� ���������

### 1. **��������� 12-�������� ������� � AM/PM** ?

#### ����� ����� � Subsystem:
```cpp
FString GetFormattedTime12Hour(bool bIncludeSeconds = false) const;
```

**������� ������:**
- `3:45 PM` - ��� ������
- `3:45:23 PM` - � ���������
- `12:00 AM` - �������
- `12:00 PM` - �������

#### ������� ����� �������:
```cpp
// � UTimeOfDayWidget::UpdateWidgetElementsSimplified()
if (bUse12HourFormat)
{
    TimeString = TimeSubsystem->GetFormattedTime12Hour(bShowSeconds);
}
else
{
  TimeString = TimeSubsystem->GetFormattedTime(bShowSeconds);
}
```

**������ `bUse12HourFormat` ������� ��������!**

---

### 2. **������� �������** ?

#### ��������� �������� �������:

**12 ������� ��������� �� �������:**

| ����� | ������ |
|-------|--------|
| **Spring** (�����) | January, February, March |
| **Summer** (����) | April, May, June |
| **Autumn** (�����) | July, August, September |
| **Winter** (����) | October, November, December |

**����������:**
- ������ ����� = 30 ����
- ������ ����� = 3 ������ = 90 ����
- 1 ��� = 12 ������� = 360 ����

#### ������� ������ ����:

**��:**
```
Year 1, Spring, Day 15 (Monday)
```

**�����:**
```
15 March, Year 1 (Monday)
```

**������:** `<����> <�����>, Year <���> (<���� ������>)`

---

### 3. **��������� �������� �������** ?

#### ��������� � TimeOfDaySettings:

```cpp
/** Custom month names (12 names for 12 months). If empty, uses default names. */
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Calendar")
TArray<FString> MonthNames;
```

**��� ������������:**

1. �������� ��� DataAsset `DA_TimeOfDay_Default`
2. � ��������� **Calendar** ������� **Month Names**
3. �������� 12 ��������� �������� (��� �������� ������ ��� �����������)

**������ ��������� �������� (�������):**
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

---

### 4. **��������� Blueprint Library �������** ?

```cpp
UFUNCTION(BlueprintPure, Category = "Time of Day")
static FString GetFormattedTime12Hour(const UObject* WorldContextObject, bool bIncludeSeconds = false);
```

**������������� � Blueprint:**
```
Get Formatted Time 12 Hour (World Context = Self, Include Seconds = false)
  ?
  ?
Returns: "3:45 PM"
```

---

## ?? ����������� ������

### ���������� ������ � ���:

```cpp
const int32 MonthIndex = (CurrentTime.DayOfSeason - 1) / 30;  // 0-2
const int32 DayOfMonth = ((CurrentTime.DayOfSeason - 1) % 30) + 1;  // 1-30

// ���������� ������ ������ (0-11)
const int32 AbsoluteMonthIndex = (static_cast<int32>(CurrentTime.Season) * 3) + MonthIndex;
```

**�������:**
- `DayOfSeason = 1` ? `MonthIndex = 0`, `DayOfMonth = 1` ? "1 January"
- `DayOfSeason = 45` ? `MonthIndex = 1`, `DayOfMonth = 15` ? "15 February"
- `DayOfSeason = 90` ? `MonthIndex = 2`, `DayOfMonth = 30` ? "30 March"

### ����������� 12-�������� �������:

```cpp
int32 Hour12 = CurrentTime.Hour % 12;
if (Hour12 == 0)
{
    Hour12 = 12; // �������/������� ������������ ��� 12
}

const TCHAR* AMPM = (CurrentTime.Hour < 12) ? TEXT("AM") : TEXT("PM");
```

**������� �����������:**

| 24-��� | 12-��� |
|--------|--------|
| 0 | 12 AM |
| 1 | 1 AM |
| 11 | 11 AM |
| 12 | 12 PM |
| 13 | 1 PM |
| 23 | 11 PM |

---

## ?? �������������

### � C++ ����:

```cpp
UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();

// 24-������� ������
FString Time24 = TimeSystem->GetFormattedTime(false);  // "15:45"
FString Time24Sec = TimeSystem->GetFormattedTime(true);  // "15:45:23"

// 12-������� ������ � AM/PM
FString Time12 = TimeSystem->GetFormattedTime12Hour(false);  // "3:45 PM"
FString Time12Sec = TimeSystem->GetFormattedTime12Hour(true);  // "3:45:23 PM"

// ���� � ��������
FString Date = TimeSystem->GetFormattedDate();  // "15 March, Year 1 (Monday)"
```

### � Blueprint:

**������� 1: ����� Subsystem**
```
Get Time of Day Subsystem
  ?
  ?
Get Formatted Time 12 Hour (Include Seconds = false)
  ?
  ?
Print String ? "3:45 PM"
```

**������� 2: ����� Blueprint Library (�����)**
```
Get Formatted Time 12 Hour (World Context = Self, Include Seconds = false)
  ?
  ?
Print String ? "3:45 PM"
```

### � UMG Widget:

```cpp
// � ����� TimeOfDayWidget Blueprint:

// ���������� �����:
bUse12HourFormat = true  // ��� 12-�������� �������
bUse12HourFormat = false  // ��� 24-�������� �������

// ������ ������������� ���������� ���������� ������!
```

---

## ?? ��������� ��������� �������

### ��� 1: �������� DA_TimeOfDay_Default

### ��� 2: � ��������� Calendar ������� Month Names

### ��� 3: �������� 12 ��������:

**������� A: �������� ������** ? ������������ ����������� English ��������

**������� B: ������� ��������**
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

**������� C: ������� ��������**
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

**?? �����:** ������ ���� ����� **12 ��������**, ����� ������������ �����������!

---

## ?? ��������� �����

### 1. TimeOfDaySubsystem.h
- ? �������� `GetFormattedTime12Hour()`

### 2. TimeOfDaySubsystem.cpp
- ? ���������� `GetFormattedTime12Hour()`
- ? ������� `GetFormattedDate()` � ��������
- ? ��������� ��������� ��������� �������� �������

### 3. TimeOfDaySettings.h
- ? �������� `TArray<FString> MonthNames`

### 4. TimeOfDayWidget.cpp
- ? ������������� `GetFormattedTime12Hour()` ����� `bUse12HourFormat = true`
- ? ��������� `GetFormattedTimeString()` ��� ��������� 12-�������� �������

### 5. TimeOfDayBlueprintLibrary.h/.cpp
- ? ��������� ������� `GetFormattedTime12Hour()`

---

## ? ������

```
Result: Succeeded
Total execution time: 8.52 seconds
Files compiled: 15
```

---

## ?? ������� ������

### �����:

**24-������� ������:**
```
00:00 - �������
06:30 - ������ ����
12:00 - �������
15:45 - ����
18:30 - �����
23:59 - ������� ����
```

**12-������� ������ � AM/PM:**
```
12:00 AM - �������
6:30 AM - ������ ����
12:00 PM - �������
3:45 PM - ����
6:30 PM - �����
11:59 PM - ������� ����
```

### ����:

**����������� ��������:**
```
1 January, Year 1 (Monday)
15 March, Year 1 (Wednesday)
30 June, Year 2 (Friday)
25 December, Year 5 (Sunday)
```

**��������� ������� ��������:**
```
1 Wintermoon, Year 1 (Monday)
15 Stormwind, Year 1 (Wednesday)
30 Sunhigh, Year 2 (Friday)
25 Starfall, Year 5 (Sunday)
```

---

## ?? ��� ������?

### ��������� ���������:

1. **�����������** - ��������� FText ��� �������� �������� �������
2. **�������������� ���** - ��������� ������� (DD/MM/YYYY, MM/DD/YYYY, etc.)
3. **�������� �������� �������** - Jan, Feb, Mar ������ January, February, March
4. **���������** - ����������� ���� � ���������� ("Christmas Day")
5. **���������� ����** - ��������� 366-������� �����

---

## ?? ������������

���������� �����:
- **README.md** - ��������� ���������� � ��������������
- **QUICKSTART.md** - ������� ������������� 12-�������� �������
- **CONTENT_GUIDE.md** - ������� � ��������

---

**������**: ? **READY TO USE**  
**������**: 1.3  
**����������**: �������  

**Happy Game Development!** ???
