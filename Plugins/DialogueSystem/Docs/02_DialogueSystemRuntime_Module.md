# DialogueSystemRuntime Module Documentation

## Обзор модуля

**DialogueSystemRuntime** — это runtime-модуль системы диалогов, предоставляющий компоненты для интеграции диалогов в игровой процесс, управление сохранениями, асинхронную загрузку ассетов и систему отношений с NPC.

## Архитектура

```
DialogueSystemRuntime/
??? Public/
?   ??? Subsystems/
?   ?   ??? DialogueSubsystem.h       # Центральная подсистема
?   ??? Components/
?   ?   ??? DialogueComponent.h       # Компонент для NPC
?   ?   ??? DialoguePresentationComponent.h  # UI презентация
?   ?   ??? DialogueCameraComponent.h  # Управление камерой
?   ?   ??? DialogueInputBlockerComponent.h  # Блокировка ввода
?   ?   ??? RelationshipComponent.h   # Система отношений
?   ?   ??? NPCMemoryComponent.h      # Память NPC
?   ??? Library/
?       ??? DialoguePresentationLibrary.h  # Blueprint функции
??? Private/
    ??? ...                           # Реализация
```

---

## Ключевые классы

### 1. UDialogueSubsystem

**Назначение**: Центральная подсистема для управления диалогами на уровне всей игры. Реализует `IDialogueService` для обеспечения dependency injection.

#### Основные возможности:

- **Управление активными диалогами**: Старт, остановка, проверка активности
- **Пулинг объектов**: DialogueRunner объекты переиспользуются для производительности
- **Асинхронная загрузка**: Диалоги загружаются асинхронно через AssetManager
- **Автосохранение**: Автоматическое сохранение и загрузка состояния диалогов
- **Настройки**: Управление настройками диалогов (скорость текста, автопродолжение и т.д.)
- **История**: Хранение истории пройденных диалогов

#### Основные методы:

```cpp
// Запуск диалога
UFUNCTION(BlueprintCallable, Category = "Dialogue|Service")
UDialogueRunner* StartDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC);

// Возобновление диалога (из сохранения)
UFUNCTION(BlueprintCallable, Category = "Dialogue|Service")
UDialogueRunner* ResumeDialogue(
    UDialogueDataAsset* DialogueAsset, 
    AActor* Player, 
    AActor* NPC, 
    const FDialogueSessionSaveData& SaveData
);

// Завершить активный диалог
UFUNCTION(BlueprintCallable, Category = "Dialogue|Service")
void EndActiveDialogue();

// Проверить, активен ли диалог
UFUNCTION(BlueprintPure, Category = "Dialogue|Service")
bool IsDialogueActive() const;

// Получить активный диалог
UFUNCTION(BlueprintPure, Category = "Dialogue|Service")
UDialogueRunner* GetActiveDialogue() const;
```

#### Асинхронная загрузка:

```cpp
// Асинхронно загрузить диалог
UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
void LoadDialogueAsync(FName DialogueId, FOnDialogueLoadedDelegate OnLoaded);

// Зарегистрировать путь к диалогу (для последующей загрузки)
UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
void RegisterDialogueAsset(FName DialogueId, const FSoftObjectPath& AssetPath);

// Отменить загрузку
UFUNCTION(BlueprintCallable, Category = "Dialogue|Loading")
void CancelAsyncLoad(FName DialogueId);

// Проверить, идет ли загрузка
UFUNCTION(BlueprintPure, Category = "Dialogue|Loading")
bool IsAsyncLoadPending(FName DialogueId) const;

// Получить прогресс загрузки (0.0 - 1.0)
UFUNCTION(BlueprintPure, Category = "Dialogue|Loading")
float GetAsyncLoadProgress(FName DialogueId) const;
```

#### Сохранение/Загрузка:

```cpp
// Сохранить состояние диалогов
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
bool SaveDialogueState();

// Загрузить состояние диалогов
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
bool LoadDialogueState();

// Проверить, есть ли активное сохранение
UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
bool HasActiveSaveState() const;

// Получить активное состояние сохранения
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
FDialogueSessionSaveData GetActiveSaveState() const;

// Очистить состояние сохранения
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
void ClearActiveSaveState();

// Был ли диалог пройден
UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
bool WasDialogueCompleted(FName DialogueId) const;

// Отметить диалог как пройденный
UFUNCTION(BlueprintCallable, Category = "Dialogue|SaveGame")
void MarkDialogueCompleted(FName DialogueId);

// Получить количество посещений узла
UFUNCTION(BlueprintPure, Category = "Dialogue|SaveGame")
int32 GetNodeVisitCount(FName DialogueId, FName NodeId) const;
```

#### Настройки:

```cpp
// Получить скорость "печатной машинки" (символов в секунду)
UFUNCTION(BlueprintPure, Category = "Dialogue|Settings")
float GetTypewriterSpeed() const;

// Установить скорость "печатной машинки"
UFUNCTION(BlueprintCallable, Category = "Dialogue|Settings")
void SetTypewriterSpeed(float NewSpeed);

// Включено ли автопродолжение
UFUNCTION(BlueprintPure, Category = "Dialogue|Settings")
bool IsAutoAdvanceEnabled() const;

// Включить/выключить автопродолжение
UFUNCTION(BlueprintCallable, Category = "Dialogue|Settings")
void SetAutoAdvanceEnabled(bool bEnabled);
```

