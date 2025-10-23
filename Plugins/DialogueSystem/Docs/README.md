# ������������ DialogueSystem Plugin

����� ���������� � ������������ ������� **DialogueSystem** ��� Unreal Engine 5!

## ?? ����������

### ������� ������������
1. **[DialogueSystemCore Module](01_DialogueSystemCore_Module.md)**  
   ���� �������: ����, ������, �������, �������

2. **[DialogueSystemRuntime Module](02_DialogueSystemRuntime_Module.md)**  
   Runtime ����������: DialogueComponent, ������, �����������

3. **[DialogueSystemUI Module](03_DialogueSystemUI_Module.md)**  
   UI ������� � ���������� � �����������

4. **[DialogueSystemAI Module](04_DialogueSystemAI_Module.md)**  
   AI ����������: ���������, ������, ������

5. **[Complete Examples](05_Complete_Examples.md)**  
 ������ ������� ���������� ���������� ������

### �����! ������� ��������

6. **[Dialogue Events Guide](06_DialogueEvents_Guide.md)** ? NEW  
   ������ ����������� �� ������ � ��������� �������� � C++ � Blueprint

7. **[Dialogue Events Quick Start](06_DialogueEvents_QuickStart.md)** ? NEW  
   ������� ����� ��� Blueprint �������������

8. **[Player Dialogue Events Guide](07_Player_Dialogue_Events_Guide.md)** ? NEW  
 ��� �������� ������� ������� �� Actor ������ (Blueprint �������)

### ����������� ������������

- **[DSL Parser Documentation](DSL_Parser_Documentation.md)**  
  ������������ �� ������� DSL ����� ��� ��������

---

## ?? ������� �����

### ��� ������������� (C++)
1. ���������� [DialogueSystemCore Module](01_DialogueSystemCore_Module.md)
2. ������� [Dialogue Events Guide](06_DialogueEvents_Guide.md)
3. ���������� ������� � [Complete Examples](05_Complete_Examples.md)

### ��� ���������� (Blueprint)
1. ������� � [Dialogue Events Quick Start](06_DialogueEvents_QuickStart.md)
2. ������� [DialogueSystemRuntime Module](02_DialogueSystemRuntime_Module.md)
3. ���������� [DialogueSystemUI Module](03_DialogueSystemUI_Module.md)

---

## ?? ��� ������?

### v1.1 - ������� ��������

��������� ��� �������� ������� � `DialogueComponent`:

#### ? OnDialogueStarted
���������� ��� ������ �������
```cpp
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueStartedSignature OnDialogueStarted;
```

#### ? OnDialogueEnded
���������� ��� ���������� �������
```cpp
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueEndedSignature OnDialogueEnded;
```

#### ? OnDialogueNodeChanged
���������� ��� ����� ������� (����)
```cpp
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueNodeChangedSignature OnDialogueNodeChanged;
```

**������ �������������:**
```cpp
// C++
DialogueComp->OnDialogueStarted.AddDynamic(this, &AMyNPC::HandleDialogueStarted);
DialogueComp->OnDialogueEnded.AddDynamic(this, &AMyNPC::HandleDialogueEnded);
DialogueComp->OnDialogueNodeChanged.AddDynamic(this, &AMyNPC::HandleNodeChanged);
```

��. [Dialogue Events Guide](06_DialogueEvents_Guide.md) ��� ������������.

---

#### ?? ���������� ������� ����� DialogueSubsystem

��������� ���������� ������� ��� ������������ ����� �������� � ����:

```cpp
// ����� ������ ������� (������ ������)
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnAnyDialogueStartedSignature OnAnyDialogueStarted;

// ����� ������ ����������
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnAnyDialogueEndedSignature OnAnyDialogueEnded;

// ����� ������� ���������
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnAnyDialogueNodeChangedSignature OnAnyDialogueNodeChanged;
```

**������ ��� Player Blueprint:**
```cpp
// � BeginPlay
Get Game Instance ? Get Subsystem (DialogueSubsystem)
    ? Bind: OnAnyDialogueStarted ? HandleStart
    ? Bind: OnAnyDialogueEnded ? HandleEnd
    ? Bind: OnAnyDialogueNodeChanged ? HandleNodeChange
```

��. [Player Dialogue Events Guide](07_Player_Dialogue_Events_Guide.md) ��� ������ ��������.

---

## ?? ��������� �������

```
DialogueSystem/
??? DialogueSystemCore/          # ���� ������� (data-driven)
?   ??? DialogueNode       # ���� ��������
?   ??? DialogueRunner   # ������ ����������
?   ??? DialogueConditions      # ������� �������
?   ??? DialogueEffects         # ������� ��������
?
??? DialogueSystemRuntime/       # Runtime ����������
?   ??? DialogueComponent       # �������� ��������� NPC
?   ??? DialogueCameraComponent # ������ ��� ��������
?   ??? DialogueSubsystem       # ���������� ��������
?   ??? RelationshipComponent   # ��������� � �������
?
??? DialogueSystemUI/       # UI �������
?   ??? DialogueWidget     # �������� ������ ��������
?   ??? ChoiceButton            # ������ ������
?   ??? DialogueHUD # HUD ����������
?
??? DialogueSystemAI/     # AI ����������
    ??? NPCMemoryComponent      # ������ NPC
    ??? EmotionSystem           # ������
    ??? BehaviorTree     # BT ������
```

