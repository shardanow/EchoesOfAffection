# NPC Memory Component - �����������

## �����

`UNPCMemoryComponent` - ��� ��������� ������������ ������ ��� NPC, ������� ��������� �� ���������� ������ �������, ������ ������ � ���� ����������. ������ ����������� �� ��������, �� ������ � ������������ ���������� ������� �������� ������.

## �������� �����������

### 1. ���� ������������

- **DialogueEvent** - ������� �� ��������
- **PlayerChoice** - ������ ������ ������
- **ItemGiven** - ������� � ��������
- **QuestEvent** - ��������� �������
- **WorldEvent** - ������ ������� �������
- **Promise** - �������� (������� ��������)
- **Secret** - ������� (����� ������� ��������)
- **PersonalMemory** - ������ ������������ NPC

### 2. ������������� �������

- **Neutral** - �����������
- **Positive** - ����������
- **Negative** - ����������
- **Bittersweet** - ������-�������
- **Traumatic** - ������������ (�������� ����� �����)
- **Joyful** - ���������
- **Regretful** - ������ ���������
- **Nostalgic** - ���������������

## ������� �������������

### ���������� ���������� � Blueprint

1. �������� Blueprint ������ NPC
2. �������� ��������� `NPCMemoryComponent`
3. ��������� ��������� � Details ������:
   - `Max Memories` - ������������ ���������� ������������ (�� ��������� 100)
   - `Enable Memory Decay` - �������� �������������� ���������
   - `Base Decay Rate` - ������� �������� ��������� (�� ��������� 0.05)
   - `Memory Retention Multiplier` - ��������� ������ NPC ������ (1.0 = �����)

### �������� ������������ �� C++

```cpp
// ������� ������
FName MemoryId = MemoryComponent->CreateMemory(
    EMemoryType::DialogueEvent,
    FText::FromString("Player asked about the war"),
    60.0f,                        // ��������
    EMemoryEmotion::Nostalgic,
    PlayerActor                   // ��������� �����
);

// ��������� ������
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

### �������� ������������ �� Blueprint

```
Create Memory
?? Memory Type: Player Choice
?? Description: "Brought me flowers"
?? Importance: 70
?? Emotion: Joyful
?? Related Actor: [Player Reference]
```

## ����� � ������������� ������������

### ����� ����������� ������������

```cpp
// ������� �������� ���������
FGameplayTagContainer ContextTags;
ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Topic.Family")));

// ���� ����������� ������������
TArray<FNPCMemoryEntry> Memories = MemoryComponent->FindRelevantMemories(
    ContextTags,
    PlayerActor,
    5  // �������� �����������
);

// ���������� � �������
for (const FNPCMemoryEntry& Memory : Memories)
{
    // NPC ��������� ������������ � �������
}
```

### �������� ����������� ������������

```cpp
if (MemoryComponent->RemembersEvent(FName("FirstMeeting")))
{
    // NPC ������ ������ �������
    DialogueText = FText::FromString("I remember when we first met...");
}
```

### ��������� ������������ �� ���������

```cpp
// �� ����
TArray<FNPCMemoryEntry> Promises = MemoryComponent->GetMemoriesByType(EMemoryType::Promise);

// �� ������
TArray<FNPCMemoryEntry> TraumaticMemories = MemoryComponent->GetMemoriesByEmotion(EMemoryEmotion::Traumatic);

// �� ������������ ������
TArray<FNPCMemoryEntry> PlayerMemories = MemoryComponent->GetMemoriesAboutActor(PlayerActor);

// � ������������� ������
FGameplayTagContainer Tags;
Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Gift")));
TArray<FNPCMemoryEntry> GiftMemories = MemoryComponent->GetMemoriesWithTags(Tags);
```

## ���� ����������

### ������������ ���

```cpp
// �������� ���������� ����
MemoryComponent->DiscussTopic(FName("Politics"), 5.0f);  // +5 � ��������

// ��������� ���� �� ���� ���������
if (MemoryComponent->WasTopicDiscussed(FName("Politics")))
{
    // �������� ������
}

