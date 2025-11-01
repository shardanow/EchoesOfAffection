# Quest System - NPC Integration Guide

������ ����������� �� �������� ������� � NPC � ������� ��������.

---

## ����������� ����������

### �������� �������:

1. **Component-Based** (? �������������) - ��������� �� NPC
2. **Data Table** - ������� �������� NPC ? Quests
3. **Actor-Based** - ������ ������ � Blueprint NPC

---

## 1. Component-Based ������ (�������������)

### ��� 1: �������� ��������� � NPC

**� Blueprint NPC:**

1. ������� Blueprint NPC (��������, `BP_QuestGiver_Blacksmith`)
2. **Components Panel ? Add Component ? Quest Giver Component**
3. ��������� � Details:

```
Quest Giver Component:
  NPC ID: Blacksmith (��� ������������� �� ����� ������)
  Auto Assign NPC ID: true
  
  Available Quests:
    [0]: DA_Quest_RepairTools
    [1]: DA_Quest_FindOre
    [2]: DA_Quest_CraftSword
    
  Quest Available Icon: T_QuestAvailable
  Quest In Progress Icon: T_QuestInProgress  
  Quest Complete Icon: T_QuestComplete
```

### ��� 2: ��������� Quest Asset

� quest asset ������� quest giver:

```
Quest Asset (DA_Quest_RepairTools):
  Quest ID: Q_RepairTools
  Quest Giver ID: Blacksmith
  Quest Location: Village_Smithy
```

### ��� 3: ������� ������� ��������������

**� BP_NPC_Base (������������ ����� ��� ���� NPC):**

```blueprint
// Event: On Component Begin Overlap (Interaction Trigger)
Event OnBeginOverlap
  ? Get Component Quest Giver Component
  ? Is Valid?
    True:
      ? Has Quest Interaction (Player Controller)
      ? Show Quest Indicator Icon
    False:
    ? Hide Quest Indicator

// Event: Player presses E (Interact)
Event OnInteract
  ? Get Component Quest Giver Component
  ? On Player Interact (Player Controller)
  ? Open Quest Dialog UI
```

### ��� 4: ������� UI ������� � ��������

**WBP_QuestDialog:**

```blueprint
Event Construct
  ? Get Quest Giver Component (from NPC)
? Get Available Quests For Player
  ? Populate Quest List

Event OnQuestSelected
  ? Accept Quest (Player Controller, Selected Quest Asset)
  ? Close Dialog

Event OnTurnInQuest
  ? Turn In Quest (Player Controller, Quest Asset)
  ? Show Reward UI
  ? Close Dialog
```

---

## 2. ���������� ���������� �������

### Widget Component ��� ������ ��� NPC

**BP_NPC � Widget Component:**

```blueprint
Components:
  ?? Mesh
  ?? Quest Giver Component
  ?? Widget Component (WBP_QuestIndicator)
      Position: (0, 0, 200) // ��� �������
      Draw Size: (64, 64)

WBP_QuestIndicator:
  Event Tick:
    ? Get Quest Giver Component
    ? Get Quest Indicator Icon (Player)
    ? Set Icon Image
    ? Set Visibility (Hidden if no icon)
```

### ���� ������:

| ������ | �������� | ����� ���������� |
|--------|----------|------------------|
| ? ������ ��������������� ���� | �������� ����� ����� | `GetAvailableQuests().Num() > 0` |
| ? ����� �������������� ���� | ����� � �������� | `GetActiveQuests().Num() > 0` |
| ? ������ �������������� ���� | ����� ����� ����� | `GetCompletableQuests().Num() > 0` |

---

## 3. ������� ����������

### ������ 1: ������� Quest Giver (������)

**BP_NPC_Blacksmith:**

```
Components:
  - Quest Giver Component
    Available Quests:
      - DA_Quest_RepairHammer
      - DA_Quest_ForgeWeapon

Event Graph:
  Event BeginPlay:
    ? Get Quest Giver Component
    ? On Available Quests Changed
      ? Update Quest Indicator

  Event OnPlayerInteract:
    ? Get Quest Giver Component
    ? Get Available Quests For Player
    ? Branch (Has Quests?)
      True:
        ? Open Dialog Widget
        ? Show Quest List
      False:
        ? Play "No quests" Dialog
```

### ������ 2: NPC � ����������� ������

**BP_NPC_Elder (� Quest Giver, � ���������� �������):**

```
Components:
  - Quest Giver Component (��� ����� �������)
  - Quest Receiver Component (��� ����������)

��� ���� Quest Giver Component:
  Available Quests:
    - DA_Quest_MeetElders
    - DA_Quest_VillageHistory
  
  Quest Giver ID: Elder
  (��������� � Available, � Completable quests)
```

### ������ 3: ������ ��� Quest Giver (������, �����)

**BP_MysteriousChest:**

```
Components:
  - Static Mesh (Chest)
  - Quest Giver Component
    Available Quests:
   - DA_Quest_AncientTreasure
    Auto Assign NPC ID: true (ID ����� "BP_MysteriousChest_1")

Event OnPlayerInteract:
  ? Get Quest Giver Component
  ? Accept Quest (Auto-start ��� ��������������)
  ? Play Open Animation
```

---

## 4. Advanced: Data Table ������

��� ������� �������� � ������� NPC.

### ������� Data Table:

**DT_NPCQuests** (���������: `FNPCQuestData`):

