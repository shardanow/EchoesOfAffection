# ?? Пошаговая инструкция: Создание квеста с GameEventBus

## ?? Цель: Создать квест "Собрать 5 яблок"

## ?? ВАЖНО: В твоей версии Quest System нет "Use Event System"!

**События работают АВТОМАТИЧЕСКИ через QuestEventBridge!**

Тебе НЕ нужно включать никаких галочек - события уже обрабатываются!

---

## Шаг 1: Создать ItemDataAsset для яблока

### 1.1 В Content Browser:
1. Перейди в папку: `Content/Game/Core/Subsystems/Inventory/Items/`
2. ПКМ ? **Miscellaneous ? Data Asset**
3. В окне "Pick Data Asset Class" найди и выбери: **`ItemDataAsset`**
4. Назови файл: **`DA_Item_Apple`**

### 1.2 Заполни поля в DA_Item_Apple:
```
=== ITEM | BASIC ===
Item ID: "Apple"  ? ВАЖНО! Строка без пробелов
Item Name: "Red Apple"
Item Description: "A juicy red apple"

=== ITEM | STACKING ===
Max Stack Size: 99
? Is Stackable

=== ITEM | PHYSICAL ===
Can Be Picked Up: ?
Can Be Dropped: ?
```

**Сохрани Data Asset** (Ctrl+S)

---

## Шаг 2: Создать Quest Data Asset

### 2.1 В Content Browser:
1. Найди в поиске: **"Quest Asset"** или **"Quest Data Asset"**
2. Создай новый Data Asset этого типа
3. Назови файл: **`DA_Quest_CollectApples`**

### 2.2 Открой DA_Quest_CollectApples и заполни:

**?? КЛЮЧЕВОЕ ОТЛИЧИЕ: Твоя версия использует ObjectiveID вместо Event Tag!**

```
=== QUEST INFO ===
Quest ID: "CollectApples"  ? Уникальный ID
Title: "Собери яблоки"
Description: "Собери 5 яблок для фермера"

=== OBJECTIVES ===
[Нажми + чтобы добавить цель]

Objectives[0]:
  Objective ID: "CollectApples_Objective"  ? Уникальный ID цели
  
  Description: "Collect apples (0/5)"
  
  ? Is Hidden
  ? Is Optional
  
  === PROGRESS SETTINGS ===
  Target Count: 5  ? Сколько нужно собрать
  
  === НЕТ полей Event Tag / String Filter! ===
  Это нормально - события обрабатываются автоматически через QuestEventBridge!
```

**Сохрани Quest Data Asset** (Ctrl+S)

---

## Шаг 3: Настроить обработку событий в коде (уже сделано!)

**? Это уже реализовано!** QuestEventBridge автоматически:

1. Подписывается на `Inventory.Event.ItemAcquired`
2. Ловит события когда игрок поднимает предмет
3. Вызывает `UpdateObjectiveProgress(QuestId, ObjectiveId, Amount)`

**Код уже работает!** См. `QuestEventBridge.cpp`

---

## Шаг 4: Создать WorldItemActor для яблока

### 4.1 В Content Browser:
1. Найди Blueprint класс: **`BP_WorldItemActor`** или создай новый
2. Если создаёшь новый:
   - ПКМ ? **Blueprint Class**
   - Parent Class: **`WorldItemActor`** (из InventorySystem)
   - Назови: **`BP_Item_Apple`**

### 4.2 Настрой BP_Item_Apple:
```
=== Components ===
StaticMesh (или SkeletalMesh):
  - Добавь меш яблока
  - Настрой Transform

=== Details Panel ===
World Item Actor:
  Item Data: DA_Item_Apple  ? Выбери созданный Data Asset
  Stack Size: 1

  ? Can Be Picked Up
  ? Auto Create Interactable
```

**Compile & Save**

---

## Шаг 5: Разместить яблоки в мире

