# ?? Memory Effects: ������ ���������� � Dialogue System

> **������:** ����� �� ��������� ������������ ����� Data Asset � �������?  
> **�����:** ��������. ��������� ��������� ����������.  
> **������:** v1.3.1  
> **����:** 2025-01-23

---

## ? ��� ��� �����������

### **1. Memory Effects ����������**

������ ��������� � `DialogueSystemCore`:

```cpp
// Plugins/DialogueSystem/Source/DialogueSystemCore/Public/Effects/MemoryEffects.h

// ������� �����
UMemoryEffectBase : public UObject

// ���������� �������:
?? UAddMemoryEffect           // ������� ������������
?? UModifyMemoryStrengthEffect // �������� ����
?? URemoveMemoryEffect// �������
?? URefreshMemoryEffect        // "��������"
```

**���������:**

| Effect Class | ��������� | ��� ������ |
|--------------|-----------|------------|
| `UAddMemoryEffect` | `MemoryTag`, `MemoryStrength (0-1)` | ������� ����� ������������ |
| `UModifyMemoryStrengthEffect` | `MemoryTag`, `StrengthModifier`, `bAdditive` | �������� ���� ������������� |
| `URemoveMemoryEffect` | `MemoryTag` | ������� ������������ |
| `URefreshMemoryEffect` | `MemoryTag` | "���������" timestamp |

---

### **2. Memory Effects �������� �������� � NPC**

```cpp
// MemoryEffects.cpp - ����������
void UAddMemoryEffect::Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const
{
    // ������� NPCMemoryComponent ����� Reflection
    UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
    
    // �������� AddMemoryByTag ����� ProcessEvent (Reflection)
    MemoryComp->ProcessEvent(AddMemoryFunc, &Params);
}
```

**�����:** ������������ **reflection** ��� ������ ������� NPCMemoryComponent, ����� �������� ����������� `DialogueSystemCore ? DialogueSystemRuntime`.

---

### **3. Dialogue Effects ����������**

������ ��������� � `DialogueSystemCore`:

```cpp
// Plugins/DialogueSystem/Source/DialogueSystemCore/Public/Effects/DialogueEffectExecutor.h

// ������� �����
UDialogueEffect : public UObject

// ���������� �������:
?? UDialogueEffect_ModifyAffinity // �������� ��������
?? UDialogueEffect_ModifyTrust         // �������� �������
?? UDialogueEffect_ModifyInventory     // ����/������� �������
?? UDialogueEffect_SetMemory           // ���������� ����� ����
?? UDialogueEffect_SetVariable       // ���������� ����������
?? UDialogueEffect_AddWorldStateTag    // �������� ���
?? UDialogueEffect_StartQuest     // ��������� �����
```

---

### **4. DialogueNode ����� Effects Array**

```cpp
// DialogueNode.h
UCLASS()
class UDialogueNode : public UObject
{
    GENERATED_BODY()
    
    /** ������� ���� */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Dialogue")
    TArray<TObjectPtr<UDialogueEffect>> Effects;
};
```

**������:** � ������ ����� ��������� ������� � Data Asset!

---

## ? ��� �� �������� (��������)

### **�������� 1: Memory Effects �� ��������� UDialogueEffect**

```cpp
// ? ������� ����������:
UMemoryEffectBase : public UObject
{
virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor);
    //   ? ������ ���������!
};

UDialogueEffect : public UObject
{
    virtual void Execute_Implementation(UDialogueSessionContext* Context);
//      ? ������ ��������!
};
```

**��������:** ������ ��������� `Execute()`:
- `UMemoryEffectBase::Execute(AActor*, AActor*)` - ��������� ������� ��������
- `UDialogueEffect::Execute(UDialogueSessionContext*)` - ��������� ��������

**���������:** Memory Effects **������ �������� � `DialogueNode::Effects`** ��������!

---

### **�������� 2: ��� ��������/Wrapper**

```cpp
// ����� ���-�� ����:
class UDialogueEffect_Memory : public UDialogueEffect
{
    UMemoryEffectBase* MemoryEffect; // Wrapper!
    
    virtual void Execute_Implementation(UDialogueSessionContext* Context) override
    {
        // ������� ������� �� Context
        AActor* NPC = Context->GetNPC();
        AActor* Player = Context->GetPlayer();
        
        // ������� ������������ Memory Effect
        MemoryEffect->Execute(NPC, Player);
    }
};
```

**��:** ������ wrapper'� **���** � ������� ����������!

---

### **�������� 3: DialogueNode Not Exposing Memory Effects � UI**

���� ���� �� wrapper �����������, � Dialogue Editor ��� UI ��� ���������� Memory Effects.

---

## ?? �������

### **������� 1: ������� Wrapper Classes (�������������)**

������� ������������� ������-��������:

```cpp
// NEW: DialogueEffect_AddMemory.h
UCLASS(DisplayName = "Add Memory", meta = (Category = "Memory"))
class UDialogueEffect_AddMemory : public UDialogueEffect
{
    GENERATED_BODY()
    
public:
    /** Memory Tag */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FGameplayTag MemoryTag;
    
    /** Memory Strength (0-1) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
  float MemoryStrength = 1.0f;
    
    virtual void Execute_Implementation(UDialogueSessionContext* Context) override
    {
        AActor* NPCActor = Context->GetNPCActor();
   AActor* PlayerActor = Context->GetPlayerActor();
        
   if (!NPCActor)
        {
            return;
        }
   
        // ������� ������������ Memory Effect ����� reflection
UClass* MemoryCompClass = FindObject<UClass>(nullptr, TEXT("/Script/DialogueSystemRuntime.NPCMemoryComponent"));
   if (!MemoryCompClass) return;
        
        UActorComponent* MemoryComp = NPCActor->GetComponentByClass(MemoryCompClass);
    if (!MemoryComp) return;
        
  UFunction* AddMemoryFunc = MemoryComp->FindFunction(TEXT("AddMemoryByTag"));
        if (!AddMemoryFunc) return;
        
        struct FAddMemoryParams
        {
            FGameplayTag Tag;
    float Strength;
        };
        
        FAddMemoryParams Params;
        Params.Tag = MemoryTag;
        Params.Strength = MemoryStrength;
        
        MemoryComp->ProcessEvent(AddMemoryFunc, &Params);
    }
    
    virtual FText GetDisplayText_Implementation() const override
    {
    return FText::Format(
FText::FromString("Add Memory: {0} (Strength: {1})"),
     FText::FromString(MemoryTag.ToString()),
            FText::AsNumber(MemoryStrength)
        );
 }
};
```

**���������� ���:**
- `UDialogueEffect_ModifyMemoryStrength`
- `UDialogueEffect_RemoveMemory`
- `UDialogueEffect_RefreshMemory`

---

### **������� 2: ������������ DSL Effects (������� Workaround)**

���� wrapper'�� ���, ����� ������������ **��������� DSL effects**:

```cpp
// � DialogueNode::EffectExpression
EffectExpression = "AddMemory(Event.Betrayal, 0.95); ModifyAffinity(Anna, -40)";
```

**��:** DSL parser (`UDialogueEffectParser`) **�� ������������ Memory Effects** � ������� ����������!

����� �������� � `DialogueEffectParser.cpp`:

```cpp
// NEW: �������� ������� Memory Effects
UDialogueEffect* UDialogueEffectParser::ParseSingleEffect(const TArray<FString>& Tokens)
{
    FString Command = Tokens[0].ToLower();
    TArray<FString> Args = Tokens;
    Args.RemoveAt(0);
    
    // ... existing parsers ...
    
    // NEW: Memory Effects
    if (Command == TEXT("addmemory"))
  {
        return ParseAddMemory(Args);
  }
    else if (Command == TEXT("modifymemorystrength"))
    {
        return ParseModifyMemoryStrength(Args);
 }
    // ... etc
}

UDialogueEffect* UDialogueEffectParser::ParseAddMemory(const TArray<FString>& Args)
{
    if (Args.Num() < 2)
    {
        UE_LOG(LogDialogueParser, Error, TEXT("AddMemory requires 2 arguments: tag, strength"));
    return nullptr;
 }
    
    // ������� wrapper!
    UDialogueEffect_AddMemory* Effect = NewObject<UDialogueEffect_AddMemory>();
    Effect->MemoryTag = FGameplayTag::RequestGameplayTag(FName(*Args[0]), false);
    Effect->MemoryStrength = FCString::Atof(*Args[1]);
    
    return Effect;
}
```

---

### **������� 3: Blueprint Approach (��������� �������)**

������� Blueprint-��������� `UDialogueEffect` � ������� Memory Component:

```
Blueprint: BP_DialogueEffect_AddMemory
Parent: UDialogueEffect

Variables:
- MemoryTag: FGameplayTag
- MemoryStrength: float

Execute (Override):
?? Get NPC Actor (from Context)
?? Get Component by Class: NPCMemoryComponent
?? NPCMemoryComponent ? Add Memory By Tag
   ?? Memory Tag: MemoryTag
   ?? Strength: MemoryStrength
```

**����:** �������� ����� ������ ��� ��������� ����  
**�����:** ����� ��������� ��������� Blueprint ��� ������� ���� �������  

---

## ?? ���� ���������� (v1.4)

### **���� 1: ������� Wrapper Classes (1 ����)**

```
����� �����:
????????????????????????????????????????????????????????????????????
Plugins/DialogueSystem/Source/DialogueSystemCore/Public/Effects/
?? DialogueMemoryEffects.h     (�����)
?? DialogueMemoryEffects.cpp   (�����)

������:
?? UDialogueEffect_AddMemory
?? UDialogueEffect_ModifyMemoryStrength
?? UDialogueEffect_RemoveMemory
?? UDialogueEffect_RefreshMemory
```

**�����:** ~300-400 ����� ����

---

### **���� 2: �������� DSL Parser Support (2-3 ����)**

