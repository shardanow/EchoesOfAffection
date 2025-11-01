# ?? Полный справочник по Objective Conditions (Условия квестов)

## ?? Что такое Objective Condition?

**Objective Condition** — это условие которое определяет **когда и как** objective (задача) квеста получает прогресс.

Каждый Objective может иметь **несколько условий** (Conditions), и прогресс засчитывается когда **хотя бы одно условие** выполнено.

---

## ?? Анатомия Condition

Каждое условие состоит из:

### 1?? EventTag (обязательно!)

**Главное поле** — определяет **тип события** которое нужно отслеживать:

```
EventTag: GameEvent.Inventory.ItemAcquired
```

**БЕЗ EventTag условие НЕ РАБОТАЕТ!**

### 2?? Фильтры (опционально)

Дополнительные фильтры уточняют **что именно** должно произойти:

- `ItemId` — конкретный предмет
- `NpcId` — конкретный NPC
- `AreaId` — конкретная зона
- `RequiredTags` — нужные теги
- `Count` — сколько раз должно произойти

---

## ?? ВСЕ ДОСТУПНЫЕ УСЛОВИЯ

### ?? 1. Inventory Events (События инвентаря)

#### ? Item Acquired — Подобрать предмет

**EventTag:** `GameEvent.Inventory.ItemAcquired`

**Когда срабатывает:** Игрок подобрал/получил предмет

**Пример:**
```
Objective: "Collect 5 apples"
  Condition:
  EventTag: GameEvent.Inventory.ItemAcquired
    ItemId: RedApple
    Count: 5
```

**Что проверяется:**
- ? `EventTag` совпадает
- ? `ItemId` совпадает (если указан)
- ? Прогресс +1 за каждый подобранный предмет

---

#### ? Item Used — Использовать предмет

**EventTag:** `GameEvent.Inventory.ItemUsed`

**Когда срабатывает:** Игрок использовал/съел/применил предмет

**Пример:**
```
Objective: "Eat 2 apples"
  Condition:
    EventTag: GameEvent.Inventory.ItemUsed
    ItemId: RedApple
    Count: 2
```

**Что проверяется:**
- ? `EventTag` совпадает
- ? `ItemId` совпадает (если указан)
- ? Прогресс +1 за каждое использование

---

#### ? Item Crafted — Скрафтить предмет

**EventTag:** `GameEvent.Inventory.ItemCrafted`

**Когда срабатывает:** Игрок скрафтил предмет

**Пример:**
```
Objective: "Craft 3 healing potions"
  Condition:
  EventTag: GameEvent.Inventory.ItemCrafted
    ItemId: HealingPotion
    Count: 3
```

---

#### ? Item Gifted — Подарить предмет NPC

**EventTag:** `GameEvent.Inventory.ItemGifted`

**Когда срабатывает:** Игрок подарил предмет NPC

**Пример:**
```
Objective: "Give flowers to Alice"
  Condition:
    EventTag: GameEvent.Inventory.ItemGifted
    ItemId: RedRose
    NpcId: Alice
    Count: 1
```

**Что проверяется:**
- ? `EventTag` совпадает
- ? `ItemId` совпадает
- ? **`NpcId` совпадает** (проверка получателя!)

---

#### ? Item Dropped — Выбросить предмет

**EventTag:** `GameEvent.Inventory.ItemDropped`

**Когда срабатывает:** Игрок выбросил предмет

**Пример:**
```
Objective: "Drop 5 trash items"
  Condition:
    EventTag: GameEvent.Inventory.ItemDropped
    RequiredTags: Item.Type.Trash
    Count: 5
```

---

### ?? 2. Dialogue Events (События диалогов)

#### ? NPC Talked — Поговорить с NPC

**EventTag:** `GameEvent.Dialogue.Started`

**Когда срабатывает:** Игрок начал диалог с NPC

**Пример:**
```
Objective: "Talk to Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    NpcId: Lianne
    Count: 1
```

**Что проверяется:**
- ? `EventTag` совпадает
- ? **`NpcId` совпадает** (проверка с кем говорим!)

---

#### ? Dialog Choice — Выбрать вариант в диалоге

