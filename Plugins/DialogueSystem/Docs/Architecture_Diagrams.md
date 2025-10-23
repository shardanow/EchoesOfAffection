# ?? Architecture Diagrams

> Visual representation of Dialogue System architecture

---

## ??? Module Dependencies

```mermaid
graph TB
    subgraph "DialogueSystem Plugin"
        Core["?? DialogueSystemCore<br/>(PreDefault)"]
  Runtime["?? DialogueSystemRuntime<br/>(Default)"]
        UI["?? DialogueSystemUI<br/>(Default)"]
        AI["?? DialogueSystemAI<br/>(Default)"]
    end
    
    subgraph "Unreal Engine"
        Engine["Engine"]
        CoreUObject["CoreUObject"]
    UMG["UMG"]
     HTTP["HTTP"]
        Json["JSON"]
    end
    
  subgraph "Game Code"
        GameModule["EchoesOfAffection"]
   Player["Player Character"]
        NPC["NPC Actors"]
    end
    
    Core --> Engine
    Core --> CoreUObject
    
    Runtime --> Core
    Runtime --> Engine
    
    UI --> Core
    UI --> Runtime
    UI --> UMG
    
    AI --> Core
    AI --> HTTP
    AI --> Json
    
    GameModule --> Runtime
  GameModule --> UI
    Player --> Runtime
    NPC --> Runtime
    
 style Core fill:#4a9eff
    style Runtime fill:#50c878
    style UI fill:#ff69b4
    style AI fill:#ffa500
```

---

## ?? Complete Dialogue Flow

```mermaid
sequenceDiagram
    participant Player
    participant InteractionComp as InteractionComponent
    participant NPC
    participant DialogueComp as DialogueComponent
    participant Subsystem as DialogueSubsystem
    participant Pool as RunnerPool
    participant Runner as DialogueRunner
    participant UI as DialogueWidget
  
    Player->>InteractionComp: Press E
    InteractionComp->>NPC: Execute_Interact()
    NPC->>DialogueComp: StartDialogue(Player)
    DialogueComp->>Subsystem: StartDialogue(Asset, Player, NPC)
    
    Subsystem->>Pool: AcquireRunner()
    Pool-->>Subsystem: DialogueRunner
    
    Subsystem->>Runner: Initialize(Context)
    Subsystem->>Runner: LoadAsset(Async)
    
    Note over Runner: State: Loading
    
    Runner-->>Subsystem: OnAssetLoaded
    
    Note over Runner: State: Active
    
    Runner->>Runner: ProcessNode(StartNode)
    Runner->>UI: OnChoicesAvailable(Choices)
    
    UI->>Player: Show Choices
    Player->>UI: Select Choice
    UI->>Runner: SelectChoice(Index)
    
    Runner->>Runner: ExecuteCommand(SelectChoice)
    Runner->>Runner: ApplyEffects()
    Runner->>Runner: GoToNode(TargetNode)
    
    Note over Runner: Loop until End Node
    
    Runner->>Runner: EndDialogue()
    
    Note over Runner: State: Ended
    
    Runner->>UI: OnDialogueEnded()
    Runner->>Subsystem: ReleaseRunner()
    Subsystem->>Pool: ReleaseRunner(Runner)
```

---

## ?? Class Hierarchy: Core Components

```mermaid
classDiagram
    class UDialogueRunner {
        +StartDialogue()
        +ProcessNode()
+SelectChoice()
 +EndDialogue()
        -Context: UDialogueSessionContext
        -StateMachine: UDialogueStateMachine
   -CommandInvoker: UDialogueCommandInvoker
  }
    
    class UDialogueSessionContext {
        +GetState(): UDialogueState
        +GetParticipants(): UDialogueParticipants
   +GetVariables(): UDialogueVariableStore
        +CreateSnapshot(): FDialogueStateSnapshot
        +RestoreFromSnapshot()
    }
    
    class UDialogueState {
     +GetCurrentNode(): UDialogueNode
        +SetCurrentNode()
     +MarkNodeVisited()
 +GetVisitedNodes()
        +GetConversationHistory()
    }
    
class UDialogueParticipants {
      +GetPlayer(): AActor
        +GetNPC(): AActor
        +SetPlayer()
  +SetNPC()
    }
    
    class UDialogueVariableStore {
        +SetVariantInt()
        +GetVariantInt()
        +SetVariantBool()
        +AddTag()
        +HasTag()
    }
    
    class UDialogueCommandInvoker {
  +ExecuteCommand()
        +UndoLastCommand()
   +RedoCommand()
        +GetHistory()
 }
    
    class UDialogueStateMachine {
        +GetCurrentState()
        +TransitionTo()
        +CanPerformOperation()
    }
    
 UDialogueRunner --> UDialogueSessionContext
    UDialogueRunner --> UDialogueCommandInvoker
  UDialogueRunner --> UDialogueStateMachine
    
    UDialogueSessionContext --> UDialogueState
    UDialogueSessionContext --> UDialogueParticipants
    UDialogueSessionContext --> UDialogueVariableStore
```

