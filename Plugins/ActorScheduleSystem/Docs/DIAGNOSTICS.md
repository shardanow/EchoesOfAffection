# Диагностика системы расписаний

## Как проверить, что система работает правильно

### 1. Проверка логов при запуске (? Обязательно)

При запуске игры вы должны видеть следующие сообщения в логах:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   - Ready to register schedule components
```

#### Что означает:
- ? Подсистема расписаний запущена
- ? Начальное время установлено (по умолчанию 08:00, понедельник)
- ? Система готова к регистрации компонентов

---

### 2. Проверка регистрации компонентов

Когда актор со `ScheduleComponent` появляется в мире, вы должны видеть:

```
?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_1 (Total registered: 1)
```

#### Что означает:
- ? NPC зарегистрирован в системе расписаний
- ? Для него будет выполняться расписание
- ? Видно общее количество зарегистрированных акторов

#### ? Если этого НЕ видно:
1. Проверьте, что у актора есть `ScheduleComponent`
2. Проверьте, что `ScheduleData` установлен в компоненте
3. Проверьте, что актор в мире (не только в Blueprint)

---

### 3. Проверка интеграции времени

#### Вариант A: Автоматическая интеграция (рекомендуется)

Добавьте `UTimeOfDayScheduleIntegration` в ваш GameState:

```cpp
// В GameState Blueprint или C++
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Systems")
UTimeOfDayScheduleIntegration* ScheduleIntegration;
```

**Ожидаемые логи:**
```
? TimeOfDayScheduleIntegration: Successfully connected!
   - Schedule system will be notified on every hour change
   - Initial time: 10:00 (Weekday: 1)
```

#### Вариант B: Ручная интеграция

Если используете `ScheduleTimeIntegrationComponent`:

```
Schedule Time Integration: Subscribed to time events (implement binding in your project)
```

**?? ВАЖНО:** Этот компонент требует ручной привязки событий!

---

### 4. Проверка изменения времени

Когда время меняется (каждый час), вы должны видеть:

```
? Schedule System: Time changed to 09:00 (Day 1)
   - Evaluating 3 registered components
   ? Evaluated 3 schedule components
```

#### Что означает:
- ? Система времени передала новое время в систему расписаний
- ? Все зарегистрированные компоненты были проверены
- ? Расписания оценены и выполнены (если есть совпадения)

#### ? Если этого НЕ видно:
1. **Интеграция времени НЕ настроена!**
2. Используйте `UTimeOfDayScheduleIntegration` (см. раздел 3)
3. Или вручную вызывайте `NotifyTimeChanged` из вашей системы времени

---

### 5. Проверка выполнения задач

Чтобы увидеть выполнение задач, включите Verbose логирование:

```cpp
// В DefaultEngine.ini или через команду консоли
LogTemp=Verbose

// Или через консоль в игре:
Log LogTemp Verbose
```

**Ожидаемые логи при выполнении задачи:**
```
?? Schedule: Actor 'BP_NPC_Shopkeeper_C_1' - Starting action 'Open Shop' (09:00-18:00)
?? Schedule: Moving to location (1200, 300, 100)
?? Schedule: Playing animation 'OpenShop_Anim'
```

*(Точные логи зависят от вашей реализации `ScheduleAction`)*

---

## Быстрая диагностика: Чек-лист

### ? Минимально рабочая система

Убедитесь, что видите:

1. ? `Schedule Subsystem Initialized`
2. ? `TimeOfDayScheduleIntegration: Successfully connected!`
3. ? `Registered schedule component for actor: XXX`
4. ? `Schedule System: Time changed to XX:XX`

Если **все 4 пункта** есть в логах ? **Система работает!**

---

### ? Что-то не работает?

| Проблема | Решение |
|----------|---------|
| Нет инициализации подсистемы | Проверьте, что плагин включен в `.uproject` |
| Нет регистрации компонентов | Добавьте `ScheduleComponent` к актору, установите `ScheduleData` |
| Нет изменения времени | **Добавьте `UTimeOfDayScheduleIntegration` в GameState!** |
| Нет выполнения задач | Проверьте расписание в `ScheduleData`, включите Verbose логи |

---

## Пошаговая установка интеграции

### Шаг 1: Добавить компонент интеграции в GameState

1. Откройте ваш GameState Blueprint (или создайте новый)
2. Добавьте компонент `TimeOfDayScheduleIntegration`
3. Сохраните Blueprint

**Готово!** Интеграция настроена автоматически.

### Шаг 2: Проверить подключение

Запустите игру и найдите в логах:
```
? TimeOfDayScheduleIntegration: Successfully connected!
```

### Шаг 3: Добавить NPC с расписанием

1. Создайте актор (например, NPC)
2. Добавьте `ScheduleComponent`
3. Создайте `ScheduleData` asset
4. Назначьте его в `ScheduleComponent -> ScheduleData`
5. Настройте расписание в Data Asset

### Шаг 4: Проверить регистрацию

Запустите игру, найдите в логах:
```
?? Registered schedule component for actor: BP_YourNPC_C_1 (Total registered: 1)
```

### Шаг 5: Дождаться изменения времени

Когда игровое время изменится (каждый час), вы увидите:
```
? Schedule System: Time changed to 10:00 (Day 1)
   - Evaluating 1 registered components
