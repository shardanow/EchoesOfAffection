# ?? ��� ������� �������� � ������������ � ��� ��������� �����

> **�������:** ��� ������� �������� � ������������? ��� ��������� ����� decay?  
> **������:** v1.3.1  
> **����:** 2025-01-23

---

## ?? ����� 1: ��� ������� �������� � ������������

### **������ 1: ����� Dialogue Effects (�������������)**

��� **�������� ������** - ������������ ��������� ������������� ����� ������� �������� ��������.

#### **A. � Dialogue Editor (���������)**

```
Dialogue Node: "You helped me save my village!"
?? Conditions: (�������� ����� ������� ����)
?  ?? ...
?
?? Effects: (��� ���������� ��� ������ ���� �������)
   ?? Effect 1: Add Memory
   ?  ?? Memory Tag: "Event.Betrayal"
   ?  ?? Memory Strength: 0.95
   ?
   ?? Effect 2: Modify Relationship
   ?  ?? Affinity: +20
   ?
   ?? Effect 3: Set Variable
      ?? "QuestCompleted" = true
```

**��� ���������� �� ��������:**

```cpp
// DialogueEffectExecutor.cpp (���������)
void ExecuteNodeEffects(const FDialogueNode& Node)
{
    for (UMemoryEffectBase* Effect : Node.MemoryEffects)
 {
        // ���������� ������������� ��� ������ ����!
        Effect->Execute(NPCActor, PlayerActor);
    }
}

// MemoryEffects.cpp
void UAddMemoryEffect::Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const
{
    UNPCMemoryComponent* MemoryComp = NPCActor->FindComponentByClass<UNPCMemoryComponent>();
    
    if (MemoryComp)
    {
        // ��������� ������������ � �����!
        MemoryComp->AddMemoryByTag(MemoryTag, MemoryStrength);
    }
}
```

**��������� Effects:**

| Effect | ���������� | ��������� |
|--------|-----------|-----------|
| **Add Memory** | ������� ����� ������������ | `MemoryTag`, `MemoryStrength (0-1)` |
| **Modify Memory Strength** | �������� ���� ������������� | `MemoryTag`, `StrengthModifier`, `bAdditive` |
| **Remove Memory** | ������� ������������ | `MemoryTag` |
| **Refresh Memory** | "��������" ������������ | `MemoryTag` |

---

#### **B. ������: ����� � ��������������**

```
Dialogue Tree: "The Betrayal"
????????????????????????????????????????????????????????????????????

Node 1: [Player] "I need to tell you something..."
?? Next ? Node 2

Node 2: [NPC] "What is it?"
?? Player Choices:
   ?? Choice A: "I told the guards about your secret."
   ?  ?? Effects:
   ?     ?? Add Memory
   ?     ?  ?? Tag: "Event.Betrayal"
   ?  ?  ?? Strength: 1.0
   ?     ?
   ?     ?? Create Memory (detailed)
   ?   ?  ?? Type: PlayerChoice
   ?     ?  ?? Description: "Player betrayed my trust"
?     ?  ?? Importance: 95
   ?     ?  ?? Emotion: Traumatic
   ?     ?  ?? Tags: ["Event.Betrayal", "Emotion.Traumatic"]
   ?   ?
   ?     ?? Modify Relationship
?        ?? Affinity: -40
   ?        ?? Trust: -60
   ?        ?? Fear: +30
   ?
   ?? Choice B: "Never mind, it's not important."
      ?? No effects
```

**���������:**
- ��� ������ Choice A: ��������� ������������ �������������
- NPC ������� �� ������� (Importance = 95, Traumatic)
- ��������� ���������

---

### **������ 2: ���������� (�� C++ ����)**

```cpp
// ��� ������ � ������� ������
void AMyNPC::OnPlayerHelped()
{
    UNPCMemoryComponent* MemComp = FindComponentByClass<UNPCMemoryComponent>();
    
 // ������ 2A: ������� (� �����)
    MemComp->AddMemoryByTag(
        FGameplayTag::RequestGameplayTag(FName("Event.Help")),
        0.75f  // Strength
    );
    
    // ������ 2B: ��������� (������ ��������)
    FNPCMemoryEntry Memory;
  Memory.MemoryId = FName("PlayerHelped_Village_001");
    Memory.Type = EMemoryType::PlayerChoice;
    Memory.Description = FText::FromString("Player helped defend the village");
    Memory.Importance = 75.0f;
    Memory.Emotion = EMemoryEmotion::Joyful;
Memory.RelatedActor = PlayerActor;
    Memory.Timestamp = FDateTime::Now();  // ?? �������� �����!
    Memory.LastRecalledTime = FDateTime::Now();
    Memory.Freshness = 1.0f;  // 100% ��������
    
    // �������� ����
    Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Event.Help")));
    Memory.ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Location.Village")));
    
    MemComp->AddMemory(Memory);
}
```

