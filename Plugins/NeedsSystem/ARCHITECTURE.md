# Needs System - Architecture Overview

## Design Principles

### 1. Data-Driven Design
**Всё через Data Assets** - никакой hardcode логики.

- ? Need Definitions ? Data Assets
- ? Effects ? Data Assets
- ? Modifiers ? Data Assets
- ? Global Settings ? Data Asset
- ? Curves ? Runtime Float Curves

**Result:** Дизайнеры могут менять баланс без программистов.

---

### 2. Event-Driven Architecture
**Нет Tick() на акторах** - всё через события и батч-обновления.

```
Subsystem Tick (batched)
    ??> Update Components (LOD-based)
   ??> Calculate Changes
          ??> Broadcast Events
   ??> Listeners React
```

**Result:** Масштабируемость и производительность.

---

### 3. SOLID Principles

**Single Responsibility:**
- Component = needs management
- Subsystem = orchestration
- Data Assets = configuration

**Open/Closed:**
- Extensible через Data Assets
- Не нужно менять код для новых needs

**Liskov Substitution:**
- Интерфейсы (Manager, Listener, Accessor, Mutator)
- Можно заменить реализации

**Interface Segregation:**
- 4 отдельных интерфейса вместо одного большого
- Каждый делает одно

**Dependency Inversion:**
- Зависимости через интерфейсы
- Никаких прямых ссылок на конкретные классы

---

## Module Structure

```
NeedsSystem (Plugin)
??? NeedsCore (PreDefault)
?   ??? Types
?   ?   ??? NeedsTypes.h (Structs, Enums)
?   ?   ??? NeedsInterface.h (4 Interfaces)
?   ??? Data Assets
?       ??? NeedDefinitionAsset
?  ??? NeedEffectAsset
?       ??? NeedModifierAsset
? ??? NeedsSettings
?
??? NeedsRuntime (Default)
?   ??? Components
?   ?   ??? CharacterNeedsComponent
?   ??? Subsystems
?   ?   ??? NeedsSubsystem
?   ??? Library
?  ??? NeedsBlueprintLibrary
?
??? NeedsUI (Default)
    ??? Widgets
        ??? NeedDisplayWidget
        ??? NeedsPanelWidget
```

---

## Core Types

### FNeedHandle
Безопасная ссылка на потребность (аналог слабого указателя).

```cpp
struct FNeedHandle
{
 FGameplayTag NeedTag;     // Идентификатор
    uint32 OwnerID;      // Валидация владельца
};
```

### FNeedRuntimeState
Текущее состояние потребности.

```cpp
struct FNeedRuntimeState
{
  float CurrentValue;            // 0..100
    bool bIsSuppressed;        // Не деградирует
    bool bIsFrozen;       // Вообще не меняется
    ENeedThresholdLevel CurrentThreshold;  // Normal/Warning/Critical
};
```

### FActiveNeedEffect
Активный эффект (runtime instance).

```cpp
struct FActiveNeedEffect
{
    UNeedEffectAsset* EffectAsset;
    uint32 InstanceID;
    float RemainingDuration;    // Для OverTime
    int32 StackCount;
};
```

### FActiveNeedModifier
Активный модификатор (runtime instance).

```cpp
struct FActiveNeedModifier
{
    UNeedModifierAsset* ModifierAsset;
    uint32 InstanceID;
    int32 Priority;     // Порядок применения
    bool bIsActive;        // Условия выполнены?
};
```

---

## Interfaces

### INeedsManager (Subsystem)
Центральный оркестратор.

```cpp
interface INeedsManager
{
    float GetCurrentGameTimeHours();
    void RegisterNeedsComponent(Component);
    void UnregisterNeedsComponent(Component);
    FVector GetCameraLocation();  // Для LOD
};
```

### INeedsAccessor (Component - Read)
Только чтение состояния.

```cpp
interface INeedsAccessor
{
    float GetNeedValue(Tag);
    FNeedHandle GetNeedHandle(Tag);
    float GetCurrentMood();
    ENeedThresholdLevel GetNeedThreshold(Tag);
};
```

### INeedsMutator (Component - Write)
Изменение состояния.

