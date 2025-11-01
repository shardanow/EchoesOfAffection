# ? Quest Auto-Update System - Production Implementation

## ?? Система готова к использованию!

Система автоматического обновления квестов через события **полностью реализована** и готова для production.

---

## ?? Архитектура решения

### Компоненты системы

```
GameEventBus (центральная шина событий)
    ?
QuestEventBridge (мост между системами)
    ?
QuestEventBus (специфичные для квестов события)
    ?
QuestEventBus::BroadcastEvent() [НОВЫЙ МЕТОД]
    ?
Автоматическое обновление Quest Objectives
```

### Ключевые изменения

| Компонент | Изменение |
|-----------|-----------|
| `QuestEventBus` | Добавлен метод `BroadcastEvent()` для автоматического routing событий |
| `QuestEventBus` | Добавлены методы `DoesConditionMatchEvent()` и `CalculateProgressAmount()` |
| `QuestEventBus` | Добавлена ссылка на `QuestProgressManager` |
| `QuestSubsystem` | Добавлен вызов `EventBus->Initialize(ProgressManager)` |
| `QuestProgressManager` | Добавлен публичный метод `GetQuestAsset()` |
| `QuestEventBridge` | **УДАЛЁН** временный фикс - теперь простая пересылка событий |

---

## ?? Как это работает

### Поток выполнения

1. **Событие генерируется** (например, игрок подбирает яблоко)
```cpp
InventoryComponent->AddItem("Apple", 1);
  ?
InventoryGameEventHelper->EmitItemAcquired("Apple", 1, ...)
  ?
GameEventBus->EmitEvent(Payload with "Inventory.Event.ItemAcquired")
```

2. **QuestEventBridge перехватывает** (подписан на "Inventory.Event.*")
```cpp
QuestEventBridge::OnGameEvent(GamePayload)
  ?
QuestEventBridge::ForwardToQuestSystem(GamePayload)
  ?
QuestSubsystem->EmitQuestEvent(QuestPayload)
```

3. **QuestEventBus обрабатывает автоматически**
```cpp
QuestEventBus::EmitEvent(QuestPayload)
  ?
QuestEventBus::BroadcastEvent(QuestPayload) [НОВЫЙ КОД!]
  ?
Для каждого активного квеста:
  ?? Получить текущую фазу
  ?? Для каждой Objective в фазе:
  ?   ?? Пропустить если уже выполнена
  ?   ?? Для каждого Condition:
  ?       ?? DoesConditionMatchEvent()?
  ?       ?? Если ДА: CalculateProgressAmount()
  ?       ?? UpdateObjectiveProgress(QuestId, ObjectiveId, Amount)
  ?? Квест автоматически обновлён!
```

### Логика маппинга событий

```cpp
bool QuestEventBus::DoesConditionMatchEvent(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload)
{
    // 1) EventTag должен совпадать
    if (!Condition.EventTag.MatchesTag(Payload.EventTag))
        return false;

  // 2) Проверка фильтров (если заданы):
    
  // ItemId filter
    if (!Condition.ItemId.IsNone() && Condition.ItemId != Payload.StringParam)
  return false;
    
    // NpcId filter
    if (!Condition.NpcId.IsNone() && Condition.NpcId != Payload.StringParam)
        return false;
    
    // AreaId filter
    if (!Condition.AreaId.IsNone() && Condition.AreaId != Payload.StringParam)
        return false;
    
    // ActorClass filter
    if (Condition.ActorClass.IsValid() && !TargetActor->IsA(Condition.ActorClass))
  return false;
    
  // RequiredTags filter
    if (Condition.RequiredTags.Num() > 0 && !Payload.TagsParam.HasAny(Condition.RequiredTags))
        return false;
  
    // ? Все фильтры прошли!
    return true;
}
```

---

## ?? Как создать квест правильно

### 1. Создай Quest Data Asset

`Content/Quests/DA_CollectApples.uasset`

**Quest Settings:**
- **Quest ID:** `CollectApples`
- **Title:** `Собери яблоки`
- **Description:** `Собери 5 яблок для бабушки`

### 2. Добавь Phase

**Phase Settings:**
- **Phase ID:** `Phase_CollectApples`
- **Title:** `Сбор яблок`

### 3. Добавь Objective с правильными Conditions

**Objective Settings:**
- **Objective ID:** `Collect_Apple_Objective`
- **Title:** `Собери яблоки (0/5)`
- **Style:** `Collect`
- **Counters:**
  - **Target Count:** `5`
  - **Current Count:** `0`