**EventTag:** `GameEvent.Dialogue.ChoiceSelected`

**Когда срабатывает:** Игрок выбрал конкретный вариант ответа

**Пример:**
```
Objective: "Agree to help Alice"
  Condition:
    EventTag: GameEvent.Dialogue.ChoiceSelected
    DialogNodeId: Choice_HelpAlice
    RequiredTags: Dialogue.Choice.Agree
    Count: 1
```

**Что проверяется:**
- ? `EventTag` совпадает
- ? `DialogNodeId` совпадает (если указан)
- ? `RequiredTags` проверяются (например, тип выбора)

---

#### ? Dialogue Completed — Завершить диалог

**EventTag:** `GameEvent.Dialogue.Completed`

**Когда срабатывает:** Диалог завершён

**Пример:**
```
Objective: "Finish conversation with merchant"
  Condition:
    EventTag: GameEvent.Dialogue.Completed
    NpcId: Merchant
    Count: 1
```

---

### ?? 3. Quest Events (События квестов)

#### ? Quest Started — Начать квест

**EventTag:** `GameEvent.Quest.Started`

**Когда срабатывает:** Игрок начал другой квест

**Пример:**
```
Objective: "Start 3 side quests"
  Condition:
    EventTag: GameEvent.Quest.Started
    RequiredTags: Quest.Type.SideQuest
    Count: 3
```

---

#### ? Quest Completed — Завершить квест

**EventTag:** `GameEvent.Quest.Completed`

**Когда срабатывает:** Игрок завершил другой квест

**Пример:**
```
Objective: "Complete 'Collect Apples' quest"
  Condition:
    EventTag: GameEvent.Quest.Completed
    NpcId: Q_CollectApples  // ID квеста в NpcId
    Count: 1
```

---

### ??? 4. Location Events (События локаций)

#### ? Enter Area — Войти в зону

**EventTag:** `GameEvent.Location.EnterArea`

**Когда срабатывает:** Игрок вошёл в триггер-зону

**Пример:**
```
Objective: "Visit the library"
  Condition:
    EventTag: GameEvent.Location.EnterArea
    AreaId: Library
    Count: 1
```

**Что проверяется:**
- ? `EventTag` совпадает
- ? **`AreaId` совпадает** (ID зоны!)

---

#### ? Leave Area — Покинуть зону

**EventTag:** `GameEvent.Location.LeaveArea`

**Когда срабатывает:** Игрок вышел из триггер-зоны

**Пример:**
```
Objective: "Leave the dungeon"
  Condition:
    EventTag: GameEvent.Location.LeaveArea
    AreaId: Dungeon
    Count: 1
```

---

### ? 5. Time Events (События времени)

#### ? Time Reached — Дождаться времени

**EventTag:** `GameEvent.Time.Reached`

**Когда срабатывает:** Наступило определённое время суток

**Пример:**
```
Objective: "Wait until midnight"
  Condition:
 EventTag: GameEvent.Time.Reached
    TimeRangeStart: 0  // 00:00
    TimeRangeEnd: 1    // 01:00
    Count: 1
```

**Поля:**
- `TimeRangeStart` — начало диапазона (часы 0-24)
- `TimeRangeEnd` — конец диапазона (часы 0-24)

---

#### ? Day Changed — Смена дня

**EventTag:** `GameEvent.Time.DayChanged`

**Когда срабатывает:** Наступил новый день

**Пример:**
```
Objective: "Wait 3 days"
  Condition:
    EventTag: GameEvent.Time.DayChanged
    Count: 3
```

---

### ??? 6. Weather Events (События погоды)

#### ? Weather Changed — Смена погоды

**EventTag:** `GameEvent.Weather.Changed`

**Когда срабатывает:** Погода изменилась

**Пример:**
```
Objective: "Wait for rain"
  Condition:
    EventTag: GameEvent.Weather.Changed
    WeatherTags: Weather.Rain
    Count: 1
```

**Поля:**
- `WeatherTags` — нужные теги погоды

---

### ?? 7. Affection Events (События отношений)

#### ? Affection Changed — Изменение привязанности

**EventTag:** `GameEvent.Affection.Changed`

**Когда срабатывает:** Привязанность с NPC изменилась

