# ?? Как События Попадают в Воспоминания и Как Считается Время

> **Вопросы:** Как события попадают в воспоминания? Как считается время decay?  
> **Версия:** v1.3.1  
> **Дата:** 2025-01-23

---

## ?? Часть 1: Как События Попадают в Воспоминания

### **Способ 1: Через Dialogue Effects (автоматически)**

Это **основной способ** - воспоминания создаются автоматически через систему эффектов диалогов.

#### **A. В Dialogue Editor (визуально)**

```
Dialogue Node: "You helped me save my village!"
?? Conditions: (проверки перед показом ноды)
?  ?? ...
?
?? Effects: (что происходит при выборе этой реплики)
   ?? Effect 1: Add Memory
   ?  ?? Memory Tag: "Event.Betrayal"
   ?  ?? Memory Strength: 0.95
   ?
   ?? Effect 2: Modify Relationship
   ?  ?? Affinity: +20
   ?
   ?? Effect 3: Set Variable
      ?? "QuestCompleted" = true
```

**Что происходит за кулисами:**

```cpp
// DialogueEffectExecutor.cpp (упрощенно)
void ExecuteNodeEffects(const FDialogueNode& Node)
{
    for (UMemoryEffectBase* Effect : Node.MemoryEffects)
 {
        // Вызывается автоматически при выборе ноды!
        Effect->Execute(NPCActor, PlayerActor);
    }
}

// MemoryEffects.cpp
void UAddMemoryEffect::Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const
{
    UNPCMemoryComponent* MemoryComp = NPCActor->FindComponentByClass<UNPCMemoryComponent>();
    
    if (MemoryComp)
    {
        // Создается воспоминание с тегом!
        MemoryComp->AddMemoryByTag(MemoryTag, MemoryStrength);
    }
}
```

**Доступные Effects:**

| Effect | Назначение | Параметры |
|--------|-----------|-----------|
| **Add Memory** | Создать новое воспоминание | `MemoryTag`, `MemoryStrength (0-1)` |
| **Modify Memory Strength** | Изменить силу существующего | `MemoryTag`, `StrengthModifier`, `bAdditive` |
| **Remove Memory** | Удалить воспоминание | `MemoryTag` |
| **Refresh Memory** | "Освежить" воспоминание | `MemoryTag` |

---

#### **B. Пример: Квест с предательством**

```
Dialogue Tree: "The Betrayal"
????????????????????????????????????????????????????????????????????

Node 1: [Player] "I need to tell you something..."
?? Next ? Node 2

Node 2: [NPC] "What is it?"
?? Player Choices:
   ?? Choice A: "I told the guards about your secret."
   ?  ?? Effects:
   ?     ?? Add Memory
   ?     ?  ?? Tag: "Event.Betrayal"
   ?  ?  ?? Strength: 1.0
   ?     ?
   ?     ?? Create Memory (detailed)
   ?   ?  ?? Type: PlayerChoice
   ?     ?  ?? Description: "Player betrayed my trust"
?     ?  ?? Importance: 95
   ?     ?  ?? Emotion: Traumatic
   ?     ?  ?? Tags: ["Event.Betrayal", "Emotion.Traumatic"]
   ?   ?
   ?     ?? Modify Relationship
?        ?? Affinity: -40
   ?        ?? Trust: -60
   ?        ?? Fear: +30
   ?
   ?? Choice B: "Never mind, it's not important."
      ?? No effects
```

**Результат:**
- При выборе Choice A: Создается воспоминание АВТОМАТИЧЕСКИ
- NPC НИКОГДА не забудет (Importance = 95, Traumatic)
- Отношения разрушены

---

### **Способ 2: Программно (из C++ кода)**

```cpp
// Где угодно в игровой логике
void AMyNPC::OnPlayerHelped()
{
    UNPCMemoryComponent* MemComp = FindComponentByClass<UNPCMemoryComponent>();
    
 // Способ 2A: Простой (с тегом)
    MemComp->AddMemoryByTag(
        FGameplayTag::RequestGameplayTag(FName("Event.Help")),
        0.75f  // Strength
    );
    
    // Способ 2B: Детальный (полный контроль)
    FNPCMemoryEntry Memory;
  Memory.MemoryId = FName("PlayerHelped_Village_001");
    Memory.Type = EMemoryType::PlayerChoice;
    Memory.Description = FText::FromString("Player helped defend the village");
    Memory.Importance = 75.0f;
    Memory.Emotion = EMemoryEmotion::Joyful;
Memory.RelatedActor = PlayerActor;
    Memory.Timestamp = FDateTime::Now();  // ?? Реальное время!
    Memory.LastRecalledTime = FDateTime::Now();
    Memory.Freshness = 1.0f;  // 100% свежесть
    
    // Добавить теги
    Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Help")));
    Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Location.Village")));
    
    MemComp->AddMemory(Memory);
}
```