```cpp
USTRUCT(BlueprintType)
struct FNPCQuestData : public FTableRowBase
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

**� �������:**

| Row Name | NPC ID | Available Quests | Completable Quests |
|----------|--------|------------------|--------------------|
| Blacksmith | Blacksmith | [DA_Quest_RepairTools, DA_Quest_ForgeWeapon] | [DA_Quest_FindOre] |
| Baker | Baker | [DA_Quest_DailyBread] | [DA_Quest_DailyBread] |
| Elder | Elder | [DA_Quest_VillageHistory] | [DA_Quest_MeetElders, DA_Quest_SaveVillage] |

### �������������:

```blueprint
Function: GetNPCQuests (NPC ID) ? Quest List
  ? Get Data Table Row (DT_NPCQuests, NPC ID)
  ? Return Available Quests
```

---

## 5. ���������� � Dialog System

���� � ��� ���� ������� ��������:

### � Dialog Node:

```
Dialog Node: "Greetings"
  Text: "Hello, traveler!"
  
  Choices:
    [0] "Tell me about quests"
   ? Branch:
   Has Available Quests?
       True: Show Quest List Dialog
        False: "No quests right now"
    
  [1] "I'm here to turn in a quest"
        ? Branch:
 Has Completable Quests?
 True: Show Turn-in Dialog
    False: "You haven't completed any quests"
```

### Auto-Quest �� Dialog Choice:

```
Dialog Choice: "Help me!"
  On Selected:
    ? Emit Dialog Choice Event
    ? Start Quest (Q_HelpVillager)
  ? Close Dialog
```

---

## 6. ������������ ����������/�������� �������

### Runtime ���������� ������ � NPC:

```blueprint
Function: AddQuestToNPC (NPC Actor, Quest Asset)
  ? Get Quest Giver Component (from NPC Actor)
  ? Available Quests.Add(Quest Asset)
  ? Load Quest Assets
  ? On Available Quests Changed (Broadcast)
```

### �������� ����� ����������:

```blueprint
Event OnQuestTurnedIn (Quest Asset, Player)
  ? Available Quests.Remove(Quest Asset)
  ? Save to persistent data
```

---

## 7. ���������� � World State

### Quest availability �� ������� �����:

```blueprint
Event OnTimeChanged (Hour)
  ? For Each NPC with Quest Giver Component:
    ? Get Available Quests
    ? Filter by Time Requirements
    ? Update Quest Indicator
```

### Quest availability �� ������:

```blueprint
Event OnWeatherChanged (Weather State)
  ? For Each Quest Giver:
    ? Check Quest Dependencies (Weather)
    ? Show/Hide Quest Icon
```

---

## 8. Best Practices

### ? DO:

1. **����������� ����������** - ����� ��������� � ����� �������
2. **��������� quest assets** - �� ���������� ������ ��� ������
3. **����������� ���������� ����������** - ������ ��� NPC
4. **����������� ������** - ���� NPC = ���� ��� ������� (������, ��������, � �.�.)
5. **����������� ��������** - ����������� Streamable Manager
6. **��������� � ���������** - ���������� QuestGiverId ��������� � NPC ID

### ? DON'T:

1. **�� ����������** - �� ���������� quest ID � ��� NPC
2. **�� ����������** - ����������� ���������� ������ ���������
3. **�� ���������� �����** - �� ���������� ������ ���������
4. **�� ��������� cleanup** - �������� completed non-repeatable quests

---

## 9. Debugging

### Console Commands:

```
// �������� ��� Quest Giver Components � ������
ListAllQuestGivers

// �������� ������ ����������� NPC
ShowNPCQuests Blacksmith

// �������� ����� � NPC runtime
AddQuestToNPC Blacksmith Q_NewQuest
```

### Debug Draw:

```blueprint
Event Tick (if Debug Mode):
  ? For Each Quest Giver Component in World:
    ? Draw Debug String (NPC ID, Available Quests Count)
    ? Draw Debug Sphere (Quest Available = Green, Complete = Yellow)
```

---

## 10. ������ ������� workflow

### �������� NPC � ��������:

1. **������� Quest Asset:**
```
   DA_Quest_FindLostHammer
     Quest ID: Q_FindLostHammer
     Quest Giver ID: Blacksmith
```

2. **������� ��� ������� NPC Blueprint:**
   ```
 BP_NPC_Blacksmith
   ```

3. **�������� Quest Giver Component:**
   ```
   Components ? Add ? Quest Giver Component
   ```

4. **��������� ���������:**
   ```
 NPC ID: Blacksmith
   Available Quests: [DA_Quest_FindLostHammer]
   Icons: [T_QuestAvailable, T_QuestInProgress, T_QuestComplete]
   ```

5. **�������� Widget ���������:**
   ```
   Components ? Add ? Widget Component
   Widget Class: WBP_QuestIndicator
 Location: (0, 0, 200)
   ```

6. **��������� ��������������:**
   ```blueprint
   Event OnPlayerPressedE:
     ? Quest Giver Component ? On Player Interact
? Open Quest Dialog UI
   ```

7. **��������������:**
   - ������� � NPC
- ������� ������ "!"
   - ������ E
   - ������� �����
   - ������� ������ "?"

---

## Conclusion

Component-based ������ ���:
- ? ������������������
- ? �������� ��������� � ���������
- ? �������� (����� ����� ����� ������ ������)
- ? ������ ��� ��� ������������
- ? ������ �������

**������������**: ����������� `UQuestGiverComponent` ��� ���� NPC � ��������, ������ ������!

---

**Next Steps:**
- [Dialog Integration](DialogIntegration.md) - ���������� � �������� ��������
- [UI Examples](UIExamples.md) - ������� quest UI
- [World Events](WorldEvents.md) - ������ �� ������� �������
