# Actor Schedule System - Quick Reference

## ?? 5-Minute Setup

### 1. Enable Plugin ?
Project Settings ? Plugins ? Search "Actor Schedule" ? Enable ? Restart Editor

### 2. Create First Schedule (2 min)
1. Content Browser ? Right-Click ? Data Asset ? `ScheduleData`
2. Name: `DA_MyFirstSchedule`
3. Add Entry:
   - Right-Click ? Data Asset ? `ScheduleEntryData`
   - Name: `DA_Entry_Morning`
   - Time: 08:00-12:00
   - Action: Create `MoveToLocation` action
4. Add entry to schedule's `Entries` array

### 3. Add to Actor (1 min)
1. Select your NPC
2. Add Component ? `Schedule Component`
3. Set `Schedule Data` ? `DA_MyFirstSchedule`
4. Done!

### 4. Time Integration (2 min)
1. Open GameState or GameMode Blueprint
2. Add Component ? `Schedule Time Integration Component`
3. Call `Notify Time Changed` from your time system
4. Test!

---

## ?? Common Tasks

### Create Morning Routine
```
Entry: "Morning Commute"
Time: 08:00 (TimeOfDay)
Action: Composite
  ? Move To Location (Work)
  ? Play Animation (Working)
  ? Wait (4 hours)
Priority: 80
```

### Make NPC Patrol
```
Entry: "Patrol"
Time: 00:00-24:00 (TimeRange)
Location: ActorTag Selector
  Tags: Location.PatrolPoint
  Mode: Random
Action: Move To Location
Recurrence: Custom (120 seconds)
```

### Interrupt for Dialogue
```cpp
// C++
ScheduleComponent->InterruptCurrentAction(true);
// ... dialogue ...
ScheduleComponent->ResumeInterruptedAction();
```

```blueprint
// Blueprint
Get Schedule Component ? Interrupt Current Action (Can Resume: true)
(After dialogue) ? Resume Interrupted Action
```

### Check Current Activity
```cpp
// C++
if (UScheduleEntryData* Entry = ScheduleComp->GetCurrentEntry())
{
    FText ActivityName = Entry->GetDisplayText();
}
```

```blueprint
// Blueprint
Get Schedule Component ? Get Current Entry ? Get Display Text
```

---

## ?? Quick Tips

### ? Performance
- ? Use per-minute time updates, not per-frame
- ? Enable distance culling (5000 units recommended)
- ? Limit concurrent actions (1-3 per NPC)
- ? Don't tick in conditions/actions
- ? Don't search all actors every frame

### ?? Design
- ? Small, focused entries (one action = one entry)
- ? Use priorities (higher = more important)
- ? Always provide fallback action
- ? Use gameplay tags for flexible conditions
- ? Don't create mega-entries with all logic
- ? Don't hardcode locations in actions

### ?? Debugging
- Enable debug visualization in Project Settings
- Check `LogTemp` for schedule messages
- Use `GetScheduleStats` for metrics
- Add `OnActionStarted` event for logging

---

## ?? Asset Types

| Type | Purpose | Example |
|------|---------|---------|
| `ScheduleData` | Complete daily routine | DA_Civilian_Schedule |
| `ScheduleEntryData` | Single time slot + action | DA_Entry_MorningWork |
| `ScheduleActionBase` | What to do | Walk, Talk, Work |
| `ScheduleConditionBase` | When to do it | Time, Weather, Tags |
| `ScheduleLocationSelectorBase` | Where to do it | Find shop, patrol point |

---

## ?? Built-in Actions

| Action | Use Case | Key Properties |
|--------|----------|----------------|
| **MoveToLocation** | Walk to place | Acceptance Radius, Filter Class |
| **PlayAnimation** | Play montage | Montage, Play Rate, Looping |
| **Wait** | Pause/delay | Duration, Use Game Time |
| **Composite** | Chain actions | Child Actions, Sequential |

---

## ?? Blueprint Nodes

### Schedule Component

#### Properties
- `Schedule Data` - The schedule asset
- `bSchedule Enabled` - Toggle on/off
- `Priority Offset` - Adjust priorities
- `Actor Schedule Tags` - Tags for conditions

