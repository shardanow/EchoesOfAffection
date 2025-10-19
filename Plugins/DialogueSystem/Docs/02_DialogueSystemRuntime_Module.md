# DialogueSystemRuntime Module Documentation

## ����� ������

**DialogueSystemRuntime** � ��� runtime-������ ������� ��������, ��������������� ���������� ��� ���������� �������� � ������� �������, ���������� ������������, ����������� �������� ������� � ������� ��������� � NPC.

## �����������

```
DialogueSystemRuntime/
??? Public/
?   ??? Subsystems/
?   ?   ??? DialogueSubsystem.h       # ����������� ����������
?   ??? Components/
?   ?   ??? DialogueComponent.h       # ��������� ��� NPC
?   ?   ??? DialoguePresentationComponent.h  # UI �����������
?   ?   ??? DialogueCameraComponent.h  # ���������� �������
?   ?   ??? DialogueInputBlockerComponent.h  # ���������� �����
?   ?   ??? RelationshipComponent.h   # ������� ���������
?   ?   ??? NPCMemoryComponent.h      # ������ NPC
?   ??? Library/
?       ??? DialoguePresentationLibrary.h  # Blueprint �������
??? Private/
    ??? ...                           # ����������
```

---

## �������� ������

### 1. UDialogueSubsystem

**����������**: ����������� ���������� ��� ���������� ��������� �� ������ ���� ����. ��������� `IDialogueService` ��� ����������� dependency injection.

#### �������� �����������:

- **���������� ��������� ���������**: �����, ���������, �������� ����������
- **������ ��������**: DialogueRunner ������� ���������������� ��� ������������������
- **����������� ��������**: ������� ����������� ���������� ����� AssetManager
- **��������������**: �������������� ���������� � �������� ��������� ��������
- **���������**: ���������� ����������� �������� (�������� ������, ��������������� � �.�.)
- **�������**: �������� ������� ���������� ��������

#### �������� ������:

```cpp
// ������ �������
UFUNCTION(BlueprintCallable, Category = "Dialogue|Service")
UDialogueRunner* StartDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC);

// ������������� ������� (�� ����������)
UFUNCTION(BlueprintCallable, Category = "Dialogue|Service")
UDialogueRunner* ResumeDialogue(
    UDialogueDataAsset* DialogueAsset, 
    AActor* Player, 
    AActor* NPC, 
    const FDialogueSessionSaveData& SaveData
);

// ��������� �������� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue|Service")
void EndActiveDialogue();

// ���������, ������� �� ������
UFUNCTION(BlueprintPure, Category = "Dialogue|Service")
bool IsDialogueActive() const;

// �������� �������� ������
UFUNCTION(BlueprintPure, Category = "Dialogue|Service")
UDialogueRunner* GetActiveDialogue() const;
```

#### ����������� ��������:

```cpp
// ���������� ��������� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
void LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded);

// ���������������� ���� � ������� (��� ����������� ��������)
UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
void RegisterDialogueAsset(FName DialogueId, const FSoftObjectPath& AssetPath);

// �������� ��������
UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
void CancelAsyncLoad(FName DialogueId);

// ���������, ���� �� ��������
UFUNCTION(BlueprintPure, Category = "Dialogue|Loading")
bool IsAsyncLoadPending(FName DialogueId) const;

// �������� �������� �������� (0.0 - 1.0)
UFUNCTION(BlueprintPure, Category = "Dialogue|Loading")
float GetAsyncLoadProgress(FName DialogueId) const;
```

#### ����������/��������:

```cpp
// ��������� ��������� ��������
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
bool SaveDialogueState();

// ��������� ��������� ��������
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
bool LoadDialogueState();

// ���������, ���� �� �������� ����������
UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
bool HasActiveSaveState() const;

// �������� �������� ��������� ����������
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
FDialogueSessionSaveData GetActiveSaveState() const;

// �������� ��������� ����������
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
void ClearActiveSaveState();

// ��� �� ������ �������
UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
bool WasDialogueCompleted(FName DialogueId) const;

// �������� ������ ��� ����������
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
void MarkDialogueCompleted(FName DialogueId);

// �������� ���������� ��������� ����
UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
int32 GetNodeVisitCount(FName DialogueId, FName NodeId) const;
```

#### ���������:

```cpp
// �������� �������� "�������� �������" (�������� � �������)
UFUNCTION(BlueprintPure, Category = "Dialogue|Settings")
float GetTypewriterSpeed() const;

// ���������� �������� "�������� �������"
UFUNCTION(BlueprintCallable, Category = "Dialogue|Settings")
void SetTypewriterSpeed(float NewSpeed);

// �������� �� ���������������
UFUNCTION(BlueprintPure, Category = "Dialogue|Settings")
bool IsAutoAdvanceEnabled() const;

// ��������/��������� ���������������
UFUNCTION(BlueprintCallable, Category = "Dialogue|Settings")
void SetAutoAdvanceEnabled(bool bEnabled);
```

