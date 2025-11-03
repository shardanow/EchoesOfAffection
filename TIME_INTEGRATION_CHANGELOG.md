# Time Integration Changelog

## Дата: 2024
## Задача: Интеграция систем памяти и отношений с системой времени

### Изменённые файлы

#### 1. NPCMemoryComponent (DialogueSystem)
**Файлы:**
- `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Public/Components/NPCMemoryComponent.h`
- `Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Components/NPCMemoryComponent.cpp`

**Изменения:**
- ? Добавлена подписка на события `Time.Event.HourChanged` и `Time.Event.DayChanged`
- ? Добавлен метод `OnGameTimeHourChanged()` — обработка деградации памяти каждый игровой час
- ? Добавлен метод `OnGameTimeDayChanged()` — ежедневное обслуживание памяти
- ? Добавлен метод `ProcessMemoryDecayByGameTime(float GameHours)` — деградация на основе игрового времени
- ? Добавлен метод `ProcessDailyMemoryMaintenance()` — консолидация воспоминаний
- ? Добавлена условная компиляция `#if GAMEEVENTBUS_AVAILABLE`
- ? Добавлен forward declaration для `FGameEventPayload`

**Новые возможности:**
- Автоматическая деградация памяти на основе игровых часов
- Укрепление часто вспоминаемых воспоминаний
- Снижение интереса к темам разговоров со временем

---

#### 2. RelationshipSubsystem (RelationshipSystem)
**Файлы:**
- `Plugins/RelationshipSystem/Source/RelationshipSystemCore/Public/Subsystems/RelationshipSubsystem.h`
- `Plugins/RelationshipSystem/Source/RelationshipSystemCore/Private/Subsystems/RelationshipSubsystem.cpp`

**Изменения:**
- ? Добавлена подписка на события `Time.Event.HourChanged` и `Time.Event.DayChanged`
- ? Добавлен метод `OnGameTimeHourChanged()` — запуск `ProcessDecay(1.0)` каждый час
- ? Добавлен метод `OnGameTimeDayChanged()` — ежедневное обслуживание отношений
- ? Добавлен метод `ProcessDailyRelationshipMaintenance()` — удаление просроченных трейтов
- ? Добавлен метод `ValidateAndCleanupRelationships()` — очистка недействительных связей
- ? Добавлена условная компиляция `#if GAMEEVENTBUS_AVAILABLE`
- ? Добавлен forward declaration для `FGameEventPayload`
- ? Обновлён метод `SendGameEvent()` для работы с GameEventBus

**Новые возможности:**
- Автоматический decay dimensions каждый игровой час
- Автоматическое удаление просроченных traits
- Переоценка состояний при смене дня
- Очистка недействительных отношений

---

#### 3. TimeSystemGameEventEmitter (GameEventBus)
**Файлы:**
- `Plugins/GameEventBus/Source/GameEventBus/Public/Integration/TimeSystemGameEventEmitter.h`
- `Plugins/GameEventBus/Source/GameEventBus/Private/Integration/TimeSystemGameEventEmitter.cpp`

**Изменения:**
- ? Добавлена подписка на события `TimeOfDaySubsystem`:
  - `OnHourChanged` ? эмитирует `Time.Event.HourChanged`
  - `OnDayChanged` ? эмитирует `Time.Event.DayChanged`
  - `OnPhaseChanged` ? эмитирует `Time.Event.PhaseChanged`
  - `OnSeasonChanged` ? эмитирует `Time.Event.SeasonChanged`
- ? Добавлена условная компиляция `#if TIMEOFDAYSYSTEM_AVAILABLE`
- ? Добавлен fallback на таймер для проектов без TimeOfDaySystem
- ? Добавлены методы обработчиков событий от TimeOfDaySubsystem

**Новые возможности:**
- Автоматическая трансляция событий TimeOfDaySubsystem в GameEventBus
- Поддержка проектов с и без TimeOfDaySystem
- Эмиссия событий фаз дня и смены сезонов

---

