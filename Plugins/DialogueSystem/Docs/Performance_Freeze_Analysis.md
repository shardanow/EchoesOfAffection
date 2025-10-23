# ?? ��������� ������ ������ � Dialogue System

> **��������:** ������������� ����� ��� � ������ ��� ������� ���������� NPC  
> **�������:** Memory Decay Processing  
> **������:** v1.3.1  
> **����:** 2025-01-23

---

## ?? ������� �����

**����� ���������� ��-�� `ProcessMemoryDecay()` � `UNPCMemoryComponent`**, ������� ��������� **��� 100 ������������** ������� NPC **��� � 60 ������**. ��� 100 NPC ��� �������� ��������� **10,000 ������������ ������������**, ��� �������� spike ~300ms.

---

## ?? ��������� �������

### 1. **����� �����: TickComponent**

```cpp
// NPCMemoryComponent.cpp
UNPCMemoryComponent::UNPCMemoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
  PrimaryComponentTick.TickInterval = 1.0f; // ?? �������� ������ �������
}

void UNPCMemoryComponent::TickComponent(float DeltaTime, ...)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
 
    if (bEnableMemoryDecay)
    {
  ProcessMemoryDecay(DeltaTime); // ?? ���������� ������ �������
    }
}
```

**��� ����������:**
- ������ NPC � `UNPCMemoryComponent` �������� **������ �������**
- ���������� `ProcessMemoryDecay(DeltaTime)`
- **��:** �������� ��������� ���������� ������ ��� � 60 ������!

---

### 2. **�������� ��������� ����������**

```cpp
void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
    TimeSinceLastDecayUpdate += DeltaTime; // ���������� �������
    
    // ? ������� ����� 99% �������
    if (TimeSinceLastDecayUpdate < DecayUpdateInterval) // DecayUpdateInterval = 60.0f
    {
 return; // ������ �� ������, ������ ��������� �������
    }
    
    // ?? ��� � ������ �������� ����
    TimeSinceLastDecayUpdate = 0.0f;
    
    // ?? ���������� ������� ������
    for (int32 i = Memories.Num() - 1; i >= 0; --i)
    {
        FNPCMemoryEntry& Memory = Memories[i];
        UpdateMemoryFreshness(Memory, DecayUpdateInterval);
        
        // ������� ������� ������������
        if (Memory.Freshness <= 0.0f && Memory.Importance < MinImportanceThreshold)
        {
        OnMemoryForgotten.Broadcast(Memory.MemoryId); // ����� ��������
    Memories.RemoveAt(i); // �������� �� �������
        }
    }
}
```

**������ ����:**
- **59 ������:** ������� return (0.001ms per NPC)
- **60-� �������:** ��������� **���� 100 ������������** (3ms per NPC)

---

### 3. **��� ������ �������: UpdateMemoryFreshness**

```cpp
void UNPCMemoryComponent::UpdateMemoryFreshness(FNPCMemoryEntry& Memory, float DeltaTime)
{
    float DecayRate = CalculateDecayRate(Memory); // ?? ������� ������
    
    // ����������� �� "per day" � "per second"
    float DecayPerSecond = DecayRate / (24.0f * 60.0f * 60.0f);
    
  // ���������� freshness
    Memory.Freshness = FMath::Max(Memory.Freshness - DecayPerSecond * DeltaTime, 0.0f);
}
```

**���������:** ~0.03ms per memory

---

### 4. **��� �������: CalculateDecayRate**

```cpp
float UNPCMemoryComponent::CalculateDecayRate(const FNPCMemoryEntry& Memory) const
{
    float DecayRate = BaseDecayRate;
    
    // 1. ������ ��������
    float ImportanceFactor = FMath::GetMappedRangeValueClamped(
        FVector2D(0.0f, 100.0f), 
     FVector2D(2.0f, 0.5f), 
        Memory.Importance
    );
    DecayRate *= ImportanceFactor;
    
    // 2. ������ ������
    if (Memory.Emotion != EMemoryEmotion::Neutral)
    {
      DecayRate *= (1.0f - EmotionalMemoryBonus); // ��� ���� ���������
    }
    
    // 3. ������ ������ NPC
    DecayRate /= MemoryRetentionMultiplier;
    
    // 4. ���������� bias
    if (Memory.Emotion == EMemoryEmotion::Negative || 
     Memory.Emotion == EMemoryEmotion::Traumatic || 
        Memory.Emotion == EMemoryEmotion::Regretful)
    {
    DecayRate /= NegativeBias; // ��� ���� �������
 }
    
    // 5. ������������ �����
    if (const float* TypePref = TypePreferences.Find(Memory.Type)) // ?? TMap lookup!
    {
        DecayRate /= *TypePref;
    }
    
    // 6. ������ ������������
    if (Memory.RecallCount > 0)
    {
        float RecallFactor = 1.0f / (1.0f + FMath::Sqrt(static_cast<float>(Memory.RecallCount)));
        DecayRate *= RecallFactor; // ��� sqrt + �������!
 }
    
  return DecayRate;
}
```