```
��������� � DialogueEffectParser.cpp:
????????????????????????????????????????????????????????????????????
1. ParseAddMemory()
2. ParseModifyMemoryStrength()
3. ParseRemoveMemory()
4. ParseRefreshMemory()
5. ����������� � ParseSingleEffect()
```

**�����:** ~100-150 ����� ����

---

### **���� 3: �������� ������������ (1 ���)**

```
�������� � README:
????????????????????????????????????????????????????????????????????
1. ������� ������������� Memory Effects � Data Asset
2. DSL ���������: AddMemory(tag, strength)
3. Blueprint �������
```

---

## ?? ������� ������: ��� ������������

### **? ��� ����� ������ ������:**

#### **������� A: ���������� �� C++**

```cpp
// � ����� ����� ������� ������
UNPCMemoryComponent* MemComp = NPC->FindComponentByClass<UNPCMemoryComponent>();
MemComp->AddMemoryByTag(
    FGameplayTag::RequestGameplayTag(FName("Event.Betrayal")),
 0.95f
);
```

#### **������� B: ����� Blueprint Events**

```
Event Graph (Dialogue Blueprint):
????????????????????????????????????????????????????????????????????
Event: On Dialogue Choice Selected
?? Branch: Choice ID == "BetrayChoice"?
?  ?? True:
?     ?? Get NPC Component: NPCMemoryComponent
?   ?? Add Memory By Tag
?  ?? Tag: "Event.Betrayal"
?    ?? Strength: 0.95
```

#### **������� C: ����� Custom DialogueEffect (Blueprint)**

������� Blueprint-����� `BP_DialogueEffect_AddMemory` ��� ������� � �������� 3 ����.

---

### **? ��� �� �������� ������:**

#### **� Dialogue Data Asset:**

```cpp
// ? �� �������� - Memory Effects �� � ������!
DialogueNode.Effects.Add(UAddMemoryEffect);

// ? �� �������� - DSL parser �� ����� ��� AddMemory
DialogueNode.EffectExpression = "AddMemory(Event.Betrayal, 0.95)";
```

---

## ?? �������� �������

| ������ | ������ | ��������� | ����� |
|--------|--------|-----------|-------|
| **���������� (C++)** | ? �������� | ? ����� | ������ |
| **Blueprint Events** | ? �������� | ? ����� | ������ |
| **Custom BP Effect** | ? �������� | ?? ������ | ������ (1 ��� setup) |
| **Data Asset (Wrapper)** | ? ������� ���� | ??? ������ | v1.4 (1 ����) |
| **Data Asset (DSL)** | ? ������� ���� | ?? ����� | v1.4 (3 ����) |

---

## ?? ������������

### **��� ������������ ������������� (������):**

**����������� Custom Blueprint Effect:**

1. �������� Blueprint ����� `BP_DialogueEffect_AddMemory`
2. Parent Class: `UDialogueEffect`
3. �������� ���������� `MemoryTag`, `MemoryStrength`
4. � `Execute` ������� �������� `NPCMemoryComponent->AddMemoryByTag()`
5. �������� ���� Blueprint � `DialogueNode::Effects` ����� Dialogue Editor

**�����:** 30-60 ����� setup  
**���������:** �������� ����� ������! ?

---

### **��� ������������� ������� (v1.4):**

**����������� Wrapper Classes:**

1. ������� `UDialogueEffect_AddMemory` � ��������� wrapper'� (C++)
2. �������� DSL parser support
3. �������� ������������

**�����:** 1 ���� ����������  
**���������:** ������ ���������� � Dialogue System ?

---

## ?? ������: ������� ������� (Blueprint)

### **��� 1: ������� Blueprint Effect**

```
Content Browser ? Right Click ? Blueprint Class
Parent: DialogueEffect
Name: BP_DialogueEffect_AddMemory
```

### **��� 2: �������� Variables**

```
Variables:
?? MemoryTag (FGameplayTag)
?  ?? Default: Event.Betrayal
?? MemoryStrength (float)
   ?? Default: 0.95
```

### **��� 3: Implement Execute**

```
Event Graph:
????????????????????????????????????????????????????????????????????
Event: Execute (Override)
?? Context ? Get NPC Actor
?? Get Component by Class: NPCMemoryComponent
?? Add Memory By Tag
   ?? Memory Tag: MemoryTag (variable)
   ?? Strength: MemoryStrength (variable)
```

### **��� 4: ������������ � Data Asset**

```
Dialogue Data Asset ? DialogueNode
?? Effects:
   ?? Add Element
      ?? Select: BP_DialogueEffect_AddMemory
   ?? Memory Tag: Event.Rescue
         ?? Memory Strength: 0.85
```

**������!** ������ ��� ������ ���� ������� ��������� ������������! ??

---

**�������� �����������:** 2025-01-23  
**������:** 1.0  
**������:** ? Complete

**�����:** Memory Effects ����������, �� �� ������������� �������� � Dialogue System. **��������� �������:** Custom Blueprint Effects (30 �����). **������ �������:** Wrapper classes � v1.4 (1 ����). ??
