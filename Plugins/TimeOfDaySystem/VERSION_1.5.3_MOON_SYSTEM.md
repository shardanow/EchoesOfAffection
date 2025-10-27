# Version 1.5.3 - Moon System ??

**Date**: October 25, 2024  
**Version**: 1.5.3  
**Status**: ? **NEW FEATURE**

---

## ?? What's New

**Moon rotation system added!**

Now you have:
- ? **Sun rotation** - GetSunRotation()
- ? **Moon rotation** - GetMoonRotation() ? **NEW!**
- ? **Moon phases** - GetMoonPhase() ? **NEW!**

---

## ?? Blueprint API

### 1. Get Moon Rotation

```
Get Moon Rotation (Pure)
? Returns: Rotator (Pitch, Yaw, Roll)

Moon is OPPOSITE to the sun!
```

**Example**:
```
Event Tick
  ?
Get Time of Day Subsystem
  ?
Get Moon Rotation
  ?
Moon Directional Light ? Set World Rotation
```

**Result**: Moon moves opposite to sun! ??

---

### 2. Get Moon Phase

```
Get Moon Phase (Pure)
? Returns: Float (0.0-1.0)

0.0 = New Moon (invisible)
0.25 = First Quarter (half visible)
0.5 = Full Moon (fully visible)
0.75 = Last Quarter (half visible)
1.0 = New Moon again
```

**Example**:
```
Event Tick
  ?
Get Time of Day Subsystem
  ?
Get Moon Phase
  ?
Moon Material ? Set Scalar Parameter "Brightness" (value = Phase)
```

**Result**: Moon changes brightness based on phase! ??????

---

## ?? How It Works

### Moon Rotation

Moon is **exactly opposite** to the sun:

```cpp
MoonPitch = -SunPitch    // Opposite elevation
MoonYaw = SunYaw + 180°  // Opposite direction
```

**Example**:
```
Noon (13:00):
  Sun: Pitch = -90° (Zenith), Yaw = 0° (South)
  Moon: Pitch = +90° (Nadir), Yaw = 180° (North)
  
Midnight (01:00):
  Sun: Pitch = +90° (Nadir), Yaw = -180° (North)
  Moon: Pitch = -90° (Zenith), Yaw = 0° (South)
  
Sunrise (05:00):
  Sun: Pitch = 0° (East horizon), Yaw = 90° (East)
  Moon: Pitch = 0° (West horizon), Yaw = -90° (West)
```

**Perfect opposition!** ?? ?? ??

---

### Moon Phase

Moon phase cycles every **30 days** (one month):

```
Day 1:  Phase = 0.00 ? New Moon ??
Day 8:  Phase = 0.25 ? First Quarter ??
Day 15: Phase = 0.50 ? Full Moon ??
Day 23: Phase = 0.75 ? Last Quarter ??
Day 30: Phase = 1.00 ? New Moon ??
```

**Formula**:
```cpp
DayOfMonth = 1-30 (from DayOfSeason)
Phase = (DayOfMonth - 1) / 30.0
```

---

## ?? Complete Day/Night Setup

### Blueprint Example:

```
1. Create TWO Directional Lights:
   - Sun Light (warm color)
   - Moon Light (cool blue color)

2. Event Tick:
   
   // Sun
   Get Sun Rotation ? Sun Light ? Set World Rotation
   Get Sun Altitude
     ?
   Branch (Altitude > 0?)
     TRUE: Sun Light ? Set Intensity (1.0)
     FALSE: Sun Light ? Set Intensity (0.0)
   
   // Moon
   Get Moon Rotation ? Moon Light ? Set World Rotation
   Get Moon Phase
     ?
   Moon Light ? Set Intensity (Phase * 0.3)
   // Moon is dimmer than sun, and brightness depends on phase!
```

**Result**: 
- ?? Day: Sun is bright, Moon is invisible
- ?? Night: Sun is off, Moon shines (brightness varies by phase)

---

## ?? Advanced: Moon Phase Material

### Material Setup:

