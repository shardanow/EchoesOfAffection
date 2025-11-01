# ?? ��������� ����������� � GameEventBusLibrary

## ? ��� �������:

��������� ��������� ����������� � `GameEventBusLibrary::EmitItemEvent` ��� ����������� ����� ���� �������:

```cpp
[EmitItemEvent] ?? Called! EventTag='GameEvent.Inventory.ItemAcquired', ItemId='RedApple', Count=1
[EmitItemEvent] ? EventBus found, creating payload...
[EmitItemEvent] ?? Calling EventBus->EmitEvent...
[EmitItemEvent] ? EmitEvent call complete!
```

����� **������������� �������� �����������** (`bLogEvent=true`) ��� ����� ������.

---

## ?? ��� ��������������:

### ��� 1: ������������ ������ GameEventBus

```powershell
cd "D:\Game Dev\Projects\UEProjects\EchoesOfAffection"
Remove-Item -Recurse -Force "Plugins\GameEventBus\Intermediate"
```

### ��� 2: �������� .uproject
Unreal Editor ���������� ������.

### ��� 3: ��������� ���� � ��������� ������

### ��� 4: ��������� Output Log

**��������� �������:**

```
[InventoryGameEventBridge] ? EventTag valid
[InventoryGameEventBridge] ?? Event emitted to GameEventBus!
    ??
[EmitItemEvent] ?? Called! EventTag='GameEvent.Inventory.ItemAcquired'
[EmitItemEvent] ? EventBus found
[EmitItemEvent] ?? Calling EventBus->EmitEvent...
    ??
[GameEventBus] ?? EmitEvent: 'GameEvent.Inventory.ItemAcquired'
    ??
[QuestEventBridge] ?? Received game event
 ??
[QuestEventBus] ??? MATCH FOUND!
```

���� **�� ����� ���** ���� `[EmitItemEvent]`, ������ �������� � ������ �����.

---

## ?? ��������� ��� ����� ������:

��������� ���� � �������� ��� **���� ���** �� ������� ������!
