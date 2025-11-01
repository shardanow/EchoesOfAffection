# Quest Rewards: Complete Guide

## ?? Overview

The Quest System supports **two types of rewards**:
1. **Quest Completion Rewards** — Given when the entire quest completes
2. **Phase Completion Rewards** — Given when a phase finishes

---

## ? **How Rewards Work**

### Automatic Reward Processing

Rewards are **automatically granted** when:
- ? A quest completes (`QuestAsset.RewardsOnComplete`)
- ? A phase completes (`FQuestPhase.PhaseRewards`)
- ? An objective completes (`FQuestObjective.Signals.OnCompleteRewards`)

**No manual code needed!** The system handles it automatically.

---

## ?? **Reward Types**

```cpp
UENUM(BlueprintType)
enum class EQuestRewardType : uint8
{
    Item,       // Give item to player
    Currency,    // Give gold/money
    Experience,     // Give XP
    StatModifier,   // Change player stat
    AffectionBonus, // Increase NPC affection
    UnlockDialog,   // Unlock dialogue branch
    UnlockRecipe,   // Unlock crafting recipe
    CustomEffect    // Blueprint event
};
```

---

## ?? **Setting Up Rewards in Data Asset**

### Example 1: Quest Completion Rewards

```cpp
Quest: "Help Lianne"
  RewardsOnComplete:
    PrimaryRewards:
      - Type: Item
        ItemId: Gold
        Amount: 100
      - Type: Experience
     Amount: 50
      - Type: AffectionBonus
NpcId: Lianne
      Amount: 10.0
    BonusTags:
      - Quest.Completed.HelpLianne
```

### Example 2: Phase Completion Rewards

```cpp
Phase: "P_CollectApples"
  PhaseRewards:
    PrimaryRewards:
   - Type: Item
        ItemId: RedApple
        Amount: 5
      - Type: Experience
        Amount: 25
```

### Example 3: Objective Completion Rewards

```cpp
Objective: "O_TalkToLianne"
  Signals:
    OnCompleteRewards:
      PrimaryRewards:
        - Type: Currency
    Amount: 50
    - Type: UnlockDialog
          NpcId: Lianne
          DialogBranchId: Friendship_Level_2
```

---

## ?? **Reward Flow**

### 1. Quest Completion
```
Player completes last objective
  ?
Phase completes (if bAutoAdvance = true)
  ?
Quest completes
  ?
QuestSubsystem::OnQuestCompletedInternal called
  ?
RewardSystem->ProcessQuestRewards(QuestAsset->RewardsOnComplete)
  ?
Rewards granted! ??
```

### 2. Phase Completion
```
Player completes all required objectives
  ?
Phase completes
  ?
QuestProgressManager::SetQuestPhase called
  ?
Phase rewards processed BEFORE advancing to next phase
  ?
QuestSubsystem::ProcessPhaseRewards called
  ?
RewardSystem->ProcessQuestRewards(Phase.PhaseRewards)
  ?
Rewards granted! ??
```

### 3. Objective Completion
```
Event triggers objective completion
  ?
QuestProgressManager::UpdateObjectiveProgress
  ?
Objective completes
  ?
QuestSubsystem::OnObjectiveCompletedInternal called
  ?
RewardSystem->ProcessQuestRewards(Objective.Signals.OnCompleteRewards)
  ?
Rewards granted! ??
```

---

## ?? **Debugging Rewards**

### Expected Log Output

When rewards are processed correctly, you'll see:

```
?????? QuestSubsystem::OnQuestCompletedInternal - Quest 'Q_CollectApples' completed!
   ? QuestAsset is valid, processing rewards...
?????? QuestRewardSystem::ProcessQuestRewards called for Quest 'Q_CollectApples'
   - PrimaryRewards count: 3
   - BonusTags count: 1
   ?? Granting reward: Type=0 (Item), for Quest 'Q_CollectApples'
QuestRewardSystem: Grant item 'Gold' x100 (Quest: Q_CollectApples)
   ?? Granting reward: Type=2 (Experience), for Quest 'Q_CollectApples'
QuestRewardSystem: Grant experience 50 (Quest: Q_CollectApples)
   ?? Granting reward: Type=4 (AffectionBonus), for Quest 'Q_CollectApples'
QuestRewardSystem: Modify affection with 'Lianne' by 10.0 (Quest: Q_CollectApples)
```

