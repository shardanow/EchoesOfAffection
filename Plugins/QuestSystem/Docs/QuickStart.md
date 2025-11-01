# Quest System - Quick Start Guide

Get your first quest running in **10 steps**!

## Prerequisites

- Unreal Engine 5.4+
- Basic understanding of Blueprints or C++
- Quest System plugin installed

## Step 0: Configure Asset Manager (IMPORTANT!)

**Before creating quests, configure Asset Manager:**

1. **Edit → Project Settings → Game → Asset Manager**
2. **Primary Asset Types to Scan → Add (+)**
3. Configure:

```
Primary Asset Type: Quest
Asset Base Class: /Script/QuestSystemRuntime.QuestAsset
Has Blueprint Classes: false

Directories:
  [0]: Path: /Game/Quests

Rules:
  Cook Rule: Always Cook
```
4. **Click "Apply" and "Restart Editor"**

**Alternative**: See [Asset Manager Setup Guide](AssetManagerSetup.md) for detailed instructions.

---

## Step 1: Enable Gameplay Tags

1. **Edit → Project Settings → Gameplay Tags**
2. Add a new Gameplay Tag source: `Config/Tags/QuestTags.ini`
3. Click **Add New Gameplay Tag**
4. Create these tags:

```
Quest.Event
Quest.Event.Item.Acquired
Quest.Event.NPC.Talked
Quest.Event.Area.Enter
Quest.Category.Tutorial
Quest.Category.Main
Quest.Category.Side
```

## Step 2: Create Quest Asset

1. **Content Browser → Right Click → Miscellaneous → Data Asset**
2. Select **Quest Asset**
3. Name: `DA_Quest_FirstQuest`
4. Double-click to open

## Step 3: Configure Quest Basics

In the Quest Asset:

```
Quest:
  Quest ID: Q_Tutorial_First
  Title: "My First Quest"
  Description: "Learn how the quest system works"
  
Meta:
  Categories: [Quest.Category.Tutorial]
  Difficulty Level: 1
  Is Repeatable: false
```

## Step 4: Add First Phase

Scroll to **Phases** array, click **+**:

```
Phase [0]:
  Phase ID: Phase_Start
  Title: "Starting Phase"
  Require All Objectives: true
```

## Step 5: Add First Objective

In **Phase [0] → Objectives**, click **+**:

```
Objective [0]:
  Objective ID: Obj_TalkToNPC
  Title: "Talk to the Quest Giver"
  Style: Talk
  Logic: And
  Track On HUD: true
  
Counters:
Target Count: 1
```

## Step 6: Add Objective Condition

In **Objective [0] → Conditions**, click **+**:

```
Condition [0]:
  Event Type: On NPC Talked
  Event Tag: Quest.Event.NPC.Talked
  NPC ID: QuestGiver
  Count: 1
```

## Step 7: Configure Start Policy

Back at Quest level:

```
Start Policy:
  Policy Type: Manual
```

## Step 8: Add Rewards

In **Rewards On Complete → Rewards**, click **+**:

```
Reward [0]:
  Reward Type: Grant Currency
  Currency Amount: 100
```

## Step 9: Add Quest Component to Player

1. Open your **Player Controller** or **Player State** Blueprint
2. **Components Panel → Add Component → Quest Log Component**
3. In **Event Begin Play**:

```
Event Begin Play
  → Get Component Quest Log
  → Subscribe To Quest Events
```

## Step 9a: Create NPC Quest Giver (Optional but Recommended)

### Create Simple NPC:

1. **Content Browser → Blueprint Class → Actor**
2. Name: `BP_QuestGiver_Test`
3. Add components:
   - **Static Mesh** or **Skeletal Mesh** (NPC visual)
   - **Quest Giver Component** (from Quest System)
   - **Box Collision** (for interaction trigger)

### Configure Quest Giver Component:

```
Quest Giver Component:
  NPC ID: QuestGiver (matches condition in quest!)
  Auto Assign NPC ID: false
  Available Quests:
    [0]: DA_Quest_FirstQuest
```

### Add Interaction:

```blueprint
Event OnComponentBeginOverlap (Box Collision):
  → Show "Press E to Talk" UI

Event OnPlayerPressE:
  → Quest Giver Component → On Player Interact
  → Quest Giver Component → Get Available Quests For Player
  → For Each Available Quest:
      → Quest Giver Component → Accept Quest
      → Break (accept first quest for simplicity)
```

## Step 10: Test the Quest

### Create Test Level Blueprint:

**Event Begin Play:**
```
Event Begin Play
  → Delay (2.0 seconds)
  → Start Quest (Quest ID: Q_Tutorial_First)
```

**Event Any Key:**
```
Event Any Key (E)
  → Emit NPC Talked (NPC ID: QuestGiver, Player: Get Player Character)
```

### Run the Game:

1. Press **Play**
2. Quest starts automatically after 2 seconds
3. Press **E** key
4. Quest completes!
5. Check **Output Log** for quest events

---

## What Just Happened?

1. ✅ Quest started automatically
2. ✅ Event was emitted (NPC Talked)
3. ✅ Objective condition matched
4. ✅ Objective completed
5. ✅ Quest completed
6. ✅ Reward granted
7. ✅ Progress saved

