# Quest System - Save/Load Guide

## ?? Где сохраняются квесты?

### ??? Файловая система

**Путь к сохранениям**:
```
Windows: 
C:\Users\<YourUsername>\AppData\Local\<ProjectName>\Saved\SaveGames\

Mac:
~/Library/Application Support/<ProjectName>/Saved/SaveGames/

Linux:
~/.config/Epic/<ProjectName>/Saved/SaveGames/
```

**Файл сохранения**:
```
QuestSaveSlot.sav  ? По умолчанию
```

---

## ?? Автоматическое сохранение

### Когда сохраняется?

1. **При завершении игры (Deinitialize)**:
   ```cpp
   void UQuestSubsystem::Deinitialize()
   {
     SaveManager->SaveQuestProgress(CurrentSaveGame);  // Авто-сохранение
   }
   ```

2. **При включении Auto-Save**:
   ```cpp
   QuestSubsystem->EnableAutoSave(60.0f);  // Каждые 60 секунд
   ```

3. **Вручную через Blueprint/C++**:
   ```cpp
   QuestSubsystem->SaveAsync();
   ```

### Текущая конфигурация

**По умолчанию Auto-Save ВЫКЛЮЧЕН!**

Нужно **вручную** включить в вашем GameMode/PlayerController:

```blueprint
Event BeginPlay
?? Get Quest Subsystem
   ?? Enable Auto Save (Interval: 60.0)
```

---

## ??? Удаление сохранений

### Вариант 1: Вручную (проводник)

1. Закройте игру
2. Перейдите в папку сохранений:
   ```
   C:\Users\<YourUsername>\AppData\Local\EchoesOfAffection\Saved\SaveGames\
   ```
3. Удалите файл `QuestSaveSlot.sav`
4. Запустите игру - будет создано новое сохранение

---

### Вариант 2: Через Blueprint (в игре)

**Добавьте Debug команду**:

```blueprint
// В вашем Player Controller или GameMode

// Событие: On Key Pressed (например, F12)
Event OnDeleteSavePressed
?? Get Quest Subsystem
?   ?? Get Save Manager
?   ?? Delete Save Slot
?           ?? Slot Name: "QuestSaveSlot"
?           ?? User Index: 0
?? Print String: "Quest save deleted! Restart game."
?? (Optional) Open Level (Restart level)
```

---

### Вариант 3: Console Command (удобно для тестирования!)

**Создайте Cheat Manager**:

```cpp
// MyCheatManager.h
UCLASS()
class UMyCheatManager : public UCheatManager
{
    GENERATED_BODY()

public:
    UFUNCTION(Exec, Category = "Quest")
    void ResetQuests();

    UFUNCTION(Exec, Category = "Quest")
    void ShowQuestSaves();
};

// MyCheatManager.cpp
void UMyCheatManager::ResetQuests()
{
    UQuestSubsystem* QuestSys = UQuestSubsystem::Get(GetWorld());
    if (QuestSys && QuestSys->GetSaveManager())
    {
        bool bSuccess = QuestSys->GetSaveManager()->DeleteSaveSlot("QuestSaveSlot", 0);
     
  if (bSuccess)
        {
            UE_LOG(LogTemp, Warning, TEXT("Quest saves deleted! Restart level."));
        }
    }
}

void UMyCheatManager::ShowQuestSaves()
{
    UQuestSubsystem* QuestSys = UQuestSubsystem::Get(GetWorld());
    if (QuestSys)
    {
     TArray<FQuestSaveData> ActiveQuests = QuestSys->GetActiveQuests();
        TArray<FQuestSaveData> CompletedQuests = QuestSys->GetCompletedQuests();

        UE_LOG(LogTemp, Log, TEXT("=== QUEST SAVES ==="));
        UE_LOG(LogTemp, Log, TEXT("Active: %d"), ActiveQuests.Num());
        UE_LOG(LogTemp, Log, TEXT("Completed: %d"), CompletedQuests.Num());
      
   for (const FQuestSaveData& Quest : ActiveQuests)
        {
         UE_LOG(LogTemp, Log, TEXT("  - %s (Phase: %s)"), 
   *Quest.QuestId.ToString(), 
       *Quest.CurrentPhaseId.ToString());
        }
    }
}
```