#### ������������������ (Pooling):

```cpp
// �������� ���������� ����
UFUNCTION(BlueprintPure, Category = "Dialogue|Performance")
void GetPoolStats(int32& OutAvailable, int32& OutInUse, int32& OutTotal) const;

// �������������� ������� Runner'� � ����
UFUNCTION(BlueprintCallable, Category = "Dialogue|Performance")
void PreWarmRunnerPool(int32 Count);

// �������� �������������� Runner'�
UFUNCTION(BlueprintCallable, Category = "Dialogue|Performance")
void TrimRunnerPool();
```

#### ������������� � Blueprint:

**������� ������ �������**:
```blueprintue
Event BeginPlay
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem (����������)

// ��� �������������� � NPC
Event E Pressed
    ? DialogueSubsystem ? Start Dialogue
        Dialogue Asset: DA_Dialogue_Lianne
        Player: Self
        NPC: Get Actor of Class [BP_NPC_Lianne]
        Return Value ? ActiveDialogueRunner
```

**����������� �������� �������**:
```blueprintue
Event BeginPlay
    ? DialogueSubsystem ? Register Dialogue Asset
        Dialogue Id: "Lianne_Main"
        Asset Path: /Game/Dialogues/DA_Dialogue_Lianne

// �����, ��� �������������
Event E Pressed
    ? DialogueSubsystem ? Load Dialogue Async
        Dialogue Id: "Lianne_Main"
        On Loaded: [Custom Event: OnDialogueLoaded]

// Custom Event: OnDialogueLoaded
    Loaded Dialogue (��������)
    Success (��������)
    ? Branch (���� Success)
        True ? DialogueSubsystem ? Start Dialogue (Loaded Dialogue, Player, NPC)
        False ? Print String "Failed to load dialogue"
```

**���������� � ��������**:
```blueprintue
// ���������� ��� ������ �� �������
Event OnDialogueEnded
    ? DialogueSubsystem ? Save Dialogue State

// �������� ��� ������� ����
Event BeginPlay
    ? DialogueSubsystem ? Load Dialogue State
    ? Branch
        True ? Print String "Dialogue state loaded"
        False ? Print String "No saved state"
```

**�������� ����������� �������**:
```blueprintue
Event BeginPlay
    ? DialogueSubsystem ? Was Dialogue Completed
        Dialogue Id: "Tutorial"
    ? Branch
        True ? Display Advanced Dialogue
        False ? Display Tutorial Dialogue
```

#### ������ � C++:

```cpp
// ��������� ����������
UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();

// ������ �������
UDialogueRunner* Runner = DialogueSubsystem->StartDialogue(
    DialogueAsset,
    PlayerActor,
    NPCActor
);

// ����������� ��������
DialogueSubsystem->RegisterDialogueAsset(
    TEXT("Lianne_Main"),
    FSoftObjectPath(TEXT("/Game/Dialogues/DA_Dialogue_Lianne"))
);

FOnDialogueLoadedDelegate OnLoaded;
OnLoaded.BindLambda([this](UDialogueDataAsset* LoadedDialogue, bool bSuccess)
{
    if (bSuccess && LoadedDialogue)
    {
        DialogueSubsystem->StartDialogue(LoadedDialogue, PlayerActor, NPCActor);
    }
});

DialogueSubsystem->LoadDialogueAsync(TEXT("Lianne_Main"), OnLoaded);

// ����������
DialogueSubsystem->SaveDialogueState();

// ��������
bool bLoaded = DialogueSubsystem->LoadDialogueState();

// �������� �����������
bool bCompleted = DialogueSubsystem->WasDialogueCompleted(TEXT("Tutorial"));
```

---

### 2. UDialogueComponent

**����������**: ��������� ��� NPC, ������� ������ ��������� �������, ��������� ��������������� � ������� � ������������� � ��������� ��������� � ������.

#### �������� ��������:

```cpp
// �������� ������ NPC (fallback)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
TSoftObjectPtr<UDialogueDataAsset> DefaultDialogue;

// �������������� ������� (�� ��������/���������)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
TMap<FName, TSoftObjectPtr<UDialogueDataAsset>> ConditionalDialogues;

// ID ��������� (��� ������� ���������)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
FName CharacterId;

// ������������ ��� ���������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
FText DisplayName;

// ������� ��� UI
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
TSoftObjectPtr<UTexture2D> Portrait;

// ����� �� ������ ������ ������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
bool bCanStartDialogue = true;

// ��������� ��������������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
float InteractionDistance = 300.0f;
```

#### ��������:

```cpp
// ����������� ��� ������ �������
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueStartedSignature OnDialogueStarted;

// ����������� ��� ��������� �������
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueEndedSignature OnDialogueEnded;
```

#### �������� ������:

```cpp
// ������ ������ � ���� NPC
UFUNCTION(BlueprintCallable, Category = "Dialogue")
bool StartDialogue(AActor* Initiator);

// ������� ���������� ������ �� ������ ���������
UFUNCTION(BlueprintCallable, Category = "Dialogue")
UDialogueDataAsset* SelectDialogue(const UDialogueSessionContext* Context);

// �������� Relationship Component
UFUNCTION(BlueprintPure, Category = "Dialogue")
URelationshipComponent* GetRelationshipComponent() const;

// �������� �������� Runner
UFUNCTION(BlueprintPure, Category = "Dialogue")
UDialogueRunner* GetActiveRunner() const;

// ������ �������?
UFUNCTION(BlueprintPure, Category = "Dialogue")
bool IsInDialogue() const;

// ���������� cooldown �� ���������� �������
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void SetDialogueCooldown(float Seconds);

// �������� Memory Component
UFUNCTION(BlueprintPure, Category = "Dialogue")
UNPCMemoryComponent* GetMemoryComponent() const;
```

#### ������������� � Blueprint:

**���������� ���������� � NPC**:
```blueprintue
// � Blueprint NPC (��������, BP_NPC_Lianne)

// 1. �������� ���������
Components Panel ? Add Component ? DialogueComponent

// 2. ��������� � Details Panel:
DialogueComponent:
  - Character Id: "Lianne"
  - Display Name: "������"
  - Portrait: T_Portrait_Lianne
  - Default Dialogue: DA_Dialogue_Lianne_Default
  - Conditional Dialogues:
      ["Morning"]: DA_Dialogue_Lianne_Morning
      ["Evening"]: DA_Dialogue_Lianne_Evening
  - Can Start Dialogue: true
  - Interaction Distance: 300.0
```

**������ ������� ��� ��������������**:
```blueprintue
// � BP_NPC_Lianne, Event Graph:

// ����� 1: ������ �����
Event E Pressed (Custom Input)
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    ? DialogueComp ? Start Dialogue
        Initiator: Get Player Controller ? Get Pawn

// ����� 2: ����� ��������� IDialogueParticipant
Event ActorBeginOverlap
    ? Cast to Player Character ? Player
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    ? DialogueComp ? Can Start Dialogue (Player)
    ? Branch
        True ? DialogueComp ? Start Dialogue (Player)
        False ? Print String "Cannot start dialogue now"
```

**�������� �� �������**:
```blueprintue
// � BP_NPC_Lianne, Event Graph:

// �������� �� ������� ������ �������
DialogueComponent ? On Dialogue Started
    ? Add Event [Handle Dialogue Started]
        Runner (��������)
        ? Play Animation [Talk_Happy]
        ? Set AI State [InDialogue]

// �������� �� ������� ��������� �������
DialogueComponent ? On Dialogue Ended
    ? Add Event [Handle Dialogue Ended]
        ? Stop Animation
        ? Set AI State [Idle]
```

**����� ������� �� ������ ���������**:
```blueprintue
// � BP_NPC_Lianne, Event Graph:

Event Tick
    ? Get Current Time of Day ? TimeTag
    ? DialogueComponent ? Select Dialogue
        Context: Create Context (Player, Self, TimeTag)
    ? Set Default Dialogue (Result)
```

#### ������ � C++:

```cpp
// ���������� ���������� � NPC � C++
UDialogueComponent* DialogueComp = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
DialogueComp->CharacterId = FName(TEXT("Lianne"));
DialogueComp->DisplayName = FText::FromString(TEXT("������"));
DialogueComp->DefaultDialogue = FSoftObjectPath(TEXT("/Game/Dialogues/DA_Dialogue_Lianne"));
DialogueComp->InteractionDistance = 300.0f;

// ���������� ��������� �������
DialogueComp->ConditionalDialogues.Add(
    FName(TEXT("Morning")),
    FSoftObjectPath(TEXT("/Game/Dialogues/DA_Dialogue_Lianne_Morning"))
);

// �������� �� �������
DialogueComp->OnDialogueStarted.AddDynamic(this, &AMyNPC::OnDialogueStarted);
DialogueComp->OnDialogueEnded.AddDynamic(this, &AMyNPC::OnDialogueEnded);

// �����������
void AMyNPC::OnDialogueStarted(UDialogueRunner* Runner)
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue started with %s"), *CharacterName);
    // ���������� AI � ����� �������
}

void AMyNPC::OnDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue ended"));
    // ������� AI � ������� �����
}

// ������ �������
bool AMyNPC::Interact(AActor* Initiator)
{
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
        return DialogueComp->StartDialogue(Initiator);
    }
    return false;
}
```

---

### 3. URelationshipComponent

**����������**: ���������� ����������� NPC � ������� (��������, �������, ������, ��������� � �.�.).

#### �������� ��������:

```cpp
// �������� ��������� (�� -100 �� 100)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Affinity = 0.0f;  // �������������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Trust = 0.0f;  // �������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Respect = 0.0f;  // ��������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Romance = 0.0f;  // ���������

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Friendship = 0.0f;  // ������

// ������� ������ ���������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
ERelationshipStatus Status = ERelationshipStatus::Neutral;
```

