# How to Add Rewards to Quest — Step-by-Step Guide

## ?? **Quick Summary**

Your quest completes successfully, but **NO rewards are granted** because `RewardsOnComplete` is empty in the Data Asset.

**Fix:** Open Quest Data Asset and fill the `RewardsOnComplete` array.

---

## ?? **Step-by-Step Instructions**

### **Step 1: Open Quest Data Asset**

```
1. In Content Browser, navigate to:
   Content/Game/Core/Subsystems/QuestSystem/

2. Find and double-click:
   DA_Quest_Apples
```

---

### **Step 2: Scroll to "Rewards On Complete" Section**

```
In the Details panel:
1. Scroll down to find: "Quest" category
2. Look for: "Rewards On Complete" (FQuestRewardSet)
```

---

### **Step 3: Add Rewards**

#### **Option A: Add Experience Reward**

```
1. Click "+" next to "Rewards" array
2. Set:
   - Reward Type: Experience
   - Amount: 50
```

#### **Option B: Add Currency Reward**

```
1. Click "+" next to "Rewards" array again
2. Set:
   - Reward Type: Currency
   - Amount: 100
```

#### **Option C: Add Item Reward**

```
1. Click "+" next to "Rewards" array
2. Set:
   - Reward Type: Item
   - Item Id: GoldCoin (or any valid item ID)
   - Amount: 5
```

#### **Option D: Add Affection Bonus**

```
1. Click "+" next to "Rewards" array
2. Set:
   - Reward Type: AffectionBonus
 - NPC Id: Lianne
   - Amount: 10.0
```

---

### **Step 4: (Optional) Add Granted Tags**

```
1. Expand "Granted Tags" (FGameplayTagContainer)
2. Click "+" to add a tag
3. Search and select:
   Quest.Event.Completed
   or
   Quest.CollectApples.Completed
```

**Purpose:** These tags can be used as conditions for other quests (e.g., "unlock after completing previous quest")

---

### **Step 5: Save**

```
1. Press Ctrl+S
2. Or click "Save" button in toolbar
```

---

## ?? **Example Configuration**

### **Basic Rewards (Recommended for testing)**

```
RewardsOnComplete:
  Rewards:
    [0] Reward Type: Experience
Amount: 50
        
    [1] Reward Type: Currency
        Amount: 100
  
  Granted Tags:
    [0] Quest.Event.Completed
```

### **Advanced Rewards**

```
RewardsOnComplete:
  Rewards:
    [0] Reward Type: Experience
        Amount: 100
        
    [1] Reward Type: Currency
        Amount: 500
      
    [2] Reward Type: Item
        Item Id: SpecialSword
        Amount: 1
        
    [3] Reward Type: AffectionBonus
   NPC Id: Lianne
      Amount: 20.0
  
  Granted Tags:
  [0] Quest.CollectApples.Complete
    [1] Quest.Event.Completed
```

---

## ?? **Verify in Logs**

After adding rewards, run PIE and complete the quest. You should see:

### **? BEFORE (Empty)**
```
?? RewardsOnComplete.Rewards count: 0
?? No rewards to grant!
```

### **? AFTER (With Rewards)**
```
?? RewardsOnComplete.Rewards count: 2
   Reward[0]: Type=2 (Experience), Amount=50
   Reward[1]: Type=1 (Currency), Amount=100
?? Granting reward: Type=2, for Quest 'Q_CollectApples'
QuestRewardSystem: Grant experience 50 (Quest: Q_CollectApples)
?? Granting reward: Type=1, for Quest 'Q_CollectApples'
QuestRewardSystem: Grant currency 100 (Quest: Q_CollectApples)
```

---

## ?? **Phase Rewards (Optional)**

You can also add rewards for **phase completion**:

```
1. Open DA_Quest_Apples
2. Expand "Phases" array
3. Select Phase[0] "P_CollectApples"
4. Scroll to "Phase Rewards"
5. Add rewards same way:
   - Click "+" next to "Rewards"
   - Set Reward Type and Amount
```

**Example:**
```
Phase: P_CollectApples
  Phase Rewards:
    Rewards:
      [0] Reward Type: Experience
      Amount: 25
```

**Result:** Player gets +25 XP when completing the phase, PLUS main quest rewards when finishing entire quest.

---

## ?? **Objective Rewards (Optional)**

Rewards for individual objectives:

```
1. Open DA_Quest_Apples
2. Expand "Phases" ? "Objectives" array
3. Select Objective[0] "O_CollectApples"
4. Expand "Signals"
5. Find "On Complete Rewards"
6. Add rewards same way
```

**Example:**
```
Objective: O_CollectApples
  Signals:
    On Complete Rewards:
      Rewards:
        [0] Reward Type: Experience
      Amount: 10
```

**Result:** Player gets +10 XP **immediately** when collecting 3rd apple (before quest completes).

---

## ?? **Reward Types Reference**

| Reward Type | Required Fields | Example |
|-------------|----------------|---------|
| **Item** | `ItemId`, `Amount` | `ItemId=RedApple`, `Amount=5` |
| **Currency** | `Amount` | `Amount=100` (gold/money) |
| **Experience** | `Amount` | `Amount=50` (XP) |
| **StatModifier** | `StatTag`, `Amount` | `StatTag=Player.Stat.Strength`, `Amount=5` |
| **AffectionBonus** | `NpcId`, `Amount` | `NpcId=Lianne`, `Amount=10.0` |
| **UnlockDialog** | `NpcId`, `DialogBranchId` | `NpcId=Lianne`, `DialogBranchId=Friendship_Tier2` |
| **UnlockRecipe** | `RecipeId` | `RecipeId=ApplePie` |
| **CustomEffect** | `BPEventName` | `BPEventName=OnQuestComplete_Special` |

---

## ?? **Important Notes**

1. **Rewards are NOT integrated by default!**
   - They are **logged** but not applied to inventory/stats
   - You need to override `QuestRewardSystem` in Blueprint or C++
   - See: [QUEST_REWARDS_GUIDE.md](QUEST_REWARDS_GUIDE.md) for integration

2. **Default behavior (logs only):**
   ```
   QuestRewardSystem: Grant item 'RedApple' x5 (Quest: Q_CollectApples)
   // ? Logged, but item NOT added to inventory yet
   ```

3. **To actually give rewards:**
 - Create BP_QuestRewardSystem (parent: QuestRewardSystem)
   - Override `OnGrantItem`, `OnGrantCurrency`, etc.
   - Connect to your Inventory/Economy systems

---

## ?? **Quick Test**

**Minimal setup to see rewards working:**

```
1. Open DA_Quest_Apples
2. Add ONE Experience reward:
   - Reward Type: Experience
   - Amount: 50
3. Save (Ctrl+S)
4. Run PIE
5. Complete quest
6. Check logs for:
   "?? Granting reward: Type=2 (Experience), Amount=50"
```

If you see this log, **rewards are working!** ?

---

## ?? **Related Documentation**

- [QUEST_REWARDS_GUIDE.md](QUEST_REWARDS_GUIDE.md) — Full reward system guide
- [REWARD_TIMING_LOGIC.md](REWARD_TIMING_LOGIC.md) — When rewards are given
- [OBJECTIVE_EXECUTION_ORDER.md](OBJECTIVE_EXECUTION_ORDER.md) — How phases work

---

**Last Updated:** 2025-11-01  
**Status:** ? System working, just needs configuration!  
**Next:** Add rewards to DA and test! ??
