# DataAsset Configuration Guide - Time of Day System ??

**Version**: 1.4.3  
**Last Updated**: October 25, 2024

---

## ?? Overview

This guide explains how to properly configure the `DA_TimeOfDaySettings` DataAsset to ensure the Time of Day system works correctly with the calendar year starting from **January**.

---

## ?? Creating the DataAsset

### Step 1: Create the DataAsset

1. In **Content Browser**, navigate to `/Content/TimeOfDaySystem/`
2. **Right-click** ? **Miscellaneous** ? **Data Asset**
3. Select **`TimeOfDaySettings`** as the class
4. Name it: **`DA_TimeOfDaySettings`**

---

## ?? Proper Configuration

### ? **CRITICAL: Initial Game Time Configuration**

The **most important** setting is `InitialGameTime`. This determines when your game starts.

#### For January 1st (Start of Year):

```
Initial Game Time:
  ?? Year: 1
  ?? Season: Winter     ? IMPORTANT!
  ?? DayOfSeason: 31       ? CRITICAL! Not 1!
  ?? DayOfWeek: Monday
  ?? Hour: 6
  ?? Minute: 0
  ?? Second: 0
  ?? TotalElapsedSeconds: 0.0
```

**?? WHY DayOfSeason = 31?**

Because **Winter** has 3 months:
1. **December** (Winter day 1-30)
2. **January** (Winter day 31-60) ? **Day 31 = January 1st**
3. **February** (Winter day 61-90)

#### For March 1st (Start of Spring):

```
Initial Game Time:
  ?? Year: 1
  ?? Season: Spring? Spring
  ?? DayOfSeason: 1          ? First day of Spring = March 1
  ?? Hour: 6
  ?? ...
```

#### For June 1st (Start of Summer):

```
Initial Game Time:
  ?? Year: 1
  ?? Season: Summer
  ?? DayOfSeason: 1          ? First day of Summer = June 1
  ?? ...
```

---

## ?? Season to Month Mapping

### **Complete Calendar Mapping Table**

| Season | DayOfSeason | Calendar Month | Description |
|--------|-------------|----------------|-------------|
| **Winter** | 1-30 | December | First month of winter |
| **Winter** | 31-60 | January | Second month (NEW YEAR!) |
| **Winter** | 61-90 | February | Third month |
| **Spring** | 1-30 | March | First month of spring |
| **Spring** | 31-60 | April | Second month |
| **Spring** | 61-90 | May | Third month |
| **Summer** | 1-30 | June | First month of summer |
| **Summer** | 31-60 | July | Second month |
| **Summer** | 61-90 | August | Third month |
| **Autumn** | 1-30 | September | First month of autumn |
| **Autumn** | 31-60 | October | Second month |
| **Autumn** | 61-90 | November | Third month |

---

## ?? DayOfSeason Calculator

Use this formula to find the correct `DayOfSeason`:

```
For January (month 1):
  DayOfSeason = 30 + DayOfMonth
  Example: January 15 ? DayOfSeason = 30 + 15 = 45

For February (month 2):
  DayOfSeason = 60 + DayOfMonth
  Example: February 10 ? DayOfSeason = 60 + 10 = 70

For March (month 3):
  DayOfSeason = DayOfMonth
  Example: March 15 ? DayOfSeason = 15

For April (month 4):
  DayOfSeason = 30 + DayOfMonth
  Example: April 20 ? DayOfSeason = 30 + 20 = 50

...and so on
```

---

## ?? Complete DataAsset Configuration

### 1. **Time Conversion Settings**

```
Time Conversion:
  ?? Real Seconds Per Game Day: 1200.0    (20 minutes real time = 1 game day)
  ?? Time Scale: 1.0          (Normal speed)
  ?? Is Paused: false
```

**Examples:**
- `1200.0` = 20 minutes real time = 1 game day (default)
- `600.0` = 10 minutes real time = 1 game day (faster)
- `2400.0` = 40 minutes real time = 1 game day (slower)

### 2. **Calendar Settings**

```
Calendar:
  ?? Days Per Season: 90          (3 months ? 30 days)
  ?? Seasons Per Year: 4    (Spring, Summer, Autumn, Winter)
  ?? Days Per Week: 7             (Monday - Sunday)
  ?? Month Names: (empty or 12 names)
```

**?? DO NOT CHANGE** `DaysPerSeason` or `SeasonsPerYear` unless you want a custom calendar!

### 3. **Day Phases**

Default phases (you can customize these):

```
Day Phases (Array of 7 elements):
  
  [0] Night:
    ?? Phase: Night
    ?? Start Hour: 0
    ?? Start Minute: 0
    ?? Display Name: "Night"
  
  [1] Dawn:
    ?? Phase: Dawn
    ?? Start Hour: 5
    ?? Start Minute: 0
    ?? Display Name: "Dawn"
  
  [2] Morning:
    ?? Phase: Morning
    ?? Start Hour: 7
    ?? Start Minute: 0
    ?? Display Name: "Morning"
  
  [3] Noon:
    ?? Phase: Noon
  ?? Start Hour: 12
    ?? Start Minute: 0
  ?? Display Name: "Noon"
  
  [4] Afternoon:
    ?? Phase: Afternoon
    ?? Start Hour: 15
    ?? Start Minute: 0
    ?? Display Name: "Afternoon"
  
  [5] Dusk:
    ?? Phase: Dusk
    ?? Start Hour: 18
    ?? Start Minute: 0
    ?? Display Name: "Dusk"
  
  [6] Evening:
    ?? Phase: Evening
    ?? Start Hour: 20
  ?? Start Minute: 0
    ?? Display Name: "Evening"
```

