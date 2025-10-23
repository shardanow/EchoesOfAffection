# Changelog

All notable changes to the Echoes of Affection project and Dialogue System Plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added - Dialogue System Plugin v1.11

#### Core Systems
- **DLS Parser**: Complete dialogue scripting language with conditions, effects, and branching
- **Memory System**: NPC memory with decay, strength, importance, and emotional context
  - Memory creation with custom descriptions and context tags
  - Memory querying by tags and time ranges
  - Memory strength modification and refresh
- Automatic memory decay based on importance
- **Condition System**: Rich condition evaluation with variable comparisons, memory checks, tag queries
- **Effect System**: Comprehensive effect execution with:
  - Variable manipulation (Set, Modify, Toggle)
  - Memory effects (Create, Remove, Refresh, Modify Strength)
  - Relation effects (Set, Modify)
  - Quest effects
- **Command Pattern**: Full undo/redo support for dialogue state
- **State Machine**: Robust state management with validation and transitions
- **Variable Store**: Type-safe variable storage with Int, Float, Bool, String, Name types
- **Dialogue Runner**: Complete dialogue execution engine with participant management
- **Serialization**: Save/Load dialogue state and NPC memories

#### Component Systems
- **DialogueComponent**: Main component for dialogue management
  - Dialogue initiation and control
  - Event system integration
  - Participant management
- **DialogueInputBlockerComponent** (v1.11): Advanced input control
  - Enhanced Input System support with priority-based mapping contexts
  - Legacy input system support
  - Multiple blocking modes (BlockAll, BlockMovement, BlockCamera, Custom)
  - Pawn movement and rotation control
  - Pawn visibility control
  - Mouse cursor and UI interaction support
  - Input mode management (Game/UI/GameAndUI)
  - State restoration with cleanup
- **DialogueCameraComponent**: Cinematic camera control
  - Camera positioning and blending
  - Look-at NPC functionality
  - Camera settings (FOV, blend time, offset)
- **DialoguePresentationComponent**: High-level presentation coordination
  - Multiple presentation modes (Cinematic, Interactive, Hybrid, FirstPerson)
  - Integration of camera and input systems
  - Time dilation and game pause support
- **NPCMemoryComponent**: NPC memory persistence
  - Memory storage and retrieval
  - Memory decay simulation
  - Integration with dialogue system

#### UI System
- **DialogueWidget**: Base UI widget for dialogue display
  - Text display and formatting
  - Choice presentation
  - Event-driven architecture

#### AI Integration
- **AIDialogueProvider**: AI behavior tree integration
  - Dialogue initiation from AI
  - Condition evaluation in behavior trees

#### Subsystems
- **DialogueSubsystem**: World subsystem for dialogue management
  - Centralized dialogue control
  - Multiple dialogue session support
  - Event broadcasting

### Changed

#### Input System Improvements (v1.6 ? v1.11)
- **v1.6**: Initial input blocking implementation
- **v1.8**: Added multiple blocking modes and pawn control
- **v1.9**: Fixed false positive errors in enhanced input detection
- **v1.10**: Added mouse cursor and UI interaction support
- **v1.11**: **CRITICAL FIX** - Complete input focus restoration
  - Fixed input not working after dialogue ends
  - Proper Enhanced Input mapping context management (add/remove instead of clear/restore)
  - Input mode reset to Game Only on dialogue end
  - Player input flush on unblock
  - Proper state validation and logging

#### Memory System Evolution (v1.3 ? v1.5)
- **v1.3**: Initial memory system with basic storage
- **v1.5**: Complete memory system with:
  - Emotional context
  - Importance levels
  - Decay simulation
  - Memory effects integration
  - Context tags for filtering

#### Context System (v1.3.1)
- Refactored context to use Players/NPCs arrays instead of generic Participants
- Added type-safe context access methods
- Improved context tag support

### Fixed

#### Critical Fixes
- **Input Focus Restoration (v1.11)**: Fixed input not responding after dialogue ends
  - Root cause: Enhanced Input contexts were being cleared instead of managed
  - Solution: Priority-based context overlay system
  - Impact: Players can now move/look immediately after dialogue
- **Enhanced Input Detection (v1.9)**: Fixed false positives in enhanced input availability check
- **End Node State (v1.3)**: Fixed dialogue not properly ending on final node
- **Memory Storage (v1.5)**: Verified memory persistence and retrieval
- **Camera Blocker Compatibility**: Fixed camera component working with input blocker
- **NPC Input Control**: Fixed input blocker working when component is on NPC

### Documentation

#### Comprehensive Guides
- **Architecture Documentation**:
  - `Architecture_Analysis_And_Recommendations.md`
  - `Architecture_Diagrams.md`
  - `Complete_Architecture_Overview.md`
  - `System_Workflows_And_Integration.md`
  - `Scalability_And_Future_Enhancements.md`

- **Memory System**:
  - `Full_Memory_System_Guide.md`
  - `Memory_Effects_Implementation_Guide.md`
  - `Memory_Decay_Design_Rationale.md`
  - `Memory_System_Creation_And_Time.md`
  - `Memory_Storage_Verification.md`

- **Input System**:
  - `Input_Blocker_v1.8_Quick_Reference.md`
  - `Input_Focus_Fix_v1.11.md`
  - `UnblockInput_Troubleshooting.md`
  - `Using_Input_Blocker_From_NPC.md`

- **Camera System**:
  - `DialogueCamera_QuickStart.md`
  - `Mouse_Cursor_Support_v1.10.md`
- `Show_Cursor_QuickStart.md`

