# Relationship System Plugin

## Overview
RelationshipSystem is a production-ready, data-driven relationship management plugin for Unreal Engine 5.6. It provides a complete framework for managing relationships between actors (NPCs, factions, players) with full Blueprint support and zero per-actor tick overhead.

## Features
- **Fully Data-Driven**: All configuration through Data Assets
- **Event-Driven Architecture**: No tick-based updates
- **Performance Optimized**: O(1)/O(logN) operations with caching
- **Blueprint Friendly**: Complete designer-accessible API
- **Modular Design**: Clean separation of concerns following SOLID principles
- **Save/Load Support**: Versioned serialization system
- **Editor Tools**: Validation, hot-reload, database viewer
- **Production Ready**: AAA-quality architecture and implementation

## Installation

1. Copy the `RelationshipSystem` folder to your project's `Plugins` directory
2. Regenerate your project files
3. Rebuild your project
4. Enable the plugin in your project settings

## ⚡ Quick Fix: "No relationships visible in UI"

### Problem
Relationship panel shows no dimensions/state during dialogue.

### Root Cause
**Relationship doesn't exist** between Player and NPC.

### Solution

#### Option 1: Auto-create in Dialogue System (Recommended)
Add to `DialogueSubsystem::StartDialogue_Implementation()`:

```cpp
// Auto-create relationship when dialogue starts
if (UNPCRelationshipComponent* PlayerComp = Player->FindComponentByClass<UNPCRelationshipComponent>())
{
    if (!PlayerComp->HasRelationshipWith(NPC))
  {
        PlayerComp->CreateRelationshipWith(NPC);
        UE_LOG(LogDialogueSubsystem, Log, TEXT("Created relationship: %s <-> %s"), 
     *Player->GetName(), *NPC->GetName());
    }
}
```

#### Option 2: Enable Auto-Create on Components
In Player/NPC `NPCRelationshipComponent`:
- Set `bAutoCreateRelationships = true`
- Set `AutoCreateRadius = 5000.0`

#### Option 3: Manual Blueprint Setup
In level BeginPlay:
```
Player->GetComponentByClass(NPCRelationshipComponent)->CreateRelationshipWith(NPC)
```

### Verify
Check Output Log for:
```
[LogRelationshipSystem] Created relationship between bp_MyAnimeCharacter_C_0 and BP_Lianne_C_1
```

---

## Quick Start

### 1. Configure Project Settings
Navigate to **Project Settings > Plugins > Relationship System** and set your main Relationship Database asset.

### 2. Create Data Assets
Create the following Data Assets:
- **Dimension**: Metrics like "Trust", "Friendship", "Hostility"
- **State**: Relationship stages like "Stranger", "Friend", "Enemy"
- **Profile**: Actor configuration combining dimensions, states, and behaviors

### 3. Add Component to Actors
Add `UNPCRelationshipComponent` to any actor that should participate in the relationship system:
```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Relationships")
UNPCRelationshipComponent* RelationshipComponent;
```

### 4. Assign Profile
In the Details panel, select the appropriate Profile for each actor.

### 5. Create Relationships
Relationships must be explicitly created:
```cpp
// Blueprint or C++
PlayerComponent->CreateRelationshipWith(NPCActor);
```

Or enable auto-creation (see Quick Fix above).

### 6. Modify Relationships
Use Blueprint or C++ to modify relationships:
```cpp
// Blueprint node: Modify Dimension Value
RelationshipComponent->ModifyDimensionValue(TargetActor, "Trust", 10.0f);

// C++
Component->ModifyDimensionValue(TargetActor, FGameplayTag::RequestGameplayTag("Dimension.Trust"), 10.0f);
```

## Architecture

### Core Modules
- **RelationshipSystemCore**: Runtime logic, subsystem, components
- **RelationshipSystemUI**: Base UMG widgets for displaying relationships
- **RelationshipSystemEditor**: Editor tools, validators, asset editors

### Key Classes
- **URelationshipSubsystem**: Global relationship manager (GameInstance subsystem)
- **UNPCRelationshipComponent**: Actor component for relationship participation
- **Data Assets**: Dimension, State, Trait, Rule, Action, Profile
- **Formula Interpreter**: Dynamic rule evaluation

## Data Asset Types

### Dimension (URelationshipDimension)
Defines a metric like Trust, Friendship, or Reputation.
- Min/Max values
- Default value
- Decay rate
- Display name/icon

### State (URelationshipState)
Represents a stage of relationship like Stranger, Friend, Ally, Enemy.
- Entry/exit conditions
- Transitions
- Effects
- Visual representation

### Trait (URelationshipTrait)
Behavioral modifiers like "Forgiving", "Suspicious", "Loyal".
- Dimension modifiers
- Event responses
- Stacking rules

### Profile (URelationshipProfile)
Complete actor configuration.
- Initial dimension values
- Active traits
- Starting state
- Available actions

## Event Integration

The system integrates with external events through:
1. **GameEventBus** (if available)
2. **Internal Delegates** (fallback)
3. **Gameplay Tags** for event identification

