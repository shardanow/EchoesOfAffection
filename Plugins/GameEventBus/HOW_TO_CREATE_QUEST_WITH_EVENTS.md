# ?? ��������� ����������: �������� ������ � GameEventBus

## ?? ����: ������� ����� "������� 5 �����"

## ?? �����: � ����� ������ Quest System ��� "Use Event System"!

**������� �������� ������������� ����� QuestEventBridge!**

���� �� ����� �������� ������� ������� - ������� ��� ��������������!

---

## ��� 1: ������� ItemDataAsset ��� ������

### 1.1 � Content Browser:
1. ������� � �����: `Content/Game/Core/Subsystems/Inventory/Items/`
2. ��� ? **Miscellaneous ? Data Asset**
3. � ���� "Pick Data Asset Class" ����� � ������: **`ItemDataAsset`**
4. ������ ����: **`DA_Item_Apple`**

### 1.2 ������� ���� � DA_Item_Apple:
```
=== ITEM | BASIC ===
Item ID: "Apple"  ? �����! ������ ��� ��������
Item Name: "Red Apple"
Item Description: "A juicy red apple"

=== ITEM | STACKING ===
Max Stack Size: 99
? Is Stackable

=== ITEM | PHYSICAL ===
Can Be Picked Up: ?
Can Be Dropped: ?
```

**������� Data Asset** (Ctrl+S)

---

## ��� 2: ������� Quest Data Asset

### 2.1 � Content Browser:
1. ����� � ������: **"Quest Asset"** ��� **"Quest Data Asset"**
2. ������ ����� Data Asset ����� ����
3. ������ ����: **`DA_Quest_CollectApples`**

### 2.2 ������ DA_Quest_CollectApples � �������:

**?? �������� �������: ���� ������ ���������� ObjectiveID ������ Event Tag!**

```
=== QUEST INFO ===
Quest ID: "CollectApples"  ? ���������� ID
Title: "������ ������"
Description: "������ 5 ����� ��� �������"

=== OBJECTIVES ===
[����� + ����� �������� ����]

Objectives[0]:
  Objective ID: "CollectApples_Objective"  ? ���������� ID ����
  
  Description: "Collect apples (0/5)"
  
  ? Is Hidden
  ? Is Optional
  
  === PROGRESS SETTINGS ===
  Target Count: 5  ? ������� ����� �������
  
  === ��� ����� Event Tag / String Filter! ===
  ��� ��������� - ������� �������������� ������������� ����� QuestEventBridge!
```

**������� Quest Data Asset** (Ctrl+S)

---

## ��� 3: ��������� ��������� ������� � ���� (��� �������!)

**? ��� ��� �����������!** QuestEventBridge �������������:

1. ������������� �� `Inventory.Event.ItemAcquired`
2. ����� ������� ����� ����� ��������� �������
3. �������� `UpdateObjectiveProgress(QuestId, ObjectiveId, Amount)`

**��� ��� ��������!** ��. `QuestEventBridge.cpp`

---

## ��� 4: ������� WorldItemActor ��� ������

### 4.1 � Content Browser:
1. ����� Blueprint �����: **`BP_WorldItemActor`** ��� ������ �����
2. ���� ������� �����:
   - ��� ? **Blueprint Class**
   - Parent Class: **`WorldItemActor`** (�� InventorySystem)
   - ������: **`BP_Item_Apple`**

### 4.2 ������� BP_Item_Apple:
```
=== Components ===
StaticMesh (��� SkeletalMesh):
  - ������ ��� ������
  - ������� Transform

=== Details Panel ===
World Item Actor:
  Item Data: DA_Item_Apple  ? ������ ��������� Data Asset
  Stack Size: 1

  ? Can Be Picked Up
  ? Auto Create Interactable
```

**Compile & Save**

---

## ��� 5: ���������� ������ � ����

### 5.1 �������� BP_Item_Apple � Level:
1. ������ ���� �������
2. �� Content Browser �������� **BP_Item_Apple** � ���
3. �������� ��������� ����� (������� 5 ����)

---

## ��� 6: ��������� QuestGiver NPC

### 6.1 ����� ��� ������ BP_QuestGiver:
1. � Content Browser �����: **`BP_QuestGiver`** ��� **`BP_Lianne`**
2. ������ Blueprint

### 6.2 ������� QuestGiverComponent:
```
=== Details Panel ? Quest Giver Component ===

Available Quests:
  [0] = DA_Quest_CollectApples  ? ������ ���� �����!

Quest Giver Settings:
  Greeting Text: "Hello! I need apples!"
  Quest Offer Text: "Can you collect 5 apples for me?"
```

**Compile & Save**

---

## ��� 7: ��������� � ��������������

### 7.1 ��������� ���� (Alt+P ��� PIE)

### 7.2 �������� ���������� �������:
```
~  ? ������� �������

GameEventBus.EnableHistory 1
GameEventBus.SetVerbose 1
```

### 7.3 ���������� � NPC:
1. ������� � QuestGiver NPC
2. ����� E (��� ������ ��������������)
3. ������ ����� "������ ������"
4. ����� ������ ���������� � ������

### 7.4 ������� ������:
1. ������� � ������ �� �����
2. ����� E
3. **������� ��� � �������:**
   ```
   ������ ���� ���������:
   LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | ItemId: Apple | Count: 1
   LogQuest: QuestEventBridge: Forwarding event to Quest System
   LogQuest: Quest objective updated: CollectApples (1/5)
   ```

### 7.5 ���������� ��������:
- ������ ��� 5 �����
- �������� ������: 1/5 ? 2/5 ? 3/5 ? 4/5 ? 5/5
- ����� ������������� ����������! ?

---

## ?? ������� ���� �� ��������

### �������� 1: ������� �� �����������

**�������:**
1. ������ ������� (`~`)
2. �������: `GameEventBus.EnableHistory 1`
3. ������� ������
4. �������: `GameEventBus.PrintHistory`

**������ ���� �����:**
```
LogGameEventBus: Event [Inventory.Event.ItemAcquired] | Apple | Count: 1
```

**���� �����:**
- ������� ��� DA_Item_Apple->ItemID = "Apple"
- ������� ��� BP_Item_Apple->ItemData ��������� �� DA_Item_Apple

---

### �������� 2: QuestEventBridge �� ���������������

**������� ��� ��� ������� ����:**
```
LogQuest: QuestSubsystem v2.0: Initializing...
LogQuest: QuestEventBridge: Initialized successfully
LogQuest: QuestEventBridge: Subscribed to GameEventBus events
```

**���� ���:**
- QuestSystem �� ����������
- ��� GameEventBus plugin ��������

**�������:**
```
Edit ? Plugins ? Search "GameEventBus"
? Enabled (������ ���� �������!)
```

---

### �������� 3: ����� �� �����������

**������� A: ������� ObjectiveID**

���� ������� ����� ��������� **������������ ObjectiveID** � ��������!

**�������� ������� Objective ID ���:**
```
Objective ID: "ItemAcquired_Apple"
```

��� ������� ��� � `QuestEventBridge.cpp` - ��� ����� ���� ������ �������������.

**������� B: ��������� UpdateObjectiveProgress �������**

������ � Blueprint ��� C++:
```cpp
// � InventoryComponent::AddItem() ��� � WorldItemActor::BeginInteract()
if (ItemData && ItemData->ItemID == FName("Apple"))
{
    UQuestSubsystem* QuestSystem = UQuestSubsystem::Get(this);
    if (QuestSystem)
    {
        QuestSystem->UpdateObjectiveProgress(
            FName("CollectApples"),      // Quest ID
            FName("CollectApples_Objective"),  // Objective ID
            1  // Amount
        );
    }
}
```