#### 4. Документация
**Файлы:**
- `Plugins/DialogueSystem/Docs/Memory_Time_Integration.md` (новый)

**Содержание:**
- Архитектура интеграции с диаграммой
- Цепочка событий между системами
- Формулы деградации памяти и отношений
- Инструкции по настройке компонентов
- Примеры использования (C++ и Blueprint)
- Таблица событий GameEventBus
- Рекомендации по производительности
- Best Practices

---

### Архитектура интеграции

```
TimeOfDaySubsystem
    ? (события)
TimeSystemGameEventEmitter
    ? (трансляция)
GameEventBusSubsystem
    ? (подписка)
?????????????????????????
?  ?
NPCMemoryComponent  RelationshipSubsystem
```

### События

**Эмитируемые:**
- `Time.Event.HourChanged` (IntParam = час)
- `Time.Event.DayChanged` (IntParam = день)
- `Time.Event.PhaseChanged` (IntParam = фаза)
- `Time.Event.SeasonChanged` (IntParam = сезон)

**Подписчики:**
- NPCMemoryComponent ? деградация памяти
- RelationshipSubsystem ? decay отношений

---

### Технические детали

**Зависимости:**
- Слабая связанность через GameEventBus
- Условная компиляация (#if __has_include)
- Forward declarations для опциональных типов

**Производительность:**
- Обработка только при смене часа/дня (не каждый кадр)
- Batch processing для всех отношений
- Настраиваемые интервалы обновления

**Совместимость:**
- Unreal Engine 5.6+
- C++14
- Опциональная зависимость от TimeOfDaySystem
- Опциональная зависимость от GameEventBus

---

### Тестирование

**Требуется проверка:**
1. ? Компиляция без ошибок
2. ? Подписка на события при запуске
3. ? Деградация памяти каждый игровой час
4. ? Ежедневное обслуживание систем
5. ? Логирование событий
6. ? Корректность работы без TimeOfDaySystem
7. ? Корректность работы без GameEventBus

**Логи для проверки:**
```
LogNPCMemory: Subscribed to Time.Event.HourChanged
LogRelationshipSystem: Subscribed to Time.Event.HourChanged
LogTimeEvents: Hour Changed: X
LogNPCMemory: Game hour changed to X, processing hourly decay
LogRelationshipSystem: Game hour changed to X, processing hourly decay
```

---

### Коммит

```bash
git add Plugins/DialogueSystem/Source/DialogueSystemRuntime/Public/Components/NPCMemoryComponent.h
git add Plugins/DialogueSystem/Source/DialogueSystemRuntime/Private/Components/NPCMemoryComponent.cpp
git add Plugins/RelationshipSystem/Source/RelationshipSystemCore/Public/Subsystems/RelationshipSubsystem.h
git add Plugins/RelationshipSystem/Source/RelationshipSystemCore/Private/Subsystems/RelationshipSubsystem.cpp
git add Plugins/GameEventBus/Source/GameEventBus/Public/Integration/TimeSystemGameEventEmitter.h
git add Plugins/GameEventBus/Source/GameEventBus/Private/Integration/TimeSystemGameEventEmitter.cpp
git add Plugins/DialogueSystem/Docs/Memory_Time_Integration.md
git add TIME_INTEGRATION_CHANGELOG.md

git commit -m "feat: integrate NPC memory and relationships with TimeOfDaySystem

- Added time-based memory decay for NPCMemoryComponent
- Added time-based relationship decay for RelationshipSubsystem
- Integrated TimeSystemGameEventEmitter with TimeOfDaySubsystem
- Added comprehensive documentation for time integration
- Implemented hourly and daily maintenance routines
- Added conditional compilation for optional dependencies
- Memory decay respects importance, emotion, and recall frequency
- Relationship decay respects dimension settings and trait duration
"
```

---

### Следующие шаги

1. Тестирование в редакторе
2. Добавление console commands для отладки
3. Создание примеров использования
4. Добавление Editor Utility Widgets для визуализации
5. Настройка GameplayTags в `DefaultGameplayTags.ini`
