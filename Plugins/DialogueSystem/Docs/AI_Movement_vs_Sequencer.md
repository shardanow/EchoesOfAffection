# ?? **AI MOVEMENT VS SEQUENCER MOVEMENT**

## ? **В ЧЁМ РАЗНИЦА?**

### **Sequencer Movement (Transform Keyframes)**

```cpp
// Sequencer напрямую устанавливает Transform:
Actor->SetActorLocation(TargetLocation);
```

**Характеристики:**
- ? Точный контроль позиции
- ? Идеально для кинематографики
- ? Повторяемый результат
- ? НЕ использует NavMesh
- ? НЕ обходит препятствия
- ? Движение по прямой или Spline
- ? НЕ использует AI Controller

**Когда использовать:**
- Кинематографические сцены
- Строго заданная камера
- Точная синхронизация с анимациями
- Cutscenes

---

### **AI Movement (NavMesh + Pathfinding)**

```cpp
// AI Controller вычисляет путь и движется:
AIController->MoveTo(TargetLocation);
```

**Характеристики:**
- ? Использует NavMesh
- ? Обходит препятствия автоматически
- ? Реалистичное поведение
- ? Работает с dynamic obstacles
- ? Непредсказуемый путь
- ? Сложно контролировать камеру
- ? Может "застрять"

**Когда использовать:**
- Open world поведение
- Schedule System
- Реалистичное NPC движение
- Gameplay

---

## ?? **МОЖНО ЛИ ИСПОЛЬЗОВАТЬ AI В SEQUENCER?**

### **? Короткий ответ: НЕТ**

Sequencer **НЕ поддерживает** AI movement напрямую.

**Почему:**
```
Sequencer нужен детерминированный результат
? Transform keyframes гарантируют точную позицию
? AI movement непредсказуем (разные пути каждый раз)
? Невозможно синхронизировать с камерой/анимациями
```

---

## ?? **РЕШЕНИЯ:**

### **Решение 1: Hybrid Approach (РЕКОМЕНДУЕТСЯ)**

**Используйте AI для движения + Sequencer для камеры**

#### **Пример: Alice идёт к фонтану**

**1. Schedule System (AI движение):**
```cpp
// DA_AliceSchedule:
08:00 - Walk to Fountain:
?? Action: MoveToLocation
?? Target: BP_Fountain_Marker
?? Speed: Normal
```

**2. Sequencer (только камера):**
```
LS_Alice_Walk_Camera:
?? Camera Cut Track
?   ?? Camera_Follow
?       ?? 0:00 ? Wide shot behind Alice
?       ?? 0:02 ? Side tracking shot
?       ?? 0:04 ? Front shot near fountain
?? ? НЕ добавляйте Alice Transform!
```

**3. Синхронизация:**
```cpp
// Blueprint:
1. Start Schedule Entry: "Walk to Fountain"
2. Wait 0.5 seconds (чтобы Alice начала идти)
3. Play Sequence: LS_Alice_Walk_Camera (wait=false)
4. Wait for Schedule Complete
```

**Результат:**
- ? Alice движется через AI (NavMesh, препятствия)
- ? Камера следует кинематографично
- ? Лучшее из обоих миров!

---

### **Решение 2: ScheduleAction_FollowSpline (НОВОЕ!)**

**AI движение по заданному пути**

#### **Что это:**
Кастомное Schedule Action которое:
- ? Использует AI Controller
- ? Следует по Spline пути
- ? Обходит препятствия
- ? Детерминированный маршрут

#### **Как использовать:**

**1. Создайте Spline путь:**
```
1. Place Actors ? Blueprint Spline
2. Настройте путь от A до B (избегая препятствий)
3. Назовите: "Path_Alice_To_Fountain"
```

**2. Создайте Schedule Entry:**
```cpp
// DA_AliceSchedule:
08:00 - Scripted Walk:
?? Action: ScheduleAction_FollowSpline
?? Spline Actor: Path_Alice_To_Fountain
?? Sample Points: 10 (waypoints вдоль spline)
?? Speed Multiplier: 1.0
?? Waypoint Radius: 100
```

