# Резюме: Event-Driven Архитектура для UE5

## Что было создано

Полноценная **Event-Driven архитектура** для связывания игровых систем без жёстких зависимостей.

## ?? Решённая проблема

**Было:**
- Quest System жёстко зависит от Dialogue System
- Dialogue System зависит от Inventory System
- Inventory зависит от Quest System
- Циклические зависимости
- Невозможно добавить новую систему без изменения кода
- Дизайнер не может настроить взаимодействие без программиста

**Стало:**
- Все системы независимы
- Связь через события (GameplayTags)
- Нулевые зависимости между плагинами
- Дизайнер настраивает всё через Data Assets
- Легко добавлять новые системы

## ?? Созданные компоненты

### 1. GameEventBus Plugin (Новый!)
**Местоположение:** `Plugins/GameEventBus/`

**Файлы:**
- `GameEventBusSubsystem.h/.cpp` — центральный event bus
- `GameEventEffectRegistry.h/.cpp` — регистрация эффектов/условий
- `GameEventBusLibrary.h/.cpp` — Blueprint helpers
- `GameEventIntegrationHelpers.h/.cpp` — упрощённая настройка
- `TimeSystemGameEventEmitter.h/.cpp` — интеграция с Time System

**Функции:**
- Pub/Sub система через GameplayTags
- Регистрация кастомных эффектов
- Регистрация условий для проверки
- Blueprint функции для эмиссии событий
- История событий для отладки

### 2. Quest System Integration
**Местоположение:** `Plugins/QuestSystem/Source/QuestSystemRuntime/`

**Новые файлы:**
- `Integration/QuestEventBridge.h/.cpp` — автоматическая подписка на события

**Изменения:**
- Опциональная зависимость на GameEventBus в Build.cs
- Автоматическая инициализация Bridge в QuestSubsystem
- Поддержка generic events через GameplayTags

**Возможности:**
- Квесты автоматически получают события из других систем
- Поддержка условий по времени, погоде, предметам и т.д.
- Всё настраивается через Quest Data Assets

### 3. Dialogue System Integration
**Местоположение:** `Plugins/DialogueSystem/Source/DialogueSystemCore/`

**Новые файлы:**
- `Effects/DialogueEffect_EmitGameEvent.h/.cpp` — эффект для эмиссии событий
- `Effects/DialogueEffect_EmitMultipleGameEvents.h` — множественные события

**Изменения:**
- Опциональная зависимость на GameEventBus
- Новые эффекты для нод диалога

**Возможности:**
- Диалоги могут эмитировать события для квестов
- Диалоги могут эмитировать события для инвентаря
- Настройка через Dialogue Data Assets

### 4. Inventory System Integration
**Местоположение:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/`

**Новые файлы:**
- `Integration/InventoryGameEventBridge.h/.cpp` — эмиссия событий инвентаря

**Функции:**
- `EmitItemAcquiredEvent` — получен предмет
- `EmitItemUsedEvent` — использован предмет
- `EmitItemCraftedEvent` — создан предмет
- `EmitItemGiftedEvent` — подарен предмет NPC
- `EmitCustomInventoryEvent` — кастомные события

**Возможности:**
- Квесты автоматически отслеживают получение предметов
- Настройка без кода

### 5. Документация
**Местоположение:** `Plugins/GameEventBus/Documentation/`

**Файлы:**
- `IntegrationGuide.md` — полное руководство по интеграции
- `GameplayTags.md` — список и настройка GameplayTags
- `README.md` — обзор и quick start
- `ARCHITECTURE.md` — архитектурное описание

## ?? Как использовать

### Для дизайнера (через Data Assets)

#### 1. Квест "Собрать 5 яблок"

Создайте **Quest Data Asset:**

```
Objective:
  EventTag: "Inventory.Event.ItemAcquired"
  ItemId: "Apple"
  Count: 5
```

**Готово!** Когда игрок соберёт 5 яблок, квест обновится автоматически.

#### 2. Диалог запускает квест

Создайте **Dialogue Data Asset:**

```
Node:
  Text: "Помоги мне спасти деревню!"
  
  Effects:
    - Emit Game Event:
        EventTag: "Quest.Event.Start"
        StringParam: "SaveVillage"
