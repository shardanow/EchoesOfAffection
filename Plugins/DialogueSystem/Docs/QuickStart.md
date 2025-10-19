# Dialogue System - Quick Start Guide

## ?? Быстрый старт за 5 минут

### Шаг 1: Создать DialogueAsset

1. В Content Browser ? ПКМ ? `Dialogue System ? Dialogue Data Asset`
2. Назвать: `DA_Lianne_FirstMeeting`
3. Открыть и заполнить:
   - **DialogueId**: `Lianne_FirstMeeting`
   - **DisplayName**: "First meeting with Lianne"
   - **Description**: "Initial conversation when player meets Lianne"

### Шаг 2: Создать узлы диалога

#### В секции **Nodes** добавить:

**Node 1: Start**
```
NodeId: Start
NodeType: NpcLine
SpeakerId: Lianne
DialogueText: "Oh, hello! I haven't seen you around here before..."
EmotionTag: Emotion.Neutral
```

**Node 2: Choice_Introduce**
```
NodeId: Choice_Introduce
NodeType: PlayerChoice
ChoiceText: "Hi, I'm new to the village"
EffectExpression: "affinity[Lianne] += 5"
```

**Node 3: Choice_Silent**
```
NodeId: Choice_Silent
NodeType: PlayerChoice
ChoiceText: "..." (stay silent)
EffectExpression: "affinity[Lianne] -= 2"
```

**Node 4: Response_Positive**
```
NodeId: Response_Positive
NodeType: NpcLine
SpeakerId: Lianne
DialogueText: "Welcome! I'm Lianne. Let me know if you need any help!"
ConditionExpression: "affinity[Lianne] >= 0"
EmotionTag: Emotion.Happy
```

**Node 5: Response_Negative**
```
NodeId: Response_Negative
NodeType: NpcLine
SpeakerId: Lianne
DialogueText: "Oh... okay then. *looks away awkwardly*"
ConditionExpression: "affinity[Lianne] < 0"
EmotionTag: Emotion.Sad
```

**Node 6: End**
```
NodeId: End
NodeType: End
```

#### В секции **Edges** добавить:

```
Start ? Choice_Introduce (Priority: 0)
Start ? Choice_Silent (Priority: 1)

Choice_Introduce ? Response_Positive (Priority: 0)
Choice_Introduce ? Response_Negative (Priority: 1)

Choice_Silent ? Response_Negative (Priority: 0)

Response_Positive ? End
Response_Negative ? End
```

### Шаг 3: Настроить NPC

1. Открыть Blueprint NPC: `BP_Lianne`
2. Добавить компонент: `Dialogue Component`
3. В компоненте установить:
   - **CharacterId**: `Lianne`
   - **DisplayName**: "Lianne"
   - **DefaultDialogue**: выбрать `DA_Lianne_FirstMeeting`
   - **Portrait**: импортировать портрет
4. Добавить компонент: `Relationship Component`

### Шаг 4: Настроить UI

1. Создать Widget Blueprint: `WBP_DialogueMain`
2. Унаследовать от: `DialogueWidget` (C++ parent)
3. Добавить элементы:
   - Image: `SpeakerPortrait` (слева)
   - TextBlock: `SpeakerName` (над портретом)
   - RichTextBlock: `DialogueText` (по центру, мультистрока)
   - VerticalBox: `ChoicesContainer` (внизу)
4. Создать Widget Blueprint: `WBP_Choice`
5. Унаследовать от: `ChoiceWidget`
6. Добавить элементы:
   - Button: `ChoiceButton`
   - TextBlock: `ChoiceText` (внутри кнопки)
   - TextBlock: `HotkeyText` (слева от текста, маленький)

### Шаг 5: Настроить взаимодействие

В `BP_PlayerCharacter`:

```cpp
// Blueprint Graph

Event BeginPlay
  ?
[Get Subsystem: DialogueSubsystem]
  ?
[Set as member variable]

Event OnInteract (E pressed)
  ?
[Line Trace от камеры]
  ?
[Get Hit Actor ? Cast to DialogueComponent]
  ?
[If valid] ? [Call: StartDialogue(Self)]
```

### Шаг 6: Тест!

1. Play In Editor
2. Подойти к Lianne
3. Нажать E
4. Выбрать вариант ответа
5. Наблюдать изменение Affinity в Relationship Component

---

## ?? Checklist для нового диалога

- [ ] Создан DialogueAsset
- [ ] Заполнены Nodes (минимум Start + End)
- [ ] Заполнены Edges (все узлы связаны)
- [ ] Настроена Persona (если нужен AI)
- [ ] У NPC есть DialogueComponent
- [ ] У NPC есть RelationshipComponent
- [ ] CharacterId совпадает в Component и в Nodes (SpeakerId)
- [ ] Portrait загружен
- [ ] UI виджет настроен в Level Blueprint
- [ ] Валидация пройдена (ValidateGraph())

