# Quest System - Best Practices Guide

Production-tested patterns and recommendations for building robust quest systems.

---

## Design Principles

### 1. Single Responsibility
Each quest should have ONE clear goal or narrative arc.

? **Bad**: "Help the Village" - collect wood, kill bandits, fix roof, talk to mayor  
? **Good**: Multiple quests - "Gather Resources", "Clear Bandits", "Repair Roof", "Meet the Mayor"

### 2. Clear Win Condition
Player should always know what to do next.

? **Bad**: "Investigate the mystery" (no hints)  
? **Good**: "Talk to 3 witnesses" ? "Examine crime scene" ? "Find evidence"

### 3. Fail-Safe Design
Handle edge cases and player mistakes gracefully.

- Quest items shouldn't be consumable
- Important NPCs shouldn't be killable
- Time limits should be generous or optional
- Alternative paths for blocked content

---

## Quest Structure

### Phase Organization

**Keep phases focused:**
```
Phase 1: Introduction (1-2 objectives)
Phase 2: Main Action (2-4 objectives)
Phase 3: Resolution (1 objective)
```

**Avoid:**
- More than 5 phases per quest
- More than 5 objectives per phase
- Optional objectives that are required (contradiction)

### Objective Clarity

**Good objective titles:**
- "Talk to the Blacksmith"
- "Collect 5 Iron Ore from the mine"
- "Defeat 3 Bandits near the bridge"
- "Return to the Quest Giver"

**Bad objective titles:**
- "Continue"
- "Do the thing"
- "Help out"
- "Finish quest"

---

## Event Emission Strategy

### Emit Discretely, Not Continuously

```cpp
// ? GOOD - Discrete event
void OnItemPickedUp(FName ItemId)
{
    EmitItemAcquired(ItemId, 1, Player, Tags);
}

// ? BAD - Continuous event
void Tick(float DeltaTime)
{
    if (IsNearNPC())
    {
        EmitNearNPC(NPCId); // Emits every frame!
    }
}
```

### Use Gates Instead of Polling

```cpp
// ? GOOD - Gate restricts when objective can progress
Objective:
  Condition: OnItemAcquired (ItemId: Apple)
  Gate: RequireTimeOfDay (6-12)
  
// ? BAD - Checking continuously
void Tick(float DeltaTime)
{
    if (CurrentHour >= 6 && CurrentHour <= 12)
    {
        CheckIfPlayerHasApple();
    }
}
```

---

## Dependency Management

### Quest Chains

**Linear progression:**
```
Q_Tutorial_1 (Complete) 
  ? Q_Tutorial_2 (Complete)
    ? Q_Tutorial_3 (Available)
```

**Branching progression:**
```
Q_Main_1 (Complete)
  ?? Q_Path_A (Choose Good)
  ?? Q_Path_B (Choose Evil)
```

### Level Gating

```cpp
FQuestDependency:
  RequiredLevel: 5  // Don't show quest until player is ready
  RequiredTags: ["Tutorial.Complete"]  // Ensure basics are learned
```

### Circular Dependency Prevention

**Run validation to detect:**
- Quest A requires Quest B complete
- Quest B requires Quest A complete
- ? This creates a deadlock!

**Solution:** Use validation tool in editor

---

## Save/Load Patterns

### Auto-Save Points

Save automatically on:
- Quest start
- Quest complete/fail
- Phase transition
- Objective completion

### Save Data Versioning

```cpp
UPROPERTY(SaveGame)
int32 SaveVersion = 1;

void LoadSaveData()
{
    if (SaveData->SaveVersion < CurrentVersion)
    {
    MigrateSaveData(SaveData);
    }
}
```

### Handle Missing Data

```cpp
// Quest asset was deleted but save has it active
if (!QuestAsset)
{
    UE_LOG(LogQuest, Warning, TEXT("Quest asset missing: %s"), *QuestId);
  
    // Option 1: Skip and continue
    continue;
    
    // Option 2: Mark as failed
 FailQuest(QuestId);
    
    // Option 3: Remove from save
    RemoveQuestData(QuestId);
}
```

---

## Performance Optimization

### Event Bus Efficiency

**Measure impact:**
```cpp
SCOPE_CYCLE_COUNTER(STAT_QuestEventProcessing);
EmitQuestEvent(Payload);
```

**Optimize hot paths:**
- Early exit if no active quests
- Cache quest asset references
- Batch multiple events

### Asset Loading Strategy

```cpp
// ? GOOD - Async load
AssetManager.LoadPrimaryAsset(QuestId, 
    FStreamableDelegate::CreateUObject(this, &UQuestSubsystem::OnQuestLoaded));

// ? BAD - Synchronous load in game thread
UQuestAsset* Asset = LoadObject<UQuestAsset>(nullptr, *Path);
```

### Memory Management

- Unload completed quest assets
- Cache only active quest data
- Limit simultaneous active quests (e.g., 20 max)

---

## UI/UX Best Practices

### Quest Notification Timing

