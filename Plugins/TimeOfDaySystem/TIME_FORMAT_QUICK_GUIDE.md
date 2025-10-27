# ? Быстрая инструкция - Форматирование времени

**Версия**: 1.3  
**Статус**: ? Готово к использованию

---

## ?? 12-часовой формат с AM/PM

### В Widget Blueprint:

1. Откройте ваш `TimeOfDayWidget` Blueprint
2. Установите **bUse12HourFormat = true**
3. Готово! Время будет показываться как "3:45 PM"

### В C++ коде:

```cpp
UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();

// 12-часовой формат
FString Time = TimeSystem->GetFormattedTime12Hour(false);  // "3:45 PM"
```

### В Blueprint:

```
Get Time of Day Subsystem ? Get Formatted Time 12 Hour
```

---

## ?? Названия месяцев

### Стандартные (по умолчанию):

```
1 January, Year 1 (Monday)
15 March, Year 1 (Wednesday)
30 December, Year 1 (Sunday)
```

### Кастомные названия:

**Шаг 1:** Откройте `DA_TimeOfDay_Default`

**Шаг 2:** В категории **Calendar** найдите **Month Names**

**Шаг 3:** Добавьте 12 названий:

#### Пример 1: Русские
```
0: Январь
1: Февраль
2: Март
3: Апрель
4: Май
5: Июнь
6: Июль
7: Август
8: Сентябрь
9: Октябрь
10: Ноябрь
11: Декабрь
```

#### Пример 2: Фэнтези
```
0: Wintermoon
1: Frostfall
2: Stormwind
3: Springtide
4: Blossomrise
5: Sunhigh
6: Harvest
7: Goldleaf
8: Amberfall
9: Frostscar
10: Darknight
11: Starfall
```

**Результат:**
```
15 Март, Year 1 (Wednesday)
15 Stormwind, Year 1 (Wednesday)
```

---

## ?? Примеры использования

### Пример 1: UI Clock Widget

```
Event Tick
 ?
    ?
Get Formatted Time 12 Hour (World Context = Self)
    ?
    ?
Set Text (ClockText) ? "11:23 AM"
```

### Пример 2: Date Display

```
Event Tick
    ?
    ?
Get Formatted Date (World Context = Self)
    ?
    ?
Set Text (DateText) ? "15 March, Year 1 (Monday)"
```

### Пример 3: Full Time & Date Widget

```
TimeText: "3:45 PM"
DateText: "15 March, Year 1 (Monday)"
SeasonText: "Spring"
PhaseText: "Afternoon"
```

---

## ?? Настройка в DataAsset

### DA_TimeOfDay_Default

```
Calendar:
  ?? Days Per Season: 90 (автоматически: 3 месяца ? 30 дней)
  ?? Seasons Per Year: 4
  ?? Days Per Week: 7
  ?? Month Names: [12 названий] (опционально)

Initial Game Time:
  ?? Year: 1
  ?? Season: Spring
  ?? Day of Season: 15  ? День 15 = середина месяца 1 (February)
  ?? Hour: 10
  ?? Minute: 30
  ?? Second: 0
```

**Соответствие:**
- `DayOfSeason 1-30` = Первый месяц сезона
- `DayOfSeason 31-60` = Второй месяц сезона
- `DayOfSeason 61-90` = Третий месяц сезона

---

## ?? Таблица форматов

### Время:

| Формат | Пример | Как получить |
|--------|--------|--------------|
| 24-часовой | `15:45` | `GetFormattedTime(false)` |
| 24-часовой с сек. | `15:45:23` | `GetFormattedTime(true)` |
| 12-часовой | `3:45 PM` | `GetFormattedTime12Hour(false)` |
| 12-часовой с сек. | `3:45:23 PM` | `GetFormattedTime12Hour(true)` |

### Дата:

| Элемент | Пример | Тип |
|---------|--------|-----|
| День месяца | `15` | 1-30 |
| Название месяца | `March` | String (кастомизируемое) |
| Год | `Year 1` | int32 |
| День недели | `Monday` | Enum |

---

## ? Готово!

Теперь у вас есть:
- ? 12-часовой формат с AM/PM
- ? Система месяцев (12 месяцев)
- ? Кастомные названия месяцев
- ? Красивый формат даты

**Просто установите `bUse12HourFormat = true` и добавьте названия месяцев!**

---

**Больше информации:** `TIME_FORMATTING_UPDATE.md`  
**Полная документация:** `README.md`

**Happy Formatting!** ???
