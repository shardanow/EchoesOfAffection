# Event-Driven Architecture — Краткое резюме

## Что было сделано

Создана **Event-Driven архитектура** для связывания игровых систем без жёстких зависимостей.

## Основные компоненты

### 1. GameEventBus Plugin (Core)
- **GameEventBusSubsystem** — центральный диспетчер событий
- **GameEventEffectRegistry** — регистрация эффектов и условий
- **GameEventBusLibrary** — Blueprint helpers
- **GameEventIntegrationHelpers** — упрощённая настройка

### 2. Bridges (Интеграции)
- **QuestEventBridge** — Quest System ? EventBus (автоматическая подписка)
- **InventoryGameEventBridge** — Inventory ? EventBus (эмиссия событий)
- **TimeSystemGameEventEmitter** — Time System ? EventBus (эмиссия событий)

### 3. Effects (Примеры)
- **DialogueEffect_EmitGameEvent** — эмиссия событий из диалогов
- Опциональные зависимости в Build.cs

### 4. Documentation
- **IntegrationGuide.md** — полное руководство
- **GameplayTags.md** — список и настройка тегов
- **README.md** — обзор и quick start

## Архитектура

```
???????????????????????????????????????????????
?         GameEventBusSubsystem           ?
?  • События через GameplayTags         ?
?  • Pub/Sub паттерн           ?
?  • Effect/Condition Registry   ?
???????????????????????????????????????????????
 ?
    ????????????????????????????????????????
    ?         ?        ?        ?   ?
????????? ???????? ????????? ????????? ?????????
? Quest ? ?Dialog? ? Invent? ? Time  ? ?  NPC  ?
?System ? ?System? ? System? ?System ? ?System ?
????????? ???????? ????????? ????????? ?????????
```

## Как это работает

### Пример 1: Квест "Собрать яблоки"

**Quest Data Asset:**
```
Objective:
  EventTag: "Inventory.Event.ItemAcquired"
  ItemId: "Apple"
  Count: 5
```

**Inventory System:**
```cpp
// Когда игрок получает яблоко
UInventoryGameEventBridge::EmitItemAcquiredEvent(this, "Apple", 1, Player);
```

**Что происходит:**
1. Inventory эмитирует событие `Inventory.Event.ItemAcquired`
2. GameEventBus получает событие
3. QuestEventBridge транслирует его в QuestEventBus
4. Quest System обновляет прогресс
5. **Никаких зависимостей между Inventory и Quest!**

### Пример 2: Диалог запускает квест

**Dialogue Data Asset:**
```
Node Effects:
  - EmitGameEvent:
      EventTag: "Quest.Event.Start"
    StringParam: "SaveVillage"
```

**Что происходит:**
1. Dialogue Effect эмитирует событие
2. GameEventBus передаёт событие
3. QuestEventBridge получает и запускает квест
4. **Dialogue не знает о Quest System!**

### Пример 3: Время влияет на квест

**Quest Data Asset:**
```
Objective ProgressGate:
  RequireTimeOfDay: true
  RequiredTimeStart: 20
  RequiredTimeEnd: 6
```

**Time System:**
```cpp
// Каждый час
UTimeSystemGameEventEmitter::OnHourChanged(CurrentHour);
```

**Что происходит:**
1. Time System эмитирует `Time.Event.HourChanged`
2. Quest System проверяет условия через события
3. Цель обновляется только ночью
4. **Time System не знает о квестах!**

## Преимущества

? **Нет жёстких зависимостей** — системы не знают друг о друге  
? **Гибкость** — легко добавлять новые системы  
? **Настройка через Data Assets** — не нужно писать код  
? **Blueprint-friendly** — всё доступно из Blueprint  
? **Масштабируемость** — работает с любым количеством систем  
? **Тестируемость** — системы можно тестировать отдельно  

## Dependency Direction

```
GameEventBus (Core, no dependencies)
    ?
    ??? QuestSystem (depends on EventBus optionally)
    ??? DialogueSystem (depends on EventBus optionally)
 ??? InventorySystem (depends on EventBus optionally)
```

**Важно:** Все зависимости ОПЦИОНАЛЬНЫЕ! Системы работают и без EventBus.

## Файловая структура

