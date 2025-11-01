# Recommended GameplayTags Configuration

## How to Setup

1. Open Project Settings -> GameplayTags
2. Add this to `DefaultGameplayTags.ini` or create tags manually in editor

## Event Tags Hierarchy

```
Game
??? Event
    ??? Generic
    ?   ??? Custom

Inventory
??? Event
    ??? ItemAcquired
    ??? ItemUsed
    ??? ItemCrafted
  ??? ItemGifted
    ??? ItemDropped
    ??? ItemSold
    ??? ItemBought

Dialogue
??? Event
    ??? Started
    ??? Ended
    ??? NodeReached
    ??? ChoiceMade
    ??? BranchUnlocked

Quest
??? Event
    ??? Started
    ??? Completed
  ??? Failed
    ??? Abandoned
    ??? ObjectiveCompleted
    ??? PhaseChanged

Time
??? Event
    ??? HourChanged
    ??? DayChanged
    ??? TimeOfDay
    ?   ??? Morning
    ?   ??? Afternoon
    ?   ??? Evening
    ???? Night
 ??? Season
        ??? Spring
        ??? Summer
   ??? Autumn
     ??? Winter

Weather
??? Event
    ??? Changed
    ??? Sunny
    ??? Rainy
    ??? Stormy
    ??? Snowy
    ??? Foggy

NPC
??? Event
    ??? TalkStarted
    ??? TalkEnded
    ??? AffinityChanged
    ??? TrustChanged
??? MemoryCreated

Location
??? Event
    ??? Entered
    ??? Exited
    ??? Discovered

Combat
??? Event
    ??? EnemyKilled
    ??? DamageTaken
    ??? DamageDealt
    ??? BossDef eated
```

## Effect Tags Hierarchy

```
Effect
??? Quest
?   ??? Start
?   ??? Complete
?   ??? Fail
?   ??? Abandon
??? Dialogue
?   ??? Start
?   ??? End
?   ??? UnlockBranch
??? Inventory
?   ??? GiveItem
?   ??? RemoveItem
?   ??? ModifyGold
??? NPC
?   ??? ModifyAffinity
?   ??? ModifyTrust
?   ??? AddMemory
??? Player
?   ??? ModifyHealth
?   ??? ModifyStamina
?   ??? ModifyStats
??? World
    ??? AddTag
    ??? RemoveTag
    ??? TriggerEvent
```

## Condition Tags Hierarchy

```
Condition
??? Quest
?   ??? IsActive
?   ??? IsCompleted
?   ??? ObjectiveComplete
??? Inventory
?   ??? HasItem
?   ??? HasItemCount
?   ??? HasGold
??? Time
?   ??? InRange
?   ??? IsTimeOfDay
?   ??? IsDay
??? NPC
?   ??? AffinityAbove
?   ??? AffinityBelow
?   ??? HasMemory
??? Location
?   ??? IsInArea
?   ??? HasDiscovered
??? Player
    ??? HasTag
    ??? StatAbove
    ??? StatBelow
```

## DefaultGameplayTags.ini Example

