# Game Event Bus - Integration Guide

## Обзор

**Game Event Bus** — это центральная система событий, которая позволяет различным игровым системам взаимодействовать друг с другом **БЕЗ жёстких зависимостей**.

### Преимущества

? **Нет хард-кода** — системы не знают друг о друге  
? **Гибкость** — легко добавлять новые системы  
? **Настройка через Data Assets** — не нужно писать код  
? **Blueprint-friendly** — всё доступно из Blueprint  
? **Масштабируемость** — работает с любым количеством систем  

---

## Архитектура

```
???????????????????????????????????????????????????????????????
?        GameEventBusSubsystem        ?
?  • Центральный диспетчер событий       ?
?  • Подписка на события через GameplayTags           ?
?  • Эмиссия событий между системами       ?
???????????????????????????????????????????????????????????????
      ?
 ?
      ???????????????????????????????????????????
         ?          ?     ?
    ????????????      ???????????????      ????????????
    ?  Quest   ?      ?  Dialogue   ?      ?  Item    ?
    ?  System  ?      ?   System    ?  ? System   ?
    ????????????      ???????????????      ????????????
```

### Как это работает

1. **Система A** эмитирует событие (например, "Inventory.Event.ItemAcquired")
2. **GameEventBus** получает событие и передаёт всем подписчикам
3. **Система B** (например, Quest System) получает событие и реагирует

**Важно:** Система A не знает о существовании Системы B!

---

## Примеры использования

### 1. Квест: "Собрать 5 яблок"

**Data Asset: DA_Quest_CollectApples**

```
Quest Objective:
  - EventType: OnItemAcquired
  - EventTag: "Inventory.Event.ItemAcquired"
  - ItemId: "Apple"
  - Count: 5
```

**Inventory System эмитирует событие:**

```cpp
// В C++
UInventoryGameEventBridge::EmitItemAcquiredEvent(this, FName("Apple"), 1, PlayerActor);

// Или в Blueprint
Emit Item Event
  EventTag: Inventory.Event.ItemAcquired
  ItemId: Apple
  Count: 1
```

**Quest System автоматически получает событие** через QuestEventBridge и обновляет прогресс!

---

### 2. Диалог -> Квест

**Data Asset: DA_Dialogue_TalkToBlacksmith**

```
Dialogue Node:
  Text: "Помоги мне собрать железо!"
  
  Effects:
    - EmitGameEvent:
        EventTag: "Quest.Event.Start"
        StringParam: "Quest_GatherIron"
```

**Quest System** подхватывает это событие и запускает квест!

---

### 3. Время -> Квест

**Data Asset: DA_Quest_NightPatrol**

```
Quest Objective:
  - EventType: OnTimeReached
  - EventTag: "Time.Event.HourChanged"
  - TimeRangeStart: 20
  - TimeRangeEnd: 6
  - RequiredArea: "City"
```

**Time System эмитирует события:**

```cpp
// Каждый час
UTimeSystemGameEventEmitter::OnHourChanged(CurrentHour);
```

**Quest System** проверяет условия и засчитывает прогресс!

---

## Как настроить для вашего проекта

### Шаг 1: Включите плагин GameEventBus

В `.uproject` или через Editor:

```json
{
  "Plugins": [
    {
      "Name": "GameEventBus",
      "Enabled": true
    }
  ]
}
```

### Шаг 2: Настройте интеграции

В вашем GameMode или GameInstance:

```cpp
// C++
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Автоматическая настройка всех интеграций
    UGameEventIntegrationHelpers::SetupAllIntegrations(this);
}
```

Или в Blueprint:

```
BeginPlay -> Setup All Integrations
```

### Шаг 3: Добавьте эмиссию событий в ваши системы

**Пример: Inventory System**

```cpp
// Когда игрок получает предмет
void UInventoryComponent::AddItem(FName ItemId, int32 Count)
{
    // ... ваша логика добавления предмета ...
    
    // Эмитируем событие
    UInventoryGameEventBridge::EmitItemAcquiredEvent(this, ItemId, Count, GetOwner());
}
```

**Пример: Dialogue System**

Добавьте эффект `EmitGameEvent` в ноды диалога:

```
Dialogue Node -> Add Effect -> Emit Game Event
  EventTag: Dialogue.Event.NodeReached
  StringParam: CurrentNodeId
```

### Шаг 4: Настройте квесты через Data Assets

Создайте Quest Data Asset:

```
Objective:
  Condition:
    EventType: OnItemAcquired
    EventTag: Inventory.Event.ItemAcquired
  ItemId: GoldCoin
    Count: 100
```

**Готово!** Квест будет автоматически отслеживать получение монет.

---

## Список событий (GameplayTags)

### Inventory Events
- `Inventory.Event.ItemAcquired` — получен предмет
- `Inventory.Event.ItemUsed` — использован предмет  
- `Inventory.Event.ItemCrafted` — создан предмет
- `Inventory.Event.ItemGifted` — подарен предмет NPC
- `Inventory.Event.ItemDropped` — выброшен предмет

### Dialogue Events
- `Dialogue.Event.NodeReached` — достигнута нода диалога
- `Dialogue.Event.ChoiceMade` — сделан выбор
- `Dialogue.Event.Started` — диалог начат
- `Dialogue.Event.Ended` — диалог завершён

### Quest Events
- `Quest.Event.Started` — квест начат
- `Quest.Event.Completed` — квест завершён
- `Quest.Event.Failed` — квест провален
- `Quest.Event.ObjectiveCompleted` — цель квеста выполнена

