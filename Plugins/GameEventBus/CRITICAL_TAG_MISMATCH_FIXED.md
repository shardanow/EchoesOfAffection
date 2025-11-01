# ?? ����������� �������� ������� � ����������!

## ?? ��������

**������ �� ����������� ������������� ��-�� �������������� GameplayTags!**

### ��� ����:

#### � InventoryGameEventBridge.cpp (�����������):
```cpp
FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Inventory.Event.ItemAcquired"), false);
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//          ������������ ���!
```

#### � ������� � ������������ (���������):
```cpp
GameEvent.Inventory.ItemAcquired
```

**���������:** ������� ������������� � ����� �����, � ������ ������� ������ ? **������� ����������!**

---

## ? �������

���������� ��� ���� � `InventoryGameEventBridge.cpp`:

| ���� (? �����������) | ����� (? ���������) |
|---|---|
| `Inventory.Event.ItemAcquired` | `GameEvent.Inventory.ItemAcquired` |
| `Inventory.Event.ItemUsed` | `GameEvent.Inventory.ItemUsed` |
| `Inventory.Event.ItemCrafted` | `GameEvent.Inventory.ItemCrafted` |
| `Inventory.Event.ItemGifted` | `GameEvent.Inventory.ItemGifted` |
| `Inventory.Event.ItemDropped` | `GameEvent.Inventory.ItemDropped` |

---

## ?? ��� ���� �������

### 1. ������ �����
���������� ��� ������� **�����������** �� **�� �������� � GameEventBus**:
```
? [InventoryComponent::AddItem] Item 'RedApple' added
? [InventoryComponentGameEventHelper::EmitItemAcquiredEvent] Event emitted
? [GameEventBus] EmitEvent � ��� ����� ����!
? [QuestEventBridge] Received game event � ���!
```

### 2. ��������� ��������� �����������
��������� ����������� � `InventoryGameEventBridge::EmitItemAcquiredEvent`:
- �������� WorldContextObject
- �������� ���������� EventTag
- �������� GAMEEVENTBUS_AVAILABLE
- ��� �������� �������

### 3. ���������� ����
��� ���� ��������� ��������� � ������� ��������� `GameEvent.*`

---

## ?? ��� ��������������

### ��� 1: ������������ ������
```powershell
# �������� Intermediate ��� InventorySystemCore
cd "D:\Game Dev\Projects\UEProjects\EchoesOfAffection"
Remove-Item -Recurse -Force "Plugins\AdvancedInventorySystem\Intermediate"
```

����� �������� `.uproject` � �� ���������� ������.

### ��� 2: ��������� ����
1. Play in Editor (PIE)
2. ��������� ������

### ��� 3: ��������� ����

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
[QuestEventBridge] ?? Received game event
    ??
[QuestEventBus::BroadcastEvent] ?? Processing event for 1 active quests
    ??
[QuestEventBus::BroadcastEvent] ??? MATCH FOUND! Adding 1 progress
    ??
[QuestProgressManager] ? Objective completed! Quest progress: 1/3
```

���� ������ ��� ������� � **������ ��������!** ??

---

## ?? ���������� �����

1. `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Private/Integration/InventoryGameEventBridge.cpp` ? ���������
2. ��� ����������� ����������� �� ����������� ����� (7 ������)

---

## ?? ����

**��������:** �������������� GameplayTags ����� �������� � �����������  
**�������:** Copy-paste ������ � ��������� ����������  
**�������:** ���������� ���� ����� � ��������� `GameEvent.*`  
**������:** ? ����������

������ ������ ������ ����������� �������������! ??
