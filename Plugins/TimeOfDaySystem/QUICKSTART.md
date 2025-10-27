# Quick Start Guide - Time of Day System

## Installation

### Step 1: Add Plugin to Project

1. Copy the `TimeOfDaySystem` folder to your project's `Plugins` directory
2. If `Plugins` folder doesn't exist, create it in your project root
3. Regenerate project files (right-click .uproject ? Generate Visual Studio project files)
4. Open project in Visual Studio and build

### Step 2: Enable Plugin

1. Open Unreal Editor
2. Edit ? Plugins
3. Search for "Time of Day"
4. Enable "Time of Day System"
5. Restart editor

## Basic Setup (5 Minutes)

### 1. Create Settings Asset

1. Content Browser ? Right Click ? Miscellaneous ? Data Asset
2. Select `TimeOfDaySettings`
3. Name it: `DA_TimeOfDay_Default`
4. Open and configure:
   - Real Seconds Per Game Day: `1200` (20 minutes)
   - Initial Game Time: Hour `6`, Minute `0` (6 AM start)
   - Keep default day phases

### 2. Initialize in Level

**Option A: Level Blueprint**
```
Event BeginPlay
  ?
  ?
Get Time of Day Subsystem
  ?
  ?
Initialize With Settings [DA_TimeOfDay_Default]
```

**Option B: Game Mode (Recommended)**

### Вариант 1: Простая инициализация (рекомендуется)

1. **Откройте ваш Game Mode Blueprint** (например, `BP_GameMode`)

2. **В Event Graph добавьте:**
```
Event BeginPlay
  ?
    ?
Get Time of Day Subsystem (World Context = Self)
    ?
    ?
Initialize With Settings
    ?? Settings = [DA_TimeOfDay_Default]  (выберите созданный DataAsset)
    ?? (подключите к Execute)
```

### Вариант 2: С использованием BP_ методов

```
Event BeginPlay
    ?
    ?
Get Time of Day Subsystem
    ?
    ?
Initialize With Settings [DA_TimeOfDay_Default]
    ?
    ?
Get Current Phase (BP)  ? Обратите внимание: используем BP версию!
    ?
    ?
Print String (для проверки)
```

### 3. Add UI Widget

**Create Widget: WBP_TimeDisplay**

1. Content Browser ? Right Click ? User Interface ? Widget Blueprint
2. Name: `WBP_TimeDisplay`
3. Open widget
4. Add to hierarchy:
   - Canvas Panel
   - Text Block ? Name: `TimeText`
5. Class Settings ? Parent Class: `TimeOfDayWidget`
6. Compile & Save

**Add to HUD:**
```
Event BeginPlay (Player Controller)
  ?
  ?
Create Widget [WBP_TimeDisplay]
  ?
  ?
Add to Viewport
```

**You're done!** Time will now advance and display on screen.

## 10-Minute Full Setup

### 1. Create Complete Settings

**DA_TimeOfDay_RPG:**
```
Time Conversion:
  Real Seconds Per Game Day: 1800 (30 minutes)
  Time Scale: 1.0
  Is Paused: false

Calendar:
  Days Per Season: 30
  Seasons Per Year: 4
  Days Per Week: 7

Initial Game Time:
  Year: 1
  Season: Spring
  Day of Season: 1
  Day of Week: Monday
  Hour: 7
  Minute: 0

Day Phases:
  Night:     00:00
  Dawn:      05:00
  Morning:   07:00
  Noon:    12:00
  Afternoon: 15:00
  Dusk:      18:00
  Evening:   20:00

Advanced:
  Auto Advance Time: true
  Update Frequency: 10 Hz
  Persist Time State: true
```

### 2. Create Street Lamp Actor

**Blueprint: BP_StreetLamp**

1. Create Actor Blueprint
2. Add Components:
   - Static Mesh (lamp post)
   - Point Light
   - Time of Day Listener Component

3. Set Listener Component:
   - Receive Phase Events: ?
   - Other events: ?

4. Event Graph:
```
On Phase Changed (from component)
  ?
  ?
Switch on Phase
  ?? Night ? Enable Light
  ?? Evening ? Enable Light
  ?? Default ? Disable Light
```

### 3. Create Day/Night Directional Light

**Blueprint: BP_DayNightSun**