---

### **������ 3: ����� Blueprint**

```
Event Graph (NPC Blueprint):
????????????????????????????????????????????????????????????????????

Event: OnPlayerGaveItem
?? Cast to Item
?? Branch: Is Item == "Rose"?
?  ?? True:
?     ?? Memory Component ? Create Memory
??? Type: ItemGiven
?        ?? Description: "Player gave me a beautiful rose"
?        ?? Importance: 70.0
?        ?? Emotion: Joyful
?        ?? Related Actor: Player
?        ?? Context Tags:
?      ?? "Event.Gift"
?           ?? "Item.Flower"
```

---

### **������ 4: ����� Quest System (����������)**

```cpp
// QuestManager.cpp
void UQuestManager::StartQuest(FName QuestId)
{
    // ... existing code ...
    
    // ��� NPC, ��������� � �������, ������������� ������� ������������
    for (ANPC* QuestNPC : GetQuestRelatedNPCs(QuestId))
    {
        UNPCMemoryComponent* MemComp = QuestNPC->GetMemoryComponent();
        
        MemComp->CreateMemory(
    EMemoryType::Promise,
          FText::Format(FText::FromString("Agreed to help with {0}"), QuestId),
            90.0f,  // �������� �����!
            EMemoryEmotion::Nostalgic,
       PlayerActor
        );
  }
}
```

---

### **������ 5: ����� Relationship System**

```cpp
// RelationshipComponent.cpp
void URelationshipComponent::ModifyAffinity(float Delta)
{
  Affinity = FMath::Clamp(Affinity + Delta, -100.0f, 100.0f);
    
    // ������������� ������� ������������ ��� ������� ����������
    if (FMath::Abs(Delta) > 20.0f)
    {
        UNPCMemoryComponent* MemComp = GetOwner()->FindComponentByClass<UNPCMemoryComponent>();
        
   FNPCMemoryEntry Memory;
        Memory.Type = EMemoryType::PlayerChoice;
      
        if (Delta > 0)
        {
      Memory.Description = FText::FromString("Player did something that made me very happy");
        Memory.Emotion = EMemoryEmotion::Joyful;
      }
  else
     {
        Memory.Description = FText::FromString("Player hurt my feelings badly");
        Memory.Emotion = EMemoryEmotion::Negative;
     }
        
        Memory.Importance = FMath::Abs(Delta) * 2.0f;  // ��������������� ���������
        Memory.RelatedActor = PlayerActor;
        
        MemComp->AddMemory(Memory);
    }
}
```

---

## ? ����� 2: ��� ��������� ����� ��� Decay

### **�����: ������������ �������� ����� (Real-World Time)**

```cpp
// NPCMemoryComponent.cpp - �������� ������������
FNPCMemoryEntry NewMemory;
NewMemory.Timestamp = FDateTime::Now();  // ?? �������� ����� ��!
NewMemory.LastRecalledTime = FDateTime::Now();
NewMemory.Freshness = 1.0f;
```

**`FDateTime::Now()` - ���:**
- �������� ��������� ����� ����������
- �� ������� �����
- �� ������� �� ����� ����
- �� ������� �� Time Dilation

---

### **��� Decay ������� ��������� �����**

```cpp
// NPCMemoryComponent.cpp - ������ �������� ������������
float UNPCMemoryComponent::CalculateRelevance(const FNPCMemoryEntry& Memory, ...) const
{
    // ...
    
    // ���������� �������� � �������� ����
    FTimespan Age = FDateTime::Now() - Memory.Timestamp;
    float AgeDays = Age.GetTotalDays();  // ?? �������� ���!
    
    // ���������������� decay �� �������
    float AgeFactor = FMath::Exp(-AgeDays * 0.1f);
    Relevance *= AgeFactor;
    
    return Relevance;
}
```

**������ �������:**

```
������������ �������: 23 ������ 2025, 10:00:00
������� �����:        25 ������ 2025, 14:30:00

Age = 25.01.2025 14:30 - 23.01.2025 10:00
 = 2 days, 4 hours, 30 minutes
    = 2.1875 days (��������!)

AgeFactor = exp(-2.1875 ? 0.1)
          = exp(-0.21875)
          = 0.803 (80.3%)

Relevance ��������� �� 19.7% ��-�� ��������
```

---

### **Decay Rate: "Per Day" = �������� ���**

