# ?? БЫСТРЫЙ СТАРТ: Тестирование автоматического обновления квестов

## ? Статус: ГОТОВО К ТЕСТИРОВАНИЮ!

Применён **временный фикс** для автоматического обновления квестов при сборе предметов.

---

## ?? Шаги для теста

### 1. Скомпилируй проект

**Visual Studio:**
```
Build > Build Solution
```

Или:
```
Rebuild только QuestSystemRuntime plugin
```

### 2. Создай Data Assets (если ещё не создавал)

#### A. Item Data Asset

**Путь:** `Content/Game/Core/Subsystems/Inventory/Items/`

**Имя файла:** `DA_Item_Apple`

**Настройки:**
```
Item ID: "Apple"  ? ВАЖНО! Без пробелов
Item Name: "Red Apple"
Item Description: "A juicy red apple"
Max Stack Size: 99
? Is Stackable
? Can Be Picked Up
? Can Be Dropped
```

#### B. Quest Data Asset

**Путь:** `Content/Game/Quests/`

**Имя файла:** `DA_Quest_CollectApples`

**Настройки:**
```
Quest ID: "CollectApples"
Title: "Собери яблоки"
Description: "Собери 5 яблок для фермера"

Objectives[0]:
  Objective ID: "Collect_Apple"  ? ВАЖНО! Формат: Collect_{ItemID}
  Description: "Collect apples"
  Target Count: 5
  ? Is Hidden
  ? Is Optional
```

#### C. World Item Blueprint

**Путь:** `Content/Game/Items/`

**Имя:** `BP_Item_Apple`

**Parent Class:** `WorldItemActor`

**Настройки:**
```
World Item Actor:
  Item Data: DA_Item_Apple
  Stack Size: 1
  ? Can Be Picked Up
  ? Auto Create Interactable
```

Добавь **Static Mesh** яблока и настрой **Transform**.

#### D. Quest Giver NPC

Найди или создай `BP_QuestGiver` / `BP_Lianne`:

```
Quest Giver Component:
  Available Quests:
    [0] = DA_Quest_CollectApples
```

---

### 3. Размести в Level

1. Перетащи **BP_Item_Apple** в мир (минимум 5 штук)
2. Размести **BP_QuestGiver** (NPC дающий квест)
3. Убедись что у игрока есть **Quest Tracker Widget** на экране

---

### 4. Запусти игру (PIE)

**Alt + P** или **Play in Editor**

---

### 5. Включи логирование

**Открой консоль:** `~`

```
GameEventBus.SetVerbose 1
GameEventBus.EnableHistory 1
```

---

### 6. Начни квест

Подойди к NPC ? Нажми `E` ? Выбери квест "Собери яблоки"

Или через консоль:
```
Quest.Start CollectApples
```

---

### 7. Собери яблоко

Подойди к яблоку ? Нажми `E`

---

### 8. Проверь результат

#### A. Лог консоли должен показать:

```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | Apple | Count: 1
LogQuestBridge: TEMP FIX: ItemAcquired event - Item='Apple' Count=1
LogQuestBridge: TEMP FIX: Checking 1 active quests
LogQuestBridge: TEMP FIX: Successfully updated quest 'CollectApples' objective 'Collect_Apple' (+1)
LogQuest: Objective progress updated: CollectApples / Collect_Apple (1/5)
```

#### B. Quest Tracker Widget должен обновиться:

```
????????????????????????????????
?  Собери яблоки   ?
?        ?
?  Collect apples (1/5)  ? ОБНОВИЛОСЬ!
?             ?
?  ??????????????????? 20%    ?
????????????????????????????????
```

#### C. Проверь историю событий:

```
GameEventBus.PrintHistory
```

Должен быть вывод:
```
[0] Inventory.Event.ItemAcquired | Apple | 1
```

---

### 9. Собери все 5 яблок

Повторяй шаг 7 пока не соберёшь все яблоки.

