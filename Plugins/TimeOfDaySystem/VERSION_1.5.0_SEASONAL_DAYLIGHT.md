# Version 1.5.0 - Seasonal Daylight System ??

**Дата**: 25 октября 2024, 16:35  
**Версия**: 1.5.0 (Seasonal Daylight)  
**Статус**: ? **NEW FEATURE**

---

## ?? Что добавлено

**Система сезонных изменений длины дня!**

Теперь:
- ? Лето: День длиннее (5:00-21:00 = 16 часов)
- ? Зима: День короче (8:00-16:00 = 8 часов)
- ? Весна/Осень: Средняя длина дня (6:30-18:30 = 12 часов)
- ? Автоматический расчёт положения солнца
- ? Готовая ротация для Directional Light

---

## ?? Новая структура: `FDaylightSettings`

### Параметры в DataAsset:

```cpp
Daylight Settings:
  ?? Enable Seasonal Daylight: true/false
  ?
  ?? Summer:
  ?   ?? Sunrise Hour: 5 (5:00 AM)
  ?   ?? Sunrise Minute: 0
  ?   ?? Sunset Hour: 21 (9:00 PM)
  ?   ?? Sunset Minute: 0
  ?
  ?? Winter:
      ?? Sunrise Hour: 8 (8:00 AM)
      ?? Sunrise Minute: 0
      ?? Sunset Hour: 16 (4:00 PM)
      ?? Sunset Minute: 0
```

**Spring/Autumn**: Автоматически интерполируется между Summer и Winter!

---

## ?? Blueprint API

### 1. **Get Sun Position**

```
Get Sun Position (Pure)
? Returns: Float (0.0-1.0 or -1.0)

0.0 = Sunrise
0.5 = Noon (highest point)
1.0 = Sunset
-1.0 = Night time
```

**Example**:
```
Event Tick
  ?
Get Time of Day Subsystem
  ?
Get Sun Position
  ?
Print String (Debug)
```

---

### 2. **Get Sun Rotation**

```
Get Sun Rotation (Pure)
? Returns: Rotator (Pitch, Yaw, Roll)

Use directly with Directional Light!
```

**Example Blueprint**:
```
Event Tick
  ?
Get Time of Day Subsystem
  ?
Get Sun Rotation
  ?
Directional Light ? Set World Rotation
```

**Result**: Солнце автоматически движется от East (восток) к West (запад)!

---

### 3. **Get Sun Altitude**

```
Get Sun Altitude (Pure)
? Returns: Float (degrees)

0° = Horizon
90° = Zenith (directly overhead)
Negative = Below horizon
```

---

### 4. **Is Daylight Hours**

```
Is Daylight Hours (Pure)
? Returns: Boolean

true = Between sunrise and sunset
false = Night time
```

**Different from `IsDaytime()`**:
- `IsDaytime()` - Based on day phases (Dawn, Morning, Noon, etc.)
- `IsDaylightHours()` - Based on actual sunrise/sunset times

---

### 5. **Get Sunrise/Sunset Times**

```
Get Sunrise Minutes (Pure)
? Returns: Int32 (minutes since midnight)

Get Sunset Minutes (Pure)
? Returns: Int32 (minutes since midnight)

Get Day Length Minutes (Pure)
? Returns: Int32 (total daylight minutes)
```

**Example**:
```
Summer:
  Sunrise: 300 (5:00 AM)
  Sunset: 1260 (21:00 PM)
  Day Length: 960 minutes (16 hours)

Winter:
  Sunrise: 480 (8:00 AM)
  Sunset: 960 (16:00 PM)
  Day Length: 480 minutes (8 hours)
```

---

## ?? Example: Directional Light Controller

### Blueprint Setup:

```
1. Create Actor Blueprint: BP_SunController

2. Add Component: Directional Light

3. Event Tick:
    Get Time of Day Subsystem
      ?
    Get Sun Rotation
      ?
    Directional Light ? Set World Rotation
      ?
    Get Sun Altitude
      ?
  Branch (Is > 0?)
      TRUE:
        Directional Light ? Set Intensity (Lerp: 0 to 1)
        Directional Light ? Set Light Color (Day Color)
    FALSE:
   Directional Light ? Set Intensity (0)
   Directional Light ? Set Light Color (Night Color)
```

---

### Advanced: Color Gradient by Time

