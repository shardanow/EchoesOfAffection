# ?? Quest System - Quick Start Guide

## ? Быстрый старт за 5 минут

### Шаг 1: Создать квест (1 мин)

```
Content Browser ? Right Click ? Quest System ? Quest Asset
Имя: DA_Quest_MyFirstQuest
```

**Заполнить:**
```
QuestId: Q_MyFirstQuest
QuestName: "My First Quest"
QuestDescription: "Learn how quests work!"
```

### Шаг 2: Добавить Objective (2 мин)

```
Objectives ? Add Element
?? ObjectiveId: O_TalkToNPC
?? Description: "Talk to Lianne"
?? RequiredCount: 1
?? Conditions ? Add Element
     ?? EventTag: GameEvent.Dialogue.Started
     ?? NpcId: Lianne
```

### Шаг 3: Настроить NPC (1 мин)

```
BP_Lianne (в редакторе уровня):
?? Details ? Tags ? Gameplay Tags: NPC.Lianne  ? Обязательно!
```

### Шаг 4: Добавить квест NPC (1 мин)

```
BP_Lianne:
?? Add Component ? QuestGiverComponent
     ?? bAutoAssignNpcId: true
     ?? AvailableQuests ? Add Element: DA_Quest_MyFirstQuest
```

### Шаг 5: Тест! (30 сек)

```
1. Play (PIE)
2. Подойти к Lianne (иконка ! над головой)
3. Кликнуть E - начать квест
4. Поговорить с Lianne
5. ? Квест завершён!
```

---

## ?? Распространённые типы квестов

### 1?? Собрать предметы

```
DA_Quest_CollectApples:
  ?? Objective: "Collect 5 apples"
       ?? Condition:
       ?? EventTag: GameEvent.Inventory.ItemAcquired
            ?? ItemId: RedApple
   ?? RequiredCount: 5
```

### 2?? Поговорить с NPC

```
DA_Quest_TalkToLianne:
  ?? Objective: "Talk to Lianne"
     ?? Condition:
      ?? EventTag: GameEvent.Dialogue.Started
 ?? NpcId: Lianne
      ?? RequiredCount: 1
```

### 3?? Использовать предмет

```
DA_Quest_UsePotion:
  ?? Objective: "Use a health potion"
       ?? Condition:
     ?? EventTag: GameEvent.Inventory.ItemUsed
   ?? ItemId: HealthPotion
            ?? RequiredCount: 1
```

### 4?? Цепочка квестов

```
DA_Quest_SecondQuest:
  ?? Dependencies: [Q_MyFirstQuest]  ? Сначала нужно завершить!
  ?? Objective: "Complete follow-up task"
```

### 5?? Достичь узла диалога

```
DA_Quest_MakeChoice:
  ?? Objective: "Reach dialogue node"
       ?? Condition:
        ?? EventTag: GameEvent.Dialogue.NodeReached
            ?? NpcId: Lianne
         ?? RequiredCount: 1
```

### 6?? Завершить другой квест

```
DA_Quest_MetaQuest:
  ?? Objective: "Complete any quest"
       ?? Condition:
    ?? EventTag: GameEvent.Quest.Completed
          ?? RequiredCount: 1
```

---

## ?? Примеры условий

### По событию инвентаря

| Цель | EventTag | Фильтр |
|------|----------|--------|
| Подобрать яблоко | `GameEvent.Inventory.ItemAcquired` | `ItemId: RedApple` |
| Использовать зелье | `GameEvent.Inventory.ItemUsed` | `ItemId: HealthPotion` |
| Скрафтить меч | `GameEvent.Inventory.ItemCrafted` | `ItemId: IronSword` |
| Подарить цветок | `GameEvent.Inventory.ItemGifted` | `ItemId: Rose` |

### По событию диалога

| Цель | EventTag | Фильтр |
|------|----------|--------|
| Поговорить с Lianne | `GameEvent.Dialogue.Started` | `NpcId: Lianne` |
| Достичь узла | `GameEvent.Dialogue.NodeReached` | `NpcId: Alice` |
| Сделать выбор | `GameEvent.Dialogue.ChoiceMade` | `NpcId: Merchant` |

### По событию квеста

| Цель | EventTag | Фильтр |
|------|----------|--------|
| Начать квест | `GameEvent.Quest.Started` | `QuestId: Q_Tutorial` |
| Завершить квест | `GameEvent.Quest.Completed` | `QuestId: Q_FirstQuest` |
| Завершить objective | `GameEvent.Quest.ObjectiveCompleted` | - |

### По событию времени

| Цель | EventTag | Фильтр |
|------|----------|--------|
| Дождаться часа | `GameEvent.Time.HourChanged` | `RequiredCount: 5` |
| Дождаться дня | `GameEvent.Time.DayChanged` | `RequiredCount: 2` |
| Дождаться рассвета | `GameEvent.Time.Sunrise` | `RequiredCount: 1` |

---

## ?? Настройка NPC

### Вариант А: С GameplayTags (рекомендуется!)

```
BP_Lianne:
  ?? Gameplay Tags: NPC.Lianne  ? Извлечётся "Lianne"
  ?? QuestGiverComponent:
       ?? bAutoAssignNpcId: true
       ?? AvailableQuests: [DA_Quest_TalkToLianne]
       ?? QuestAvailableIcon: Icon_YellowExclamation
       ?? QuestInProgressIcon: Icon_GrayQuestion
  ?? QuestCompleteIcon: Icon_GoldCheck
```

