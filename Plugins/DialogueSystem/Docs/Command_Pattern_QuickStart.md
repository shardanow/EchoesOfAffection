# Command Pattern - Quick Start Guide (v1.2)

> **Version:** 1.2.0  
> **Feature:** Command Pattern ��� �������� �������  
> **Status:** ? Ready

---

## ?? ��� ���?

**Command Pattern** � ��� ������� ��������������, ������� ������������� �������� ������� � �������-�������.

### ������������:
- ? **Undo/Redo** � ����� �������� � ��������� ��������
- ? **History** � ������ ������� ���� ��������
- ? **Replay** � ����������� ������������� ������ ������
- ? **Debugging** � ����� ����������� ��� ���������
- ? **Logging** � �������������� ����������� ���� ������

---

## ?? �������������

**CommandInvoker ���������� ������� �������������:**
- CommandInvoker ��������� ������������� ��� ������ �������������
- �� ��������� ������ �������������
- ��������� ��� ������������� � ����� ������

```cpp
// CommandInvoker ��������� �������������
Runner->ExecuteCommand(Cmd); // ��������� ��� ������ ������

// ��� ��� ������ �������
Runner->StartDialogue(Asset, Participants); // ��������� �����
```

---

## ?? Quick Start (C++)

### 1. ��������� �������

```cpp
// Get dialogue runner
UDialogueRunner* Runner = ...; // �� Subsystem ��� Component

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

### 3. ������� ������

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

### 1. ��������� �������

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

### 3. �������� �������

```
[Dialogue Runner] ? Get Command History As String
? Print String
```

---

## ?? ���������� �������

| ������� | �������� | Undo Support |
|---------|----------|--------------|
| `UDialogueCommand_NavigateToNode` | ������� � ���� | ? Yes |
| `UDialogueCommand_ApplyEffects` | ��������� ������� | ? Yes |
| `UDialogueCommand_SelectChoice` | ������� choice | ? Yes |
| `UDialogueCommand_SetVariable` | ���������� ���������� | ? Yes |
| `UDialogueCommand_StartSession` | ������ ������ | ? No |
| `UDialogueCommand_EndSession` | ��������� ������ | ? Yes |
| `UDialogueCommand_Composite` | ��������� ������ | ? Yes |

---

## ?? ������� �������������

### ������ 1: ��������� � Undo

```cpp
// Navigate to node
UDialogueCommand_NavigateToNode* NavCmd = NewObject<UDialogueCommand_NavigateToNode>();
NavCmd->SetTargetNode(QuestNode);
Runner->ExecuteCommand(NavCmd);

// Player made mistake, wants to go back
Runner->UndoLastCommand(); // Returns to previous node
```

### ������ 2: ��������� ������� � �������

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

### ������ 3: Composite �������

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

## ?? �������� ����� �������

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

1. �������� ����� Blueprint ����� �� ������ `DialogueCommand_Blueprint`
2. ���������� �������:
   - **Execute** � ��� ������ �������
   - **Undo** � ��� �������� �������
   - **Can Execute** � ����� ����� ���������
   - **Get Description** � �������� ��� �����

---

## ?? Command History

### �������� �������

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

### Replay �������

```cpp
// Replay entire history
int32 ReplayedCount = History->ReplayHistory(Context, true);
UE_LOG(LogTemp, Log, TEXT("Replayed %d commands"), ReplayedCount);
```

---

## ?? ���������

### ��������/��������� ������ �������

```cpp
// Disable history recording
Runner->SetCommandRecordingEnabled(false);

// Commands executed now won't be recorded
Runner->ExecuteCommand(TempCommand);

// Re-enable
Runner->SetCommandRecordingEnabled(true);
```

### ���������� ������ �������

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

### Console Commands (��� ����������)

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

## ?? ��������� �����

- **Interfaces:**
  - `DialogueCommand.h/cpp` � ������� ���������
  - `DialogueCommands.h/cpp` � ���������� �������
  - `DialogueCommandHistory.h/cpp` � ������� � invoker

- **Integration:**
  - `DialogueRunner.h` � Command Pattern ������

- **Documentation:**
  - `Command_Pattern_Architecture.md` � ��������� �����������
  - `Command_Pattern_Examples.md` � ������� �������������

---

## ?? Best Practices

### 1. ������ ����������� ������� ��� ��������� ���������

? **�� ���:**
```cpp
Context->SetIntegerVariable(TEXT("QuestState"), 2);
Context->SetCurrentNode(NextNode);
```

? **���:**
```cpp
UDialogueCommand_SetVariable* Cmd = NewObject<UDialogueCommand_SetVariable>();
Cmd->SetVariable(TEXT("QuestState"), 2);
Runner->ExecuteCommand(Cmd);
```

### 2. ����������� Composite ��� ��������� ��������

```cpp
// Create atomic operation
UDialogueCommand_Composite* Transaction = NewObject<UDialogueCommand_Composite>();
Transaction->AddCommand(SetVarCmd);
Transaction->AddCommand(ApplyEffectsCmd);
Transaction->AddCommand(NavigateCmd);

// Execute as one - undo reverts all
Runner->ExecuteCommand(Transaction);
```

### 3. ���������� CanExecute

```cpp
if (Cmd->CanExecute(Context))
{
    Runner->ExecuteCommand(Cmd);
}
```

### 4. ��������� ������ �������

```cpp
UDialogueCommand* Cmd = ...;
Runner->ExecuteCommand(Cmd);

if (bImportantAction)
{
    UE_LOG(LogDialogue, Log, TEXT("Important: %s"), *Cmd->GetDescription());
}
```

---

## ?? ��������� �����������

1. **Undo �� �������� ���:**
   - `UDialogueCommand_StartSession` (������ �������� ������ �������)

2. **Memory:**
   - ������� ������ �������� � ������
   - �� ��������� �����: 100 ������
   - ���������� `SetMaxHistorySize(0)` ��� unlimited

3. **Thread Safety:**
   - ������� ������ ����������� �� Game Thread
   - ������� �� thread-safe

---

## ?? Troubleshooting

### ������� �� �����������

```cpp
if (!Cmd->CanExecute(Context))
{
    UE_LOG(LogTemp, Error, TEXT("Command cannot execute: %s"), 
        *Cmd->GetDescription());
    // Check preconditions
}
```

### Undo �� ��������

```cpp
if (!Cmd->CanUndo())
{
    UE_LOG(LogTemp, Warning, TEXT("Command doesn't support undo"));
}
```

### ������� �� ������������

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