#### Производительность (Pooling):

```cpp
// Получить статистику пула
UFUNCTION(BlueprintPure, Category = "Dialogue|Performance")
void GetPoolStats(int32& OutAvailable, int32& OutInUse, int32& OutTotal) const;

// Предварительно создать Runner'ы в пуле
UFUNCTION(BlueprintCallable, Category = "Dialogue|Performance")
void PreWarmRunnerPool(int32 Count);

// Очистить неиспользуемые Runner'ы
UFUNCTION(BlueprintCallable, Category = "Dialogue|Performance")
void TrimRunnerPool();
```

#### Использование в Blueprint:

**Базовый запуск диалога**:
```blueprintue
Event BeginPlay
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem (переменная)

// При взаимодействии с NPC
Event E Pressed
    ? DialogueSubsystem ? Start Dialogue
        Dialogue Asset: DA_Dialogue_Lianne
        Player: Self
        NPC: Get Actor of Class [BP_NPC_Lianne]
        Return Value ? ActiveDialogueRunner
```

**Асинхронная загрузка диалога**:
```blueprintue
Event BeginPlay
    ? DialogueSubsystem ? Register Dialogue Asset
        Dialogue Id: "Lianne_Main"
        Asset Path: /Game/Dialogues/DA_Dialogue_Lianne

// Позже, при необходимости
Event E Pressed
    ? DialogueSubsystem ? Load Dialogue Async
        Dialogue Id: "Lianne_Main"
        On Loaded: [Custom Event: OnDialogueLoaded]

// Custom Event: OnDialogueLoaded
    Loaded Dialogue (параметр)
    Success (параметр)
    ? Branch (если Success)
        True ? DialogueSubsystem ? Start Dialogue (Loaded Dialogue, Player, NPC)
        False ? Print String "Failed to load dialogue"
```

**Сохранение и загрузка**:
```blueprintue
// Сохранение при выходе из диалога
Event OnDialogueEnded
    ? DialogueSubsystem ? Save Dialogue State

// Загрузка при запуске игры
Event BeginPlay
    ? DialogueSubsystem ? Load Dialogue State
    ? Branch
        True ? Print String "Dialogue state loaded"
        False ? Print String "No saved state"
```

**Проверка прохождения диалога**:
```blueprintue
Event BeginPlay
    ? DialogueSubsystem ? Was Dialogue Completed
        Dialogue Id: "Tutorial"
    ? Branch
        True ? Display Advanced Dialogue
        False ? Display Tutorial Dialogue
```

#### Пример в C++:

```cpp
// Получение подсистемы
UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();

// Запуск диалога
UDialogueRunner* Runner = DialogueSubsystem->StartDialogue(
    DialogueAsset,
    PlayerActor,
    NPCActor
);

// Асинхронная загрузка
DialogueSubsystem->RegisterDialogueAsset(
    TEXT("Lianne_Main"),
    FSoftObjectPath(TEXT("/Game/Dialogues/DA_Dialogue_Lianne"))
);

FOnDialogueLoadedDelegate OnLoaded;
OnLoaded.BindLambda([this](UDialogueDataAsset* LoadedDialogue, bool bSuccess)
{
    if (bSuccess && LoadedDialogue)
    {
        DialogueSubsystem->StartDialogue(LoadedDialogue, PlayerActor, NPCActor);
    }
});

DialogueSubsystem->LoadDialogueAsync(TEXT("Lianne_Main"), OnLoaded);

// Сохранение
DialogueSubsystem->SaveDialogueState();

// Загрузка
bool bLoaded = DialogueSubsystem->LoadDialogueState();

// Проверка прохождения
bool bCompleted = DialogueSubsystem->WasDialogueCompleted(TEXT("Tutorial"));
```

---

### 2. UDialogueComponent

**Назначение**: Компонент для NPC, который хранит доступные диалоги, управляет взаимодействием с игроком и интегрируется с системами отношений и памяти.

#### Основные свойства:

```cpp
// Основной диалог NPC (fallback)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
TSoftObjectPtr<UDialogueDataAsset> DefaultDialogue;

// Дополнительные диалоги (по условиям/контексту)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
TMap<FName, TSoftObjectPtr<UDialogueDataAsset>> ConditionalDialogues;

// ID персонажа (для системы отношений)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
FName CharacterId;

// Отображаемое имя персонажа
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
FText DisplayName;

// Портрет для UI
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
TSoftObjectPtr<UTexture2D> Portrait;

// Можно ли начать диалог сейчас
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
bool bCanStartDialogue = true;

// Дистанция взаимодействия
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
float InteractionDistance = 300.0f;
```

#### Делегаты:

```cpp
// Срабатывает при старте диалога
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueStartedSignature OnDialogueStarted;

// Срабатывает при окончании диалога
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueEndedSignature OnDialogueEnded;
```

#### Основные методы:

```cpp
// Начать диалог с этим NPC
UFUNCTION(BlueprintCallable, Category = "Dialogue")
bool StartDialogue(AActor* Initiator);

// Выбрать подходящий диалог на основе контекста
UFUNCTION(BlueprintCallable, Category = "Dialogue")
UDialogueDataAsset* SelectDialogue(const UDialogueSessionContext* Context);

// Получить Relationship Component
UFUNCTION(BlueprintPure, Category = "Dialogue")
URelationshipComponent* GetRelationshipComponent() const;

// Получить активный Runner
UFUNCTION(BlueprintPure, Category = "Dialogue")
UDialogueRunner* GetActiveRunner() const;

// Диалог активен?
UFUNCTION(BlueprintPure, Category = "Dialogue")
bool IsInDialogue() const;

// Установить cooldown до следующего диалога
UFUNCTION(BlueprintCallable, Category = "Dialogue")
void SetDialogueCooldown(float Seconds);

// Получить Memory Component
UFUNCTION(BlueprintPure, Category = "Dialogue")
UNPCMemoryComponent* GetMemoryComponent() const;
```

#### Использование в Blueprint:

**Добавление компонента к NPC**:
```blueprintue
// В Blueprint NPC (например, BP_NPC_Lianne)

// 1. Добавить компонент
Components Panel ? Add Component ? DialogueComponent

// 2. Настроить в Details Panel:
DialogueComponent:
  - Character Id: "Lianne"
  - Display Name: "Лианна"
  - Portrait: T_Portrait_Lianne
  - Default Dialogue: DA_Dialogue_Lianne_Default
  - Conditional Dialogues:
      ["Morning"]: DA_Dialogue_Lianne_Morning
      ["Evening"]: DA_Dialogue_Lianne_Evening
  - Can Start Dialogue: true
  - Interaction Distance: 300.0
```

**Запуск диалога при взаимодействии**:
```blueprintue
// В BP_NPC_Lianne, Event Graph:

// Метод 1: Прямой вызов
Event E Pressed (Custom Input)
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    ? DialogueComp ? Start Dialogue
        Initiator: Get Player Controller ? Get Pawn

// Метод 2: Через интерфейс IDialogueParticipant
Event ActorBeginOverlap
    ? Cast to Player Character ? Player
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    ? DialogueComp ? Can Start Dialogue (Player)
    ? Branch
        True ? DialogueComp ? Start Dialogue (Player)
        False ? Print String "Cannot start dialogue now"
```

**Подписка на события**:
```blueprintue
// В BP_NPC_Lianne, Event Graph:

// Подписка на событие старта диалога
DialogueComponent ? On Dialogue Started
    ? Add Event [Handle Dialogue Started]
        Runner (параметр)
        ? Play Animation [Talk_Happy]
        ? Set AI State [InDialogue]

// Подписка на событие окончания диалога
DialogueComponent ? On Dialogue Ended
    ? Add Event [Handle Dialogue Ended]
        ? Stop Animation
        ? Set AI State [Idle]
```

**Выбор диалога на основе контекста**:
```blueprintue
// В BP_NPC_Lianne, Event Graph:

Event Tick
    ? Get Current Time of Day ? TimeTag
    ? DialogueComponent ? Select Dialogue
        Context: Create Context (Player, Self, TimeTag)
    ? Set Default Dialogue (Result)
```

#### Пример в C++:

```cpp
// Добавление компонента к NPC в C++
UDialogueComponent* DialogueComp = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
DialogueComp->CharacterId = FName(TEXT("Lianne"));
DialogueComp->DisplayName = FText::FromString(TEXT("Лианна"));
DialogueComp->DefaultDialogue = FSoftObjectPath(TEXT("/Game/Dialogues/DA_Dialogue_Lianne"));
DialogueComp->InteractionDistance = 300.0f;

// Добавление условного диалога
DialogueComp->ConditionalDialogues.Add(
    FName(TEXT("Morning")),
    FSoftObjectPath(TEXT("/Game/Dialogues/DA_Dialogue_Lianne_Morning"))
);

// Подписка на события
DialogueComp->OnDialogueStarted.AddDynamic(this, &AMyNPC::OnDialogueStarted);
DialogueComp->OnDialogueEnded.AddDynamic(this, &AMyNPC::OnDialogueEnded);

// Обработчики
void AMyNPC::OnDialogueStarted(UDialogueRunner* Runner)
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue started with %s"), *CharacterName);
    // Установить AI в режим диалога
}

void AMyNPC::OnDialogueEnded()
{
    UE_LOG(LogTemp, Log, TEXT("Dialogue ended"));
    // Вернуть AI в обычный режим
}

// Запуск диалога
bool AMyNPC::Interact(AActor* Initiator)
{
    UDialogueComponent* DialogueComp = FindComponentByClass<UDialogueComponent>();
    if (DialogueComp)
    {
        return DialogueComp->StartDialogue(Initiator);
    }
    return false;
}
```

---

### 3. URelationshipComponent

**Назначение**: Управление отношениями NPC с игроком (аффинити, доверие, дружба, романтика и т.д.).

#### Основные свойства:

```cpp
// Значения отношений (от -100 до 100)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Affinity = 0.0f;  // Привязанность

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Trust = 0.0f;  // Доверие

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Respect = 0.0f;  // Уважение

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Romance = 0.0f;  // Романтика

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
float Friendship = 0.0f;  // Дружба

// Текущий статус отношений
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
ERelationshipStatus Status = ERelationshipStatus::Neutral;
```

#### Основные методы:

```cpp
// Изменить значение отношения
UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyAffinity(float Delta);

UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyTrust(float Delta);

UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyRespect(float Delta);

UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyRomance(float Delta);

UFUNCTION(BlueprintCallable, Category = "Relationship")
void ModifyFriendship(float Delta);

// Установить значение отношения
UFUNCTION(BlueprintCallable, Category = "Relationship")
void SetAffinity(float Value);

// Получить значение отношения
UFUNCTION(BlueprintPure, Category = "Relationship")
float GetAffinity() const { return Affinity; }

// Получить статус отношений
UFUNCTION(BlueprintPure, Category = "Relationship")
ERelationshipStatus GetStatus() const { return Status; }

// Установить статус отношений
UFUNCTION(BlueprintCallable, Category = "Relationship")
void SetStatus(ERelationshipStatus NewStatus);

// Проверить, достигнуто ли минимальное значение
UFUNCTION(BlueprintPure, Category = "Relationship")
bool HasMinimumAffinity(float MinValue) const;

// Получить общий уровень отношений (среднее)
UFUNCTION(BlueprintPure, Category = "Relationship")
float GetOverallRelationship() const;
```

#### Статусы отношений:

```cpp
UENUM(BlueprintType)
enum class ERelationshipStatus : uint8
{
    Hostile,      // Враждебный
    Unfriendly,   // Недружелюбный
    Neutral,      // Нейтральный
    Friendly,     // Дружелюбный
    Close,        // Близкий друг
    Romantic,     // Романтический партнер
    Married       // Супруг/супруга
};
```

#### Использование в Blueprint:

**Добавление компонента**:
```blueprintue
// В BP_NPC_Lianne

Components Panel ? Add Component ? RelationshipComponent

// Настройка в Details Panel:
RelationshipComponent:
  - Affinity: 0.0
  - Trust: 0.0
  - Respect: 0.0
  - Romance: 0.0
  - Friendship: 0.0
  - Status: Neutral
```

**Изменение отношений в диалоге**:
```blueprintue
// В Dialogue Effect:
"affinity[Lianne] += 10"

// Или вручную в Event Graph:
Event OnPlayerGaveGift
    ? Get Component by Class [RelationshipComponent]
    ? Modify Affinity (10.0)
    ? Modify Romance (5.0)
```

**Проверка уровня отношений**:
```blueprintue
Event BeginPlay
    ? Get Component by Class [RelationshipComponent] ? RelComp
    ? RelComp ? Get Affinity ? Affinity
    ? Branch (Affinity >= 50)
        True ? Unlock Romantic Dialogue
        False ? Use Standard Dialogue
```

**Автоматическое обновление статуса**:
```blueprintue
Event Tick
    ? Get Component by Class [RelationshipComponent] ? RelComp
    ? RelComp ? Get Overall Relationship ? Overall
    ? Branch (Overall >= 80)
        True ? RelComp ? Set Status [Close]
    ? Branch (Overall >= 50)
        True ? RelComp ? Set Status [Friendly]
    ? Branch (Overall < 20)
        True ? RelComp ? Set Status [Unfriendly]
```

#### Пример в C++:

```cpp
// Добавление компонента
URelationshipComponent* RelComp = CreateDefaultSubobject<URelationshipComponent>(TEXT("RelationshipComponent"));

// Изменение отношений
RelComp->ModifyAffinity(10.0f);
RelComp->ModifyTrust(5.0f);

// Проверка уровня
if (RelComp->GetAffinity() >= 50.0f)
{
    // Разблокировать романтический контент
}

// Установка статуса
RelComp->SetStatus(ERelationshipStatus::Friendly);

// Получение общего уровня
float Overall = RelComp->GetOverallRelationship();
```

---

### 4. UNPCMemoryComponent

**Назначение**: Хранение памяти NPC о событиях, взаимодействиях и разговорах с игроком.

#### Основные методы:

```cpp
// Установить флаг памяти
UFUNCTION(BlueprintCallable, Category = "Memory")
void SetMemory(const FString& Key, bool Value);

// Получить флаг памяти
UFUNCTION(BlueprintPure, Category = "Memory")
bool GetMemory(const FString& Key, bool DefaultValue = false) const;

// Установить переменную памяти (число)
UFUNCTION(BlueprintCallable, Category = "Memory")
void SetMemoryVariable(const FString& Key, float Value);

// Получить переменную памяти
UFUNCTION(BlueprintPure, Category = "Memory")
float GetMemoryVariable(const FString& Key, float DefaultValue = 0.0f) const;

// Отметить узел диалога как посещенный
UFUNCTION(BlueprintCallable, Category = "Memory")
void MarkNodeVisited(const FString& NodeId);

// Был ли узел посещен
UFUNCTION(BlueprintPure, Category = "Memory")
bool WasNodeVisited(const FString& NodeId) const;

// Получить количество посещений узла
UFUNCTION(BlueprintPure, Category = "Memory")
int32 GetNodeVisitCount(const FString& NodeId) const;

// Очистить всю память
UFUNCTION(BlueprintCallable, Category = "Memory")
void ClearMemory();
```

