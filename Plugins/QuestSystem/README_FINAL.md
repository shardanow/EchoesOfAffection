# ?? Quest System - ��������� ������������

## ?? ������� ��������

**Quest System v2.0** - ������������������� ������� ������� ��� Unreal Engine � ����������� ������� ����� GameEventBus.

## ? ������� �������

### 1?? ������� �������
- ? �������� ������� ����� Data Assets
- ? ������������� objectives
- ? ������� ������� (dependencies)
- ? ����������� ������
- ? ������� ������
- ? ���������� ���������

### 2?? ������� �������
- ? ������� ��������� (ItemAcquired, ItemUsed, ItemCrafted, ItemGifted)
- ? ������� �������� (DialogueStarted, NodeReached, ChoiceMade)
- ? ������� ������� (QuestStarted, QuestCompleted, ObjectiveCompleted)
- ? ������� ������� (HourChanged, DayChanged, SeasonChanged)
- ? ���������� �� EventTag
- ? ���������� �� ItemId / NpcId / QuestId
- ? ���������� �� Quantity / Count

### 3?? UI ����������
- ? Quest Tracker Widget (������������ �������� �������)
- ? Quest Giver Component (������ ��� NPC, ������ �������)
- ? �������������� ���������� UI ��� ��������� ���������

### 4?? ����������
- ? GameEventBus - ���������� ���� �������
- ? Inventory System - ������� ���������
- ? Dialogue System - ������� ��������
- ? Time System - ������� �������

## ?? ������ ������������

### �������� �����

| �������� | ���������� |
|----------|-----------|
| **NPC_ID_COMPLETE_GUIDE.md** | ?? �� ��� NPC ID: ��� ��������, ��� �����������, ��� ������������ |
| **QUEST_GIVER_COMPONENT_EXPLAINED.md** | ?? ������ ���������� QuestGiverComponent |
| **QUEST_GIVER_QUICK_REF.md** | ? ������� ������� �� QuestGiverComponent |
| **QUEST_CONDITIONS_COMPLETE_REFERENCE.md** | ?? ��� ���� ������� ������� � ��������� |
| **QUEST_CONDITIONS_QUICK_REFERENCE.md** | ? ������� ������� �� �������� |
| **EVENT_TYPE_VS_EVENT_TAG_EXPLAINED.md** | ?? ������� ����� EventType � EventTag |
| **EVENT_TAG_FILTER_FIXED.md** | ?? ��� �������� ���������� �� EventTag |

### ������� �����������

| �������� | ��� ���������� |
|----------|---------------|
| **NPCID_EXTRACTION_FIXED.md** | ?? ����������� ���������� NPC ID �� GameplayTags |
| **CRITICAL_TAG_MISMATCH_FIXED.md** | ?? ����������� ������������ EventTag |
| **QUEST_EVENT_BRIDGE_FIXED.md** | ?? ����������� QuestEventBridge |
| **STRICT_EVENT_TAG_CHECK_AND_WIDGET_HIDING.md** | ?? ������� �������� EventTag � ������� ������� |
| **QUEST_TRACKER_SHOWS_ALL_OBJECTIVES.md** | ?? ����������� ���� objectives � ������� |
| **PROGRESS_BAR_FIXED.md** | ?? ����������� ��������-���� |
| **UFUNCTION_FIXED_FINAL.md** | ?? ����������� UFUNCTION �������� |
| **ADDED_BRIDGE_INIT_LOGGING.md** | ?? ���������� ����������� � Bridge |

### ������������

| ���� | ���������� |
|------|-----------|
| **Config/DefaultGameplayTags.ini** | ��� GameplayTags ��� ������� |
| **TAGS_FIXED_FINAL.md** | ������������ �� ����� |

## ?? ������� �����

### 1. �������� ������

```
Content Browser ? Right Click ? Quest System ? Quest Asset
���: DA_Quest_MyFirstQuest
```

```
DA_Quest_MyFirstQuest:
  ?? QuestId: Q_MyFirstQuest
  ?? QuestName: "My First Quest"
  ?? QuestDescription: "Complete your first quest!"
  ?? Objectives:
       ?? [0]:
      ?? ObjectiveId: O_TalkToNPC
    ?? Description: "Talk to the NPC"
 ?? RequiredCount: 1
            ?? Conditions:
    ?? [0]:
      ?? EventTag: GameEvent.Dialogue.Started
  ?? NpcId: Lianne
```

