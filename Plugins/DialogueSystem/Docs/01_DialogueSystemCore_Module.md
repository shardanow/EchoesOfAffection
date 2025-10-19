# DialogueSystemCore Module Documentation

## Обзор модуля

**DialogueSystemCore** — это ядро системы диалогов, предоставляющее базовые классы, интерфейсы и логику для работы с диалогами. Этот модуль не зависит от UI или конкретных платформ и может использоваться как в C++, так и в Blueprint.

## Архитектура

```
DialogueSystemCore/
??? Public/
?   ??? Core/                   # Основные классы диалогов
?   ?   ??? DialogueNode.h      # Узел диалога
?   ?   ??? DialogueConnection.h# Связь между узлами
?   ?   ??? DialogueContext.h   # Контекст выполнения диалога
?   ?   ??? DialogueRunner.h    # Управление выполнением диалога
?   ??? Data/                   # Данные и ассеты
?   ?   ??? DialogueDataAsset.h # Data Asset для хранения диалогов
?   ??? Conditions/             # Система условий
?   ?   ??? DialogueConditionEvaluator.h
?   ??? Effects/                # Система эффектов
?   ?   ??? DialogueEffectExecutor.h
?   ??? SaveGame/               # Сохранение состояния
?   ?   ??? DialogueSaveData.h
?   ??? Pool/                   # Объектный пул
?   ?   ??? DialogueObjectPool.h
?   ??? Interfaces/             # Интерфейсы
?       ??? IDialogueParticipant.h
?       ??? IDialogueService.h
??? Private/
    ??? ...                     # Реализация
```

---

## Ключевые классы

### 1. UDialogueDataAsset

**Назначение**: Data Asset для хранения диалогов в виде графа узлов и связей.

#### Основные свойства:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
TArray<UDialogueNode*> Nodes;  // Все узлы диалога

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
TArray<FDialogueConnection> Connections;  // Связи между узлами

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
FString StartNodeId;  // ID начального узла

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
FGameplayTagContainer RequiredTags;  // Теги для фильтрации
```

#### Использование в Blueprint:

1. **Создание Data Asset**:
   - В Content Browser: `Right Click ? Miscellaneous ? Data Asset`
   - Выберите класс: `DialogueDataAsset`
   - Назовите, например: `DA_Dialogue_Intro`

2. **Заполнение данных**:
   - Откройте созданный ассет
   - В секции `Nodes` добавьте узлы диалога через `+`
   - В секции `Connections` создайте связи между узлами
   - Укажите `Start Node Id` (ID первого узла)

#### Пример в C++:

```cpp
// Загрузка диалога
UDialogueDataAsset* DialogueAsset = LoadObject<UDialogueDataAsset>(
    nullptr,
    TEXT("/Game/Dialogues/DA_Dialogue_Intro")
);

if (DialogueAsset)
{
    // Получить начальный узел
    UDialogueNode* StartNode = DialogueAsset->GetNodeById(DialogueAsset->StartNodeId);
    
    // Получить все доступные связи из узла
    TArray<FDialogueConnection> AvailableChoices = DialogueAsset->GetConnectionsFromNode(StartNode->NodeId);
}
```

---

### 2. UDialogueNode

**Назначение**: Представляет один узел диалога (реплика, выбор игрока, условие и т.д.).

#### Типы узлов:

```cpp
UENUM(BlueprintType)
enum class EDialogueNodeType : uint8
{
    Dialogue,      // Обычная реплика NPC
    PlayerChoice,  // Выбор игрока
    Condition,     // Условный узел
    Effect,        // Узел с эффектами
    Random,        // Случайный выбор
    Jump           // Переход к другому узлу
};
```

#### Основные свойства:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
FString NodeId;  // Уникальный ID узла

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
EDialogueNodeType NodeType;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content", meta = (MultiLine = "true"))
FText DialogueText;  // Текст реплики

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
FName SpeakerName;  // Имя говорящего

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
TSoftObjectPtr<UTexture2D> SpeakerPortrait;  // Портрет говорящего

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content")
TSoftObjectPtr<USoundBase> VoiceOver;  // Озвучка

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
FName AnimationName;  // Имя анимации для проигрывания

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
FName CameraTag;  // Тег камеры для этого узла

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
FGameplayTagContainer Tags;  // Теги узла
```