Прогресс должен обновляться:
```
Collect apples (1/5) ? 
Collect apples (2/5) ? 
Collect apples (3/5) ? 
Collect apples (4/5) ? 
Collect apples (5/5) ? COMPLETE!
```

При сборе последнего яблока квест должен автоматически завершиться!

---

## ?? Отладка если не работает

### Проблема 1: События не эмитируются

**Проверь лог:**
```
GameEventBus.PrintHistory
```

Если пусто:
- Проверь что `DA_Item_Apple->ItemID = "Apple"`
- Проверь что `BP_Item_Apple->ItemData` указывает на `DA_Item_Apple`
- Убедись что GameEventBus plugin включён:
  - `Edit ? Plugins ? Search "GameEventBus"`
  - ? Enabled

---

### Проблема 2: События эмитируются, но квест не обновляется

**Проверь ObjectiveID:**

Фикс пробует следующие варианты (в порядке приоритета):

1. `Collect_Apple` ? **РЕКОМЕНДУЕТСЯ!**
2. `ItemAcquired_Apple`
3. `CollectApples_Objective`
4. `Apple`
5. `CollectionObjective`

**Попробуй использовать один из этих вариантов в Quest Data Asset!**

**Если используешь свой формат:**

Добавь логирование:
```
GameEventBus.SetVerbose 1
```

И проверь лог - увидишь какой ObjectiveID система пытается обновить.

---

### Проблема 3: Квест не запускается

Проверь:
- Quest Asset правильно настроен
- Quest Giver Component содержит квест в `Available Quests`
- Игрок взаимодействует с NPC корректно

Попробуй запустить вручную:
```
Quest.Start CollectApples
```

---

### Проблема 4: GameplayTag не найден

**Проверь файл:** `Config/DefaultGameplayTags.ini`

Должна быть строка:
```ini
+GameplayTagList=(Tag="Inventory.Event.ItemAcquired",DevComment="")
```

**Перезапусти редактор!** (Теги загружаются только при старте)

Или:
**Tools ? Refresh Gameplay Tags**

---

## ?? Поддерживаемые форматы ObjectiveID

| Формат | Пример | Когда использовать |
|--------|--------|-------------------|
| `Collect_{ItemID}` | `Collect_Apple` | **ЛУЧШИЙ!** Стандарт |
| `ItemAcquired_{ItemID}` | `ItemAcquired_Apple` | Явный event |
| `{QuestID}_Objective` | `CollectApples_Objective` | Универсальная цель |
| `{ItemID}` | `Apple` | Простой вариант |
| `CollectionObjective` | `CollectionObjective` | Общая цель |

---

## ?? Известные ограничения

- ? Работает для `Inventory.Event.ItemAcquired`
- ? НЕ работает для других событий:
  - Dialogue.Event.*
  - Time.Event.*
  - Location.Event.*
  - Combat.Event.*

Для других типов квестов нужно будет добавить аналогичную логику!

---

## ? Чеклист готовности

- [ ] Код скомпилирован
- [ ] DA_Item_Apple создан (ItemID = "Apple")
- [ ] DA_Quest_CollectApples создан (ObjectiveID = "Collect_Apple")
- [ ] BP_Item_Apple настроен
- [ ] 5+ яблок размещены в уровне
- [ ] Quest Giver настроен с квестом
- [ ] Quest Tracker Widget добавлен на UI
- [ ] GameEventBus plugin включён
- [ ] Редактор перезапущен

---

## ?? Готово!

Теперь система автоматически обновляет квесты при сборе предметов!

**Удачи с тестированием!** ???

---

## ?? Дополнительные материалы

- `CRITICAL_BUG_FOUND.md` - Детальное описание проблемы
- `TEMP_FIX_APPLIED.md` - Техническая документация фикса
- `DIAGNOSIS_SUMMARY.md` - Полная диагностика и анализ
- `HOW_TO_CREATE_QUEST_WITH_EVENTS.md` - Оригинальная инструкция

**Если возникнут проблемы - читай эти файлы!**