- **Events System**:
  - `06_DialogueEvents_Guide.md`
  - `06_DialogueEvents_QuickStart.md`
  - `07_Player_Dialogue_Events_Guide.md`
  - `DialogueEvents_CheatSheet.md`
  - `Dialogue_Events_Architecture.md`
  - `Player_Events_QuickRef.md`

- **Command Pattern**:
  - `Command_Pattern_QuickRef.md`
  - `Command_Pattern_QuickStart.md`
  - `Command_Pattern_Completion_Report.md`
  - `Commands_Snapshots_Complete.md`

- **State Machine**:
  - `StateMachine_Design_v1.3.md`
  - `StateMachine_Integration_Complete.md`
  - `FullUndo_Design_v1.3.md`

- **Version-Specific**:
  - `Context_Tags_Support_v1.6.md`
  - `Memory_Effects_v1.5_Summary.md`
  - `v1.3.1_Context_Refactoring_Guide.md`
  - `v1.3_Roadmap.md`

- **Bugfixes**:
  - `Bugfix_End_Node_State_v1.3.md`
  - `False_Positive_Error_Fix_v1.9.md`
  - `Input_Blocker_Fix_v1.6.md`

#### Code Examples
- `Player_Dialogue_Events_Example.cpp`: Complete player event integration example

### Project Structure

#### Content
- **Game/Characters**: Player and NPC character blueprints
  - Player: `bp_MyAnimeCharacter`
  - NPC: Lianne character with test actors
- **Game/Data**: Data tables and configuration assets
- **Game/Dialogue**: Dialogue data assets and scripts
- **Game/UI**: UI widgets for dialogue system
- **Game/World/Maps**: Village level with dialogue integration

#### Plugin Structure
```
Plugins/DialogueSystem/
??? Content/           # Plugin content assets
??? Docs/     # Comprehensive documentation
??? Source/
?   ??? DialogueSystemCore/      # Core dialogue logic (C++)
?   ?   ??? Commands/       # Command pattern implementation
?   ?   ??? Conditions/      # Condition evaluation
?   ?   ??? Effects/ # Effect execution
?   ?   ??? Serialization/       # Save/Load system
?   ?   ??? Core/     # Core classes (Runner, Context, Node, etc.)
?   ??? DialogueSystemRuntime/   # Runtime components (C++)
?   ?   ??? Components/          # Actor components
?   ?   ??? Subsystems/         # Game subsystems
?   ??? DialogueSystemUI/        # UI widgets (C++)
?   ??? DialogueSystemAI/        # AI integration (C++)
```

### Technical Details

#### Supported Engines
- Unreal Engine 5.6 (primary)
- Compatible with UE 5.3+

#### Supported Input Systems
- Enhanced Input System (recommended)
- Legacy Input System (fallback)

#### Dependencies
- GameplayTags module
- Enhanced Input plugin
- UMG module
- AI Module

## [0.2.0] - 2025-10-20 (c73ae94)

### Added
- Complete Dialogue System Plugin with all features listed above
- DLS parser with full scripting support
- Memory system with decay and emotions
- Command pattern with undo/redo
- State machine architecture
- Input blocker with Enhanced Input support (v1.11)
- Camera system for cinematic dialogues
- Presentation component for high-level control
- Comprehensive documentation (50+ guides)

### Changed
- Refactored input system to use priority-based context overlay (v1.11)
- Improved context system with type-safe access
- Enhanced memory system with emotional context and importance

### Fixed
- Critical input focus restoration bug (v1.11)
- Enhanced Input false positive detection (v1.9)
- End node state handling (v1.3)
- Various stability improvements

## [0.1.0] - 2025-10-19 (ee8820d)

### Added
- Basic dialogue/relation system as plugin solution
- Initial DialogueComponent
- Basic DialogueRunner
- Simple UI widget
- Initial documentation

## [0.0.1] - 2025-10-19 (df23057)

### Added
- Base project structure
- Unreal Engine 5.6 project setup
- Plugin folder structure
- Initial README

---

## Version History Summary

- **v1.11** (Current): Input focus restoration fix - CRITICAL UPDATE
- **v1.10**: Mouse cursor and UI interaction support
- **v1.9**: Enhanced Input false positive fix
- **v1.8**: Multiple input blocking modes
- **v1.6**: Initial input blocker and context tags
- **v1.5**: Complete memory system with emotions and decay
- **v1.3.1**: Context refactoring
- **v1.3**: State machine and command pattern
- **v1.0**: Initial release

## Known Issues

None currently. See [GitHub Issues](https://github.com/shardanow/EchoesOfAffection/issues) for tracking.

## Upgrade Notes

### From v1.10 to v1.11
**CRITICAL UPDATE - Fixes input not working after dialogue**

If you're experiencing input not working after dialogue ends:
1. Update DialogueInputBlockerComponent to v1.11
2. Test input focus restoration
3. See `Input_Focus_Fix_v1.11.md` for details

No breaking changes to API.

### From v1.5 to v1.6+
- Context now uses Players/NPCs arrays instead of Participants
- Update custom dialogue scripts to use GetPlayer()/GetNPC()
- See `v1.3.1_Context_Refactoring_Guide.md`

## Contributing

This is a personal project, but feedback and suggestions are welcome!
Please open an issue on GitHub: https://github.com/shardanow/EchoesOfAffection/issues

## License

Copyright Epic Games, Inc. All Rights Reserved.

---

**Project**: Echoes of Affection  
**Repository**: https://github.com/shardanow/EchoesOfAffection  
**Author**: Shardanow  
**Engine**: Unreal Engine 5.6