#### Использование в Blueprint:

**Создание узла**:
```cpp
// В DialogueDataAsset, в массиве Nodes:
[0]
  - Node Id: "Node_001"
  - Node Type: Dialogue
  - Dialogue Text: "Привет! Как дела?"
  - Speaker Name: "Lianne"
  - Speaker Portrait: T_Portrait_Lianne
  - Voice Over: VO_Lianne_Greeting
  - Animation Name: "Talk_Happy"
  - Camera Tag: "Camera.Closeup"
```

#### Пример в C++:

```cpp
// Создание нового узла
UDialogueNode* NewNode = NewObject<UDialogueNode>();
NewNode->NodeId = TEXT("Node_001");
NewNode->NodeType = EDialogueNodeType::Dialogue;
NewNode->DialogueText = FText::FromString(TEXT("Привет! Как дела?"));
NewNode->SpeakerName = FName(TEXT("Lianne"));

// Добавление в ассет
DialogueAsset->Nodes.Add(NewNode);
```

---

### 3. FDialogueConnection

**Назначение**: Описывает связь между двумя узлами диалога (переход от одного узла к другому).

#### Структура:

```cpp
USTRUCT(BlueprintType)
struct FDialogueConnection
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString FromNodeId;  // ID исходного узла

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ToNodeId;  // ID целевого узла

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FText ChoiceText;  // Текст выбора (если это выбор игрока)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString Condition;  // Условие для отображения (DSL строка)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true"))
    FString Effect;  // Эффект при выборе (DSL строка)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Priority;  // Приоритет (для сортировки)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer RequiredTags;  // Необходимые теги

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHidden;  // Скрыть выбор от игрока
};
```

#### Использование в Blueprint:

**Создание связи**:
```cpp
// В DialogueDataAsset, в массиве Connections:
[0]
  - From Node Id: "Node_001"
  - To Node Id: "Node_002"
  - Choice Text: "Хорошо, спасибо!"
  - Condition: "affinity[Lianne] >= 10"
  - Effect: "affinity[Lianne] += 5; memory(Greeted, true)"
  - Priority: 1
  - Required Tags: (пусто)
  - Hidden: false
```

#### Пример в C++:

```cpp
// Создание связи
FDialogueConnection NewConnection;
NewConnection.FromNodeId = TEXT("Node_001");
NewConnection.ToNodeId = TEXT("Node_002");
NewConnection.ChoiceText = FText::FromString(TEXT("Хорошо, спасибо!"));
NewConnection.Condition = TEXT("affinity[Lianne] >= 10");
NewConnection.Effect = TEXT("affinity[Lianne] += 5; memory(Greeted, true)");
NewConnection.Priority = 1;

// Добавление в ассет
DialogueAsset->Connections.Add(NewConnection);
```

---

### 4. UDialogueContext

**Назначение**: Контекст выполнения диалога, содержащий все необходимые данные для оценки условий и выполнения эффектов.

#### Основные свойства:

```cpp
UPROPERTY(BlueprintReadWrite, Category = "Context")
AActor* Player;  // Игрок

UPROPERTY(BlueprintReadWrite, Category = "Context")
AActor* NPC;  // NPC, с которым идет диалог

UPROPERTY(BlueprintReadWrite, Category = "Context")
TMap<FString, bool> Memories;  // Флаги памяти

UPROPERTY(BlueprintReadWrite, Category = "Context")
TMap<FString, float> Variables;  // Переменные

UPROPERTY(BlueprintReadWrite, Category = "Context")
FGameplayTagContainer WorldStateTags;  // Теги состояния мира

UPROPERTY(BlueprintReadWrite, Category = "Context")
FGameplayTag CurrentTimeTag;  // Текущее время суток
```

#### Методы:

