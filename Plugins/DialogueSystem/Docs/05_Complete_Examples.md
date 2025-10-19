# Complete Integration Examples

## Обзор

Этот документ содержит полные пошаговые примеры интеграции всей системы диалогов в ваш проект Unreal Engine 5. Примеры покрывают все основные сценарии использования от простого диалога до сложных AI-генерируемых разговоров с системой отношений.

---

## Пример 1: Базовый диалог с NPC (От начала до конца)

### Цель
Создать простой диалог с NPC "Лианна", который приветствует игрока и предлагает несколько вариантов ответа.

### Шаг 1: Подготовка проекта

1. **Активация плагина**:
   - `Edit ? Plugins ? Search "Dialogue System"`
   - Поставьте галочку на плагине
   - Перезапустите редактор

2. **Создание папок**:
   ```
   Content/
   ??? Dialogues/          # Data Assets диалогов
   ??? NPCs/               # Blueprints NPC
   ?   ??? Lianne/
   ??? UI/                 # UI виджеты
   ?   ??? Dialogue/
   ??? Data/               # Конфигурация
       ??? Personas/
   ```

### Шаг 2: Создание Data Asset диалога

1. **Создание ассета**:
   - `Right Click в Content/Dialogues ? Miscellaneous ? Data Asset`
   - Выберите класс: `DialogueDataAsset`
   - Назовите: `DA_Dialogue_Lianne_Greeting`

2. **Заполнение узлов**:

```
Nodes[0]: (Приветствие NPC)
  - Node Id: "node_greeting"
  - Node Type: Dialogue
  - Dialogue Text: "Привет! Я рада тебя видеть! Как дела?"
  - Speaker Name: "Lianne"
  - Speaker Portrait: T_Portrait_Lianne
  - Animation Name: "Talk_Happy"
  - Camera Tag: "Camera.Closeup"

Nodes[1]: (Выбор игрока)
  - Node Id: "node_player_choice"
  - Node Type: PlayerChoice
  - Dialogue Text: "" (пусто)

Nodes[2]: (Дружелюбный ответ)
  - Node Id: "node_response_friendly"
  - Node Type: Dialogue
  - Dialogue Text: "Отлично! Я тоже рада тебя видеть. Давай поговорим!"
  - Speaker Name: "Lianne"
  - Animation Name: "Talk_Happy"

Nodes[3]: (Нейтральный ответ)
  - Node Id: "node_response_neutral"
  - Node Type: Dialogue
  - Dialogue Text: "Хорошо. Чем могу помочь?"
  - Speaker Name: "Lianne"
  - Animation Name: "Talk_Neutral"

Nodes[4]: (Грубый ответ)
  - Node Id: "node_response_rude"
  - Node Type: Dialogue
  - Dialogue Text: "Ладно... Тогда до встречи."
  - Speaker Name: "Lianne"
  - Animation Name: "Talk_Sad"
```

3. **Создание связей**:

```
Connections[0]: (Автопереход к выбору)
  - From Node Id: "node_greeting"
  - To Node Id: "node_player_choice"
  - Choice Text: "" (автопереход)

Connections[1]: (Дружелюбный выбор)
  - From Node Id: "node_player_choice"
  - To Node Id: "node_response_friendly"
  - Choice Text: "Привет, Лианна! Очень рад тебя видеть!"
  - Effect: "affinity[Lianne] += 10; memory(FirstMeeting, true)"
  - Priority: 1

Connections[2]: (Нейтральный выбор)
  - From Node Id: "node_player_choice"
  - To Node Id: "node_response_neutral"
  - Choice Text: "Привет. Всё нормально."
  - Effect: "affinity[Lianne] += 2"
  - Priority: 2

Connections[3]: (Грубый выбор)
  - From Node Id: "node_player_choice"
  - To Node Id: "node_response_rude"
  - Choice Text: "Мне некогда. Пока."
  - Effect: "affinity[Lianne] -= 5"
  - Priority: 3
```

4. **Установка начального узла**:
   ```
   Start Node Id: "node_greeting"
   ```

### Шаг 3: Создание NPC Blueprint

1. **Создание Blueprint**:
   - `Right Click в Content/NPCs/Lianne ? Blueprint Class ? Character`
   - Назовите: `BP_NPC_Lianne`

2. **Добавление компонентов**:

```blueprintue
// В BP_NPC_Lianne, Components Panel:

Add Component:
  ? DialogueComponent
  ? RelationshipComponent
  ? NPCMemoryComponent
  ? CapsuleComponent (для интеракции)
```

3. **Настройка DialogueComponent**:

```
DialogueComponent (Details Panel):
  - Character Id: "Lianne"
  - Display Name: "Лианна"
  - Portrait: T_Portrait_Lianne
  - Default Dialogue: DA_Dialogue_Lianne_Greeting
  - Can Start Dialogue: true
  - Interaction Distance: 300.0
```

4. **Настройка RelationshipComponent**:

```
RelationshipComponent (Details Panel):
  - Affinity: 0.0
  - Trust: 0.0
  - Romance: 0.0
  - Status: Neutral
```

### Шаг 4: Создание UI виджета

1. **Создание ChoiceWidget**:
   - `Right Click в Content/UI/Dialogue ? User Interface ? Widget Blueprint`
   - Parent Class: `ChoiceWidget`
   - Назовите: `WBP_ChoiceButton`

2. **Настройка иерархии WBP_ChoiceButton**:

```
Button (Name: "ChoiceButton") **REQUIRED**
??? Overlay
    ??? Background (Image)
    ?   - Color: (R=0.1, G=0.1, B=0.1, A=0.8)
    ??? HotkeyText (Text Block) [Name: "HotkeyText"] *Optional*
    ?   - Text: "[1]"
    ?   - Color: Yellow
    ?   - Position: Top-Left
    ??? ChoiceText (Text Block) [Name: "ChoiceText"] **REQUIRED**
        - Text: "Choice Text Here"
        - Font: Roboto, Size 16
        - Color: White
        - Padding: (10, 5, 10, 5)
```

3. **Создание DialogueWidget**:
   - `Right Click в Content/UI/Dialogue ? User Interface ? Widget Blueprint`
   - Parent Class: `DialogueWidget`
   - Назовите: `WBP_DialogueWidget`

4. **Настройка иерархии WBP_DialogueWidget**:

```
Canvas Panel
??? Background (Image)
?   - Brush: SolidColor
?   - Color: (R=0, G=0, B=0, A=0.7)
?   - Anchors: Fill Screen
?
??? MainContainer (Vertical Box)
?   - Anchors: Bottom Center
?   - Position: (0, -200)
?   - Size: (800, 400)
?   ?
?   ??? PortraitContainer (Horizontal Box)
?   ?   ??? SpeakerPortrait (Image) [Name: "SpeakerPortrait"] **REQUIRED**
?   ?       - Size: (100, 100)
?   ?       - Brush: T_Portrait_Placeholder
?   ?
?   ??? TextContainer (Vertical Box)
?   ?   ??? SpeakerName (Text Block) [Name: "SpeakerName"] **REQUIRED**
?   ?   ?   - Text: "Speaker Name"
?   ?   ?   - Font: Roboto-Bold, Size 20
?   ?   ?   - Color: Yellow
?   ?   ?
?   ?   ??? DialogueText (Rich Text Block) [Name: "DialogueText"] **REQUIRED**
?   ?       - Text: "Dialogue text goes here..."
?   ?       - Font: Roboto, Size 16
?   ?       - Color: White
?   ?       - Auto Wrap Text: true
?   ?
?   ??? ChoicesContainer (Vertical Box) [Name: "ChoicesContainer"] **REQUIRED**
?       - Padding: (0, 20, 0, 0)
```

5. **Настройка в Details Panel WBP_DialogueWidget**:

```
DialogueWidget (Details Panel):
  - Choice Widget Class: WBP_ChoiceButton
  - Enable Typewriter: true
  - Show Portrait: true
  - Show Emotions: false
```

6. **Создание анимаций в WBP_DialogueWidget**:

```
// Animations Panel ? New Animation: "Intro_Anim" (0.5 sec)
Track: Canvas Panel
  - 0.0s: Render Opacity = 0.0, Translation Y = -50
  - 0.5s: Render Opacity = 1.0, Translation Y = 0

// New Animation: "Outro_Anim" (0.5 sec)
Track: Canvas Panel
  - 0.0s: Render Opacity = 1.0, Translation Y = 0
  - 0.5s: Render Opacity = 0.0, Translation Y = 50
```

7. **Реализация Blueprint Events в WBP_DialogueWidget**:

```blueprintue
// Event Graph:

// Event: Play Intro Animation
Event Play Intro Animation
    ? Play Animation [Intro_Anim]

// Event: Play Outro Animation
Event Play Outro Animation
    ? Play Animation [Outro_Anim]
```

### Шаг 5: Интеграция с игроком

1. **В BP_ThirdPersonCharacter (или вашем игроке)**:

```blueprintue
// Variables:
- DialogueWidgetInstance (UDialogueWidget*)
- DialogueWidgetClass (TSubclassOf<UDialogueWidget>) [Default: WBP_DialogueWidget]
- CurrentDialogueNPC (AActor*)

// Event BeginPlay:
Event BeginPlay
    ? Parent: BeginPlay

// Input Action: Interact (E)
Event InputAction Interact
    ? Get Overlapping Actors (Self) ? Actors
    ? For Each Loop (Actors)
        ? Cast to BP_NPC_Lianne ? LianneNPC
        ? Branch (Cast Success)
            True:
                ? Set CurrentDialogueNPC (LianneNPC)
                ? LianneNPC ? Get Component by Class [DialogueComponent] ? DialogueComp
                ? DialogueComp ? Start Dialogue (Self)
                ? Branch (return value)
                    True:
                        ? Show Dialogue Widget
                    False:
                        ? Print String "Cannot start dialogue"
```

2. **Custom Function: Show Dialogue Widget**:

```blueprintue
Function: Show Dialogue Widget

// Inputs: (none)
// Outputs: (none)

// Implementation:
? Branch (DialogueWidgetInstance == null)
    True:
        ? Create Widget [DialogueWidgetClass] ? DialogueWidgetInstance
        ? Add to Viewport (DialogueWidgetInstance)
    
? Get Game Instance
    ? Get Subsystem [DialogueSubsystem]
    ? Get Active Dialogue ? ActiveRunner

? DialogueWidgetInstance ? Bind To Runner (ActiveRunner)
? DialogueWidgetInstance ? Play Intro Animation

? Get Player Controller
    ? Set Input Mode UI Only
    ? Set Show Mouse Cursor (true)
```

3. **Custom Function: Hide Dialogue Widget**:

```blueprintue
Function: Hide Dialogue Widget

// Inputs: (none)
// Outputs: (none)

// Implementation:
? Branch (DialogueWidgetInstance != null)
    True:
        ? DialogueWidgetInstance ? Unbind From Runner
        ? DialogueWidgetInstance ? Play Outro Animation
        ? Delay (0.5)
        ? Remove from Parent (DialogueWidgetInstance)
        ? Set DialogueWidgetInstance = null
        
        ? Get Player Controller
            ? Set Input Mode Game Only
            ? Set Show Mouse Cursor (false)
```

### Шаг 6: Обработка событий диалога

```blueprintue
// В BP_NPC_Lianne, Event Graph:

// Variables:
- DialogueComp (UDialogueComponent*)

Event BeginPlay
    ? Parent: BeginPlay
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    
    // Подписка на события
    ? DialogueComp ? On Dialogue Started
        ? Add Event [Handle Dialogue Started]
    
    ? DialogueComp ? On Dialogue Ended
        ? Add Event [Handle Dialogue Ended]

// Custom Event: Handle Dialogue Started
Event Handle Dialogue Started
    Runner (параметр)
    ? Print String "Dialogue Started with Lianne"
    ? Play Animation Montage [AM_Talk_Idle]
    ? Set AI State [InDialogue]

// Custom Event: Handle Dialogue Ended
Event Handle Dialogue Ended
    ? Print String "Dialogue Ended"
    ? Stop Animation Montage
    ? Set AI State [Idle]
    
    // Вызов Hide Dialogue Widget на игроке
    ? Get Player Character
        ? Cast to BP_ThirdPersonCharacter
        ? Hide Dialogue Widget
```

### Шаг 7: Тестирование

1. **Размещение NPC**:
   - Перетащите `BP_NPC_Lianne` в уровень
   - Разместите рядом с точкой спавна игрока

2. **Запуск игры**:
   - `Alt + P` (Play In Editor)
   - Подойдите к NPC
   - Нажмите `E` для начала диалога

3. **Проверка**:
   - Диалог должен появиться с анимацией
   - Портрет Лианны должен отобразиться
   - Должны появиться 3 кнопки выбора
   - При выборе должен измениться affinity (проверить через Debug)

