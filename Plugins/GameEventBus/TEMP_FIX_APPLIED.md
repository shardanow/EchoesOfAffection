# ? ��������� ���� �����ͨ�!

## ?? ��������� ��������

**QuestEventBus** �� ���� ������ ��� ��������������� ���������� ������� ��� ��������� �������.

### ��� ���� �������:

```
GameEventBus ��������� ������� ?
   ?
QuestEventBridge �������� ������� ?
   ?
QuestEventBridge ���������� � QuestEventBus ?
   ?
QuestEventBus::EmitEvent() ���������� ����������� ?
   ?
? ��! ����� �� �������� UpdateObjectiveProgress()!
   ?
? ������ �� �����������!
```

---

## ?? ���������� �������

�������� **��������� ��� in S** � `QuestEventBridge.cpp`:

### ��� ������ ����������:

```cpp
void UQuestEventBridge::ForwardToQuestSystem(const FGameEventPayload& GamePayload)
{
    // ���������: ��� ������� ItemAcquired?
    if (GamePayload.EventTag.MatchesTag("Inventory.Event.ItemAcquired"))
    {
  FName ItemID = GamePayload.StringParam;  // "Apple"
 int32 Count = GamePayload.IntParam;  // 1
        
 // �������� ��� �������� ������
   TArray<FQuestSaveData> ActiveQuests = QuestSubsystem->GetActiveQuests();
        
        // ��� ������� ������
        for (const FQuestSaveData& QuestData : ActiveQuests)
        {
       // ����������� ������ �������� ObjectiveID:
      TArray<FName> PossibleIDs = {
         "Collect_Apple",           // ����������� ���������
        "ItemAcquired_Apple",      // Event-based naming
 "CollectApples_Objective", // Quest-based naming
            "Apple",     // ������ ID ��������
                "CollectionObjective"      // ������������� ����
     };
            
      // ������� �������� � ������ ID
      for (FName ObjectiveID : PossibleIDs)
            {
       if (QuestSubsystem->UpdateObjectiveProgress(QuestData.QuestId, ObjectiveID, Count))
              {
 // ������� ��������!
     break;
  }
            }
  }
    }
}
```

---

## ?? ��� ��������������

### 1. ����������� ������

```bash
# Rebuild Solution � Visual Studio
```

### 2. ������� ����

### 3. ������ ����:

```
~  ? ������� �������
GameEventBus.SetVerbose 1
GameEventBus.EnableHistory 1
```

### 4. ����� ����� "Collect Apples"

```
Quest.Start CollectApples
```

### 5. ������� ������

����� `E` �� ������ � ����.

### 6. ������� ��� - ������ ����:

```
LogGameEventBus: [EMIT] Event: Inventory.Event.ItemAcquired | StringParam=Apple | IntParam=1
LogQuestBridge: TEMP FIX: ItemAcquired event - Item='Apple' Count=1
LogQuestBridge: TEMP FIX: Checking 1 active quests
LogQuestBridge: TEMP FIX: Successfully updated quest 'CollectApples' objective 'Collect_Apple' (+1)
LogQuest: Objective updated: CollectApples (1/5)
```

### 7. ������� Quest Tracker Widget

������ ������ ��������: **"Collect apples (1/5)"**

### 8. ������ ��� 5 �����

�������� ������ �����������: `1/5 ? 2/5 ? 3/5 ? 4/5 ? 5/5`

---

## ?? ����� ObjectiveID ��������������

���� ������� **5 ���������** ObjectiveID:

| ������� | ������ | ����� ������������ |
|---------|--------|--------------------|
| `Collect_{ItemID}` | `Collect_Apple` | **�������������!** ����������� ��������� |
| `ItemAcquired_{ItemID}` | `ItemAcquired_Apple` | ���� ������ ���� ������� ������� |
| `{QuestID}_Objective` | `CollectApples_Objective` | ���� objective �� ���� ����� |
| `{ItemID}` | `Apple` | ������ ID �������� |
| `CollectionObjective` | `CollectionObjective` | ������������� ���� ����� |

### ������� �������������:

#### ������� 1: Collect_{ItemID} (������!)

**Quest Data Asset:**
```
Quest ID: "CollectApples"
Objective ID: "Collect_Apple"  ? ������������� ��������!
Target Count: 5
```

#### ������� 2: ItemAcquired_{ItemID}

**Quest Data Asset:**
```
Quest ID: "GatherItems"
Objective ID: "ItemAcquired_Apple"
Target Count: 10
```

#### ������� 3: {QuestID}_Objective

**Quest Data Asset:**
```
Quest ID: "CollectApples"
Objective ID: "CollectApples_Objective"  ? �������������
Target Count: 5
```

**�����:** ItemID � ItemDataAsset ������ ��������� � ������������ � ObjectiveID!

---

##  ��������� ����������� ���������� �����

### ? �������� ������ ��� ItemAcquired

������ ������� (Dialogue, Time, Location) **�� ��������������**!

����� ����� �������� ����������� ������ ��� ������� ���� �������.

### ? ������� ������� ObjectiveID

���� ������� ��� �������� ������ - �� ���������� ��� �������� ���������� �������.

### ? �� ������������ data-driven ������

� ������� ����� ������� ������������ Event ? Objective � QuestAsset.

---

## ?? ��� ������ ������

### ������������ (��� �����):

? ��������� ��������� `Collect_{ItemID}` ��� objectives  
? ������� ��� ItemDataAsset->ItemID ������������� ������������� � ObjectiveID

### ����������� (TODO):

1. **����������� ������ � QuestEventBus**
   - ������� `QuestEventBus::BroadcastEvent()` �����
   - �������� ������� Event Tag ? Quest Objectives

2. **�������� ��������� ������ �������**
   - Dialogue.Event.NodeReached
 - Time.Event.HourChanged
   - Location.Event.Entered

3. **�������� data-driven routing**
   - � QuestObjective �������� ���� `EventTag` � `EventFilter`
   - ������������� �������� ���������� objectives

---

## ?? ������� ���������� � �����

- [ ] ��� �������������
- [ ] DA_Item_Apple ������ � ItemID = "Apple"
- [ ] DA_Quest_CollectApples ������
- [ ] Objective ID ���������� ��� ���� �� �������������� ���������
- [ ] BP_Item_Apple �������� � �������� � ���� (5 ����)
- [ ] Quest Tracker Widget �������� �� �����
- [ ] ���� ��������: `GameEventBus.SetVerbose 1`

---

## ? ������!

������ ������� ������ ��������! ������� ������ � �������� ��� ������ �����������!

���� �������� �������� - ������� ���� � ������� ���:
1. ������� �����������
2. QuestEventBridge �� ��������
3. ObjectiveID ��������� � ����� �� �������������� ���������

**�����!** ???