---

## ?? Command Pattern Flow

```mermaid
graph LR
    subgraph "Command Creation"
        A[User Action] --> B[Create Command]
        B --> C{Command Type}
    C -->|Navigation| D[NavigateToNode]
  C -->|Effects| E[ApplyEffects]
        C -->|Choice| F[SelectChoice]
    end
    
    subgraph "Command Execution"
     D --> G[CommandInvoker]
        E --> G
    F --> G
 G --> H{CanExecute?}
        H -->|Yes| I[Create Snapshot]
H -->|No| J[Reject]
        I --> K[Execute Command]
        K --> L[Push to History]
    end
    
    subgraph "Undo/Redo"
 L --> M{User Action}
        M -->|Undo| N[Pop from History]
        M -->|Redo| O[Pop from Redo Stack]
  N --> P[Execute Undo]
O --> Q[Execute Again]
        P --> R[Push to Redo]
      Q --> L
    end
    
    style G fill:#4a9eff
    style K fill:#50c878
    style P fill:#ffa500
```

---

## ?? State Machine Transitions

```mermaid
stateDiagram-v2
    [*] --> Idle
 
    Idle --> Loading: StartDialogue()
    Loading --> Active: Asset Loaded
Loading --> Error: Load Failed
  Loading --> Idle: Cancelled
    
    Active --> Paused: PauseDialogue()
    Active --> Transitioning: SelectChoice()
    Active --> Ended: EndDialogue()
    Active --> Error: Critical Error
  
    Paused --> Active: ResumeDialogue()
    Paused --> Ended: EndDialogue()
    
    Transitioning --> Active: ProcessNode()
    Transitioning --> Error: Navigation Failed
    
    Ended --> Idle: Reset()
    Error --> Idle: Reset()
    
    note right of Active
        Main dialogue state
        Can process nodes
 Can select choices
    end note
    
    note right of Transitioning
   Between nodes
     Brief state
        Auto-transitions
    end note
```

---

## ?? System Integration Map

```mermaid
graph TB
 subgraph "Player Systems"
        PlayerInput["Player Input"]
        PlayerChar["Player Character"]
      InteractionComp["InteractionComponent"]
    end
    
    subgraph "NPC Systems"
     NPCActor["NPC Actor"]
    DialogueComp["DialogueComponent"]
        RelationshipComp["RelationshipComponent"]
MemoryComp["NPCMemoryComponent"]
    end
    
    subgraph "Dialogue System"
        Subsystem["DialogueSubsystem"]
        Runner["DialogueRunner"]
Context["DialogueSessionContext"]
        ConditionEval["ConditionEvaluator"]
        EffectExec["EffectExecutor"]
    end
    
    subgraph "UI Systems"
 Widget["DialogueWidget"]
        ChoiceButton["ChoiceButton"]
        HUD["Game HUD"]
    end
    
    subgraph "Game Systems"
        QuestSystem["Quest System"]
        InventorySystem["Inventory System"]
    SaveSystem["Save/Load System"]
    end
    
    subgraph "AI Systems"
        AIProvider["AI Provider"]
    OpenAI["OpenAI API"]
        Claude["Claude API"]
    end
    
    PlayerInput --> InteractionComp
    InteractionComp --> NPCActor
    NPCActor --> DialogueComp
    
    DialogueComp --> Subsystem
    Subsystem --> Runner
    Runner --> Context
    Runner --> ConditionEval
    Runner --> EffectExec
    
    Runner --> Widget
    Widget --> ChoiceButton
    Widget --> HUD
    
    EffectExec --> RelationshipComp
    EffectExec --> MemoryComp
    EffectExec --> QuestSystem
    EffectExec --> InventorySystem
    
    ConditionEval --> RelationshipComp
    ConditionEval --> MemoryComp
    ConditionEval --> QuestSystem
    
    Subsystem --> SaveSystem
    Context --> SaveSystem
    
    Runner -.-> AIProvider
    AIProvider -.-> OpenAI
  AIProvider -.-> Claude
    
    style Subsystem fill:#4a9eff
    style Runner fill:#4a9eff
    style Context fill:#4a9eff
    style EffectExec fill:#50c878
    style ConditionEval fill:#50c878
    style Widget fill:#ff69b4
    style AIProvider fill:#ffa500
```

