# DialogueSystemCore Module Documentation

## ����� ������

**DialogueSystemCore** � ��� ���� ������� ��������, ��������������� ������� ������, ���������� � ������ ��� ������ � ���������. ���� ������ �� ������� �� UI ��� ���������� �������� � ����� �������������� ��� � C++, ��� � � Blueprint.

## �����������

```
DialogueSystemCore/
??? Public/
?   ??? Core/                   # �������� ������ ��������
?   ?   ??? DialogueNode.h      # ���� �������
?   ?   ??? DialogueConnection.h# ����� ����� ������
?   ?   ??? DialogueContext.h   # �������� ���������� �������
?   ?   ??? DialogueRunner.h    # ���������� ����������� �������
?   ??? Data/                   # ������ � ������
?   ?   ??? DialogueDataAsset.h # Data Asset ��� �������� ��������
?   ??? Conditions/             # ������� �������
?   ?   ??? DialogueConditionEvaluator.h
?   ??? Effects/                # ������� ��������
?   ?   ??? DialogueEffectExecutor.h
?   ??? SaveGame/               # ���������� ���������
?   ?   ??? DialogueSaveData.h
?   ??? Pool/                   # ��������� ���
?   ?   ??? DialogueObjectPool.h
?   ??? Interfaces/             # ����������
?       ??? IDialogueParticipant.h
?       ??? IDialogueService.h
??? Private/
    ??? ...                     # ����������
```

---

## �������� ������

### 1. UDialogueDataAsset

**����������**: Data Asset ��� �������� �������� � ���� ����� ����� � ������.

#### �������� ��������:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
TArray<UDialogueNode*> Nodes;  // ��� ���� �������

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
TArray<FDialogueConnection> Connections;  // ����� ����� ������

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
FString StartNodeId;  // ID ���������� ����

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
FGameplayTagContainer RequiredTags;  // ���� ��� ����������
```

#### ������������� � Blueprint:

1. **�������� Data Asset**:
   - � Content Browser: `Right Click ? Miscellaneous ? Data Asset`
   - �������� �����: `DialogueDataAsset`
   - ��������, ��������: `DA_Dialogue_Intro`

2. **���������� ������**:
   - �������� ��������� �����
   - � ������ `Nodes` �������� ���� ������� ����� `+`
   - � ������ `Connections` �������� ����� ����� ������
   - ������� `Start Node Id` (ID ������� ����)

#### ������ � C++:

```cpp
// �������� �������
UDialogueDataAsset* DialogueAsset = LoadObject<UDialogueDataAsset>(
    nullptr,
    TEXT("/Game/Dialogues/DA_Dialogue_Intro")
);

if (DialogueAsset)
{
    // �������� ��������� ����
    UDialogueNode* StartNode = DialogueAsset->GetNodeById(DialogueAsset->StartNodeId);
    
    // �������� ��� ��������� ����� �� ����
    TArray<FDialogueConnection> AvailableChoices = DialogueAsset->GetConnectionsFromNode(StartNode->NodeId);
}
```

---

### 2. UDialogueNode

**����������**: ������������ ���� ���� ������� (�������, ����� ������, ������� � �.�.).

#### ���� �����:

```cpp
UENUM(BlueprintType)
enum class EDialogueNodeType : uint8
{
    Dialogue,      // ������� ������� NPC
    PlayerChoice,  // ����� ������
    Condition,     // �������� ����
    Effect,        // ���� � ���������
    Random,        // ��������� �����
    Jump           // ������� � ������� ����
};
```

#### �������� ��������:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
FString NodeId;  // ���������� ID ����

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
EDialogueNodeType NodeType;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content", meta = (MultiLine = "true"))
FText DialogueText;  // ����� �������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
FName SpeakerName;  // ��� ����������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
TSoftObjectPtr<UTexture2D> SpeakerPortrait;  // ������� ����������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
TSoftObjectPtr<USoundBase> VoiceOver;  // �������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
FName AnimationName;  // ��� �������� ��� ������������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
FName CameraTag;  // ��� ������ ��� ����� ����

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
FGameplayTagContainer Tags;  // ���� ����
```