#### �������� ������:

```cpp
// �������� �������� ���������
UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyAffinity(float Delta);

UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyTrust(float Delta);

UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyRespect(float Delta);

UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyRomance(float Delta);

UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyFriendship(float Delta);

// ���������� �������� ���������
UFUNCTION(BlueprintCallable, Category = "Relationship")
void SetAffinity(float Value);

// �������� �������� ���������
UFUNCTION(BlueprintPure, Category = "Relationship")
float GetAffinity() const { return Affinity; }

// �������� ������ ���������
UFUNCTION(BlueprintPure, Category = "Relationship")
ERelationshipStatus GetStatus() const { return Status; }

// ���������� ������ ���������
UFUNCTION(BlueprintCallable, Category = "Relationship")
void SetStatus(ERelationshipStatus NewStatus);

// ���������, ���������� �� ����������� ��������
UFUNCTION(BlueprintPure, Category = "Relationship")
bool HasMinimumAffinity(float MinValue) const;

// �������� ����� ������� ��������� (�������)
UFUNCTION(BlueprintPure, Category = "Relationship")
float GetOverallRelationship() const;
```

#### ������� ���������:

```cpp
UENUM(BlueprintType)
enum class ERelationshipStatus : uint8
{
    Hostile,      // ����������
    Unfriendly,   // �������������
    Neutral,      // �����������
    Friendly,     // �����������
    Close,        // ������� ����
    Romantic,     // ������������� �������
    Married       // ������/�������
};
```

#### ������������� � Blueprint:

**���������� ����������**:
```blueprintue
// � BP_NPC_Lianne

Components Panel ? Add Component ? RelationshipComponent

// ��������� � Details Panel:
RelationshipComponent:
  - Affinity: 0.0
  - Trust: 0.0
  - Respect: 0.0
  - Romance: 0.0
  - Friendship: 0.0
  - Status: Neutral
```

**��������� ��������� � �������**:
```blueprintue
// � Dialogue Effect:
"affinity[Lianne] += 10"

// ��� ������� � Event Graph:
Event OnPlayerGaveGift
    ? Get Component by Class [RelationshipComponent]
    ? Modify Affinity (10.0)
    ? Modify Romance (5.0)
```

**�������� ������ ���������**:
```blueprintue
Event BeginPlay
    ? Get Component by Class [RelationshipComponent] ? RelComp
    ? RelComp ? Get Affinity ? Affinity
    ? Branch (Affinity >= 50)
        True ? Unlock Romantic Dialogue
        False ? Use Standard Dialogue
```

**�������������� ���������� �������**:
```blueprintue
Event Tick
    ? Get Component by Class [RelationshipComponent] ? RelComp
    ? RelComp ? Get Overall Relationship ? Overall
    ? Branch (Overall >= 80)
        True ? RelComp ? Set Status [Close]
    ? Branch (Overall >= 50)
        True ? RelComp ? Set Status [Friendly]
    ? Branch (Overall < 20)
        True ? RelComp ? Set Status [Unfriendly]
```

#### ������ � C++:

```cpp
// ���������� ����������
URelationshipComponent* RelComp = CreateDefaultSubobject<URelationshipComponent>(TEXT("RelationshipComponent"));

// ��������� ���������
RelComp->ModifyAffinity(10.0f);
RelComp->ModifyTrust(5.0f);

// �������� ������
if (RelComp->GetAffinity() >= 50.0f)
{
    // �������������� ������������� �������
}

// ��������� �������
RelComp->SetStatus(ERelationshipStatus::Friendly);

// ��������� ������ ������
float Overall = RelComp->GetOverallRelationship();
```

---

### 4. UNPCMemoryComponent

**����������**: �������� ������ NPC � ��������, ��������������� � ���������� � �������.

#### �������� ������:

```cpp
// ���������� ���� ������
UFUNCTION(BlueprintCallable, Category = "Memory")
void SetMemory(const FString& Key, bool Value);

// �������� ���� ������
UFUNCTION(BlueprintPure, Category = "Memory")
bool GetMemory(const FString& Key, bool DefaultValue = false) const;

// ���������� ���������� ������ (�����)
UFUNCTION(BlueprintCallable, Category = "Memory")
void SetMemoryVariable(const FString& Key, float Value);

// �������� ���������� ������
UFUNCTION(BlueprintPure, Category = "Memory")
float GetMemoryVariable(const FString& Key, float DefaultValue = 0.0f) const;

// �������� ���� ������� ��� ����������
UFUNCTION(BlueprintCallable, Category = "Memory")
void MarkNodeVisited(const FString& NodeId);

// ��� �� ���� �������
UFUNCTION(BlueprintPure, Category = "Memory")
bool WasNodeVisited(const FString& NodeId) const;

// �������� ���������� ��������� ����
UFUNCTION(BlueprintPure, Category = "Memory")
int32 GetNodeVisitCount(const FString& NodeId) const;

// �������� ��� ������
UFUNCTION(BlueprintCallable, Category = "Memory")
void ClearMemory();
```

