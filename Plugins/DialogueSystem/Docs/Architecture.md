# Dialogue System - ������������� ������������

## ?? ����� �������

���������, ����������� ������� �������� ��� RPG/relationship simulator � Unreal Engine 5.
�������������� �� ��������� SOLID, data-driven, � ������ ���������� AI-���������.

---

## ?? ������������� ��������

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
- **��� ������ � DataAssets/DataTables** ? ��� hardcode
- **Runtime �� ����� � ���������� NPC** ? �� ����� ����������
- **�������� ������ �� ������** ? �������� �������� ����������

### 3. **Event-Driven Architecture**
```cpp
// ��� Tick-based �������
void TickComponent() { if (ShouldUpdate) {...} } // ?

// ���� Event-driven �������
OnNodeEntered.Broadcast(Node); // ?
```

### 4. **Dependency Injection**
```cpp
// ��� ������ ������������
URelationshipComponent* Comp = NPC->FindComponentByClass(); // ?

// ���� ���������� � injection
IDialogueParticipant* Participant = Cast<IDialogueParticipant>(NPC); // ?
```

---

## ??? ��������� �������

### **DialogueSystemCore** (Pure Logic)
**�����������**: ������ `CoreMinimal`, `GameplayTags`

**������**:
- `UDialogueRunner` � state machine �������
- `UDialogueNode` � ���� �����
- `UDialogueContext` � �������� ����������
- `UDialogueConditionEvaluator` � DSL ��� �������
- `UDialogueEffectExecutor` � DSL ��� ��������
- `UDialogueDataAsset` � ������ �������

**��������**:
- ? ��� UI ������������
- ? ��� Actor ������������ (������ ����� ����������)
- ? Pure functions ��� ��������
- ? Immutable data structures (��� ��������)

### **DialogueSystemRuntime** (Components & Subsystems)
**�����������**: `Core`, `Engine`

**������**:
- `UDialogueComponent` � ��������� ��� NPC
- `URelationshipComponent` � ��������� � ������
- `UDialogueMemoryComponent` � ������������ ������ NPC
- `UDialogueSubsystem` � ���������� ��������

**��������**:
- ? ���������� � gameplay
- ? Replication-ready (��� multiplayer)
- ? Save/Load ����� Serialize

### **DialogueSystemUI** (Widgets)
**�����������**: `Core`, `Runtime`, `UMG`, `Slate`

**������**:
- `UDialogueWidget` � ������� UI
- `UChoiceWidget` � ������ ������
- `UHistoryWidget` � ������� ��������
- `UDialogueViewModel` � MVVM ������

**��������**:
- ? MVVM pattern (UI �� ������� �� ������-������)
- ? �������� ����� Blueprint
- ? ����������� ����� FText

### **DialogueSystemAI** (AI Integration)
**�����������**: `Core`, `HTTP`, `Json`

**������**:
- `UAIDialogueProvider` � ������� ���������
- `UAIDialogueProvider_OpenAI` � ���������� ��� OpenAI
- `UPromptBuilder` � ����������� ��������
- `UAIResponseCache` � ��� �������

**��������**:
- ? Async API calls (�� ��������� ����)
- ? Fallback �� ��������� �����
- ? Rate limiting � error handling
- ? ���������� ����� ���������

---

## ?? Workflow (Authoring ? Runtime ? UI)

### 1. **Authoring Phase** (��������/���������)

#### �������� ������� � DataTable
```
File: DT_Dialogue_Lianne_FirstMeeting.uasset

Row 1: Node_Start
  - NodeId: Node_Start
  - NodeType: NpcLine
  - SpeakerId: Lianne
  - DialogueText: "Oh, hello! I haven't seen you around here before..."
  - ConditionExpression: "" (������ ��������)
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

#### ������� � Google Sheets (�����������)
```python
# Tools/ExportDialogueToSheets.py
dialogue_asset = LoadAsset("DT_Dialogue_Lianne_FirstMeeting")
sheets_client.export(dialogue_asset, "Dialogue_Lianne_Sheet")
```

### 2. **Runtime Phase** (����� ���������������)

#### ����� ����������
```cpp
1. ����� �������� E ����� � NPC
   ?
2. InteractComponent::OnInteract() ����������
   ?
3. NPC->DialogueComponent->StartDialogue(Player)
   ?
4. DialogueSubsystem->StartDialogue(Asset, Player, NPC)
   ?
5. DialogueRunner �������� � ����������������
   ?
6. Runner->StartDialogue() ? ��������� ����, ������ Context
   ?
7. Runner->ProcessNode(StartNode)
   ?
8. OnNodeEntered.Broadcast() ? UI ������� � ����������
   ?
9. ����� �������� ������� ? SelectChoice()
   ?
10. EffectExecutor->Execute() ? ����������� �������
    ?
11. ComputeNextNode() ? ������� � ���������� ����
    ?
12. (����������� ���� �� ��������� End ����)
    ?
13. OnDialogueEnded.Broadcast() ? UI �����������
```

### 3. **UI Phase** (�����������)

#### ������� � Runner
```cpp
void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // �������� �������� Runner �� Subsystem
    UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    UDialogueRunner* Runner = Subsystem->GetActiveDialogue();
    
    BindToRunner(Runner);
}

void UDialogueWidget::BindToRunner(UDialogueRunner* Runner)
{
    CurrentRunner = Runner;
    
    // ����������� �� �������
    Runner->OnNodeEntered.AddDynamic(this, &UDialogueWidget::HandleNodeEntered);
    Runner->OnChoicesReady.AddDynamic(this, &UDialogueWidget::HandleChoicesReady);
    Runner->OnDialogueEnded.AddDynamic(this, &UDialogueWidget::HandleDialogueEnded);
}