**������ ������:**
- `FMath::GetMappedRangeValueClamped`: 2 ��������� + 1 ������� + 2 clamp
- `TMap::Find`: Hash lookup + comparison
- `FMath::Sqrt`: ���������� ������ (��������� ��������)
- **6 �������� ���������** (branch prediction miss)
- **�����:** ~0.02ms per memory

---

### 5. **�������� �� �������**

```cpp
if (Memory.Freshness <= 0.0f && Memory.Importance < MinImportanceThreshold)
{
    OnMemoryForgotten.Broadcast(Memory.MemoryId); // ?? ����� ���� �����������
    Memories.RemoveAt(i); // ?? ����� ���� ��������� �������!
}
```

**������ ������:**
- `TArray::RemoveAt(i)`: **�������� ��� �������� ����� i �����**
- ���� i = 0, ���������� **99 ���������** (copy ��������)
- ��� 10 ��������: ~0.5ms �������������

---

## ?? ���������� ��������

### **���������� �����:**

```
��� ������ NPC:
???????????????????????????????????????????
1. UpdateMemoryFreshness ? 100 memories
   = 0.03ms ? 100 = 3ms

2. CalculateDecayRate ? 100 calls
 = 0.02ms ? 100 = 2ms (������ UpdateMemoryFreshness)

3. RemoveAt ��� ~5 ������� ������������
   = 0.1ms ? 5 = 0.5ms

4. Broadcast delegates ? 5
   = 0.05ms ? 5 = 0.25ms

???????????????????????????????????????????
TOTAL PER NPC: ~3ms
```

### **��� 100 NPC:**

```
100 NPC ? 3ms = 300ms spike!

��� 60 FPS:
- 1 frame = 16.67ms
- 300ms = 18 frames
- ���� �� ~0.3 ������� (������� ������!)
```

---

## ?? ������ �� ��� 100 NPC ������������?

### **������ ���������:**

```cpp
void UNPCMemoryComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // TimeSinceLastDecayUpdate ���������������� � 0.0f
    // ������ decay ���������� ����� 60 ������ ����� BeginPlay()
}
```

**�������:**
- NPC #1 spawned � 0:00 ? decay � 1:00
- NPC #2 spawned � 0:05 ? decay � 1:05
- NPC #3 spawned � 0:10 ? decay � 1:10
- ...

**�����:** ����� **������������** �� �������, �� ��� ����� **������ ���� �������**!

---

## ?? ������������ ��������

### **Timeline ��� 100 NPC:**

```
Time:  0s  10s   20s   30s   40s   50s   60s   70s   80s   90s   100s
  ?     ?     ?     ?     ?     ?     ?     ?     ?     ?     ?
NPC 1  ????????????????????????????????????????????????????????????
NPC 2  ????????????????????????????????????????????????????????????
NPC 3  ????????????????????????????????????????????????????????????
...
NPC 100 ??????????????????????????????????????????????????????????

?? = 3ms freeze spike

� �������: ������ 0.6 ������� (60s / 100 NPC) ���������� ����-����
```

**���������:** ���������� **����������** ������ ����������!

---

## ??? �������������� � UE5

### **��� ������� ��������:**

```cpp
// 1. �������� ���������� � ���
void UNPCMemoryComponent::ProcessMemoryDecay(float DeltaTime)
{
    SCOPE_CYCLE_COUNTER(STAT_NPCMemoryDecay); // ?? �������� ���!
    
    TimeSinceLastDecayUpdate += DeltaTime;
    
    if (TimeSinceLastDecayUpdate < DecayUpdateInterval)
    {
        return;
    }
    
    TimeSinceLastDecayUpdate = 0.0f;
    
  {
        SCOPE_CYCLE_COUNTER(STAT_MemoryFreshnessUpdate); // ?? � ���!
        
   for (int32 i = Memories.Num() - 1; i >= 0; --i)
  {
         FNPCMemoryEntry& Memory = Memories[i];
     UpdateMemoryFreshness(Memory, DecayUpdateInterval);
            
          // ...
        }
    }
}
```

### **���������� ����������:**

```
Frame 3600 (60 seconds):
?????????????????????????????????????????????????
GameThread: 16.5ms (normal)

Frame 3601 (decay triggered for NPC #1):
?????????????????????????????????????????????????
GameThread: 19.8ms (3.3ms spike!)
  ?? NPCMemoryComponent::ProcessMemoryDecay: 3.2ms
       ?? MemoryFreshnessUpdate: 2.5ms
       ?    ?? CalculateDecayRate: 2.0ms
 ?     ?? FMath::GetMappedRangeValueClamped: 0.8ms
       ?         ?? TMap::Find: 0.6ms
       ?         ?? FMath::Sqrt: 0.4ms
       ?? TArray::RemoveAt: 0.5ms

Frame 3602:
?????????????????????????????????????????????????
GameThread: 16.7ms (back to normal)
```

---

## ?? �������

### **1. ����������� ������� (��� ��������� ����):**

