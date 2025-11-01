# Quest Reward Timing Logic — Когда Выдаются Награды

## ?? **Краткий ответ:**

**Награды выдаются В МОМЕНТ завершения**, а не "в конце квеста":

| Тип награды | Момент выдачи | Порядок |
|-------------|---------------|---------|
| **Objective Rewards** | Сразу при завершении цели | 1?? ПЕРВЫМ |
| **Phase Rewards** | Сразу при завершении фазы (перед переходом к следующей) | 2?? ВТОРЫМ |
| **Quest Rewards** | Сразу при завершении последней фазы квеста | 3?? ПОСЛЕДНИМ |

---

## ?? **Детальная логика выполнения**

### **Scenario: Игрок завершает последнюю цель в последней фазе**

```
Player completes objective "O_ReturnToLianne" (last objective)
  ?
  ?? 1?? OBJECTIVE REWARDS (immediate)
  ?    ?? QuestProgressManager::UpdateObjectiveProgress detects completion
  ?    ?? Broadcasts OnObjectiveCompleted
  ?    ?? QuestSubsystem::OnObjectiveCompletedInternal
  ?    ?? RewardSystem->ProcessQuestRewards(Objective.Signals.OnCompleteRewards)
  ?    ?? ?? Player receives objective rewards RIGHT NOW
  ?
  ?? 2?? PHASE COMPLETION CHECK
  ?    ?? QuestProgressManager checks if ALL objectives in phase are complete
  ?    ?? Phase completes ? SetQuestPhase(NextPhaseId)
  ?  ?? Before advancing, broadcasts OnQuestPhaseChanged(OldPhaseId, NewPhaseId)
  ?    ?? QuestSubsystem::OnPhaseChangedInternal
  ?    ?? RewardSystem->ProcessQuestRewards(OldPhase.PhaseRewards)
  ?    ?? ?? Player receives phase rewards RIGHT NOW
  ?
  ?? 3?? QUEST COMPLETION CHECK
       ?? If NextPhaseId is NONE (no more phases) ? Quest completes
       ?? QuestProgressManager::CompleteQuest
       ?? Broadcasts OnQuestCompleted
       ?? QuestSubsystem::OnQuestCompletedInternal
     ?? RewardSystem->ProcessQuestRewards(QuestAsset.RewardsOnComplete)
  ?? ?? Player receives quest completion rewards RIGHT NOW
```

**ИТОГО:** Все 3 награды выдаются **в течение одного игрового кадра** (frame), НО в строгом порядке.

---

## ?? **Timeline Example: Linear Quest**

### Quest: "Help Lianne"

```
Time: 10:00 AM — Player talks to Lianne
  ?? Objective "O_TalkToLianne" completes
       ?? ?? +5 XP (Objective reward)
       ?? Phase "P_Introduction" completes
     ?? ?? +10 XP (Phase reward)
  ?? Quest advances to Phase "P_CollectApples"

Time: 10:15 AM — Player collects 3rd apple
?? Objective "O_CollectApples" progress: 3/3
       ?? Objective completes
            ?? ?? +15 XP (Objective reward)
       ?? Phase "P_CollectApples" completes
      ?? ?? +50 Gold (Phase reward)
       ?? Quest advances to Phase "P_Return"

Time: 10:30 AM — Player returns to Lianne
  ?? Objective "O_ReturnToLianne" completes
       ?? ?? +10 XP (Objective reward)
       ?? Phase "P_Return" completes
            ?? ?? +25 XP (Phase reward)
       ?? NO MORE PHASES ? Quest completes!
    ?? ?? +100 XP + SpecialSword (Quest reward)
```

**Player's total XP progression:**
```
10:00 ? +5 +10  = 15 XP
10:15 ? +15 +50 gold   = 30 XP + 50 gold
10:30 ? +10 +25 +100   = 165 XP + 50 gold + Sword
```

**Награды выдаются ПОСТЕПЕННО, а не все в конце!** ?

---

## ?? **Код: Как это работает**

### 1?? **Objective Completion (Первым)**

