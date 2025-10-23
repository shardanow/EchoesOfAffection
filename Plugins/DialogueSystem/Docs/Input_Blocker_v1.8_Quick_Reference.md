# DialogueInputBlockerComponent v1.8 - Quick Reference

## ?? CRITICAL FIX v1.8: Enhanced Input Now Works!

### Проблема (v1.7 и ранее)
После `UnblockInput()` управление **полностью не работало** при использовании Enhanced Input System.

### Причина
Input Mapping Contexts очищались при блокировке, но **НЕ восстанавливались** при разблокировке.

### Решение v1.8
? Сохранение всех активных Input Mapping Contexts ПЕРЕД блокировкой  
? Восстановление всех контекстов с приоритетами ПОСЛЕ разблокировки  
? Использование `GetAppliedInputContexts()` API

---

## Как проверить что v1.8 работает

### 1. Включите Verbose логирование
```
Log LogDialogueInput Verbose
```

### 2. Запустите диалог и проверьте логи

#### ? ПРАВИЛЬНЫЕ логи (v1.8):
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

#### ? НЕПРАВИЛЬНЫЕ логи (v1.7):
```
LogDialogueInput: Clearing all input mapping contexts for dialogue
...
LogDialogueInput: Input contexts restored - game should re-add its default contexts
```
^^^ Контексты НЕ восстанавливались!

### 3. Проверьте управление после диалога
- ? WASD должно двигать персонажа
- ? Мышь должна вращать камеру
- ? Все Input Actions должны работать

---

## Использование в Blueprint

### Из NPC Actor
```blueprint
[Start Dialogue]
    ?
[Self] ? [Get Component: DialogueInputBlocker]
    ?
[BlockInputWithDefaults]
  ?
// Компонент автоматически:
// 1. Найдет PlayerController
// 2. Сохранит все Input Mapping Contexts
// 3. Очистит их
// 4. Добавит диалоговый контекст

[End Dialogue]
    ?
[Self] ? [Get Component: DialogueInputBlocker]
?
[UnblockInput]
    ?
// Компонент автоматически:
// 1. Найдет PlayerController
// 2. Очистит накопленный ввод
// 3. Восстановит ВСЕ сохраненные контексты
// 4. Восстановит состояние Pawn
```

---

## Changelog (кратко)

### v1.8 (ТЕКУЩАЯ) - Enhanced Input Context Restoration
- ? Сохранение Input Mapping Contexts
- ? Восстановление Input Mapping Contexts
- ? Использование `GetAppliedInputContexts()`
- ? Детальное логирование

### v1.7 - NPC PlayerController Fix
- ? Динамическое получение PlayerController
- ? Работа из NPC Blueprint

### v1.6 - Camera Jump & Legacy Input Fix
- ? `FlushPressedKeys()` против прыжка камеры
- ? `EnableInput()` для Legacy Input

---

## Troubleshooting

### Проблема: "No input contexts were saved!"
**Причина:** PlayerController не имел активных Input Mapping Contexts

**Решение:**
1. Убедитесь, что ваш PlayerController или Pawn добавляет Input Mapping Contexts в `BeginPlay()`
2. Проверьте что контексты добавлены ПЕРЕД началом диалога
3. Используйте `AddMappingContext()` в BeginPlay:
```cpp
// В вашем PlayerController::BeginPlay()
if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
{
    if (DefaultMappingContext)
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}
```

### Проблема: Управление все еще не работает
**Проверьте логи:**
```
LogDialogueInput: Restoring 0 input mapping contexts  // ? ПЛОХО!
LogDialogueInput: Restoring 2 input mapping contexts  // ? ХОРОШО!
```

**Решение:**
- Убедитесь что контексты существуют ДО диалога
- Проверьте BeginPlay() вашего PlayerController/Pawn
- Используйте Verbose логи для отладки

---

## API Reference (кратко)

```cpp
// Блокировка с defaults
DialogueInputBlocker->BlockInputWithDefaults();

// Блокировка с custom settings
FDialogueInputBlockingSettings Settings;
Settings.BlockingMode = EInputBlockingMode::BlockMovementAndCamera;
Settings.bDisablePawnMovement = true;
Settings.bRestorePawnState = true;
DialogueInputBlocker->BlockInput(Settings);

// Разблокировка (автоматически восстанавливает контексты!)
DialogueInputBlocker->UnblockInput();

// Проверка состояния
bool bBlocked = DialogueInputBlocker->IsInputBlocked();
```

---

**Версия:** 1.8  
**Статус:** ? Production Ready  
**Поддержка:** Enhanced Input + Legacy Input  
**Платформа:** UE5.0+
