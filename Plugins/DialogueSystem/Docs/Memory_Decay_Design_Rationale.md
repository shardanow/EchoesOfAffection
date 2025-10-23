# ?? Memory Decay: ����� ��� ����� � ��� ��������

> **�������:** ����� ����� Memory Decay? ��� �� 100 NPC �������������� ������������?  
> **������:** v1.3.1  
> **����:** 2025-01-23

---

## ?? ������� ������

### **Q1: ����� ����� Memory Decay?**

> **Memory Decay ����� ��� �������� ������������, ����� NPC � ������������ �������**, ������� �������� �������� ������� � ������ ������, �������� �������� ��������� ��������� � ������������� �������.

### **Q2: ��� �� 100 NPC �������������� ������������?**

> **���!** ������ NPC �������������� **� ���� �����** (����� 60 ������ ����� ������ spawn). �� ��� 100 NPC ���������� **~1.67 NPC � �������** (100 / 60s), ��� ���� ���������� ����������.

---

## ?? ����� 1: ����� ����� Memory Decay?

### **1.1. ����������� �������**

#### **A. ������� � ��������**

```
�������� ��� Memory Decay:
????????????????????????????????????????????????????????????
Day 1:   �����: "������!"
         NPC:   "������! ��� ����?"

Day 30:  �����: "������!"
         NPC:   "�����, 30 ���� ����� �� ������ '������!' 
       (freshness = 1.0, importance = 10)"
    
? ��������: NPC ������ ��������� ��� � ��������� ���������!
```

```
�������� � Memory Decay:
????????????????????????????????????????????????????????????
Day 1:   �����: "������!"
         NPC:   "������! ��� ����?"
         Memory: freshness = 1.0, importance = 10

Day 30:  Memory: freshness = 0.2, importance = 10
       NPC:   "�������, �� ����������� ������..."

Day 60:  Memory: freshness = 0.0 (FORGOTTEN!)
         NPC:   "��������, �� ���?"
         
? �������: NPC �������� �������� ���������!
```

---

#### **B. ������ ������� �������� ��������**

```cpp
// ������ �� ����:
void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
    for (int32 i = Memories.Num() - 1; i >= 0; --i)
    {
 FNPCMemoryEntry& Memory = Memories[i];
    UpdateMemoryFreshness(Memory, DecayUpdateInterval);
        
        // ?? ������ �������!
        if (Memory.Freshness <= 0.0f && Memory.Importance < MinImportanceThreshold)
        {
    // ������� ������ �������� ������������
       Memories.RemoveAt(i);
   }
    }
}
```

**��� ��� ������:**

| ������� | Importance | Freshness ����� 30 ���� | ��������� |
|---------|------------|------------------------|-----------|
| ��������� �������� | 10 | 0.2 ? ������ | ? ������ |
| ������� ����� | 70 | 0.5 | ? ������ ����� |
| ���� ����� NPC | 95 | 0.8 | ? **������ ������!** |
| ������������� | 100 (Traumatic) | 0.9 | ? **������� �� �������!** |

---

#### **C. ������������� �������**

```cpp
// ��� ����������, ��� ������ ������ �� decay:
float UNPCMemoryComponent::CalculateDecayRate(const FNPCMemoryEntry& Memory) const
{
    float DecayRate = BaseDecayRate;
    
    // ������������� ������������ decay ���������
    if (Memory.Emotion != EMemoryEmotion::Neutral)
    {
        DecayRate *= (1.0f - EmotionalMemoryBonus); // ������ 0.5
    }
    
    // ���������� ������������ decay ��� ���������
    if (Memory.Emotion == EMemoryEmotion::Traumatic)
    {
        DecayRate /= NegativeBias; // ������ 1.0-2.0
    }
    
    return DecayRate;
}
```

**����������� ������:**

```
������ 1: ����������� ��������
?????????????????????????????????????????????????
Importance: 50
Emotion: Neutral
Decay: 0.05 per day ? 1.0 (no bonus) = FAST decay

Day 0:  Freshness = 1.0 "�� ������, ��� ������ �������"
Day 30: Freshness = 0.1 "�������, �� ���-�� �������..."
Day 40: FORGOTTEN  "..."
```

```
������ 2: �������������� �������
?????????????????????????????????????????????????
Importance: 95
Emotion: Traumatic
Decay: 0.05 / (0.5 ? 2.0) = 0.05 per day (����� ��������!)

Day 0:   Freshness = 1.0 "�� ������ ����!"
Day 30:  Freshness = 0.85 "� �� ���� ������..."
Day 60:  Freshness = 0.7  "� ��� ��� �����..."
Day 365: Freshness = 0.2  "���� �� ���� �������..."

������� �� ��������� ��������� (importance = 95 > MinImportanceThreshold = 10)
```

---

### **1.2. ����������� �������**

#### **A. ���������� �������**

