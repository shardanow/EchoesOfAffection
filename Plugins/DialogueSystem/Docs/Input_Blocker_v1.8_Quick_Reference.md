# DialogueInputBlockerComponent v1.8 - Quick Reference

## ?? CRITICAL FIX v1.8: Enhanced Input Now Works!

### �������� (v1.7 � �����)
����� `UnblockInput()` ���������� **��������� �� ��������** ��� ������������� Enhanced Input System.

### �������
Input Mapping Contexts ��������� ��� ����������, �� **�� �����������������** ��� �������������.

### ������� v1.8
? ���������� ���� �������� Input Mapping Contexts ����� �����������  
? �������������� ���� ���������� � ������������ ����� �������������  
? ������������� `GetAppliedInputContexts()` API

---

## ��� ��������� ��� v1.8 ��������

### 1. �������� Verbose �����������
```
Log LogDialogueInput Verbose
```

### 2. ��������� ������ � ��������� ����

#### ? ���������� ���� (v1.8):
```
LogDialogueInput: Saving 2 input mapping contexts
LogDialogueInput: Saved context: IMC_Default with priority 0
LogDialogueInput: Saved context: IMC_Player with priority 1
LogDialogueInput: Clearing all input mapping contexts for dialogue
...
LogDialogueInput: Restoring 2 input mapping contexts
LogDialogueInput: Restored context: IMC_Default with priority 0
LogDialogueInput: Restored context: IMC_Player with priority 1
LogDialogueInput: Input contexts restored successfully
```

#### ? ������������ ���� (v1.7):
```
LogDialogueInput: Clearing all input mapping contexts for dialogue
...
LogDialogueInput: Input contexts restored - game should re-add its default contexts
```
^^^ ��������� �� �����������������!

### 3. ��������� ���������� ����� �������
- ? WASD ������ ������� ���������
- ? ���� ������ ������� ������
- ? ��� Input Actions ������ ��������

---

## ������������� � Blueprint

### �� NPC Actor
```blueprint
[Start Dialogue]
    ?
[Self] ? [Get Component: DialogueInputBlocker]
    ?
[BlockInputWithDefaults]
  ?
// ��������� �������������:
// 1. ������ PlayerController
// 2. �������� ��� Input Mapping Contexts
// 3. ������� ��
// 4. ������� ���������� ��������

[End Dialogue]
    ?
[Self] ? [Get Component: DialogueInputBlocker]
?
[UnblockInput]
    ?
// ��������� �������������:
// 1. ������ PlayerController
// 2. ������� ����������� ����
// 3. ����������� ��� ����������� ���������
// 4. ����������� ��������� Pawn
```

---

## Changelog (������)

### v1.8 (�������) - Enhanced Input Context Restoration
- ? ���������� Input Mapping Contexts
- ? �������������� Input Mapping Contexts
- ? ������������� `GetAppliedInputContexts()`
- ? ��������� �����������

### v1.7 - NPC PlayerController Fix
- ? ������������ ��������� PlayerController
- ? ������ �� NPC Blueprint

### v1.6 - Camera Jump & Legacy Input Fix
- ? `FlushPressedKeys()` ������ ������ ������
- ? `EnableInput()` ��� Legacy Input

---

## Troubleshooting

### ��������: "No input contexts were saved!"
**�������:** PlayerController �� ���� �������� Input Mapping Contexts

**�������:**
1. ���������, ��� ��� PlayerController ��� Pawn ��������� Input Mapping Contexts � `BeginPlay()`
2. ��������� ��� ��������� ��������� ����� ������� �������
3. ����������� `AddMappingContext()` � BeginPlay:
```cpp
// � ����� PlayerController::BeginPlay()
if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
{
    if (DefaultMappingContext)
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}
```

### ��������: ���������� ��� ��� �� ��������
**��������� ����:**
```
LogDialogueInput: Restoring 0 input mapping contexts  // ? �����!
LogDialogueInput: Restoring 2 input mapping contexts  // ? ������!
```

**�������:**
- ��������� ��� ��������� ���������� �� �������
- ��������� BeginPlay() ������ PlayerController/Pawn
- ����������� Verbose ���� ��� �������

---

## API Reference (������)

```cpp
// ���������� � defaults
DialogueInputBlocker->BlockInputWithDefaults();

// ���������� � custom settings
FDialogueInputBlockingSettings Settings;
Settings.BlockingMode = EInputBlockingMode::BlockMovementAndCamera;
Settings.bDisablePawnMovement = true;
Settings.bRestorePawnState = true;
DialogueInputBlocker->BlockInput(Settings);

// ������������� (������������� ��������������� ���������!)
DialogueInputBlocker->UnblockInput();

// �������� ���������
bool bBlocked = DialogueInputBlocker->IsInputBlocked();
```

---

**������:** 1.8  
**������:** ? Production Ready  
**���������:** Enhanced Input + Legacy Input  
**���������:** UE5.0+
