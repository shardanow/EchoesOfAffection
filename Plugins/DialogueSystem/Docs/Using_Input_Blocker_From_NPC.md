# Использование DialogueInputBlockerComponent из NPC Blueprint

## Проблема
При вызове `UnblockInput()` из NPC Actor Blueprint, управление игрока не восстанавливается.

## Причина
Компонент кэшировал PlayerController при `BeginPlay()` NPC, что могло быть:
- Неправильным контроллером
- Устаревшей ссылкой
- NULL-ом, если игрок еще не был создан

## Решение (v1.7)

### Изменения в коде
1. **BeginPlay()** - больше НЕ кэширует PlayerController
2. **BlockInput()** - ВСЕГДА получает свежий PlayerController через `FindPlayerController()`
3. **UnblockInput()** - ВСЕГДА получает свежий PlayerController
4. **Детальное логирование** - для отладки проблем

### Как использовать в Blueprint

#### 1. Добавьте компонент в NPC Actor
```
Components Panel:
?? NPC_Character (Root)
?? DialogueInputBlocker (Component)
?? ...
```

#### 2. В Event Graph NPC
```blueprint
[Event: Start Dialogue]
    ?
[Get Component: DialogueInputBlocker]
    ?
[BlockInputWithDefaults] или [BlockInput]
    ?
[... диалоговая логика ...]
    ?
[Event: End Dialogue]
    ?
[Get Component: DialogueInputBlocker]
    ?
[UnblockInput]
```

#### 3. Пример полного Blueprint

```blueprint
// Начало диалога
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

// Конец диалога  
Event Custom: On Dialogue Ended
    ?
    [Get Component: DialogueInputBlocker]
    ?
    [UnblockInput]
    ?
    [Hide Dialogue UI]
```

### Важные моменты

#### ? ПРАВИЛЬНО
```blueprint
// NPC Blueprint
[Start Dialogue Event]
    ?
[Self] ? [Get Component: DialogueInputBlocker]
    ?
[BlockInputWithDefaults]
    ?
// Компонент САМ найдет PlayerController

// End Dialogue Event
[Self] ? [Get Component: DialogueInputBlocker]
    ?
[UnblockInput]
    ?
// Компонент САМ найдет PlayerController
```

#### ? НЕПРАВИЛЬНО
```blueprint
// НЕ НУЖНО получать PlayerController вручную
[Get Player Controller] ? [DialogueInputBlocker] // НЕТ!

// Компонент сам найдет правильный PlayerController
```

### Проверка работоспособности

#### 1. Включите Verbose логирование
В `DefaultEngine.ini` или через консоль:
```ini
[Core.Log]
LogDialogueInput=Verbose
```

Или в консоли Unreal:
```
Log LogDialogueInput Verbose
```

#### 2. Проверьте логи при блокировке
Вы должны увидеть:
```
LogDialogueInput: Input blocked with mode: 2 for PC: BP_PlayerController_C_0
```

#### 3. Проверьте логи при разблокировке
Вы должны увидеть:
```
LogDialogueInput: Unblocking input for PC: BP_PlayerController_C_0
LogDialogueInput: Flushed pressed keys
LogDialogueInput: Reset ignore input flags - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: Input unblocked successfully
```

#### 4. Проверка в игре
После разблокировки:
- ? Клавиши WASD должны двигать персонажа
- ? Мышь должна вращать камеру
- ? Камера НЕ должна "прыгать" или "улетать"

### Отладка проблем

#### Проблема: "No valid PlayerController found"
**Причина:** PlayerController не существует в мире

**Решение:**
```blueprint
// Проверьте, что игра запущена
[Is Valid] ? [Get Player Controller 0]
    ? (True)
[Start Dialogue]
```

#### Проблема: Управление не восстанавливается
**Проверьте логи:**
```
LogDialogueInput: Reset ignore input flags - MoveIgnored: 1, LookIgnored: 1
^^^^^^^^^^^^   ^^^^^^^^^^^^
        ДОЛЖНО БЫТЬ 0!
```

