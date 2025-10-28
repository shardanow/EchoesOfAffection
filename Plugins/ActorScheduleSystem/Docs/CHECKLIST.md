# ? Чек-лист проверки системы расписаний

## ?? Используйте этот чек-лист для быстрой диагностики

---

## 1?? Инициализация подсистемы

### Что искать в логах:
```
LogTemp: Schedule Subsystem Initialized
```

### ? Если видите:
- [x] Подсистема запущена
- [x] Система готова к работе

### ? Если НЕ видите:
- [ ] Плагин не включен в `.uproject`
- [ ] Ошибка при инициализации (проверьте другие error логи)

**Действия:**
1. Откройте `YourProject.uproject`
2. Убедитесь, что плагин `ActorScheduleSystem` включен
3. Перегенерируйте project files
4. Перекомпилируйте проект

---

## 2?? Интеграция с TimeOfDay

### Что искать в логах:
```
LogTemp: ? TimeOfDayScheduleIntegration: Successfully connected!
LogTemp:    - Schedule system will be notified on every hour change
LogTemp:    - Initial time: 10:00 (Weekday: 1)
```

### ? Если видите:
- [x] Интеграция работает
- [x] Время будет передаваться автоматически

### ? Если НЕ видите:
- [ ] Компонент `TimeOfDayScheduleIntegration` не добавлен в GameState
- [ ] GameState не установлен в GameMode
- [ ] Компонент не активен

**Действия:**
1. Откройте GameState Blueprint
2. Add Component ? `TimeOfDayScheduleIntegration`
3. Compile + Save
4. Откройте GameMode Blueprint
5. Установите `Game State Class` = ваш GameState
6. Compile + Save
7. Перезапустите игру

---

## 3?? Регистрация компонентов

### Что искать в логах:
```
LogTemp: ?? Registered schedule component for actor: BP_NPC_Shopkeeper_C_1 (Total registered: 1)
```

### ? Если видите:
- [x] NPC зарегистрирован в системе
- [x] Расписание будет проверяться

### ? Если НЕ видите:
- [ ] У актора нет `ScheduleComponent`
- [ ] `ScheduleData` не назначен
- [ ] Актор не размещен на уровне (только в Content Browser)
- [ ] Компонент не активен (`bAutoActivate = false`)

**Действия:**
1. Откройте ваш NPC Blueprint
2. Добавьте `ScheduleComponent` (если нет)
3. В Details: установите `Schedule Data` = ваш Data Asset
4. Убедитесь, что `Auto Activate = true`
5. Compile + Save
6. Разместите актор на уровне (Drag & Drop в viewport)
7. Перезапустите игру

---

## 4?? Изменение времени

### Что искать в логах (каждый игровой час):
```
LogTemp: ? Schedule System: Time changed to 09:00 (Day 1)
LogTemp: - Evaluating 3 registered components
```

### ? Если видите:
- [x] Время передается в систему расписаний
- [x] Компоненты проверяются
- [x] Задачи выполняются (если время совпадает)

### ? Если НЕ видите:
- [ ] **Интеграция времени НЕ настроена!**
- [ ] См. пункт 2?? выше

**Действия:**
1. Вернитесь к пункту 2??
2. Добавьте `TimeOfDayScheduleIntegration` в GameState
3. **Это критично для работы системы!**

---

## 5?? Выполнение задач (Verbose logs)

### Включить подробные логи:
Консоль игры (~ тильда):
```
Log LogTemp Verbose
```

### Что искать в логах:
```
LogTemp:    ? Evaluated 3 schedule components
LogTemp: ?? Actor 'BP_NPC_1' - Starting action 'Work' (09:00-18:00)
```

### ? Если видите:
- [x] Задачи выполняются
- [x] Система полностью функциональна

### ? Если НЕ видите:
- [ ] Расписание не настроено (пустой `ScheduleData`)
- [ ] Время не совпадает с расписанием
- [ ] Action не реализован или пустой

**Действия:**
1. Откройте ваш `ScheduleData` asset
2. Убедитесь, что есть записи в `Schedule Entries`
3. Проверьте время начала/конца задачи
4. Проверьте, что текущее игровое время попадает в этот интервал
5. Проверьте, что `Action` назначен и настроен

---

## ?? Итоговая таблица: Статус систем

