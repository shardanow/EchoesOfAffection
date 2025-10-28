# ?? Быстрое решение: Актор не двигается

## ? Проблема

Вы видите в логах:
```
? Registered schedule component
? Time changed to 11:00
? Evaluating 1 registered components
? НО актор не двигается!
```

---

## ? Быстрое решение (30 секунд)

### 1. Проверьте тип времени в Schedule Entry

Откройте ваш `Schedule Entry Data Asset` (например, `DA_Entry_GoToShop`).

#### ? Если стоит **TimeOfDay:**

```
Time Type: TimeOfDay        ? ПРОБЛЕМА!
Start Hour: 11
Start Minute: 0
```

**Измените на:**

```
Time Type: TimeRange        ? РЕШЕНИЕ!
Start Hour: 11
Start Minute: 0
End Hour: 18     ? Добавьте конец
End Minute: 0
```

**Почему?**
- `TimeOfDay` работает ТОЛЬКО в точное время (11:00:00)
- В 11:01 он уже НЕ сработает
- Для длительных действий используйте `TimeRange`

---

### 2. Включите подробные логи

В консоли игры (~ тильда):
```
Log LogTemp Verbose
```

Перезапустите игру и проверьте логи.

---

## ?? Что вы должны увидеть

### После исправления:

```
? Schedule System: Time changed to 11:00 (Day 0)
   Evaluating schedule for BP_Lianne_C_1 at 11:00
   Checking 1 entries
     Entry 'Go to Shop' (Priority 50): MATCH
       TimeRange: 11:00 in [11:00 - 18:00] ? YES
?? Starting action 'Go to Shop' for BP_Lianne_C_1
   ?? Selected location: X=1200.0 Y=300.0 Z=100.0
   ? Action started successfully
```

---

## ?? Дополнительная диагностика

### Если все еще не работает, проверьте:

#### 1. Есть ли Action в Entry?

Откройте `Schedule Entry Data Asset`:
- **Action** ? должен быть назначен (например, `Move To Location`)
- Если пусто ? создайте и назначьте Action

#### 2. Настроен ли Action правильно?

Если используете `Move To Location`:
- **Location Selector** ? настроен?
- **Target Location** ? указан?
- **Acceptance Radius** ? > 0

#### 3. Есть ли AIController у NPC?

Откройте NPC Blueprint ? **Class Defaults**:
- **Auto Possess AI** ? `Placed in World or Spawned`
- **AIController Class** ? `AIController` (минимум)

#### 4. Есть ли NavMesh?

В viewport нажмите **P**:
- Должна быть **зеленая зона** вокруг NPC и целевой точки
- Если нет ? добавьте `Nav Mesh Bounds Volume`

---

## ?? Чек-лист быстрой проверки

- [ ] Time Type = **TimeRange** (не TimeOfDay)
- [ ] Start/End Hour правильные
- [ ] **Action** назначен в Entry
- [ ] **Location Selector** или **Target Location** настроен
- [ ] NPC имеет **AIController**
- [ ] **NavMesh** покрывает зону движения
- [ ] Логи `Verbose` включены

---

## ?? Когда использовать TimeOfDay vs TimeRange

### TimeOfDay - для мгновенных событий:

? Проснуться в 7:00  
? Позвонить в колокол в 12:00  
? Лечь спать в 22:00  

### TimeRange - для длительных действий:

? Работать 9:00-18:00  
? Патрулировать ночью 20:00-06:00  
? Обедать 12:00-13:00  
? **ДВИГАТЬСЯ К ТОЧКЕ** (ваш случай!)

---

## ?? Итог

1. **Измените** `Time Type` на `TimeRange`
2. **Укажите** `End Hour` и `End Minute`
3. **Перезапустите** игру
4. **Проверьте** логи (`Log LogTemp Verbose`)

**Готово!** Актор должен двигаться. ??

---

## ?? Документация

- **BUGFIX_Actor_Not_Moving.md** - Полное техническое описание бага
- **QUICKSTART.md** - Быстрый старт с примерами
- **DIAGNOSTICS.md** - Диагностика всех проблем

**Удачи!** ??
