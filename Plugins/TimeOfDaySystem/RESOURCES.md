# Time of Day System - Resources and Assets

## Plugin Icon

The plugin uses a clock/sun icon to represent time of day functionality.

### Icon Specifications
- **Size**: 128x128 pixels
- **Format**: PNG with transparency
- **Style**: Minimalist, professional
- **Colors**: Warm gradients (sunrise/sunset theme)

### Icon Location
Place icon at: `Plugins/TimeOfDaySystem/Resources/Icon128.png`

## Example Content

### Recommended Content to Create

#### 1. Data Assets
Create in: `Content/TimeOfDaySystem/DataAssets/`
- `DA_TimeOfDay_Default` - 20-minute days
- `DA_TimeOfDay_Fast` - 10-minute days
- `DA_TimeOfDay_Realistic` - 2-hour days
- `DA_TimeOfDay_Debug` - 2-minute days for testing

#### 2. UI Widgets
Create in: `Content/TimeOfDaySystem/UI/`
- `WBP_TimeDisplay_HUD` - Main HUD clock
- `WBP_TimeDisplay_Full` - Detailed time panel
- `WBP_TimeControls` - Debug time controls
- `WBP_Calendar` - Calendar widget

#### 3. Example Actors
Create in: `Content/TimeOfDaySystem/Examples/`
- `BP_StreetLamp` - Phase-based lighting
- `BP_DayNightSun` - Directional light
- `BP_TimeDebug` - Visual debug actor
- `BP_NPCSchedule` - NPC with schedule

#### 4. Materials
Create in: `Content/TimeOfDaySystem/Materials/`
- `M_SkyDome` - Sky material with time-based colors
- `M_PhaseIndicator` - Phase visualization
- `MI_LampLight` - Lamp material instances

## Texture Assets

### UI Textures
- `T_Clock_Background` - Clock widget background
- `T_Sun_Icon` - Day phase icon
- `T_Moon_Icon` - Night phase icon
- `T_Season_Spring` - Spring season icon
- `T_Season_Summer` - Summer season icon
- `T_Season_Autumn` - Autumn season icon
- `T_Season_Winter` - Winter season icon

### Phase Icons
- `T_Phase_Night` - Night icon
- `T_Phase_Dawn` - Dawn/sunrise icon
- `T_Phase_Morning` - Morning icon
- `T_Phase_Noon` - Noon/sun icon
- `T_Phase_Afternoon` - Afternoon icon
- `T_Phase_Dusk` - Dusk/sunset icon
- `T_Phase_Evening` - Evening icon

## Audio Assets (Optional)

### Ambient Sounds
- `A_Ambient_Morning` - Birds chirping
- `A_Ambient_Day` - Daytime ambience
- `A_Ambient_Evening` - Evening crickets
- `A_Ambient_Night` - Night owls, wind

### Music
- `M_Phase_Morning` - Peaceful morning music
- `M_Phase_Day` - Upbeat day music
- `M_Phase_Evening` - Calm evening music
- `M_Phase_Night` - Ambient night music

## Sample Level

### Demo Level: `L_TimeOfDayDemo`

**Setup:**
1. Post Process Volume with time-based settings
2. Directional Light (BP_DayNightSun)
3. Sky Sphere with time synchronization
4. 10-20 Street Lamps (BP_StreetLamp)
5. Debug Actor (BP_TimeDebug)
6. Example NPCs with schedules
7. Time Control UI in HUD

**Demonstrates:**
- Full day/night cycle
- Phase transitions
- Seasonal changes
- UI integration
- Event system
- NPC schedules
- Dynamic lighting

## Content Browser Structure

```
Content/
??? TimeOfDaySystem/
  ??? DataAssets/
    ?   ??? DA_TimeOfDay_Default
    ?   ??? DA_TimeOfDay_Fast
    ?   ??? DA_TimeOfDay_Realistic
?   ??? DA_TimeOfDay_Debug
    ?
    ??? UI/
    ?   ??? Widgets/
    ? ?   ??? WBP_TimeDisplay_HUD
    ?   ?   ??? WBP_TimeDisplay_Full
    ?   ?   ??? WBP_TimeControls
    ?   ?   ??? WBP_Calendar
    ?   ??? Textures/
    ?       ??? Icons/
    ?       ??? Backgrounds/
    ?
    ??? Examples/
    ?   ??? Blueprints/
  ?   ?   ??? BP_StreetLamp
    ?   ?   ??? BP_DayNightSun
    ?   ?   ??? BP_TimeDebug
    ?   ?   ??? BP_NPCSchedule
 ?   ??? Materials/
?       ??? Instances/
    ?
    ??? Materials/
    ?   ??? M_SkyDome
    ?   ??? M_PhaseIndicator
    ?
    ??? Levels/
        ??? L_TimeOfDayDemo
```

