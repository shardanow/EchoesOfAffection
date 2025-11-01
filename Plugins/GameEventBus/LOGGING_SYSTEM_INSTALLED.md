# ? ������� ����������� ��� ����������� ������� �����������!

## ?? ��� ���� ���������

### ?? ��������� ����������� �� ���� �������:

1. **InventoryComponent::AddItem**
   - �������� ���������� ��������
   - ���������� ItemID, ����������, ���������

2. **InventoryComponentGameEventHelper::EmitItemAcquiredEvent**
   - �������� ������� �������
   - ��������� ����������� GameEventBus
   - ���������� ��� ��������� �������

3. **GameEventBusSubsystem::EmitEvent**
   - �������� ������ ������� � ������� �����������
   - ���� ���������� �������

4. **QuestEventBridge::OnGameEvent**
   - �������� ��������� ������� �� GameEventBus
   - ���������� ������� ����������� GameEventPayload ? QuestEventPayload

5. **QuestEventBus::BroadcastEvent** (����������� ��������!)
   - ���������� �������� �������
   - ��� ������� ������: �������� objectives � conditions
   - ��������� ����������� ������������� EventTag, ItemId, Count
   - �������� ����������: ������� objectives ���������, ������� matches �������

6. **QuestEventBus::DoesConditionMatchEvent**
   - ��������� �������� ������� �������
   - ���������� ������ condition �������� ��� �� ��������

### ?? ���������� �������

```cpp
// �������� ��� �������� ������ � �� objectives � ���������
QuestDebugPrint

// ��������� ����������� GameEventBridge � EventBus
QuestDebugListeners

// �������������� ������� ������� �������
QuestTestEvent GameEvent.Inventory.ItemAcquired Item_Apple 1
```

### ?? ������������

������ ����: `Plugins/GameEventBus/QUEST_DEBUGGING_GUIDE.md`
- ��������� ���������� �� �����������
- �������� �������� � �������
- ���-���� ����� ��������

---

## ?? ��� ��������������

### ��� 1: �������� ��� (��� ���������)
```powershell
# Intermediate build folders ��� �������� ��� �������
```

### ��� 2: ������������ ������ � Unreal Editor
1. �������� Unreal Editor (���� ������)
2. �������� `.uproject` ���� � �� ���������� ��� �������
3. ��������� ���������� ����������

### ��� 3: ��������� ���� � PIE
1. ��������/��������� ����� � �������� �� ������ ��������
2. � ������� ���������:
```
QuestDebugPrint
```
3. ��������� ��� ����� ������� � objective ��������� ���������

### ��� 4: ��������� �������
�������� � **Output Log** � ������ ��������� **������ ������� �����**:

```
[InventoryComponent::AddItem] ? Item 'Item_Apple' added! Quantity=1, Owner=BP_PlayerCharacter_C
    ??
[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ?? Emitting ItemAcquired event: ItemID='Item_Apple', Quantity=1
    ??
[GameEventBus] ?? EmitEvent: 'GameEvent.Inventory.ItemAcquired' | StringParam='Item_Apple' | IntParam=1
    ??
[QuestEventBridge] ?? Received game event: 'GameEvent.Inventory.ItemAcquired' | StringParam='Item_Apple'
    ??
[QuestEventBus::BroadcastEvent] ?? Processing event for 1 active quests
    ??
[QuestEventBus::BroadcastEvent] ??? MATCH FOUND! Event matched objective in quest
```

### ��� 5: ������� ��������
���� ����� �� ����������� � **������� ����� ������� ��� ������ ��� ��������!**

�������� `QUEST_DEBUGGING_GUIDE.md` ��� ���������� ������� �������� �������.

---

## ?? ����������� � ����

### ����� ��������:
1. `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Private/InventoryComponent.cpp`
2. `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Private/InventoryComponentGameEventHelper.cpp`
3. `Plugins/GameEventBus/Source/GameEventBus/Private/GameEventBusSubsystem.cpp`
4. `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/Integration/QuestEventBridge.cpp`
5. `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestEventBus.cpp`
6. `Plugins/QuestSystem/Source/QuestSystemRuntime/Public/QuestSubsystem.h`
7. `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestSubsystem.cpp`

### ����� �����:
1. `Plugins/GameEventBus/QUEST_DEBUGGING_GUIDE.md` � ������ ����������� �� �����������

---

## ?? ��������� ����

1. ? ������������ ������ (File ? Refresh Visual Studio Project, ����� Build)
2. ? ��������� ����
3. ? ��������� `QuestDebugPrint` � ��������� �����
4. ? ��������� ������� � �������� ����
5. ? ������� ��� ������� ��������
6. ? ����������� `QUEST_DEBUGGING_GUIDE.md` ��� �����������

---

## ?? ���� �� ����� �� ����������

��������� ��� �������� ���� Intermediate:
```powershell
cd "D:\Game Dev\Projects\UEProjects\EchoesOfAffection"
Remove-Item -Recurse -Force "Intermediate\Build"
Remove-Item -Recurse -Force "Plugins\GameEventBus\Intermediate"
Remove-Item -Recurse -Force "Plugins\QuestSystem\Intermediate"
Remove-Item -Recurse -Force "Plugins\AdvancedInventorySystem\Intermediate"
```

����� �������� `.uproject` � ��������� ����������.

---

������ � ��� ���� **������ ������� �����������** �������! ??
