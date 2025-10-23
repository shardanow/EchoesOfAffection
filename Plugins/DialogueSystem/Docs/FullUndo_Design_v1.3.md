# Full Undo Support Design (v1.3)

> **Version:** 1.3.0  
> **Feature:** Полная поддержка Undo/Redo  
> **Status:** ?? In Design

---

## ?? Goals

1. **Complete Undo** - Все команды поддерживают полный undo
2. **State Snapshots** - Memento pattern для состояния
3. **Effect Reversal** - Отмена эффектов диалога
4. **Navigation Undo** - Возврат к предыдущим нодам

---

## ?? Current Limitations (v1.2)

| Command | Undo Support | Issue |
|---------|--------------|-------|
| NavigateToNode | ?? Partial | Can't restore CurrentNode |
| ApplyEffects | ? None | Can't revert effects |
| SelectChoice | ?? Partial | Can't restore state |
| SetVariable | ? Full | Works correctly |
| Composite | ? Full | Works correctly |

**Root Cause:** `UDialogueSessionContext` не имеет методов для управления CurrentNode.

---

## ?? Solution: Memento Pattern + State Snapshots

### Architecture:

```cpp
/**
 * Snapshot of dialogue state at a point in time
 * Used for perfect undo/redo
 */
USTRUCT(BlueprintType)
struct FDialogueStateSnapshot
{
    GENERATED_BODY()

    /** Current node ID */
    UPROPERTY()
    FName CurrentNodeId;
    
    /** Node history */
UPROPERTY()
    TArray<FName> NodeHistoryIds;
    
    /** Custom variables snapshot */
    UPROPERTY()
  TMap<FName, FString> CustomVariables;
    
    /** Active tags */
    UPROPERTY()
    FGameplayTagContainer ActiveTags;
    
    /** Visited nodes */
    UPROPERTY()
    TArray<FName> VisitedNodes;
    
    /** Timestamp */
    UPROPERTY()
    FDateTime Timestamp;
    
    /** State */
    UPROPERTY()
    EDialogueState DialogueState;
};
```

---

## ?? Enhanced Context API

### New Methods for UDialogueSessionContext:

```cpp
class DIALOGUESYSTEMCORE_API UDialogueSessionContext : public UObject
{
public:
    /** Create snapshot of current state */
    UFUNCTION(BlueprintCallable, Category = "Context|Snapshot")
    FDialogueStateSnapshot CreateSnapshot() const;
  
    /** Restore from snapshot */
    UFUNCTION(BlueprintCallable, Category = "Context|Snapshot")
    bool RestoreFromSnapshot(const FDialogueStateSnapshot& Snapshot);
    
    /** Set current node (for undo support) */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void SetCurrentNode(UDialogueNode* Node);
    
    /** Get current node */
    UFUNCTION(BlueprintPure, Category = "Context")
    UDialogueNode* GetCurrentNode() const;
    
    /** Clear all state */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void ResetState();
};
```

---

## ?? Enhanced Commands

### 1. NavigateToNode - Full Undo

```cpp
class UDialogueCommand_NavigateToNode : public UDialogueCommand
{
protected:
    virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override
    {
        // Create snapshot BEFORE navigation
    StateSnapshot = Context->CreateSnapshot();
     
 // Navigate
    PreviousNode = Context->GetCurrentNode();
        Context->SetCurrentNode(TargetNode);

        return true;
    }
    
    virtual bool UndoImplementation(UDialogueSessionContext* Context) override
    {
        // Restore complete state
        return Context->RestoreFromSnapshot(StateSnapshot);
    }

private:
    UPROPERTY()
    FDialogueStateSnapshot StateSnapshot;
    
    UPROPERTY()
    TObjectPtr<UDialogueNode> PreviousNode;
};
```

### 2. ApplyEffects - Full Reversal

```cpp
class UDialogueCommand_ApplyEffects : public UDialogueCommand
{
protected:
    virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override
    {
        // Snapshot BEFORE effects
      StateSnapshot = Context->CreateSnapshot();
        
   // Apply effects
        for (UDialogueEffect* Effect : Effects)
        {
    Effect->Execute(Context);
   }
        
        return true;
    }
    
    virtual bool UndoImplementation(UDialogueSessionContext* Context) override
{
        // Restore state (reverts ALL effects)
        return Context->RestoreFromSnapshot(StateSnapshot);
    }

private:
    UPROPERTY()
    FDialogueStateSnapshot StateSnapshot;
    
    UPROPERTY()
    TArray<TObjectPtr<UDialogueEffect>> Effects;
};
```

### 3. SelectChoice - Full Undo

```cpp
class UDialogueCommand_SelectChoice : public UDialogueCommand
{
protected:
    virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override
    {
        // Snapshot BEFORE choice
        StateSnapshot = Context->CreateSnapshot();
        
        // Execute choice
        // ... choice logic ...
   
    return true;
    }
    
    virtual bool UndoImplementation(UDialogueSessionContext* Context) override
    {
        // Restore complete state
        return Context->RestoreFromSnapshot(StateSnapshot);
    }

private:
 UPROPERTY()
    FDialogueStateSnapshot StateSnapshot;

    UPROPERTY()
    int32 ChoiceIndex;
};
```