**3. (Опционально) Добавьте Sequencer камеру:**
```
LS_Alice_Walk_Camera:
?? Camera tracking Alice
```

**Результат:**
- ? Alice движется через AI
- ? Путь предсказуем (по Spline)
- ? Обходит dynamic препятствия
- ? Можно добавить кинематографичную камеру

---

### **Решение 3: Sequencer с Path Track**

**Если AI не нужен вообще**

#### **Когда использовать:**
- Строго заданная сцена
- НЕ нужна реакция на препятствия
- Нужна точная синхронизация

#### **Как сделать:**

**1. Создайте Spline:**
```
Path_Alice_Route
```

**2. В Sequencer:**
```
LS_Alice_Scripted_Walk:
?? BP_Alice_C_1 (Possessable)
?   ?? Attach Track
?   ?   ?? Parent: Path_Alice_Route
?   ?? Attach Component
?       ?? 0:00 ? Position on Spline: 0.0
?       ?? 0:05 ? Position on Spline: 1.0
?? Camera Cut Track
```

**Результат:**
- ? Alice движется по сложному пути
- ? Точная синхронизация
- ? НЕ использует AI
- ? НЕ обходит препятствия

---

## ?? **СРАВНЕНИЕ:**

| Метод | AI | NavMesh | Препятствия | Камера | Точность |
|-------|-----|---------|-------------|---------|----------|
| **Sequencer Transform** | ? | ? | ? | ??? | ??? |
| **Sequencer + Path** | ? | ? | ? | ??? | ?? |
| **Schedule MoveTo** | ? | ? | ? | ? | ? |
| **Hybrid (AI + Camera)** | ? | ? | ? | ?? | ? |
| **FollowSpline (NEW)** | ? | ? | ? | ? | ?? |

---

## ?? **РЕКОМЕНДАЦИИ:**

### **Для Cutscenes/Кинематографики:**
```
? Используйте Sequencer Transform
? Точный контроль важнее реализма
```

### **Для Gameplay сцен:**
```
? Используйте Schedule MoveTo
? Реализм важнее точности
```

### **Для Скриптованных событий:**
```
? Используйте Hybrid подход
? AI движение + Sequencer камера
? ИЛИ ScheduleAction_FollowSpline
```

### **Для Open World диалогов:**
```
? Используйте только Schedule System
? Sequencer только для camera cuts
? restoreActors=false!
```

---

## ?? **ПРИМЕРЫ ИСПОЛЬЗОВАНИЯ:**

### **Пример 1: Кинематографическая встреча**

```
Задача: Alice и Lianne встречаются у фонтана (cutscene)

Решение: Sequencer Transform
1. Создайте LS_Meeting_Cutscene
2. Добавьте обеих как Possessable
3. Keyframe движение Alice к фонтану
4. Keyframe поворот Lianne к Alice
5. Настройте камеру (multiple shots)
6. PlaySequence в диалоге
```

### **Пример 2: NPC идёт на работу**

```
Задача: Alice каждое утро идёт в таверну (gameplay)

Решение: Schedule MoveTo
1. Создайте DA_AliceSchedule
2. 08:00 - MoveTo(Tavern)
3. AI автоматически найдёт путь
4. Обойдёт игрока и препятствия
```

### **Пример 3: Скриптованный парад**

```
Задача: NPC проходят парадом по главной улице

Решение: ScheduleAction_FollowSpline
1. Создайте Spline по главной улице
2. Для каждого NPC: FollowSpline action
3. Разные Start Offset для интервала
4. (Опционально) Sequencer камера следует
```

---

## ? **РЕЗЮМЕ:**

```
? Sequencer НЕ использует AI Controller
? Используйте Hybrid подход для лучшего результата
? ScheduleAction_FollowSpline = AI + заданный путь
? Выбирайте метод в зависимости от задачи
```

---

**Версия:** 1.14  
**Дата:** 2024  
**См. также:** 
- `Sequencer_Tutorial_Complete.md`
- `ActorSchedule_Architecture.md`