### ?? No Rewards? Check These:

1. **RewardsOnComplete is empty in DA**
   ```
   ?? No rewards to grant! RewardsOnComplete is empty in Quest Asset.
   ```
   ? Open Quest Data Asset ? Fill `RewardsOnComplete` array

2. **QuestAsset is NULL**
   ```
   ? QuestAsset is NULL! Cannot process rewards.
   ```
   ? Quest asset failed to load (check `QuestAssetLoader`)

3. **OnQuestCompletedInternal not called**
   ? Check that `ProgressManager->OnQuestCompleted` delegate is bound

---

## ??? **Integrating Rewards with Game Systems**

### Default Behavior (Logs Only)

By default, rewards are **logged but not applied** to the game:

```cpp
void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
    UE_LOG(LogTemp, Log, TEXT("Grant item '%s' x%d"), *ItemId.ToString(), Amount);
    // TODO: Integrate with inventory system
}
```

### Option 1: Blueprint Integration (Easy ?)

Create a **Blueprint** that inherits from `QuestRewardSystem`:

```
1. Create Blueprint: BP_QuestRewardSystem (Parent: QuestRewardSystem)
2. Override functions:
   - OnGrantItem
   - OnGrantCurrency
   - OnModifyStat
   - etc.
3. Set in QuestSubsystem (if using custom subsystem)
```

**Example Blueprint:**
```
Event OnGrantItem
  ?? Get Player Inventory
  ?? Call AddItem(ItemId, Amount)
```

### Option 2: C++ Integration (Advanced)

Subclass `UQuestRewardSystem` in C++:

```cpp
// MyQuestRewardSystem.h
UCLASS()
class UMyQuestRewardSystem : public UQuestRewardSystem
{
    GENERATED_BODY()

protected:
    virtual void OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId) override;
};

// MyQuestRewardSystem.cpp
void UMyQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
    // Get player inventory
    UInventoryComponent* Inventory = GetPlayerInventory();
    if (Inventory)
    {
        Inventory->AddItem(ItemId, Amount);
    }
}
```

---

## ?? **Reward Examples by Quest Type**

### Linear Quest (3 Phases)
```
Phase 1: "Talk to NPC"
  PhaseRewards:
  - Experience: 10

Phase 2: "Collect Items"
  PhaseRewards:
    - Currency: 50

Phase 3: "Return to NPC"
  (No phase rewards)

Quest Completion:
  RewardsOnComplete:
    - Item: SpecialSword, Amount: 1
    - Experience: 100
    - AffectionBonus: NpcId=Lianne, Amount=20
```

### Collection Quest (Parallel Objectives)
```
Phase: "Collect Resources"
  Objective 1: Collect Apples
    OnCompleteRewards:
      - Experience: 5
  
  Objective 2: Collect Flowers
    OnCompleteRewards:
      - Experience: 5

  PhaseRewards:
    - Currency: 30

Quest Completion:
  RewardsOnComplete:
    - UnlockRecipe: ApplePie
```

### Branching Quest
```
Phase 1: "Choose Side"
(No rewards)

Phase 2A: "Help Faction A"
  PhaseRewards:
    - AffectionBonus: NpcId=FactionA_Leader, Amount=50

Phase 2B: "Help Faction B"
  PhaseRewards:
    - AffectionBonus: NpcId=FactionB_Leader, Amount=50

Quest Completion:
  RewardsOnComplete:
    - Experience: 200
    - Currency: 500
```

---

## ?? **Best Practices**

### ? DO:
- Use **phase rewards** for incremental progress (XP, small currency)
- Use **quest rewards** for major items (weapons, unlocks)
- Use **objective rewards** for instant feedback (small XP)
- Always fill `RewardsOnComplete` if you want end-of-quest rewards
- Test rewards by checking logs (`??` emojis)

### ? DON'T:
- Give all rewards at quest completion (feels unrewarding mid-quest)
- Forget to integrate rewards with your inventory/stats systems
- Use `CustomEffect` without implementing the handler

---

## ?? **Related Documentation**

- [Objective Execution Order](OBJECTIVE_EXECUTION_ORDER.md)
- [Quest Conditions Reference](QUEST_CONDITIONS_COMPLETE_REFERENCE.md)
- [Quick Start Guide](QUICK_START.md)

---

**Last Updated:** 2025-11-01
**Quest System Version:** 2.0
