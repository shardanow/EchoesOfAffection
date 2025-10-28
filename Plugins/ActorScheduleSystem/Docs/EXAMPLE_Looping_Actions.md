# Quick Example: Looping Actions

## Простой пример - Бесконечный рабочий цикл

### Задача
Создать NPC рабочего, который с 08:00 до 17:00 постоянно повторяет цикл:
1. Идет к станку
2. Работает 30 секунд
3. Идет на склад
4. Берет деталь (5 секунд)
5. Возвращается к станку

### Решение

#### 1. Создайте маркеры локаций

```cpp
// В уровне разместите LocationMarker'ы:
LocationMarker_Machine (Tag: Location.Workplace.Machine)
LocationMarker_Storage (Tag: Location.Workplace.Storage)
```

#### 2. Создайте Composite Action

В Content Browser: **Right-click ? Miscellaneous ? Data Asset ? ScheduleAction_Composite**

Назовите: `DA_Action_WorkLoop`

**Настройки:**
- `Action Name`: "Рабочий цикл"
- `bExecuteSequentially`: ? true
- `bLoopChildActions`: ? true
- `MaxLoops`: 0 (бесконечно)
- `CheckInterval`: 0.5

**Child Actions** (добавьте по порядку):

1. **MoveToLocation** (к станку)
   - Action Name: "Идти к станку"
   - Location Selector: ActorTag
     - Required Tags: `Location.Workplace.Machine`
     - Selection Mode: Closest
   - Acceptance Radius: 100

2. **PlayAnimation** (работа)
   - Action Name: "Работать"
   - Animation: `Anim_Working` (ваша анимация)
   - Duration: 30.0

3. **MoveToLocation** (на склад)
   - Action Name: "Идти на склад"
   - Location Selector: ActorTag
     - Required Tags: `Location.Workplace.Storage`
     - Selection Mode: Closest
   - Acceptance Radius: 100

4. **Wait** (взять деталь)
   - Action Name: "Взять деталь"
   - Wait Duration: 5.0

> **Результат**: После шага 4 цикл автоматически вернется к шагу 1!

#### 3. Создайте Schedule Entry

В Content Browser: **Right-click ? Miscellaneous ? Data Asset ? ScheduleEntryData**

Назовите: `DA_Entry_WorkShift`

**Настройки:**
- `Entry Name`: "Рабочая смена"
- `Priority`: 80
- `Recurrence Type`: Daily
- `Time Condition`:
  - Time Type: TimeRange
  - Start Hour: 8
  - Start Minute: 0
  - End Hour: 17
  - End Minute: 0
  - Days Of Week: [1, 2, 3, 4, 5] (Пн-Пт)
- `Action`: ? Выберите `DA_Action_WorkLoop`

#### 4. Создайте Schedule Data

В Content Browser: **Right-click ? Miscellaneous ? Data Asset ? ScheduleData**

Назовите: `DA_Schedule_Worker`

**Настройки:**
- `Schedule Name`: "Расписание рабочего"
- `Entries`: Добавьте `DA_Entry_WorkShift`

#### 5. Добавьте компонент на NPC

1. Выберите вашего NPC в уровне
2. **Add Component** ? `Schedule Component`
3. В компоненте:
   - `Schedule Data`: ? Выберите `DA_Schedule_Worker`
   - `bScheduleEnabled`: ? true

#### 6. Настройте интеграцию времени

Убедитесь, что у вас есть `TimeOfDayScheduleIntegration` компонент в уровне или GameState.

### Результат

?? **Поведение NPC:**
- **До 08:00**: Idle (или другое расписание)
- **08:00-17:00**: Бесконечно повторяет цикл:
  - Идет к станку ? Работает 30 сек ? Идет на склад ? Ждет 5 сек ? [снова к станку]
- **После 17:00**: Расписание отменяет действие, NPC переходит к следующему Entry

### Модификации

#### Ограничить количество циклов
```
MaxLoops: 5  // Выполнить ровно 5 циклов за смену
```

#### Добавить перерыв каждые N циклов
Используйте вложенный Composite:
```
Composite (внешний, MaxLoops = 0):
  ?? Composite (рабочий цикл, MaxLoops = 3)
  ?  ?? [станок ? работа ? склад ? пауза]
  ?? Wait (перерыв 60 секунд)
```

## Отладка

### Проверка работы цикла

1. **Откройте Visual Logger** (Ctrl+L в PIE)
2. Выберите вашего NPC
3. В категории `LogScheduleComposite` вы увидите:
   ```
   Starting composite action with 4 children (Sequential: YES)
   Starting first child action
   Child action 0 completed, advancing to next
   Starting next child action 1
 ...
   Child action 3 completed, advancing to next
   Looping child actions (Loop 1/-1)  // -1 = infinite
   Restarting first child action
   ```

### Типичные проблемы

? **NPC не двигается**
- Проверьте, что у NPC есть AIController
- Проверьте, что NavMesh построен в уровне
- Убедитесь, что LocationMarker'ы имеют правильные теги

? **Цикл не повторяется**
- Убедитесь: `bLoopChildActions = true`
- Убедитесь: `bExecuteSequentially = true`

? **Цикл останавливается раньше времени**
- Проверьте `MaxLoops` (0 = бесконечно)
- Проверьте время окна в Schedule Entry

## См. также
- [Полная документация по зацикливанию](FEATURE_Loop_Child_Actions.md)
- [README.md](../README.md)
- [QUICKSTART.md](../QUICKSTART.md)