```cpp
// Память (флаги)
UFUNCTION(BlueprintCallable)
void SetMemory(const FString& Key, bool Value);

UFUNCTION(BlueprintCallable)
bool GetMemory(const FString& Key, bool DefaultValue = false) const;

// Переменные
UFUNCTION(BlueprintCallable)
void SetVariable(const FString& Key, float Value);

UFUNCTION(BlueprintCallable)
float GetVariable(const FString& Key, float DefaultValue = 0.0f) const;

// Теги
UFUNCTION(BlueprintCallable)
bool HasTag(const FGameplayTag& Tag) const;

UFUNCTION(BlueprintCallable)
void AddTag(const FGameplayTag& Tag);

UFUNCTION(BlueprintCallable)
void RemoveTag(const FGameplayTag& Tag);
```

#### Использование в Blueprint:

**Создание контекста**:
```blueprintue
// В Event Graph:

Event BeginPlay
    ? Create Dialogue Context
        Return Value ? DialogueContext (переменная)
    ? Set Player (DialogueContext, Self)
    ? Set NPC (DialogueContext, Get Actor of Class [BP_NPC_Lianne])
```

**Работа с памятью**:
```blueprintue
// Установка флага
Set Memory (DialogueContext, "FirstMeeting", true)

// Проверка флага
Get Memory (DialogueContext, "FirstMeeting") 
    ? Branch (если true, то...)
```

**Работа с переменными**:
```blueprintue
// Установка переменной
Set Variable (DialogueContext, "QuestProgress", 3.0)

// Получение переменной
Get Variable (DialogueContext, "QuestProgress")
    ? Print String
```

#### Пример в C++:

```cpp
// Создание контекста
UDialogueContext* Context = NewObject<UDialogueContext>();
Context->Player = PlayerActor;
Context->NPC = NPCActor;

// Работа с памятью
Context->SetMemory(TEXT("FirstMeeting"), true);
bool bHasMet = Context->GetMemory(TEXT("FirstMeeting"));

// Работа с переменными
Context->SetVariable(TEXT("QuestProgress"), 3.0f);
float QuestProgress = Context->GetVariable(TEXT("QuestProgress"));

// Работа с тегами
FGameplayTag WeatherTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Weather.Rainy")));
Context->AddTag(WeatherTag);
bool bIsRainy = Context->HasTag(WeatherTag);
```

---

### 5. UDialogueRunner

**Назначение**: Управляет выполнением диалога, обрабатывает переходы между узлами, оценивает условия и применяет эффекты.

#### Основные методы:

```cpp
// Запуск диалога
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void StartDialogue(UDialogueDataAsset* DialogueAsset, UDialogueContext* Context);

// Получить текущий узел
UFUNCTION(BlueprintCallable, Category = "Dialogue")
UDialogueNode* GetCurrentNode() const;

// Получить доступные выборы
UFUNCTION(BlueprintCallable, Category = "Dialogue")
TArray<FDialogueConnection> GetAvailableChoices() const;

// Выбрать вариант ответа
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void SelectChoice(int32 ChoiceIndex);

// Продолжить диалог (для узлов без выбора)
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void Continue();

// Завершить диалог
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void EndDialogue();

// Проверка, активен ли диалог
UFUNCTION(BlueprintCallable, Category = "Dialogue")
bool IsDialogueActive() const;
```

#### События (делегаты):

```cpp
// Срабатывает при смене узла
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeChanged, UDialogueNode*, NewNode);

// Срабатывает при завершении диалога
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);

// Срабатывает при обновлении выборов
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicesUpdated, const TArray<FDialogueConnection>&, Choices);
```

#### Использование в Blueprint:

**Базовый поток диалога**:
```blueprintue
// 1. Создание Runner
Event BeginPlay
    ? Create Dialogue Runner
        Return Value ? DialogueRunner (переменная)
    
// 2. Подписка на события
Dialogue Runner ? On Node Changed
    ? Add Event [Display Node]
        New Node ? Get Dialogue Text ? Set Text (Widget)

Dialogue Runner ? On Dialogue Ended
    ? Add Event [Close Dialogue Widget]

Dialogue Runner ? On Choices Updated
    ? Add Event [Populate Choice Buttons]

// 3. Запуск диалога
Dialogue Runner ? Start Dialogue
    Dialogue Asset: DA_Dialogue_Intro
    Context: DialogueContext

// 4. Выбор варианта ответа (при клике на кнопку)
Button Click Event
    ? Dialogue Runner ? Select Choice
        Choice Index: 0 (или индекс выбранной кнопки)

// 5. Продолжение диалога (для узлов без выбора)
Button "Continue" Click Event
    ? Dialogue Runner ? Continue

// 6. Завершение диалога
Button "Exit" Click Event
    ? Dialogue Runner ? End Dialogue
```

#### Пример в C++:

```cpp
// Создание Runner
UDialogueRunner* Runner = NewObject<UDialogueRunner>();

// Подписка на события
Runner->OnNodeChanged.AddDynamic(this, &AMyActor::OnDialogueNodeChanged);
Runner->OnDialogueEnded.AddDynamic(this, &AMyActor::OnDialogueEnded);
Runner->OnChoicesUpdated.AddDynamic(this, &AMyActor::OnChoicesUpdated);

// Запуск диалога
Runner->StartDialogue(DialogueAsset, Context);

// Обработчики событий
void AMyActor::OnDialogueNodeChanged(UDialogueNode* NewNode)
{
    if (NewNode)
    {
        UE_LOG(LogTemp, Log, TEXT("Current Node: %s - %s"), 
               *NewNode->NodeId, 
               *NewNode->DialogueText.ToString());
    }
}

void AMyActor::OnDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue Ended"));
}

void AMyActor::OnChoicesUpdated(const TArray<FDialogueConnection>& Choices)
{
    for (int32 i = 0; i < Choices.Num(); ++i)
    {
        UE_LOG(LogTemp, Log, TEXT("Choice %d: %s"), i, *Choices[i].ChoiceText.ToString());
    }
}

// Выбор варианта ответа
Runner->SelectChoice(0);

// Продолжение диалога
Runner->Continue();
```

---

### 6. UDialogueConditionEvaluator

**Назначение**: Оценивает условия, написанные на DSL (Domain-Specific Language), и определяет, доступен ли тот или иной выбор/узел.

#### Основные методы:

```cpp
// Оценить условие из строки
UFUNCTION(BlueprintCallable, Category = "Dialogue|Conditions")
bool EvaluateString(const FString& ConditionString, UDialogueContext* Context);

// Оценить условие (объект)
UFUNCTION(BlueprintCallable, Category = "Dialogue|Conditions")
bool Evaluate(UDialogueCondition* Condition, UDialogueContext* Context);
```

#### Поддерживаемые условия (DSL):

```cpp
// Предметы
hasItem(ItemName)
hasItem(ItemName, Count)
has(Rose, 2)

// Память/Флаги
memory(Key)
memory(Key, Value)
visited(NodeId)

// Отношения
affinity[NPCName] >= 50
affinity[Lianne] > 75

// Переменные
variable[VarName] == Value
var[QuestProgress] >= 10

// Время
time == Morning
time == Evening

// Теги
tag(TagName)
hasTag(Event.Festival)

// Логические операторы
&&, ||, !, ()
```

#### Использование в Blueprint:

**Проверка условия из строки**:
```blueprintue
// Создание Evaluator
Create Dialogue Condition Evaluator
    ? Return Value ? ConditionEval (переменная)

// Оценка условия
Condition Eval ? Evaluate String
    Condition String: "hasItem(Rose) && affinity[Lianne] >= 30"
    Context: DialogueContext
    ? Branch
        True ? Выполнить действие A
        False ? Выполнить действие B
```

#### Примеры условий:

```cpp
// Простые условия
"hasItem(Rose)"
"affinity[Lianne] >= 50"
"memory(FirstMeeting)"

// Сложные условия
"hasItem(Rose) && affinity[Lianne] >= 30"
"(memory(QuestStarted) && !visited(Node_Boss)) || affinity[Marcus] > 80"
"time == Evening && hasItem(Letter) && variable[QuestStage] == 2"

// Вложенные условия
"(affinity[Lianne] >= 50 || hasItem(Gift)) && !memory(GaveGift)"
```

