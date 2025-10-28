# ? РЕШЕНИЕ: Почему актор не двигается по расписанию

## ?? Ваша проблема

Вы видите логи:
```
? Registered schedule component for actor: BP_Lianne_C_1
? Schedule System: Time changed to 11:00 (Day 0)
? Evaluating 1 registered components
```

**НО актор BP_Lianne не двигается!**

---

## ?? Исправления применены

Мы исправили **2 критических бага** в системе:

### ? Bug #1: Неправильное получение времени
- **Было:** Компонент получал мусорные значения времени
- **Стало:** Добавлен метод `GetCurrentTime()` для корректного получения

### ? Bug #2: Недостаточно логирования
- **Было:** Невозможно понять почему не работает
- **Стало:** Подробные логи на каждом шаге

---

## ?? Что нужно сделать СЕЙЧАС

### Шаг 1: Перекомпилировать проект ? (УЖЕ СДЕЛАНО)

Проект успешно скомпилирован:
```
Target is up to date
Result: Succeeded
```

---

### Шаг 2: Проверить настройки Schedule Entry

#### Откройте ваш Schedule Entry Data Asset

Например: `DA_Entry_GoToShop` или как вы его назвали.

#### Проверьте Time Condition:

**Если стоит TimeOfDay:**

```
Time Condition:
  Time Type: TimeOfDay        ? ? ПРОБЛЕМА!
  Start Hour: 11
  Start Minute: 0
```

**ИЗМЕНИТЕ НА TimeRange:**

```
Time Condition:
  Time Type: TimeRange        ? ? РЕШЕНИЕ!
  Start Hour: 11
  Start Minute: 0
  End Hour: 18      ? ОБЯЗАТЕЛЬНО добавьте!
  End Minute: 0
```

#### Сохраните изменения

---

### Шаг 3: Проверить Action в Entry

В том же Schedule Entry:

1. **Action** ? должен быть назначен
2. Если используете `Move To Location`:
   - **Location Selector** ? настроен? (например, `Actor Tag Selector`)
   - **Tags** ? указаны? (например, `Location.Shop`)
   - **Acceptance Radius** ? 100.0 (или больше)

---

### Шаг 4: Проверить NPC

Откройте **BP_Lianne** Blueprint:

1. **Components** ? есть `Schedule Component`? ?
2. **Schedule Data** ? назначен ваш ScheduleData? ?

**Class Defaults:**
3. **Auto Possess AI** ? `Placed in World or Spawned` ?
4. **AIController Class** ? `AIController` (минимум) ?

---

### Шаг 5: Проверить NavMesh

1. Откройте ваш уровень
2. Нажмите **P** (показать навигацию)
3. Должна быть **зеленая зона** вокруг NPC и целевой точки

**Если нет:**
- **Place Actors** ? **Volumes** ? `Nav Mesh Bounds Volume`
- Растяните чтобы покрыть всю зону движения
- Нажмите **P** снова - должна появиться зеленая зона

---

### Шаг 6: Включить подробные логи

**Запустите игру (Play)**

**Откройте консоль (~)** и введите:
```
Log LogTemp Verbose
```

---

### Шаг 7: Проверить логи

**Output Log** должен показать:

```
? Schedule System: Time changed to 11:00 (Day 0)
   - Evaluating 1 registered components
   Evaluating schedule for BP_Lianne_C_1 at 11:00 (Day 0)
   Checking X entries for BP_Lianne_C_1
     Entry 'Go to Shop' (Priority 50): MATCH
     TimeRange: 11:00 (660) in [11:00 (660) - 18:00 (1080)] ? YES
?? Starting action 'Go to Shop' for BP_Lianne_C_1 (Priority: 50)
   ?? Selected location: X=1200.0 Y=300.0 Z=100.0
   ? Action started successfully
```

**Если видите это** ? ? **РАБОТАЕТ!**

---

## ?? Troubleshooting

### Вижу "No matching entry found"

```
?? No matching entry found for BP_Lianne_C_1 at 11:00
```

**Причины:**

1. **Time Type = TimeOfDay** (вместо TimeRange)
   - Решение: Измените на `TimeRange` как описано выше

2. **Неправильные Start/End Hour**
   - Решение: Проверьте, что текущее время входит в диапазон

3. **DaysOfWeek не включает текущий день**
   - Решение: В Schedule Entry ? Days Of Week ? оставьте пустым (все дни) ИЛИ добавьте `0`

---

### Вижу "Entry has no Action assigned"

```
?? Entry 'XXX' has no Action assigned!
```

**Решение:**

1. Откройте Schedule Entry
2. **Action** ? создайте или назначьте Action
3. Сохраните

---

### Вижу "Failed to select location"

```
?? Failed to select location for schedule entry: XXX
```

**Причины:**

1. **Location Selector не настроен**
   - Решение: В Entry ? Location Selector ? создайте `Actor Tag Selector`

2. **Не найден актор с нужным тегом**
   - Решение: 
 - Создайте Target Point
     - Добавьте тег (например, `Location.Shop`)
     - Разместите на уровне

3. **Актор слишком далеко**
   - Решение: В Location Selector ? Max Distance ? увеличьте ИЛИ поставьте 0 (без ограничений)

---

### Актор не двигается, хотя логи говорят "Action started"

**Причины:**

1. **Нет AIController**
   - Решение: В NPC Blueprint ? Class Defaults ? Auto Possess AI = `Placed in World`

2. **Нет NavMesh**
   - Решение: Добавьте `Nav Mesh Bounds Volume` (см. Шаг 5)

3. **Целевая точка недостижима**
   - Решение: Убедитесь что точка на зеленой зоне NavMesh

---

## ? Финальный чек-лист

Пройдите по списку и убедитесь:

- [ ] Проект перекомпилирован (уже сделано ?)
- [ ] Schedule Entry использует `TimeRange` (не `TimeOfDay`)
- [ ] End Hour и End Minute установлены
- [ ] Action назначен в Entry
- [ ] Location Selector настроен
- [ ] Target Point с тегом существует на уровне
- [ ] NPC имеет Schedule Component
- [ ] NPC имеет AIController
- [ ] NavMesh покрывает зону движения
- [ ] Логи Verbose включены
- [ ] Логи показывают "Action started successfully"

**Если все ?** ? Актор ДОЛЖЕН двигаться!

---

## ?? Важная информация

### TimeOfDay vs TimeRange

| Тип | Поведение | Когда использовать |
|-----|-----------|-------------------|
| **TimeOfDay** | Срабатывает **ТОЛЬКО** в точное время (11:00:00) | Мгновенные события: проснуться, событие |
| **TimeRange** | Активен весь диапазон (11:00 - 18:00) | **Длительные действия: движение, работа, патруль** |

**Для движения актора ВСЕГДА используйте TimeRange!**

---

## ?? Дополнительная документация

- **BUGFIX_Actor_Not_Moving.md** - Технические детали багов
- **QUICK_FIX_Not_Moving.md** - Супер-быстрое решение
- **QUICKSTART.md** - Полное руководство по системе

---

## ?? Итог

1. ? Баги исправлены в коде
2. ? Проект скомпилирован
3. ?? Вам нужно изменить **Time Type** на **TimeRange**
4. ?? Проверить остальные настройки по чек-листу
5. ? Включить логи Verbose
6. ? Тестировать!

**После выполнения всех шагов актор будет двигаться!** ??

**Удачи!** ??