```ini
[/Script/GameplayTags.GameplayTagsList]

; ============================================
; Event Tags
; ============================================

+GameplayTagList=(Tag="Inventory.Event.ItemAcquired",DevComment="Item was acquired by player")
+GameplayTagList=(Tag="Inventory.Event.ItemUsed",DevComment="Item was used")
+GameplayTagList=(Tag="Inventory.Event.ItemCrafted",DevComment="Item was crafted")
+GameplayTagList=(Tag="Inventory.Event.ItemGifted",DevComment="Item was gifted to NPC")

+GameplayTagList=(Tag="Dialogue.Event.Started",DevComment="Dialogue session started")
+GameplayTagList=(Tag="Dialogue.Event.NodeReached",DevComment="Dialogue node was reached")
+GameplayTagList=(Tag="Dialogue.Event.ChoiceMade",DevComment="Player made a choice")

+GameplayTagList=(Tag="Quest.Event.Started",DevComment="Quest was started")
+GameplayTagList=(Tag="Quest.Event.Completed",DevComment="Quest was completed")
+GameplayTagList=(Tag="Quest.Event.ObjectiveCompleted",DevComment="Quest objective completed")

+GameplayTagList=(Tag="Time.Event.HourChanged",DevComment="Game hour changed")
+GameplayTagList=(Tag="Time.Event.DayChanged",DevComment="Game day changed")

+GameplayTagList=(Tag="NPC.Event.AffinityChanged",DevComment="NPC affinity changed")
+GameplayTagList=(Tag="NPC.Event.TalkStarted",DevComment="Started talking to NPC")

+GameplayTagList=(Tag="Location.Event.Entered",DevComment="Entered area/location")
+GameplayTagList=(Tag="Location.Event.Exited",DevComment="Exited area/location")

+GameplayTagList=(Tag="Combat.Event.EnemyKilled",DevComment="Enemy was killed")

; ============================================
; Effect Tags
; ============================================

+GameplayTagList=(Tag="Effect.Quest.Start",DevComment="Start a quest")
+GameplayTagList=(Tag="Effect.Quest.Complete",DevComment="Complete a quest")

+GameplayTagList=(Tag="Effect.Dialogue.Start",DevComment="Start dialogue")
+GameplayTagList=(Tag="Effect.Dialogue.UnlockBranch",DevComment="Unlock dialogue branch")

+GameplayTagList=(Tag="Effect.Inventory.GiveItem",DevComment="Give item to player")
+GameplayTagList=(Tag="Effect.Inventory.RemoveItem",DevComment="Remove item from player")

+GameplayTagList=(Tag="Effect.NPC.ModifyAffinity",DevComment="Modify NPC affinity")

; ============================================
; Condition Tags
; ============================================

+GameplayTagList=(Tag="Condition.Quest.IsActive",DevComment="Check if quest is active")
+GameplayTagList=(Tag="Condition.Quest.IsCompleted",DevComment="Check if quest is completed")

+GameplayTagList=(Tag="Condition.Inventory.HasItem",DevComment="Check if player has item")
+GameplayTagList=(Tag="Condition.Inventory.HasItemCount",DevComment="Check if player has N items")

+GameplayTagList=(Tag="Condition.Time.InRange",DevComment="Check if time is in range")

+GameplayTagList=(Tag="Condition.NPC.AffinityAbove",DevComment="Check if NPC affinity is above threshold")
```

## Usage in Data Assets

### Quest Data Asset Example

```
Objective:
  EventType: OnItemAcquired
  EventTag: "Inventory.Event.ItemAcquired"  ? Tag from hierarchy
  ItemId: "Apple"
  Count: 5
```

### Dialogue Data Asset Example

```
Node Effects:
  - EmitGameEvent:
  EventTag: "Quest.Event.Started"  ? Tag from hierarchy
      StringParam: "MyQuestId"
```

### Conditional Node Example

```
Node Conditions:
  - EvaluateCondition:
      ConditionTag: "Condition.Quest.IsActive"  ? Tag from hierarchy
   StringParam: "MyQuestId"
```

## Best Practices

1. **Use Hierarchy** — Always organize tags in logical hierarchy
2. **Be Consistent** — Follow naming conventions
3. **Document** — Add DevComment to every tag
4. **Avoid Duplication** — Check existing tags before creating new ones
5. **Use Specific Tags** — Prefer "Inventory.Event.ItemAcquired" over "Game.Event.Generic"

## Migration from Old System

If you're migrating from quest-specific events to generic events:

**Old:**
```
EConditionEventType::OnItemAcquired
```

**New:**
```
EventTag: "Inventory.Event.ItemAcquired"
```

The Quest System's QuestEventBridge automatically converts GameEventBus events to quest events!
