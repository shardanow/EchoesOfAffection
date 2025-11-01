# ?? Time System & Needs System - GameEventBus Integration Plan

## Current Status

### ? Already Integrated:
- Inventory System
- Quest System  
- Dialogue System

### ?? Missing Integration:
- **TimeOfDay System** - No GameEventBus emissions
- **Needs System** - No GameEventBus emissions

---

## TimeOfDay System Integration

### Events to Emit:

1. **`Time.Event.HourChanged`** ? Emitted every game hour
   - IntParam: New hour (0-23)
   - Use case: Quest "Wait until 14:00"

2. **`Time.Event.DayChanged`** ? Emitted when new day starts
   - IntParam: New day number
   - Use case: Quest "Wait 3 days"

3. **`Time.Event.SeasonChanged`** ? Emitted when season changes
 - StringParam: New season name
   - Use case: Quest "Collect mushrooms in Autumn"

4. **`Time.Event.Sunrise`** ? Emitted at sunrise
   - Use case: Quest "Meet at sunrise"

5. **`Time.Event.Sunset`** ? Emitted at sunset
   - Use case: Quest "Return home before sunset"

### Where to Add:

**File:** `TimeOfDaySubsystem.cpp`

**Methods:**
- `ProcessTimeChanges()` ? Add emissions after each change
- Already has delegates: `OnHourChanged`, `OnDayChanged`, `OnSeasonChanged`, `OnSunrise`, `OnSunset`

**Implementation:**
```cpp
// In ProcessTimeChanges():
if (CurrentTime.Hour != PreviousHour)
{
    // Existing code...
    OnHourChanged.Broadcast(CurrentTime);
    
    // ?? NEW: Emit GameEventBus event
    #if GAMEEVENTBUS_AVAILABLE
        UTimeSystemGameEventEmitter::EmitHourChangedEvent(this, CurrentTime.Hour);
    #endif
}
```

---

## Needs System Integration

### Events to Emit:

1. **`Needs.Event.NeedCritical`** ? Emitted when need reaches critical level
   - StringParam: Need name (Hunger, Thirst, Sleep)
   - FloatParam: Current value
   - Use case: Quest "Eat something before you starve"

2. **`Needs.Event.NeedWarning`** ? Emitted when need reaches warning level
   - Use case: Trigger dialogue "You look tired"

3. **`Needs.Event.NeedSatisfied`** ? Emitted when critical/warning need is satisfied
   - Use case: Quest complete "Rest well"

### Where to Add:

**File:** `CharacterNeedsComponent.cpp`

**Methods:**
- `UpdateNeed()` ? Check threshold changes, emit events

**Implementation:**
```cpp
void UCharacterNeedsComponent::UpdateNeed(FGameplayTag NeedTag, float DeltaValue)
{
    // Existing code...
    
    ENeedThresholdLevel OldThreshold = GetNeedThreshold(NeedTag);
    
  // Update value...
 CurrentValue += DeltaValue;
    
    ENeedThresholdLevel NewThreshold = GetNeedThreshold(NeedTag);
    
    // ?? NEW: Emit event if threshold changed
    #if GAMEEVENTBUS_AVAILABLE
        if (NewThreshold != OldThreshold)
        {
         if (NewThreshold == ENeedThresholdLevel::Critical)
      {
      UNeedsGameEventBridge::EmitNeedCriticalEvent(this, NeedTag, CurrentValue);
 }
     else if (OldThreshold == ENeedThresholdLevel::Critical)
     {
 UNeedsGameEventBridge::EmitNeedSatisfiedEvent(this, NeedTag, CurrentValue);
}
        }
    #endif
}
```

---

## Implementation Plan

### Step 1: TimeOfDay System ? (Recommended)
**Why:** Already has all delegates, just need to emit events  
**Effort:** ~30 minutes  
**Impact:** HIGH - Allows time-based quests

### Step 2: Needs System ?? (Optional)
**Why:** Needs more complex logic (threshold detection)  
**Effort:** ~1 hour  
**Impact:** MEDIUM - Allows survival/needs-based quests

---

## Decision

### Recommendation: Implement TimeOfDay System ONLY

**Reasons:**
1. TimeOfDay already has all event infrastructure
2. High value for quest system (time-based quests are common)
3. Minimal code changes needed
4. Needs System events are less common in quests

### Files to Modify:

1. **Create Helper:**
   - `TimeSystemGameEventEmitter.h` ? Already exists in GameEventBus!
   - `TimeSystemGameEventEmitter.cpp` ? Already exists!

2. **Modify TimeOfDaySubsystem:**
   - Add include for TimeSystemGameEventEmitter
   - Add emissions in `ProcessTimeChanges()`

3. **Update Build.cs:**
   - `TimeOfDayRuntime.Build.cs` ? Optional GameEventBus dependency

---

## Next Steps (FOR YOU)

**Do you want me to:**
1. ? Implement TimeOfDay System integration (RECOMMENDED)
2. ?? Also implement Needs System integration (OPTIONAL)
3. ? Skip both (not recommended - limits quest functionality)

**Let me know and I'll implement it!** ??