---

## ?? Effect Application Flow

```mermaid
sequenceDiagram
    participant Runner
    participant EffectExec as EffectExecutor
    participant NPC
    participant RelComp as RelationshipComponent
    participant MemComp as MemoryComponent
    participant QuestSys as QuestSystem
    
    Runner->>EffectExec: ApplyEffects(Effects[])
    
    loop For each effect
        EffectExec->>EffectExec: GetImplementation(Type)
        
        alt ModifyAffinity
        EffectExec->>NPC: FindComponent<Relationship>()
         NPC-->>EffectExec: RelComp
            EffectExec->>RelComp: ModifyAffinity(Delta)
EffectExec->>MemComp: CreateMemory(Change)
     else SetVariable
     EffectExec->>Runner: Context->Variables->Set()
      else AddTag
        EffectExec->>Runner: Context->Variables->AddTag()
     else StartQuest
         EffectExec->>QuestSys: StartQuest(QuestId)
    EffectExec->>MemComp: CreateMemory(Promise)
 else ModifyInventory
            EffectExec->>NPC: InventoryComp->AddItem()
 EffectExec->>MemComp: CreateMemory(Item)
        end
 end
    
    EffectExec-->>Runner: Effects Applied
```

---

## ?? Memory System Architecture

```mermaid
graph TB
    subgraph "Memory Creation"
        Event["Dialogue Event"] --> Create["CreateMemory()"]
        Create --> Store["FNPCMemoryEntry"]
        Store --> Array["MemoryArray"]
    end
    
    subgraph "Memory Storage"
      Array --> Type["Type Classification"]
        Type --> DialogueEvent["DialogueEvent"]
        Type --> PlayerChoice["PlayerChoice"]
        Type --> Promise["Promise"]
        Type --> ItemGiven["ItemGiven"]
 Type --> RelChange["RelationshipChange"]
 end
    
    subgraph "Memory Properties"
        Store --> Importance["Importance (0-100)"]
        Store --> Freshness["Freshness (decay)"]
  Store --> Emotion["Emotion Type"]
        Store --> Timestamp["Timestamp"]
      Store --> ContextTags["Context Tags"]
    end
    
    subgraph "Memory Queries"
        Array --> Query["Query Methods"]
    Query --> RecentMemories["GetRecentMemories()"]
        Query --> AboutActor["GetMemoriesAboutActor()"]
        Query --> WithTag["FindMemoriesWithTag()"]
      Query --> Relevant["FindRelevantMemories()"]
    end
    
    subgraph "Memory Decay"
        Array --> Decay["ProcessMemoryDecay()"]
     Decay --> Age["Calculate Age"]
        Age --> FreshnessCalc["Update Freshness"]
  FreshnessCalc --> Threshold["Check Threshold"]
        Threshold -->|Too Low| Remove["Remove Memory"]
    Threshold -->|OK| Keep["Keep Memory"]
    end
    
    style Store fill:#4a9eff
    style Query fill:#50c878
    style Decay fill:#ffa500
```

---

## ?? Relationship System Flow

```mermaid
graph LR
    subgraph "Input"
     A[Dialogue Effect] --> B[ModifyAffinity/Trust/etc]
        C[Player Action] --> B
        D[Quest Event] --> B
  end
    
    subgraph "Relationship Component"
        B --> E{Personality Modifiers}
      E -->|Positive| F[?PositiveSensitivity]
        E -->|Negative| G[?NegativeSensitivity]
        F --> H[Update Value]
  G --> H
        
        H --> I[Clamp to Range]
        I --> J{Threshold Check}
        J -->|Changed| K[UpdateRelationshipRank]
        J -->|Same| L[No Rank Change]
end
    
    subgraph "Side Effects"
        K --> M[Broadcast OnAffinityChanged]
     K --> N[Create Memory]
     K --> O{Rank Up?}
        O -->|Yes| P[Unlock Dialogues]
        O -->|No| Q[Continue]
    end
    
    subgraph "Decay System"
        R[Tick] --> S[ProcessDecay]
        S --> T[FInterpTo towards target]
      T --> H
    end
    
    style B fill:#4a9eff
    style H fill:#50c878
  style K fill:#ffa500
```

