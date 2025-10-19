# NPC Memory Component - Руководство

## Обзор

`UNPCMemoryComponent` - это компонент долгосрочной памяти для NPC, который позволяет им запоминать важные события, выборы игрока и темы разговоров. Память деградирует со временем, но важные и эмоционально окрашенные события помнятся дольше.

## Основные возможности

### 1. Типы воспоминаний

- **DialogueEvent** - События из диалогов
- **PlayerChoice** - Важные выборы игрока
- **ItemGiven** - Подарки и предметы
- **QuestEvent** - Квестовые события
- **WorldEvent** - Важные мировые события
- **Promise** - Обещания (высокая важность)
- **Secret** - Секреты (очень высокая важность)
- **PersonalMemory** - Личные воспоминания NPC

### 2. Эмоциональная окраска

- **Neutral** - Нейтральное
- **Positive** - Позитивное
- **Negative** - Негативное
- **Bittersweet** - Горько-сладкое
- **Traumatic** - Травматичное (помнится очень долго)
- **Joyful** - Радостное
- **Regretful** - Полное сожаления
- **Nostalgic** - Ностальгическое

## Базовое использование

### Добавление компонента в Blueprint

1. Откройте Blueprint вашего NPC
2. Добавьте компонент `NPCMemoryComponent`
3. Настройте параметры в Details панели:
   - `Max Memories` - максимальное количество воспоминаний (по умолчанию 100)
   - `Enable Memory Decay` - включить автоматическое забывание
   - `Base Decay Rate` - базовая скорость забывания (по умолчанию 0.05)
   - `Memory Retention Multiplier` - насколько хорошо NPC помнит (1.0 = норма)

### Создание воспоминания из C++

```cpp
// Простой способ
FName MemoryId = MemoryComponent->CreateMemory(
    EMemoryType::DialogueEvent,
    FText::FromString("Player asked about the war"),
    60.0f,                        // Важность
    EMemoryEmotion::Nostalgic,
    PlayerActor                   // Связанный актер
);

// Детальный способ
FNPCMemoryEntry Memory;
Memory.Type = EMemoryType::PlayerChoice;
Memory.Description = FText::FromString("Player chose to spare the enemy");
Memory.Importance = 85.0f;
Memory.Emotion = EMemoryEmotion::Positive;
Memory.RelatedActor = PlayerActor;
Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Choice.Mercy")));
Memory.MetaData = TEXT("{\"QuestId\":\"MainQuest_01\"}");

MemoryComponent->AddMemory(Memory);
```

### Создание воспоминания из Blueprint

```
Create Memory
?? Memory Type: Player Choice
?? Description: "Brought me flowers"
?? Importance: 70
?? Emotion: Joyful
?? Related Actor: [Player Reference]
```

## Поиск и использование воспоминаний

### Поиск релевантных воспоминаний

```cpp
// Создаем контекст разговора
FGameplayTagContainer ContextTags;
ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Topic.Family")));

// Ищем релевантные воспоминания
TArray<FNPCMemoryEntry> Memories = MemoryComponent->FindRelevantMemories(
    ContextTags,
    PlayerActor,
    5  // Максимум результатов
);

// Используем в диалоге
for (const FNPCMemoryEntry& Memory : Memories)
{
    // NPC упоминает воспоминание в диалоге
}
```

### Проверка конкретного воспоминания

```cpp
if (MemoryComponent->RemembersEvent(FName("FirstMeeting")))
{
    // NPC помнит первую встречу
    DialogueText = FText::FromString("I remember when we first met...");
}
```

### Получение воспоминаний по критериям

```cpp
// По типу
TArray<FNPCMemoryEntry> Promises = MemoryComponent->GetMemoriesByType(EMemoryType::Promise);

// По эмоции
TArray<FNPCMemoryEntry> TraumaticMemories = MemoryComponent->GetMemoriesByEmotion(EMemoryEmotion::Traumatic);

// Об определенном акторе
TArray<FNPCMemoryEntry> PlayerMemories = MemoryComponent->GetMemoriesAboutActor(PlayerActor);

// С определенными тегами
FGameplayTagContainer Tags;
Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Gift")));
TArray<FNPCMemoryEntry> GiftMemories = MemoryComponent->GetMemoriesWithTags(Tags);
```

## Темы разговоров

### Отслеживание тем

