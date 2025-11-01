# Quest System Plugin for Unreal Engine 5

AAA-grade modular, data-driven, event-based quest system with full Blueprint support.

## Features

- **Data-Driven**: All quest configuration through Data Assets
- **Event-Based**: No per-actor ticks, only event subscriptions
- **Modular**: Easy to extend with new objective types and rewards
- **Blueprint-Friendly**: Full Blueprint API with custom nodes
- **Save/Load**: Complete quest progress serialization
- **Production-Ready**: Validated, optimized, and documented

## ?? Quick Start

**?? IMPORTANT: [Configure Asset Manager first!](ASSET_MANAGER_REQUIRED.md)**

1. **Configure Asset Manager** (Edit ? Project Settings ? Asset Manager)
   - Add Primary Asset Type: `Quest`
   - Set directories to `/Game/Quests`
   - Restart Editor

2. **Create Quest Folder**
   ```
   Content ? New Folder ? "Quests"
```

3. **Create Quest Asset**
   ```
 Content/Quests ? Right Click ? Data Asset ? Quest Asset
   ```

4. **Configure Quest**
   ```
   Quest ID: Q_MyFirstQuest
   Add Phase ? Add Objective ? Add Condition
   ```

5. **Start Quest in Blueprint**
   ```blueprint
   Event Begin Play:
     ? Get Quest Subsystem
     ? Start Quest (Q_MyFirstQuest)
   ```

**Full Guide**: [Quick Start Guide](Docs/QuickStart.md)

---

## Installation

1. Copy the `QuestSystem` folder to your project's `Plugins` directory
2. Right-click your `.uproject` file and select "Generate Visual Studio project files"
3. Open your project in Unreal Engine
4. The plugin will be automatically enabled

## Create Your First Quest

1. In Content Browser, right-click ? **Miscellaneous** ? **Data Asset** ? **Quest Asset**
2. Name it `DA_Quest_Tutorial`
3. Open the asset and configure:

```
Quest ID: Q_Tutorial_First
Title: "My First Quest"
Description: "Complete your first objective"

Phases:
  - Phase ID: Phase_Start
    Objectives:
      - Objective ID: Obj_Talk
      Title: "Talk to the NPC"
        Style: Talk
        Conditions:
          - Event Type: On NPC Talked
    NPC ID: NPC_Tutorial
       Count: 1

Start Policy: Manual
```

## Add Quest Component to Player

1. Open your Player Controller or Player State Blueprint
2. Add Component ? **Quest Log Component**
3. In BeginPlay, call **Subscribe To Quest Events**

## Start the Quest

In your code or Blueprint:

```cpp
// C++
#include "QuestBlueprintLibrary.h"

UQuestBlueprintLibrary::StartQuest(GetWorld(), FName("Q_Tutorial_First"));
```

```blueprint
// Blueprint
Start Quest (Quest ID: Q_Tutorial_First)
```

## Emit Events

When player talks to NPC:

```cpp
// C++
UQuestBlueprintLibrary::EmitNpcTalked(GetWorld(), FName("NPC_Tutorial"), PlayerActor, FGameplayTagContainer());
```

```blueprint
// Blueprint
Emit NPC Talked (NPC ID: NPC_Tutorial, Player: Self)
```

## Check Quest State

```cpp
// C++
EQuestState State = UQuestBlueprintLibrary::GetQuestState(GetWorld(), FName("Q_Tutorial_First"));
bool bIsActive = UQuestBlueprintLibrary::IsQuestActive(GetWorld(), FName("Q_Tutorial_First"));
```

```blueprint
// Blueprint
Get Quest State (Quest ID: Q_Tutorial_First) -> Quest State
Is Quest Active (Quest ID: Q_Tutorial_First) -> Boolean
```

## Architecture

### Core Components

- **QuestSubsystem** - Main quest manager (Game Instance Subsystem)
- **QuestEventBus** - Event dispatcher for game events
- **QuestLogComponent** - Player-side quest tracking (Actor Component)
- **QuestAsset** - Data Asset defining quest structure
- **QuestSaveGame** - Save game object for persistence

