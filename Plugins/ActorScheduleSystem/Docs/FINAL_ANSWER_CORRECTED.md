# ?? ОКОНЧАТЕЛЬНЫЙ ОТВЕТ: Диагностика системы расписаний

## ?? Ваш вопрос

> "Где мы можем видеть и должны ли что система расписаний запустилась корректно, тайм менеджмент интеграция успешна и задачи по расписанию выполняются?"

---

## ? Краткий ответ

**Смотрите в Output Log (Window ? Developer Tools ? Output Log)**

### Что вы ДОЛЖНЫ видеть:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   
? TimeOfDaySubsystem initialized. Current time: Year 1, Winter Day 31, 10:00:00

?? Registered schedule component for actor: BP_NPC_1 (Total: 1)

? Schedule System: Time changed to 11:00 (Day 1)
   - Evaluating 1 registered components
```

### ? Что вы видите СЕЙЧАС:

```
? Schedule Subsystem Initialized         (Есть)
? TimeOfDaySubsystem initialized...      (Есть)
? Registered schedule component: ...     (НЕТ!)
? Time changed to XX:XX         (НЕТ!)
```

---

## ?? Анализ проблемы

### ? Что работает:

| Компонент | Статус |
|-----------|--------|
| ScheduleSubsystem | ? Инициализирован |
| TimeOfDaySubsystem | ? Инициализирован |
| **Интеграция времени** | ? **УЖЕ ВСТРОЕНА!** |

### ? Что НЕ работает:

| Проблема | Причина |
|----------|---------|
| Нет регистрации NPC | Нет акторов с `ScheduleComponent` |
| Нет изменения времени | Время не проходит ИЛИ нет NPC для проверки |

---

## ?? Важное уточнение: Интеграция УЖЕ работает!

### Интеграция между TimeOfDay и Schedule **встроена** в код!

В файле `TimeOfDaySubsystem.cpp` (строки 305-330, 259):

```cpp
void UTimeOfDaySubsystem::NotifyScheduleSystem()
{
    if (UWorld* World = GetWorld())
    {
      UScheduleSubsystem* ScheduleSystem = World->GetSubsystem<UScheduleSubsystem>();
        
        if (ScheduleSystem && ScheduleSystem->GetWorld())
        {
            ScheduleSystem->NotifyTimeChanged(CurrentTime.Hour, CurrentTime.Minute, DayOfWeek);
        }
    }
}

// Вызывается автоматически каждую минуту:
if (CurrentTime.Minute != PreviousMinute)
{
    NotifyScheduleSystem(); // ? ЗДЕСЬ!
}
```

**Никаких дополнительных компонентов НЕ НУЖНО!**

---

## ?? Почему нет логов "Time changed"?

### Возможная причина #1: Нет акторов с расписаниями

Если нет зарегистрированных компонентов, лог НЕ выводится.

**Решение:**
1. Создайте NPC Blueprint
2. Добавьте `ScheduleComponent`
3. Создайте `ScheduleData` asset
4. Назначьте его в компонент
5. Разместите NPC на уровне

### Возможная причина #2: Время не меняется

Проверьте `DA_TimeOfDaySettings`:
- `Auto Advance Time` = ? True
- `Time Scale` > 0 (например, 60 для быстрого времени)
- Время не на паузе

---

## ?? Полный чек-лист диагностики

### 1?? Инициализация подсистем

**Ищите в логах:**
```
Schedule Subsystem Initialized
TimeOfDaySubsystem initialized
```

**Ваш статус:** ? Есть

---

### 2?? Регистрация NPC

**Ищите в логах:**
```
?? Registered schedule component for actor: BP_NPC_1
```

**Ваш статус:** ? НЕТ

**Действия:**
- Добавьте `ScheduleComponent` к актору
- Назначьте `ScheduleData`
- Разместите актор на уровне (Drag & Drop в viewport)
- Перезапустите игру

---

### 3?? Изменение времени

**Ищите в логах (каждую минуту игрового времени):**
```
? Schedule System: Time changed to 09:00 (Day 1)
   - Evaluating 1 registered components