**Show notifications for:**
- Quest started (2-3 seconds)
- Objective updated (1-2 seconds)
- Objective completed (2-3 seconds)
- Quest completed (3-5 seconds)
- Quest failed (3-5 seconds)

**Don't spam:**
- Batch rapid updates (e.g., kill 10 enemies)
- Debounce frequent events
- Queue notifications if player is busy

### Journal Organization

**Filters to include:**
- Active / Completed / Failed / All
- Main / Side / Tutorial
- By location / By NPC
- By difficulty

**Sorting options:**
- Priority (recommended as default)
- Distance to objective
- Time remaining
- Alphabetical

### Objective Tracking

**Track on HUD:**
- Main quest objectives
- Currently selected quest
- Time-limited objectives
- Nearest objectives

**Don't track:**
- Hidden objectives (spoilers)
- Completed objectives
- Optional objectives (unless player wants)

---

## Localization

### Use FText Everywhere

```cpp
// ? GOOD
UPROPERTY(EditAnywhere)
FText QuestTitle;

// ? BAD
UPROPERTY(EditAnywhere)
FString QuestTitle;
```

### Provide Context

```cpp
// ? GOOD - Clear context for translators
FText::Format(
    LOCTEXT("CollectObjective", "Collect {ItemName} ({Current}/{Target})"),
    ItemNameText, CurrentCount, TargetCount
);

// ? BAD - No context
FText::Format(FText::FromString("{0} ({1}/{2})"), ...);
```

### Test All Languages

- Ensure UI doesn't break with long text (German)
- Test right-to-left languages (Arabic)
- Check special characters (Chinese, Japanese)

---

## Testing Strategies

### Automated Testing

```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FQuestSystemTest, "Game.QuestSystem.BasicFlow", EAutomationTestFlags::ProductFilter)

bool FQuestSystemTest::RunTest(const FString& Parameters)
{
    UQuestSubsystem* QuestSystem = GetQuestSubsystem();
    
    // Test start
    bool bStarted = QuestSystem->StartQuest(FName("Q_Test"));
    TestTrue("Quest started", bStarted);
    
    // Test progress
 EmitTestEvent();
    TestEqual("Objective progress", GetProgress(), 1);
    
    // Test complete
    TestTrue("Quest completed", IsQuestCompleted(FName("Q_Test")));
    
    return true;
}
```

### Manual Testing Checklist

- [ ] Quest starts correctly
- [ ] All objectives can be completed
- [ ] Progress saves/loads
- [ ] Failure conditions work
- [ ] Rewards granted properly
- [ ] UI updates correctly
- [ ] Quest can be abandoned
- [ ] Dependencies checked
- [ ] No console errors
- [ ] Works after save/load
- [ ] Works in packaged build

### Edge Cases to Test

1. **Quest already active**: Try starting twice
2. **Quest completed**: Try starting again if not repeatable
3. **Missing NPC**: What if required NPC is dead?
4. **Missing item**: What if quest item was dropped/sold?
5. **Level transition**: Save/load during quest
6. **Time expires**: Timer runs out during pause
7. **Rapid events**: Spam event emission
8. **Memory**: 100+ completed quests

---

## Debug Tools

### Console Commands

Implement these for testing:

```cpp
// Start quest
StartQuest Q_Tutorial_First

// Complete objective
CompleteObjective Q_Tutorial_First Obj_TalkToNPC

// Skip to phase
SetQuestPhase Q_Tutorial_First Phase_2

// Complete quest instantly
CompleteQuest Q_Tutorial_First

// List active quests
ListActiveQuests

// Print quest state
PrintQuestState Q_Tutorial_First

// Reset progress
ResetQuestProgress
```

### Debug UI

**In-game debug menu:**
- List all quests with states
- Force start/complete/fail
- Teleport to objectives
- Grant quest items
- Skip time/change weather

### Logging

**Use log categories:**
```cpp
DECLARE_LOG_CATEGORY_EXTERN(LogQuestSystem, Log, All);

UE_LOG(LogQuestSystem, Verbose, TEXT("Event: %s"), *EventTag.ToString());
UE_LOG(LogQuestSystem, Log, TEXT("Quest started: %s"), *QuestId.ToString());
UE_LOG(LogQuestSystem, Warning, TEXT("Objective gates not met: %s"), *ObjectiveId.ToString());
UE_LOG(LogQuestSystem, Error, TEXT("Failed to load quest asset: %s"), *QuestId.ToString());
```

---

## Common Pitfalls

### 1. String Comparison Hell

? **Bad:**
```cpp
if (ItemName == "Apple")  // Typo breaks quest!
```

? **Good:**
```cpp
if (ItemId == FName("Item_Apple"))  // FName is fast and safe
```

### 2. Forgetting to Save

? **Bad:**
```cpp
void OnQuestCompleted()
{
    // Progress lost on game restart!
}
```

? **Good:**
```cpp
void OnQuestCompleted()
{
    SaveQuestProgress();
}
```

### 3. Tight Coupling