```cpp
// NPCMemoryComponent.cpp - ���������� freshness
void UNPCMemoryComponent::UpdateMemoryFreshness(FNPCMemoryEntry& Memory, float DeltaTime)
{
    float DecayRate = CalculateDecayRate(Memory);  // Per day!
    
    // ����������� "per day" ? "per second"
    float DecayPerSecond = DecayRate / (24.0f * 60.0f * 60.0f);
    //        ?     ?     ?
    //        24   60    60
    //           hours mins  secs
    //   = 86,400 ������ � ���
    
    // ���������� freshness
  Memory.Freshness = FMath::Max(Memory.Freshness - DecayPerSecond * DeltaTime, 0.0f);
}
```

**������ �������:**

```
BaseDecayRate = 0.05 (5% per REAL day)

��� ������������ � Importance = 50, Emotion = Neutral:
????????????????????????????????????????????????????????????????????
DecayRate = 0.05 (�������)
          ? 1.25 (������ �������� ��� 50)
          ? 1.0 (��� �������������� ������)
          = 0.0625 per day

DecayPerSecond = 0.0625 / 86400 = 0.0000007233 per second

�� 1 �������� ������ (60 ������):
Freshness -= 0.0000007233 ? 60 = 0.0000434
           = -0.00434% per minute

�� 1 �������� ��� (3600 ������):
Freshness -= 0.0000007233 ? 3600 = 0.0026
           = -0.26% per hour

�� 1 �������� ���� (86400 ������):
Freshness -= 0.0000007233 ? 86400 = 0.0625
 = -6.25% per day ?
```

---

### **Timeline: �������� ����� vs. ������� �����**

```
�������� ����� (��� ������������):
????????????????????????????????????????????????????????????????????

23.01.2025 10:00: ������������ ������� (Freshness = 100%)
23.01.2025 10:01: Decay update (Freshness = 99.99%)
24.01.2025 10:00: ������ 1 �������� ���� (Freshness = 93.75%)
25.01.2025 10:00: ������ 2 �������� ��� (Freshness = 87.89%)
...
30.01.2025 10:00: ������ 7 �������� ���� (Freshness = 60.58%)


������� ����� (�� ������������):
????????????????????????????????????????????????????????????????????

Game Time Day 1:����� ����� 2 ����
Game Time Day 5:  ����� �� ����� 3 ��� (pause)
Game Time Day 10: ����� ��������

? ������� �� ��������� ������� �����!
? ������� ��������� �������� �����!

���������:
- �� 5 �������� ���� ������������ decay ���� ���� ���� �� �����
- Freshness ����������� ���� ����� ����� �� ������
```

---

### **������ �������� �����?**

#### **�����:**

? **��������:**
- �� ����� ���������������� � ������� ��������
- �� ����� ������������ �����
- �� ����� save/load ��������� ������

? **������� ��� ��������� ���������:**
- ����� �������� ����� �������� ? NPC "�����" �������� �������
- Simulate "real life" ���������

#### **������:**

? **��������:**
- ����� �� ����� ������ ? ��� ������������ ������
- �� ��������� ������� �����
- ������� ��� RPG (����� ���� 8 ����� ? ������������ �� ��������, �� ������� ������ 1 ��� ? decay!)

---

### **�������������� ������: ������� ����� (�� �����������)**

```cpp
// ? ������� ���������� (�������� �����):
FDateTime::Now()

// ? ��������� ������������ (������� �����):
UGameplayStatics::GetWorldDeltaSeconds(WorldContext) // ������ �������� ������� �����
UGameplayStatics::GetTimeSeconds(WorldContext)       // ������� timestamp

// ��� custom ������� ������� �������:
UMyGameTimeManager::GetGameDateTime()  // ������� day/night cycle
```

**���� �� ������������ ������� �����:**

```
������� ����: 1 ������� ���� = 20 ����� ��������� �������

������������ �������: Game Day 1, Morning
����� ������ 1 ��� ��������� ������� = 3 ������� ���
Freshness decay �� 3 ������� ��� (�� �� 1 ��� ���������)

����� �� ����� 1 ������ ��������� �������
Freshness �� �������� (������� ����� �����������)
```

---

## ?? ������������ �������

### **������ 1: �������� ������������ � �������**

```
Dialogue Node: "Thank you for saving me!"
????????????????????????????????????????????????????????????????????

Effects:
1. Add Memory
   ?? Memory Tag: "Event.Rescue"
   ?? Strength: 0.95

2. Create Memory (detailed)
   ?? Type: PlayerChoice
   ?? Description: "Player risked their life to save mine"
   ?? Importance: 95.0
   ?? Emotion: Joyful
   ?? Tags: ["Event.Rescue", "Emotion.Grateful"]

3. Modify Relationship
   ?? Affinity: +50
   ?? Trust: +40

���������:
- ������������ ������� � Timestamp = FDateTime::Now()
- Freshness = 100%
- ������ decay ����� 60 ������ (��������)
```

