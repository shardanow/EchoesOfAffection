# Quest Integration - NPC & Dialog System

## Архитектура интеграции квестов с другими системами

---

## 1?? Привязка Квеста к NPC

### Способ 1: Quest Giver Component (Уже реализован)

**BP_NPC_Blacksmith:**
```
Components:
  ?? Quest Giver Component
      NPC ID: Blacksmith
      Available Quests:
        [0]: DA_Quest_RepairHammer
        [1]: DA_Quest_ForgeWeapon
```

**В Blueprint:**
```blueprint
Event OnPlayerPressE:
  ? Get Component: Quest Giver Component
  ? Get Available Quests For Player (Player Controller)
  ? For Each Quest:
      ? Show in UI (Quest Title, Description)
      
  ? On Quest Accept Button Clicked:
    ? Quest Giver Component ? Accept Quest
```

---

### Способ 2: Data Table привязка

**DT_NPCQuests** (Row Structure: `FNPCQuestBinding`):

```cpp
USTRUCT(BlueprintType)
struct FNPCQuestBinding : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName NpcId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSoftObjectPtr<UQuestAsset>> AvailableQuests;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSoftObjectPtr<UQuestAsset>> CompletableQuests;
};
```

**Таблица:**
| Row Name | NPC ID | Available Quests | Completable Quests |
|----------|--------|------------------|--------------------|
| Blacksmith | Blacksmith | [DA_Quest_RepairHammer] | [DA_Quest_FindOre] |
| Baker | Baker | [DA_Quest_DailyBread] | [DA_Quest_DailyBread] |

**Использование:**
```cpp
// QuestBindingSystem.h
class UQuestBindingSystem : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    TArray<UQuestAsset*> GetQuestsForNPC(FName NpcId);

protected:
    UPROPERTY(EditDefaultsOnly)
    UDataTable* NPCQuestBindings; // DT_NPCQuests
};
```

---

## 2?? Интеграция с Dialog System

### Архитектура:

```
Dialog System ?? Quest System
     ?      ?
Dialog Node  Quest Events
     ?             ?
Choice Selected ? Start Quest
     ?                ?
Condition Met  ? Quest State
```

---

### Пример 1: Начать квест из реплики диалога

**Dialog Asset: DLG_Blacksmith_Greeting**

```
Node [0]: "Greetings, traveler!"
  Speaker: Blacksmith
  
  Choices:
    [0] "Can you help me with something?"
  ? Next Node: 1
    
    [1] "Tell me about quests"
      ? Next Node: 2
        ? Execute Blueprint: BP_StartQuest

Node [2]: "I need someone to repair my hammer..."
  Execute on Enter:
    ? Start Quest: Q_RepairHammer
  
  Choices:
    [0] "I'll help you!"
     ? Close Dialog
        ? Quest Starts
  
    [1] "Maybe later"
        ? Close Dialog
```

**Blueprint реализация (BP_StartQuest):**
```blueprint
Function: StartQuestFromDialog
  Inputs:
    - Quest ID (FName)
    - Player (APlayerController)
  
  Execution:
    ? Get Quest Subsystem
    ? Start Quest Async (Quest ID)
   On Success:
        ? Show Notification "Quest Started"
   ? Play Sound (QuestAcceptSound)
        ? Add to Journal
```

---

### Пример 2: Condition-based Dialog (проверка состояния квеста)

**Dialog Node с условием:**
```
Node: "Do you have the ore?"
  
  Conditions:
    ? Check Quest State:
  Quest ID: Q_FindOre
        Required State: Active
        Objective: Obj_CollectOre
        Required Progress: >= 10/10
  
  If Conditions Met:
    ? Show Choice: "Yes, I have the ore [Turn In]"
? Execute: CompleteQuest(Q_FindOre)
        ? Grant Rewards
     ? Next Dialog Node
  
  Else:
    ? Show: "Come back when you have 10 ore"
```

**C++ Implementation:**
```cpp
// DialogCondition_QuestState.h
UCLASS()
class UDialogCondition_QuestState : public UDialogCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName QuestId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EQuestState RequiredState;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ObjectiveId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredProgress;

    virtual bool EvaluateCondition(UDialogContext* Context) const override
    {
        UQuestSubsystem* QuestSys = Context->GetQuestSubsystem();
        
        // Check quest state
if (QuestSys->GetQuestState(QuestId) != RequiredState)
         return false;
     
  // Check objective progress
        if (!ObjectiveId.IsNone())
        {
     int32 Current, Target;
   bool bComplete;
       QuestSys->GetObjectiveProgress(QuestId, ObjectiveId, Current, Target, bComplete);
            return Current >= RequiredProgress;
     }
      
        return true;
    }
};
```