#### Использование в Blueprint:

**Добавление компонента**:
```blueprintue
// В BP_NPC_Lianne

Components Panel ? Add Component ? NPCMemoryComponent
```

**Установка памяти**:
```blueprintue
// После события
Event OnQuestCompleted
    ? Get Component by Class [NPCMemoryComponent] ? MemoryComp
    ? MemoryComp ? Set Memory ("QuestCompleted", true)
    ? MemoryComp ? Set Memory Variable ("QuestReward", 500.0)
```

**Проверка памяти**:
```blueprintue
// В условии диалога
Event BeginPlay
    ? Get Component by Class [NPCMemoryComponent] ? MemoryComp
    ? MemoryComp ? Get Memory ("FirstMeeting")
    ? Branch
        True ? Load Dialogue [DA_Dialogue_Returning]
        False ? Load Dialogue [DA_Dialogue_First]
```

**Отслеживание посещений узлов**:
```blueprintue
// В DialogueRunner, OnNodeChanged
Event OnNodeChanged
    Node (параметр)
    ? Get Node Id ? NodeId
    ? Get Component by Class [NPCMemoryComponent] ? MemoryComp
    ? MemoryComp ? Mark Node Visited (NodeId)
    ? MemoryComp ? Get Node Visit Count (NodeId) ? Count
    ? Print String ("Visited {Count} times")
```

#### Пример в C++:

```cpp
// Добавление компонента
UNPCMemoryComponent* MemoryComp = CreateDefaultSubobject<UNPCMemoryComponent>(TEXT("MemoryComponent"));

// Установка памяти
MemoryComp->SetMemory(TEXT("FirstMeeting"), true);
MemoryComp->SetMemoryVariable(TEXT("GiftsGiven"), 3.0f);

// Проверка памяти
bool bFirstMeeting = MemoryComp->GetMemory(TEXT("FirstMeeting"));
float GiftsGiven = MemoryComp->GetMemoryVariable(TEXT("GiftsGiven"));

// Отметка узла
MemoryComp->MarkNodeVisited(TEXT("Node_001"));

// Проверка посещения
bool bVisited = MemoryComp->WasNodeVisited(TEXT("Node_001"));
int32 VisitCount = MemoryComp->GetNodeVisitCount(TEXT("Node_001"));
```

---

### 5. UDialoguePresentationComponent

**Назначение**: Управление визуальной презентацией диалога (анимации, эффекты, таймлайны).

#### Основные методы:

```cpp
// Показать текст диалога с эффектом "печатной машинки"
UFUNCTION(BlueprintCallable, Category = "Presentation")
void DisplayTextWithTypewriter(const FText& Text, float Speed);

// Показать выборы игрока
UFUNCTION(BlueprintCallable, Category = "Presentation")
void DisplayChoices(const TArray<FDialogueConnection>& Choices);

// Воспроизвести анимацию говорящего
UFUNCTION(BlueprintCallable, Category = "Presentation")
void PlaySpeakerAnimation(const FName& AnimationName, AActor* Speaker);

// Показать/скрыть портрет
UFUNCTION(BlueprintCallable, Category = "Presentation")
void ShowPortrait(UTexture2D* Portrait, bool bShow = true);

// Применить эффект появления/исчезновения
UFUNCTION(BlueprintCallable, Category = "Presentation")
void FadeIn(float Duration = 0.5f);

UFUNCTION(BlueprintCallable, Category = "Presentation")
void FadeOut(float Duration = 0.5f);
```

#### Использование в Blueprint:

**Добавление компонента**:
```blueprintue
// В Actor, который управляет UI диалога

Components Panel ? Add Component ? DialoguePresentationComponent
```

**Отображение текста с эффектом**:
```blueprintue
Event OnNodeChanged
    Node (параметр)
    ? Get Dialogue Text ? Text
    ? DialoguePresentationComponent ? Display Text With Typewriter
        Text: Text
        Speed: 30.0
```

**Отображение выборов**:
```blueprintue
Event OnChoicesUpdated
    Choices (параметр)
    ? DialoguePresentationComponent ? Display Choices
        Choices: Choices
```

**Воспроизведение анимации**:
```blueprintue
Event OnNodeChanged
    Node (параметр)
    ? Get Animation Name ? AnimName
    ? Get NPC Actor ? NPC
    ? DialoguePresentationComponent ? Play Speaker Animation
        Animation Name: AnimName
        Speaker: NPC
```

#### Пример в C++:

```cpp
// Создание компонента
UDialoguePresentationComponent* PresentationComp = CreateDefaultSubobject<UDialoguePresentationComponent>(TEXT("PresentationComponent"));

// Отображение текста
PresentationComp->DisplayTextWithTypewriter(DialogueText, 30.0f);

// Отображение выборов
PresentationComp->DisplayChoices(AvailableChoices);

// Воспроизведение анимации
PresentationComp->PlaySpeakerAnimation(FName(TEXT("Talk_Happy")), NPCActor);

// Показать портрет
PresentationComp->ShowPortrait(PortraitTexture, true);

// Эффекты
PresentationComp->FadeIn(0.5f);
```

---

### 6. UDialogueCameraComponent

**Назначение**: Управление камерой во время диалога (переключение между камерами, установка ракурсов).

#### Основные методы:

```cpp
// Установить камеру для узла диалога
UFUNCTION(BlueprintCallable, Category = "Camera")
void SetCameraForNode(const FName& CameraTag, float BlendTime = 1.0f);

// Вернуться к камере игрока
UFUNCTION(BlueprintCallable, Category = "Camera")
void RestorePlayerCamera(float BlendTime = 1.0f);

// Создать динамическую камеру для диалога
UFUNCTION(BlueprintCallable, Category = "Camera")
ACameraActor* CreateDialogueCamera(const FTransform& Transform);

// Установить фокус камеры на актора
UFUNCTION(BlueprintCallable, Category = "Camera")
void FocusOnActor(AActor* Target, float BlendTime = 1.0f);
```

#### Использование в Blueprint:

**Добавление компонента**:
```blueprintue
// В Actor, который управляет диалогом

Components Panel ? Add Component ? DialogueCameraComponent
```

**Установка камеры для узла**:
```blueprintue
Event OnNodeChanged
    Node (параметр)
    ? Get Camera Tag ? CameraTag
    ? DialogueCameraComponent ? Set Camera For Node
        Camera Tag: CameraTag
        Blend Time: 1.0
```

**Восстановление камеры игрока**:
```blueprintue
Event OnDialogueEnded
    ? DialogueCameraComponent ? Restore Player Camera
        Blend Time: 1.0
```

**Фокус на актора**:
```blueprintue
Event OnNodeChanged
    Node (параметр)
    ? Get Speaker ? Speaker
    ? DialogueCameraComponent ? Focus On Actor
        Target: Speaker
        Blend Time: 0.5
```

#### Пример в C++:

```cpp
// Создание компонента
UDialogueCameraComponent* CameraComp = CreateDefaultSubobject<UDialogueCameraComponent>(TEXT("CameraComponent"));

// Установка камеры
CameraComp->SetCameraForNode(FName(TEXT("Camera.Closeup")), 1.0f);

// Восстановление камеры
CameraComp->RestorePlayerCamera(1.0f);

// Фокус
CameraComp->FocusOnActor(NPCActor, 0.5f);
```

---

### 7. UDialogueInputBlockerComponent

**Назначение**: Блокировка игрового ввода во время диалога, чтобы игрок не мог двигаться или совершать другие действия.

#### Основные методы:

```cpp
// Заблокировать ввод
UFUNCTION(BlueprintCallable, Category = "Input")
void BlockInput();

// Разблокировать ввод
UFUNCTION(BlueprintCallable, Category = "Input")
void UnblockInput();

// Проверить, заблокирован ли ввод
UFUNCTION(BlueprintPure, Category = "Input")
bool IsInputBlocked() const;
```

#### Использование в Blueprint:

**Добавление компонента**:
```blueprintue
// В Actor игрока или контроллера

Components Panel ? Add Component ? DialogueInputBlockerComponent
```

**Блокировка ввода при старте диалога**:
```blueprintue
Event OnDialogueStarted
    ? DialogueInputBlockerComponent ? Block Input
```

**Разблокировка ввода при окончании диалога**:
```blueprintue
Event OnDialogueEnded
    ? DialogueInputBlockerComponent ? Unblock Input
```

#### Пример в C++:

```cpp
// Создание компонента
UDialogueInputBlockerComponent* InputBlocker = CreateDefaultSubobject<UDialogueInputBlockerComponent>(TEXT("InputBlocker"));

// Блокировка
InputBlocker->BlockInput();

// Разблокировка
InputBlocker->UnblockInput();

// Проверка
bool bBlocked = InputBlocker->IsInputBlocked();
```

---

## Пошаговые примеры использования

### Пример 1: Полная интеграция диалогов с NPC (Blueprint)

**Шаг 1. Настройка NPC**:
```blueprintue
// В BP_NPC_Lianne

// 1. Добавить компоненты
Components Panel:
  ? Add Component ? DialogueComponent
  ? Add Component ? RelationshipComponent
  ? Add Component ? NPCMemoryComponent

// 2. Настроить DialogueComponent
DialogueComponent (Details Panel):
  - Character Id: "Lianne"
  - Display Name: "Лианна"
  - Portrait: T_Portrait_Lianne
  - Default Dialogue: DA_Dialogue_Lianne_Default
  - Conditional Dialogues:
      ["Morning"]: DA_Dialogue_Lianne_Morning
      ["Evening"]: DA_Dialogue_Lianne_Evening
  - Can Start Dialogue: true
  - Interaction Distance: 300.0

// 3. Настроить RelationshipComponent
RelationshipComponent:
  - Affinity: 0.0
  - Trust: 0.0
  - Romance: 0.0
  - Status: Neutral
```

