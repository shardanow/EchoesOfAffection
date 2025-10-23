# ������������� DialogueInputBlockerComponent �� NPC Blueprint

## ��������
��� ������ `UnblockInput()` �� NPC Actor Blueprint, ���������� ������ �� �����������������.

## �������
��������� ��������� PlayerController ��� `BeginPlay()` NPC, ��� ����� ����:
- ������������ ������������
- ���������� �������
- NULL-��, ���� ����� ��� �� ��� ������

## ������� (v1.7)

### ��������� � ����
1. **BeginPlay()** - ������ �� �������� PlayerController
2. **BlockInput()** - ������ �������� ������ PlayerController ����� `FindPlayerController()`
3. **UnblockInput()** - ������ �������� ������ PlayerController
4. **��������� �����������** - ��� ������� �������

### ��� ������������ � Blueprint

#### 1. �������� ��������� � NPC Actor
```
Components Panel:
?? NPC_Character (Root)
?? DialogueInputBlocker (Component)
?? ...
```

#### 2. � Event Graph NPC
```blueprint
[Event: Start Dialogue]
    ?
[Get Component: DialogueInputBlocker]
    ?
[BlockInputWithDefaults] ��� [BlockInput]
    ?
[... ���������� ������ ...]
    ?
[Event: End Dialogue]
    ?
[Get Component: DialogueInputBlocker]
    ?
[UnblockInput]
```

#### 3. ������ ������� Blueprint

```blueprint
// ������ �������
Event BeginPlay
    ?
[Bind Event: On Dialogue Started]
    ?
    OnDialogueStarted:
    ?
   [Get Component: DialogueInputBlocker]
        ?
        [BlockInputWithDefaults]
        ?
 [Show Dialogue UI]

// ����� �������  
Event Custom: On Dialogue Ended
    ?
    [Get Component: DialogueInputBlocker]
    ?
    [UnblockInput]
    ?
    [Hide Dialogue UI]
```

### ������ �������

#### ? ���������
```blueprint
// NPC Blueprint
[Start Dialogue Event]
    ?
[Self] ? [Get Component: DialogueInputBlocker]
    ?
[BlockInputWithDefaults]
    ?
// ��������� ��� ������ PlayerController

// End Dialogue Event
[Self] ? [Get Component: DialogueInputBlocker]
    ?
[UnblockInput]
    ?
// ��������� ��� ������ PlayerController
```

#### ? �����������
```blueprint
// �� ����� �������� PlayerController �������
[Get Player Controller] ? [DialogueInputBlocker] // ���!

// ��������� ��� ������ ���������� PlayerController
```

### �������� �����������������

#### 1. �������� Verbose �����������
� `DefaultEngine.ini` ��� ����� �������:
```ini
[Core.Log]
LogDialogueInput=Verbose
```

��� � ������� Unreal:
```
Log LogDialogueInput Verbose
```

#### 2. ��������� ���� ��� ����������
�� ������ �������:
```
LogDialogueInput: Input blocked with mode: 2 for PC: BP_PlayerController_C_0
```

#### 3. ��������� ���� ��� �������������
�� ������ �������:
```
LogDialogueInput: Unblocking input for PC: BP_PlayerController_C_0
LogDialogueInput: Flushed pressed keys
LogDialogueInput: Reset ignore input flags - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: Input unblocked successfully
```

#### 4. �������� � ����
����� �������������:
- ? ������� WASD ������ ������� ���������
- ? ���� ������ ������� ������
- ? ������ �� ������ "�������" ��� "�������"

### ������� �������

#### ��������: "No valid PlayerController found"
**�������:** PlayerController �� ���������� � ����

**�������:**
```blueprint
// ���������, ��� ���� ��������
[Is Valid] ? [Get Player Controller 0]
    ? (True)
[Start Dialogue]
```

#### ��������: ���������� �� �����������������
**��������� ����:**
```
LogDialogueInput: Reset ignore input flags - MoveIgnored: 1, LookIgnored: 1
^^^^^^^^^^^^   ^^^^^^^^^^^^
        ������ ���� 0!
```

**�������:**
- ���������, ��� ���������� `UnblockInput()`
- ���������, ��� ��� ������ ����������� ����������� ����
- ���������, ��� �� ���������� ������� ����������

#### ��������: ������ "�������" ����� �������������
**�������:** ����������� ���� ���� �� ���������

**��������� ����:**
```
LogDialogueInput: Flushed pressed keys  // ������ ����!
```

**�������:**
- �������� ��� �� ������ 1.7
- ���������, ��� `FlushPressedKeys()` ����������

### ����������� �������������

#### Custom Blocking Settings
```blueprint
[Create DialogueInputBlockingSettings]
    BlockingMode = Custom
    bBlockMovement = True
    bBlockCamera = True
    bBlockJump = False  // ��������� ������ �� ����� �������
    bDisablePawnMovement = True
  bRestorePawnState = True
    ?
[BlockInput] (� ����� �����������)
```

#### ������������ ��������� ������
```blueprint
// �� ����� ������� ����� �������� �����
[Is Important Dialogue Choice?]
    ? (True)
    [UpdateBlockingMode] ? BlockAll
    ? (False)
    [UpdateBlockingMode] ? BlockMovementAndCamera
```

### Performance Tips

1. **�� ���������� ��������� ������ ���**
   ```blueprint
   // ? �����
   [Construct] ? [Add Component: DialogueInputBlocker]
   [Destruct] ? [Destroy Component]
   
   // ? ������
   // �������� ��������� � Components Panel ���� ���
   ```

2. **����������� DefaultBlockingSettings**
   ```blueprint
   // ? ��������� ���� ��� � BeginPlay
   [BeginPlay]
       ?
   [SetDefaultBlockingSettings] (Custom Settings)
   
   // ����� ����� �����������
   [BlockInputWithDefaults]
   ```

3. **���������� IsInputBlocked ����� �����������**
   ```blueprint
   [IsInputBlocked?]
       ? (False)
     [BlockInput]
   ```

## Changelog

### v1.7 (Current)
- ? ���������� �������� � ���������� PlayerController �� NPC
- ? ������ �������� ������ PlayerController
- ? ��������� ��������� �����������
- ? �������� ��������� ������

### v1.6
- ����������� "������" ������
- ���������� �������������� Legacy Input
- Verbose ����������� ��������� Pawn

### v1.5
- ������� ����������/�������������
- Enhanced Input ���������
- Legacy Input �������������

## ������������

### ���� 1: ������� ����������
1. ��������� ����
2. ��������� � NPC
3. ������� ������ (input ������ ���������������)
4. ���������� ��������� (�� ������ ��������)
5. ���������� ������� ������ (�� ������ ��������)
6. ��������� ������ (input ������ ��������������)
7. ���������� ��������� � ������� ������ (������ ��������)

### ���� 2: ������������� �������
1. ������� ������ � NPC1
2. ��������� ������
3. ������� ������ � NPC2
4. ��������� ������
5. ���������, ��� ���������� �������� ���������

### ���� 3: ���������� ������
1. ������� ������
2. ���������� NPC (Destroy Actor)
3. ���������, ��� ���������� �������������� (EndPlay ������� UnblockInput)

## Support

��� ������������� �������:
1. �������� Verbose �����������
2. ��������� Output Log
3. ���������, ��� ����������� ������ 1.7+
4. ���������, ��� PlayerController ����������

## ������� ����

### C++ (���� ���������� ���������)
```cpp
// � ����� NPC ������
void ANPC_Character::StartDialogue()
{
    if (UDialogueInputBlockerComponent* InputBlocker = 
        FindComponentByClass<UDialogueInputBlockerComponent>())
    {
        FDialogueInputBlockingSettings Settings;
   Settings.BlockingMode = EInputBlockingMode::BlockMovementAndCamera;
        Settings.bDisablePawnMovement = true;
        Settings.bRestorePawnState = true;
        
        InputBlocker->BlockInput(Settings);
    }
}

void ANPC_Character::EndDialogue()
{
    if (UDialogueInputBlockerComponent* InputBlocker = 
 FindComponentByClass<UDialogueInputBlockerComponent>())
{
   InputBlocker->UnblockInput();
    }
}
```

### Blueprint Function Library (��� ��������)
```cpp
UCLASS()
class UDialogueHelpers : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    static void BlockPlayerInput(AActor* DialogueInstigator)
    {
        if (UDialogueInputBlockerComponent* Component = 
            DialogueInstigator->FindComponentByClass<UDialogueInputBlockerComponent>())
        {
            Component->BlockInputWithDefaults();
        }
    }
    
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    static void UnblockPlayerInput(AActor* DialogueInstigator)
    {
        if (UDialogueInputBlockerComponent* Component = 
         DialogueInstigator->FindComponentByClass<UDialogueInputBlockerComponent>())
        {
     Component->UnblockInput();
     }
    }
};
```

����� � Blueprint:
```blueprint
[NPC Actor] ? [BlockPlayerInput]
[NPC Actor] ? [UnblockPlayerInput]
```

---

**Version:** 1.7  
**Status:** Production Ready  
**Last Updated:** 2024
