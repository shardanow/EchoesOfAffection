# ?? Memory Decay: Зачем это нужно и как работает

> **Вопросы:** Зачем нужен Memory Decay? Все ли 100 NPC обрабатываются одновременно?  
> **Версия:** v1.3.1  
> **Дата:** 2025-01-23

---

## ?? Краткие Ответы

### **Q1: Зачем нужен Memory Decay?**

> **Memory Decay нужен для создания реалистичных, живых NPC с динамической памятью**, которые забывают неважные события и помнят важные, создавая ощущение настоящих отношений и эмоциональной глубины.

### **Q2: Все ли 100 NPC обрабатываются одновременно?**

> **НЕТ!** Каждый NPC обрабатывается **в свое время** (через 60 секунд после своего spawn). Но при 100 NPC получается **~1.67 NPC в секунду** (100 / 60s), что дает постоянные микрофризы.

---

## ?? Часть 1: Зачем нужен Memory Decay?

### **1.1. Геймплейные причины**

#### **A. Реализм и иммерсия**

```
Сценарий БЕЗ Memory Decay:
????????????????????????????????????????????????????????????
Day 1:   Игрок: "Привет!"
         NPC:   "Привет! Как дела?"

Day 30:  Игрок: "Привет!"
         NPC:   "Помню, 30 дней назад ты сказал 'Привет!' 
       (freshness = 1.0, importance = 10)"
    
? ПРОБЛЕМА: NPC помнит АБСОЛЮТНО ВСЕ с идеальной точностью!
```

```
Сценарий С Memory Decay:
????????????????????????????????????????????????????????????
Day 1:   Игрок: "Привет!"
         NPC:   "Привет! Как дела?"
         Memory: freshness = 1.0, importance = 10

Day 30:  Memory: freshness = 0.2, importance = 10
       NPC:   "Кажется, мы встречались раньше..."

Day 60:  Memory: freshness = 0.0 (FORGOTTEN!)
         NPC:   "Извините, вы кто?"
         
? РЕАЛИЗМ: NPC забывает неважные разговоры!
```

---

#### **B. Важные события остаются навсегда**

```cpp
// Пример из кода:
void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
    for (int32 i = Memories.Num() - 1; i >= 0; --i)
    {
 FNPCMemoryEntry& Memory = Memories[i];
    UpdateMemoryFreshness(Memory, DecayUpdateInterval);
        
        // ?? Важное условие!
        if (Memory.Freshness <= 0.0f && Memory.Importance < MinImportanceThreshold)
        {
    // Удаляем ТОЛЬКО неважные воспоминания
       Memories.RemoveAt(i);
   }
    }
}
```

**Что это значит:**

| Событие | Importance | Freshness после 30 дней | Результат |
|---------|------------|------------------------|-----------|
| Случайный разговор | 10 | 0.2 ? забыто | ? Забыто |
| Подарил цветы | 70 | 0.5 | ? Помнит слабо |
| Спас жизнь NPC | 95 | 0.8 | ? **Помнит хорошо!** |
| Предательство | 100 (Traumatic) | 0.9 | ? **НИКОГДА не забудет!** |

---

#### **C. Эмоциональная глубина**

```cpp
// Код показывает, что эмоции влияют на decay:
float UNPCMemoryComponent::CalculateDecayRate(const FNPCMemoryEntry& Memory) const
{
    float DecayRate = BaseDecayRate;
    
    // Эмоциональные воспоминания decay медленнее
    if (Memory.Emotion != EMemoryEmotion::Neutral)
    {
        DecayRate *= (1.0f - EmotionalMemoryBonus); // Обычно 0.5
    }
    
    // Негативные воспоминания decay ЕЩЕ медленнее
    if (Memory.Emotion == EMemoryEmotion::Traumatic)
    {
        DecayRate /= NegativeBias; // Обычно 1.0-2.0
    }
    
    return DecayRate;
}
```

**Геймплейный эффект:**

```
Пример 1: Нейтральный разговор
?????????????????????????????????????????????????
Importance: 50
Emotion: Neutral
Decay: 0.05 per day ? 1.0 (no bonus) = FAST decay

Day 0:  Freshness = 1.0 "Он сказал, что погода хорошая"
Day 30: Freshness = 0.1 "Кажется, он что-то говорил..."
Day 40: FORGOTTEN  "..."
```

```
Пример 2: Травматическое событие
?????????????????????????????????????????????????
Importance: 95
Emotion: Traumatic
Decay: 0.05 / (0.5 ? 2.0) = 0.05 per day (очень медленно!)

Day 0:   Freshness = 1.0 "ОН ПРЕДАЛ МЕНЯ!"
Day 30:  Freshness = 0.85 "Я не могу забыть..."
Day 60:  Freshness = 0.7  "Я все еще помню..."
Day 365: Freshness = 0.2  "Шрам на душе остался..."

НИКОГДА НЕ ЗАБУДЕТСЯ ПОЛНОСТЬЮ (importance = 95 > MinImportanceThreshold = 10)
```

---

### **1.2. Технические причины**

#### **A. Управление памятью**