**Шаг 2. Настройка взаимодействия**:
```blueprintue
// В BP_NPC_Lianne, Event Graph:

// При приближении игрока
Event ActorBeginOverlap
    Other Actor (параметр)
    ? Cast to Player Character ? Player
    ? Branch (cast success)
        True ? Enable Input (Self)
            ? Print String "Press E to talk"

// При удалении игрока
Event ActorEndOverlap
    Other Actor (параметр)
    ? Cast to Player Character ? Player
    ? Branch (cast success)
        True ? Disable Input (Self)

// При нажатии E
Event E Pressed
    ? Get Component by Class [DialogueComponent] ? DialogueComp
    ? DialogueComp ? Start Dialogue
        Initiator: Get Player Controller ? Get Pawn
    ? Branch (return value)
        True ? Print String "Dialogue started"
        False ? Print String "Cannot start dialogue"
```

**Шаг 3. Обработка событий диалога**:
```blueprintue
// В BP_NPC_Lianne, Event Graph:

// Подписка на события
DialogueComponent ? On Dialogue Started
    ? Add Event [Handle Dialogue Started]
        Runner (параметр)
        ? Play Animation Montage [AM_Talk_Happy]
        ? Set AI State [InDialogue]
        ? Disable Movement

DialogueComponent ? On Dialogue Ended
    ? Add Event [Handle Dialogue Ended]
        ? Stop Animation Montage
        ? Set AI State [Idle]
        ? Enable Movement
```

**Шаг 4. Настройка UI**:
```blueprintue
// В WBP_DialogueWidget, Event Graph:

// При создании виджета
Event Construct
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
    ? DialogueSubsystem ? Get Active Dialogue ? ActiveDialogue
    ? Branch (ActiveDialogue != null)
        True ? Initialize Widget with ActiveDialogue

// Обновление при смене узла
Event OnNodeChanged (Custom)
    Node (параметр)
    ? Get Dialogue Text ? Text
    ? Set Text (Text Block Widget, Text)
    ? Get Speaker Name ? SpeakerName
    ? Set Text (Speaker Name Widget, SpeakerName)
    ? Get Speaker Portrait ? Portrait
    ? Set Brush from Texture (Image Widget, Portrait)

// Обработка кликов на кнопки выбора
Button Choice Click Event
    ? Get Button Index ? Index
    ? ActiveDialogue ? Select Choice (Index)
```

---

### Пример 2: Асинхронная загрузка и запуск диалога (Blueprint)

**Шаг 1. Регистрация диалогов при запуске**:
```blueprintue
// В BP_GameMode или BP_GameInstance, Event BeginPlay:

Event BeginPlay
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
    
    // Регистрируем диалоги для асинхронной загрузки
    ? DialogueSubsystem ? Register Dialogue Asset
        Dialogue Id: "Lianne_Main"
        Asset Path: /Game/Dialogues/Lianne/DA_Dialogue_Lianne_Main
    
    ? DialogueSubsystem ? Register Dialogue Asset
        Dialogue Id: "Lianne_Romance"
        Asset Path: /Game/Dialogues/Lianne/DA_Dialogue_Lianne_Romance
    
    ? DialogueSubsystem ? Register Dialogue Asset
        Dialogue Id: "Marcus_Main"
        Asset Path: /Game/Dialogues/Marcus/DA_Dialogue_Marcus_Main
```

**Шаг 2. Асинхронная загрузка при взаимодействии**:
```blueprintue
// В BP_NPC_Lianne, Event Graph:

Event E Pressed
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
    
    // Проверяем, идет ли уже загрузка
    ? DialogueSubsystem ? Is Async Load Pending ("Lianne_Main")
    ? Branch
        True ? Print String "Loading dialogue, please wait..."
        False:
            ? DialogueSubsystem ? Load Dialogue Async
                Dialogue Id: "Lianne_Main"
                On Loaded: [Bind to Event: OnDialogueLoaded]

// Custom Event: OnDialogueLoaded
Event OnDialogueLoaded
    Loaded Dialogue (параметр)
    Success (параметр)
    ? Branch (Success)
        True:
            ? Get Game Instance
                ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
            ? DialogueSubsystem ? Start Dialogue
                Dialogue Asset: Loaded Dialogue
                Player: Get Player Pawn
                NPC: Self
        False:
            ? Print String "Failed to load dialogue"
```

**Шаг 3. Отображение прогресса загрузки (опционально)**:
```blueprintue
// В WBP_LoadingDialogue, Event Tick:

Event Tick
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem] ? DialogueSubsystem
    ? DialogueSubsystem ? Get Async Load Progress ("Lianne_Main") ? Progress
    ? Set Percent (Progress Bar Widget, Progress)
    ? Branch (Progress >= 1.0)
        True ? Hide Loading Widget
```

---

### Пример 3: Сохранение и загрузка состояния диалогов (C++)