```

**Ваш статус:** ? НЕТ (потому что нет зарегистрированных компонентов)

**Действия:**
- Сначала исправьте пункт 2??
- Подождите изменения игрового времени
- Проверьте, что время действительно идет (`Auto Advance Time = true`)

---

## ?? Пошаговая инструкция

### Шаг 1: Создать NPC с расписанием

1. **Content Browser** ? **Blueprint Class** ? выберите **Actor** или **Character**
2. Назовите: `BP_NPC_Shopkeeper`
3. Откройте Blueprint

### Шаг 2: Добавить Schedule Component

1. В **Components** panel ? **Add Component**
2. Найдите **Schedule Component**
3. Добавьте

### Шаг 3: Создать Schedule Data

1. **Content Browser** ? **Right Click** ? **Miscellaneous** ? **Data Asset**
2. Выберите класс: **Schedule Data**
3. Назовите: `DA_Shopkeeper_Schedule`
4. Откройте asset

### Шаг 4: Настроить расписание

В `DA_Shopkeeper_Schedule`:

1. **Schedule Entries** ? **Add Element**
2. Настройте первую задачу:
   - **Start Hour**: 9
   - **Start Minute**: 0
   - **End Hour**: 18
   - **End Minute**: 0
   - **Action**: создайте простой `ScheduleAction` (см. документацию)
   - **Active Days**: выберите дни (например, все будни)

### Шаг 5: Назначить Schedule Data

В `BP_NPC_Shopkeeper`:
1. Выберите **Schedule Component**
2. В **Details** panel найдите **Schedule Data**
3. Установите: `DA_Shopkeeper_Schedule`
4. **Compile** + **Save**

### Шаг 6: Разместить на уровне

1. Drag & Drop `BP_NPC_Shopkeeper` в viewport
2. Разместите где нужно
3. **Save** уровень

### Шаг 7: Проверить

1. **Play (PIE)**
2. Откройте **Output Log**
3. Найдите:

```
?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_0 (Total registered: 1)
```

? **Успех!** NPC зарегистрирован.

### Шаг 8: Дождаться изменения времени

Подождите 1 игровую минуту (в зависимости от `Time Scale`).

**Должны увидеть:**
```
? Schedule System: Time changed to 09:01 (Day 1)
   - Evaluating 1 registered components
```

? **Полный успех!** Интеграция работает!

---

## ?? Критерий успеха

**Если видите ВСЕ эти строки:**

1. ? `Schedule Subsystem Initialized`
2. ? `TimeOfDaySubsystem initialized`
3. ? `Registered schedule component for actor: XXX`
4. ? `Schedule System: Time changed to XX:XX`

**? Система работает полностью!** ??

---

## ? Быстрая проверка: Работает ли интеграция?

### Тест в console (~ тильда):

```cpp
// Получить статистику расписаний
Log LogTemp Log
```

Затем подождите 1 игровую минуту.

**Если видите** `Time changed to XX:XX` ? **Интеграция работает!** ?

---

## ?? Документация

- **CORRECTION_Integration_Already_Exists.md** - Важное уточнение про интеграцию
- **DIAGNOSTICS.md** - Полная диагностика
- **QUICK_CHECK.md** - Быстрая проверка
- **CHECKLIST.md** - Детальный чек-лист
- **ContentGuide.md** - Создание расписаний
- **QUICKSTART.md** - Быстрый старт

---

## ?? Итог

### Ваша текущая проблема:

**Нет акторов с `ScheduleComponent`** ? Нет регистрации ? Нет логов

### Решение:

1. Создать NPC с `ScheduleComponent`
2. Создать и назначить `ScheduleData`
3. Разместить на уровне
4. Проверить логи

**Интеграция уже работает автоматически!** Никаких дополнительных компонентов не нужно!

**Удачи!** ??
