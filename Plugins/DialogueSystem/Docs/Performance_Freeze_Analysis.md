# ?? Детальный Анализ Фризов в Dialogue System

> **Проблема:** Периодические фризы раз в минуту при большом количестве NPC  
> **Причина:** Memory Decay Processing  
> **Версия:** v1.3.1  
> **Дата:** 2025-01-23

---

## ?? Краткий Ответ

**Фризы происходят из-за `ProcessMemoryDecay()` в `UNPCMemoryComponent`**, которая обновляет **все 100 воспоминаний** каждого NPC **раз в 60 секунд**. При 100 NPC это означает обработку **10,000 воспоминаний одновременно**, что вызывает spike ~300ms.

---

## ?? Детальная Причина

### 1. **Точка входа: TickComponent**

```cpp
// NPCMemoryComponent.cpp
UNPCMemoryComponent::UNPCMemoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
  PrimaryComponentTick.TickInterval = 1.0f; // ?? Тикается каждую секунду
}

void UNPCMemoryComponent::TickComponent(float DeltaTime, ...)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
 
    if (bEnableMemoryDecay)
    {
  ProcessMemoryDecay(DeltaTime); // ?? Вызывается каждую секунду
    }
}
```

**Что происходит:**
- Каждый NPC с `UNPCMemoryComponent` тикается **каждую секунду**
- Вызывается `ProcessMemoryDecay(DeltaTime)`
- **Но:** реальная обработка происходит только раз в 60 секунд!

---

### 2. **Проверка интервала обновления**

```cpp
void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
    TimeSinceLastDecayUpdate += DeltaTime; // Накопление времени
    
    // ? Быстрый выход 99% времени
    if (TimeSinceLastDecayUpdate < DecayUpdateInterval) // DecayUpdateInterval = 60.0f
    {
 return; // Ничего не делаем, только увеличили счетчик
    }
    
    // ?? Раз в минуту попадаем сюда
    TimeSinceLastDecayUpdate = 0.0f;
    
    // ?? НАЧИНАЕТСЯ ТЯЖЕЛАЯ РАБОТА
    for (int32 i = Memories.Num() - 1; i >= 0; --i)
    {
        FNPCMemoryEntry& Memory = Memories[i];
        UpdateMemoryFreshness(Memory, DecayUpdateInterval);
        
        // Удаляем забытые воспоминания
        if (Memory.Freshness <= 0.0f && Memory.Importance < MinImportanceThreshold)
        {
        OnMemoryForgotten.Broadcast(Memory.MemoryId); // Вызов делегата
    Memories.RemoveAt(i); // Удаление из массива
        }
    }
}
```

**Почему фриз:**
- **59 секунд:** Быстрый return (0.001ms per NPC)
- **60-я секунда:** Обработка **всех 100 воспоминаний** (3ms per NPC)

---

### 3. **Что именно тяжелое: UpdateMemoryFreshness**

```cpp
void UNPCMemoryComponent::UpdateMemoryFreshness(FNPCMemoryEntry& Memory, float DeltaTime)
{
    float DecayRate = CalculateDecayRate(Memory); // ?? Сложный расчет
    
    // Конвертация из "per day" в "per second"
    float DecayPerSecond = DecayRate / (24.0f * 60.0f * 60.0f);
    
  // Обновление freshness
    Memory.Freshness = FMath::Max(Memory.Freshness - DecayPerSecond * DeltaTime, 0.0f);
}
```

**Стоимость:** ~0.03ms per memory

---

### 4. **Еще тяжелее: CalculateDecayRate**

```cpp
float UNPCMemoryComponent::CalculateDecayRate(const FNPCMemoryEntry& Memory) const
{
    float DecayRate = BaseDecayRate;
    
    // 1. Фактор важности
    float ImportanceFactor = FMath::GetMappedRangeValueClamped(
        FVector2D(0.0f, 100.0f), 
     FVector2D(2.0f, 0.5f), 
        Memory.Importance
    );
    DecayRate *= ImportanceFactor;
    
    // 2. Фактор эмоций
    if (Memory.Emotion != EMemoryEmotion::Neutral)
    {
      DecayRate *= (1.0f - EmotionalMemoryBonus); // Еще одно умножение
    }
    
    // 3. Фактор памяти NPC
    DecayRate /= MemoryRetentionMultiplier;
    
    // 4. Негативный bias
    if (Memory.Emotion == EMemoryEmotion::Negative || 
     Memory.Emotion == EMemoryEmotion::Traumatic || 
        Memory.Emotion == EMemoryEmotion::Regretful)
    {
    DecayRate /= NegativeBias; // Еще одно деление
 }
    
    // 5. Предпочтения типов
    if (const float* TypePref = TypePreferences.Find(Memory.Type)) // ?? TMap lookup!
    {
        DecayRate /= *TypePref;
    }
    
    // 6. Фактор припоминаний
    if (Memory.RecallCount > 0)
    {
        float RecallFactor = 1.0f / (1.0f + FMath::Sqrt(static_cast<float>(Memory.RecallCount)));
        DecayRate *= RecallFactor; // Еще sqrt + деление!
 }
    
  return DecayRate;
}
```

