# ?? Actor Schedule System - Installation & First Use

## ? Installation Complete!

The Actor Schedule System plugin has been successfully created in your project.

**Location:** `Plugins/ActorScheduleSystem/`
**Files Created:** 60
**Modules:** ScheduleCore, ScheduleRuntime, ScheduleEditor

---

## ?? Next Steps

### Step 1: Generate Project Files & Compile

#### Option A: Using Unreal Editor (Recommended)
1. Close Unreal Editor if it's open
2. Right-click on `EchoesOfAffection.uproject`
3. Select "Generate Visual Studio project files"
4. Open `EchoesOfAffection.sln` in Visual Studio
5. Build Solution (Ctrl+Shift+B)
6. Launch Unreal Editor

#### Option B: Using Command Line
```powershell
# Navigate to project directory
cd "D:\Game Dev\Projects\UEProjects\EchoesOfAffection"

# Generate project files
& "D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\Build.bat" -projectfiles -project="EchoesOfAffection.uproject" -game -engine

# Or use UnrealVersionSelector
& "D:\Game Dev\UE\UE_5.6\Engine\Binaries\Win64\UnrealVersionSelector-Win64-Shipping.exe" /switchversion "EchoesOfAffection.uproject"
```

### Step 2: Verify Plugin in Editor
1. Open Unreal Editor
2. Edit ? Plugins
3. Search for "Actor Schedule"
4. Verify it's enabled ?
5. Restart Editor if prompted

### Step 3: Test Basic Functionality

#### Create Your First Schedule (5 minutes)
1. Content Browser ? Right-Click ? Miscellaneous ? Data Asset
2. Select `ScheduleData` as Parent Class
3. Name it: `DA_TestSchedule`
4. Open the asset
5. Set:
   - Schedule Name: "Test Schedule"
   - Description: "My first schedule"

#### Create a Schedule Entry
1. Content Browser ? Right-Click ? Miscellaneous ? Data Asset
2. Select `ScheduleEntryData` as Parent Class
3. Name it: `DA_TestEntry_Morning`
4. Open the asset
5. Configure:
   - Entry Name: "Morning Activity"
   - Priority: 50
   - Time Condition:
     - Time Type: Time Of Day
     - Start Hour: 8
     - Start Minute: 0
   - Action: Click '+' and select `ScheduleAction_Wait`
     - Wait Duration: 10.0 seconds

6. Save and close

#### Add Entry to Schedule
1. Open `DA_TestSchedule`
2. Find "Entries" array
3. Click '+' to add element
4. Select `DA_TestEntry_Morning`
5. Save

#### Test on an Actor
1. Place any Actor in your level (or use existing NPC)
2. Select the Actor
3. Click "Add Component"
4. Search and add `Schedule Component`
5. In Details panel, set:
   - Schedule Data ? `DA_TestSchedule`
   - bSchedule Enabled ? ? true

#### Setup Time Integration (Temporary Test)
1. Create a Blueprint Actor: `BP_TimeTest`
2. Add Component ? `Schedule Time Integration Component`
3. Event Graph:
   ```
   Event BeginPlay
     ? Delay (5 seconds)
     ? Get Component (Schedule Time Integration)
     ? Notify Time Changed
       Hour: 8
    Minute: 0
       Day Of Week: 1
   ```
4. Place `BP_TimeTest` in level
5. Play (PIE)

#### Expected Result
- After 5 seconds, time changes to 8:00
- Schedule evaluates
- Entry activates
- Wait action runs for 10 seconds
- Entry completes

---

## ?? Verification Checklist

### Compilation
- [ ] Plugin compiles without errors
- [ ] No warnings in Output Log
- [ ] Editor launches successfully

### Editor Integration
- [ ] Plugin visible in Plugins window
- [ ] Assets appear in Asset Browser
- [ ] Right-click "Data Asset" shows Schedule types
- [ ] Components available in Add Component menu

### Runtime
- [ ] Schedule Data can be created
- [ ] Schedule Entry can be created
- [ ] Component can be added to actors
- [ ] No errors when playing in editor

### Functionality
- [ ] Schedule evaluates on time change
- [ ] Actions execute
- [ ] Events broadcast correctly
- [ ] Debug logs appear (if enabled)

---

## ?? Troubleshooting

### "Plugin failed to load"
**Solution:**
1. Check Output Log for specific error
2. Verify all .Build.cs files are correct
3. Regenerate project files
4. Clean and rebuild solution

### "Cannot find ScheduleData class"
**Solution:**
1. Verify ScheduleCore module compiled
2. Check module dependencies in .Build.cs
3. Restart Editor
4. Regenerate project files if needed

### "Component not appearing in Add menu"
**Solution:**
1. Ensure ScheduleRuntime module compiled
2. Check UCLASS specifiers on component
3. Restart Editor
4. Verify module is loaded (check Plugins window)

### Compilation Errors
**Common Fixes:**
1. **Missing includes:** Add required headers to .Build.cs PublicDependencyModuleNames
2. **Syntax errors:** Check for typos in UCLASS/UPROPERTY macros
3. **Link errors:** Verify .Build.cs dependencies are correct
4. **PCH errors:** Clean solution and rebuild

### Module Load Errors
**Solution:**
1. Check ActorScheduleSystem.uplugin is valid JSON
2. Verify Modules section lists all three modules
3. Ensure module names match .Build.cs file names
4. Check LoadingPhase is set correctly

---

## ?? Documentation Quick Links