#### ������������� � Blueprint:

**���������� ����������**:
```blueprintue
// � BP_NPC_Lianne

Components Panel ? Add Component ? NPCMemoryComponent
```

**��������� ������**:
```blueprintue
// ����� �������
Event OnQuestCompleted
    ? Get Component by Class [NPCMemoryComponent] ? MemoryComp
    ? MemoryComp ? Set Memory ("QuestCompleted", true)
    ? MemoryComp ? Set Memory Variable ("QuestReward", 500.0)
```

**�������� ������**:
```blueprintue
// � ������� �������
Event BeginPlay
    ? Get Component by Class [NPCMemoryComponent] ? MemoryComp
    ? MemoryComp ? Get Memory ("FirstMeeting")
    ? Branch
        True ? Load Dialogue [DA_Dialogue_Returning]
        False ? Load Dialogue [DA_Dialogue_First]
```

**������������ ��������� �����**:
```blueprintue
// � DialogueRunner, OnNodeChanged
Event OnNodeChanged
    Node (��������)
    ? Get Node Id ? NodeId
    ? Get Component by Class [NPCMemoryComponent] ? MemoryComp
    ? MemoryComp ? Mark Node Visited (NodeId)
    ? MemoryComp ? Get Node Visit Count (NodeId) ? Count
    ? Print String ("Visited {Count} times")
```

#### ������ � C++:

```cpp
// ���������� ����������
UNPCMemoryComponent* MemoryComp = CreateDefaultSubobject<UNPCMemoryComponent>(TEXT("MemoryComponent"));

// ��������� ������
MemoryComp->SetMemory(TEXT("FirstMeeting"), true);
MemoryComp->SetMemoryVariable(TEXT("GiftsGiven"), 3.0f);

// �������� ������
bool bFirstMeeting = MemoryComp->GetMemory(TEXT("FirstMeeting"));
float GiftsGiven = MemoryComp->GetMemoryVariable(TEXT("GiftsGiven"));

// ������� ����
MemoryComp->MarkNodeVisited(TEXT("Node_001"));

// �������� ���������
bool bVisited = MemoryComp->WasNodeVisited(TEXT("Node_001"));
int32 VisitCount = MemoryComp->GetNodeVisitCount(TEXT("Node_001"));
```

---

### 5. UDialoguePresentationComponent

**����������**: ���������� ���������� ������������ ������� (��������, �������, ���������).

#### �������� ������:

```cpp
// �������� ����� ������� � �������� "�������� �������"
UFUNCTION(BlueprintCallable, Category = "Presentation")
void DisplayTextWithTypewriter(const FText& Text, float Speed);

// �������� ������ ������
UFUNCTION(BlueprintCallable, Category = "Presentation")
void DisplayChoices(const TArray<FDialogueConnection>& Choices);

// ������������� �������� ����������
UFUNCTION(BlueprintCallable, Category = "Presentation")
void PlaySpeakerAnimation(const FName& AnimationName, AActor* Speaker);

// ��������/������ �������
UFUNCTION(BlueprintCallable, Category = "Presentation")
void ShowPortrait(UTexture2D* Portrait, bool bShow = true);

// ��������� ������ ���������/������������
UFUNCTION(BlueprintCallable, Category = "Presentation")
void FadeIn(float Duration = 0.5f);

UFUNCTION(BlueprintCallable, Category = "Presentation")
void FadeOut(float Duration = 0.5f);
```

#### ������������� � Blueprint:

**���������� ����������**:
```blueprintue
// � Actor, ������� ��������� UI �������

Components Panel ? Add Component ? DialoguePresentationComponent
```

**����������� ������ � ��������**:
```blueprintue
Event OnNodeChanged
    Node (��������)
    ? Get Dialogue Text ? Text
    ? DialoguePresentationComponent ? Display Text With Typewriter
        Text: Text
        Speed: 30.0
```

**����������� �������**:
```blueprintue
Event OnChoicesUpdated
    Choices (��������)
    ? DialoguePresentationComponent ? Display Choices
        Choices: Choices
```

**��������������� ��������**:
```blueprintue
Event OnNodeChanged
    Node (��������)
    ? Get Animation Name ? AnimName
    ? Get NPC Actor ? NPC
    ? DialoguePresentationComponent ? Play Speaker Animation
        Animation Name: AnimName
        Speaker: NPC
```

#### ������ � C++:

```cpp
// �������� ����������
UDialoguePresentationComponent* PresentationComp = CreateDefaultSubobject<UDialoguePresentationComponent>(TEXT("PresentationComponent"));

// ����������� ������
PresentationComp->DisplayTextWithTypewriter(DialogueText, 30.0f);

// ����������� �������
PresentationComp->DisplayChoices(AvailableChoices);

// ��������������� ��������
PresentationComp->PlaySpeakerAnimation(FName(TEXT("Talk_Happy")), NPCActor);

// �������� �������
PresentationComp->ShowPortrait(PortraitTexture, true);

// �������
PresentationComp->FadeIn(0.5f);
```

---

### 6. UDialogueCameraComponent

**����������**: ���������� ������� �� ����� ������� (������������ ����� ��������, ��������� ��������).

#### �������� ������:

```cpp
// ���������� ������ ��� ���� �������
UFUNCTION(BlueprintCallable, Category = "Camera")
void SetCameraForNode(const FName& CameraTag, float BlendTime = 1.0f);

// ��������� � ������ ������
UFUNCTION(BlueprintCallable, Category = "Camera")
void RestorePlayerCamera(float BlendTime = 1.0f);

// ������� ������������ ������ ��� �������
UFUNCTION(BlueprintCallable, Category = "Camera")
ACameraActor* CreateDialogueCamera(const FTransform& Transform);

// ���������� ����� ������ �� ������
UFUNCTION(BlueprintCallable, Category = "Camera")
void FocusOnActor(AActor* Target, float BlendTime = 1.0f);
```

#### ������������� � Blueprint:

**���������� ����������**:
```blueprintue
// � Actor, ������� ��������� ��������

Components Panel ? Add Component ? DialogueCameraComponent
```

**��������� ������ ��� ����**:
```blueprintue
Event OnNodeChanged
    Node (��������)
    ? Get Camera Tag ? CameraTag
    ? DialogueCameraComponent ? Set Camera For Node
        Camera Tag: CameraTag
        Blend Time: 1.0
```

**�������������� ������ ������**:
```blueprintue
Event OnDialogueEnded
    ? DialogueCameraComponent ? Restore Player Camera
        Blend Time: 1.0
```

**����� �� ������**:
```blueprintue
Event OnNodeChanged
    Node (��������)
    ? Get Speaker ? Speaker
    ? DialogueCameraComponent ? Focus On Actor
        Target: Speaker
        Blend Time: 0.5
```

#### ������ � C++:

```cpp
// �������� ����������
UDialogueCameraComponent* CameraComp = CreateDefaultSubobject<UDialogueCameraComponent>(TEXT("CameraComponent"));

// ��������� ������
CameraComp->SetCameraForNode(FName(TEXT("Camera.Closeup")), 1.0f);

// �������������� ������
CameraComp->RestorePlayerCamera(1.0f);

// �����
CameraComp->FocusOnActor(NPCActor, 0.5f);
```

---

### 7. UDialogueInputBlockerComponent

**����������**: ���������� �������� ����� �� ����� �������, ����� ����� �� ��� ��������� ��� ��������� ������ ��������.

#### �������� ������:

```cpp
// ������������� ����
UFUNCTION(BlueprintCallable, Category = "Input")
void BlockInput();

// �������������� ����
UFUNCTION(BlueprintCallable, Category = "Input")
void UnblockInput();

// ���������, ������������ �� ����
UFUNCTION(BlueprintPure, Category = "Input")
bool IsInputBlocked() const;
```

#### ������������� � Blueprint:

**���������� ����������**:
```blueprintue
// � Actor ������ ��� �����������

Components Panel ? Add Component ? DialogueInputBlockerComponent
```

**���������� ����� ��� ������ �������**:
```blueprintue
Event OnDialogueStarted
    ? DialogueInputBlockerComponent ? Block Input
```

**������������� ����� ��� ��������� �������**:
```blueprintue
Event OnDialogueEnded
    ? DialogueInputBlockerComponent ? Unblock Input
```

#### ������ � C++:

```cpp
// �������� ����������
UDialogueInputBlockerComponent* InputBlocker = CreateDefaultSubobject<UDialogueInputBlockerComponent>(TEXT("InputBlocker"));

// ����������
InputBlocker->BlockInput();

// �������������
InputBlocker->UnblockInput();

// ��������
bool bBlocked = InputBlocker->IsInputBlocked();
```

---

## ��������� ������� �������������

### ������ 1: ������ ���������� �������� � NPC (Blueprint)

**��� 1. ��������� NPC**:
```blueprintue
// � BP_NPC_Lianne

// 1. �������� ����������
Components Panel:
  ? Add Component ? DialogueComponent
  ? Add Component ? RelationshipComponent
  ? Add Component ? NPCMemoryComponent

// 2. ��������� DialogueComponent
DialogueComponent (Details Panel):
  - Character Id: "Lianne"
  - Display Name: "������"
  - Portrait: T_Portrait_Lianne
  - Default Dialogue: DA_Dialogue_Lianne_Default
  - Conditional Dialogues:
      ["Morning"]: DA_Dialogue_Lianne_Morning
      ["Evening"]: DA_Dialogue_Lianne_Evening
  - Can Start Dialogue: true
  - Interaction Distance: 300.0

// 3. ��������� RelationshipComponent
RelationshipComponent:
  - Affinity: 0.0
  - Trust: 0.0
  - Romance: 0.0
  - Status: Neutral
```

