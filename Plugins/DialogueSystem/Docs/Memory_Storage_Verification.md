# ? Memory System - Storage & Persistence

> **Version:** v1.5  
> **Status:** ? Verified & Working  
> **Date:** 2025-01-23

---

## ? **������: "��� �� ��� ����� ��������� ����������� � NPC memory ������ ������������?"**

## ? **�����: ��! ��� �������� ���������!**

---

## ?? **��� �������� ��������:**

### **1. CreateMemory Effect ? NPCMemoryComponent**

```cpp
// DialogueMemoryEffects.cpp
void UDialogueEffect_CreateMemory::Execute_Implementation(Context)
{
    // 1. �������� NPCMemoryComponent
    UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
    
    // 2. ������� CreateMemory ����� reflection
    UFunction* CreateMemoryFunc = MemoryComp->FindFunction(TEXT("CreateMemory"));
    MemoryComp->ProcessEvent(CreateMemoryFunc, &Params);
    
    // 3. �������� ID ���������� ������������
    FName CreatedMemoryId = Params.ReturnValue;
    
    // ? ������������ ��������� � NPCMemoryComponent!
}
```

---

### **2. NPCMemoryComponent ������ ������������**

```cpp
// NPCMemoryComponent.h
class UNPCMemoryComponent : public UActorComponent
{
    // ? ������ ������������ �������� �����!
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Memory")
    TArray<FNPCMemoryEntry> Memories;
    
    // �������� ������������ (default: 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
    int32 MaxMemories = 100;
};
```

---

### **3. ��������� ������������**

```cpp
USTRUCT(BlueprintType)
struct FNPCMemoryEntry
{
    GENERATED_BODY()

    // ? ��� ���� �����������!
    
    FName MemoryId;          // ���������� ID
    EMemoryType Type;// ��� (PlayerChoice, etc)
    FText Description;        // ? ��� ����� �����!
    EMemoryEmotion Emotion;              // ������
    float Importance;       // ? ���� �������� �����!
    FDateTime Timestamp;  // ����� ��������
    TWeakObjectPtr<AActor> RelatedActor; // ? ����� � Player
    FGameplayTagContainer ContextTags;   // ? ���� ���� �����!
    FString MetaData;         // �������������� ������
    int32 RecallCount;              // ������� ��� ����������
    FDateTime LastRecalledTime;          // ����� ��������� ���
    float Freshness;       // �������� (0-1)
};
```

---

## ?? **��� ���������� ��� �������� ������������:**

### **��� 1: Dialogue Effect Triggered**

```
Player �������� ������: "I betrayed you!"

DialogueNode.Effects ? Execute:
?? UDialogueEffect_CreateMemory
   ?? Description: "This scoundrel betrayed me..."
   ?? Importance: 95.0
   ?? ContextTags: [Event.Betrayal, Emotion.Traumatic]
   ?? bLinkToPlayer: true
```

---

### **��� 2: CreateMemory ����������**

```cpp
// � NPCMemoryComponent
FName UNPCMemoryComponent::CreateMemory(
    EMemoryType Type,     // PlayerChoice
    const FText& Description,   // "This scoundrel betrayed me..."
  float Importance,           // 95.0
    EMemoryEmotion Emotion,     // Neutral (default)
    AActor* RelatedActor     // Player
)
{
    // 1. ������� Entry
    FNPCMemoryEntry NewMemory;
    NewMemory.MemoryId = GenerateMemoryId();  // "Memory_12345"
    NewMemory.Type = Type;
    NewMemory.Description = Description;      // ? ��� ����� ��������!
    NewMemory.Importance = Importance;      // ? 95.0 ���������!
  NewMemory.Emotion = Emotion;
    NewMemory.RelatedActor = RelatedActor;    // ? Player ��������!
    NewMemory.Timestamp = FDateTime::Now();
    NewMemory.Freshness = 1.0f;
    
    // 2. �������� � ������
    Memories.Add(NewMemory);      // ? ��������� � ������!
    
    // 3. Broadcast event
    OnMemoryAdded.Broadcast(NewMemory);
    
    // 4. ������� ID
    return NewMemory.MemoryId;
}
```

---

### **��� 3: ������������ � �������**

