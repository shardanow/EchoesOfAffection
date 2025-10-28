# Actor Schedule System - Complete Documentation

## Overview
Professional, modular, data-driven schedule and action system for Unreal Engine 5.4+. Event-driven architecture with no per-actor ticks, designed for AAA production environments.

## Architecture

### Core Modules
- **ScheduleCore** - Core data structures, interfaces, and base classes
- **ScheduleRuntime** - Runtime execution, subsystems, and components
- **ScheduleEditor** - Editor tools, asset actions, and customizations

### Key Principles
- **SOLID Design** - Single responsibility, open/closed, dependency inversion
- **Data-Driven** - All configuration through Data Assets, no hardcoded logic
- **Event-Driven** - No per-actor ticks, updates triggered by time system events
- **Extensible** - Add new Actions/Conditions/Selectors without modifying core

## Quick Start

### 1. Enable Plugin
Add to your .uproject:
```json
{
  "Name": "ActorScheduleSystem",
  "Enabled": true
}
```

### 2. Create Schedule Data Asset
1. Right-click in Content Browser ? Miscellaneous ? Data Asset
2. Select `ScheduleData` as parent class
3. Name it (e.g., `DA_CivilianSchedule`)

### 3. Create Schedule Entry
1. Create another Data Asset with `ScheduleEntryData` parent
2. Configure:
   - **Time Condition**: When to execute (e.g., 08:00-17:00)
   - **Action**: What to do (e.g., Move To Location)
   - **Location Selector**: Where to go (e.g., Actor Tag Selector)
   - **Conditions**: Additional requirements (optional)

### 4. Add to Schedule
1. Open your Schedule Data Asset
2. Add the Entry to the `Entries` array
3. Set priority (higher = executes first)

### 5. Add Component to Actor
1. Select your actor (NPC, character, etc.)
2. Add Component ? `Schedule Component`
3. Assign your Schedule Data Asset

### 6. Setup Time Integration
1. Add `ScheduleTimeIntegrationComponent` to GameState or persistent actor
2. In your Time System Blueprint/C++, call `NotifyTimeChanged` when time updates

## Core Concepts

### Schedule Data (`UScheduleData`)
Primary asset defining an actor's complete daily routine.

**Properties:**
- `Entries` - Array of schedule entries (sorted by priority)
- `FallbackAction` - Action when no entry matches
- `bAllowConcurrentEntries` - Execute multiple actions simultaneously
- `MaxConcurrentEntries` - Limit concurrent actions

### Schedule Entry (`UScheduleEntryData`)
Atomic unit of scheduling: Time + Conditions + Action.

**Properties:**
- `TimeCondition` - When this entry is active
- `Conditions` - Additional requirements (AND logic)
- `LocationSelector` - Where the action occurs
- `Action` - What to execute
- `Priority` - Execution priority (0-100)
- `bCanBeInterrupted` - Allow player interruption
- `bResumeAfterInterruption` - Resume when interruption ends

### Time Conditions (`FScheduleTimeCondition`)
Define when an entry should execute.

**Types:**
- **TimeOfDay** - Specific time (e.g., 14:30)
- **TimeRange** - Time window (e.g., 08:00-17:00)
- **Duration** - Elapsed time from start
- **GameTime** - Based on game time

**Properties:**
- `DaysOfWeek` - Limit to specific days (1=Monday, 0=All)
- `StartHour/Minute` - Start time
- `EndHour/Minute` - End time (for ranges)

## Built-in Components

### Actions
- **MoveToLocation** - AI pathfinding movement
- **Wait** - Delay/pause for duration
- **PlayAnimation** - Play animation montage
- **Composite** - Chain multiple actions (sequential/parallel)
  - **NEW: Loop Support** - Can loop child actions indefinitely or N times (see [Loop Feature](Docs/FEATURE_Loop_Child_Actions.md))

### Conditions
- **GameplayTag** - Check actor tags

### Location Selectors
- **ActorTag** - Find actors by gameplay tags
  - Closest, Farthest, Random, First

## Creating Custom Components