**��� 2. ��������� ��������������**:
```blueprintue
// � BP_NPC_Lianne, Event Graph:

// ��� ����������� ������
Event ActorBeginOverlap
    Other Actor (��������)
    ? Cast to Player Character ? Player
    ? Branch (cast success)
        True ? Enable Input (Self)
            ? Print String "Press E to talk"

// ��� �������� ������
Event ActorEndOverlap
    Other Actor (��������)
    ? Cast to Player Character ? Player
    ? Branch (cast success)
        True ? Disable Input (Self)

// ��� ������� E
Event E Pressed
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    ? DialogueComp ? Start Dialogue
        Initiator: Get Player Controller ? Get Pawn
    ? Branch (return value)
        True ? Print String "Dialogue started"
        False ? Print String "Cannot start dialogue"
```

**��� 3. ��������� ������� �������**:
```blueprintue
// � BP_NPC_Lianne, Event Graph:

// �������� �� �������
DialogueComponent ? On Dialogue Started
    ? Add Event [Handle Dialogue Started]
        Runner (��������)
        ? Play Animation Montage [AM_Talk_Happy]
        ? Set AI State [InDialogue]
        ? Disable Movement

DialogueComponent ? On Dialogue Ended
    ? Add Event [Handle Dialogue Ended]
        ? Stop Animation Montage
        ? Set AI State [Idle]
        ? Enable Movement
```

**��� 4. ��������� UI**:
```blueprintue
// � WBP_DialogueWidget, Event Graph:

// ��� �������� �������
Event Construct
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
    ? DialogueSubsystem ? Get Active Dialogue ? ActiveDialogue
    ? Branch (ActiveDialogue != null)
        True ? Initialize Widget with ActiveDialogue

// ���������� ��� ����� ����
Event OnNodeChanged (Custom)
    Node (��������)
    ? Get Dialogue Text ? Text
    ? Set Text (Text Block Widget, Text)
    ? Get Speaker Name ? SpeakerName
    ? Set Text (Speaker Name Widget, SpeakerName)
    ? Get Speaker Portrait ? Portrait
    ? Set Brush from Texture (Image Widget, Portrait)

// ��������� ������ �� ������ ������
Button Choice Click Event
    ? Get Button Index ? Index
    ? ActiveDialogue ? Select Choice (Index)
```

---

### ������ 2: ����������� �������� � ������ ������� (Blueprint)

**��� 1. ����������� �������� ��� �������**:
```blueprintue
// � BP_GameMode ��� BP_GameInstance, Event BeginPlay:

Event BeginPlay
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
    
    // ������������ ������� ��� ����������� ��������
    ? DialogueSubsystem ? Register Dialogue Asset
        Dialogue Id: "Lianne_Main"
        Asset Path: /Game/Dialogues/Lianne/DA_Dialogue_Lianne_Main
    
    ? DialogueSubsystem ? Register Dialogue Asset
        Dialogue Id: "Lianne_Romance"
        Asset Path: /Game/Dialogues/Lianne/DA_Dialogue_Lianne_Romance
    
    ? DialogueSubsystem ? Register Dialogue Asset
        Dialogue Id: "Marcus_Main"
        Asset Path: /Game/Dialogues/Marcus/DA_Dialogue_Marcus_Main
```

**��� 2. ����������� �������� ��� ��������������**:
```blueprintue
// � BP_NPC_Lianne, Event Graph:

Event E Pressed
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
    
    // ���������, ���� �� ��� ��������
    ? DialogueSubsystem ? Is Async Load Pending ("Lianne_Main")
    ? Branch
        True ? Print String "Loading dialogue, please wait..."
        False:
            ? DialogueSubsystem ? Load Dialogue Async
                Dialogue Id: "Lianne_Main"
                On Loaded: [Bind to Event: OnDialogueLoaded]

// Custom Event: OnDialogueLoaded
Event OnDialogueLoaded
    Loaded Dialogue (��������)
    Success (��������)
    ? Branch (Success)
        True:
            ? Get Game Instance
                ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
            ? DialogueSubsystem ? Start Dialogue
                Dialogue Asset: Loaded Dialogue
                Player: Get Player Pawn
                NPC: Self
        False:
            ? Print String "Failed to load dialogue"
```

**��� 3. ����������� ��������� �������� (�����������)**:
```blueprintue
// � WBP_LoadingDialogue, Event Tick:

Event Tick
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
    ? DialogueSubsystem ? Get Async Load Progress ("Lianne_Main") ? Progress
    ? Set Percent (Progress Bar Widget, Progress)
    ? Branch (Progress >= 1.0)
        True ? Hide Loading Widget
```

---

### ������ 3: ���������� � �������� ��������� �������� (C++)