#### ������������� � Blueprint:

**�������� ����**:
```cpp
// � DialogueDataAsset, � ������� Nodes:
[0]
  - Node Id: "Node_001"
  - Node Type: Dialogue
  - Dialogue Text: "������! ��� ����?"
  - Speaker Name: "Lianne"
  - Speaker Portrait: T_Portrait_Lianne
  - Voice Over: VO_Lianne_Greeting
  - Animation Name: "Talk_Happy"
  - Camera Tag: "Camera.Closeup"
```

#### ������ � C++:

```cpp
// �������� ������ ����
UDialogueNode* NewNode = NewObject<UDialogueNode>();
NewNode->NodeId = TEXT("Node_001");
NewNode->NodeType = EDialogueNodeType::Dialogue;
NewNode->DialogueText = FText::FromString(TEXT("������! ��� ����?"));
NewNode->SpeakerName = FName(TEXT("Lianne"));

// ���������� � �����
DialogueAsset->Nodes.Add(NewNode);
```

---

### 3. FDialogueConnection

**����������**: ��������� ����� ����� ����� ������ ������� (������� �� ������ ���� � �������).

#### ���������:

```cpp
USTRUCT(BlueprintType)
struct FDialogueConnection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString FromNodeId;  // ID ��������� ����

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ToNodeId;  // ID �������� ����

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FText ChoiceText;  // ����� ������ (���� ��� ����� ������)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString Condition;  // ������� ��� ����������� (DSL ������)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString Effect;  // ������ ��� ������ (DSL ������)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Priority;  // ��������� (��� ����������)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer RequiredTags;  // ����������� ����

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHidden;  // ������ ����� �� ������
};
```

#### ������������� � Blueprint:

**�������� �����**:
```cpp
// � DialogueDataAsset, � ������� Connections:
[0]
  - From Node Id: "Node_001"
  - To Node Id: "Node_002"
  - Choice Text: "������, �������!"
  - Condition: "affinity[Lianne] >= 10"
  - Effect: "affinity[Lianne] += 5; memory(Greeted, true)"
  - Priority: 1
  - Required Tags: (�����)
  - Hidden: false
```

#### ������ � C++:

```cpp
// �������� �����
FDialogueConnection NewConnection;
NewConnection.FromNodeId = TEXT("Node_001");
NewConnection.ToNodeId = TEXT("Node_002");
NewConnection.ChoiceText = FText::FromString(TEXT("������, �������!"));
NewConnection.Condition = TEXT("affinity[Lianne] >= 10");
NewConnection.Effect = TEXT("affinity[Lianne] += 5; memory(Greeted, true)");
NewConnection.Priority = 1;

// ���������� � �����
DialogueAsset->Connections.Add(NewConnection);
```

---

### 4. UDialogueContext

**����������**: �������� ���������� �������, ���������� ��� ����������� ������ ��� ������ ������� � ���������� ��������.

#### �������� ��������:

```cpp
UPROPERTY(BlueprintReadWrite, Category = "Context")
AActor* Player;  // �����

UPROPERTY(BlueprintReadWrite, Category = "Context")
AActor* NPC;  // NPC, � ������� ���� ������

UPROPERTY(BlueprintReadWrite, Category = "Context")
TMap<FString, bool> Memories;  // ����� ������

UPROPERTY(BlueprintReadWrite, Category = "Context")
TMap<FString, float> Variables;  // ����������

UPROPERTY(BlueprintReadWrite, Category = "Context")
FGameplayTagContainer WorldStateTags;  // ���� ��������� ����

UPROPERTY(BlueprintReadWrite, Category = "Context")
FGameplayTag CurrentTimeTag;  // ������� ����� �����
```

#### ������:

```cpp
// ������ (�����)
UFUNCTION(BlueprintCallable)
void SetMemory(const FString& Key, bool Value);

UFUNCTION(BlueprintCallable)
bool GetMemory(const FString& Key, bool DefaultValue = false) const;

// ����������
UFUNCTION(BlueprintCallable)
void SetVariable(const FString& Key, float Value);

UFUNCTION(BlueprintCallable)
float GetVariable(const FString& Key, float DefaultValue = 0.0f) const;

// ����
UFUNCTION(BlueprintCallable)
bool HasTag(const FGameplayTag& Tag) const;

UFUNCTION(BlueprintCallable)
void AddTag(const FGameplayTag& Tag);

UFUNCTION(BlueprintCallable)
void RemoveTag(const FGameplayTag& Tag);
```

#### ������������� � Blueprint:

**�������� ���������**:
```blueprintue
// � Event Graph:

Event BeginPlay
    ? Create Dialogue Context
        Return Value ? DialogueContext (����������)
    ? Set Player (DialogueContext, Self)
    ? Set NPC (DialogueContext, Get Actor of Class [BP_NPC_Lianne])
```

**������ � �������**:
```blueprintue
// ��������� �����
Set Memory (DialogueContext, "FirstMeeting", true)

// �������� �����
Get Memory (DialogueContext, "FirstMeeting") 
    ? Branch (���� true, ��...)
```

**������ � �����������**:
```blueprintue
// ��������� ����������
Set Variable (DialogueContext, "QuestProgress", 3.0)

// ��������� ����������
Get Variable (DialogueContext, "QuestProgress")
    ? Print String
```

#### ������ � C++:

```cpp
// �������� ���������
UDialogueContext* Context = NewObject<UDialogueContext>();
Context->Player = PlayerActor;
Context->NPC = NPCActor;

// ������ � �������
Context->SetMemory(TEXT("FirstMeeting"), true);
bool bHasMet = Context->GetMemory(TEXT("FirstMeeting"));

// ������ � �����������
Context->SetVariable(TEXT("QuestProgress"), 3.0f);
float QuestProgress = Context->GetVariable(TEXT("QuestProgress"));

// ������ � ������
FGameplayTag WeatherTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Weather.Rainy")));
Context->AddTag(WeatherTag);
bool bIsRainy = Context->HasTag(WeatherTag);
```

---

### 5. UDialogueRunner

**����������**: ��������� ����������� �������, ������������ �������� ����� ������, ��������� ������� � ��������� �������.

#### �������� ������:

```cpp
// ������ �������
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void StartDialogue(UDialogueDataAsset* DialogueAsset, UDialogueContext* Context);

// �������� ������� ����
UFUNCTION(BlueprintCallable, Category = "Dialogue")
UDialogueNode* GetCurrentNode() const;

// �������� ��������� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue")
TArray<FDialogueConnection> GetAvailableChoices() const;

// ������� ������� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void SelectChoice(int32 ChoiceIndex);

// ���������� ������ (��� ����� ��� ������)
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void Continue();

// ��������� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void EndDialogue();

// ��������, ������� �� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue")
bool IsDialogueActive() const;
```

#### ������� (��������):

```cpp
// ����������� ��� ����� ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeChanged, UDialogueNode*, NewNode);

// ����������� ��� ���������� �������
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);

// ����������� ��� ���������� �������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicesUpdated, const TArray<FDialogueConnection>&, Choices);
```

#### ������������� � Blueprint:

**������� ����� �������**:
```blueprintue
// 1. �������� Runner
Event BeginPlay
    ? Create Dialogue Runner
        Return Value ? DialogueRunner (����������)
    
// 2. �������� �� �������
Dialogue Runner ? On Node Changed
    ? Add Event [Display Node]
        New Node ? Get Dialogue Text ? Set Text (Widget)

Dialogue Runner ? On Dialogue Ended
    ? Add Event [Close Dialogue Widget]

Dialogue Runner ? On Choices Updated
    ? Add Event [Populate Choice Buttons]

// 3. ������ �������
Dialogue Runner ? Start Dialogue
    Dialogue Asset: DA_Dialogue_Intro
    Context: DialogueContext

// 4. ����� �������� ������ (��� ����� �� ������)
Button Click Event
    ? Dialogue Runner ? Select Choice
        Choice Index: 0 (��� ������ ��������� ������)

// 5. ����������� ������� (��� ����� ��� ������)
Button "Continue" Click Event
    ? Dialogue Runner ? Continue

// 6. ���������� �������
Button "Exit" Click Event
    ? Dialogue Runner ? End Dialogue
```

#### ������ � C++:

```cpp
// �������� Runner
UDialogueRunner* Runner = NewObject<UDialogueRunner>();

// �������� �� �������
Runner->OnNodeChanged.AddDynamic(this, &AMyActor::OnDialogueNodeChanged);
Runner->OnDialogueEnded.AddDynamic(this, &AMyActor::OnDialogueEnded);
Runner->OnChoicesUpdated.AddDynamic(this, &AMyActor::OnChoicesUpdated);

// ������ �������
Runner->StartDialogue(DialogueAsset, Context);

// ����������� �������
void AMyActor::OnDialogueNodeChanged(UDialogueNode* NewNode)
{
    if (NewNode)
    {
        UE_LOG(LogTemp, Log, TEXT("Current Node: %s - %s"), 
               *NewNode->NodeId, 
               *NewNode->DialogueText.ToString());
    }
}

void AMyActor::OnDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue Ended"));
}

void AMyActor::OnChoicesUpdated(const TArray<FDialogueConnection>& Choices)
{
    for (int32 i = 0; i < Choices.Num(); ++i)
    {
        UE_LOG(LogTemp, Log, TEXT("Choice %d: %s"), i, *Choices[i].ChoiceText.ToString());
    }
}

// ����� �������� ������
Runner->SelectChoice(0);

// ����������� �������
Runner->Continue();
```

---

### 6. UDialogueConditionEvaluator

**����������**: ��������� �������, ���������� �� DSL (Domain-Specific Language), � ����������, �������� �� ��� ��� ���� �����/����.

#### �������� ������:

```cpp
// ������� ������� �� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue|Conditions")
bool EvaluateString(const FString& ConditionString, UDialogueContext* Context);

// ������� ������� (������)
UFUNCTION(BlueprintCallable, Category = "Dialogue|Conditions")
bool Evaluate(UDialogueCondition* Condition, UDialogueContext* Context);
```

#### �������������� ������� (DSL):

```cpp
// ��������
hasItem(ItemName)
hasItem(ItemName, Count)
has(Rose, 2)

// ������/�����
memory(Key)
memory(Key, Value)
visited(NodeId)

// ���������
affinity[NPCName] >= 50
affinity[Lianne] > 75

// ����������
variable[VarName] == Value
var[QuestProgress] >= 10

// �����
time == Morning
time == Evening

// ����
tag(TagName)
hasTag(Event.Festival)

// ���������� ���������
&&, ||, !, ()
```

#### ������������� � Blueprint:

**�������� ������� �� ������**:
```blueprintue
// �������� Evaluator
Create Dialogue Condition Evaluator
    ? Return Value ? ConditionEval (����������)

// ������ �������
Condition Eval ? Evaluate String
    Condition String: "hasItem(Rose) && affinity[Lianne] >= 30"
    Context: DialogueContext
    ? Branch
        True ? ��������� �������� A
        False ? ��������� �������� B
```

#### ������� �������:

```cpp
// ������� �������
"hasItem(Rose)"
"affinity[Lianne] >= 50"
"memory(FirstMeeting)"

// ������� �������
"hasItem(Rose) && affinity[Lianne] >= 30"
"(memory(QuestStarted) && !visited(Node_Boss)) || affinity[Marcus] > 80"
"time == Evening && hasItem(Letter) && variable[QuestStage] == 2"

// ��������� �������
"(affinity[Lianne] >= 50 || hasItem(Gift)) && !memory(GaveGift)"
```