? **Bad:**
```cpp
// Quest system directly accesses inventory
UInventoryComponent* Inv = Player->FindComponent<UInventoryComponent>();
Inv->AddItem(ItemId);
```

? **Good:**
```cpp
// Emit event, other systems handle it
FQuestEventPayload Payload;
Payload.EventTag = Tag_GiveItem;
EventBus->EmitEvent(Payload);
```

### 4. No Validation

? **Bad:**
```cpp
// Ship with broken quests
```

? **Good:**
```cpp
#if WITH_EDITOR
virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
{
    return ValidateQuestData(Context);
}
#endif
```

### 5. Hardcoded IDs

? **Bad:**
```cpp
if (QuestId == FName("Q_12345"))  // Magic number!
```

? **Good:**
```cpp
// Define constants or use Data Assets
static const FName QUEST_TUTORIAL = FName("Q_Tutorial_First");
```

---

## Integration Patterns

### Inventory Integration

```cpp
class UInventoryComponent
{
  void AddItem(FName ItemId, int32 Count)
    {
      // Add to inventory
        Items.Add(ItemId, Count);
        
        // Notify quest system
        UQuestBlueprintLibrary::EmitItemAcquired(this, ItemId, Count, Owner, Tags);
        
        // Notify UI
        OnInventoryChanged.Broadcast();
    }
};
```

### Dialog Integration

```cpp
class UDialogComponent
{
    void OnChoiceSelected(FName ChoiceId)
    {
        // Process dialog choice
        ProcessChoice(ChoiceId);
        
        // Emit quest event
  UQuestBlueprintLibrary::EmitDialogChoice(this, CurrentDialogId, ChoiceId, Tags);
        
   // Check for quest start
  if (ChoiceStartsQuest(ChoiceId))
        {
            UQuestBlueprintLibrary::StartQuest(this, GetQuestIdForChoice(ChoiceId));
        }
    }
};
```

### Time System Integration

```cpp
class ATimeOfDayManager
{
    void OnHourChanged(int32 NewHour)
  {
        // Update world time
    UpdateSunPosition(NewHour);
        
        // Notify quest system
  UQuestSubsystem* QuestSys = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
        QuestSys->GetEventBus()->EmitTimeReached(NewHour, 0);
        
        // Check quest failures
        CheckTimeBasedQuestFailures(NewHour);
    }
};
```

---

## Documentation Standards

### Quest Asset Comments

```cpp
// Quest ID: Q_Village_Baker_DailyBread
// Designer: John Doe
// Date: 2024-01-15
// Status: Final
// Dependencies: Q_Tutorial_Complete
// Estimated Time: 5 minutes
// Rewards: 50 gold, +5 Baker reputation
// Notes: Repeatable daily quest. Auto-starts at 06:00 if player is in village.
```

### Code Comments

```cpp
/**
 * Checks if quest can be started based on dependencies
 * 
 * @param QuestAsset Quest to check
 * @param OutFailReasons Array filled with reasons if dependencies not met
 * @return true if all dependencies are satisfied
 * 
 * Dependencies checked:
 * - Required quest states
 * - Player level
 * - Gameplay tags (required/forbidden)
 * - Time windows
 * - Location requirements
 * - NPC affection levels
 */
bool CheckDependencies(const UQuestAsset* QuestAsset, TArray<FText>& OutFailReasons) const;
```

---

## Metrics and Analytics

### Track Important Data

```cpp
// Quest completion rate
float GetCompletionRate(FName QuestId)
{
    int32 Started = Analytics->GetQuestStartCount(QuestId);
    int32 Completed = Analytics->GetQuestCompleteCount(QuestId);
    return (float)Completed / Started;
}

// Average completion time
float GetAverageCompletionTime(FName QuestId)
{
    return Analytics->GetAverageTime(QuestId);
}

// Abandonment points
TArray<FName> GetCommonAbandonmentObjectives(FName QuestId)
{
    return Analytics->GetAbandonmentPoints(QuestId);
}
```

### Use Data to Improve

- If completion rate < 50% ? Quest too hard or unclear
- If avg time >> estimated ? Quest longer than expected
- If high abandonment at specific objective ? Fix that objective

---

## Summary Checklist

**Before shipping a quest:**

- [ ] Quest ID is unique and descriptive
- [ ] All text is localized (FText)
- [ ] Objectives are clear and achievable
- [ ] Dependencies are correct
- [ ] Rewards are balanced
- [ ] Validation passes with no errors
- [ ] Tested start to finish
- [ ] Tested save/load
- [ ] Tested edge cases
- [ ] No console errors/warnings
- [ ] Performance is acceptable
- [ ] Documentation is complete

**Quality bar:**
- Completion rate: > 70%
- Average time: Within 20% of estimate
- Bug reports: < 1% of players
- Player satisfaction: > 4/5 stars

---

## Further Reading

- [Examples](Examples.md) - Complete quest examples
- [API Reference](API_Reference.md) - Full API documentation
- [Quick Start](QuickStart.md) - Get started in 10 steps

---

**Remember:** The best quest is one that's fun to play, easy to complete, and impossible to break! ??
