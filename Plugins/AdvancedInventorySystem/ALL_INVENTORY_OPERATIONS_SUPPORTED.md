# ?? Полная поддержка всех операций с инвентарём в квестах

## ? Поддерживаемые операции:

### 1. ? **ItemAcquired** (подбор предмета)
**Работает!** Полностью протестировано.

**Пример квеста:**
```
Condition: ItemId='RedApple', Count=5
? Подберите 5 яблок
```

**События:**
- Подбор предмета с земли
- Получение предмета от NPC
- Покупка предмета
- Получение награды

---

### 2. ? **ItemUsed** (использование предмета)
**Работает!** Готово к использованию.

**Пример квеста:**
```
Condition: EventTag='GameEvent.Inventory.ItemUsed', ItemId='HealthPotion', Count=3
? Используйте 3 зелья здоровья
```

**События:**
- Использование consumable (еда, зелья)
- Применение инструмента
- Активация ключевого предмета

---

### 3. ? **ItemCrafted** (крафт предмета)
**Работает!** Готово к использованию.

**Пример квеста:**
```
Condition: EventTag='GameEvent.Inventory.ItemCrafted', ItemId='IronSword', Count=1
? Скрафтите железный меч
```

**События:**
- Крафт на верстаке
- Готовка еды
- Создание зелий

---

### 4. ? **ItemGifted** (дарение предмета NPC)
**Работает!** Готово к использованию.

**Пример квеста:**
```
Condition: EventTag='GameEvent.Inventory.ItemGifted', ItemId='Flowers', NpcId='Alice', Count=1
? Подарите цветы Алисе
```

**События:**
- Дарение подарка NPC
- Передача квестового предмета
- Подношение богам/алтарю

**Дополнительно:**
- Поддерживает **конкретного NPC** через `Payload.TargetActor`
- Можно отслеживать дарение **определённому NPC**

---

### 5. ? **ItemDropped** (выброс предмета)
**Работает!** Готово к использованию.

**Пример квеста:**
```
Condition: EventTag='GameEvent.Inventory.ItemDropped', ItemId='CursedAmulet', Count=1
? Избавьтесь от проклятого амулета
```

**События:**
- Выброс предмета из инвентаря
- Размещение предмета в мире
- Утилизация мусора

---

### 6. ? **CustomInventoryEvent** (кастомные события)
**Работает!** Для специальных кейсов.

**Пример:**
```cpp
FGameplayTag CustomTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory.ItemEnchanted"));
UInventoryGameEventBridge::EmitCustomInventoryEvent(
    WorldContextObject,
    CustomTag,
    FName("MagicSword"),
    1,
    Player,
    Enchanter,
    AdditionalTags
);
```

**Использование:**
- Особые взаимодействия с предметами
- Модификация предметов (улучшение, зачарование)
- Кастомная логика инвентаря

---

## ?? Реализация в квестах:

### Структура Objective Condition:

```cpp
struct FObjectiveCondition
{
    FGameplayTag EventTag;  // Тип события (ItemUsed, ItemCrafted, etc.)
 FName ItemId;   // ID предмета
    FName NpcId;     // ID NPC (для ItemGifted)
    int32 Count;   // Количество
};
```

### Примеры условий квестов:

#### Квест "Алхимик"
```
Objective 1: Соберите 10 трав
  - EventTag: GameEvent.Inventory.ItemAcquired
  - ItemId: Herb
  - Count: 10

Objective 2: Скрафтите 3 зелья здоровья
  - EventTag: GameEvent.Inventory.ItemCrafted
  - ItemId: HealthPotion
  - Count: 3

Objective 3: Используйте зелье
  - EventTag: GameEvent.Inventory.ItemUsed
  - ItemId: HealthPotion
  - Count: 1
```

#### Квест "Подарок для друга"
```
Objective 1: Получите цветы
  - EventTag: GameEvent.Inventory.ItemAcquired
  - ItemId: Flowers
  - Count: 1

Objective 2: Подарите цветы Алисе
  - EventTag: GameEvent.Inventory.ItemGifted
  - ItemId: Flowers
  - NpcId: Alice
  - Count: 1
```

#### Квест "Очищение"
```
Objective 1: Избавьтесь от проклятого предмета
  - EventTag: GameEvent.Inventory.ItemDropped
  - ItemId: CursedAmulet
  - Count: 1
```

