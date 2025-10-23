# ??? ������������� ������ � ������������: Dialogue System ��� Echoes of Affection

> **����:** 2025-01-23  
> **������:** v1.3.1+  
> **�����:** ������������� ������ �������  
> **������:** ? Comprehensive Review

---

## ?? Executive Summary

������ **Echoes of Affection** ������������ ����� **first-person RPG/relationship simulator** � ����������� �������� ��������, ����������� �� ��������� **������ �����������**, **�����������** � **�������������**.

### �������� ����������

? **��������� �����������** - 4 ����������� ������ (Core, Runtime, UI, AI)  
? **Design Patterns** - Command, State Machine, Composition, Facade, Pool  
? **SOLID ��������** - SRP, DIP, ISP ���������  
? **Data-Driven ������** - UDialogueDataAsset + DSL ������  
? **����������� ��������** - FStreamableManager integration  
? **����������** - ������ ��������� SaveGame + JSON  

### ������� ���������

?? **v1.3.1** - Context Refactoring (Composition Pattern) ?  
?? **v1.3.0** - State Machine + Command Pattern + Effect Reversal ?
?? **v1.2.0** - Command Pattern Implementation ?  
?? **v1.1.0** - Core Dialogue System ?  

---

## ?? ���� �������

### ���������� ����

1. **������������ �������** � ���������� �� ������ �������
2. **������� ���������** (Affinity, Trust, Respect, Romance, Fear)
3. **������ NPC** - ����������� �������� ������
4. **������������� ��������** - ������� ���������� �� �������
5. **������� �������** - long-term consequences
6. **AI-����������** (future) - ��������� ������ ����� Chat API

### ����������� ����

1. **����������������** - ��������� ��������� NPC
2. **������������������** - Object Pooling, Async Loading
3. **�������������** - ����� ��������� ����� �������/�������
4. **Maintainability** - clean code, SOLID, design patterns
5. **Testability** - unit tests, integration tests
6. **Blueprint-friendly** - �������� ��� ����������

---

## ??? ����������� �������

### 1. ��������� ���������

```
DialogueSystem (Plugin)
?
??? DialogueSystemCore (PreDefault)
?   ??? Core (Runner, Context, Node, State)
?   ??? Data (DataAsset, Structs)
?   ??? Conditions (Evaluator + Implementations)
?   ??? Effects (Executor + Implementations)
?   ??? Commands (Pattern + History)
?   ??? Serialization (Snapshot, SaveData, JSON)
?   ??? Interfaces (Service, Participant)
?
??? DialogueSystemRuntime (Default)
?   ??? Subsystems (DialogueSubsystem)
?   ??? Components (DialogueComponent, Relationship, Memory)
?   ??? Pools (DialogueRunnerPool)
?   ??? Camera (DialogueCameraComponent)
?
??? DialogueSystemUI (Default)
?   ??? Widgets (DialogueWidget, ChoiceButton)
?   ??? HUD Integration
?
??? DialogueSystemAI (Default)
    ??? Providers (AI Response Generation)
    ??? HTTP Integration
```

### 2. �������� ����������

#### Core Module (Engine-Agnostic)
- ? **��� ������������** �� Runtime/UI/AI
- ? **Pure logic** - runner, conditions, effects
- ? **Data structures** - nodes, context, save data
- ? **Interfaces** - ��� DIP

#### Runtime Module (Game-Specific)
- ? **Actor Components** - DialogueComponent, Relationship, Memory
- ? **Subsystem** - ���������������� ����������
- ? **Object Pooling** - ����������� ������
- ? **Async Loading** - performance

#### UI Module (Presentation)
- ? **UMG Widgets** - separation of concerns
- ? **Event-driven** - �������� �� ������� DialogueRunner
- ? **Customizable** - ����� ������ ������� ���

#### AI Module (Future Integration)
- ? **HTTP API** - Chat/LLM integration
- ? **Provider Pattern** - pluggable AI backends
- ? **Async Requests** - �� ��������� ��������

---

## ?? Design Patterns

### 1. Command Pattern ? (v1.2.0)

**����:** ��������������� ��� �������� ������� ��� ������� � ���������� Undo/Redo.