```cpp
// QuestProgressManager.cpp
bool UQuestProgressManager::UpdateObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount)
{
    // ... update progress ...
    
    // Check if objective is complete
    if (ObjectiveData.CurrentCount >= ObjectiveDef.TargetCount && !ObjectiveData.bIsCompleted)
    {
        ObjectiveData.bIsCompleted = true;
        
        // ?? IMMEDIATE BROADCAST (Step 1)
     OnObjectiveCompleted.Broadcast(QuestId, ObjectiveId);
        
        UE_LOG(LogTemp, Log, TEXT("Objective completed: %s"), *ObjectiveId.ToString());
  }
    
    // Check if PHASE is complete (all objectives done)
    if (AreAllObjectivesComplete(CurrentPhase))
    {
        AdvanceToNextPhase(QuestId); // ? Triggers Phase rewards
    }
    
    return true;
}
```

**QuestSubsystem listens:**
```cpp
// QuestSubsystem.cpp
void UQuestSubsystem::OnObjectiveCompletedInternal(FName QuestId, FName ObjectiveId)
{
    // Find objective definition
    FQuestObjective ObjectiveDef = GetObjectiveDefinition(QuestId, ObjectiveId);
    
    // ?? GRANT OBJECTIVE REWARDS IMMEDIATELY
    RewardSystem->ProcessQuestRewards(
    ObjectiveDef.Signals.OnCompleteRewards, 
        QuestId
    );
    
    UE_LOG(LogTemp, Log, TEXT("Granted objective rewards for %s"), *ObjectiveId.ToString());
}
```

---

### 2?? **Phase Completion (Вторым)**

```cpp
// QuestProgressManager.cpp
bool UQuestProgressManager::SetQuestPhase(FName QuestId, FName PhaseId)
{
    FName OldPhaseId = QuestData.CurrentPhaseId;
    QuestData.CurrentPhaseId = PhaseId;
    
    // ?? BROADCAST PHASE CHANGE (Step 2)
    OnQuestPhaseChanged.Broadcast(QuestId, OldPhaseId, PhaseId);
    
    // If PhaseId is NONE ? Quest complete
    if (PhaseId.IsNone())
    {
        CompleteQuest(QuestId); // ? Triggers Quest rewards
    }
    
    return true;
}
```

**QuestSubsystem listens:**
```cpp
// QuestSubsystem.cpp
void UQuestSubsystem::OnPhaseChangedInternal(FName QuestId, FName OldPhaseId, FName NewPhaseId)
{
    if (!OldPhaseId.IsNone())
    {
        // Get OLD phase definition
        FQuestPhase OldPhase = QuestAsset->GetPhase(OldPhaseId);
        
        // ?? GRANT PHASE REWARDS BEFORE ADVANCING
        RewardSystem->ProcessQuestRewards(
 OldPhase.PhaseRewards, 
            QuestId
        );
        
   UE_LOG(LogTemp, Log, TEXT("Granted phase rewards for %s"), *OldPhaseId.ToString());
    }
}
```

---

### 3?? **Quest Completion (Последним)**

```cpp
// QuestProgressManager.cpp
bool UQuestProgressManager::CompleteQuest(FName QuestId)
{
  QuestData.State = EQuestState::Completed;
    
    // ?? BROADCAST QUEST COMPLETED (Step 3)
    OnQuestCompleted.Broadcast(QuestId, QuestAsset);
    
    UE_LOG(LogTemp, Log, TEXT("Quest completed: %s"), *QuestId.ToString());
    return true;
}
```

**QuestSubsystem listens:**
```cpp
// QuestSubsystem.cpp
void UQuestSubsystem::OnQuestCompletedInternal(FName QuestId, UQuestAsset* QuestAsset)
{
    if (QuestAsset)
    {
   // ?? GRANT QUEST COMPLETION REWARDS
     RewardSystem->ProcessQuestRewards(
      QuestAsset->RewardsOnComplete, 
            QuestId
  );
    
   UE_LOG(LogTemp, Log, TEXT("Granted quest completion rewards for %s"), *QuestId.ToString());
    }
}
```

---

## ?? **Execution Order (Single Frame)**

