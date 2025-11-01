# ������: Event-Driven ����������� ��� UE5

## ��� ���� �������

����������� **Event-Driven �����������** ��� ���������� ������� ������ ��� ������ ������������.

## ?? �������� ��������

**����:**
- Quest System ����� ������� �� Dialogue System
- Dialogue System ������� �� Inventory System
- Inventory ������� �� Quest System
- ����������� �����������
- ���������� �������� ����� ������� ��� ��������� ����
- �������� �� ����� ��������� �������������� ��� ������������

**�����:**
- ��� ������� ����������
- ����� ����� ������� (GameplayTags)
- ������� ����������� ����� ���������
- �������� ����������� �� ����� Data Assets
- ����� ��������� ����� �������

## ?? ��������� ����������

### 1. GameEventBus Plugin (�����!)
**��������������:** `Plugins/GameEventBus/`

**�����:**
- `GameEventBusSubsystem.h/.cpp` � ����������� event bus
- `GameEventEffectRegistry.h/.cpp` � ����������� ��������/�������
- `GameEventBusLibrary.h/.cpp` � Blueprint helpers
- `GameEventIntegrationHelpers.h/.cpp` � ���������� ���������
- `TimeSystemGameEventEmitter.h/.cpp` � ���������� � Time System

**�������:**
- Pub/Sub ������� ����� GameplayTags
- ����������� ��������� ��������
- ����������� ������� ��� ��������
- Blueprint ������� ��� ������� �������
- ������� ������� ��� �������

### 2. Quest System Integration
**��������������:** `Plugins/QuestSystem/Source/QuestSystemRuntime/`

**����� �����:**
- `Integration/QuestEventBridge.h/.cpp` � �������������� �������� �� �������

**���������:**
- ������������ ����������� �� GameEventBus � Build.cs
- �������������� ������������� Bridge � QuestSubsystem
- ��������� generic events ����� GameplayTags

**�����������:**
- ������ ������������� �������� ������� �� ������ ������
- ��������� ������� �� �������, ������, ��������� � �.�.
- �� ������������� ����� Quest Data Assets

### 3. Dialogue System Integration
**��������������:** `Plugins/DialogueSystem/Source/DialogueSystemCore/`

**����� �����:**
- `Effects/DialogueEffect_EmitGameEvent.h/.cpp` � ������ ��� ������� �������
- `Effects/DialogueEffect_EmitMultipleGameEvents.h` � ������������� �������

**���������:**
- ������������ ����������� �� GameEventBus
- ����� ������� ��� ��� �������

**�����������:**
- ������� ����� ����������� ������� ��� �������
- ������� ����� ����������� ������� ��� ���������
- ��������� ����� Dialogue Data Assets

### 4. Inventory System Integration
**��������������:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/`

**����� �����:**
- `Integration/InventoryGameEventBridge.h/.cpp` � ������� ������� ���������

**�������:**
- `EmitItemAcquiredEvent` � ������� �������
- `EmitItemUsedEvent` � ����������� �������
- `EmitItemCraftedEvent` � ������ �������
- `EmitItemGiftedEvent` � ������� ������� NPC
- `EmitCustomInventoryEvent` � ��������� �������

**�����������:**
- ������ ������������� ����������� ��������� ���������
- ��������� ��� ����

### 5. ������������
**��������������:** `Plugins/GameEventBus/Documentation/`

**�����:**
- `IntegrationGuide.md` � ������ ����������� �� ����������
- `GameplayTags.md` � ������ � ��������� GameplayTags
- `README.md` � ����� � quick start
- `ARCHITECTURE.md` � ������������� ��������

## ?? ��� ������������

### ��� ��������� (����� Data Assets)

#### 1. ����� "������� 5 �����"

�������� **Quest Data Asset:**

```
Objective:
  EventTag: "Inventory.Event.ItemAcquired"
  ItemId: "Apple"
  Count: 5
```

**������!** ����� ����� ������ 5 �����, ����� ��������� �������������.

#### 2. ������ ��������� �����

�������� **Dialogue Data Asset:**

```
Node:
  Text: "������ ��� ������ �������!"
  
  Effects:
    - Emit Game Event:
        EventTag: "Quest.Event.Start"
        StringParam: "SaveVillage"