#### ������ � C++:

```cpp
// �������� Evaluator
UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();

// ������ �������
FString ConditionString = TEXT("hasItem(Rose) && affinity[Lianne] >= 30");
bool bResult = Evaluator->EvaluateString(ConditionString, Context);

if (bResult)
{
    UE_LOG(LogTemp, Log, TEXT("Condition is TRUE"));
}
else
{
    UE_LOG(LogTemp, Log, TEXT("Condition is FALSE"));
}
```

---

### 7. UDialogueEffectExecutor

**����������**: ��������� �������, ���������� �� DSL, ������� �������� ��������� ���� (���������, ���������, ������ � �.�.).

#### �������� ������:

```cpp
// ��������� ������ �� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
void ExecuteString(const FString& EffectString, UDialogueContext* Context);

// ��������� ������ (������)
UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
void Execute(UDialogueEffect* Effect, UDialogueContext* Context);
```

#### �������������� ������� (DSL):

```cpp
// ���������
giveItem(ItemName, Count)
give(ItemName)
removeItem(ItemName, Count)
remove(ItemName, Count)

// ������
gold += 100
gold -= 50

// ���������
affinity[NPCName] += 10
affinity[NPCName] -= 5
trust[NPCName] += 15

// ������/�����
setMemory(Key, Value)
memory(Key, Value)
memory(Key)

// ����������
setVariable(VarName, Value)
var(VarName, Value)

// ������
startQuest(QuestId)
quest(QuestId)
completeQuest(QuestId)

// ����
addTag(TagName)
removeTag(TagName)

// ������������� ������� (����� ����� � �������)
effect1; effect2; effect3
```

#### ������������� � Blueprint:

**���������� ������� �� ������**:
```blueprintue
// �������� Executor
Create Dialogue Effect Executor
    ? Return Value ? EffectExec (����������)

// ���������� �������
Effect Exec ? Execute String
    Effect String: "giveItem(Rose, 1); affinity[Lianne] += 10; startQuest(RomanceLianne)"
    Context: DialogueContext
```

#### ������� ��������:

```cpp
// ��������� �������
"giveItem(Rose, 1)"
"affinity[Lianne] += 10"
"startQuest(RomanceLianne)"

// ������������� �������
"giveItem(Rose, 1); gold -= 25; affinity[Lianne] += 5"
"setMemory(GaveGift); startQuest(LianneQuest); affinity[Lianne] += 10"

// ������� ��������� ���������
"gold += 500; giveItem(Artifact); completeQuest(FindArtifact); addTag(Quest.Completed)"
```

#### ������ � C++:

```cpp
// �������� Executor
UDialogueEffectExecutor* Executor = NewObject<UDialogueEffectExecutor>();

// ���������� ��������
FString EffectString = TEXT("giveItem(Rose, 1); affinity[Lianne] += 10; startQuest(RomanceLianne)");
Executor->ExecuteString(EffectString, Context);
```

---

## ��������� ������� �������������

### ������ 1: ������� ������ � NPC (Blueprint)

**��� 1. �������� Data Asset**:
1. � Content Browser: `Right Click ? Miscellaneous ? Data Asset`
2. �������� �����: `DialogueDataAsset`
3. ��������: `DA_Dialogue_SimpleGreeting`

**��� 2. ���������� �����**:
```cpp
Nodes[0]:
  - Node Id: "Node_Start"
  - Node Type: Dialogue
  - Dialogue Text: "������! ��� ���� ������."
  - Speaker Name: "Lianne"

Nodes[1]:
  - Node Id: "Node_Player_Choice"
  - Node Type: PlayerChoice
  - Dialogue Text: "" (������ ��� ����� ������)

Nodes[2]:
  - Node Id: "Node_Response_Friendly"
  - Node Type: Dialogue
  - Dialogue Text: "�������! ����� ���������."
  - Speaker Name: "Lianne"

Nodes[3]:
  - Node Id: "Node_Response_Rude"
  - Node Type: Dialogue
  - Dialogue Text: "�� �����, ����� �� �������."
  - Speaker Name: "Lianne"
```