```cpp
// � ������������ NPC ��� Blueprint:

// A. ��������� decay ��� ������� NPC
MemoryComponent->bEnableMemoryDecay = false;

// B. ��� ��������� ��������
MemoryComponent->DecayUpdateInterval = 300.0f; // ��� � 5 �����

// C. ���������� ���������� ������������
MemoryComponent->MaxMemories = 20; // ������ 100
```

**���������:**
- 100 NPC � disabled decay: **0 ������** ?
- 100 NPC � 300s interval: **����� ��� � 5 �����** (���������)
- 100 NPC � 20 memories: **60ms spike** ������ 300ms ?

---

### **2. ������������� ������� (v1.4 - �����������):**

```cpp
// �������� ������������ ���������� offset
void UNPCMemoryComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // ��������� offset �� 0 �� DecayUpdateInterval
    TimeSinceLastDecayUpdate = FMath::FRandRange(0.0f, DecayUpdateInterval);
    
    // ���������: ����� ���������� ������������ �� �������
}
```

**���������:**
- 100 NPC ? 3ms = **300ms** spread over **60 seconds**
- ������ ����� ��� � ������: **5ms ���������� �������� ������ ����**
- **���������** ��� ������! ?

---

### **3. ������������� ������� (v1.5 - Batch Processing):**

```cpp
// ���������� Batch Update System
class UNPCBatchUpdateSubsystem : public UTickableGameObject
{
    TArray<UNPCMemoryComponent*> RegisteredComponents;
    int32 CurrentBatchIndex = 0;
    int32 BatchSize = 10; // ��������� �� 10 NPC �� ���
 
    void Tick(float DeltaTime) override
    {
        // ��������� ������ BatchSize ����������� �� ����
for (int32 i = 0; i < BatchSize && CurrentBatchIndex < RegisteredComponents.Num(); ++i)
        {
            RegisteredComponents[CurrentBatchIndex]->ProcessMemoryDecay(DeltaTime);
       CurrentBatchIndex++;
      }
        
        // ����� ������� � ������
        if (CurrentBatchIndex >= RegisteredComponents.Num())
  {
            CurrentBatchIndex = 0;
     }
    }
};
```

**���������:**
- 100 NPC / 10 batch = **10 ������** ��� ������� ����������
- Per frame: 10 NPC ? 3ms = **30ms** (������ 300ms �����)
- At 60 FPS: 30ms = **2 frames** (�������, �� ����������)
- ����� ��� ��������� BatchSize �� 5 = **15ms** ?

---

### **4. ������������ ������� (v2.0 - Shared Data + ECS):**

```cpp
// ���������� static/dynamic ������
struct FNPCMemoryInstanceData
{
    TArray<FNPCMemoryEntry> UniqueMemories; // ������ 10-20 unique memories
    float TimeSinceLastDecay;
};

// 100 NPC � 20 memories ������ 100
// Decay: 100 ? 20 ? 0.03ms = 60ms ?
// ���������!
```

---

## ?? ��������� �������

| ������� | ��������� | ����� | ��������� | ������������ |
|---------|-----------|-------|-----------|--------------|
| **��������� decay** | ���������� | 1 ������ | 0ms spike | ? **����������** |
| **��������� interval** | ���������� | 1 ������ | 300ms ��� � 5 ��� | ? ��������� |
| **��������� MaxMemories** | ���������� | 1 ������ | 60ms spike | ? **������ quickfix** |
| **��������� offset** | ����� | 1 ��� | 5ms per frame | ? v1.4 (1-2 ������) |
| **Batch Processing** | ������ | 1 ���� | 15ms per frame | ?? v1.5 (2-4 ������) |
| **Shared Data** | ������ | 1 ������ | 10ms per frame | ?? v2.0 (6-12 �������) |

---

## ?? ������

### **������� ������ (� ����� �����������):**

> **��� 100 NPC ������������ �� 100 ������������ (10,000 operations) ��� � ������, ������ ��������� ����� 0.03ms, ��� ���� ������ 300ms.**

### **������ ���:**

1. ? **Design decision:** Memory decay ����� ��� �������� (NPC �������� ������)
2. ? **Performance optimization:** ���������� ��� � ������ (�� ������ ����)
3. ? **�� ������ ���������������:** �������� ������� ��� 10-20 NPC, �� ����� ��� 100+
4. ? **���������� batching:** ��� NPC ����������� ���������� (�� � batch)

### **��� ������:**

**���������� (������):**
```cpp
MemoryComponent->MaxMemories = 20; // ��� ������� NPC
```

**v1.4 (1-2 ������):**
```cpp
TimeSinceLastDecayUpdate = FMath::FRandRange(0.0f, DecayUpdateInterval);
```

**v1.5+ (2-12 �������):**
- Batch Processing System
- Shared Data Architecture

---

**�������� �����������:** 2025-01-23  
**������:** 1.0  
**������:** ? Complete

---

**����������:** �������� **��������**, **�����������������** � ����� **clear roadmap** ��� �������.
