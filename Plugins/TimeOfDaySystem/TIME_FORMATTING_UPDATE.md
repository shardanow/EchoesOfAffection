# Time of Day System - Time Formatting Updates

**Дата**: 25 октября 2024, 14:30  
**Версия**: 1.3 (Time Formatting Update)  
**Статус**: ? **УСПЕШНО ДОБАВЛЕНО**

---

## ?? Что было добавлено

### 1. **Поддержка 12-часового формата с AM/PM** ?

#### Новый метод в Subsystem:
```cpp
FString GetFormattedTime12Hour(bool bIncludeSeconds = false) const;
```

**Примеры вывода:**
- `3:45 PM` - без секунд
- `3:45:23 PM` - с секундами
- `12:00 AM` - полночь
- `12:00 PM` - полдень

#### Улучшен метод виджета:
```cpp
// В UTimeOfDayWidget::UpdateWidgetElementsSimplified()
if (bUse12HourFormat)
{
    TimeString = TimeSubsystem->GetFormattedTime12Hour(bShowSeconds);
}
else
{
  TimeString = TimeSubsystem->GetFormattedTime(bShowSeconds);
}
```

**Теперь `bUse12HourFormat` реально работает!**

---

### 2. **Система месяцев** ?

#### Добавлены названия месяцев:

**12 месяцев разделены по сезонам:**

| Сезон | Месяцы |
|-------|--------|
| **Spring** (Весна) | January, February, March |
| **Summer** (Лето) | April, May, June |
| **Autumn** (Осень) | July, August, September |
| **Winter** (Зима) | October, November, December |

**Вычисление:**
- Каждый месяц = 30 дней
- Каждый сезон = 3 месяца = 90 дней
- 1 год = 12 месяцев = 360 дней

#### Улучшен формат даты:

**До:**
```
Year 1, Spring, Day 15 (Monday)
```

**После:**
```
15 March, Year 1 (Monday)
```

**Формат:** `<День> <Месяц>, Year <Год> (<День недели>)`

---

### 3. **Кастомные названия месяцев** ?

#### Добавлено в TimeOfDaySettings:

```cpp
/** Custom month names (12 names for 12 months). If empty, uses default names. */
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Calendar")
TArray<FString> MonthNames;
```

**Как использовать:**

1. Откройте ваш DataAsset `DA_TimeOfDay_Default`
2. В категории **Calendar** найдите **Month Names**
3. Добавьте 12 кастомных названий (или оставьте пустым для стандартных)

**Пример кастомных названий (фэнтези):**
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

---

### 4. **Добавлена Blueprint Library функция** ?

```cpp
UFUNCTION(BlueprintPure, Category = "Time of Day")
static FString GetFormattedTime12Hour(const UObject* WorldContextObject, bool bIncludeSeconds = false);
```

**Использование в Blueprint:**
```
Get Formatted Time 12 Hour (World Context = Self, Include Seconds = false)
  ?
  ?
Returns: "3:45 PM"
```

---

## ?? Технические детали

### Вычисление месяца и дня:

```cpp
const int32 MonthIndex = (CurrentTime.DayOfSeason - 1) / 30;  // 0-2
const int32 DayOfMonth = ((CurrentTime.DayOfSeason - 1) % 30) + 1;  // 1-30

// Абсолютный индекс месяца (0-11)
const int32 AbsoluteMonthIndex = (static_cast<int32>(CurrentTime.Season) * 3) + MonthIndex;
```

**Примеры:**
- `DayOfSeason = 1` ? `MonthIndex = 0`, `DayOfMonth = 1` ? "1 January"
- `DayOfSeason = 45` ? `MonthIndex = 1`, `DayOfMonth = 15` ? "15 February"
- `DayOfSeason = 90` ? `MonthIndex = 2`, `DayOfMonth = 30` ? "30 March"

### Конвертация 12-часового формата:

```cpp
int32 Hour12 = CurrentTime.Hour % 12;
if (Hour12 == 0)
{
    Hour12 = 12; // Полночь/Полдень показываются как 12
}

const TCHAR* AMPM = (CurrentTime.Hour < 12) ? TEXT("AM") : TEXT("PM");
```

**Таблица конвертации:**

| 24-час | 12-час |
|--------|--------|
| 0 | 12 AM |
| 1 | 1 AM |
| 11 | 11 AM |
| 12 | 12 PM |
| 13 | 1 PM |
| 23 | 11 PM |

---

## ?? Использование

### В C++ коде:

```cpp
UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();

// 24-часовой формат
FString Time24 = TimeSystem->GetFormattedTime(false);  // "15:45"
FString Time24Sec = TimeSystem->GetFormattedTime(true);  // "15:45:23"

// 12-часовой формат с AM/PM
FString Time12 = TimeSystem->GetFormattedTime12Hour(false);  // "3:45 PM"
FString Time12Sec = TimeSystem->GetFormattedTime12Hour(true);  // "3:45:23 PM"

// Дата с месяцами
FString Date = TimeSystem->GetFormattedDate();  // "15 March, Year 1 (Monday)"
```

### В Blueprint:

**Вариант 1: Через Subsystem**
```
Get Time of Day Subsystem
  ?
  ?
Get Formatted Time 12 Hour (Include Seconds = false)
  ?
  ?
Print String ? "3:45 PM"
```

**Вариант 2: Через Blueprint Library (проще)**
```
Get Formatted Time 12 Hour (World Context = Self, Include Seconds = false)
  ?
  ?
Print String ? "3:45 PM"
```

### В UMG Widget:

```cpp
// В вашем TimeOfDayWidget Blueprint:

// Установите опцию:
bUse12HourFormat = true  // Для 12-часового формата
bUse12HourFormat = false  // Для 24-часового формата

// Виджет автоматически использует правильный формат!
```

---

## ?? Настройка кастомных месяцев

### Шаг 1: Откройте DA_TimeOfDay_Default

### Шаг 2: В категории Calendar найдите Month Names

### Шаг 3: Добавьте 12 названий:

**Вариант A: Оставить пустым** ? используются стандартные English названия

**Вариант B: Русские названия**
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

**Вариант C: Фэнтези названия**
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

**?? ВАЖНО:** Должно быть ровно **12 названий**, иначе используются стандартные!

---

## ?? Изменённые файлы

### 1. TimeOfDaySubsystem.h
- ? Добавлен `GetFormattedTime12Hour()`

### 2. TimeOfDaySubsystem.cpp
- ? Реализован `GetFormattedTime12Hour()`
- ? Улучшен `GetFormattedDate()` с месяцами
- ? Добавлена поддержка кастомных названий месяцев

### 3. TimeOfDaySettings.h
- ? Добавлен `TArray<FString> MonthNames`

### 4. TimeOfDayWidget.cpp
- ? Использование `GetFormattedTime12Hour()` когда `bUse12HourFormat = true`
- ? Исправлен `GetFormattedTimeString()` для поддержки 12-часового формата

### 5. TimeOfDayBlueprintLibrary.h/.cpp
- ? Добавлена функция `GetFormattedTime12Hour()`

---

## ? Сборка

```
Result: Succeeded
Total execution time: 8.52 seconds
Files compiled: 15
```

---

## ?? Примеры вывода

### Время:

**24-часовой формат:**
```
00:00 - Полночь
06:30 - Раннее утро
12:00 - Полдень
15:45 - День
18:30 - Вечер
23:59 - Поздняя ночь
```

**12-часовой формат с AM/PM:**
```
12:00 AM - Полночь
6:30 AM - Раннее утро
12:00 PM - Полдень
3:45 PM - День
6:30 PM - Вечер
11:59 PM - Поздняя ночь
```

### Даты:

**Стандартные названия:**
```
1 January, Year 1 (Monday)
15 March, Year 1 (Wednesday)
30 June, Year 2 (Friday)
25 December, Year 5 (Sunday)
```

**Кастомные фэнтези названия:**
```
1 Wintermoon, Year 1 (Monday)
15 Stormwind, Year 1 (Wednesday)
30 Sunhigh, Year 2 (Friday)
25 Starfall, Year 5 (Sunday)
```

---

## ?? Что дальше?

### Возможные улучшения:

1. **Локализация** - поддержка FText для перевода названий месяцев
2. **Форматирование дат** - кастомные шаблоны (DD/MM/YYYY, MM/DD/YYYY, etc.)
3. **Короткие названия месяцев** - Jan, Feb, Mar вместо January, February, March
4. **Праздники** - специальные даты с названиями ("Christmas Day")
5. **Високосные годы** - поддержка 366-дневных годов

---

## ?? Документация

Обновлённые файлы:
- **README.md** - добавлена информация о форматировании
- **QUICKSTART.md** - примеры использования 12-часового формата
- **CONTENT_GUIDE.md** - примеры с месяцами

---

**Статус**: ? **READY TO USE**  
**Версия**: 1.3  
**Компиляция**: Успешно  

**Happy Game Development!** ???