---

## Next Steps

### Add UI

Create a simple HUD widget:

**Blueprint (WBP_QuestHUD):**

```
Canvas Panel
  └─ Text Block (QuestTitle)
  └─ Text Block (ObjectiveText)
  └─ Progress Bar (QuestProgress)
```

**Graph:**

```
Event Construct
  → Get Quest Log Component
  → Set Tracked Quest (Quest ID: Q_Tutorial_First)

Event Tick
  → Get Quest Log Component
  → Get Current Objective Text (Tracked Quest)
  → Set Text (ObjectiveText)
```

### Add More Objectives

Add a second objective - collect an item:

**Objective [1]:**
```
Objective ID: Obj_CollectApple
Title: "Collect 3 Apples"
Style: Collect

Counters:
  Target Count: 3

Conditions:
  Event Type: On Item Acquired
  Item ID: Apple
  Count: 1
```

**Test:**
```
Event Any Key (F)
  → Emit Item Acquired (Item ID: Apple, Count: 1, Instigator: Self)
```

### Make Quest Auto-Start

Change **Start Policy**:

```
Start Policy:
  Policy Type: Auto On Condition
  Auto Start Condition:
    Event Type: On Enter Area
    Area ID: Village
```

### Add Time Limit

Configure **Failure Policy**:

```
Failure Policy:
Failure Type: Time Limit
  Time Limit: 300.0 (5 minutes)
  Allow Retry: true
```

---

## Common Blueprint Nodes

### Start Quest
```
Start Quest
  Quest ID: Q_Tutorial_First
  Return Value: Boolean (success/fail)
```

### Check Quest State
```
Get Quest State
  Quest ID: Q_Tutorial_First
  Return Value: Quest State (enum)

Is Quest Active
  Quest ID: Q_Tutorial_First
  Return Value: Boolean
```

### Emit Events
```
Emit Item Acquired
  Item ID: Apple
  Count: 1
  Instigator: Self
  Tags: (none)

Emit NPC Talked
  NPC ID: Blacksmith
  Player: Self
  Tags: (none)

Emit Custom Event
  Event Tag: Quest.Event.Custom.MyEvent
  String Param: CustomData
  Int Param: 42
  Float Param: 0.0
```

### Get Objective Progress
```
Get Objective Progress
  Quest ID: Q_Tutorial_First
  Objective ID: Obj_CollectApple
  
Outputs:
  Current Count: Integer
  Target Count: Integer
  Is Completed: Boolean
```

### Update Objective
```
Update Objective Progress
  Quest ID: Q_Tutorial_First
  Objective ID: Obj_CollectApple
  Amount: 1
  Return Value: Boolean (success/fail)

Complete Objective
  Quest ID: Q_Tutorial_First
  Objective ID: Obj_CollectApple
  Return Value: Boolean
```

---

## Debugging

### Enable Quest Logs

**Project Settings → Engine → General → Log:**

Add filter: `LogTemp` with verbosity `Log` or `Verbose`

### Common Log Messages

```
QuestSubsystem: Started quest 'Q_Tutorial_First'
QuestSubsystem: Completed objective 'Obj_TalkToNPC' in quest 'Q_Tutorial_First'
QuestSubsystem: Completed quest 'Q_Tutorial_First'
QuestSubsystem: Saved quest progress
```

### Check Quest State in Console

```
# Print all active quests
stat game

# Force complete quest (cheat)
CompleteQuest Q_Tutorial_First

# Reset quest progress
ResetQuestProgress
```

---

## Validation

Before testing, validate your quest:

1. Open Quest Asset
2. Click **Quest Tools → Validate Quest Data**
3. Fix any errors shown

---

## Troubleshooting

### Quest Doesn't Start

**Problem:** `StartQuest` returns false

**Solutions:**
- Check dependencies are met
- Verify Quest ID matches exactly
- Check if quest is already active/completed
- Look for errors in Output Log

### Objective Doesn't Update

**Problem:** Event emitted but objective not updating

**Solutions:**
- Verify event parameters match condition (NPC ID, Item ID, etc)
- Check Event Tag matches
- Verify objective is not already complete
- Check gates (time, weather, location)
- Enable verbose logging

### Quest Not Saving

**Problem:** Progress lost on game restart

**Solutions:**
- Check save slot name
- Verify save directory permissions
- Call `SaveQuestProgress` manually
- Check for save errors in log

### UI Not Updating

**Problem:** Quest HUD shows old data

**Solutions:**
- Subscribe to quest events in widget
- Refresh UI on objective updated event
- Check Quest Log Component is valid
- Verify tracked quest is set

---

## Video Tutorial

[Coming Soon] Watch the video guide: **Creating Your First Quest in 5 Minutes**

---

## Next Reading

- **[Complete Examples](Examples.md)** - Full quest configurations
- **[API Reference](API_Reference.md)** - All functions and classes
- **[Integration Guide](Integration.md)** - Connect to other systems
- **[Best Practices](Best_Practices.md)** - Tips and patterns

---

## Support

Need help?

- 📖 Check the [README](../README.md)
- 🐛 Report issues on GitHub
- 💬 Ask in Discord
- 📧 Email support

---

**Congratulations!** You've created your first working quest. Now go build something amazing! 🎮
