# Blueprint Integration Examples

## Example 1: Simple NPC with Daily Routine

### Step 1: Create Location Markers
1. Place empty actors in your level:
   - "Home_Marker" (tag: `Location.Home`)
   - "Work_Marker" (tag: `Location.Workplace`)
   - "Shop_Marker" (tag: `Location.Shop`)

2. Add `GameplayTagAssetInterface` to markers
3. Assign appropriate tags

### Step 2: Create Schedule Entry - Morning Commute
**Asset: DA_Entry_MorningCommute**

Properties:
```
Entry Name: "Morning Commute"
Priority: 80

Time Condition:
  Time Type: Time Of Day
  Start Hour: 8
  Start Minute: 0
  Days Of Week: [1,2,3,4,5] // Monday-Friday

Location Selector: ScheduleLocationSelector_ActorTag
  Required Location Tags: Location.Workplace
  Selection Mode: Closest

Action: ScheduleAction_Composite
  Execute Sequentially: true
  Child Actions:
    [0] ScheduleAction_MoveToLocation
      Acceptance Radius: 100
        Use Pathfinding: true
    [1] ScheduleAction_PlayAnimation
        Animation Montage: AM_Working
    [2] ScheduleAction_Wait
    Wait Duration: 14400 // 4 hours game time
```

### Step 3: Create Complete Schedule
**Asset: DA_NPCSchedule_Worker**

Properties:
```
Schedule Name: "Worker Daily Routine"
Entries: [
  DA_Entry_MorningCommute,
  DA_Entry_LunchBreak,
  DA_Entry_AfternoonWork,
  DA_Entry_EveningHome,
  DA_Entry_NightSleep
]
Fallback Action: ScheduleAction_PlayAnimation (AM_Idle)
bAutoSortByPriority: true
```

### Step 4: Setup Actor Blueprint
**BP_NPC_Worker**

Components:
- Skeletal Mesh
- Character Movement
- AIController Reference
- **Schedule Component**
  - Schedule Data: DA_NPCSchedule_Worker
  - bSchedule Enabled: true

### Step 5: Time Integration
**BP_GameMode or BP_GameState**

Add Component: Schedule Time Integration Component

Event Graph:
```blueprint
Event BeginPlay
  ? Get Schedule Time Integration Component
  ? Subscribe To Time Events

// From your time system
Event OnTimeUpdated (Hour, Minute, Day)
  ? Get Schedule Time Integration Component
  ? Notify Time Changed (Hour, Minute, Day)
```

## Example 2: Weather-Dependent Schedule

### Custom Condition Blueprint
**BP_Condition_Weather**
Parent: ScheduleConditionBase

Variables:
- Required Weather (Enum): Sunny

Event Graph:
```blueprint
Event Evaluate Condition Internal
  ? Get Weather System
  ? Get Current Weather
  ? Return (Current Weather == Required Weather)
```

### Schedule Entry with Weather
**DA_Entry_OutdoorWork**

Conditions:
- [0] BP_Condition_Weather
  - Required Weather: Sunny
  - bInvert Condition: false

Action: Move to outdoor location

This entry only executes on sunny days!

## Example 3: Interruption for Player Interaction

### NPC Blueprint
**BP_NPC_Interactive**

Functions:
```blueprint
Function: OnPlayerInteract
  ? Get Schedule Component
  ? Interrupt Current Action (Can Resume: true)
  ? Start Dialogue
  ? Enable Dialogue UI

Function: OnDialogueEnd
  ? Get Schedule Component
  ? Resume Interrupted Action
```

Event Graph:
```blueprint
Event OnSchedule Action Interrupted (Action Handle)
  ? Print String ("NPC was interrupted")
  ? Stop Movement
  ? Look At Player

Event OnSchedule Action Started (Action Handle)
  ? Print String ("NPC started new action")
```

## Example 4: Dynamic Schedule Switching

### Quest System Integration
**BP_QuestManager**

Function: OnQuestComplete
```blueprint
Get All Actors of Class (BP_NPC_Quest Giver)
  ? ForEach
    ? Get Schedule Component
    ? Branch (Quest ID == "Quest_001")
? True:
        ? Set Schedule Data (DA_Schedule_After Quest)
    ? Cancel Current: true
```

This switches NPC behavior after quest completion.

## Example 5: Custom Action - Open Door

### Blueprint Action
**BP_Action_OpenDoor**
Parent: ScheduleActionBase

Variables:
- Door To Open (Actor Reference)
- Open Duration (Float): 3.0

Event Graph:
```blueprint
Event Execute Action Internal (Context)
  ? Get Executing Actor from Context
  ? Find Nearest Actor of Class (BP_Door)
  ? Set Door To Open
  ? Call "Open" on Door
  ? Delay (Open Duration)
  ? Call "Close" on Door
  ? Return Handle

Event Is Action Active Internal (Handle)
  ? Check if Door is Opening or Closing
  ? Return Boolean
```

### Using in Schedule
**DA_Entry_EnterBuilding**

Action: ScheduleAction_Composite
- [0] Move To Location (Tag: Location.Door)
- [1] BP_Action_OpenDoor
  - Open Duration: 3.0
- [2] Move To Location (Inside)

## Example 6: Location Selector - Random Patrol

### Blueprint Selector
**BP_Selector_PatrolPoints**
Parent: ScheduleLocationSelectorBase

Variables:
- Patrol Tag (GameplayTag): Location.PatrolPoint

Event Graph:
```blueprint
Event Select Location Internal (Context)
  ? Get All Actors With Tag (Patrol Tag)
  ? Random Array Element
  ? Create Schedule Location
? Location: Actor Location
 ? Reference Actor: Selected Actor
  ? Return (Success: true, Out Location)
```

