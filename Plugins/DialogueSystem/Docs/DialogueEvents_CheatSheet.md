# ��������� �� �������� DialogueComponent

## ?? ��� ������� �������

| ������� | ����� ����������� | ��������� | ������ ������������� |
|---------|------------------|-----------|---------------------|
| **OnDialogueStarted** | ��� ������ ������� | `UDialogueRunner* Runner` | ��������� ����������, �������� UI |
| **OnDialogueEnded** | ��� ���������� ������� | ��� | �������� ����������, ������ UI |
| **OnDialogueNodeChanged** | ��� ����� ������� | `UDialogueNode* NewNode`<br>`UDialogueNode* PreviousNode` | �������, ��������, �������� |

---

## ? ������� ����� (Blueprint)

### ������� 1: ����� Details ������
1. ������� **Dialogue Component**
2. � Details ? Events ? ������ **+** ����� � ��������
3. �������� ������ � ��������� Event Node

### ������� 2: �������
```
Event BeginPlay
    ?
Get Dialogue Component
?
Bind Event to On Dialogue Started
```

---

## ?? ���-�������

### C++ - ������� ��������

```cpp
// � BeginPlay ��� ������������
void AMyNPC::BeginPlay()
{
    Super::BeginPlay();
    
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
        DialogueComp->OnDialogueStarted.AddDynamic(this, &AMyNPC::OnDialogueStart);
        DialogueComp->OnDialogueEnded.AddDynamic(this, &AMyNPC::OnDialogueEnd);
DialogueComp->OnDialogueNodeChanged.AddDynamic(this, &AMyNPC::OnNodeChange);
    }
}

// �����������
UFUNCTION()
void AMyNPC::OnDialogueStart(UDialogueRunner* Runner) { /* ������ */ }

UFUNCTION()
void AMyNPC::OnDialogueEnd() { /* ������ */ }

UFUNCTION()
void AMyNPC::OnNodeChange(UDialogueNode* NewNode, UDialogueNode* PrevNode) { /* ������ */ }
```

### C++ - ������� (�����!)

```cpp
void AMyNPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>())
    {
        DialogueComp->OnDialogueStarted.RemoveDynamic(this, &AMyNPC::OnDialogueStart);
        DialogueComp->OnDialogueEnded.RemoveDynamic(this, &AMyNPC::OnDialogueEnd);
        DialogueComp->OnDialogueNodeChanged.RemoveDynamic(this, &AMyNPC::OnNodeChange);
    }
  
    Super::EndPlay(EndPlayReason);
}
```

---

## ?? ������� �������� (Blueprint)

### ������� 1: ���������� ������
```
On Dialogue Started ? Get Player Controller ? Disable Input
On Dialogue Ended ? Get Player Controller ? Enable Input
```

### ������� 2: UI �������
```
On Dialogue Started ? Create Widget ? Add to Viewport ? Store Ref
On Dialogue Ended ? Get Ref ? Is Valid? ? Remove from Parent
```

### ������� 3: �������
```
On Dialogue Node Changed ? Is Valid (New Node)? 
  ? Get Voice Clip ? Play Sound 2D
```

### ������� 4: �������� ������
```
On Dialogue Node Changed ? Get Emotion Tag ? Switch on Name
  ?? Happy ? Play Animation (Smile)
  ?? Sad ? Play Animation (Frown)
  ?? Angry ? Play Animation (Angry)
```

### ������� 5: ����������
```
On Dialogue Ended ? Increment Counter ? Branch (Counter >= 10)
  ? True ? Unlock Achievement
```

---

## ?? ������� ������

```
1. StartDialogue()
     ?
2. OnDialogueStarted (Dialogue Component)
     ?
3. OnNodeEntered (Runner) ? OnDialogueNodeChanged (������ �������)
     ?
4. [����� �������� �������]
     ?
5. OnNodeEntered (Runner) ? OnDialogueNodeChanged (��������� �������)
     ?
6. [���������� ����� 4-5]
  ?
7. OnDialogueEnded (Dialogue Component)
```

---

## ?? ������ ������

