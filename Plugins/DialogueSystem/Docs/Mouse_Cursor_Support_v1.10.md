# Mouse Cursor Support � Dialogue System

## ?? �����

������ 1.10 ��������� ��������� ������ ������� ���� �� ����� ��������.

## ? ����� �����������

### �������������� ����� �������
- ? ���������� ������ ��� ������ �������
- ? �������� ���� ������� ��� UI
- ? �������� mouse-over �������
- ? ��������������� ������������ ��������� ����� �������

### ��������� � FDialogueInputBlockingSettings

```cpp
/** Show mouse cursor during dialogue */
bool bShowMouseCursor = true;

/** Enable click events during dialogue (for UI interaction) */
bool bEnableClickEvents = true;

/** Enable mouse over events during dialogue */
bool bEnableMouseOverEvents = true;
```

## ?? �������������

### � Blueprint:

#### ������� 1: ������������� (����� ���������)

```
// � Details Panel DialogueInputBlocker ����������:
Default Blocking Settings:
  ?? Show Mouse Cursor = TRUE ?
  ?? Enable Click Events = TRUE ?
  ?? Enable Mouse Over Events = TRUE ?
```

������ ����� ������������� ������������ ��� ������ `BlockInput()` ��� `BlockInputWithDefaults()`.

#### ������� 2: ����������

```blueprint
// �������� custom ���������
[Make DialogueInputBlockingSettings]
    Blocking Mode = Block Movement And Camera
 Show Mouse Cursor = True
    Enable Click Events = True
    Enable Mouse Over Events = True
    ?
[Block Input] (� ����� �����������)
```

#### ������� 3: ������ ������ (��� ���������� �����)

```blueprint
// ���� ������ ������ �������� ������ ��� ����������
[Get Player Controller]
    ?
[Set Show Mouse Cursor] = True
[Set Enable Click Events] = True
[Set Enable Mouse Over Events] = True
```

### � C++:

```cpp
// � ����� DialogueComponent ��� Handler
void AMyDialogueHandler::StartDialogue()
{
    if (UDialogueInputBlockerComponent* InputBlocker = 
        FindComponentByClass<UDialogueInputBlockerComponent>())
    {
        FDialogueInputBlockingSettings Settings;
        Settings.BlockingMode = EInputBlockingMode::BlockMovementAndCamera;
        Settings.bShowMouseCursor = true;
        Settings.bEnableClickEvents = true;
        Settings.bEnableMouseOverEvents = true;
    
        InputBlocker->BlockInput(Settings);
    }
}
```

## ?? ������� �������������

### ������ 1: ������ � UI �������

```blueprint
// NPC �������� ������
Event: On Dialogue Started
    ?
[Get Component: DialogueInputBlocker]
    ?
[Make Settings]
    Blocking Mode = Block Movement And Camera
    Show Mouse Cursor = TRUE ?  // ��� ������ �� ��������� ������
    Enable Click Events = TRUE ?
 ?
[Block Input] (settings)
```

### ������ 2: ������ ��� UI (������ �����)

```blueprint
// ������� ��������� ������
Event: On Dialogue Started
    ?
[Get Component: DialogueInputBlocker]
    ?
[Make Settings]
    Blocking Mode = Block Movement And Camera
  Show Mouse Cursor = FALSE ?  // ������ �� �����
 ?
[Block Input] (settings)
```

### ������ 3: ������������� ������ (hover effects)

```blueprint
// ������ � hover ��������� �� ���������
Event: On Dialogue Started
    ?
[Get Component: DialogueInputBlocker]
    ?
[Make Settings]
    Show Mouse Cursor = TRUE ?
    Enable Click Events = TRUE ?
    Enable Mouse Over Events = TRUE ?  // ��� hover ��������
    ?
[Block Input] (settings)
```

## ?? �������������� ��������������

��� ������ `UnblockInput()`:
1. ? ����������������� ������������ ��������� �������
2. ? ����������������� ��������� click events
3. ? ����������������� ��������� mouse over events
4. ? ���������� � Output Log

```
LogDialogueInput: Restored mouse cursor state - Show: 0, Click: 0, MouseOver: 0
          ^^^    ^^^    ^^^
           ������������ �������� (FALSE/0)
```

## ?? UI Integration

### DialogueWidget ���������

��� DialogueWidget ������������� �������:
- ? ���� ������� �� ������� ������
- ? Mouse-over ��� ���������
- ? ������ ��� ���������

```
// � ����� WBP_DialogueWidget:
Button_Option1
    ?? On Clicked ? [Select Dialogue Option 1]
    ?? On Hovered ? [Highlight Button]  // �������� � bEnableMouseOverEvents
```

## ?? ��������� �� ���������

### ������������� ��������� ��� ������ ����� ��������:

#### 1. UI Dialogue (� �������� ������)
```
Show Mouse Cursor = TRUE
Enable Click Events = TRUE
Enable Mouse Over Events = TRUE
```

#### 2. Cutscene Dialogue (������ �����)
```
Show Mouse Cursor = FALSE
Enable Click Events = FALSE
Enable Mouse Over Events = FALSE
```

#### 3. Quick Time Event Dialogue
```
Show Mouse Cursor = FALSE  // ���������� ����������
Enable Click Events = FALSE
Enable Mouse Over Events = FALSE
```

#### 4. Hybrid (����� + ������������ UI)
```
Show Mouse Cursor = TRUE
Enable Click Events = TRUE
Enable Mouse Over Events = TRUE
```

## ?? �������

### �������� ��������� �������

� ������� Unreal:
```
Log LogDialogueInput Verbose
```

�� �������:
```
LogDialogueInput: Mouse cursor shown
LogDialogueInput: Click events enabled
LogDialogueInput: Mouse over events enabled
...
LogDialogueInput: Restored mouse cursor state - Show: 0, Click: 0, MouseOver: 0
```

### �������� � Blueprint

```blueprint
// Debug: Check cursor state
[Get Player Controller]
    ?
[Get bShowMouseCursor] ? [Print String]
[Get bEnableClickEvents] ? [Print String]
```

## ?? �������������� �������

### Blueprint Helper Functions

����� �������� � BlueprintFunctionLibrary:

```cpp
UCLASS()
class UDialogueCursorHelpers : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
 /** Show cursor for dialogue */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Cursor")
    static void ShowDialogueCursor(UObject* WorldContextObject, bool bShow = true)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
      {
            PC->bShowMouseCursor = bShow;
            PC->bEnableClickEvents = bShow;
            PC->bEnableMouseOverEvents = bShow;
        }
    }
    
    /** Toggle dialogue cursor */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Cursor")
    static void ToggleDialogueCursor(UObject* WorldContextObject)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
     {
     PC->bShowMouseCursor = !PC->bShowMouseCursor;
        }
    }
};
```

## ?? Changelog

### v1.10 (Current)
- ? Added `bShowMouseCursor` to `FDialogueInputBlockingSettings`
- ? Added `bEnableClickEvents` to settings
- ? Added `bEnableMouseOverEvents` to settings
- ? Automatic restore of original cursor state
- ? Verbose logging for cursor state changes

---

**Version:** 1.10  
**Status:** Production Ready  
**Compatibility:** UE 5.x Enhanced Input + Legacy Input

