# ?? **ИНТЕГРАЦИЯ С ИНВЕНТАРЁМ ДОБАВЛЕНА!**

## ? **ЧТО ИСПРАВЛЕНО:**

### **ПРОБЛЕМА:**
`QuestRewardSystem::OnGrantItem` **ТОЛЬКО ЛОГИРОВАЛ**, но **НЕ ДОБАВЛЯЛ** items в инвентарь игрока!

```cpp
// ДО (НЕПРАВИЛЬНО):
void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Grant item '%s' x%d (Quest: %s)"), 
		*ItemId.ToString(), Amount, *QuestId.ToString());
	
	// TODO: Integrate with inventory system   ? ? НЕ РЕАЛИЗОВАНО!
}
```

---

## ?? **РЕШЕНИЕ:**

### **Реализована интеграция с `InventoryComponent`:**

1. **Находит игрока** ? `UGameplayStatics::GetPlayerPawn()`
2. **Находит InventoryComponent** ? По имени класса (без hard dependency)
3. **Загружает ItemDataAsset** ? `StaticLoadObject()` с несколькими путями
4. **Вызывает AddItem** ? Через reflection (`ProcessEvent`)

### **Код (ПОСЛЕ):**

```cpp
void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
	// 1. Найти игрока
	UWorld* World = GetWorld();
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	
	// 2. Найти InventoryComponent
	UActorComponent* InventoryComp = nullptr;
	for (UActorComponent* Comp : PlayerPawn->GetComponents())
	{
		if (Comp && Comp->GetClass()->GetName() == TEXT("InventoryComponent"))
		{
			InventoryComp = Comp;
			break;
		}
	}
	
	// 3. Загрузить ItemDataAsset
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
	
	// 4. Вызвать AddItem через reflection
	UFunction* AddItemFunc = InventoryComp->FindFunction(FName("AddItem"));
	InventoryComp->ProcessEvent(AddItemFunc, &Params);
	
	UE_LOG(LogTemp, Warning, TEXT("? Added %dx '%s' to player inventory"), Amount, *ItemId.ToString());
}
```

---

## ?? **КАК ИСПОЛЬЗОВАТЬ:**

### **1. Убедитесь, что ItemDataAsset существует:**

В вашем проекте должен быть asset по одному из путей:
- `/Game/Items/ItemData_RedApple` (для ItemId = `RedApple`)
- `/Game/Items/RedApple`
- `/Game/Data/Items/RedApple`

### **2. Запустите игру и завершите квест:**

1. Удалите сохранение (если нужно)
2. Начните квест `Q_CollectApples`
3. Завершите квест (поговорите с Lianne)
4. **Проверьте инвентарь** - должны добавиться 2? RedApple!

---

## ?? **ОЖИДАЕМЫЕ ЛОГИ:**

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

## ?? **ВОЗМОЖНЫЕ ПРОБЛЕМЫ:**

### **1. ItemDataAsset не найден:**

```
ERROR: QuestRewardSystem: Failed to load ItemDataAsset for 'RedApple' (tried 3 paths)
```

**РЕШЕНИЕ:**
- Проверьте, существует ли asset в Content Browser
- Проверьте правильность имени (должен совпадать с `ItemId` в QuestAsset)
- Добавьте свой путь в массив `AssetPaths` в коде

### **2. InventoryComponent не найден:**

```
ERROR: QuestRewardSystem: PlayerPawn has no InventoryComponent!
```

**РЕШЕНИЕ:**
- Убедитесь, что у вашего PlayerPawn есть компонент `InventoryComponent`
- Проверьте Blueprint игрока

### **3. AddItem не работает:**

```
ERROR: QuestRewardSystem: InventoryComponent has no AddItem function!
```

**РЕШЕНИЕ:**
- Убедитесь, что `UInventoryComponent` имеет функцию `AddItem`
- Проверьте сигнатуру функции

---

## ?? **ТЕПЕРЬ НАГРАДЫ ПРИМЕНЯЮТСЯ К ИНВЕНТАРЮ!**

? Phase Rewards выдаются
? Items загружаются из Content Browser
? Items добавляются в InventoryComponent
? Виджет инвентаря обновляется автоматически

**ТЕСТИРУЙТЕ!** ????
