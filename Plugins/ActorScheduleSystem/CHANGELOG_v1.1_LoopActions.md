# CHANGELOG - Loop Child Actions Feature

## v1.1 - 2025-01-XX

### Added - Зацикливание дочерних экшенов в Composite Action

#### Новые свойства в `UScheduleAction_Composite`:

1. **`bLoopChildActions`** (bool)
   - Включает зацикливание дочерних экшенов
   - По умолчанию: `false`
   - Работает только при `bExecuteSequentially = true`

2. **`MaxLoops`** (int32)
   - Максимальное количество циклов (0 = бесконечно)
   - По умолчанию: `0`
   - Отображается только если `bLoopChildActions = true`

3. **`LoopCount`** (int32) - в `FScheduleChildActionData`
   - Внутренний счетчик выполненных циклов

#### Измененные файлы:

**Header:**
- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Public/Actions/ScheduleAction_Composite.h`
  - Добавлены новые UPROPERTY
  - Добавлено поле `LoopCount` в `FScheduleChildActionData`

**Implementation:**
- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Private/Actions/ScheduleAction_Composite.cpp`
  - Модифицирована `CheckAndAdvanceSequential()` для поддержки зацикливания
  - Обновлена `GetActionDescription_Implementation()` для отображения информации о циклах
  - Добавлено логирование при зацикливании

#### Новая документация:

1. `Plugins/ActorScheduleSystem/Docs/FEATURE_Loop_Child_Actions.md`
   - Полное описание функциональности
   - Примеры использования
   - Технические детали

2. `Plugins/ActorScheduleSystem/Docs/EXAMPLE_Looping_Actions.md`
   - Пошаговый пример создания зацикленного рабочего расписания
   - Отладка и troubleshooting

3. `Plugins/ActorScheduleSystem/README.md`
   - Обновлена секция "Built-in Components"
   - Добавлена секция "Looping Child Actions" в Advanced Features

#### Совместимость:

- ? Полная обратная совместимость
- ? Существующие Composite Actions продолжают работать без изменений
- ? Никаких breaking changes

#### Использование:

```cpp
// Бесконечный цикл (до отмены)
Composite Action:
?? bExecuteSequentially = true
?? bLoopChildActions = true
?? MaxLoops = 0  // Infinite
?? Child Actions: [Action1, Action2, Action3]

// Ограниченное зацикливание (3 раза)
Composite Action:
?? bExecuteSequentially = true
?? bLoopChildActions = true
?? MaxLoops = 3
?? Child Actions: [Action1, Action2, Action3]
```

#### Логирование:

Новые лог-сообщения в категории `LogScheduleComposite`:
- `Looping child actions (Loop N/M)` - При начале нового цикла
- `Restarting first child action` - При рестарте первого экшена
- `Max loops (N) reached for composite` - При достижении лимита циклов

#### Use Cases:

1. **Рабочие смены** - NPC повторяет цикл работы весь день
2. **Патрульные маршруты** - Бесконечный обход точек
3. **Повторяющиеся действия** - Любая активность с N повторений

#### Known Limitations:

1. Работает только для последовательного выполнения (`bExecuteSequentially = true`)
2. Для параллельных экшенов не поддерживается
3. Отмена прерывает текущий цикл немедленно

#### Performance:

- Накладные расходы: минимальные (один int32 и несколько проверок)
- Память: +4 байта на активный Composite Action
- CPU: Без дополнительных таймеров или обновлений

---

## Контакты

Вопросы и предложения: https://github.com/shardanow/EchoesOfAffection
