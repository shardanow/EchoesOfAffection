# Mouse Cursor Support в Dialogue System

## ?? Обзор

Версия 1.10 добавляет поддержку показа курсора мыши во время диалогов.

## ? Новые возможности

### Автоматический показ курсора
- ? Показывает курсор при начале диалога
- ? Включает клик события для UI
- ? Включает mouse-over события
- ? Восстанавливает оригинальное состояние после диалога

### Настройки в FDialogueInputBlockingSettings

```cpp
/** Show mouse cursor during dialogue */
bool bShowMouseCursor = true;

/** Enable click events during dialogue (for UI interaction) */
bool bEnableClickEvents = true;

/** Enable mouse over events during dialogue */
bool bEnableMouseOverEvents = true;
```

## ?? Использование

### В Blueprint:

#### Вариант 1: Автоматически (через настройки)

```
// В Details Panel DialogueInputBlocker компонента:
Default Blocking Settings:
  ?? Show Mouse Cursor = TRUE ?
  ?? Enable Click Events = TRUE ?
  ?? Enable Mouse Over Events = TRUE ?
```

Курсор будет автоматически показываться при вызове `BlockInput()` или `BlockInputWithDefaults()`.

#### Вариант 2: Программно

```blueprint
// Создайте custom настройки
[Make DialogueInputBlockingSettings]
    Blocking Mode = Block Movement And Camera
 Show Mouse Cursor = True
    Enable Click Events = True
    Enable Mouse Over Events = True
    ?
[Block Input] (с этими настройками)
```

#### Вариант 3: Только курсор (без блокировки ввода)

```blueprint
// Если хотите только показать курсор без блокировки
[Get Player Controller]
    ?
[Set Show Mouse Cursor] = True
[Set Enable Click Events] = True
[Set Enable Mouse Over Events] = True
```

### В C++:

```cpp
// В вашем DialogueComponent или Handler
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

## ?? Примеры использования

### Пример 1: Диалог с UI выбором

```blueprint
// NPC начинает диалог
Event: On Dialogue Started
    ?
[Get Component: DialogueInputBlocker]
    ?
[Make Settings]
    Blocking Mode = Block Movement And Camera
    Show Mouse Cursor = TRUE ?  // Для кликов по вариантам ответа
    Enable Click Events = TRUE ?
 ?
[Block Input] (settings)
```

### Пример 2: Диалог без UI (только текст)

```blueprint
// Простой текстовый диалог
Event: On Dialogue Started
    ?
[Get Component: DialogueInputBlocker]
    ?
[Make Settings]
    Blocking Mode = Block Movement And Camera
  Show Mouse Cursor = FALSE ?  // Курсор не нужен
 ?
[Block Input] (settings)
```

### Пример 3: Интерактивный диалог (hover effects)

```blueprint
// Диалог с hover эффектами на вариантах
Event: On Dialogue Started
    ?
[Get Component: DialogueInputBlocker]
    ?
[Make Settings]
    Show Mouse Cursor = TRUE ?
    Enable Click Events = TRUE ?
    Enable Mouse Over Events = TRUE ?  // Для hover эффектов
    ?
[Block Input] (settings)
```

## ?? Автоматическое восстановление

При вызове `UnblockInput()`:
1. ? Восстанавливается оригинальное состояние курсора
2. ? Восстанавливается состояние click events
3. ? Восстанавливается состояние mouse over events
4. ? Логируется в Output Log

```
LogDialogueInput: Restored mouse cursor state - Show: 0, Click: 0, MouseOver: 0
          ^^^    ^^^    ^^^
           Оригинальные значения (FALSE/0)
```

## ?? UI Integration

### DialogueWidget поддержка

Ваш DialogueWidget автоматически получит:
- ? Клик события на кнопках выбора
- ? Mouse-over для подсветки
- ? Курсор для наведения

```
// В вашем WBP_DialogueWidget:
Button_Option1
    ?? On Clicked ? [Select Dialogue Option 1]
    ?? On Hovered ? [Highlight Button]  // Работает с bEnableMouseOverEvents
```

## ?? Настройки по умолчанию

### Рекомендуемые настройки для разных типов диалогов:

#### 1. UI Dialogue (с кнопками выбора)
```
Show Mouse Cursor = TRUE
Enable Click Events = TRUE
Enable Mouse Over Events = TRUE
```

#### 2. Cutscene Dialogue (только текст)
```
Show Mouse Cursor = FALSE
Enable Click Events = FALSE
Enable Mouse Over Events = FALSE
```

#### 3. Quick Time Event Dialogue
```
Show Mouse Cursor = FALSE  // Используем клавиатуру
Enable Click Events = FALSE
Enable Mouse Over Events = FALSE
```

#### 4. Hybrid (текст + опциональный UI)
```
Show Mouse Cursor = TRUE
Enable Click Events = TRUE
Enable Mouse Over Events = TRUE
```

## ?? Отладка

### Проверка состояния курсора

В консоли Unreal:
```
Log LogDialogueInput Verbose
```

Вы увидите:
```
LogDialogueInput: Mouse cursor shown
LogDialogueInput: Click events enabled
LogDialogueInput: Mouse over events enabled
...
LogDialogueInput: Restored mouse cursor state - Show: 0, Click: 0, MouseOver: 0
```

### Проверка в Blueprint

```blueprint
// Debug: Check cursor state
[Get Player Controller]
    ?
[Get bShowMouseCursor] ? [Print String]
[Get bEnableClickEvents] ? [Print String]
```

## ?? Дополнительные функции

### Blueprint Helper Functions

Можно добавить в BlueprintFunctionLibrary:

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

