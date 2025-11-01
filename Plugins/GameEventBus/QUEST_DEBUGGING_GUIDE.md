# ?? ����������� ������� � �������������� ����������� �������

## ?? ����
���� ���� ������� ����� �������, ������ ������ �� ����������� ������������� ��� ��������� ��������� ��� ������ ������� ��������.

---

## ?? ��� 1: �������� ������� ���������

### ? ��������� ��� ����� �������
```cpp
// � ������� Unreal Engine:
QuestDebugPrint
```

**��� ��������:**
- ���� �� �������� ������?
- ���������� �� ������� Phase?
- ����� objectives � �� ������� (EventTag, ItemId, Count)?

**������ ����������� ������:**
```
?? Quest: 'Q_AppleCollection'
   State: 1 (Active)
   Current Phase: 'CollectApples'
   ?? Objectives (1):
    ? IN PROGRESS 'Obj_CollectApples': 0/3
      Conditions (1):
        - EventTag='GameEvent.Inventory.ItemAcquired', ItemId='Item_Apple', Count=1
```

---

## ?? ��� 2: �������� ����������� GameEventBridge

```cpp
// � �������:
QuestDebugListeners
```

**��� ��������:**
- `? GameEventBridge is CONNECTED` � ������ ����!
- ���� `?? GameEventBridge is NULL` � ������ ������ GameEventBus �� ���������!

**��� ���������:**
1. �������� `.uproject`
2. ��������� ��� `GameEventBus` � ������ `Plugins` � `"Enabled": true`
3. ������������� ��������

---

## ?? ��� 3: �������� ��������� � ������� �������

### ��������� ������� (��������, ������) � �������� � ���:

**������ ���� ������� �����:**

```
[InventoryComponent::AddItem] ? Item 'Item_Apple' added! Quantity=1, Owner=BP_PlayerCharacter_C
    ??
[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ?? Emitting ItemAcquired event: ItemID='Item_Apple', Quantity=1
    ??
[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ? Event emitted to GameEventBridge
    ??
[GameEventBus] ?? EmitEvent: 'GameEvent.Inventory.ItemAcquired' | StringParam='Item_Apple' | IntParam=1
    ??
[QuestEventBridge] ?? Received game event: 'GameEvent.Inventory.ItemAcquired' | StringParam='Item_Apple' | IntParam=1
    ??
[QuestEventBridge] ?? ForwardToQuestSystem: Converting GameEventPayload to QuestEventPayload...
    ??
[QuestEventBridge] ?? Emitting to QuestSubsystem: EventTag='GameEvent.Inventory.ItemAcquired', StringParam='Item_Apple', IntParam=1
    ??
[QuestEventBus::BroadcastEvent] ?? Processing event 'GameEvent.Inventory.ItemAcquired' for 1 active quests
    ??
[QuestEventBus::BroadcastEvent] ??? MATCH FOUND! Event matched objective 'Obj_CollectApples' in quest 'Q_AppleCollection'
```

---

## ?? �������� �������� � �������

### ? �������� 1: "GameEventBus NOT AVAILABLE"
```
[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ?? GameEventBus NOT AVAILABLE!
```

**�������:** ������ `GameEventBus` �� ��������� � `InventorySystemRuntime.Build.cs`

**�������:**
```csharp
// Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/InventorySystemRuntime.Build.cs
PublicDependencyModuleNames.AddRange(new string[] 
{
    "Core",
    "CoreUObject",
    "Engine",
    "GameplayTags",
    "GameEventBus" // ? ������ ����!
});
```

---

### ? �������� 2: "No active quests to update!"
```
[QuestEventBus::BroadcastEvent] ?? No active quests to update!
```

**�������:** ����� �� �������

**�������:**
```cpp
// ��������� ����� ����� Blueprint ��� C++:
UQuestSubsystem* QuestSystem = UQuestSubsystem::Get(this);
QuestSystem->StartQuestAsync(FName("Q_AppleCollection"), OnQuestStarted);
```

