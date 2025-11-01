# ?? NPC ID - Полный гайд по всем использованиям

## ?? Обзор: Где и зачем используется NPC ID

| Место | Тип | Назначение | Пример значения |
|-------|-----|-----------|----------------|
| **GameplayTags** | `FGameplayTag` | ? **Основной источник** для событий | `NPC.Lianne` |
| **Actor Name** | `FString` | Fallback для событий | `BP_Lianne_C_1` |
| **QuestGiverComponent.NpcId** | `FName` | UI и отладка | `Lianne` |
| **Quest Condition.NpcId** | `FName` | Фильтр события | `Lianne` |
| **DialogueAsset.SpeakerName** | `FText` | UI диалога | `"Лианна"` |

---

## ?? Основной источник: GameplayTags

### Где устанавливается?

```
BP_Lianne (Actor Blueprint)
 ?? Details Panel
      ?? Tags
  ?? Gameplay Tags: NPC.Lianne
```

### Как извлекается?

```cpp
// DialogueSubsystem.cpp: StartDialogue()
FName NpcId = NAME_None;
if (NPC && NPC->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
{
    IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(NPC);
    if (TagInterface)
    {
        FGameplayTagContainer Tags;
  TagInterface->GetOwnedGameplayTags(Tags);
        
  // Ищем тег вида "NPC.*"
    FGameplayTag NPCParent = FGameplayTag::RequestGameplayTag(FName("NPC"), false);
      for (const FGameplayTag& Tag : Tags)
        {
            if (Tag.MatchesTag(NPCParent))
     {
     // Извлекаем "Lianne" из "NPC.Lianne"
      FString TagString = Tag.ToString();  // "NPC.Lianne"
         int32 LastDotIndex;
     if (TagString.FindLastChar('.', LastDotIndex))
        {
       NpcId = FName(*TagString.RightChop(LastDotIndex + 1));  // "Lianne"
    break;
             }
    }
  }
    }
}
```

### Где используется?

```cpp
// Эмиссия события диалога
UGameEventBusLibrary::EmitDialogueEvent(
 this,
    FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started")),
    NpcId,  // ? "Lianne" (из тега NPC.Lianne)
    NAME_None,
    Player,
 NPC
);

// Результат в GameEventBus:
// EventTag: "GameEvent.Dialogue.Started"
// StringParam: "Lianne"  ? ВОТ ЭТОТ ID сравнивается с условием квеста!
```

---

## ?? Fallback: Actor Name

### Когда используется?

Если у актора **нет тега `NPC.*`**, используется упрощённое имя актора.

### Как работает?

```cpp
// Fallback в DialogueSubsystem.cpp
if (NpcId == NAME_None && NPC)
{
    FString ActorName = NPC->GetName();  // "BP_Lianne_C_1"
  
    // Удаляем префикс "BP_"
    ActorName.RemoveFromStart(TEXT("BP_"));  // "Lianne_C_1"
    
    // Берём часть до первого '_'
 int32 UnderscoreIndex;
    if (ActorName.FindChar('_', UnderscoreIndex))
    {
        ActorName = ActorName.Left(UnderscoreIndex);  // "Lianne"
    }
    
    NpcId = FName(*ActorName);  // "Lianne"
}
```

### Примеры:

| Actor Name | Извлечённый ID |
|-----------|---------------|
| `BP_Lianne_C_1` | `Lianne` |
| `BP_Alice_C_0` | `Alice` |
| `NPC_Merchant_2` | `Merchant` |
| `Lianne` | `Lianne` |

---

## ?? QuestGiverComponent.NpcId

### Назначение

**Только для UI и отладки!** Не влияет на события или условия квестов.

### Где устанавливается?

```
BP_Lianne (Actor Blueprint)
 ?? QuestGiverComponent
      ?? NpcId: "Lianne"  ? Ручное заполнение (опционально)
      ?? bAutoAssignNpcId: true  ? Авто-заполнение из имени актора
```

### Как авто-заполняется?

```cpp
// QuestGiverComponent.cpp: BeginPlay()
void UQuestGiverComponent::BeginPlay()
{
    Super::BeginPlay();

    // Auto-assign NPC ID from actor name
    if (bAutoAssignNpcId && NpcId.IsNone())
    {
   AActor* Owner = GetOwner();
   if (Owner)
        {
      NpcId = FName(*Owner->GetName());  // "BP_Lianne_C_1"
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("QuestGiverComponent: Initialized on '%s' with NPC ID '%s'"), 
        *GetOwner()->GetName(), *NpcId.ToString());
}
```

