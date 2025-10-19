# NPC Memory Component

## ?? ������� ��������

**NPCMemoryComponent** - ��� ��������� ��������� ������������ ������ ��� NPC � Unreal Engine 5, ������������� ��� �������� ����� � �������������� ���������� � RPG � ����������� ���������.

## ? �������� �����������

- ?? **������������ ������** - NPC ������ ������ �������, ������ ������ � ���� ����������
- ?? **������� ���������** - ������ ����������� ����������� �� �������� (� ������ �������� � ������)
- ?? **������������� �������** - ������� � �������� �������� �������� ������
- ?? **��������������** - ��������� ������ ��� �������� NPC (�������/������ ������, ��������/���������)
- ?? **���������������� �����** - ����������� ����� ������������ �� ���������
- ??? **������������ ���** - �������������� ������������ ����������� ��� � �������� � ���
- ?? **����������** - ������ ���������� � RelationshipComponent � DialogueSystem
- ?? **����������** - ������ ��������� ����������/�������� ����� JSON

## ?? ����� ����������

```
Plugins/DialogueSystem/Source/DialogueSystemRuntime/
??? Public/Components/
?   ??? NPCMemoryComponent.h          # �������� ���������
?   ??? NPCMemoryExamples.h           # ������� �������������
?   ??? NPCMemoryTestHelpers.h        # �������� �������
??? Private/Components/
    ??? NPCMemoryComponent.cpp        # ����������

Plugins/DialogueSystem/Source/DialogueSystemCore/
??? Public/Conditions/
?   ??? MemoryConditions.h            # ������� ��� ��������
??? Public/Effects/
    ??? MemoryEffects.h               # ������� ��� ��������

Plugins/DialogueSystem/Documentation/
??? NPCMemoryComponent_Guide.md       # ������ �����������
??? Blueprint_Setup_Guide.md          # Blueprint �������
```

## ?? ������� �����

### 1. ���������� ����������

```cpp
// � ������������ ������ NPC
MemoryComponent = CreateDefaultSubobject<UNPCMemoryComponent>(TEXT("MemoryComponent"));
```

��� ����� Blueprint:
1. �������� Blueprint NPC
2. �������� ��������� `NPCMemoryComponent`
3. ��������� ��������� � Details ������

### 2. �������� ������������

```cpp
// ������� ������
MemoryComponent->CreateMemory(
    EMemoryType::DialogueEvent,
    FText::FromString("Player helped defend the village"),
    75.0f,                        // ��������
    EMemoryEmotion::Joyful,
    PlayerActor
);
```

### 3. ����� ������������

```cpp
// ����� ����������� ������������ ��� �������� ���������
FGameplayTagContainer ContextTags;
ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Topic.Family")));

TArray<FNPCMemoryEntry> Memories = MemoryComponent->FindRelevantMemories(
    ContextTags,
    PlayerActor,
    5
);
```

### 4. ������������� � ��������

� DialogueNode �������� �������:
```
Condition: Remembers Event
- Memory ID: "PlayerSavedMe"
- Min Importance: 70.0
```

� ������:
```
Effect: Create Memory
- Memory Type: Player Choice
- Description: "Player promised to help"
- Importance: 90.0
- Emotion: Nostalgic
```

## ?? ���� ������������

| ��� | �������� | �������� �������� |
|-----|----------|-------------------|
| DialogueEvent | ������� �� �������� | 30-50 |
| PlayerChoice | ������ ������ ������ | 60-85 |
| ItemGiven | ������� � �������� | 50-75 |
| QuestEvent | ��������� ������� | 60-80 |
| WorldEvent | ������ ������� ������� | 50-70 |
| Promise | �������� | 85-95 |
| Secret | ������� | 90-100 |
| PersonalMemory | ������ ������������ | 40-80 |

## ?? ������

| ������ | ������� �� ������ |
|--------|-------------------|
| Neutral | ����������� ��������� |
| Positive | +20% � ������������ |
| Negative | +30% � ������ Negative Bias |
| Bittersweet | +25% � ������������ |
| Traumatic | +50% � ������������ (�������� ����� �����) |
| Joyful | +30% � ������������ |
| Regretful | +25% � ������������ |
| Nostalgic | +20% � ������������ |

## ?? �������� ���������

### Memory Storage
- `Max Memories` (100) - ������������ ���������� ������������
- `Max Topics` (50) - ������������ ���������� ���

### Memory Decay
- `Enable Memory Decay` (true) - �������� �������������� ���������
- `Base Decay Rate` (0.05) - ������� �������� ��������� (�� ����)
- `Min Importance Threshold` (10.0) - ����������� �������� ��� ����������
- `Emotional Memory Bonus` (0.5) - ����� ��� ������������� �������

### Personality
- `Memory Retention Multiplier` (1.0) - ��������� �������� ������
  - 0.5 = ������ ������
  - 1.0 = ���������� ������
  - 2.0 = �������� ������
  
- `Negative Bias` (1.0) - ���������� ������� ����������
  - 0.5 = ��������
  - 1.0 = ����������������
  - 2.0 = ���������

### Performance
- `Decay Update Interval` (60.0) - ������� ���������� decay (�������)

## ?? ����������

### � RelationshipComponent

```cpp
// ������� ������ � �������������
FNPCMemoryEntry Memory;
Memory.Type = EMemoryType::PlayerChoice;
Memory.Description = FText::FromString("Player betrayed my trust");
Memory.Emotion = EMemoryEmotion::Traumatic;
MemoryComponent->AddMemory(Memory);

// ������ �� ���������
RelationshipComponent->ModifyAffinity(-40.0f);
RelationshipComponent->ModifyTrust(-60.0f);
```