---

### ? �������� 3: "EventTag mismatch"
```
[QuestEventBus::DoesConditionMatchEvent] ? EventTag mismatch - rejecting
  Condition.EventTag = 'GameEvent.Inventory.ItemAcquired'
  Payload.EventTag   = 'GameEvent.Item.Acquired' ? �����������!
```

**�������:** EventTag � ������� ������ �� ��������� � ���, ��� ������ �������

**�������:**
1. ��������� Data Asset ������ (DA_Quest_AppleCollection)
2. ��������� ��� `EventTag` � `FObjectiveCondition` = `GameEvent.Inventory.ItemAcquired`
3. ��������� `Config/DefaultGameplayTags.ini` � ��� ������ ���� ���������������

---

### ? �������� 4: "ItemId mismatch"
```
[QuestEventBus::DoesConditionMatchEvent] ? ItemId mismatch: wanted 'Item_Apple' got 'Apple'
```

**�������:** ItemID � ������� ������ �� ��������� � ItemID � Data Asset ��������

**�������:**
1. �������� DA_Item_Apple
2. ��������� ���� `ItemID` (������ ���� `Item_Apple`)
3. �������� ������� � ������, ���� �����

---

### ? �������� 5: "Quest asset NOT FOUND"
```
[QuestEventBus::BroadcastEvent] ? Quest asset NOT FOUND for 'Q_AppleCollection'
```

**�������:** ����� �� �������� � ������

**�������:**
����� ������ ���������� �� ����: `Content/Quests/DA_Quest_AppleCollection`

��������� � QuestAssetLoader:
```cpp
// QuestAssetLoader.cpp ������ ������ �� ����:
FString AssetPath = FString::Printf(TEXT("/Game/Quests/DA_Quest_%s.DA_Quest_%s"), *QuestId.ToString(), *QuestId.ToString());
```

---

## ?? ��� 4: ������������ �������

������ ������� ������� ������� ��� ��������:

```cpp
// � ������� Unreal:
QuestTestEvent GameEvent.Inventory.ItemAcquired Item_Apple 1
```

�������� � ��� � ��������� �� �����!

---

## ? ����������� ���-����

����� �������� ���� ���������:

- [ ] GameEventBus ������� � `.uproject`
- [ ] InventorySystemRuntime.Build.cs �������� ����������� �� GameEventBus
- [ ] QuestSystemRuntime.Build.cs �������� ����������� �� GameEventBus
- [ ] DefaultGameplayTags.ini �������� ��� ������ ����
- [ ] ����� ������� (����������� `QuestDebugPrint` ��� ��������)
- [ ] ItemID � ������� ������ ��������� � ItemID � Data Asset ��������
- [ ] EventTag � ������� ������ ��������� � ���, ��� ��������

---

## ?? ��������� ��������

����� ������� ������ ������ ���� ����� ���:

```
[InventoryComponent::AddItem] ? Item 'Item_Apple' added! Quantity=1
[GameEventBus] ?? EmitEvent: 'GameEvent.Inventory.ItemAcquired' | StringParam='Item_Apple' | IntParam=1
[QuestEventBridge] ?? Received game event
[QuestEventBus::BroadcastEvent] ?? Processing event for 1 active quests
[QuestEventBus::BroadcastEvent] ??? MATCH FOUND! Adding 1 progress
[QuestProgressManager] ? Objective 'Obj_CollectApples' progress updated: 1/3
```

���� ������ ��� � **�� ��������!** ??

---

## ?? ���� �� ����� �� ��������

1. ��������� `QuestDebugPrint` � ��������� ��� ����� �������
2. ��������� `QuestDebugListeners` � ��������� ����������� GameEventBridge
3. ��������� ������� � �������� ���� ��� �� ������ �� �����
4. ������� ��� ������� �������� � ��� � ����� ��������
5. ����������� `QuestTestEvent` ��� ������ ������� �������

�����! ??
