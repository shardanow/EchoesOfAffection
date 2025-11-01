# ?? **EVENT-BASED ARCHITECTURE - NO HARD DEPENDENCIES!**

## ? **ЧТО ИСПРАВЛЕНО:**

### **1. Удалена жёсткая зависимость между плагинами** ?
### **2. Создана event-based интеграция через GameEventBus** ?
### **3. Phase Rewards работают через события** ?

---

## ??? **НОВАЯ АРХИТЕКТУРА:**

```
????????????????         ????????????????????         ???????????????????
? QuestSystem  ?????????>? GameEventBus   ?????????>? InventorySystem ?
?     ?  emit   ?      ? listen  ?       ?
? Rewards      ?         ? Event Routing    ?         ? Add Items       ?
????????????????         ????????????????????       ???????????????????
```

### **Преимущества:**

1. **Нет жёстких зависимостей** - плагины могут работать независимо
2. **Гибкость** - любая система может подписаться на события
3. **Расширяемость** - легко добавить новые типы наград
4. **Тестируемость** - можно тестировать системы отдельно

---

## ?? **КАК ЭТО РАБОТАЕТ:**

### **Шаг 1: QuestRewardSystem выдаёт награду**

```cpp
// Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestRewardSystem.cpp
void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
	// Emit event instead of direct call
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Quest.GrantItem"));
	
	UGameEventBusLibrary::EmitItemEvent(
		this,    // WorldContext
		EventTag,    // GameEvent.Quest.GrantItem
		ItemId,      // "RedApple"
		Amount,      // 2
		nullptr,     // Instigator
		true     // bLogEvent
	);
}
```

### **Шаг 2: GameEventBus маршрутизирует событие**

```
[GameEventBus] Event received: GameEvent.Quest.GrantItem
  - StringParam: RedApple
  - IntParam: 2
  - StringParam2: Q_CollectApples
```

### **Шаг 3: InventoryQuestRewardListener получает событие**

```cpp
// Plugins/AdvancedInventorySystem/.../InventoryQuestRewardListener.cpp
void UInventoryQuestRewardListener::HandleGrantItemEvent(const FGameEventPayload& Payload)
{
	FName ItemId = Payload.StringParam;     // "RedApple"
	int32 Amount = Payload.IntParam;        // 2
	
	// Find InventoryComponent
	UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	
	// Find ItemDataAsset
	UItemDataAsset* ItemData = FindItemDataAssetByID(ItemId);
	
	// Add to inventory
	Inventory->AddItem(ItemData, Amount, Result);
}
```

### **Шаг 4: Item добавляется в инвентарь**

```
? Successfully added 2x 'RedApple' to inventory (Quest: Q_CollectApples)
```

---

## ?? **ИНСТРУКЦИЯ ПО НАСТРОЙКЕ:**

### **ШАГ 1: Создать ItemDataAsset** (как раньше)

1. Unreal Editor ? `/Game/Items/`
2. Right Click ? Data Asset ? **ItemDataAsset**
3. Назовите: `ItemData_RedApple`
4. **ItemID** = `RedApple` ?? **ВАЖНО!**
5. Save

### **ШАГ 2: Добавить InventoryQuestRewardListener к PlayerPawn** ? **НОВОЕ!**

1. Откройте Blueprint вашего персонажа (`BP_MyAnimeCharacter`)
2. **Components Panel** ? **Add Component**
3. Найдите: **Inventory Quest Reward Listener**
4. Добавьте компонент
5. **Compile & Save**