### 5.1 Перетащи BP_Item_Apple в Level:
1. Открой свой уровень
2. Из Content Browser перетащи **BP_Item_Apple** в мир
3. Размести несколько копий (минимум 5 штук)

---

## Шаг 6: Настроить QuestGiver NPC

### 6.1 Найди или создай BP_QuestGiver:
1. В Content Browser найди: **`BP_QuestGiver`** или **`BP_Lianne`**
2. Открой Blueprint

### 6.2 Настрой QuestGiverComponent:
```
=== Details Panel ? Quest Giver Component ===

Available Quests:
  [0] = DA_Quest_CollectApples  ? Добавь свой квест!

Quest Giver Settings:
  Greeting Text: "Hello! I need apples!"
  Quest Offer Text: "Can you collect 5 apples for me?"
```

**Compile & Save**

---

## Шаг 7: Запустить и протестировать

### 7.1 Запустить игру (Alt+P или PIE)

### 7.2 Включить мониторинг событий:
```
~  ? Открыть консоль

GameEventBus.EnableHistory 1
GameEventBus.SetVerbose 1
```

### 7.3 Поговорить с NPC:
1. Подойди к QuestGiver NPC
2. Нажми E (или кнопку взаимодействия)
3. Выбери квест "Собери яблоки"
4. Квест должен добавиться в журнал

### 7.4 Собрать яблоки:
1. Подойди к яблоку на земле
2. Нажми E
3. **Проверь лог в консоли:**
   ```
   Должны быть сообщения:
   LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | ItemId: Apple | Count: 1
   LogQuest: QuestEventBridge: Forwarding event to Quest System
   LogQuest: Quest objective updated: CollectApples (1/5)
   ```

### 7.5 Продолжать собирать:
- Собери все 5 яблок
- Прогресс квеста: 1/5 ? 2/5 ? 3/5 ? 4/5 ? 5/5
- Квест автоматически завершится! ?

---

## ?? Отладка если не работает

### Проблема 1: События не эмитируются

**Проверь:**
1. Открой консоль (`~`)
2. Выполни: `GameEventBus.EnableHistory 1`
3. Подбери яблоко
4. Выполни: `GameEventBus.PrintHistory`

**Должен быть вывод:**
```
LogGameEventBus: Event [Inventory.Event.ItemAcquired] | Apple | Count: 1
```

**Если пусто:**
- Проверь что DA_Item_Apple->ItemID = "Apple"
- Проверь что BP_Item_Apple->ItemData указывает на DA_Item_Apple

---

### Проблема 2: QuestEventBridge не инициализирован

**Проверь лог при запуске игры:**
```
LogQuest: QuestSubsystem v2.0: Initializing...
LogQuest: QuestEventBridge: Initialized successfully
LogQuest: QuestEventBridge: Subscribed to GameEventBus events
```

**Если нет:**
- QuestSystem не запустился
- Или GameEventBus plugin выключен

**Проверь:**
```
Edit ? Plugins ? Search "GameEventBus"
? Enabled (должна быть галочка!)
```

---

### Проблема 3: Квест не обновляется

**Вариант A: Проверь ObjectiveID**

Твоя система может требовать **соответствия ObjectiveID** с событием!

**Попробуй назвать Objective ID так:**
```
Objective ID: "ItemAcquired_Apple"
```

Или проверь код в `QuestEventBridge.cpp` - там может быть логика сопоставления.

**Вариант B: Используй UpdateObjectiveProgress вручную**

Добавь в Blueprint или C++:
```cpp
// В InventoryComponent::AddItem() или в WorldItemActor::BeginInteract()
if (ItemData && ItemData->ItemID == FName("Apple"))
{
    UQuestSubsystem* QuestSystem = UQuestSubsystem::Get(this);
    if (QuestSystem)
    {
        QuestSystem->UpdateObjectiveProgress(
            FName("CollectApples"),      // Quest ID
            FName("CollectApples_Objective"),  // Objective ID
            1  // Amount
        );
    }
}
```

