# ? ВРЕМЕННЫЙ ФИКС ПРИМЕНЁН!

## ?? Найденная проблема

**QuestEventBus** не имел логики для автоматического обновления квестов при получении событий.

### Что было сломано:

```
GameEventBus эмитирует событие ?
   ?
QuestEventBridge получает событие ?
   ?
QuestEventBridge отправляет в QuestEventBus ?
   ?
QuestEventBus::EmitEvent() бродкастит подписчикам ?
   ?
? НО! Никто не вызывает UpdateObjectiveProgress()!
   ?
? Квесты НЕ обновляются!
```

---

## ?? Применённое решение

Добавлен **ВРЕМЕННЫЙ ФИК in S** в `QuestEventBridge.cpp`:

### Что теперь происходит:

```cpp
void UQuestEventBridge::ForwardToQuestSystem(const FGameEventPayload& GamePayload)
{
    // Проверяем: это событие ItemAcquired?
    if (GamePayload.EventTag.MatchesTag("Inventory.Event.ItemAcquired"))
    {
  FName ItemID = GamePayload.StringParam;  // "Apple"
 int32 Count = GamePayload.IntParam;  // 1
        
 // Получить все активные квесты
   TArray<FQuestSaveData> ActiveQuests = QuestSubsystem->GetActiveQuests();
        
        // Для каждого квеста
        for (const FQuestSaveData& QuestData : ActiveQuests)
        {
       // Попробовать разные варианты ObjectiveID:
      TArray<FName> PossibleIDs = {
         "Collect_Apple",           // Стандартная конвенция
        "ItemAcquired_Apple",      // Event-based naming
 "CollectApples_Objective", // Quest-based naming
            "Apple",     // Просто ID предмета
                "CollectionObjective"      // Универсальная цель
     };
            
      // Пробуем обновить с каждым ID
      for (FName ObjectiveID : PossibleIDs)
            {
       if (QuestSubsystem->UpdateObjectiveProgress(QuestData.QuestId, ObjectiveID, Count))
              {
 // Успешно обновили!
     break;
  }
            }
  }
    }
}
```

---

## ?? Как протестировать

### 1. Скомпилируй проект

```bash
# Rebuild Solution в Visual Studio
```

### 2. Запусти игру

### 3. Включи логи:

```
~  ? Открыть консоль
GameEventBus.SetVerbose 1
GameEventBus.EnableHistory 1
```

### 4. Начни квест "Collect Apples"

```
Quest.Start CollectApples
```

### 5. Подбери яблоко

Нажми `E` на яблоке в мире.

### 6. Проверь лог - должен быть:

```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | StringParam=Apple | IntParam=1
LogQuestBridge: TEMP FIX: ItemAcquired event - Item='Apple' Count=1
LogQuestBridge: TEMP FIX: Checking 1 active quests
LogQuestBridge: TEMP FIX: Successfully updated quest 'CollectApples' objective 'Collect_Apple' (+1)
LogQuest: Objective updated: CollectApples (1/5)
```

### 7. Проверь Quest Tracker Widget

Виджет должен показать: **"Collect apples (1/5)"**

### 8. Собери все 5 яблок

Прогресс должен обновляться: `1/5 ? 2/5 ? 3/5 ? 4/5 ? 5/5`

---

## ?? Какие ObjectiveID поддерживаются

Фикс пробует **5 вариантов** ObjectiveID:

| Вариант | Пример | Когда использовать |
|---------|--------|--------------------|
| `Collect_{ItemID}` | `Collect_Apple` | **РЕКОМЕНДУЕТСЯ!** Стандартная конвенция |
| `ItemAcquired_{ItemID}` | `ItemAcquired_Apple` | Если хочешь явно указать событие |
| `{QuestID}_Objective` | `CollectApples_Objective` | Один objective на весь квест |
| `{ItemID}` | `Apple` | Просто ID предмета |
| `CollectionObjective` | `CollectionObjective` | Универсальная цель сбора |

### Примеры использования:

#### Вариант 1: Collect_{ItemID} (ЛУЧШИЙ!)

**Quest Data Asset:**
```
Quest ID: "CollectApples"
Objective ID: "Collect_Apple"  ? Автоматически работает!
Target Count: 5
```

#### Вариант 2: ItemAcquired_{ItemID}

**Quest Data Asset:**
```
Quest ID: "GatherItems"
Objective ID: "ItemAcquired_Apple"
Target Count: 10
```

#### Вариант 3: {QuestID}_Objective

**Quest Data Asset:**
```
Quest ID: "CollectApples"
Objective ID: "CollectApples_Objective"  ? Универсальный
Target Count: 5
```

**ВАЖНО:** ItemID в ItemDataAsset должен совпадать с используемым в ObjectiveID!

---

##  Известные ограничения ВРЕМЕННОГО ФИКСА

### ? Работает ТОЛЬКО для ItemAcquired

Другие события (Dialogue, Time, Location) **НЕ обрабатываются**!

Нужно будет добавить аналогичную логику для каждого типа события.

### ? Простой перебор ObjectiveID

Фикс пробует все варианты подряд - не оптимально для большого количества квестов.

### ? Не используется data-driven подход

В будущем нужно хранить соответствие Event ? Objective в QuestAsset.

---

## ?? Что дальше делать

### Краткосрочно (для теста):

? Используй конвенцию `Collect_{ItemID}` для objectives  
? Убедись что ItemDataAsset->ItemID соответствует используемому в ObjectiveID

### Долгосрочно (TODO):

1. **Переместить логику в QuestEventBus**
   - Сделать `QuestEventBus::BroadcastEvent()` умнее
   - Добавить маппинг Event Tag ? Quest Objectives

2. **Добавить поддержку других событий**
   - Dialogue.Event.NodeReached
 - Time.Event.HourChanged
   - Location.Event.Entered

3. **Добавить data-driven routing**
   - В QuestObjective добавить поле `EventTag` и `EventFilter`
   - Автоматически находить подходящие objectives

---

## ?? Чеклист готовности к тесту

- [ ] Код скомпилирован
- [ ] DA_Item_Apple создан с ItemID = "Apple"
- [ ] DA_Quest_CollectApples создан
- [ ] Objective ID установлен как один из поддерживаемых вариантов
- [ ] BP_Item_Apple настроен и размещён в мире (5 штук)
- [ ] Quest Tracker Widget добавлен на экран
- [ ] Логи включены: `GameEventBus.SetVerbose 1`

---

## ? Готово!

Теперь система должна работать! Собирай яблоки и наблюдай как квесты обновляются!

Если проблемы остаются - проверь логи и убедись что:
1. События эмитируются
2. QuestEventBridge их получает
3. ObjectiveID совпадает с одним из поддерживаемых вариантов

**Удачи!** ???
