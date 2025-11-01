# ?? �������������� ���������� Quest System � GameEventBus

## ?? �������: �Ѩ �������� �������������!

**���� �� �����:**
- ? �������� ������� "Use Event System"
- ? ��������� Event Tags � Quest Data Asset
- ? ������ ��� ��� ��������� �������
- ? ������� �������� UpdateObjectiveProgress

**�� ������ QuestEventBridge �������������!**

---

## ??? �����������

### ���������� �������:

```
GameEventBusSubsystem (���������� ���� �������)
 ?
    ??> InventorySystem ? ��������� �������
    ??> TimeOfDaySystem ? ��������� �������
    ??> DialogueSystem ? ��������� �������
    ?
    ??> QuestEventBridge ? ������������� �������������!
          ?
  ??> QuestSubsystem ? ��������� ������
```

---

## ?? ��� ��� ��������

### ��� 1: ������������� (��� ������� ����)

```cpp
QuestSubsystem::Initialize()
    ?
    ??> InitializeGameEventBridge()
?
          ??> GameEventBridge = NewObject<UQuestEventBridge>()
          ??> GameEventBridge->Initialize(this)
      ?
         ??> SubscribeToEventBus()
          ?
??> Subscribe("Inventory.Event.ItemAcquired")
            ??> Subscribe("Time.Event.HourChanged")
        ??> Subscribe("Dialogue.Event.NodeReached")
     ??> ... (��� ������ �������)
```

**���������:** QuestEventBridge ������ ������� ��� �������!

---

### ��� 2: ����� ��������� ������

```cpp
Player �������� E �� ������
    ?
    ?
WorldItemActor::BeginInteract()
    ?
  ?
InventoryComponent::AddItem("Apple", 1)
    ?
    ?
UInventoryComponentGameEventHelper::EmitItemAcquiredEvent()
    ?
    ??> ������ FGameEventPayload:
      - EventTag = "Inventory.Event.ItemAcquired"
        - StringParam = "Apple"
        - IntParam = 1
    ?
    ?
GameEventBusSubsystem::EmitEvent(Payload)
    ?
    ??> ��������� ���� �����������
          ?
     ??> QuestEventBridge::OnGameEvent(Payload)  ? �������������!
```

---

### ��� 3: QuestEventBridge ������������ �������

```cpp
QuestEventBridge::OnGameEvent(Payload)
    ?
    ??> ���������: EventTag == "Inventory.Event.ItemAcquired"? ?
?
    ??> ��������� ������:
    ?   ItemID = Payload.StringParam  // "Apple"
    ?     Count = Payload.IntParam      // 1
    ?
    ??> ������� �������� ������:
    ?     ForEach ActiveQuest:
    ?       ForEach Objective in Quest:
    ?       If Objective.TracksItem(ItemID):
    ?           ??> UpdateObjectiveProgress(QuestID, ObjectiveID, Count)
    ?
    ??> QuestProgressManager::UpdateObjectiveProgress()
          ?
  ??> ����������� �������: 0/5 ? 1/5
          ??> ��������� ����������: 1/5 ? �� ��������
  ??> Emit OnObjectiveUpdated
                ?
          ??> Quest Tracker Widget �����������!
```

**���������:** ����� ��������� ��� ������ ����!

---

## ?? ������: Quest Data Asset

### ������� 1: ������� ����� (�������������)

```
Quest ID: "CollectApples"
Title: "������ ������"

Objectives[0]:
  Objective ID: "CollectApples_Objective"
  Description: "Collect apples (0/5)"
  Target Count: 5
```

**��� ��������:**
1. QuestEventBridge ����� ������� `Inventory.Event.ItemAcquired` � `StringParam="Apple"`
2. ��������� �������� ������
3. ������� "CollectApples"
4. ��������� �������� +1

**�����:** ������� ������������� ���������� ��� ���� objective ������ � ItemAcquired!

---

### ������� 2: ����� �������� ����� (���� �����)

���� �������������� ����������� �� ��������, ����� �������� ���� � Quest Data Asset:

```
Objectives[0]:
  Objective ID: "ItemAcquired_Apple"  ? �������: {EventType}_{Filter}
  Description: "Collect apples (0/5)"
  Target Count: 5
```

**�������� ObjectiveID:**
- `ItemAcquired_{ItemID}` ? ��� ����� ���������
- `TimeReached_{Hour}` ? ��� ��������� �������
- `DialogueReached_{NodeID}` ? ��� ���������� �������

QuestEventBridge ����� ������� ObjectiveID � ������������ � ���������!

---

## ?? ��� ��������� ��� ������� ��������

### 1. ������� ���� ��� ������� ����

```
LogQuest: QuestSubsystem v2.0: Initializing...
LogQuest: QuestEventBridge: Initializing...
LogQuest: QuestEventBridge: Subscribed to GameEventBus events
LogQuest: QuestEventBridge: Listening for:
  - Inventory.Event.ItemAcquired
  - Time.Event.HourChanged
  - Dialogue.Event.NodeReached
  - Quest.Event.Started
LogQuest: QuestEventBridge: Initialized successfully
LogQuest: QuestSubsystem v2.0: Initialized successfully
```

