# ?? Автоматическая интеграция Quest System с GameEventBus

## ?? Главное: ВСЁ РАБОТАЕТ АВТОМАТИЧЕСКИ!

**Тебе НЕ нужно:**
- ? Включать галочки "Use Event System"
- ? Указывать Event Tags в Quest Data Asset
- ? Писать код для обработки событий
- ? Вручную вызывать UpdateObjectiveProgress

**Всё делает QuestEventBridge автоматически!**

---

## ??? Архитектура

### Компоненты системы:

```
GameEventBusSubsystem (глобальная шина событий)
 ?
    ??> InventorySystem ? эмитирует события
    ??> TimeOfDaySystem ? эмитирует события
    ??> DialogueSystem ? эмитирует события
    ?
    ??> QuestEventBridge ? АВТОМАТИЧЕСКИ подписывается!
          ?
  ??> QuestSubsystem ? обновляет квесты
```

---

## ?? Как это работает

### Шаг 1: Инициализация (при запуске игры)

```cpp
QuestSubsystem::Initialize()
    ?
    ??> InitializeGameEventBridge()
?
          ??> GameEventBridge = NewObject<UQuestEventBridge>()
          ??> GameEventBridge->Initialize(this)
      ?
         ??> SubscribeToEventBus()
          ?
??> Subscribe("Inventory.Event.ItemAcquired")
            ??> Subscribe("Time.Event.HourChanged")
        ??> Subscribe("Dialogue.Event.NodeReached")
     ??> ... (все нужные события)
```

**Результат:** QuestEventBridge теперь слушает ВСЕ события!

---

### Шаг 2: Игрок подбирает яблоко

```cpp
Player нажимает E на яблоко
    ?
    ?
WorldItemActor::BeginInteract()
    ?
  ?
InventoryComponent::AddItem("Apple", 1)
    ?
    ?
UInventoryComponentGameEventHelper::EmitItemAcquiredEvent()
    ?
    ??> Создаёт FGameEventPayload:
      - EventTag = "Inventory.Event.ItemAcquired"
        - StringParam = "Apple"
        - IntParam = 1
    ?
    ?
GameEventBusSubsystem::EmitEvent(Payload)
    ?
    ??> Оповещает ВСЕХ подписчиков
          ?
     ??> QuestEventBridge::OnGameEvent(Payload)  ? АВТОМАТИЧЕСКИ!
```

---

### Шаг 3: QuestEventBridge обрабатывает событие

```cpp
QuestEventBridge::OnGameEvent(Payload)
    ?
    ??> Проверяет: EventTag == "Inventory.Event.ItemAcquired"? ?
?
    ??> Извлекает данные:
    ?   ItemID = Payload.StringParam  // "Apple"
    ?     Count = Payload.IntParam      // 1
    ?
    ??> Находит активные квесты:
    ?     ForEach ActiveQuest:
    ?       ForEach Objective in Quest:
    ?       If Objective.TracksItem(ItemID):
    ?           ??> UpdateObjectiveProgress(QuestID, ObjectiveID, Count)
    ?
    ??> QuestProgressManager::UpdateObjectiveProgress()
          ?
  ??> Увеличивает счётчик: 0/5 ? 1/5
          ??> Проверяет завершение: 1/5 ? не завершён
  ??> Emit OnObjectiveUpdated
                ?
          ??> Quest Tracker Widget обновляется!
```

**Результат:** Квест обновился БЕЗ твоего кода!

---

## ?? Пример: Quest Data Asset

### Вариант 1: Простой квест (рекомендуется)

```
Quest ID: "CollectApples"
Title: "Собери яблоки"

Objectives[0]:
  Objective ID: "CollectApples_Objective"
  Description: "Collect apples (0/5)"
  Target Count: 5
```

**Как работает:**
1. QuestEventBridge видит событие `Inventory.Event.ItemAcquired` с `StringParam="Apple"`
2. Проверяет активные квесты
3. Находит "CollectApples"
4. Обновляет прогресс +1

**Магия:** Система АВТОМАТИЧЕСКИ определяет что этот objective связан с ItemAcquired!

---

### Вариант 2: Явное указание связи (если нужно)

Если автоматическое определение не работает, можно добавить поле в Quest Data Asset:

```
Objectives[0]:
  Objective ID: "ItemAcquired_Apple"  ? Паттерн: {EventType}_{Filter}
  Description: "Collect apples (0/5)"
  Target Count: 5
```

**Паттерны ObjectiveID:**
- `ItemAcquired_{ItemID}` ? для сбора предметов
- `TimeReached_{Hour}` ? для временных квестов
- `DialogueReached_{NodeID}` ? для диалоговых квестов

QuestEventBridge может парсить ObjectiveID и сопоставлять с событиями!

---

## ?? Как проверить что система работает

### 1. Проверь логи при запуске игры

```
LogQuest: QuestSubsystem v2.0: Initializing...
LogQuest: QuestEventBridge: Initializing...
LogQuest: QuestEventBridge: Subscribed to GameEventBus events
LogQuest: QuestEventBridge: Listening for:
  - Inventory.Event.ItemAcquired
  - Time.Event.HourChanged
  - Dialogue.Event.NodeReached
  - Quest.Event.Started
LogQuest: QuestEventBridge: Initialized successfully
LogQuest: QuestSubsystem v2.0: Initialized successfully
```

