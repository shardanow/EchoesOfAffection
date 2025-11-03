# Example Content for Relationship System

This directory contains example Data Assets and configurations for the Relationship System plugin.

## Quick Start Examples

### 1. Basic NPC Relationship Setup

This example shows how to set up a simple NPC with relationship capabilities.

**Dimensions:**
- Trust (-100 to 100)
- Friendship (0 to 100)
- Fear (0 to 100)

**States:**
- Stranger (default)
- Acquaintance
- Friend
- Close Friend

**Actions:**
- Talk (+5 Friendship)
- Gift (+15 Friendship, +10 Trust)
- Compliment (+3 Friendship)

### 2. Creating Data Assets

#### Step 1: Create Dimensions

1. Right-click in Content Browser
2. Miscellaneous > Data Asset
3. Select `RelationshipDimension`
4. Name it `DA_Dimension_Trust`

**Configuration:**
```
Asset Tag: Relationship.Dimension.Trust
Display Name: Trust
Min Value: -100
Max Value: 100
Default Value: 0
Enable Decay: true
Decay Target: 0
Decay Rate Per Hour: 1.0
Display Color: (R=0.3, G=0.7, B=1.0)
```

Repeat for Friendship and Fear dimensions.

#### Step 2: Create States

Create `DA_State_Stranger`:
```
Asset Tag: Relationship.State.Stranger
Display Name: Stranger
Is Default State: true
Transitions: (See transition setup below)
Available Actions: Relationship.Action.Talk, Relationship.Action.Compliment
Display Color: (R=0.5, G=0.5, B=0.5)
```

Create `DA_State_Friend`:
```
Asset Tag: Relationship.State.Friend
Display Name: Friend
Transitions: -> Close Friend when Friendship > 70
Available Actions: Relationship.Action.Talk, Relationship.Action.Gift, Relationship.Action.Compliment
Display Color: (R=0.3, G=1.0, B=0.3)
```

#### Step 3: Create Actions

Create `DA_Action_Talk`:
```
Asset Tag: Relationship.Action.Talk
Display Name: Talk
Effects:
  - Dimension: Relationship.Dimension.Friendship
    Base Value: 5.0
    Random Variance: 0.2
    Success Probability: 1.0
Cooldown: 10.0 seconds
```

Create `DA_Action_Gift`:
```
Asset Tag: Relationship.Action.Gift
Display Name: Give Gift
Effects:
  - Dimension: Relationship.Dimension.Friendship
    Base Value: 15.0
    Random Variance: 0.3
    Success Probability: 0.9
  - Dimension: Relationship.Dimension.Trust
    Base Value: 10.0
    Random Variance: 0.1
    Success Probability: 1.0
Costs:
  - Dimension: Relationship.Dimension.Friendship
  Minimum Value: 10.0 (must be at least acquaintance)
Cooldown: 60.0 seconds
```

#### Step 4: Create Traits

Create `DA_Trait_Forgiving`:
```
Asset Tag: Relationship.Trait.Forgiving
Display Name: Forgiving
Can Stack: false
Dimension Modifiers:
  - Dimension: Relationship.Dimension.Trust
    Additive Modifier: 0
    Multiplicative Modifier: 0.5 (negative trust changes are halved)
    Filter Actions: (empty = all actions)
```

Create `DA_Trait_Suspicious`:
```
Asset Tag: Relationship.Trait.Suspicious
Display Name: Suspicious
Can Stack: false
Dimension Modifiers:
  - Dimension: Relationship.Dimension.Trust
    Additive Modifier: -2
    Multiplicative Modifier: 1.5 (trust gains are slower)
Conflicting Traits: Relationship.Trait.Forgiving
```

#### Step 5: Create Profile

Create `DA_Profile_FriendlyNPC`:
```
Asset Tag: Relationship.Profile.FriendlyNPC
Display Name: Friendly NPC
Initial Dimension Values:
  - Relationship.Dimension.Trust: 10
  - Relationship.Dimension.Friendship: 5
  - Relationship.Dimension.Fear: 0
Initial State: Relationship.State.Stranger
Initial Traits: (none)
Can Be Subject: true
Can Be Target: true
Persist Relationships: true
```