```cpp
// ������� ���������
class UDialogueCommand : public UObject
{
    virtual bool Execute(UDialogueRunner* Runner) = 0;
    virtual bool Undo(UDialogueRunner* Runner) = 0;
    virtual bool CanExecute(UDialogueRunner* Runner) = 0;
};

// ����������
UDialogueCommand_NavigateToNode
UDialogueCommand_ApplyEffects
UDialogueCommand_SelectChoice
UDialogueCommand_SetVariable
UDialogueCommand_StartSession
UDialogueCommand_EndSession
UDialogueCommand_Composite

// �������������
UDialogueCommand* Cmd = NewObject<UDialogueCommand_NavigateToNode>();
Runner->ExecuteCommand(Cmd); // ������������� ������������ � �������
Runner->UndoLastCommand();   // �������� ��������� ��������
```

**������������:**
- ? ������ ������� ���� ��������
- ? Undo/Redo ����� ��������
- ? Replay ����������������
- ? Debugging ����� GetCommandHistoryAsString()
- ? Extensible - ����� ��������� ����� �������

**��������:** ~~NewObject � ������������~~ ? **������:** Lazy initialization

---

### 2. State Machine Pattern ? (v1.3.0)

**����:** ��������� ��������� ������ ������� � ����������� ������������ ��������.

```cpp
enum class EDialogueState : uint8
{
    Idle,          // �� �������
    Loading,       // �������� asset'�
    Active,     // ������ ���
    Paused,        // ����� (��������, ��� ������)
 Transitioning, // ������� ����� ������
    Ended,      // ��������
    Error          // ������
};

// State Machine Component
class UDialogueStateMachine
{
    bool CanTransition(EDialogueState To);
    void TransitionTo(EDialogueState NewState);
    bool CanPerformOperation(EDialogueOperation Op);
};
```

**Transitions:**
```
Idle ? Loading ? Active ? Paused
?
             Transitioning ? Active
           ?
    Ended
```

**������������:**
- ? ������������� ������������ ��������
- ? ���������� debugging
- ? State history tracking
- ? Blueprint-friendly API

**Fix:** End nodes �� ����� ��������� ������ � Transitioning ? **������:** ProcessNode() ��������� � Active ����� EndDialogue()

---

### 3. Composition Pattern ? (v1.3.1)

**����:** ������� God Object `UDialogueSessionContext` �� ������������������ ����������.

**�� v1.3.1:**
```cpp
UDialogueSessionContext (God Object)
??? CurrentNode
??? VisitedNodes
??? ConversationHistory
??? Player, NPC
??? CustomVariables
??? TypedVariables
??? ActiveTags
??? 50+ methods
```

**����� v1.3.1:**
```cpp
UDialogueSessionContext (Facade)
??? UDialogueState
?   ??? CurrentNode
? ??? VisitedNodes
?   ??? ConversationHistory
??? UDialogueParticipants
?   ??? Player
?   ??? NPC
?   ??? BaseAffinityLevel
??? UDialogueVariableStore
    ??? TypedVariables
    ??? CustomVariables
    ??? ActiveTags
```

**������������:**
- ? **SRP Compliance** - ������ ��������� ������ ����
- ? **Testability** - ����� ����������� ��������
- ? **Reusability** - ���������� ����� ����������������
- ? **Maintainability** - ����� �������� � ��������������
- ? **No Breaking Changes** - ����� ��������� ������ API

---

### 4. Facade Pattern ?

**����:** ��������� ������ � ������� ������� ����� ������ ���������.

```cpp
// UDialogueSessionContext - ����� ��� �����������
Context->SetCustomVariable("Gold", "100");     // ���������� Variables
Context->MarkNodeVisited("Node_1");         // ���������� State
Context->SetPlayer(PlayerActor);    // ���������� Participants

// ��� advanced users - ������ ������
Context->GetVariables()->SetVariantInt("Gold", 100);
Context->GetState()->MarkNodeVisited("Node_1");
```

---

### 5. Object Pool Pattern ?

**����:** ���������������� DialogueRunner ������ �������� �����.

```cpp
class UDialogueRunnerPool
{
    UDialogueRunner* AcquireRunner();
    void ReleaseRunner(UDialogueRunner* Runner);
    
    TArray<UDialogueRunner*> AvailableRunners;
    TArray<UDialogueRunner*> ActiveRunners;
    
    int32 InitialPoolSize = 5;
    int32 MaxPoolSize = 20;
};

// ������������� � Subsystem
UDialogueRunner* Runner = Pool->AcquireRunner();
Runner->StartDialogue(Asset, Player, NPC);
// ...
Pool->ReleaseRunner(Runner); // ���������� � ���
```