// �������� ���������� ����
TArray<FConversationTopic> InterestingTopics = MemoryComponent->GetInterestingTopics(3);
```

## ������������� � ��������

### ������� (Conditions)

� DialogueNode ����� ������������ ��������� �������:

#### Remembers Event
��������� ������ �� NPC �������:
```
Memory ID: "PlayerSavedMe"
Min Importance: 50.0
Min Freshness: 0.3
```

#### Memory Count Check
��������� ���������� ������������:
```
Memory Type: Player Choice
Min Count: 3
Max Count: 10
```

#### Topic Discussed
��������� ���� �� ��������� ����:
```
Topic Name: "Family"
Min Discussion Count: 2
Max Days Since Discussion: 7.0
```

#### Has Memories With Tags
��������� ������� ������������ � ������:
```
Required Tags: Event.Gift, Emotion.Happy
Require All Tags: false
Min Memories Found: 1
```

#### Memory Emotional Balance
��������� ������������� ������:
```
Min Positive Memories: 3
Max Negative Memories: 1
```

### ������� (Effects)

� DialogueNode ����� ������������ ��������� �������:

#### Create Memory
������� ������������:
```
Memory Type: Dialogue Event
Description: "Talked about the war"
Importance: 60.0
Emotion: Nostalgic
Remember Player: true
```

#### Remember Promise
���������� ��������:
```
Promise ID: "HelpWithQuest"
Description: "Player promised to help find lost artifact"
Importance: 90.0
Related Tags: Quest.MainStory
```

#### Remember Secret
���������� ������:
```
Secret ID: "PlayerIdentity"
Description: "Player revealed their true identity"
Importance: 95.0
Secret Tags: Secret.Identity, Important
```

#### Discuss Topic
�������� ���������� ����:
```
Topic Name: "Politics"
Interest Modifier: -5.0  // NPC �� ����� ��������
```

## ��������� �������� NPC

### ��������� ������

```cpp
// NPC � �������� ������� (������, ������)
MemoryComponent->MemoryRetentionMultiplier = 2.0f;
MemoryComponent->BaseDecayRate = 0.02f;

// NPC � ������ ������� (������, �������)
MemoryComponent->MemoryRetentionMultiplier = 0.5f;
MemoryComponent->BaseDecayRate = 0.1f;

// ��������� NPC (������ ������ ������)
MemoryComponent->NegativeBias = 2.0f;

// �������� (������ �������)
MemoryComponent->NegativeBias = 0.5f;
```

### ���������� ����� ������

```cpp
// ������� NPC - ������ ��������
TypePreferences.Add(EMemoryType::Promise, 3.0f);

// �������� - ������ �������
TypePreferences.Add(EMemoryType::Secret, 2.5f);

// ����������� - ������ �������
TypePreferences.Add(EMemoryType::ItemGiven, 2.0f);
```

## ���������� � ������� ���������

### � Relationship Component

```cpp
// ������� ���������� ������������
FNPCMemoryEntry Memory;
Memory.Type = EMemoryType::PlayerChoice;
Memory.Description = FText::FromString("Player betrayed my trust");
Memory.Emotion = EMemoryEmotion::Traumatic;
Memory.Importance = 90.0f;
MemoryComponent->AddMemory(Memory);

// ������ �� ���������
RelationshipComponent->ModifyAffinity(-30.0f);
RelationshipComponent->ModifyTrust(-50.0f);
```

### � Quest System

```cpp
// ��� ������ ������
FNPCMemoryEntry QuestMemory;
QuestMemory.Type = EMemoryType::Promise;
QuestMemory.Description = FText::FromString("Agreed to help with the quest");
QuestMemory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Quest.MainQuest_01")));
QuestMemory.MetaData = FString::Printf(TEXT("{\"QuestId\":\"%s\"}"), *QuestId.ToString());
MemoryComponent->AddMemory(QuestMemory);

