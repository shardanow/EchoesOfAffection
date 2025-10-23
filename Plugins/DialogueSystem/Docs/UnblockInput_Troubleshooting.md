# ����������� �������� � UnblockInput

## ?? ���� ��� �������

### 1. �������� Verbose �����������
� ������� Unreal:
```
Log LogDialogueInput Verbose
```

### 2. ��������� ���� � ��������� ����

#### ? ��� ���������� ������ ���� ����:
```
LogDialogueInput: === BlockEnhancedInput START ===
LogDialogueInput: Adding dialogue context: IMC_Dialogue with priority 1000
LogDialogueInput: ? Dialogue input mapping context added successfully
LogDialogueInput: === BlockEnhancedInput END ===
LogDialogueInput: Input blocked with mode: X for PC: PlayerController_0
```

#### ? ��� ������������� ������ ���� ����:
```
LogDialogueInput: === UnblockInput START for PC: PlayerController_0 ===
LogDialogueInput: Flushed pressed keys
LogDialogueInput: BEFORE Restore - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: === RestoreEnhancedInput START ===
LogDialogueInput: Removing dialogue context: IMC_Dialogue
LogDialogueInput: Removed dialogue input mapping context
LogDialogueInput: === RestoreEnhancedInput END - Original contexts should be active ===
LogDialogueInput: AFTER Restore - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: AFTER Reset - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: Enhanced Input Subsystem exists
LogDialogueInput: === UnblockInput END - Success ===
```

### 3. ��������� �������� � �����

#### ?? �������� 1: MoveIgnored ��� LookIgnored = 1 ����� �������������
**����:**
```
LogDialogueInput: AFTER Reset - MoveIgnored: 1, LookIgnored: 1
      ^^^       ^^^
           ������ ���� 0!
```

**�������:** ���-�� ��� ��������� ���� (������ ���������, Blueprint, � �.�.)

**�������:**
- ������� ��� ����� ��� ���������� `SetIgnoreMoveInput(true)` ��� `SetIgnoreLookInput(true)`
- ��������� ��� ��� ������ ����������� ����������� ����
- ��������� DialoguePresentationComponent - �� ���� ����� �����������

#### ?? �������� 2: "No dialogue input mapping context"
**����:**
```
LogDialogueInput: No dialogue input mapping context - using legacy blocking
```

**�������:** `DialogueInputMappingContext` �� ���������� � ���������� ����������

**�������:**
1. �������� Blueprint � DialogueInputBlockerComponent
2. ������� ��������� `Dialogue Input Mapping Context`
3. ���������� ��� Input Mapping Context ��� ��������
4. ��� �������� ����� IMC � ������� ���������� (��������� ���)

#### ?? �������� 3: "Enhanced Input Subsystem is NULL"
**����:**
```
LogDialogueInput: Enhanced Input Subsystem is NULL!
```

**�������:** Enhanced Input �� �������� ��� ������

**�������:**
1. ��������� ��� � Project Settings ? Input ������� Enhanced Input
2. ��������� ��� PlayerController ����� LocalPlayer
3. ���� ����������� Legacy Input - ��������� ������������ �������������

#### ?? �������� 4: PlayerController �� ������
**����:**
```
LogDialogueInput: UnblockInput: No valid PlayerController found
```

**�������:** PlayerController �� ���������� ��� ��� ���������

**�������:**
- ��������� ��� UnblockInput ���������� ���� ����� ����������
- ��������� ��� �� ��������� UnblockInput ����� Destroy Actor

### 4. ��������� Blueprint ���������

#### �� Player Character/Controller:
1. **���� �� DialogueInputBlockerComponent?**
   - ���� ��� - �������� ���
   - ���� ���� - ��������� ���������

2. **��������� DialogueInputBlockerComponent:**
   ```
   Enable Input Blocking = TRUE
   Dialogue Input Mapping Context = [�������� IMC]
   Default Blocking Mode = Block Movement (��� Custom)
   ```

3. **����� �� Blueprint:**
   ```
   Event: On Dialogue Ended
       ?
   [Get Component: DialogueInputBlocker]
       ?
   [UnblockInput]
   ```

### 5. �������������� ������� - DialoguePresentationComponent

���� ������ �� ��������, ����������� **DialoguePresentationComponent**:

```
// � Player Character BeginPlay

[Add Component: DialoguePresentationComponent]
    ?
[Set Auto Activate On Dialogue Start = True]
    ?
[Set Auto Deactivate On Dialogue End = True]
```

�� ������������� ��������� ������ � �������.

### 6. �������� ����� Blueprint Debugger

1. �������� Blueprint ������
2. ���������� Breakpoint �� ���� `UnblockInput`
3. ��������� ���� � ������ Play In Editor (PIE)
4. ����� ������ ������������� - ���������:
   - ���������� �� UnblockInput?
   - ����� ��������� ������������?
   - ���� �� ������?

### 7. �������� Input Mapping Contexts � Runtime

�������� ��� ���������� ������� � ����:
```
ShowDebug EnhancedInput
```

��� �������:
- ��� �������� Input Mapping Contexts
- �� ����������
- ����� �������� �������������

### 8. ������� ������� - ������ �����

���� ������ �� ��������, �������� � UnblockInput:

```cpp
// � ����� Player Character C++

void AMyPlayerCharacter::ForceUnblockInput()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        // ����� ���� ������
        PC->ResetIgnoreInputFlags();
        PC->EnableInput(PC);
        
        // �������������� Enhanced Input
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
       ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
   {
        // ������� ��� ���������
  Subsystem->ClearAllMappings();
       
          // �������� ��������� �������
            if (DefaultInputMappingContext)
            {
           Subsystem->AddMappingContext(DefaultInputMappingContext, 0);
  }
        }
 }
}
```

## ?? ��������� ����������

### ? ��������� (�������� ��������):
```
LogDialogueInput: === UnblockInput START ===
LogDialogueInput: BEFORE Restore - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: AFTER Restore - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: Enhanced Input Subsystem exists
LogDialogueInput: === UnblockInput END - Success ===

// � ����:
- WASD ������� ��������� ?
- ���� ������� ������ ?
```

### ? ����������� (�������� �� ��������):
```
LogDialogueInput: === UnblockInput START ===
LogDialogueInput: AFTER Reset - MoveIgnored: 1, LookIgnored: 1
    ^^^
             ��������!

// � ����:
- WASD �� ������� ��������� ?
- ���� �� ������� ������ ?
```

## ?? ���� ������ �� ��������

1. ��������� ��� � ��� ������ v1.8+ ����
2. �������� Intermediate � Binaries �����
3. ������������ ������ (Rebuild)
4. ��������� ��� �� ��������� BlockInput ����� UnblockInput
5. ����������� DialoguePresentationComponent ������ ������� ������

---

**��������� ���:** ��������� ����, ��������� ������, � �������� ���� �� Output Log!