---

### **Способ 3: Через Blueprint**

```
Event Graph (NPC Blueprint):
????????????????????????????????????????????????????????????????????

Event: OnPlayerGaveItem
?? Cast to Item
?? Branch: Is Item == "Rose"?
?  ?? True:
?     ?? Memory Component ? Create Memory
??? Type: ItemGiven
?        ?? Description: "Player gave me a beautiful rose"
?        ?? Importance: 70.0
?        ?? Emotion: Joyful
?        ?? Related Actor: Player
?        ?? Context Tags:
?      ?? "Event.Gift"
?           ?? "Item.Flower"
```

---

### **Способ 4: Через Quest System (интеграция)**

```cpp
// QuestManager.cpp
void UQuestManager::StartQuest(FName QuestId)
{
    // ... existing code ...
    
    // Все NPC, связанные с квестом, автоматически создают воспоминание
    for (ANPC* QuestNPC : GetQuestRelatedNPCs(QuestId))
    {
        UNPCMemoryComponent* MemComp = QuestNPC->GetMemoryComponent();
        
        MemComp->CreateMemory(
    EMemoryType::Promise,
          FText::Format(FText::FromString("Agreed to help with {0}"), QuestId),
            90.0f,  // Обещания важны!
            EMemoryEmotion::Nostalgic,
       PlayerActor
        );
  }
}
```

---

### **Способ 5: Через Relationship System**

```cpp
// RelationshipComponent.cpp
void URelationshipComponent::ModifyAffinity(float Delta)
{
  Affinity = FMath::Clamp(Affinity + Delta, -100.0f, 100.0f);
    
    // Автоматически создать воспоминание при больших изменениях
    if (FMath::Abs(Delta) > 20.0f)
    {
        UNPCMemoryComponent* MemComp = GetOwner()->FindComponentByClass<UNPCMemoryComponent>();
        
   FNPCMemoryEntry Memory;
        Memory.Type = EMemoryType::PlayerChoice;
      
        if (Delta > 0)
        {
      Memory.Description = FText::FromString("Player did something that made me very happy");
        Memory.Emotion = EMemoryEmotion::Joyful;
      }
  else
     {
        Memory.Description = FText::FromString("Player hurt my feelings badly");
        Memory.Emotion = EMemoryEmotion::Negative;
     }
        
        Memory.Importance = FMath::Abs(Delta) * 2.0f;  // Пропорционально изменению
        Memory.RelatedActor = PlayerActor;
        
        MemComp->AddMemory(Memory);
    }
}
```

---

## ? Часть 2: Как Считается Время для Decay

### **ВАЖНО: Используется РЕАЛЬНОЕ ВРЕМЯ (Real-World Time)**

```cpp
// NPCMemoryComponent.cpp - создание воспоминания
FNPCMemoryEntry NewMemory;
NewMemory.Timestamp = FDateTime::Now();  // ?? РЕАЛЬНОЕ ВРЕМЯ ПК!
NewMemory.LastRecalledTime = FDateTime::Now();
NewMemory.Freshness = 1.0f;
```

**`FDateTime::Now()` - это:**
- Реальное системное время компьютера
- НЕ игровое время
- НЕ зависит от паузы игры
- НЕ зависит от Time Dilation

---

### **Как Decay Считает Прошедшее Время**

```cpp
// NPCMemoryComponent.cpp - расчет возраста воспоминания
float UNPCMemoryComponent::CalculateRelevance(const FNPCMemoryEntry& Memory, ...) const
{
    // ...
    
    // Вычисление возраста в РЕАЛЬНЫХ днях
    FTimespan Age = FDateTime::Now() - Memory.Timestamp;
    float AgeDays = Age.GetTotalDays();  // ?? Реальные дни!
    
    // Экспоненциальный decay по времени
    float AgeFactor = FMath::Exp(-AgeDays * 0.1f);
    Relevance *= AgeFactor;
    
    return Relevance;
}
```

**Пример расчета:**

```
Воспоминание создано: 23 января 2025, 10:00:00
Текущее время:        25 января 2025, 14:30:00

Age = 25.01.2025 14:30 - 23.01.2025 10:00
 = 2 days, 4 hours, 30 minutes
    = 2.1875 days (РЕАЛЬНЫХ!)

AgeFactor = exp(-2.1875 ? 0.1)
          = exp(-0.21875)
          = 0.803 (80.3%)

Relevance уменьшена на 19.7% из-за возраста
```

---

### **Decay Rate: "Per Day" = Реальные Дни**