### Event Flow

```
Game Event (Item Acquired, NPC Talked, etc)
    ?
QuestEventBus::EmitEvent()
    ?
QuestSubsystem::OnQuestEvent()
    ?
Check Active Quests ? Check Objectives ? Update Progress
    ?
Broadcast Delegates (OnObjectiveUpdated, OnQuestCompleted, etc)
    ?
UI Updates / Rewards Granted
```

## Event Types

### Built-in Events

- **Item Events**: Acquired, Used, Gifted, Crafted, Delivered
- **NPC Events**: Talked, Affection Changed
- **Dialog Events**: Choice Picked, Node Reached
- **Location Events**: Enter Area, Leave Area
- **Combat Events**: Actor Killed
- **Time Events**: Time Reached, Day Changed
- **Weather Events**: Weather Changed
- **Needs Events**: Need Changed
- **Custom Events**: Tag-based custom events

### Emitting Events

```cpp
// From C++
#include "QuestEventBus.h"

UQuestSubsystem* QuestSystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
UQuestEventBus* EventBus = QuestSystem->GetEventBus();

// Item acquired
EventBus->EmitItemAcquired(FName("Apple"), 3, PlayerActor, Tags);

// NPC talked
EventBus->EmitNpcTalked(FName("Blacksmith"), PlayerActor, Tags);

// Custom event
FQuestEventPayload Payload;
Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Custom.MyEvent"));
Payload.StringParam = FName("CustomData");
Payload.IntParam = 42;
EventBus->EmitEvent(Payload);
```

```blueprint
// From Blueprint
Emit Item Acquired (Item ID: Apple, Count: 3, Instigator: Self)
Emit NPC Talked (NPC ID: Blacksmith, Player: Self)
Emit Custom Event (Event Tag: Quest.Event.Custom.MyEvent, String: CustomData, Int: 42)
```

## Quest Structure

### Phases

Quests are divided into **Phases** (stages/chapters). Each phase can have multiple objectives.

```
Quest
  ?? Phase 1: "Introduction"
      ?? Objective 1: Talk to NPC
      ?? Objective 2: Collect 5 apples
  ?? Phase 2: "Main Task"
      ?? Objective 3: Deliver apples to Baker
  ?? Phase 3: "Conclusion"
 ?? Objective 4: Return to quest giver
```

### Objectives

Each objective has:
- **Style**: Collect, Kill, Talk, Visit, Use, Deliver, Craft, Wait, etc.
- **Logic**: AND (all conditions), OR (any condition), Count, Sequence
- **Conditions**: Events that trigger progress
- **Gates**: Restrictions (time of day, weather, location, affection)
- **Counters**: Target count and current progress

### Conditions

Conditions define what events count toward objective progress:

```cpp
FObjectiveCondition Condition;
Condition.EventType = EConditionEventType::OnItemAcquired;
Condition.ItemId = FName("Apple");
Condition.Count = 5;
```

### Rewards

Rewards can be granted at multiple points:
- **On Objective Complete** - Per objective
- **On Phase Complete** - Per phase
- **On Quest Complete** - Final rewards

Reward types:
- Give Item
- Grant Currency
- Modify Stats/Needs
- Modify Affection
- Unlock Dialog Branch
- Unlock Recipe
- Grant Gameplay Tag
- Run Blueprint Event
- Execute Custom Effect

## Dependencies and Prerequisites

### Quest Dependencies

Quests can have prerequisites:

```cpp
FQuestDependency Dependency;
Dependency.RequiredQuestId = FName("Q_Tutorial_First");
Dependency.RequiredQuestState = EQuestState::Completed;
Dependency.RequiredLevel = 5;
Dependency.RequiredTags = TagContainer; // Player must have these tags
Dependency.ForbiddenTags = ForbiddenTags; // Player must NOT have these tags
```

### Start Policies