---

### �������� 4: GameplayTag �� ������

**�������:**
1. ���� `Config/DefaultGameplayTags.ini` ��������:
   ```ini
   +GameplayTagList=(Tag="Inventory.Event.ItemAcquired",DevComment="")
   ```

2. **����������� ��������!** (���� ����������� ������ ��� ������)

3. ��� �������: **Tools ? Refresh Gameplay Tags**

---

## ?? ����������� ������ (���� ������)

```
BP_Item_Apple (� ����)
     ?
     ? [Player �������� E]
     ?
WorldItemActor::BeginInteract()
     ?
     ?
InventoryComponent::AddItem("Apple", 1)
     ?
     ?
UInventoryComponentGameEventHelper::EmitItemAcquiredEvent()
   ?
     ?
GameEventBusSubsystem::EmitEvent()
     ?
     Event Tag: "Inventory.Event.ItemAcquired"
     StringParam: "Apple"
     IntParam: 1
     ?
     ?
QuestEventBridge::OnGameEvent()  ? �����������KI ��������!
     ?
 ���������: Event Tag == "Inventory.Event.ItemAcquired"? ?
     ?
     ?
QuestSubsystem::UpdateObjectiveProgress()
     ?
     Quest ID: "CollectApples"
   Objective ID: "CollectApples_Objective"
     Amount: 1
     ?
     ?
QuestProgressManager::UpdateObjectiveProgress()
     ?
     ����������� �������: 0/5 ? 1/5
     ?
     ?
Quest Tracker Widget �����������
```

---

## ? ������� ����������

- [ ] DA_Item_Apple ������ � ItemID = "Apple"
- [ ] DA_Quest_CollectApples ������
- [ ] Objectives[0] ��������:
  - [ ] Objective ID = "CollectApples_Objective"
  - [ ] Target Count = 5
  - [ ] Description ���������
- [ ] BP_Item_Apple ������ � ��������
- [ ] ������ ��������� � Level (������� 5 ����)
- [ ] QuestGiver �������� � �������
- [ ] �������� ����������� (��� �������� �����)
- [ ] GameEventBus.EnableHistory 1 �������� � �������
- [ ] GameEventBus plugin �������

---

## ?? ������!

������ ������ �������� ������ � ������ ��� ����� ������������� �����������! ???

---

## ?? �������������� �������

### ������ 2: ��������� ����� (������������� ��������!)
```
Quest ID: "MeetAtNoon"
Objective ID: "WaitUntil14"
Target Count: 1

�������: Time.Event.HourChanged (IntParam=14)
QuestEventBridge ������������� ����������!
```

### ������ 3: ���������� ����� (������������� ��������!)
```
Quest ID: "TalkToFarmer"
Objective ID: "CompleteFarmerDialogue"
Target Count: 1

�������: Dialogue.Event.NodeReached (StringParam="FarmerNode_Complete")
QuestEventBridge ������������� ����������!
```

---

## ?? ���� �� ��� �� ��������

**��������� ��������:**

������ `QuestEventBridge.cpp` � ������� ����� `OnGameEvent`:

�� ������ ��������� ������ ����:
```cpp
void UQuestEventBridge::OnGameEvent(const FGameEventPayload& Payload)
{
    if (!bIsActive || !QuestSubsystemRef.IsValid())
    {
        return;
    }
    
 // Extract event data
    FGameplayTag EventTag = Payload.EventTag;
    FName StringParam = Payload.StringParam;
    int32 IntParam = Payload.IntParam;
    
    // Route to appropriate quest handler
    if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired")))
    {
        // Update quest objectives that track this item
   // Find active quests, check objectives, update progress
    }
    
    // ... similar for other events
}
```

**���� ���� ����� ������ ��� �� ������������ �������:**
- ������ issue
- � ������� ������ ����������!

---

**�����! ������� ��� �������� �������������!** ??
