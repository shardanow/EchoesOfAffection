# ?? **���������� � �������Ш� ���������!**

## ? **��� ����������:**

### **��������:**
`QuestRewardSystem::OnGrantItem` **������ ���������**, �� **�� ��������** items � ��������� ������!

```cpp
// �� (�����������):
void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Grant item '%s' x%d (Quest: %s)"), 
		*ItemId.ToString(), Amount, *QuestId.ToString());
	
	// TODO: Integrate with inventory system   ? ? �� �����������!
}
```

---

## ?? **�������:**

### **����������� ���������� � `InventoryComponent`:**

1. **������� ������** ? `UGameplayStatics::GetPlayerPawn()`
2. **������� InventoryComponent** ? �� ����� ������ (��� hard dependency)
3. **��������� ItemDataAsset** ? `StaticLoadObject()` � ����������� ������
4. **�������� AddItem** ? ����� reflection (`ProcessEvent`)

### **��� (�����):**

```cpp
void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
	// 1. ����� ������
	UWorld* World = GetWorld();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	
	// 2. ����� InventoryComponent
	UActorComponent* InventoryComp = nullptr;
	for (UActorComponent* Comp : PlayerPawn->GetComponents())
	{
		if (Comp && Comp->GetClass()->GetName() == TEXT("InventoryComponent"))
		{
			InventoryComp = Comp;
			break;
		}
	}
	
	// 3. ��������� ItemDataAsset
	UClass* ItemDataClass = FindObject<UClass>(nullptr, TEXT("/Script/InventorySystemCore.ItemDataAsset"));
	TArray<FString> AssetPaths = {
		FString::Printf(TEXT("/Game/Items/ItemData_%s.ItemData_%s"), *ItemId.ToString(), *ItemId.ToString()),
		FString::Printf(TEXT("/Game/Items/%s.%s"), *ItemId.ToString(), *ItemId.ToString()),
		FString::Printf(TEXT("/Game/Data/Items/%s.%s"), *ItemId.ToString(), *ItemId.ToString())
	};
	
	UObject* ItemData = nullptr;
	for (const FString& Path : AssetPaths)
	{
		ItemData = StaticLoadObject(ItemDataClass, nullptr, *Path);
		if (ItemData) break;
	}
	
	// 4. ������� AddItem ����� reflection
	UFunction* AddItemFunc = InventoryComp->FindFunction(FName("AddItem"));
	InventoryComp->ProcessEvent(AddItemFunc, &Params);
	
	UE_LOG(LogTemp, Warning, TEXT("? Added %dx '%s' to player inventory"), Amount, *ItemId.ToString());
}
```

---

## ?? **��� ������������:**

### **1. ���������, ��� ItemDataAsset ����������:**

� ����� ������� ������ ���� asset �� ������ �� �����:
- `/Game/Items/ItemData_RedApple` (��� ItemId = `RedApple`)
- `/Game/Items/RedApple`
- `/Game/Data/Items/RedApple`

### **2. ��������� ���� � ��������� �����:**

1. ������� ���������� (���� �����)
2. ������� ����� `Q_CollectApples`
3. ��������� ����� (���������� � Lianne)
4. **��������� ���������** - ������ ���������� 2? RedApple!

---

## ?? **��������� ����:**

```
[XX:XX:XX] ?? Last phase 'P_ReturnApples' completed - broadcasting phase change for rewards
[XX:XX:XX] ?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_CollectApples': 'P_ReturnApples' ? 'None'
[XX:XX:XX] ?? Processing phase completion rewards for 'P_ReturnApples'
[XX:XX:XX] ??? QuestRewardSystem::ProcessQuestRewards called
[XX:XX:XX]    Reward[0]: Type=0, Amount=2, ItemId='RedApple'
[XX:XX:XX] ?? Granting reward: Type=0
[XX:XX:XX] QuestRewardSystem: Grant item 'RedApple' x2 (Quest: Q_CollectApples)
[XX:XX:XX] QuestRewardSystem: Loaded ItemDataAsset from path '/Game/Items/ItemData_RedApple'
[XX:XX:XX] ? QuestRewardSystem: Added 2x 'RedApple' to player inventory (Result: 0)
```

---

## ?? **��������� ��������:**

### **1. ItemDataAsset �� ������:**

```
ERROR: QuestRewardSystem: Failed to load ItemDataAsset for 'RedApple' (tried 3 paths)
```

**�������:**
- ���������, ���������� �� asset � Content Browser
- ��������� ������������ ����� (������ ��������� � `ItemId` � QuestAsset)
- �������� ���� ���� � ������ `AssetPaths` � ����

### **2. InventoryComponent �� ������:**

```
ERROR: QuestRewardSystem: PlayerPawn has no InventoryComponent!
```

**�������:**
- ���������, ��� � ������ PlayerPawn ���� ��������� `InventoryComponent`
- ��������� Blueprint ������

### **3. AddItem �� ��������:**

```
ERROR: QuestRewardSystem: InventoryComponent has no AddItem function!
```

**�������:**
- ���������, ��� `UInventoryComponent` ����� ������� `AddItem`
- ��������� ��������� �������

---

## ?? **������ ������� ����������� � ���������!**

? Phase Rewards ��������
? Items ����������� �� Content Browser
? Items ����������� � InventoryComponent
? ������ ��������� ����������� �������������

**����������!** ????