### Вариант Б: Без тегов (fallback)

```
BP_Lianne_C_1:  ? Имя актора ВАЖНО!
  ?? QuestGiverComponent:
       ?? (всё то же самое)

Результат: "BP_Lianne_C_1" ? "Lianne" (упрощено)
```

---

## ?? UI компоненты

### Quest Tracker Widget

**Автоматически отслеживает активные квесты:**

```cpp
// Добавить в HUD Blueprint
UQuestTrackerWidget* QuestTracker = CreateWidget<UQuestTrackerWidget>(this, QuestTrackerClass);
QuestTracker->AddToViewport();

// Widget автоматически:
// ? Подписывается на OnQuestProgressChanged
// ? Обновляет список objectives
// ? Показывает прогресс (X/Y)
// ? Скрывается, когда нет квестов
```

### Quest Giver иконки

**Над головой NPC (Widget Component):**

```cpp
// Blueprint Tick
void UpdateQuestIcon()
{
    UQuestGiverComponent* QuestGiver = NPC->FindComponentByClass<UQuestGiverComponent>();
    UTexture2D* Icon = QuestGiver->GetQuestIndicatorIcon(PlayerController);
    
 if (Icon)
    {
        IconWidget->SetBrushFromTexture(Icon);
     IconWidget->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
 IconWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}
```

**Иконки:**
- `!` (жёлтый) - Новый квест доступен
- `?` (серый) - Квест активен
- `?` (золотой) - Квест можно сдать

---

## ?? Отладка

### Включить логи

```ini
// Config/DefaultEngine.ini
[Core.Log]
LogGameEventBus=Verbose
LogQuestEventBus=Verbose
LogQuestBridge=Verbose
LogInventoryEvents=Verbose
LogDialogueSubsystem=Verbose
```

### Проверить в логах

**? Правильный поток:**

```
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] ?? Called! EventTag='GameEvent.Dialogue.Started', NPC=BP_Lianne_C_1
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne'
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started' | StringParam='Lianne'
[LogQuestEventBus] ?? Checking objective 'O_TalkToLianne'
[LogQuestEventBus] ? CONDITION MATCH! StringParam='Lianne' == NpcId='Lianne'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

**? Типичные ошибки:**

```
// Ошибка 1: EventTag не совпадает
[LogQuestEventBus] ? EventTag mismatch
Решение: Проверить точное совпадение EventTag

// Ошибка 2: NPC ID не совпадает
[LogQuestEventBus] ? NPC ID mismatch: Expected 'Lianne', got 'lianne_greeting'
Решение: Добавить GameplayTag "NPC.Lianne"

// Ошибка 3: Событие не эмитится
(нет логов GameEventBus)
Решение: Проверить, что GameEventBus plugin включён
```

---

## ?? Частые ошибки

| Проблема | Причина | Решение |
|----------|---------|---------|
| Квест не начинается | Dependencies не выполнены | Проверить зависимости |
| Условие не срабатывает | EventTag не совпадает | Точное совпадение! |
| NPC ID неправильный | Нет GameplayTag | Добавить `NPC.{Name}` |
| UI не обновляется | Виджет не подписан | Проверить подписку |
| Событие не эмитится | Плагин выключен | Включить GameEventBus |
| Иконка не появляется | QuestGiver не настроен | Добавить компонент |

---

## ?? Чеклист перед тестом

```
? Quest Asset создан
   ?? QuestId заполнен
   ?? QuestName заполнен
   ?? Objectives добавлены

? Objectives настроены
   ?? ObjectiveId заполнен
   ?? Description заполнено
   ?? RequiredCount > 0
   ?? Conditions добавлены

? Conditions настроены
   ?? EventTag выбран из списка (точное совпадение!)
   ?? Фильтры заполнены (ItemId/NpcId/QuestId)

? NPC настроен
   ?? GameplayTag добавлен: NPC.{Name}
   ?? QuestGiverComponent добавлен
    ?? AvailableQuests содержит квест

? Плагины включены
   ?? GameEventBus ?
   ?? QuestSystem ?
   ?? InventorySystem ? (если используется)
   ?? DialogueSystem ? (если используется)

? Логи включены (для отладки)
   ?? DefaultEngine.ini настроен
```

---

## ?? Следующие шаги

### Углубиться в систему:
1. **NPC_ID_COMPLETE_GUIDE.md** - Всё про NPC ID
2. **QUEST_CONDITIONS_COMPLETE_REFERENCE.md** - Все типы условий
3. **QUEST_GIVER_COMPONENT_EXPLAINED.md** - UI компонент
4. **VISUAL_FLOW_DIAGRAM.md** - Визуальные схемы
5. **README_FINAL.md** - Полная документация

### Примеры квестов:
- `DA_Quest_CollectApples` - Собрать предметы
- `DA_Quest_TalkToLianne` - Поговорить с NPC
- `DA_Quest_ChainedQuest` - Цепочка квестов

---

## ?? Полезные советы

1. **Всегда используйте GameplayTags** для NPC - это надёжно
2. **EventTag должен точно совпадать** - копируйте из списка!
3. **RequiredCount > 0** - иначе objective сразу завершится
4. **Тестируйте с логами** - Verbose режим ваш друг
5. **Один квест за раз** - сначала простой, потом сложнее

---

**Готово! Теперь создайте свой первый квест! ??**

**Время на создание:** ~5 минут  
**Сложность:** Лёгкая  
**Результат:** Работающий квест! ?
