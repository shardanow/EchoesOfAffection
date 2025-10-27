# Version 1.5.4 - Celestial Events ????

**Date**: October 25, 2024  
**Version**: 1.5.4  
**Status**: ? **NEW FEATURE**

---

## ?? What's New

**Celestial event delegates added!**

Now you can react to:
- ?? **OnSunrise** - Sun has risen
- ?? **OnSunset** - Sun has set
- ?? **OnMoonrise** - Moon has risen
- ?? **OnMoonset** - Moon has set

---

## ?? Blueprint Usage

### 1. Subscribe to Sunrise Event

```
Event BeginPlay
  ?
Get Time of Day Subsystem
  ?
Bind Event to OnSunrise
  ?
Custom Event: "Handle Sunrise"
```

**Your event receives**:
- `CurrentTime` (FTimeOfDayTime) - exact time of sunrise

---

### 2. Subscribe to Sunset Event

```
Event BeginPlay
  ?
Get Time of Day Subsystem
  ?
Bind Event to OnSunset
  ?
Custom Event: "Handle Sunset"
```

**Example**:
```
Handle Sunset
  ?
Print String ("The sun has set!")
  ?
Sun Light ? Set Intensity (0.0)
  ?
Enable Street Lights
```

---

### 3. Subscribe to Moonrise Event

```
Event BeginPlay
  ?
Get Time of Day Subsystem
  ?
Bind Event to OnMoonrise
  ?
Custom Event: "Handle Moonrise"
```

**Your event receives**:
- `MoonPhase` (float, 0-1) - current phase of moon
- `CurrentTime` (FTimeOfDayTime) - exact time of moonrise

**Example**:
```
Handle Moonrise
  ?
Print String ("Moon is rising! Phase: " + MoonPhase)
  ?
Moon Light ? Set Intensity (MoonPhase * 0.3)
  ?
Branch (MoonPhase > 0.45 && MoonPhase < 0.55?)
  TRUE: Trigger Werewolf Transformation ??
```

---

### 4. Subscribe to Moonset Event

```
Event BeginPlay
  ?
Get Time of Day Subsystem
  ?
Bind Event to OnMoonset
  ?
Custom Event: "Handle Moonset"
```

**Your event receives**:
- `MoonPhase` (float, 0-1)
- `CurrentTime` (FTimeOfDayTime)

---

## ?? How It Works

### Detection Logic:

Events fire when state **changes** from previous minute:

```cpp
// Every minute:
IsSunUp = (CurrentMinutes >= Sunrise && CurrentMinutes < Sunset)

if (IsSunUp && !PreviousWasSunUp)
    OnSunrise.Broadcast() // ?? Sun just rose!

if (!IsSunUp && PreviousWasSunUp)
    OnSunset.Broadcast() // ?? Sun just set!

// Moon is opposite
IsMoonUp = !IsSunUp

if (IsMoonUp && !PreviousWasMoonUp)
    OnMoonrise.Broadcast(MoonPhase) // ?? Moon just rose!

if (!IsMoonUp && PreviousWasMoonUp)
    OnMoonset.Broadcast(MoonPhase) // ?? Moon just set!
```

**Precision**: 1 minute (events fire on the minute of sunrise/sunset)

---

## ?? Event Timeline

### Summer Day Example:

```
05:00 AM:
  ?? OnSunrise fires
  ?? OnMoonset fires (at the same time!)
  Log: "?? Sunrise at 05:00"
  Log: "?? Moonset at 05:00 (Phase: 0.50)"

09:00 AM:
  (Nothing, sun is already up)

13:00 PM:
  (Nothing, sun at zenith)

21:00 PM:
  ?? OnSunset fires
  ?? OnMoonrise fires (at the same time!)
  Log: "?? Sunset at 21:00"
  Log: "?? Moonrise at 21:00 (Phase: 0.50)"

01:00 AM:
  (Nothing, moon at zenith)
```

**Sunrise = Moonset**  
**Sunset = Moonrise**

---

## ?? Use Cases

### 1. Street Lights

```
OnSunset:
  For Each Street Light
    ? Turn On

OnSunrise:
  For Each Street Light
    ? Turn Off
```

### 2. NPC Schedules

```
OnSunrise:
  NPCs ? Start Work Routine

OnSunset:
  NPCs ? Go Home Routine
```

### 3. Werewolf Transformation

```
OnMoonrise:
  if (MoonPhase > 0.45 && MoonPhase < 0.55)
    Player ? Transform to Werewolf
    Play Sound "Howl"

OnMoonset:
  if (Player is Werewolf)
    Player ? Transform Back to Human
```

### 4. Dynamic Music

```
OnSunrise:
  Music ? Crossfade to "Morning Theme"

OnSunset:
  Music ? Crossfade to "Evening Theme"
```

### 5. Enemy Spawning

```
OnSunset:
  Spawn Night Creatures
  Increase Enemy Difficulty

OnSunrise:
  Despawn Night Creatures
  Decrease Enemy Difficulty
```

---

## ?? Complete Example Blueprint