### � DialogueSystem

����������� Memory Conditions � Effects � DialogueNodes ���:
- �������� ���������� ��������
- ���������� ������� �������
- ���������� ������������� ����������
- ������������ ��������� ��������

### � Quest System

```cpp
// ��� ������ ������
OnQuestStarted.AddDynamic([&](FName QuestId) {
    MemoryComponent->CreateMemory(
        EMemoryType::Promise,
        FText::Format(FText::FromString("Agreed to help with {0}"), QuestId),
        90.0f
    );
});

// ��� ���������� ������
OnQuestCompleted.AddDynamic([&](FName QuestId) {
    // ����� �������� � ������� ���������� ������
});
```

## ?? ������� �������������

### ������������ �����������

```cpp
TArray<FNPCMemoryEntry> RecentMemories = MemoryComponent->GetRecentMemories(1);
if (RecentMemories.Num() > 0)
{
    switch (RecentMemories[0].Emotion)
    {
        case EMemoryEmotion::Joyful:
            Greeting = "It's wonderful to see you again!";
            break;
        case EMemoryEmotion::Traumatic:
            Greeting = "What do you want?";
            break;
        default:
            Greeting = "Hello.";
    }
}
```

### �������� ��������

```cpp
TArray<FNPCMemoryEntry> Promises = MemoryComponent->GetMemoriesByType(EMemoryType::Promise);
for (const FNPCMemoryEntry& Promise : Promises)
{
    FTimespan Age = FDateTime::Now() - Promise.Timestamp;
    if (Age.GetTotalDays() > 7.0f)
    {
        // ����� �� �������� �������� - ������� �������
        RelationshipComponent->ModifyTrust(-10.0f);
    }
}
```

## ?? ������������

```cpp
// ������� �������� ����� ������������
UNPCMemoryTestHelpers::CreateTestMemories(WorldContext, NPC, Player);

// ������� ��� ������������ � ���
UNPCMemoryTestHelpers::PrintMemories(NPC);

// ���� �������������
UNPCMemoryTestHelpers::TestRelevance(NPC, Player);

// ���� ������������������
UNPCMemoryTestHelpers::PerformanceTest(NPC, 1000);
```

## ?? ������������

������ ������������ �������� �:
- `Documentation/NPCMemoryComponent_Guide.md` - ��������� �����������
- `Documentation/Blueprint_Setup_Guide.md` - Blueprint �������

## ?? Blueprint API

### �������� �������

```
Create Memory
?? Memory Type
?? Description
?? Importance
?? Emotion
?? Related Actor

Get Memory
?? Memory ID

Find Relevant Memories
?? Context Tags
?? Related Actor
?? Max Results

Discuss Topic
?? Topic Name
?? Interest Modifier
```

### ������� ��� ��������

- `Remembers Event` - ��������� ������� ������������
- `Memory Count Check` - ��������� ���������� ������������
- `Topic Discussed` - ��������� ���������� ����
- `Has Memories With Tags` - ����� �� �����
- `Memory Emotional Balance` - ��������� ������������� ������

### ������� ��� ��������

- `Create Memory` - ������� ������������
- `Remember Promise` - ���������� ��������
- `Remember Secret` - ���������� ������
- `Discuss Topic` - �������� ���������� ����
- `Forget Memory` - ������� ������������
- `Recall Memory` - ��������� ������

## ? ������������������

- **������������� ����� ������**: 50-150 ������������ �� NPC
- **Update interval**: 60 ������ ��� ����������� �������
- **Complexity**: O(n) ��� ����������� ��������, O(n log n) ��� ����������

��� �����������:
- ����������� `Decay Update Interval` >= 60 ������
- ���������� `Max Memories` �������� ���������
- ��� ������� NPC ��������� `Enable Memory Decay`

## ?? �������

```cpp
// ����� ����������
UE_LOG(LogTemp, Log, TEXT("Total Memories: %d"), MemoryComponent->GetMemoryCount());
UE_LOG(LogTemp, Log, TEXT("Topics: %d"), MemoryComponent->GetAllTopics().Num());

// ��������� ����������
for (const FNPCMemoryEntry& Memory : MemoryComponent->GetAllMemories())
{
    UE_LOG(LogTemp, Log, TEXT("%s - Importance: %.1f, Freshness: %.2f"),
        *Memory.Description.ToString(),
        Memory.Importance,
        Memory.Freshness);
}
```

## ?? �����

��������� �������� ������ ��������� ������� �������� � ������������� ��� ������� ����������:

1. �������� ����� ���� ������������ � `EMemoryType`
2. �������� ��������� Conditions/Effects
3. ��������� ������� �������������
4. �������� ���������� � ������ ���������

## ?? ��������

Copyright Epic Games, Inc. All Rights Reserved.

## ?? ������

����������� ��� ����� ��������� ������� �������� ��� ������� EchoesOfAffection.

## ?? ����� ��������

- [ ] Machine Learning ��� ������������ ��������� NPC
- [ ] �������������� ��������� �������� �� ������ ������
- [ ] ������� ���������� ����� ��������������
- [ ] ���������� �������� ������ � Editor
- [ ] �������������� � �������������� �����������

## ?? ���������

��� ������������� �������:
1. ��������� ���� (LogTemp)
2. ����������� Test Helpers ��� �������
3. ���������� � ������ ������������
4. ��������� ������� �������������

---

**���������� ������������ NPC! ???**