#### Пример в C++:

```cpp
// Создание Evaluator
UDialogueConditionEvaluator* Evaluator = NewObject<UDialogueConditionEvaluator>();

// Оценка условия
FString ConditionString = TEXT("hasItem(Rose) && affinity[Lianne] >= 30");
bool bResult = Evaluator->EvaluateString(ConditionString, Context);

if (bResult)
{
    UE_LOG(LogTemp, Log, TEXT("Condition is TRUE"));
}
else
{
    UE_LOG(LogTemp, Log, TEXT("Condition is FALSE"));
}
```

---

### 7. UDialogueEffectExecutor

**Назначение**: Выполняет эффекты, написанные на DSL, которые изменяют состояние игры (инвентарь, отношения, квесты и т.д.).

#### Основные методы:

```cpp
// Выполнить эффект из строки
UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
void ExecuteString(const FString& EffectString, UDialogueContext* Context);

// Выполнить эффект (объект)
UFUNCTION(BlueprintCallable, Category = "Dialogue|Effects")
void Execute(UDialogueEffect* Effect, UDialogueContext* Context);
```

#### Поддерживаемые эффекты (DSL):

```cpp
// Инвентарь
giveItem(ItemName, Count)
give(ItemName)
removeItem(ItemName, Count)
remove(ItemName, Count)

// Валюта
gold += 100
gold -= 50

// Отношения
affinity[NPCName] += 10
affinity[NPCName] -= 5
trust[NPCName] += 15

// Память/Флаги
setMemory(Key, Value)
memory(Key, Value)
memory(Key)

// Переменные
setVariable(VarName, Value)
var(VarName, Value)

// Квесты
startQuest(QuestId)
quest(QuestId)
completeQuest(QuestId)

// Теги
addTag(TagName)
removeTag(TagName)

// Множественные эффекты (через точку с запятой)
effect1; effect2; effect3
```

#### Использование в Blueprint:

**Выполнение эффекта из строки**:
```blueprintue
// Создание Executor
Create Dialogue Effect Executor
    ? Return Value ? EffectExec (переменная)

// Выполнение эффекта
Effect Exec ? Execute String
    Effect String: "giveItem(Rose, 1); affinity[Lianne] += 10; startQuest(RomanceLianne)"
    Context: DialogueContext
```

#### Примеры эффектов:

```cpp
// Одиночные эффекты
"giveItem(Rose, 1)"
"affinity[Lianne] += 10"
"startQuest(RomanceLianne)"

// Множественные эффекты
"giveItem(Rose, 1); gold -= 25; affinity[Lianne] += 5"
"setMemory(GaveGift); startQuest(LianneQuest); affinity[Lianne] += 10"

// Сложные изменения состояния
"gold += 500; giveItem(Artifact); completeQuest(FindArtifact); addTag(Quest.Completed)"
```

#### Пример в C++:

```cpp
// Создание Executor
UDialogueEffectExecutor* Executor = NewObject<UDialogueEffectExecutor>();

// Выполнение эффектов
FString EffectString = TEXT("giveItem(Rose, 1); affinity[Lianne] += 10; startQuest(RomanceLianne)");
Executor->ExecuteString(EffectString, Context);
```

---

## Пошаговые примеры использования

### Пример 1: Простой диалог с NPC (Blueprint)

**Шаг 1. Создание Data Asset**:
1. В Content Browser: `Right Click ? Miscellaneous ? Data Asset`
2. Выберите класс: `DialogueDataAsset`
3. Назовите: `DA_Dialogue_SimpleGreeting`

**Шаг 2. Заполнение узлов**:
```cpp
Nodes[0]:
  - Node Id: "Node_Start"
  - Node Type: Dialogue
  - Dialogue Text: "Привет! Рад тебя видеть."
  - Speaker Name: "Lianne"

Nodes[1]:
  - Node Id: "Node_Player_Choice"
  - Node Type: PlayerChoice
  - Dialogue Text: "" (пустой для узлов выбора)

Nodes[2]:
  - Node Id: "Node_Response_Friendly"
  - Node Type: Dialogue
  - Dialogue Text: "Отлично! Давай поговорим."
  - Speaker Name: "Lianne"

Nodes[3]:
  - Node Id: "Node_Response_Rude"
  - Node Type: Dialogue
  - Dialogue Text: "Ну ладно, тогда до встречи."
  - Speaker Name: "Lianne"
```