```
��� Memory Decay:
?????????????????????????????????????????????????
100 NPC ? 100 memories = 10,000 memories
����� 10 ����� ����: 100 NPC ? 500 memories = 50,000 memories!
Memory: 50,000 ? 500 bytes = 25MB

� Memory Decay:
?????????????????????????????????????????????????
100 NPC ? 20-50 memories = 2,000-5,000 memories
����� 10 �����: ��� ��� 100 NPC ? 50 memories = 5,000 memories
Memory: 5,000 ? 500 bytes = 2.5MB

��������: 90%!
```

---

#### **B. ������������������ save/load**

```cpp
// ��� decay: ��������� ��� ������������
void SaveGame()
{
    for (NPC in AllNPCs) // 100 NPC
    {
        SaveMemories(NPC->Memories); // 500 memories per NPC
    }
}
// Total: 50,000 memories ? 500 bytes = 25MB save file!
// Save time: ~500ms

// � decay: ��������� ������ ������
void SaveGame()
{
    for (NPC in AllNPCs) // 100 NPC
    {
  SaveMemories(NPC->Memories); // 50 memories per NPC
    }
}
// Total: 5,000 memories ? 500 bytes = 2.5MB save file
// Save time: ~50ms
```

---

#### **C. Relevance Queries ���������� �������**

```cpp
// FindRelevantMemories ���������� ��� ������������
TArray<FNPCMemoryEntry> UNPCMemoryComponent::FindRelevantMemories(
    const FGameplayTagContainer& ContextTags,
    AActor* RelatedActor,
    int32 MaxResults) const
{
    for (const FNPCMemoryEntry& Memory : Memories) // ?? O(N)
    {
    float Relevance = CalculateRelevance(Memory, ContextTags, RelatedActor);
        // ...
    }
}
```

**Performance:**

| Memories per NPC | Query time | With 100 NPC queries |
|------------------|------------|----------------------|
| 500 (no decay) | 10ms | 1000ms (1 second!) |
| 50 (with decay) | 1ms | 100ms ? |

---

### **1.3. ������������ �������**

#### **A. �������� ���������� ���������**

```
��������: ����� �������� � NPC 30 ���� �����
?????????????????????????????????????????????????

��� Decay:
Day 30: NPC: "�����, �� 30 ���� ����� ���������� ���!" (freshness = 1.0)
      �����: "��... � ��� �����..."
        ? AWKWARD

� Decay:
Day 30: NPC: "�������, �� �����-�� ���������... ��� ���?" (freshness = 0.3)
        �����: "��, ����� ����..."
        ? NATURAL

Day 60: NPC: "..." (FORGOTTEN, freshness = 0.0)
        ? ����� ������ ����� ���������!
```

---

#### **B. Incentive ��� ���������� ��������������**

```
����������� ���:
?????????????????????????????????????????????????

1. ����� �������� NPC (importance = 70, freshness = 1.0)
   NPC: "�������! � ����� ���� �������!"

2. ����� �� ������������ 30 ����
   Memory: freshness = 0.4
   NPC: "�������, �� �����-�� �������..."

3. ����� ������������ � �������������
   RecallMemory(MemoryId) ? freshness = 0.5 (+0.1 bonus!)
 NPC: "�����! �� ���� ����!"

4. ����� ����� ��������
 RecallCount++, decay slower
   NPC ������� �� �������!

? REWARD: ���������� ������� ��������� ������!
```

---

## ?? ����� 2: ��� �� 100 NPC �������������� ������������?

### **2.1. ���, ��� ������������ �� �������**

#### **��� ����������:**

```cpp
// NPCMemoryComponent.cpp
UNPCMemoryComponent::UNPCMemoryComponent()
{
    PrimaryComponentTick.TickInterval = 1.0f; // �������� ������ �������
}

void UNPCMemoryComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // ?? TimeSinceLastDecayUpdate ���������������� � 0.0f!
    // ������ decay ���������� ����� 60 ������ ����� BeginPlay()
}

void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
    TimeSinceLastDecayUpdate += DeltaTime; // ������������� � BeginPlay
    
    if (TimeSinceLastDecayUpdate < 60.0f) // �������� ���������
    {
        return; // ������� �����
    }
    
    // ?? �������� ���� ������ ��� � ������ (� ������� BeginPlay)
    TimeSinceLastDecayUpdate = 0.0f;
    
    // ��������� ���� 100 ������������
    for (int32 i = Memories.Num() - 1; i >= 0; --i)
    {
        UpdateMemoryFreshness(Memories[i], 60.0f); // 3ms
    }
}
```

---

#### **��� ���������� � ����������:**

```
Timeline ��� 100 NPC (spawned � ������ �����):
????????????????????????????????????????????????????????????????????

NPC #1 spawned � 00:00:00 ? BeginPlay() ? decay � 00:01:00
NPC #2 spawned � 00:00:01 ? BeginPlay() ? decay � 00:01:01
NPC #3 spawned � 00:00:02 ? BeginPlay() ? decay � 00:01:02
...
NPC #100 spawned � 00:01:39 ? BeginPlay() ? decay � 00:02:39

���������: Decay events ������������ �� 100 ��������!
```