```cpp
// Отметить обсуждение темы
MemoryComponent->DiscussTopic(FName("Politics"), 5.0f);  // +5 к интересу

// Проверить была ли тема обсуждена
if (MemoryComponent->WasTopicDiscussed(FName("Politics")))
{
    // Изменить диалог
}

// Получить интересные темы
TArray<FConversationTopic> InterestingTopics = MemoryComponent->GetInterestingTopics(3);
```

## Использование в диалогах

### Условия (Conditions)

В DialogueNode можно использовать следующие условия:

#### Remembers Event
Проверяет помнит ли NPC событие:
```
Memory ID: "PlayerSavedMe"
Min Importance: 50.0
Min Freshness: 0.3
```

#### Memory Count Check
Проверяет количество воспоминаний:
```
Memory Type: Player Choice
Min Count: 3
Max Count: 10
```

#### Topic Discussed
Проверяет была ли обсуждена тема:
```
Topic Name: "Family"
Min Discussion Count: 2
Max Days Since Discussion: 7.0
```

#### Has Memories With Tags
Проверяет наличие воспоминаний с тегами:
```
Required Tags: Event.Gift, Emotion.Happy
Require All Tags: false
Min Memories Found: 1
```

#### Memory Emotional Balance
Проверяет эмоциональный баланс:
```
Min Positive Memories: 3
Max Negative Memories: 1
```

### Эффекты (Effects)

В DialogueNode можно использовать следующие эффекты:

#### Create Memory
Создает воспоминание:
```
Memory Type: Dialogue Event
Description: "Talked about the war"
Importance: 60.0
Emotion: Nostalgic
Remember Player: true
```

#### Remember Promise
Записывает обещание:
```
Promise ID: "HelpWithQuest"
Description: "Player promised to help find lost artifact"
Importance: 90.0
Related Tags: Quest.MainStory
```

#### Remember Secret
Записывает секрет:
```
Secret ID: "PlayerIdentity"
Description: "Player revealed their true identity"
Importance: 95.0
Secret Tags: Secret.Identity, Important
```

#### Discuss Topic
Отмечает обсуждение темы:
```
Topic Name: "Politics"
Interest Modifier: -5.0  // NPC не любит политику
```

## Настройка личности NPC

### Параметры памяти

```cpp
// NPC с отличной памятью (ученый, мудрец)
MemoryComponent->MemoryRetentionMultiplier = 2.0f;
MemoryComponent->BaseDecayRate = 0.02f;

// NPC с плохой памятью (старик, пьяница)
MemoryComponent->MemoryRetentionMultiplier = 0.5f;
MemoryComponent->BaseDecayRate = 0.1f;

// Обидчивый NPC (помнит плохое дольше)
MemoryComponent->NegativeBias = 2.0f;

// Оптимист (помнит хорошее)
MemoryComponent->NegativeBias = 0.5f;
```

### Приоритеты типов памяти

```cpp
// Честный NPC - помнит обещания
TypePreferences.Add(EMemoryType::Promise, 3.0f);

// Сплетник - помнит секреты
TypePreferences.Add(EMemoryType::Secret, 2.5f);

// Материалист - помнит подарки
TypePreferences.Add(EMemoryType::ItemGiven, 2.0f);
```

## Интеграция с другими системами

### С Relationship Component

```cpp
// Создаем негативное воспоминание
FNPCMemoryEntry Memory;
Memory.Type = EMemoryType::PlayerChoice;
Memory.Description = FText::FromString("Player betrayed my trust");
Memory.Emotion = EMemoryEmotion::Traumatic;
Memory.Importance = 90.0f;
MemoryComponent->AddMemory(Memory);

// Влияем на отношения
RelationshipComponent->ModifyAffinity(-30.0f);
RelationshipComponent->ModifyTrust(-50.0f);
```

### С Quest System

```cpp
// При начале квеста
FNPCMemoryEntry QuestMemory;
QuestMemory.Type = EMemoryType::Promise;
QuestMemory.Description = FText::FromString("Agreed to help with the quest");
QuestMemory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Quest.MainQuest_01")));
QuestMemory.MetaData = FString::Printf(TEXT("{\"QuestId\":\"%s\"}"), *QuestId.ToString());
MemoryComponent->AddMemory(QuestMemory);

// При завершении квеста
if (bQuestCompleted)
{
    // Обновляем важность - игрок выполнил обещание
    // Или создаем новое позитивное воспоминание
}
```

## Производительность

### Оптимизация

