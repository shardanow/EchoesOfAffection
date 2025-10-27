# Date Format Options - Update v1.4

**����**: 25 ������� 2024, 14:45  
**������**: 1.4 (Date Format Options)  
**������**: ? **������� ���������**

---

## ?? ��� ���� ���������

### ����� Enum: `EDateFormat`

������ �� ������ ������� ������ ����������� ���� �� **6 ��������� ���������**!

```cpp
UENUM(BlueprintType)
enum class EDateFormat : uint8
{
    Full,     // 15 March, Year 1 (Monday)
    DotShort,          // 15.03.01
    SlashEuropean,     // 15/03/2001
    SlashAmerican,     // 03/15/2001
    ISO,     // 2001-03-15
    DotEuropean        // 15.03.2001
};
```

---

## ?? ������� ���� ��������

### ��� ����: 15 ����� 2001 ���� (�����������)

| ������ | ������ ������ | �������� |
|--------|---------------|----------|
| **Full** | `15 March, Year 1 (Monday)` | ������ ������ � ��������� ������ � ��� ������ |
| **DotShort** | `15.03.01` | �������� ������ � ������� (DD.MM.YY) |
| **DotEuropean** | `15.03.2001` | ����������� ������ � ������� (DD.MM.YYYY) |
| **SlashEuropean** | `15/03/2001` | ����������� ������ �� ������� (DD/MM/YYYY) |
| **SlashAmerican** | `03/15/2001` | ������������ ������ (MM/DD/YYYY) |
| **ISO** | `2001-03-15` | ISO 8601 ������ (YYYY-MM-DD) |

---

## ?? �������������

### � Widget Blueprint:

1. �������� ��� `TimeOfDayWidget` Blueprint
2. ������� �������� **Date Format**
3. �������� ������ ������ �� ����������� ������:
   - Full (�� ���������)
   - Dot Short
   - Dot European
   - Slash European
   - Slash American
   - ISO

**������ ������������� ������� ����������� ����!**

---

### � C++ ����:

```cpp
UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();

// ������ �������
FString FullDate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::Full);
// "15 March, Year 1 (Monday)"

FString ShortDate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::DotShort);
// "15.03.01"

FString EuroDate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::SlashEuropean);
// "15/03/2001"

FString USADate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::SlashAmerican);
// "03/15/2001"

FString ISODate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::ISO);
// "2001-03-15"
```

---

### � Blueprint:

**������� 1: ����� Subsystem**
```
Get Time of Day Subsystem
  ?
  ?
Get Formatted Date With Format (Format = Dot Short)
  ?
  ?
Print String ? "15.03.01"
```

**������� 2: ����� Blueprint Library (�����)**
```
Get Formatted Date With Format (World Context = Self, Format = ISO)
  ?
  ?
Print String ? "2001-03-15"
```

---

## ?? ��������� �����

### 1. **TimeOfDayTypes.h** - �������� enum
```cpp
UENUM(BlueprintType)
enum class EDateFormat : uint8
{
    Full,
    DotShort,
    SlashEuropean,
    SlashAmerican,
    ISO,
    DotEuropean
};
```

### 2. **TimeOfDaySubsystem.h** - ����� �����
```cpp
UFUNCTION(BlueprintCallable, Category = "Time of Day")
FString GetFormattedDateWithFormat(EDateFormat Format) const;
```

### 3. **TimeOfDaySubsystem.cpp** - ����������
```cpp
FString UTimeOfDaySubsystem::GetFormattedDateWithFormat(EDateFormat Format) const
{
    // ���������� ������ � ���
    const int32 MonthIndex = (CurrentTime.DayOfSeason - 1) / 30;
    const int32 DayOfMonth = ((CurrentTime.DayOfSeason - 1) % 30) + 1;
    const int32 MonthNumber = AbsoluteMonthIndex + 1;
    
    switch (Format)
    {
        case EDateFormat::DotShort:
       return FString::Printf(TEXT("%02d.%02d.%02d"), DayOfMonth, MonthNumber, YearShort);

        case EDateFormat::ISO:
            return FString::Printf(TEXT("%04d-%02d-%02d"), Year, MonthNumber, DayOfMonth);
        
        // ... � �.�.
    }
}
```

### 4. **TimeOfDayWidget.h** - ����� ��������
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
EDateFormat DateFormat = EDateFormat::Full;
```

### 5. **TimeOfDayWidget.cpp** - ������������� �������
```cpp
DateText->SetText(FText::FromString(TimeSubsystem->GetFormattedDateWithFormat(DateFormat)));
```

### 6. **TimeOfDayBlueprintLibrary.h/.cpp** - Blueprint �������
```cpp
UFUNCTION(BlueprintPure, Category = "Time of Day")
static FString GetFormattedDateWithFormat(const UObject* WorldContextObject, EDateFormat Format);
```

---

## ?? ����������� ����������

### ���������� ����:

```cpp
// ����� � ������ (0-2)
const int32 MonthIndex = (CurrentTime.DayOfSeason - 1) / 30;