```
Frame N:
  ?
  ?? Event: Player picks up last apple
  ?    ?? GameEventBus emits "GameEvent.Inventory.ItemAcquired"
  ?    ?? QuestEventBridge forwards to QuestEventBus
  ?    ?? QuestEventBus checks active quests
  ?    ?? Matches objective "O_CollectApples"
  ?    ?? QuestProgressManager::UpdateObjectiveProgress(+1)
  ?       ?
  ?   ?? CurrentCount = 3/3 ? Objective completes
  ?         ?  ?? OnObjectiveCompleted.Broadcast()
  ?         ?  ?? QuestSubsystem::OnObjectiveCompletedInternal()
  ?  ?              ?? ?? Objective rewards granted (15 XP)
  ?         ?
  ?         ?? Check: All objectives in phase complete? YES
  ?       ?    ?? AdvanceToNextPhase()
  ?         ?         ?? SetQuestPhase(NextPhaseId)
  ?         ?     ?? OnQuestPhaseChanged.Broadcast()
  ?         ? ?    ?? QuestSubsystem::OnPhaseChangedInternal()
  ?       ?       ?         ?? ?? Phase rewards granted (50 gold)
  ?         ?     ?
  ?         ?        ?? NextPhaseId is NONE? YES
  ?         ?         ?? CompleteQuest()
  ?         ?          ?? OnQuestCompleted.Broadcast()
  ?         ?              ?? QuestSubsystem::OnQuestCompletedInternal()
  ?    ?        ?? ?? Quest rewards granted (100 XP + Sword)
  ?         ?
  ?       ?? Return true
  ?
  ?? Frame N ends
       Player now has: +15 +50 gold +100 XP + Sword
```

**Всё происходит синхронно в одном кадре!** ?

---

## ?? **Player Experience**

### **What the player sees (UI perspective):**

**Before completing last objective:**
```
Quest Tracker:
  Help Lianne (2/3 phases)
    - Return to Lianne (0/1)
```

**Player clicks "Talk to Lianne":**
```
[INSTANT FEEDBACK — same frame]

? +10 XP (Objective reward)
? +25 XP (Phase reward)
? +100 XP + SpecialSword unlocked! (Quest reward)

?? Quest "Help Lianne" completed!

Quest Tracker:
  (Quest removed from tracker)
  
Notification:
  "You received: SpecialSword, 135 XP"
```

**Player feels instant gratification!** ??

---

## ?? **Design Patterns**

### **Pattern 1: Small Rewards During, Big Reward at End**
```
Quest: "Epic Journey"

Phase 1: "Preparation"
  Objective: Buy supplies (+5 XP)
  Phase Reward: +10 XP

Phase 2: "Travel"
  Objective: Reach mountain (+10 XP)
  Phase Reward: +20 XP

Phase 3: "Boss Fight"
  Objective: Defeat dragon (+50 XP)
  Phase Reward: +100 XP
  
Quest Reward: +500 XP + Legendary Sword
```

**Effect:** Player gets steady progression (small rewards), then BIG payoff at end.

---

### **Pattern 2: Equal Rewards Throughout**
```
Quest: "Daily Chores"

Phase 1: "Morning"
  Objective: Water plants (+10 XP)
  Phase Reward: +10 XP

Phase 2: "Noon"
  Objective: Feed animals (+10 XP)
  Phase Reward: +10 XP

Phase 3: "Evening"
  Objective: Lock barn (+10 XP)
  Phase Reward: +10 XP
  
Quest Reward: +30 XP
```

**Effect:** Consistent reward pace, no "big finish".

---

### **Pattern 3: No Phase/Objective Rewards (All at End)**
```
Quest: "Mystery Investigation"

Phase 1: "Clue 1"
  Objective: Find note (no reward)

Phase 2: "Clue 2"
  Objective: Question witness (no reward)

Phase 3: "Solve"
  Objective: Reveal culprit (no reward)
  
Quest Reward: +1000 XP + Achievement
```

**Effect:** Delayed gratification, suspense builds.

---

## ?? **Important Notes**