```

**Готово!** Когда игрок достигнет этой ноды, квест запустится.

#### 3. Квест зависит от времени

```
Objective Progress Gate:
  Require Time Of Day: true
  Required Time Start: 20
  Required Time End: 6
```

**Готово!** Цель будет обновляться только ночью.

### Для программиста (C++)

#### 1. Эмиссия события

```cpp
// Простое событие
UGameEventBusLibrary::EmitSimpleEvent(this, EventTag);

// Событие с предметом
UGameEventBusLibrary::EmitItemEvent(this, EventTag, ItemId, Count, Actor);

// Или напрямую через инвентарь
UInventoryGameEventBridge::EmitItemAcquiredEvent(this, ItemId, Count, Player);
```

#### 2. Подписка на события

```cpp
// Получить subsystem
UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);

// Подписаться на событие
FDelegateHandle Handle = EventBus->SubscribeToEventNative(
    EventTag,
    FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UMyClass::OnEvent)
);

// Обработка события
void UMyClass::OnEvent(const FGameEventPayload& Payload)
{
    // Your logic
}
```

#### 3. Регистрация эффекта

```cpp
// Создайте класс эффекта
UCLASS()
class UMyEffect : public UObject, public IGenericGameEffect
{
    virtual bool ExecuteEffect_Implementation(UObject* Context, const FGameEventPayload& Payload) override
    {
   // Your effect logic
        return true;
    }
};

// Зарегистрируйте его
UGameEventEffectRegistry* Registry = EventBus->GetEffectRegistry();
Registry->RegisterEffect(EffectTag, UMyEffect::StaticClass(), "MySystem");
```

## ?? Примеры сценариев

### Сценарий 1: Квест "Принеси 10 железа кузнецу ночью"

**Quest Data Asset:**

```
Objective 1: Collect Iron
  EventTag: "Inventory.Event.ItemAcquired"
  ItemId: "Iron"
  Count: 10

Objective 2: Talk to Blacksmith
  EventTag: "Dialogue.Event.NodeReached"
  NpcId: "Blacksmith"
  DialogNodeId: "GiveIron"
  
  Progress Gate:
    Require Time Of Day: true
    Required Time Start: 20
    Required Time End: 6
```

**Что происходит:**
1. Игрок собирает железо ? Inventory эмитирует события ? Цель 1 прогрессирует
2. Игрок приходит к кузнецу ночью ? Dialogue эмитирует событие ? Цель 2 выполняется
3. Quest System выдаёт награды

**Никакого кода!** Всё настроено через Data Assets.

### Сценарий 2: NPC даёт квест только при высокой близости

**Dialogue Data Asset:**

```
Node:
  Text: "У меня есть для тебя важное задание..."
  
  Conditions:
    - Evaluate Condition:
        ConditionTag: "Condition.NPC.AffinityAbove"
        StringParam: "Blacksmith"
        FloatParam: 50.0

  Effects:
    - Emit Game Event:
        EventTag: "Quest.Event.Start"
        StringParam: "SpecialQuest"
```

**Что происходит:**
1. Система проверяет условие через Effect Registry
2. Если близость > 50 ? нода доступна
3. При выборе опции эмитируется событие
4. Quest System запускает квест

### Сценарий 3: Предмет даёт квест при использовании

**Item Data Asset:**

```
Item: Mysterious Letter

On Use Effects:
  - Emit Game Event:
      EventTag: "Quest.Event.Start"
      StringParam: "InvestigateLetter"
```

**Что происходит:**
1. Игрок использует письмо
2. Inventory эмитирует событие
3. Quest System запускает квест расследования

## ?? Архитектура

```
      ????????????????????????????????????
            ?   GameEventBusSubsystem          ?
     ?           ?
        ?  • Subscribe/Emit events         ?
                  ?  • GameplayTag routing           ?
       ?  • Effect/Condition Registry     ?
          ?  • Event history & debugging     ?
    ????????????????????????????????????
           ?
      ???????????????????????????????????????????????????
      ?    ?  ?