**��� 3. �������� ������**:
```cpp
Connections[0]:
  - From Node Id: "Node_Start"
  - To Node Id: "Node_Player_Choice"
  - Choice Text: "" (�������������� �������)

Connections[1]:
  - From Node Id: "Node_Player_Choice"
  - To Node Id: "Node_Response_Friendly"
  - Choice Text: "������! ��� ����?"
  - Effect: "affinity[Lianne] += 5"

Connections[2]:
  - From Node Id: "Node_Player_Choice"
  - To Node Id: "Node_Response_Rude"
  - Choice Text: "�� ������."
  - Effect: "affinity[Lianne] -= 2"
```

**��� 4. ������ ������� � Blueprint**:
```blueprintue
// � Actor BP (��������, BP_Player)

// ����������:
- DialogueRunner (UDialogueRunner*)
- DialogueContext (UDialogueContext*)
- CurrentDialogueAsset (UDialogueDataAsset*)

Event BeginPlay
    ? Create Dialogue Runner ? DialogueRunner
    ? Create Dialogue Context ? DialogueContext
    ? Set Player (DialogueContext, Self)

// ��� �������������� � NPC (��������, E)
Event E Pressed
    ? Set NPC (DialogueContext, Get Actor of Class [BP_NPC_Lianne])
    ? Load Asset [DA_Dialogue_SimpleGreeting] ? CurrentDialogueAsset
    ? Dialogue Runner ? Start Dialogue (CurrentDialogueAsset, DialogueContext)
```

---

### ������ 2: ������ � ��������� (Blueprint)

**��� 1. �������� Data Asset**:
1. �������� Data Asset: `DA_Dialogue_ConditionalGift`

**��� 2. ���������� �����**:
```cpp
Nodes[0]:
  - Node Id: "Node_Start"
  - Node Type: Dialogue
  - Dialogue Text: "�, �� ������ ��� �������?"
  - Speaker Name: "Lianne"

Nodes[1]:
  - Node Id: "Node_Player_Choice"
  - Node Type: PlayerChoice

Nodes[2]:
  - Node Id: "Node_Give_Rose"
  - Node Type: Dialogue
  - Dialogue Text: "����� �������� ����! �������!"
  - Speaker Name: "Lianne"

Nodes[3]:
  - Node Id: "Node_No_Gift"
  - Node Type: Dialogue
  - Dialogue Text: "�� �����, ����� � ��������� ���."
  - Speaker Name: "Lianne"
```

**��� 3. �������� ������ � ���������**:
```cpp
Connections[0]:
  - From Node Id: "Node_Start"
  - To Node Id: "Node_Player_Choice"

Connections[1]:
  - From Node Id: "Node_Player_Choice"
  - To Node Id: "Node_Give_Rose"
  - Choice Text: "���, ����� ����!"
  - Condition: "hasItem(Rose)"  // �������: � ������ ���� ����
  - Effect: "removeItem(Rose, 1); affinity[Lianne] += 15; memory(GaveGift, true)"

Connections[2]:
  - From Node Id: "Node_Player_Choice"
  - To Node Id: "Node_No_Gift"
  - Choice Text: "������, � ������ �� ������."
  - Effect: "affinity[Lianne] -= 3"
```

**��� 4. ������ �������**:
```blueprintue
// ���������� ������� 1, �� ����������� DA_Dialogue_ConditionalGift
```

**���������**: ���� � ������ ���� ���� � ���������, �� ������ ����� "���, ����� ����!". � ��������� ������ ���� ����� ����� �����.

---

### ������ 3: ������ � ������� � ����������� (C++)

