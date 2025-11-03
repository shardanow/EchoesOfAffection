# Blueprint API Reference

Complete reference for using the Relationship System in Blueprints.

## Table of Contents

1. [Component Setup](#component-setup)
2. [Basic Operations](#basic-operations)
3. [Dimension Operations](#dimension-operations)
4. [State Operations](#state-operations)
5. [Trait Operations](#trait-operations)
6. [Action Execution](#action-execution)
7. [Queries](#queries)
8. [Save/Load](#saveload)
9. [Events](#events)
10. [UI Widgets](#ui-widgets)

---

## Component Setup

### Add Relationship Component

1. Open Actor Blueprint
2. Components panel > Add Component
3. Search "Relationship"
4. Select "Relationship Component"
5. In Details:
   - Set **Profile** (e.g., DA_Profile_FriendlyNPC)
   - (Optional) Enable **Auto Create Relationships**
   - (Optional) Set **Auto Create Radius**

### Get Relationship Component

**Node:** `Get Component By Class`
- **Class:** Relationship Component
- **Return:** Relationship Component (or None if not found)

```
[Actor] -> Get Component By Class -> [Relationship Component]
```

---

## Basic Operations

### Create Relationship With Actor

**Node:** `Create Relationship With`
**Category:** Relationship

**Inputs:**
- **Target:** Target Actor

**Outputs:**
- **Return Value:** Boolean (success)

**Example:**
```
[Player] -> Get Relationship Component -> Create Relationship With
  Target: [NPC Actor]
-> [Success]
```

### Check if Has Relationship

**Node:** `Has Relationship With`
**Category:** Relationship

**Inputs:**
- **Target:** Target Actor

**Outputs:**
- **Return Value:** Boolean

**Example:**
```
[Player] -> Get Relationship Component -> Has Relationship With
 Target: [NPC Actor]
-> Branch (If True: Show relationship UI)
```

### Remove Relationship

**Node:** `Remove Relationship With`
**Category:** Relationship

**Inputs:**
- **Target:** Target Actor

**Outputs:**
- **Return Value:** Boolean

---

## Dimension Operations

### Get Dimension Value

**Node:** `Get Dimension Value`
**Category:** Relationship | Dimensions

**Inputs:**
- **Target Actor:** The actor you have relationship with
- **Dimension Tag:** Gameplay Tag (e.g., Relationship.Dimension.Trust)

**Outputs:**
- **Return Value:** Float (dimension value)

**Example:**
```
[Player] -> Get Relationship Component -> Get Dimension Value
  Target Actor: [NPC]
  Dimension Tag: Relationship.Dimension.Trust
-> [Float: 45.0]
```

### Set Dimension Value (Absolute)

**Node:** `Set Dimension Value`
**Category:** Relationship | Dimensions

**Inputs:**
- **Target Actor:** Target
- **Dimension Tag:** Which dimension
- **New Value:** Absolute value to set

**Outputs:**
- **Return Value:** Boolean (success)

**Example:**
```
[Player] -> Get Relationship Component -> Set Dimension Value
  Target Actor: [NPC]
  Dimension Tag: Relationship.Dimension.Friendship
  New Value: 100.0
```

### Modify Dimension Value (Relative)

**Node:** `Modify Dimension Value`
**Category:** Relationship | Dimensions

**Inputs:**
- **Target Actor:** Target
- **Dimension Tag:** Which dimension
- **Delta:** Amount to add/subtract

**Outputs:**
- **Return Value:** Boolean (success)

**Example:**
```
On Player Helps NPC:
  [Player] -> Get Relationship Component -> Modify Dimension Value
    Target Actor: [NPC]
    Dimension Tag: Relationship.Dimension.Trust
  Delta: +10.0
```

### Batch Modify Multiple Dimensions

**Node:** `Modify Dimension Values`
**Category:** Relationship | Dimensions

**Inputs:**
- **Target Actor:** Target
- **Dimension Deltas:** Map (Gameplay Tag -> Float)

**Outputs:**
- **Return Value:** Boolean

**Example:**
```
Create Map:
  Relationship.Dimension.Trust: +15
  Relationship.Dimension.Friendship: +20
  Relationship.Dimension.Fear: -5
  
[Player] -> Get Relationship Component -> Modify Dimension Values
  Target Actor: [NPC]
  Dimension Deltas: [Map from above]
```

---

## State Operations

### Get Current State

**Node:** `Get Current State`
**Category:** Relationship | State

**Inputs:**
- **Target Actor:** Target

**Outputs:**
- **Return Value:** Gameplay Tag (current state)

**Example:**
```
[Player] -> Get Relationship Component -> Get Current State
  Target Actor: [NPC]
-> [Tag: Relationship.State.Friend]
  
-> Compare Tag (Matches Tag: Relationship.State.Friend?)
   -> Branch
```

### Force State Change

**Node:** `Force State Change`
**Category:** Relationship | State

**Inputs:**
- **Target Actor:** Target
- **New State:** State tag to set

**Outputs:**
- **Return Value:** Boolean

**Example:**
```
On Quest Complete:
  [Player] -> Get Relationship Component -> Force State Change
    Target Actor: [Quest Giver]
    New State: Relationship.State.Ally
```

**Note:** This bypasses transition rules. Use sparingly.

---

## Trait Operations

### Add Trait

**Node:** `Add Trait`
**Category:** Relationship | Traits

**Inputs:**
- **Target Actor:** Target
- **Trait Tag:** Trait to add

**Outputs:**
- **Return Value:** Boolean (success)

**Example:**
```
On NPC Forgives Player:
  [Player] -> Get Relationship Component -> Add Trait
    Target Actor: [NPC]
    Trait Tag: Relationship.Trait.Forgiving
```

### Remove Trait

**Node:** `Remove Trait`
**Category:** Relationship | Traits

**Inputs:**
- **Target Actor:** Target
- **Trait Tag:** Trait to remove

**Outputs:**
- **Return Value:** Boolean

### Check if Has Trait

**Node:** `Has Trait`
**Category:** Relationship | Traits

**Inputs:**
- **Target Actor:** Target
- **Trait Tag:** Trait to check

**Outputs:**
- **Return Value:** Boolean

**Example:**
```
Check if NPC is suspicious:
  [Player] -> Get Relationship Component -> Has Trait
    Target Actor: [NPC]
   Trait Tag: Relationship.Trait.Suspicious
  -> Branch (If True: Show different dialogue)
```

---

## Action Execution

### Can Execute Action

**Node:** `Can Execute Action`
**Category:** Relationship | Actions

**Inputs:**
- **Target Actor:** Target
- **Action Tag:** Action to check

**Outputs:**
- **Return Value:** Boolean

**Example:**
```
[Player] -> Get Relationship Component -> Can Execute Action
  Target Actor: [NPC]
  Action Tag: Relationship.Action.Gift
-> Branch (Enable/Disable UI button)
```

### Execute Action

**Node:** `Execute Action`
**Category:** Relationship | Actions

**Inputs:**
- **Target Actor:** Target
- **Action Tag:** Action to execute

**Outputs:**
- **Return Value:** Boolean (success)

**Example:**
```
On Player Presses "Talk" Button:
  [Player] -> Get Relationship Component -> Execute Action
    Target Actor: [Selected NPC]
    Action Tag: Relationship.Action.Talk
-> Branch
     True: Play success animation
     False: Show "Cannot talk right now" message
```

### Get Available Actions

**Node:** `Get Available Actions`
**Category:** Relationship | Actions

**Inputs:**
- **Target Actor:** Target

**Outputs:**
- **Return Value:** Array of Gameplay Tags

**Example:**
```
[Player] -> Get Relationship Component -> Get Available Actions
  Target Actor: [NPC]
-> ForEach Loop
   -> Create UI button for each action
```

---

## Queries

### Get All Relationship Targets

**Node:** `Get All Relationship Targets`
**Category:** Relationship | Query

**Outputs:**
- **Return Value:** Array of Actors

**Example:**
```
Get all NPCs player has relationships with:
  [Player] -> Get Relationship Component -> Get All Relationship Targets
  -> ForEach Loop
     -> Display in UI list
```

### Find Targets by State

**Node:** `Find Targets By State`
**Category:** Relationship | Query

**Inputs:**
- **State Tag:** State to filter by

**Outputs:**
- **Return Value:** Array of Actors

**Example:**
```
Get all friends:
  [Player] -> Get Relationship Component -> Find Targets By State
    State Tag: Relationship.State.Friend
  -> [Array of Friend Actors]
```

### Find Targets by Dimension Range

**Node:** `Find Targets By Dimension Range`
**Category:** Relationship | Query

**Inputs:**
- **Dimension Tag:** Dimension to filter
- **Min Value:** Minimum value
- **Max Value:** Maximum value

**Outputs:**
- **Return Value:** Array of Actors

**Example:**
```
Get NPCs with high trust:
  [Player] -> Get Relationship Component -> Find Targets By Dimension Range
    Dimension Tag: Relationship.Dimension.Trust
    Min Value: 70.0
    Max Value: 100.0
  -> [Array of High Trust Actors]
```

### Get Best/Worst Target by Dimension

**Nodes:**
- `Get Best Target By Dimension`
- `Get Worst Target By Dimension`

**Category:** Relationship | Query

**Inputs:**
- **Dimension Tag:** Dimension to compare

**Outputs:**
- **Return Value:** Actor (or None)

**Example:**
```
Get best friend:
  [Player] -> Get Relationship Component -> Get Best Target By Dimension
    Dimension Tag: Relationship.Dimension.Friendship
  -> [Best Friend Actor]
  
Get biggest enemy:
  [Player] -> Get Relationship Component -> Get Worst Target By Dimension
    Dimension Tag: Relationship.Dimension.Trust
  -> [Lowest Trust Actor]
```

---

## Save/Load

### Save Relationships

**Node:** `Save Relationships`
**Category:** Relationships | Save
**Target:** Relationship Save Game Helper (static function)

**Inputs:**
- **World Context Object:** Self
- **Slot Name:** String (empty = default)

**Outputs:**
- **Return Value:** Boolean (success)

**Example:**
```
On Save Game:
  Save Relationships
    World Context Object: Self
    Slot Name: "MyRelationshipSave"
  -> Branch
     True: Show "Saved successfully"
     False: Show "Save failed"
```

### Load Relationships

**Node:** `Load Relationships`
**Category:** Relationships | Save
**Target:** Relationship Save Game Helper

**Inputs:**
- **World Context Object:** Self
- **Slot Name:** String (empty = default)

**Outputs:**
- **Return Value:** Boolean

**Example:**
```
On Load Game:
  Load Relationships
    World Context Object: Self
    Slot Name: "MyRelationshipSave"
  -> Branch
     True: Show "Loaded successfully"
     False: Show "No save found"
```

### Check if Save Exists

**Node:** `Does Save Exist`
**Category:** Relationships | Save

**Inputs:**
- **Slot Name:** String

**Outputs:**
- **Return Value:** Boolean

---

## Events

### Bind to Subsystem Events

You can bind to relationship events for reactive behavior:

#### On Dimension Changed

**Event:** `On Dimension Changed`
**Delegate:** Relationship Subsystem -> On Dimension Changed

**Parameters:**
- **Subject:** Actor
- **Target:** Actor
- **Dimension Tag:** Gameplay Tag
- **New Value:** Float

**Example:**
```
Event BeginPlay:
  Get Game Instance
  -> Get Subsystem (Relationship Subsystem)
  -> Bind Event to On Dimension Changed
     Event: OnDimChanged
     
Event OnDimChanged(Subject, Target, DimensionTag, NewValue):
  Branch (Subject == Player Character?)
 True:
      Branch (DimensionTag == Relationship.Dimension.Trust?)
        True:
          Branch (NewValue >= 50?)
    True: Play "Trust Gained" effect
```

#### On State Transitioned

**Event:** `On State Transitioned`

**Parameters:**
- **Subject:** Actor
- **Target:** Actor
- **Old State:** Gameplay Tag
- **New State:** Gameplay Tag

**Example:**
```
Bind Event to On State Transitioned:

Event OnStateChanged(Subject, Target, OldState, NewState):
  Branch (Subject == Player?)
    True:
  Switch on Tag (NewState):
        Case Relationship.State.Friend:
          -> Unlock friend dialogue
      Case Relationship.State.Enemy:
          -> Trigger combat mode
```

#### Other Events

- `On Trait Added`
- `On Trait Removed`
- `On Action Executed`

---

## UI Widgets

### Using Relationship Dimension Bar

**Base Class:** `RelationshipDimensionBar` (C++)
**Usage:** Create Blueprint child widget

**Setup:**
1. Create Widget Blueprint
2. Parent Class: Relationship Dimension Bar
3. Add Progress Bar widget (name it "ProgressBar")
4. (Optional) Add Text Block "ValueText"
5. (Optional) Add Text Block "NameText"

**Properties to Set:**
- **Dimension Tag:** Which dimension to display
- **Show Numeric Value:** Show number?
- **Show Dimension Name:** Show label?
- **Normalize Value:** Convert to 0-1 range?
- **Auto Refresh:** Update automatically?
- **Refresh Interval:** Seconds between updates

**Blueprint Usage:**
```
Create Widget (WBP_DimensionBar)
-> Set Actors
   Subject: Player
   Target: NPC
-> Add to Viewport
```

### Using Relationship Panel

**Base Class:** `RelationshipPanel`

**Setup:**
1. Create Widget Blueprint
2. Parent Class: Relationship Panel
3. Add Vertical Box widget (name it "DimensionContainer")
4. Set Dimension Bar Class property to your dimension bar BP

**Properties:**
- **Dimension Bar Class:** Blueprint class to spawn for each dimension
- **Show All Dimensions:** Show all or only non-zero?

**Usage:**
```
Create Widget (WBP_RelationshipPanel)
-> Set Actors
   Subject: Player
   Target: Selected NPC
-> Add to Viewport
```

This will automatically create dimension bars for all dimensions.

### Custom Widget Base

For custom widgets, inherit from `RelationshipWidgetBase`:

**Override Events:**
- `BP On Refresh Display`: Called when widget should update
- `Native Refresh Display` (C++): Native implementation

**Properties:**
- **Subject:** Auto-set when calling Set Actors
- **Target:** Auto-set when calling Set Actors
- **Subject Component:** Cached component reference
- **Target Component:** Cached component reference

---

## Common Patterns

### Dialogue Integration

```
On Dialogue Choice Selected:
  Switch on Choice Tag:
    Case "Choice.Agree":
   [Player] -> Relationship Component -> Modify Dimension Value
 Target: Dialogue Partner
        Dimension: Trust
        Delta: +5
    Case "Choice.Disagree":
      [Player] -> Relationship Component -> Modify Dimension Value
        Target: Dialogue Partner
        Dimension: Trust
    Delta: -10
```

### Quest Reward

```
On Quest Completed:
  [Player] -> Relationship Component -> Modify Dimension Values
    Target: Quest Giver
    Deltas:
      Trust: +20
      Friendship: +15
```

### AI Behavior

```
Event Tick (AI Controller):
  [Pawn] -> Relationship Component -> Get Current State
    Target: Player
  -> Switch on Tag:
       Case Enemy: Attack Player
       Case Friend: Follow Player
   Case Stranger: Ignore Player
```

### Shop Prices

```
Calculate Discount:
  [Merchant] -> Relationship Component -> Get Dimension Value
    Target: Player
    Dimension: Friendship
  -> Map Range (0-100 to 0-50)
  -> [Discount Percentage]
  
Final Price = Base Price * (1 - Discount%)
```

---

## Debugging

### Enable Debug Logging

Console command:
```
RelationshipSystem.Debug 1
```

### Debug Widget

Create a debug widget to visualize relationships:

```
Widget Update (Tick):
  [Player] -> Relationship Component -> Get All Relationship Targets
  -> ForEach:
     Create Text Block:
       Text: [Target Name] - [Get Current State]
       
     Get Dimension Value (Trust)
  Get Dimension Value (Friendship)
     Display in debug panel
```

---

## Best Practices

1. **Cache Component References**
   ```
   Event BeginPlay:
     Get Relationship Component -> [Save to Variable]
   
   Later:
     [Use Cached Variable] instead of getting component again
   ```

2. **Use Events Instead of Tick**
   ```
   DON'T: Tick -> Get Dimension Value -> Check if changed
   DO: Bind to On Dimension Changed event
   ```

3. **Batch Operations**
   ```
   DON'T: Modify Trust, then Modify Friendship, then Modify Fear
   DO: Create Map {Trust: +5, Friendship: +10, Fear: -3}
     -> Modify Dimension Values
   ```

4. **Use Actions for Complex Effects**
   ```
   DON'T: Manually modify dimensions in Blueprint
   DO: Create Action Data Asset, Execute Action
   ```

This keeps balance changes in Data Assets where designers can adjust them.
