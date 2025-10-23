# ����������� �� �������� �������� (Dialogue Events)

## �����

`DialogueComponent` ������������� ��� �������� ������� (��������), ������� ����� ������������ �� Blueprint ��� C++ ��� ������������ �� ��������� ��������� �������.

## ��������� �������

### 1. OnDialogueStarted
**���:** `FOnDialogueStartedSignature`  
**���������:** `UDialogueRunner* Runner`

���������� ����� ������ ����������.

**������ ������������� � Blueprint:**

```
????????????????????????
?  Dialogue Component  ?
????????????????????????
? OnDialogueStarted ??????? [Custom Logic]
????????????????????????    ?
     ?
    Print String: "Dialogue Started!"
           Disable Player Input
      Show Dialogue UI
```

**������ ������������� � C++:**

```cpp
// � ����� Actor/Component
void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
  
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
        DialogueComp->OnDialogueStarted.AddDynamic(this, &AMyNPC::HandleDialogueStarted);
  }
}

void AMyNPC::HandleDialogueStarted(UDialogueRunner* Runner)
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue started with runner: %s"), *Runner->GetName());
 // ���� ������ �����
}
```

---

### 2. OnDialogueEnded
**���:** `FOnDialogueEndedSignature`  
**���������:** ���

���������� ����� ������ �����������.

**������ ������������� � Blueprint:**

```
????????????????????????
?  Dialogue Component  ?
????????????????????????
? OnDialogueEnded ????????? [Custom Logic]
????????????????????????       ?
         ?
         Print String: "Dialogue Ended!"
        Enable Player Input
              Hide Dialogue UI
       Give Quest Reward
```

**������ ������������� � C++:**

```cpp
void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
    
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
        DialogueComp->OnDialogueEnded.AddDynamic(this, &AMyNPC::HandleDialogueEnded);
    }
}

void AMyNPC::HandleDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue ended"));
    // ������� ���������� ������
    // �������� ������
    // ��������� ���������
}
```

---

### 3. OnDialogueNodeChanged
**���:** `FOnDialogueNodeChangedSignature`  
**���������:** 
- `UDialogueNode* NewNode` - ����� �������/����
- `UDialogueNode* PreviousNode` - ���������� ���� (����� ���� nullptr ��� ������ �������)

���������� ������ ��� ����� ������ ��������� � ����� �������.

**������ ������������� � Blueprint:**

```
????????????????????????????
?   Dialogue Component  ?
????????????????????????????
? OnDialogueNodeChanged ??????? [Branch: Is Valid?]
????????????????????????????       ?
          ??? True  ??? Play Voice Line
   ?Update Subtitles
  ?            Trigger Animation
        ?
        ??? False ??? (First line)
```

**������ ������������� � C++:**

```cpp
void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
    
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
     DialogueComp->OnDialogueNodeChanged.AddDynamic(this, &AMyNPC::HandleNodeChanged);
    }
}

void AMyNPC::HandleNodeChanged(UDialogueNode* NewNode, UDialogueNode* PreviousNode)
{
    if (NewNode)
    {
 UE_LOG(LogTemp, Log, TEXT("New dialogue line: %s"), *NewNode->GetDialogueText().ToString());
        
        // ������������� �������
 if (NewNode->VoiceClip)
        {
     PlayVoiceClip(NewNode->VoiceClip);
    }
    
     // �������� ��������
        TriggerEmotionAnimation(NewNode->EmotionTag);
    }
}
```

---

## ������������ �������

### ������ 1: ������� ����� ��� ��������

```cpp
// � ����� PlayerController ��� ����������� ����������
void ADialogueCameraManager::SetupDialogueListeners()
{
    // ����� ��� DialogueComponent � ����
    TArray<AActor*> NPCs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), NPCs);
    
    for (AActor* Actor : NPCs)
    {
        if (UDialogueComponent* DialogueComp = Actor->FindComponentByClass<UDialogueComponent>())
        {
            DialogueComp->OnDialogueStarted.AddDynamic(this, &ADialogueCameraManager::OnAnyDialogueStarted);
            DialogueComp->OnDialogueEnded.AddDynamic(this, &ADialogueCameraManager::OnAnyDialogueEnded);
   }
    }
}

void ADialogueCameraManager::OnAnyDialogueStarted(UDialogueRunner* Runner)
{
    // ����������� �� ���������� ������
    SetDialogueCameraActive(true);
}

void ADialogueCameraManager::OnAnyDialogueEnded()
{
    // ������� ������� ������
    SetDialogueCameraActive(false);
}
```

### ������ 2: ������� ������� � ���������

```cpp
UCLASS()
class UDialogueAudioManager : public UActorComponent
{
    GENERATED_BODY()
    
public:
 UFUNCTION()
    void HandleNodeChanged(UDialogueNode* NewNode, UDialogueNode* PreviousNode)
    {
      // ���������� ���������� �������
        if (CurrentAudioComponent)
  {
            CurrentAudioComponent->Stop();
  }
        
    if (NewNode && NewNode->VoiceClip)
        {
        // ������������� ����� �������
 CurrentAudioComponent = UGameplayStatics::SpawnSound2D(
  GetWorld(), 
                NewNode->VoiceClip
            );
   
       // �������� ��������
            ShowSubtitles(NewNode->GetDialogueText(), NewNode->VoiceClip->Duration);
        }
    }
    
private:
    UPROPERTY(Transient)
    UAudioComponent* CurrentAudioComponent;
};
```

### ������ 3: ������� ����������

```cpp
void UAchievementManager::SetupDialogueTracking()
{
    // ����������� �� ��� ���������� ��������
    for (UDialogueComponent* DialogueComp : AllDialogueComponents)
    {
        DialogueComp->OnDialogueEnded.AddDynamic(this, &UAchievementManager::OnDialogueCompleted);
    }
}

void UAchievementManager::OnDialogueCompleted()
{
    TotalDialoguesCompleted++;
    
    // ��������� ����������
    if (TotalDialoguesCompleted >= 10)
    {
        UnlockAchievement("Chatty");
    }
    
    if (TotalDialoguesCompleted >= 100)
    {
        UnlockAchievement("Conversationalist");
 }
}
```

---

## Blueprint: ��������� �����������

### ��� 1: �������� Dialogue Component

1. � ����� Blueprint (��������, NPC Character)
2. �������� `Dialogue Component` ����� ���������� ������
3. ��� �������� ��� ����� `Get Component by Class`

### ��� 2: ����������� �� �������

1. �������� Dialogue Component
2. � Details ������ ������� ������ **Events**
3. ������� `+` ����� � ������ ��������:
   - `On Dialogue Started`
   - `On Dialogue Ended`
   - `On Dialogue Node Changed`

### ��� 3: ����������� ������

������ Blueprint Event Graph:

```
Event BeginPlay
    ?
    ?
Get Component by Class (DialogueComponent)
    ?
    ?
Bind Event to OnDialogueStarted ??? Custom Event: MyDialogueStartedHandler
    ?
    ?
Bind Event to OnDialogueEnded ??? Custom Event: MyDialogueEndedHandler
    ?
    ?
Bind Event to OnDialogueNodeChanged ??? Custom Event: MyNodeChangedHandler


Custom Event: MyNodeChangedHandler (NewNode, PreviousNode)
    ?
    ?
Is Valid? (NewNode)
    ??? True
    ?   ?
    ?   ?
    ?   Get Dialogue Text
    ?   ?
    ?   ?
    ?   Print String
    ?   ?
    ?   ?
    ?   Play Sound 2D (Voice Clip)
    ?
    ??? False
  ?
        ?
  (Do nothing - first line)
```

---

## ������ ���������

### ������� ������ �������

��� ������ �������:
1. `OnDialogueStarted` (���������)
2. `OnNodeEntered` ? `OnDialogueNodeChanged` (������ �������)

��� ����� �������:
1. `OnNodeEntered` ? `OnDialogueNodeChanged`

��� ���������� �������:
1. `OnDialogueEnded` (���������)

### Thread Safety

��� ������� ���������� � **Game Thread**, ������� ���������:
- ���������� � UObject
- �������� Blueprint ����
- ����������������� � UI

### Memory Management

- �� ������� ������ ������ �� `UDialogueRunner` ������ ��� ����������
- `PreviousNode` ����� ���� `nullptr` ��� ������ �������
- ������ ���������� `IsValid()` ����� �������������� �����

### Performance

������� ���������� ���������, �������:
- ��������� ������ �������� � ������������
- ����������� Async Tasks ��� ���������� ��������
- ��������� ������ ��� ��������

---

## ����� ���������� �������

**Q: ���� �� � ����������� �� ������� ���������� NPC ������������?**  
A: ��! ������ ����������� �� ������� ������� `DialogueComponent` ��������.

**Q: ��� ���������� �� �������?**  
A: ����������� `Remove` ��� `RemoveDynamic`:
```cpp
DialogueComp->OnDialogueStarted.RemoveDynamic(this, &AMyClass::MyHandler);
```

**Q: ����� �� �������� ������ ������ ����������� �������?**  
A: ��, �� ������ ��������� � ���������. ����� ������������ `JumpToNode` ����� ��������.

**Q: ������� ����������� � ������������?**  
A: ������� ��������� ��� ������� �������. ��� ������������� ����������� RPC.

---

## ��. �����

- [DialogueSystemRuntime Module](02_DialogueSystemRuntime_Module.md)
- [Complete Examples](05_Complete_Examples.md)
- [DialogueComponent API Reference](../Source/DialogueSystemRuntime/Public/Components/DialogueComponent.h)