```cpp
interface INeedsMutator
{
    void SetNeedValue(Tag, Value);
    void ModifyNeedValue(Tag, Delta);
    void ApplyEffect(Effect, StackCount);
    void AddModifier(Modifier);
    void SuppressNeed(Tag, bSuppress);
    void RegisterListener(Listener);
};
```

### INeedsListener (External)
Получение событий.

```cpp
interface INeedsListener
{
    void OnNeedValueChanged(Handle, Old, New);
    void OnNeedThresholdChanged(Handle, OldLevel, NewLevel);
    void OnMoodChanged(Old, New);
 void OnEffectApplied(Effect);
    void OnEffectExpired(Effect);
    void OnModifierAdded(Modifier);
    void OnModifierRemoved(Modifier);
};
```

---

## Component Flow

### Initialization
```
BeginPlay()
  ??> Initialize Needs (from DataAssets or Settings)
  ??> Calculate Initial Mood
  ??> Register with Subsystem
```

### Update Cycle (Called by Subsystem)
```
UpdateNeeds(DeltaGameHours, LODLevel)
  ??> Update Modifiers (check conditions)
  ??> For Each Need:
  ?   ??> Apply Decay (modified by modifiers)
  ?   ??> Check Threshold
  ?   ??> Broadcast Events (if changed)
  ??> Update Active Effects (OverTime)
  ??> Update Mood
```

### Effect Application
```
ApplyEffect(EffectAsset, StackCount)
  ??> Check Tags (required/blocked)
  ??> Check if Already Active
  ?   ??> Stack (if allowed)
  ?   ??> Refresh Duration
  ??> If Instant:
  ?   ??> Apply Immediately
  ??> If OverTime:
  ??> Add to ActiveEffects[]
```

### Modifier Application
```
AddModifier(ModifierAsset)
  ??> Check Conditions (tags, time)
  ??> Add to ActiveModifiers[]
  ??> Sort by Priority
  ??> Update Gameplay Tags
```

---

## Subsystem Flow

### Batch Update Strategy
```
Tick(DeltaTime)
  ??> Update Game Time (from Time System or real time)
  ??> Accumulate Time (based on UpdateFrequency)
  ??> When Threshold Reached:
      ??> Calculate Batch Size
      ??> For Each Component in Batch:
      ?   ??> Calculate LOD (distance to camera)
      ?   ??> Get LOD Multiplier
      ?   ??> UpdateNeeds(DeltaTime * Multiplier, LOD)
      ??> Advance Batch Index
```

### LOD Calculation
```
CalculateLOD(Component)
  ??> Get Component Location
  ??> Get Camera Location
  ??> Calculate Distance
  ??> Return LOD Level:
      ??> < FarDistance ? Near (1.0x)
  ??> < ParkedDistance ? Far (0.5x)
    ??> >= ParkedDistance ? Parked (0.1x)
```

---

## Data Asset Pipeline

### Need Definition ? Runtime
```
UNeedDefinitionAsset (Data)
  ??> Tag
  ??> Display Info
  ??> Decay Config
  ??> Threshold Config
  ??> Mood Weight
      ?
FNeedData (Runtime)
  ??> Definition (reference)
  ??> State (runtime)
```

### Effect ? Application
```
UNeedEffectAsset (Data)
  ??> Type (Instant/OverTime)
  ??> Modifications[]
  ??> Tags (required/blocked/granted)
      ?
FActiveNeedEffect (Runtime)
  ??> EffectAsset (reference)
  ??> InstanceID
  ??> RemainingDuration
  ??> StackCount
```

### Modifier ? Behavior Change
```
UNeedModifierAsset (Data)
  ??> Priority
  ??> Conditions
  ??> Modifications[]
      ?
FActiveNeedModifier (Runtime)
  ??> ModifierAsset (reference)
  ??> InstanceID
  ??> Priority
  ??> bIsActive
      ?
Applied During Update:
  ??> Decay Rate Modification
  ??> Effect Power Modification
  ??> Value Cap
```

---

## Event Flow

