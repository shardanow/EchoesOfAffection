# Quest System - NPC Integration Guide

Полное руководство по привязке квестов к NPC и игровым объектам.

---

## Архитектура интеграции

### Основные подходы:

1. **Component-Based** (? Рекомендуется) - Компонент на NPC
2. **Data Table** - Таблица привязок NPC ? Quests
3. **Actor-Based** - Прямая ссылка в Blueprint NPC

---

## 1. Component-Based подход (Рекомендуется)

### Шаг 1: Добавить компонент к NPC

**В Blueprint NPC:**

1. Открыть Blueprint NPC (например, `BP_QuestGiver_Blacksmith`)
2. **Components Panel ? Add Component ? Quest Giver Component**
3. Настроить в Details:

```
Quest Giver Component:
  NPC ID: Blacksmith (или автоматически из имени актора)
  Auto Assign NPC ID: true
  
  Available Quests:
    [0]: DA_Quest_RepairTools
    [1]: DA_Quest_FindOre
    [2]: DA_Quest_CraftSword
    
  Quest Available Icon: T_QuestAvailable
  Quest In Progress Icon: T_QuestInProgress  
  Quest Complete Icon: T_QuestComplete
```

### Шаг 2: Настроить Quest Asset

В quest asset укажите quest giver:

```
Quest Asset (DA_Quest_RepairTools):
  Quest ID: Q_RepairTools
  Quest Giver ID: Blacksmith
  Quest Location: Village_Smithy
```

### Шаг 3: Создать систему взаимодействия

**В BP_NPC_Base (родительский класс для всех NPC):**

```blueprint
// Event: On Component Begin Overlap (Interaction Trigger)
Event OnBeginOverlap
  ? Get Component Quest Giver Component
  ? Is Valid?
    True:
      ? Has Quest Interaction (Player Controller)
      ? Show Quest Indicator Icon
    False:
    ? Hide Quest Indicator

// Event: Player presses E (Interact)
Event OnInteract
  ? Get Component Quest Giver Component
  ? On Player Interact (Player Controller)
  ? Open Quest Dialog UI
```

### Шаг 4: Создать UI диалога с квестами

**WBP_QuestDialog:**

```blueprint
Event Construct
  ? Get Quest Giver Component (from NPC)
? Get Available Quests For Player
  ? Populate Quest List

Event OnQuestSelected
  ? Accept Quest (Player Controller, Selected Quest Asset)
  ? Close Dialog

Event OnTurnInQuest
  ? Turn In Quest (Player Controller, Quest Asset)
  ? Show Reward UI
  ? Close Dialog
```

---

## 2. Визуальные индикаторы квестов

### Widget Component для иконки над NPC

**BP_NPC с Widget Component:**

```blueprint
Components:
  ?? Mesh
  ?? Quest Giver Component
  ?? Widget Component (WBP_QuestIndicator)
      Position: (0, 0, 200) // Над головой
      Draw Size: (64, 64)

WBP_QuestIndicator:
  Event Tick:
    ? Get Quest Giver Component
    ? Get Quest Indicator Icon (Player)
    ? Set Icon Image
    ? Set Visibility (Hidden if no icon)
```

### Типы иконок:

| Иконка | Значение | Когда показывать |
|--------|----------|------------------|
| ? Желтый восклицательный знак | Доступен новый квест | `GetAvailableQuests().Num() > 0` |
| ? Серый вопросительный знак | Квест в процессе | `GetActiveQuests().Num() > 0` |
| ? Желтый вопросительный знак | Можно сдать квест | `GetCompletableQuests().Num() > 0` |

---

## 3. Примеры интеграции

### Пример 1: Простой Quest Giver (Кузнец)

**BP_NPC_Blacksmith:**

```
Components:
  - Quest Giver Component
    Available Quests:
      - DA_Quest_RepairHammer
      - DA_Quest_ForgeWeapon

Event Graph:
  Event BeginPlay:
    ? Get Quest Giver Component
    ? On Available Quests Changed
      ? Update Quest Indicator

  Event OnPlayerInteract:
    ? Get Quest Giver Component
    ? Get Available Quests For Player
    ? Branch (Has Quests?)
      True:
        ? Open Dialog Widget
        ? Show Quest List
      False:
        ? Play "No quests" Dialog
```

### Пример 2: NPC с несколькими ролями

**BP_NPC_Elder (и Quest Giver, и завершение квестов):**

```
Components:
  - Quest Giver Component (для новых квестов)
  - Quest Receiver Component (для завершения)

Или один Quest Giver Component:
  Available Quests:
    - DA_Quest_MeetElders
    - DA_Quest_VillageHistory
  
  Quest Giver ID: Elder
  (Проверяет и Available, и Completable quests)
```

### Пример 3: Объект как Quest Giver (Сундук, Книга)

**BP_MysteriousChest:**

```
Components:
  - Static Mesh (Chest)
  - Quest Giver Component
    Available Quests:
   - DA_Quest_AncientTreasure
    Auto Assign NPC ID: true (ID будет "BP_MysteriousChest_1")

Event OnPlayerInteract:
  ? Get Quest Giver Component
  ? Accept Quest (Auto-start при взаимодействии)
  ? Play Open Animation
```

---

## 4. Advanced: Data Table подход

Для больших проектов с сотнями NPC.

### Создать Data Table:

**DT_NPCQuests** (структура: `FNPCQuestData`):

```cpp
USTRUCT(BlueprintType)
struct FNPCQuestData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NpcId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<TSoftObjectPtr<UQuestAsset>> AvailableQuests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSoftObjectPtr<UQuestAsset>> CompletableQuests;
};
```

**В таблице:**

