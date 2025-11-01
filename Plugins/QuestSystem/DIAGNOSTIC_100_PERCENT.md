# ?? **100% �����������: �������� �������� PHASEREWARDS**

## ? **��� ���������:**

### **1. ��������� ����������� � `UQuestAsset::GetPhase()`**

��� ������ ������ `GetPhase()` ������ ����������:
- ��� ���� � ������
- ���������� ������ � `PhaseRewards` ��� ������ ����
- ������ ������ ������� (Type, Amount, ItemId)

### **2. ��������� ����������� � `QuestAssetLoader::OnQuestAssetLoadComplete()`**

��� �������� ������ �� .uasset ����� ������ ����������:
- �������� ������������ ������
- ��� ���� � �� PhaseRewards
- ������ ������ �������

---

## ?? **��� ���������:**

### **��� 1: ������������� ������**

1. �������� Unreal Editor
2. �������� Visual Studio / Rider
3. ������������� ������ (Ctrl+Shift+B)
4. ��������� �������� ����������

### **��� 2: ������� ����������**

```powershell
Remove-Item "Saved\SaveGames\QuestSaveSlot.sav" -Force
```

### **��� 3: ��������� ����**

1. �������� Unreal Editor
2. ��������� PIE (Play In Editor)
3. **������� ����� Q_CollectApples**

---

## ?? **��� �� ������� � �����:**

### **��� �������� ������:**

```
?????? QUEST ASSET LOADED: 'Q_CollectApples' (QuestId='Q_CollectApples')
   Total Phases in asset: 2
   Phase[0]: 'P_CollectApples' (Title='Collect Apples')
  PhaseRewards.Rewards.Num() = 0    ? ��� ���������� ������!
      PhaseRewards.GrantedTags.Num() = 0
  ?? Phase 'P_CollectApples' has NO rewards configured!
   Phase[1]: 'P_ReturnApples' (Title='Return Apples')
    PhaseRewards.Rewards.Num() = X    ? ������� ������ ���������!
    PhaseRewards.GrantedTags.Num() = 0
      ? Phase 'P_ReturnApples' HAS REWARDS:
         Reward[0]: Type=1, Amount=50, ItemId='None'
         Reward[1]: Type=0, Amount=2, ItemId='RedApple'
```

### **��� ����� ���� P_CollectApples ? P_ReturnApples:**

```
?? UQuestAsset::GetPhase called for PhaseId='P_CollectApples' in Quest='Q_CollectApples'
   Total phases in asset: 2
   Phase[0]: PhaseId='P_CollectApples', Title='Collect Apples'
      - Objectives: 2
      - PhaseRewards.Rewards: 0    ? ��� ���������� ������!
      - PhaseRewards.GrantedTags: 0
   Phase[1]: PhaseId='P_ReturnApples', Title='Return Apples'
      - Objectives: 1
      - PhaseRewards.Rewards: X    ? ������� ������ ���������!
      - PhaseRewards.GrantedTags: 0
     Reward[0]: Type=1, Amount=50, ItemId='None'
   ? MATCH FOUND! Returning phase 'P_CollectApples'
```

�����:

```
?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_CollectApples': 'P_CollectApples' ? 'P_ReturnApples'
?? Phase 'P_CollectApples' rewards check:
   - PhaseRewards.Rewards count: 0    ? ��� ���������� ������!
   - PhaseRewards.GrantedTags count: 0
```

---

## ?? **��� �����������:**

### **1. �������� �� .uasset �����**

���� ��� �������� ������ �� ������:
```
Phase[1]: 'P_ReturnApples' (Title='Return Apples')
   PhaseRewards.Rewards.Num() = 0    ? �����!
```

**��������:** Data Asset **�� ����������** � ���������! Unreal �� �������� `PhaseRewards` � .uasset ����!

**�������:**
1. �������� `Q_CollectApples` � ���������
2. ���������, ��� ������� **������� ���������** � Phase Rewards
3. **��������� �����** (Ctrl+S)
4. **������������** (File ? Save All)
5. ���������� �����

### **2. ����������� ������ � GetPhase()**

���� ��� �������� �� ��, �� ��� ������ `GetPhase()` ������� ���������:

```
?????? QUEST ASSET LOADED: 'Q_CollectApples'
   Phase[1]: 'P_ReturnApples'
      PhaseRewards.Rewards.Num() = 2    ? ���� �������!

?? UQuestAsset::GetPhase called for PhaseId='P_CollectApples'
   Phase[1]: PhaseId='P_ReturnApples'
      - PhaseRewards.Rewards: 0    ? �������!
```

**��������:** ����������� ��������� �������� �����������!

**��� ����������** ��� ���������� ���������� `USTRUCT`, �� ���� ��� �������� - �������� � Unreal Engine!

### **3. �������� � QuestSubsystem**

���� � `GetPhase()` �� ��, �� � `OnPhaseChangedInternal()` ������� ���������:

```
?? UQuestAsset::GetPhase called for PhaseId='P_CollectApples'
   ? MATCH FOUND! Returning phase 'P_CollectApples'
     Reward[0]: Type=1, Amount=50, ItemId='None'    ? ����!

?? QuestSubsystem::OnPhaseChangedInternal
?? Phase 'P_CollectApples' rewards check:
   - PhaseRewards.Rewards count: 0    ? �������!
```

**��������:** ����������� ����� �������� �������� �����������!

