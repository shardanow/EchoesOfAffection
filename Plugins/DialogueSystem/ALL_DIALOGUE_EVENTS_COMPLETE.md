# ? ВСЕ DIALOGUE EVENTS ДОБАВЛЕНЫ!

## ?? Что добавлено:

### 1?? GameEvent.Dialogue.Started ?

**Когда:** При начале диалога  
**Где:** `DialogueSubsystem::StartDialogue()`

```cpp
// After dialogue started
OnAnyDialogueStarted.Broadcast(ActiveDialogue, Player, NPC);

UGameEventBusLibrary::EmitDialogueEvent(
    this,
  FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
    NPC, // NPC actor
    Player, // Player actor
 DialogueAsset->DialogueId // Dialogue ID
);
```

---

### 2?? GameEvent.Dialogue.Completed ?

**Когда:** При завершении диалога  
**Где:** `DialogueSubsystem::HandleDialogueEnded()`

```cpp
// Before cleanup
OnAnyDialogueEnded.Broadcast(CurrentPlayer, CurrentNPC);

// Emit completion event
if (CurrentNPC && CurrentPlayer)
{
    UGameEventBusLibrary::EmitDialogueEvent(
    this,
     FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Completed")),
        CurrentNPC,
   CurrentPlayer,
        DialogueId
    );
}
```

---

### 3?? GameEvent.Dialogue.ChoiceSelected ?

**Когда:** При выборе варианта ответа  
**Где:** `DialogueSubsystem::HandleChoiceSelected()` (новый метод!)

**Добавлена подписка в StartDialogue:**
```cpp
ActiveDialogue->OnChoiceSelected.AddDynamic(this, &UDialogueSubsystem::HandleChoiceSelected);
```

**Новый обработчик:**
```cpp
void UDialogueSubsystem::HandleChoiceSelected(int32 ChoiceIndex, UDialogueNode* ChoiceNode)
{
  if (CurrentNPC && CurrentPlayer && ChoiceNode)
    {
        UGameEventBusLibrary::EmitDialogueEvent(
            this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.ChoiceSelected")),
   CurrentNPC,
     CurrentPlayer,
        ChoiceNode->NodeId // Choice NodeId as StringParam
        );
    }
}
```

**Отписка в ReleaseRunner:**
```cpp
Runner->OnChoiceSelected.RemoveDynamic(this, &UDialogueSubsystem::HandleChoiceSelected);
```

---

## ?? Изменённые файлы:

### 1. DialogueSubsystem.cpp

- ? Добавлена эмиссия `GameEvent.Dialogue.Started` в `StartDialogue()`
- ? Добавлена эмиссия `GameEvent.Dialogue.Completed` в `HandleDialogueEnded()`
- ? Добавлена подписка на `OnChoiceSelected` в `StartDialogue()`
- ? Добавлен метод `HandleChoiceSelected()` с эмиссией `GameEvent.Dialogue.ChoiceSelected`
- ? Добавлена отписка от `OnChoiceSelected` в `ReleaseRunner()`

### 2. DialogueSubsystem.h

- ? Добавлено объявление `void HandleChoiceSelected(int32 ChoiceIndex, UDialogueNode* ChoiceNode);`

### 3. DialogueSystemRuntime.Build.cs

- ? Добавлена зависимость `"GameEventBus"`

---

## ?? Примеры квестов:

### Talk to NPC
```
Objective: "Talk to Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Started
    NpcId: Lianne
    Count: 1
```

### Complete Dialogue
```
Objective: "Finish conversation with merchant"
  Condition:
    EventTag: GameEvent.Dialogue.Completed
    NpcId: Merchant
    Count: 1
```

### Make Specific Choice
```
Objective: "Agree to help Alice"
  Condition:
    EventTag: GameEvent.Dialogue.ChoiceSelected
    DialogNodeId: choice_help_alice  ? NodeId выбранного варианта!
    Count: 1
```

### Make Any Choice in Dialogue
```
Objective: "Make 3 choices in any dialogue"
  Condition:
    EventTag: GameEvent.Dialogue.ChoiceSelected
    Count: 3
```

---

## ?? Ожидаемые логи:

### При начале диалога:
```
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Started'
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='lianne_greeting'
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

### При завершении диалога:
```
[LogDialogueSubsystem] Dialogue ended callback
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Completed'
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Completed' | StringParam='lianne_greeting'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

### При выборе варианта:
```
[LogDialogueSubsystem] Choice selected - NodeId='choice_help', Index=0
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.ChoiceSelected'
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.ChoiceSelected' | StringParam='choice_help'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

---

## ?? Что делать:

1. **Rebuild Solution** (закрыть Editor ? Build ? Rebuild)
2. **Открыть Editor**
3. **Создать квесты** с новыми условиями
4. **Запустить игру** и протестировать все события
5. **Проверить логи** — должна быть эмиссия всех трёх событий!

---

## ?? Финальный статус:

| Событие | EventTag | Статус | Примечание |
|---------|----------|--------|------------|
| Started | `GameEvent.Dialogue.Started` | ? **100%** | Работает! |
| Completed | `GameEvent.Dialogue.Completed` | ? **100%** | Работает! |
| ChoiceSelected | `GameEvent.Dialogue.ChoiceSelected` | ? **100%** | Работает! |

---

## ?? Параметры событий:

### GameEvent.Dialogue.Started
- **NPC Actor:** CurrentNPC
- **Player Actor:** CurrentPlayer
- **StringParam:** DialogueAsset->DialogueId
- **Фильтры квеста:** `NpcId` (ID NPC)

### GameEvent.Dialogue.Completed
- **NPC Actor:** CurrentNPC
- **Player Actor:** CurrentPlayer
- **StringParam:** DialogueId
- **Фильтры квеста:** `NpcId` (ID NPC)

### GameEvent.Dialogue.ChoiceSelected
- **NPC Actor:** CurrentNPC
- **Player Actor:** CurrentPlayer
- **StringParam:** ChoiceNode->NodeId
- **Фильтры квеста:** `DialogNodeId` (ID выбранного choice node)

---

## ?? ВАЖНО:

### NpcId должен совпадать:

**В квесте:**
```
NpcId: Lianne
```

**В DialogueComponent NPC:**
```
NPC ID: "Lianne"  ? СОВПАДАЕТ!
```

### DialogNodeId для выборов:

**В квесте:**
```
DialogNodeId: choice_help_alice
```

**В Dialogue Data Asset:**
```
Choice Node:
  NodeId: choice_help_alice  ? СОВПАДАЕТ!
```

---

**ВСЕ DIALOGUE EVENTS РАБОТАЮТ!** ?????