### 2. ��������� NPC

```
BP_Lianne (Actor Blueprint):
  ?? Gameplay Tags: NPC.Lianne  ? �����������!
  ?? QuestGiverComponent:
       ?? bAutoAssignNpcId: true
    ?? AvailableQuests: [DA_Quest_MyFirstQuest]
```

### 3. �������� � ����

```
1. ��������� ���� (PIE)
2. ������� � NPC (������ ��������� ������ !)
3. ������ ������
4. ��������� ����:
   [LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne'
   [LogQuestEventBus] ? CONDITION MATCH! StringParam='Lianne' == NpcId='Lianne'
   [LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

## ?? �������

### �������� ��������� ����

```cpp
// DefaultEngine.ini
[Core.Log]
LogGameEventBus=Verbose
LogQuestEventBus=Verbose
LogQuestBridge=Verbose
LogInventoryEvents=Verbose
LogDialogueSubsystem=Verbose
```

### �������� ��������

| �������� | ������� | ������� |
|----------|---------|---------|
| ����� �� �������� �������� | EventTag �� ��������� | ��������� ������ ���������� ���� |
| NPC ID �� ��������� | ��� GameplayTag | �������� ��� `NPC.Lianne` |
| ������� �� �������� | ������ �� ������� | �������� GameEventBus plugin |
| UI �� ����������� | �������� �� ������� | ��������� OnQuestProgressChanged |

## ?? �����������

```
????????????????????????????????????????????????????
?          QuestSubsystem (Coordinator)          ?
?  ???????????????  ????????????????           ?
?  ?AssetLoader  ?  ?ProgressMgr   ?             ?
?  ?     ?  ?       ?     ?
?  ?LoadQuestAsync????StartQuest   ?      ?
?  ???????????????  ?CompleteQuest?     ?
?       ?GetState  ?             ?
?  ???????????????  ????????????????        ?
?  ? RewardSystem?     ?       ?
?  ?    ?          ?     ?
?  ?GiveReward   ?  ????????????????          ?
?  ???????????????  ?  EventBus  ?             ?
? ?  ?       ?    ?
?  ???????????????  ?OnGameEvent   ?   ?
?  ? SaveManager ?  ?CheckCondition? ?
?  ?  ?  ????????????????    ?
?  ?SaveProgress ?          ?    ?
?  ?LoadProgress ?          ?          ?
?  ???????????????          ?  ?
????????????????????????????????????????????????????
      ?
     ??????????????????????????????????
       ?   ?
    ????????????????????        ??????????????????????
    ? QuestEventBridge ?        ?  GameEventBus      ?
    ?    ?        ?   Subsystem        ?
? Subscribe to:??????????         ?
    ? - Inventory      ? ? EmitEvent()   ?
    ? - Dialogue       ?        ? Subscribe()        ?
    ? - Time       ?        ??????????????????????
    ? - Quest    ?        ?
    ????????????????????      ?
             ?
     ??????????????????????????????????????
              ??
     ???????????????????              ????????????????????
     ?InventoryComponent?      ?DialogueSubsystem?
     ? ?              ?        ?
     ?AddItem() ??????EmitEvent        ?StartDialogue()  ?
     ?UseItem() ??????EmitEvent        ?  ???EmitEvent   ?
     ????????????????????   ???????????????????
```

## ?? ������� �������������

### ������ 1: ����� "Collect Apples"

```
DA_Quest_CollectApples:
  ?? Objective: "Collect 5 apples"
       ?? Condition:
        ?? EventTag: GameEvent.Inventory.ItemAcquired
         ?? ItemId: RedApple
       ?? RequiredCount: 5
```

### ������ 2: ����� "Talk to Lianne"

```
DA_Quest_TalkToLianne:
  ?? Objective: "Talk to Lianne"
     ?? Condition:
 ?? EventTag: GameEvent.Dialogue.Started
            ?? NpcId: Lianne
            ?? RequiredCount: 1