```cpp
// NPCMemoryComponent.cpp - обновление freshness
void UNPCMemoryComponent::UpdateMemoryFreshness(FNPCMemoryEntry& Memory, float DeltaTime)
{
    float DecayRate = CalculateDecayRate(Memory);  // Per day!
    
    // Конвертация "per day" ? "per second"
    float DecayPerSecond = DecayRate / (24.0f * 60.0f * 60.0f);
    //        ?     ?     ?
    //        24   60    60
    //           hours mins  secs
    //   = 86,400 секунд в дне
    
    // Обновление freshness
  Memory.Freshness = FMath::Max(Memory.Freshness - DecayPerSecond * DeltaTime, 0.0f);
}
```

**Пример расчета:**

```
BaseDecayRate = 0.05 (5% per REAL day)

Для воспоминания с Importance = 50, Emotion = Neutral:
????????????????????????????????????????????????????????????????????
DecayRate = 0.05 (базовая)
          ? 1.25 (фактор важности для 50)
          ? 1.0 (нет эмоционального бонуса)
          = 0.0625 per day

DecayPerSecond = 0.0625 / 86400 = 0.0000007233 per second

За 1 РЕАЛЬНУЮ минуту (60 секунд):
Freshness -= 0.0000007233 ? 60 = 0.0000434
           = -0.00434% per minute

За 1 РЕАЛЬНЫЙ час (3600 секунд):
Freshness -= 0.0000007233 ? 3600 = 0.0026
           = -0.26% per hour

За 1 РЕАЛЬНЫЙ день (86400 секунд):
Freshness -= 0.0000007233 ? 86400 = 0.0625
 = -6.25% per day ?
```

---

### **Timeline: Реальное Время vs. Игровое Время**

```
РЕАЛЬНОЕ ВРЕМЯ (что используется):
????????????????????????????????????????????????????????????????????

23.01.2025 10:00: Воспоминание создано (Freshness = 100%)
23.01.2025 10:01: Decay update (Freshness = 99.99%)
24.01.2025 10:00: Прошел 1 РЕАЛЬНЫЙ день (Freshness = 93.75%)
25.01.2025 10:00: Прошло 2 РЕАЛЬНЫХ дня (Freshness = 87.89%)
...
30.01.2025 10:00: Прошло 7 РЕАЛЬНЫХ дней (Freshness = 60.58%)


ИГРОВОЕ ВРЕМЯ (НЕ используется):
????????????????????????????????????????????????????????????????????

Game Time Day 1:Игрок играл 2 часа
Game Time Day 5:  Игрок не играл 3 дня (pause)
Game Time Day 10: Игрок вернулся

? Система НЕ учитывает игровое время!
? Система учитывает РЕАЛЬНОЕ время!

Результат:
- За 5 реальных дней воспоминание decay даже если игра на паузе
- Freshness уменьшается даже когда игрок не играет
```

---

### **Почему Реальное Время?**

#### **Плюсы:**

? **Простота:**
- Не нужно синхронизировать с игровым временем
- Не нужно обрабатывать паузы
- Не нужно save/load временных данных

? **Реализм для некоторых сценариев:**
- Игрок вернулся после перерыва ? NPC "забыл" неважные события
- Simulate "real life" забывание

#### **Минусы:**

? **Проблемы:**
- Игрок не играл неделю ? все воспоминания забыты
- Не учитывает игровую паузу
- Странно для RPG (игрок спит 8 часов ? воспоминания не меняются, но реально прошел 1 час ? decay!)

---

### **Альтернативный Подход: Игровое Время (не реализовано)**

```cpp
// ? ТЕКУЩАЯ РЕАЛИЗАЦИЯ (реальное время):
FDateTime::Now()

// ? ВОЗМОЖНАЯ АЛЬТЕРНАТИВА (игровое время):
UGameplayStatics::GetWorldDeltaSeconds(WorldContext) // Только активное игровое время
UGameplayStatics::GetTimeSeconds(WorldContext)       // Игровой timestamp

// Или custom игровая система времени:
UMyGameTimeManager::GetGameDateTime()  // Игровой day/night cycle
```

**Если бы использовали игровое время:**

```
Игровой цикл: 1 игровой день = 20 минут реального времени

Воспоминание создано: Game Day 1, Morning
Игрок играет 1 час реального времени = 3 игровых дня
Freshness decay за 3 игровых дня (не за 1 час реального)

Игрок на паузе 1 неделю реального времени
Freshness НЕ меняется (игровое время остановлено)
```

---

## ?? Практические Примеры

### **Пример 1: Создание Воспоминания в Диалоге**

```
Dialogue Node: "Thank you for saving me!"
????????????????????????????????????????????????????????????????????

Effects:
1. Add Memory
   ?? Memory Tag: "Event.Rescue"
   ?? Strength: 0.95

2. Create Memory (detailed)
   ?? Type: PlayerChoice
   ?? Description: "Player risked their life to save mine"
   ?? Importance: 95.0
   ?? Emotion: Joyful
   ?? Tags: ["Event.Rescue", "Emotion.Grateful"]

3. Modify Relationship
   ?? Affinity: +50
   ?? Trust: +40

Результат:
- Воспоминание создано с Timestamp = FDateTime::Now()
- Freshness = 100%
- Начнет decay через 60 секунд (реальных)
```

