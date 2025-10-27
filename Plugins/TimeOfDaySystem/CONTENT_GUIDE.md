# Time of Day System - Content Structure

## Recommended Content Organization

```
Content/
??? TimeOfDaySystem/
    ??? DataAssets/
    ?   ??? DA_TimeOfDaySettings_Default.uasset
  ?   ??? DA_TimeOfDaySettings_FastPaced.uasset
    ? ??? DA_TimeOfDaySettings_Realistic.uasset
    ??? UI/
    ?   ??? Widgets/
    ?   ?   ??? WBP_TimeOfDay_HUD.uasset
    ?   ?   ??? WBP_Clock_Compact.uasset
 ?   ?   ??? WBP_Calendar_Full.uasset
    ?   ??? Textures/
    ???? T_PhaseIcon_Night.uasset
    ?       ??? T_PhaseIcon_Day.uasset
    ?       ??? T_PhaseIcon_Dawn.uasset
    ??? Examples/
    ??? BP_TimeDebugActor.uasset
        ??? BP_DayNightDirectionalLight.uasset
```

## Example: Creating Time Settings Asset

### 1. Default Settings (20-minute days)
Create: `DA_TimeOfDaySettings_Default`

**Time Conversion:**
- Real Seconds Per Game Day: 1200 (20 minutes)
- Time Scale: 1.0
- Is Paused: false

**Calendar:**
- Days Per Season: 30
- Seasons Per Year: 4
- Days Per Week: 7

**Initial Game Time:**
- Year: 1
- Season: Spring
- Day of Season: 1
- Hour: 6
- Minute: 0
- Second: 0

**Day Phases:**
| Phase | Start Hour | Start Minute |
|-------|------------|--------------|
| Night | 0 | 0 |
| Dawn | 5 | 0 |
| Morning | 7 | 0 |
| Noon | 11 | 0 |
| Afternoon | 14 | 0 |
| Dusk | 18 | 0 |
| Evening | 20 | 0 |

### 2. Fast-Paced Settings (10-minute days)
Create: `DA_TimeOfDaySettings_FastPaced`

**Time Conversion:**
- Real Seconds Per Game Day: 600 (10 minutes)
- Time Scale: 1.0

**Calendar:**
- Days Per Season: 20
- Seasons Per Year: 4

### 3. Realistic Settings (2-hour days)
Create: `DA_TimeOfDaySettings_Realistic`

**Time Conversion:**
- Real Seconds Per Game Day: 7200 (2 hours)
- Time Scale: 1.0

**Calendar:**
- Days Per Season: 90
- Seasons Per Year: 4

## Example: Setting Up in Level

### Blueprint Setup

1. **In Level Blueprint or Game Mode:**
```
Event BeginPlay
  |
  V
Get Time of Day Subsystem
  |
V
Initialize With Settings [DA_TimeOfDaySettings_Default]
```

2. **Add Debug Actor:**
```cpp
// BP_TimeDebugActor - displays time info on screen
Event Tick
  |
  V
Get Time of Day Subsystem -> Get Formatted Time
  |
  V
Print String (Time)
```

### Widget Setup

1. **Create HUD Widget: WBP_TimeOfDay_HUD**
   - Parent Class: `TimeOfDayWidget`
   - Add TextBlocks:
     - `TimeText` (bound)
     - `DateText` (bound, optional)
  - `SeasonText` (bound, optional)
     - `PhaseText` (bound, optional)
   - Add ProgressBar:
     - `DayProgressBar` (bound, optional)

2. **Customize Appearance:**
   - Time Text: Large, bold font
   - Date Text: Smaller, secondary font
   - Progress Bar: Custom style showing day cycle

3. **Add to Viewport:**
```
Event BeginPlay (Player Controller)
  |
  V
Create Widget [WBP_TimeOfDay_HUD]
  |
  V
Add to Viewport
```

## Example: Dynamic Day/Night Lighting

### BP_DayNightDirectionalLight

