# Date Format Options - Update v1.4

**Дата**: 25 октября 2024, 14:45  
**Версия**: 1.4 (Date Format Options)  
**Статус**: ? **УСПЕШНО ДОБАВЛЕНО**

---

## ?? Что было добавлено

### Новый Enum: `EDateFormat`

Теперь вы можете выбрать формат отображения даты из **6 различных вариантов**!

```cpp
UENUM(BlueprintType)
enum class EDateFormat : uint8
{
    Full,     // 15 March, Year 1 (Monday)
    DotShort,          // 15.03.01
    SlashEuropean,     // 15/03/2001
    SlashAmerican,     // 03/15/2001
    ISO,     // 2001-03-15
    DotEuropean        // 15.03.2001
};
```

---

## ?? Примеры всех форматов

### Для даты: 15 марта 2001 года (понедельник)

| Формат | Пример вывода | Описание |
|--------|---------------|----------|
| **Full** | `15 March, Year 1 (Monday)` | Полный формат с названием месяца и дня недели |
| **DotShort** | `15.03.01` | Короткий формат с точками (DD.MM.YY) |
| **DotEuropean** | `15.03.2001` | Европейский формат с точками (DD.MM.YYYY) |
| **SlashEuropean** | `15/03/2001` | Европейский формат со слэшами (DD/MM/YYYY) |
| **SlashAmerican** | `03/15/2001` | Американский формат (MM/DD/YYYY) |
| **ISO** | `2001-03-15` | ISO 8601 формат (YYYY-MM-DD) |

---

## ?? Использование

### В Widget Blueprint:

1. Откройте ваш `TimeOfDayWidget` Blueprint
2. Найдите свойство **Date Format**
3. Выберите нужный формат из выпадающего списка:
   - Full (по умолчанию)
   - Dot Short
   - Dot European
   - Slash European
   - Slash American
   - ISO

**Виджет автоматически обновит отображение даты!**

---

### В C++ коде:

```cpp
UTimeOfDaySubsystem* TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();

// Разные форматы
FString FullDate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::Full);
// "15 March, Year 1 (Monday)"

FString ShortDate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::DotShort);
// "15.03.01"

FString EuroDate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::SlashEuropean);
// "15/03/2001"

FString USADate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::SlashAmerican);
// "03/15/2001"

FString ISODate = TimeSystem->GetFormattedDateWithFormat(EDateFormat::ISO);
// "2001-03-15"
```

---

### В Blueprint:

**Вариант 1: Через Subsystem**
```
Get Time of Day Subsystem
  ?
  ?
Get Formatted Date With Format (Format = Dot Short)
  ?
  ?
Print String ? "15.03.01"
```

**Вариант 2: Через Blueprint Library (проще)**
```
Get Formatted Date With Format (World Context = Self, Format = ISO)
  ?
  ?
Print String ? "2001-03-15"
```

---

## ?? Изменённые файлы

### 1. **TimeOfDayTypes.h** - Добавлен enum
```cpp
UENUM(BlueprintType)
enum class EDateFormat : uint8
{
    Full,
    DotShort,
    SlashEuropean,
    SlashAmerican,
    ISO,
    DotEuropean
};
```

### 2. **TimeOfDaySubsystem.h** - Новый метод
```cpp
UFUNCTION(BlueprintCallable, Category = "Time of Day")
FString GetFormattedDateWithFormat(EDateFormat Format) const;
```

### 3. **TimeOfDaySubsystem.cpp** - Реализация
```cpp
FString UTimeOfDaySubsystem::GetFormattedDateWithFormat(EDateFormat Format) const
{
    // Вычисление месяца и дня
    const int32 MonthIndex = (CurrentTime.DayOfSeason - 1) / 30;
    const int32 DayOfMonth = ((CurrentTime.DayOfSeason - 1) % 30) + 1;
    const int32 MonthNumber = AbsoluteMonthIndex + 1;
    
    switch (Format)
    {
        case EDateFormat::DotShort:
       return FString::Printf(TEXT("%02d.%02d.%02d"), DayOfMonth, MonthNumber, YearShort);

        case EDateFormat::ISO:
            return FString::Printf(TEXT("%04d-%02d-%02d"), Year, MonthNumber, DayOfMonth);
        
        // ... и т.д.
    }
}
```

### 4. **TimeOfDayWidget.h** - Новое свойство
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day")
EDateFormat DateFormat = EDateFormat::Full;
```

### 5. **TimeOfDayWidget.cpp** - Использование формата
```cpp
DateText->SetText(FText::FromString(TimeSubsystem->GetFormattedDateWithFormat(DateFormat)));
```

### 6. **TimeOfDayBlueprintLibrary.h/.cpp** - Blueprint функция
```cpp
UFUNCTION(BlueprintPure, Category = "Time of Day")
static FString GetFormattedDateWithFormat(const UObject* WorldContextObject, EDateFormat Format);
```

---

## ?? Техническая информация

### Вычисление даты:

```cpp
// Месяц в сезоне (0-2)
const int32 MonthIndex = (CurrentTime.DayOfSeason - 1) / 30;