How quest becomes available:
- **Manual** - Player or script starts explicitly
- **Auto on Condition** - Auto-starts when conditions met
- **On Dialog** - Starts from dialog choice
- **On Item Pickup** - Starts when item acquired
- **On Overlap Trigger** - Starts in trigger volume
- **On Schedule Event** - Starts at scheduled time

## Save/Load System

### Auto-Save

Quest progress is automatically saved when:
- Quest starts
- Quest completes/fails
- Objective progress updates
- Phase changes

### Manual Save/Load

```cpp
// C++
UQuestSubsystem* QuestSystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();

// Save
QuestSystem->SaveQuestProgress();

// Load
QuestSystem->LoadQuestProgress();

// Reset all
QuestSystem->ResetAllProgress();
```

```blueprint
// Blueprint
Quest Subsystem ? Save Quest Progress
Quest Subsystem ? Load Quest Progress
Quest Subsystem ? Reset All Progress
```

## UI Integration

### Quest Log Component

Add to Player Controller/State for easy UI access:

```cpp
UPROPERTY(BlueprintReadOnly, Category = "Quest")
UQuestLogComponent* QuestLog;

// Get active quests for journal UI
TArray<FQuestSaveData> ActiveQuests = QuestLog->GetActiveQuests();

// Get current objective text for HUD
FText ObjectiveText = QuestLog->GetCurrentObjectiveText(QuestId);

// Track quest on HUD
QuestLog->SetTrackedQuest(QuestId);
```

### Event Delegates

Subscribe to events for UI updates:

```cpp
// C++
QuestSubsystem->OnQuestStarted.AddDynamic(this, &UMyClass::OnQuestStarted);
QuestSubsystem->OnQuestCompleted.AddDynamic(this, &UMyClass::OnQuestCompleted);
QuestSubsystem->OnObjectiveUpdated.AddDynamic(this, &UMyClass::OnObjectiveUpdated);
```

```blueprint
// Blueprint
Event On Quest Started
Event On Quest Completed
Event On Objective Updated
```

## Gameplay Tags

The system uses Gameplay Tags for categorization and event routing.

### Recommended Tag Structure

```
Quest
  ?? Quest.Event
  ?   ?? Quest.Event.Item
  ?   ?   ?? Quest.Event.Item.Acquired
  ?   ?   ?? Quest.Event.Item.Used
  ?   ?   ?? Quest.Event.Item.Crafted
  ?   ?? Quest.Event.NPC
  ?   ?   ?? Quest.Event.NPC.Talked
  ? ?   ?? Quest.Event.NPC.Killed
  ?   ?? Quest.Event.Dialog
  ?   ?? Quest.Event.Location
  ?   ?? Quest.Event.Custom
  ?? Quest.Category
  ?   ?? Quest.Category.Main
  ?   ?? Quest.Category.Side
  ?   ?? Quest.Category.Tutorial
  ?? Quest.Status
      ?? Quest.Status.Active
      ?? Quest.Status.Completed
      ?? Quest.Status.Failed
```

Setup in Project Settings ? GameplayTags.

## Best Practices

### 1. Use Unique Quest IDs

Always use unique, descriptive Quest IDs:
- ? `Q_Main_001_Tutorial`
- ? `Q_Side_Village_HelpBaker`
- ? `Quest1`
- ? `Tutorial`

### 2. Structure Phases Logically

Break complex quests into clear phases:

```
Phase 1: "Setup" - Get the quest, learn about problem
Phase 2: "Action" - Do the main task
Phase 3: "Resolution" - Return, get reward
```

### 3. Use Gameplay Tags Consistently

Create a tag hierarchy and stick to it. Use tags for:
- Event routing
- Quest categorization
- Conditional logic
- Filtering

### 4. Test with Validation

Always validate quest data:
- In editor: Click "Validate Quest Data" button
- At runtime: Check logs for warnings
- Use Data Validation plugin

### 5. Handle Edge Cases

Consider:
- What if player abandons quest?
- What if required NPC is dead?
- What if item is consumed before quest starts?
- Time-limited quests and player going AFK

