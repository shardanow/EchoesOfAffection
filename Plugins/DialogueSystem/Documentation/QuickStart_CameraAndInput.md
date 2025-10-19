# Camera Control & Input Blocking - Quick Start

## ��� �������� (Blueprint)

### ��� 1: �������� ���������
1. �������� Blueprint ������ ��������� ������
2. ������� "Add Component"
3. ������� "Dialogue Presentation Component"
4. �������� ���

### ��� 2: ���������
� Details ������ ����������:
- **Default Presentation Mode**: �������� "Cinematic"
- �������� ��������� ��������� �� ���������

### ��� 3: �����������
� Event Graph (��������, ��� �������������� � NPC):

```
[Event] On Interact With NPC
   ?
Start Dialogue Presentation (from DialoguePresentationLibrary)
   ?? Target NPC: [��� NPC Actor]
   ?? Mode: Cinematic
   ?? ?
   [���� ������ �������]
   ?? ?
   End Dialogue Presentation (from DialoguePresentationLibrary)
```

**������!** ������ ��� ������� ������ ������������� ������������ �� NPC, � ���������� �������������.

---

## ��� ������� ������������� (C++)

### ������� ����������

```cpp
// PlayerCharacter.h
UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
UDialoguePresentationComponent* PresentationComponent;

// PlayerCharacter.cpp (Constructor)
PresentationComponent = CreateDefaultSubobject<UDialoguePresentationComponent>(TEXT("DialoguePresentation"));

// �������������
void APlayerCharacter::StartDialogue(AActor* NPC)
{
    PresentationComponent->ActivatePresentation(NPC, EDialoguePresentationMode::Cinematic);
    // ... ���� ������ �������
}

void APlayerCharacter::EndDialogue()
{
    PresentationComponent->DeactivatePresentation();
}
```

---

## ������ ��������

### 1. ������������� ������ (����� ����� ���������)
```cpp
// Blueprint
Mode: Interactive

// C++
PresentationComponent->ActivatePresentation(NPC, EDialoguePresentationMode::Interactive);
```

### 2. First-Person ������
```cpp
// Blueprint
Mode: First-Person

// C++
PresentationComponent->ActivatePresentation(NPC, EDialoguePresentationMode::FirstPerson);
```

### 3. ����� ������ �� ����� �������
```cpp
// Blueprint
Transition To New Target (from PresentationComponent)
   ?? New Target NPC: [������ ��������]
   ?? Blend Time: 0.5

// C++
PresentationComponent->TransitionToNewTarget(OtherNPC, 0.5f);
```

### 4. ��������� ��������� ������
```cpp
// Blueprint
1. Make Default Camera Settings (from DialoguePresentationLibrary)
   ?? Framing: Close-Up
2. Set ��������� (Distance, Height, FOV, etc.)
3. Activate Dialogue Camera (from CameraComponent)
   ?? Target Actor: [NPC]
   ?? Settings: [���� ���������]

// C++
FDialogueCameraSettings CustomSettings;
CustomSettings.Framing = EDialogueCameraFraming::CloseUp;
CustomSettings.Distance = 150.0f;
CustomSettings.FOVOverride = 55.0f;
CameraComponent->ActivateDialogueCamera(NPC, CustomSettings);
```

---

## ������� �������

### ������ �� ������������?
- ���������, ��� ��������� �������� �� ��������� ������
- ���������, ��� `bEnableCamera = true`
- ���������, ��� NPC - �������� Actor

### ���� �� �����������?
- ���������, ��� `bEnableInputBlocking = true`
- ��� UE5 ���������, ��� Enhanced Input ��������
- ��������� ����� ���������� (Mode)

### ������ ���������?
- ��������� `Blend Time` (���������� 0.5-1.0)
- �������� `bEnableCollision = true`
- ���������, ��� `bUseSpringArm = true`

---

## ��������� ����

?? ������ �����������: `CameraAndInput_Guide.md`
?? ������� Blueprint: `Content/DialogueSystem/Examples/`
?? API ������������: �������� ����������� � `.h` ������

---

## ���������

���� �������� �������:
1. ��������� `CameraAndInput_Guide.md` (��������� ������������)
2. ������� ������� � `Content/DialogueSystem/Examples/`
3. ��������� ����: ��������� `LogDialogueCamera` � `LogDialogueInput`

---

**�����**: ������� � ������ "Cinematic" ��� �������� � "Interactive" ��� ��������� ����������.
