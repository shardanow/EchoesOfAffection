# Location Selector в Actions

## Проблема

При использовании композитных действий (Composite Action) все дочерние действия используют **одну и ту же** локацию из `FScheduleExecutionContext`, которая выбирается один раз на уровне `ScheduleEntry`.

Если вам нужно, чтобы каждое дочернее действие двигалось к **разным** локациям с разными тегами, старый подход не работал.

## Решение

Теперь `UScheduleAction_MoveToLocation` поддерживает собственный `LocationSelector`, который переопределяет локацию из Context.

## Как использовать

### 1. Один Location Selector для всех (старый способ)

Если все действия идут к одной локации, назначьте `LocationSelector` на уровне `ScheduleEntry`:

```
ScheduleEntry
  ?? LocationSelector: ActorTag("Location.Fields")
  ?? Action: Composite
     ?? MoveToLocation (без LocationSelector)
?? Wait (5 сек)
       ?? MoveToLocation (без LocationSelector)
```

Результат: Оба MoveToLocation идут к одной локации с тегом "Location.Fields".

### 2. Разные Location Selectors (новый способ)

Если каждое действие должно идти к разным локациям, **не назначайте LocationSelector на ScheduleEntry**, а назначьте его на каждое дочернее действие:

```
ScheduleEntry
  ?? LocationSelector: НЕ НАЗНАЧЕН!
  ?? Action: Composite
       ?? MoveToLocation
       ?    ?? LocationSelector: ActorTag("Location.Fields")
       ?? Wait (5 сек)
       ?? MoveToLocation
    ?    ?? LocationSelector: ActorTag("Location.Town")
    ?? Wait (3 сек)
       ?? MoveToLocation
       ?? LocationSelector: ActorTag("Location.Home")
```

Результат:
1. Движение к маркеру с тегом "Location.Fields"
2. Ожидание 5 секунд
3. Движение к маркеру с тегом "Location.Town"
4. Ожидание 3 секунды
5. Движение к маркеру с тегом "Location.Home"

## Логирование

В логах вы увидите:

```
MoveToLocation: Using own LocationSelector: ScheduleLocationSelector_ActorTag_0
MoveToLocation: Selected location: X=1311.576 Y=-6383.472 Z=-0.000
   ReferenceActor: BP_LocationMarker_C_2
   LocationTags: (GameplayTags=((TagName="Location.Fields")))
```

Или, если используется Context:

```
MoveToLocation: Using location from Context
   Context LocationTags: (GameplayTags=((TagName="Location.Fields")))
```

## Приоритет выбора локации

1. **Собственный LocationSelector** у MoveToLocation ? выбирается новая локация
2. **Локация из Context** ? используется локация, выбранная на уровне ScheduleEntry
3. **Нет локации** ? ошибка, действие не выполняется

## Примечание

Этот функционал также применим к другим действиям, которые могут добавить поддержку собственного LocationSelector в будущем.