### Custom Action (C++)
```cpp
UCLASS()
class UScheduleAction_Custom : public UScheduleActionBase
{
    GENERATED_BODY()
    
protected:
    virtual FScheduleActionHandle ExecuteActionInternal_Implementation(
        const FScheduleExecutionContext& Context) override
    {
        // Your execution logic
        FScheduleActionHandle Handle = Super::ExecuteActionInternal_Implementation(Context);
    
        // Track active handles
        return Handle;
    }
    
    virtual bool IsActionActiveInternal_Implementation(
        FScheduleActionHandle ActionHandle) const override
    {
   // Check if action is still running
        return false;
    }
};
```

### Custom Action (Blueprint)
1. Create Blueprint based on `ScheduleActionBase`
2. Override `Execute Action Internal` event
3. Implement your action logic
4. Return valid action handle

### Custom Condition (C++)
```cpp
UCLASS()
class UScheduleCondition_Weather : public UScheduleConditionBase
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EWeatherType> RequiredWeather;
  
protected:
    virtual bool EvaluateConditionInternal_Implementation(
   const FScheduleEvaluationContext& Context) const override
    {
  // Get weather system and check condition
    return true; // or false
    }
 
    virtual bool IsTimeBased_Implementation() const override
    {
     return false; // Weather is not time-based
    }
};
```

### Custom Location Selector (C++)
```cpp
UCLASS()
class UScheduleLocationSelector_NavMesh : public UScheduleLocationSelectorBase
{
    GENERATED_BODY()
    
protected:
    virtual bool SelectLocationInternal_Implementation(
        const FScheduleLocationContext& Context,
        FScheduleLocation& OutLocation) override
    {
        // Use navigation system to find valid location
        // Set OutLocation.Location, OutLocation.Rotation
   return true; // Success
  }
};
```

## Time System Integration

### Option 1: Blueprint Integration
```cpp
// In your Time System Blueprint
Event OnMinuteChanged
{
    // Get Schedule Time Integration Component
    Get Component by Class (ScheduleTimeIntegrationComponent)
    
  // Notify time changed
    Notify Time Changed (Hour, Minute, Day of Week)
}
```

### Option 2: C++ Integration
```cpp
// In your time system subsystem
void UMyTimeSystem::OnMinuteChanged(int32 NewHour, int32 NewMinute)
{
    // Get schedule subsystem
    if (UScheduleSubsystem* ScheduleSystem = GetWorld()->GetSubsystem<UScheduleSubsystem>())
    {
        ScheduleSystem->NotifyTimeChanged(NewHour, NewMinute, CurrentDayOfWeek);
    }
}
```

### Option 3: Event Dispatcher
```cpp
// Bind to your time system's event dispatcher
TimeSystem->OnTimeChanged.AddDynamic(this, &UScheduleTimeIntegrationComponent::NotifyTimeChanged);
```

## API Reference

### UScheduleComponent

**Properties:**
- `ScheduleData` - The schedule asset to use
- `bScheduleEnabled` - Enable/disable scheduling
- `PriorityOffset` - Adjust entry priorities for this actor
- `ActorScheduleTags` - Tags for condition evaluation

**Methods:**
- `InterruptCurrentAction(bool bCanResume)` - Stop current action
- `ResumeInterruptedAction()` - Resume paused action
- `CancelCurrentAction()` - Cancel completely
- `PauseSchedule()` - Pause all evaluation
- `ResumeSchedule()` - Resume evaluation
- `HasActiveAction()` - Check if action is running
- `GetCurrentEntry()` - Get active schedule entry

**Events:**
- `OnActionStarted` - Action begins
- `OnActionCompleted` - Action finishes naturally
- `OnActionInterrupted` - Action is interrupted

### UScheduleSubsystem

**Methods:**
- `NotifyTimeChanged(Hour, Minute, DayOfWeek)` - Trigger evaluation
- `RefreshAllSchedules()` - Force re-evaluation
- `GetActorsWithScheduleTags(Tags)` - Query scheduled actors
- `PauseAllSchedules()` - Global pause
- `ResumeAllSchedules()` - Global resume
- `GetScheduleStats(...)` - Performance metrics

**Events:**
- `OnScheduleEvent` - Global schedule events

## Best Practices

### Performance
1. **Use Priority Wisely** - Higher priority entries evaluated first
2. **Limit Conditions** - Each condition adds evaluation cost
3. **Batch Time Updates** - Update per-minute, not per-frame
4. **Use Concurrent Entries Sparingly** - Most NPCs need only 1 action