// День месяца (1-30)
const int32 DayOfMonth = ((CurrentTime.DayOfSeason - 1) % 30) + 1;

// Абсолютный месяц года (0-11)
const int32 AbsoluteMonthIndex = (static_cast<int32>(CurrentTime.Season) * 3) + MonthIndex;

// Номер месяца (1-12)
const int32 MonthNumber = AbsoluteMonthIndex + 1;
```

### Примеры вычислений:

| DayOfSeason | Season | MonthIndex | DayOfMonth | AbsoluteMonth | MonthNumber |
|-------------|--------|------------|------------|---------------|-------------|
| 1 | Spring | 0 | 1 | 0 | 1 (January) |
| 15 | Spring | 0 | 15 | 0 | 1 (January) |
| 31 | Spring | 1 | 1 | 1 | 2 (February) |
| 45 | Spring | 1 | 15 | 1 | 2 (February) |
| 61 | Spring | 2 | 1 | 2 | 3 (March) |
| 90 | Spring | 2 | 30 | 2 | 3 (March) |
| 1 | Summer | 0 | 1 | 3 | 4 (April) |

---

## ?? Для разных регионов

### Европа (большинство стран):
- **DotEuropean**: `15.03.2001`
- **SlashEuropean**: `15/03/2001`
- Формат: День.Месяц.Год

### США:
- **SlashAmerican**: `03/15/2001`
- Формат: Месяц/День/Год

### Для баз данных и API:
- **ISO**: `2001-03-15`
- ISO 8601 стандарт
- Формат: Год-Месяц-День

### Для компактного отображения:
- **DotShort**: `15.03.01`
- Экономия места в UI
- Формат: ДД.ММ.ГГ

---

## ?? Рекомендации по использованию

### Когда использовать каждый формат:

**Full** - Идеально для:
- Главный экран игры
- Журналы квестов
- Сюжетные элементы
- Когда важно показать день недели

**DotShort** - Идеально для:
- Компактный UI
- Списки сохранений
- Маленькие виджеты
- Мобильные версии

**DotEuropean / SlashEuropean** - Идеально для:
- Европейская локализация
- Стандартный формат
- Привычный для большинства игроков

**SlashAmerican** - Идеально для:
- Американская локализация
- США рынок

**ISO** - Идеально для:
- Сохранения в файлы
- API запросы
- Сортировка дат
- Технические логи

---

## ?? Пример UI Layout

```
???????????????????????????????????
?  Time: 3:45 PM                 ?  ? bUse12HourFormat = true
?  Date: 15.03.01     ?  ? DateFormat = DotShort
?  Season: Spring          ?  ? bShowSeason = true
?  Phase: Afternoon    ?  ? bShowPhase = true
???????????????????????????????????
```

Или более подробный:

```
???????????????????????????????????
?  15:45:23             ?  ? bUse12HourFormat = false, bShowSeconds = true
?  15 March, Year 1 (Monday)     ?? DateFormat = Full
?  Spring - Afternoon         ?
???????????????????????????????????
```

---

## ? Сборка

```
Result: Succeeded
Total execution time: 8.72 seconds
Files compiled: 23
UHT generated: 10 files
```

---

## ?? Полный список форматов

### 1. Full (Полный)
```
15 March, Year 1 (Monday)
```
- Показывает: день месяца, название месяца, год, день недели
- Использует кастомные названия месяцев (если заданы)
- Самый информативный формат

### 2. DotShort (Короткий с точками)
```
15.03.01
```
- Формат: DD.MM.YY
- Год показывается двумя цифрами
- Самый компактный формат

### 3. DotEuropean (Европейский с точками)
```
15.03.2001
```
- Формат: DD.MM.YYYY
- Полный год (4 цифры)
- Стандарт для Европы

### 4. SlashEuropean (Европейский со слэшами)
```
15/03/2001
```
- Формат: DD/MM/YYYY
- Полный год (4 цифры)
- Альтернатива DotEuropean

### 5. SlashAmerican (Американский)
```
03/15/2001
```
- Формат: MM/DD/YYYY
- Месяц идёт первым!
- Стандарт для США

### 6. ISO (ISO 8601)
```
2001-03-15
```
- Формат: YYYY-MM-DD
- Международный стандарт
- Идеально для сортировки

---

## ?? Что дальше?

### Возможные улучшения:

1. **Локализация форматов** - автоматический выбор формата по языку игры
2. **Кастомные разделители** - настройка точек/слэшей/тире
3. **Короткие названия месяцев** - Jan, Feb, Mar вместо January
4. **Формат без года** - "15 марта" вместо "15 March, Year 1"
5. **Относительные даты** - "Сегодня", "Вчера", "3 дня назад"

---

**Статус**: ? **READY TO USE**  
**Версия**: 1.4  
**Компиляция**: Успешно  

**Happy Game Development!** ???