```cpp
// В вашем GameMode или SaveGame система

#include "Subsystems/DialogueSubsystem.h"
#include "Kismet/GameplayStatics.h"

void AMyGameMode::SaveGame()
{
    UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    
    // Сохранить состояние диалогов
    bool bSaved = DialogueSubsystem->SaveDialogueState();
    if (bSaved)
    {
        UE_LOG(LogTemp, Log, TEXT("Dialogue state saved successfully"));
    }
    
    // Получить JSON представление для дополнительного сохранения
    FString Json = DialogueSubsystem->SaveStateToJson();
    // Сохраните Json в ваш SaveGame объект
}

void AMyGameMode::LoadGame()
{
    UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    
    // Загрузить состояние диалогов
    bool bLoaded = DialogueSubsystem->LoadDialogueState();
    if (bLoaded)
    {
        UE_LOG(LogTemp, Log, TEXT("Dialogue state loaded successfully"));
    }
    
    // Или загрузить из JSON
    // FString Json = LoadJsonFromSaveGame();
    // DialogueSubsystem->LoadStateFromJson(Json);
}

void AMyGameMode::CheckDialogueCompletion()
{
    UDialogueSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    
    // Проверить, был ли пройден диалог
    bool bCompleted = DialogueSubsystem->WasDialogueCompleted(FName(TEXT("Tutorial")));
    if (bCompleted)
    {
        // Разблокировать следующий контент
        UnlockNextLevel();
    }
}
```

---

### Пример 4: Интеграция с системой отношений (Blueprint + C++)

**В Blueprint (изменение отношений)**:
```blueprintue
// В Data Asset диалога, настройка Effect:

Connection[0]:
  - Effect: "affinity[Lianne] += 10; trust[Lianne] += 5"

// Или вручную в Event Graph:
Event OnPlayerGaveGift
    ? Get Actor of Class [BP_NPC_Lianne] ? Lianne
    ? Lianne ? Get Component by Class [RelationshipComponent] ? RelComp
    ? RelComp ? Modify Affinity (10.0)
    ? RelComp ? Modify Trust (5.0)
    ? RelComp ? Modify Romance (3.0)
```

**В C++ (реализация IDialogueService для отношений)**:
```cpp
// В вашем DialogueService класс

float UMyDialogueService::GetRelationshipValue(const FString& NPCId, const FString& RelationType) const
{
    // Найти NPC по ID
    AActor* NPCActor = FindNPCById(NPCId);
    if (!NPCActor)
    {
        return 0.0f;
    }
    
    // Получить RelationshipComponent
    URelationshipComponent* RelComp = NPCActor->FindComponentByClass<URelationshipComponent>();
    if (!RelComp)
    {
        return 0.0f;
    }
    
    // Вернуть нужное значение
    if (RelationType == TEXT("affinity"))
    {
        return RelComp->GetAffinity();
    }
    else if (RelationType == TEXT("trust"))
    {
        return RelComp->GetTrust();
    }
    else if (RelationType == TEXT("romance"))
    {
        return RelComp->GetRomance();
    }
    
    return 0.0f;
}

void UMyDialogueService::ModifyRelationship(const FString& NPCId, const FString& RelationType, float Delta)
{
    AActor* NPCActor = FindNPCById(NPCId);
    if (!NPCActor)
    {
        return;
    }
    
    URelationshipComponent* RelComp = NPCActor->FindComponentByClass<URelationshipComponent>();
    if (!RelComp)
    {
        return;
    }
    
    if (RelationType == TEXT("affinity"))
    {
        RelComp->ModifyAffinity(Delta);
    }
    else if (RelationType == TEXT("trust"))
    {
        RelComp->ModifyTrust(Delta);
    }
    else if (RelationType == TEXT("romance"))
    {
        RelComp->ModifyRomance(Delta);
    }
}
```

---

## Лучшие практики

1. **Используйте DialogueSubsystem**: Не создавайте DialogueRunner вручную, используйте Subsystem для получения преимуществ пулинга и управления.

2. **Регистрируйте диалоги заранее**: Зарегистрируйте все диалоги при старте игры для быстрой асинхронной загрузки.

3. **Сохраняйте часто**: Используйте автосохранение или сохраняйте состояние диалогов при каждом важном событии.

4. **Используйте компоненты**: DialogueComponent, RelationshipComponent и NPCMemoryComponent обеспечивают модульность и переиспользование.

5. **Блокируйте ввод**: Всегда блокируйте игровой ввод во время диалога, чтобы избежать нежелательных действий.

6. **Управляйте камерой**: Используйте DialogueCameraComponent для плавных переходов камеры во время диалога.

7. **Оптимизируйте UI**: Используйте DialoguePresentationComponent для стандартизации UI презентации.

---

## Заключение

**DialogueSystemRuntime** предоставляет полный набор компонентов и подсистем для интеграции диалогов в вашу игру. Благодаря модульной архитектуре, dependency injection и системе компонентов, вы можете легко настроить и расширить систему под свои нужды.

Для получения дополнительной информации см.:
- `01_DialogueSystemCore_Module.md` — Ядро системы (Data, Conditions, Effects)
- `03_DialogueSystemUI_Module.md` — UI виджеты для диалогов
- `04_DialogueSystemAI_Module.md` — Интеграция с AI