---

### Пример 3: Dynamic Dialog based on Quest Progress

```cpp
// DialogNode_QuestBranch.h
UCLASS()
class UDialogNode_QuestBranch : public UDialogNode
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    FName QuestId;

    UPROPERTY(EditAnywhere)
    UDialogNode* NodeIfNotStarted;

    UPROPERTY(EditAnywhere)
    UDialogNode* NodeIfActive;

 UPROPERTY(EditAnywhere)
    UDialogNode* NodeIfCompleted;

    virtual UDialogNode* GetNextNode(UDialogContext* Context) const override
    {
        EQuestState State = Context->GetQuestSubsystem()->GetQuestState(QuestId);
        
     switch (State)
        {
        case EQuestState::NotStarted:
     return NodeIfNotStarted;
    case EQuestState::Active:
     return NodeIfActive;
        case EQuestState::Completed:
            return NodeIfCompleted;
        default:
            return nullptr;
}
    }
};
```

**Usage в Dialog Graph:**
```
Dialog Start
  ?
Quest Branch Node (Q_RepairHammer)
  ?? Not Started ? "I need help fixing my hammer"
  ?            ? Choice: Accept Quest
  ?
  ?? Active ? "How's the repair going?"
  ?     ? Check Objective Progress
  ?   ? If Complete: Turn In Option
  ?
  ?? Completed ? "Thanks for your help!"
      ? Shop unlocked
```

---

## 3?? Event-Based Integration

### Quest Events ? Dialog System

```cpp
// DialogTrigger_OnQuestCompleted.h
UCLASS()
class UDialogTrigger_OnQuestCompleted : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    FName QuestId;

    UPROPERTY(EditAnywhere)
  TSoftObjectPtr<UDialogAsset> DialogToTrigger;

    void Initialize(UQuestSubsystem* QuestSys)
    {
        QuestSys->OnQuestCompleted.AddDynamic(this, &UDialogTrigger_OnQuestCompleted::OnQuestComplete);
  }

    UFUNCTION()
    void OnQuestComplete(FName CompletedQuestId, UQuestAsset* QuestAsset)
    {
        if (CompletedQuestId == QuestId)
        {
  // Trigger dialog
   UDialogSystem* DialogSys = GetDialogSystem();
   DialogSys->StartDialog(DialogToTrigger.LoadSynchronous());
   }
    }
};
```

**Example:**
```
Quest "Find Lost Ring" completed
  ?
Auto-trigger Dialog "Thank You"
  ?
NPC: "You found my ring! Thank you!"
  ?
Give Reward
```

---

## 4?? Bi-directional Communication

### Quest System ?? Dialog System

```cpp
// QuestDialogBridge.h
UCLASS()
class UQuestDialogBridge : public UObject
{
    GENERATED_BODY()

public:
    // Quest ? Dialog
    UFUNCTION(BlueprintCallable)
    void OnQuestObjectiveCompleted(FName QuestId, FName ObjectiveId)
    {
        // Trigger dialog update
    DialogSystem->UpdateAvailableChoices();
    }

    // Dialog ? Quest
    UFUNCTION(BlueprintCallable)
    void OnDialogChoiceSelected(FName DialogId, FName ChoiceId)
    {
        // Update quest progress
        if (ChoiceId == "GaveItem_Apple")
        {
       QuestSystem->UpdateObjectiveProgress("Q_DailyBread", "Obj_GiveApple", 1);
     }
    }

protected:
    UPROPERTY()
    UQuestSubsystem* QuestSystem;

    UPROPERTY()
 UDialogSystem* DialogSystem;
};
```

---

## 5?? Complete Example Workflow

### Scenario: "Repair Hammer" Quest

#### Step 1: NPC Setup
```
BP_NPC_Blacksmith:
  Quest Giver Component:
    Available Quests: [DA_Quest_RepairHammer]
```