```cpp
// � ����� Actor ��� Controller

void AMyPlayerController::StartDialogueWithLianne()
{
    // 1. �������� Context
    UDialogueContext* Context = NewObject<UDialogueContext>();
    Context->Player = GetPawn();
    Context->NPC = LianneActor;

    // 2. �������� �������
    UDialogueDataAsset* DialogueAsset = LoadObject<UDialogueDataAsset>(
        nullptr,
        TEXT("/Game/Dialogues/DA_Dialogue_Lianne_Main")
    );

    // 3. �������� Runner
    UDialogueRunner* Runner = NewObject<UDialogueRunner>();
    Runner->OnNodeChanged.AddDynamic(this, &AMyPlayerController::OnNodeChanged);
    Runner->OnDialogueEnded.AddDynamic(this, &AMyPlayerController::OnDialogueEnded);

    // 4. ��������� ��������� �������
    Context->SetMemory(TEXT("FirstMeeting"), true);  // �������, ��� �� ��� �����������
    Context->SetVariable(TEXT("LianneAffinity"), 50.0f);  // ��������� ���������

    // 5. ������ �������
    Runner->StartDialogue(DialogueAsset, Context);

    // ��������� Runner ��� ������������ �������������
    CurrentDialogueRunner = Runner;
    CurrentDialogueContext = Context;
}

void AMyPlayerController::OnNodeChanged(UDialogueNode* NewNode)
{
    if (NewNode)
    {
        UE_LOG(LogTemp, Log, TEXT("Node: %s - %s"), 
               *NewNode->NodeId, 
               *NewNode->DialogueText.ToString());

        // ��������� UI
        if (DialogueWidget)
        {
            DialogueWidget->UpdateDialogueText(NewNode->DialogueText);
            DialogueWidget->UpdateSpeakerName(NewNode->SpeakerName);

            // ��������� �������
            if (NewNode->SpeakerPortrait.IsValid())
            {
                UTexture2D* Portrait = NewNode->SpeakerPortrait.LoadSynchronous();
                DialogueWidget->UpdateSpeakerPortrait(Portrait);
            }
        }

        // �������� ��������� ������
        TArray<FDialogueConnection> Choices = CurrentDialogueRunner->GetAvailableChoices();
        if (Choices.Num() > 0)
        {
            // ��������� ������ ������ � UI
            if (DialogueWidget)
            {
                DialogueWidget->PopulateChoices(Choices);
            }
        }
    }
}

void AMyPlayerController::OnDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue Ended"));

    // ��������� UI
    if (DialogueWidget)
    {
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;
    }

    // ��������� ��������� Context
    SaveDialogueState();
}

void AMyPlayerController::OnPlayerSelectChoice(int32 ChoiceIndex)
{
    if (CurrentDialogueRunner && CurrentDialogueRunner->IsDialogueActive())
    {
        CurrentDialogueRunner->SelectChoice(ChoiceIndex);
    }
}

void AMyPlayerController::SaveDialogueState()
{
    if (CurrentDialogueContext)
    {
        // ��������� ������
        UDialogueSaveData* SaveData = NewObject<UDialogueSaveData>();
        SaveData->Memories = CurrentDialogueContext->Memories;
        SaveData->Variables = CurrentDialogueContext->Variables;
        SaveData->WorldStateTags = CurrentDialogueContext->WorldStateTags;

        // ��������� � ����
        UGameplayStatics::SaveGameToSlot(SaveData, TEXT("DialogueSaveSlot"), 0);
    }
}
```

---

## ���������� � ������� ���������

### ���������� � Inventory System