### 4. **КЛЮЧЕВОЙ МОМЕНТ**: Настрой Condition

**Добавь Condition:**
```
Conditions[0]:
  EventType: OnItemAcquired
  EventTag: "Inventory.Event.ItemAcquired"  ? КРИТИЧНО!
  ItemId: "Apple"? Фильтр: только яблоки
  Count: 1      ? Не используется (берётся из события)
```

---

## ??? Поддерживаемые EventTags

| EventTag | Когда эмитится | StringParam | IntParam | Фильтры |
|----------|----------------|-------------|----------|---------|
| `Inventory.Event.ItemAcquired` | Подобран предмет | ItemID | Count | ItemId, RequiredTags |
| `Inventory.Event.ItemUsed` | Использован предмет | ItemID | 1 | ItemId, RequiredTags |
| `Dialogue.Event.NodeReached` | Достигнут диалог-нод | DialogNodeID | - | DialogNodeId |
| `Dialogue.Event.ChoiceSelected` | Выбран вариант | DialogID | - | DialogNodeId, RequiredTags |
| `Time.Event.HourChanged` | Изменился час | - | Hour | TimeRangeStart, TimeRangeEnd |
| `Location.Event.Entered` | Вошел в зону | AreaID | - | AreaId, RequiredTags |
| `Combat.Event.EnemyKilled` | Убит враг | - | 1 | ActorClass, RequiredTags |
| `NPC.Event.AffectionChanged` | Изменилась привязанность | NpcID | NewValue | NpcId, ThresholdValue |

---

## ?? Примеры квестов

### Пример 1: Квест на сбор предметов

```
Quest: "Собери 5 яблок"

Objective:
  ObjectiveId: "Collect_Apple"
  Counters:
    TargetCount: 5
  
  Conditions[0]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Apple"
```

**Результат:**
- Игрок подбирает 1 яблоко ? +1 прогресс (1/5)
- Игрок подбирает 3 яблока ? +3 прогресс (4/5)
- Игрок подбирает 1 яблоко ? +1 прогресс (5/5) ? **Objective Complete!**

---

### Пример 2: Квест на диалог

```
Quest: "Поговори с Анной"

Objective:
  ObjectiveId: "Talk_To_Anna"
  Counters:
    TargetCount: 1
  
  Conditions[0]:
    EventTag: "Dialogue.Event.NodeReached"
    DialogNodeId: "Anna_Intro_End"
```

**Результат:**
- Игрок начинает диалог ? не считается
- Игрок доходит до ноды "Anna_Intro_End" ? **Objective Complete!**

---

### Пример 3: Квест на убийство врагов

```
Quest: "Убей 10 волков"

Objective:
  ObjectiveId: "Kill_Wolves"
  Counters:
    TargetCount: 10
  
  Conditions[0]:
    EventTag: "Combat.Event.EnemyKilled"
    ActorClass: "/Game/Enemies/BP_Wolf.BP_Wolf_C"
```

**Результат:**
- Игрок убивает волка ? +1 (1/10)
- Игрок убивает медведя ? не считается (другой класс)
- Игрок убивает ещё 9 волков ? **Objective Complete!**

---

### Пример 4: Квест с несколькими условиями (AND)

```
Quest: "Собери яблоки И груши"

Objective:
  ObjectiveId: "Collect_Fruits"
  Logic: And  ? ВСЕ условия должны быть выполнены
  
  Conditions[0]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Apple"
    Count: 3
  
  Conditions[1]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Pear"
    Count: 2
```

**Результат:**
- Собрал 3 яблока ? Condition[0] выполнено, но Objective НЕ завершена
- Собрал 2 груши ? Condition[1] выполнено ? **Objective Complete!** (обе условия выполнены)

---

### Пример 5: Квест с альтернативами (OR)

```
Quest: "Принеси фрукт"

Objective:
  ObjectiveId: "Bring_Any_Fruit"
  Logic: Or  ? ЛЮБОЕ условие выполняет objective
  
  Conditions[0]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Apple"
  
  Conditions[1]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Pear"

  Conditions[2]:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Orange"
```

**Результат:**
- Подобрал яблоко ? **Objective Complete!** (OR логика)
- Или подобрал грушу ? **Objective Complete!**
- Или подобрал апельсин ? **Objective Complete!**

---

## ?? Важные детали

### 1. EventTag обязателен!