```
БЕЗ Memory Decay:
?????????????????????????????????????????????????
100 NPC ? 100 memories = 10,000 memories
После 10 часов игры: 100 NPC ? 500 memories = 50,000 memories!
Memory: 50,000 ? 500 bytes = 25MB

С Memory Decay:
?????????????????????????????????????????????????
100 NPC ? 20-50 memories = 2,000-5,000 memories
После 10 часов: Все еще 100 NPC ? 50 memories = 5,000 memories
Memory: 5,000 ? 500 bytes = 2.5MB

ЭКОНОМИЯ: 90%!
```

---

#### **B. Производительность save/load**

```cpp
// Без decay: сохраняем ВСЕ воспоминания
void SaveGame()
{
    for (NPC in AllNPCs) // 100 NPC
    {
        SaveMemories(NPC->Memories); // 500 memories per NPC
    }
}
// Total: 50,000 memories ? 500 bytes = 25MB save file!
// Save time: ~500ms

// С decay: сохраняем только важные
void SaveGame()
{
    for (NPC in AllNPCs) // 100 NPC
    {
  SaveMemories(NPC->Memories); // 50 memories per NPC
    }
}
// Total: 5,000 memories ? 500 bytes = 2.5MB save file
// Save time: ~50ms
```

---

#### **C. Relevance Queries становятся быстрее**

```cpp
// FindRelevantMemories перебирает ВСЕ воспоминания
TArray<FNPCMemoryEntry> UNPCMemoryComponent::FindRelevantMemories(
    const FGameplayTagContainer& ContextTags,
    AActor* RelatedActor,
    int32 MaxResults) const
{
    for (const FNPCMemoryEntry& Memory : Memories) // ?? O(N)
    {
    float Relevance = CalculateRelevance(Memory, ContextTags, RelatedActor);
        // ...
    }
}
```

**Performance:**

| Memories per NPC | Query time | With 100 NPC queries |
|------------------|------------|----------------------|
| 500 (no decay) | 10ms | 1000ms (1 second!) |
| 50 (with decay) | 1ms | 100ms ? |

---

### **1.3. Дизайнерские причины**

#### **A. Создание динамичных отношений**

```
Сценарий: Игрок флиртует с NPC 30 дней назад
?????????????????????????????????????????????????

БЕЗ Decay:
Day 30: NPC: "Помню, ты 30 дней назад подмигивал мне!" (freshness = 1.0)
      Игрок: "Эм... я уже женат..."
        ? AWKWARD

С Decay:
Day 30: NPC: "Кажется, ты когда-то флиртовал... или нет?" (freshness = 0.3)
        Игрок: "Да, давно было..."
        ? NATURAL

Day 60: NPC: "..." (FORGOTTEN, freshness = 0.0)
        ? Можно начать новые отношения!
```

---

#### **B. Incentive для постоянных взаимодействий**

```
Геймплейный луп:
?????????????????????????????????????????????????

1. Игрок помогает NPC (importance = 70, freshness = 1.0)
   NPC: "Спасибо! Я помню твою доброту!"

2. Игрок не возвращается 30 дней
   Memory: freshness = 0.4
   NPC: "Кажется, ты когда-то помогал..."

3. Игрок возвращается и разговаривает
   RecallMemory(MemoryId) ? freshness = 0.5 (+0.1 bonus!)
 NPC: "Точно! Ты спас меня!"

4. Игрок часто общается
 RecallCount++, decay slower
   NPC НИКОГДА не забудет!

? REWARD: Постоянное общение укрепляет память!
```

---

## ?? Часть 2: Все ли 100 NPC обрабатываются одновременно?

### **2.1. НЕТ, они распределены во времени**

#### **Код показывает:**

```cpp
// NPCMemoryComponent.cpp
UNPCMemoryComponent::UNPCMemoryComponent()
{
    PrimaryComponentTick.TickInterval = 1.0f; // Тикается каждую секунду
}

void UNPCMemoryComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // ?? TimeSinceLastDecayUpdate инициализируется в 0.0f!
    // Значит decay произойдет через 60 секунд после BeginPlay()
}

void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
    TimeSinceLastDecayUpdate += DeltaTime; // Накапливается с BeginPlay
    
    if (TimeSinceLastDecayUpdate < 60.0f) // Проверка интервала
    {
        return; // Быстрый выход
    }
    
    // ?? Попадаем сюда ТОЛЬКО раз в минуту (с момента BeginPlay)
    TimeSinceLastDecayUpdate = 0.0f;
    
    // Обработка всех 100 воспоминаний
    for (int32 i = Memories.Num() - 1; i >= 0; --i)
    {
        UpdateMemoryFreshness(Memories[i], 60.0f); // 3ms
    }
}
```

---

#### **Что происходит в реальности:**

```
Timeline для 100 NPC (spawned в разное время):
????????????????????????????????????????????????????????????????????

NPC #1 spawned в 00:00:00 ? BeginPlay() ? decay в 00:01:00
NPC #2 spawned в 00:00:01 ? BeginPlay() ? decay в 00:01:01
NPC #3 spawned в 00:00:02 ? BeginPlay() ? decay в 00:01:02
...
NPC #100 spawned в 00:01:39 ? BeginPlay() ? decay в 00:02:39

Результат: Decay events распределены по 100 секундам!
```

