# NPC Memory & Relationship Time Integration

## Обзор

Системы памяти NPCMemoryComponent и отношений RelationshipSystem интегрированы с системой времени TimeOfDaySystem через централизованную шину событий GameEventBus.

## Архитектура

```
TimeOfDaySubsystem (источник времени)
     ? (события: OnHourChanged, OnDayChanged)
TimeSystemGameEventEmitter (трансляция)
        ? (события: Time.Event.HourChanged, Time.Event.DayChanged)
GameEventBusSubsystem (центральная шина)
        ? (подписка)
    ?????????????????????????
    ?         ?
NPCMemoryComponent RelationshipSubsystem
(деградация памяти)   (decay отношений)
```

## Цепочка событий

### 1. TimeOfDaySubsystem
- **Роль**: Центральный источник игрового времени
- **События**:
  - `OnHourChanged` - каждый игровой час
  - `OnDayChanged` - каждый игровой день
  - `OnPhaseChanged` - смена фазы дня (рассвет, день, закат, ночь)
  - `OnSeasonChanged` - смена сезона

### 2. TimeSystemGameEventEmitter
- **Роль**: Мост между TimeOfDaySubsystem и GameEventBus
- **Действия**:
  - Подписывается на события TimeOfDaySubsystem
  - Транслирует их в GameEventBus с унифицированными тегами
- **Эмитируемые события**:
  - `Time.Event.HourChanged` (IntParam = час)
  - `Time.Event.DayChanged` (IntParam = день)
  - `Time.Event.PhaseChanged` (IntParam = фаза, StringParam = имя фазы)
  - `Time.Event.SeasonChanged` (IntParam = сезон, StringParam = имя сезона)

### 3. GameEventBusSubsystem
- **Роль**: Центральная шина событий
- **Возможности**:
  - Подписка на события по тегам (точное совпадение или иерархия)
  - Хранение истории событий для отладки
  - Поддержка Blueprint и C++ подписчиков

### 4. NPCMemoryComponent
- **Роль**: Система памяти NPC с забыванием
- **Подписка**:
  - `Time.Event.HourChanged` ? `OnGameTimeHourChanged`
  - `Time.Event.DayChanged` ? `OnGameTimeDayChanged`
- **Обработка**:
  - **Каждый час**: `ProcessMemoryDecayByGameTime(1.0)` - деградация памяти
  - **Каждый день**: `ProcessDailyMemoryMaintenance()` - консолидация памяти

#### Деградация памяти
```cpp
// Формула деградации за игровой час
DecayAmount = GameHours / 24.0f;  // 1 час = 1/24 дня
FreshnessLoss = BaseDecayRate * DecayAmount * Modifiers;

// Модификаторы:
// - Важность (Importance): более важные помнятся дольше
// - Эмоция (Emotion): эмоциональные помнятся лучше (+50% стойкость)
// - Память NPC (MemoryRetentionMultiplier): индивидуальная память
// - Негативный bias (NegativeBias): склонность помнить плохое
// - Частота вспоминания (RecallCount): укрепляет память
```

#### Ежедневное обслуживание
- Укрепление часто вспоминаемых воспоминаний (+0.5 важности за 5+ вспоминаний)
- Консолидация похожих воспоминаний (опционально)
- Снижение интереса к давно не обсуждавшимся темам (-5% за неделю)

### 5. RelationshipSubsystem
- **Роль**: Глобальная система отношений между акторами
- **Подписка**:
  - `Time.Event.HourChanged` ? `OnGameTimeHourChanged`
  - `Time.Event.DayChanged` ? `OnGameTimeDayChanged`
- **Обработка**:
  - **Каждый час**: `ProcessDecay(1.0)` - деградация всех измерений
  - **Каждый день**: `ProcessDailyRelationshipMaintenance()` - удаление просроченных трейтов