```
Plugins/
??? GameEventBus/        ? НОВЫЙ плагин
?   ??? Source/GameEventBus/
?   ?   ??? Public/
?   ?   ?   ??? GameEventBusSubsystem.h    ? Центральный bus
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
?       ?   ??? QuestEventBridge.h   ? НОВЫЙ файл
?       ??? Private/Integration/
?           ??? QuestEventBridge.cpp
?
??? DialogueSystem/
?   ??? Source/DialogueSystemCore/
?       ??? Public/Effects/
?       ?   ??? DialogueEffect_EmitGameEvent.h  ? НОВЫЙ файл
?       ??? Private/Effects/
?           ??? DialogueEffect_EmitGameEvent.cpp
?
??? AdvancedInventorySystem/
    ??? Source/InventorySystemCore/
        ??? Public/Integration/
        ?   ??? InventoryGameEventBridge.h     ? НОВЫЙ файл
 ??? Private/Integration/
        ??? InventoryGameEventBridge.cpp
```

## Использование

### Для дизайнера (через Blueprint)

1. **Настройка при старте игры:**
   ```
   Event BeginPlay (GameMode) ? Setup All Integrations
   ```

2. **Создание квеста:**
   ```
   Quest Data Asset:
     Objective:
       EventTag: "Inventory.Event.ItemAcquired"
     ItemId: "Gold"
       Count: 100
   ```

3. **Диалог запускает квест:**
   ```
   Dialogue Node ? Add Effect ? Emit Game Event
     EventTag: "Quest.Event.Start"
     StringParam: "MyQuestId"
   ```

4. **Готово!** Всё работает автоматически.

### Для программиста (C++)

1. **Эмиссия события:**
   ```cpp
   #if __has_include("GameEventBusSubsystem.h")
       #include "GameEventBusLibrary.h"
  UGameEventBusLibrary::EmitItemEvent(this, EventTag, ItemId, Count, Actor);
   #endif
```

2. **Подписка на события:**
   ```cpp
   UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
   EventBus->SubscribeToEventNative(Tag, 
       FGameEventNativeDelegate::FDelegate::CreateUObject(this, &MyClass::OnEvent)
 );
   ```

3. **Регистрация эффекта:**
   ```cpp
   UGameEventEffectRegistry* Registry = EventBus->GetEffectRegistry();
   Registry->RegisterEffect(EffectTag, UMyEffect::StaticClass(), "MySystem");
   ```

## События (GameplayTags)

### Категории
- `Inventory.Event.*` — события инвентаря
- `Dialogue.Event.*` — события диалогов
- `Quest.Event.*` — события квестов
- `Time.Event.*` — события времени
- `NPC.Event.*` — события NPC
- `Location.Event.*` — события локаций
- `Combat.Event.*` — события боя
- `Weather.Event.*` — события погоды

### Примеры
```
Inventory.Event.ItemAcquired    — получен предмет
Dialogue.Event.NodeReached      — достигнута нода диалога
Quest.Event.ObjectiveCompleted  — цель квеста выполнена
Time.Event.HourChanged     — изменился час
NPC.Event.AffinityChanged       — изменилась близость с NPC
```

## Best Practices

### ? DO
- Используйте иерархию тегов (System.Event.Action)
- Эмитируйте события ПОСЛЕ выполнения действия
- Используйте `SetupAllIntegrations()` при старте игры
- Настраивайте квесты/диалоги через Data Assets

### ? DON'T
- Не создавайте прямые зависимости между плагинами
- Не эмитируйте события слишком часто
- Не забывайте отписываться при Destroy
- Не используйте EventBus для tick-based логики

## Расширение

### Добавление новой системы

1. **Создайте Bridge для вашей системы:**
   ```cpp
   class UMySystemBridge : public UObject
   {
       void EmitMyEvent(FName Param) {
        UGameEventBusLibrary::EmitSimpleEvent(this, MyEventTag);
       }
   };
   ```

2. **Эмитируйте события при важных действиях:**
   ```cpp
   UMySystemBridge::EmitCustomEvent(this, EventData);
   ```

3. **Quest/Dialogue системы автоматически получат события!**

### Добавление нового эффекта

1. **Создайте класс эффекта:**
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

2. **Зарегистрируйте его:**
   ```cpp
   Registry->RegisterEffect(EffectTag, UMyCustomEffect::StaticClass(), "MySystem");
   ```

3. **Используйте в квестах/диалогах!**

## Итог

Вы получили:

1. ? **Гибкую архитектуру** без жёстких зависимостей
2. ? **Простую настройку** через Data Assets
3. ? **Blueprint-friendly** инструменты
4. ? **Масштабируемую** систему для любого количества плагинов
5. ? **Документацию** и примеры использования

Теперь ваши системы (квесты, диалоги, инвентарь, время, NPC) могут **свободно взаимодействовать** без знания друг о друге!

**Всё настраивается через Data Assets — программист не нужен! ??**
