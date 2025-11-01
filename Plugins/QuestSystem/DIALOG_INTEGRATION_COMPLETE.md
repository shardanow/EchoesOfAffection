# Quest System v2.1 - Dialog Integration Complete! ?

## ?? Что сделано

### ? Удалено

- ? `UQuestDialogWidget` - не нужен, Dialog System имеет свой UI
- ? Хардкод зависимостей между системами

### ? Добавлено

#### 1. Quest Dialogue Effects (чистая интеграция)

**Файлы**:
- `QuestDialogueEffects.h/.cpp` - эффекты для Dialog System

**Классы**:
```cpp
UQuestDialogueEffect_StartQuest
?? Запускает квест при обработке DialogueNode
?? Свойства: QuestId, QuestAcceptor

UQuestDialogueEffect_CompleteObjective
?? Завершает objective квеста
?? Свойства: QuestId, ObjectiveId

UQuestDialogueEffect_EmitEvent
?? Отправляет quest event (NpcTalked, ItemAcquired, etc.)
?? Свойства: EventType, NpcId, ItemId, Count
```

#### 2. Документация

- `DialogIntegration_v2.1.md` - полный гайд интеграции
- Обновлён `BeginnerGuide_v2.1_EditorOnly.md`

---

## ??? Архитектура интеграции

### Event-driven подход (БЕЗ хардкода)

```
????????????????????????????????????????????
? Dialog System (независимый плагин)       ?
????????????????????????????????????????????
? UDialogueNode                  ?
? ?? DialogueText             ?
? ?? Conditions      ?
? ?? Effects: TArray<UDialogueEffect*>     ? ? Абстракция
?     ?? UDialogueEffect_ModifyAffinity    ?
?     ?? UDialogueEffect_ModifyInventory   ?
?     ?? ... (расширяемо)   ?
????????????????????????????????????????????
         ?
         Execution через DialogueRunner
   ?
????????????????????????????????????????????
? Quest System регистрирует свои эффекты   ?
????????????????????????????????????????????
? UQuestDialogueEffect_StartQuest          ?
? ?? Execute(WorldContext)       ?
? ?? ? QuestSubsystem->StartQuest()    ?
?     ?
? UQuestDialogueEffect_CompleteObjective   ?
? ?? Execute(WorldContext)  ?
? ?? ? QuestSubsystem->CompleteObjective() ?
?             ?
? UQuestDialogueEffect_EmitEvent           ?
? ?? Execute(WorldContext, Actor)          ?
? ?? ? EventBus->EmitEvent()        ?
????????????????????????????????????????????
```

**Ключевые моменты**:
- ? **Слабая связанность** - системы не знают друг о друге напрямую
- ? **Расширяемость** - легко добавлять новые эффекты
- ? **Переиспользование** - Dialog Effects работают с любыми системами
- ? **Blueprint-friendly** - всё настраивается в редакторе

---

## ?? Использование (Quick Start)

### В Dialog System Editor

#### Шаг 1: Открыть DialogueNode

1. Откройте ваш **DialogueDataAsset**
2. Выберите нужную **DialogueNode**
3. В **Details** найдите **Effects**

#### Шаг 2: Добавить Quest Effect

```
Effects:
  ?? [Add] Quest Dialogue Effect - Start Quest
    ?? Quest Id: "Q_FirstMeeting"
      ?? Quest Acceptor: [None] ? Авто из контекста
```

#### Шаг 3: Сохранить

**Готово!** При обработке ноды квест запустится автоматически.

---

### Пример: Квест при первой встрече

```
DialogueNode: "FirstMeeting"
?? Speaker: "NPC_Elder"
?? DialogueText: "Welcome, traveler! I need your help."
?? Conditions: [None]
?? Effects:
    ?? [0] UQuestDialogueEffect_StartQuest
        ?? QuestId: "Q_HelpElder"
```

```
QuestAsset: QA_HelpElder
?? Quest ID: Q_HelpElder
?? Title: "Help the Elder"
?? Phase 0:
    ?? Objective 0:
  ?? Title: "Talk to elder again"
     ?? Condition:
?? Type: NpcTalked
  ?? String Param: NPC_Elder
   ?? Int Param: 2 ? Поговорить 2 раза
```

**Результат**:
1. Игрок говорит с NPC ? DialogueNode обрабатывается
2. Effect выполняется ? Quest "Q_HelpElder" запускается
3. Игрок говорит снова ? NpcTalked event ? Objective +1
4. Третий раз ? Objective completed ? Quest completed ?

