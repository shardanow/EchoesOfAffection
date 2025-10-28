# BUGFIX: Access Violation при добавлении записей в Schedule Data Asset

## Проблема

При добавлении нового элемента в массив `Entries` в дата ассете расписания (`UScheduleData`) возникала критическая ошибка:

```
Exception thrown at 0x000001D3B4B17770 (UnrealEditor-ScheduleCore.dll) in UnrealEditor.exe: 
0xC0000005: Access violation reading location 0x0000000000000068.
```

### Воспроизведение
1. Открыть Schedule Data Asset в редакторе
2. Добавить второй элемент в массив `Entries`
3. Fatal Error с Access Violation

## Причина

В методе `UScheduleData::PostEditChangeProperty` и `UScheduleData::GetSortedEntries` лямбда-функция сортировки имела две критические ошибки:

1. **Неправильный тип параметра**: Лямбда принимала `const UScheduleEntryData&` вместо `const TObjectPtr<UScheduleEntryData>&`
2. **Отсутствие проверки на nullptr**: При добавлении нового элемента в массив он может быть временно `nullptr`, что приводило к разыменованию нулевого указателя

### Проблемный код

```cpp
// НЕПРАВИЛЬНО - вызывает Access Violation
Entries.Sort([](const UScheduleEntryData& A, const UScheduleEntryData& B)
{
    return A.Priority > B.Priority; // Разыменование без проверки
});
```

## Решение

Исправлены оба метода с добавлением:
1. Корректного типа параметра (`TObjectPtr<UScheduleEntryData>`)
2. Проверки на `nullptr` с безопасной обработкой

### Исправленный код

```cpp
// ПРАВИЛЬНО - безопасная сортировка
SortedEntries.Sort([](const TObjectPtr<UScheduleEntryData>& A, const TObjectPtr<UScheduleEntryData>& B)
{
    // Handle null entries - push them to the end
    if (!A && !B) return false;
    if (!A) return false; // A is null, B comes first
    if (!B) return true;  // B is null, A comes first
    
  return A->Priority > B->Priority;
});
```

## Измененные файлы

- `Plugins/ActorScheduleSystem/Source/ScheduleCore/Private/Data/ScheduleData.cpp`
  - `UScheduleData::GetSortedEntries()` - исправлена лямбда сортировки
  - `UScheduleData::PostEditChangeProperty()` - исправлена лямбда сортировки

## Тестирование

После применения исправления:
1. ? Добавление новых записей в Schedule Data Asset работает без ошибок
2. ? Автоматическая сортировка по приоритету работает корректно
3. ? Обработка null-элементов в массиве происходит безопасно

## Дополнительные улучшения

Логика обработки null-указателей:
- Если оба элемента `null` ? они считаются равными
- Если только A null ? B имеет приоритет
- Если только B null ? A имеет приоритет
- Null-элементы всегда размещаются в конце отсортированного массива

## Рекомендации

При работе с `TArray<TObjectPtr<T>>` в Unreal Engine всегда:
1. Используйте `const TObjectPtr<T>&` в лямбдах, а не `const T&`
2. Проверяйте указатели на `nullptr` перед разыменованием
3. Обрабатывайте null-элементы явно в функциях сортировки/сравнения

## Версия

- Дата исправления: 27.01.2025
- Затронутые модули: ScheduleCore
- Версия плагина: 1.2+