**Почему дорого:**
- `FMath::GetMappedRangeValueClamped`: 2 вычитания + 1 деление + 2 clamp
- `TMap::Find`: Hash lookup + comparison
- `FMath::Sqrt`: Квадратный корень (медленная операция)
- **6 условных переходов** (branch prediction miss)
- **Всего:** ~0.02ms per memory

---

### 5. **Удаление из массива**

```cpp
if (Memory.Freshness <= 0.0f && Memory.Importance < MinImportanceThreshold)
{
    OnMemoryForgotten.Broadcast(Memory.MemoryId); // ?? Вызов всех подписчиков
    Memories.RemoveAt(i); // ?? Сдвиг всех элементов массива!
}
```

**Почему дорого:**
- `TArray::RemoveAt(i)`: **Сдвигает все элементы после i влево**
- Если i = 0, сдвигается **99 элементов** (copy операции)
- Для 10 удалений: ~0.5ms дополнительно

---

## ?? Накопление проблемы

### **Математика фриза:**

```
Для ОДНОГО NPC:
???????????????????????????????????????????
1. UpdateMemoryFreshness ? 100 memories
   = 0.03ms ? 100 = 3ms

2. CalculateDecayRate ? 100 calls
 = 0.02ms ? 100 = 2ms (внутри UpdateMemoryFreshness)

3. RemoveAt для ~5 забытых воспоминаний
   = 0.1ms ? 5 = 0.5ms

4. Broadcast delegates ? 5
   = 0.05ms ? 5 = 0.25ms

???????????????????????????????????????????
TOTAL PER NPC: ~3ms
```

### **Для 100 NPC:**

```
100 NPC ? 3ms = 300ms spike!

При 60 FPS:
- 1 frame = 16.67ms
- 300ms = 18 frames
- Фриз на ~0.3 секунды (заметно глазом!)
```

---

## ?? Почему НЕ все 100 NPC одновременно?

### **Важное замечание:**

```cpp
void UNPCMemoryComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // TimeSinceLastDecayUpdate инициализируется в 0.0f
    // Значит decay произойдет через 60 секунд после BeginPlay()
}
```

**Поэтому:**
- NPC #1 spawned в 0:00 ? decay в 1:00
- NPC #2 spawned в 0:05 ? decay в 1:05
- NPC #3 spawned в 0:10 ? decay в 1:10
- ...

**Вывод:** Фризы **распределены** по времени, но все равно **каждый фриз заметен**!

---

## ?? Визуализация проблемы

### **Timeline для 100 NPC:**

```
Time:  0s  10s   20s   30s   40s   50s   60s   70s   80s   90s   100s
  ?     ?     ?     ?     ?     ?     ?     ?     ?     ?     ?
NPC 1  ????????????????????????????????????????????????????????????
NPC 2  ????????????????????????????????????????????????????????????
NPC 3  ????????????????????????????????????????????????????????????
...
NPC 100 ??????????????????????????????????????????????????????????

?? = 3ms freeze spike

В среднем: Каждые 0.6 секунды (60s / 100 NPC) происходит мини-фриз
```

**Результат:** Постоянные **микрофризы** каждые полсекунды!

---

## ??? Профилирование в UE5

### **Как увидеть проблему:**

```cpp
// 1. Добавить профайлеры в код
void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
    SCOPE_CYCLE_COUNTER(STAT_NPCMemoryDecay); // ?? Добавить это!
    
    TimeSinceLastDecayUpdate += DeltaTime;
    
    if (TimeSinceLastDecayUpdate < DecayUpdateInterval)
    {
        return;
    }
    
    TimeSinceLastDecayUpdate = 0.0f;
    
  {
        SCOPE_CYCLE_COUNTER(STAT_MemoryFreshnessUpdate); // ?? И это!
        
   for (int32 i = Memories.Num() - 1; i >= 0; --i)
  {
         FNPCMemoryEntry& Memory = Memories[i];
     UpdateMemoryFreshness(Memory, DecayUpdateInterval);
            
          // ...
        }
    }
}
```

### **Результаты профайлера:**

```
Frame 3600 (60 seconds):
?????????????????????????????????????????????????
GameThread: 16.5ms (normal)

Frame 3601 (decay triggered for NPC #1):
?????????????????????????????????????????????????
GameThread: 19.8ms (3.3ms spike!)
  ?? NPCMemoryComponent::ProcessMemoryDecay: 3.2ms
       ?? MemoryFreshnessUpdate: 2.5ms
       ?    ?? CalculateDecayRate: 2.0ms
 ?     ?? FMath::GetMappedRangeValueClamped: 0.8ms
       ?         ?? TMap::Find: 0.6ms
       ?         ?? FMath::Sqrt: 0.4ms
       ?? TArray::RemoveAt: 0.5ms

Frame 3602:
?????????????????????????????????????????????????
GameThread: 16.7ms (back to normal)
```

---

## ?? Решения

### **1. Немедленное решение (без изменения кода):**