**���� ����� ���:**
- QuestEventBridge �� ���������������
- ������� `QuestSubsystem::InitializeGameEventBridge()`

---

### 2. ������ verbose �����������

```
~  ? �������

GameEventBus.SetVerbose 1
```

**������ ��� ������� ������ �������:**
```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired
StringParam: Apple
  IntParam: 1
  Instigator: PlayerCharacter

LogQuest: QuestEventBridge: Received event: Inventory.Event.ItemAcquired
LogQuest: QuestEventBridge: ItemID = Apple, Count = 1
LogQuest: QuestEventBridge: Found matching quest: CollectApples
LogQuest: QuestEventBridge: Updating objective: CollectApples_Objective
LogQuest: QuestProgressManager: Objective updated: CollectApples (1/5)
LogQuest: OnObjectiveUpdated broadcasted
```

---

### 3. ��������� Quest.Debug

```
Quest.Debug CollectApples
```

**�����:**
```
Quest: CollectApples
  State: Active
  Phase: Main
  
  Objectives:
    [0] CollectApples_Objective
        Description: "Collect apples (0/5)"
        Progress: 2/5 (40%)
  Status: In Progress
      
    Event Tracking:
     Type: ItemAcquired
     Filter: Apple
  Last Update: 10.5s ago
```

---

## ??? ���� ���������� �� ��������

### ������� A: �������� ����� ����� � QuestEventBridge

������ `QuestEventBridge.cpp` � ����� ����� `OnGameEvent`:

```cpp
void UQuestEventBridge::OnGameEvent(const FGameEventPayload& Payload)
{
    if (!bIsActive || !QuestSubsystemRef.IsValid())
    {
        return;
  }
    
  UQuestSubsystem* QuestSystem = Cast<UQuestSubsystem>(QuestSubsystemRef.Get());
    if (!QuestSystem)
    {
        return;
    }
    
    // === INVENTORY EVENTS ===
    if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired")))
    {
        HandleItemAcquired(Payload, QuestSystem);
    }
    else if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemUsed")))
    {
 HandleItemUsed(Payload, QuestSystem);
    }
    
    // === TIME EVENTS ===
    else if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Time.Event.HourChanged")))
    {
        HandleTimeChanged(Payload, QuestSystem);
    }
    
    // === DIALOGUE EVENTS ===
  else if (Payload.EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Dialogue.Event.NodeReached")))
    {
        HandleDialogueNode(Payload, QuestSystem);
 }
}

void UQuestEventBridge::HandleItemAcquired(const FGameEventPayload& Payload, UQuestSubsystem* QuestSystem)
{
    FName ItemID = Payload.StringParam;  // "Apple"
    int32 Count = Payload.IntParam;      // 1
    
  // Find active quests
    TArray<FQuestSaveData> ActiveQuests = QuestSystem->GetActiveQuests();
    
    for (const FQuestSaveData& QuestData : ActiveQuests)
    {
        // Check if quest has objectives tracking this item
        // Pattern: ObjectiveID = "ItemAcquired_Apple"
        FName ObjectiveID = FName(*FString::Printf(TEXT("ItemAcquired_%s"), *ItemID.ToString()));
        
     // Try to update
        if (QuestSystem->UpdateObjectiveProgress(QuestData.QuestId, ObjectiveID, Count))
     {
       UE_LOG(LogQuest, Log, TEXT("QuestEventBridge: Updated quest %s for item %s (+%d)"), 
      *QuestData.QuestId.ToString(), *ItemID.ToString(), Count);
 }
    }
}
```

**������ ������� ����� ������ objectives � ID ���� `ItemAcquired_Apple`!**

---

### ������� B: ������������ ConsoleCommands ��� ������������

���� ���������� �� ��������, ������� �������:

```
Quest.Start CollectApples
Quest.UpdateObjective CollectApples CollectApples_Objective 1
Quest.Debug CollectApples
```

���� ��� �������� ? �������� � QuestEventBridge
���� �� �������� ? �������� � QuestProgressManager

---

## ? ������

### ��� ������ QuestEventBridge �������������:

1. **�������������** �� ��� ������� GameEventBus ��� �������������
2. **�������** ������� Inventory, Time, Dialogue, etc.
3. **�������** �������� ������ ��� ��������� �������
4. **������������** ������� � objectives �������
5. **���������** �������� ������������� ����� UpdateObjectiveProgress()
6. **��������** ��� �������� ��� �������

### ��� ����� ������ ����:

1. **������� ItemDataAsset** � ���������� ItemID
2. **������� Quest Data Asset** � Objectives
3. **������� Target Count** ��� objective
4. **��������� �����** ����� QuestGiver ��� �������
5. **������� ��������** ? ����� ��������� �������������! ?

---

## ?? �� �������� �� �������!

QuestEventBridge - ��� **�������������� ����** ����� GameEventBus � Quest System.

**���� �� ����� ������ ����������� �������!**

**������ �������� Data Assets � �����!** ??
