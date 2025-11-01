# ?? НАЙДЕНА КРИТИЧЕСКАЯ ПРОБЛЕМА!

## ? Проблема: QuestSubsystem::EmitQuestEvent() НЕ РЕАЛИЗОВАН!

### Что сломано:

```cpp
// QuestEventBridge.cpp передаёт событие:
QuestSubsystem->EmitQuestEvent(QuestPayload);  // ? Вызывается

// НО! В QuestSubsystem.cpp этого метода НЕТ!
```

### Почему квесты не обновляются:

```
Player подбирает яблоко
  ?
InventoryComponent::AddItem("Apple", 1)
  ?
GameEventBus::EmitEvent(Inventory.Event.ItemAcquired) ? РАБОТАЕТ
  ?
QuestEventBridge::OnGameEvent(Payload) ? РАБОТАЕТ
  ?
QuestSubsystem->EmitQuestEvent(QuestPayload) ? МЕТОД НЕ СУЩЕСТВУЕТ!
  ?
? НИЧЕГО НЕ ПРОИСХОДИТ ?
```

---

## ?? Что нужно сделать:

### 1. Добавить метод в QuestSubsystem.cpp

```cpp
void UQuestSubsystem::EmitQuestEvent(const FQuestEventPayload& Payload)
{
    if (!EventBus)
    {
        UE_LOG(LogTemp, Error, TEXT("QuestSubsystem::EmitQuestEvent: EventBus is null!"));
return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("QuestSubsystem::EmitQuestEvent: %s"), *Payload.EventTag.ToString());
    
    // Передать в QuestEventBus
    EventBus->BroadcastEvent(Payload);
}
```

### 2. Проверить что QuestEventBus::BroadcastEvent() обрабатывает события

Нужно добавить логику обновления квестов при получении событий!

---

## ?? Полная диагностика

### Что работает:
- ? GameEventBusSubsystem эмитирует события
- ? InventoryComponentGameEventHelper эмитирует события
- ? QuestEventBridge подписывается и получает события
- ? QuestEventBridge транслирует в QuestSubsystem

### Что сломано:
- ? QuestSubsystem::EmitQuestEvent() не реализован
- ? QuestEventBus::BroadcastEvent() не обновляет квесты
- ? Нет связи между событиями и UpdateObjectiveProgress()

---

## ?? План исправления

### Файл 1: QuestSubsystem.cpp

Добавить реализацию `EmitQuestEvent`:

```cpp
void UQuestSubsystem::EmitQuestEvent(const FQuestEventPayload& Payload)
{
    if (!EventBus)
    {
        UE_LOG(LogTemp, Error, TEXT("EmitQuestEvent: EventBus is null!"));
        return;
    }
    
    // Логирование для отладки
  UE_LOG(LogTemp, Log, TEXT("EmitQuestEvent: Tag=%s, String=%s, Int=%d"), 
        *Payload.EventTag.ToString(), 
        *Payload.StringParam.ToString(), 
        Payload.IntParam);
    
    // Передать событие в EventBus
    EventBus->BroadcastEvent(Payload);
}
```

### Файл 2: QuestEventBus.cpp

Добавить логику обновления квестов:

```cpp
void UQuestEventBus::BroadcastEvent(const FQuestEventPayload& Payload)
{
    UE_LOG(LogTemp, Log, TEXT("QuestEventBus::BroadcastEvent: %s"), *Payload.EventTag.ToString());
    
    // Получить ProgressManager
    UQuestProgressManager* ProgressMgr = GetProgressManager();
    if (!ProgressMgr)
    {
      UE_LOG(LogTemp, Error, TEXT("BroadcastEvent: ProgressManager is null!"));
   return;
    }
    
    // Получить все активные квесты
    TArray<FQuestSaveData> ActiveQuests = ProgressMgr->GetActiveQuests();
    
    UE_LOG(LogTemp, Log, TEXT("BroadcastEvent: Checking %d active quests"), ActiveQuests.Num());
  
    // Для каждого активного квеста
    for (const FQuestSaveData& QuestData : ActiveQuests)
    {
    // Проверить objectives
    // Если objective слушает это событие ? обновить прогресс
        HandleQuestEvent(QuestData.QuestId, Payload);
    }
}

void UQuestEventBus::HandleQuestEvent(FName QuestId, const FQuestEventPayload& Payload)
{
    // ВРЕМЕННОЕ РЕШЕНИЕ: Простая логика для ItemAcquired
    
    if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired")))
    {
        FName ItemID = Payload.StringParam;
 int32 Count = Payload.IntParam;
     
        // Попытаться обновить прогресс
// ObjectiveID может быть любым - попробуем с ItemID
 FName ObjectiveID = FName(*FString::Printf(TEXT("Collect_%s"), *ItemID.ToString()));
        
        UE_LOG(LogTemp, Log, TEXT("Trying to update: Quest=%s, Objective=%s, Amount=%d"),
            *QuestId.ToString(), *ObjectiveID.ToString(), Count);
        
        GetProgressManager()->UpdateObjectiveProgress(QuestId, ObjectiveID, Count);
    }
}
```

