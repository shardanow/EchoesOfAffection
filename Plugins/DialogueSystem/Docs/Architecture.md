# Dialogue System - Архитектурная документация

## ?? Обзор системы

Модульная, расширяемая система диалогов для RPG/relationship simulator в Unreal Engine 5.
Спроектирована по принципам SOLID, data-driven, с полной поддержкой AI-генерации.

---

## ?? Архитектурные принципы

### 1. **Separation of Concerns (SoC)**
```
???????????????????????????????????????????????
?              PRESENTATION LAYER              ?
?  ????????????  ????????????  ????????????  ?
?  ?DialogueUI?  ?ChoiceUI  ?  ?HistoryUI ?  ?
?  ????????????  ????????????  ????????????  ?
???????????????????????????????????????????????
        ?             ?             ?
???????????????????????????????????????????????
?       ?             ?             ?         ?
?              APPLICATION LAYER               ?
?  ????????????????????????????????????????   ?
?  ?      DialogueRunner (State Machine)  ?   ?
?  ????????????????????????????????????????   ?
?       ?                             ?       ?
?  ????????????                  ???????????  ?
?  ?Condition ?                  ? Effect  ?  ?
?  ?Evaluator ?                  ?Executor ?  ?
?  ????????????                  ???????????  ?
????????????????????????????????????????????????
        ?                             ?
?????????????????????????????????????????????????
?       ?                             ?         ?
?                 DOMAIN LAYER                  ?
?  ????????????  ????????????  ????????????   ?
?  ? Context  ?  ?   Node   ?  ?  Asset   ?   ?
?  ????????????  ????????????  ????????????   ?
?????????????????????????????????????????????????
        ?                             ?
?????????????????????????????????????????????????
?       ?                             ?         ?
?              INFRASTRUCTURE LAYER             ?
?  ????????????  ????????????  ????????????   ?
?  ?  Cache   ?  ? Save/Load?  ? AI API   ?   ?
?  ????????????  ????????????  ????????????   ?
?????????????????????????????????????????????????
```

### 2. **Data-Driven Design**
- **Все данные в DataAssets/DataTables** ? нет hardcode
- **Runtime не знает о конкретных NPC** ? всё через интерфейсы
- **Авторинг отделён от логики** ? писатели работают независимо

### 3. **Event-Driven Architecture**
```cpp
// НЕТ Tick-based опросов
void TickComponent() { if (ShouldUpdate) {...} } // ?

// ЕСТЬ Event-driven реакции
OnNodeEntered.Broadcast(Node); // ?
```

### 4. **Dependency Injection**
```cpp
// НЕТ прямых зависимостей
URelationshipComponent* Comp = NPC->FindComponentByClass(); // ?

// ЕСТЬ интерфейсы и injection
IDialogueParticipant* Participant = Cast<IDialogueParticipant>(NPC); // ?
```

---

## ??? Структура модулей

### **DialogueSystemCore** (Pure Logic)
**Зависимости**: только `CoreMinimal`, `GameplayTags`

**Классы**:
- `UDialogueRunner` — state machine диалога
- `UDialogueNode` — узел графа
- `UDialogueContext` — контекст выполнения
- `UDialogueConditionEvaluator` — DSL для условий
- `UDialogueEffectExecutor` — DSL для эффектов
- `UDialogueDataAsset` — данные диалога

**Принципы**:
- ? Нет UI зависимостей
- ? Нет Actor зависимостей (только через интерфейсы)
- ? Pure functions где возможно
- ? Immutable data structures (где возможно)

### **DialogueSystemRuntime** (Components & Subsystems)
**Зависимости**: `Core`, `Engine`

**Классы**:
- `UDialogueComponent` — компонент для NPC
- `URelationshipComponent` — отношения и эмоции
- `UDialogueMemoryComponent` — долгосрочная память NPC
- `UDialogueSubsystem` — глобальный менеджер

**Принципы**:
- ? Интеграция с gameplay
- ? Replication-ready (для multiplayer)
- ? Save/Load через Serialize

### **DialogueSystemUI** (Widgets)
**Зависимости**: `Core`, `Runtime`, `UMG`, `Slate`

**Классы**:
- `UDialogueWidget` — главный UI
- `UChoiceWidget` — кнопка выбора
- `UHistoryWidget` — история диалогов
- `UDialogueViewModel` — MVVM логика