1. Create Actor Blueprint
2. Add Components:
   - Directional Light Component
   - Time of Day Listener Component

3. Set Listener:
   - Receive Phase Events: ?
   - Receive Hour Events: ? (for smooth rotation)

4. Event Graph:
```
Event Tick
  ?
  ?
Get Time of Day Subsystem
  ?
  ?
Get Normalized Time of Day (0-1)
  ?
  ?
Map to Rotation: Pitch = (NormalizedTime * 180) - 90
  ?
  ?
Set Actor Rotation

On Phase Changed
  ?
  ?
Switch on Phase
  ?? Night ? Intensity = 0.1, Color = Dark Blue
  ?? Dawn ? Intensity = 0.5, Color = Orange
  ?? Morning ? Intensity = 1.0, Color = White
  ?? Noon ? Intensity = 1.5, Color = Bright White
?? Afternoon ? Intensity = 1.2, Color = Warm White
  ?? Dusk ? Intensity = 0.6, Color = Orange/Red
  ?? Evening ? Intensity = 0.3, Color = Blue
```

### 4. Create Time Control UI

**Widget: WBP_TimeControls**

Elements:
- Button: Pause/Resume
- Button: 0.5x Speed
- Button: 1x Speed
- Button: 2x Speed
- Button: 5x Speed
- Button: Skip 1 Hour
- Button: Skip 1 Day

**Event Graph:**
```
Button "Pause/Resume" Clicked
  ?
  ?
Get Time of Day Subsystem
  ?
  ?
Branch on Is Time Paused
  ?? True ? Resume Time
  ?? False ? Pause Time

Button "2x" Clicked
  ?
  ?
Get Time of Day Subsystem ? Set Time Scale (2.0)

Button "Skip Hour" Clicked
  ?
  ?
Get Time of Day Subsystem ? Advance Time (3600.0)
```

### 5. Create NPC with Schedule

**Blueprint: BP_NPC_Scheduled**

1. Create Character Blueprint
2. Add Component:
   - Time of Day Listener Component
   - Set: Receive Hour Events ?

3. Create Schedule Data Table:

**DT_NPCSchedule:**
| Hour | Activity | Location |
|------|----------|----------|
| 7 | Wake Up | Home |
| 8 | Go to Work | Workplace |
| 12 | Lunch Break | Restaurant |
| 13 | Return to Work | Workplace |
| 17 | Go Home | Home |
| 19 | Dinner | Home |
| 22 | Sleep | Home |

4. Event Graph:
```
On Hour Changed
  ?
  ?
Get Row from Data Table [Hour]
  ?
  ?
Switch on Activity
  ?? "Go to Work" ? Move to Workplace
  ?? "Lunch Break" ? Move to Restaurant
  ?? "Go Home" ? Move to Home
  ?? "Sleep" ? Play Sleep Animation
```

## Common Use Cases

### Use Case 1: Quest "Meet at Noon"

```cpp
// In Quest System
void UQuest_MeetAtNoon::OnQuestStart()
{
    UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();
    TimeSystem->OnHourChanged.AddDynamic(this, &UQuest_MeetAtNoon::CheckQuestTime);
}

void UQuest_MeetAtNoon::CheckQuestTime(const FGameTime& CurrentTime)
{
 if (CurrentTime.Hour == 12 && CurrentTime.Minute < 15)
    {
    // Player has 15 minutes to complete
        if (IsPlayerAtLocation())
        {
       CompleteQuest();
   }
    }
    else if (CurrentTime.Hour == 12 && CurrentTime.Minute >= 15)
 {
        FailQuest(); // Missed appointment
    }
}
```

### Use Case 2: Shop Hours

```cpp
// BP_Shop
On Hour Changed
  ?
  ?
If Hour >= 9 AND Hour < 18
  ?? Open Shop
  ?   ?? Enable Interaction
  ?   ?? Show "Open" Sign
  ?? Close Shop
   ?? Disable Interaction
   ?? Show "Closed" Sign
```

### Use Case 3: Dynamic Music

```cpp
// Music Manager
On Phase Changed
  ?
  ?
Switch on Phase
  ?? Morning ? Play Peaceful Music
  ?? Afternoon ? Play Upbeat Music
  ?? Evening ? Play Calm Music
  ?? Night ? Play Ambient/Creepy Music
```

