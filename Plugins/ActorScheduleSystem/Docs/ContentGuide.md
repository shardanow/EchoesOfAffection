# Actor Schedule System - Content Structure

## Folder Organization

```
Plugins/ActorScheduleSystem/Content/
??? Blueprints/
?   ??? Actions/
?   ?   ??? BP_Action_Custom.uasset
?   ?   ??? Examples/
?   ??? Conditions/
?   ?   ??? BP_Condition_Custom.uasset
?   ?   ??? Examples/
?   ??? Selectors/
?   ?   ??? BP_Selector_Custom.uasset
?   ?   ??? Examples/
?   ??? Components/
?   ??? BP_TimeIntegration_Example.uasset
??? Data/
?   ??? Schedules/
?   ?   ??? DA_Schedule_Civilian.uasset
?   ?   ??? DA_Schedule_Guard.uasset
?   ?   ??? DA_Schedule_Merchant.uasset
?   ?   ??? DA_Schedule_Farmer.uasset
?   ??? Entries/
?       ??? DA_Entry_MorningWork.uasset
?       ??? DA_Entry_Lunch.uasset
?       ??? DA_Entry_EveningHome.uasset
?       ??? DA_Entry_Sleep.uasset
??? Examples/
?   ??? Maps/
?   ?   ??? MAP_ScheduleDemo.umap
?   ??? Characters/
?   ?   ??? BP_NPC_Civilian.uasset
?   ?   ??? BP_NPC_Guard.uasset
?   ?   ??? BP_NPC_Merchant.uasset
?   ??? Markers/
?    ??? BP_LocationMarker.uasset
?   ??? BP_POI_Base.uasset
??? UI/
    ??? WBP_ScheduleDebug.uasset
    ??? WBP_ScheduleViewer.uasset
```

## Asset Naming Conventions

### Data Assets
- **Schedules**: `DA_Schedule_<Type>`
  - Examples: DA_Schedule_Civilian, DA_Schedule_Guard
  
- **Entries**: `DA_Entry_<Activity>`
  - Examples: DA_Entry_MorningWork, DA_Entry_Patrol

### Blueprints
- **Actions**: `BP_Action_<ActionName>`
  - Examples: BP_Action_OpenDoor, BP_Action_Dialogue

- **Conditions**: `BP_Condition_<ConditionType>`
  - Examples: BP_Condition_Weather, BP_Condition_QuestState

- **Selectors**: `BP_Selector_<SelectorType>`
  - Examples: BP_Selector_PatrolPoints, BP_Selector_RandomNearby

- **Components**: `BP_<ComponentName>_<Variant>`
  - Examples: BP_TimeIntegration_GameMode

### Characters
- **NPCs**: `BP_NPC_<Role>`
  - Examples: BP_NPC_Civilian, BP_NPC_Merchant, BP_NPC_Guard

### Markers/POIs
- **Markers**: `BP_Marker_<Type>`
  - Examples: BP_Marker_Home, BP_Marker_Workplace
  
- **POIs**: `BP_POI_<Type>`
  - Examples: BP_POI_Shop, BP_POI_Restaurant

## Example Assets Description

### DA_Schedule_Civilian
Complete daily schedule for civilian NPCs:
- 06:00-08:00: Wake up, morning routine
- 08:00-12:00: Travel to work, work
- 12:00-13:00: Lunch break
- 13:00-17:00: Afternoon work
- 17:00-19:00: Return home, dinner
- 19:00-22:00: Evening activities
- 22:00-06:00: Sleep

### DA_Schedule_Guard
Patrol and guard schedule:
- 24-hour patrol shifts
- Regular waypoint visits
- Alert responses
- Shift changes

### BP_NPC_Civilian
Example NPC with:
- Schedule Component
- AI Controller
- Character Movement
- Visual customization
- Interaction support

### MAP_ScheduleDemo
Demo level showcasing:
- Multiple NPCs with schedules
- Location markers
- Time system integration
- Debug visualization
- Performance metrics

## Creating New Assets

### Quick Setup Checklist

#### 1. Schedule Data Asset
- [ ] Create DA_Schedule
- [ ] Set name and description
- [ ] Configure fallback action
- [ ] Set concurrent settings
- [ ] Add schedule tags

#### 2. Schedule Entries
- [ ] Create DA_Entry assets
- [ ] Configure time conditions
- [ ] Set priorities
- [ ] Assign actions
- [ ] Add conditions (if needed)
- [ ] Configure location selectors
- [ ] Set interruption behavior

#### 3. Character Setup
- [ ] Add Schedule Component
- [ ] Assign Schedule Data
- [ ] Configure AI Controller
- [ ] Setup tags
- [ ] Test with one entry