---

## Пример 2: Диалог с условиями и памятью

### Цель
Создать диалог, который изменяется в зависимости от предыдущих действий игрока и его отношений с NPC.

### Шаг 1: Создание диалога с условиями

1. **Создание Data Asset**:
   - Создайте `DA_Dialogue_Lianne_Conditional`

2. **Заполнение узлов**:

```
Nodes[0]: (Первая встреча)
  - Node Id: "node_first_meeting"
  - Node Type: Dialogue
  - Dialogue Text: "Привет! Я Лианна. Приятно познакомиться!"
  - Speaker Name: "Lianne"

Nodes[1]: (Повторная встреча)
  - Node Id: "node_returning"
  - Node Type: Dialogue
  - Dialogue Text: "О, ты вернулся! Рада снова тебя видеть!"
  - Speaker Name: "Lianne"

Nodes[2]: (Высокие отношения)
  - Node Id: "node_high_affinity"
  - Node Type: Dialogue
  - Dialogue Text: "Привет, дорогой! Я так по тебе скучала! ??"
  - Speaker Name: "Lianne"

Nodes[3]: (Выбор игрока)
  - Node Id: "node_choice"
  - Node Type: PlayerChoice
```

3. **Создание связей с условиями**:

```
Connections[0]: (Первая встреча - только если не встречались)
  - From Node Id: "START" (виртуальный начальный узел)
  - To Node Id: "node_first_meeting"
  - Condition: "!memory(MetLianne)"
  - Effect: "memory(MetLianne, true)"

Connections[1]: (Повторная встреча - низкие отношения)
  - From Node Id: "START"
  - To Node Id: "node_returning"
  - Condition: "memory(MetLianne) && affinity[Lianne] < 50"

Connections[2]: (Высокие отношения)
  - From Node Id: "START"
  - To Node Id: "node_high_affinity"
  - Condition: "memory(MetLianne) && affinity[Lianne] >= 50"

Connections[3-5]: (Переходы к выбору)
  - From Node Id: "node_first_meeting" / "node_returning" / "node_high_affinity"
  - To Node Id: "node_choice"
```

### Шаг 2: Обновление BP_NPC_Lianne

```blueprintue
// В BP_NPC_Lianne:

// Изменить Default Dialogue:
DialogueComponent:
  - Default Dialogue: DA_Dialogue_Lianne_Conditional
```

### Шаг 3: Тестирование памяти

```blueprintue
// В BP_ThirdPersonCharacter, добавить Debug функцию:

// Input Action: Debug Affinity (F1)
Event InputAction Debug Affinity
    ? Get Actor of Class [BP_NPC_Lianne] ? Lianne
    ? Lianne ? Get Component by Class [RelationshipComponent] ? RelComp
    ? RelComp ? Get Affinity ? Affinity
    ? Print String ("Affinity: " + ToString(Affinity))
    
    ? Lianne ? Get Component by Class [NPCMemoryComponent] ? MemoryComp
    ? MemoryComp ? Get Memory ("MetLianne") ? HasMet
    ? Print String ("Has Met: " + ToString(HasMet))
```

---

## Пример 3: Система квестов через диалоги

### Цель
Создать диалог, который стартует квест, отслеживает прогресс и завершает квест.

### Шаг 1: Создание квестового диалога