### 4. **Advanced Settings**

```
Advanced:
  ?? Auto Advance Time: true  (Time flows automatically)
  ?? Update Frequency: 10.0           (10 updates per second)
  ?? Persist Time State: true   (Save/load with save games)
```

### 5. **Month Names (Optional)**

If you want custom month names, add **exactly 12 names** in calendar order:

```
Month Names (Array of 12):
  [0] "January"      ? Month 1
  [1] "February"     ? Month 2
  [2] "March"      ? Month 3
  [3] "April"        ? Month 4
  [4] "May"          ? Month 5
  [5] "June"         ? Month 6
  [6] "July"         ? Month 7
  [7] "August"       ? Month 8
  [8] "September"    ? Month 9
  [9] "October"      ? Month 10
  [10] "November"    ? Month 11
  [11] "December"    ? Month 12
```

**?? IMPORTANT**: 
- Must have **exactly 12 names** or leave empty
- Order is **January ? December** (calendar order, NOT season order!)

---

## ?? Common Start Configurations

### Configuration 1: New Year's Day Morning

```yaml
InitialGameTime:
  Year: 1
  Season: Winter
  DayOfSeason: 31           # January 1st
  DayOfWeek: Monday
  Hour: 6    # 6:00 AM
  Minute: 0
  Second: 0
```
? **Result**: "1 January, Year 1, 06:00 (Monday)"

---

### Configuration 2: Spring Morning (March 1st)

```yaml
InitialGameTime:
  Year: 1
  Season: Spring
  DayOfSeason: 1    # March 1st
  DayOfWeek: Monday
  Hour: 8            # 8:00 AM
  Minute: 0
  Second: 0
```
? **Result**: "1 March, Year 1, 08:00 (Monday)"

---

### Configuration 3: Summer Afternoon (July 15th)

```yaml
InitialGameTime:
  Year: 1
  Season: Summer
  DayOfSeason: 46    # July 15th (30 June + 15 July = 45 + 1)
  DayOfWeek: Wednesday
  Hour: 14   # 2:00 PM
  Minute: 30
  Second: 0
```
? **Result**: "15 July, Year 1, 14:30 (Wednesday)"

---

### Configuration 4: Christmas Evening (December 25th)

```yaml
InitialGameTime:
  Year: 1
  Season: Winter
  DayOfSeason: 25           # December 25th
  DayOfWeek: Friday
  Hour: 18       # 6:00 PM
  Minute: 0
  Second: 0
```
? **Result**: "25 December, Year 1, 18:00 (Friday)"

---

## ?? Verification Checklist

After configuring your DataAsset, verify:

### ? Checklist:

- [ ] `DaysPerSeason = 90`
- [ ] `SeasonsPerYear = 4`
- [ ] `DaysPerWeek = 7`
- [ ] For **January start**: `Season = Winter` AND `DayOfSeason = 31`
- [ ] For **March start**: `Season = Spring` AND `DayOfSeason = 1`
- [ ] `MonthNames` array has **0 or 12** elements
- [ ] `DayPhases` array has **at least 1** element
- [ ] `RealSecondsPerGameDay > 0`
- [ ] DataAsset is saved in `/Game/TimeOfDaySystem/` or `/TimeOfDaySystem/`

---

## ?? Common Mistakes

### ? Mistake 1: Wrong DayOfSeason for January

```yaml
# WRONG!
Season: Winter
DayOfSeason: 1    # This is December 1st, NOT January 1st!

# CORRECT!
Season: Winter
DayOfSeason: 31   # January 1st
```

### ? Mistake 2: Wrong Season for January

```yaml
# WRONG!
Season: Spring    # Spring starts in March, not January!
DayOfSeason: 1

# CORRECT!
Season: Winter
DayOfSeason: 31
```

### ? Mistake 3: Incomplete Month Names

```yaml
# WRONG!
MonthNames:
  [0] "January"
  [1] "February"
  [2] "March"
  # Only 3 names!

# CORRECT!
MonthNames:
  (empty)           # Use defaults
# OR
MonthNames:
  [0-11] (all 12)# Provide all 12 names
```

---

## ?? Troubleshooting

### Problem: Game shows "December" instead of "January"

**Cause**: `DayOfSeason = 1` when `Season = Winter`  
**Solution**: Set `DayOfSeason = 31` for January 1st

### Problem: Custom month names not showing

**Cause**: Array has less than 12 elements  
**Solution**: Add all 12 month names or leave array empty

### Problem: Time doesn't advance

**Cause**: `bAutoAdvanceTime = false` or `TimeScale = 0`  
**Solution**: Set `bAutoAdvanceTime = true` and `TimeScale = 1.0`

---

## ?? Additional Resources

- [Calendar Year Fix v1.4.3](CALENDAR_YEAR_FIX_v1.4.3.md)
- [Quick Start Guide](QUICKSTART.md)
- [Date Format Options](DATE_FORMAT_OPTIONS.md)

---

**Status**: ? **VERIFIED AND TESTED**  
**Version**: 1.4.3  
**Last Updated**: October 25, 2024

**Your Time of Day system is ready to use!** ??