#### 4. World Setup
- [ ] Place location markers
- [ ] Assign gameplay tags
- [ ] Setup NavMesh
- [ ] Add time integration
- [ ] Test time updates

## Content Best Practices

### Asset Organization
1. **Group by Function** - Keep actions with actions, schedules with schedules
2. **Use Folders** - Separate examples from production content
3. **Version Control** - Name assets clearly for version control
4. **Documentation** - Add descriptions to all Data Assets

### Performance
1. **Asset Loading** - Use soft references for large assets
2. **Reuse Assets** - Share common entries between schedules
3. **Lazy Loading** - Don't load all schedules at once
4. **Memory** - Monitor loaded schedule data

### Workflow
1. **Prototype First** - Test with simple schedules
2. **Iterate Quickly** - Use Blueprint actions for prototyping
3. **Convert to C++** - Move final implementations to C++
4. **Document Examples** - Keep working examples updated

## Integration Points

### With Other Systems

#### Time System
- Add ScheduleTimeIntegrationComponent to GameState
- Call NotifyTimeChanged from time system
- Subscribe to time events

#### Quest System
- Use gameplay tags for quest conditions
- Switch schedules on quest completion
- Query NPCs by schedule tags

#### Dialogue System
- Interrupt schedules during dialogue
- Resume after dialogue ends
- Check schedule state for dialogue options

#### Save System
- Save current schedule data reference
- Save interrupted action state
- Save actor schedule tags

## Content Templates

### Template: Daily Routine
Use for: Civilians, workers, merchants

Entries:
1. Morning (06:00-08:00) - Wake, prepare
2. Commute (08:00-09:00) - Travel to work
3. Work Morning (09:00-12:00) - Work activity
4. Lunch (12:00-13:00) - Eat, rest
5. Work Afternoon (13:00-17:00) - Work activity
6. Commute Home (17:00-18:00) - Travel home
7. Evening (18:00-22:00) - Dinner, leisure
8. Sleep (22:00-06:00) - Rest

### Template: Guard Patrol
Use for: Guards, security

Entries:
1. Patrol Route A (00:00-08:00)
2. Shift Change (08:00-08:30)
3. Patrol Route B (08:30-16:00)
4. Shift Change (16:00-16:30)
5. Patrol Route C (16:30-24:00)

Fallback: Stand at post

### Template: Merchant
Use for: Shopkeepers, vendors

Entries:
1. Open Shop (08:00) - Unlock, prepare
2. Serve Customers (08:00-20:00) - Idle at counter
3. Close Shop (20:00) - Lock, clean
4. Go Home (20:00-21:00)
5. Rest (21:00-08:00)

### Template: Farmer
Use for: Agricultural workers

Entries (Season-dependent):
1. Morning chores (05:00-07:00)
2. Field work (07:00-12:00) - Condition: Good weather
3. Lunch (12:00-13:00)
4. Afternoon work (13:00-18:00)
5. Evening chores (18:00-19:00)
6. Dinner/Rest (19:00-21:00)
7. Sleep (21:00-05:00)

## Debug Content

### WBP_ScheduleDebug
Widget showing:
- Current time
- Active scheduled actors count
- Action states
- Performance metrics
- Event log

### WBP_ScheduleViewer
Widget for:
- Viewing actor schedules
- Current entry
- Next scheduled entry
- Manual control
- Testing interruptions

## Migration Guide

### From Other Systems

#### Behavior Tree ? Schedule System
1. Identify time-based behaviors
2. Convert to schedule entries
3. Replace BT tasks with actions
4. Remove per-tick logic
5. Test new schedule

#### State Machine ? Schedule System
1. Map states to schedule entries
2. Convert transitions to conditions
3. Use composite actions for sequences
4. Remove manual state management
5. Verify behavior

## Quality Assurance

### Testing Checklist
- [ ] All entries execute at correct times
- [ ] Transitions are smooth
- [ ] Interruptions work correctly
- [ ] Resume functionality works
- [ ] Performance is acceptable
- [ ] No memory leaks
- [ ] Concurrent actions work
- [ ] Pathfinding is correct
- [ ] Debug visualization works
- [ ] Save/load persists state

### Performance Targets
- Schedule evaluation: <1ms per 100 actors
- Action execution: <0.5ms per action
- Memory: <1MB per 100 scheduled actors
- Update frequency: 1-60 seconds (configurable)

## Support Files

See also:
- README.md - Main documentation
- BlueprintExamples.md - Blueprint usage
- API_Reference.md - C++ API details
