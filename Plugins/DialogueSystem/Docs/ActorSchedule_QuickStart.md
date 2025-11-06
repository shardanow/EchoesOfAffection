# ? QUICK START: ACTOR SCHEDULE + DIALOGUE + SEQUENCER

## ?? **SETUP (5 МИНУТ)**

### **1. Добавить ActorScheduleComponent к NPC:**

```cpp
// BP_Lia (Blueprint)
Components:
?? ActorScheduleComponent
   ?? bAutoPauseOnDialogue = true ?
   ?? bAutoResumeOnDialogueEnd = true ?
   ?? bDisableAIDuringPause = true ?
   ?? bStopMovementDuringPause = true ?
```

---

### **2. Создать Sequence с Possessable:**

```
Sequencer:
+ Track ? Actor to Sequencer (Possessable!)
Select: BP_Lia from World Outliner

Timeline 0:00s: Teleport to (X=100, Y=100)
Timeline 0:05s: Move to (X=150, Y=150)
```

---

### **3. Dialogue Node с Sequence:**

```
Node: cinematic_moment (Event)
?? Text: "[Сцена...]"
?? Effect: "PlaySequence(/Game/Sequences/LS_MyScene, true, true, false)"
          ?
       restoreActors = FALSE! ? ВАЖНО!
```

---

## ?? **ЧТО ПРОИСХОДИТ:**

```
1. Dialogue starts
   ?? ActorScheduleComponent ? Pause AI

2. Sequence plays (Possessable)
   ?? Teleport actor ? (X=100, Y=100)
   ?? Move actor ? (X=150, Y=150)

3. Dialogue ends
   ?? ActorScheduleComponent ? Resume AI from (X=150, Y=150) ?

Result: Actor продолжает с НОВОЙ позиции!
```

---

## ? **КЛЮЧЕВЫЕ ПАРАМЕТРЫ:**

| Параметр | Значение | Почему |
|----------|----------|--------|
| **Actor Type** | Possessable | Тот же актор (state сохраняется) |
| **restoreActors** | `false` | НЕ восстанавливать позицию после |
| **bAutoPauseOnDialogue** | `true` | Пауза AI автоматически |
| **bAutoResumeOnDialogueEnd** | `true` | Возобновление AI автоматически |

---

## ?? **ПОДРОБНЕЕ:**

`ActorSchedule_Architecture.md` - Полная архитектура

---

**TL;DR:** Possessable + restoreActors=false + ActorScheduleComponent = Natural Open World! ??