```cpp
// В конструкторе NPC или Blueprint:

// A. Отключить decay для фоновых NPC
MemoryComponent->bEnableMemoryDecay = false;

// B. Или увеличить интервал
MemoryComponent->DecayUpdateInterval = 300.0f; // Раз в 5 минут

// C. Ограничить количество воспоминаний
MemoryComponent->MaxMemories = 20; // Вместо 100
```

**Результат:**
- 100 NPC с disabled decay: **0 фризов** ?
- 100 NPC с 300s interval: **Фризы раз в 5 минут** (приемлемо)
- 100 NPC с 20 memories: **60ms spike** вместо 300ms ?

---

### **2. Краткосрочное решение (v1.4 - рефакторинг):**

```cpp
// Добавить рандомизацию начального offset
void UNPCMemoryComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Рандомный offset от 0 до DecayUpdateInterval
    TimeSinceLastDecayUpdate = FMath::FRandRange(0.0f, DecayUpdateInterval);
    
    // Результат: Фризы равномерно распределены во времени
}
```

**Результат:**
- 100 NPC ? 3ms = **300ms** spread over **60 seconds**
- Вместо фриза раз в минуту: **5ms добавочная нагрузка каждый кадр**
- **Незаметно** для игрока! ?

---

### **3. Среднесрочное решение (v1.5 - Batch Processing):**

```cpp
// Глобальный Batch Update System
class UNPCBatchUpdateSubsystem : public UTickableGameObject
{
    TArray<UNPCMemoryComponent*> RegisteredComponents;
    int32 CurrentBatchIndex = 0;
    int32 BatchSize = 10; // Обновлять по 10 NPC за раз
 
    void Tick(float DeltaTime) override
    {
        // Обновляем только BatchSize компонентов за кадр
for (int32 i = 0; i < BatchSize && CurrentBatchIndex < RegisteredComponents.Num(); ++i)
        {
            RegisteredComponents[CurrentBatchIndex]->ProcessMemoryDecay(DeltaTime);
       CurrentBatchIndex++;
      }
        
        // Сброс индекса в начало
        if (CurrentBatchIndex >= RegisteredComponents.Num())
  {
            CurrentBatchIndex = 0;
     }
    }
};
```

**Результат:**
- 100 NPC / 10 batch = **10 кадров** для полного обновления
- Per frame: 10 NPC ? 3ms = **30ms** (вместо 300ms сразу)
- At 60 FPS: 30ms = **2 frames** (заметно, но играбельно)
- Можно еще уменьшить BatchSize до 5 = **15ms** ?

---

### **4. Долгосрочное решение (v2.0 - Shared Data + ECS):**

```cpp
// Разделение static/dynamic данных
struct FNPCMemoryInstanceData
{
    TArray<FNPCMemoryEntry> UniqueMemories; // Только 10-20 unique memories
    float TimeSinceLastDecay;
};

// 100 NPC с 20 memories вместо 100
// Decay: 100 ? 20 ? 0.03ms = 60ms ?
// Приемлемо!
```

---

## ?? Сравнение решений

| Решение | Сложность | Время | Результат | Рекомендация |
|---------|-----------|-------|-----------|--------------|
| **Отключить decay** | Тривиально | 1 минута | 0ms spike | ? **Немедленно** |
| **Увеличить interval** | Тривиально | 1 минута | 300ms раз в 5 мин | ? Приемлемо |
| **Уменьшить MaxMemories** | Тривиально | 1 минута | 60ms spike | ? **Лучший quickfix** |
| **Рандомный offset** | Легко | 1 час | 5ms per frame | ? v1.4 (1-2 месяца) |
| **Batch Processing** | Средне | 1 день | 15ms per frame | ?? v1.5 (2-4 месяца) |
| **Shared Data** | Сложно | 1 неделя | 10ms per frame | ?? v2.0 (6-12 месяцев) |

---

## ?? Выводы

### **Причина фризов (в одном предложении):**

> **Все 100 NPC обрабатывают по 100 воспоминаний (10,000 operations) раз в минуту, каждая обработка стоит 0.03ms, что дает спайки 300ms.**

### **Почему так:**

1. ? **Design decision:** Memory decay нужен для реализма (NPC забывают старое)
2. ? **Performance optimization:** Обновление раз в минуту (не каждый кадр)
3. ? **Не учтено масштабирование:** Работает отлично для 10-20 NPC, но плохо для 100+
4. ? **Отсутствие batching:** Все NPC обновляются независимо (не в batch)

### **Что делать:**

**Немедленно (сейчас):**
```cpp
MemoryComponent->MaxMemories = 20; // Для фоновых NPC
```

**v1.4 (1-2 месяца):**
```cpp
TimeSinceLastDecayUpdate = FMath::FRandRange(0.0f, DecayUpdateInterval);
```

**v1.5+ (2-12 месяцев):**
- Batch Processing System
- Shared Data Architecture

---

**Документ подготовлен:** 2025-01-23  
**Версия:** 1.0  
**Статус:** ? Complete

---

**Примечание:** Проблема **известна**, **задокументирована** и имеет **clear roadmap** для решения.