```

**������!** ����� ����� ��������� ���� ����, ����� ����������.

#### 3. ����� ������� �� �������

```
Objective Progress Gate:
  Require Time Of Day: true
  Required Time Start: 20
  Required Time End: 6
```

**������!** ���� ����� ����������� ������ �����.

### ��� ������������ (C++)

#### 1. ������� �������

```cpp
// ������� �������
UGameEventBusLibrary::EmitSimpleEvent(this, EventTag);

// ������� � ���������
UGameEventBusLibrary::EmitItemEvent(this, EventTag, ItemId, Count, Actor);

// ��� �������� ����� ���������
UInventoryGameEventBridge::EmitItemAcquiredEvent(this, ItemId, Count, Player);
```

#### 2. �������� �� �������

```cpp
// �������� subsystem
UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);

// ����������� �� �������
FDelegateHandle Handle = EventBus->SubscribeToEventNative(
    EventTag,
    FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UMyClass::OnEvent)
);

// ��������� �������
void UMyClass::OnEvent(const FGameEventPayload& Payload)
{
    // Your logic
}
```

#### 3. ����������� �������

```cpp
// �������� ����� �������
UCLASS()
class UMyEffect : public UObject, public IGenericGameEffect
{
    virtual bool ExecuteEffect_Implementation(UObject* Context, const FGameEventPayload& Payload) override
    {
   // Your effect logic
        return true;
    }
};

// ��������������� ���
UGameEventEffectRegistry* Registry = EventBus->GetEffectRegistry();
Registry->RegisterEffect(EffectTag, UMyEffect::StaticClass(), "MySystem");
```

## ?? ������� ���������

### �������� 1: ����� "������� 10 ������ ������� �����"

**Quest Data Asset:**

```
Objective 1: Collect Iron
  EventTag: "Inventory.Event.ItemAcquired"
  ItemId: "Iron"
  Count: 10

Objective 2: Talk to Blacksmith
  EventTag: "Dialogue.Event.NodeReached"
  NpcId: "Blacksmith"
  DialogNodeId: "GiveIron"
  
  Progress Gate:
    Require Time Of Day: true
    Required Time Start: 20
    Required Time End: 6
```

**��� ����������:**
1. ����� �������� ������ ? Inventory ��������� ������� ? ���� 1 �������������
2. ����� �������� � ������� ����� ? Dialogue ��������� ������� ? ���� 2 �����������
3. Quest System ����� �������

**�������� ����!** �� ��������� ����� Data Assets.

### �������� 2: NPC ��� ����� ������ ��� ������� ��������

**Dialogue Data Asset:**

```
Node:
  Text: "� ���� ���� ��� ���� ������ �������..."
  
  Conditions:
    - Evaluate Condition:
        ConditionTag: "Condition.NPC.AffinityAbove"
        StringParam: "Blacksmith"
        FloatParam: 50.0

  Effects:
    - Emit Game Event:
        EventTag: "Quest.Event.Start"
        StringParam: "SpecialQuest"
```

**��� ����������:**
1. ������� ��������� ������� ����� Effect Registry
2. ���� �������� > 50 ? ���� ��������
3. ��� ������ ����� ����������� �������
4. Quest System ��������� �����

### �������� 3: ������� ��� ����� ��� �������������

**Item Data Asset:**

```
Item: Mysterious Letter

On Use Effects:
  - Emit Game Event:
      EventTag: "Quest.Event.Start"
      StringParam: "InvestigateLetter"
```

**��� ����������:**
1. ����� ���������� ������
2. Inventory ��������� �������
3. Quest System ��������� ����� �������������

## ?? �����������

```
      ????????????????????????????????????
            ?   GameEventBusSubsystem          ?
     ?           ?
        ?  � Subscribe/Emit events         ?
                  ?  � GameplayTag routing           ?
       ?  � Effect/Condition Registry     ?
          ?  � Event history & debugging     ?
    ????????????????????????????????????
           ?
      ???????????????????????????????????????????????????
      ?    ?  ?