**Пример:**
```
Objective: "Reach 50% affection with Alice"
  Condition:
    EventTag: GameEvent.Affection.Changed
    NpcId: Alice
    ThresholdValue: 0.5  // 50%
    Count: 1
```

**Поля:**
- `NpcId` — ID NPC
- `ThresholdValue` — минимальное значение (0.0 - 1.0)

---

### ?? 8. Needs Events (События потребностей)

#### ? Need Changed — Изменение потребности

**EventTag:** `GameEvent.Need.Changed`

**Когда срабатывает:** Потребность (голод, энергия, и т.д.) изменилась

**Пример:**
```
Objective: "Restore hunger to 80%"
  Condition:
    EventTag: GameEvent.Need.Changed
    RequiredTags: Need.Hunger
 ThresholdValue: 0.8  // 80%
    Count: 1
```

**Поля:**
- `RequiredTags` — тег потребности (Need.Hunger, Need.Energy)
- `ThresholdValue` — целевое значение (0.0 - 1.0)

---

## ?? Как работает проверка условий?

Когда событие происходит, система проверяет:

### 1?? EventTag (ОБЯЗАТЕЛЬНО)

```cpp
if (Condition.EventTag != Payload.EventTag)
{
    return false; // Не совпадает — отклонить
}
```

### 2?? ItemId (если указан)

```cpp
if (!Condition.ItemId.IsNone() && Condition.ItemId != Payload.StringParam)
{
    return false; // Не тот предмет — отклонить
}
```

### 3?? NpcId (если указан)

```cpp
if (!Condition.NpcId.IsNone() && Condition.NpcId != Payload.StringParam)
{
    return false; // Не тот NPC — отклонить
}
```

### 4?? RequiredTags (если указаны)

```cpp
if (Condition.RequiredTags.Num() > 0)
{
if (!Payload.TagsParam.HasAll(Condition.RequiredTags))
    {
        return false; // Теги не совпадают — отклонить
    }
}
```

### ? Все проверки прошли — засчитать прогресс!

```cpp
Manager->UpdateObjectiveProgress(QuestId, ObjectiveId, ProgressAmount);
```

---

## ?? Примеры сложных квестов

### Пример 1: Собрать ресурсы

```
Quest: "Gather Resources"

Phase 1: "Collect Materials"
  Objective 1: "Collect 10 wood"
    Condition:
    EventTag: GameEvent.Inventory.ItemAcquired
      ItemId: Wood
Count: 10

  Objective 2: "Collect 5 stones"
    Condition:
      EventTag: GameEvent.Inventory.ItemAcquired
      ItemId: Stone
  Count: 5
```

---

### Пример 2: Поговорить с NPC

```
Quest: "Village Gossip"

Phase 1: "Talk to Villagers"
  Objective 1: "Talk to Alice"
    Condition:
    EventTag: GameEvent.Dialogue.Started
      NpcId: Alice
      Count: 1

  Objective 2: "Talk to Bob"
    Condition:
      EventTag: GameEvent.Dialogue.Started
    NpcId: Bob
      Count: 1

  Objective 3: "Talk to Charlie"
    Condition:
      EventTag: GameEvent.Dialogue.Started
      NpcId: Charlie
   Count: 1
```

---

### Пример 3: Подарить предметы

```
Quest: "Birthday Gifts"

Phase 1: "Give Presents"
  Objective 1: "Give flowers to Alice"
    Condition:
      EventTag: GameEvent.Inventory.ItemGifted
      ItemId: RedRose
      NpcId: Alice
      Count: 1

  Objective 2: "Give book to Lianne"
  Condition:
      EventTag: GameEvent.Inventory.ItemGifted
      ItemId: OldBook
    NpcId: Lianne
      Count: 1
```

---

### Пример 4: Комбинированные условия

```
Quest: "Master Crafter"

Phase 1: "Craft Items"
  Objective 1: "Craft any 5 items"
    Condition [0]:
      EventTag: GameEvent.Inventory.ItemCrafted
      ItemId: [любой]  // Не указываем ItemId!
    Count: 5

Phase 2: "Gift Crafted Items"
  Objective 1: "Gift 3 crafted items to NPCs"
    Condition [0]:
      EventTag: GameEvent.Inventory.ItemGifted
      RequiredTags: Item.Crafted  // Только скрафченные!
  Count: 3
```