#### Functions
- `Interrupt Current Action` - Stop current
- `Resume Interrupted Action` - Continue
- `Cancel Current Action` - Stop completely
- `Pause Schedule` - Pause evaluation
- `Resume Schedule` - Resume evaluation
- `Has Active Action` - Check if running
- `Get Current Entry` - Get active entry
- `Set Schedule Data` - Change schedule

#### Events
- `On Action Started` - Action begins
- `On Action Completed` - Action finishes
- `On Action Interrupted` - Action stopped

### Schedule Subsystem

#### Functions
- `Notify Time Changed` - Trigger evaluation
- `Refresh All Schedules` - Force update
- `Get Actors With Schedule Tags` - Query
- `Pause All Schedules` - Global pause
- `Resume All Schedules` - Global resume
- `Get Schedule Stats` - Metrics

#### Events
- `On Schedule Event` - Global events

---

## ??? Custom Components

### Custom Action (Minimal)
```cpp
UCLASS()
class UMyAction : public UScheduleActionBase
{
    GENERATED_BODY()
protected:
    virtual FScheduleActionHandle ExecuteActionInternal_Implementation(
    const FScheduleExecutionContext& Context) override
{
     // Your logic here
        return Super::ExecuteActionInternal_Implementation(Context);
}
};
```

### Custom Condition (Minimal)
```cpp
UCLASS()
class UMyCondition : public UScheduleConditionBase
{
    GENERATED_BODY()
protected:
 virtual bool EvaluateConditionInternal_Implementation(
        const FScheduleEvaluationContext& Context) const override
    {
        // Your check here
      return true;
 }
};
```

### Custom Selector (Minimal)
```cpp
UCLASS()
class UMySelector : public UScheduleLocationSelectorBase
{
    GENERATED_BODY()
protected:
    virtual bool SelectLocationInternal_Implementation(
        const FScheduleLocationContext& Context,
      FScheduleLocation& OutLocation) override
    {
    // Your selection logic
        OutLocation.Location = FVector::ZeroVector;
        return true;
    }
};
```

---

## ?? Learning Path

### Beginner (Day 1)
1. ? Setup plugin
2. ? Create simple schedule (1-2 entries)
3. ? Test with one NPC
4. ? Add time integration

### Intermediate (Day 2-3)
1. ? Multiple entries with priorities
2. ? Use conditions (tags, time ranges)
3. ? Implement interruptions
4. ? Create composite actions

### Advanced (Week 1)
1. ? Custom actions/conditions
2. ? Complex location selectors
3. ? Performance optimization
4. ? Quest/dialogue integration

### Expert (Ongoing)
1. ? Weather-dependent schedules
2. ? Dynamic schedule switching
3. ? Concurrent action management
4. ? Production deployment

---

## ?? Troubleshooting

### "Actions not executing"
- ? Check `bScheduleEnabled` is true
- ? Verify time integration is calling `NotifyTimeChanged`
- ? Check entry conditions are met
- ? Verify action is assigned
- ? Check priority conflicts

### "NPC won't move"
- ? Ensure NPC has AIController
- ? Check NavMesh exists
- ? Verify target is reachable
- ? Check acceptance radius

### "Time not updating"
- ? ScheduleTimeIntegrationComponent added?
- ? Time system calling notify?
- ? Subsystem initialized?

### "Poor performance"
- ? Enable distance culling
- ? Reduce time update frequency
- ? Limit concurrent schedules
- ? Profile with Unreal Insights

---

## ?? Support

### Documentation
- `README.md` - Complete documentation
- `BlueprintExamples.md` - Blueprint patterns
- `ContentGuide.md` - Asset organization

### Community
- Check plugin folder for examples
- Review example map
- Study included assets

### Reporting Issues
1. Check troubleshooting section
2. Enable debug visualization
3. Check logs (`LogTemp`)
4. Note UE version and plugin version

---

## ?? Success Checklist

- [ ] Plugin enabled and compiling
- [ ] First schedule created
- [ ] NPC following schedule
- [ ] Time integration working
- [ ] Interruptions functional
- [ ] Performance acceptable
- [ ] No errors in log

**Congratulations! You're ready to build living, breathing game worlds!**

---

## ?? Next Steps

1. **Study Examples** - Review included demo map
2. **Expand Gradually** - Add more NPCs one at a time
3. **Optimize Early** - Profile with few NPCs first
4. **Document Your Work** - Note what works for your project
5. **Share Knowledge** - Help others learn the system

**Happy Scheduling! ??**