?????????????          ?????????????????        ???????????????
?  Quest    ?          ?   Dialogue    ?        ?  Inventory  ?
?  System   ?          ?    System     ?        ?System    ?
?           ?        ?     ?     ?             ?
? + Bridge  ?       ? + Effect      ? ? + Bridge    ?
?   (Listen)??   (Emit)      ?      ?   (Emit)    ?
?????????????    ?????????????????        ???????????????

     ?        ?           ?
     ???????????????????????????????????????????????????
         ?
???????????????????????
       ?    Time System      ?
           ?              ?
        ?  + Emitter          ?
   ?    (Emit events)    ?
              ???????????????????????
```

## ? ������������

1. **��� ������ ������������**
   - Quest System �� ����� � Dialogue
   - Dialogue �� ����� � Quest
   - Inventory �� ����� � Quest
   - ��� �������� ����� �������

2. **��������**
   - ����� �������� ����� �������
   - ����� �������� ����� �������
   - ����� �������� ����� ������
   - ��������� � ����� ������� �� ������ ������

3. **��������� ��� ����**
   - �������� ������ Data Assets
   - ����������� ������� ����� GUI
   - �� ����� �����������

4. **Blueprint-friendly**
   - ��� ������� �������� � Blueprint
   - ������� ���� ��� ������� �������
   - ���������� ���������

5. **����������������**
   - �������� � ����� ����������� ������
   - ������������������ �� ������� �� ���������� ��������
   - Native delegate ������� UE

6. **�������**
   - ����������� �������
   - ������� �������
   - Debug �������

## ?? �������� ������

| ����� | ���������� |
|-------|-----------|
| `UGameEventBusSubsystem` | ����������� event bus |
| `UGameEventEffectRegistry` | ����������� ��������/������� |
| `UGameEventBusLibrary` | Blueprint helpers |
| `UGameEventIntegrationHelpers` | ���������� ��������� |
| `UQuestEventBridge` | Quest ? EventBus ���������� |
| `UInventoryGameEventBridge` | Inventory ? EventBus ������� |
| `UDialogueEffect_EmitGameEvent` | Dialogue ? EventBus ������ |
| `UTimeSystemGameEventEmitter` | Time ? EventBus ������� |

## ?? GameplayTags (�������)

### Events
```
Inventory.Event.ItemAcquired
Inventory.Event.ItemUsed
Inventory.Event.ItemCrafted

Dialogue.Event.Started
Dialogue.Event.NodeReached
Dialogue.Event.ChoiceMade

Quest.Event.Started
Quest.Event.Completed
Quest.Event.ObjectiveCompleted

Time.Event.HourChanged
Time.Event.DayChanged

NPC.Event.AffinityChanged
NPC.Event.TalkStarted

Location.Event.Entered
Combat.Event.EnemyKilled
Weather.Event.Changed
```

### Effects
```
Effect.Quest.Start
Effect.Quest.Complete

Effect.Dialogue.Start
Effect.Dialogue.UnlockBranch

Effect.Inventory.GiveItem
Effect.NPC.ModifyAffinity
```

### Conditions
```
Condition.Quest.IsActive
Condition.Inventory.HasItem
Condition.Time.InRange
Condition.NPC.AffinityAbove
```

## ?? ��������� ����

1. **��������� GameplayTags** � Project Settings
2. **�������� Setup All Integrations** � GameMode BeginPlay
3. **�������� ������� �������** � ���� ������� (Inventory, Time, etc)
4. **���������� ������/�������** ����� Data Assets
5. **����������!**

## ?? ������������

- **[Integration Guide](Documentation/IntegrationGuide.md)** � ������ �����������
- **[GameplayTags](Documentation/GameplayTags.md)** � ������ �����
- **[README](README.md)** � quick start
- **[ARCHITECTURE](ARCHITECTURE.md)** � �����������

## ?? ����

�� ��������:

? ����������� **Event-Driven �����������**  
? **�����������** ������� ��� ����������� ������������  
? **Data-Driven** ��������� ����� Data Assets  
? **Blueprint-friendly** �����������  
? **��������������** �������  
? **������ ������������**  

**������ ���� ������� �������� ��������������� ��� ��������! ??**

---

*Created with ?? for game designers and developers*