---

## ?? Пример: Комплексный диалог с ветвлениями

### Сценарий: "Подарок для Lianne"

```
??????????????
?   Start    ?  "Oh, is that... for me?"
??????????????
       ?
       ???????????????????????????????????????????
       ?                   ?                     ?
  ???????????        ????????????         ????????????
  ?Give Rose?        ?Give Gold ?         ?Give Nothing?
  ? +10 aff ?        ?  +5 aff  ?         ?  -5 aff  ?
  ???????????        ????????????         ????????????
       ?                   ?                     ?
       ?                   ?                     ?
  ???????????        ????????????         ????????????
  ? Shy &   ?        ? Polite   ?         ? Hurt &   ?
  ? Happy   ?        ? Thank    ?         ? Sad      ?
  ? +5 rom  ?        ?          ?         ?          ?
  ???????????        ????????????         ????????????
       ?                   ?                     ?
       ???????????????????????????????????????????
                           ?
                           ?
                      ???????????
                      ?   End   ?
                      ???????????
```

### Код узлов:

**Start:**
```
NodeId: Start
NodeType: NpcLine
DialogueText: "Oh, is that... for me? *notices item in your hand*"
EmotionTag: Emotion.Curious
```

**Choice_GiveRose:**
```
NodeId: Choice_GiveRose
NodeType: PlayerChoice
ChoiceText: "Yes, I picked this rose for you"
ConditionExpression: "hasItem(Rose)"
EffectExpression: "removeItem(Rose); affinity[Lianne] += 10; romance[Lianne] += 5; setMemory(GaveRose)"
ChoiceIcon: Icon_Rose
```

**Choice_GiveGold:**
```
NodeId: Choice_GiveGold
NodeType: PlayerChoice
ChoiceText: "Here, take some gold"
ConditionExpression: "gold >= 50"
EffectExpression: "gold -= 50; affinity[Lianne] += 5"
ChoiceIcon: Icon_Gold
```

**Choice_GiveNothing:**
```
NodeId: Choice_GiveNothing
NodeType: PlayerChoice
ChoiceText: "Actually, never mind"
EffectExpression: "affinity[Lianne] -= 5"
```

**Response_Rose:**
```
NodeId: Response_Rose
NodeType: NpcLine
DialogueText: "A rose? That's... really sweet of you! I love roses... *blushes*"
EmotionTag: Emotion.Shy
PauseAfter: 2.0
```

**Response_Gold:**
```
NodeId: Response_Gold
NodeType: NpcLine
DialogueText: "Oh, thank you! That's very kind, though you didn't have to..."
EmotionTag: Emotion.Grateful
```

**Response_Nothing:**
```
NodeId: Response_Nothing
NodeType: NpcLine
DialogueText: "Oh... I see. *looks down sadly*"
EmotionTag: Emotion.Hurt
```

---

## ?? Продвинутые техники

### 1. **Conditional Start Nodes**

Разные начала диалога в зависимости от контекста:

```cpp
// В DialogueAsset
ConditionalStartNodes.Add("affinity[Lianne] >= 70", TEXT("Start_Friend"));
ConditionalStartNodes.Add("affinity[Lianne] < 0", TEXT("Start_Enemy"));
DefaultStartNodeId = TEXT("Start_Neutral");
```

**Start_Friend:**
```
"Hey! Good to see you again! How have you been?"
EmotionTag: Emotion.Happy
```

**Start_Neutral:**
```
"Hello. What can I do for you?"
EmotionTag: Emotion.Neutral
```

**Start_Enemy:**
```
"You again? What do you want?"
EmotionTag: Emotion.Angry
```

### 2. **Random Variations**

Добавить вариативность в реплики:

```cpp
Node_Random:
  NodeType: Random
  
Edges:
  Node_Random ? Variation_1 (Weight: 50)
  Node_Random ? Variation_2 (Weight: 30)
  Node_Random ? Variation_3 (Weight: 20)
```

**Variation_1:**
```
"The weather is nice today, isn't it?"
```

**Variation_2:**
```
"Have you been to the market yet?"
```

**Variation_3:**
```
"I heard there's a festival coming up!"
```

### 3. **Condition Chains**

Комплексные условия:

```cpp
ConditionExpression:
  "affinity[Lianne] >= 50 && hasItem(Rose) && time == Evening && !memory(GaveRoseToday)"
```

**Логика**:
- Affinity минимум 50 (друзья)
- У игрока есть роза
- Сейчас вечер
- Ещё не дарил розу сегодня

### 4. **Multi-Stage Effects**

Последовательность эффектов:

```cpp
EffectExpression:
  "affinity[Lianne] += 10; romance[Lianne] += 5; setMemory(ConfessedFeelings); startQuest(DateWithLianne); gold += 100"
```