---

## ?? Object Pooling Flow

```mermaid
sequenceDiagram
    participant Subsystem as DialogueSubsystem
  participant Pool as RunnerPool
    participant Runner as DialogueRunner
    
    Note over Pool: Initial State:<br/>Available: [R1, R2, R3, R4, R5]<br/>Active: []
    
    Subsystem->>Pool: AcquireRunner()
    
    alt Pool has available runners
        Pool->>Pool: Pop from Available
        Pool-->>Subsystem: Return R1
        Note over Pool: Available: [R2, R3, R4, R5]<br/>Active: [R1]
  else Pool empty but below max
        Pool->>Pool: Create new Runner
     Pool-->>Subsystem: Return R6
        Note over Pool: Available: [R2, R3, R4, R5]<br/>Active: [R1, R6]
    else Pool at max capacity
        Pool-->>Subsystem: Return nullptr
        Note over Subsystem: Queue request or deny
    end
    
    Subsystem->>Runner: StartDialogue()
    Note over Runner: Dialogue in progress...
  Runner->>Subsystem: OnDialogueEnded
    
    Subsystem->>Pool: ReleaseRunner(R1)
    Pool->>Runner: Reset()
    Pool->>Pool: Move to Available
 
    Note over Pool: Available: [R1, R2, R3, R4, R5]<br/>Active: [R6]
```

---

## ?? Condition Evaluation Tree

```mermaid
graph TB
    Start[Evaluate Conditions] --> Type{Condition Type}
    
    Type -->|HasVariable| Var[Check Variable Exists]
    Var --> VarMatch{Value Matches?}
 VarMatch -->|Yes| Pass1[? Pass]
    VarMatch -->|No| Fail1[? Fail]
    
    Type -->|HasTag| Tag[Check Tag Present]
    Tag --> TagMatch{Tag Found?}
    TagMatch -->|Yes| Pass2[? Pass]
    TagMatch -->|No| Fail2[? Fail]
    
    Type -->|AffinityCheck| Aff[Get NPC Affinity]
    Aff --> AffComp{Compare to Threshold}
    AffComp -->|>=| Pass3[? Pass]
    AffComp -->|<| Fail3[? Fail]
 
    Type -->|MemoryCheck| Mem[Query Memories]
    Mem --> MemFound{Memories Found?}
    MemFound -->|Yes| MemCount{Count >= Min?}
    MemFound -->|No| Fail4[? Fail]
    MemCount -->|Yes| Pass4[? Pass]
    MemCount -->|No| Fail4
    
    Type -->|QuestCompleted| Quest[Query Quest System]
    Quest --> QuestState{Quest State?}
    QuestState -->|Completed| Pass5[? Pass]
    QuestState -->|Other| Fail5[? Fail]
    
    Type -->|Composite| Comp[Evaluate Sub-Conditions]
    Comp --> CompType{Operator}
    CompType -->|AND| And[All must pass]
    CompType -->|OR| Or[At least one passes]
    And --> AndResult{All Pass?}
    Or --> OrResult{Any Pass?}
    AndResult -->|Yes| Pass6[? Pass]
    AndResult -->|No| Fail6[? Fail]
    OrResult -->|Yes| Pass7[? Pass]
    OrResult -->|No| Fail7[? Fail]
    
    style Pass1 fill:#50c878
    style Pass2 fill:#50c878
    style Pass3 fill:#50c878
    style Pass4 fill:#50c878
    style Pass5 fill:#50c878
    style Pass6 fill:#50c878
    style Pass7 fill:#50c878
    style Fail1 fill:#ff6b6b
    style Fail2 fill:#ff6b6b
    style Fail3 fill:#ff6b6b
    style Fail4 fill:#ff6b6b
    style Fail5 fill:#ff6b6b
    style Fail6 fill:#ff6b6b
    style Fail7 fill:#ff6b6b
```

---

## ?? Performance Optimization Layers