```
Nodes[0]: (Предложение квеста)
  - Node Id: "node_quest_offer"
  - Node Type: Dialogue
  - Dialogue Text: "Можешь помочь мне? Мне нужно найти редкий цветок в лесу."
  - Speaker Name: "Lianne"

Nodes[1]: (Выбор: принять/отклонить)
  - Node Id: "node_quest_choice"
  - Node Type: PlayerChoice

Nodes[2]: (Квест принят)
  - Node Id: "node_quest_accepted"
  - Node Type: Dialogue
  - Dialogue Text: "Спасибо! Ты мне очень поможешь!"
  - Speaker Name: "Lianne"

Nodes[3]: (Квест отклонен)
  - Node Id: "node_quest_declined"
  - Node Type: Dialogue
  - Dialogue Text: "Ох... Ну ладно, может в другой раз."
  - Speaker Name: "Lianne"

Nodes[4]: (Квест в прогрессе)
  - Node Id: "node_quest_progress"
  - Node Type: Dialogue
  - Dialogue Text: "Нашел цветок?"
  - Speaker Name: "Lianne"

Nodes[5]: (Квест завершен)
  - Node Id: "node_quest_complete"
  - Node Type: Dialogue
  - Dialogue Text: "Ты нашел его! Большое спасибо! Вот, возьми награду."
  - Speaker Name: "Lianne"

Connections:
[0]: START ? node_quest_offer
     Condition: "!memory(QuestOffered)"
     
[1]: node_quest_offer ? node_quest_choice

[2]: node_quest_choice ? node_quest_accepted
     Choice Text: "Конечно, я помогу!"
     Effect: "startQuest(FindFlower); memory(QuestOffered, true); memory(QuestActive, true); affinity[Lianne] += 10"

[3]: node_quest_choice ? node_quest_declined
     Choice Text: "Извини, я занят."
     Effect: "memory(QuestOffered, true); affinity[Lianne] -= 5"

[4]: START ? node_quest_progress
     Condition: "memory(QuestActive) && !hasItem(RareFlower)"

[5]: START ? node_quest_complete
     Condition: "memory(QuestActive) && hasItem(RareFlower)"
     Effect: "completeQuest(FindFlower); removeItem(RareFlower, 1); gold += 100; affinity[Lianne] += 20; memory(QuestActive, false); memory(QuestCompleted, true)"
```

### Шаг 2: Интеграция с системой квестов

```cpp
// В вашем DialogueService (C++):

void UMyDialogueService::StartQuest(const FString& QuestId)
{
    UQuestManager* QuestManager = GetGameInstance()->GetSubsystem<UQuestManager>();
    if (QuestManager)
    {
        QuestManager->StartQuest(FName(*QuestId));
        UE_LOG(LogDialogue, Log, TEXT("Quest started: %s"), *QuestId);
    }
}

void UMyDialogueService::CompleteQuest(const FString& QuestId)
{
    UQuestManager* QuestManager = GetGameInstance()->GetSubsystem<UQuestManager>();
    if (QuestManager)
    {
        QuestManager->CompleteQuest(FName(*QuestId));
        UE_LOG(LogDialogue, Log, TEXT("Quest completed: %s"), *QuestId);
    }
}

bool UMyDialogueService::HasItem(const FString& ItemName, int32 Count) const
{
    UInventoryComponent* Inventory = GetPlayerInventory();
    if (Inventory)
    {
        return Inventory->HasItem(FName(*ItemName), Count);
    }
    return false;
}
```

---

## Пример 4: AI-генерируемые диалоги

### Цель
Создать динамический диалог, где ответы NPC генерируются с помощью AI (OpenAI GPT).

### Шаг 1: Настройка API ключа

1. **Создание .env файла**:
   - В корне проекта создайте `Config/.env`
   - Добавьте: `OPENAI_API_KEY=sk-proj-your-key-here`

2. **Настройка в Project Settings**:
   ```
   Edit ? Project Settings ? Plugins ? Dialogue System ? AI:
     - API Key: (загрузится из .env)
     - Model: "gpt-4"
     - Temperature: 0.7
     - Max Tokens: 150
   ```

### Шаг 2: Создание Persona Data

1. **Создание Data Asset**:
   - `Right Click ? Miscellaneous ? Data Asset`
   - Класс: `DialoguePersonaData`
   - Назовите: `DA_PersonaData_Lianne`

2. **Заполнение**:

```
Character Name: "Лианна"

Description: 
"Молодая художница, 25 лет. Добрая, мечтательная и творческая личность. 
Любит природу, цветы и живопись. Немного застенчивая, но искренняя."

Personality:
  - "Добрая"
  - "Творческая"
  - "Романтичная"
  - "Застенчивая"
  - "Оптимистичная"
  - "Чуткая"

Backstory:
"Лианна выросла в маленькой прибрежной деревне. С детства мечтала стать художницей. 
Переехала в город, чтобы учиться в художественной школе. Сейчас живет одна в маленькой 
студии и работает над своими картинами. Мечтает о большой выставке."

Speech Style:
"Говорит мягко и нежно. Часто использует художественные метафоры. 
Улыбается при разговоре. Иногда становится задумчивой и мечтательной."

Example Dialogues:
  [0]: "User: Привет! ? Lianne: О, привет! *улыбается* Рада тебя видеть! Сегодня такой прекрасный день, правда?"
  [1]: "User: Как дела? ? Lianne: Прекрасно! Сегодня утром рисовала закат... он был таким вдохновляющим! Цвета... *мечтательно* потрясающие."
  [2]: "User: Что тебе нравится? ? Lianne: Мне нравится рисовать природу. Особенно цветы... они такие нежные и красивые."
```

