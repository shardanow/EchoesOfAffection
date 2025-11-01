# Quest System - Content Examples

This folder will contain example quest assets and UI widgets.

## Directory Structure

```
Content/
?? Quests/
?  ?? Tutorial/
?  ?  ?? DA_Quest_FirstSteps.uasset
?  ?  ?? DA_Quest_TalkToNPC.uasset
?  ?? Main/
?  ?  ?? DA_Quest_MainStory_01.uasset
??? Side/
?     ?? DA_Quest_WarmBread.uasset
?     ?? DA_Quest_SecretOak.uasset
?? UI/
?  ?? WBP_QuestJournal.uasset
?  ?? WBP_QuestTracker.uasset
?  ?? WBP_QuestToast.uasset
?  ?? WBP_ObjectiveEntry.uasset
?? Examples/
?  ?? BP_QuestTestActor.uasset
?  ?? MAP_QuestSystemDemo.umap
?? README.md (this file)
```

## Creating Example Assets

To create example quest assets:

1. **Create Quest Asset**
 - Content Browser ? Right Click
   - Miscellaneous ? Data Asset
   - Select **Quest Asset**
   - Name: `DA_Quest_[Name]`

2. **Configure Quest**
   - Follow examples in `/Docs/Examples.md`
   - Use validation button to check for errors

3. **Create UI Widget**
   - Content Browser ? Right Click
   - User Interface ? Widget Blueprint
   - Parent Class: **QuestJournalWidget**
   - Name: `WBP_[Name]`

## Example Quests Included

### Tutorial Quests

**DA_Quest_FirstSteps**
- Simple introduction quest
- Single objective: Talk to NPC
- Teaches basic quest mechanics

**DA_Quest_TalkToNPC**
- Multi-step tutorial
- Teaches: Talk, Collect, Deliver
- Rewards: Currency + Tutorial Tag

### Side Quests

**DA_Quest_WarmBread**
- Collection and delivery quest
- Time-gated (06:00-10:00)
- Multiple phases
- See: `/Docs/Examples.md` for full configuration

**DA_Quest_SecretOak**
- Branching quest with choices
- Time and weather requirements
- Moral decision with consequences
- See: `/Docs/Examples.md` for full configuration

## Using Examples

### 1. Load Example Map

```
Open: Content/QuestSystemContent/Examples/MAP_QuestSystemDemo
Play in Editor
```

### 2. Test Quest Flow

```
1. Press 'Q' to open quest journal
2. Press 'E' to interact with NPCs
3. Press 'Tab' to see quest tracker
4. Press 'F' to pick up items
```

### 3. Inspect Assets

Open any `DA_Quest_*` asset to see:
- Quest structure
- Objective configuration
- Condition setup
- Reward definition
- Validation results

## Blueprint Examples

### BP_QuestTestActor

Test actor with debug functions:
- Start any quest by name
- Emit test events
- Complete objectives
- View quest state

**Usage:**
1. Place in level
2. Call functions from Blueprint or Details panel
3. Check Output Log for results

### Example Functions

```blueprint
// Start quest
Call Function: Start Test Quest
  Quest ID: Q_WarmBreadForElyn

// Emit event
Call Function: Emit Test Event
  Event Type: Item Acquired
  Item ID: Wheat
  Count: 1

// Check state
Call Function: Print Quest State
  Quest ID: Q_WarmBreadForElyn
```

## UI Widget Examples

### WBP_QuestJournal

Full quest journal with:
- Quest list (active/completed filters)
- Quest details panel
- Objective list with progress
- Abandon/Track buttons

**To use:**
1. Add to player HUD
2. Toggle with input action
3. Automatically updates via events

### WBP_QuestTracker

On-screen objective tracker:
- Shows current objective
- Progress bar
- Updates in real-time

**To use:**
1. Add to HUD overlay
2. Component automatically tracks active quest
3. Hides when no quest active

### WBP_QuestToast

Notification popup:
- Quest started
- Objective updated
- Quest completed
- Auto-dismiss after delay

**To use:**
1. Listen to quest subsystem events
2. Create widget and add to viewport
3. Widget handles animation and removal

## Creating Your Own

### Custom Quest

1. Duplicate `DA_Quest_Template`
2. Rename to your quest name
3. Configure:
   - Quest ID (unique!)
   - Title and description
   - Phases and objectives
   - Rewards
4. Validate
5. Test in level

### Custom UI

1. Create Widget Blueprint
2. Parent Class: `QuestJournalWidget`
3. Override events:
   - `OnQuestListUpdated`
   - `OnQuestSelected`
   - `OnQuestProgressChanged`
4. Build UI layout
5. Bind to component events

## Integration Example

```cpp
// In your game mode or player controller

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Get quest subsystem
    UQuestSubsystem* QuestSystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    
    // Subscribe to events
    QuestSystem->OnQuestStarted.AddDynamic(this, &AMyGameMode::OnQuestStarted);
    
    // Auto-start tutorial
    QuestSystem->StartQuest(FName("Q_FirstSteps"));
}

void AMyGameMode::OnQuestStarted(FName QuestId, UQuestAsset* QuestAsset)
{
// Show quest started notification
    ShowQuestToast(QuestAsset->Title);
}
```

## Tips

1. **Always validate** quests before testing
2. **Use unique IDs** for all quests/phases/objectives
3. **Test save/load** with your quests
4. **Check Output Log** for quest events
5. **Use quest tracker** to debug conditions

## Troubleshooting

**Quest doesn't start?**
- Check dependencies
- Verify quest ID matches
- Look for validation errors

**Objective doesn't update?**
- Check event is being emitted (log)
- Verify condition parameters match
- Check gates are satisfied

**UI doesn't refresh?**
- Ensure widget subscribed to events
- Check quest log component is active
- Verify event delegates are bound

## Next Steps

1. Read [Quick Start Guide](../Docs/QuickStart.md)
2. Study [Examples](../Docs/Examples.md)
3. Review [Best Practices](../Docs/BestPractices.md)
4. Build your own quests!

---

**Note:** This folder contains Blueprint-based examples. For C++ integration examples, see `/Docs/Examples.md`