### Design
1. **Granular Entries** - Small, focused entries over complex ones
2. **Use Composite Actions** - Chain simple actions for complex behavior
3. **Leverage Tags** - Use gameplay tags for flexible conditions
4. **Fallback Actions** - Always provide default behavior

### Debugging
1. **Name Everything** - Use descriptive names for assets
2. **Visual Logger** - Enable in Project Settings ? AI ? Visual Logger
3. **Stats** - Use `GetScheduleStats` to monitor active actions
4. **Logs** - Check `LogTemp` for schedule system messages

## Example: Civilian Daily Routine

### Schedule: DA_CivilianSchedule

**Entry 1: Morning Work**
- Time: 08:00-12:00, Monday-Friday
- Action: Composite
  - Move To Location (Tag: Location.Workplace)
  - Play Animation (Working)
  - Wait (240 seconds = 4 hours game time)
- Priority: 80

**Entry 2: Lunch Break**
- Time: 12:00-13:00
- Action: Composite
  - Move To Location (Tag: Location.Restaurant)
  - Play Animation (Eating)
  - Wait (60 seconds)
- Priority: 90 (higher than work)

**Entry 3: Afternoon Work**
- Time: 13:00-17:00, Monday-Friday
- Action: Same as Entry 1
- Priority: 80

**Entry 4: Evening Home**
- Time: 17:00-22:00
- Action: Composite
  - Move To Location (Tag: Location.Home)
  - Play Animation (Relaxing)
- Priority: 70

**Entry 5: Sleep**
- Time: 22:00-08:00
- Action: Play Animation (Sleeping)
- Priority: 60

**Fallback Action: Idle**
- Action: Play Animation (Idle)

## Advanced Features

### Looping Child Actions (NEW in v1.1)
Composite actions can now loop their child actions indefinitely or for a specific number of iterations:

```cpp
Composite Action: "Factory Worker Loop"
??? bExecuteSequentially = true
??? bLoopChildActions = true      // Enable looping
??? MaxLoops = 0       // 0 = infinite, or set a number
??? Child Actions:
    1. MoveToLocation (Machine)
    2. PlayAnimation (Working)
    3. Wait (30 seconds)
    4. MoveToLocation (Storage)
 5. Wait (10 seconds)
    // Loops back to step 1 automatically
```

**Use cases:**
- Work shifts with repeating cycles
- Patrol routes that loop
- Repeating daily routines within a time window

See [Loop Feature Documentation](Docs/FEATURE_Loop_Child_Actions.md) for details.

### NavArea Costs for Roads

**? FULLY SUPPORTED** - NPCs can follow roads instead of cutting through terrain!

The `FilterClass` property in `MoveToLocation` allows precise control over NPC pathfinding:

**Quick Setup (5 minutes):**
1. Create `NavigationQueryFilter` Data Asset
2. Configure area costs (Road = 0.5, Grass = 100.0)
3. Place `Nav Modifier Volumes` over roads
4. Assign filter to `MoveToLocation` action
5. NPCs automatically prefer roads! ???

**?? Documentation:**
- ?? **[Quick Start (5 min)](Docs/ROAD_QUICK_START_RU.md)** - Get started immediately
- ?? **[Complete Guide](Docs/ROAD_NAVIGATION_RU.md)** - Detailed tutorial with examples
- ?? **[Visual Guide](Docs/ROAD_VISUAL_GUIDE_RU.md)** - Step-by-step screenshots
- ?? **[C++ Examples](Docs/ROAD_NAVIGATION_CPP.md)** - For programmers
- ?? **[Index](Docs/ROAD_NAVIGATION_INDEX.md)** - All road navigation docs

**Example:**
```cpp
MoveToLocation Action:
  ?? Filter Class: NQF_RoadOnly  ? Navigation filter
  ?? Location Selector: ActorTag (Workplace)
  ?? bUsePathfinding: true

Result: NPC walks on roads only, no shortcuts through grass!
```

**Visual check:** Press **P** in PIE to see NavMesh colors:
- ?? Yellow = Road (low cost, preferred)
- ?? Blue = Default (high cost, avoided)