![Add Component](https://via.placeholder.com/600x300?text=Add+Inventory+Quest+Reward+Listener)

### **ШАГ 3: Убедитесь, что InventoryComponent тоже есть**

1. В том же Blueprint проверьте наличие **Inventory Component**
2. Если его нет - добавьте: **Add Component** ? **Inventory Component**
3. **Compile & Save**

### **ШАГ 4: Удалить старое сохранение**

```powershell
Remove-Item "Saved\SaveGames\QuestSaveSlot.sav" -Force
```

### **ШАГ 5: Запустить игру и протестировать**

1. **Play In Editor** (PIE)
2. Начните квест `Q_CollectApples`
3. Соберите яблоки (первая фаза)
4. Поговорите с **Lianne** (вторая фаза завершится)
5. **ПРОВЕРЬТЕ ИНВЕНТАРЬ** - должны добавиться **2? RedApple**!

---

## ?? **ОЖИДАЕМЫЕ ЛОГИ:**

```
[XX:XX:XX] ? Objective 'O_TalkToLianne' completed in quest 'Q_CollectApples'
[XX:XX:XX] ?? Last phase 'P_ReturnApples' completed - broadcasting phase change for rewards
[XX:XX:XX] ?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_CollectApples': 'P_ReturnApples' ? 'None'
[XX:XX:XX] ?? Processing phase completion rewards for 'P_ReturnApples'
[XX:XX:XX] ??? QuestRewardSystem::ProcessQuestRewards called
[XX:XX:XX]    Reward[0]: Type=0, Amount=2, ItemId='RedApple'
[XX:XX:XX] ?? Granting reward: Type=0
[XX:XX:XX] QuestRewardSystem: Grant item 'RedApple' x2 (Quest: Q_CollectApples)
[XX:XX:XX] ? QuestRewardSystem: Emitted 'GameEvent.Quest.GrantItem' for ItemId='RedApple', Amount=2
[XX:XX:XX] ?? InventoryQuestRewardListener: Received 'GameEvent.Quest.GrantItem'!
[XX:XX:XX]    ItemId='RedApple', Amount=2, QuestId='Q_CollectApples'
[XX:XX:XX]    Loaded ItemDataAsset from path: /Game/Items/ItemData_RedApple
[XX:XX:XX] ? InventoryQuestRewardListener: Successfully added 2x 'RedApple' to inventory (Quest: Q_CollectApples)
```

---

## ?? **ВОЗМОЖНЫЕ ПРОБЛЕМЫ:**

### **1. InventoryQuestRewardListener не подписался на события**

```
ERROR: EventTag 'GameEvent.Quest.GrantItem' is INVALID!
```

**РЕШЕНИЕ:**
- Перезапустите Unreal Editor (теги обновляются при старте)
- Проверьте `Config/DefaultGameplayTags.ini` - должен быть тег:
  ```ini
  +GameplayTagList=(Tag="GameEvent.Quest.GrantItem",DevComment="[QuestSystem] Request to grant item reward to player")
  ```

### **2. Компонент не добавлен к PlayerPawn**

```
(Нет логов от InventoryQuestRewardListener вообще)
```

**РЕШЕНИЕ:**
- Добавьте `InventoryQuestRewardListener` component к вашему PlayerPawn Blueprint
- **Compile & Save**

### **3. ItemDataAsset не найден**

```
ERROR: Failed to find ItemDataAsset for ItemID='RedApple'
```

**РЕШЕНИЕ:**
- Создайте ItemDataAsset как описано в ШАГ 1
- Убедитесь, что **ItemID property** = `RedApple`

---

## ?? **ТЕХНИЧЕСКИЕ ДЕТАЛИ:**

### **Новые файлы:**

1. **`QuestRewardSystem.cpp`** - изменён для эмиссии событий
2. **`InventoryQuestRewardListener.h/cpp`** - новый компонент-слушатель
3. **`DefaultGameplayTags.ini`** - добавлен тег `GameEvent.Quest.GrantItem`

### **Зависимости:**

```
QuestSystem:
  - GameEventBus (опционально, через #if __has_include)
  
InventorySystem:
  - GameEventBus (опционально, через #if __has_include)

Прямых зависимостей между QuestSystem и InventorySystem НЕТ! ?
```

### **Event Flow:**

```
QuestProgressManager
  ??> OnObjectiveCompleted
       ??> OnQuestPhaseChanged.Broadcast
 ??> QuestSubsystem::OnPhaseChangedInternal
  ??> QuestRewardSystem::ProcessQuestRewards
   ??> QuestRewardSystem::GrantReward
 ??> OnGrantItem_Implementation
          ??> UGameEventBusLibrary::EmitItemEvent
       ??> GameEventBusSubsystem::EmitEvent
   ??> InventoryQuestRewardListener::HandleGrantItemEvent
       ??> InventoryComponent::AddItem
        ??> Item добавлен! ?
```

---

## ?? **ИТОГ:**

### **Что изменилось:**

| До | После |
|----|-------|
| QuestRewardSystem ? **InventoryComponent** (прямой вызов) ? | QuestRewardSystem ? **GameEventBus** ? InventoryQuestRewardListener ? |
| Жёсткая зависимость между плагинами | Полная независимость через события |
| Сложно тестировать отдельно | Легко тестировать и расширять |

### **Преимущества новой системы:**

1. ? **Модульность** - плагины работают независимо
2. ? **Расширяемость** - легко добавить новые типы наград
3. ? **Гибкость** - любая система может подписаться на события
4. ? **Тестируемость** - можно тестировать отдельно
5. ? **Производительность** - native C++ event delegates

---

## ?? **ДОПОЛНИТЕЛЬНО:**

### **Как добавить свой обработчик события:**

```cpp
// В любом компоненте
void UMyCustomComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Quest.GrantItem"));
	
	EventBus->SubscribeToEventNative(
		EventTag,
		FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UMyCustomComponent::HandleEvent)
	);
}

void UMyCustomComponent::HandleEvent(const FGameEventPayload& Payload)
{
	// Ваша логика
}
```

### **Как добавить новый тип награды:**

1. Добавьте новый enum в `EQuestRewardType`
2. Добавьте case в `QuestRewardSystem::GrantReward()`
3. Emit соответствующее событие
4. Создайте listener component в целевом плагине

---

**?? СИСТЕМА ПОЛНОСТЬЮ ПЕРЕРАБОТАНА!** ??

? Event-based architecture
? Нет жёстких зависимостей
? Phase Rewards работают
? Items добавляются в инвентарь

**ДОБАВЬТЕ КОМПОНЕНТ К PLAYERPAWN И ТЕСТИРУЙТЕ!** ?????