---

### **2.2. НО проблема все равно есть!**

#### **Распределение не идеальное:**

```
Сценарий 1: NPC spawned равномерно (best case)
????????????????????????????????????????????????????????????????????
100 NPC spawned over 60 seconds = 1.67 NPC per second

Decay pattern:
Time:  60s   61s   62s   63s   64s   65s   66s   67s   68s
       ??    ??    ??    ??    ??    ??    ??    ??    ??
       
Каждую секунду: 1-2 NPC ? 3ms = 3-6ms spike
При 60 FPS: 3-6ms = незаметно ?


Сценарий 2: NPC spawned пачками (worst case)
????????????????????????????????????????????????????????????????????
50 NPC spawned в 00:00:00-00:00:05 (уровень загрузился)
50 NPC spawned в 00:00:30-00:00:35 (enemy wave)

Decay pattern:
Time:  60s   61s   62s   63s   64s   65s   90s   91s   92s
   ????  ????  ????  ????  ????  ????  ????  ????  ????
       
В 60-65s: 10 NPC ? 3ms = 30ms PER SECOND
В 90-95s: 10 NPC ? 3ms = 30ms PER SECOND

При 60 FPS: 30ms = 2 frames (заметно!) ??
```

---

### **2.3. Математика распределения**

```
Если 100 NPC spawned равномерно за X секунд:
????????????????????????????????????????????????????????????????????

X = 60s  ? Decay каждые 0.6s (1.67 NPC/s) ? 5ms spike каждые 0.6s  ?
X = 30s  ? Decay каждые 0.3s (3.3 NPC/s)  ? 10ms spike каждые 0.3s ??
X = 10s  ? Decay каждые 0.1s (10 NPC/s)   ? 30ms spike каждые 0.1s ??
X = 1s   ? Decay каждые 0.01s (100 NPC/s) ? 300ms spike!   ??

ВЫВОД: Чем быстрее spawn, тем хуже распределение!
```

---

## ?? Решения

### **Немедленное (без кода):**

```cpp
// 1. Отключить decay для фоновых NPC
MemoryComponent->bEnableMemoryDecay = false;

// 2. Уменьшить количество воспоминаний
MemoryComponent->MaxMemories = 20; // Вместо 100
```

### **Краткосрочное (v1.4 - 1 час работы):**

```cpp
// Добавить в BeginPlay() рандомный offset
void UNPCMemoryComponent::BeginPlay()
{
    Super::BeginPlay();
  
    // ? РЕШЕНИЕ: Рандомизация начального времени
TimeSinceLastDecayUpdate = FMath::FRandRange(0.0f, DecayUpdateInterval);
    
    // Результат: Decay events равномерно распределены!
}
```

**Эффект:**
```
100 NPC с рандомным offset:
????????????????????????????????????????????????????????????????????
Decay pattern:
Time:  0s10s   20s   30s   40s   50s   60s   70s   80s
       ????  ????  ????  ????  ????  ????  ????  ????  ????

Каждые ~0.6s: 1-2 NPC ? 3ms = 3-6ms
100 NPC ? 3ms = 300ms spread over 60s = 5ms per frame average

ПРИ 60 FPS: 5ms = НЕЗАМЕТНО! ?
```

---

## ?? Итоговые Ответы

### **Q1: Зачем нужен Memory Decay?**

**Краткий ответ:**
> Для создания **реалистичных NPC**, которые **забывают неважное** и **помнят важное**, создавая **динамичные отношения** и **эмоциональную глубину**.

**Полный список причин:**

1. ? **Геймплей:** Реализм, динамичные отношения, incentive для общения
2. ? **Техника:** Управление памятью (90% savings), быстрее save/load
3. ? **Дизайн:** Естественное развитие сюжета, forgiveness механик
4. ? **Performance:** Быстрее queries, меньше памяти

---

### **Q2: Все ли 100 NPC обрабатываются одновременно?**

**Краткий ответ:**
> **НЕТ**, каждый NPC обрабатывается **через 60 секунд после своего spawn**. Но если много NPC spawned одновременно (уровень загрузился), их decay events **группируются**, что дает **периодические фризы**.

**Решение:**
```cpp
// v1.4 - добавить рандомный offset (1 час работы)
TimeSinceLastDecayUpdate = FMath::FRandRange(0.0f, 60.0f);
```

---

## ?? Дополнительные материалы

- **Performance Analysis:** [`Performance_Freeze_Analysis.md`](Performance_Freeze_Analysis.md)
- **Architecture Overview:** [`Complete_Architecture_Overview.md`](Complete_Architecture_Overview.md)
- **Scalability Roadmap:** [`Scalability_And_Future_Enhancements.md`](Scalability_And_Future_Enhancements.md)

---

**Документ подготовлен:** 2025-01-23  
**Версия:** 1.0  
**Статус:** ? Complete

**Вывод:** Memory Decay - это **критически важная feature** для глубоких relationship mechanics, и проблема с фризами **имеет простое решение** (рандомный offset).