### Patrol Schedule Entry
**DA_Entry_Patrol**

Location Selector: BP_Selector_PatrolPoints
  - Patrol Tag: Location.PatrolPoint

Action: ScheduleAction_Composite
  - Move To Location
  - Wait (30 seconds)
  - Play Animation (Look Around)

Recurrence: Custom (120 seconds)

This makes NPC patrol random points every 2 minutes.

## Example 7: Composite Sequential Actions

### Asset: DA_Entry_MorningRoutine
**Complex morning sequence**

Action: ScheduleAction_Composite
  Execute Sequentially: true
  Child Actions:
  ```
  [0] PlayAnimation (AM_WakeUp)
  [1] Wait (2 seconds)
  [2] MoveToLocation (Tag: Location.Bathroom)
  [3] PlayAnimation (AM_Washing)
  [4] Wait (5 seconds)
  [5] MoveToLocation (Tag: Location.Kitchen)
  [6] PlayAnimation (AM_Eating)
  [7] Wait (10 seconds)
  [8] MoveToLocation (Tag: Location.Door)
  [9] BP_Action_OpenDoor
  [10] MoveToLocation (Tag: Location.Workplace)
  ```

All execute in order, one after another!

## Example 8: Concurrent Actions

### Schedule Data Configuration
**DA_Schedule_Multitask**

Properties:
```
bAllow Concurrent Entries: true
Max Concurrent Entries: 3
```

### Entry 1: Walk
Priority: 50
Time: 08:00-17:00
Action: Move To Location (patrol)

### Entry 2: Talk
Priority: 51
Time: 08:00-17:00
Action: Play Animation (Talking)

### Entry 3: Gesture
Priority: 49
Time: 08:00-17:00
Action: Play Animation (Gesturing)

NPC will walk, talk, and gesture simultaneously!

## Example 9: Tag-Based Conditions

### Setup Gameplay Tags
Project Settings ? GameplayTags:
```
State.Working
State.Tired
State.Happy
Location.Indoor
Location.Outdoor
```

### Schedule Entry
**DA_Entry_RestIfTired**

Conditions:
- [0] ScheduleCondition_GameplayTag
  - Required Tags: State.Tired
  - Require All: false

Action: Play Animation (Resting)
Priority: 95 (high priority)

### Controlling Tags
In your NPC Blueprint:
```blueprint
Event OnWorkComplete
  ? Get Schedule Component
  ? Get Actor Schedule Tags
  ? Add Gameplay Tag (State.Tired)
  ? Set Actor Schedule Tags

// After rest
Event OnRestComplete
  ? Remove Gameplay Tag (State.Tired)
```

## Example 10: Road Following with NavAreas

### Setup Nav Area Costs

1. Create Blueprint Class: NavigationQueryFilter
   **BP_Filter_PreferRoads**

2. Set Area Costs:
```
NavArea_Road: 0.3
NavArea_Grass: 1.0
NavArea_Water: 100.0
```

### Movement Action Configuration
**DA_Action_WalkOnRoads**

Type: ScheduleAction_MoveToLocation
Properties:
```
Use Pathfinding: true
Filter Class: BP_Filter_PreferRoads
Acceptance Radius: 100
Speed Multiplier: 1.0
```

NPC will prefer roads when pathfinding!

## Common Blueprint Patterns

### Pattern: State Machine Integration
```blueprint
Event OnSchedule Action Started (Handle)
  ? Get Current Entry
  ? Get Entry Tags
  ? Branch (Has Tag: Action.Combat)
    ? True: Switch To Combat State
    ? False: Switch To Normal State
```

### Pattern: Cooldown System
```blueprint
Variable: LastActionTime (Float)
Variable: ActionCooldown (Float) = 60.0

Custom Condition: Check Cooldown
  ? Get Game Time
  ? Return (Current Time - Last Action Time > Action Cooldown)
```

### Pattern: Distance Check
```blueprint
Custom Condition: Near Location
  Variable: Required Distance (Float) = 500.0
  
  Evaluate:
    ? Get Actor Location
    ? Get Target Location from Custom Data
    ? Calculate Distance
  ? Return (Distance < Required Distance)
```

### Pattern: Time-of-Day Events
```blueprint
Function: Broadcast Time Event
  ? Get Schedule Subsystem
  ? Get All Actors With Schedule
  ? ForEach:
    ? Call Custom Event (OnTimeOfDay)
```

## Debugging Blueprint Schedules

### Add Debug Display
**BP_NPC** (any scheduled actor)

Event Tick (temporary, for debugging):
```blueprint
Event Tick
  ? Get Schedule Component
  ? Get Current Entry
  ? Branch (Is Valid)
    ? True:
      ? Get Display Text
      ? Draw Debug String
        Text: Entry Display Text
        Duration: 0.1
        Color: Green
```

### Log Schedule Changes
```blueprint
Event OnSchedule Action Started
  ? Print String ("Action Started: " + Action Handle)
  
Event OnSchedule Action Completed
  ? Print String ("Action Completed: " + Action Handle)
  
Event OnSchedule Action Interrupted
  ? Print String ("Action Interrupted: " + Action Handle)
```

## Performance Tips

1. **Avoid Per-Tick Conditions**
   - Don't check distances/states every tick
   - Use event-driven updates

2. **Cache Location Searches**
   - Store found locations in custom data
   - Reuse for repeated actions

3. **Limit Active Schedules**
   - Disable schedules for distant NPCs
   - Use distance-based LOD

4. **Batch Updates**
   - Update time per-minute, not per-second
   - Group nearby NPCs

## Next Steps

1. Study included example assets
2. Create your first schedule
3. Test with one NPC
4. Expand to full world
5. Profile and optimize

Happy Scheduling!