### Use Case 4: Enemy Spawn by Time

```cpp
// Enemy Spawner
On Phase Changed
  ?
  ?
If Phase == Night
  ?? Enable Spawner
  ?   ?? Spawn Dangerous Enemies
  ?? Disable Spawner
```

### Use Case 5: Crop Growth

```cpp
// Farming System
On Day Changed
  ?
  ?
For Each Planted Crop
  ?
  ?
Increment Growth Days
  ?
  ?
If Growth Days >= Required Days
  ?? Set Crop to Harvestable
```

## Debug Tips

### Console Commands (add these to your game)

```cpp
// In your custom subsystem or player controller
IConsoleCommand* TimeSetHourCommand = IConsoleManager::Get().RegisterConsoleCommand(
    TEXT("time.sethour"),
    TEXT("Set current hour (0-23)"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
 // Implementation
    })
);
```

**Useful commands to add:**
- `time.sethour 12` - Jump to noon
- `time.setscale 10` - 10x speed
- `time.pause` - Pause time
- `time.resume` - Resume time
- `time.skipday` - Skip to next day
- `time.debug` - Show time debug info

### Debug Actor in Level

Place `BP_TimeDebugActor` in your level to see:
- Current time (HH:MM:SS)
- Current date
- Current phase
- Day/night status

### Blueprint Debug

Add to HUD:
```
Event Tick
  ?
  ?
Get Time of Day Subsystem
  ?
  ?
Get Formatted Time ? Print String
Get Formatted Date ? Print String
Get Current Phase ? Print String
```

## Performance Tips

### Optimize Event Subscriptions

**Bad:**
```cpp
// Subscribes to ALL events
TimeListener->bReceiveMinuteEvents = true;
TimeListener->bReceiveHourEvents = true;
TimeListener->bReceiveDayEvents = true;
TimeListener->bReceivePhaseEvents = true;
// ... but only uses one!
```

**Good:**
```cpp
// Only subscribe to what you need
TimeListener->bReceiveMinuteEvents = false;
TimeListener->bReceiveHourEvents = false;
TimeListener->bReceiveDayEvents = false;
TimeListener->bReceivePhaseEvents = true; // Only this!
```

### Batch Updates

**Bad:**
```cpp
On Hour Changed ? Update Light
On Hour Changed ? Update Sound
On Hour Changed ? Update Weather
// Each binds separately
```

**Good:**
```cpp
On Hour Changed
  ?
  ?
Update All Systems at Once
  ?? Update Light
  ?? Update Sound
  ?? Update Weather
```

### Use Appropriate Update Frequency

```
Fast-paced game: 30-60 Hz
Standard game: 10-20 Hz
Turn-based game: Manual updates only
```

## Troubleshooting

### "Time not advancing"
- Check: `bAutoAdvanceTime = true` in settings
- Check: Time is not paused
- Check: Time scale > 0

### "Events not firing"
- Check: Listener is registered
- Check: Event flags are enabled on component
- Check: Subsystem is initialized

### "UI not updating"
- Check: Widget is properly constructed
- Check: `bAutoUpdate = true`
- Check: Text blocks are bound correctly

### "Performance issues"
- Reduce update frequency
- Disable unused events
- Check number of listeners (<1000 recommended)

## Next Steps

1. **Integrate with Save System**
   - Save `FGameTime` to save game
   - Restore on load

2. **Add Weather System**
   - Create weather subsystem
 - Listen to season/phase changes
   - Adjust weather accordingly

3. **Create Day/Night Post Process**
   - Adjust exposure based on time
   - Adjust color grading
   - Adjust fog density

4. **Add Moon Phases**
   - Extend with lunar cycle
   - Affect tides, werewolves, etc.

5. **Create Calendar Events**
   - Birthdays, holidays, festivals
   - Quest deadlines
   - Seasonal events

## Resources

- Full Documentation: `README.md`
- Architecture Details: `ARCHITECTURE.md`
- Extension Guide: `EXTENSION_GUIDE.md`
- Content Guide: `CONTENT_GUIDE.md`

## Support

For issues and questions:
- Check documentation first
- Review example actors
- Enable debug logging
- Check console for errors

Happy time management! ??