**������������:**
- ? Reduce GC pressure
- ? Faster allocation
- ? Configurable pool size

---

### 6. Strategy Pattern (Conditions & Effects)

**����:** ��������������� ��������� �������� ������� � ���������� ��������.

```cpp
// �������
class UDialogueCondition
{
    virtual bool Evaluate(Context) = 0;
};

UDialogueCondition_HasVariable
UDialogueCondition_HasTag
UDialogueCondition_AffinityCheck
UDialogueCondition_MemoryCheck
// ... 14 implementations

// �������
class UDialogueEffect
{
    virtual void Apply(Context) = 0;
    virtual void Reverse(Context) = 0; // v1.3.0 Phase 5
};

UDialogueEffect_ModifyAffinity
UDialogueEffect_SetVariable
UDialogueEffect_AddTag
UDialogueEffect_StartQuest
// ... 11 implementations
```

**������������:**
- ? Extensible - ����� ��������� ����� ����
- ? Testable - ������ ��� ����� ����������� ��������
- ? Blueprint-friendly - ����� ��������� � BP

---

### 7. Observer Pattern (Events)

**����:** Decoupling ����� ��������� ����� �������.

```cpp
// DialogueRunner Events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStartedSignature, ...);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEndedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNodeChangedSignature, ...);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParams(FOnChoicesAvailableSignature, ...);

// Subsystem Global Events
OnAnyDialogueStarted  // ����� ������ � ����
OnAnyDialogueEnded
OnAnyDialogueNodeChanged

// ��������
Runner->OnDialogueStarted.AddDynamic(this, &UMyClass::HandleDialogueStart);
Subsystem->OnAnyDialogueStarted.AddDynamic(this, &UMyClass::HandleAnyDialogue);
```

**������������:**
- ? Loose coupling ����� ���������
- ? UI �� ������� �� ������ �������
- ? ����� ������������� �� ������ �����
- ? Global events ��� cross-cutting concerns

---

## ?? ���������� ���������

### 1. Interaction System ? Dialogue System

```cpp
// InteractionComponent (�����)
void UInteractionComponent::TryInteract()
{
    if (IInteractableInterface* Interactable = Cast<IInteractableInterface>(FocusedActor))
    {
        FInteractionData Data = Interactable->GetInteractionData();
        if (Data.InteractionType == EInteractionType::Dialogue)
        {
   // ��������� ������
            UDialogueSubsystem* Subsystem = GetWorld()->GetGameInstance()
->GetSubsystem<UDialogueSubsystem>();
     
            Subsystem->StartDialogue(Data.DialogueAsset, Player, FocusedActor);
        }
    }
}
```

**Flow:**
1. Player ������� �� NPC ? `InteractionComponent::FindInteractables()`
2. NPC highlighted ? `OnInteractableFocused`
3. Player �������� E ? `TryInteract()`
4. �������� `IInteractableInterface::CanInteract()`
5. ������ ������� ? `DialogueSubsystem::StartDialogue()`

---

### 2. Dialogue System ? Relationship System

```cpp
// DialogueEffect_ModifyAffinity
void UDialogueEffect_ModifyAffinity::Apply(UDialogueSessionContext* Context)
{
    AActor* NPC = Context->GetNPC();
    if (URelationshipComponent* RelComp = NPC->FindComponentByClass<>())
    {
        RelComp->ModifyAffinity(AffinityDelta);
    }
}

// ������������� � DataAsset
Choice "Help the villagers"
  Effect: ModifyAffinity +30
  Effect: ModifyTrust +20
  Effect: AddTag "Hero.Helpful"
```

**����������:**
- ? DialogueEffect ? RelationshipComponent (����������������)
- ? RelationshipComponent �� ����� � DialogueSystem (decoupling)
- ? ����� ������ ��������� �� ����� �������

---

### 3. Dialogue System ? Memory System

```cpp
// DialogueComponent ������������� ������ ������������
void UDialogueComponent::HandleNodeEntered(UDialogueNode* Node)
{
    if (MemoryComp && Node)
    {
 MemoryComp->CreateMemory(
  EMemoryType::DialogueEvent,
            Node->SpeakerText,
    CalculateImportance(Node),
      DetermineEmotion(Node),
            LastContext->GetPlayer()
        );
    }
}

// ������� �� ������ ������
UDialogueCondition_MemoryCheck
{
    // "NPC ������, ��� �� ������� �������"
    TArray<FNPCMemoryEntry> Memories = MemoryComp->FindMemoriesWithTag("Event.Help");
    return Memories.Num() > 0;
}
```