```

---

## Команды консоли для отладки

```cpp
// Включить подробные логи
Log LogTemp Verbose

// Выключить подробные логи (только важные сообщения)
Log LogTemp Log

// Показать все логи
Log LogTemp VeryVerbose
```

---

## Метрики успешной интеграции

### Что должно происходить каждый игровой час:

1. ? TimeOfDay ? отправляет событие OnHourChanged
2. ?? Integration ? перехватывает событие
3. ?? Integration ? вызывает `ScheduleSubsystem->NotifyTimeChanged()`
4. ?? ScheduleSubsystem ? оценивает все зарегистрированные компоненты
5. ?? ScheduleComponent ? проверяет свое расписание
6. ? ScheduleAction ? выполняется, если время совпадает

**Все это автоматически логируется!**

---

## Дополнительные проверки

### Проверить количество зарегистрированных акторов

```cpp
// В Blueprint или C++
UScheduleSubsystem* Subsystem = GetWorld()->GetSubsystem<UScheduleSubsystem>();
int32 TotalActors, ActiveActions, PausedActions;
Subsystem->GetScheduleStats(TotalActors, ActiveActions, PausedActions);

UE_LOG(LogTemp, Log, TEXT("?? Schedule Stats: Total=%d, Active=%d, Paused=%d"), 
    TotalActors, ActiveActions, PausedActions);
```

### Найти всех NPC с определенными тегами

```cpp
FGameplayTagContainer SearchTags;
SearchTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Schedule.NPC.Shopkeeper")));

TArray<AActor*> Shopkeepers = Subsystem->GetActorsWithScheduleTags(SearchTags, false);

UE_LOG(LogTemp, Log, TEXT("Found %d shopkeepers with schedules"), Shopkeepers.Num());
```

---

## Troubleshooting: Частые проблемы

### 1. "Schedule Subsystem Initialized" есть, но нет регистрации компонентов

**Причина:** NPC не появляются в мире или компонент не активен

**Решение:**
- Убедитесь, что актор размещен в уровне (не только в Content Browser)
- Проверьте `bAutoActivate = true` в компоненте
- Проверьте, что у актора не отключен `BeginPlay`

### 2. Нет логов "Time changed"

**Причина:** Интеграция времени не настроена

**Решение:**
- Добавьте `UTimeOfDayScheduleIntegration` в GameState
- Или вручную вызывайте `NotifyTimeChanged` из вашей системы времени

### 3. Время меняется, но задачи не выполняются

**Причина:** Расписание не настроено или время не совпадает

**Решение:**
- Проверьте `ScheduleData` ? есть ли там записи?
- Проверьте время начала задачи (должно совпадать с текущим временем)
- Включите `Log LogTemp Verbose` для детальных логов

### 4. Задачи выполняются многократно

**Причина:** Нет проверки на уже активную задачу

**Решение:**
- Это ожидаемое поведение для длительных задач
- Используйте `InterruptCurrentAction()` если нужно прервать

---

## Итоговая проверка: "Все работает!"

? **Минимальный набор логов при запуске:**

```
[LogTemp] ? Schedule Subsystem Initialized
[LogTemp]    - Initial time: 08:00 (Day 1)
[LogTemp] ? TimeOfDayScheduleIntegration: Successfully connected!
[LogTemp] ?? Registered schedule component for actor: BP_NPC_1 (Total registered: 1)
```

? **При изменении времени (каждый час):**

```
[LogTemp] ? Schedule System: Time changed to 09:00 (Day 1)
[LogTemp]    - Evaluating 1 registered components
```

? **Если все это есть ? Система работает правильно!** ??

---

## Дополнительные ресурсы

- `TIME_INTEGRATION.md` - Детальное описание интеграции
- `QUICKSTART.md` - Быстрый старт с примерами
- `ContentGuide.md` - Создание расписаний через Data Assets
- `BUGFIX_Tickable_Initialization.md` - Решенные проблемы

---

## Контакты

Если система не работает после всех проверок:

1. Включите `Log LogTemp VeryVerbose`
2. Скопируйте все логи, начиная с "Schedule Subsystem Initialized"
3. Проверьте, что `UTimeOfDayScheduleIntegration` добавлен в GameState
4. Убедитесь, что `ScheduleData` настроен и назначен компоненту

**Самая частая ошибка:** Забыли добавить `UTimeOfDayScheduleIntegration` в GameState! ??