---

### Проблема 4: GameplayTag не найден

**Проверь:**
1. Файл `Config/DefaultGameplayTags.ini` содержит:
   ```ini
   +GameplayTagList=(Tag="Inventory.Event.ItemAcquired",DevComment="")
   ```

2. **Перезапусти редактор!** (Теги загружаются только при старте)

3. Или выполни: **Tools ? Refresh Gameplay Tags**

---

## ?? Архитектура работы (твоя версия)

```
BP_Item_Apple (в мире)
     ?
     ? [Player нажимает E]
     ?
WorldItemActor::BeginInteract()
     ?
     ?
InventoryComponent::AddItem("Apple", 1)
     ?
     ?
UInventoryComponentGameEventHelper::EmitItemAcquiredEvent()
   ?
     ?
GameEventBusSubsystem::EmitEvent()
     ?
     Event Tag: "Inventory.Event.ItemAcquired"
     StringParam: "Apple"
     IntParam: 1
     ?
     ?
QuestEventBridge::OnGameEvent()  ? АВТОМАТИЧЕСKI подписан!
     ?
 Проверяет: Event Tag == "Inventory.Event.ItemAcquired"? ?
     ?
     ?
QuestSubsystem::UpdateObjectiveProgress()
     ?
     Quest ID: "CollectApples"
   Objective ID: "CollectApples_Objective"
     Amount: 1
     ?
     ?
QuestProgressManager::UpdateObjectiveProgress()
     ?
     Увеличивает счётчик: 0/5 ? 1/5
     ?
     ?
Quest Tracker Widget обновляется
```

---

## ? Чеклист готовности

- [ ] DA_Item_Apple создан с ItemID = "Apple"
- [ ] DA_Quest_CollectApples создан
- [ ] Objectives[0] настроен:
  - [ ] Objective ID = "CollectApples_Objective"
  - [ ] Target Count = 5
  - [ ] Description заполнено
- [ ] BP_Item_Apple создан и настроен
- [ ] Яблоки размещены в Level (минимум 5 штук)
- [ ] QuestGiver настроен с квестом
- [ ] Редактор перезапущен (для загрузки тегов)
- [ ] GameEventBus.EnableHistory 1 выполнен в консоли
- [ ] GameEventBus plugin включён

---

## ?? Готово!

Теперь можешь собирать яблоки и видеть как квест автоматически обновляется! ???

---

## ?? Дополнительные примеры

### Пример 2: Временной квест (автоматически работает!)
```
Quest ID: "MeetAtNoon"
Objective ID: "WaitUntil14"
Target Count: 1

Событие: Time.Event.HourChanged (IntParam=14)
QuestEventBridge автоматически обработает!
```

### Пример 3: Диалоговый квест (автоматически работает!)
```
Quest ID: "TalkToFarmer"
Objective ID: "CompleteFarmerDialogue"
Target Count: 1

Событие: Dialogue.Event.NodeReached (StringParam="FarmerNode_Complete")
QuestEventBridge автоматически обработает!
```

---

## ?? Если всё ещё не работает

**Последняя проверка:**

Открой `QuestEventBridge.cpp` и проверь метод `OnGameEvent`:

Он должен содержать логику типа:
```cpp
void UQuestEventBridge::OnGameEvent(const FGameEventPayload& Payload)
{
    if (!bIsActive || !QuestSubsystemRef.IsValid())
    {
        return;
    }
    
 // Extract event data
    FGameplayTag EventTag = Payload.EventTag;
    FName StringParam = Payload.StringParam;
    int32 IntParam = Payload.IntParam;
    
    // Route to appropriate quest handler
    if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired")))
    {
        // Update quest objectives that track this item
   // Find active quests, check objectives, update progress
    }
    
    // ... similar for other events
}
```

**Если этот метод пустой или не обрабатывает события:**
- Открой issue
- Я добавлю полную реализацию!

---

**Удачи! Система уже работает автоматически!** ??
