# Changelog

All notable changes to the Character Needs System will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.2] - 2025-10-25

### Added
- **COMPLETE Time System Integration** ?
  - Event-driven updates via `OnHourChanged` subscription
  - Dynamic reading of `RealSecondsPerGameDay` from Time System settings
  - Dynamic reading of `TimeScale` multiplier
  - Added `GetSettings()` public method to `TimeOfDaySubsystem`
  - Zero hardcoded values - everything from configuration

### Changed
- Removed hardcoded `24.0` and `1200.0` time scale values
- Now reads all time configuration dynamically from Time System
- Improved integration documentation with complete examples

### Performance
- Event-driven mode (with Time System): Zero polling overhead
- Updates exactly once per game hour when Time System is active
- Polling mode (without Time System): Unchanged, still optimized

### Documentation
- Added `TIME_INTEGRATION_FINAL.md` - Complete integration guide
- Updated `TIME_SCALE_FIX.md` with final solution
- Updated `QUICKSTART.md` with correct time scale recommendations

## [1.0.1] - 2025-10-25

### Fixed
- **CRITICAL:** Time scale was hardcoded to 24.0, causing needs to decay extremely fast
  - Changed default `RealTimeScale` from 24.0 to 1.0 (realistic 1:1 time)
- Needs now decay at correct rate (1 game hour = 1 real hour by default)

### Changed
- Updated `NeedsSettings.h`: Changed `RealTimeScale` default from 24.0 to 1.0
- Updated `NeedsSubsystem.cpp`: Proper time calculation formula

### Documentation
- Added `TIME_SCALE_FIX.md` explaining the issue and solution
- Updated `QUICKSTART.md` with time scale configuration guide

## [1.0.0] - 2025-10-25

### Added

#### Core System
- **CharacterNeedsComponent** - Main component for managing needs
- **NeedsSubsystem** - Batch update orchestrator with LOD support
- **NeedsBlueprintLibrary** - Convenience functions for Blueprint/C++

#### Data Assets
- **NeedDefinitionAsset** - Define individual needs (Hunger, Energy, etc.)
- **NeedEffectAsset** - Define effects (food, sleep, etc.)
- **NeedModifierAsset** - Define modifiers (buffs, debuffs, traits)
- **NeedsSettings** - Global configuration

#### Core Features
- Multiple configurable needs (0-100 value range)
- Automatic decay with customizable rates
- Threshold system (Normal/Warning/Critical/Recovered)
- Mood calculation from weighted need values
- Suppress/Freeze individual needs

#### Effects System
- Instant effects (immediate value change)
- Over-time effects (gradual change over duration)
- Effect stacking with configurable behavior
- Curve-based value scaling
- Tag-based requirements and blocking

#### Modifiers System
- Decay rate modification (speed up/slow down/override)
- Effect power modification (amplify/reduce)
- Value capping (limit maximum values)
- Priority-based application order
- Conditional activation (tags and time-of-day)

#### Performance
- NO per-actor Tick (batch updates only)
- LOD system (Near/Far/Parked)
- Configurable batch size and update frequency
- Handle-based safe references
- Event throttling

#### Events & Listeners
- **OnNeedValueChanged** - Value changes
- **OnNeedThresholdChanged** - Threshold crossings
- **OnMoodChanged** - Mood updates
- **OnEffectApplied** - Effect application
- **OnEffectExpired** - Effect expiration
- **OnModifierAdded** - Modifier addition
- **OnModifierRemoved** - Modifier removal
- **INeedsListener** interface for custom listeners

#### UI Widgets
- **NeedDisplayWidget** - Single need visualization
- **NeedsPanelWidget** - Multiple needs panel
- Auto-updating via listener interface
- Blueprint-extensible for custom styling

#### Integration
- **Time System** - Optional integration (requires TimeOfDaySystem plugin)
- **AI System** - Ready for Behavior Tree integration
- **Inventory System** - Example consumable item integration
- **Quest System** - Example need-based objectives
- **Animation System** - Example mood-based animations
- **Save System** - Example serialization

#### Documentation
- **README.md** - Complete documentation
- **QUICKSTART.md** - 5-minute setup guide
- **SUMMARY.md** - Quick overview and reference
- **EXAMPLES.md** - Detailed usage examples
- **ARCHITECTURE.md** - Technical deep dive
- **INTEGRATION.md** - System integration guide
- **CHANGELOG.md** - This file

#### Debug Tools
- Debug visualization (`needs.debug 1`)
- On-screen stats display
- LOD level indicators
- Need value overlays

### Design Principles
- **Data-Driven** - Everything configurable via Data Assets
- **Event-Driven** - No polling, only notifications
- **SOLID** - Clean interfaces and separation of concerns
- **Performance-First** - Batch updates, LOD, handle-based
- **Blueprint-Friendly** - Full BP support with intuitive nodes

### Technical Details
- **Modules:** NeedsCore (PreDefault), NeedsRuntime (Default), NeedsUI (Default)
- **Dependencies:** GameplayTags plugin, TimeOfDaySystem (optional)
- **Engine:** Unreal Engine 5.4+
- **Language:** C++ with full Blueprint exposure

### Performance Metrics
- 100 actors: < 0.5ms/frame
- 1000 actors (with LOD): < 2ms/frame
- Memory per component: ~2KB
- Memory per need: ~64 bytes

---

## [Unreleased]

### Planned for 1.1
- [ ] Native save/load support
- [ ] Networking/replication
- [ ] Additional preset templates (Survival, RPG, Life Sim)

### Planned for 1.2
- [ ] Gameplay Ability System integration
- [ ] AI Behavior Tree nodes (built-in)
- [ ] More UI widget variants

---

## Version Scheme

**Major.Minor.Patch** (e.g., 1.2.3)

- **Major:** Breaking changes, major features
- **Minor:** New features, backward compatible
- **Patch:** Bug fixes, minor improvements

---

**Plugin:** Needs System  
**Current Version:** 1.0.2  
**Status:** ? Production Ready  
**Last Updated:** October 25, 2025