**Что произойдёт**:
1. +10 Affinity
2. +5 Romance
3. Флаг памяти установлен
4. Начат квест
5. Награда 100 золота

### 5. **One-Time Events**

Диалоги, которые показываются только раз:

```cpp
Node_FirstMeeting:
  bOneTimeOnly: true
  ConditionExpression: "!visited(Node_FirstMeeting)"
```

**Применение**:
- Cutscene диалоги
- Квестовые триггеры
- Уникальные события

---

## ?? Работа с эмоциями и анимациями

### Таблица эмоций

| EmotionTag | Анимация лица | Цвет UI | Иконка |
|------------|---------------|---------|--------|
| Emotion.Neutral | Default | White | ?? |
| Emotion.Happy | Smile_01 | Yellow | ?? |
| Emotion.Sad | Frown_01 | Blue | ?? |
| Emotion.Angry | Angry_01 | Red | ?? |
| Emotion.Shy | Blush_01 | Pink | ?? |
| Emotion.Surprised | Shocked_01 | Orange | ?? |
| Emotion.Curious | Think_01 | Purple | ?? |

### Настройка Presentation Data

```cpp
Node_Example:
  Presentation:
    EmotionTag: Emotion.Shy
    Intensity: Strong
    AnimationName: Blush_Intense
    VoiceAudio: VO_Lianne_Shy_01
    PauseAfter: 1.5
    bFocusCamera: true
    CameraDistance: 150.0
```

**Результат**:
- NPC покраснеет (анимация)
- Камера приблизится до 150 units
- Воспроизведётся аудио
- После реплики — пауза 1.5 сек

---

## ?? Debugging Tips

### Console Commands

```cpp
// Показать текущий Context
dialogue.dumpcontext

// Показать активный узел
dialogue.showcurrentnode

// Пропустить к узлу
dialogue.jumptonode NodeId

// Установить Affinity
dialogue.setaffinity Lianne 100

// Очистить память
dialogue.clearmemory

// Reload диалога
dialogue.reload
```

### Debug Draw

В `DialogueRunner::ProcessNode()`:

```cpp
#if !UE_BUILD_SHIPPING
    if (CVarDialogueDebug.GetValueOnGameThread())
    {
        DrawDebugString(
            GetWorld(),
            NPC->GetActorLocation() + FVector(0, 0, 100),
            *CurrentNode->Data.NodeId.ToString(),
            nullptr,
            FColor::Green,
            5.0f
        );
    }
#endif
```

### Validation Errors

Запустить валидацию:

```cpp
TArray<FString> Errors;
bool bValid = DialogueAsset->ValidateGraph(Errors);

if (!bValid)
{
    for (const FString& Error : Errors)
    {
        UE_LOG(LogDialogue, Error, TEXT("%s"), *Error);
    }
}
```

**Типичные ошибки**:
- "Node 'X' has no outgoing edges"
- "Edge references non-existent node 'Y'"
- "Circular dependency detected: X ? Y ? X"
- "Start node not found"

---

## ?? Performance Monitoring

### Stat Commands

```cpp
stat dialogue
stat dialogueai
stat dialogueui
```

### Profiling Markers

```cpp
SCOPE_CYCLE_COUNTER(STAT_DialogueRunner_ProcessNode);

void UDialogueRunner::ProcessNode(UDialogueNode* Node)
{
    SCOPE_CYCLE_COUNTER(STAT_DialogueRunner_ProcessNode);
    
    // ... logic ...
}
```

### Memory Tracking

```cpp
// Замер размера кэша
int32 CacheSize = NodeCache.Num() * sizeof(UDialogueNode*);
UE_LOG(LogDialogue, Log, TEXT("Node cache size: %d KB"), CacheSize / 1024);
```

---

## ?? Localization Setup

### 1. Настроить Localization Dashboard

1. `Window ? Localization Dashboard`
2. Создать новый `Dialogue` target
3. Добавить культуры: `en`, `ru`, `ja`, и т.д.

### 2. Пометить тексты для локализации

```cpp
// В DataTable
DialogueText: NSLOCTEXT("Dialogue", "Lianne_FirstMeeting_Start", "Hello! I haven't seen you around here before...")
```

### 3. Gather & Compile

```cpp
// Command line
UnrealEditor-Cmd.exe ProjectName -Run=GatherText -Config=Config/Localization/Dialogue.ini
```

### 4. Перевести в Локализационных файлах

```
Content/Localization/Dialogue/ru/Dialogue.po:

msgid "Hello! I haven't seen you around here before..."
msgstr "Привет! Я не видела тебя здесь раньше..."
```

---

**Следующие шаги**:
- Прочитать `/Docs/DSL_Reference.md` для полного синтаксиса DSL
- Прочитать `/Docs/AI_Integration.md` для AI генерации
- Посмотреть `/Content/Examples/` для готовых примеров

**Версия**: 1.0