```cpp
// � ����� GameMode ��� SaveGame �������

#include "Subsystems/DialogueSubsystem.h"
#include "Kismet/GameplayStatics.h"

void AMyGameMode::SaveGame()
{
    UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    
    // ��������� ��������� ��������
    bool bSaved = DialogueSubsystem->SaveDialogueState();
    if (bSaved)
    {
        UE_LOG(LogTemp, Log, TEXT("Dialogue state saved successfully"));
    }
    
    // �������� JSON ������������� ��� ��������������� ����������
    FString Json = DialogueSubsystem->SaveStateToJson();
    // ��������� Json � ��� SaveGame ������
}

void AMyGameMode::LoadGame()
{
    UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    
    // ��������� ��������� ��������
    bool bLoaded = DialogueSubsystem->LoadDialogueState();
    if (bLoaded)
    {
        UE_LOG(LogTemp, Log, TEXT("Dialogue state loaded successfully"));
    }
    
    // ��� ��������� �� JSON
    // FString Json = LoadJsonFromSaveGame();
    // DialogueSubsystem->LoadStateFromJson(Json);
}

void AMyGameMode::CheckDialogueCompletion()
{
    UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    
    // ���������, ��� �� ������� ������
    bool bCompleted = DialogueSubsystem->WasDialogueCompleted(FName(TEXT("Tutorial")));
    if (bCompleted)
    {
        // �������������� ��������� �������
        UnlockNextLevel();
    }
}
```

---

### ������ 4: ���������� � �������� ��������� (Blueprint + C++)

**� Blueprint (��������� ���������)**:
```blueprintue
// � Data Asset �������, ��������� Effect:

Connection[0]:
  - Effect: "affinity[Lianne] += 10; trust[Lianne] += 5"

// ��� ������� � Event Graph:
Event OnPlayerGaveGift
    ? Get Actor of Class [BP_NPC_Lianne] ? Lianne
    ? Lianne ? Get Component by Class [RelationshipComponent] ? RelComp
    ? RelComp ? Modify Affinity (10.0)
    ? RelComp ? Modify Trust (5.0)
    ? RelComp ? Modify Romance (3.0)
```

**� C++ (���������� IDialogueService ��� ���������)**:
```cpp
// � ����� DialogueService �����

float UMyDialogueService::GetRelationshipValue(const FString& NPCId, const FString& RelationType) const
{
    // ����� NPC �� ID
    AActor* NPCActor = FindNPCById(NPCId);
    if (!NPCActor)
    {
        return 0.0f;
    }
    
    // �������� RelationshipComponent
    URelationshipComponent* RelComp = NPCActor->FindComponentByClass<URelationshipComponent>();
    if (!RelComp)
    {
        return 0.0f;
    }
    
    // ������� ������ ��������
    if (RelationType == TEXT("affinity"))
    {
        return RelComp->GetAffinity();
    }
    else if (RelationType == TEXT("trust"))
    {
        return RelComp->GetTrust();
    }
    else if (RelationType == TEXT("romance"))
    {
        return RelComp->GetRomance();
    }
    
    return 0.0f;
}

void UMyDialogueService::ModifyRelationship(const FString& NPCId, const FString& RelationType, float Delta)
{
    AActor* NPCActor = FindNPCById(NPCId);
    if (!NPCActor)
    {
        return;
    }
    
    URelationshipComponent* RelComp = NPCActor->FindComponentByClass<URelationshipComponent>();
    if (!RelComp)
    {
        return;
    }
    
    if (RelationType == TEXT("affinity"))
    {
        RelComp->ModifyAffinity(Delta);
    }
    else if (RelationType == TEXT("trust"))
    {
        RelComp->ModifyTrust(Delta);
    }
    else if (RelationType == TEXT("romance"))
    {
        RelComp->ModifyRomance(Delta);
    }
}
```

---

## ������ ��������

1. **����������� DialogueSubsystem**: �� ���������� DialogueRunner �������, ����������� Subsystem ��� ��������� ����������� ������� � ����������.

2. **������������� ������� �������**: ��������������� ��� ������� ��� ������ ���� ��� ������� ����������� ��������.

3. **���������� �����**: ����������� �������������� ��� ���������� ��������� �������� ��� ������ ������ �������.

4. **����������� ����������**: DialogueComponent, RelationshipComponent � NPCMemoryComponent ������������ ����������� � �����������������.

5. **���������� ����**: ������ ���������� ������� ���� �� ����� �������, ����� �������� ������������� ��������.

6. **���������� �������**: ����������� DialogueCameraComponent ��� ������� ��������� ������ �� ����� �������.

7. **������������� UI**: ����������� DialoguePresentationComponent ��� �������������� UI �����������.

---

## ����������

**DialogueSystemRuntime** ������������� ������ ����� ����������� � ��������� ��� ���������� �������� � ���� ����. ��������� ��������� �����������, dependency injection � ������� �����������, �� ������ ����� ��������� � ��������� ������� ��� ���� �����.

��� ��������� �������������� ���������� ��.:
- `01_DialogueSystemCore_Module.md` � ���� ������� (Data, Conditions, Effects)
- `03_DialogueSystemUI_Module.md` � UI ������� ��� ��������
- `04_DialogueSystemAI_Module.md` � ���������� � AI
