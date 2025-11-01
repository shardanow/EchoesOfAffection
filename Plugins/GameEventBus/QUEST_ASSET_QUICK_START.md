# ?? Quick Start: Создание Quest Asset для авто-обновления

## Шаг 1: Создай Quest Data Asset

1. В Content Browser:
   - **Right Click** ? **Miscellaneous** ? **Data Asset**
   - Выбери **QuestAsset**
   - Назови: `DA_CollectApples`

2. Открой `DA_CollectApples`

## Шаг 2: Заполни основные данные

**General Settings:**
```
Quest ID: CollectApples
Title: "Собери яблоки"
Description: "Собери 5 яблок для бабушки Агаты"
```

**Quest Settings:**
```
Start Policy:
  Policy Type: Manual  (запуск через команду)

Failure Policy:
  Failure Type: None
```

## Шаг 3: Создай Phase

**Phases** ? **Add Element [0]:**
```
Phase ID: Phase_Collect
Title: "Сбор яблок"
Description: "Найди и собери 5 яблок"
bRequireAllObjectives: true
```

## Шаг 4: Создай Objective

**Phases[0] ? Objectives** ? **Add Element [0]:**

```
Objective ID: Collect_Apple_Objective
Title: "Собери яблоки"
Description: "Собери 5 яблок (0/5)"

Style: Collect

Logic: And

Counters:
  Target Count: 5
  Current Count: 0

bIsOptional: false
bIsHidden: false
bTrackOnHUD: true
```

## Шаг 5: ?? КЛЮЧЕВОЙ МОМЕНТ - Настрой Condition!

**Objectives[0] ? Conditions** ? **Add Element [0]:**

```
EventType: OnItemAcquired

EventTag: Inventory.Event.ItemAcquired  ? КРИТИЧНО! Должен совпадать с эмитируемым событием

ItemId: Apple  ? Фильтр: только яблоки

Count: 1  ? Не используется (прогресс из события)

NpcId: None
AreaId: None
ActorClass: None
RequiredTags: []
```

## Шаг 6: Сохрани и протестируй

### В редакторе:

1. **Play (PIE)**
2. Открой консоль: **`~`**
3. Включи логи:
```
GameEventBus.SetVerbose 1
```
4. Запусти квест:
```
Quest.Start CollectApples
```
5. Подбери яблоко (нажми `E` на яблоке)

### Ожидаемый результат в логах:

```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | Apple | Count: 1
LogQuestEventBus: Processing event 'Inventory.Event.ItemAcquired' for 1 active quests
LogQuestEventBus: Event 'Inventory.Event.ItemAcquired' matched objective 'Collect_Apple_Objective' in quest 'CollectApples' - Adding 1 progress
LogQuestProgressManager: Objective progress updated: CollectApples / Collect_Apple_Objective (1/5)
```

### Quest Tracker Widget должен показать:

```
???????????????????????????????????
?  Собери яблоки            ?
?         ?
?  Collect apples (1/5)        ?  ? ОБНОВИЛОСЬ АВТОМАТИЧЕСКИ!
?   ?
?  ????????????????????? 20%      ?
???????????????????????????????????
```

---

## ?? Важные моменты

### ? ОБЯЗАТЕЛЬНО заполни EventTag!

```
EventTag: Inventory.Event.ItemAcquired
```

Без этого система НЕ БУДЕТ работать!

### ? ItemId = фильтр (опционально)

Если **ItemId = None** ? будет считать **ВСЕ** предметы!

Если **ItemId = "Apple"** ? только яблоки.

### ? Target Count = цель

```
Target Count: 5  ? Нужно собрать 5 яблок
```

### ? Logic = способ выполнения

- **And** = ВСЕ conditions должны быть выполнены
- **Or** = ЛЮБАЯ condition выполняет objective
- **Count** = N из M conditions

---

## ?? Тестовые команды

```bash
# Запустить квест
Quest.Start CollectApples

# Проверить статус квеста
Quest.Info CollectApples

# Добавить яблоко вручную (триггерит событие)
Inventory.AddItem Apple 1

# Проверить прогресс objective
Quest.Objective.Info CollectApples Collect_Apple_Objective

# Завершить квест вручную (для теста)
Quest.Complete CollectApples

# Посмотреть все активные квесты
Quest.List Active
```

---

## ?? Checklist для проверки

- [ ] Quest ID заполнен
- [ ] Phase создана
- [ ] Objective создана
- [ ] **EventTag заполнен!** (`Inventory.Event.ItemAcquired`)
- [ ] ItemId = `Apple` (для фильтрации)
- [ ] Target Count = `5`
- [ ] Квест запущен через `Quest.Start CollectApples`
- [ ] Подобрано яблоко
- [ ] Прогресс обновился автоматически!

---

## ?? Следующие шаги

После того как базовый квест работает, попробуй:

1. **Квест с несколькими условиями:**
   - Собери 3 яблока **И** 2 груши
   - Logic = And
   - 2 Conditions

2. **Квест с альтернативами:**
   - Принеси **ЛЮБОЙ** фрукт
   - Logic = Or
   - 3 Conditions (Apple, Pear, Orange)

3. **Диалоговый квест:**
   - EventTag: `Dialogue.Event.NodeReached`
   - DialogNodeId: `Anna_Intro_End`

4. **Квест на убийство:**
   - EventTag: `Combat.Event.EnemyKilled`
   - ActorClass: `BP_Wolf`

---

## ?? Troubleshooting

### Квест не обновляется?

1. Проверь логи:
```
LogQuestEventBus: Processing event...
```

Если нет ? EventTag неправильный!

2. Проверь ItemId:
```
ItemId в Condition должен совпадать с ID предмета!
```

3. Проверь квест активен:
```
Quest.Info CollectApples
? State: Active
```

### Обновляется для всех предметов?

**ItemId = None** ? срабатывает для ВСЕХ!

Заполни ItemId = `Apple`

---

## ? Готово!

Теперь у тебя есть рабочий Quest Asset с автоматическим обновлением!

**Просто подбирай яблоки ? квест обновляется сам!** ???