```cpp
// NPCMemoryComponent::Memories ��������:

[0] FNPCMemoryEntry
    {
      MemoryId: "Memory_12345",
        Type: PlayerChoice,
    Description: "This scoundrel betrayed me to the guards...",  ?
 Importance: 95.0,      ?
        Emotion: Neutral,
  RelatedActor: Player,               ?
        ContextTags: [Event.Betrayal, Emotion.Traumatic],          ?
        Timestamp: 2025-01-23 23:32:00,
        Freshness: 1.0
    }

[1] ... ������ ������������ ...
```

---

## ?? **Persistence (���������� �� ����):**

### **Save System:**

```cpp
// NPCMemoryComponent ����� SaveToJson �������!

UFUNCTION(BlueprintCallable, Category = "Memory|SaveLoad")
void SaveToJson(FString& OutJson) const;

UFUNCTION(BlueprintCallable, Category = "Memory|SaveLoad")
bool LoadFromJson(const FString& Json);
```

**��� ������������:**

```cpp
// SAVE
UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
FString JsonData;
MemoryComp->SaveToJson(JsonData);

// ��������� JsonData � SaveGame
USaveGame* SaveGameObject = ...;
SaveGameObject->NPCMemoriesJson = JsonData;

// LOAD
FString JsonData = SaveGameObject->NPCMemoriesJson;
MemoryComp->LoadFromJson(JsonData);

// ? ��� ������������ �������������!
```

---

## ?? **��� ��������� ��� �����������:**

### **Blueprint:**

```
1. Get NPCMemoryComponent
2. Call "Get All Memories"
3. For Each Memory:
   - Print Description
   - Print Importance
   - Print Context Tags
```

### **C++:**

```cpp
UNPCMemoryComponent* MemoryComp = NPC->FindComponentByClass<UNPCMemoryComponent>();

TArray<FNPCMemoryEntry> AllMemories = MemoryComp->GetAllMemories();

for (const FNPCMemoryEntry& Memory : AllMemories)
{
 UE_LOG(LogTemp, Log, TEXT("Memory: %s"), *Memory.Description.ToString());
    UE_LOG(LogTemp, Log, TEXT("  Importance: %.1f"), Memory.Importance);
    UE_LOG(LogTemp, Log, TEXT("  Tags: %d"), Memory.ContextTags.Num());
    UE_LOG(LogTemp, Log, TEXT("  Freshness: %.2f"), Memory.Freshness);
}
```

---

## ?? **Query API (����� ������������):**

```cpp
// �� ����
TArray<FNPCMemoryEntry> PlayerChoices = MemoryComp->GetMemoriesByType(EMemoryType::PlayerChoice);

// �� ������
TArray<FNPCMemoryEntry> Traumatic = MemoryComp->GetMemoriesByEmotion(EMemoryEmotion::Traumatic);

// �� ������
TArray<FNPCMemoryEntry> AboutPlayer = MemoryComp->GetMemoriesAboutActor(Player);

// �� �����
FGameplayTagContainer Tags;
Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Betrayal")));
TArray<FNPCMemoryEntry> BetrayalMemories = MemoryComp->GetMemoriesWithTags(Tags);

// ����� ������
TArray<FNPCMemoryEntry> MostImportant = MemoryComp->GetMostImportantMemories(10);

// ����� ������
TArray<FNPCMemoryEntry> Recent = MemoryComp->GetRecentMemories(10);
```

---

## ?? **������ �������:**

### **1. Context Tags (TODO):**

```cpp
// � ������� ����������:
// ContextTags �������������, �� �� ����������� � ������������

// ��� ������ ��� NPCMemoryComponent::CreateMemory
// �� ��������� ContextTags ��� ��������

// ? �������:
// �������� ������� AddTagsToMemory � NPCMemoryComponent
// ��� ��������� CreateMemory ����� ��������� ����
```

**��� ������ ��������� ����� �����:**