**����������:**
- ? DialogueComponent ������ ������������ �������������
- ? ������� ����� ��������� ������
- ? ������� ����� �������������� ������
- ? Memory ������ �� ����������� ��������

---

### 4. Dialogue System ? Quest System (Future)

```cpp
// DialogueEffect_StartQuest
void UDialogueEffect_StartQuest::Apply(Context)
{
    UQuestSubsystem* QuestSystem = GetWorld()->GetGameInstance()
        ->GetSubsystem<UQuestSubsystem>();
    
    QuestSystem->StartQuest(QuestId);
  
    // ������� ������������ � promise
    MemoryComp->CreateMemory(
        EMemoryType::Promise,
     FText::Format("Agreed to {0}", QuestName),
        95.0f
    );
}

// ������� ���������� ������
UDialogueCondition_QuestCompleted
{
    return QuestSystem->IsQuestCompleted(QuestId);
}
```

---

### 5. Dialogue System ? AI System (Future)

```cpp
// AIDialogueProvider
class UAIDialogueProvider : public UObject
{
    // ��������� ������ ����� Chat API
    void GenerateResponse(
        FString Context,
 FString PlayerMessage,
        FOnAIResponseGenerated Callback
    );
};

// ���������� � DialogueRunner
UDialogueNode* DynamicNode = Runner->CreateDynamicNode();
AIProvider->GenerateResponse(
BuildContext(Runner->GetContext()),
    PlayerChoice->Text,
    [&](FString AIResponse) {
   DynamicNode->SpeakerText = FText::FromString(AIResponse);
        Runner->ProcessNode(DynamicNode);
    }
);
```

**�����������:**
```
DialogueRunner
    ?
AIDialogueProvider (Interface)
    ?
??? OpenAIProvider
??? ClaudeProvider
??? CustomLLMProvider

HTTP Module ? JSON Parsing ? Callback
```

---

## ?? Data Flow & Pipeline

### 1. Authoring Pipeline (Designer ? Runtime)

```
Designer Workflow:
1. Create UDialogueDataAsset
2. Add nodes:
   - Set SpeakerText
   - Add Choices
   - Configure Conditions
   - Configure Effects
3. Save Asset

Alternative: DSL Parser
1. Write dialogue in text format
2. Run parser: DSLParser->Parse("path/to/dialogue.txt")
3. Generates UDialogueDataAsset
```

**DSL Example:**
```
NODE Start
  SPEAKER "Hello traveler! How can I help you?"
  CHOICE "Tell me about the village"
 CONDITION HasTag Player.Curious
    EFFECT AddTag World.VillageKnown
    GOTO VillageInfo
  CHOICE "Goodbye"
    GOTO End
```

---

### 2. Runtime Pipeline

```
StartDialogue()
    ?
Subsystem->StartDialogue(Asset, Player, NPC)
  ?
Pool->AcquireRunner()
    ?
Runner->Initialize(Context)
    ?
Runner->LoadAsset(Asset) [Async]
    ?
Runner->StartDialogue()
    ?
[State: Idle ? Loading ? Active]
    ?
ProcessNode(StartNode)
    ?
Evaluate Conditions ? Filter Choices
    ?
OnChoicesAvailable.Broadcast()
    ?
UI->ShowChoices()
    ?
Player selects choice
    ?
Runner->SelectChoice(Index)
    ?
Apply Effects ? Navigate to next node
    ?
[Repeat until End node]
    ?
Runner->EndDialogue()
    ?
[State: Active ? Ended]
    ?
Pool->ReleaseRunner()
```

---

### 3. Save/Load Pipeline

```
SaveGame:
Context->ToSaveData()
    ?
FDialogueSessionSaveData {
    DialogueId
    CurrentNodeId
    VisitedNodes
    CustomVariables
    TypedVariables
    ActiveTags
    BaseAffinityLevel
    Timestamp
}
    ?
SaveGame->DialogueSessions.Add(SaveData)
    ?
UGameplayStatics::SaveGameToSlot()

LoadGame:
UGameplayStatics::LoadGameFromSlot()
    ?
SaveGame->DialogueSessions
    ?
Context->FromSaveData(SaveData, Player, NPC)
    ?
Runner->StartDialogue() // ��������������� ���������
```