**Принципы**:
- ? MVVM pattern (UI не зависит от бизнес-логики)
- ? Анимации через Blueprint
- ? Локализация через FText

### **DialogueSystemAI** (AI Integration)
**Зависимости**: `Core`, `HTTP`, `Json`

**Классы**:
- `UAIDialogueProvider` — базовый провайдер
- `UAIDialogueProvider_OpenAI` — реализация для OpenAI
- `UPromptBuilder` — конструктор промптов
- `UAIResponseCache` — кэш ответов

**Принципы**:
- ? Async API calls (не блокируют игру)
- ? Fallback на статичный текст
- ? Rate limiting и error handling
- ? Провайдеры легко заменяемы

---

## ?? Workflow (Authoring ? Runtime ? UI)

### 1. **Authoring Phase** (Писатели/Дизайнеры)

#### Создание диалога в DataTable
```
File: DT_Dialogue_Lianne_FirstMeeting.uasset

Row 1: Node_Start
  - NodeId: Node_Start
  - NodeType: NpcLine
  - SpeakerId: Lianne
  - DialogueText: "Oh, hello! I haven't seen you around here before..."
  - ConditionExpression: "" (всегда доступен)
  - EffectExpression: "setMemory(FirstMeeting)"
  - EmotionTag: Emotion.Neutral

Row 2: Node_Choice_Intro
  - NodeType: PlayerChoice
  - ChoiceText: "Hi, I'm new to the village"
  - EffectExpression: "affinity[Lianne] += 5"

Row 3: Node_Response_Positive
  - NodeType: NpcLine
  - SpeakerId: Lianne
  - DialogueText: "Welcome! Let me show you around..."
  - ConditionExpression: "affinity[Lianne] >= 0"
  - EmotionTag: Emotion.Happy
```

#### Экспорт в Google Sheets (опционально)
```python
# Tools/ExportDialogueToSheets.py
dialogue_asset = LoadAsset("DT_Dialogue_Lianne_FirstMeeting")
sheets_client.export(dialogue_asset, "Dialogue_Lianne_Sheet")
```

### 2. **Runtime Phase** (Игрок взаимодействует)

#### Поток выполнения
```cpp
1. Игрок нажимает E рядом с NPC
   ?
2. InteractComponent::OnInteract() вызывается
   ?
3. NPC->DialogueComponent->StartDialogue(Player)
   ?
4. DialogueSubsystem->StartDialogue(Asset, Player, NPC)
   ?
5. DialogueRunner создаётся и инициализируется
   ?
6. Runner->StartDialogue() ? загружает граф, создаёт Context
   ?
7. Runner->ProcessNode(StartNode)
   ?
8. OnNodeEntered.Broadcast() ? UI слушает и отображает
   ?
9. Игрок выбирает вариант ? SelectChoice()
   ?
10. EffectExecutor->Execute() ? применяются эффекты
    ?
11. ComputeNextNode() ? переход к следующему узлу
    ?
12. (повторяется пока не достигнут End узел)
    ?
13. OnDialogueEnded.Broadcast() ? UI закрывается
```

### 3. **UI Phase** (Отображение)

#### Биндинг к Runner
```cpp
void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Получить активный Runner из Subsystem
    UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    UDialogueRunner* Runner = Subsystem->GetActiveDialogue();
    
    BindToRunner(Runner);
}

void UDialogueWidget::BindToRunner(UDialogueRunner* Runner)
{
    CurrentRunner = Runner;
    
    // Подписаться на события
    Runner->OnNodeEntered.AddDynamic(this, &UDialogueWidget::HandleNodeEntered);
    Runner->OnChoicesReady.AddDynamic(this, &UDialogueWidget::HandleChoicesReady);
    Runner->OnDialogueEnded.AddDynamic(this, &UDialogueWidget::HandleDialogueEnded);
}

void UDialogueWidget::HandleNodeEntered(const UDialogueNode* Node, const UDialogueContext* Context)
{
    // Обновить портрет
    UpdatePortrait(Node, Context);
    
    // Обновить имя
    SpeakerName->SetText(Node->GetSpeakerDisplayName(Context));
    
    // Запустить typewriter
    StartTypewriter(Node->GetResolvedText(Context));
    
    // Обновить эмоцию
    UpdateEmotion(Node->Data.Presentation.EmotionTag);
    
    // Воспроизвести аудио
    if (Node->Data.Presentation.VoiceAudio.IsValid())
    {
        PlayVoiceAudio(Node->Data.Presentation.VoiceAudio.Get());
    }
}
```

