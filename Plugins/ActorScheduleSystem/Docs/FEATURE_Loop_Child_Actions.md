# Feature: Loop Child Actions in Composite Action

## Описание
Добавлена функциональность зацикливания списка дочерних экшенов в `UScheduleAction_Composite`.

## Новые свойства

### `bLoopChildActions` (bool)
- **Описание**: Включает зацикливание дочерних экшенов после завершения последнего
- **Значение по умолчанию**: `false`
- **Категория**: Action
- **Условие отображения**: Только для последовательного выполнения (`bExecuteSequentially = true`)

### `MaxLoops` (int32)
- **Описание**: Максимальное количество повторений цикла (0 = бесконечно)
- **Значение по умолчанию**: `0` (бесконечно)
- **Категория**: Action
- **Условие отображения**: Только если `bLoopChildActions = true`
- **Ограничения**: Минимум 0

### `LoopCount` (int32) - внутреннее поле
- **Описание**: Счетчик текущего количества выполненных циклов
- **Тип**: Внутреннее поле в `FScheduleChildActionData`

## Использование

### Пример 1: Бесконечное зацикливание (рабочий цикл)

```cpp
// Создаем Composite Action в редакторе
Composite Action: "Рабочий процесс"
??? bExecuteSequentially = true
??? bLoopChildActions = true
??? MaxLoops = 0  // Бесконечно
??? Child Actions:
    1. MoveToLocation (Станок)
    2. PlayAnimation (Работа)
  3. Wait (30 секунд)
    4. MoveToLocation (Склад)
    5. Wait (10 секунд)
```

**Результат**: NPC будет бесконечно повторять цикл: идти к станку ? работать ? ждать ? идти на склад ? ждать ? снова к станку...

### Пример 2: Ограниченное зацикливание

```cpp
// NPC должен 3 раза обойти патрульный маршрут
Composite Action: "Патруль 3 круга"
??? bExecuteSequentially = true
??? bLoopChildActions = true
??? MaxLoops = 3  // Ровно 3 раза
??? Child Actions:
    1. MoveToLocation (Точка 1)
    2. Wait (5 секунд)
    3. MoveToLocation (Точка 2)
    4. Wait (5 секунд)
    5. MoveToLocation (Точка 3)
    6. Wait (5 секунд)
```

**Результат**: NPC обойдет маршрут ровно 3 раза, затем экшен завершится.

### Пример 3: Без зацикливания (по умолчанию)

```cpp
Composite Action: "Утренняя рутина"
??? bExecuteSequentially = true
??? bLoopChildActions = false  // Выполнить один раз
??? Child Actions:
    1. MoveToLocation (Кухня)
    2. PlayAnimation (Пить кофе)
    3. MoveToLocation (Ванная)
    4. PlayAnimation (Умываться)
```

**Результат**: NPC выполнит последовательность один раз и завершит.

## Логика работы

### Последовательность выполнения с зацикливанием:

1. **Старт**: Выполняется первый дочерний экшен
2. **Прогресс**: После завершения текущего экшена запускается следующий
3. **Конец последовательности**: 
   - Если `bLoopChildActions = false` ? Composite завершается
   - Если `bLoopChildActions = true`:
     - Увеличивается `LoopCount`
     - Проверяется условие: `(MaxLoops == 0) || (LoopCount < MaxLoops)`
     - **Если true**: Сбрасывается `CurrentActionIndex = 0`, очищается `ChildHandles`, запускается первый экшен снова
  - **Если false**: Composite завершается (достигнут `MaxLoops`)

### Логирование

```cpp
// При зацикливании
UE_LOG(LogScheduleComposite, Log, TEXT("Looping child actions (Loop %d/%d)"), LoopCount, MaxLoops);

// При достижении лимита
UE_LOG(LogScheduleComposite, Log, TEXT("Max loops (%d) reached for composite"), MaxLoops);
```

## Ограничения

1. **Только для последовательного выполнения**: Зацикливание работает только при `bExecuteSequentially = true`
2. **Параллельное выполнение не поддерживается**: Если `bExecuteSequentially = false`, свойство `bLoopChildActions` игнорируется
3. **Отмена прерывает цикл**: Вызов `CancelAction()` немедленно останавливает все дочерние экшены и сбрасывает цикл

## Комбинация с расписанием

### Пример: Рабочий день с зацикливанием

```cpp
Schedule Entry: "Рабочая смена"
??? TimeCondition: 08:00-17:00 (9 часов)
??? RecurrenceType: Daily
??? Action: Composite (Loop)
    ??? bLoopChildActions = true
    ??? MaxLoops = 0  // Бесконечно до конца временного окна
    ??? Child Actions:
        1. MoveToLocation (Рабочее место)
      2. PlayAnimation (Работа)
3. Wait (60 секунд)
        4. MoveToLocation (Перерыв)
      5. Wait (30 секунд)
```

**Результат**: 
- С 08:00 до 17:00 NPC будет бесконечно повторять цикл работы
- В 17:00 временное окно закончится, расписание отменит экшен
- На следующий день в 08:00 цикл начнется заново (благодаря `RecurrenceType: Daily`)

## Производительность

- **Накладные расходы**: Минимальные, только проверка условия и сброс индекса
- **Память**: Один дополнительный `int32` (LoopCount) на каждый активный Composite Action
- **Таймеры**: Используется тот же таймер `CheckInterval`, что и без зацикливания

## Отладка

### Проверка текущего цикла (C++)
```cpp
if (FScheduleChildActionData* Data = ChildActionData.Find(ActionHandle.HandleID))
{
  int32 CurrentLoop = Data->LoopCount;
    int32 CurrentAction = Data->CurrentActionIndex;
    
    UE_LOG(LogTemp, Log, TEXT("Loop %d, Action %d/%d"), 
        CurrentLoop, CurrentAction, ChildActions.Num());
}
```

### Visual Logger
Включите категорию `LogScheduleComposite` для отслеживания зацикливания.

## Миграция существующих данных

Существующие `Composite Action` **не требуют изменений**:
- `bLoopChildActions` по умолчанию `false` — поведение не изменится
- Все старые действия будут работать как раньше

## Changelog

### v1.1 - 2025-01-XX
- ? Добавлено свойство `bLoopChildActions`
- ? Добавлено свойство `MaxLoops`
- ? Добавлен счетчик `LoopCount` в `FScheduleChildActionData`
- ? Реализована логика зацикливания в `CheckAndAdvanceSequential()`
- ? Обновлено описание в `GetActionDescription_Implementation()`
- ? Добавлено логирование циклов

## См. также
- [README.md](../README.md) - Общая документация системы расписания
- [ScheduleAction_Composite.h](../Source/ScheduleRuntime/Public/Actions/ScheduleAction_Composite.h) - Заголовочный файл
- [ScheduleAction_Composite.cpp](../Source/ScheduleRuntime/Private/Actions/ScheduleAction_Composite.cpp) - Реализация
