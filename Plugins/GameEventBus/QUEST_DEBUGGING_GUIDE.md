# ?? Диагностика проблем с автоматическим обновлением квестов

## ?? Цель
Этот гайд поможет найти причину, почему квесты не обновляются автоматически при получении предметов или других игровых событиях.

---

## ?? Шаг 1: Проверка базовой настройки

### ? Проверьте что квест запущен
```cpp
// В консоли Unreal Engine:
QuestDebugPrint
```

**Что смотреть:**
- Есть ли активные квесты?
- Правильный ли текущий Phase?
- Какие objectives и их условия (EventTag, ItemId, Count)?

**Пример правильного вывода:**
```
?? Quest: 'Q_AppleCollection'
   State: 1 (Active)
   Current Phase: 'CollectApples'
   ?? Objectives (1):
    ? IN PROGRESS 'Obj_CollectApples': 0/3
      Conditions (1):
        - EventTag='GameEvent.Inventory.ItemAcquired', ItemId='Item_Apple', Count=1
```

---

## ?? Шаг 2: Проверка подключения GameEventBridge

```cpp
// В консоли:
QuestDebugListeners
```

**Что смотреть:**
- `? GameEventBridge is CONNECTED` — должно быть!
- Если `?? GameEventBridge is NULL` — значит плагин GameEventBus не подключён!

**Как исправить:**
1. Откройте `.uproject`
2. Проверьте что `GameEventBus` в секции `Plugins` с `"Enabled": true`
3. Перезапустите редактор

---

## ?? Шаг 3: Проверка инвентаря и эмиссии событий

### Подберите предмет (например, яблоко) и смотрите в лог:

**Должна быть цепочка логов:**

```
[InventoryComponent::AddItem] ? Item 'Item_Apple' added! Quantity=1, Owner=BP_PlayerCharacter_C
    ??
[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ?? Emitting ItemAcquired event: ItemID='Item_Apple', Quantity=1
    ??
[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ? Event emitted to GameEventBridge
    ??
[GameEventBus] ?? EmitEvent: 'GameEvent.Inventory.ItemAcquired' | StringParam='Item_Apple' | IntParam=1
    ??
[QuestEventBridge] ?? Received game event: 'GameEvent.Inventory.ItemAcquired' | StringParam='Item_Apple' | IntParam=1
    ??
[QuestEventBridge] ?? ForwardToQuestSystem: Converting GameEventPayload to QuestEventPayload...
    ??
[QuestEventBridge] ?? Emitting to QuestSubsystem: EventTag='GameEvent.Inventory.ItemAcquired', StringParam='Item_Apple', IntParam=1
    ??
[QuestEventBus::BroadcastEvent] ?? Processing event 'GameEvent.Inventory.ItemAcquired' for 1 active quests
    ??
[QuestEventBus::BroadcastEvent] ??? MATCH FOUND! Event matched objective 'Obj_CollectApples' in quest 'Q_AppleCollection'
```

---

## ?? Типичные проблемы и решения

### ? Проблема 1: "GameEventBus NOT AVAILABLE"
```
[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ?? GameEventBus NOT AVAILABLE!
```

**Причина:** Плагин `GameEventBus` не подключён в `InventorySystemRuntime.Build.cs`

**Решение:**
```csharp
// Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/InventorySystemRuntime.Build.cs
PublicDependencyModuleNames.AddRange(new string[] 
{
    "Core",
    "CoreUObject",
    "Engine",
    "GameplayTags",
    "GameEventBus" // ? Должно быть!
});
```

---

### ? Проблема 2: "No active quests to update!"
```
[QuestEventBus::BroadcastEvent] ?? No active quests to update!
```

**Причина:** Квест не запущен

**Решение:**
```cpp
// Запустите квест через Blueprint или C++:
UQuestSubsystem* QuestSystem = UQuestSubsystem::Get(this);
QuestSystem->StartQuestAsync(FName("Q_AppleCollection"), OnQuestStarted);
```

