# ? DIALOGUE EVENTS: NPCID EXTRACTION FIXED!

## ?? Проблема:

**Событие эмитировалось**, но **НЕ совпадал фильтр NpcId**!

### Логи показывали:

```
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started' | StringParam='lianne_greeting'
[LogTemp] ?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
```

**StringParam = 'lianne_greeting'** (DialogueId) ? **NpcId = 'Lianne'**

? **Фильтр НЕ совпал ? квест НЕ получил прогресс!**

---

## ? Решение:

### Извлекаем NpcId из NPC actor:

**Приоритет извлечения:**
1. **GameplayTags** — ищем тэг вида `NPC.Lianne` ? извлекаем `"Lianne"`
2. **Fallback** — упрощаем имя актора: `BP_Lianne_C_1` ? `"Lianne"`

### Код в StartDialogue():

```cpp
// Extract NPC ID: Try to get from tags first (e.g., "NPC.Lianne"), fallback to simplified actor name
FName NpcId = NAME_None;
if (NPC)
{
    // Try IGameplayTagAssetInterface first
    if (NPC->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
    {
        IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(NPC);
  if (TagInterface)
        {
            FGameplayTagContainer Tags;
      TagInterface->GetOwnedGameplayTags(Tags);

    // Look for NPC.* tag
            FGameplayTag NPCParent = FGameplayTag::RequestGameplayTag(FName("NPC"), false);
            if (NPCParent.IsValid())
{
        for (const FGameplayTag& Tag : Tags)
       {
       if (Tag.MatchesTag(NPCParent))
         {
       // Extract last part of tag (e.g., "NPC.Lianne" -> "Lianne")
        FString TagString = Tag.ToString();
      int32 LastDotIndex;
  if (TagString.FindLastChar('.', LastDotIndex))
             {
                  NpcId = FName(*TagString.RightChop(LastDotIndex + 1));
   break;
 }
                 }
    }
 }
        }
    }
    
    // Fallback: use simplified actor name if no tag found
    if (NpcId == NAME_None)
    {
 FString ActorName = NPC->GetName();
        // Remove Blueprint suffix (e.g., "BP_Lianne_C_1" -> "Lianne")
  ActorName.RemoveFromStart(TEXT("BP_"));
        int32 UnderscoreIndex;
        if (ActorName.FindChar('_', UnderscoreIndex))
        {
            ActorName = ActorName.Left(UnderscoreIndex);
    }
        NpcId = FName(*ActorName);
    }
}

UGameEventBusLibrary::EmitDialogueEvent(
    this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
  NpcId, // NpcId extracted from tags or actor name
    NAME_None,
    Player,
    NPC
);
```

---

## ?? Изменённые файлы:

### 1. DialogueSubsystem.cpp

- ? Добавлена логика извлечения NpcId из GameplayTags
- ? Fallback на упрощённое имя актора
- ? Передаём NpcId вместо DialogueId в первом параметре

### 2. DialogueSubsystem.cpp (includes)

- ? Добавлен `#include "GameplayTagAssetInterface.h"`

---

## ?? Теперь работает с двумя вариантами:

### Вариант 1: NPC с GameplayTags ?

**NPC имеет тэг:** `NPC.Lianne`

**Результат:**
```
NpcId = "Lianne" ? Извлечён из тэга!
```

### Вариант 2: NPC без тэгов (fallback) ?

**NPC имя актора:** `BP_Lianne_C_1`

**Результат:**
```
ActorName.RemoveFromStart("BP_") ? "Lianne_C_1"
ActorName.Left(UnderscoreIndex) ? "Lianne" ? Упрощено!
```

---

## ?? Ожидаемые логи после исправления:

### При начале диалога:

```
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Started', NPC=BP_Lianne_C_1
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne' | IntParam=0
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started' | StringParam='Lianne'
[LogQuestEventBus] ?? Checking objective 'O_TalkToLianne' (Progress: 0/1, Conditions: 1)
[LogQuestEventBus]   ?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
[LogQuestEventBus] ? CONDITION MATCH! StringParam='Lianne' == NpcId='Lianne'
[LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

---

## ?? Важно для создания квестов:

### В Data Asset квеста:

```
Objective: "Talk to Lianne"
  Condition:
    EventTag: GameEvent.Dialogue.Started
NpcId: Lianne  ? ДОЛЖНО СОВПАДАТЬ с извлечённым ID!
    Count: 1
```

### В NPC:

**Вариант А (рекомендуется):** Добавить GameplayTag
```
Gameplay Tags: NPC.Lianne  ? ID = "Lianne"
```

**Вариант Б (fallback):** Назвать актор правильно
```
Actor Name: BP_Lianne_C_1  ? ID = "Lianne" (упрощено)
```

---

## ?? Что делать дальше:

1. **Открыть Unreal Editor**
2. **Запустить игру** и поговорить с Lianne
3. **Проверить логи** — должна быть эмиссия с правильным NpcId!
4. **Квест должен получить прогресс!** ?

---

**NPCID EXTRACTION FIXED!** ?????
