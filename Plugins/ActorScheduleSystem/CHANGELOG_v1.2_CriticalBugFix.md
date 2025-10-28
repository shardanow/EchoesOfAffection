# CHANGELOG v1.2 - Critical Bug Fix

## Version 1.2.0 (2025-01-27)

### Critical Bug Fixes

#### ?? FIXED: Access Violation при добавлении записей в Schedule Data Asset

**Проблема:**
При добавлении второго или последующих элементов в массив `Entries` в Schedule Data Asset происходил Fatal Error:
```
Exception thrown at 0x000001D3B4B17770 (UnrealEditor-ScheduleCore.dll) in UnrealEditor.exe: 
0xC0000005: Access violation reading location 0x0000000000000068.
```

**Причина:**
В методах `UScheduleData::GetSortedEntries()` и `UScheduleData::PostEditChangeProperty()` лямбда-функция сортировки использовала некорректные типы параметров и не проверяла указатели на `nullptr`:

```cpp
// ПРОБЛЕМНЫЙ КОД
Entries.Sort([](const UScheduleEntryData& A, const UScheduleEntryData& B)
{
    return A.Priority > B.Priority; // Access Violation при A или B == nullptr
});
```

При добавлении нового элемента в массив через редактор UE может временно создавать `nullptr` элементы, что приводило к разыменованию нулевого указателя.

**Решение:**
1. Исправлен тип параметров лямбды на `const TObjectPtr<UScheduleEntryData>&`
2. Добавлена проверка на `nullptr` с корректной обработкой null-элементов

```cpp
// ИСПРАВЛЕННЫЙ КОД
Entries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B)
{
    // Handle null entries - push them to the end
    if (!A && !B) return false;
    if (!A) return false; // A is null, B comes first
    if (!B) return true;  // B is null, A comes first
    
  return A->Priority > B->Priority;
});
```

**Затронутые файлы:**
- `Plugins/ActorScheduleSystem/Source/ScheduleCore/Private/Data/ScheduleData.cpp`
  - Метод `GetSortedEntries()`
  - Метод `PostEditChangeProperty()`

**Документация:**
- Создан документ `Docs/BUGFIX_Array_Sort_Crash.md` с подробным описанием проблемы и решения

### Воздействие

**До исправления:**
- ? Невозможно добавить более одной записи в Schedule Data Asset
- ? Crash при редактировании существующих Schedule Data с несколькими entries
- ? Автосортировка по приоритету вызывала Access Violation

**После исправления:**
- ? Безопасное добавление любого количества записей
- ? Корректная автоматическая сортировка по приоритету
- ? Правильная обработка null-элементов в массиве
- ? Стабильная работа редактора Schedule Data Asset

### Технические детали

#### Логика обработки null-указателей
```cpp
if (!A && !B) return false;  // Оба null - считаются равными
if (!A) return false;    // A null - B имеет приоритет
if (!B) return true;         // B null - A имеет приоритет
// Иначе - сравнение по Priority
```

Null-элементы всегда помещаются в конец отсортированного массива.

#### Рекомендации для разработчиков

При работе с `TArray<TObjectPtr<T>>` в Unreal Engine:
1. ? Всегда используйте `const TObjectPtr<T>&` в лямбдах, а не `const T&`
2. ? Проверяйте указатели на `nullptr` перед разыменованием
3. ? Обрабатывайте null-элементы явно в функциях сортировки/сравнения
4. ? Помните, что UE может создавать временные null-элементы при редактировании массивов

### Обратная совместимость

? **Полная обратная совместимость**
- Нет изменений в API
- Нет изменений в data layout
- Существующие Schedule Data Assets работают без изменений
- Безопасно обновляться с любой предыдущей версии

### Тестирование

Проверено:
- ? Добавление нового элемента в пустой Schedule Data Asset
- ? Добавление нескольких элементов подряд
- ? Изменение приоритета существующих элементов
- ? Удаление элементов
- ? Автосортировка при включенном `bAutoSortByPriority`
- ? Работа с null-элементами в массиве

### Связанные изменения

Нет других изменений в этой версии.

### Обновление

1. Обновите файл `ScheduleData.cpp`
2. Перекомпилируйте плагин
3. Перезапустите редактор
4. Проверьте, что существующие Schedule Data открываются корректно

### Следующие шаги

После этого исправления рекомендуется:
1. Проверить все существующие Schedule Data Assets
2. Обновить документацию проекта
3. Провести регрессионное тестирование системы расписаний

---

## История версий

- **v1.2.0** (2025-01-27): Critical bug fix - Access Violation при добавлении entries
- **v1.1.0**: Road Navigation System
- **v1.0.0**: Initial Release

## Ссылки

- Основной CHANGELOG: `CHANGELOG.md`
- Документация по баг фиксу: `Docs/BUGFIX_Array_Sort_Crash.md`
- Road Navigation CHANGELOG: `CHANGELOG_v1.0_RoadNavigation.md`
- Loop Actions CHANGELOG: `CHANGELOG_v1.1_LoopActions.md`

## Контакты

**Project**: Echoes of Affection  
**Repository**: https://github.com/shardanow/EchoesOfAffection  
**Plugin**: Actor Schedule System

---

*Этот CHANGELOG описывает критическое исправление бага, обнаруженного 27 января 2025 года.*
