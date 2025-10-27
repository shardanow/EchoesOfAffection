# Version 1.4.3 - DataAsset Configuration Fix ??

**Дата**: 25 октября 2024, 15:30  
**Версия**: 1.4.3 (DataAsset Configuration)  
**Статус**: ? **DOCUMENTATION UPDATE**

---

## ?? Summary

Добавлена полная документация по правильной настройке DataAsset для корректной работы календарной системы.

---

## ?? What's New

### 1. **DataAsset Configuration Guide**

Создан подробный гайд: **`DATAASSET_CONFIGURATION_GUIDE.md`**

**Содержимое**:
- ? Как создать DataAsset
- ? Правильная настройка `InitialGameTime`
- ? Полная таблица соответствия Season ? Month
- ? Калькулятор DayOfSeason
- ? Примеры конфигураций
- ? Checklist для проверки
- ? Типичные ошибки и их решения
- ? Troubleshooting

### 2. **DayOfSeason Quick Reference**

Создана быстрая справочная таблица: **`DAYOFSEASON_QUICK_REFERENCE.md`**

**Содержимое**:
- ? Таблица конверсии: Month ? DayOfSeason
- ? Примеры для всех 12 месяцев
- ? Формулы расчёта
- ? One-line reference table

---

## ?? CRITICAL: Correct InitialGameTime Setup

### For January 1st (Start of Year):

```yaml
InitialGameTime:
  Year: 1
  Season: Winter        ? MUST be Winter!
  DayOfSeason: 31          ? CRITICAL! Not 1!
  DayOfWeek: Monday
  Hour: 6
  Minute: 0
```

**WHY 31?**  
Because Winter includes:
- December (days 1-30)
- **January (days 31-60)** ? Day 31 = January 1st
- February (days 61-90)

---

## ?? Season-Month Mapping

### Complete Mapping:

| Season | DayOfSeason Range | Months |
|--------|-------------------|--------|
| **Winter** | 1-30 | December |
| **Winter** | 31-60 | January |
| **Winter** | 61-90 | February |
| **Spring** | 1-30 | March |
| **Spring** | 31-60 | April |
| **Spring** | 61-90 | May |
| **Summer** | 1-30 | June |
| **Summer** | 31-60 | July |
| **Summer** | 61-90 | August |
| **Autumn** | 1-30 | September |
| **Autumn** | 31-60 | October |
| **Autumn** | 61-90 | November |

---

## ?? Quick Examples

### Example 1: New Year's Day
```
Season: Winter
DayOfSeason: 31
Hour: 6
```
? Result: "1 January, Year 1, 06:00"

### Example 2: Spring Start
```
Season: Spring
DayOfSeason: 1
Hour: 8
```
? Result: "1 March, Year 1, 08:00"

### Example 3: Summer Day
```
Season: Summer
DayOfSeason: 46    // July 15th
Hour: 14
```
? Result: "15 July, Year 1, 14:00"

### Example 4: Christmas
```
Season: Winter
DayOfSeason: 25
Hour: 18
```
? Result: "25 December, Year 1, 18:00"

---

## ?? Verification

### Checklist for DataAsset:

- [x] `DaysPerSeason = 90`
- [x] `SeasonsPerYear = 4`
- [x] `DaysPerWeek = 7`
- [x] For January: `Season = Winter` AND `DayOfSeason = 31`
- [x] For March: `Season = Spring` AND `DayOfSeason = 1`
- [x] `MonthNames` has 0 or 12 elements
- [x] `DayPhases` has at least 1 element

---

## ?? Files Added

1. **DATAASSET_CONFIGURATION_GUIDE.md**  
   - Complete configuration guide
   - ~400 lines of documentation
 - Examples, formulas, troubleshooting

2. **DAYOFSEASON_QUICK_REFERENCE.md**  
   - Quick lookup table
   - All 12 months covered
   - Copy-paste ready examples

---

## ?? Updated Files

1. **INDEX.md**  
   - Added links to new documentation

2. **SUMMARY.md**  
   - Will be updated with v1.4.3 info

---

## ?? For Users

### If you're starting a new game:

1. Open **Content Browser**
2. Create DataAsset: `TimeOfDaySettings`
3. Name it: `DA_TimeOfDaySettings`
4. Open the guide: **`DATAASSET_CONFIGURATION_GUIDE.md`**
5. Follow the configuration steps
6. Use **`DAYOFSEASON_QUICK_REFERENCE.md`** for quick lookups

### If your game shows wrong dates:

1. Check `InitialGameTime.Season` and `InitialGameTime.DayOfSeason`
2. Use the **DayOfSeason Calculator** in the guide
3. Verify against the **Quick Reference** table

---

## ?? Key Takeaways

### 1. Year Structure:
```
Year 1:
  ?? Days 0-29   ? January  (Winter 31-60)
  ?? Days 30-59  ? February (Winter 61-90)
  ?? Days 60-89  ? March    (Spring 1-30)
  ?? ...
  ?? Days 330-359 ? December (Winter 1-30)
```

### 2. Season ? Calendar Start:
- **Season.Spring** starts at **March**, not January
- **Year** starts at **January** (Winter day 31)

### 3. DayOfSeason is Relative:
- Each season has days 1-90
- Multiple months within one season
- Use the calculator to find correct value

---

## ?? Documentation Structure

```
Plugins/TimeOfDaySystem/
?? DATAASSET_CONFIGURATION_GUIDE.md  ? MAIN GUIDE
?? DAYOFSEASON_QUICK_REFERENCE.md      ? QUICK LOOKUP
?? CALENDAR_YEAR_FIX_v1.4.2.md? Technical explanation
?? SEASON_FIX_v1.4.1.md       ? Historical fix
?? INDEX.md    ? All documentation links
?? SUMMARY.md          ? Version history
```

---

## ?? Next Steps

1. ? Create your DataAsset
2. ? Configure using the guide
3. ? Test in-game
4. ? Verify date display
5. ? Enjoy!

---

## ?? Related Documents

- [DataAsset Configuration Guide](DATAASSET_CONFIGURATION_GUIDE.md) - Complete setup
- [DayOfSeason Quick Reference](DAYOFSEASON_QUICK_REFERENCE.md) - Lookup table
- [Calendar Year Fix v1.4.2](CALENDAR_YEAR_FIX_v1.4.2.md) - Technical details
- [Quick Start Guide](QUICKSTART.md) - General setup

---

**Status**: ? **DOCUMENTATION COMPLETE**  
**Version**: 1.4.3  
**Code Changes**: None (documentation only)  
**Compilation**: Not required  

**Your Time of Day system is now fully documented!** ???
