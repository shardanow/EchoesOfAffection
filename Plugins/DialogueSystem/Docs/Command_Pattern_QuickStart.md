# Command Pattern - Quick Start Guide (v1.2)

> **Version:** 1.2.0  
> **Feature:** Command Pattern для действий диалога  
> **Status:** ? Ready

---

## ?? Что это?

**Command Pattern** — это паттерн проектирования, который инкапсулирует действия диалога в объекты-команды.

### Преимущества:
- ? **Undo/Redo** — можно отменять и повторять действия
- ? **History** — полная история всех действий
- ? **Replay** — возможность воспроизвести диалог заново
- ? **Debugging** — легко отслеживать что произошло
- ? **Logging** — автоматическое логирование всех команд

---

## ?? Инициализация

**CommandInvoker использует ленивую инициализацию:**
- CommandInvoker создается автоматически при первом использовании
- Не требуется ручная инициализация
- Безопасно для использования в любой момент

```cpp
// CommandInvoker создается автоматически
Runner->ExecuteCommand(Cmd); // Создается при первом вызове

// Или при старте диалога
Runner->StartDialogue(Asset, Participants); // Создается здесь
```

---

## ?? Quick Start (C++)

### 1. Выполнить команду

```cpp
// Get dialogue runner
UDialogueRunner* Runner = ...; // из Subsystem или Component

// Create command
UDialogueCommand_NavigateToNode* Cmd = NewObject<UDialogueCommand_NavigateToNode>();
Cmd->SetTargetNode(TargetNode);

// Execute
bool bSuccess = Runner->ExecuteCommand(Cmd);
```

### 2. Undo/Redo

```cpp
// Undo last command
if (Runner->CanUndo())
{
    Runner->UndoLastCommand();
}

// Redo
if (Runner->CanRedo())
{
    Runner->RedoCommand();
}
```

### 3. История команд

```cpp
// Get command history
UDialogueCommandInvoker* Invoker = Runner->GetCommandInvoker();
UDialogueCommandHistory* History = Invoker->GetHistory();

// Get stats
int32 Total, Successful, Failed;
float TotalDuration;
History->GetHistoryStats(Total, Successful, Failed, TotalDuration);

UE_LOG(LogTemp, Log, TEXT("Commands: %d total, %d successful, %.3fs"),
    Total, Successful, TotalDuration);
```

---

## ?? Quick Start (Blueprint)

### 1. Выполнить команду

![Execute Command Blueprint](images/command_execute_bp.png)

```
[Dialogue Runner] ? Execute Command
    ?? Command: [Create Navigate Command]
    ?? Record In History: true
? Success
```

### 2. Undo/Redo

```
[Dialogue Runner] ? Can Undo? ? Branch
    ?? True: Undo Last Command
```

### 3. Просмотр истории

```
[Dialogue Runner] ? Get Command History As String
? Print String
```

---

## ?? Встроенные команды

| Команда | Описание | Undo Support |
|---------|----------|--------------|
| `UDialogueCommand_NavigateToNode` | Переход к ноде | ? Yes |
| `UDialogueCommand_ApplyEffects` | Применить эффекты | ? Yes |
| `UDialogueCommand_SelectChoice` | Выбрать choice | ? Yes |
| `UDialogueCommand_SetVariable` | Установить переменную | ? Yes |
| `UDialogueCommand_StartSession` | Начать диалог | ? No |
| `UDialogueCommand_EndSession` | Закончить диалог | ? Yes |
| `UDialogueCommand_Composite` | Несколько команд | ? Yes |

---

## ?? Примеры использования

### Пример 1: Навигация с Undo

```cpp
// Navigate to node
UDialogueCommand_NavigateToNode* NavCmd = NewObject<UDialogueCommand_NavigateToNode>();
NavCmd->SetTargetNode(QuestNode);
Runner->ExecuteCommand(NavCmd);

// Player made mistake, wants to go back
Runner->UndoLastCommand(); // Returns to previous node
```