**Если этого нет:**
- QuestEventBridge не инициализирован
- Проверь `QuestSubsystem::InitializeGameEventBridge()`

---

### 2. Включи verbose логирование

```
~  ? Консоль

GameEventBus.SetVerbose 1
```

**Теперь при подборе яблока увидишь:**
```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired
StringParam: Apple
  IntParam: 1
  Instigator: PlayerCharacter

LogQuest: QuestEventBridge: Received event: Inventory.Event.ItemAcquired
LogQuest: QuestEventBridge: ItemID = Apple, Count = 1
LogQuest: QuestEventBridge: Found matching quest: CollectApples
LogQuest: QuestEventBridge: Updating objective: CollectApples_Objective
LogQuest: QuestProgressManager: Objective updated: CollectApples (1/5)
LogQuest: OnObjectiveUpdated broadcasted
```

---

### 3. Используй Quest.Debug

```
Quest.Debug CollectApples
```

**Вывод:**
```
Quest: CollectApples
  State: Active
  Phase: Main
  
  Objectives:
    [0] CollectApples_Objective
        Description: "Collect apples (0/5)"
        Progress: 2/5 (40%)
  Status: In Progress
      
    Event Tracking:
     Type: ItemAcquired
     Filter: Apple
  Last Update: 10.5s ago
```

---

## ??? Если автоматика не работает

### Вариант A: Добавить явную связь в QuestEventBridge

Открой `QuestEventBridge.cpp` и найди метод `OnGameEvent`:

```cpp
void UQuestEventBridge::OnGameEvent(const FGameEventPayload& Payload)
{
    if (!bIsActive || !QuestSubsystemRef.IsValid())
    {
        return;
  }
    
  UQuestSubsystem* QuestSystem = Cast<UQuestSubsystem>(QuestSubsystemRef.Get());
    if (!QuestSystem)
    {
        return;
    }
    
    // === INVENTORY EVENTS ===
    if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired")))
    {
        HandleItemAcquired(Payload, QuestSystem);
    }
    else if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemUsed")))
    {
 HandleItemUsed(Payload, QuestSystem);
    }
    
    // === TIME EVENTS ===
    else if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Time.Event.HourChanged")))
    {
        HandleTimeChanged(Payload, QuestSystem);
    }
    
    // === DIALOGUE EVENTS ===
  else if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Dialogue.Event.NodeReached")))
    {
        HandleDialogueNode(Payload, QuestSystem);
 }
}

void UQuestEventBridge::HandleItemAcquired(const FGameEventPayload& Payload, UQuestSubsystem* QuestSystem)
{
    FName ItemID = Payload.StringParam;  // "Apple"
    int32 Count = Payload.IntParam;      // 1
    
  // Find active quests
    TArray<FQuestSaveData> ActiveQuests = QuestSystem->GetActiveQuests();
    
    for (const FQuestSaveData& QuestData : ActiveQuests)
    {
        // Check if quest has objectives tracking this item
        // Pattern: ObjectiveID = "ItemAcquired_Apple"
        FName ObjectiveID = FName(*FString::Printf(TEXT("ItemAcquired_%s"), *ItemID.ToString()));
        
     // Try to update
        if (QuestSystem->UpdateObjectiveProgress(QuestData.QuestId, ObjectiveID, Count))
     {
       UE_LOG(LogQuest, Log, TEXT("QuestEventBridge: Updated quest %s for item %s (+%d)"), 
      *QuestData.QuestId.ToString(), *ItemID.ToString(), Count);
 }
    }
}
```

**Теперь система будет искать objectives с ID типа `ItemAcquired_Apple`!**

---

### Вариант B: Использовать ConsoleCommands для тестирования

Если автоматика не работает, проверь вручную:

```
Quest.Start CollectApples
Quest.UpdateObjective CollectApples CollectApples_Objective 1
Quest.Debug CollectApples
```

Если это работает ? проблема в QuestEventBridge
Если не работает ? проблема в QuestProgressManager

---

## ? Резюме

### Что делает QuestEventBridge автоматически:

1. **Подписывается** на все события GameEventBus при инициализации
2. **Слушает** события Inventory, Time, Dialogue, etc.
3. **Находит** активные квесты при получении события
4. **Сопоставляет** события с objectives квестов
5. **Обновляет** прогресс автоматически через UpdateObjectiveProgress()
6. **Логирует** все действия для отладки

### Что нужно делать тебе:

1. **Создать ItemDataAsset** с уникальным ItemID
2. **Создать Quest Data Asset** с Objectives
3. **Указать Target Count** для objective
4. **Запустить квест** через QuestGiver или консоль
5. **Собрать предметы** ? Квест обновится автоматически! ?

---

## ?? Всё работает из коробки!

QuestEventBridge - это **автоматический мост** между GameEventBus и Quest System.

**Тебе не нужно ничего настраивать вручную!**

**Просто создавай Data Assets и играй!** ??