---

## ?? Доступные Quest Effects

### 1. Start Quest

**Назначение**: Запускает квест.

**Параметры**:
- `Quest Id` - ID квеста (FName)
- `Quest Acceptor` - (Optional) Actor принимающий квест

**Пример**:
```
Effect: Start Quest
?? Quest Id: "Q_MainStory_01"
```

---

### 2. Complete Objective

**Назначение**: Завершает конкретный objective.

**Параметры**:
- `Quest Id` - ID квеста
- `Objective Id` - ID objective

**Пример**:
```
Effect: Complete Objective
?? Quest Id: "Q_GatherInfo"
?? Objective Id: "Obj_TalkMerchant"
```

**Когда использовать**: Для "talk to NPC" objectives.

---

### 3. Emit Quest Event

**Назначение**: Отправляет событие для quest objectives.

**Параметры**:
- `Event Type` - Тип ("NpcTalked", "ItemAcquired", "ItemUsed")
- `Npc Id` - ID NPC (для NpcTalked)
- `Item Id` - ID предмета (для Item events)
- `Count` - Количество

**Пример 1 - NPC Talked**:
```
Effect: Emit Quest Event
?? Event Type: "NpcTalked"
?? Npc Id: "NPC_Merchant"
?? Count: 1
```

**Пример 2 - Item Given**:
```
Effect: Emit Quest Event
?? Event Type: "ItemUsed"
?? Item Id: "IronOre"
?? Count: 1
```

---

## ?? Расширение системы

### Создание custom Quest Effect

```cpp
// MyCustomQuestEffect.h
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "My Quest Effect"))
class UMyCustomQuestEffect : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName MyParameter;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void Execute(UObject* WorldContextObject)
  {
        UQuestSubsystem* QuestSys = /* Get subsystem */;
        // Ваша логика...
    }

    UFUNCTION(BlueprintPure, Category = "Quest")
    FText GetDisplayText() const
    {
        return FText::FromString(TEXT("My Custom Effect"));
    }
};
```

**Использование**:
1. Compile project
2. Open DialogueNode
3. Add Effect ? **My Quest Effect**
4. Set parameters

---

## ?? Важные замечания

### Dialog System НЕ зависит от Quest System

```cpp
// ? WRONG (в Dialog System):
#include "QuestSubsystem.h" // НЕТ!

// ? CORRECT (в Quest System):
#include "Effects/DialogueEffectExecutor.h" // Только абстракция
```

Dialog System знает только про **UDialogueEffect** (абстракцию).

Quest System регистрирует свои конкретные эффекты.

### Порядок выполнения Effects

Effects выполняются **в порядке добавления**:

```
Effects:
  [0] ModifyInventory (give item)
  [1] StartQuest      (start quest)
  [2] EmitEvent    (notify quest)
```

Если нужен другой порядок - измените индексы.

---

## ?? Сравнение: Old vs New

### ? Старый подход (хардкод):

```cpp
// В Dialog System:
if (QuestSystem && QuestId != NAME_None)
{
    QuestSystem->StartQuest(QuestId); // Прямая зависимость!
}
```

**Проблемы**:
- ? Dialog зависит от Quest
- ? Нельзя отключить Quest без ошибок
- ? Сложно расширять
- ? Нарушение SRP

### ? Новый подход (event-driven):

```cpp
// Dialog System:
for (UDialogueEffect* Effect : Effects)
{
    Effect->Execute(Context); // Абстракция!
}

// Quest System (отдельно):
void UQuestDialogueEffect_StartQuest::Execute(Context)
{
    QuestSystem->StartQuest(QuestId); // Реализация
}
```

**Преимущества**:
- ? Слабая связанность
- ? Легко отключить Quest System
- ? Расширяемость (добавляй новые эффекты)
- ? Соблюдение SRP

---

## ?? Заключение v2.1

**Quest + Dialog интеграция**:
- ? **Event-driven** architecture
- ? **Нет хардкода** между системами
- ? **Blueprint-friendly** настройка
- ? **Расширяемая** через custom effects
- ? **Профессиональная** архитектура

**Две независимые системы работают вместе БЕЗ прямых зависимостей!**

---

**Документация**:
- `DialogIntegration_v2.1.md` - Полный гайд
- `BeginnerGuide_v2.1_EditorOnly.md` - Для новичков
- `ARCHITECTURE_V2.md` - Архитектура

**Quest System v2.1 - Professional Integration!** ?
