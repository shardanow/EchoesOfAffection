# ? ��������� �����������: ���� GameEvent ���������!

## ?? ��������� ��������

**��� `GameEvent.Inventory.ItemAcquired` �� ��� ��������������� � `DefaultGameplayTags.ini`!**

### ��� �������� ����:

```
[InventoryGameEventBridge::EmitItemAcquiredEvent] ?? Called! ItemId='RedApple'
[InventoryGameEventBridge] ? EventTag 'GameEvent.Inventory.ItemAcquired' is INVALID!
```

**�������:** 
- � `Config/DefaultGameplayTags.ini` ���� ���������������� ���� `Inventory.Event.ItemAcquired`
- �� ��� ����������� ���� `GameEvent.Inventory.ItemAcquired`
- **FGameplayTag::RequestGameplayTag() ������ Invalid tag!**

---

## ? �������

��������� ��� ����������� ���� � ��������� `GameEvent` � `Config/DefaultGameplayTags.ini`:

```ini
;===========================================
; GAME EVENT BUS TAGS (unified namespace)
;===========================================
+GameplayTagList=(Tag="GameEvent.Inventory.ItemAcquired",DevComment="[GameEventBus] Item was acquired/picked up")
+GameplayTagList=(Tag="GameEvent.Inventory.ItemUsed",DevComment="[GameEventBus] Item was used/consumed")
+GameplayTagList=(Tag="GameEvent.Inventory.ItemDropped",DevComment="[GameEventBus] Item was dropped")
+GameplayTagList=(Tag="GameEvent.Inventory.ItemCrafted",DevComment="[GameEventBus] Item was crafted")
+GameplayTagList=(Tag="GameEvent.Inventory.ItemGifted",DevComment="[GameEventBus] Item was given to NPC")
+GameplayTagList=(Tag="GameEvent.Quest.Started",DevComment="[GameEventBus] Quest was started")
+GameplayTagList=(Tag="GameEvent.Quest.Completed",DevComment="[GameEventBus] Quest was completed")
+GameplayTagList=(Tag="GameEvent.Quest.ObjectiveCompleted",DevComment="[GameEventBus] Quest objective completed")
+GameplayTagList=(Tag="GameEvent.Dialogue.Started",DevComment="[GameEventBus] Dialogue started")
+GameplayTagList=(Tag="GameEvent.Dialogue.NodeReached",DevComment="[GameEventBus] Dialogue node reached")
```

---

## ?? ��� ��������������

### ��� 1: ������������� Unreal Editor

**�����:** ��������� � `DefaultGameplayTags.ini` ������� ����������� ���������!

```powershell
# �������� Unreal Editor
# �������� .uproject ������
```

### ��� 2: ��������� ���� (PIE)

1. Play in Editor
2. ��������� ������

### ��� 3: ��������� Output Log

**������ ������ ���� ������ �������:**

```
[InventoryComponent::AddItem] ? Item 'RedApple' added! Quantity=1
    ??
[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ?? Emitting event
    ??
[InventoryGameEventBridge::EmitItemAcquiredEvent] ?? Called! ItemId='RedApple'
    ??
[InventoryGameEventBridge] ? EventTag valid: 'GameEvent.Inventory.ItemAcquired'
    ??
[InventoryGameEventBridge] ?? Event emitted to GameEventBus!
    ??
[GameEventBus] ?? EmitEvent: 'GameEvent.Inventory.ItemAcquired' | StringParam='RedApple'
    ??
[QuestEventBridge] ?? Received game event: 'GameEvent.Inventory.ItemAcquired'
    ??
[QuestEventBus::BroadcastEvent] ?? Processing event for 1 active quests
    ??
[QuestEventBus::BroadcastEvent] ??? MATCH FOUND! Adding 1 progress
    ??
[QuestProgressManager] ? Objective progress updated: 1/3
```

**���� ������ ��� ������� � ������ ��������!** ??

---

## ?? �������� ������ �������

### �������� #1: ������������ ���� � ���� ? ����������
- **����:** `Inventory.Event.ItemAcquired`
- **�����:** `GameEvent.Inventory.ItemAcquired`

### �������� #2: ���������� ����� � ������� ? ����������
- ��������� ��� ���� `GameEvent.*` � `DefaultGameplayTags.ini`

---

## ?? ��������� �����

1. `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Private/Integration/InventoryGameEventBridge.cpp` ? ���������� ���� + �����������
2. `Config/DefaultGameplayTags.ini` ? ��������� ���� `GameEvent.*`

---

## ?? ��������� ����

1. ? **�������� Unreal Editor**
2. ? **�������� .uproject ������** (����� ��������� ����� ����)
3. ? **��������� ����**
4. ? **��������� ������**
5. ? **��������� Output Log** � ������ ���� ������ �������!

---

**������ �� ������ ��������!** ???