---

## ?? DSL (Domain Specific Language)

### Conditions DSL

#### Синтаксис
```
condition ::= expression
expression ::= term ( "&&" term | "||" term )*
term ::= "!" atom | atom
atom ::= comparison | function | boolean

comparison ::= identifier operator value
operator ::= "==" | "!=" | ">" | "<" | ">=" | "<="

function ::= name "(" args ")"

examples:
  "affinity[Lianne] >= 50"
  "hasItem(Rose) && time == Evening"
  "!visited(Node_002) || memory(FirstMeeting)"
  "romance[Lianne] > 70 && worldState(Festival.Started)"
```

#### Парсинг
```cpp
UDialogueCondition* UDialogueConditionEvaluator::ParseExpression(
    const TArray<FString>& Tokens, int32& Index)
{
    UDialogueCondition* Left = ParseTerm(Tokens, Index);
    
    while (Index < Tokens.Num())
    {
        if (Tokens[Index] == "&&")
        {
            Index++;
            UDialogueCondition* Right = ParseTerm(Tokens, Index);
            
            UDialogueCondition_And* AndNode = NewObject<UDialogueCondition_And>();
            AndNode->Conditions.Add(Left);
            AndNode->Conditions.Add(Right);
            Left = AndNode;
        }
        else if (Tokens[Index] == "||")
        {
            Index++;
            UDialogueCondition* Right = ParseTerm(Tokens, Index);
            
            UDialogueCondition_Or* OrNode = NewObject<UDialogueCondition_Or>();
            OrNode->Conditions.Add(Left);
            OrNode->Conditions.Add(Right);
            Left = OrNode;
        }
        else
        {
            break;
        }
    }
    
    return Left;
}
```

### Effects DSL

#### Синтаксис
```
effect ::= statement ( ";" statement )*
statement ::= assignment | function_call

assignment ::= identifier operator value
operator ::= "+=" | "-=" | "="

function_call ::= name "(" args ")"

examples:
  "affinity[Lianne] += 10"
  "giveItem(Rose, 1); gold -= 50"
  "setMemory(GaveGift); startQuest(HelpLianne)"
  "romance[Lianne] += 5; mood = Happy"
```

---

## ?? Save/Load System

### Архитектура сохранений
```
SaveGame
??? WorldState (Global)
?   ??? WorldStateTags
?   ??? QuestProgress
?   ??? TimeOfDay
??? RelationshipStates (Per-NPC)
?   ??? Lianne: {Affinity: 75, Trust: 60, Romance: 40}
?   ??? Marcus: {Affinity: 30, Trust: 80, Romance: 0}
?   ??? ...
??? DialogueHistory
?   ??? VisitedNodes: [Node_001, Node_005, ...]
?   ??? MemoryFlags: {FirstMeeting: true, GaveGift: true}
?   ??? CustomVariables: {QuestStep: "2", SecretRevealed: "true"}
??? PlayerState
    ??? Inventory: {Rose: 5, Sword: 1}
    ??? Gold: 1500
    ??? Level: 10
```

### Сериализация Context
```cpp
void UDialogueContext::SaveToJson(FString& OutJson) const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    
    // Serialize relationships
    TSharedPtr<FJsonObject> RelationshipsObj = MakeShareable(new FJsonObject);
    for (const auto& Pair : AffinityValues)
    {
        RelationshipsObj->SetNumberField(Pair.Key.ToString(), Pair.Value);
    }
    JsonObject->SetObjectField(TEXT("Relationships"), RelationshipsObj);
    
    // Serialize memory
    TArray<TSharedPtr<FJsonValue>> MemoryArray;
    for (const FName& Key : VisitedNodes)
    {
        MemoryArray.Add(MakeShareable(new FJsonValueString(Key.ToString())));
    }
    JsonObject->SetArrayField(TEXT("VisitedNodes"), MemoryArray);
    
    // Serialize inventory
    TSharedPtr<FJsonObject> InventoryObj = MakeShareable(new FJsonObject);
    for (const auto& Pair : PlayerInventory)
    {
        InventoryObj->SetNumberField(Pair.Key.ToString(), Pair.Value);
    }
    JsonObject->SetObjectField(TEXT("Inventory"), InventoryObj);
    
    // Serialize to string
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
}
```

