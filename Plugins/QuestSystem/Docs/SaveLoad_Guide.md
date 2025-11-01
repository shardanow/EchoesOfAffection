# Quest System - Save/Load Guide

## ?? ��� ����������� ������?

### ??? �������� �������

**���� � �����������**:
```
Windows: 
C:\Users\<YourUsername>\AppData\Local\<ProjectName>\Saved\SaveGames\

Mac:
~/Library/Application Support/<ProjectName>/Saved/SaveGames/

Linux:
~/.config/Epic/<ProjectName>/Saved/SaveGames/
```

**���� ����������**:
```
QuestSaveSlot.sav  ? �� ���������
```

---

## ?? �������������� ����������

### ����� �����������?

1. **��� ���������� ���� (Deinitialize)**:
   ```cpp
   void UQuestSubsystem::Deinitialize()
   {
     SaveManager->SaveQuestProgress(CurrentSaveGame);  // ����-����������
   }
   ```

2. **��� ��������� Auto-Save**:
   ```cpp
   QuestSubsystem->EnableAutoSave(60.0f);  // ������ 60 ������
   ```

3. **������� ����� Blueprint/C++**:
   ```cpp
   QuestSubsystem->SaveAsync();
   ```

### ������� ������������

**�� ��������� Auto-Save ��������!**

����� **�������** �������� � ����� GameMode/PlayerController:

```blueprint
Event BeginPlay
?? Get Quest Subsystem
   ?? Enable Auto Save (Interval: 60.0)
```

---

## ??? �������� ����������

### ������� 1: ������� (���������)

1. �������� ����
2. ��������� � ����� ����������:
   ```
   C:\Users\<YourUsername>\AppData\Local\EchoesOfAffection\Saved\SaveGames\
   ```
3. ������� ���� `QuestSaveSlot.sav`
4. ��������� ���� - ����� ������� ����� ����������

---

### ������� 2: ����� Blueprint (� ����)

**�������� Debug �������**:

```blueprint
// � ����� Player Controller ��� GameMode

// �������: On Key Pressed (��������, F12)
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

### ������� 3: Console Command (������ ��� ������������!)

**�������� Cheat Manager**:

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

**�������������**:
1. �������� ������� (~ ��� `)
2. �������: `ResetQuests`
3. ������������� �������

---

## ?? Blueprint Utility Widget (��� ������������)

### ��� 1: ������� Debug Widget

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

### ��� 2: �������� �� HUD (������ � Development)

```blueprint
// In Player Controller or HUD

Event BeginPlay
?? Branch (Is Development Build?)
?   ?? TRUE:
?       ?? Create Widget (WBP_QuestDebug)
?  ?? Add to Viewport (Z-Order: 999)
```

---

## ?? ��� �����������?

### FQuestSaveData (���������)

```cpp
struct FQuestSaveData
{
    FName QuestId;          // ID ������
    EQuestState State;      // ������ (Active, Completed, Failed)
    FName CurrentPhaseId;       // ������� Phase
    TArray<FPhaseSaveData> Phases;    // ������ ���� Phases
    FDateTime StartedTime; // ����� �����
    FDateTime CompletedTime;   // ����� ��������
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
    int32 CurrentCount;               // �������� (5/10)
    int32 TargetCount;
    bool bIsCompleted;
};
```

**������ ���������� ������**:
```
Quest: Q_HelpElder
?? State: Active
?? Current Phase: Phase_Main
?? Started: 2024-01-15 14:30
?? Phases:
    ?? Phase_Main:
        ?? Objectives:
            ?? Obj_TalkToElder: 1/2 (�� ��������)
            ?? Obj_GatherInfo: 0/1 (�� �����)
```

---

## ?? Testing Workflow

### ������ ���� ������������

1. **��������� ����**
   ```
   Play in Editor (Alt+P)
   ```

2. **������� �����**
   ```
   ���������� � NPC ? Quest started
   ```

3. **��������� objective**
   ```
   �������� ? Objective progress: 1/2
 ```

4. **��������� �������** (�����������)
   ```
   Console: SaveQuests (���� �������� �������)
   ```

5. **�������� ����**
   ```
   Stop (Shift+F5) ? Auto-save triggered
```

6. **��������� ����������**
   ```
   ��������� ? AppData/Local/.../SaveGames/QuestSaveSlot.sav
   ���� ����������? ?
   ```

7. **��������� �����**
   ```
   Play ? QuestSubsystem ��������� ����������
   Output Log: "Loaded 1 quests from save"
   ```

8. **��������� ��������**
   ```
   Console: ShowQuestSaves
   Output: Active: 1 (Q_HelpElder, Phase_Main)
   ```

9. **������� ����������** (��� ���������� �����)
 ```
   Console: ResetQuests
 Output: "Quest saves deleted!"
   Restart level
   ```

---

## ?? Troubleshooting

### ? ���������� �� �����������

**�������**: SaveGame ���� �������� ��� ������ ����������.

**�������**:
1. ������� ������ `QuestSaveSlot.sav`
2. ������������� ����
3. ��������� Output Log:
   ```
   LogTemp: Created new quest save game
   ```

---

### ? Auto-Save �� ��������

**�������**: Auto-Save �� ������� �� ���������.

**�������**:
```blueprint
Event BeginPlay (GameMode)
?? Get Quest Subsystem
   ?? Enable Auto Save (60.0)
```

��� �������� � `QuestSubsystem::Initialize()`:
```cpp
void UQuestSubsystem::Initialize()
{
    // ...
 EnableAutoSave(60.0f);  // ����-���������� ������ 60 ���
}
```

---

### ? �������� �� ����������� ��� ������

**�������**: ���� �������� ����������� (crash).

**�������**:
1. �������� Auto-Save:
   ```cpp
   QuestSubsystem->EnableAutoSave(30.0f);  // ������ 30 ���
   ```

2. ��� ���������� ������� ��� ������ ��������:
   ```cpp
   // ����� ���������� ������
   OnQuestCompleted.AddLambda([QuestSys]()
   {
       QuestSys->SaveAsync();
   });
   ```

---

## ?? Best Practices ��� ������������

### 1. Debug Console Commands

**�������� � CheatManager**:
```cpp
UFUNCTION(Exec) void StartQuest(FName QuestId);
UFUNCTION(Exec) void CompleteQuest(FName QuestId);
UFUNCTION(Exec) void ResetQuests();
UFUNCTION(Exec) void ShowQuestSaves();
UFUNCTION(Exec) void SaveQuests();
UFUNCTION(Exec) void LoadQuests();
```

**�������������**:
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

**Widget ����������**:
- �������� ������
- �������� objectives
- ������: Save / Load / Reset

---

### 3. Auto-Save ������ � Development

```cpp
void UQuestSubsystem::Initialize()
{
    // ...
    
    #if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
        EnableAutoSave(30.0f);  // ������ � dev builds
        UE_LOG(LogTemp, Log, TEXT("Quest Auto-Save enabled (Development)"));
    #endif
}
```

---

### 4. Log ���������

**�������� ��������� ���**:
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

## ?? ����������

**Quest System Save/Load**:
- ? **��������������** ���������� ��� ������
- ? **Auto-Save** ����� (����� ��������)
- ? **Async** �������� (�� ��������� ����)
- ? **������� �������** ��� ������������
- ? **Console Commands** ��� ����������

**��� ������������**:
1. �������� Debug Widget ��� Console Commands
2. �������� Auto-Save � Development
3. ���������� Output Log
4. ����������� ResetQuests ��� �������

---

**Quest System v2.1 - Save/Load Complete!** ?
