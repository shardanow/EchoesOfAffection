# Быстрая проверка: Работает ли система расписаний?

## ?? Что искать в логах

### 1. При запуске игры (PIE)

Ищите эти строки в **Output Log**:

```
? Schedule Subsystem Initialized
   - Initial time: 08:00 (Day 1)
   - Ready to register schedule components
```

**Статус:** ? Подсистема расписаний запущена

---

### 2. Интеграция с TimeOfDay

Если вы добавили `TimeOfDayScheduleIntegration` в GameState:

```
? TimeOfDayScheduleIntegration: Successfully connected!
   - Schedule system will be notified on every hour change
   - Initial time: 10:00 (Weekday: 1)
```

**Статус:** ? Интеграция времени работает

? **Если этого нет:**
- Откройте ваш **GameState Blueprint**
- Добавьте компонент `TimeOfDayScheduleIntegration`
- Сохраните и перезапустите

---

### 3. Регистрация NPC

Когда NPC с `ScheduleComponent` появляется в мире:

```
?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_1 (Total registered: 1)
```

**Статус:** ? NPC зарегистрирован

? **Если этого нет:**
- Проверьте, что у актора есть `ScheduleComponent`
- Проверьте, что `ScheduleData` назначен в компоненте
- Проверьте, что актор размещен на уровне (не только в Content Browser)

---

### 4. Изменение времени

Каждый игровой час вы должны видеть:

```
? Schedule System: Time changed to 09:00 (Day 1)
   - Evaluating 3 registered components
```

**Статус:** ? Система времени работает, расписания проверяются

? **Если этого нет:**
- **Интеграция времени НЕ настроена!**
- Добавьте `TimeOfDayScheduleIntegration` в GameState (см. пункт 2)

---

## ?? Быстрое исправление: Нет интеграции времени

### Вариант 1: Автоматическая интеграция (рекомендуется)

1. Откройте **GameState Blueprint** (или создайте новый)
2. **Add Component** ? `TimeOfDayScheduleIntegration`
3. **Compile** и **Save**
4. Перезапустите игру

**Готово!** Теперь должны появиться все логи из пунктов 2 и 4.

---

### Вариант 2: Ручная интеграция (если нет GameState)

Добавьте в любой persistent actor (например, GameMode):

**C++:**
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
UTimeOfDayScheduleIntegration* ScheduleIntegration;
```

**Blueprint:**
- Add Component ? `TimeOfDayScheduleIntegration`
- Готово!

---

## ?? Проверка статистики

### В Blueprint или C++:

```cpp
// Получить статистику
int32 TotalActors, ActiveActions, PausedActions;
UScheduleSubsystem* Subsystem = GetWorld()->GetSubsystem<UScheduleSubsystem>();
Subsystem->GetScheduleStats(TotalActors, ActiveActions, PausedActions);

// Вывести в лог
UE_LOG(LogTemp, Log, TEXT("?? Schedule Stats:"));
UE_LOG(LogTemp, Log, TEXT("   Total registered actors: %d"), TotalActors);
UE_LOG(LogTemp, Log, TEXT("   Active actions: %d"), ActiveActions);
UE_LOG(LogTemp, Log, TEXT("   Paused actions: %d"), PausedActions);
```

---

## ? Чек-лист: "Все работает!"

Если в логах есть **ВСЕ** эти строки:

- ? `Schedule Subsystem Initialized`
- ? `TimeOfDayScheduleIntegration: Successfully connected!`
- ? `Registered schedule component for actor: XXX`
- ? `Schedule System: Time changed to XX:XX`

**? Система работает правильно! ??**

---

## ? Чек-лист: "Что-то не так!"

| Проблема | Что проверить |
|----------|---------------|
| Нет инициализации | Плагин включен в `.uproject`? |
| Нет интеграции времени | Добавили `TimeOfDayScheduleIntegration` в GameState? |
| Нет регистрации NPC | Есть `ScheduleComponent`? Есть `ScheduleData`? Актор в мире? |
| Нет изменения времени | Интеграция времени настроена? (см. пункт 4) |

---

## ?? Команды консоли

Для более подробных логов, в консоли игры введите:

```
Log LogTemp Verbose
```

Для очень подробных логов:

```
Log LogTemp VeryVerbose
```

Вернуть обычные логи:

```
Log LogTemp Log
```

---

## ?? Дополнительная информация

- **Полная диагностика:** `DIAGNOSTICS.md`
- **Настройка интеграции:** `TIME_INTEGRATION.md`
- **Создание расписаний:** `ContentGuide.md`
- **Быстрый старт:** `QUICKSTART.md`

---

## ?? Самая частая ошибка

**90% проблем:** Забыли добавить `TimeOfDayScheduleIntegration` в GameState!

**Решение:** 
1. GameState Blueprint
2. Add Component ? `TimeOfDayScheduleIntegration`
3. Compile + Save
4. Profit! ??
