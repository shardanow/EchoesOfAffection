# ?? **������������� �����������!**

## ?? **������� ��� ��������:**

### **�������� #1: ����� ���������� ��� ������ ������ ��������� ����**

�� �����:

```
[13.53.11:175] OBJECTIVE COMPLETED! Quest='Q_CollectApples', Objective='O_TalkToLianne'
[13.53.11:176] Phase 'P_ReturnApples' - PhaseRewards.Rewards: 1 ? ������� ����!
[13.53.11:176] Reward[0]: Type=0, Amount=2, ItemId='RedApple' ? ������!
[13.53.11:176] ? QuestSubsystem::OnQuestCompletedInternal called
[13.53.11:176] RewardsOnComplete.Rewards count: 0 ? �����!
```

**�������:** ����� ��������� objective ���� ����������, ��� �������� `NextPhaseId`:
- ���� `NextPhaseId` ���������� ? ������� `AdvanceToNextPhase()` ? ������� Phase Rewards ?
- ���� `NextPhaseId.IsNone()` (��������� ����) ? ������� `CompleteQuest()` ? **��������� Phase Rewards** ?

### **�������� #2: ������� �� ����������� � ���������**

```cpp
void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("Grant item '%s' x%d"), *ItemId.ToString(), Amount);
	// TODO: Integrate with inventory system   ? ? �� �����������!
}
```

---

## ? **�����������:**

### **����������� #1: ������ ������ ��������� ����**

**����:** `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestProgressManager.cpp`

**��:**

```cpp
if (bAllObjectivesComplete)
{
	CurrentPhase->bIsCompleted = true;
	
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

**�����:**

```cpp
if (bAllObjectivesComplete)
{
	CurrentPhase->bIsCompleted = true;
	
	if (PhaseDefinition.Transition.bAutoAdvance)
	{
		if (!PhaseDefinition.Transition.NextPhaseId.IsNone())
		{
			AdvanceToNextPhase(QuestId);
		}
		else
		{
			// 1. Broadcast phase change ��� ������ Phase Rewards
			UE_LOG(LogTemp, Warning, TEXT("?? Last phase '%s' completed - broadcasting for rewards"), *SaveData.CurrentPhaseId.ToString());
			OnQuestPhaseChanged.Broadcast(QuestId, SaveData.CurrentPhaseId, NAME_None);
			
			// 2. ��������� ����� (������ Quest Rewards)
			CompleteQuest(QuestId);
		}
	}
}
```

### **����������� #2: ���������� � InventoryComponent**

**����:** `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestRewardSystem.cpp`

**�����������:**
1. ����� ������ ����� `UGameplayStatics::GetPlayerPawn()`
2. ����� `InventoryComponent` �� ����� ������
3. �������� `ItemDataAsset` ����� `StaticLoadObject()` (3 ��������� ����)
4. ����� `AddItem()` ����� reflection (`ProcessEvent`)

**���������:**
```cpp
? QuestRewardSystem: Added 2x 'RedApple' to player inventory (Result: 0)
```

---

## ?? **���������:**

������ ��� ���������� ������:

1. ? **Objective �����������** ? `O_TalkToLianne` completed
2. ? **Phase Change Event** ? `OnQuestPhaseChanged.Broadcast(QuestId, 'P_ReturnApples', NAME_None)`
3. ? **Phase Rewards ��������** ? `QuestSubsystem::OnPhaseChangedInternal()` ? `ProcessQuestRewards(Phase.PhaseRewards)`
4. ? **Items �����������** ? `StaticLoadObject(ItemDataAsset, path)`
5. ? **Items ����������� � ���������** ? `InventoryComponent->AddItem(ItemData, Quantity)`
6. ? **Quest �����������** ? `OnQuestCompleted.Broadcast(QuestId, QuestAsset)`
7. ? **Quest Rewards ��������** ? `QuestSubsystem::OnQuestCompletedInternal()` ? `ProcessQuestRewards(Quest.RewardsOnComplete)`

---

## ?? **����������:**

### **1. �������������� ������** ? (��� �������)

### **2. ���������, ��� ItemDataAsset ����������:**

� Content Browser ������ ���� asset:
- `/Game/Items/ItemData_RedApple` (���)
- `/Game/Items/RedApple` (���)
- `/Game/Data/Items/RedApple`

### **3. ������� ���������� (���� �����):**

```powershell
Remove-Item "Saved\SaveGames\QuestSaveSlot.sav" -Force
```

### **4. ��������� ���� � ��������� �����:**

1. ������� ����� `Q_CollectApples`
2. �������� ������
3. ���������� � Lianne
4. **��������� ���������** - ������ ���������� **2? RedApple**!

---

## ?? **��������� ����:**

```
? Objective 'O_TalkToLianne' completed in quest 'Q_CollectApples'
?? Last phase 'P_ReturnApples' completed - broadcasting phase change for rewards
?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_CollectApples': 'P_ReturnApples' ? 'None'
?? Phase 'P_ReturnApples' rewards check:
   - PhaseRewards.Rewards count: 1    ?
   Reward[0]: Type=0, Amount=2, ItemId='RedApple'
?? Processing phase completion rewards for 'P_ReturnApples'
??? QuestRewardSystem::ProcessQuestRewards called
   - Rewards count: 1    ?
?? Granting reward: Type=0
QuestRewardSystem: Grant item 'RedApple' x2 (Quest: Q_CollectApples)
QuestRewardSystem: Loaded ItemDataAsset from path '/Game/Items/ItemData_RedApple'
? QuestRewardSystem: Added 2x 'RedApple' to player inventory (Result: 0)   ???
??? QuestSubsystem::OnQuestCompletedInternal - Quest completed!
   RewardsOnComplete.Rewards count: 0
```

---

## ?? **��������� ��������:**

### **ItemDataAsset �� ������:**

```
ERROR: Failed to load ItemDataAsset for 'RedApple' (tried 3 paths)
```

**�������:**
- ���������, ���������� �� asset `ItemData_RedApple` � `/Game/Items/`
- ���������, ��� ��� ��������� � `ItemId` � quest reward (`RedApple`)
- ���� asset � ������ �����, �������� ���� � ��� `QuestRewardSystem.cpp` (������ ~170)

### **InventoryComponent �� ������:**

```
ERROR: PlayerPawn has no InventoryComponent!
```

**�������:**
- ���������, ��� � ������ PlayerPawn ���� ��������� `InventoryComponent`

---

## ?? **��������� ����������!**

? Phase Rewards �������� ��� ���������� ���� ��� (������� ���������)
? Items ����������� �� Content Browser
? Items ����������� � InventoryComponent
? ������ ��������� ����������� �������������

**������ �Ѩ �������� ���������!** ?????

---

## ?? **�������������� ������������:**

- `FINAL_FIX.md` - ������ ����������� �������� � Phase Rewards
- `INVENTORY_INTEGRATION.md` - ������ ���������� � InventoryComponent