---

## ?? AI Integration Guide

### Промпт структура
```
SYSTEM PROMPT:
You are Lianne, a shy but kind village girl. You are talking to a traveler.

PERSONALITY:
- Shy and reserved initially
- Warms up as affinity increases
- Loves flowers and nature
- Easily flustered by compliments

CURRENT CONTEXT:
- Time: Evening
- Location: Village Square
- Affinity: 75/100 (Friend)
- Player recently gave you a rose

CONVERSATION HISTORY:
Player: "Hi, I brought you this rose."
Lianne: "Oh! That's... really sweet of you. Thank you..."

INSTRUCTIONS:
Generate Lianne's next response (50-100 words). Show her growing comfort with the player.

RESPONSE:
```

### Использование
```cpp
void UDialogueRunner::ProcessAINode(UDialogueNode* Node)
{
    // Получить AI Provider из Subsystem
    UAIDialogueProvider* AIProvider = DialogueSubsystem->GetAIProvider();
    
    if (!AIProvider || !AIProvider->IsAPIAvailable())
    {
        // Fallback на статичный текст
        ProcessStaticNode(Node);
        return;
    }
    
    // Построить промпт
    FString Prompt = UPromptBuilder::BuildFullPrompt(
        CurrentDialogue->PrimaryPersona,
        CurrentContext,
        Node->Data.AIPromptHint
    );
    
    // Async запрос
    FAIDialogueResponseDelegate Callback;
    Callback.BindDynamic(this, &UDialogueRunner::HandleAIResponse);
    
    AIProvider->GenerateDialogue(
        CurrentDialogue->PrimaryPersona,
        CurrentContext,
        Node->Data.AIPromptHint,
        Callback
    );
}

void UDialogueRunner::HandleAIResponse(const FAIDialogueResponse& Response)
{
    if (Response.bSuccess)
    {
        // Обновить узел сгенерированным текстом
        CurrentNode->Data.DialogueText = Response.GeneratedText;
        CurrentNode->Data.Presentation.EmotionTag = Response.SuggestedEmotion;
        
        // Продолжить выполнение
        OnNodeEntered.Broadcast(CurrentNode, CurrentContext);
    }
    else
    {
        // Fallback или показать ошибку
        UE_LOG(LogDialogue, Warning, TEXT("AI Generation failed: %s"), *Response.ErrorMessage);
        ProcessStaticNode(CurrentNode);
    }
}
```

---

## ?? Performance Optimization

### 1. **Кэширование**
```cpp
// НЕТ: парсинг каждый раз
UDialogueCondition* Cond = Evaluator->ParseCondition("affinity >= 50"); // ?

// ЕСТЬ: кэшированный парсинг
UDialogueCondition* Cond = Evaluator->GetOrParseCondition("affinity >= 50"); // ?
```

### 2. **Object Pooling для UI**
```cpp
// Переиспользование кнопок выбора вместо Create/Destroy
TArray<UChoiceWidget*> WidgetPool;

UChoiceWidget* UDialogueWidget::GetOrCreateChoiceWidget()
{
    for (UChoiceWidget* Widget : WidgetPool)
    {
        if (!Widget->IsVisible())
        {
            return Widget; // Переиспользовать
        }
    }
    
    // Создать новый только если пул пуст
    UChoiceWidget* NewWidget = CreateWidget<UChoiceWidget>(this, ChoiceWidgetClass);
    WidgetPool.Add(NewWidget);
    return NewWidget;
}
```

### 3. **Lazy Loading DataAssets**
```cpp
// Soft Object Pointers вместо Hard References
UPROPERTY(EditAnywhere)
TSoftObjectPtr<UDialogueDataAsset> DialogueAsset;

// Загрузка по требованию
if (!DialogueAsset.IsValid())
{
    DialogueAsset.LoadSynchronous();
}
```

### 4. **Async Operations**
```cpp
// Все длительные операции в async tasks
FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
Streamable.RequestAsyncLoad(AssetPath, [this]()
{
    OnAssetLoaded();
});
```

---

## ?? Testing Strategy

