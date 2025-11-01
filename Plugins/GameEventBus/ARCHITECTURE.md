# Event-Driven Architecture � ������� ������

## ��� ���� �������

������� **Event-Driven �����������** ��� ���������� ������� ������ ��� ������ ������������.

## �������� ����������

### 1. GameEventBus Plugin (Core)
- **GameEventBusSubsystem** � ����������� ��������� �������
- **GameEventEffectRegistry** � ����������� �������� � �������
- **GameEventBusLibrary** � Blueprint helpers
- **GameEventIntegrationHelpers** � ���������� ���������

### 2. Bridges (����������)
- **QuestEventBridge** � Quest System ? EventBus (�������������� ��������)
- **InventoryGameEventBridge** � Inventory ? EventBus (������� �������)
- **TimeSystemGameEventEmitter** � Time System ? EventBus (������� �������)

### 3. Effects (�������)
- **DialogueEffect_EmitGameEvent** � ������� ������� �� ��������
- ������������ ����������� � Build.cs

### 4. Documentation
- **IntegrationGuide.md** � ������ �����������
- **GameplayTags.md** � ������ � ��������� �����
- **README.md** � ����� � quick start

## �����������

```
???????????????????????????????????????????????
?         GameEventBusSubsystem           ?
?  � ������� ����� GameplayTags         ?
?  � Pub/Sub �������           ?
?  � Effect/Condition Registry   ?
???????????????????????????????????????????????
 ?
    ????????????????????????????????????????
    ?         ?        ?        ?   ?
????????? ???????? ????????? ????????? ?????????
? Quest ? ?Dialog? ? Invent? ? Time  ? ?  NPC  ?
?System ? ?System? ? System? ?System ? ?System ?
????????? ???????? ????????? ????????? ?????????
```

## ��� ��� ��������

### ������ 1: ����� "������� ������"

**Quest Data Asset:**
```
Objective:
  EventTag: "Inventory.Event.ItemAcquired"
  ItemId: "Apple"
  Count: 5
```

**Inventory System:**
```cpp
// ����� ����� �������� ������
UInventoryGameEventBridge::EmitItemAcquiredEvent(this, "Apple", 1, Player);
```

**��� ����������:**
1. Inventory ��������� ������� `Inventory.Event.ItemAcquired`
2. GameEventBus �������� �������
3. QuestEventBridge ����������� ��� � QuestEventBus
4. Quest System ��������� ��������
5. **������� ������������ ����� Inventory � Quest!**

### ������ 2: ������ ��������� �����

**Dialogue Data Asset:**
```
Node Effects:
  - EmitGameEvent:
      EventTag: "Quest.Event.Start"
    StringParam: "SaveVillage"
```

**��� ����������:**
1. Dialogue Effect ��������� �������
2. GameEventBus ������� �������
3. QuestEventBridge �������� � ��������� �����
4. **Dialogue �� ����� � Quest System!**

### ������ 3: ����� ������ �� �����

**Quest Data Asset:**
```
Objective ProgressGate:
  RequireTimeOfDay: true
  RequiredTimeStart: 20
  RequiredTimeEnd: 6
```

**Time System:**
```cpp
// ������ ���
UTimeSystemGameEventEmitter::OnHourChanged(CurrentHour);
```

**��� ����������:**
1. Time System ��������� `Time.Event.HourChanged`
2. Quest System ��������� ������� ����� �������
3. ���� ����������� ������ �����
4. **Time System �� ����� � �������!**

## ������������

? **��� ������ ������������** � ������� �� ����� ���� � �����  
? **��������** � ����� ��������� ����� �������  
? **��������� ����� Data Assets** � �� ����� ������ ���  
? **Blueprint-friendly** � �� �������� �� Blueprint  
? **����������������** � �������� � ����� ����������� ������  
? **�������������** � ������� ����� ����������� ��������  

## Dependency Direction

```
GameEventBus (Core, no dependencies)
    ?
    ??? QuestSystem (depends on EventBus optionally)
    ??? DialogueSystem (depends on EventBus optionally)
 ??? InventorySystem (depends on EventBus optionally)
```

**�����:** ��� ����������� ������������! ������� �������� � ��� EventBus.

## �������� ���������

```
Plugins/
??? GameEventBus/        ? ����� ������
?   ??? Source/GameEventBus/
?   ?   ??? Public/
?   ?   ?   ??? GameEventBusSubsystem.h    ? ����������� bus
?   ?   ?   ??? GameEventEffectRegistry.h  ? Registry
?   ?   ?   ??? GameEventBusLibrary.h      ? BP helpers
?   ?   ?   ??? GameEventIntegrationHelpers.h
?   ?   ?   ??? Integration/
?   ?   ?       ??? TimeSystemGameEventEmitter.h
?   ?   ??? Private/
?   ?   ?   ??? (implementations)
?   ?   ??? GameEventBus.Build.cs
?   ??? Documentation/
?   ?   ??? IntegrationGuide.md
?   ?   ??? GameplayTags.md
?   ??? README.md
?   ??? GameEventBus.uplugin
?
??? QuestSystem/
?   ??? Source/QuestSystemRuntime/
?       ??? Public/Integration/
?       ?   ??? QuestEventBridge.h   ? ����� ����
?       ??? Private/Integration/
?           ??? QuestEventBridge.cpp
?
??? DialogueSystem/
?   ??? Source/DialogueSystemCore/
?       ??? Public/Effects/
?       ?   ??? DialogueEffect_EmitGameEvent.h  ? ����� ����
?       ??? Private/Effects/
?           ??? DialogueEffect_EmitGameEvent.cpp
?
??? AdvancedInventorySystem/
    ??? Source/InventorySystemCore/
        ??? Public/Integration/
        ?   ??? InventoryGameEventBridge.h     ? ����� ����
 ??? Private/Integration/
        ??? InventoryGameEventBridge.cpp
```

## �������������

### ��� ��������� (����� Blueprint)

1. **��������� ��� ������ ����:**
   ```
   Event BeginPlay (GameMode) ? Setup All Integrations
   ```

2. **�������� ������:**
   ```
   Quest Data Asset:
     Objective:
       EventTag: "Inventory.Event.ItemAcquired"
     ItemId: "Gold"
       Count: 100
   ```

3. **������ ��������� �����:**
   ```
   Dialogue Node ? Add Effect ? Emit Game Event
     EventTag: "Quest.Event.Start"
     StringParam: "MyQuestId"
   ```

4. **������!** �� �������� �������������.

### ��� ������������ (C++)

1. **������� �������:**
   ```cpp
   #if __has_include("GameEventBusSubsystem.h")
       #include "GameEventBusLibrary.h"
  UGameEventBusLibrary::EmitItemEvent(this, EventTag, ItemId, Count, Actor);
   #endif
```

2. **�������� �� �������:**
   ```cpp
   UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
   EventBus->SubscribeToEventNative(Tag, 
       FGameEventNativeDelegate::FDelegate::CreateUObject(this, &MyClass::OnEvent)
 );
   ```

3. **����������� �������:**
   ```cpp
   UGameEventEffectRegistry* Registry = EventBus->GetEffectRegistry();
   Registry->RegisterEffect(EffectTag, UMyEffect::StaticClass(), "MySystem");
   ```

## ������� (GameplayTags)

### ���������
- `Inventory.Event.*` � ������� ���������
- `Dialogue.Event.*` � ������� ��������
- `Quest.Event.*` � ������� �������
- `Time.Event.*` � ������� �������
- `NPC.Event.*` � ������� NPC
- `Location.Event.*` � ������� �������
- `Combat.Event.*` � ������� ���
- `Weather.Event.*` � ������� ������

### �������
```
Inventory.Event.ItemAcquired    � ������� �������
Dialogue.Event.NodeReached      � ���������� ���� �������
Quest.Event.ObjectiveCompleted  � ���� ������ ���������
Time.Event.HourChanged     � ��������� ���
NPC.Event.AffinityChanged       � ���������� �������� � NPC
```

## Best Practices

### ? DO
- ����������� �������� ����� (System.Event.Action)
- ���������� ������� ����� ���������� ��������
- ����������� `SetupAllIntegrations()` ��� ������ ����
- ������������ ������/������� ����� Data Assets

### ? DON'T
- �� ���������� ������ ����������� ����� ���������
- �� ���������� ������� ������� �����
- �� ��������� ������������ ��� Destroy
- �� ����������� EventBus ��� tick-based ������

## ����������

### ���������� ����� �������

1. **�������� Bridge ��� ����� �������:**
   ```cpp
   class UMySystemBridge : public UObject
   {
       void EmitMyEvent(FName Param) {
        UGameEventBusLibrary::EmitSimpleEvent(this, MyEventTag);
       }
   };
   ```

2. **���������� ������� ��� ������ ���������:**
   ```cpp
   UMySystemBridge::EmitCustomEvent(this, EventData);
   ```

3. **Quest/Dialogue ������� ������������� ������� �������!**

### ���������� ������ �������

1. **�������� ����� �������:**
   ```cpp
   UCLASS()
   class UMyCustomEffect : public UObject, public IGenericGameEffect
{
       virtual bool ExecuteEffect_Implementation(UObject* Context, const FGameEventPayload& Payload) override {
           // Your logic
      return true;
       }
   };
   ```

2. **��������������� ���:**
   ```cpp
   Registry->RegisterEffect(EffectTag, UMyCustomEffect::StaticClass(), "MySystem");
   ```

3. **����������� � �������/��������!**

## ����

�� ��������:

1. ? **������ �����������** ��� ������ ������������
2. ? **������� ���������** ����� Data Assets
3. ? **Blueprint-friendly** �����������
4. ? **��������������** ������� ��� ������ ���������� ��������
5. ? **������������** � ������� �������������

������ ���� ������� (������, �������, ���������, �����, NPC) ����� **�������� �����������������** ��� ������ ���� � �����!

**�� ������������� ����� Data Assets � ����������� �� �����! ??**