1. **Лимит воспоминаний**: Устанавливайте разумный `MaxMemories` (50-150)
2. **Интервал обновления**: `DecayUpdateInterval` контролирует частоту обработки забывания
3. **Отключение Tick**: Если не нужен decay в реальном времени, отключите `bEnableMemoryDecay`

### Batch операции

```cpp
// Удаление старых воспоминаний
MemoryComponent->PruneOldMemories(30.0f);  // Старше 30 дней

// Очистка всей памяти
MemoryComponent->ClearAllMemories();
```

## Сохранение и загрузка

### JSON сериализация

```cpp
// Сохранение
FString JsonData;
MemoryComponent->SaveToJson(JsonData);
// Сохраните JsonData в SaveGame

// Загрузка
bool bSuccess = MemoryComponent->LoadFromJson(JsonData);
```

### Интеграция с SaveGame

```cpp
// В вашем SaveGame классе
UPROPERTY()
FString NPCMemoryData;

// При сохранении
MemoryComponent->SaveToJson(SaveGameInstance->NPCMemoryData);

// При загрузке
MemoryComponent->LoadFromJson(SaveGameInstance->NPCMemoryData);
```

## Best Practices

1. **Важность воспоминаний**:
   - Обычные диалоги: 30-50
   - Важные выборы: 60-80
   - Обещания и секреты: 85-100

2. **Использование тегов**:
   - Всегда добавляйте релевантные теги для поиска
   - Используйте иерархию: `Event.Gift.Flower`

3. **MetaData**:
   - Используйте JSON для сложных данных
   - Храните ID квестов, предметов и т.д.

4. **Эмоции**:
   - Травматичные события помнятся намного дольше
   - Нейтральные забываются быстрее всего

5. **Интеграция**:
   - Связывайте память с RelationshipComponent
   - Используйте память для динамической генерации диалогов
   - Проверяйте память в Condition'ах перед показом узлов

## Примеры сценариев

### Сценарий 1: Обещание игрока

```cpp
// NPC просит помощи
Effect: Remember Promise
- Promise ID: "FindLostSister"
- Description: "Player promised to find my lost sister"
- Importance: 95.0

// Через несколько часов игрового времени
Condition: Remembers Event("FindLostSister")
Node: "Have you found my sister yet?"

// Если игрок выполнил
Effect: Create Memory
- Type: Player Choice
- Description: "Player found my sister"
- Emotion: Joyful
- Importance: 100.0
```

### Сценарий 2: Повторяющиеся темы

```cpp
// Первый разговор о семье
Effect: Discuss Topic("Family", 10.0)

// Второй разговор
Condition: Topic Discussed("Family", MinCount: 1)
Node: "We already talked about this..."

Condition: Topic Discussed("Family", MinCount: 3)
Node: "I don't want to discuss this anymore"
```

### Сценарий 3: Динамическое приветствие

```cpp
// В BeginPlay NPC или при встрече
TArray<FNPCMemoryEntry> RecentMemories = MemoryComponent->GetRecentMemories(1);
if (RecentMemories.Num() > 0)
{
    switch (RecentMemories[0].Emotion)
    {
        case EMemoryEmotion::Joyful:
            Greeting = "It's great to see you again!";
            break;
        case EMemoryEmotion::Traumatic:
            Greeting = "You... What do you want?";
            break;
        default:
            Greeting = "Hello.";
    }
}
```

## Отладка

### Логирование

Компонент использует следующие уровни логов:

- `Log` - Важные события (создание/удаление памяти)
- `Verbose` - Детальная информация (вспоминание, decay)
- `Warning` - Предупреждения (отсутствие компонента)

### Debug команды

```cpp
// Вывести все воспоминания
for (const FNPCMemoryEntry& Memory : MemoryComponent->GetAllMemories())
{
    UE_LOG(LogTemp, Log, TEXT("Memory: %s (Importance: %.1f, Freshness: %.2f)"),
        *Memory.Description.ToString(),
        Memory.Importance,
        Memory.Freshness);
}

// Статистика
UE_LOG(LogTemp, Log, TEXT("Total Memories: %d"), MemoryComponent->GetMemoryCount());
UE_LOG(LogTemp, Log, TEXT("Topics Discussed: %d"), MemoryComponent->GetAllTopics().Num());
```

## Заключение

NPCMemoryComponent предоставляет мощную и гибкую систему долгосрочной памяти для NPC, которая позволяет создавать живых и запоминающихся персонажей. Используйте её вместе с RelationshipComponent и DialogueSystem для создания глубокой системы отношений и динамичных диалогов.