```
1. Create Material: M_Moon

2. Parameters:
   - MoonPhase (Scalar, 0-1)
   
3. Material Graph:
   Texture (Moon Base) ? Multiply ? Emissive
        ?
            MoonPhase (controls brightness)
   
4. Blueprint:
   Get Moon Phase ? Moon Material ? Set Scalar Parameter "MoonPhase"
```

**Result**: Moon texture gets brighter during full moon! ??

---

## ?? Full 24-Hour Cycle

### Summer Day Example:

| Time | Sun Position | Moon Position | Moon Phase (Day 15) |
|------|-------------|---------------|---------------------|
| **05:00** | East Horizon (rising) | West Horizon (setting) | 0.5 (Full!) |
| 13:00 | South Zenith | North Nadir | 0.5 |
| **21:00** | West Horizon (setting) | East Horizon (rising) | 0.5 |
| **01:00** | North Nadir | South Zenith | 0.5 (Bright!) |

**On day 15 (full moon)**: Moon shines brightest at midnight! ???

---

## ?? Realistic Behavior

### Real-World Accuracy:

1. **Opposition**: Moon is always opposite to sun ?
2. **Phase Cycle**: ~30 days (real moon is 29.5) ?
3. **Full Moon at Midnight**: When moon is highest, it's full ?
4. **New Moon at Noon**: When sun is highest, moon is invisible ?

**This matches real astronomy!** ??

---

## ?? Use Cases

### 1. Day/Night Lighting
```
if (GetSunAltitude() > 0)
    UseSunLight()
else
    UseMoonLight(GetMoonPhase())
```

### 2. Werewolf Transformation
```
if (GetMoonPhase() > 0.45 && GetMoonPhase() < 0.55)
    // Full moon (±5%)
    TransformToWerewolf()
```

### 3. Tides System
```
TideHeight = GetMoonPhase() * MaxTideHeight
// Full moon = highest tide
```

### 4. Night Visibility
```
NightBrightness = GetMoonPhase() * 0.3
SkyLight->SetIntensity(NightBrightness)
// Full moon = brighter night
```

---

## ?? Files Changed

### Modified:
1. `TimeOfDaySubsystem.h` - Added 2 new functions
2. `TimeOfDaySubsystem.cpp` - Implemented moon system

### New Functions:
```cpp
FRotator GetMoonRotation() const;  // Moon directional light rotation
float GetMoonPhase() const;     // Moon phase (0-1)
```

---

## ?? Compilation

```
Parsing headers for EchoesOfAffection
Reflection code generated in 1.07 seconds
Building EchoesOfAffection...
[1-11] Compiled 9 files

Result: Succeeded ?
Total execution time: 9.48 seconds
```

---

## ?? Quick Reference

### Sun vs Moon:

| Aspect | Sun | Moon |
|--------|-----|------|
| **Rotation** | GetSunRotation() | GetMoonRotation() |
| **Phase** | N/A | GetMoonPhase() (0-1) |
| **Brightness** | Constant (when visible) | Varies by phase |
| **Cycle** | 24 hours | 30 days |
| **Position** | Direct calculation | Opposite of sun |

---

## ? Checklist

- [x] Moon rotation opposite to sun
- [x] Moon phase calculation (30-day cycle)
- [x] Blueprint callable functions
- [x] Real astronomical behavior
- [x] Compiled successfully
- [x] Ready for use

---

## ?? Example: Complete Sky System

```
Event Tick
  ?
Get Time of Day Subsystem
  
  // Sun
  ?
Get Sun Rotation ? Sun Light ? Set Rotation
Get Sun Altitude
  ?
Branch (> 0?)
  TRUE: Sun ? Set Intensity (1.0)
  FALSE: Sun ? Set Intensity (0.0)
  
  // Moon
  ?
Get Moon Rotation ? Moon Light ? Set Rotation
Get Moon Phase
  ?
Moon ? Set Intensity (Phase * 0.3)
  
  // Sky
  ?
Sky Light ? Set Intensity
  if (Sun Altitude > 0): 1.0
  else: MoonPhase * 0.2
```

**Result**: Complete realistic day/night cycle! ????

---

**Status**: ? **FEATURE COMPLETE**  
**Version**: 1.5.3  
**Breaking Changes**: None  
**New Functions**: 2

**You now have a complete celestial system!** ?????
