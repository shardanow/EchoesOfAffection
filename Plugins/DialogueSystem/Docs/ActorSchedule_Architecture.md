# ?? АРХИТЕКТУРА: DIALOGUE + ACTOR SCHEDULE INTEGRATION

## ?? **ВАША ЛОГИКА (ПРАВИЛЬНАЯ!):**

```
WORKFLOW:

1. Сцена начинается (Dialogue Start)
   ?? DialogueRunner ? StartDialogue()
   ?? ? GameEventBus ? Event "GameEvent.Dialogue.Started"
   ?? ? ScheduleComponent (на каждом NPC) получает событие автоматически
   ?? ? Приостанавливает AI Schedule (если актор участвует в диалоге)
   ?? Останавливает движение
   ?? Акторы "замирают" где стоят

2. Sequence управляет (Possessable)
   ?? PlaySequence(path, wait=true, restoreCamera=true, restoreActors=FALSE!)
   ?? Sequence ТЕЛЕПОРТИРУЕТ акторов в нужные позиции для сцены
   ?? Двигает по keyframes (A ? B)
   ?? Анимации, камера, всё визуальное
   ?? Тот же актор = state (inventory, affinity) сохраняется!

3. Диалог продолжается
   ?? После Sequence игрок делает выбор
   ?? Effects применяются (ModifyAffinity, AddItem, etc.)
   ?? Акторы всё ещё на паузе

4. Диалог заканчивается (Dialogue End)
   ?? DialogueRunner ? EndDialogue()
   ?? ? GameEventBus ? Event "GameEvent.Dialogue.Ended"
   ?? ? ScheduleComponent получает событие автоматически
   ?? ? Восстанавливает AI Schedule (если актор был на паузе)
   ?? Восстанавливает движение
   ?? Акторы продолжают с ТЕКУЩЕЙ позиции (где Sequence оставил!)
```

**ЭТО ЕСТЕСТВЕННО ДЛЯ ОТКРЫТОГО МИРА!** ??

---

## ??? **КОМПОНЕНТЫ СИСТЕМЫ**

### **1. DialogueRunner** (DialogueSystem plugin)

```cpp
// Plugins/DialogueSystem/Source/DialogueSystemCore/Private/Core/DialogueRunner.cpp

void StartDialogue(...)
{
    // ...existing code...
    
    // ? EMIT GameEventBus event
    EmitDialogueStartedEvent(DialogueId, Participants);
    
    // Go to start node
    GoToNode(StartNodeId);
}

void EndDialogue()
{
    // ...existing code...

    // ? EMIT GameEventBus event
    EmitDialogueEndedEvent();
    
    // Cleanup
}
```

**Events emitted:**
- `GameEvent.Dialogue.Started` - когда диалог начинается
- `GameEvent.Dialogue.Ended` - когда диалог заканчивается

---

### **2. ScheduleComponent** (ActorScheduleSystem plugin)

```cpp
// Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Private/Components/ScheduleComponent.cpp

void BeginPlay()
{
    // ...existing code...
    
    // ? Автоматически подписывается на события диалога
    SubscribeToDialogueEvents();
}

void OnDialogueStarted(const FGameEventPayload& Payload)
{
    // Проверяет, участвует ли этот актор в диалоге
    bool bIsInvolvedInDialogue = (Payload.TargetActor == Owner) || (Payload.InstigatorActor == Owner);
    
    if (bIsInvolvedInDialogue)
    {
     // ? Автоматически паузит расписание
     PauseSchedule();
    }
}

void OnDialogueEnded(const FGameEventPayload& Payload)
{
    bool bWasInvolvedInDialogue = (Payload.TargetActor == Owner) || (Payload.InstigatorActor == Owner);
    
    if (bWasInvolvedInDialogue && bIsPaused)
    {
        // ? Автоматически возобновляет расписание
        // НЕ восстанавливает позицию! Продолжает с текущей.
        ResumeSchedule();
    }
}
```

**? СОФТ СВЯЗКА ЧЕРЕЗ GameEventBus!**
- Нет хард dependency между плагинами
- ScheduleComponent работает даже если DialogueSystem отключён
- DialogueSystem работает даже если ActorScheduleSystem отключён

---

## ?? **SETUP В BLUEPRINTS**

### **NPC Blueprint (BP_Lia):**

```
Components:
?? DialogueComponent (DialogueSystem)
?? NPCMemoryComponent (DialogueSystem)
?? RelationshipComponent (RelationshipSystem)
?? ScheduleComponent (ActorScheduleSystem) ? ? ВСЁЁ! Больше ничего не нужно!
   ?? ScheduleData = DA_LiaSchedule
   ?? bScheduleEnabled = true
```

**ВСЁ! Больше никакой настройки не требуется!**

- ? ScheduleComponent автоматически слушает GameEventBus
- ? Когда диалог с Lia начинается ? автоматически пауза
- ? Когда диалог заканчивается ? автоматически возобновление

---

## ?? **ПРИМЕР СЦЕНАРИЯ**

### **Сцена: "Lia и Ellen спорят на площади"**

#### **SETUP:**