**Шаг 3. Создание связей**:
```cpp
Connections[0]:
  - From Node Id: "Node_Start"
  - To Node Id: "Node_Player_Choice"
  - Choice Text: "" (автоматический переход)

Connections[1]:
  - From Node Id: "Node_Player_Choice"
  - To Node Id: "Node_Response_Friendly"
  - Choice Text: "Привет! Как дела?"
  - Effect: "affinity[Lianne] += 5"

Connections[2]:
  - From Node Id: "Node_Player_Choice"
  - To Node Id: "Node_Response_Rude"
  - Choice Text: "Не сейчас."
  - Effect: "affinity[Lianne] -= 2"
```

**Шаг 4. Запуск диалога в Blueprint**:
```blueprintue
// В Actor BP (например, BP_Player)

// Переменные:
- DialogueRunner (UDialogueRunner*)
- DialogueContext (UDialogueContext*)
- CurrentDialogueAsset (UDialogueDataAsset*)

Event BeginPlay
    ? Create Dialogue Runner ? DialogueRunner
    ? Create Dialogue Context ? DialogueContext
    ? Set Player (DialogueContext, Self)

// При взаимодействии с NPC (например, E)
Event E Pressed
    ? Set NPC (DialogueContext, Get Actor of Class [BP_NPC_Lianne])
    ? Load Asset [DA_Dialogue_SimpleGreeting] ? CurrentDialogueAsset
    ? Dialogue Runner ? Start Dialogue (CurrentDialogueAsset, DialogueContext)
```

---

### Пример 2: Диалог с условиями (Blueprint)

**Шаг 1. Создание Data Asset**:
1. Создайте Data Asset: `DA_Dialogue_ConditionalGift`

**Шаг 2. Заполнение узлов**:
```cpp
Nodes[0]:
  - Node Id: "Node_Start"
  - Node Type: Dialogue
  - Dialogue Text: "О, ты принес мне подарок?"
  - Speaker Name: "Lianne"

Nodes[1]:
  - Node Id: "Node_Player_Choice"
  - Node Type: PlayerChoice

Nodes[2]:
  - Node Id: "Node_Give_Rose"
  - Node Type: Dialogue
  - Dialogue Text: "Какая красивая роза! Спасибо!"
  - Speaker Name: "Lianne"

Nodes[3]:
  - Node Id: "Node_No_Gift"
  - Node Type: Dialogue
  - Dialogue Text: "Ну ладно, может в следующий раз."
  - Speaker Name: "Lianne"
```

**Шаг 3. Создание связей с условиями**:
```cpp
Connections[0]:
  - From Node Id: "Node_Start"
  - To Node Id: "Node_Player_Choice"

Connections[1]:
  - From Node Id: "Node_Player_Choice"
  - To Node Id: "Node_Give_Rose"
  - Choice Text: "Вот, держи розу!"
  - Condition: "hasItem(Rose)"  // Условие: у игрока есть роза
  - Effect: "removeItem(Rose, 1); affinity[Lianne] += 15; memory(GaveGift, true)"

Connections[2]:
  - From Node Id: "Node_Player_Choice"
  - To Node Id: "Node_No_Gift"
  - Choice Text: "Извини, я ничего не принес."
  - Effect: "affinity[Lianne] -= 3"
```

**Шаг 4. Запуск диалога**:
```blueprintue
// Аналогично Примеру 1, но используйте DA_Dialogue_ConditionalGift
```

**Результат**: Если у игрока есть роза в инвентаре, он увидит выбор "Вот, держи розу!". В противном случае этот выбор будет скрыт.

---

### Пример 3: Диалог с памятью и отношениями (C++)