Create `DA_Profile_SuspiciousNPC`:
```
Asset Tag: Relationship.Profile.SuspiciousNPC
Display Name: Suspicious NPC
Initial Dimension Values:
  - Relationship.Dimension.Trust: -10
  - Relationship.Dimension.Friendship: 0
  - Relationship.Dimension.Fear: 5
Initial State: Relationship.State.Stranger
Initial Traits: Relationship.Trait.Suspicious
Can Be Subject: true
Can Be Target: true
```

#### Step 6: Create Database

Create `DA_RelationshipDatabase`:
```
Dimensions:
  - DA_Dimension_Trust
  - DA_Dimension_Friendship
  - DA_Dimension_Fear

States:
  - DA_State_Stranger
  - DA_State_Acquaintance
  - DA_State_Friend
  - DA_State_CloseF Friend

Traits:
  - DA_Trait_Forgiving
  - DA_Trait_Suspicious

Actions:
  - DA_Action_Talk
  - DA_Action_Gift
  - DA_Action_Compliment

Profiles:
- DA_Profile_FriendlyNPC
  - DA_Profile_SuspiciousNPC
  - DA_Profile_Player

Default Profile: DA_Profile_FriendlyNPC
```

#### Step 7: Configure Project Settings

1. Edit > Project Settings
2. Plugins > Relationship System
3. Set Database: DA_RelationshipDatabase
4. Enable Automatic Decay: true
5. Decay Processing Interval: 60.0
6. Game Hours Per Real Second: 1.0

### 3. Blueprint Setup

#### Add Component to NPC Blueprint

In your NPC Blueprint (e.g., BP_FriendlyNPC):

1. Add Component > Relationship Component
2. Set Profile: DA_Profile_FriendlyNPC
3. (Optional) Enable Auto Create Relationships
4. (Optional) Set Auto Create Radius: 2000

#### Player Interaction Blueprint

```
On Player Interact with NPC:
  1. Get Relationship Component (from Player)
  2. Execute Action
     - Target Actor: NPC
     - Action Tag: Relationship.Action.Talk
  3. If successful:
     - Play animation
     - Show dialogue
```

#### Query Relationship in Blueprint

```
To check if NPC is friend:
  1. Get Relationship Component (from Player)
  2. Get Current State
     - Target: NPC
  3. Compare: Is Equal to Relationship.State.Friend
```

#### Display UI

```
On Show Relationship UI:
  1. Create Widget: WBP_RelationshipPanel
  2. Set Actors:
     - Subject: Player
     - Target: Selected NPC
  3. Add to Viewport
```

### 4. Advanced: State Transitions

Create rule for Stranger -> Acquaintance transition:

Create `DA_Rule_BecomeAcquaintance`:
```
Conditions:
  - Dimension: Relationship.Dimension.Friendship
    Operator: Greater
    Compare Value: 20
```

In DA_State_Stranger, add transition:
```
Transitions:
  - Target State: Relationship.State.Acquaintance
    Required Rules: Relationship.Rule.BecomeAcquaintance
    Priority: 10
    Hysteresis Delay: 0 (instant)
```

### 5. Integration with Dialogue System

In dialogue choices, add relationship effects:

```
[DialogueChoice]
On Choice Selected:
  1. Get Relationship Component
  2. Modify Dimension Value
     - Target: Dialogue Participant
   - Dimension: Relationship.Dimension.Trust
     - Delta: +5
```

Dialogue condition based on relationship:

```
[DialogueCondition]
Check if friendly enough to ask favor:
  1. Get Relationship Component
  2. Get Dimension Value
     - Target: NPC
     - Dimension: Relationship.Dimension.Friendship
  3. Compare: >= 50
```

## Testing in PIE

1. Place two actors with RelationshipComponent in level
2. Assign different profiles
3. Play in Editor
4. Use console commands:
   ```
   RelationshipSystem.Debug 1
   ```
5. Or create debug UI widget to visualize relationships

## Performance Tips

- Use events instead of tick-based updates
- Process decay periodically (not every frame)
- Cache component references
- Use indexed lookups (already handled by subsystem)
- Limit number of active relationships per actor

## Common Patterns

### Gradual Trust Building
```
Multiple small positive actions (Talk) are more reliable than one big action (Gift)
Add variance to make interactions feel organic
Use decay to require ongoing interaction
```

### Faction Relationships
```
Create faction actors with RelationshipComponent
Player relationships with faction representatives
Use traits to represent faction policies
```

### Reputation System
```
Create global "Reputation" dimension
All NPCs share initial value based on player reputation
Individual relationships can diverge from reputation
```