Example events:
- `Relationship.DimensionChanged`
- `Relationship.StateTransitioned`
- `Relationship.ActionExecuted`

## Performance Considerations

- **No Tick**: Event-driven, zero per-actor tick cost
- **Indexed Lookups**: O(1) relationship queries
- **Batch Operations**: Grouped modifications for efficiency
- **Lazy Evaluation**: Rules evaluated only when needed
- **Memory Pooling**: Minimal allocations during runtime

## Save/Load System

Relationships are automatically saved with:
- Version tracking
- Deterministic serialization
- Delta compression
- Migration support

## Editor Tools

### Database Viewer
Browse all active relationships in PIE/Standalone.
- Filter by actor
- Search by dimension
- State visualization
- Real-time updates

### Validation System
Automated checks for:
- Data asset integrity
- Circular dependencies
- Invalid formulas
- Missing references

### Hot Reload
Changes to Data Assets are detected and reloaded without editor restart.

## Blueprint API

All core functionality is exposed to Blueprints:
- `GetDimensionValue`
- `ModifyDimensionValue`
- `GetCurrentState`
- `ExecuteAction`
- `QueryRelationships`
- Event callbacks

## Integration with Other Systems

### Dialogue System
The plugin provides soft integration points:
- Event-based relationship modifications from dialogue choices
- Relationship-based dialogue conditions
- No hard dependencies

### Quest System
Similar soft integration:
- Quest rewards can modify relationships
- Relationship states can gate quest availability

## Troubleshooting

### ⚡ Quick Fixes

#### "Dimension Bar показывает 50% при значении 0"
**Причина:** DataAssets используют диапазон -100 до +100, где 0 = середина.

**Решение:** Измените `MinValue` на `0` в DataAssets:
- `DA_Dimension_Trust`
- `DA_Dimension_Friendship`

📖 См. `QUICKFIX_DIMENSION_BAR.md` для деталей.

#### "No relationships visible in UI"

### Problem
Relationship panel shows no dimensions/state during dialogue.

### Root Cause
**Relationship doesn't exist** between Player and NPC.

### Solution

#### Option 1: Auto-create in Dialogue System (Recommended)
Add to `DialogueSubsystem::StartDialogue_Implementation()`:

```cpp
// Auto-create relationship when dialogue starts
if (UNPCRelationshipComponent* PlayerComp = Player->FindComponentByClass<UNPCRelationshipComponent>())
{
    if (!PlayerComp->HasRelationshipWith(NPC))
  {
        PlayerComp->CreateRelationshipWith(NPC);
        UE_LOG(LogDialogueSubsystem, Log, TEXT("Created relationship: %s <-> %s"), 
     *Player->GetName(), *NPC->GetName());
    }
}
```

#### Option 2: Enable Auto-Create on Components
In Player/NPC `NPCRelationshipComponent`:
- Set `bAutoCreateRelationships = true`
- Set `AutoCreateRadius = 5000.0`

#### Option 3: Manual Blueprint Setup
In level BeginPlay:
```
Player->GetComponentByClass(NPCRelationshipComponent)->CreateRelationshipWith(NPC)
```

### Verify
Check Output Log for:
```
[LogRelationshipSystem] Created relationship between bp_MyAnimeCharacter_C_0 and BP_Lianne_C_1
```

---

### Common Issues
1. **Component not registering**: Ensure Profile is assigned in Details panel
2. **Events not firing**: Check GameEventBus integration or delegate binding
3. **Values not persisting**: Verify SaveGame component is active
4. **Hot reload not working**: Restart editor after first Data Asset creation

📖 **Полное руководство:** [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

---

## 📚 Документация

### Быстрый доступ
- **[Quick Fix: 50% Problem](QUICKFIX_DIMENSION_BAR.md)** - Самая частая проблема
- **[Troubleshooting Guide](TROUBLESHOOTING.md)** - Решение всех проблем
- **[Setup Guide](SETUP_ISSUE.md)** - Настройка и интеграция
- **[Cheat Sheet](Documentation/CHEATSHEET.md)** - Краткая справка

### Детальные руководства
- **[Dimension Bar 50% Issue](Documentation/DIMENSION_BAR_50_PERCENT_ISSUE.md)** - Полное объяснение
- **[Negative Values Analysis](Documentation/NEGATIVE_VALUES_ANALYSIS.md)** - Нужны ли отрицательные значения?
- **[Save and Reset Guide](Documentation/SAVE_AND_RESET_GUIDE.md)** - Сохранение и сброс данных
- **[Dialogue Integration and Profiles](Documentation/DIALOGUE_INTEGRATION_AND_PROFILES.md)** - Интеграция с диалогами
- **[Documentation Index](Documentation/INDEX.md)** - Навигация по всей документации

---

## Support & Contribution

For issues, feature requests, or contributions, visit:
https://github.com/shardanow/EchoesOfAffection

## License

[Your License Here]

## Version History

### 1.0.0 (Initial Release)
- Complete data-driven relationship system
- Event-driven architecture
- Save/load support
- Editor tools and validators
- Example content