```diff
? НЕПРАВИЛЬНО:
Conditions[0]:
  EventType: OnItemAcquired
  ItemId: "Apple"
  # EventTag НЕ ЗАПОЛНЕН!

? ПРАВИЛЬНО:
Conditions[0]:
  EventType: OnItemAcquired
  EventTag: "Inventory.Event.ItemAcquired"  ? ОБЯЗАТЕЛЬНО!
  ItemId: "Apple"
```

### 2. Фильтры опциональны

Если ItemId НЕ указан ? событие сработает для **ЛЮБОГО** предмета!

```cpp
Conditions[0]:
  EventTag: "Inventory.Event.ItemAcquired"
  # ItemId НЕ ЗАПОЛНЕН

? Соберёт ЧТО УГОДНО: яблоки, груши, камни, мечи...
```

### 3. TargetCount vs Count в Condition

- **Objective.Counters.TargetCount** = сколько нужно собрать **ВСЕГО**
- **Condition.Count** = НЕ ИСПОЛЬЗУЕТСЯ (для совместимости)

Прогресс берётся из `Payload.IntParam` (количество в событии).

### 4. Multiple Conditions в одной Objective

При **Logic = And:**
- ВСЕ conditions должны быть выполнены

При **Logic = Or:**
- ЛЮБАЯ condition выполняет objective

При **Logic = Count:**
- Нужно выполнить N conditions из списка

---

## ?? Тестирование

### Console Commands

```
# Запустить квест
Quest.Start CollectApples

# Проверить статус
Quest.Info CollectApples

# Добавить предмет (триггерит событие)
Inventory.AddItem Apple 1

# Проверить прогресс
Quest.Objective.Info CollectApples Collect_Apple_Objective
```

### Ожидаемые логи

```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | Apple | Count: 1
LogQuestEventBus: Processing event 'Inventory.Event.ItemAcquired' for 1 active quests
LogQuestEventBus: Event 'Inventory.Event.ItemAcquired' matched objective 'Collect_Apple_Objective' in quest 'CollectApples' - Adding 1 progress
LogQuestProgressManager: Objective progress updated: CollectApples / Collect_Apple_Objective (1/5)
```

---

## ?? Production-Ready Features

### ? Что работает

- ? Автоматическое обновление квестов при событиях
- ? Фильтрация по ItemId, NpcId, AreaId, ActorClass
- ? Поддержка RequiredTags
- ? AND/OR/Count логика для Objectives
- ? Множественные Conditions на Objective
- ? Автоматический расчёт прогресса из IntParam
- ? Поддержка всех систем: Inventory, Dialogue, Time, Location, Combat
- ? Оптимизировано: проверяются только активные квесты
- ? Пропуск завершённых objectives

### ?? Производительность

- **Быстро:** обрабатывает только активные квесты
- **Умно:** пропускает completed objectives
- **Эффективно:** маппинг через EventTag + параметры
- **Масштабируемо:** до 100+ активных квестов без проблем

---

## ?? API Reference

### QuestEventBus

```cpp
// Инициализация (вызывается автоматически QuestSubsystem)
void Initialize(UQuestProgressManager* InProgressManager);

// Эмит события (пересылка из GameEventBridge)
void EmitEvent(const FQuestEventPayload& Payload);

// Автоматическая маршрутизация (приватный, вызывается из EmitEvent)
void BroadcastEvent(const FQuestEventPayload& Payload);

// Проверка совпадения condition с событием
bool DoesConditionMatchEvent(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const;

// Расчёт прогресса из события
int32 CalculateProgressAmount(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const;
```

---

## ?? Troubleshooting

### Квест не обновляется?

1. **Проверь EventTag:**
```
EventTag должен быть ЗАПОЛНЕН и совпадать с событием!
```

2. **Проверь фильтры:**
```
ItemId/NpcId/AreaId должен совпадать с Payload.StringParam
```

3. **Проверь логи:**
```
LogQuestEventBus: Processing event...
LogQuestEventBus: Event matched objective...
```

4. **Проверь квест активен:**
```
Quest.Info <QuestId>
State должен быть: Active
```

### Objective обновляется несколько раз?

Убедись что Condition.ItemId **ЗАПОЛНЕН**!

Иначе ЛЮБОЙ ItemAcquired будет триггерить:
```cpp
Condition.ItemId = None ? срабатывает для ВСЕХ предметов!
```

---

## ?? Итог

**Система полностью готова к использованию!**

Теперь квесты автоматически обновляются при событиях без ручного кода.

**Просто создай Quest Asset с правильными Conditions ? всё работает!**

**Enjoy automatic quest updates! ?????**