```
Actor: BP_DayNightController

Event BeginPlay:
  ?
  Get Time of Day Subsystem
  ?
  Bind Event to OnSunrise ? Handle Sunrise
  Bind Event to OnSunset ? Handle Sunset
  Bind Event to OnMoonrise ? Handle Moonrise
  Bind Event to OnMoonset ? Handle Moonset

Handle Sunrise:
  ?
  Print String "?? Good morning!"
  ?
  Sun Light ? Set Intensity (1.0)
  Moon Light ? Set Intensity (0.0)
  ?
  Sky Sphere ? Set Time of Day Parameter (0.25)
  ?
  For Each Street Light ? Turn Off

Handle Sunset:
  ?
  Print String "?? Good evening!"
  ?
  Sun Light ? Set Intensity (0.0)
  Moon Light ? Get Moon Phase ? Set Intensity (Phase * 0.3)
  ?
  Sky Sphere ? Set Time of Day Parameter (0.75)
  ?
  For Each Street Light ? Turn On

Handle Moonrise (MoonPhase):
  ?
  Print String "?? Moon rising! Phase: " + MoonPhase
  ?
  Branch (MoonPhase > 0.45 && MoonPhase < 0.55)
 TRUE:
      Trigger Special Full Moon Event
      Spawn Werewolves
    FALSE:
    Normal Night

Handle Moonset (MoonPhase):
  ?
  Print String "?? Moon setting"
  ?
  Despawn Night Creatures
```

---

## ?? All Celestial Events

| Event | Fires When | Parameters | Example Time (Summer) |
|-------|------------|------------|----------------------|
| **OnSunrise** | Sun rises | CurrentTime | 05:00 AM |
| **OnSunset** | Sun sets | CurrentTime | 21:00 PM |
| **OnMoonrise** | Moon rises | MoonPhase, CurrentTime | 21:00 PM |
| **OnMoonset** | Moon sets | MoonPhase, CurrentTime | 05:00 AM |

**Note**: Moonrise/Moonset happen at same time as Sunset/Sunrise!

---

## ?? Logging

Events are logged to console:

```
LogTemp: ?? Sunrise at 05:00
LogTemp: ?? Moonset at 05:00 (Phase: 0.50)
LogTemp: ?? Sunset at 21:00
LogTemp: ?? Moonrise at 21:00 (Phase: 0.50)
```

Useful for debugging!

---

## ?? Files Changed

### Modified:
1. `TimeOfDaySubsystem.h`:
 - Added 4 new delegate declarations
   - Added 4 new event properties
   - Added tracking variables (`bPreviousWasSunUp`, `bPreviousWasMoonUp`)
   - Added `CheckCelestialEvents()` method

2. `TimeOfDaySubsystem.cpp`:
   - Implemented `CheckCelestialEvents()`
   - Initialized tracking variables in constructor
   - Added celestial state initialization
   - Added event checking in `ProcessTimeChanges()`

---

## ?? Compilation

```
Parsing headers for EchoesOfAffection
Reflection code generated in 0.54 seconds
Building EchoesOfAffection...
[1-11] Compiled 9 files

Result: Succeeded ?
Total execution time: 8.46 seconds
```

---

## ? New Delegates

```cpp
// In header:
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSunriseEvent, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSunsetEvent, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoonriseEvent, float, MoonPhase, const FTimeOfDayTime&, CurrentTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoonsetEvent, float, MoonPhase, const FTimeOfDayTime&, CurrentTime);

// In class:
UPROPERTY(BlueprintAssignable, Category = "Time of Day Events|Celestial")
FOnSunriseEvent OnSunrise;

UPROPERTY(BlueprintAssignable, Category = "Time of Day Events|Celestial")
FOnSunsetEvent OnSunset;

UPROPERTY(BlueprintAssignable, Category = "Time of Day Events|Celestial")
FOnMoonriseEvent OnMoonrise;

UPROPERTY(BlueprintAssignable, Category = "Time of Day Events|Celestial")
FOnMoonsetEvent OnMoonset;
```

---

## ?? Best Practices

### 1. Bind in BeginPlay
```
? GOOD:
Event BeginPlay ? Bind Events

? BAD:
Event Tick ? Bind Events (wasteful!)
```

### 2. Unbind in EndPlay
```
Event EndPlay
  ?
  Get Time of Day Subsystem
  ?
  Unbind All Events from OnSunrise
```

### 3. Check Moon Phase
```
OnMoonrise (MoonPhase)
  ?
  Branch (MoonPhase > 0.4 && MoonPhase < 0.6)
    TRUE: Full Moon logic
    FALSE: Normal moon logic
```

---

## ?? Advanced: State Machine

```
Actor: BP_DayNightStateMachine

enum EDayNightState:
  - Dawn
  - Day
  - Dusk
  - Night

OnSunrise:
  Set State = Dawn
  Wait 1 hour
  Set State = Day

OnSunset:
  Set State = Dusk
  Wait 1 hour
  Set State = Night
```

---

**Status**: ? **FEATURE COMPLETE**  
**Version**: 1.5.4  
**New Events**: 4  
**Breaking Changes**: None

**React to celestial events in real-time!** ?????