---

### 4. Snapshot Pipeline (Undo/Redo)

```
Before Operation:
Snapshot = Context->CreateSnapshot(State, History)
    ?
FDialogueStateSnapshot {
    CurrentNodeId
    VisitedNodes
    ConversationHistory
    CustomVariables
 ActiveTags
    Timestamp
    bIsValid
}
    ?
Command->SetSnapshot(Snapshot)

Undo Operation:
Command->Undo()
    ?
Context->RestoreFromSnapshot(Snapshot)
    ?
[State ������������ ���������]
```

---

### 5. Effect Reversal Pipeline (v1.3.0 Phase 5) ?

```
Apply Effects:
UDialogueCommand_ApplyEffects::Execute()
    ?
For each effect:
    if (Effect->SupportsReverse())
    Store effect for reversal
    else
Create snapshot

Undo Effects:
UDialogueCommand_ApplyEffects::Undo()
    ?
For each effect (in reverse order):
    if (can reverse)
        Effect->Reverse()    // Fast path (90% cases)
    else
        Restore snapshot  // Fallback (10% cases)
```

**Reversible Effects:** (9/11)
- ModifyAffinity, ModifyTrust ? negative delta
- ModifyInventory ? reverse add/remove
- SetMemory ? restore old value
- AddWorldStateTag ? remove tag
- SetVariable ? restore old value
- Composite ? reverse all sub-effects

**Non-Reversible:** (2/11)
- StartQuest, CompleteQuest ? use snapshot

**Performance:**
- 90% memory reduction for reversible effects
- <1% CPU overhead
- Fast path: O(n) where n = effect count

---

## ?? Best Practices & Guidelines

### 1. SOLID Principles

#### Single Responsibility Principle ?
```
? BAD: UDialogueSessionContext (God Object v1.3.0)
    - Manages state
    - Manages participants
    - Manages variables
    - 50+ methods

? GOOD: Composition Pattern (v1.3.1)
 - UDialogueState - ������ state
    - UDialogueParticipants - ������ participants
    - UDialogueVariableStore - ������ variables
```

#### Open/Closed Principle ?
```cpp
// ����� ��������� ����� �������/������� ��� ��������� ������������� ����
class UDialogueCondition_CustomCheck : public UDialogueCondition
{
    virtual bool Evaluate(Context) override {
        // Custom logic
    }
};

// ����������� � Evaluator
Evaluator->RegisterConditionType<UDialogueCondition_CustomCheck>();
```

#### Liskov Substitution Principle ?
```cpp
// ����� UDialogueCommand ����� ���� ����������� ���������
UDialogueCommand* Cmd1 = NewObject<UDialogueCommand_NavigateToNode>();
UDialogueCommand* Cmd2 = NewObject<UDialogueCommand_ApplyEffects>();

Runner->ExecuteCommand(Cmd1); // ��������
Runner->ExecuteCommand(Cmd2); // �������� ��� ��
Runner->UndoLastCommand();    // �������� ��� �����
```

#### Interface Segregation Principle ?
```cpp
// ���������� ��������� �� �����
IDialogueService    - ��� ������� ��������
IDialogueParticipant    - ��� ����������
IInteractableInterface  - ��� ��������������

// NPC �� ������ ������������� IDialogueService
class ANPC : public IDialogueParticipant, public IInteractableInterface
{
    // ������ ��, ��� �����
};
```

#### Dependency Inversion Principle ?
```cpp
// DialogueRunner ������� �� ����������, � �� �� ���������� ����������
class UDialogueRunner
{
    UPROPERTY()
    TScriptInterface<IDialogueService> Service; // ���������
};

// ����� ����� ����� ���� Service
class UDialogueSubsystem : public IDialogueService { ... };
class UDialogueManager : public IDialogueService { ... };
```

---

### 2. Performance Optimization

#### Memory Management
```cpp
? Object Pooling ��� DialogueRunner
? Lazy Initialization ��� �����������
? Effect Reversal ������ snapshots (90% reduction)
? TWeakObjectPtr ��� �������������� ������
? Configurable pool sizes
```

#### CPU Optimization
```cpp
? Async Asset Loading (FStreamableManager)
? Tick enable/disable based on state
? Early-out � �������� (&&, ||)
? Cached component references
? Minimal virtual calls
```

