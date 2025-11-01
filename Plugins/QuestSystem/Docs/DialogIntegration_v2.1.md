# Quest System ? Dialog System Integration Guide

## ? Чистая Event-driven интеграция БЕЗ хардкода

---

## ?? Содержание

1. [Обзор интеграции](#обзор-интеграции)
2. [Quest Dialogue Effects](#quest-dialogue-effects)
3. [Использование в Dialog System](#использование-в-dialog-system)
4. [Примеры](#примеры)
5. [Blueprint Setup](#blueprint-setup)

---

## 1?? Обзор интеграции

### Архитектура

```
Dialog System (независимый)
  ?
  UDialogueNode
  ?? Effects: TArray<UDialogueEffect*>
  ?? Execute Effects при обработке ноды
      ?
Quest System (регистрирует свои эффекты)
  ?? UQuestDialogueEffect_StartQuest
  ?? UQuestDialogueEffect_CompleteObjective
  ?? UQuestDialogueEffect_EmitEvent
```

**Преимущества**:
- ? **Нет хардкода** - системы независимы
- ? **Event-driven** - слабая связанность
- ? **Расширяемый** - можно добавлять новые эффекты
- ? **Blueprint-friendly** - работает из редактора

---

## 2?? Quest Dialogue Effects

### UQuestDialogueEffect_StartQuest

**Назначение**: Запускает квест когда dialogue node обрабатывается.

**Свойства**:
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName QuestId; // ID квеста для запуска

UPROPERTY(EditAnywhere, BlueprintReadWrite)
AActor* QuestAcceptor; // (Optional) Кто принимает квест
```

**Использование**:
- Добавить в DialogueNode.Effects
- Установить QuestId
- Квест запустится автоматически при обработке ноды

---

### UQuestDialogueEffect_CompleteObjective

**Назначение**: Завершает objective квеста.

**Свойства**:
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName QuestId; // ID квеста

UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName ObjectiveId; // ID objective для завершения
```

**Использование**:
- Полезно для "talk to NPC" objectives
- Автоматическое завершение при диалоге

---

### UQuestDialogueEffect_EmitEvent

**Назначение**: Отправляет quest event (для условных objectives).

**Свойства**:
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName EventType; // "NpcTalked", "ItemAcquired", etc.

UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName NpcId; // Для NpcTalked

UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName ItemId; // Для ItemAcquired/ItemUsed

UPROPERTY(EditAnywhere, BlueprintReadWrite)
int32 Count; // Количество (для счётчиков)
```

**Использование**:
- Для event-based objectives
- Квест подписывается на событие и автоматически обновляется

---

## 3?? Использование в Dialog System

### В Dialog Asset (Editor)

#### Шаг 1: Открыть DialogueNode

1. Откройте ваш **DialogueDataAsset**
2. Выберите нужную **DialogueNode**
3. В **Details** панели найдите **Effects**

#### Шаг 2: Добавить Quest Effect

```
Effects:
  [0]: UQuestDialogueEffect_StartQuest
    ?? Quest Id: "Q_FirstMeeting"
    ?? Quest Acceptor: [None] ? Авто из контекста
```

#### Шаг 3: Сохранить

**Готово!** При обработке этой ноды квест запустится автоматически.

---

### В Blueprint DialogueNode

```blueprint
Event OnNodeEntered
?? For Each (Effects)
?   ?? Cast to QuestDialogueEffect_StartQuest
?       ?? Execute (WorldContext: Self)
?? [Quest started automatically]
```

---

## 4?? Примеры

### Пример 1: Простой квест при встрече

**Сценарий**: Игрок впервые говорит с NPC ? автоматически начинается квест.

**DialogueNode настройка**:
```
Node: "FirstMeeting"
?? Speaker: "NPC_Elder"
?? DialogueText: "Welcome, traveler! I need your help."
?? Effects:
    ?? [0] UQuestDialogueEffect_StartQuest
 ?? QuestId: "Q_HelpElder"
```

**QuestAsset настройка**:
```
Quest: QA_HelpElder
?? Quest ID: Q_HelpElder
?? Title: "Help the Elder"
?? Phase 0:
    ?? Objective 0:
        ?? Objective ID: Obj_TalkMore
        ?? Title: "Talk to elder again"
        ?? Condition:
     ?? Type: NpcTalked
            ?? String Param: NPC_Elder
?? Int Param: 2 ? Нужно 2 раза поговорить
```

---

### Пример 2: Завершение objective через диалог

**Сценарий**: У игрока квест "Собрать информацию", один из objectives - "Поговорить с торговцем".

**DialogueNode настройка**:
```
Node: "MerchantInfo"
?? Speaker: "NPC_Merchant"
?? DialogueText: "I saw suspicious people near the forest..."
?? Condition: Quest.Active("Q_GatherInfo") ? Только если квест активен
?? Effects:
    ?? [0] UQuestDialogueEffect_CompleteObjective
     ?? QuestId: "Q_GatherInfo"
        ?? ObjectiveId: "Obj_TalkMerchant"
```

**QuestAsset настройка**:
```
Quest: QA_GatherInfo
?? Quest ID: Q_GatherInfo
?? Phase 0:
 ?? Objective 0: Obj_TalkMerchant
    ?   ?? Title: "Talk to merchant"
    ?   ?? Style: EventBased
    ?? Objective 1: Obj_TalkGuard
   ?? Title: "Talk to guard"
     ?? Style: EventBased
```

---

### Пример 3: Event-based прогресс

**Сценарий**: Игрок отдаёт предмет NPC ? quest objective обновляется.

**DialogueNode настройка**:
```
Node: "GiveItem"
?? Speaker: "NPC_Blacksmith"
?? DialogueText: "Ah, the iron ore! Thank you!"
?? Condition: HasItem("IronOre")
?? Effects:
    ?? [0] UDialogueEffect_ModifyInventory
    ?   ?? ItemId: "IronOre"
    ?   ?? DeltaCount: -1 ? Убрать предмет
    ?? [1] UQuestDialogueEffect_EmitEvent
        ?? EventType: "ItemUsed"
        ?? ItemId: "IronOre"
        ?? Count: 1
```

**QuestAsset настройка**:
```
Quest: QA_HelpBlacksmith
?? Quest ID: Q_HelpBlacksmith
?? Phase 0:
    ?? Objective 0:
        ?? Objective ID: Obj_BringOre
        ?? Title: "Bring iron ore to blacksmith"
        ?? Style: EventBased
        ?? Condition:
 ?? Type: ItemUsed
            ?? String Param: IronOre
            ?? Int Param: 1
```

---

## 5?? Blueprint Setup

### В DialogueRunner

**Если используете custom DialogueRunner**:

```blueprint
Event OnNodeEntered (Node: UDialogueNode)
?? Get Effects (from Node)
?? For Each (Effect)
?   ?? Cast to QuestDialogueEffect_StartQuest
?   ?   ?? Success? ? Execute (WorldContext: Self)
?   ?? Cast to QuestDialogueEffect_CompleteObjective
?   ?   ?? Success? ? Execute (WorldContext: Self)
?   ?? Cast to QuestDialogueEffect_EmitEvent
?       ?? Success? ? Execute (WorldContext: Self, Instigator: Player)
?? Continue dialogue...
```

**Если используете встроенный DialogueRunner**:
- Ничего делать не нужно!
- Effects выполняются автоматически через `UDialogueEffectExecutor`

---

## 6?? Расширение системы

### Создание custom Quest Effect

#### Шаг 1: Создать класс

```cpp
// MyQuestEffect.h
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "My Quest Effect"))
class UMyQuestDialogueEffect : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName MyParameter;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void Execute(UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "Quest")
    FText GetDisplayText() const;
};
```

#### Шаг 2: Реализовать Execute()

```cpp
// MyQuestEffect.cpp
void UMyQuestDialogueEffect::Execute(UObject* WorldContextObject)
{
    // Получить Quest Subsystem
    UQuestSubsystem* QuestSys = UQuestSubsystem::Get(WorldContextObject);
    if (!QuestSys) return;

    // Ваша логика
    // ...
}
```

#### Шаг 3: Использовать в Dialog System

```
DialogueNode ? Effects ? Add:
  ?? UMyQuestDialogueEffect
      ?? MyParameter: "Value"
```

---

## 7?? Troubleshooting

### ? Квест не запускается

**Решение**:
1. Проверьте `QuestId` совпадает с Quest Asset
2. Проверьте Quest Subsystem инициализирован
3. Включите Output Log ? должно быть:
   ```
   LogTemp: Quest 'Q_HelpElder' started from dialogue
   ```

---

### ? Objective не завершается

**Решение**:
1. Проверьте квест активен (`IsQuestActive()`)
2. Проверьте `QuestId` и `ObjectiveId` правильные
3. Проверьте objective не уже завершён

---

### ? Events не работают

**Решение**:
1. Проверьте `EventType` правильный ("NpcTalked", "ItemAcquired", etc.)
2. Проверьте `NpcId` или `ItemId` совпадают с Quest Condition
3. Проверьте Event Bus инициализирован

---

## 8?? Best Practices

### ? DO:

1. **Используйте EmitEvent** для гибкости:
   ```
   UQuestDialogueEffect_EmitEvent вместо CompleteObjective
   ```
   Квест сам решит что делать с событием.

2. **Проверяйте условия** в DialogueNode:
   ```
 Condition: Quest.Active("Q_MyQuest")
   ```
   Чтобы эффект срабатывал только когда нужно.

3. **Логируйте** в custom effects:
   ```cpp
   UE_LOG(LogTemp, Log, TEXT("MyEffect executed"));
   ```

### ? DON'T:

1. **Не хардкодьте** Quest System в Dialog System
2. **Не дублируйте логику** - используйте события
3. **Не забывайте** про null-checks

---

## ?? Заключение

**Quest ? Dialog интеграция через Effects**:
- ? Чистая архитектура (no hardcode)
- ? Event-driven (слабая связанность)
- ? Blueprint-friendly (легко использовать)
- ? Расширяемая (легко добавлять эффекты)

**Системы независимы, но отлично работают вместе!**

---

**Quest System v2.1 + Dialog System - Perfect Integration!** ?
