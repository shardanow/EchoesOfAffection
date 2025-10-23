# Диагностика проблемы с UnblockInput

## ?? Шаги для отладки

### 1. Включите Verbose логирование
В консоли Unreal:
```
Log LogDialogueInput Verbose
```

### 2. Запустите игру и проверьте логи

#### ? При БЛОКИРОВКЕ должны быть логи:
```
LogDialogueInput: === BlockEnhancedInput START ===
LogDialogueInput: Adding dialogue context: IMC_Dialogue with priority 1000
LogDialogueInput: ? Dialogue input mapping context added successfully
LogDialogueInput: === BlockEnhancedInput END ===
LogDialogueInput: Input blocked with mode: X for PC: PlayerController_0
```

#### ? При РАЗБЛОКИРОВКЕ должны быть логи:
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

### 3. Проверьте значения в логах

#### ?? Проблема 1: MoveIgnored или LookIgnored = 1 после разблокировки
**Логи:**
```
LogDialogueInput: AFTER Reset - MoveIgnored: 1, LookIgnored: 1
      ^^^       ^^^
           ДОЛЖНО БЫТЬ 0!
```

**Причина:** Кто-то еще блокирует ввод (другой компонент, Blueprint, и т.д.)

**Решение:**
- Найдите все места где вызывается `SetIgnoreMoveInput(true)` или `SetIgnoreLookInput(true)`
- Убедитесь что нет других компонентов блокирующих ввод
- Проверьте DialoguePresentationComponent - он тоже может блокировать

#### ?? Проблема 2: "No dialogue input mapping context"
**Логи:**
```
LogDialogueInput: No dialogue input mapping context - using legacy blocking
```

**Причина:** `DialogueInputMappingContext` не установлен в настройках компонента

**Решение:**
1. Откройте Blueprint с DialogueInputBlockerComponent
2. Найдите настройку `Dialogue Input Mapping Context`
3. Установите ваш Input Mapping Context для диалогов
4. ИЛИ создайте новый IMC с пустыми маппингами (блокирует ВСЕ)

#### ?? Проблема 3: "Enhanced Input Subsystem is NULL"
**Логи:**
```
LogDialogueInput: Enhanced Input Subsystem is NULL!
```

**Причина:** Enhanced Input не настроен для игрока

**Решение:**
1. Убедитесь что в Project Settings ? Input включен Enhanced Input
2. Проверьте что PlayerController имеет LocalPlayer
3. Если используете Legacy Input - компонент переключится автоматически

#### ?? Проблема 4: PlayerController не найден
**Логи:**
```
LogDialogueInput: UnblockInput: No valid PlayerController found
```

**Причина:** PlayerController не существует или был уничтожен

**Решение:**
- Убедитесь что UnblockInput вызывается ПОКА игрок существует
- Проверьте что не вызываете UnblockInput после Destroy Actor

### 4. Проверьте Blueprint настройки

#### На Player Character/Controller:
1. **Есть ли DialogueInputBlockerComponent?**
   - Если НЕТ - добавьте его
   - Если ЕСТЬ - проверьте настройки

2. **Настройки DialogueInputBlockerComponent:**
   ```
   Enable Input Blocking = TRUE
   Dialogue Input Mapping Context = [Выберите IMC]
   Default Blocking Mode = Block Movement (или Custom)
   ```

3. **Вызов из Blueprint:**
   ```
   Event: On Dialogue Ended
       ?
   [Get Component: DialogueInputBlocker]
       ?
   [UnblockInput]
   ```

### 5. Альтернативное решение - DialoguePresentationComponent

Если ничего не помогает, используйте **DialoguePresentationComponent**:

```
// В Player Character BeginPlay

[Add Component: DialoguePresentationComponent]
    ?
[Set Auto Activate On Dialogue Start = True]
    ?
[Set Auto Deactivate On Dialogue End = True]
```

Он автоматически управляет вводом и камерой.

### 6. Проверка через Blueprint Debugger

1. Откройте Blueprint игрока
2. Установите Breakpoint на узле `UnblockInput`
3. Запустите игру в режиме Play In Editor (PIE)
4. Когда диалог заканчивается - проверьте:
   - Вызывается ли UnblockInput?
   - Какой компонент используется?
   - Есть ли ошибки?

### 7. Проверка Input Mapping Contexts в Runtime

Добавьте эту консольную команду в игре:
```
ShowDebug EnhancedInput
```

Она покажет:
- Все активные Input Mapping Contexts
- Их приоритеты
- Какие действия заблокированы

### 8. Ядерный вариант - полный сброс

Если ничего не работает, добавьте в UnblockInput:

```cpp
// В вашем Player Character C++

void AMyPlayerCharacter::ForceUnblockInput()
{
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        // Сброс ВСЕХ флагов
        PC->ResetIgnoreInputFlags();
        PC->EnableInput(PC);
        
        // Восстановление Enhanced Input
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
       ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
   {
        // Удалить ВСЕ контексты
  Subsystem->ClearAllMappings();
       
          // Добавить дефолтные обратно
            if (DefaultInputMappingContext)
            {
           Subsystem->AddMappingContext(DefaultInputMappingContext, 0);
  }
        }
 }
}
```

## ?? Ожидаемые результаты

### ? ПРАВИЛЬНО (движение работает):
```
LogDialogueInput: === UnblockInput START ===
LogDialogueInput: BEFORE Restore - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: AFTER Restore - MoveIgnored: 0, LookIgnored: 0
LogDialogueInput: Enhanced Input Subsystem exists
LogDialogueInput: === UnblockInput END - Success ===

// В игре:
- WASD двигает персонажа ?
- Мышь вращает камеру ?
```

### ? НЕПРАВИЛЬНО (движение не работает):
```
LogDialogueInput: === UnblockInput START ===
LogDialogueInput: AFTER Reset - MoveIgnored: 1, LookIgnored: 1
    ^^^
             ПРОБЛЕМА!

// В игре:
- WASD НЕ двигает персонажа ?
- Мышь НЕ вращает камеру ?
```

## ?? Если ничего не помогает

1. Убедитесь что у вас версия v1.8+ кода
2. Очистите Intermediate и Binaries папки
3. Пересоберите проект (Rebuild)
4. Проверьте что не вызываете BlockInput после UnblockInput
5. Используйте DialoguePresentationComponent вместо прямого вызова

---

**Следующий шаг:** Запустите игру, завершите диалог, и пришлите логи из Output Log!
