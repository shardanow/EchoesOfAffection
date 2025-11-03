# Relationship System - Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-01-02

### Added

#### Core Systems
- **RelationshipSubsystem**: GameInstance subsystem for centralized relationship management
  - O(1) relationship lookups using hash maps
  - Event-driven architecture with zero per-actor tick overhead
  - Support for unlimited actor relationships
- Batch dimension modification operations
  - Integrated decay system with configurable intervals

- **RelationshipComponent**: ActorComponent for relationship participation
  - Auto-registration with subsystem
  - Blueprint-friendly API for all operations
  - Optional auto-creation of nearby relationships
  - Cached subsystem and profile references for performance
  - Query methods for finding relationships by state/dimension

#### Data Asset System
- **RelationshipDimension**: Configurable metrics (Trust, Friendship, etc.)
  - Min/max bounds with clamping
  - Default values and normalization
  - Decay system with optional curves
  - Per-dimension decay rates and targets

- **RelationshipState**: Relationship stages (Stranger, Friend, Enemy, etc.)
  - Priority-based transition system
  - Hysteresis support for stable transitions
  - State-specific available actions
  - Auto-applied traits per state
  - Dimension modifiers while in state

- **RelationshipTrait**: Behavioral modifiers (Forgiving, Suspicious, etc.)
  - Stackable traits with max stack limits
  - Dimension modifiers (additive and multiplicative)
  - Event-based responses with cooldowns
  - Conflict and requirement systems

- **RelationshipRule**: Conditional logic system
  - Multi-condition evaluation (AND logic)
  - Formula interpreter with mathematical expressions
  - Support for dimension comparisons
  - Used for state transitions and action gates

- **RelationshipAction**: Executable interactions (Talk, Gift, etc.)
  - Multiple effects with variance and success probability
  - Cost system (dimension requirements and consumption)
  - Cooldowns and max usage limits
  - Triggered events for external integration

- **RelationshipProfile**: Actor configuration presets
  - Initial dimension values
  - Starting state and traits
  - Decay rate overrides
  - Subject/target capability flags
  - Persistence settings

- **RelationshipDatabase**: Central registry
  - Indexed access to all data assets
  - O(1) lookups by Gameplay Tag
  - Hot-reload support
  - Validation system

#### Formula Interpreter
- Mathematical expression evaluation (e.g., "Trust * 0.5 + Friendship - 10")
- Shunting-yard algorithm for infix to postfix conversion
- Support for: +, -, *, /, parentheses
- Dynamic dimension value substitution
- Syntax validation with error reporting

#### Save/Load System
- **URelationshipSaveGame**: Versioned save format
  - Deterministic serialization
  - Migration support for format changes
  - Validation on load
  - Timestamp and metadata tracking

- **URelationshipSaveGameHelper**: Blueprint-friendly save/load
  - Named save slots
  - Save existence checks
  - Automatic actor name resolution
  - Slot management utilities

#### Project Settings
- **URelationshipSystemSettings**: Editor-accessible configuration
  - Database asset reference
  - Automatic decay settings
  - Debug options
  - Registered in Project Settings > Plugins > Relationship System

#### UI System
- **URelationshipWidgetBase**: Base class for all relationship widgets
  - Auto-refresh system with configurable intervals
  - Actor and component caching
  - Blueprint event integration

- **URelationshipDimensionBar**: Progress bar for single dimension
  - Automatic normalization support
  - Optional numeric value display
  - Optional dimension name label
  - Widget binding support (ProgressBar, ValueText, NameText)

- **URelationshipPanel**: Multi-dimension display panel
  - Automatic dimension bar creation
  - Filterable (show all or non-zero only)
  - State display
  - Dynamic widget spawning

#### Editor Tools
- Project Settings integration
- Data Asset validation system
- Hot-reload support for data assets
- Editor module with proper registration/unregistration

#### Documentation
- Comprehensive README with quick start guide
- Integration guide for Dialogue, Quest, and Combat systems
- Complete Blueprint API reference
- Example content and configurations
- Gameplay Tag examples
- Best practices and performance tips

### Features

#### Performance Optimizations
- No tick-based updates (100% event-driven)
- O(1) relationship lookups via hash maps
- Indexed data asset access
- Component reference caching
- Batch operation support
- Lazy rule evaluation

#### Designer-Friendly
- Complete Blueprint API exposure
- Data-driven configuration (no code required)
- Visual state machine through data assets
- Formula editor for complex conditions
- Hot-reload for rapid iteration
- Validation with clear error messages

#### Integration
- Soft dependency on GameEventBus (optional)
- Gameplay Tag system integration
- SaveGame compatibility
- UMG widget system
- Event broadcasting for external systems
- No hard dependencies on other gameplay systems

#### Extensibility
- Virtual functions for customization
- Blueprint implementable events
- Modular architecture
- SOLID principles adherence
- Plugin-based distribution

### Technical Details

#### Modules
- **RelationshipSystemCore**: Runtime logic (C++14)
  - Subsystem, Component, Data Assets
  - Save/Load, Settings
  - Formula interpreter
  
- **RelationshipSystemUI**: UMG widgets
  - Base widget classes
  - Dimension bars, panels
  - Blueprint-extensible

- **RelationshipSystemEditor**: Editor tools
  - Project Settings registration
  - Asset validation
  - Future: Custom editors, visualizers

#### Dependencies
- Core
- CoreUObject
- Engine
- GameplayTags
- DeveloperSettings
- UMG, Slate, SlateCore (UI module)
- Json, JsonUtilities (serialization)

#### Platform Support
- Windows (Win64)
- Mac
- Linux
- Android
- iOS

### Examples

- Sample Gameplay Tags configuration
- Example Data Asset setups
- Integration code snippets
- Blueprint usage patterns
- Common gameplay scenarios

### Known Limitations

- SaveGame serialization requires actors to have stable names for restoration
- No built-in visual debugging tools (planned for future release)
- Formula interpreter limited to basic arithmetic (no functions like sin/cos/pow)

### Migration Guide

N/A - Initial release

---

## [Unreleased]

### Planned Features
- Visual state machine editor
- In-game relationship debugger widget
- Advanced formula functions (math, string operations)
- Relationship templates and inheritance
- Analytics and telemetry hooks
- Localization support for display names
- Advanced decay curves (exponential, logarithmic)
- Relationship groups and factions
- Reputation propagation system

---

## Development Notes

**Engine Version**: Unreal Engine 5.6  
**Language**: C++14  
**License**: [Your License]  
**Repository**: https://github.com/shardanow/EchoesOfAffection

### Contributing

We welcome contributions! Please follow these guidelines:
1. Follow Epic's coding standards
2. Maintain SOLID principles
3. Add documentation for public APIs
4. Include examples for new features
5. Test on all supported platforms

### Support

For issues, feature requests, or questions:
- GitHub Issues: https://github.com/shardanow/EchoesOfAffection/issues
- Documentation: See Plugins/RelationshipSystem/Docs/

---

[1.0.0]: https://github.com/shardanow/EchoesOfAffection/releases/tag/v1.0.0