```mermaid
graph TB
    subgraph "Layer 1: Object Pooling"
        A[Request Runner] --> B{Pool Available?}
  B -->|Yes| C[Reuse from Pool]
        B -->|No| D[Create New]
        C --> E[Runner Ready]
        D --> E
    end
    
    subgraph "Layer 2: Async Loading"
        E --> F[Load Dialogue Asset]
        F --> G{Already Loaded?}
        G -->|Yes| H[Use Cached]
G -->|No| I[Async Load]
        H --> J[Asset Ready]
        I --> J
    end
    
    subgraph "Layer 3: Batch Processing"
        J --> K[Dialogue Active]
      K --> L[Update NPCs]
        L --> M{Update Mode}
        M -->|Individual| N[Update Each Frame]
        M -->|Batch| O[Batch Update Every N Seconds]
        O --> P[Parallel Processing]
    end
    
    subgraph "Layer 4: Spatial Partitioning"
 P --> Q[World Grid]
        Q --> R{NPC Near Player?}
        R -->|Yes| S[Full Update]
        R -->|No| T[Reduced Update]
        S --> U[Active NPCs Only]
    end
    
    subgraph "Layer 5: Effect Reversal"
     U --> V[Apply Effects]
        V --> W{Reversible?}
        W -->|Yes| X[Store Reversal Info]
     W -->|No| Y[Create Snapshot]
    X --> Z[90% Memory Saved]
   Y --> Z
    end
    
    style C fill:#50c878
    style H fill:#50c878
    style O fill:#50c878
    style S fill:#50c878
    style X fill:#50c878
```

---

## ?? AI Integration Architecture (v1.5)

```mermaid
graph TB
    subgraph "Dialogue System"
      Runner[DialogueRunner]
        Context[DialogueSessionContext]
    end
    
    subgraph "AI Layer"
        Provider[IAIDialogueProvider]
        OpenAI[OpenAIProvider]
      Claude[ClaudeProvider]
        LocalLLM[LocalLLMProvider]
 end
    
    subgraph "Context Building"
        CtxBuilder[ContextBuilder]
        Personality[NPC Personality]
        Relationship[Relationship Data]
        Memory[Recent Memories]
        History[Conversation History]
    end
    
  subgraph "Analysis & Generation"
        Sentiment[Sentiment Analysis]
        Generator[Response Generator]
        Memory2[Conversation Memory]
 end
    
    subgraph "External APIs"
        API1[OpenAI API]
        API2[Claude API]
        API3[Local LLM Server]
    end
    
    Runner --> Provider
    Runner --> CtxBuilder
    
    CtxBuilder --> Personality
    CtxBuilder --> Relationship
    CtxBuilder --> Memory
    CtxBuilder --> History
    
    Provider --> OpenAI
    Provider --> Claude
    Provider --> LocalLLM
    
    Provider --> Sentiment
    Provider --> Generator
    Provider --> Memory2
  
 OpenAI --> API1
    Claude --> API2
    LocalLLM --> API3
    
    Generator --> Runner
    Sentiment --> Context
    
style Provider fill:#4a9eff
    style Generator fill:#50c878
    style Sentiment fill:#ffa500
```

---

## ?? Scalability Roadmap

```mermaid
timeline
    title Dialogue System Evolution
    
    section v1.3 (Current)
        Command Pattern : State Machine : Composition
     Support 10-20 NPC : 60 FPS : 74KB per NPC
 
    section v1.4 (1-2 months)
    Event Bus : Refactoring
        Support 50 NPC : 60 FPS : Reduced duplication
    
    section v1.5 (2-4 months)
        AI Integration : Sentiment Analysis
        Dynamic Responses : Conversation Memory
   Support 50-100 NPC : 60 FPS
    
    section v1.6 (4-6 months)
        Visual Editor : Shared Data Architecture
        Batch Processing : Spatial Partitioning
        Support 200 NPC : 60 FPS : 2KB per NPC
    
    section v2.0 (6-12 months)
     ECS Architecture : Multiplayer Support
        Advanced AI : Modding Support
   Support 500+ NPC : 60 FPS : 0.5KB per NPC
```

---

**Документ обновлён:** 2025-01-23  
**Версия:** 1.0  
**Инструмент:** Mermaid diagrams

*Все диаграммы можно рендерить в GitHub, GitLab, VS Code (с расширением), или на mermaid.live*