**Решение:**
- Убедитесь, что вызывается `UnblockInput()`
- Проверьте, что нет других компонентов блокирующих ввод
- Проверьте, что не вызывается двойная блокировка

#### Проблема: Камера "прыгает" после разблокировки
**Причина:** Накопленный ввод мыши не очищается

**Проверьте логи:**
```
LogDialogueInput: Flushed pressed keys  // Должно быть!
```

**Решение:**
- Обновите код до версии 1.7
- Убедитесь, что `FlushPressedKeys()` вызывается

### Расширенное использование

#### Custom Blocking Settings
```blueprint
[Create DialogueInputBlockingSettings]
    BlockingMode = Custom
    bBlockMovement = True
    bBlockCamera = True
    bBlockJump = False  // Разрешить прыжки во время диалога
    bDisablePawnMovement = True
  bRestorePawnState = True
    ?
[BlockInput] (с этими настройками)
```

#### Динамическое изменение режима
```blueprint
// Во время диалога можно изменить режим
[Is Important Dialogue Choice?]
    ? (True)
    [UpdateBlockingMode] ? BlockAll
    ? (False)
    [UpdateBlockingMode] ? BlockMovementAndCamera
```

### Performance Tips

1. **Не создавайте компонент каждый раз**
   ```blueprint
   // ? ПЛОХО
   [Construct] ? [Add Component: DialogueInputBlocker]
   [Destruct] ? [Destroy Component]
   
   // ? ХОРОШО
   // Добавьте компонент в Components Panel один раз
   ```

2. **Используйте DefaultBlockingSettings**
   ```blueprint
   // ? Настройте один раз в BeginPlay
   [BeginPlay]
       ?
   [SetDefaultBlockingSettings] (Custom Settings)
   
   // Затем везде используйте
   [BlockInputWithDefaults]
   ```

3. **Проверяйте IsInputBlocked перед блокировкой**
   ```blueprint
   [IsInputBlocked?]
       ? (False)
     [BlockInput]
   ```

## Changelog

### v1.7 (Current)
- ? Исправлена проблема с получением PlayerController из NPC
- ? Всегда получаем свежий PlayerController
- ? Добавлено детальное логирование
- ? Улучшена обработка ошибок

### v1.6
- Исправление "прыжка" камеры
- Улучшенное восстановление Legacy Input
- Verbose логирование состояния Pawn

### v1.5
- Базовая блокировка/разблокировка
- Enhanced Input поддержка
- Legacy Input совместимость

## Тестирование

### Тест 1: Базовая блокировка
1. Запустите игру
2. Подойдите к NPC
3. Начните диалог (input должен заблокироваться)
4. Попробуйте двигаться (не должно работать)
5. Попробуйте вращать камеру (не должно работать)
6. Закончите диалог (input должен восстановиться)
7. Попробуйте двигаться и вращать камеру (должно работать)

### Тест 2: Множественные диалоги
1. Начните диалог с NPC1
2. Закончите диалог
3. Начните диалог с NPC2
4. Закончите диалог
5. Проверьте, что управление работает корректно

### Тест 3: Прерванный диалог
1. Начните диалог
2. Уничтожьте NPC (Destroy Actor)
3. Проверьте, что управление восстановилось (EndPlay вызовет UnblockInput)

## Support

При возникновении проблем:
1. Включите Verbose логирование
2. Проверьте Output Log
3. Убедитесь, что используете версию 1.7+
4. Проверьте, что PlayerController существует

## Примеры кода

### C++ (если расширяете компонент)
```cpp
// В вашем NPC классе
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

### Blueprint Function Library (для удобства)
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

Затем в Blueprint:
```blueprint
[NPC Actor] ? [BlockPlayerInput]
[NPC Actor] ? [UnblockPlayerInput]
```

---

**Version:** 1.7  
**Status:** Production Ready  
**Last Updated:** 2024
