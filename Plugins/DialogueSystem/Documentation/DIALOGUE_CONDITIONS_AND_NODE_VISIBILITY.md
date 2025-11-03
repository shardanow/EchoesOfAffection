# Dialogue System: Условия и Видимость Нодов

## Оглавление
1. [Краткий Ответ](#краткий-ответ)
2. [Детальное Описание Системы Условий](#детальное-описание-системы-условий)
3. [Типы Условий (Condition Types)](#типы-условий-condition-types)
4. [Как Работает Фильтрация Нодов](#как-работает-фильтрация-нодов)
5. [Интеграция с GameEventBus](#интеграция-с-gameeventbus)
6. [Примеры Использования](#примеры-использования)
7. [Где Настраивать в Data Asset](#где-настраивать-в-data-asset)

---

## Краткий Ответ

### ? Да, система условий полностью реализована и работает!

**Возможности:**
- ? Создание условий для показа/скрытия нодов в Dialogue Data Asset (DA)
- ? Автоматическая фильтрация нодов на основе условий
- ? Поддержка GameEventBus для реакции на события
- ? DSL (Domain-Specific Language) для написания условий в текстовом виде
- ? Visual node-based conditions (через UDialogueCondition объекты)

---

## Детальное Описание Системы Условий

### Архитектура

Система условий построена на трех компонентах:

1. **UDialogueCondition** - базовый класс условия
2. **UDialogueConditionEvaluator** - оценщик условий (поддерживает DSL)
3. **DialogueRunner** - движок диалогов, который проверяет условия перед показом нодов

### Где Хранятся Условия

#### 1. На Уровне Нода (UDialogueNode)

```cpp
// В структуре FDialogueNodeData (Data Asset)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
FString ConditionExpression;  // DSL строка, например: "affinity[Lianne] >= 50"

// В runtime классе UDialogueNode
UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialogue")
TObjectPtr<UDialogueCondition> Condition;  // Visual condition object
```

#### 2. На Уровне Связей (FDialogueConnection)

```cpp
USTRUCT(BlueprintType)
struct FDialogueConnection
{
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TargetNodeId;
    
    // Условие для перехода по этой связи
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EdgeCondition;  // DSL или пустое для "всегда доступно"
  
    // Runtime compiled condition
    UPROPERTY()
    TObjectPtr<UDialogueCondition> Condition;
};
```

---

## Типы Условий (Condition Types)

### 1. Логические Операторы

**AND (И):**
```cpp
UCLASS()
class UDialogueCondition_And : public UDialogueCondition
{
    UPROPERTY(EditAnywhere, Instanced)
    TArray<TObjectPtr<UDialogueCondition>> Conditions;
    
    // Возвращает true только если ВСЕ условия true
};
```

**OR (ИЛИ):**
```cpp
UCLASS()
class UDialogueCondition_Or : public UDialogueCondition
{
    UPROPERTY(EditAnywhere, Instanced)
    TArray<TObjectPtr<UDialogueCondition>> Conditions;
    
    // Возвращает true если ХОТЯ БЫ ОДНО условие true
};
```

**NOT (НЕ):**
```cpp
UCLASS()
class UDialogueCondition_Not : public UDialogueCondition
{
    UPROPERTY(EditAnywhere, Instanced)
    TObjectPtr<UDialogueCondition> Condition;
    
    // Инвертирует результат вложенного условия
};
```

### 2. Игровые Условия

**Affinity (Отношения с NPC):**
```cpp
UCLASS()
class UDialogueCondition_Affinity : public UDialogueCondition
{
    UPROPERTY(EditAnywhere)
  FName NPCName;  // Имя NPC
    
    UPROPERTY(EditAnywhere)
    float MinValue = 0.0f;  // Минимальное значение affinity
    
    // Пример: affinity >= 50 -> разблокировать романтическую ветку
};
```

**HasItem (Наличие предмета):**
```cpp
UCLASS()
class UDialogueCondition_HasItem : public UDialogueCondition
{
    UPROPERTY(EditAnywhere)
    FName ItemId;  // ID предмета
    
    UPROPERTY(EditAnywhere)
    int32 MinCount = 1;  // Минимальное количество
};
```

**Memory (Память NPC):**
```cpp
UCLASS()
class UDialogueCondition_Memory : public UDialogueCondition
{
    UPROPERTY(EditAnywhere)
    FName MemoryKey;  // Ключ воспоминания
    
    UPROPERTY(EditAnywhere)
    bool ExpectedValue = true;  // Ожидаемое значение
};
```

**TimeOfDay (Время суток):**
```cpp
UCLASS()
class UDialogueCondition_TimeOfDay : public UDialogueCondition
{
    UPROPERTY(EditAnywhere)
    FGameplayTag RequiredTime;  // Time.Morning, Time.Evening, etc.
};
```

**WorldState (Состояние мира через теги):**
```cpp
UCLASS()
class UDialogueCondition_WorldState : public UDialogueCondition
{
    UPROPERTY(EditAnywhere)
    FGameplayTag RequiredTag;  // Например: Quest.MainQuest.Completed
};
```

**CustomVariable (Пользовательские переменные):**
```cpp
UCLASS()
class UDialogueCondition_CustomVariable : public UDialogueCondition
{
    UPROPERTY(EditAnywhere)
    FName VariableKey;  // Ключ переменной
    
    UPROPERTY(EditAnywhere)
    FString ExpectedValue;  // Ожидаемое значение
 
    UPROPERTY(EditAnywhere)
    bool bNumericComparison = false;  // Сравнивать как числа?
};
```

---

## Как Работает Фильтрация Нодов

### Алгоритм в DialogueRunner

```cpp
// 1. Получение доступных выборов
TArray<UDialogueNode*> UDialogueRunner::GetAvailableChoices() const
{
    return GatherChoices(CurrentNode);
}

// 2. Фильтрация по условиям
TArray<UDialogueNode*> UDialogueRunner::GatherChoices(UDialogueNode* FromNode) const
{
  TArray<UDialogueNode*> Result;
    
    // Проходим по всем связям из текущего нода
    for (const FDialogueConnection& Connection : FromNode->Connections)
    {
     // ? ПРОВЕРКА УСЛОВИЯ СВЯЗИ
        if (IsConnectionAvailable(Connection))
        {
      UDialogueNode* TargetNode = NodeCache[Connection.TargetNodeId];
     
    // Только player choice ноды показываются как выборы
            if (TargetNode && TargetNode->NodeType == EDialogueNodeType::PlayerChoice)
     {
        // ? ПРОВЕРКА УСЛОВИЯ САМОГО НОДА
    if (TargetNode->IsAvailable(CurrentContext))
 {
            Result.Add(TargetNode);
  }
         }
        }
    }
    
    return Result;
}

// 3. Проверка условия связи
bool UDialogueRunner::IsConnectionAvailable(const FDialogueConnection& Connection) const
{
    // Пустое условие = всегда доступно
    if (!Connection.Condition)
    {
        return true;
    }
    
    // Оценка условия через Evaluator
    return ConditionEvaluator->EvaluateCondition(Connection.Condition, CurrentContext);
}
```

### Проверка на Уровне Нода

```cpp
// В UDialogueNode::IsAvailable()
bool UDialogueNode::IsAvailable(const UDialogueSessionContext* Context) const
{
    // 1. Проверка OneTimeOnly
    if (bOneTimeOnly && bVisitedThisSession)
    {
        return false;  // Уже посещали - скрываем
 }
    
  // 2. Проверка условия нода
    if (Condition)
  {
        return Condition->Evaluate(Context);
    }
    
 // Нет условия = всегда доступно
return true;
}
```

---

## Интеграция с GameEventBus

### Автоматическая Отправка Событий

Dialogue System автоматически отправляет события через GameEventBus:

```cpp
// В DialogueSubsystem при старте диалога
FDialogueGameEventHelper::EmitDialogueStartedEvent(this, Player, NPC, DialogueId);

// События:
// - Dialogue.Event.Started
// - Dialogue.Event.Ended
// - Dialogue.Event.NodeChanged
// - Dialogue.Event.ChoiceSelected
```

### Прослушивание Событий

Другие системы могут подписываться на эти события:

```cpp
// Пример из RelationshipPanel - слушает события диалога
#if GAMEEVENTBUS_AVAILABLE
UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();

FGameplayTag DialogueStartedTag = FGameplayTag::RequestGameplayTag("GameEvent.Dialogue.Started");
EventBus->SubscribeToEventNative(
    DialogueStartedTag,
    FGameEventNativeDelegate::FDelegate::CreateLambda([WeakThis](const FGameEventPayload& Payload)
    {
        // Реакция на начало диалога
    })
);
#endif
```

### Реакция на События в Условиях

Вы можете создать условия, которые проверяют события:

```cpp
// Пример: показать выбор только если прошло событие "Quest.Started"
UCLASS()
class UDialogueCondition_EventTriggered : public UDialogueCondition
{
    UPROPERTY(EditAnywhere)
    FGameplayTag RequiredEvent;
    
    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override
    {
        // Проверить в контексте, было ли это событие
 return Context->HasEventBeenTriggered(RequiredEvent);
    }
};
```

---

## Примеры Использования

### Пример 1: Простое Условие (DSL)

**В Data Asset, поле `ConditionExpression`:**
```
affinity[Lianne] >= 50
```

**Что происходит:**
- Нод виден только если отношения с Lianne >= 50
- Иначе нод скрыт от игрока

### Пример 2: Сложное Условие (DSL)

```
(affinity[Lianne] >= 60 && time == Evening && variable[Location] == Garden) || affinity[Lianne] >= 85
```

**Что происходит:**
- Романтическая сцена доступна если:
  - Отношения >= 60 И вечер И локация = Сад
  - ИЛИ отношения очень высокие (>= 85)

### Пример 3: Visual Condition (Blueprint)

**В Data Asset, создать Condition object:**
```cpp
// 1. Create UDialogueCondition_And
// 2. Add child conditions:
//    - UDialogueCondition_Affinity (NPCName = "Lianne", MinValue = 50)
//    - UDialogueCondition_HasItem (ItemId = "Rose", MinCount = 1)
//    - UDialogueCondition_Memory (MemoryKey = "MetInGarden", ExpectedValue = true)
```

**Результат:**
- Нод виден только если ВСЕ три условия true

### Пример 4: OneTimeOnly Choice

```cpp
// В Data Asset:
NodeType = PlayerChoice
bOneTimeOnly = true
ChoiceText = "Confess your feelings" // Признаться в чувствах

// После выбора этого варианта он больше не появится в списке
```

### Пример 5: Условие на Связи (Edge Condition)

```cpp
// Connection 1:
FromNodeId = "NPC_Question"
ToNodeId = "PlayerChoice_Honest"
EdgeCondition = "affinity[Lianne] >= 30"  // Доступно только при хороших отношениях

// Connection 2:
FromNodeId = "NPC_Question"
ToNodeId = "PlayerChoice_Lie"
EdgeCondition = ""  // Всегда доступно
```

---

## Где Настраивать в Data Asset

### Шаг 1: Открыть Dialogue Data Asset

```
Content Browser -> Right-Click -> Dialogue -> Dialogue Data Asset
```

### Шаг 2: Настроить Nodes

**Для каждого Node в массиве `Nodes`:**

| Свойство | Описание | Пример |
|----------|----------|--------|
| `NodeId` | Уникальный ID | `"Choice_Romantic_01"` |
| `NodeType` | Тип нода | `PlayerChoice` |
| `ChoiceText` | Текст выбора | `"Ask about her feelings"` |
| `ConditionExpression` | DSL условие | `"affinity[Lianne] >= 50"` |
| `Condition` | Visual condition | Create `UDialogueCondition_Affinity` |
| `bOneTimeOnly` | Разовый выбор | `true` |

### Шаг 3: Настроить Connections

**Для каждой связи в `Node->Connections`:**

| Свойство | Описание | Пример |
|----------|----------|--------|
| `FromNodeId` | Откуда | `"NPC_Line_01"` |
| `ToNodeId` | Куда | `"Choice_Romantic_01"` |
| `EdgeCondition` | Условие связи | `"time == Evening"` |
| `Priority` | Приоритет | `10` |

---

## DSL Синтаксис

### Операторы Сравнения
```
==  (равно)
!=  (не равно)
>   (больше)
<   (меньше)
>=  (больше или равно)
<=  (меньше или равно)
```

### Логические Операторы
```
&&  (AND - И)
||  (OR - ИЛИ)
!   (NOT - НЕ)
```

### Функции

| Функция | Описание | Пример |
|---------|----------|--------|
| `affinity[NPCName]` | Отношения с NPC | `affinity[Lianne] >= 50` |
| `hasItem(ItemId)` | Наличие предмета | `hasItem(Rose)` |
| `hasItem(ItemId, Count)` | Кол-во предметов | `hasItem(Gold, 100)` |
| `memory(Key)` | Память NPC | `memory(MetInGarden)` |
| `visited(NodeId)` | Посещен ли нод | `visited(Node_002)` |
| `variable[Key]` | Пользовательская переменная | `variable[QuestStage] == 2` |
| `tag(TagName)` | Проверка тега в контексте | `tag(Time.Evening)` |
| `time` | Текущее время суток | `time == Evening` |

### Примеры DSL Выражений

```cpp
// 1. Простое условие
"affinity[Lianne] >= 50"

// 2. Комбинация условий (AND)
"hasItem(Rose) && affinity[Lianne] >= 30"

// 3. Альтернативы (OR)
"affinity[Lianne] >= 70 || hasItem(LoveLetter)"

// 4. Отрицание
"!visited(Node_BadChoice)"

// 5. Сложная логика с группировкой
"(variable[QuestStage] == 2 && memory(MetLianne)) || affinity[Lianne] >= 70"

// 6. Проверка времени и локации
"time == Evening && variable[Location] == Garden && !memory(RomanticScenePlayed)"
```

---

## Итоговая Таблица: Что Реализовано

| Функция | Статус | Использует GameEventBus | Комментарий |
|---------|--------|-------------------------|-------------|
| **Условия на нодах** | ? Да | ? Нет | Проверяется в `UDialogueNode::IsAvailable()` |
| **Условия на связях** | ? Да | ? Нет | Проверяется в `DialogueRunner::IsConnectionAvailable()` |
| **DSL парсер** | ? Да | ? Нет | `UDialogueConditionEvaluator::ParseCondition()` |
| **Visual conditions** | ? Да | ? Нет | Через `UDialogueCondition` классы |
| **OneTimeOnly** | ? Да | ? Нет | Автоматическая фильтрация посещенных нодов |
| **Автофильтрация выборов** | ? Да | ? Нет | `DialogueRunner::GatherChoices()` |
| **Отправка событий диалога** | ? Да | ? Да | `Dialogue.Event.Started/Ended/etc` |
| **Реакция на внешние события** | ?? Частично | ? Да | Нужно создать `UDialogueCondition_EventTriggered` |
| **Интеграция с RelationshipSystem** | ? Да | ? Да | Через `UDialogueCondition_Affinity` |
| **Интеграция с Memory System** | ? Да | ? Нет | Через `UDialogueCondition_Memory` |

---

## Заключение

### ? Система Полностью Работоспособна

**Что можно делать прямо сейчас:**
1. Создавать условия в DA через `ConditionExpression` (DSL)
2. Создавать visual conditions через Blueprint
3. Фильтровать выборы по affinity, items, memory, time, tags
4. Использовать OneTimeOnly для разовых выборов
5. Комбинировать условия с помощью AND/OR/NOT
6. Слушать события диалога через GameEventBus в других системах

**Что работает через GameEventBus:**
- ? Отправка событий начала/конца диалога
- ? Отправка событий смены нода
- ? Отправка событий выбора
- ? Прослушивание этих событий в RelationshipPanel, Camera, Input systems

**Что НЕ использует GameEventBus напрямую:**
- Сама логика условий (она локальная, в рамках DialogueRunner)
- Проверка IsAvailable (выполняется синхронно)

### Рекомендации

1. **Для простых условий** - используйте DSL в `ConditionExpression`
2. **Для сложных условий** - создавайте visual conditions
3. **Для реакции на события** - подписывайтесь на GameEventBus в своих системах
4. **Для создания новых типов условий** - наследуйтесь от `UDialogueCondition`

---

## Дополнительные Ресурсы

**Примеры кода:**
- `Plugins/DialogueSystem/Source/DialogueSystemCore/Public/Examples/DialogueConditionExamples.h`

**Тесты:**
- `Plugins/DialogueSystem/Source/DialogueSystemCore/Private/Tests/DialogueConditionTests.cpp`

**Документация:**
- `CHANGELOG.md` - История изменений системы условий
- `06_DialogueEvents_Guide.md` - Интеграция с GameEventBus

---

**Дата Создания:** 2025-01-02  
**Версия Dialogue System:** v1.11  
**Версия RelationshipSystem:** v1.0