### Шаг 3: Создание AI Dialogue Component

```blueprintue
// В BP_NPC_Lianne, добавить:

// Variables:
- AIProvider (UAIDialogueProvider_OpenAI*)
- PersonaData (FDialoguePersonaData) [Load from DA_PersonaData_Lianne]
- bUseAIDialogue (bool) [Default: true]

Event BeginPlay
    ? Parent: BeginPlay
    
    ? Branch (bUseAIDialogue)
        True:
            ? Create Object [UAIDialogueProvider_OpenAI] ? AIProvider
            ? Load Persona Data from Asset ? PersonaData
            ? Print String "AI Dialogue Enabled"
```

### Шаг 4: Обработка свободного ввода игрока

```blueprintue
// В WBP_DialogueWidget, добавить:

// New Widget: Editable Text Box (Name: "PlayerInputBox")
PlayerInputBox:
  - Hint Text: "Type your message..."
  - On Text Committed: ? Handle Player Input

// Event: Handle Player Input
Event Handle Player Input
    Text (параметр)
    Commit Method (параметр)
    
    ? Branch (Commit Method == OnEnter)
        True:
            ? Get Current NPC ? NPC
            ? Cast to BP_NPC_Lianne ? LianneNPC
            ? LianneNPC ? Generate AI Response (Text)
            ? Clear Text (PlayerInputBox)

// В BP_NPC_Lianne:

Function: Generate AI Response
Inputs: UserPrompt (String)

// Implementation:
? Get Component by Class [DialogueComponent] ? DialogueComp
? DialogueComp ? Get Context ? Context

? AIProvider ? Generate Dialogue
    Persona Data: PersonaData
    Context: Context
    User Prompt: UserPrompt
    On Complete: [Bind to: OnAIResponse]

// Custom Event: OnAIResponse
Event OnAIResponse
    Response (параметр: FAIDialogueResponse)
    
    ? Branch (Response.bSuccess)
        True:
            // Создать динамический узел
            ? Create Dialogue Node ? NewNode
                - Dialogue Text: Response.GeneratedText
                - Speaker Name: "Lianne"
                - Tags: [Response.SuggestedEmotion]
            
            // Применить изменения отношений
            ? Get Component by Class [RelationshipComponent] ? RelComp
            ? RelComp ? Modify Affinity (Response.SuggestedAffinityDelta)
            
            // Отобразить в UI
            ? Get Player Character
                ? Cast to BP_ThirdPersonCharacter
                ? Get Dialogue Widget Instance
                ? Update Node Display (NewNode)
        
        False:
            ? Print String ("AI Error: " + Response.ErrorMessage)
            // Fallback на предустановленный диалог
            ? Use Default Dialogue
```

### Шаг 5: Продвинутый промпт