## Blueprint Library Nodes

All nodes available in Blueprint:

### Category: "Time of Day"
- Get Time of Day Subsystem
- Get Current Time
- Get Current Phase
- Get Normalized Time of Day
- Get Current Season
- Is Daytime
- Is Nighttime
- Get Formatted Time
- Get Formatted Date

### Category: "Time of Day|Control"
- Set Current Time
- Advance Time
- Set Time Scale
- Get Time Scale
- Pause Time
- Resume Time
- Is Time Paused

### Category: "Time of Day|Utilities"
- Make Game Time
- Break Game Time
- Is Time After
- Get Time Difference
- Hours To Seconds
- Days To Seconds
- Get Phase Display Name
- Get Season Display Name
- Get Weekday Display Name

## Editor Integration

### Project Settings
Add section: `Project Settings ? Game ? Time of Day`
- Default Time Settings Asset
- Auto-Initialize on Game Start
- Debug Visualization

### Editor Preferences
Add section: `Editor Preferences ? Time of Day`
- Show Time in Editor
- Editor Time Scale
- Auto-Pause in Editor

## Localization

### Supported Languages
- English (default)
- Russian (optional)
- More can be added

### Localized Strings
- Phase names
- Season names
- Weekday names
- UI labels
- Tooltips
- Error messages

## Performance Profiling

### Stat Commands
```
stat TimeOfDay       // Show time system stats
stat TimeOfDayEvents // Show event broadcast stats
stat TimeOfDayUI     // Show UI update stats
```

### Console Commands
```
time.sethour [0-23]    // Set current hour
time.setday [1-365]    // Set current day
time.setscale [float]// Set time scale
time.pause   // Pause time
time.resume      // Resume time
time.skipday     // Skip to next day
time.debug   // Toggle debug display
```

## Testing Assets

### Unit Test Maps
- `L_TimeTest_Basic` - Basic functionality
- `L_TimeTest_Events` - Event system
- `L_TimeTest_Performance` - Performance test
- `L_TimeTest_Multiplayer` - Network sync (future)

### Test Actors
- `BP_TimeTest_EventListener` - Event testing
- `BP_TimeTest_PerformanceStress` - 1000+ listeners
- `BP_TimeTest_SaveLoad` - Persistence testing

## Marketplace Assets (Optional Integration)

### Lighting
- Ultra Dynamic Sky
- Time of Day (Epic)
- Sky Atmosphere

### Audio
- Environmental Audio Pack
- Ambient Sound Library
- Dynamic Music System

### Weather
- Ultra Dynamic Weather
- Realistic Weather System
- Climate System

## Development Tools

### Debug Visualizers
- Time Flow Visualizer (shows time progression)
- Event Graph (shows event connections)
- Phase Timeline (visual phase display)
- Season Calendar (calendar view)

### Profiling Tools
- Event Performance Monitor
- Listener Count Display
- Memory Usage Tracker
- Tick Time Graph

## Documentation Assets

### Video Tutorials (Recommended)
1. "Quick Setup" (5 minutes)
2. "Creating Time-Aware Actors" (10 minutes)
3. "UI Integration" (8 minutes)
4. "Advanced: Weather Integration" (15 minutes)
5. "Advanced: NPC Schedules" (12 minutes)

### Example Projects
- Minimal Setup Project
- Full RPG Integration
- Simulation Game Example
- Multiplayer Demo (future)

## Community Content

### Sharable Content Packs
- Time Settings Collection
- UI Theme Pack
- Example Actor Pack
- Integration Examples

### Template Projects
- Life Simulation Template
- RPG Time Template
- Farming Sim Template
- Survival Game Template

---

**Note**: This is a resource planning document. Actual asset creation would be done in Unreal Editor.

For creating these assets:
1. Open Unreal Editor
2. Navigate to Content/TimeOfDaySystem/
3. Create assets as described above
4. Follow CONTENT_GUIDE.md for detailed instructions

**Status**: Documentation complete, assets to be created in editor
**Version**: 1.0.0
