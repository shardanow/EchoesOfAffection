# ?? **�������� ������! ������� ����� ����������!**

## ?? **��� ���� �� ���:**

### **��� � ������ ���������� ��������� ���� ������**

����� ��������� objective � **��������� ����** ������ ����������, ��� �������� `NextPhaseId`:

- **���� ���� NextPhaseId** ? ������� `AdvanceToNextPhase()` ? ����� ���� ? `OnPhaseChangedInternal()` ? **Phase Rewards ����������** ?
- **���� NextPhaseId ������** (��������� ����) ? ������� `CompleteQuest()` �������� ? **Phase Rewards �� ����������** ?

---

## ?? **��� ����������:**

### **����:** `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestProgressManager.cpp`

### **������ 410-430** (��������)

#### **��:**

```cpp
if (bAllObjectivesComplete)
{
	CurrentPhase->bIsCompleted = true;
	CurrentPhase->CompletionTime = FDateTime::Now();

	if (PhaseDefinition.Transition.bAutoAdvance)
	{
		if (!PhaseDefinition.Transition.NextPhaseId.IsNone())
		{
			AdvanceToNextPhase(QuestId);
		}
		else
		{
			CompleteQuest(QuestId);  // ? ��������� PHASE REWARDS!
		}
	}
}
```

#### **�����:**

```cpp
if (bAllObjectivesComplete)
{
	CurrentPhase->bIsCompleted = true;
	CurrentPhase->CompletionTime = FDateTime::Now();

	if (PhaseDefinition.Transition.bAutoAdvance)
	{
		if (!PhaseDefinition.Transition.NextPhaseId.IsNone())
		{
			// Has next phase - advance and process phase rewards in OnPhaseChanged
			AdvanceToNextPhase(QuestId);
		}
		else
		{
			// No next phase - this is the last phase
			// Trigger phase change event to process phase rewards BEFORE completing quest
			UE_LOG(LogTemp, Warning, TEXT("?? Last phase '%s' completed - broadcasting phase change for rewards"), *SaveData.CurrentPhaseId.ToString());
			OnQuestPhaseChanged.Broadcast(QuestId, SaveData.CurrentPhaseId, NAME_None);
			
			// Now complete the quest (quest completion rewards processed in OnQuestCompleted)
			CompleteQuest(QuestId);
		}
	}
}
```

---

## ?? **������ ������ �������� ���:**

### **��� ���������� ��������� ����:**

1. **Objective �����������** ? `O_TalkToLianne` completed ?
2. **��������:** ��� objectives ���������? ? �� ?
3. **��������:** ���� NextPhaseId? ? ��� (��������� ����) ?
4. **Broadcast `OnQuestPhaseChanged`** ? `('Q_CollectApples', 'P_ReturnApples', NAME_None)` ?
5. **`QuestSubsystem::OnPhaseChangedInternal()` ����������** ?
6. **Phase Rewards ��������������** ? `ProcessQuestRewards(Phase.PhaseRewards)` ?
7. **Quest �����������** ? `CompleteQuest()` ?
8. **Quest Rewards ��������������** ? `ProcessQuestRewards(Quest.RewardsOnComplete)` ?

---

## ?? **����������:**

### **1. ������������� ������:**

```
Visual Studio ? Build ? Build Solution (Ctrl+Shift+B)
���
Rider ? Build ? Build Solution
```

### **2. ������� ������ ����������:**

```powershell
Remove-Item "Saved\SaveGames\QuestSaveSlot.sav" -Force
```

### **3. ��������� ����:**

1. �������� Unreal Editor
2. Play In Editor (PIE)
3. ������� ����� `Q_CollectApples`
4. ��������� ��� ����
5. ���������� � Lianne

---

## ?? **��������� ��������� � �����:**

```
[XX:XX:XX] ? Objective 'O_TalkToLianne' completed in quest 'Q_CollectApples'
[XX:XX:XX] ?? Last phase 'P_ReturnApples' completed - broadcasting phase change for rewards
[XX:XX:XX] ?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_CollectApples': 'P_ReturnApples' ? 'None'
[XX:XX:XX] ?? Phase 'P_ReturnApples' rewards check:
[XX:XX:XX]    - PhaseRewards.Rewards count: 1    ?
[XX:XX:XX]    Reward[0]: Type=0, Amount=2, ItemId='RedApple'
[XX:XX:XX] ?? Processing phase completion rewards for 'P_ReturnApples'
[XX:XX:XX] ??? QuestRewardSystem::ProcessQuestRewards called for Quest 'Q_CollectApples'
[XX:XX:XX]    - Rewards count: 1    ?
[XX:XX:XX]    - GrantedTags count: 0
[XX:XX:XX]    Reward[0]: Type=0, Amount=2, ItemId='RedApple'
[XX:XX:XX] ?? Processing reward - Type: 0, Amount: 2, ItemId: RedApple
[XX:XX:XX] ??? QuestSubsystem::OnQuestCompletedInternal - Quest 'Q_CollectApples' completed!
[XX:XX:XX]    RewardsOnComplete.Rewards count: 0 (no quest-level rewards)
```

---

## ?? **����������� ������:**

### **��������:**
- ��������� ���� ������ ����������� ��� ������ Phase Rewards

### **�������:**
- ��� `NextPhaseId.IsNone()` ��� ������� `CompleteQuest()` ��������, ����� `OnPhaseChangedInternal()`

### **�������:**
- �������� broadcast `OnQuestPhaseChanged` ����� `CompleteQuest()` ��� ��������� ����

### **���������:**
- Phase Rewards ������ �������� ��� ���� ���, ������� ���������
- Quest Rewards �������� �������� ��� ���������� ������

---

## ? **��������:**

����� ���������� � ������������ �� ������ �������:
- ? ���� ���������� ������ ������ ����
- ? `ProcessQuestRewards` ���������� � `Rewards count: 1`
- ? ������� `2? RedApple` ��������������

**�Ѩ ������! ������������ � ����������!** ????