**Использование**:
1. Откройте консоль (~ или `)
2. Введите: `ResetQuests`
3. Перезапустите уровень

---

## ?? Blueprint Utility Widget (для тестирования)

### Шаг 1: Создать Debug Widget

```
Content/Game/UI/Debug/WBP_QuestDebug
```

**Designer**:
```
Canvas Panel
?? Vertical Box
? ?? Text Block: "Quest Debug Panel"
?   ?? Button: "Delete All Saves"
?   ?? Button: "Show Active Quests"
?   ?? Button: "Enable Auto-Save (60s)"
?   ?? Button: "Disable Auto-Save"
?   ?? Text Block: "SaveStatus" (variable)
```

**Event Graph**:

```blueprint
// Button: Delete All Saves
Event OnDeleteClicked
?? Get Quest Subsystem
?   ?? Get Save Manager
?  ?? Delete Save Slot
?           ?? Slot Name: "QuestSaveSlot"
?           ?? User Index: 0
?? Set Text (SaveStatus): "Saves deleted! Restart game."
?? Play Sound (optional)

// Button: Show Active Quests
Event OnShowQuestsClicked
?? Get Quest Subsystem
?   ?? Get Active Quests ? Array
?   ?? For Each Loop
?       ?? Print String: Quest ID + State

// Button: Enable Auto-Save
Event OnEnableAutoSaveClicked
?? Get Quest Subsystem
?   ?? Enable Auto Save (60.0)
?? Set Text (SaveStatus): "Auto-save enabled (60s)"

// Button: Disable Auto-Save
Event OnDisableAutoSaveClicked
?? Get Quest Subsystem
?   ?? Get Save Manager
? ?? Disable Auto Save
?? Set Text (SaveStatus): "Auto-save disabled"
```

---

### Шаг 2: Добавить на HUD (только в Development)

```blueprint
// In Player Controller or HUD

Event BeginPlay
?? Branch (Is Development Build?)
?   ?? TRUE:
?       ?? Create Widget (WBP_QuestDebug)
?  ?? Add to Viewport (Z-Order: 999)
```

---

## ?? Что сохраняется?

### FQuestSaveData (структура)

```cpp
struct FQuestSaveData
{
    FName QuestId;          // ID квеста
    EQuestState State;      // Статус (Active, Completed, Failed)
    FName CurrentPhaseId;       // Текущий Phase
    TArray<FPhaseSaveData> Phases;    // Данные всех Phases
    FDateTime StartedTime; // Когда начат
    FDateTime CompletedTime;   // Когда завершён
};

struct FPhaseSaveData
{
    FName PhaseId;
    bool bIsActive;
    TArray<FObjectiveSaveData> Objectives;
};

struct FObjectiveSaveData
{
  FName ObjectiveId;
    int32 CurrentCount;               // Прогресс (5/10)
    int32 TargetCount;
    bool bIsCompleted;
};
```

**Пример сохранённых данных**:
```
Quest: Q_HelpElder
?? State: Active
?? Current Phase: Phase_Main
?? Started: 2024-01-15 14:30
?? Phases:
    ?? Phase_Main:
        ?? Objectives:
            ?? Obj_TalkToElder: 1/2 (не завершён)
            ?? Obj_GatherInfo: 0/1 (не начат)
```

---

## ?? Testing Workflow

### Полный цикл тестирования

1. **Запустите игру**
   ```
   Play in Editor (Alt+P)
   ```

2. **Начните квест**
   ```
   Поговорите с NPC ? Quest started
   ```

3. **Выполните objective**
   ```
   Действие ? Objective progress: 1/2
 ```

4. **Сохраните вручную** (опционально)
   ```
   Console: SaveQuests (если добавили команду)
   ```

5. **Закройте игру**
   ```
   Stop (Shift+F5) ? Auto-save triggered
```

6. **Проверьте сохранение**
   ```
   Проводник ? AppData/Local/.../SaveGames/QuestSaveSlot.sav
   Файл существует? ?
   ```

7. **Запустите снова**
   ```
   Play ? QuestSubsystem загружает сохранение
   Output Log: "Loaded 1 quests from save"
   ```

8. **Проверьте прогресс**
   ```
   Console: ShowQuestSaves
   Output: Active: 1 (Q_HelpElder, Phase_Main)
   ```

9. **Удалите сохранение** (для повторного теста)
 ```
   Console: ResetQuests
 Output: "Quest saves deleted!"
   Restart level
   ```

---

## ?? Troubleshooting

### ? Сохранения не загружаются

**Причина**: SaveGame файл повреждён или версия изменилась.

**Решение**:
1. Удалите старый `QuestSaveSlot.sav`
2. Перезапустите игру
3. Проверьте Output Log:
   ```
   LogTemp: Created new quest save game
   ```

---

### ? Auto-Save не работает

**Причина**: Auto-Save не включён по умолчанию.

**Решение**:
```blueprint
Event BeginPlay (GameMode)
?? Get Quest Subsystem
   ?? Enable Auto Save (60.0)
```

Или добавьте в `QuestSubsystem::Initialize()`:
```cpp
void UQuestSubsystem::Initialize()
{
    // ...
 EnableAutoSave(60.0f);  // Авто-сохранение каждые 60 сек
}
```

---

### ? Прогресс не сохраняется при выходе

**Причина**: Игра аварийно завершается (crash).

**Решение**:
1. Включите Auto-Save:
   ```cpp
   QuestSubsystem->EnableAutoSave(30.0f);  // Каждые 30 сек
   ```

2. Или сохраняйте вручную при важных событиях:
   ```cpp
   // После завершения квеста
   OnQuestCompleted.AddLambda([QuestSys]()
   {
       QuestSys->SaveAsync();
   });
   ```

---

## ?? Best Practices для тестирования

### 1. Debug Console Commands

**Добавьте в CheatManager**:
```cpp
UFUNCTION(Exec) void StartQuest(FName QuestId);
UFUNCTION(Exec) void CompleteQuest(FName QuestId);
UFUNCTION(Exec) void ResetQuests();
UFUNCTION(Exec) void ShowQuestSaves();
UFUNCTION(Exec) void SaveQuests();
UFUNCTION(Exec) void LoadQuests();
```

**Использование**:
```
Console:
  StartQuest Q_HelpElder
  CompleteQuest Q_HelpElder
  ShowQuestSaves
  ResetQuests
```

---

### 2. Debug Widget (F1)

**Bind F1 key**:
```blueprint
Event F1 Pressed
?? Branch (Is Debug Widget Visible?)
?   ?? TRUE: Remove from Parent
?   ?? FALSE: Create and Add to Viewport
```

**Widget показывает**:
- Активные квесты
- Прогресс objectives
- Кнопки: Save / Load / Reset

---

### 3. Auto-Save только в Development

```cpp
void UQuestSubsystem::Initialize()
{
    // ...
    
    #if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
        EnableAutoSave(30.0f);  // Только в dev builds
        UE_LOG(LogTemp, Log, TEXT("Quest Auto-Save enabled (Development)"));
    #endif
}
```

---

### 4. Log прогресса

**Включите подробный лог**:
```cpp
void UQuestProgressManager::UpdateObjectiveProgress(...)
{
    // ...
    
    #if !UE_BUILD_SHIPPING
        UE_LOG(LogTemp, Log, TEXT("Objective '%s': %d/%d"), 
         *ObjectiveId.ToString(), CurrentCount, TargetCount);
    #endif
}
```

---

## ?? API Reference

### C++ API

```cpp
// Get subsystem
UQuestSubsystem* QuestSys = UQuestSubsystem::Get(WorldContext);

// Save/Load
QuestSys->SaveAsync();
QuestSys->LoadAsync();

// Auto-save
QuestSys->EnableAutoSave(60.0f);

// Save Manager direct access
UQuestSaveManager* SaveMgr = QuestSys->GetSaveManager();
SaveMgr->DeleteSaveSlot("QuestSaveSlot", 0);
SaveMgr->DoesSaveSlotExist("QuestSaveSlot", 0);

// Get save data
TArray<FQuestSaveData> ActiveQuests = QuestSys->GetActiveQuests();
FQuestSaveData SaveData = QuestSys->GetQuestSaveData(QuestId);
```

### Blueprint API

```blueprint
Get Quest Subsystem
?? Save Async
?? Load Async
?? Enable Auto Save (Interval)
?? Get Save Manager
?   ?? Delete Save Slot (Slot Name, User Index)
?   ?? Does Save Slot Exist
?   ?? Disable Auto Save
?? Get Active Quests ? Array
```

---

## ?? Заключение

**Quest System Save/Load**:
- ? **Автоматическое** сохранение при выходе
- ? **Auto-Save** опция (нужно включить)
- ? **Async** операции (не блокирует игру)
- ? **Удобные утилиты** для тестирования
- ? **Console Commands** для разработки

**Для тестирования**:
1. Добавьте Debug Widget или Console Commands
2. Включите Auto-Save в Development
3. Проверяйте Output Log
4. Используйте ResetQuests для очистки

---

**Quest System v2.1 - Save/Load Complete!** ?