---

### **Пример 2: Decay Timeline (Реальное Время)**

```
Воспоминание: "Player saved me" (Importance = 95, Traumatic)
????????????????????????????????????????????????????????????????????

Day 0 (23.01.2025):
- Freshness: 100%
- DecayRate: 0.05 ? 0.55 (importance) ? 0.5 (emotional) = 0.01375 per day

Day 1 (24.01.2025):
- Freshness: 100% - 1.375% = 98.625%

Day 7 (30.01.2025):
- Freshness: 100% - 9.625% = 90.375%

Day 30 (22.02.2025):
- Freshness: 100% - 41.25% = 58.75%

Day 90 (23.04.2025):
- Freshness: 100% - 123.75% = 0% (capped)
- НО Importance = 95 > MinImportanceThreshold = 10
- ? НЕ УДАЛЯЕТСЯ (защищено важностью)

Day 365 (23.01.2026):
- Freshness: 0% (уже давно)
- Importance: 95
- ? ВСЕ ЕЩЕ ПОМНИТ (никогда не забудет!)
```

---

### **Пример 3: Игрок Вернулся После Перерыва**

```
Ситуация:
????????????????????????????????????????????????????????????????????
Игрок играл 20.01.2025
Создал 10 воспоминаний (Importance 10-60)
Потом не играл 2 недели
Вернулся 03.02.2025

Что произошло:
????????????????????????????????????????????????????????????????????
Прошло 14 РЕАЛЬНЫХ дней

Неважные воспоминания (Importance < 30):
- DecayRate ? 0.1 per day
- После 14 дней: Freshness ? 0%
- Importance < MinImportanceThreshold
- ? УДАЛЕНЫ (забыты)

Средние воспоминания (Importance 40-60):
- DecayRate ? 0.05 per day
- После 14 дней: Freshness ? 30%
- ? ЕЩЕ ПОМНИТ (но слабо)

Важные воспоминания (Importance 80+):
- DecayRate ? 0.02 per day
- После 14 дней: Freshness ? 70%
- ? ОТЛИЧНО ПОМНИТ

NPC при встрече:
"Давно не виделись! Помню, ты помогал мне тогда... (вспоминает важное)"
```

---

## ?? Итоговая Сводка

### **Создание Воспоминаний:**

| Способ | Когда использовать | Сложность |
|--------|-------------------|-----------|
| **Dialogue Effects** | Основной способ (автоматически в диалогах) | ? Легко |
| **C++ Программно** | Для сложной логики, quest events | ?? Средне |
| **Blueprint** | Для дизайнеров, простые события | ? Легко |
| **Quest/Relationship Integration** | Автоматическая интеграция | ??? Сложно (требует рефакторинга) |

### **Время для Decay:**

| Параметр | Значение | Примечание |
|----------|----------|------------|
| **Timestamp** | `FDateTime::Now()` | РЕАЛЬНОЕ время создания |
| **Decay Rate** | Per REAL day | Например, 0.05 = 5% per REAL day |
| **Update Interval** | 60 REAL seconds | Проверка decay каждую минуту |
| **Age Calculation** | `(Now - Timestamp).GetTotalDays()` | В РЕАЛЬНЫХ днях |

### **Формула Decay:**

```
DecayPerSecond = DecayRate / 86400  // 86400 = секунд в дне

Freshness -= DecayPerSecond ? DeltaTime

Где:
- DecayRate = BaseDecayRate ? ImportanceFactor ? EmotionFactor ? ...
- DeltaTime = Реальное время с последнего update (обычно 60 секунд)
```

---

## ?? Рекомендации

### **Для текущей реализации (v1.3.1):**

1. ? **Используйте Dialogue Effects** - основной способ создания воспоминаний
2. ? **Учитывайте реальное время** - decay происходит даже когда игра на паузе
3. ? **Важность защищает** - воспоминания с Importance > 80 почти не забываются
4. ? **Emotion усиливает** - Traumatic/Joyful decay в 2-4 раза медленнее

### **Для будущих версий (v2.0):**

1. ?? **Опция игрового времени** - позволить выбирать real-time vs game-time
2. ?? **Pause-aware decay** - не decay когда игра на паузе
3. ?? **Hybrid approach** - комбинировать реальное и игровое время

---

**Документ подготовлен:** 2025-01-23  
**Версия:** 1.0  
**Статус:** ? Complete

**Вывод:** Воспоминания создаются через Effects в диалогах или программно, а время считается на основе **реального системного времени (FDateTime::Now)**, что означает decay происходит даже когда игра не запущена! ??