```
Event Tick
  ?
Get Sun Position (0.0 - 1.0)
  ?
Select Float (Switch on ranges):
  0.0 - 0.1: Dawn Color (Orange/Pink)
  0.1 - 0.4: Morning Color (Yellow)
  0.4 - 0.6: Noon Color (White)
  0.6 - 0.9: Afternoon Color (Golden)
  0.9 - 1.0: Dusk Color (Orange/Red)
  ?
Directional Light ? Set Light Color
```

---

##  ?? C++ API

### Get Sun Data:

```cpp
UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();

// Get sun position (0.0-1.0)
float SunPos = TimeSystem->GetSunPosition();

// Get sun rotation for Directional Light
FRotator SunRotation = TimeSystem->GetSunRotation();

// Apply to light
DirectionalLight->SetWorldRotation(SunRotation);

// Get sunrise/sunset
int32 Sunrise = TimeSystem->GetSunriseMinutes(); // e.g. 300 (5:00 AM)
int32 Sunset = TimeSystem->GetSunsetMinutes();   // e.g. 1260 (21:00 PM)

// Check if daylight
bool bIsDaylight = TimeSystem->IsDaylightHours();
```

---

## ?? How Sun Position Works

### Math Behind It:

```
Summer Day (16 hours):
  Sunrise: 5:00 AM (300 min)
  Sunset: 21:00 PM (1260 min)
  Day Length: 960 min

Current Time: 13:00 (780 min)
Minutes Since Sunrise: 780 - 300 = 480 min
Sun Position: 480 / 960 = 0.5 (Noon!)

Sun Altitude: sin(0.5 * ?) * 90° = 90° (Zenith!)
```

### Result:
- `GetSunPosition()` ? 0.5
- `GetSunAltitude()` ? 90.0° (directly overhead)
- `GetSunRotation()` ? Pitch: -90°, Yaw: 0° (South)

---

## ?? Real-World Example

### Moscow, Russia:
```
Summer (June):
  Sunrise: ~4:45 AM
  Sunset: ~22:00 PM
  Day: 17.25 hours

Winter (December):
  Sunrise: ~8:45 AM
  Sunset: ~15:50 PM
  Day: 7 hours
```

### Your DataAsset:
```
Summer Sunrise Hour: 5
Summer Sunset Hour: 22
Winter Sunrise Hour: 9
Winter Sunset Hour: 16
```

**Spring/Autumn**: Automatically ~13:30 day length!

---

## ? Files Changed

### New Files:
- `TimeOfDayTypes.cpp` - Implementation of sun methods

### Modified Files:
- `TimeOfDayTypes.h` - Added `FDaylightSettings` struct
- `TimeOfDaySettings.h` - Added Daylight property
- `TimeOfDaySubsystem.h` - Added sun/daylight functions (7 новых)
- `TimeOfDaySubsystem.cpp` - Implemented sun functions

---

## ?? Compilation

```
Parsing headers for EchoesOfAffection
Reflection code generated in 0.57 seconds
Building EchoesOfAffection...
[1-21] Compiled 19 files

Result: Succeeded ?
Total execution time: 11.41 seconds
```

---

## ?? Seasonal Comparison

| Season | Sunrise | Sunset | Day Length | Example Date |
|--------|---------|--------|------------|--------------|
| **Summer** | 5:00 | 21:00 | 16 hours | June 21 |
| **Spring** | 6:30 | 18:30 | 12 hours | March 21 |
| **Autumn** | 6:30 | 18:30 | 12 hours | September 21 |
| **Winter** | 8:00 | 16:00 | 8 hours | December 21 |

**Fully configurable in DataAsset!**

---

## ?? Pro Tips

### 1. Disable Seasonal Changes:
```
Daylight Settings:
  Enable Seasonal Daylight: false
```
? Always uses default 6:00-18:00

### 2. Extreme Locations:
```
Arctic Summer (Midnight Sun):
  Summer Sunrise: 0
  Summer Sunset: 24

Arctic Winter (Polar Night):
  Winter Sunrise: 12
  Winter Sunset: 12
```

### 3. Custom Planets:
```
Fast Rotation Planet:
  Summer: 2:00 - 10:00 (8 hours)
Winter: 4:00 - 8:00 (4 hours)
```

---

**Status**: ? **FEATURE COMPLETE**  
**Version**: 1.5.0  
**Breaking Changes**: None  
**Blueprint Compatible**: 100%

**Your sun now follows seasons realistically!** ????