---

## ?? ВРЕМЕННОЕ РЕШЕНИЕ (БЫСТРЫЙ ФИКС)

Если нужно быстро протестировать, добавь прямой вызов в QuestEventBridge:

```cpp
void UQuestEventBridge::ForwardToQuestSystem(const FGameEventPayload& GamePayload)
{
    UQuestSubsystem* QuestSubsystem = Cast<UQuestSubsystem>(QuestSubsystemRef.Get());
    if (!QuestSubsystem)
    {
        return;
    }
  
    // ВРЕМЕННЫЙ ФИКС: Прямое обновление для ItemAcquired
    if (GamePayload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired")))
    {
      FName ItemID = GamePayload.StringParam;
        int32 Count = GamePayload.IntParam;
        
        UE_LOG(LogQuestBridge, Log, TEXT("TEMP FIX: ItemAcquired - %s x%d"), *ItemID.ToString(), Count);
        
        // Получить все активные квесты
        TArray<FQuestSaveData> ActiveQuests = QuestSubsystem->GetActiveQuests();
        
  for (const FQuestSaveData& QuestData : ActiveQuests)
   {
            // Попытка с разными вариантами ObjectiveID
        TArray<FName> PossibleObjectiveIDs = {
   FName(*FString::Printf(TEXT("Collect_%s"), *ItemID.ToString())),
  FName(*FString::Printf(TEXT("ItemAcquired_%s"), *ItemID.ToString())),
   FName(*FString::Printf(TEXT("%s_Objective"), *QuestData.QuestId.ToString()))
            };

       for (FName ObjectiveID : PossibleObjectiveIDs)
        {
      if (QuestSubsystem->UpdateObjectiveProgress(QuestData.QuestId, ObjectiveID, Count))
                {
            UE_LOG(LogQuestBridge, Log, TEXT("SUCCESS: Updated %s -> %s (+%d)"),
  *QuestData.QuestId.ToString(), *ObjectiveID.ToString(), Count);
         break; // Если удалось - выходим
      }
            }
     }
    }
    
    // Оригинальный код (если понадобится позже):
    // FQuestEventPayload QuestPayload;
    // QuestPayload.EventTag = GamePayload.EventTag;
    // QuestPayload.StringParam = GamePayload.StringParam;
    // QuestPayload.IntParam = GamePayload.IntParam;
    // QuestPayload.FloatParam = GamePayload.FloatParam;
    // QuestPayload.TagsParam = GamePayload.AdditionalTags;
  // QuestPayload.InstigatorActor = GamePayload.InstigatorActor;
    // QuestPayload.TargetActor = GamePayload.TargetActor;
    // QuestSubsystem->EmitQuestEvent(QuestPayload);
}
```

---

## ? После исправления проверь:

### 1. Запусти игру

### 2. Включи логи:
```
GameEventBus.EnableHistory 1
GameEventBus.SetVerbose 1
```

### 3. Собери яблоко

### 4. Проверь лог:
```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | Apple
LogQuestBridge: TEMP FIX: ItemAcquired - Apple x1
LogQuestBridge: SUCCESS: Updated CollectApples -> CollectApples_Objective (+1)
LogQuest: Objective updated: CollectApples (1/5)
```

---

## ?? Итог

**ПРОБЛЕМА:**  
`QuestSubsystem::EmitQuestEvent()` не реализован!

**РЕШЕНИЕ:**  
1. Добавить реализацию метода
2. Добавить логику обновления в QuestEventBus
3. ИЛИ использовать временный фикс в QuestEventBridge

**ПОСЛЕ ИСПРАВЛЕНИЯ:**
Квесты будут обновляться при сборе предметов! ?