### Пример 2: Применить эффекты с отменой

```cpp
// Apply effects
TArray<FDialogueEffect> Effects;
// ... fill effects ...

UDialogueCommand_ApplyEffects* EffectCmd = NewObject<UDialogueCommand_ApplyEffects>();
EffectCmd->SetEffects(Effects);
Runner->ExecuteCommand(EffectCmd);

// Changed mind
Runner->UndoLastCommand(); // Reverts all effects
```

### Пример 3: Composite команда

```cpp
// Create composite command
UDialogueCommand_Composite* CompositeCmd = NewObject<UDialogueCommand_Composite>();

// Add sub-commands
CompositeCmd->AddCommand(SetVariableCmd);
CompositeCmd->AddCommand(ApplyEffectsCmd);
CompositeCmd->AddCommand(NavigateCmd);

// Execute all at once
Runner->ExecuteCommand(CompositeCmd);

// Undo all at once
Runner->UndoLastCommand(); // Undoes all 3 commands in reverse order
```

---

## ?? Создание своей команды

### C++ Custom Command

```cpp
UCLASS()
class UMyCustomCommand : public UDialogueCommand
{
    GENERATED_BODY()

public:
    UMyCustomCommand()
    {
        bSupportsUndo = true; // Enable undo
    }

protected:
    virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override
    {
        // Your command logic here
     UE_LOG(LogTemp, Log, TEXT("Executing my custom command!"));
        return true;
    }

    virtual bool UndoImplementation(UDialogueSessionContext* Context) override
    {
        // Undo logic
    UE_LOG(LogTemp, Log, TEXT("Undoing my custom command!"));
   return true;
    }

    virtual FString GetDescriptionImplementation() const override
    {
     return TEXT("My Custom Command");
    }
};
```

### Blueprint Custom Command

1. Создайте новый Blueprint класс на основе `DialogueCommand_Blueprint`
2. Реализуйте события:
   - **Execute** — что делает команда
   - **Undo** — как отменить команду
   - **Can Execute** — когда можно выполнить
   - **Get Description** — описание для логов

---

## ?? Command History

### Просмотр истории

```cpp
UDialogueCommandHistory* History = Runner->GetCommandInvoker()->GetHistory();

// Export to string
FString HistoryStr = History->ExportHistoryToString();
UE_LOG(LogTemp, Log, TEXT("%s"), *HistoryStr);

// Output:
// Command History [5 commands, position: 5]
// =====================================
// ? [0] Navigate to node 'Start' | Success | 0.001s
// ? [1] Apply 3 dialogue effects | Success | 0.002s
// ? [2] Select dialogue choice 0 | Success | 0.001s
// ? [3] Set variable 'QuestState' = 2 | Success | 0.000s
// ? [4] Navigate to node 'End' | Success | 0.001s
```

### Replay диалога

```cpp
// Replay entire history
int32 ReplayedCount = History->ReplayHistory(Context, true);
UE_LOG(LogTemp, Log, TEXT("Replayed %d commands"), ReplayedCount);
```

---

## ?? Настройки

### Включить/выключить запись истории

```cpp
// Disable history recording
Runner->SetCommandRecordingEnabled(false);

// Commands executed now won't be recorded
Runner->ExecuteCommand(TempCommand);

// Re-enable
Runner->SetCommandRecordingEnabled(true);
```

### Ограничить размер истории

```cpp
UDialogueCommandHistory* History = Runner->GetCommandInvoker()->GetHistory();

// Keep only last 50 commands
History->SetMaxHistorySize(50);

// Unlimited history (default: 100)
History->SetMaxHistorySize(0);
```

---

## ?? Debugging

### Print command info

```cpp
UDialogueCommand* Cmd = ...;

// Get metadata
const FDialogueCommandMetadata& Meta = Cmd->GetMetadata();

UE_LOG(LogTemp, Log, TEXT("Command: %s"), *Cmd->GetDescription());
UE_LOG(LogTemp, Log, TEXT("  Result: %s"), *UEnum::GetValueAsString(Meta.Result));
UE_LOG(LogTemp, Log, TEXT("Duration: %.3fs"), Meta.ExecutionDuration);
UE_LOG(LogTemp, Log, TEXT("  Timestamp: %s"), *Meta.Timestamp.ToString());
```