### Value Change
```
Need Value Changes
  ??> Check Threshold (evaluate rules)
  ??> If Threshold Changed:
  ?   ??> Broadcast OnNeedThresholdChanged
  ??> Broadcast OnNeedValueChanged
  ??> Trigger Mood Recalculation
  ??> Notify Listeners:
  ??> Component Multicast Delegates
      ??> Registered INeedsListener interfaces
```

### Mood Calculation
```
UpdateMood()
  ??> For Each Need:
  ?   ??> Get Current Value
  ?   ??> Calculate Contribution (curve or linear)
  ? ??> Multiply by Weight
  ??> Calculate Weighted Average
  ??> Apply Optional Mood Curve
  ??> If Changed > Threshold:
  ?   ??> Broadcast OnMoodChanged
  ??> Update UI (via listeners)
```

---

## Performance Optimizations

### 1. No Per-Actor Tick
- ? Component->Tick()
- ? Subsystem batch update

### 2. LOD System
- Near: Full simulation (1.0x)
- Far: Half speed (0.5x)
- Parked: Minimal (0.1x)

### 3. Batch Processing
- Update N components per frame
- Distribute load over time
- Configurable batch size

### 4. Event Throttling
- Mood change threshold
- Prevent event spam
- Delta-based updates

### 5. Handle-Based References
- No direct pointers
- Safe for networking
- Validation via OwnerID

---

## Extension Points

### Add New Need Type
1. Create `UNeedDefinitionAsset`
2. Add to `UNeedsSettings`
3. Done! (no code changes)

### Add Custom Effect
1. Create `UNeedEffectAsset`
2. Configure modifications
3. Apply via Blueprint/C++

### Add Custom Modifier
1. Create `UNeedModifierAsset`
2. Set conditions
3. Configure behavior modifications

### Custom UI
1. Inherit from `UNeedDisplayWidget`
2. Override `BP_OnValueChanged`
3. Custom styling in UMG

### Custom Listener
1. Implement `INeedsListener`
2. Register with component
3. React to events

---

## Integration Points

### Time System (Optional)
```
TimeOfDaySubsystem
  ??> OnHourChanged Event
      ??> NeedsSubsystem
??> Update CurrentGameTimeHours
```

### Gameplay Abilities (Future)
```
AbilitySystemComponent
  ??> Gameplay Effects
      ??> Trigger Need Effects
```

### Animation (Future)
```
Mood Value
  ??> Animation Blueprint
      ??> Blend Spaces (Happy/Sad)
```

---

## Testing Strategy

### Unit Tests (Future)
- FNeedRuntimeState operations
- Threshold evaluation
- Mood calculation
- Effect stacking

### Integration Tests
- Component lifecycle
- Subsystem registration
- Event broadcasting
- LOD transitions

### Performance Tests
- 100 actors
- 1000 actors
- LOD effectiveness
- Batch update timing

---

## Future Enhancements

### Version 1.1 (Planned)
- [ ] Networking support (replication)
- [ ] Save/Load helpers
- [ ] Advanced curves (time-of-day based decay)
- [ ] Need dependencies (hunger affects energy)
- [ ] Buff/Debuff visual indicators
- [ ] Analytics integration

### Version 2.0 (Ideas)
- [ ] AI integration (behavior tree decorators)
- [ ] Gameplay Ability System integration
- [ ] Automated balancing tools
- [ ] Preset templates (Survival, RPG, Life Sim)
- [ ] Multiplayer optimizations

---

## Summary

**What We Built:**
? Production-ready needs system
? Fully data-driven
? Event-driven architecture
? LOD optimization
? Modular (Core/Runtime/UI)
? Blueprint friendly
? Extensible without code changes
? Clean API with SOLID principles

**Key Features:**
- ?? Any need type (Hunger, Energy, Social, etc.)
- ?? Effects (instant or over time)
- ??? Modifiers (decay, caps, power)
- ?? Mood calculation
- ?? LOD system
- ? Batch updates
- ?? Event system
- ?? Time integration
- ?? Handle-based references
- ?? UI widgets

**Perfect For:**
- RPG games
- Life simulation
- Survival games
- Character-driven narratives
- Any game with living NPCs

---

**Ready for production! ??**