// ��� ���������� ������
if (bQuestCompleted)
{
    // ��������� �������� - ����� �������� ��������
    // ��� ������� ����� ���������� ������������
}
```

## ������������������

### �����������

1. **����� ������������**: �������������� �������� `MaxMemories` (50-150)
2. **�������� ����������**: `DecayUpdateInterval` ������������ ������� ��������� ���������
3. **���������� Tick**: ���� �� ����� decay � �������� �������, ��������� `bEnableMemoryDecay`

### Batch ��������

```cpp
// �������� ������ ������������
MemoryComponent->PruneOldMemories(30.0f);  // ������ 30 ����

// ������� ���� ������
MemoryComponent->ClearAllMemories();
```

## ���������� � ��������

### JSON ������������

```cpp
// ����������
FString JsonData;
MemoryComponent->SaveToJson(JsonData);
// ��������� JsonData � SaveGame

// ��������
bool bSuccess = MemoryComponent->LoadFromJson(JsonData);
```

### ���������� � SaveGame

```cpp
// � ����� SaveGame ������
UPROPERTY()
FString NPCMemoryData;

// ��� ����������
MemoryComponent->SaveToJson(SaveGameInstance->NPCMemoryData);

// ��� ��������
MemoryComponent->LoadFromJson(SaveGameInstance->NPCMemoryData);
```

## Best Practices

1. **�������� ������������**:
   - ������� �������: 30-50
   - ������ ������: 60-80
   - �������� � �������: 85-100

2. **������������� �����**:
   - ������ ���������� ����������� ���� ��� ������
   - ����������� ��������: `Event.Gift.Flower`

3. **MetaData**:
   - ����������� JSON ��� ������� ������
   - ������� ID �������, ��������� � �.�.

4. **������**:
   - ������������ ������� �������� ������� ������
   - ����������� ���������� ������� �����

5. **����������**:
   - ���������� ������ � RelationshipComponent
   - ����������� ������ ��� ������������ ��������� ��������
   - ���������� ������ � Condition'�� ����� ������� �����

## ������� ���������

### �������� 1: �������� ������

```cpp
// NPC ������ ������
Effect: Remember Promise
- Promise ID: "FindLostSister"
- Description: "Player promised to find my lost sister"
- Importance: 95.0

// ����� ��������� ����� �������� �������
Condition: Remembers Event("FindLostSister")
Node: "Have you found my sister yet?"

// ���� ����� ��������
Effect: Create Memory
- Type: Player Choice
- Description: "Player found my sister"
- Emotion: Joyful
- Importance: 100.0
```

### �������� 2: ������������� ����

```cpp
// ������ �������� � �����
Effect: Discuss Topic("Family", 10.0)

// ������ ��������
Condition: Topic Discussed("Family", MinCount: 1)
Node: "We already talked about this..."

Condition: Topic Discussed("Family", MinCount: 3)
Node: "I don't want to discuss this anymore"
```

### �������� 3: ������������ �����������

```cpp
// � BeginPlay NPC ��� ��� �������
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

## �������

### �����������

��������� ���������� ��������� ������ �����:

- `Log` - ������ ������� (��������/�������� ������)
- `Verbose` - ��������� ���������� (�����������, decay)
- `Warning` - �������������� (���������� ����������)

### Debug �������

```cpp
// ������� ��� ������������
for (const FNPCMemoryEntry& Memory : MemoryComponent->GetAllMemories())
{
    UE_LOG(LogTemp, Log, TEXT("Memory: %s (Importance: %.1f, Freshness: %.2f)"),
        *Memory.Description.ToString(),
        Memory.Importance,
        Memory.Freshness);
}

// ����������
UE_LOG(LogTemp, Log, TEXT("Total Memories: %d"), MemoryComponent->GetMemoryCount());
UE_LOG(LogTemp, Log, TEXT("Topics Discussed: %d"), MemoryComponent->GetAllTopics().Num());
```

## ����������

NPCMemoryComponent ������������� ������ � ������ ������� ������������ ������ ��� NPC, ������� ��������� ��������� ����� � �������������� ����������. ����������� � ������ � RelationshipComponent � DialogueSystem ��� �������� �������� ������� ��������� � ���������� ��������.
