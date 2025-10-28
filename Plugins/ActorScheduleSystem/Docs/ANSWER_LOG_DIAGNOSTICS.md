# Ответ: Как проверить работу системы расписаний

## ?? Анализ ваших логов

Из предоставленных логов видно:

```
[2025.10.27-14.13.47:222][287]LogTemp: Schedule Subsystem Initialized
[2025.10.27-14.13.47:222][287]LogTemp: TimeOfDaySubsystem initialized. Current time: Year 1, Winter Day 31, 06:00:00
[2025.10.27-14.13.47:337][287]LogTemp: TimeOfDaySubsystem initialized with settings. Time: Year 1, Winter Day 31, 10:00:00
```

### ? Что работает:

1. **ScheduleSubsystem** - инициализирован ?
2. **TimeOfDaySubsystem** - инициализирован ?

### ? Что НЕ работает:

1. **Нет интеграции между системами** ?
2. **Нет регистрации компонентов расписания** ?
3. **Нет уведомлений об изменении времени** ?

---

## ?? Проблема: Системы не связаны!

TimeOfDaySystem работает, ScheduleSystem работает, но **они не общаются друг с другом**.

### Почему задачи не выполняются?

ScheduleSystem **не знает**, что время изменилось, потому что:
- TimeOfDay меняет время: `06:00` ? `10:00`
- ScheduleSystem **не получает** это уведомление
- Расписания **не проверяются**
- Задачи **не выполняются**

---

## ? Решение: Добавить компонент интеграции

### Шаг 1: Открыть GameState Blueprint

1. В Content Browser найдите ваш GameState Blueprint
2. Если его нет - создайте новый: `Blueprint Class` ? `GameStateBase`

### Шаг 2: Добавить компонент

1. Откройте GameState Blueprint
2. Click **Add Component**
3. Найдите `TimeOfDayScheduleIntegration`
4. Добавьте его
5. **Compile** и **Save**

### Шаг 3: Установить GameState в GameMode

Если создали новый GameState:
1. Откройте ваш GameMode Blueprint
2. В настройках найдите `Game State Class`
3. Установите ваш новый GameState
4. Compile и Save

### Шаг 4: Перезапустить игру

После перезапуска вы должны увидеть:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   - Ready to register schedule components

? TimeOfDayScheduleIntegration: Successfully connected!
   - Schedule system will be notified on every hour change
   - Initial time: 10:00 (Weekday: 1)
```

---

## ?? Что вы должны видеть после исправления

### При запуске игры:

```
[LogTemp] ? Schedule Subsystem Initialized
[LogTemp]    - Initial time: 08:00 (Day 1)
[LogTemp]    - Ready to register schedule components

[LogTemp] ? TimeOfDayScheduleIntegration: Successfully connected!
[LogTemp]    - Schedule system will be notified on every hour change
[LogTemp]    - Initial time: 10:00 (Weekday: 1)
```

### Когда NPC появляется в мире:

```
[LogTemp] ?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_1 (Total registered: 1)
```

### Каждый игровой час:

```
[LogTemp] ? Schedule System: Time changed to 11:00 (Day 1)
[LogTemp]    - Evaluating 1 registered components
```

### При выполнении задачи (с Verbose логами):

```
[LogTemp]    ? Evaluated 1 schedule components
[LogTemp] ?? Schedule: Actor 'BP_NPC_Shopkeeper_C_1' - Starting action 'Open Shop' (09:00-18:00)
```

---

## ?? Проверка: Работает ли интеграция?

### Тест 1: Проверка логов

Запустите игру и найдите в Output Log:
- `TimeOfDayScheduleIntegration: Successfully connected!`

**Есть?** ? ? Интеграция работает  
**Нет?** ? ? Компонент не добавлен или не активен

### Тест 2: Изменение времени

Подождите 1 игровой час и найдите:
- `Schedule System: Time changed to XX:XX`

**Есть?** ? ? Время передается в систему расписаний  
**Нет?** ? ? Интеграция не работает

### Тест 3: Регистрация NPC

Разместите актор с `ScheduleComponent` на уровне, найдите:
- `Registered schedule component for actor: XXX`

**Есть?** ? ? Компоненты регистрируются  
**Нет?** ? ? Компонент не настроен или не активен

---

## ?? Дополнительные проверки

### Включить подробные логи

В консоли игры (тильда `~`):
```
Log LogTemp Verbose
```

### Получить статистику в runtime

В Blueprint или C++:
```cpp
UScheduleSubsystem* Subsystem = GetWorld()->GetSubsystem<UScheduleSubsystem>();

int32 TotalActors, ActiveActions, PausedActions;
Subsystem->GetScheduleStats(TotalActors, ActiveActions, PausedActions);

UE_LOG(LogTemp, Log, TEXT("?? Total actors: %d, Active: %d, Paused: %d"), 
    TotalActors, ActiveActions, PausedActions);
```

### Проверить, зарегистрирован ли конкретный NPC

```cpp
// В C++ или через Blueprint
if (ScheduleComponent && ScheduleComponent->IsRegisteredWithSubsystem())
{
    UE_LOG(LogTemp, Log, TEXT("? Component is registered"));
}
else
{
    UE_LOG(LogTemp, Warning, TEXT("? Component is NOT registered!"));
}
```

---

## ?? Документация

Для более детальной информации:

- **QUICK_CHECK.md** - Быстрая проверка (1 минута)
- **DIAGNOSTICS.md** - Полная диагностика (все кейсы)
- **TIME_INTEGRATION.md** - Настройка интеграции времени
- **QUICKSTART.md** - Быстрый старт с примерами
- **ContentGuide.md** - Создание расписаний

---

## ?? Итоговый чек-лист

После добавления `TimeOfDayScheduleIntegration` в GameState:

- [ ] Вижу "Schedule Subsystem Initialized"
- [ ] Вижу "TimeOfDayScheduleIntegration: Successfully connected!"
- [ ] Вижу "Registered schedule component for actor: XXX"
- [ ] Вижу "Schedule System: Time changed to XX:XX" (каждый час)

**Если все 4 пункта ?** ? **Система работает полностью!** ??

---

## ?? Важно!

**Самая частая ошибка:** Забыли добавить `TimeOfDayScheduleIntegration` в GameState.

**Без этого компонента:**
- TimeOfDay работает ?
- ScheduleSystem работает ?
- Но они **не связаны** ?
- Задачи **не выполняются** ?

**С этим компонентом:**
- TimeOfDay работает ?
- ScheduleSystem работает ?
- Интеграция работает ?
- Задачи выполняются ?

---

## ?? Следующие шаги

1. ? Добавить `TimeOfDayScheduleIntegration` в GameState
2. ? Перезапустить игру
3. ? Проверить логи (см. раздел "Что вы должны видеть")
4. ? Создать NPC с `ScheduleComponent`
5. ? Настроить `ScheduleData` с расписанием
6. ? Дождаться изменения времени
7. ? Проверить выполнение задач

**Готово!** Система полностью функциональна. ??