```cpp
// ������: �������� ������� �������� ����� IDialogueService

// � ����� ���� �������� �����, ����������� IDialogueService
class UMyDialogueService : public UObject, public IDialogueService
{
    GENERATED_BODY()

public:
    virtual bool HasItem(const FString& ItemName, int32 Count) const override
    {
        // �������� ��������� ��������� ������
        UInventoryComponent* Inventory = GetPlayerInventory();
        if (Inventory)
        {
            return Inventory->HasItem(ItemName, Count);
        }
        return false;
    }

    virtual void GiveItem(const FString& ItemName, int32 Count) override
    {
        UInventoryComponent* Inventory = GetPlayerInventory();
        if (Inventory)
        {
            Inventory->AddItem(ItemName, Count);
        }
    }

    virtual void RemoveItem(const FString& ItemName, int32 Count) override
    {
        UInventoryComponent* Inventory = GetPlayerInventory();
        if (Inventory)
        {
            Inventory->RemoveItem(ItemName, Count);
        }
    }

    // ... ������ ������ IDialogueService
};

// ����������� ������� � DialogueContext
Context->SetDialogueService(MyDialogueService);
```

### ���������� � Quest System

```cpp
// ���������� Inventory, ���������� ������ ������� � IDialogueService

virtual void StartQuest(const FString& QuestId) override
{
    UQuestSystem* QuestSystem = GetQuestSystem();
    if (QuestSystem)
    {
        QuestSystem->StartQuest(QuestId);
    }
}

virtual void CompleteQuest(const FString& QuestId) override
{
    UQuestSystem* QuestSystem = GetQuestSystem();
    if (QuestSystem)
    {
        QuestSystem->CompleteQuest(QuestId);
    }
}
```

---

## ���������� � �������� ���������

### UDialogueSaveData

**����������**: ���������� ��������� �������� (������, ����������, ����).

```cpp
USTRUCT(BlueprintType)
struct FDialogueSaveData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, SaveGame)
    TMap<FString, bool> Memories;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    TMap<FString, float> Variables;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FGameplayTagContainer WorldStateTags;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    TMap<FString, int32> VisitedNodesCount;  // ������� ��������� �����
};
```

#### ���������� (Blueprint):

```blueprintue
// �������� Save Data
Create Save Game Object [UDialogueSaveData] ? SaveData

// ���������� ������
Set Memories (SaveData, DialogueContext.Memories)
Set Variables (SaveData, DialogueContext.Variables)
Set World State Tags (SaveData, DialogueContext.WorldStateTags)

// ����������
Save Game to Slot (SaveData, "DialogueSaveSlot", 0)
```

#### �������� (Blueprint):

```blueprintue
// �������� Save Data
Load Game from Slot ("DialogueSaveSlot", 0) ? Cast to DialogueSaveData ? SaveData

// �������������� ���������
Get Memories (SaveData) ? Set Memories (DialogueContext, ...)
Get Variables (SaveData) ? Set Variables (DialogueContext, ...)
Get World State Tags (SaveData) ? Set World State Tags (DialogueContext, ...)
```

---

## ������ ��������

1. **����������� �����������**: DialogueRunner � Context �������� �������/������� ��� ��������� ������������������.

2. **���������� �������**: ���������� ��������� Data Assets ��� ������ ����/NPC.

3. **����������� ����**: ���� ��������� ����������� ������� �� ������� �����, ������, �������� � �.�.

4. **���������� �������**: ���������, ��� ������� ��������� ���������, ����� �������� ����������� �������.

5. **���������� ���������**: ��������� ���������� Context ��� �������������� ��������� ������.

6. **����������� ����������**: �������������� ���������� ��� ������, ����� �������������� ������� �������.

7. **��������� ������**: �������� ����������� ��� ������� ������� � ��������.

---

## ����������

**DialogueSystemCore** ������������� ������ � ������ ������ ��� �������� �������� � ����. ��������� DSL ��� ������� � ��������, �� ������ ����� ����������� ������� ������ �������� ��� ��������� ����. ������� ��������� ������������� � Blueprint � ������������ ���������� ����� ���������������� �������.

��� ��������� �������������� ���������� ��.:
- `02_DialogueSystemRuntime_Module.md` � Runtime ���������� (Subsystem, Components)
- `03_DialogueSystemUI_Module.md` � UI ������� ��� ��������
- `04_DialogueSystemAI_Module.md` � ���������� � AI

