# Quest System - Cheat Sheet

Quick reference for common operations.

---

## Blueprint Nodes

### Start/Stop Quests

```
Start Quest
?? Quest ID: FName
?? Return: Boolean

Complete Quest
?? Quest ID: FName
?? Return: Boolean

Fail Quest
?? Quest ID: FName
?? Return: Boolean

Abandon Quest
?? Quest ID: FName
?? Return: Boolean
```

### Quest Queries

```
Get Quest State
?? Quest ID: FName
?? Return: EQuestState

Is Quest Active
?? Quest ID: FName
?? Return: Boolean

Get Active Quests
?? Return: Array<FQuestSaveData>
```

### Objective Management

```
Update Objective Progress
?? Quest ID: FName
?? Objective ID: FName
?? Amount: Integer (default 1)
?? Return: Boolean

Get Objective Progress
?? Quest ID: FName
?? Objective ID: FName
?? Return: Current, Target, IsCompleted
```

### Emit Events

```
Emit Item Acquired
?? Item ID: FName
?? Count: Integer
?? Instigator: Actor
?? Tags: GameplayTagContainer

Emit NPC Talked
?? NPC ID: FName
?? Player: Actor
?? Tags: GameplayTagContainer

Emit Dialog Choice
?? Dialog ID: FName
?? Choice ID: FName
?? Tags: GameplayTagContainer

Emit Enter Area
?? Area ID: FName
?? Actor: Actor
?? Tags: GameplayTagContainer

Emit Custom Event
?? Event Tag: GameplayTag
?? String Param: FName
?? Int Param: Integer
?? Float Param: Float
```

---

## C++ Quick Reference

### Get Subsystem

```cpp
UQuestSubsystem* QuestSystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
```

### Start Quest

```cpp
bool bStarted = QuestSystem->StartQuest(FName("Q_MyQuest"));
```

### Emit Event

```cpp
UQuestBlueprintLibrary::EmitItemAcquired(
    this, 
    FName("Apple"), 
    1, 
    PlayerActor, 
    FGameplayTagContainer()
);
```

### Subscribe to Delegate

```cpp
QuestSystem->OnQuestCompleted.AddDynamic(this, &UMyClass::OnQuestCompleted);

void UMyClass::OnQuestCompleted(FName QuestId, UQuestAsset* QuestAsset)
{
    UE_LOG(LogTemp, Log, TEXT("Quest completed: %s"), *QuestId.ToString());
}
```

---

## Quest Asset Structure

```
QuestAsset
?? Quest ID (FName)
?? Title (FText)
?? Description (FText)
?? Meta
?  ?? Categories (Tags)
?  ?? Difficulty (1-5)
?  ?? Is Repeatable (Bool)
?  ?? Is Main Quest (Bool)
?? Phases []
?  ?? Phase ID (FName)
?  ?? Title (FText)
?  ?? Objectives []
?  ?  ?? Objective ID (FName)
?  ??? Title (FText)
?  ?  ?? Style (Enum)
?  ?  ?? Logic (AND/OR/Count/Sequence)
?  ?  ?? Conditions []
?  ?  ?  ?? Event Type (Enum)
?  ?  ?  ?? Event Tag (GameplayTag)
?  ?  ?  ?? Parameters (ItemId, NpcId, etc)
?  ?  ?? Counters
?  ?  ?  ?? Target Count (Int)
?  ?  ?  ?? Current Count (Int)
?  ?  ?? Gates []
?  ?     ?? Time of Day
?  ?     ?? Weather
?  ?     ?? Location
?  ?? Transition
?     ?? Next Phase ID (FName)
?? Dependencies []
?  ?? Required Quest (FName)
?  ?? Required State (Enum)
?  ?? Required Tags (Tags)
?  ?? Required Level (Int)
?? Start Policy
?  ?? Type (Enum)
?  ?? Condition (varies)
?? Failure Policy
?  ?? Type (None/Time/Condition/Event)
?  ?? Time Limit (Float)
?  ?? Allow Retry (Bool)
?? Rewards []
   ?? Type (Enum)
   ?? Amount (Int)
   ?? Parameters (varies)
```

