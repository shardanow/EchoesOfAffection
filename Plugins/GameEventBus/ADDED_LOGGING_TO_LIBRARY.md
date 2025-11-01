# ?? Добавлено логирование в GameEventBusLibrary

## ? Что сделано:

Добавлено детальное логирование в `GameEventBusLibrary::EmitItemEvent` для трассировки всего пути события:

```cpp
[EmitItemEvent] ?? Called! EventTag='GameEvent.Inventory.ItemAcquired', ItemId='RedApple', Count=1
[EmitItemEvent] ? EventBus found, creating payload...
[EmitItemEvent] ?? Calling EventBus->EmitEvent...
[EmitItemEvent] ? EmitEvent call complete!
```

Также **принудительно включено логирование** (`bLogEvent=true`) для этого метода.

---

## ?? Как протестировать:

### Шаг 1: Пересоберите плагин GameEventBus

```powershell
cd "D:\Game Dev\Projects\UEProjects\EchoesOfAffection"
Remove-Item -Recurse -Force "Plugins\GameEventBus\Intermediate"
```

### Шаг 2: Откройте .uproject
Unreal Editor пересоберёт плагин.

### Шаг 3: Запустите игру и подберите яблоко

### Шаг 4: Проверьте Output Log

**Ожидаемая цепочка:**

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

Если **всё равно нет** лога `[EmitItemEvent]`, значит проблема в другом месте.

---

## ?? Следующий шаг после сборки:

Запустите игру и покажите мне **весь лог** от подбора яблока!