```
Component Setup:
- Directional Light Component
- Time of Day Listener Component

Event On Phase Changed (from component):
  Switch on Phase
    - Night: Set Light Intensity = 0.1, Color = Blue
    - Dawn: Set Light Intensity = 0.5, Color = Orange
    - Morning: Set Light Intensity = 1.0, Color = White
    - Noon: Set Light Intensity = 1.5, Color = White
    - Afternoon: Set Light Intensity = 1.2, Color = Warm White
    - Dusk: Set Light Intensity = 0.6, Color = Orange
    - Evening: Set Light Intensity = 0.3, Color = Dark Blue

Event Tick:
  Get Normalized Time of Day -> Map to Rotation
  Pitch = (NormalizedTime * 180) - 90
  Set Actor Rotation
```

## Example: NPC Schedule System

### BP_ScheduledNPC

```cpp
Time of Day Listener Component settings:
- Receive Hour Events: True
- Other events: False

Event On Hour Changed:
  Switch on CurrentTime.Hour
    - 8:00: Go To Work
    - 12:00: Go To Lunch
    - 13:00: Return To Work
 - 17:00: Go Home
    - 19:00: Go To Dinner
    - 22:00: Go To Bed
```

## Example: Time Control UI

### WBP_TimeControls

```
Buttons:
- [Pause/Resume] -> Call Pause Time / Resume Time
- [0.5x] -> Set Time Scale (0.5)
- [1x] -> Set Time Scale (1.0)
- [2x] -> Set Time Scale (2.0)
- [10x] -> Set Time Scale (10.0)
- [Skip Hour] -> Advance Time (3600)
- [Skip Day] -> Advance Time (86400)

Slider:
- Time Scale Slider (0.1 - 10.0)
  On Value Changed -> Set Time Scale
```

## Example: Save/Load System

### Game Save Data

```cpp
USTRUCT()
struct FMyGameSaveData
{
    GENERATED_BODY()
    
UPROPERTY()
    FGameTime SavedGameTime;
    
    UPROPERTY()
    float SavedTimeScale;
    
    // ... other save data
};

// On Save:
Get Time of Day Subsystem
  -> Get Current Time -> Store in SavedGameTime
  -> Get Time Scale -> Store in SavedTimeScale

// On Load:
Get Time of Day Subsystem
  -> Set Current Time (SavedGameTime)
  -> Set Time Scale (SavedTimeScale)
```

## Example: Quest System Integration

### Timed Quest

```cpp
Quest: "Meet at noon"

On Quest Start:
  Get Time of Day Subsystem -> Bind to OnHourChanged

On Hour Changed:
  Get Current Time
  If Hour == 12:
    If IsNighttime == false:
      Complete Quest
    Else:
      Fail Quest (missed appointment)
```

### Seasonal Quest

```cpp
Quest: "Plant seeds in Spring"

On Quest Start:
  Get Current Season
  If Season != Spring:
    Quest Not Available

On Season Changed:
  If NewSeason == Summer && Quest Active:
    Fail Quest (too late to plant)
```

## Performance Testing

### Test Level Setup

```
1. Spawn 100 actors with TimeOfDayListenerComponent
2. Enable all event types
3. Set update frequency to 60 Hz
4. Profile with stat commands:
   - stat game
   - stat unitgraph
   - stat timeofdaysubsystem (if custom stat added)
```

### Expected Results
- < 0.1ms total time system cost
- < 0.001ms per event broadcast
- Scalable to 1000+ listeners

## Debug Commands

### Console Commands (add to subsystem)

```cpp
// In TimeOfDaySubsystem.cpp
static FAutoConsoleCommand TimeSetHourCmd(
    TEXT("Time.SetHour"),
    TEXT("Set the current hour (0-23)"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        // Implementation
    })
);
```

**Available Commands:**
- `Time.SetHour 12` - Jump to noon
- `Time.SetDay 5` - Jump to day 5
- `Time.SetScale 5` - Set 5x speed
- `Time.Pause` - Pause time
- `Time.Resume` - Resume time
- `Time.Debug` - Show debug info