| # | Что проверяем | Ожидаемый лог | Статус |
|---|---------------|---------------|--------|
| 1 | Инициализация | `Schedule Subsystem Initialized` | ? |
| 2 | Интеграция | `TimeOfDayScheduleIntegration: Successfully connected!` | ? |
| 3 | Регистрация | `Registered schedule component for actor: XXX` | ? |
| 4 | Время | `Schedule System: Time changed to XX:XX` | ? |
| 5 | Задачи | `? Evaluated N schedule components` | ? |

### Ваша оценка:

- [ ] **0/5** - Система не работает вообще
- [ ] **1/5** - Только инициализация (критические проблемы)
- [ ] **2/5** - Интеграция не настроена
- [ ] **3/5** - Нет NPC с расписаниями
- [ ] **4/5** - Почти работает, проверьте настройки расписания
- [ ] **5/5** - ? **Все работает!** ??

---

## ?? Минимум для работы: Пункты 1-2-3-4

Если есть **ВСЕ** эти 4 лога:

1. ? `Schedule Subsystem Initialized`
2. ? `TimeOfDayScheduleIntegration: Successfully connected!`
3. ? `Registered schedule component for actor: XXX`
4. ? `Schedule System: Time changed to XX:XX`

**? Система настроена правильно!**

Пункт 5 (выполнение задач) зависит от:
- Настроек расписания (`ScheduleData`)
- Текущего игрового времени
- Реализации `Action`

---

## ?? Команды для отладки

```
// Включить подробные логи (для пункта 5)
Log LogTemp Verbose

// Очень подробные логи (все события)
Log LogTemp VeryVerbose

// Вернуть обычные логи
Log LogTemp Log

// Вывести статистику в runtime (Blueprint/C++)
UScheduleSubsystem* Subsystem = GetWorld()->GetSubsystem<UScheduleSubsystem>();
int32 Total, Active, Paused;
Subsystem->GetScheduleStats(Total, Active, Paused);
PrintString(FString::Printf(TEXT("Stats: %d total, %d active, %d paused"), Total, Active, Paused));
```

---

## ?? Следующие шаги

После того, как все 5 пунктов ?:

1. Создайте `ScheduleData` assets для разных типов NPC
2. Настройте расписания (время, действия, локации)
3. Добавьте `ScheduleComponent` к вашим NPC
4. Назначьте соответствующий `ScheduleData`
5. Реализуйте `ScheduleAction` классы для ваших задач
6. Тестируйте и итерируйте!

---

## ?? Документация

- **SUMMARY.md** - Краткий ответ на ваш вопрос
- **ANSWER_LOG_DIAGNOSTICS.md** - Детальный анализ логов
- **QUICK_CHECK.md** - Быстрая проверка (1 минута)
- **DIAGNOSTICS.md** - Полная диагностика
- **TIME_INTEGRATION.md** - Настройка интеграции времени
- **QUICKSTART.md** - Быстрый старт
- **ContentGuide.md** - Создание расписаний

---

## ? Самые частые ошибки

### ?? #1: Нет интеграции времени (90% проблем)

**Симптомы:**
- Есть: `Schedule Subsystem Initialized`
- Нет: `TimeOfDayScheduleIntegration: Successfully connected!`
- Нет: `Time changed to XX:XX`

**Решение:**
- GameState Blueprint ? Add Component ? `TimeOfDayScheduleIntegration`

---

### ?? #2: Компонент не зарегистрирован

**Симптомы:**
- Актор в мире
- Есть `ScheduleComponent`
- Нет: `Registered schedule component for actor: XXX`

**Решение:**
- Убедитесь, что `Schedule Data` назначен
- Убедитесь, что `Auto Activate = true`
- Проверьте BeginPlay логи на ошибки

---

### ?? #3: Задачи не выполняются

**Симптомы:**
- Все логи есть (1-4)
- Время проходит
- Задачи не выполняются

**Решение:**
- Проверьте расписание: совпадает ли время?
- Включите `Log LogTemp Verbose`
- Проверьте, реализован ли `Action`

---

## ?? Поддержка

Если после всех проверок что-то не работает:

1. Включите `Log LogTemp VeryVerbose`
2. Перезапустите игру
3. Скопируйте логи от старта до первого изменения времени
4. Проверьте все пункты этого чек-листа
5. Смотрите документацию (DIAGNOSTICS.md)

**Удачи!** ??