?????????????          ?????????????????        ???????????????
?  Quest    ?          ?   Dialogue    ?        ?  Inventory  ?
?  System   ?          ?    System     ?        ?System    ?
?           ?        ?     ?     ?             ?
? + Bridge  ?       ? + Effect      ? ? + Bridge    ?
?   (Listen)??   (Emit)      ?      ?   (Emit)    ?
?????????????    ?????????????????        ???????????????

     ?        ?           ?
     ???????????????????????????????????????????????????
         ?
???????????????????????
       ?    Time System      ?
           ?              ?
        ?  + Emitter          ?
   ?    (Emit events)    ?
              ???????????????????????
```

## ? Преимущества

1. **Нет жёстких зависимостей**
   - Quest System не знает о Dialogue
   - Dialogue не знает о Quest
   - Inventory не знает о Quest
   - Все общаются через события

2. **Гибкость**
   - Легко добавить новую систему
   - Легко добавить новое событие
   - Легко добавить новый эффект
   - Изменения в одной системе не ломают другие

3. **Настройка без кода**
   - Дизайнер создаёт Data Assets
   - Настраивает события через GUI
   - Не нужен программист

4. **Blueprint-friendly**
   - Все функции доступны в Blueprint
   - Простые ноды для эмиссии событий
   - Визуальная настройка

5. **Масштабируемость**
   - Работает с любым количеством систем
   - Производительность не зависит от количества плагинов
   - Native delegate система UE

6. **Отладка**
   - Логирование событий
   - История событий
   - Debug функции

## ?? Ключевые классы

| Класс | Назначение |
|-------|-----------|
| `UGameEventBusSubsystem` | Центральный event bus |
| `UGameEventEffectRegistry` | Регистрация эффектов/условий |
| `UGameEventBusLibrary` | Blueprint helpers |
| `UGameEventIntegrationHelpers` | Упрощённая настройка |
| `UQuestEventBridge` | Quest ? EventBus интеграция |
| `UInventoryGameEventBridge` | Inventory ? EventBus эмиссия |
| `UDialogueEffect_EmitGameEvent` | Dialogue ? EventBus эффект |
| `UTimeSystemGameEventEmitter` | Time ? EventBus эмиссия |

## ?? GameplayTags (примеры)

### Events
```
Inventory.Event.ItemAcquired
Inventory.Event.ItemUsed
Inventory.Event.ItemCrafted

Dialogue.Event.Started
Dialogue.Event.NodeReached
Dialogue.Event.ChoiceMade

Quest.Event.Started
Quest.Event.Completed
Quest.Event.ObjectiveCompleted

Time.Event.HourChanged
Time.Event.DayChanged

NPC.Event.AffinityChanged
NPC.Event.TalkStarted

Location.Event.Entered
Combat.Event.EnemyKilled
Weather.Event.Changed
```

### Effects
```
Effect.Quest.Start
Effect.Quest.Complete

Effect.Dialogue.Start
Effect.Dialogue.UnlockBranch

Effect.Inventory.GiveItem
Effect.NPC.ModifyAffinity
```

### Conditions
```
Condition.Quest.IsActive
Condition.Inventory.HasItem
Condition.Time.InRange
Condition.NPC.AffinityAbove
```

## ?? Следующие шаги

1. **Настройте GameplayTags** в Project Settings
2. **Вызовите Setup All Integrations** в GameMode BeginPlay
3. **Добавьте эмиссию событий** в ваши системы (Inventory, Time, etc)
4. **Создавайте квесты/диалоги** через Data Assets
5. **Тестируйте!**

## ?? Документация

- **[Integration Guide](Documentation/IntegrationGuide.md)** — полное руководство
- **[GameplayTags](Documentation/GameplayTags.md)** — список тегов
- **[README](README.md)** — quick start
- **[ARCHITECTURE](ARCHITECTURE.md)** — архитектура

## ?? Итог

Вы получили:

? Полноценную **Event-Driven архитектуру**  
? **Независимые** плагины без циклических зависимостей  
? **Data-Driven** настройку через Data Assets  
? **Blueprint-friendly** инструменты  
? **Масштабируемую** систему  
? **Полную документацию**  

**Теперь ваши системы свободно взаимодействуют без хардкода! ??**

---

*Created with ?? for game designers and developers*