| Row Name | NPC ID | Available Quests | Completable Quests |
|----------|--------|------------------|--------------------|
| Blacksmith | Blacksmith | [DA_Quest_RepairTools, DA_Quest_ForgeWeapon] | [DA_Quest_FindOre] |
| Baker | Baker | [DA_Quest_DailyBread] | [DA_Quest_DailyBread] |
| Elder | Elder | [DA_Quest_VillageHistory] | [DA_Quest_MeetElders, DA_Quest_SaveVillage] |

### Использование:

```blueprint
Function: GetNPCQuests (NPC ID) ? Quest List
  ? Get Data Table Row (DT_NPCQuests, NPC ID)
  ? Return Available Quests
```

---

## 5. Интеграция с Dialog System

Если у вас есть система диалогов:

### В Dialog Node:

```
Dialog Node: "Greetings"
  Text: "Hello, traveler!"
  
  Choices:
    [0] "Tell me about quests"
   ? Branch:
   Has Available Quests?
       True: Show Quest List Dialog
        False: "No quests right now"
    
  [1] "I'm here to turn in a quest"
        ? Branch:
 Has Completable Quests?
 True: Show Turn-in Dialog
    False: "You haven't completed any quests"
```

### Auto-Quest на Dialog Choice:

```
Dialog Choice: "Help me!"
  On Selected:
    ? Emit Dialog Choice Event
    ? Start Quest (Q_HelpVillager)
  ? Close Dialog
```

---

## 6. Динамическое добавление/удаление квестов

### Runtime добавление квеста к NPC:

```blueprint
Function: AddQuestToNPC (NPC Actor, Quest Asset)
  ? Get Quest Giver Component (from NPC Actor)
  ? Available Quests.Add(Quest Asset)
  ? Load Quest Assets
  ? On Available Quests Changed (Broadcast)
```

### Удаление после выполнения:

```blueprint
Event OnQuestTurnedIn (Quest Asset, Player)
  ? Available Quests.Remove(Quest Asset)
  ? Save to persistent data
```

---

## 7. Интеграция с World State

### Quest availability по времени суток:

```blueprint
Event OnTimeChanged (Hour)
  ? For Each NPC with Quest Giver Component:
    ? Get Available Quests
    ? Filter by Time Requirements
    ? Update Quest Indicator
```

### Quest availability по погоде:

```blueprint
Event OnWeatherChanged (Weather State)
  ? For Each Quest Giver:
    ? Check Quest Dependencies (Weather)
    ? Show/Hide Quest Icon
```

---

## 8. Best Practices

### ? DO:

1. **Используйте компоненты** - легко добавлять к любым акторам
2. **Кешируйте quest assets** - не загружайте каждый раз заново
3. **Показывайте визуальные индикаторы** - иконки над NPC
4. **Группируйте квесты** - один NPC = один тип квестов (кузнец, торговец, и т.д.)
5. **Асинхронная загрузка** - используйте Streamable Manager
6. **Валидация в редакторе** - проверяйте QuestGiverId совпадает с NPC ID

### ? DON'T:

1. **Не хардкодите** - не вписывайте quest ID в код NPC
2. **Не дублируйте** - используйте компоненты вместо копипасты
3. **Не блокируйте поток** - не загружайте квесты синхронно
4. **Не забывайте cleanup** - удаляйте completed non-repeatable quests

---

## 9. Debugging

### Console Commands:

```
// Показать все Quest Giver Components в уровне
ListAllQuestGivers

// Показать квесты конкретного NPC
ShowNPCQuests Blacksmith

// Добавить квест к NPC runtime
AddQuestToNPC Blacksmith Q_NewQuest
```

### Debug Draw:

```blueprint
Event Tick (if Debug Mode):
  ? For Each Quest Giver Component in World:
    ? Draw Debug String (NPC ID, Available Quests Count)
    ? Draw Debug Sphere (Quest Available = Green, Complete = Yellow)
```

---

## 10. Пример полного workflow

### Создание NPC с квестами:

1. **Создать Quest Asset:**
```
   DA_Quest_FindLostHammer
     Quest ID: Q_FindLostHammer
     Quest Giver ID: Blacksmith
```

2. **Создать или открыть NPC Blueprint:**
   ```
 BP_NPC_Blacksmith
   ```

3. **Добавить Quest Giver Component:**
   ```
   Components ? Add ? Quest Giver Component
   ```

4. **Настроить компонент:**
   ```
 NPC ID: Blacksmith
   Available Quests: [DA_Quest_FindLostHammer]
   Icons: [T_QuestAvailable, T_QuestInProgress, T_QuestComplete]
   ```

5. **Добавить Widget индикатор:**
   ```
   Components ? Add ? Widget Component
   Widget Class: WBP_QuestIndicator
 Location: (0, 0, 200)
   ```

6. **Настроить взаимодействие:**
   ```blueprint
   Event OnPlayerPressedE:
     ? Quest Giver Component ? On Player Interact
? Open Quest Dialog UI
   ```

7. **Протестировать:**
   - Подойти к NPC
- Увидеть иконку "!"
   - Нажать E
   - Принять квест
   - Увидеть иконку "?"

---

## Conclusion

Component-based подход даёт:
- ? Переиспользуемость
- ? Простоту настройки в редакторе
- ? Гибкость (любой актор может давать квесты)
- ? Чистый код без дублирования
- ? Легкую отладку

**Рекомендация**: Используйте `UQuestGiverComponent` для всех NPC и объектов, дающих квесты!

---

**Next Steps:**
- [Dialog Integration](DialogIntegration.md) - Интеграция с системой диалогов
- [UI Examples](UIExamples.md) - Примеры quest UI
- [World Events](WorldEvents.md) - Квесты от мировых событий