void UDialogueWidget::HandleNodeEntered(const UDialogueNode* Node, const UDialogueContext* Context)
{
    // �������� �������
    UpdatePortrait(Node, Context);
    
    // �������� ���
    SpeakerName->SetText(Node->GetSpeakerDisplayName(Context));
    
    // ��������� typewriter
    StartTypewriter(Node->GetResolvedText(Context));
    
    // �������� ������
    UpdateEmotion(Node->Data.Presentation.EmotionTag);
    
    // ������������� �����
    if (Node->Data.Presentation.VoiceAudio.IsValid())
    {
        PlayVoiceAudio(Node->Data.Presentation.VoiceAudio.Get());
    }
}
```

---

## ?? DSL (Domain Specific Language)

### Conditions DSL

#### ���������
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

#### �������
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

#### ���������
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

### ����������� ����������
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

### ������������ Context
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

### ������ ���������
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

### �������������
```cpp
void UDialogueRunner::ProcessAINode(UDialogueNode* Node)
{
    // �������� AI Provider �� Subsystem
    UAIDialogueProvider* AIProvider = DialogueSubsystem->GetAIProvider();
    
    if (!AIProvider || !AIProvider->IsAPIAvailable())
    {
        // Fallback �� ��������� �����
        ProcessStaticNode(Node);
        return;
    }
    
    // ��������� ������
    FString Prompt = UPromptBuilder::BuildFullPrompt(
        CurrentDialogue->PrimaryPersona,
        CurrentContext,
        Node->Data.AIPromptHint
    );
    
    // Async ������
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
        // �������� ���� ��������������� �������
        CurrentNode->Data.DialogueText = Response.GeneratedText;
        CurrentNode->Data.Presentation.EmotionTag = Response.SuggestedEmotion;
        
        // ���������� ����������
        OnNodeEntered.Broadcast(CurrentNode, CurrentContext);
    }
    else
    {
        // Fallback ��� �������� ������
        UE_LOG(LogDialogue, Warning, TEXT("AI Generation failed: %s"), *Response.ErrorMessage);
        ProcessStaticNode(CurrentNode);
    }
}
```

---

## ?? Performance Optimization

### 1. **�����������**
```cpp
// ���: ������� ������ ���
UDialogueCondition* Cond = Evaluator->ParseCondition("affinity >= 50"); // ?

// ����: ������������ �������
UDialogueCondition* Cond = Evaluator->GetOrParseCondition("affinity >= 50"); // ?
```

### 2. **Object Pooling ��� UI**
```cpp
// ����������������� ������ ������ ������ Create/Destroy
TArray<UChoiceWidget*> WidgetPool;

UChoiceWidget* UDialogueWidget::GetOrCreateChoiceWidget()
{
    for (UChoiceWidget* Widget : WidgetPool)
    {
        if (!Widget->IsVisible())
        {
            return Widget; // ����������������
        }
    }
    
    // ������� ����� ������ ���� ��� ����
    UChoiceWidget* NewWidget = CreateWidget<UChoiceWidget>(this, ChoiceWidgetClass);
    WidgetPool.Add(NewWidget);
    return NewWidget;
}
```

### 3. **Lazy Loading DataAssets**
```cpp
// Soft Object Pointers ������ Hard References
UPROPERTY(EditAnywhere)
TSoftObjectPtr<UDialogueDataAsset> DialogueAsset;

// �������� �� ����������
if (!DialogueAsset.IsValid())
{
    DialogueAsset.LoadSynchronous();
}
```

### 4. **Async Operations**
```cpp
// ��� ���������� �������� � async tasks
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
    // ������� �������� ����
    UDialogueDataAsset* TestAsset = CreateTestDialogue();
    
    // ������� Runner
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
// NSFW ������
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
    // �������� �� ��������
    FString IdStr = NodeId.ToString();
    if (IdStr.Contains(TEXT(";")))  // SQL-like injection
    {
        return false;
    }
    
    // �������� �����
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
- ������������ `FName` ��� ID (������� ��� `FString`)
- ���������� ������������ �������/�������
- ������������ `TSoftObjectPtr` ��� assets
- ������ ��� API calls ����������
- ���������� ��� ������
- ������ unit tests ��� core logic
- ������������ GameplayTags ��� �������������
- �������� ������ �� ������
- ������������ ������� ������ Tick
- ������������ ���������� ������ ������ ������������

### ? DON'T
- �� ������������ Tick ��� ��������
- �� ������� ������ ������ �� assets
- �� ������� DSL ������ ���
- �� ����������� ���� �� AI requests
- �� ������� API ����� � assets
- �� ������ ������ �������� � UI thread
- �� �������� ������������ �� ���������
- �� ������������ `FindComponentByClass` � runtime
- �� ���������� ������
- �� ������������ �����������

---

## ?? �������������� �������

### ��������� � �������
- `/Docs/DSL_Reference.md` � ������ ���������� �� DSL
- `/Docs/AI_Integration.md` � ���� �� AI
- `/Docs/Authoring_Guide.md` � ��� ���������
- `/Docs/API_Reference.md` � ������ API reference

### �������
- `/Content/Examples/SimpleDialogue` � ������� ������
- `/Content/Examples/BranchingDialogue` � ������������
- `/Content/Examples/AIGeneratedDialogue` � AI ����������
- `/Content/Examples/EmotionalDialogue` � ������ � ��������

### ����� ��� �������
- `/Tools/DialogueEditor` � standalone ��������
- `/Tools/ExportToSheets.py` � ������� � Google Sheets
- `/Tools/ImportFromSheets.py` � ������ �� Sheets
- `/Tools/ValidateDialogues.py` � ��������� ������

---

**������ ���������**: 1.0
**��������� ����������**: 2024
**�����**: Senior Game Designer & Technical Architect
