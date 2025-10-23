# Input Focus Fix v1.11 - Cursor Persistence Issue

## ?? ��������

����� ���������� ������� ������ ���� �������� ������� �� ��� ���, ���� ������������ �� ������� �� viewport ����.

### ��������:
```
1. ������ ������ ? ������ �������� ?
2. ��������� ������ ? ������ ������� ?
3. �������� �� ���� ? ������ ����� ?
```

## ?? �������

����� UI ������ ������� ������������, �� ����������� **input focus**. ��� �������� ������� ����� �������� �� UI, ���� �� ���������� ���� �� viewport.

**Unreal Engine Input Modes:**
- `GameOnly` - ����� �� ����, UI �� �������� ����
- `UIOnly` - ����� ������ �� UI
- `GameAndUI` - ����� �� �����, �� UI ����� ���������

## ? ������� v1.11

### �������������� �������������� Input Mode

������ `DialogueInputBlockerComponent` �������������:
1. ? ��������� ������������ input mode ����� ��������
2. ? ������������� `GameAndUI` ��� ������ (��� ������ �� UI)
3. ? ��������������� ������������ mode ����� �������
4. ? ���������� ����� �� ���� ����� `SetInputMode()`

### ��� ���������:

#### � BlockInput():
```cpp
// ��������� ������������ input mode
if (PC->bShowMouseCursor)
{
    OriginalInputMode = PC->bEnableClickEvents ? EInputMode::GameAndUI : EInputMode::GameOnly;
}
else
{
    OriginalInputMode = EInputMode::GameOnly;
}

// ������������� GameAndUI ��� ������� (���� ����� ������)
if (Settings.bShowMouseCursor)
{
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);
}
```

#### � UnblockInput():
```cpp
// ��������������� ������������ input mode
if (!bOriginalShowMouseCursor)
{
    FInputModeGameOnly InputMode;
    PC->SetInputMode(InputMode);
    UE_LOG(LogDialogueInput, Verbose, TEXT("Set input mode to Game Only"));
}
else
{
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);
    UE_LOG(LogDialogueInput, Verbose, TEXT("Set input mode to Game And UI"));
}
```

## ?? ����������

### �� ����������� (v1.10):
```
Start Dialogue:
  bShowMouseCursor = TRUE
  Input Focus = UI (������)
      ?
End Dialogue:
  bShowMouseCursor = FALSE ?
  Input Focus = UI ? (����� �� ������������!)
      ?
User clicks viewport:
  Input Focus = Game ? (������ ����� �����)
```

### ����� ����������� (v1.11):
```
Start Dialogue:
  bShowMouseCursor = TRUE
  Input Mode = Game And UI
  Input Focus = UI
      ?
End Dialogue:
  bShowMouseCursor = FALSE ?
  Input Mode = Game Only ?
  Input Focus = Game ? (�������������!)
      ?
User can play immediately! ?
```

## ?? ������������

### ���� 1: ������ � ��������
```
1. ������� ������ (������ ����������)
2. ��������� ������
3. ? ������ �������� �����
4. ? ����� ������� ��������� ��� �����
```

### ���� 2: ������ ��� �������
```
1. ������� ������ (������ �� ����������)
2. ��������� ������
3. ? ���������� �������� �����
```

### ���� 3: ������������ ������ �����������
```
Setup: �������� ������ � ���� (Settings.bShowMouseCursor = true)
1. ������� ������
2. ��������� ������
3. ? ������ �������� ������� (��� � ���� �� �������)
```

## ?? �����������

������ � Output Log �� �������:

```
// ��� ����������:
LogDialogueInput: Mouse cursor shown
LogDialogueInput: Set input mode to Game And UI for dialogue

// ��� �������������:
LogDialogueInput: Restored mouse cursor state - Show: 0, Click: 0, MouseOver: 0
LogDialogueInput: Set input mode to Game Only
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^
  ��� ��� - ����� ������������!
```

## ?? Changelog

### v1.11 (Current)
- ? Added `OriginalInputMode` tracking
- ? Added `EInputMode` enum
- ? Automatic `SetInputMode()` on block/unblock
- ? Saves original input mode before dialogue
- ? Restores input mode after dialogue
- ? Input focus returns to game immediately
- ? No more "click viewport to resume" issue

### v1.10
- ? Mouse cursor show/hide
- ? Click events enable/disable
- ? Input focus not restored (FIXED in v1.11)

## ?? Troubleshooting

### ������ ��� ��� ��������?

**���������:**
1. **������ ����������:** ��������� ��� ����������� v1.11+
2. **����:** ����� "Set input mode to Game Only" � Output Log
3. **UI ������:** ��������� ��� ������ ������������� �����������
4. **Custom code:** �������� �� ������� �������������� input mode � Blueprint

### ������ ��������� ����� �� ������?

**���������:**
```cpp
// � BlockInput ������ ����:
if (Settings.bShowMouseCursor)  // ? ������ ���� ������ ������� ������
{
    PC->SetInputMode(InputMode);
}
```

### Debug �������:

```
Log LogDialogueInput Verbose
```

����� ��������� ���� ����� �������:
```
LogDialogueInput: Set input mode to Game Only  ? ������ ����!
```

## ?? ���������� � UI

### ��� DialogueWidget:

��� ������ ������ �� ������ ������� ��������� input mode:

```cpp
// ������ ��� (�� �����):
void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();
 
    // ? �������:
    // FInputModeUIOnly InputMode;
    // GetOwningPlayer()->SetInputMode(InputMode);
}

void UDialogueWidget::NativeDestruct()
{
    // ? �������:
    // FInputModeGameOnly InputMode;
    // GetOwningPlayer()->SetInputMode(InputMode);
    
    Super::NativeDestruct();
}
```

**DialogueInputBlocker ������ ������ ��� �������������!**

---

**Version:** 1.11  
**Status:** Production Ready  
**Fix Type:** CRITICAL - Cursor/Focus Management  
**Compatibility:** UE 5.x, Enhanced Input + Legacy Input