**��� ����������** ��� ���������� �������� �� ��������!

---

## ? **��������� ��������:**

���� �� **���� ��� ������** ������� **= 0**:

```
?????? QUEST ASSET LOADED:
   Phase[1]: 'P_ReturnApples'
      PhaseRewards.Rewards.Num() = 0    ? �����!

?? UQuestAsset::GetPhase:
   Phase[1]: PhaseId='P_ReturnApples'
      - PhaseRewards.Rewards: 0    ? �����!

?? OnPhaseChangedInternal:
   - PhaseRewards.Rewards count: 0    ? �����!
```

**�����:** ������� **�� ���������** � Data Asset!

**�������:** �������� `Q_CollectApples` � Unreal Editor � **��������� �������**, ��������� �� ������� � Phase Rewards!

---

## ?? **��������� ��������� (���� ������� ���������):**

```
?????? QUEST ASSET LOADED: 'Q_CollectApples'
   Phase[1]: 'P_ReturnApples'
      PhaseRewards.Rewards.Num() = 2    ?
    ? Phase 'P_ReturnApples' HAS REWARDS:
         Reward[0]: Type=1, Amount=50, ItemId='None'
         Reward[1]: Type=0, Amount=2, ItemId='RedApple'

?? UQuestAsset::GetPhase called for PhaseId='P_CollectApples'
   Phase[1]: PhaseId='P_ReturnApples'
      - PhaseRewards.Rewards: 2    ?
     Reward[0]: Type=1, Amount=50, ItemId='None'

?? QuestSubsystem::OnPhaseChangedInternal
?? Phase 'P_CollectApples' rewards check:
   - PhaseRewards.Rewards count: 2    ?
   Reward[0]: Type=1, Amount=50, ItemId='None'
   Reward[1]: Type=0, Amount=2, ItemId='RedApple'
?? Processing phase completion rewards for 'P_CollectApples'
```

---

## ?? **�������� ������� � ����������!**

## ?? **��� ��� � ����, � �� � DATA ASSET!**

### **?? ��� ���� ����������:**

�� �����:

```
[13.47.57:369] OBJECTIVE COMPLETED! Quest='Q_CollectApples', Objective='O_TalkToLianne'
[13.47.57:369] Phase 'P_ReturnApples' rewards check: PhaseRewards.Rewards: 1    ? ����!
[13.47.57:369] ? QuestRewardSystem::ProcessQuestRewards called - Rewards count: 0    ? �����!
```

**��������:** `OnObjectiveCompletedInternal()` ������� `ProcessQuestRewards()` � **Objective rewards** ������ **Phase rewards**!

```cpp
// ���� (�����������):
void UQuestSubsystem::OnObjectiveCompletedInternal(FName QuestId, FName ObjectiveId)
{
	RewardSystem->ProcessQuestRewards(Obj.Signals.OnCompleteRewards, QuestId);  // ? ������� �� OBJECTIVE!
}
```

**Objective rewards** (`Obj.Signals.OnCompleteRewards`) ���� �������, ������� ������� �� ����������!

**Phase rewards** (`Phase.PhaseRewards`) ���� ���������, �� ��� �� �����������!

---

## ? **�����������:**

������ ��� **��������� ���������**:
- **Objective Rewards** ? �������� ��� ���������� objective (���� ���������)
- **Phase Rewards** ? �������� ��� ����� ���� (� `OnPhaseChangedInternal`)

```cpp
// ����� (���������):
void UQuestSubsystem::OnObjectiveCompletedInternal(FName QuestId, FName ObjectiveId)
{
	// Process objective-specific rewards (NOT phase rewards!)
	// Phase rewards are processed in OnPhaseChangedInternal()
	if (Obj.Signals.OnCompleteRewards.Rewards.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("?? Processing objective-specific rewards"));
		RewardSystem->ProcessQuestRewards(Obj.Signals.OnCompleteRewards, QuestId);
	}
}
```

---

## ?? **���������:**

������ ��� ���������� ������:
1. **Objective �����������** ? �������� objective rewards (���� ����)
2. **���� �����������** ? ���������� `OnPhaseChangedInternal()`
3. **Phase rewards ��������** ? `ProcessQuestRewards(Phase.PhaseRewards, QuestId)`

---

## ?? **��� ������ ������:**

1. **������������� ������** (Visual Studio / Rider)
2. **������� ����������:**
   ```powershell
   Remove-Item "Saved\SaveGames\QuestSaveSlot.sav"
   ```
3. **��������� ����** � ��������� �����
4. **��������� ����** - ������ ������ ���������� �������!

---

## ?? **��������� ����:**

```
? Objective 'O_TalkToLianne' completed in quest 'Q_CollectApples'
?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_CollectApples': 'P_ReturnApples' ? 'None'
?? Phase 'P_ReturnApples' rewards check:
   - PhaseRewards.Rewards count: 1    ?
   Reward[0]: Type=0, Amount=2, ItemId='RedApple'
?? Processing phase completion rewards for 'P_ReturnApples'
??? QuestRewardSystem::ProcessQuestRewards called for Quest 'Q_CollectApples'
   - Rewards count: 1    ? ������� ��������!
   - GrantedTags count: 0
   Reward[0]: Type=0, Amount=2, ItemId='RedApple'
?? Processing reward - Type: 0, Amount: 2, ItemId: RedApple
```

---

**��� ���������! ���������������� � ���������!** ??
