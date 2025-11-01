# ?? ��������� �����������: QuestEventBridge ������ �������� �� GameEvent.*!

## ?? ����������� �������� �������!

**QuestEventBridge ��� �������� �� ������ ����, � ������� ����������� � ������!**

### ��� ����:

```cpp
// QuestEventBridge ������������ ��:
FGameplayTag ItemEventTag = FGameplayTag::RequestGameplayTag(FName("Inventory.Event"), false);
//       ^^^^^^^^^^^^^^^^
//             ������ ���!
```

###  ��� �������������:

```cpp
// Inventory ����������:
FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory.ItemAcquired"), false);
//       ^^^^^^^^^^^^^^^^^^
//      ����� ���!
```

**���������:** ������� �������������, �� QuestEventBridge �� �� ������� � **������ �������� �����**!

---

## ? �������:

��������� �������� �� **����� ���� GameEvent.*** � `QuestEventBridge::SubscribeToEventTags()`:

```cpp
// 1. GameEvent.Inventory.* ? �����!
// 2. GameEvent.Quest.* ? �����!
// 3. GameEvent.Dialogue.* ? �����!
// 4. Inventory.Event.* ? ������ (��� �������������)
// 5. Dialogue.Event.* ? ������ (��� �������������)
// 6. Time.Event.* ? ������
```

������ QuestEventBridge ����� �������� **��� �������** �� ����� ��������!

---

## ?? ��� ��������������:

### ��� 1: Intermediate ��� QuestSystem ��� ������

### ��� 2: ������������ ������
- � Visual Studio: Build ? Rebuild Solution
- ��� �������� Unreal Editor � �������� `.uproject` ������

### ��� 3: ��������� ���� (PIE)

### ��� 4: ��������� Output Log ��� ������

**������ �������:**
```
[LogQuestBridge] ?? SubscribeToEventTags: Starting subscription...
[LogQuestBridge] ? Subscribed to hierarchy: GameEvent.Inventory
[LogQuestBridge] ? Subscribed to hierarchy: GameEvent.Quest
[LogQuestBridge] ? Subscribed to hierarchy: GameEvent.Dialogue
[LogQuestBridge] ?? SubscribeToEventTags: Complete! Total subscriptions: 6
```

### ��� 5: ��������� ������

**������ ���� ������ �������:**

```
[InventoryGameEventBridge] ? EventTag valid: 'GameEvent.Inventory.ItemAcquired'
[InventoryGameEventBridge] ?? Event emitted to GameEventBus!
    ??
[EmitItemEvent] ?? Called! EventTag='GameEvent.Inventory.ItemAcquired'
[EmitItemEvent] ?? Calling EventBus->EmitEvent...
 ??
[GameEventBus] ?? EmitEvent: 'GameEvent.Inventory.ItemAcquired'
    ??
[QuestEventBridge] ?? Received game event: 'GameEvent.Inventory.ItemAcquired'  ? ����� ���!
    ??
[QuestEventBridge] ?? ForwardToQuestSystem: Converting...
    ??
[QuestEventBridge] ?? Emitting to QuestSubsystem
    ??
[QuestEventBus::BroadcastEvent] ?? Processing event for 1 active quests
    ??
[QuestEventBus::BroadcastEvent] ??? MATCH FOUND! Adding 1 progress
    ??
[QuestProgressManager] ? Objective completed! Quest progress: 1/3
```

**���� ������ ��� ������� � ������ ����������!!!** ??????

---

## ?? ������ ���� �����������:

### �������� #1: ������������ ���� � ���� ? ����������
- `InventoryGameEventBridge.cpp`: ���� �������� �� `GameEvent.Inventory.*`

### �������� #2: ���������� ����� � ������� ? ����������
- `Config/DefaultGameplayTags.ini`: ��������� ��� ���� `GameEvent.*`

### �������� #3: QuestEventBridge �� �������� �� ����� ���� ? ����������
- `QuestEventBridge.cpp`: ��������� �������� �� `GameEvent.*`

---

## ?? ��������� ����:

1. ? **������������ ������**
2. ? **��������� ����**
3. ? **��������� ������**
4. ? **�������� Output Log** � ������ ������� **������ �������!**

**������ �� ������ �������� �� 100%!** ???