#### Деградация измерений (dimensions)
```cpp
// Для каждого измерения с включенным decay:
DecayedValue = Value - (DecayRate * GameHours);
ClampedValue = Clamp(DecayedValue, MinValue, MaxValue);

// Параметры задаются в RelationshipDimension asset:
// - bEnableDecay: включить деградацию
// - DecayRate: скорость (единиц в час)
// - DecayTowards: целевое значение (центр, мин, макс)
```

#### Ежедневное обслуживание
- Удаление просроченных трейтов (с учётом Duration)
- Переоценка переходов состояний
- Очистка недействительных отношений (destroyed actors)

## Настройка

### 1. Добавление TimeSystemGameEventEmitter

В акторе с TimeOfDaySubsystem (обычно GameMode или Level Blueprint):

```cpp
// C++
UTimeSystemGameEventEmitter* Emitter = CreateDefaultSubobject<UTimeSystemGameEventEmitter>(TEXT("TimeEventEmitter"));
Emitter->bEmitHourlyEvents = true;
Emitter->bEmitDailyEvents = true;
Emitter->bLogEvents = true; // для отладки
```

Или в Blueprint:
1. Добавьте компонент `TimeSystemGameEventEmitter` к актору
2. В Details настройте:
   - ? Emit Events
   - ? Emit Hourly Events
   - ? Emit Daily Events
   - ? Log Events (для отладки)

### 2. Настройка NPCMemoryComponent

```cpp
// C++ (в конструкторе NPC)
MemoryComponent = CreateDefaultSubobject<UNPCMemoryComponent>(TEXT("MemoryComponent"));
MemoryComponent->bEnableMemoryDecay = true;
MemoryComponent->BaseDecayRate = 0.05f; // 5% за день
MemoryComponent->MinImportanceThreshold = 10.0f;
MemoryComponent->MemoryRetentionMultiplier = 1.0f; // 1.0 = нормальная память
```

Blueprint:
1. Добавьте `NPCMemoryComponent` к NPC
2. В Details ? Decay:
   - ? Enable Memory Decay
   - Base Decay Rate: 0.05 (5% в день)
   - Min Importance Threshold: 10.0
3. В Personality:
   - Memory Retention Multiplier: 1.0 (можно варьировать 0.5-2.0)

### 3. Настройка RelationshipSubsystem

Система автоматически подписывается на события времени при инициализации.

Настройка decay для измерений (в RelationshipDimension asset):
- ? Enable Decay
- Decay Rate: 1.0 (единиц в час)
- Decay Towards: Center (или Min/Max)

Настройка времени жизни трейтов (в RelationshipTrait asset):
- Duration: 24.0 (часов, 0 = бесконечно)

## Теги событий

### Эмитируемые TimeSystemGameEventEmitter
- `Time.Event.HourChanged`
- `Time.Event.DayChanged`
- `Time.Event.PhaseChanged`
- `Time.Event.SeasonChanged`
- `Weather.Event.Changed` (если настроена погода)

### Эмитируемые NPCMemoryComponent
- `NPC.Memory.Created` (при создании важной памяти, если `bAutoAffectRelationships`)

### Эмитируемые RelationshipSubsystem
- `Relationship.DimensionChanged`
- `Relationship.StateTransitioned`
- `Relationship.TraitAdded`
- `Relationship.TraitRemoved`
- `Relationship.ActionExecuted`

## Пример: Создание воспоминания с auto-decay

```cpp
// C++
UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
if (MemoryComp)
{
    FName MemoryId = MemoryComp->CreateMemory(
 EMemoryType::PlayerChoice,
        FText::FromString("Player saved my village from bandits"),
        85.0f,  // Высокая важность - будет помниться долго
        EMemoryEmotion::Joyful,  // Эмоциональное - доп. бонус к стойкости
        PlayerCharacter
    );
    
    // Добавляем контекстные теги
    FGameplayTagContainer Tags;
    Tags.AddTag(FGameplayTag::RequestGameplayTag("Context.Combat"));
Tags.AddTag(FGameplayTag::RequestGameplayTag("Context.Heroic"));
    MemoryComp->AddTagsToMemory(MemoryId, Tags);
}
```

