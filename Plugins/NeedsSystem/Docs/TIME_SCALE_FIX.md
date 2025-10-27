# Time Scale Fix - v1.0.1

## Problem

Needs were decaying extremely fast - dropping to zero in just 1 game minute instead of over hours. Values were going into negative range almost instantly.

## Root Cause

The time scale was hardcoded to `24.0` in the Subsystem code:

```cpp
// OLD CODE (WRONG)
DeltaGameHours = DeltaTime * 24.0f; // 1 real second = 24 game hours!
```

This meant:
- **1 real second = 24 game hours**
- **1 real minute = 1440 game hours (60 days!)**
- A decay rate of `5.0 points/hour` would lose 5 points every 2.5 real seconds

## Solution

### Code Changes

1. **NeedsSettings.h** - Changed defaults:
   ```cpp
   // OLD
   float RealTimeScale = 24.0f;
   
   // NEW
   float RealTimeScale = 1.0f;  // Realistic 1:1 time
   float GameTimeFallbackScale = 1.0f;  // Fallback when Time System is missing
   ```

2. **NeedsSubsystem.cpp** - Use settings instead of hardcoded value:
   ```cpp
   // OLD
   DeltaGameHours = DeltaTime * 24.0f;  // Hardcoded!
   
   // NEW
   DeltaGameHours = DeltaTime * Settings->GameTimeFallbackScale;  // From settings
   ```

### Updated Defaults

| Setting | Old Value | New Value | Meaning |
|---------|-----------|-----------|---------|
| `RealTimeScale` | 24.0 | 1.0 | 1 game hour = 1 real hour |
| `GameTimeFallbackScale` | N/A | 1.0 | Same as RealTimeScale when Time System is absent |

## How to Configure Time Scale

### For Normal Gameplay (Recommended)

```
Use Game Time: ? or ?
Real Time Scale: 1.0
Game Time Fallback Scale: 1.0
```

**Result:** Needs decay naturally over hours of gameplay
- Decay rate 5.0/hour = loses 5 points every real hour
- Full depletion (100 points) takes 20 real hours

### For Fast Testing

```
Real Time Scale: 24.0
Game Time Fallback Scale: 24.0
```

**Result:** Accelerated decay for quick testing
- 1 real hour = 24 game hours
- Decay rate 5.0/hour = loses 5 points every 2.5 real minutes
- Full depletion (100 points) takes 50 real minutes

### For Slow Testing

```
Real Time Scale: 0.1
Game Time Fallback Scale: 0.1
```

**Result:** Very slow decay for long-term testing
- 10 real hours = 1 game hour
- Decay rate 5.0/hour = loses 5 points every 10 real hours
- Full depletion (100 points) takes 200 real hours

## Migration Guide

### Existing Projects

1. **Close Unreal Editor** (if open)
2. **Rebuild** the project
3. **Open** `DA_NeedsSettings`
4. **Verify** settings:
   - `Real Time Scale` = 1.0 (or your desired value)
   - `Game Time Fallback Scale` = 1.0 (or your desired value)
5. **Save** the asset
6. **Test** in PIE - needs should now decay normally

### If Needs Still Decay Too Fast

Check your **Need Definitions** (`DA_Need_Hunger`, etc.):
- `Base Decay Rate` should be **reasonable** (e.g., 5.0-10.0 points per hour)
- NOT 50.0 or 100.0 unless you want very fast decay

Example calculation:
```
Base Decay Rate: 5.0 points/hour
Time Scale: 1.0
Result: 5 points per real hour
Time to depletion: 100 / 5 = 20 real hours
```

## Console Commands

Debug your time scale in PIE:

```cpp
needs.debug 1  // Show debug info
```

Look at the "Game Time" value - it should increment slowly (1.0 per real hour with scale 1.0).

## Summary

- **v1.0.0:** Time scale was hardcoded to 24.0 ? needs decayed too fast ?
- **v1.0.1:** Time scale is now configurable with default 1.0 ? realistic decay ?

**Default behavior:** 1 game hour = 1 real hour (realistic RPG pacing)

---

**Date:** October 25, 2025  
**Version:** 1.0.1  
**Status:** ? Fixed
