# Input Focus Fix v1.11 - Cursor Persistence Issue

## ?? Проблема

После завершения диалога курсор мыши остается видимым до тех пор, пока пользователь не кликнет на viewport игры.

### Симптомы:
```
1. Начали диалог ? Курсор появился ?
2. Закончили диалог ? Курсор остался ?
3. Кликнули на игру ? Курсор исчез ?
```

## ?? Причина

Когда UI виджет диалога отображается, он захватывает **input focus**. При закрытии виджета фокус остается на UI, пока не произойдет клик на viewport.

**Unreal Engine Input Modes:**
- `GameOnly` - Фокус на игре, UI не получает ввод
- `UIOnly` - Фокус только на UI
- `GameAndUI` - Фокус на обоих, но UI имеет приоритет

## ? Решение v1.11

### Автоматическое восстановление Input Mode

Теперь `DialogueInputBlockerComponent` автоматически:
1. ? Сохраняет оригинальный input mode перед диалогом
2. ? Устанавливает `GameAndUI` при начале (для кликов по UI)
3. ? Восстанавливает оригинальный mode после диалога
4. ? Возвращает фокус на игру через `SetInputMode()`

### Код изменений:

#### В BlockInput():
```cpp
// Сохраняем оригинальный input mode
if (PC->bShowMouseCursor)
{
    OriginalInputMode = PC->bEnableClickEvents ? EInputMode::GameAndUI : EInputMode::GameOnly;
}
else
{
    OriginalInputMode = EInputMode::GameOnly;
}

// Устанавливаем GameAndUI для диалога (если нужен курсор)
if (Settings.bShowMouseCursor)
{
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    PC->SetInputMode(InputMode);
}
```

#### В UnblockInput():
```cpp
// Восстанавливаем оригинальный input mode
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

## ?? Результаты

### До исправления (v1.10):
```
Start Dialogue:
  bShowMouseCursor = TRUE
  Input Focus = UI (виджет)
      ?
End Dialogue:
  bShowMouseCursor = FALSE ?
  Input Focus = UI ? (фокус НЕ восстановлен!)
      ?
User clicks viewport:
  Input Focus = Game ? (только после клика)
```

### После исправления (v1.11):
```
Start Dialogue:
  bShowMouseCursor = TRUE
  Input Mode = Game And UI
  Input Focus = UI
      ?
End Dialogue:
  bShowMouseCursor = FALSE ?
  Input Mode = Game Only ?
  Input Focus = Game ? (автоматически!)
      ?
User can play immediately! ?
```

## ?? Тестирование

### Тест 1: Диалог с курсором
```
1. Начните диалог (курсор появляется)
2. Закончите диалог
3. ? Курсор исчезает СРАЗУ
4. ? Можно двигать персонажа БЕЗ клика
```

### Тест 2: Диалог без курсора
```
1. Начните диалог (курсор НЕ появляется)
2. Закончите диалог
3. ? Управление работает сразу
```

### Тест 3: Оригинальный курсор сохраняется
```
Setup: Включите курсор в игре (Settings.bShowMouseCursor = true)
1. Начните диалог
2. Закончите диалог
3. ? Курсор ОСТАЕТСЯ видимым (как и было до диалога)
```

## ?? Логирование

Теперь в Output Log вы увидите:

```
// При блокировке:
LogDialogueInput: Mouse cursor shown
LogDialogueInput: Set input mode to Game And UI for dialogue

// При разблокировке:
LogDialogueInput: Restored mouse cursor state - Show: 0, Click: 0, MouseOver: 0
LogDialogueInput: Set input mode to Game Only
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^
  ВОТ ОНО - ФОКУС ВОССТАНОВЛЕН!
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

### Курсор все еще остается?

**Проверьте:**
1. **Версия компонента:** Убедитесь что используете v1.11+
2. **Логи:** Ищите "Set input mode to Game Only" в Output Log
3. **UI виджет:** Убедитесь что виджет действительно закрывается
4. **Custom code:** Возможно вы вручную устанавливаете input mode в Blueprint

### Курсор пропадает когда не должен?

**Проверьте:**
```cpp
// В BlockInput должно быть:
if (Settings.bShowMouseCursor)  // ? Только если диалог требует курсор
{
    PC->SetInputMode(InputMode);
}
```

### Debug команда:

```
Log LogDialogueInput Verbose
```

Затем проверьте логи после диалога:
```
LogDialogueInput: Set input mode to Game Only  ? ДОЛЖНО БЫТЬ!
```

## ?? Интеграция с UI

### Для DialogueWidget:

Ваш виджет больше НЕ должен вручную управлять input mode:

```cpp
// СТАРЫЙ КОД (НЕ НУЖЕН):
void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();
 
    // ? УДАЛИТЕ:
    // FInputModeUIOnly InputMode;
    // GetOwningPlayer()->SetInputMode(InputMode);
}

void UDialogueWidget::NativeDestruct()
{
    // ? УДАЛИТЕ:
    // FInputModeGameOnly InputMode;
    // GetOwningPlayer()->SetInputMode(InputMode);
    
    Super::NativeDestruct();
}
```

**DialogueInputBlocker теперь делает это автоматически!**

---

**Version:** 1.11  
**Status:** Production Ready  
**Fix Type:** CRITICAL - Cursor/Focus Management  
**Compatibility:** UE 5.x, Enhanced Input + Legacy Input

