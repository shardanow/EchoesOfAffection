# Game Event Bus Plugin

**Центральная система событий для Unreal Engine — связывайте игровые системы БЕЗ жёстких зависимостей!**

## Что это?

Game Event Bus — это plugin для UE5, который позволяет разным игровым системам взаимодействовать друг с другом через события (GameplayTags), без необходимости прямых зависимостей между плагинами.

### Зачем это нужно?

**Проблема:** У вас есть Quest System, Dialogue System, Inventory System, Time System... Как им взаимодействовать?

? **Плохой подход:** Quest System зависит от Dialogue System, который зависит от Inventory...  
? **Хороший подход:** Все системы общаются через центральный Event Bus!

## Возможности

- ? **Event-Driven Architecture** — системы общаются через события
- ? **GameplayTags** — типобезопасные события через Unreal Tag System
- ? **Zero Dependencies** — каждый плагин работает независимо
- ? **Blueprint-Friendly** — настройка без кода
- ? **Data-Driven** — всё через Data Assets
- ? **Effect Registry** — регистрация кастомных эффектов
- ? **Condition System** — проверка условий из разных систем
- ? **Debug Tools** — логирование, история событий, отладка

## Быстрый старт

### 1. Установка

Скопируйте плагин в папку `Plugins/GameEventBus` вашего проекта.

Включите плагин в `.uproject`:

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

### 2. Настройка интеграций

В вашем GameMode или GameInstance:

**C++:**
```cpp
void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();
  UGameEventIntegrationHelpers::SetupAllIntegrations(this);
}
```

**Blueprint:**
```
Event BeginPlay -> Setup All Integrations
```

### 3. Используйте!

**Эмиссия события из инвентаря:**
```cpp
UInventoryGameEventBridge::EmitItemAcquiredEvent(this, FName("Apple"), 5, PlayerActor);
```

**Квест автоматически получит событие и обновит прогресс!**

## Примеры

### Квест: Собрать 5 яблок

```
Quest Data Asset:
  Objective:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Apple"
    Count: 5
```

Инвентарь эмитирует событие:
```cpp
EmitItemAcquiredEvent(this, "Apple", 1, Player);
```

Quest System получает событие и обновляет прогресс ? **Цель выполнена!**

### Диалог запускает квест

```
Dialogue Node:
  Text: "Помоги мне!"
  Effects:
    - EmitGameEvent:
        EventTag: "Quest.Event.Start"
        StringParam: "HelpFarmer"
```

Quest System получает событие ? **Квест запущен!**

## Архитектура

```
????????????????????????????????????????
?     GameEventBusSubsystem            ?
? • Subscribe/Emit events  ?
?   • GameplayTag routing              ?
?   • Effect Registry        ?
????????????????????????????????????????
      ?
     ???????????????????????
     ?       ?      ?
??????????  ????????  ??????????
? Quest  ?  ?Dialog?  ? Invent.?
? System ?  ?System?  ? System ?
??????????  ????????  ??????????
```

## Основные компоненты

### 1. GameEventBusSubsystem

Центральная система событий:
- Подписка на события
- Эмиссия событий
- Роутинг по GameplayTags

### 2. GameEventEffectRegistry

Регистрация эффектов и условий:
- Эффекты (Quest.Start, Dialogue.Start, etc)
- Условия (Quest.IsActive, Inventory.HasItem, etc)

### 3. Bridges

Связь между системами:
- **QuestEventBridge** — Quest ? EventBus
- **InventoryGameEventBridge** — Inventory ? EventBus
- **TimeSystemGameEventEmitter** — Time ? EventBus

### 4. Integration Helpers

Упрощённые Blueprint функции для настройки

## События (GameplayTags)

### Inventory
- `Inventory.Event.ItemAcquired`
- `Inventory.Event.ItemUsed`
- `Inventory.Event.ItemCrafted`
- `Inventory.Event.ItemGifted`

### Dialogue
- `Dialogue.Event.Started`
- `Dialogue.Event.NodeReached`
- `Dialogue.Event.ChoiceMade`

### Quest
- `Quest.Event.Started`
- `Quest.Event.Completed`
- `Quest.Event.ObjectiveCompleted`

### Time
- `Time.Event.HourChanged`
- `Time.Event.DayChanged`

### NPC
- `NPC.Event.AffinityChanged`
- `NPC.Event.TalkStarted`

[Полный список тегов](Documentation/GameplayTags.md)

## Документация

- [Integration Guide](Documentation/IntegrationGuide.md) — полное руководство по интеграции
- [GameplayTags](Documentation/GameplayTags.md) — список и настройка тегов

## API

### C++ API

```cpp
// Get subsystem
UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);

// Emit event
FGameEventPayload Payload(EventTag);
Payload.StringParam = "MyParam";
EventBus->EmitEvent(Payload);

// Subscribe to event
EventBus->SubscribeToEventNative(EventTag, 
    FGameEventNativeDelegate::FDelegate::CreateUObject(this, &MyClass::OnEvent)
);

// Register effect
UGameEventEffectRegistry* Registry = EventBus->GetEffectRegistry();
Registry->RegisterEffect(EffectTag, UMyEffect::StaticClass(), "MySystem");
```

### Blueprint API

```
// Emit
Emit Simple Event (EventTag)
Emit Item Event (EventTag, ItemId, Count)
Emit Dialogue Event (EventTag, DialogueId, NodeId)

// Subscribe
Subscribe To Event (EventTag, Delegate)
Subscribe To Event Hierarchy (ParentTag, Delegate)

// Setup
Setup All Integrations
Setup Quest Dialogue Integration
Setup Inventory Quest Integration
```

## Интеграция с вашими системами

### Добавить поддержку EventBus в свою систему

1. Добавьте опциональную зависимость в Build.cs:
```csharp
if (System.IO.Directory.Exists(Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins", "GameEventBus")))
{
    PrivateDependencyModuleNames.Add("GameEventBus");
}
```

2. Эмитируйте события при важных действиях:
```cpp
#if __has_include("GameEventBusSubsystem.h")
    #include "GameEventBusSubsystem.h"
#endif

void MySystem::DoSomething()
{
    // Your logic...
    
    #if defined(GAMEEVENTBUS_AVAILABLE)
        UGameEventBusLibrary::EmitSimpleEvent(this, MyEventTag);
    #endif
}
```

3. Готово! Другие системы смогут реагировать на ваши события.

## Системные требования

- Unreal Engine 5.4+
- C++17
- Gameplay Tags module

## Зависимости

### Required
- Core
- CoreUObject
- Engine
- GameplayTags

### Optional (для интеграции)
- QuestSystem
- DialogueSystem
- InventorySystem

## FAQ

**Q: Можно ли использовать без других плагинов?**  
A: Да! EventBus работает независимо.

**Q: Нужно ли добавлять зависимость в Build.cs?**  
A: Только опциональную (см. пример выше).

**Q: Как добавить своё событие?**  
A: Создайте GameplayTag и эмитируйте через EventBus.

**Q: Влияет ли на производительность?**  
A: Минимально. Используется native delegate система UE.

**Q: Можно ли использовать в multiplayer?**  
A: Да, но учтите что EventBus локален для каждого клиента.

## Roadmap

- [ ] Network replication support
- [ ] Event replay system
- [ ] Visual debugger widget
- [ ] Auto-discovery of event emitters
- [ ] Event batching optimization

## License

MIT License (или ваша лицензия)

## Support

- GitHub Issues: [link]
- Discord: [link]
- Forum: [link]

## Credits

Developed by [Your Studio Name]  
Special thanks to [Contributors]

---

**Happy Game Development! ??**