---

### **2.2. �� �������� ��� ����� ����!**

#### **������������� �� ���������:**

```
�������� 1: NPC spawned ���������� (best case)
????????????????????????????????????????????????????????????????????
100 NPC spawned over 60 seconds = 1.67 NPC per second

Decay pattern:
Time:  60s   61s   62s   63s   64s   65s   66s   67s   68s
       ??    ??    ??    ??    ??    ??    ??    ??    ??
       
������ �������: 1-2 NPC ? 3ms = 3-6ms spike
��� 60 FPS: 3-6ms = ��������� ?


�������� 2: NPC spawned ������� (worst case)
????????????????????????????????????????????????????????????????????
50 NPC spawned � 00:00:00-00:00:05 (������� ����������)
50 NPC spawned � 00:00:30-00:00:35 (enemy wave)

Decay pattern:
Time:  60s   61s   62s   63s   64s   65s   90s   91s   92s
   ????  ????  ????  ????  ????  ????  ????  ????  ????
       
� 60-65s: 10 NPC ? 3ms = 30ms PER SECOND
� 90-95s: 10 NPC ? 3ms = 30ms PER SECOND

��� 60 FPS: 30ms = 2 frames (�������!) ??
```

---

### **2.3. ���������� �������������**

```
���� 100 NPC spawned ���������� �� X ������:
????????????????????????????????????????????????????????????????????

X = 60s  ? Decay ������ 0.6s (1.67 NPC/s) ? 5ms spike ������ 0.6s  ?
X = 30s  ? Decay ������ 0.3s (3.3 NPC/s)  ? 10ms spike ������ 0.3s ??
X = 10s  ? Decay ������ 0.1s (10 NPC/s)   ? 30ms spike ������ 0.1s ??
X = 1s   ? Decay ������ 0.01s (100 NPC/s) ? 300ms spike!   ??

�����: ��� ������� spawn, ��� ���� �������������!
```

---

## ?? �������

### **����������� (��� ����):**

```cpp
// 1. ��������� decay ��� ������� NPC
MemoryComponent->bEnableMemoryDecay = false;

// 2. ��������� ���������� ������������
MemoryComponent->MaxMemories = 20; // ������ 100
```

### **������������� (v1.4 - 1 ��� ������):**

```cpp
// �������� � BeginPlay() ��������� offset
void UNPCMemoryComponent::BeginPlay()
{
    Super::BeginPlay();
  
    // ? �������: ������������ ���������� �������
TimeSinceLastDecayUpdate = FMath::FRandRange(0.0f, DecayUpdateInterval);
    
    // ���������: Decay events ���������� ������������!
}
```

**������:**
```
100 NPC � ��������� offset:
????????????????????????????????????????????????????????????????????
Decay pattern:
Time:  0s10s   20s   30s   40s   50s   60s   70s   80s
       ????  ????  ????  ????  ????  ????  ????  ????  ????

������ ~0.6s: 1-2 NPC ? 3ms = 3-6ms
100 NPC ? 3ms = 300ms spread over 60s = 5ms per frame average

��� 60 FPS: 5ms = ���������! ?
```

---

## ?? �������� ������

### **Q1: ����� ����� Memory Decay?**

**������� �����:**
> ��� �������� **������������ NPC**, ������� **�������� ��������** � **������ ������**, �������� **���������� ���������** � **������������� �������**.

**������ ������ ������:**

1. ? **��������:** �������, ���������� ���������, incentive ��� �������
2. ? **�������:** ���������� ������� (90% savings), ������� save/load
3. ? **������:** ������������ �������� ������, forgiveness �������
4. ? **Performance:** ������� queries, ������ ������

---

### **Q2: ��� �� 100 NPC �������������� ������������?**

**������� �����:**
> **���**, ������ NPC �������������� **����� 60 ������ ����� ������ spawn**. �� ���� ����� NPC spawned ������������ (������� ����������), �� decay events **������������**, ��� ���� **������������� �����**.

**�������:**
```cpp
// v1.4 - �������� ��������� offset (1 ��� ������)
TimeSinceLastDecayUpdate = FMath::FRandRange(0.0f, 60.0f);
```

---

## ?? �������������� ���������

- **Performance Analysis:** [`Performance_Freeze_Analysis.md`](Performance_Freeze_Analysis.md)
- **Architecture Overview:** [`Complete_Architecture_Overview.md`](Complete_Architecture_Overview.md)
- **Scalability Roadmap:** [`Scalability_And_Future_Enhancements.md`](Scalability_And_Future_Enhancements.md)

---

**�������� �����������:** 2025-01-23  
**������:** 1.0  
**������:** ? Complete

**�����:** Memory Decay - ��� **���������� ������ feature** ��� �������� relationship mechanics, � �������� � ������� **����� ������� �������** (��������� offset).