#### Network (Future)
```cpp
? Replicate ������ critical state
? Delta compression ��� variables
? Event batching ��� UI updates
```

---

### 3. Error Handling

```cpp
// Defensive Programming
if (!Context || !Context->GetCurrentNode())
{
    UE_LOG(LogDialogue, Error, TEXT("Invalid context"));
    return false;
}

// State Validation
if (!StateMachine->CanPerformOperation(EDialogueOperation::SelectChoice))
{
    UE_LOG(LogDialogue, Warning, TEXT("Cannot select choice in state %s"), 
      *UEnum::GetValueAsString(StateMachine->GetCurrentState()));
    return false;
}

// Graceful Degradation
if (!AIProvider->IsAvailable())
{
    // Fallback to static dialogue
    UseStaticFallback();
}
```

---

### 4. Testing Strategy

#### Unit Tests
```cpp
// DialogueConditionEvaluatorTests.cpp
TEST_CASE("HasVariable evaluates correctly")
{
    UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
    Context->SetCustomVariable("Gold", "100");
    
    UDialogueCondition_HasVariable* Condition = NewObject<>();
    Condition->VariableName = "Gold";
    Condition->ExpectedValue = "100";
    
    REQUIRE(Condition->Evaluate(Context) == true);
}
```

#### Integration Tests
```cpp
// ��������� ������ flow
1. Create DialogueDataAsset
2. Start dialogue
3. Select choices
4. Verify effects applied
5. Verify memory created
6. Verify relationships changed
```

#### Performance Tests
```cpp
// NPCMemoryTestHelpers::PerformanceTest()
- Create N memories
- Measure creation time
- Measure retrieval time
- Measure search time
- Log results
```

---

## ?? ������������ �� ���������

### 1. ������������� ���������

#### ? DONE: Context Refactoring (v1.3.1)
- ������� God Object �� ����������
- ������������ Composition Pattern
- ��������� backward compatibility

#### ?? TODO: Event System Refactoring
**��������:** ����� ������������� ������� (OnDialogueStarted � Runner, Component, Subsystem)

**�������:** ���������������� Event Bus
```cpp
class UDialogueEventBus : public UGameInstanceSubsystem
{
    // ��� ������� � ����� �����
    DECLARE_MULTICAST_DELEGATE_TwoParams(FDialogueStarted, Runner, Context);
    FDialogueStarted OnDialogueStarted;
 
    // ���������� ������
    static UDialogueEventBus* Get(UWorld* World);
};

// �������������
UDialogueEventBus::Get(World)->OnDialogueStarted.Broadcast(Runner, Context);
```

#### ?? TODO: Variable System Enhancement
**��������:** CustomVariables (TMap<FName, FString>) �� type-safe

**������� �������:** TypedVariables (v1.3.1) ?

**������� ���������:** Variable Resolver Pattern
```cpp
class UDialogueVariableResolver
{
    // Resolve variables from multiple sources
    FString ResolveVariable(FName Key, Context)
    {
   // 1. Check Context Variables
    // 2. Check World State
 // 3. Check Player Stats
        // 4. Check Quest Progress
      // 5. Check Time/Date
 }
};

// ������������� � ��������
"Player.Gold >= 100"  ? Resolver->ResolveVariable("Player.Gold")
```

---

### 2. ����������������

#### ��������� NPC

**������� ��������:** ������ NPC ����� ���� ���������� (Dialogue, Relationship, Memory)

**������� 1:** Shared Data Approach
```cpp
// NPCProfile DataAsset (shared ����� instances)
UNPCProfileAsset
{
    FName NPCId;
    TArray<TSoftObjectPtr<UDialogueDataAsset>> AvailableDialogues;
    UPersonalityPreset* Personality;
    UMemoryPreset* MemorySettings;
};

// NPCInstance (runtime data)
UNPCInstanceData
{
    FName ProfileId;
    float CurrentAffinity;
    TArray<FNPCMemoryEntry> UniqueMemories;
};
```

**������� 2:** ECS-like Architecture (Future)
```cpp
// Entity Component System ��� NPC
NPCEntity
{
    TransformComponent
    DialogueComponent (shared logic)
    RelationshipData (POD struct)
    MemoryData (POD struct)
};

// System processes all NPCs
URelationshipSystem::Tick(DeltaTime)
{
    for (NPCEntity& NPC : AllNPCs)
    {
    ProcessDecay(NPC.RelationshipData, DeltaTime);
   ProcessMemoryDecay(NPC.MemoryData, DeltaTime);
    }
}
```