---

## ?? �������� ���������

### 1. Data-Driven Architecture
��� ������ �������� �������� � **DialogueDataAsset**, ��� �������� � ���������� NPC.

### 2. Event-Based System
������� ���������� ������� (��������) ��� ����������� � ���������� ���������.

### 3. Dependency Injection
���������� ���������� ���������� (`IDialogueService`, `IDialogueParticipant`) ��� ����������.

### 4. Thread-Safe Design
��������� ��� ������������� � AI � ������������ ����������.

---

## ?? �������� �������� �������������

### �������� 1: ������� NPC � ����� ��������
```cpp
// 1. �������� DialogueComponent � NPC
// 2. ���������� DefaultDialogue � Details
// 3. ����������� �� ������� (�����������)
```

### �������� 2: NPC � ����������� ��������� �� ��������
```cpp
// 1. �������� DialogueComponent � RelationshipComponent
// 2. ��������� ConditionalDialogues ����
// 3. ������� ������������� ������� ���������� ������
```

### �������� 3: ������ � �������� � ����������
```cpp
// 1. ����������� �� OnDialogueNodeChanged
// 2. � ����������� �������������� VoiceClip �� ����
// 3. ���������� �������� �� ������ EmotionTag
```

### �������� 4: ��������� ������ ��� ��������
```cpp
// 1. �������� DialogueCameraComponent
// 2. ����������� �� OnDialogueStarted/Ended
// 3. ��������� blend ����� ��������
```

---

## ??? ����������� ����������

### Blueprint
- ��� ���������� �������� �� Blueprint
- ��� ������� ����� ������������ ����� Event Graph
- ������ ��������� Blueprint ��������

### C++
- ������ C++ API
- ������ ��������� �����������
- Thread-safe ��������

### Visual Scripting (�����������)
- ����� ������������ ������ Blueprint
- �� �� ����� ���� � �������

---

## ?? ������� ����������

### � EnhancedInput
```cpp
// ��������� E � ������ �������
InputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, 
  this, &APlayer::TryStartDialogue);
```

### � Behavior Tree
```cpp
// ������� BT Task ��� �������
UBTTask_StartDialogue::ExecuteTask()
{
    DialogueComponent->StartDialogue(PlayerCharacter);
}
```

### � Save System
```cpp
// ��������� ��������� ��������
DialogueSubsystem->SaveDialogueState(SaveGame);
DialogueSubsystem->LoadDialogueState(SaveGame);
```

---

## ?? �������

### �������� �����������
```cpp
// � DefaultEngine.ini
[Core.Log]
LogDialogueComponent=Verbose
LogDialogueRunner=Verbose
LogDialogueSubsystem=Verbose
```

### ������������ Print String � Blueprint
```
On Dialogue Started ??? Print String "Dialogue Started!"
On Dialogue Ended ??? Print String "Dialogue Ended!"
```

### ��������� ������������ ���
```cpp
if (ensure(NewNode))
{
    UE_LOG(LogTemp, Log, TEXT("Node: %s"), *NewNode->GetDialogueText().ToString());
}
```

---

## ?? ���������

### Issues
- �������� issue � ����������� GitHub
- ��������� ���� � ���������
- ������� ���� ���������������

### Pull Requests
- �������� code style �������
- �������� ����� (���� ��������)
- �������� ������������

### �������
- ��������� [Complete Examples](05_Complete_Examples.md)
- ���������� [Dialogue Events Guide](06_DialogueEvents_Guide.md)
- �������� � Discord/������ �������

---

## ?? Changelog

### v1.1.0 (2025-01-22)
- ? ��������� ������� ��������: OnDialogueStarted, OnDialogueEnded, OnDialogueNodeChanged
- ?? ��������� ���������� ������� � DialogueSubsystem: OnAnyDialogueStarted, OnAnyDialogueEnded, OnAnyDialogueNodeChanged
- ?? ����� ������������ �� �������� (3 ����� �����)
- ?? ����������� ����������� ��� Player Blueprint
- ?? ���������� memory leaks ��� ���������� �������
- ? �������������� �������� �� ������� Runner

### v1.0.0 (Initial Release)
- ?? ������ ����� DialogueSystem
- ? ������ ���������� Core, Runtime, UI, AI �������
- ?? ������� ������������

---

## ?? ��������

Copyright Epic Games, Inc. All Rights Reserved.

�������� LICENSE ���� � ����� �������.

---

## ?? �������������

������� ���� �������������� � ������������� �������!

---

**��������� ����������:** 22 ������ 2025  
**������ ������������:** 1.1.0  
**������ �������:** 1.1.0