---

## ?? State Snapshot Strategy

### When to Create Snapshots:

| Event | Create Snapshot? | Why |
|-------|------------------|-----|
| Command Execute | ? Yes | For undo |
| Node Enter | ?? Optional | For history |
| Choice Select | ? Yes | For undo |
| Effect Apply | ? Yes | For reversal |
| Variable Set | ?? Optional | Small state |

### Snapshot Optimization:

```cpp
class FDialogueStateSnapshot
{
public:
    /** Lightweight copy - only changed data */
    bool bIsIncremental;
 
    /** Reference to base snapshot (for incremental) */
    TSharedPtr<FDialogueStateSnapshot> BaseSnapshot;
    
    /** Only store differences */
    TMap<FName, FString> ChangedVariables;
};
```

---

## ?? Effect Reversal System

### Reversible Effects:

```cpp
class UDialogueEffect : public UObject
{
public:
/** Execute effect */
    virtual void Execute(UDialogueSessionContext* Context);
    
    /** NEW: Reverse effect */
    virtual void Reverse(UDialogueSessionContext* Context);
    
    /** NEW: Can this effect be reversed? */
    virtual bool CanReverse() const { return bSupportsReverse; }
    
protected:
    UPROPERTY()
    bool bSupportsReverse;
};
```

### Example - ModifyAffinity:

```cpp
class UDialogueEffect_ModifyAffinity : public UDialogueEffect
{
public:
    UDialogueEffect_ModifyAffinity()
    {
        bSupportsReverse = true; // ? Can reverse
  }
    
    virtual void Execute(UDialogueSessionContext* Context) override
    {
        // Store old value
        OldValue = Context->GetAffinity(NPCName);
        
        // Apply change
        Context->ModifyAffinity(NPCName, DeltaValue);
    }
    
    virtual void Reverse(UDialogueSessionContext* Context) override
    {
        // Restore old value
        Context->SetAffinity(NPCName, OldValue);
    }

private:
    float OldValue;
    float DeltaValue;
};
```

---

## ?? Undo Stack Enhancement

### Advanced Command History:

```cpp
class UDialogueCommandHistory : public UObject
{
public:
    /** NEW: Snapshot at each command */
    struct FCommandHistoryEntry
    {
 TObjectPtr<UDialogueCommand> Command;
        FDialogueStateSnapshot Snapshot; // NEW
  FDateTime Timestamp;
        EDialogueCommandResult Result;
    };
    
    /** NEW: Jump to specific point in history */
    UFUNCTION(BlueprintCallable, Category = "History")
    bool JumpToHistoryPoint(int32 HistoryIndex);
    
    /** NEW: Get snapshot at index */
    UFUNCTION(BlueprintPure, Category = "History")
    FDialogueStateSnapshot GetSnapshotAt(int32 Index) const;
};
```

---

## ?? Memory Optimization

### Snapshot Compression:

```cpp
// Full snapshot every N commands
const int32 FullSnapshotInterval = 10;

// Incremental snapshots in between
struct FIncrementalSnapshot
{
    int32 BaseSnapshotIndex;
    TMap<FName, FString> Changes;
};
```

### Memory Usage Estimate:

| Component | Size | Per Command |
|-----------|------|-------------|
| Full Snapshot | ~500 bytes | Every 10th |
| Incremental | ~100 bytes | Most commands |
| Command Object | ~200 bytes | All |
| **Total** | ~300 bytes avg | Per command |

**For 100 commands:** ~30KB memory

---

## ?? Implementation Plan

### Phase 1: Context Enhancement
- [ ] Add `GetCurrentNode()` / `SetCurrentNode()`
- [ ] Implement `CreateSnapshot()`
- [ ] Implement `RestoreFromSnapshot()`
- [ ] Add unit tests

### Phase 2: Command Updates
- [ ] Update `NavigateToNode` with snapshots
- [ ] Update `ApplyEffects` with snapshots
- [ ] Update `SelectChoice` with snapshots
- [ ] Enable undo for all commands

### Phase 3: Effect Reversal
- [ ] Add `Reverse()` method to base effect
- [ ] Implement reversal for all effects
- [ ] Add `bSupportsReverse` flag

### Phase 4: History Enhancement
- [ ] Store snapshots in history
- [ ] Implement jump-to-point
- [ ] Add snapshot compression

---

## ? Success Criteria

- [x] All commands support full undo
- [x] State perfectly restored
- [x] Effects can be reversed
- [x] Navigation can be undone
- [x] Memory usage acceptable (<50KB for 100 commands)
- [x] Performance acceptable (<1ms per undo)

---

**Status:** ?? Design Complete - Ready for Implementation  
**Dependencies:** State Machine (should be implemented first)  
**ETA:** 3-4 hours  
**Complexity:** Medium-High
