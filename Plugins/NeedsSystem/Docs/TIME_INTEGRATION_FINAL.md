# Needs System - Time Integration COMPLETE ?

## Финальная версия v1.0.2

### ? ЧТО РЕАЛИЗОВАНО

1. **Event-Driven Updates**
   - Подписка на `TimeOfDaySubsystem::OnHourChanged`
   - Обновление каждый игровой час (не каждый кадр!)
   - Автоматическое отключение polling при активном Time System

2. **Dynamic Time Scale Reading**
   - Читает `RealSecondsPerGameDay` из настроек Time System
   - Читает `TimeScale` multiplier динамически
   - **Нет hardcoded значений** - всё из конфигов

3. **Optional Dependency**
   - Работает с и без Time System
   - Conditional compilation (`WITH_TIME_SYSTEM=1/0`)
   - Graceful fallback на real-time mode

4. **Public API в TimeOfDaySubsystem**
   - Добавлен `GetSettings()` метод
   - Доступ к `TimeConversion` настройкам для внешних систем

### ?? КАК ИСПОЛЬЗОВАТЬ

#### С Time System (рекомендуется):

```
DA_NeedsSettings:
Use Game Time: ?

DA_TimeOfDaySettings:
  Real Seconds Per Game Day: 1200  (20 минут = 1 день)
  Time Scale: 1.0

Результат:
  - 1 игровой час = 50 реальных секунд
  - Потребности синхронизированы с циклом день/ночь
  - Decay rate 5.0/час = -5 поинтов каждые 50 секунд
```

#### Без Time System (fallback):

```
DA_NeedsSettings:
  Use Game Time: ?
  Real Time Scale: 1.0  (1 игровой час = 1 реальный час)

Результат:
  - Независимо от Time System
  - Decay rate 5.0/час = -5 поинтов каждый реальный час
```

### ?? ПРИМЕРЫ НАСТРОЕК

**Быстрое время (10 минут на день):**
```
Real Seconds Per Game Day: 600
Time Scale: 1.0
? 1 игровой час = 25 реальных секунд
```

**2x скорость:**
```
Real Seconds Per Game Day: 1200
Time Scale: 2.0
? 1 игровой час = 25 реальных секунд
```

**Реальное время:**
```
Use Game Time: ?
Real Time Scale: 1.0
? 1 игровой час = 1 реальный час
```

### ?? КОД

**Подписка на события:**
```cpp
#if WITH_TIME_SYSTEM
UTimeOfDaySubsystem* TimeSubsystem = World->GetSubsystem<UTimeOfDaySubsystem>();
TimeSubsystem->OnHourChanged.AddDynamic(this, &UNeedsSubsystem::OnTimeSystemHourChanged);
#endif
```

**Чтение настроек:**
```cpp
const UTimeOfDaySettings* Settings = TimeSubsystem->GetSettings();
float RealSecondsPerGameDay = Settings->TimeConversion.RealSecondsPerGameDay;
float TimeScale = Settings->TimeConversion.TimeScale;
```

### ?? ОТЛАДКА

```
needs.debug 1  // Включить debug overlay
```

Смотрите в логах:
```
? NeedsSubsystem: Successfully subscribed to Time System events
? NeedsSubsystem: Updated needs for 1 game hour (Total: X hours)
```

### ?? ПРОИЗВОДИТЕЛЬНОСТЬ

**С Time System:**
- ? Zero polling overhead
- ? Обновления ровно 1 раз в игровой час
- ? LOD система работает (Near/Far/Parked)

**Без Time System:**
- Polling с настраиваемой частотой (default 10 Hz)
- Batch processing
- LOD оптимизация

### ? CHECKLIST ИНТЕГРАЦИИ

- [x] Добавлен `GetSettings()` в TimeOfDaySubsystem
- [x] Подписка на `OnHourChanged` event
- [x] Динамическое чтение `RealSecondsPerGameDay`
- [x] Динамическое чтение `TimeScale`
- [x] Conditional compilation (`WITH_TIME_SYSTEM`)
- [x] Optional plugin dependency
- [x] Event-driven mode (no polling)
- [x] Fallback mode (real time)
- [x] Документация обновлена
- [x] Проект собирается успешно

### ?? СЛЕДУЮЩИЕ ШАГИ

1. ? Закройте редактор (если открыт)
2. ? Проект уже собран
3. ? Откройте проект
4. ? Проверьте `DA_NeedsSettings`:
   - `Use Game Time = ?`
5. ? Проверьте `DA_TimeOfDaySettings`:
   - `Real Seconds Per Game Day = 1200`
   - `Time Scale = 1.0`
6. ? Тестируйте!

### ?? ФОРМУЛЫ

**С Time System:**
```
GameSecondsPerRealSecond = (86400 / RealSecondsPerGameDay) * TimeScale

Пример (1200s/день, 1.0x):
  = (86400 / 1200) * 1.0
  = 72 игровых секунд за реальную секунду
  = 0.02 игровых часа за реальную секунду
  = 50 реальных секунд за игровой час
```

**Без Time System:**
```
DeltaGameHours = DeltaTime * (RealTimeScale / 3600)

Пример (RealTimeScale 1.0):
  = DeltaTime * (1.0 / 3600)
  = 1 реальный час = 1 игровой час
```

---

## ? ГОТОВО!

**Версия:** 1.0.2 (COMPLETE)  
**Дата:** 25 октября 2025  
**Статус:** Production Ready  
**Все TODO выполнены!** ??