// ���� ������ (1-30)
const int32 DayOfMonth = ((CurrentTime.DayOfSeason - 1) % 30) + 1;

// ���������� ����� ���� (0-11)
const int32 AbsoluteMonthIndex = (static_cast<int32>(CurrentTime.Season) * 3) + MonthIndex;

// ����� ������ (1-12)
const int32 MonthNumber = AbsoluteMonthIndex + 1;
```

### ������� ����������:

| DayOfSeason | Season | MonthIndex | DayOfMonth | AbsoluteMonth | MonthNumber |
|-------------|--------|------------|------------|---------------|-------------|
| 1 | Spring | 0 | 1 | 0 | 1 (January) |
| 15 | Spring | 0 | 15 | 0 | 1 (January) |
| 31 | Spring | 1 | 1 | 1 | 2 (February) |
| 45 | Spring | 1 | 15 | 1 | 2 (February) |
| 61 | Spring | 2 | 1 | 2 | 3 (March) |
| 90 | Spring | 2 | 30 | 2 | 3 (March) |
| 1 | Summer | 0 | 1 | 3 | 4 (April) |

---

## ?? ��� ������ ��������

### ������ (����������� �����):
- **DotEuropean**: `15.03.2001`
- **SlashEuropean**: `15/03/2001`
- ������: ����.�����.���

### ���:
- **SlashAmerican**: `03/15/2001`
- ������: �����/����/���

### ��� ��� ������ � API:
- **ISO**: `2001-03-15`
- ISO 8601 ��������
- ������: ���-�����-����

### ��� ����������� �����������:
- **DotShort**: `15.03.01`
- �������� ����� � UI
- ������: ��.��.��

---

## ?? ������������ �� �������������

### ����� ������������ ������ ������:

**Full** - �������� ���:
- ������� ����� ����
- ������� �������
- �������� ��������
- ����� ����� �������� ���� ������

**DotShort** - �������� ���:
- ���������� UI
- ������ ����������
- ��������� �������
- ��������� ������

**DotEuropean / SlashEuropean** - �������� ���:
- ����������� �����������
- ����������� ������
- ��������� ��� ����������� �������

**SlashAmerican** - �������� ���:
- ������������ �����������
- ��� �����

**ISO** - �������� ���:
- ���������� � �����
- API �������
- ���������� ���
- ����������� ����

---

## ?? ������ UI Layout

```
???????????????????????????????????
?  Time: 3:45 PM                 ?  ? bUse12HourFormat = true
?  Date: 15.03.01     ?  ? DateFormat = DotShort
?  Season: Spring          ?  ? bShowSeason = true
?  Phase: Afternoon    ?  ? bShowPhase = true
???????????????????????????????????
```

��� ����� ���������:

```
???????????????????????????????????
?  15:45:23             ?  ? bUse12HourFormat = false, bShowSeconds = true
?  15 March, Year 1 (Monday)     ?? DateFormat = Full
?  Spring - Afternoon         ?
???????????????????????????????????
```

---

## ? ������

```
Result: Succeeded
Total execution time: 8.72 seconds
Files compiled: 23
UHT generated: 10 files
```

---

## ?? ������ ������ ��������

### 1. Full (������)
```
15 March, Year 1 (Monday)
```
- ����������: ���� ������, �������� ������, ���, ���� ������
- ���������� ��������� �������� ������� (���� ������)
- ����� ������������� ������

### 2. DotShort (�������� � �������)
```
15.03.01
```
- ������: DD.MM.YY
- ��� ������������ ����� �������
- ����� ���������� ������

### 3. DotEuropean (����������� � �������)
```
15.03.2001
```
- ������: DD.MM.YYYY
- ������ ��� (4 �����)
- �������� ��� ������

### 4. SlashEuropean (����������� �� �������)
```
15/03/2001
```
- ������: DD/MM/YYYY
- ������ ��� (4 �����)
- ������������ DotEuropean

### 5. SlashAmerican (������������)
```
03/15/2001
```
- ������: MM/DD/YYYY
- ����� ��� ������!
- �������� ��� ���

### 6. ISO (ISO 8601)
```
2001-03-15
```
- ������: YYYY-MM-DD
- ������������� ��������
- �������� ��� ����������

---

## ?? ��� ������?

### ��������� ���������:

1. **����������� ��������** - �������������� ����� ������� �� ����� ����
2. **��������� �����������** - ��������� �����/������/����
3. **�������� �������� �������** - Jan, Feb, Mar ������ January
4. **������ ��� ����** - "15 �����" ������ "15 March, Year 1"
5. **������������� ����** - "�������", "�����", "3 ��� �����"

---

**������**: ? **READY TO USE**  
**������**: 1.4  
**����������**: �������  

**Happy Game Development!** ???