---

### **������ 2: Decay Timeline (�������� �����)**

```
������������: "Player saved me" (Importance = 95, Traumatic)
????????????????????????????????????????????????????????????????????

Day 0 (23.01.2025):
- Freshness: 100%
- DecayRate: 0.05 ? 0.55 (importance) ? 0.5 (emotional) = 0.01375 per day

Day 1 (24.01.2025):
- Freshness: 100% - 1.375% = 98.625%

Day 7 (30.01.2025):
- Freshness: 100% - 9.625% = 90.375%

Day 30 (22.02.2025):
- Freshness: 100% - 41.25% = 58.75%

Day 90 (23.04.2025):
- Freshness: 100% - 123.75% = 0% (capped)
- �� Importance = 95 > MinImportanceThreshold = 10
- ? �� ��������� (�������� ���������)

Day 365 (23.01.2026):
- Freshness: 0% (��� �����)
- Importance: 95
- ? ��� ��� ������ (������� �� �������!)
```

---

### **������ 3: ����� �������� ����� ��������**

```
��������:
????????????????????????????????????????????????????????????????????
����� ����� 20.01.2025
������ 10 ������������ (Importance 10-60)
����� �� ����� 2 ������
�������� 03.02.2025

��� ���������:
????????????????????????????????????????????????????????????????????
������ 14 �������� ����

�������� ������������ (Importance < 30):
- DecayRate ? 0.1 per day
- ����� 14 ����: Freshness ? 0%
- Importance < MinImportanceThreshold
- ? ������� (������)

������� ������������ (Importance 40-60):
- DecayRate ? 0.05 per day
- ����� 14 ����: Freshness ? 30%
- ? ��� ������ (�� �����)

������ ������������ (Importance 80+):
- DecayRate ? 0.02 per day
- ����� 14 ����: Freshness ? 70%
- ? ������� ������

NPC ��� �������:
"����� �� ��������! �����, �� ������� ��� �����... (���������� ������)"
```

---

## ?? �������� ������

### **�������� ������������:**

| ������ | ����� ������������ | ��������� |
|--------|-------------------|-----------|
| **Dialogue Effects** | �������� ������ (������������� � ��������) | ? ����� |
| **C++ ����������** | ��� ������� ������, quest events | ?? ������ |
| **Blueprint** | ��� ����������, ������� ������� | ? ����� |
| **Quest/Relationship Integration** | �������������� ���������� | ??? ������ (������� ������������) |

### **����� ��� Decay:**

| �������� | �������� | ���������� |
|----------|----------|------------|
| **Timestamp** | `FDateTime::Now()` | �������� ����� �������� |
| **Decay Rate** | Per REAL day | ��������, 0.05 = 5% per REAL day |
| **Update Interval** | 60 REAL seconds | �������� decay ������ ������ |
| **Age Calculation** | `(Now - Timestamp).GetTotalDays()` | � �������� ���� |

### **������� Decay:**

```
DecayPerSecond = DecayRate / 86400  // 86400 = ������ � ���

Freshness -= DecayPerSecond ? DeltaTime

���:
- DecayRate = BaseDecayRate ? ImportanceFactor ? EmotionFactor ? ...
- DeltaTime = �������� ����� � ���������� update (������ 60 ������)
```

---

## ?? ������������

### **��� ������� ���������� (v1.3.1):**

1. ? **����������� Dialogue Effects** - �������� ������ �������� ������������
2. ? **���������� �������� �����** - decay ���������� ���� ����� ���� �� �����
3. ? **�������� ��������** - ������������ � Importance > 80 ����� �� ����������
4. ? **Emotion ���������** - Traumatic/Joyful decay � 2-4 ���� ���������

### **��� ������� ������ (v2.0):**

1. ?? **����� �������� �������** - ��������� �������� real-time vs game-time
2. ?? **Pause-aware decay** - �� decay ����� ���� �� �����
3. ?? **Hybrid approach** - ������������� �������� � ������� �����

---

**�������� �����������:** 2025-01-23  
**������:** 1.0  
**������:** ? Complete

**�����:** ������������ ��������� ����� Effects � �������� ��� ����������, � ����� ��������� �� ������ **��������� ���������� ������� (FDateTime::Now)**, ��� �������� decay ���������� ���� ����� ���� �� ��������! ??
