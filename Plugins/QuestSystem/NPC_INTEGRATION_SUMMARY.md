# Quest System - Архитектура привязки квестов

## ? Правильная архитектура реализована!

### Что создано:

1. **UQuestGiverComponent** - Компонент для NPC и объектов, дающих квесты
2. **Полная документация** по интеграции с NPC
3. **Blueprint-friendly API** для работы с quest givers

---

## Как это работает:

### 1. Component-Based подход (используется в плагине)

```
NPC Actor (BP_NPC_Blacksmith)
?? Mesh Component
?? Quest Giver Component ? Новый компонент!
?  ?? NPC ID: "Blacksmith"
?  ?? Available Quests: [DA_Quest_RepairTools, DA_Quest_ForgeWeapon]
?  ?? Quest Icons (Available, InProgress, Complete)
?  ?? Events (OnQuestAccepted, OnQuestTurnedIn)
?? Box Collision (для взаимодействия)
```

### 2. Привязка в редакторе:

**Шаг 1: Открыть BP NPC**
- Например, `BP_NPC_Blacksmith`

**Шаг 2: Добавить Quest Giver Component**
```
Components Panel ? Add Component ? Quest Giver Component
```

**Шаг 3: Настроить в Details**
```
Quest Giver Component:
  NPC ID: Blacksmith (можно оставить Auto Assign)
  Available Quests:
    [0]: /Game/Quests/DA_Quest_RepairHammer
    [1]: /Game/Quests/DA_Quest_ForgeWeapon
```

**Шаг 4: В Quest Asset указать Quest Giver**
```
DA_Quest_RepairHammer:
  Quest Giver ID: Blacksmith
  Quest Location: Village_Smithy
```

---

## Преимущества этого подхода:

### ? Визуальная настройка в редакторе
- Drag & Drop quest assets на NPC
- Видно сразу какие квесты у кого
- Не нужно писать код

### ? Переиспользуемость
- Один компонент ? любой актор
- NPC, сундуки, книги, двери - всё может давать квесты

### ? Автоматическая фильтрация
- Компонент сам проверяет dependencies
- Показывает только доступные квесты
- Скрывает completed non-repeatable

### ? Индикаторы
- Автоматические иконки над NPC
- "!" - доступен новый квест
- "?" - квест в процессе
- "?" (желтый) - можно сдать квест

---

## Примеры использования:

### В Blueprint NPC:

```blueprint
Event OnPlayerPressE:
  ? Get Component Quest Giver Component
  ? Get Available Quests For Player (Player Controller)
  ? For Each Quest:
 ? Show in UI List
  
  ? On UI Quest Clicked:
    ? Quest Giver Component ? Accept Quest
```

### Динамическое добавление квеста:

```blueprint
Event OnStoryEvent:
  ? Get Actor BP_Elder
  ? Get Component Quest Giver Component
  ? Available Quests.Add(DA_Quest_SecretMission)
  ? Reload Quests
```

### Проверка наличия квестов:

```blueprint
Event Tick:
  ? Get Component Quest Giver Component
  ? Has Quest Interaction (Player)
  ? Branch:
      True: Show "E to Talk" prompt
   False: Hide prompt
```

---

## Интеграция с другими системами:

### С Dialog System:
```
Dialog Node: "Hello!"
  Choices:
    ? "Tell me about quests"
      ? Open Quest List from Quest Giver Component
```

### С UI:
```
WBP_QuestDialog:
  Construct:
    ? Get Quest Giver from Target NPC
    ? Populate Quest List
```

### С Save System:
```
Quest completion автоматически сохраняется через UQuestSubsystem
```

---

## Документация:

Полное руководство:
- **[NPCIntegration.md](NPCIntegration.md)** - Подробная документация
- **[QuickStart.md](QuickStart.md)** - Быстрый старт с примером
- **[Examples.md](Examples.md)** - Примеры квестов

---

## Что дальше?

1. **Создайте NPC** с Quest Giver Component
2. **Настройте Available Quests** в Details
3. **Добавьте взаимодействие** (E key)
4. **Создайте UI** для отображения квестов
5. **Протестируйте!**

---

**Всё готово к использованию!** ??

Теперь вы можете:
- ? Привязывать квесты к NPC через редактор
- ? Фильтровать доступные квесты автоматически
- ? Показывать визуальные индикаторы
- ? Интегрировать с dialog/UI системами