### Где используется?

```cpp
// ТОЛЬКО для UI логов и отладки!
UE_LOG(LogTemp, Log, TEXT("Quest accepted from NPC: %s"), *NpcId.ToString());

// НЕ используется в:
// ? Условиях квестов
// ? Событиях диалогов
// ? Фильтрации событий
```

---

## ?? Quest Condition.NpcId

### Назначение

**Фильтр для события** - с каким NPC произошло событие.

### Где устанавливается?

```
DA_Quest_TalkToLianne (QuestAsset)
 ?? Objectives[0]: "Talk to Lianne"
      ?? Conditions[0]
           ?? EventTag: GameEvent.Dialogue.Started
           ?? NpcId: Lianne  ? Должен совпадать с ID из события!
```

### Как проверяется?

```cpp
// QuestEventBus.cpp: CheckCondition()
bool UQuestEventBus::CheckCondition(const FQuestCondition& Condition, const FGameEventPayload& Payload)
{
    // Проверка EventTag
    if (!Payload.EventTag.MatchesTag(Condition.EventTag))
    {
        return false;  // Тег не совпадает
 }
    
    // Проверка NpcId (если указан)
    if (!Condition.NpcId.IsNone())
    {
    if (Payload.StringParam != Condition.NpcId)
  {
            UE_LOG(LogQuestEventBus, Verbose, TEXT("   ? NPC ID mismatch: Expected '%s', got '%s'"),
  *Condition.NpcId.ToString(),
    *Payload.StringParam.ToString());
return false;  // NPC ID не совпадает
      }
     
        UE_LOG(LogQuestEventBus, Log, TEXT("   ? CONDITION MATCH! StringParam='%s' == NpcId='%s'"),
        *Payload.StringParam.ToString(),
    *Condition.NpcId.ToString());
    }
    
    return true;  // Условие выполнено
}
```

### Пример проверки:

```
Event: GameEvent.Dialogue.Started
  ?? StringParam: "Lianne"  ? Из GameplayTags актора

Condition:
  ?? EventTag: GameEvent.Dialogue.Started  ? Совпадает
  ?? NpcId: Lianne  ? Совпадает с StringParam

Результат: Условие выполнено! Quest прогресс +1
```

---

## ?? DialogueAsset.SpeakerName

### Назначение

**Только UI диалога!** Отображаемое имя персонажа.

### Где устанавливается?

```
DA_Dialogue_LianneGreeting (DialogueAsset)
 ?? Nodes[0]
      ?? SpeakerName: "Лианна"  ? Локализованное имя для UI
```

### Где используется?

```cpp
// Только в UI диалога
DialogueWidget->SetSpeakerName(CurrentNode->SpeakerName);  // "Лианна"
```

### ?? НЕ используется в:
- ? Событиях
- ? Условиях квестов
- ? Идентификации NPC

---

## ?? Сравнительная таблица

| Аспект | GameplayTags | Actor Name | QuestGiver.NpcId | Condition.NpcId | Dialogue.SpeakerName |
|--------|-------------|-----------|-----------------|----------------|---------------------|
| **Приоритет** | ?? Основной | ?? Fallback | ?? UI only | - | - |
| **Используется в событиях** | ? Да | ? Fallback | ? Нет | - | ? Нет |
| **Влияет на условия** | ? Да | ? Да | ? Нет | ? Да | ? Нет |
| **Заполняется** | Вручную | Авто | Авто/Вручную | Вручную | Вручную |
| **Локализация** | ? Нет | ? Нет | ? Нет | ? Нет | ? Да |
| **Тип** | `FGameplayTag` | `FString` | `FName` | `FName` | `FText` |

---

## ? Рекомендации

### 1?? Основная настройка NPC (обязательно!)

```
BP_Lianne (Actor Blueprint)
 ?? Details ? Tags ? Gameplay Tags: NPC.Lianne
```

**Это обеспечит:**
- ? Корректные события диалогов
- ? Правильную работу условий квестов
- ? Надёжную идентификацию NPC

### 2?? Fallback (опционально)

Если не хотите использовать GameplayTags, назовите актор правильно:
```
BP_Lianne_C_1  ?  Извлечётся "Lianne"
```

### 3?? QuestGiverComponent (если нужен UI квестов)