### 6. Optimize Event Emission

Don't emit events every frame:
- ? Emit on discrete events (item acquired, dialog finished)
- ? Don't emit in Tick for continuous checks
- Use gates for time/location restrictions

## Integration Examples

### Inventory System Integration

```cpp
// In your inventory system
void UMyInventoryComponent::AddItem(FName ItemId, int32 Count)
{
    // Add item to inventory
    Inventory.Add(ItemId, Count);
    
    // Emit quest event
    UQuestBlueprintLibrary::EmitItemAcquired(this, ItemId, Count, GetOwner(), FGameplayTagContainer());
}
```

### Dialog System Integration

```cpp
// In your dialog system
void UMyDialogComponent::OnChoicePicked(FName DialogId, FName ChoiceId)
{
    // Process choice
    ProcessDialogChoice(ChoiceId);
    
    // Emit quest event
    UQuestBlueprintLibrary::EmitDialogChoice(this, DialogId, ChoiceId, FGameplayTagContainer());
}
```

### Time/Weather Integration

```cpp
// In your time-of-day system
void UTimeOfDaySystem::OnHourChanged(int32 NewHour)
{
    UQuestSubsystem* QuestSystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    if (QuestSystem)
    {
      QuestSystem->GetEventBus()->EmitTimeReached(NewHour, 0);
    }
}
```

## Troubleshooting

### Quest Doesn't Start

1. Check dependencies are met: `CanStartQuest(QuestId, Errors)`
2. Verify Quest ID matches asset
3. Check quest state is not already Active/Completed
4. Check if quest is repeatable

### Objective Not Updating

1. Verify event is being emitted (check logs)
2. Check condition parameters match (ItemId, NpcId, etc)
3. Verify gates are satisfied (time, weather, location)
4. Check objective is not already complete

### Save Not Working

1. Check save slot name
2. Verify save game object is created
3. Check file permissions
4. Look for errors in Output Log

## Performance

### No Tick Overhead

The system is fully event-driven with **ZERO per-actor tick cost**.

### Optimizations

- Event subscriptions use native delegates
- Quest assets loaded on-demand via Asset Manager
- Save data is compact and versioned
- Minimal allocations per frame

### Profiling

Use Unreal Insights to profile:
- `QuestSubsystem::OnQuestEvent`
- `QuestEventBus::EmitEvent`
- Save/Load operations

## Advanced Topics

### Custom Objective Types

Extend by creating new condition evaluators:

```cpp
UCLASS()
class UMyCustomConditionEvaluator : public UObject
{
    GENERATED_BODY()
    
public:
    bool Evaluate(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload)
{
        // Custom logic
   return true;
    }
};
```

### Custom Rewards

Create custom reward executors:

```cpp
void UMyRewardSystem::ExecuteCustomReward(const FQuestReward& Reward)
{
    // Custom reward logic
}
```

### Branching Quests

Use phase transitions with conditional branches:

```cpp
FQuestPhaseTransition Transition;
Transition.ConditionalBranches.Add(FName("GoodPath"), FName("Phase_Good"));
Transition.ConditionalBranches.Add(FName("EvilPath"), FName("Phase_Evil"));
```

## ?? Documentation

Comprehensive documentation included:

- **[Quick Start Guide](Docs/QuickStart.md)** - Get your first quest running in 10 steps
- **[NPC Integration Guide](Docs/NPCIntegration.md)** - How to attach quests to NPCs and objects
- **[Complete Examples](Docs/Examples.md)** - Detailed quest configurations
- **[API Reference](Docs/API_Reference.md)** - Full API documentation  
- **[Best Practices](Docs/BestPractices.md)** - Production-tested patterns
- **[Cheat Sheet](Docs/CheatSheet.md)** - Quick reference

## Support

- Documentation: `/Docs/`
- Issues: GitHub Issues
- Discord: [Link]

## License

Copyright © 2024. All rights reserved.

## Credits

Created for EchoesOfAffection project.
