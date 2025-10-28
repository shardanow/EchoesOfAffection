# Исправление: Разные локации в Composite Action

## Что было исправлено

**Проблема**: При использовании композитного действия (например, "Движение ? Ожидание ? Движение ? Ожидание") все дочерние MoveToLocation использовали **одну и ту же** локацию, выбранную один раз на уровне ScheduleEntry.

**Решение**: Добавлена поддержка собственного LocationSelector в каждое действие MoveToLocation.

## Как теперь настроить разные локации

### Шаг 1: Откройте ваш ScheduleEntry

В Content Browser найдите ваш ScheduleEntry (например, `Morning_Activity`).

### Шаг 2: Уберите LocationSelector с Entry

**ВАЖНО**: Если вы хотите, чтобы каждое движение было к разным точкам, **не назначайте** LocationSelector на уровне Entry!

### Шаг 3: Настройте LocationSelector для каждого MoveToLocation

Откройте ваш Composite Action и для каждого дочернего MoveToLocation:

1. Выберите действие MoveToLocation
2. В Details найдите раздел **Movement**
3. Раскройте свойство **Location Selector**
4. Нажмите **+** для создания нового селектора
5. Выберите тип (например, `ScheduleLocationSelector_ActorTag`)
6. Настройте **Required Tags** для этого конкретного движения

### Пример конфигурации

**Morning Activity**:
- LocationSelector: **НЕ НАЗНАЧЕН**
- Action: Composite (Sequential)
  - **Child 0**: MoveToLocation
    - LocationSelector: ActorTag
      - Required Tags: `Location.Fields`
  - **Child 1**: Wait (Duration: 5 seconds)
  - **Child 2**: MoveToLocation
    - LocationSelector: ActorTag
      - Required Tags: `Location.Town`
  - **Child 3**: Wait (Duration: 3 seconds)
  - **Child 4**: MoveToLocation
    - LocationSelector: ActorTag
      - Required Tags: `Location.Home`
  - **Child 5**: Wait (Duration: 2 seconds)

## Результат

Теперь персонаж будет:
1. Идти к маркеру с тегом `Location.Fields`
2. Ждать 5 секунд
3. Идти к маркеру с тегом `Location.Town`
4. Ждать 3 секунды
5. Идти к маркеру с тегом `Location.Home`
6. Ждать 2 секунды
7. Завершить активность

## Проверка в логах

После запуска игры вы увидите в логах:

```
MoveToLocation: Using own LocationSelector: ScheduleLocationSelector_ActorTag_0
MoveToLocation: Selected location: X=1311.576 Y=-6383.472 Z=-0.000
   ReferenceActor: BP_LocationMarker_Fields
   LocationTags: (GameplayTags=((TagName="Location.Fields")))
```

И при следующем движении:

```
MoveToLocation: Using own LocationSelector: ScheduleLocationSelector_ActorTag_1
MoveToLocation: Selected location: X=2456.123 Y=-4521.789 Z=-0.000
   ReferenceActor: BP_LocationMarker_Town
   LocationTags: (GameplayTags=((TagName="Location.Town")))
```

## Совместимость

Старый способ (один LocationSelector на Entry) всё ещё работает! Если вам нужно, чтобы все движения были к одной точке, просто:
- Назначьте LocationSelector на уровне ScheduleEntry
- Не назначайте LocationSelector в дочерних MoveToLocation

## Файлы изменений

- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Public/Actions/ScheduleAction_MoveToLocation.h`
- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Private/Actions/ScheduleAction_MoveToLocation.cpp`