```cpp
// В вашем Actor или Controller

void AMyPlayerController::StartDialogueWithLianne()
{
    // 1. Создание Context
    UDialogueContext* Context = NewObject<UDialogueContext>();
    Context->Player = GetPawn();
    Context->NPC = LianneActor;

    // 2. Загрузка диалога
    UDialogueDataAsset* DialogueAsset = LoadObject<UDialogueDataAsset>(
        nullptr,
        TEXT("/Game/Dialogues/DA_Dialogue_Lianne_Main")
    );

    // 3. Создание Runner
    UDialogueRunner* Runner = NewObject<UDialogueRunner>();
    Runner->OnNodeChanged.AddDynamic(this, &AMyPlayerController::OnNodeChanged);
    Runner->OnDialogueEnded.AddDynamic(this, &AMyPlayerController::OnDialogueEnded);

    // 4. Установка начальных условий
    Context->SetMemory(TEXT("FirstMeeting"), true);  // Отметка, что мы уже встречались
    Context->SetVariable(TEXT("LianneAffinity"), 50.0f);  // Начальное отношение

    // 5. Запуск диалога
    Runner->StartDialogue(DialogueAsset, Context);

    // Сохраняем Runner для последующего использования
    CurrentDialogueRunner = Runner;
    CurrentDialogueContext = Context;
}

void AMyPlayerController::OnNodeChanged(UDialogueNode* NewNode)
{
    if (NewNode)
    {
        UE_LOG(LogTemp, Log, TEXT("Node: %s - %s"), 
               *NewNode->NodeId, 
               *NewNode->DialogueText.ToString());

        // Обновляем UI
        if (DialogueWidget)
        {
            DialogueWidget->UpdateDialogueText(NewNode->DialogueText);
            DialogueWidget->UpdateSpeakerName(NewNode->SpeakerName);

            // Загружаем портрет
            if (NewNode->SpeakerPortrait.IsValid())
            {
                UTexture2D* Portrait = NewNode->SpeakerPortrait.LoadSynchronous();
                DialogueWidget->UpdateSpeakerPortrait(Portrait);
            }
        }

        // Получаем доступные выборы
        TArray<FDialogueConnection> Choices = CurrentDialogueRunner->GetAvailableChoices();
        if (Choices.Num() > 0)
        {
            // Обновляем кнопки выбора в UI
            if (DialogueWidget)
            {
                DialogueWidget->PopulateChoices(Choices);
            }
        }
    }
}

void AMyPlayerController::OnDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue Ended"));

    // Закрываем UI
    if (DialogueWidget)
    {
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;
    }

    // Сохраняем состояние Context
    SaveDialogueState();
}

void AMyPlayerController::OnPlayerSelectChoice(int32 ChoiceIndex)
{
    if (CurrentDialogueRunner && CurrentDialogueRunner->IsDialogueActive())
    {
        CurrentDialogueRunner->SelectChoice(ChoiceIndex);
    }
}

void AMyPlayerController::SaveDialogueState()
{
    if (CurrentDialogueContext)
    {
        // Сохраняем память
        UDialogueSaveData* SaveData = NewObject<UDialogueSaveData>();
        SaveData->Memories = CurrentDialogueContext->Memories;
        SaveData->Variables = CurrentDialogueContext->Variables;
        SaveData->WorldStateTags = CurrentDialogueContext->WorldStateTags;

        // Сохраняем в слот
        UGameplayStatics::SaveGameToSlot(SaveData, TEXT("DialogueSaveSlot"), 0);
    }
}
```

---

## Интеграция с другими системами

### Интеграция с Inventory System

