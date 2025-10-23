# State Machine Design (v1.3)

> **Version:** 1.3.0  
> **Feature:** State Machine для DialogueRunner  
> **Status:** ?? In Design

---

## ?? Goals

1. **Clear States** - Explicit dialogue states
2. **Safe Transitions** - Prevent invalid operations
3. **Better Debugging** - Easy to track dialogue flow
4. **Thread Safety** - Atomic state changes

---

## ?? State Diagram

```
         ???????????
      ?  Idle   ?
      ???????????
      ? StartDialogue()
       ?
    ???????????
          ??????????? Loading ?
     ?         ???????????
          ?      ? Asset Loaded
          ?              ?
    ?         ???????????
  ?         ? Active  ?????????
          ? ???????????       ?
   ?              ?   ?
          ?     ?? Pause() ????? ??????????
   ?     ?            ????? Paused ?
        ?        ?      ??????????
      ?          ? ?
          ?        ?    Resume()
   ?    ?      ?
          ?      ?              ?
    ?         ??????????????        ?
  ?         ?Transitioning??????????????
          ?         ??????????????
          ?   ? Node Changed
          ?      ????????????????????
    ?  ?
          ?    ?
          ?           ??????????
          ??????????????????????????????? Ended  ?
           ??????????
    ?
       ? Cleanup
     ?
          ??????????
       ?  Idle  ?
       ??????????
```

---

## ?? Implementation

### 1. Dialogue States Enum

```cpp
UENUM(BlueprintType)
enum class EDialogueState : uint8
{
    /** Not running */
    Idle       UMETA(DisplayName = "Idle"),
    
    /** Loading dialogue asset */
    Loading       UMETA(DisplayName = "Loading"),
    
    /** Dialogue is active and running */
    Active UMETA(DisplayName = "Active"),
    
    /** Temporarily paused */
    Paused          UMETA(DisplayName = "Paused"),
    
    /** Transitioning between nodes */
    Transitioning   UMETA(DisplayName = "Transitioning"),
  
    /** Dialogue has ended */
    Ended           UMETA(DisplayName = "Ended"),
    
    /** Error state */
    Error           UMETA(DisplayName = "Error")
};
```

### 2. State Transition Rules

```cpp
// Valid transitions
Idle -> Loading -> Active
Active <-> Paused
Active -> Transitioning -> Active
Active -> Ended -> Idle
Any -> Error

// Invalid transitions (will be blocked)
Loading -> Paused  ?
Ended -> Active    ?
Transitioning -> Loading ?
```

### 3. State Machine Class

```cpp
UCLASS()
class UDialogueStateMachine : public UObject
{
    GENERATED_BODY()

public:
    /** Get current state */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
EDialogueState GetCurrentState() const { return CurrentState; }
    
    /** Attempt state transition */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    bool TransitionTo(EDialogueState NewState);
    
    /** Can transition to state? */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    bool CanTransitionTo(EDialogueState NewState) const;
    
    /** Get state duration */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    float GetTimeInCurrentState() const;

protected:
    UPROPERTY()
    EDialogueState CurrentState;
    
    UPROPERTY()
    EDialogueState PreviousState;
    
    UPROPERTY()
    FDateTime StateStartTime;
    
    /** Transition rules */
    bool IsTransitionValid(EDialogueState From, EDialogueState To) const;
};
```

---

## ?? Benefits

### Before (v1.2):
```cpp
// No state tracking
if (bIsActive) // Simple bool
{
    ProcessNode(); // Can be called anytime
}
```

### After (v1.3):
```cpp
// Explicit state machine
if (StateMachine->GetCurrentState() == EDialogueState::Active)
{
    if (StateMachine->TransitionTo(EDialogueState::Transitioning))
    {
        ProcessNode(); // Only when state allows
 StateMachine->TransitionTo(EDialogueState::Active);
    }
}
```

---

## ?? State-Based Operations

| Operation | Allowed States |
|-----------|----------------|
| `StartDialogue()` | Idle, Ended |
| `EndDialogue()` | Active, Paused, Error |
| `PauseDialogue()` | Active, Transitioning |
| `ResumeDialogue()` | Paused |
| `GoToNode()` | Active |
| `SelectChoice()` | Active |
| `ExecuteCommand()` | Active, Transitioning |
| `UndoCommand()` | Active, Paused |

---

## ?? Debugging Features

### State History
```cpp
class UDialogueStateMachine
{
    /** Track state changes for debugging */
    TArray<FDialogueStateChange> StateHistory;
    
    /** Max history size */
    int32 MaxHistorySize = 100;
};

struct FDialogueStateChange
{
    EDialogueState FromState;
    EDialogueState ToState;
    FDateTime Timestamp;
    float Duration;
};
```

### State Logging
```cpp
// Automatic logging of all state changes
UE_LOG(LogDialogue, Log, TEXT("State: %s -> %s (%.3fs)"),
    *PrevStateStr, *NewStateStr, Duration);
```

---

## ?? Blueprint Integration

```
[Event Graph]
  ?? Get Dialogue Runner
      ?? Get State Machine
 ?? Get Current State
        ?? Switch on EDialogueState
             ?? Idle: [Show Start Button]
  ?? Active: [Show Dialogue UI]
      ?? Paused: [Show Resume Button]
                  ?? Ended: [Show Results]
```

---

## ? Performance

| Metric | Impact |
|--------|--------|
| State check | O(1) - single enum comparison |
| State transition | O(1) - validation + assignment |
| Memory | +24 bytes per DialogueRunner |
| CPU | Negligible |

---

## ?? Implementation Plan

### Phase 1: Core State Machine
- [ ] Create `EDialogueState` enum
- [ ] Create `UDialogueStateMachine` class
- [ ] Implement transition validation
- [ ] Add state history tracking

### Phase 2: DialogueRunner Integration
- [ ] Replace `bIsActive` with StateMachine
- [ ] Update all methods to check state
- [ ] Add state transitions to methods
- [ ] Update events to include state

### Phase 3: Command Pattern Integration
- [ ] Commands check state before execution
- [ ] State transitions via commands
- [ ] Undo state transitions

### Phase 4: Testing & Documentation
- [ ] Unit tests for transitions
- [ ] Blueprint examples
- [ ] Documentation

---

**Status:** ?? Design Complete - Ready for Implementation  
**Next:** Implementation Phase 1  
**ETA:** 2-3 hours