#### Step 2: Dialog Setup
```
DLG_Blacksmith_FirstMeeting:
  Node 0: "Hello, traveler!"
    Condition: Quest "Q_RepairHammer" = Not Started
    
    Choice: "Can I help you?"
      ? Node 1: "My hammer is broken..."
      ? Choice: "I'll help!"
  Execute: Start Quest (Q_RepairHammer)
   Close Dialog

DLG_Blacksmith_QuestActive:
  Node 0: "Did you find the ore?"
    Condition: Quest "Q_RepairHammer" = Active
    
    If Objective "Obj_FindOre" Complete:
      Choice: "Yes, here's the ore [Turn In]"
        Execute: Complete Quest (Q_RepairHammer)
        ? Dialog continues: "Thank you!"
    
 Else:
      Text: "Come back with 10 iron ore"

DLG_Blacksmith_QuestComplete:
  Node 0: "Thank you for fixing my hammer!"
    Condition: Quest "Q_RepairHammer" = Completed

    Unlocks: Blacksmith Shop
```

#### Step 3: Quest Asset
```
DA_Quest_RepairHammer:
  Quest ID: Q_RepairHammer
  Quest Giver ID: Blacksmith
  
  Phase 0:
    Objective: Obj_FindOre
      Type: Collect
      Target Count: 10
   Condition: Item ID = IronOre
  
  Rewards:
    - Currency: 100 gold
    - Unlock Dialog Branch: Blacksmith_Shop
```

#### Step 4: Blueprint Glue Code
```blueprint
// BP_DialogManager
Event OnDialogStarted:
  ? Get NPC (Blacksmith)
  ? Get Quest Giver Component
  ? Get Available Quests
  ? Populate Dialog Choices

Event OnDialogChoiceSelected (Choice ID):
  ? If Choice ID == "AcceptQuest":
      ? Quest Giver Component ? Accept Quest
 ? Close Dialog
      ? Show Quest Notification
```

---

## 6?? Best Practices

### ? DO:
1. **Separate Quest Logic from Dialog** - loose coupling через events
2. **Use Quest Events** - для триггера диалогов
3. **Dynamic Dialog Nodes** - проверяют quest state
4. **Bridge Pattern** - QuestDialogBridge для интеграции
5. **Data-Driven** - всё в assets, не хардкод

### ? DON'T:
1. **Hard-code quest IDs** в dialog nodes
2. **Tight coupling** - Dialog не должен знать внутренности Quest
3. **Duplicate state** - одна правда о прогрессе (Quest System)
4. **Sync loading** в dialog callbacks

---

## 7?? Tools for Designers

### Quest-Dialog Linker Tool (Editor Utility Widget)

```cpp
// QuestDialogLinkerTool.h
UCLASS()
class UQuestDialogLinkerTool : public UEditorUtilityWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void LinkQuestToDialog(UQuestAsset* Quest, UDialogAsset* Dialog, FName TriggerEvent);

    UFUNCTION(BlueprintCallable)
    void ValidateQuestDialogLinks();

  UFUNCTION(BlueprintCallable)
    TArray<FString> GetBrokenLinks();
};
```

**UI:**
```
[Quest Dropdown] ? DA_Quest_RepairHammer
[Dialog Dropdown] ? DLG_Blacksmith_FirstMeeting
[Trigger] ? On Quest Start
[Link Button]

Validation Results:
? DA_Quest_RepairHammer ? DLG_Blacksmith_FirstMeeting (OK)
? DA_Quest_FindCat ? DLG_OldLady_Missing (Dialog not found!)
```

---

## 8?? Debug Tools

### Quest-Dialog Inspector

```cpp
// In-game console command
UFUNCTION(Exec)
void ShowQuestDialogBindings(FName QuestId)
{
    UQuestAsset* Quest = GetQuestAsset(QuestId);
    TArray<UDialogAsset*> LinkedDialogs = GetLinkedDialogs(Quest);
    
    for (UDialogAsset* Dialog : LinkedDialogs)
  {
   UE_LOG(LogTemp, Log, TEXT("Quest '%s' linked to Dialog '%s'"), 
  *QuestId.ToString(), *Dialog->GetName());
    }
}
```

---

**Summary**:
- ? Quest Giver Component - для simple cases
- ? Dialog Conditions - для dynamic choices
- ? Event Bridge - для loose coupling
- ? Data Tables - для complex mappings

**Next**: Хотите полный пример интеграции с вашей Dialog System?