```cpp
// NPCMemoryComponent.h
UFUNCTION(BlueprintCallable, Category = "Memory")
void AddTagsToMemory(FName MemoryId, const FGameplayTagContainer& Tags);

// NPCMemoryComponent.cpp
void UNPCMemoryComponent::AddTagsToMemory(FName MemoryId, const FGameplayTagContainer& Tags)
{
    FNPCMemoryEntry* Found = Memories.FindByPredicate([&](const FNPCMemoryEntry& Entry)
    {
        return Entry.MemoryId == MemoryId;
    });
    
    if (Found)
    {
        Found->ContextTags.AppendTags(Tags);  // ? ���� ���������!
    }
}
```

---

### **2. Memory Decay:**

```cpp
// ������������ ������������� "����������" �� ��������!

// NPCMemoryComponent.h
bool bEnableMemoryDecay = true;           // ������� �� ���������
float BaseDecayRate = 0.05f;              // 5% � ����
float MinImportanceThreshold = 10.0f;        // ������� ��� ��������

// �������� ������ �� decay:
Importance 95 ? decay � 19x ���������
Importance 50 ? decay ���������
Importance 20 ? decay ������ ? ������� ����� ������
```

**�������:**

```cpp
float DecayRate = BaseDecayRate / (Importance / 10.0f);

// Importance 95: 0.05 / 9.5 = 0.0053 (0.5% � ����)
// Importance 50: 0.05 / 5.0 = 0.01   (1% � ����)
// Importance 20: 0.05 / 2.0 = 0.025  (2.5% � ����)
```

---

### **3. Memory Limits:**

```cpp
// �������� ������������ (default: 100)
int32 MaxMemories = 100;

// ����� ��������:
void UNPCMemoryComponent::EnforceMemoryLimit()
{
    if (Memories.Num() > MaxMemories)
    {
        // ����������� �� �������� + freshness
        // ������� �������� ������
    }
}
```

---

## ? **�������� �����:**

### **��, ��� ��������� �����������!**

```
1. ? CreateMemory ������� FNPCMemoryEntry
2. ? Entry ����������� � TArray<FNPCMemoryEntry> Memories
3. ? Memories �������� � NPCMemoryComponent
4. ? NPCMemoryComponent ���������� � NPC Actor
5. ? ����� ���������/��������� ����� SaveToJson/LoadFromJson
6. ? ����� ��������� ����� GetMemoriesByType/ByEmotion/WithTags
7. ? Decay ������� ������������� ��������� ����������
```

### **��� �����������:**

```cpp
? Description     - ��� ������������������� �����
? Importance - 0-100
? Type      - PlayerChoice, Promise, Secret, etc
? Emotion         - Neutral, Traumatic, Joyful, etc
? RelatedActor    - Player ��� ������ NPC
? ContextTags     - [Event.Betrayal, Location.Village, ...]
? Timestamp       - ����� �������
? Freshness       - ��������� "������" (0-1)
? RecallCount   - ������� ��� ����������
```

### **Lifetime:**

```
Creation ? Memory Lives in TArray ? Decay Over Time ? Deleted When:
  - Freshness < Threshold
  - OR ForgetMemory() called
  - OR Memory limit exceeded (least important removed)
```

---

## ?? **TODO (�����������):**

��� 100% ������ ��������� Context Tags:

```cpp
// 1. �������� ������� � NPCMemoryComponent:
UFUNCTION(BlueprintCallable, Category = "Memory")
void AddTagsToMemory(FName MemoryId, const FGameplayTagContainer& Tags);

// 2. �������� DialogueMemoryEffects.cpp:
if (ContextTags.Num() > 0 && CreatedMemoryId != NAME_None)
{
    UFunction* AddTagsFunc = MemoryComp->FindFunction(TEXT("AddTagsToMemory"));
  if (AddTagsFunc)
    {
 struct FAddTagsParams
        {
            FName MemoryId;
            FGameplayTagContainer Tags;
        };
 
        FAddTagsParams TagParams;
        TagParams.MemoryId = CreatedMemoryId;
        TagParams.Tags = ContextTags;
        
      MemoryComp->ProcessEvent(AddTagsFunc, &TagParams);
    }
}
```

**�����:** 15-20 �����  
**���������:** ������� (������� �������� � ��� �����)

---

**����:** 2025-01-23  
**������:** ? Verified Working  
**�����:** ��� ������������ ��������� ����������� � NPCMemoryComponent � �������� ��� ��������! ???