---

### ? Проблема 3: "EventTag mismatch"
```
[QuestEventBus::DoesConditionMatchEvent] ? EventTag mismatch - rejecting
  Condition.EventTag = 'GameEvent.Inventory.ItemAcquired'
  Payload.EventTag   = 'GameEvent.Item.Acquired' ? НЕПРАВИЛЬНО!
```

**Причина:** EventTag в условии квеста не совпадает с тем, что эмитит система

**Решение:**
1. Проверьте Data Asset квеста (DA_Quest_AppleCollection)
2. Убедитесь что `EventTag` в `FObjectiveCondition` = `GameEvent.Inventory.ItemAcquired`
3. Проверьте `Config/DefaultGameplayTags.ini` — тег должен быть зарегистрирован

---

### ? Проблема 4: "ItemId mismatch"
```
[QuestEventBus::DoesConditionMatchEvent] ? ItemId mismatch: wanted 'Item_Apple' got 'Apple'
```

**Причина:** ItemID в условии квеста не совпадает с ItemID в Data Asset предмета

**Решение:**
1. Откройте DA_Item_Apple
2. Проверьте поле `ItemID` (должно быть `Item_Apple`)
3. Обновите условие в квесте, если нужно

---

### ? Проблема 5: "Quest asset NOT FOUND"
```
[QuestEventBus::BroadcastEvent] ? Quest asset NOT FOUND for 'Q_AppleCollection'
```

**Причина:** Квест не загружен в память

**Решение:**
Квест должен находиться по пути: `Content/Quests/DA_Quest_AppleCollection`

Проверьте в QuestAssetLoader:
```cpp
// QuestAssetLoader.cpp должен искать по пути:
FString AssetPath = FString::Printf(TEXT("/Game/Quests/DA_Quest_%s.DA_Quest_%s"), *QuestId.ToString(), *QuestId.ToString());
```

---

## ?? Шаг 4: Тестирование вручную

Можете эмитить событие вручную для проверки:

```cpp
// В консоли Unreal:
QuestTestEvent GameEvent.Inventory.ItemAcquired Item_Apple 1
```

Смотрите в лог — обновился ли квест!

---

## ? Проверочный чек-лист

Перед запуском игры убедитесь:

- [ ] GameEventBus включён в `.uproject`
- [ ] InventorySystemRuntime.Build.cs содержит зависимость на GameEventBus
- [ ] QuestSystemRuntime.Build.cs содержит зависимость на GameEventBus
- [ ] DefaultGameplayTags.ini содержит все нужные теги
- [ ] Квест запущен (используйте `QuestDebugPrint` для проверки)
- [ ] ItemID в условии квеста совпадает с ItemID в Data Asset предмета
- [ ] EventTag в условии квеста совпадает с тем, что эмитится

---

## ?? Финальная проверка

После подбора яблока должен быть такой лог:

```
[InventoryComponent::AddItem] ? Item 'Item_Apple' added! Quantity=1
[GameEventBus] ?? EmitEvent: 'GameEvent.Inventory.ItemAcquired' | StringParam='Item_Apple' | IntParam=1
[QuestEventBridge] ?? Received game event
[QuestEventBus::BroadcastEvent] ?? Processing event for 1 active quests
[QuestEventBus::BroadcastEvent] ??? MATCH FOUND! Adding 1 progress
[QuestProgressManager] ? Objective 'Obj_CollectApples' progress updated: 1/3
```

Если видите это — **всё работает!** ??

---

## ?? Если всё равно не работает

1. Запустите `QuestDebugPrint` — проверьте что квест активен
2. Запустите `QuestDebugListeners` — проверьте подключение GameEventBridge
3. Подберите предмет — смотрите весь лог от начала до конца
4. Найдите где цепочка ломается — там и ищите проблему
5. Используйте `QuestTestEvent` для ручной эмиссии события

Удачи! ??