---

### Пример 5: Временные квесты

```
Quest: "Night Owl"

Phase 1: "Visit Library at Night"
  Objective 1: "Enter library between 20:00 and 06:00"
    Condition:
      EventTag: GameEvent.Location.EnterArea
    AreaId: Library
      TimeRangeStart: 20  // 20:00
   TimeRangeEnd: 6     // 06:00
      Count: 1
```

---

## ?? Частые ошибки

### ? Ошибка 1: Пустой EventTag

```
Condition:
  EventTag: [пусто]  ? ОШИБКА!
  ItemId: RedApple
```

**Исправление:**
```
Condition:
  EventTag: GameEvent.Inventory.ItemAcquired  ? ОБЯЗАТЕЛЬНО!
  ItemId: RedApple
```

---

###? Ошибка 2: Неправильный тег

```
Condition:
  EventTag: Quest.Event.Item.Acquired  ? Старый тег! Не работает!
  ItemId: RedApple
```

**Исправление:**
```
Condition:
EventTag: GameEvent.Inventory.ItemAcquired  ? Правильный!
  ItemId: RedApple
```

---

### ? Ошибка 3: Перепутаны ItemId и NpcId

```
Objective: "Talk to Alice"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    ItemId: Alice  ? ОШИБКА! Это NPC, не предмет!
```

**Исправление:**
```
Objective: "Talk to Alice"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    NpcId: Alice  ? Правильно!
```

---

## ?? Итого: Что можно делать?

### ? Работает СЕЙЧАС:

| Тип | EventTag | Что отслеживает |
|-----|----------|-----------------|
| Inventory | `GameEvent.Inventory.ItemAcquired` | Подбор предметов |
| Inventory | `GameEvent.Inventory.ItemUsed` | Использование предметов |
| Inventory | `GameEvent.Inventory.ItemCrafted` | Крафт предметов |
| Inventory | `GameEvent.Inventory.ItemGifted` | Дарение предметов NPC |
| Inventory | `GameEvent.Inventory.ItemDropped` | Выброс предметов |
| Dialogue | `GameEvent.Dialogue.Started` | **Разговор с NPC** ? |
| Dialogue | `GameEvent.Dialogue.ChoiceSelected` | Выбор в диалоге |
| Dialogue | `GameEvent.Dialogue.Completed` | Завершение диалога |
| Quest | `GameEvent.Quest.Started` | Начало квеста |
| Quest | `GameEvent.Quest.Completed` | Завершение квеста |

### ?? ТРЕБУЕТ ИНТЕГРАЦИИ:

| Тип | EventTag | Статус |
|-----|----------|--------|
| Location | `GameEvent.Location.EnterArea` | ? Нужен триггер-компонент |
| Location | `GameEvent.Location.LeaveArea` | ? Нужен триггер-компонент |
| Time | `GameEvent.Time.Reached` | ?? Частично (есть система времени) |
| Time | `GameEvent.Time.DayChanged` | ?? Частично (есть система времени) |
| Weather | `GameEvent.Weather.Changed` | ? Нужна система погоды |
| Affection | `GameEvent.Affection.Changed` | ? Нужна система отношений |
| Needs | `GameEvent.Need.Changed` | ? Есть! (NeedsSubsystem) |

---

## ?? Рекомендации

### Для дизайнеров квестов:

1. **ВСЕГДА** указывайте `EventTag`!
2. Используйте фильтры (`ItemId`, `NpcId`) для точности
3. Проверяйте логи — там видно что не совпадает
4. Тестируйте каждый objective отдельно

### Для программистов:

1. Для новых событий — добавляйте теги в `DefaultGameplayTags.ini`
2. Создавайте `Emit*` функции в `QuestEventBus` или `GameEventBus`
3. Проверяйте что `StringParam` / `IntParam` правильно заполнены
4. Используйте логирование для отладки

---

**КВЕСТЫ ПОЛНОСТЬЮ ФУНКЦИОНАЛЬНЫ!** ?????
