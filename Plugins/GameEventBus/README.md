# Game Event Bus Plugin

**����������� ������� ������� ��� Unreal Engine � ���������� ������� ������� ��� ������ ������������!**

## ��� ���?

Game Event Bus � ��� plugin ��� UE5, ������� ��������� ������ ������� �������� ����������������� ���� � ������ ����� ������� (GameplayTags), ��� ������������� ������ ������������ ����� ���������.

### ����� ��� �����?

**��������:** � ��� ���� Quest System, Dialogue System, Inventory System, Time System... ��� �� �����������������?

? **������ ������:** Quest System ������� �� Dialogue System, ������� ������� �� Inventory...  
? **������� ������:** ��� ������� �������� ����� ����������� Event Bus!

## �����������

- ? **Event-Driven Architecture** � ������� �������� ����� �������
- ? **GameplayTags** � �������������� ������� ����� Unreal Tag System
- ? **Zero Dependencies** � ������ ������ �������� ����������
- ? **Blueprint-Friendly** � ��������� ��� ����
- ? **Data-Driven** � �� ����� Data Assets
- ? **Effect Registry** � ����������� ��������� ��������
- ? **Condition System** � �������� ������� �� ������ ������
- ? **Debug Tools** � �����������, ������� �������, �������

## ������� �����

### 1. ���������

���������� ������ � ����� `Plugins/GameEventBus` ������ �������.

�������� ������ � `.uproject`:

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

### 2. ��������� ����������

� ����� GameMode ��� GameInstance:

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

### 3. �����������!

**������� ������� �� ���������:**
```cpp
UInventoryGameEventBridge::EmitItemAcquiredEvent(this, FName("Apple"), 5, PlayerActor);
```

**����� ������������� ������� ������� � ������� ��������!**

## �������

### �����: ������� 5 �����

```
Quest Data Asset:
  Objective:
    EventTag: "Inventory.Event.ItemAcquired"
    ItemId: "Apple"
    Count: 5
```

��������� ��������� �������:
```cpp
EmitItemAcquiredEvent(this, "Apple", 1, Player);
```

Quest System �������� ������� � ��������� �������� ? **���� ���������!**

### ������ ��������� �����

```
Dialogue Node:
  Text: "������ ���!"
  Effects:
    - EmitGameEvent:
        EventTag: "Quest.Event.Start"
        StringParam: "HelpFarmer"
```

Quest System �������� ������� ? **����� �������!**

## �����������

```
????????????????????????????????????????
?     GameEventBusSubsystem            ?
? � Subscribe/Emit events  ?
?   � GameplayTag routing              ?
?   � Effect Registry        ?
????????????????????????????????????????
      ?
     ???????????????????????
     ?       ?      ?
??????????  ????????  ??????????
? Quest  ?  ?Dialog?  ? Invent.?
? System ?  ?System?  ? System ?
??????????  ????????  ??????????
```

## �������� ����������

### 1. GameEventBusSubsystem

����������� ������� �������:
- �������� �� �������
- ������� �������
- ������� �� GameplayTags

### 2. GameEventEffectRegistry

����������� �������� � �������:
- ������� (Quest.Start, Dialogue.Start, etc)
- ������� (Quest.IsActive, Inventory.HasItem, etc)

### 3. Bridges

����� ����� ���������:
- **QuestEventBridge** � Quest ? EventBus
- **InventoryGameEventBridge** � Inventory ? EventBus
- **TimeSystemGameEventEmitter** � Time ? EventBus

### 4. Integration Helpers

���������� Blueprint ������� ��� ���������

## ������� (GameplayTags)

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

[������ ������ �����](Documentation/GameplayTags.md)

## ������������

- [Integration Guide](Documentation/IntegrationGuide.md) � ������ ����������� �� ����������
- [GameplayTags](Documentation/GameplayTags.md) � ������ � ��������� �����

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

## ���������� � ������ ���������

### �������� ��������� EventBus � ���� �������

1. �������� ������������ ����������� � Build.cs:
```csharp
if (System.IO.Directory.Exists(Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins", "GameEventBus")))
{
    PrivateDependencyModuleNames.Add("GameEventBus");
}
```

2. ���������� ������� ��� ������ ���������:
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

3. ������! ������ ������� ������ ����������� �� ���� �������.

## ��������� ����������

- Unreal Engine 5.4+
- C++17
- Gameplay Tags module

## �����������

### Required
- Core
- CoreUObject
- Engine
- GameplayTags

### Optional (��� ����������)
- QuestSystem
- DialogueSystem
- InventorySystem

## FAQ

**Q: ����� �� ������������ ��� ������ ��������?**  
A: ��! EventBus �������� ����������.

**Q: ����� �� ��������� ����������� � Build.cs?**  
A: ������ ������������ (��. ������ ����).

**Q: ��� �������� ��� �������?**  
A: �������� GameplayTag � ���������� ����� EventBus.

**Q: ������ �� �� ������������������?**  
A: ����������. ������������ native delegate ������� UE.

**Q: ����� �� ������������ � multiplayer?**  
A: ��, �� ������ ��� EventBus ������� ��� ������� �������.

## Roadmap

- [ ] Network replication support
- [ ] Event replay system
- [ ] Visual debugger widget
- [ ] Auto-discovery of event emitters
- [ ] Event batching optimization

## License

MIT License (��� ���� ��������)

## Support

- GitHub Issues: [link]
- Discord: [link]
- Forum: [link]

## Credits

Developed by [Your Studio Name]  
Special thanks to [Contributors]

---

**Happy Game Development! ??**
