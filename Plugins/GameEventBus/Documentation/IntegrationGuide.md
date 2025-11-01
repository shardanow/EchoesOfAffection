# Game Event Bus - Integration Guide

## �����

**Game Event Bus** � ��� ����������� ������� �������, ������� ��������� ��������� ������� �������� ����������������� ���� � ������ **��� ������ ������������**.

### ������������

? **��� ����-����** � ������� �� ����� ���� � �����  
? **��������** � ����� ��������� ����� �������  
? **��������� ����� Data Assets** � �� ����� ������ ���  
? **Blueprint-friendly** � �� �������� �� Blueprint  
? **����������������** � �������� � ����� ����������� ������  

---

## �����������

```
???????????????????????????????????????????????????????????????
?        GameEventBusSubsystem        ?
?  � ����������� ��������� �������       ?
?  � �������� �� ������� ����� GameplayTags           ?
?  � ������� ������� ����� ���������       ?
???????????????????????????????????????????????????????????????
      ?
 ?
      ???????????????????????????????????????????
         ?          ?     ?
    ????????????      ???????????????      ????????????
    ?  Quest   ?      ?  Dialogue   ?      ?  Item    ?
    ?  System  ?      ?   System    ?  ? System   ?
    ????????????      ???????????????      ????????????
```

### ��� ��� ��������

1. **������� A** ��������� ������� (��������, "Inventory.Event.ItemAcquired")
2. **GameEventBus** �������� ������� � ������� ���� �����������
3. **������� B** (��������, Quest System) �������� ������� � ���������

**�����:** ������� A �� ����� � ������������� ������� B!

---

## ������� �������������

### 1. �����: "������� 5 �����"

**Data Asset: DA_Quest_CollectApples**

```
Quest Objective:
  - EventType: OnItemAcquired
  - EventTag: "Inventory.Event.ItemAcquired"
  - ItemId: "Apple"
  - Count: 5
```

**Inventory System ��������� �������:**

```cpp
// � C++
UInventoryGameEventBridge::EmitItemAcquiredEvent(this, FName("Apple"), 1, PlayerActor);

// ��� � Blueprint
Emit Item Event
  EventTag: Inventory.Event.ItemAcquired
  ItemId: Apple
  Count: 1
```

**Quest System ������������� �������� �������** ����� QuestEventBridge � ��������� ��������!

---

### 2. ������ -> �����

**Data Asset: DA_Dialogue_TalkToBlacksmith**

```
Dialogue Node:
  Text: "������ ��� ������� ������!"
  
  Effects:
    - EmitGameEvent:
        EventTag: "Quest.Event.Start"
        StringParam: "Quest_GatherIron"
```

**Quest System** ������������ ��� ������� � ��������� �����!

---

### 3. ����� -> �����

**Data Asset: DA_Quest_NightPatrol**

```
Quest Objective:
  - EventType: OnTimeReached
  - EventTag: "Time.Event.HourChanged"
  - TimeRangeStart: 20
  - TimeRangeEnd: 6
  - RequiredArea: "City"
```

**Time System ��������� �������:**

```cpp
// ������ ���
UTimeSystemGameEventEmitter::OnHourChanged(CurrentHour);
```

**Quest System** ��������� ������� � ����������� ��������!

---

## ��� ��������� ��� ������ �������

### ��� 1: �������� ������ GameEventBus

� `.uproject` ��� ����� Editor:

```json
{
  "Plugins": [
    {
      "Name": "GameEventBus",
      "Enabled": true
    }
  ]
}
```

### ��� 2: ��������� ����������

� ����� GameMode ��� GameInstance:

```cpp
// C++
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // �������������� ��������� ���� ����������
    UGameEventIntegrationHelpers::SetupAllIntegrations(this);
}
```

��� � Blueprint:

```
BeginPlay -> Setup All Integrations
```

### ��� 3: �������� ������� ������� � ���� �������

**������: Inventory System**

```cpp
// ����� ����� �������� �������
void UInventoryComponent::AddItem(FName ItemId, int32 Count)
{
    // ... ���� ������ ���������� �������� ...
    
    // ��������� �������
    UInventoryGameEventBridge::EmitItemAcquiredEvent(this, ItemId, Count, GetOwner());
}
```

**������: Dialogue System**

�������� ������ `EmitGameEvent` � ���� �������:

```
Dialogue Node -> Add Effect -> Emit Game Event
  EventTag: Dialogue.Event.NodeReached
  StringParam: CurrentNodeId
```

### ��� 4: ��������� ������ ����� Data Assets

�������� Quest Data Asset:

```
Objective:
  Condition:
    EventType: OnItemAcquired
    EventTag: Inventory.Event.ItemAcquired
  ItemId: GoldCoin
    Count: 100
```

**������!** ����� ����� ������������� ����������� ��������� �����.

---

## ������ ������� (GameplayTags)

### Inventory Events
- `Inventory.Event.ItemAcquired` � ������� �������
- `Inventory.Event.ItemUsed` � ����������� �������  
- `Inventory.Event.ItemCrafted` � ������ �������
- `Inventory.Event.ItemGifted` � ������� ������� NPC
- `Inventory.Event.ItemDropped` � �������� �������

### Dialogue Events
- `Dialogue.Event.NodeReached` � ���������� ���� �������
- `Dialogue.Event.ChoiceMade` � ������ �����
- `Dialogue.Event.Started` � ������ �����
- `Dialogue.Event.Ended` � ������ ��������

### Quest Events
- `Quest.Event.Started` � ����� �����
- `Quest.Event.Completed` � ����� ��������
- `Quest.Event.Failed` � ����� ��������
- `Quest.Event.ObjectiveCompleted` � ���� ������ ���������

### Time Events
- `Time.Event.HourChanged` � ��������� ���
- `Time.Event.DayChanged` � ��������� ����
- `Time.Event.TimeOfDay` � ���������� ����������� �����

### Weather Events
- `Weather.Event.Changed` � ���������� ������

### NPC Events
- `NPC.Event.AffinityChanged` � ���������� �������� � NPC
- `NPC.Event.TalkStarted` � ������� �������� � NPC

### Location Events
- `Location.Event.Entered` � ����� � �������
- `Location.Event.Exited` � ����� �� �������

### Combat Events
- `Combat.Event.EnemyKilled` � ���� ����
- `Combat.Event.DamageTaken` � ������� ����

---

## Blueprint �������

### ������� �������

```
Emit Simple Event (EventTag)
Emit Event With String (EventTag, StringParam)
Emit Event With Int (EventTag, IntParam)
Emit Item Event (EventTag, ItemId, Count)
Emit Dialogue Event (EventTag, DialogueId, NodeId)
Emit Quest Event (EventTag, QuestId, ObjectiveId)
Emit Time Event (EventTag, Hour, Day)
```

### �������� �� �������

```
Subscribe To Event (EventTag, Delegate)
Subscribe To Event Hierarchy (ParentTag, Delegate)
Unsubscribe From Event (EventTag, Delegate)
```

### ����������� ��������

```
Register Quest Start Effect (EffectTag, QuestId)
Register Dialogue Start Effect (EffectTag, DialogueId)
Register Item Give Effect (EffectTag, ItemId, Count)
```

### ����������� �������

```
Register Quest Active Condition (ConditionTag, QuestId)
Register Item Count Condition (ConditionTag, ItemId, Count)
Register Time Range Condition (ConditionTag, StartHour, EndHour)
```

---

## ������ � Best Practices

### ? DO

- ����������� �������� ����� (Inventory.Event.ItemAcquired)
- ���������� ������� ����� ���������� ��������
- ����������� `SetupAllIntegrations()` ��� ������ ����
- ��������� ������� ��� ������� (`bLogEvent = true`)

### ? DON'T

- �� ���������� ������ ����������� ����� ���������
- �� ���������� ������� ������� ����� (����������� �������)
- �� ��������� ������������ �� ������� ��� Destroy

---

## ���������� �������

### �������� ������ �������

1. �������� GameplayTag � ������:
   ```
MySystem.Event.CustomAction
   ```

2. ���������� �������:
   ```cpp
   FGameplayTag MyTag = FGameplayTag::RequestGameplayTag(FName("MySystem.Event.CustomAction"));
   UGameEventBusLibrary::EmitSimpleEvent(this, MyTag);
   ```

3. ����������� �� �������:
   ```cpp
   UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
   EventBus->SubscribeToEvent(MyTag, MyDelegate);
   ```

### �������� ������ �������

1. �������� �����, ����������� `IGenericGameEffect`
2. ��������������� ��� � Registry:
   ```cpp
   UGameEventEffectRegistry* Registry = EventBus->GetEffectRegistry();
   Registry->RegisterEffect(EffectTag, UMyCustomEffect::StaticClass(), "MySystem");
   ```

3. ����������� � �������/�������� ����� EffectTag!

---

## FAQ

**Q: ����� �� ��������� ����������� �� GameEventBus � Build.cs?**  
A: ���! ����������� ������������. ������� �������� � ��� GameEventBus.

**Q: ��� �������� ����� �������?**  
A: ������ �������� ����� GameplayTag � ���������� ���. ��� ���������� ������� �������.

**Q: ����� �� ������������ � multiplayer?**  
A: ��, �� ����� ������ ����������. EventBus �������� �������� �� ������ �������.

**Q: ��� �������� �������?**  
A: ����������� `SetEventHistoryEnabled(true)` � `GetRecentEvents()` ��� �������� `bLogEvent = true`.

---

## ������: ������ ���������� ������

```
Quest: "������� ������� 10 ������ � ������ �����"

Objective 1: Collect Iron
  EventType: OnItemAcquired
  EventTag: Inventory.Event.ItemAcquired
  ItemId: Iron
  Count: 10

Objective 2: Talk to Blacksmith at Night
  EventType: OnDialogNodeReached
  EventTag: Dialogue.Event.NodeReached
  NpcId: Blacksmith
  DialogNodeId: GiveIron
  ProgressGate:
    RequireTimeOfDay: true
    RequiredTimeStart: 20
    RequiredTimeEnd: 6

Rewards:
  - GiveItem: GoldCoin x100
  - ModifyAffection: Blacksmith +20
  - UnlockDialogBranch: BlacksmithAdvanced
```

**���������:**

1. ��������� ��������� `Inventory.Event.ItemAcquired` ����� ����� �������� ������
2. ������ ��������� `Dialogue.Event.NodeReached` ����� ����� ������� � ��������
3. Time System ��������� `Time.Event.HourChanged` ������ ���
4. Quest System ��������� ��� ������� � ����� �������

**���������:** �� �������� ������������� ��� ������ ������� ����!

---

## Support

���� �������� ������� ��� ��������, ���������:

1. ������� �� ������ GameEventBus
2. ������� �� �� `SetupAllIntegrations()`
3. ��������� �� ��������� GameplayTags
4. ����������� �� ������� (��������� ����� `bLogEvent = true`)

Happy game developing! ??