### Time Events
- `Time.Event.HourChanged` — изменился час
- `Time.Event.DayChanged` — изменился день
- `Time.Event.TimeOfDay` — достигнуто определённое время

### Weather Events
- `Weather.Event.Changed` — изменилась погода

### NPC Events
- `NPC.Event.AffinityChanged` — изменилась близость с NPC
- `NPC.Event.TalkStarted` — начался разговор с NPC

### Location Events
- `Location.Event.Entered` — вошли в область
- `Location.Event.Exited` — вышли из области

### Combat Events
- `Combat.Event.EnemyKilled` — убит враг
- `Combat.Event.DamageTaken` — получен урон

---

## Blueprint Функции

### Эмиссия событий

```
Emit Simple Event (EventTag)
Emit Event With String (EventTag, StringParam)
Emit Event With Int (EventTag, IntParam)
Emit Item Event (EventTag, ItemId, Count)
Emit Dialogue Event (EventTag, DialogueId, NodeId)
Emit Quest Event (EventTag, QuestId, ObjectiveId)
Emit Time Event (EventTag, Hour, Day)
```

### Подписка на события

```
Subscribe To Event (EventTag, Delegate)
Subscribe To Event Hierarchy (ParentTag, Delegate)
Unsubscribe From Event (EventTag, Delegate)
```

### Регистрация эффектов

```
Register Quest Start Effect (EffectTag, QuestId)
Register Dialogue Start Effect (EffectTag, DialogueId)
Register Item Give Effect (EffectTag, ItemId, Count)
```

### Регистрация условий

```
Register Quest Active Condition (ConditionTag, QuestId)
Register Item Count Condition (ConditionTag, ItemId, Count)
Register Time Range Condition (ConditionTag, StartHour, EndHour)
```

---

## Советы и Best Practices

### ? DO

- Используйте иерархию тегов (Inventory.Event.ItemAcquired)
- Эмитируйте события ПОСЛЕ выполнения действия
- Используйте `SetupAllIntegrations()` при старте игры
- Логируйте события для отладки (`bLogEvent = true`)

### ? DON'T

- Не создавайте прямые зависимости между плагинами
- Не эмитируйте события слишком часто (используйте фильтры)
- Не забывайте отписываться от событий при Destroy

---

## Расширение системы

### Создание своего события

1. Добавьте GameplayTag в проект:
   ```
MySystem.Event.CustomAction
   ```

2. Эмитируйте событие:
   ```cpp
   FGameplayTag MyTag = FGameplayTag::RequestGameplayTag(FName("MySystem.Event.CustomAction"));
   UGameEventBusLibrary::EmitSimpleEvent(this, MyTag);
   ```

3. Подпишитесь на событие:
   ```cpp
   UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
   EventBus->SubscribeToEvent(MyTag, MyDelegate);
   ```

### Создание своего эффекта

1. Создайте класс, реализующий `IGenericGameEffect`
2. Зарегистрируйте его в Registry:
   ```cpp
   UGameEventEffectRegistry* Registry = EventBus->GetEffectRegistry();
   Registry->RegisterEffect(EffectTag, UMyCustomEffect::StaticClass(), "MySystem");
   ```

3. Используйте в квестах/диалогах через EffectTag!

---

## FAQ

**Q: Нужно ли добавлять зависимость на GameEventBus в Build.cs?**  
A: Нет! Зависимость опциональная. Системы работают и без GameEventBus.

**Q: Как добавить новое событие?**  
A: Просто создайте новый GameplayTag и эмитируйте его. Все подписчики получат событие.

**Q: Можно ли использовать в multiplayer?**  
A: Да, но нужно учесть репликацию. EventBus работает локально на каждом клиенте.

**Q: Как дебажить события?**  
A: Используйте `SetEventHistoryEnabled(true)` и `GetRecentEvents()` или включите `bLogEvent = true`.

---

## Пример: Полная интеграция квеста

```
Quest: "Принеси кузнецу 10 железа в ночное время"

Objective 1: Collect Iron
  EventType: OnItemAcquired
  EventTag: Inventory.Event.ItemAcquired
  ItemId: Iron
  Count: 10

Objective 2: Talk to Blacksmith at Night
  EventType: OnDialogNodeReached
  EventTag: Dialogue.Event.NodeReached
  NpcId: Blacksmith
  DialogNodeId: GiveIron
  ProgressGate:
    RequireTimeOfDay: true
    RequiredTimeStart: 20
    RequiredTimeEnd: 6

Rewards:
  - GiveItem: GoldCoin x100
  - ModifyAffection: Blacksmith +20
  - UnlockDialogBranch: BlacksmithAdvanced
```

**Настройка:**

1. Инвентарь эмитирует `Inventory.Event.ItemAcquired` когда игрок собирает железо
2. Диалог эмитирует `Dialogue.Event.NodeReached` когда игрок говорит с кузнецом
3. Time System эмитирует `Time.Event.HourChanged` каждый час
4. Quest System проверяет все условия и выдаёт награды

**Результат:** Всё работает автоматически без единой строчки кода!

---

## Support

Если возникли вопросы или проблемы, проверьте:

1. Включен ли плагин GameEventBus
2. Вызвали ли вы `SetupAllIntegrations()`
3. Правильно ли настроены GameplayTags
4. Эмитируются ли события (проверьте через `bLogEvent = true`)

Happy game developing! ??