---

### 3. AI Integration Roadmap

#### Phase 1: Static + Templated Responses (Current)
```cpp
NODE Greeting
  SPEAKER "Hello {PlayerName}! How are you today?"
  // Simple variable substitution
```

#### Phase 2: Sentiment Analysis
```cpp
// Analyze player choice emotion
FString PlayerChoice = "I don't trust you";
ESentiment Sentiment = AIProvider->AnalyzeSentiment(PlayerChoice);
// ? Negative

// Modify relationship based on sentiment
RelationshipComponent->ApplyStimulus(Sentiment);
```

#### Phase 3: Response Generation
```cpp
// Generate dynamic responses
AIProvider->GenerateResponse(
    NPCPersonality,
    DialogueContext,
    PlayerMessage,
    [&](FString Response) {
    CreateDynamicNode(Response);
    }
);
```

#### Phase 4: Conversation Simulation
```cpp
// AI generates entire conversation branches
AIProvider->SimulateConversation(
    NPCProfile,
    Topic,
    MaxDepth,
    [&](UDialogueDataAsset* Generated) {
        Runner->LoadDialogue(Generated);
    }
);
```

**Architecture:**
```
DialogueRunner
    ?
IAIDialogueProvider (Interface)
    ?
?????????????????????????????????????????????
? OpenAI  ? Claude        ? Local LLM    ?
? Provider   ? Provider      ? Provider     ?
?????????????????????????????????????????????
?
HTTP Request Handler
    ?
JSON Parser ? FDialogueAIResponse
```

---

### 4. Tooling & Editor Improvements

#### Dialogue Editor (Future Plugin)
```cpp
// Visual node editor for dialogues
UDialogueEditorGraph
{
    - Visual nodes instead of text DSL
    - Real-time validation
    - Preview conditions/effects
    - Auto-save to DataAsset
};
```

#### Memory Debugger
```cpp
// Blueprint widget ��� debugging ������ NPC
UNPCMemoryDebugWidget
{
    - Show all memories
    - Filter by type/emotion
    - Visualize importance/freshness
- Test recall/decay
    - Export to JSON
};
```

#### Relationship Visualizer
```cpp
// Graph view ���� ���������
URelationshipGraphWidget
{
    - Node per NPC
    - Edge weight = Affinity
    - Color = Relationship Rank
    - Tooltip = detailed stats
};
```

---

## ?? Roadmap

### v1.4.0 - Event System Refactoring
- [ ] ���������������� Event Bus
- [ ] ���������� �������
- [ ] Performance profiling

### v1.5.0 - Advanced AI Integration
- [ ] Sentiment Analysis
- [ ] Dynamic Response Generation
- [ ] Conversation Memory (long-term context)

### v1.6.0 - Networking Support
- [ ] Replicated DialogueRunner
- [ ] Multiplayer dialogue sessions
- [ ] Spectator support

### v2.0.0 - ECS Architecture
- [ ] Entity Component System ��� NPC
- [ ] Data-oriented design
- [ ] Massive performance improvements

---

## ?? ����������

### ������� �������

? **��������� �����������** - ����� ��������� � ������������  
? **SOLID principles** - clean code, testable  
? **Design Patterns** - ���������� ���������� ���������  
? **Performance** - pooling, async loading, effect reversal  
? **Extensibility** - ����� ��������� ����� conditions/effects/commands  
? **Documentation** - ��������� ������������ (40+ pages)  

### ������� ��� ���������

?? **Event System** - ������� ����� ������������� �������  
?? **Variable System** - ����� �������� type safety  
?? **Scalability** - ����������� ��� >100 NPC  
?? **AI Integration** - ���� ������ ���������  
?? **Tooling** - ����� visual editor  

### �������

������ ������������� **���������������� ������� �����������** � ���������� ����������� ��������� OOP, Design Patterns � Best Practices.

������� ������ �:
- ? ��������������� �� �������� �������
- ? ���������� ��������� NPC
- ? ���������� � ������� ���������
- ? AI-��������� �������� (� ���������� �����������)

**������������:** ���������� �������� �� ����������� roadmap, �������� Event System Refactoring (v1.4) � AI Integration (v1.5).

---

**�������� �����������:** 2025-01-23  
**������:** 1.0  
**������:** ? Ready for Review