```cpp
// Пример: Проверка наличия предмета через IDialogueService

// В вашей игре создайте класс, реализующий IDialogueService
class UMyDialogueService : public UObject, public IDialogueService
{
    GENERATED_BODY()

public:
    virtual bool HasItem(const FString& ItemName, int32 Count) const override
    {
        // Получаем компонент инвентаря игрока
        UInventoryComponent* Inventory = GetPlayerInventory();
        if (Inventory)
        {
            return Inventory->HasItem(ItemName, Count);
        }
        return false;
    }

    virtual void GiveItem(const FString& ItemName, int32 Count) override
    {
        UInventoryComponent* Inventory = GetPlayerInventory();
        if (Inventory)
        {
            Inventory->AddItem(ItemName, Count);
        }
    }

    virtual void RemoveItem(const FString& ItemName, int32 Count) override
    {
        UInventoryComponent* Inventory = GetPlayerInventory();
        if (Inventory)
        {
            Inventory->RemoveItem(ItemName, Count);
        }
    }

    // ... другие методы IDialogueService
};

// Регистрация сервиса в DialogueContext
Context->SetDialogueService(MyDialogueService);
```

### Интеграция с Quest System

```cpp
// Аналогично Inventory, реализуйте методы квестов в IDialogueService

virtual void StartQuest(const FString& QuestId) override
{
    UQuestSystem* QuestSystem = GetQuestSystem();
    if (QuestSystem)
    {
        QuestSystem->StartQuest(QuestId);
    }
}

virtual void CompleteQuest(const FString& QuestId) override
{
    UQuestSystem* QuestSystem = GetQuestSystem();
    if (QuestSystem)
    {
        QuestSystem->CompleteQuest(QuestId);
    }
}
```

---

## Сохранение и загрузка состояния

### UDialogueSaveData

**Назначение**: Сохранение состояния диалогов (память, переменные, теги).

```cpp
USTRUCT(BlueprintType)
struct FDialogueSaveData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, SaveGame)
    TMap<FString, bool> Memories;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    TMap<FString, float> Variables;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    FGameplayTagContainer WorldStateTags;

    UPROPERTY(BlueprintReadWrite, SaveGame)
    TMap<FString, int32> VisitedNodesCount;  // Счетчик посещений узлов
};
```

#### Сохранение (Blueprint):

```blueprintue
// Создание Save Data
Create Save Game Object [UDialogueSaveData] ? SaveData

// Заполнение данных
Set Memories (SaveData, DialogueContext.Memories)
Set Variables (SaveData, DialogueContext.Variables)
Set World State Tags (SaveData, DialogueContext.WorldStateTags)

// Сохранение
Save Game to Slot (SaveData, "DialogueSaveSlot", 0)
```

#### Загрузка (Blueprint):

```blueprintue
// Загрузка Save Data
Load Game from Slot ("DialogueSaveSlot", 0) ? Cast to DialogueSaveData ? SaveData

// Восстановление состояния
Get Memories (SaveData) ? Set Memories (DialogueContext, ...)
Get Variables (SaveData) ? Set Variables (DialogueContext, ...)
Get World State Tags (SaveData) ? Set World State Tags (DialogueContext, ...)
```

---

## Лучшие практики

1. **Используйте кэширование**: DialogueRunner и Context кэшируют условия/эффекты для повышения производительности.

2. **Разделяйте диалоги**: Создавайте отдельные Data Assets для разных сцен/NPC.

3. **Используйте теги**: Теги позволяют фильтровать диалоги по времени суток, погоде, событиям и т.д.

4. **Тестируйте условия**: Убедитесь, что условия корректно настроены, чтобы избежать недоступных выборов.

5. **Сохраняйте состояние**: Регулярно сохраняйте Context для восстановления прогресса игрока.

6. **Используйте приоритеты**: Устанавливайте приоритеты для связей, чтобы контролировать порядок выборов.

7. **Логируйте ошибки**: Включите логирование для отладки условий и эффектов.

---

## Заключение

**DialogueSystemCore** предоставляет мощную и гибкую основу для создания диалогов в игре. Благодаря DSL для условий и эффектов, вы можете легко настраивать сложную логику диалогов без написания кода. Система полностью интегрируется с Blueprint и поддерживает расширение через пользовательские сервисы.

Для получения дополнительной информации см.:
- `02_DialogueSystemRuntime_Module.md` — Runtime компоненты (Subsystem, Components)
- `03_DialogueSystemUI_Module.md` — UI виджеты для диалогов
- `04_DialogueSystemAI_Module.md` — Интеграция с AI