| ������ | ������ | ������� |
|--------|--------|---------|
| **Previous Node = null** | ������ ������� | ����������� `Is Valid?` |
| **������� �� �����������** | �� ����������� | ��������� �������� � BeginPlay |
| **Crash ��� ����������** | �� ���������� | �������� RemoveDynamic � EndPlay |
| **UI �� ����������** | ������ Add to Viewport | ��������� ������� Create?Add |
| **������� �� ������** | Voice Clip = null | ��������� `Is Valid (Voice Clip)` |

---

## ?? �������

### ������� ������� (Blueprint)
```
On Dialogue Started ? Print String "Started!" (Green, 2s)
On Dialogue Ended ? Print String "Ended!" (Red, 2s)
On Node Changed ? Print String (Get Dialogue Text) (Yellow, 3s)
```

### ����������� ������� (C++)
```cpp
UE_LOG(LogTemp, Log, TEXT("Dialogue started with %s"), *Runner->GetName());
UE_LOG(LogTemp, Log, TEXT("Node changed: %s"), *NewNode->GetDialogueText().ToString());
UE_LOG(LogTemp, Log, TEXT("Dialogue ended"));
```

### �������� ���� ������
```ini
; DefaultEngine.ini
[Core.Log]
LogDialogueComponent=Verbose
LogDialogueRunner=Verbose
```

---

## ?? �������� �������

### ����� Dialogue Component
- `GetActiveRunner()` - ������� ������
- `IsInDialogue()` - ��� �� ������
- `GetRelationshipComponent()` - ��������� ���������
- `GetMemoryComponent()` - ��������� ������

### ����� Dialogue Runner
- `GetCurrentNode()` - ������� ����
- `GetContext()` - �������� �������
- `GetAvailableChoices()` - ��������� ��������
- `IsActive()` - ������� �� ������

### ����� Dialogue Node
- `GetDialogueText()` - ����� �������
- `GetSpeakerName()` - ��� ����������
- `VoiceClip` - ������� (����� ���� null)
- `EmotionTag` - ��� ������

---

## ?? ������� �� �������� ��������

### ������ 1: The Witcher-style �������
```cpp
// ������ ��� ��������������� ��������
void OnNodeChange(UDialogueNode* NewNode, UDialogueNode* PrevNode)
{
    if (NewNode && NewNode->VoiceClip)
    {
        float Duration = NewNode->VoiceClip->Duration;
  GetWorld()->GetTimerManager().SetTimer(
        AutoSkipTimer, 
            this, 
         &ADialogueManager::AutoSkipNode,
         Duration + 1.0f
        );
    }
}
```

### ������ 2: Mass Effect-style ����������
```cpp
// Interrupt actions
void OnNodeChange(UDialogueNode* NewNode, UDialogueNode* PrevNode)
{
    if (NewNode && NewNode->Tags.Contains("Renegade_Interrupt"))
    {
     ShowInterruptPrompt(EInterruptType::Renegade);
    }
}
```

### ������ 3: Disco Elysium-style �����
```cpp
// Internal thoughts
void OnNodeChange(UDialogueNode* NewNode, UDialogueNode* PrevNode)
{
    if (NewNode && NewNode->Speaker == "InternalVoice")
    {
        UIManager->ShowThoughtBubble(NewNode->GetDialogueText());
    }
}
```

---

## ?? ������ �� ������������

- **������ �����������:** [06_DialogueEvents_Guide.md](06_DialogueEvents_Guide.md)
- **������� �����:** [06_DialogueEvents_QuickStart.md](06_DialogueEvents_QuickStart.md)
- **�������:** [05_Complete_Examples.md](05_Complete_Examples.md)
- **API Reference:** [DialogueComponent.h](../Source/DialogueSystemRuntime/Public/Components/DialogueComponent.h)

---

## ?? ��������� ������ (�����������)

```
?? ���������: DialogueComponent Events

? �������:
  � OnDialogueStarted(Runner) - ������ �������
  � OnDialogueEnded() - ������ ����������  
  � OnDialogueNodeChanged(New, Prev) - ��������� �������

? �������� (C++):
  Component->OnDialogueStarted.AddDynamic(this, &Class::Handler);

?? �������� (BP):
  Dialogue Component ? Details ? Events ? +

?? �� ��������:
  � Is Valid? ��� Previous Node
  � RemoveDynamic � EndPlay
  � Is Valid? ��� Voice Clips

?? �������:
  Print String + Is Valid? = ??
```

---

**������:** 1.1.0  
**����:** 22.01.2025  
**�����:** DialogueSystem Team