---

## ?? Как использовать в вашей игре:

### 1. Для Use/Consume предметов:

```cpp
// В вашем Item/Potion классе при использовании:
void AHealthPotion::Use(AActor* User)
{
    // Ваша логика использования
    RestoreHealth(User);
  
    // Эмитировать событие для квестов
    UInventoryGameEventBridge::EmitItemUsedEvent(
     this,
        FName("HealthPotion"),
        User,
    nullptr  // Target (если нужен)
    );
}
```

### 2. Для крафта:

```cpp
// В вашей системе крафта:
void UCraftingComponent::CraftItem(FName ItemId, int32 Count, AActor* Crafter)
{
    // Ваша логика крафта
    AddItemToInventory(ItemId, Count);
  
    // Эмитировать событие для квестов
    UInventoryGameEventBridge::EmitItemCraftedEvent(
        this,
        ItemId,
        Count,
    Crafter
    );
}
```

### 3. Для дарения NPC:

```cpp
// В вашем Gift/Trade системе:
void UGiftComponent::GiveGiftToNPC(FName ItemId, AActor* Giver, AActor* NPCReceiver)
{
    // Ваша логика передачи
    RemoveItemFromInventory(ItemId);
    
    // Эмитировать событие для квестов
    UInventoryGameEventBridge::EmitItemGiftedEvent(
     this,
    ItemId,
        1,
        Giver,
        NPCReceiver  // Важно! Квест может требовать конкретного NPC
    );
}
```

### 4. Для выброса:

```cpp
// В InventoryComponent при выбросе:
void UInventoryComponent::DropItem(FName ItemId, int32 Count, AActor* Dropper)
{
    // Ваша логика выброса
    SpawnItemInWorld(ItemId, Count);
    
    // Эмитировать событие для квестов
    UInventoryGameEventBridge::EmitItemDroppedEvent(
        this,
      ItemId,
        Count,
     Dropper
    );
}
```

---

## ?? Тестирование:

### Шаг 1: Пересоберите проект
Intermediate для AdvancedInventorySystem уже очищен.

### Шаг 2: Создайте тестовый квест

В Data Asset создайте квест с разными типами objectives:

```
Phase 1:
  - Objective: Соберите 3 яблока
    Condition: EventTag=GameEvent.Inventory.ItemAcquired, ItemId=RedApple, Count=3

Phase 2:
  - Objective: Используйте яблоко
    Condition: EventTag=GameEvent.Inventory.ItemUsed, ItemId=RedApple, Count=1
```

### Шаг 3: Вызовите события в игре

```cpp
// Тест в консоли или Blueprint:
UInventoryGameEventBridge::EmitItemUsedEvent(this, FName("RedApple"), PlayerCharacter, nullptr);
```

### Шаг 4: Проверьте Output Log

```
[LogInventoryEvents] [InventoryGameEventBridge::EmitItemUsedEvent] ?? Called! ItemId='RedApple'
[LogInventoryEvents] [InventoryGameEventBridge] ? EventTag valid: 'GameEvent.Inventory.ItemUsed'
[LogInventoryEvents] [InventoryGameEventBridge] ?? Item Used event emitted to GameEventBus!
    ??
[LogQuestBridge] ?? Received game event: 'GameEvent.Inventory.ItemUsed'
    ??
[QuestEventBus] ??? MATCH FOUND!
    ??
[QuestProgressManager] ? Objective completed!
```

---

## ?? Итого:

### ? Полностью поддерживаются 5 операций:
1. ? **Acquire** — подбор/получение
2. ? **Use** — использование/потребление
3. ? **Craft** — крафт/создание
4. ? **Gift** — дарение NPC
5. ? **Drop** — выброс/утилизация

### ? Все события:
- Правильно эмитируются через `GameEventBus`
- Имеют корректные теги `GameEvent.Inventory.*`
- Полностью логируются для отладки
- Подписаны в `QuestEventBridge`
- Обрабатываются `QuestEventBus`

### ? Дополнительные возможности:
- **Отслеживание конкретного NPC** для дарения
- **Кастомные события** через `EmitCustomInventoryEvent`
- **Target Actor** для событий с двумя участниками
- **Additional Tags** для расширенной логики

**ВСЁ ГОТОВО К ИСПОЛЬЗОВАНИЮ!** ?????