### **1. Rewards Are NOT Cumulative**
```cpp
// ? WRONG ASSUMPTION:
"All rewards are collected and given at quest end"

// ? CORRECT:
"Each reward is given IMMEDIATELY when its trigger completes"
```

### **2. Order Matters**
```
If player completes last objective:
  1. Objective rewards   (e.g., +10 XP)
  2. Phase rewards       (e.g., +25 XP)
  3. Quest rewards       (e.g., +100 XP + Sword)

Player sees: +10, then +25, then +100 + Sword
NOT: +135 XP + Sword all at once
```

**UI должен показывать награды последовательно для clarity!**

### **3. Empty Rewards = No Processing**
```cpp
// QuestRewardSystem.cpp
if (Rewards.Rewards.Num() == 0 && Rewards.GrantedTags.Num() == 0)
{
    UE_LOG(LogTemp, Warning, TEXT("No rewards to grant"));
    return; // Exit early
}
```

If a phase/objective has no rewards configured, **nothing happens** (no logs, no events).

---

## ?? **Debugging Rewards**

### **Expected Log Sequence:**

```
[QuestEventBus] Event 'GameEvent.Dialogue.Started' matched objective 'O_ReturnToLianne'
[QuestProgressManager] Objective completed: O_ReturnToLianne
?????? QuestRewardSystem::ProcessQuestRewards called for Quest 'Q_HelpLianne'
   - Rewards count: 1
   ?? Granting reward: Type=2 (Experience), Amount=10
QuestRewardSystem: Grant experience 10 (Quest: Q_HelpLianne)

[QuestProgressManager] Phase 'P_Return' completed, advancing
?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_HelpLianne': 'P_Return' ? 'None'
?? Processing phase completion rewards for 'P_Return'
?????? QuestRewardSystem::ProcessQuestRewards called for Quest 'Q_HelpLianne'
   - Rewards count: 1
   ?? Granting reward: Type=2 (Experience), Amount=25
QuestRewardSystem: Grant experience 25 (Quest: Q_HelpLianne)

[QuestProgressManager] Quest 'Q_HelpLianne' completed
?????? QuestSubsystem::OnQuestCompletedInternal - Quest 'Q_HelpLianne' completed!
   ?? Quest Title: 'Help Lianne'
   ?? RewardsOnComplete.Rewards count: 2
   Reward[0]: Type=2, Amount=100, ItemId='None'
   Reward[1]: Type=0, Amount=1, ItemId='SpecialSword'
?????? QuestRewardSystem::ProcessQuestRewards called for Quest 'Q_HelpLianne'
   ?? Granting reward: Type=2 (Experience), Amount=100
 ?? Granting reward: Type=0 (Item), Amount=1
QuestRewardSystem: Grant item 'SpecialSword' x1 (Quest: Q_HelpLianne)
```

**3 separate `ProcessQuestRewards` calls = 3 reward moments!**

---

## ?? **Summary**

| Question | Answer |
|----------|--------|
| **When are objective rewards given?** | Immediately when objective completes |
| **When are phase rewards given?** | Immediately when phase completes (before next phase starts) |
| **When are quest rewards given?** | Immediately when quest completes (last phase finishes) |
| **Are rewards batched?** | No, each level grants rewards separately |
| **Can I delay rewards?** | Not by default (need custom RewardSystem override) |
| **What if no rewards configured?** | Nothing happens (silent skip) |
| **Do rewards stack in one frame?** | Yes, all 3 can happen in same frame if last objective completes last phase |

---

## ?? **Related Documentation**

- [QUEST_REWARDS_GUIDE.md](QUEST_REWARDS_GUIDE.md) — Full reward system reference
- [OBJECTIVE_EXECUTION_ORDER.md](OBJECTIVE_EXECUTION_ORDER.md) — How phases and objectives execute
- [BUILD_SUCCESS_REWARDS_FIXED.md](BUILD_SUCCESS_REWARDS_FIXED.md) — Recent reward fixes

---

**Last Updated:** 2025-11-01  
**Quest System Version:** 2.0  
**Status:** ? Fully Implemented