### Console Commands (для добавления)

```cpp
// In your GameMode or similar
FConsoleCommandDelegate Cmd = FConsoleCommandDelegate::CreateLambda([this]()
{
  if (UDialogueRunner* Runner = GetDialogueRunner())
    {
   FString History = Runner->GetCommandHistoryAsString();
        UE_LOG(LogTemp, Log, TEXT("%s"), *History);
    }
});

IConsoleManager::Get().RegisterConsoleCommand(
    TEXT("dialogue.history"),
    TEXT("Show dialogue command history"),
    Cmd,
    ECVF_Cheat
);
```

---

## ?? Связанные файлы

- **Interfaces:**
  - `DialogueCommand.h/cpp` — Базовый интерфейс
  - `DialogueCommands.h/cpp` — Встроенные команды
  - `DialogueCommandHistory.h/cpp` — История и invoker

- **Integration:**
  - `DialogueRunner.h` — Command Pattern методы

- **Documentation:**
  - `Command_Pattern_Architecture.md` — Подробная архитектура
  - `Command_Pattern_Examples.md` — Примеры использования

---

## ?? Best Practices

### 1. Всегда используйте команды для изменения состояния

? **НЕ ТАК:**
```cpp
Context->SetIntegerVariable(TEXT("QuestState"), 2);
Context->SetCurrentNode(NextNode);
```

? **ТАК:**
```cpp
UDialogueCommand_SetVariable* Cmd = NewObject<UDialogueCommand_SetVariable>();
Cmd->SetVariable(TEXT("QuestState"), 2);
Runner->ExecuteCommand(Cmd);
```

### 2. Используйте Composite для атомарных операций

```cpp
// Create atomic operation
UDialogueCommand_Composite* Transaction = NewObject<UDialogueCommand_Composite>();
Transaction->AddCommand(SetVarCmd);
Transaction->AddCommand(ApplyEffectsCmd);
Transaction->AddCommand(NavigateCmd);

// Execute as one - undo reverts all
Runner->ExecuteCommand(Transaction);
```

### 3. Проверяйте CanExecute

```cpp
if (Cmd->CanExecute(Context))
{
    Runner->ExecuteCommand(Cmd);
}
```

### 4. Логируйте важные команды

```cpp
UDialogueCommand* Cmd = ...;
Runner->ExecuteCommand(Cmd);

if (bImportantAction)
{
    UE_LOG(LogDialogue, Log, TEXT("Important: %s"), *Cmd->GetDescription());
}
```

---

## ?? Известные ограничения

1. **Undo не работает для:**
   - `UDialogueCommand_StartSession` (нельзя отменить начало диалога)

2. **Memory:**
   - История команд хранится в памяти
   - По умолчанию лимит: 100 команд
   - Установите `SetMaxHistorySize(0)` для unlimited

3. **Thread Safety:**
   - Команды должны выполняться на Game Thread
   - История не thread-safe

---

## ?? Troubleshooting

### Команда не выполняется

```cpp
if (!Cmd->CanExecute(Context))
{
    UE_LOG(LogTemp, Error, TEXT("Command cannot execute: %s"), 
        *Cmd->GetDescription());
    // Check preconditions
}
```

### Undo не работает

```cpp
if (!Cmd->CanUndo())
{
    UE_LOG(LogTemp, Warning, TEXT("Command doesn't support undo"));
}
```

### История не записывается

```cpp
// Check if recording is enabled
if (!Runner->IsCommandRecordingEnabled())
{
    Runner->SetCommandRecordingEnabled(true);
}
```

---

**Version:** 1.2.0  
**Date:** 22.01.2025  
**Status:** ? Production Ready  
**Documentation:** Complete