### Unit Tests (Core Logic)
```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDialogueConditionTest, "DialogueSystem.Core.Condition.Affinity", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDialogueConditionTest::RunTest(const FString& Parameters)
{
    // Setup
    UDialogueContext* Context = NewObject<UDialogueContext>();
    Context->SetAffinity(TEXT("TestNPC"), 75.0f);
    
    UDialogueCondition_Affinity* Condition = NewObject<UDialogueCondition_Affinity>();
    Condition->NPCName = TEXT("TestNPC");
    Condition->MinValue = 50.0f;
    
    // Act
    bool Result = Condition->Evaluate(Context);
    
    // Assert
    TestTrue(TEXT("Affinity condition should pass"), Result);
    
    return true;
}
```

### Integration Tests (Runtime)
```cpp
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDialogueRunnerTest, "DialogueSystem.Runtime.Runner.Flow", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDialogueRunnerTest::RunTest(const FString& Parameters)
{
    // Создать тестовый граф
    UDialogueDataAsset* TestAsset = CreateTestDialogue();
    
    // Создать Runner
    UDialogueRunner* Runner = NewObject<UDialogueRunner>();
    bool Started = Runner->StartDialogue(TestAsset, TestPlayer, TestNPC);
    
    TestTrue(TEXT("Dialogue should start"), Started);
    TestNotNull(TEXT("Current node should exist"), Runner->GetCurrentNode());
    
    return true;
}
```

---

## ?? Security & Safety

### 1. **AI Safety**
```cpp
// NSFW фильтр
bool IsContentSafe(const FString& Text)
{
    static const TArray<FString> BlockedWords = {
        TEXT("explicit_word_1"),
        TEXT("explicit_word_2")
    };
    
    for (const FString& Word : BlockedWords)
    {
        if (Text.Contains(Word, ESearchCase::IgnoreCase))
        {
            return false;
        }
    }
    
    return true;
}

// Rate limiting
bool CanMakeAPIRequest()
{
    double CurrentTime = FPlatformTime::Seconds();
    if (CurrentTime - LastRequestTime < MinRequestInterval)
    {
        return false; // Too soon
    }
    
    LastRequestTime = CurrentTime;
    return true;
}
```

### 2. **Input Validation**
```cpp
bool ValidateNodeId(FName NodeId)
{
    // Проверка на инъекции
    FString IdStr = NodeId.ToString();
    if (IdStr.Contains(TEXT(";")))  // SQL-like injection
    {
        return false;
    }
    
    // Проверка длины
    if (IdStr.Len() > 64)
    {
        return false;
    }
    
    return true;
}
```

---

## ?? Best Practices Summary

### ? DO
- Использовать `FName` для ID (быстрее чем `FString`)
- Кэшировать распарсенные условия/эффекты
- Использовать `TSoftObjectPtr` для assets
- Делать все API calls асинхронно
- Логировать все ошибки
- Писать unit tests для core logic
- Использовать GameplayTags для категоризации
- Отделять данные от логики
- Использовать события вместо Tick
- Использовать интерфейсы вместо прямых зависимостей

### ? DON'T
- Не использовать Tick для диалогов
- Не хранить жёсткие ссылки на assets
- Не парсить DSL каждый раз
- Не блокировать игру на AI requests
- Не хранить API ключи в assets
- Не делать тяжёлые операции в UI thread
- Не забывать отписываться от делегатов
- Не использовать `FindComponentByClass` в runtime
- Не хардкодить строки
- Не игнорировать локализацию

---

## ?? Дополнительные ресурсы

### Документы в проекте
- `/Docs/DSL_Reference.md` — полный справочник по DSL
- `/Docs/AI_Integration.md` — гайд по AI
- `/Docs/Authoring_Guide.md` — для писателей
- `/Docs/API_Reference.md` — полный API reference

### Примеры
- `/Content/Examples/SimpleDialogue` — базовый диалог
- `/Content/Examples/BranchingDialogue` — разветвления
- `/Content/Examples/AIGeneratedDialogue` — AI интеграция
- `/Content/Examples/EmotionalDialogue` — эмоции и анимации

### Тулза для авторов
- `/Tools/DialogueEditor` — standalone редактор
- `/Tools/ExportToSheets.py` — экспорт в Google Sheets
- `/Tools/ImportFromSheets.py` — импорт из Sheets
- `/Tools/ValidateDialogues.py` — валидация графов

---

**Версия документа**: 1.0
**Последнее обновление**: 2024
**Автор**: Senior Game Designer & Technical Architect