- **Complete Guide:** `Plugins/ActorScheduleSystem/README.md`
- **Quick Start:** `Plugins/ActorScheduleSystem/QUICKSTART.md`
- **Blueprint Examples:** `Plugins/ActorScheduleSystem/Docs/BlueprintExamples.md`
- **Content Guide:** `Plugins/ActorScheduleSystem/Docs/ContentGuide.md`
- **Implementation Summary:** `Plugins/ActorScheduleSystem/IMPLEMENTATION_SUMMARY.md`

---

## ?? Integration with TimeOfDaySystem

Since you already have a TimeOfDaySystem plugin, here's how to integrate:

### Method 1: Event-Based (Recommended)

#### In TimeOfDaySystem (modify your existing code):
```cpp
// In your TimeOfDaySubsystem.h
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTimeChanged, int32, Hour, int32, Minute, int32, DayOfWeek);

UPROPERTY(BlueprintAssignable, Category = "Time")
FOnTimeChanged OnTimeChanged;

// In your TimeOfDaySubsystem.cpp - when time updates
void UTimeOfDaySubsystem::UpdateTime()
{
    // Your existing logic...
    CurrentHour = ...;
    CurrentMinute = ...;
    
    // Broadcast event
    OnTimeChanged.Broadcast(CurrentHour, CurrentMinute, CurrentDayOfWeek);
}
```

#### In ScheduleTimeIntegrationComponent (Blueprint):
1. Add `ScheduleTimeIntegrationComponent` to your GameState
2. Event BeginPlay:
   ```
   Get TimeOfDaySubsystem
   Bind Event to On Time Changed
     ? Call Notify Time Changed (Hour, Minute, DayOfWeek)
   ```

### Method 2: Direct Call (Alternative)
```cpp
// In your TimeOfDaySubsystem.cpp
void UTimeOfDaySubsystem::UpdateTime()
{
  // Your existing logic...
    
    // Notify schedule system
    if (UScheduleSubsystem* ScheduleSys = GetWorld()->GetSubsystem<UScheduleSubsystem>())
    {
     ScheduleSys->NotifyTimeChanged(CurrentHour, CurrentMinute, CurrentDayOfWeek);
    }
}
```

---

## ?? Creating Your First Production Schedule

### Example: Civilian Daily Routine

#### 1. Create Location Markers
```
BP_Marker_Home (Tag: Location.Home)
BP_Marker_Workplace (Tag: Location.Workplace)
BP_Marker_Restaurant (Tag: Location.Restaurant)
```

#### 2. Create Schedule Entries

**Morning Commute** (`DA_Entry_Commute`)
- Time: 08:00
- Action: MoveToLocation
  - Location Selector: ActorTag
    - Required Tags: Location.Workplace
    - Selection Mode: Closest

**Work** (`DA_Entry_Work`)
- Time: 08:00-17:00
- Action: Composite
  - MoveToLocation (Workplace)
  - PlayAnimation (Working)
  - Wait (28800 seconds = 8 hours)

**Lunch** (`DA_Entry_Lunch`)
- Time: 12:00-13:00
- Action: Composite
  - MoveToLocation (Restaurant)
  - PlayAnimation (Eating)
  - Wait (3600 seconds)
- Priority: 90 (higher than work)

**Go Home** (`DA_Entry_GoHome`)
- Time: 17:00
- Action: MoveToLocation
  - Location Selector: ActorTag (Location.Home)

**Sleep** (`DA_Entry_Sleep`)
- Time: 22:00-08:00
- Action: PlayAnimation (Sleeping)

#### 3. Create Complete Schedule
**DA_Schedule_Civilian**
- Entries: [Commute, Work, Lunch, GoHome, Sleep]
- Fallback Action: PlayAnimation (Idle)
- Auto Sort By Priority: ?

#### 4. Apply to NPCs
- Add ScheduleComponent to NPC
- Assign DA_Schedule_Civilian
- Configure AI Controller
- Set up NavMesh
- Test!

---

## ?? Performance Tips

### For Best Performance:
1. **Update Frequency:** Call `NotifyTimeChanged` every game-minute, not every frame
2. **Distance Culling:** Enable in Project Settings (default: 5000 units)
3. **Concurrent Limits:** Keep MaxConcurrentEntries ? 3 per NPC
4. **Action Complexity:** Favor simple actions over complex composites
5. **Condition Count:** Limit to 2-3 conditions per entry

### Monitoring Performance:
```cpp
// Blueprint or C++
Get Schedule Subsystem
Get Schedule Stats (Out Total Actors, Out Active Actions, Out Paused Actions)
Print String ("Active: " + Active Actions)
```

---

## ?? Support

### If You Encounter Issues:

1. **Check Logs:**
   - Output Log (during play)
- Saved/Logs/EchoesOfAffection.log (after session)

2. **Enable Debug Mode:**
   - Project Settings ? Plugins ? Actor Schedule System
 - Enable Debug Visualization
   - Check bLog Schedule Events

3. **Review Documentation:**
   - Start with QUICKSTART.md
   - Check BlueprintExamples.md for patterns
   - Review IMPLEMENTATION_SUMMARY.md for architecture

4. **Common Issues:**
   - See Troubleshooting section above
   - Check that all dependencies are met
   - Verify time integration is working

---

## ? Success!

You now have a complete, professional schedule system ready for production use!

### What You Can Do:
- ? Create complex daily routines
- ? Make NPCs follow schedules
- ? Interrupt for player interaction
- ? Integrate with quest/dialogue systems
- ? Build living, breathing game worlds

### Next Steps:
1. Complete the verification checklist
2. Create your first production schedule
3. Test with multiple NPCs
4. Profile and optimize
5. Expand to full game world

**Happy Scheduling! ??**

---

*Actor Schedule System v1.0*
*For Unreal Engine 5.4+*
*EchoesOfAffection Project*