---

## Event Tags

```
Quest.Event
?? Quest.Event.Item
?  ?? Quest.Event.Item.Acquired
?  ?? Quest.Event.Item.Used
?  ?? Quest.Event.Item.Crafted
?  ?? Quest.Event.Item.Delivered
?? Quest.Event.NPC
?  ?? Quest.Event.NPC.Talked
?  ?? Quest.Event.NPC.Killed
?? Quest.Event.Dialog
?  ?? Quest.Event.Dialog.Choice
?  ?? Quest.Event.Dialog.NodeReached
?? Quest.Event.Area
?  ?? Quest.Event.Area.Enter
?  ?? Quest.Event.Area.Leave
?? Quest.Event.Combat
?  ?? Quest.Event.Actor.Killed
?? Quest.Event.Time
?  ?? Quest.Event.Time.Reached
?  ?? Quest.Event.Day.Changed
?? Quest.Event.Weather
?  ?? Quest.Event.Weather.Changed
?? Quest.Event.Custom
```

---

## Common Patterns

### Simple Talk Quest

```
Objective:
  Style: Talk
  Condition:
    Event Type: OnNpcTalked
    NPC ID: Blacksmith
    Count: 1
  Counters:
    Target Count: 1
```

### Collection Quest

```
Objective:
  Style: Collect
  Condition:
    Event Type: OnItemAcquired
    Item ID: Apple
    Count: 1
  Counters:
    Target Count: 5
```

### Kill Quest

```
Objective:
  Style: Kill
  Condition:
    Event Type: OnActorKilled
    Actor Class: BP_Bandit
    Required Tags: [Enemy.Bandit]
    Count: 1
  Counters:
    Target Count: 10
```

### Timed Quest

```
Failure Policy:
  Type: TimeLimit
  Time Limit: 300.0  (5 minutes)
  Allow Retry: true
```

### Location Quest

```
Objective:
  Style: Visit
  Condition:
    Event Type: OnEnterArea
    Area ID: Village
    Count: 1
  Gates:
    Require Time: true
    Time Start: 6
    Time End: 18
```

---

## Validation

### In Editor

1. Open Quest Asset
2. Click "Validate Quest Data" button
3. Fix errors shown

### Common Errors

- ? Empty Quest ID
- ? Empty Phase ID
- ? Empty Objective ID
- ? Duplicate IDs
- ? No objectives in phase
- ? No conditions in objective
- ? Invalid phase transitions
- ? Circular dependencies

---

## Debug Commands

```cpp
// Start quest
StartQuest Q_MyQuest

// Complete quest
CompleteQuest Q_MyQuest

// Fail quest
FailQuest Q_MyQuest

// List active
ListActiveQuests

// Reset progress
ResetQuestProgress
```

---

## Performance Tips

- ? Emit events discretely (on actions)
- ? Don't emit in Tick
- ? Use gates for restrictions
- ? Don't poll conditions
- ? Load assets async
- ? Don't load all at once
- ? Cache quest references
- ? Don't lookup every frame

---

## Keyboard Shortcuts

**In Quest Asset Editor:**

- `Ctrl+S` - Save
- `F5` - Validate Data
- `Ctrl+D` - Duplicate Phase/Objective
- `Del` - Delete Phase/Objective

---

## File Locations

```
Plugins/QuestSystem/
?? Source/
?  ?? QuestSystemRuntime/
?  ?  ?? Public/
?  ?  ?? Private/
?  ?? QuestSystemEditor/
?     ?? Public/
?     ?? Private/
?? Config/
?  ?? Tags/
?     ?? QuestSystem.ini
?? Content/
?  ?? Examples/
?? Docs/
?  ?? README.md
?  ?? QuickStart.md
?  ?? Examples.md
?  ?? API_Reference.md
?  ?? BestPractices.md
?? Resources/
```

---

## Support Resources

?? [Full Documentation](../README.md)  
?? [Quick Start Guide](QuickStart.md)  
?? [Complete Examples](Examples.md)  
?? [API Reference](API_Reference.md)  
? [Best Practices](BestPractices.md)

---

**Print this page for quick reference while developing! ??**