```

### ������ 3: ����� "Complete Another Quest"

```
DA_Quest_ChainedQuest:
  ?? Dependencies: [Q_TalkToLianne]  ? ������� ����� ��������� ����
  ?? Objective: "Complete Lianne's first quest"
       ?? Condition:
            ?? EventTag: GameEvent.Quest.Completed
  ?? QuestId: Q_TalkToLianne
   ?? RequiredCount: 1
```

## ?? ������������

### ���������� �������

```cpp
// ������ �����
QuestSubsystem->StartQuestAsync(FName("Q_MyQuest"), OnStartedDelegate);

// ��������� �����
QuestSubsystem->CompleteQuest(FName("Q_MyQuest"));

// �������� ��������� ������
EQuestState State = QuestSubsystem->GetProgressManager()->GetQuestState(FName("Q_MyQuest"));

// �������� �������� objective
int32 Progress = QuestSubsystem->GetProgressManager()->GetObjectiveProgress(FName("Q_MyQuest"), FName("O_Objective1"));
```

### Blueprint Testing

```cpp
// Event BeginPlay
void ATestActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Get Quest Subsystem
    UQuestSubsystem* QuestSystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    
    // Start test quest
    QuestSystem->StartQuestAsync(
        FName("Q_TestQuest"),
        FOnQuestStartedDelegate::CreateUObject(this, &ATestActor::OnQuestStarted)
    );
}

void ATestActor::OnQuestStarted(bool bSuccess, UQuestAsset* QuestAsset)
{
    if (bSuccess)
    {
UE_LOG(LogTemp, Log, TEXT("Test quest started: %s"), *QuestAsset->QuestName.ToString());
    }
}
```

## ?? ��������� �������

### Inventory System
- `Plugins/AdvancedInventorySystem/ALL_INVENTORY_OPERATIONS_SUPPORTED.md`
- �������: ItemAcquired, ItemUsed, ItemCrafted, ItemGifted, ItemDropped

### Dialogue System
- `Plugins/DialogueSystem/FINAL_SUMMARY_ALL_EVENTS.md`
- `Plugins/DialogueSystem/ALL_DIALOGUE_EVENTS_COMPLETE.md`
- �������: DialogueStarted, DialogueEnded, NodeReached, ChoiceMade

### GameEventBus
- `Plugins/GameEventBus/README.md`
- `Plugins/GameEventBus/CRITICAL_TAG_MISMATCH_FIXED.md`
- ���������� ���� ������� ��� ���� ������

## ?? Changelog

### v2.0 (Current)
- ? ������ ���������� � GameEventBus
- ? ��������� ���� ����� �������
- ? ���������� ���������� NPC ID
- ? ���������� ���������� �� EventTag
- ? Quest Tracker Widget
- ? Quest Giver Component
- ? ��������� �����������
- ? ������ ������������

### v1.0
- ������� ������� �������
- Objectives � Conditions
- ������� ������
- ���������� ���������

## ?? Roadmap

### ����������� � �������
- [ ] ������ �� ������� (time-limited)
- [ ] ��������� objectives
- [ ] ��������� objectives (random selection)
- [ ] ������ �� ���������� NPC
- [ ] ��������� ������
- [ ] PvP ������
- [ ] Achievements integration

## ?? ���������

### ��� ������������� �������:

1. **��������� ����** - �������� Verbose �����������
2. **��������� ����** - EventTag ������ ����� ���������
3. **��������� NPC ID** - �������� GameplayTag `NPC.{Name}`
4. **��������� �������** - GameEventBus ������ ���� �������
5. **��������� ������������** - ��. ������ "������ ������������"

### ����� ���������� �������

**Q: ����� �� ����������**
A: ��������� Dependencies - ��������, ��������� ��������� ������ �����

**Q: ������� �� �����������**
A: ��������� ������ ���������� EventTag � ItemId/NpcId/QuestId

**Q: UI �� �����������**
A: ���������, ��� ������ �������� �� OnQuestProgressChanged

**Q: ������� �� ��������**
A: ���������, ��� GameEventBus plugin ������� � .uproject

---

**Quest System v2.0** - Production Ready! ??

**Created by:** Game Development Team  
**Last Updated:** 2025-01-31  
**Version:** 2.0.0