```cpp
// В C++ (UAIDialogueProvider_OpenAI::BuildPrompt_Implementation):

FString UAIDialogueProvider_OpenAI::BuildPrompt_Implementation(
    const FDialoguePersonaData& PersonaData,
    const UDialogueSessionContext* Context,
    const FString& UserPrompt) const
{
    // System Prompt
    FString SystemPrompt = FString::Printf(TEXT(
        "You are %s, a character in a video game.\n\n"
        "Character Description:\n%s\n\n"
        "Personality Traits: %s\n\n"
        "Backstory:\n%s\n\n"
        "Speech Style:\n%s\n\n"
        "Instructions:\n"
        "- Stay in character at all times\n"
        "- Respond naturally and conversationally\n"
        "- Keep responses concise (1-2 sentences)\n"
        "- Use the speech style described above\n"
        "- Show emotion through text (*smiles*, *looks down*, etc.)\n"
        "- Consider the current context\n\n"
    ),
    *PersonaData.CharacterName,
    *PersonaData.Description,
    *FString::Join(PersonaData.Personality, TEXT(", ")),
    *PersonaData.Backstory,
    *PersonaData.SpeechStyle
    );

    // Context Prompt
    FString ContextPrompt = TEXT("");
    if (Context)
    {
        float Affinity = Context->GetRelationshipValue(TEXT("Lianne"), TEXT("affinity"));
        FString TimeOfDay = Context->GetCurrentTimeString();
        
        ContextPrompt = FString::Printf(TEXT(
            "Current Context:\n"
            "- Time: %s\n"
            "- Your affinity with the player: %.0f/100 (%s)\n"
            "- Recent events: %s\n\n"
        ),
        *TimeOfDay,
        Affinity,
        Affinity > 70 ? TEXT("Very friendly") : (Affinity > 30 ? TEXT("Friendly") : TEXT("Neutral")),
        *Context->GetRecentEventsString()
        );
    }

    // Few-Shot Examples
    FString FewShotExamples = TEXT("Example Interactions:\n\n");
    for (const FString& Example : PersonaData.ExampleDialogues)
    {
        FewShotExamples += Example + TEXT("\n");
    }
    FewShotExamples += TEXT("\n");

    // Full Prompt
    FString FullPrompt = FString::Printf(TEXT(
        "%s%s%sUser: %s\n\n%s:"
    ),
    *SystemPrompt,
    *ContextPrompt,
    *FewShotExamples,
    *UserPrompt,
    *PersonaData.CharacterName
    );

    return FullPrompt;
}
```

---

## Пример 5: Полная интеграция с сохранением

### Цель
Создать систему с сохранением состояния диалогов, отношений и памяти NPC между сессиями игры.

### Шаг 1: Создание SaveGame класса

```cpp
// В C++ (MySaveGame.h):

UCLASS()
class UMySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    // Состояние диалогов
    UPROPERTY(VisibleAnywhere, Category = "Dialogue")
    TMap<FString, bool> DialogueMemories;

    UPROPERTY(VisibleAnywhere, Category = "Dialogue")
    TMap<FString, float> DialogueVariables;

    UPROPERTY(VisibleAnywhere, Category = "Dialogue")
    TArray<FName> CompletedDialogues;

    // Отношения с NPC
    UPROPERTY(VisibleAnywhere, Category = "Relationships")
    TMap<FString, float> NPCAffinityLevels;

    UPROPERTY(VisibleAnywhere, Category = "Relationships")
    TMap<FString, int32> NPCRelationshipStatus;

    // Память NPC
    UPROPERTY(VisibleAnywhere, Category = "Memory")
    TMap<FString, TMap<FString, bool>> NPCMemories;

    UPROPERTY(VisibleAnywhere, Category = "Memory")
    TMap<FString, TMap<FString, int32>> NPCNodeVisitCounts;

    // Метаданные
    UPROPERTY(VisibleAnywhere, Category = "Meta")
    FDateTime SaveTime;

    UPROPERTY(VisibleAnywhere, Category = "Meta")
    int32 SaveVersion = 1;
};
```

### Шаг 2: Реализация сохранения

```cpp
// В C++ (MyGameInstance или SaveSystem):

void UMyGameInstance::SaveGame()
{
    UMySaveGame* SaveGame = Cast<UMySaveGame>(
        UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass())
    );

    if (!SaveGame)
        return;

    // Сохранение состояния диалогов через Subsystem
    UDialogueSubsystem* DialogueSubsystem = GetSubsystem<UDialogueSubsystem>();
    if (DialogueSubsystem)
    {
        // Сохранить JSON представление
        FString DialogueStateJson = DialogueSubsystem->SaveStateToJson();
        // Парсить и сохранить в SaveGame структуры
        
        // Или использовать встроенное сохранение
        DialogueSubsystem->SaveDialogueState();
    }

    // Сохранение отношений
    TArray<AActor*> NPCs;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("NPC"), NPCs);
    
    for (AActor* Actor : NPCs)
    {
        URelationshipComponent* RelComp = Actor->FindComponentByClass<URelationshipComponent>();
        if (RelComp)
        {
            FString NPCId = RelComp->GetOwner()->GetName();
            SaveGame->NPCAffinityLevels.Add(NPCId, RelComp->GetAffinity());
            SaveGame->NPCRelationshipStatus.Add(NPCId, (int32)RelComp->GetStatus());
        }

        UNPCMemoryComponent* MemoryComp = Actor->FindComponentByClass<UNPCMemoryComponent>();
        if (MemoryComp)
        {
            FString NPCId = MemoryComp->GetOwner()->GetName();
            SaveGame->NPCMemories.Add(NPCId, MemoryComp->GetAllMemories());
            SaveGame->NPCNodeVisitCounts.Add(NPCId, MemoryComp->GetAllVisitCounts());
        }
    }

    // Метаданные
    SaveGame->SaveTime = FDateTime::Now();

    // Сохранить в слот
    UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("MainSaveSlot"), 0);
    
    UE_LOG(LogTemp, Log, TEXT("Game Saved at %s"), *SaveGame->SaveTime.ToString());
}
```

### Шаг 3: Реализация загрузки

```cpp
void UMyGameInstance::LoadGame()
{
    UMySaveGame* SaveGame = Cast<UMySaveGame>(
        UGameplayStatics::LoadGameFromSlot(TEXT("MainSaveSlot"), 0)
    );

    if (!SaveGame)
    {
        UE_LOG(LogTemp, Warning, TEXT("No save game found"));
        return;
    }

    // Загрузка состояния диалогов
    UDialogueSubsystem* DialogueSubsystem = GetSubsystem<UDialogueSubsystem>();
    if (DialogueSubsystem)
    {
        DialogueSubsystem->LoadDialogueState();
    }

    // Загрузка отношений (после spawn NPC)
    TArray<AActor*> NPCs;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("NPC"), NPCs);
    
    for (AActor* Actor : NPCs)
    {
        FString NPCId = Actor->GetName();

        URelationshipComponent* RelComp = Actor->FindComponentByClass<URelationshipComponent>();
        if (RelComp && SaveGame->NPCAffinityLevels.Contains(NPCId))
        {
            RelComp->SetAffinity(SaveGame->NPCAffinityLevels[NPCId]);
            RelComp->SetStatus((ERelationshipStatus)SaveGame->NPCRelationshipStatus[NPCId]);
        }

        UNPCMemoryComponent* MemoryComp = Actor->FindComponentByClass<UNPCMemoryComponent>();
        if (MemoryComp && SaveGame->NPCMemories.Contains(NPCId))
        {
            MemoryComp->LoadMemories(SaveGame->NPCMemories[NPCId]);
            MemoryComp->LoadVisitCounts(SaveGame->NPCNodeVisitCounts[NPCId]);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Game Loaded from %s"), *SaveGame->SaveTime.ToString());
}
```

### Шаг 4: Автосохранение

```blueprintue
// В BP_GameMode:

Event BeginPlay
    ? Set Timer by Function Name
        Function Name: "AutoSave"
        Time: 300.0 (каждые 5 минут)
        Looping: true

Function: AutoSave
? Get Game Instance
    ? Cast to MyGameInstance
    ? Save Game
? Print String "Auto-saved"
```

---

## Заключение

Эти примеры покрывают основные сценарии использования DialogueSystem от простых статических диалогов до сложных AI-генерируемых разговоров с сохранением состояния. Система полностью модульная и может быть адаптирована под любые нужды вашего проекта.

### Дополнительные ресурсы:

- `01_DialogueSystemCore_Module.md` — Подробности о ядре системы
- `02_DialogueSystemRuntime_Module.md` — Runtime компоненты и Subsystem
- `03_DialogueSystemUI_Module.md` — UI виджеты и их настройка
- `04_DialogueSystemAI_Module.md` — AI интеграция и промпт инжиниринг
- `DSL_Parser_Documentation.md` — Синтаксис DSL для условий и эффектов

### Советы по отладке:

1. **Включите логирование**:
   ```cpp
   LogDialogue: Log
   LogDialogueAI: Verbose
   ```

2. **Используйте Debug Draw**:
   - Отображайте текущий affinity level над NPC
   - Показывайте доступные диалоги

3. **Тестируйте пошагово**:
   - Начните с простого диалога
   - Постепенно добавляйте условия, эффекты, AI

4. **Профилируйте производительность**:
   - Используйте Unreal Insights
   - Проверьте статистику пула объектов

### Известные ограничения:

- AI запросы требуют интернет-соединения (если не используете локальные модели)
- Большие диалоги могут занимать много памяти
- Typewriter эффект может быть медленным для очень длинных текстов

Для поддержки и вопросов см. README.md в корне плагина.