```
Level (Runtime):
?? BP_Lia: Гуляет по деревне (X=500, Y=300, Z=100)
?  ?? ScheduleComponent ? слушает GameEventBus
?? BP_Ellen: В таверне (X=1200, Y=800, Z=100)
?  ?? ScheduleComponent ? слушает GameEventBus
?? BP_Player: Контролируется игроком (X=0, Y=0, Z=100)
```

---

#### **DIALOGUE FLOW:**

**Node 1: player_greets_lia (NpcLine)**
```
Speaker: Lia
Text: "Привет! Как дела?"

Что происходит:
1. ? DialogueRunner ? StartDialogue()
2. ? GameEventBus ? EmitEvent("GameEvent.Dialogue.Started")
   Payload:
   ?? InstigatorActor = Player
   ?? TargetActor = Lia
3. ? ScheduleComponent (на Lia) ? OnDialogueStarted()
   ?? Проверяет: bIsInvolvedInDialogue = (TargetActor == Lia) ? TRUE!
   ?? PauseSchedule() ? Lia останавливается на (X=500, Y=300)
4. ? ScheduleComponent (на Ellen) ? OnDialogueStarted()
   ?? Проверяет: bIsInvolvedInDialogue = (TargetActor == Ellen) ? FALSE
   ?? НЕ паузит (Ellen не участвует)
```

---

**Node 4: dialogue_end (End)**
```
Type: End

Что происходит:
1. ? DialogueRunner ? EndDialogue()
2. ? GameEventBus ? EmitEvent("GameEvent.Dialogue.Ended")
   Payload:
   ?? InstigatorActor = Player
   ?? TargetActor = Lia
3. ? ScheduleComponent (на Lia) ? OnDialogueEnded()
   ?? Проверяет: bWasInvolvedInDialogue = TRUE
   ?? bIsPaused = TRUE
   ?? ResumeSchedule() ? Lia продолжает с (X=100, Y=100) ? площадь!
4. ? AI Schedule продолжается с НОВОЙ позиции!
```

---

## ? **ПРЕИМУЩЕСТВА ВАШЕЙ АРХИТЕКТУРЫ:**

### **1. Софт связка (Soft Coupling):**
```
DialogueSystem ?????[GameEventBus]????? ActorScheduleSystem
    ?           ?
    ?         ?
  Emit events  Listen events
    ?     ?
    ?? НЕ знает про ActorScheduleSystem!        ?
        ?
         ?? НЕ знает про DialogueSystem!
```

**Преимущества:**
- ? Плагины независимы
- ? Можно отключить любой плагин без ошибок
- ? Легко тестировать
- ? Легко расширять

### **2. Автоматизация:**
- ? Не нужно вручную паузить/возобновлять
- ? Не нужно проверять, кто участвует в диалоге
- ? Всё работает автоматически

### **3. Естественность:**
- ? Акторы НЕ "телепортируются обратно"
- ? Продолжают с логичного места (где сцена закончилась)
- ? Реалистично для открытого мира!

### **4. State Continuity:**
- ? Тот же актор = inventory, affinity сохраняются
- ? Нет проблем с Spawnable (новый экземпляр)

---

## ?? **ТЕХНИЧЕСКИЕ ДЕТАЛИ**

### **GameEventBus Events:**

```cpp
// Event 1: Dialogue Started
FGameEventPayload Payload;
Payload.EventTag = "GameEvent.Dialogue.Started";
Payload.StringParam = DialogueId; // FName
Payload.InstigatorActor = Player; // TWeakObjectPtr<AActor>
Payload.TargetActor = NPC; // TWeakObjectPtr<AActor>

// Event 2: Dialogue Ended
FGameEventPayload Payload;
Payload.EventTag = "GameEvent.Dialogue.Ended";
Payload.InstigatorActor = Player;
Payload.TargetActor = NPC;
```

### **Compile-time defines:**

```cpp
// ScheduleRuntime.Build.cs
if (GameEventBus plugin exists)
{
    PrivateDependencyModuleNames.Add("GameEventBus");
    PublicDefinitions.Add("WITH_GAMEEVENTBUS=1");
}
else
{
    PublicDefinitions.Add("WITH_GAMEEVENTBUS=0");
}

// ScheduleComponent.cpp
#if WITH_GAMEEVENTBUS
    // GameEventBus integration code
#endif
```

**Результат:** ScheduleComponent компилируется даже если GameEventBus недоступен!

---

## ? **РЕЗЮМЕ:**

**ВАША ЛОГИКА ПРАВИЛЬНАЯ И ЭЛЕГАНТНАЯ!**

```
? Софт связка через GameEventBus (без хард dependency)
? Автоматическая пауза/возобновление Schedule
? Possessable (тот же актор)
? bRestoreActors = false (НЕ восстанавливать позиции)
? Акторы продолжают с ТЕКУЩЕЙ позиции после диалога
? Естественно для открытого мира!
```

**ЭТО ПРОФЕССИОНАЛЬНЫЙ ПОДХОД ДЛЯ OPEN WORLD DATING SIM!** ??

---

**Version:** 1.13  
**Author:** Dialogue System Team + Actor Schedule System Team  
**Last Updated:** 2024