```
BP_Lianne
 ?? QuestGiverComponent
 ?? bAutoAssignNpcId: true  ? Оставить по умолчанию
   ?? NpcId: (авто-заполнится)
```

### 4?? Условие квеста (важно!)

```
DA_Quest_TalkToLianne
 ?? Objective: "Talk to Lianne"
      ?? Condition
  ?? EventTag: GameEvent.Dialogue.Started
   ?? NpcId: Lianne  ? ТОЧНО как в GameplayTag (после точки)!
```

### 5?? Dialogue Speaker (для UI)

```
DA_Dialogue_LianneGreeting
 ?? Node
      ?? SpeakerName: "Лианна"  ? Красивое имя для игрока
```

---

## ?? Отладка: Проверка совпадения ID

### Проверка в логах:

```
// 1. DialogueSubsystem эмитит событие
[LogDialogueSubsystem] Started dialogue 'lianne_greeting'
[LogGameEventBusLibrary] [EmitDialogueEvent] ?? Called! EventTag='GameEvent.Dialogue.Started', NPC=BP_Lianne_C_1
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne' | IntParam=0
      ^^^^^^^ ? Извлечённый ID

// 2. QuestEventBridge получает событие
[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started' | StringParam='Lianne'

// 3. QuestEventBus проверяет условие
[LogQuestEventBus] ?? Checking objective 'O_TalkToLianne' (Progress: 0/1, Conditions: 1)
[LogQuestEventBus]?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
[LogQuestEventBus]   ? CONDITION MATCH! StringParam='Lianne' == NpcId='Lianne'
         ^^^^^^^      ^^^^^^^
        ИЗ СОБЫТИЯ   ИЗ УСЛОВИЯ
```

### ? Ошибка: ID не совпадают

```
[LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='lianne_greeting'
  ^^^^^^^^^^^^^^^^ ? Неправильно!
[LogQuestEventBus]   ?? Condition: EventTag='GameEvent.Dialogue.Started', NpcId='Lianne'
[LogQuestEventBus]   ? NPC ID mismatch: Expected 'Lianne', got 'lianne_greeting'
```

**Причина:** DialogueSubsystem передавал DialogueId вместо NpcId (уже исправлено!)

---

## ?? Итоговая схема

```
???????????????????????????????????????????????????????
? BP_Lianne (Actor)            ?
? ?
? ???????????????????????   ??????????????????????   ?
? ? GameplayTags        ?   ? QuestGiverComponent?   ?
? ?  ?? NPC.Lianne ??????????  ?? NpcId: auto    ?   ?
? ???????????????????????   ??????????????????????   ?
???????????????????????????????????????????????????????
            ?
   ? Извлекается как "Lianne"
            ?
?????????????????????????????????????????????????????
? DialogueSubsystem::StartDialogue()    ?
?  ?? UGameEventBusLibrary::EmitDialogueEvent()     ?
?       ?? StringParam = "Lianne" ???????????????   ?
?????????????????????????????????????????????????????
          ?
             ?
?????????????????????????????????????????????????????
? GameEventBus::EmitEvent()   ?
?  EventTag: GameEvent.Dialogue.Started             ?
?  StringParam: "Lianne" ????????????????????????   ?
?????????????????????????????????????????????????????
     ?
         ?
?????????????????????????????????????????????????????
? QuestEventBridge::OnGameEvent()          ?
?  ?? ForwardToQuestSystem() ???????????????????    ?
?????????????????????????????????????????????????????
        ?
               ?
?????????????????????????????????????????????????????
? QuestEventBus::CheckCondition()?
?            ?
?  Payload.StringParam: "Lianne" ??? Из события    ?
?      ?        ?
?  Condition.NpcId: "Lianne" ???? Из Data Asset    ?
?   ?        ?
?  ? MATCH! ? Quest Progress +1       ?
?????????????????????????????????????????????????????
```

---

## ?? См. также

- `NPCID_EXTRACTION_FIXED.md` - Как работает извлечение NPC ID
- `QUEST_GIVER_COMPONENT_EXPLAINED.md` - Полное объяснение QuestGiverComponent
- `QUEST_CONDITIONS_COMPLETE_REFERENCE.md` - Как работают условия квестов
- `EVENT_TAG_FILTER_FIXED.md` - Как фильтруются события

---

**TL;DR:** Используйте **GameplayTag `NPC.Lianne`** - это основной и надёжный источник ID! ??