Blueprint:
1. Get Component ? NPC Memory Component
2. Create Memory:
   - Type: `Player Choice`
   - Description: "Player saved my village"
   - Importance: `85.0`
   - Emotion: `Joyful`
   - Related Actor: `Player Character`
3. Add Tags To Memory

Это воспоминание:
- Начнёт деградировать согласно `BaseDecayRate`
- Будет помниться дольше благодаря высокой важности (85)
- Получит бонус от эмоциональной окраски (Joyful)
- Может триггерить события в RelationshipSystem (если `bAutoAffectRelationships`)

## Пример: Настройка decay измерения отношений

В RelationshipDimension asset (например, "Trust"):

```
Trust Dimension:
- Min Value: 0.0
- Max Value: 100.0
- Default Value: 50.0
- Enable Decay: ?
- Decay Rate: 0.5  (0.5 единиц в час = 12 единиц в день)
- Decay Towards: Center (стремится к 50.0)
```

Результат:
- Если Trust = 100, через 24 часа ? ~88 (-12)
- Если Trust = 0, через 24 часа ? ~12 (+12)
- Если Trust = 50, остаётся 50 (уже в центре)

## Отладка

### Логирование

Включите логирование для отладки:

```cpp
// В DefaultEngine.ini
[Core.Log]
LogNPCMemory=Verbose
LogRelationshipSystem=Verbose
LogTimeEvents=Log
```

### Console Commands

```
// Проверка статуса памяти
NPCMemory.DumpMemories

// Проверка отношений
Relationship.DumpAll

// Ручное ускорение времени
Time.SetTimeScale 10.0
```

### События в GameEventBus

```cpp
// Подписка на все события времени для отладки
FGameplayTag TimeEventsTag = FGameplayTag::RequestGameplayTag("Time.Event");
EventBus->SubscribeToEventHierarchy(TimeEventsTag, [](const FGameEventPayload& Payload) {
    UE_LOG(LogTemp, Log, TEXT("Time Event: %s"), *Payload.EventTag.ToString());
});
```

## Производительность

### NPCMemoryComponent
- **Tick interval**: 1.0 секунда (настраиваемо через `DecayUpdateInterval`)
- **Decay обработка**: раз в 60 секунд по умолчанию
- **Game Time decay**: срабатывает только при смене часа (экономит CPU)

### RelationshipSubsystem
- **Hourly decay**: обрабатывает все активные отношения (~O(n) где n = количество отношений)
- **Daily maintenance**: O(n*m) где m = количество трейтов на отношение
- **Рекомендуется**: <1000 активных отношений для оптимальной производительности

## Best Practices

1. **Важность воспоминаний**: 
   - 0-30: Мелочи, быстро забываются
   - 31-60: Нормальные события
   - 61-85: Важные события
   - 86-100: Критические, почти не забываются

2. **Decay Rate**:
   - Память: 0.03-0.1 (3-10% в день)
   - Отношения: 0.1-2.0 единиц в час в зависимости от диапазона

3. **Подписка на события**:
   - Используйте hierarchy tags (`Time.Event`) для подписки на все события времени
   - Отписывайтесь в EndPlay для предотвращения утечек памяти

4. **Performance**:
   - Ограничивайте количество воспоминаний (`MaxMemories`)
   - Используйте `PruneOldMemories` периодически
   - Включайте decay только для нужных измерений

## Совместимость

- **Unreal Engine**: 5.6+
- **C++ Standard**: C++14
- **Требуемые плагины**:
  - TimeOfDaySystem
  - GameEventBus
  - DialogueSystem (для NPCMemoryComponent)
  - RelationshipSystem

## См. также

- [TimeOfDaySystem Documentation](../../TimeOfDaySystem/Docs/README.md)
- [GameEventBus Documentation](../../GameEventBus/Docs/README.md)
- [NPC Memory Guide](./README_NPCMemory.md)
- [Relationship System Integration](../../RelationshipSystem/Docs/Integration.md)
