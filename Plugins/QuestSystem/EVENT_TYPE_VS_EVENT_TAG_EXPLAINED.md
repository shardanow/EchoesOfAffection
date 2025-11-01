# ?? EventType vs EventTag: В чём разница?

## ?? Вопрос: Для чего нужен EventType dropdown если мы используем EventTag?

Отличный вопрос! Давайте разберём:

---

## ?? EventType (Enum Dropdown)

### Что это:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
EConditionEventType EventType = EConditionEventType::Custom;
```

**Enum** с предопределёнными типами событий:

```cpp
enum class EConditionEventType : uint8
{
    OnItemAcquired,      // Item Acquired
    OnItemUsed,          // Item Used  
    OnItemGifted,        // Item Gifted
    OnNpcTalked,   // NPC Talked
    OnDialogChoice,      // Dialog Choice
    OnKillActor,  // Kill Actor
    OnEnterArea,         // Enter Area
    OnNeedChanged,       // Need Changed
    OnAffectionChanged,  // Affection Changed
    OnTimeReached,     // Time Reached
    OnDayChanged,        // Day Changed
    OnWeatherChanged,    // Weather Changed
    Custom     // Custom Event (использует EventTag)
};
```

### Для чего:

1. **Удобство для дизайнеров** — быстрый выбор из списка распространённых событий
2. **Визуальная подсказка** — видно какой тип события без изучения тегов
3. **Валидация** — можно показывать/скрывать поля в зависимости от типа
4. **Legacy support** — старые квесты могут использовать enum

---

## ??? EventTag (Gameplay Tag)

### Что это:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (Categories = "GameEvent"))
FGameplayTag EventTag;
```

**Gameplay Tag** для точного указания события:

- `GameEvent.Inventory.ItemAcquired`
- `GameEvent.Inventory.ItemUsed`
- `GameEvent.Dialogue.Started`
- `GameEvent.Quest.Completed`

### Для чего:

1. **Гибкость** — можно создавать любые кастомные события
2. **Иерархия** — теги поддерживают вложенность (`GameEvent.Inventory.*`)
3. **Интеграция** — используется в GameEventBus для роутинга событий
4. **Extensibility** — легко добавлять новые события без изменения кода

---

## ?? Как они работают вместе?

### Вариант 1: Использовать EventType (УСТАРЕЛО)

**Старый подход** (до интеграции с GameEventBus):

```
EventType: OnItemAcquired  ? Выбираем из dropdown
EventTag: [пусто]
ItemId: RedApple
```

**Проблема:** Не работает с новой системой GameEventBus!

---

### Вариант 2: Использовать EventTag (РЕКОМЕНДУЕТСЯ)

**Новый подход** (с GameEventBus):

```
EventType: Custom  ? Всегда Custom!
EventTag: GameEvent.Inventory.ItemAcquired  ? Точный тег
ItemId: RedApple
```

**Работает:** GameEventBus эмитирует события с тегами `GameEvent.*`

---

## ?? Сравнение:

| Характеристика | EventType (Enum) | EventTag (Tag) |
|----------------|------------------|----------------|
| **Тип** | Предопределённый список | Любой Gameplay Tag |
| **Гибкость** | Ограничен enum | Бесконечная |
| **Интеграция** | НЕ работает с GameEventBus ? | Работает с GameEventBus ? |
| **Визуальность** | Удобный dropdown | Нужно знать имя тега |
| **Extensibility** | Требует изменения кода | Добавляй теги в конфиг |
| **Статус** | **Legacy / Deprecated** | **Актуальный** |

---

## ? Рекомендация:

### Используйте ТОЛЬКО EventTag!

**Правильная настройка:**

```
EventType: Custom  ? Игнорируйте, оставьте Custom
EventTag: GameEvent.Inventory.ItemAcquired  ? ВАЖНО!
ItemId: RedApple
Count: 5
```

### Почему EventType всё ещё есть?

1. **Обратная совместимость** — старые квесты могут его использовать
2. **Будущее улучшение** — можно добавить автозаполнение EventTag при выборе EventType
3. **UI подсказки** — планируется показывать релевантные поля в зависимости от типа

---

## ?? Возможное будущее улучшение:

Можно добавить логику которая **автоматически заполняет EventTag** при выборе EventType:

```cpp
// В кастомизации редактора:
if (EventType == EConditionEventType::OnItemAcquired)
{
    EventTag = FGameplayTag::RequestGameplayTag("GameEvent.Inventory.ItemAcquired");
}
else if (EventType == EConditionEventType::OnItemUsed)
{
    EventTag = FGameplayTag::RequestGameplayTag("GameEvent.Inventory.ItemUsed");
}
// ...
```

Но **сейчас** это НЕ реализовано, поэтому:

---

## ?? Итого:

### EventType — что это:
- **Удобный dropdown** с распространёнными типами событий
- **Legacy feature** для старых квестов
- **Визуальная подсказка** для дизайнеров

### EventTag — что это:
- **Реальный тег** который используется системой
- **Обязательное поле** для работы с GameEventBus
- **Гибкая система** для любых кастомных событий

### Что использовать:
? **EventTag** — ВСЕГДА!
? **EventType** — игнорируйте (оставьте `Custom`)

---

## ?? Пример правильного квеста:

```
Quest: "Collect Apples"

Objective 1: "Collect 5 apples"
  Conditions:
    [0]:
      EventType: Custom  ? Игнорируем
   EventTag: GameEvent.Inventory.ItemAcquired  ? ИСПОЛЬЗУЕМ!
   ItemId: RedApple
      Count: 5

Objective 2: "Eat 2 apples"
  Conditions:
    [0]:
      EventType: Custom  ? Игнорируем
      EventTag: GameEvent.Inventory.ItemUsed  ? ИСПОЛЬЗУЕМ!
      ItemId: RedApple
Count: 2
```

**КВЕСТЫ ЗАРАБОТАЮТ!** ???

---

## ?? Дополнительная документация

- **[Полный справочник по Conditions](QUEST_CONDITIONS_COMPLETE_REFERENCE.md)** — все доступные типы условий
- **[Быстрая памятка](QUEST_CONDITIONS_QUICK_REFERENCE.md)** — шаблоны и примеры

**ВСЁ ПОНЯТНО!** ???
