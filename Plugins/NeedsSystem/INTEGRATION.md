# Needs System - Integration Guide

## Integration with Time System

### Automatic Integration

Subsystem автоматически пытается найти и подписаться на Time System при инициализации:

```cpp
// In NeedsSubsystem::Initialize()
void UNeedsSubsystem::SubscribeToTimeSystem()
{
    TimeSystem = GetWorld()->GetSubsystem<UTimeOfDaySubsystem>();
 
    if (TimeSystem)
    {
        TimeSystem->OnHourChanged.AddDynamic(this, &UNeedsSubsystem::OnTimeSystemHourChanged);
        // Sync initial time
   CurrentGameTimeHours = CalculateFromTimeSystem();
    }
}
```

**Что происходит:**
1. При старте Needs Subsystem ищет Time Subsystem
2. Подписывается на события (`OnHourChanged`, `OnDayChanged`, etc.)
3. Синхронизирует начальное время
4. Обновления идут через события (НЕ через Tick)

### Fallback to Real Time

Если Time System не найдена:

```cpp
// In Settings
bUseGameTime = false;
RealTimeScale = 24.0f; // 24 game hours per real hour

// In Subsystem
CurrentGameTimeHours += DeltaTime * RealTimeScale;
```

### Time-Based Modifiers

Модификаторы могут активироваться по времени суток:

```cpp
// In UNeedModifierAsset
FModifierCondition ActivationCondition;
{
    bUseTimeCondition = true;
    RequiredTimeStart = 22; // 10 PM
    RequiredTimeEnd = 6;  // 6 AM
}

// Example: Night Owl Modifier
// Active only during night (22:00 - 06:00)
Modifications:
  - Energy Decay: Multiply 0.5 (slower decay at night)
```

---

## Integration with Inventory System

### Consumable Items

```cpp
// Item Data
USTRUCT()
struct FConsumableItemData
{
    UPROPERTY()
    UNeedEffectAsset* ConsumptionEffect;
    
    UPROPERTY()
    int32 StackSize = 1;
};

// On Item Use
void UInventoryComponent::UseItem(FItemID ItemID)
{
    FConsumableItemData* ItemData = GetItemData(ItemID);
    if (!ItemData || !ItemData->ConsumptionEffect)
    {
        return;
    }
    
    // Apply effect to character
    UCharacterNeedsComponent* NeedsComp = Owner->FindComponentByClass<UCharacterNeedsComponent>();
    if (NeedsComp)
    {
        NeedsComp->ApplyEffect(ItemData->ConsumptionEffect, ItemData->StackSize);
}
    
 // Remove from inventory
    RemoveItem(ItemID, 1);
}
```

### Item Quality System

```cpp
// Different quality = different effect power
USTRUCT()
struct FQualityMultipliers
{
    UPROPERTY()
    float PoorQuality = 0.5f;  // 50% effect
    
    UPROPERTY()
    float NormalQuality = 1.0f;  // 100% effect
    
    UPROPERTY()
    float ExcellentQuality = 1.5f;  // 150% effect
};

// Apply with quality modifier
void ApplyConsumableWithQuality(UNeedEffectAsset* Effect, EItemQuality Quality)
{
    // Clone effect and modify values
    UNeedEffectAsset* ModifiedEffect = DuplicateEffect(Effect);
    
    float Multiplier = GetQualityMultiplier(Quality);
    
    for (FNeedEffectModification& Mod : ModifiedEffect->Modifications)
    {
        Mod.ValueChange *= Multiplier;
    }
  
    ApplyEffect(ModifiedEffect, 1);
}
```

---

## Integration with AI System

### Behavior Tree Decorators

```cpp
// BTDecorator_CheckNeed.h
UCLASS()
class UBTDecorator_CheckNeed : public UBTDecorator
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    FGameplayTag NeedTag;
    
    UPROPERTY(EditAnywhere)
    ENeedThresholdLevel RequiredLevel;
    
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override
    {
        AAIController* AI = OwnerComp.GetAIOwner();
    if (!AI)
    {
     return false;
        }
 
        UCharacterNeedsComponent* NeedsComp = AI->GetPawn()->FindComponentByClass<UCharacterNeedsComponent>();
        if (!NeedsComp)
        {
       return false;
        }
        
        return NeedsComp->GetNeedThreshold(NeedTag) <= RequiredLevel;
}
};

// Usage in Behavior Tree:
// Decorator: Check Need (Need.Hunger, Critical)
//   ??> Task: Find Food
```

### AI Tasks

```cpp
// BTTask_SatisfyNeed.h
UCLASS()
class UBTTask_SatisfyNeed : public UBTTaskNode
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    FGameplayTag NeedTag;

    UPROPERTY(EditAnywhere)
    UNeedEffectAsset* SatisfactionEffect;
    
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override
    {
    AAIController* AI = OwnerComp.GetAIOwner();
        UCharacterNeedsComponent* NeedsComp = AI->GetPawn()->FindComponentByClass<UCharacterNeedsComponent>();
        
   if (NeedsComp && SatisfactionEffect)
        {
    NeedsComp->ApplyEffect(SatisfactionEffect, 1);
            return EBTNodeResult::Succeeded;
}
        
        return EBTNodeResult::Failed;
    }
};

// Usage:
// Task: Satisfy Need (Need.Hunger, DA_Effect_EatFood)
```

### Utility AI Integration

```cpp
// Utility consideration based on need value
class UNeedConsideration : public UUtilityConsideration
{
    FGameplayTag NeedTag;
    
    float Evaluate(AAIController* Controller) const override
{
        UCharacterNeedsComponent* NeedsComp = GetNeedsComponent(Controller);
    if (!NeedsComp)
        {
            return 0.0f;
        }
     
        float NeedValue = NeedsComp->GetNeedValue(NeedTag);
        
    // Invert: low need value = high score
     return (100.0f - NeedValue) / 100.0f;
    }
};
```

---

## Integration with Character System

### Character Stats Influence

```cpp
// Trait system affects needs
USTRUCT()
struct FCharacterTraits
{
    UPROPERTY()
    int32 Constitution = 5;  // 1-10
    
    UPROPERTY()
    int32 Metabolism = 5;  // 1-10
};

// On character creation
void AMyCharacter::ApplyTraitModifiers(const FCharacterTraits& Traits)
{
    UCharacterNeedsComponent* NeedsComp = FindComponentByClass<UCharacterNeedsComponent>();
    if (!NeedsComp)
    {
  return;
    }
    
    // High constitution = slower energy decay
    if (Traits.Constitution >= 7)
    {
        NeedsComp->AddModifier(AthleticModifier);
    }
    
    // High metabolism = faster hunger decay
    if (Traits.Metabolism >= 7)
    {
        NeedsComp->AddModifier(FastMetabolismModifier);
    }
    
// Low metabolism = slower hunger but slower recovery
    if (Traits.Metabolism <= 3)
  {
        NeedsComp->AddModifier(SlowMetabolismModifier);
    }
}
```

### Level-Based Modifiers

```cpp
// As character levels up, needs become easier to manage
void AMyCharacter::OnLevelUp(int32 NewLevel)
{
    UCharacterNeedsComponent* NeedsComp = FindComponentByClass<UCharacterNeedsComponent>();
    if (!NeedsComp)
    {
  return;
    }
    
    // Every 10 levels, reduce decay rates
    if (NewLevel % 10 == 0)
    {
        UNeedModifierAsset* LevelModifier = CreateLevelModifier(NewLevel);
        NeedsComp->AddModifier(LevelModifier);
    }
}

UNeedModifierAsset* AMyCharacter::CreateLevelModifier(int32 Level)
{
 UNeedModifierAsset* Modifier = NewObject<UNeedModifierAsset>();
    Modifier->ModifierTag = FGameplayTag::RequestGameplayTag("Modifier.Level");
    Modifier->Priority = 100;
    
    // Reduce all decay rates by 5% per 10 levels
    float DecayMultiplier = 1.0f - (Level / 10 * 0.05f);
    
    for (const FGameplayTag& NeedTag : AllNeedTags)
    {
        FNeedBehaviorModification Mod;
 Mod.TargetNeedTag = NeedTag;
        Mod.bModifyDecayRate = true;
        Mod.DecayOperation = ENeedModifierOperation::Multiply;
        Mod.DecayValue = DecayMultiplier;
   
        Modifier->Modifications.Add(Mod);
    }
    
    return Modifier;
}
```

---

## Integration with Quest System

### Quest Objectives Based on Needs

```cpp
// Quest objective: Reach Critical Hunger
UCLASS()
class UQuestObjective_ReachNeedState : public UQuestObjective
{
    UPROPERTY()
    FGameplayTag NeedTag;
    
    UPROPERTY()
    ENeedThresholdLevel TargetState;
    
    virtual void OnActivated() override
    {
        UCharacterNeedsComponent* NeedsComp = GetPlayerNeedsComponent();
        if (NeedsComp)
     {
          NeedsComp->OnNeedThresholdChanged.AddDynamic(this, &UQuestObjective_ReachNeedState::OnThresholdChanged);
     }
    }
    
    void OnThresholdChanged(FGameplayTag Tag, ENeedThresholdLevel Old, ENeedThresholdLevel New)
    {
   if (Tag == NeedTag && New == TargetState)
      {
      CompleteObjective();
        }
    }
};
```

### Quest Rewards as Effects

```cpp
// Quest completion applies beneficial effects
void UQuestReward::ApplyReward(ACharacter* Character)
{
    UCharacterNeedsComponent* NeedsComp = Character->FindComponentByClass<UCharacterNeedsComponent>();
    if (!NeedsComp)
    {
        return;
 }
    
    // Reward: Fully restore all needs
    for (const FGameplayTag& NeedTag : NeedsComp->GetAllNeedTags())
  {
        NeedsComp->SetNeedValue(NeedTag, 100.0f);
    }
    
    // Add temporary buff
    NeedsComp->AddModifier(QuestCompletionBuffModifier);
}
```

---

## Integration with Save System

### Serialization

```cpp
// Save data structure
USTRUCT()
struct FNeedsSaveData
{
    GENERATED_BODY()
    
    UPROPERTY()
    TMap<FGameplayTag, float> NeedValues;
    
    UPROPERTY()
    float CurrentMood;
    
    UPROPERTY()
    TArray<FGameplayTag> ActiveEffectTags;
    
    UPROPERTY()
    TArray<float> EffectRemainingDurations;
    
    UPROPERTY()
    TArray<FGameplayTag> ActiveModifierTags;
};

// Save
FNeedsSaveData UCharacterNeedsComponent::SaveToData() const
{
    FNeedsSaveData Data;
    
    // Save need values
    for (const auto& Pair : Needs)
    {
        Data.NeedValues.Add(Pair.Key, Pair.Value.State.CurrentValue);
    }
    
    // Save mood
    Data.CurrentMood = MoodState.MoodValue;
    
    // Save active effects
    for (const FActiveNeedEffect& Effect : ActiveEffects)
    {
        if (Effect.EffectAsset)
        {
  Data.ActiveEffectTags.Add(Effect.EffectAsset->EffectTag);
    Data.EffectRemainingDurations.Add(Effect.RemainingDuration);
        }
    }
    
    // Save active modifiers
    for (const FActiveNeedModifier& Modifier : ActiveModifiers)
    {
        if (Modifier.ModifierAsset)
   {
   Data.ActiveModifierTags.Add(Modifier.ModifierAsset->ModifierTag);
        }
    }
    
    return Data;
}

// Load
void UCharacterNeedsComponent::LoadFromData(const FNeedsSaveData& Data)
{
    // Restore need values
    for (const auto& Pair : Data.NeedValues)
    {
        SetNeedValue(Pair.Key, Pair.Value);
    }
    
    // Restore mood
    MoodState.MoodValue = Data.CurrentMood;
    
    // Restore effects
    for (int32 i = 0; i < Data.ActiveEffectTags.Num(); ++i)
    {
        UNeedEffectAsset* Effect = FindEffectByTag(Data.ActiveEffectTags[i]);
    if (Effect)
     {
 ApplyEffect(Effect, 1);
         
      // Restore remaining duration
       if (i < ActiveEffects.Num())
         {
       ActiveEffects[i].RemainingDuration = Data.EffectRemainingDurations[i];
     }
        }
    }
    
  // Restore modifiers
    for (const FGameplayTag& ModifierTag : Data.ActiveModifierTags)
    {
        UNeedModifierAsset* Modifier = FindModifierByTag(ModifierTag);
     if (Modifier)
        {
    AddModifier(Modifier);
}
    }
}
```

---

## Integration with UI System

### Data Table for Item Display

```cpp
// Data table row
USTRUCT()
struct FNeedItemDisplayData : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY()
    UNeedEffectAsset* Effect;
    
    UPROPERTY()
    FText ItemName;
    
    UPROPERTY()
    UTexture2D* ItemIcon;
    
    UPROPERTY()
    FText EffectDescription;  // "Restores 30 Hunger"
};

// Usage in UI
void UItemTooltipWidget::UpdateDisplay(FName ItemID)
{
    FNeedItemDisplayData* Data = ItemDataTable->FindRow<FNeedItemDisplayData>(ItemID, TEXT(""));
    if (!Data)
    {
        return;
    }
    
  ItemNameText->SetText(Data->ItemName);
    ItemIcon->SetBrushFromTexture(Data->ItemIcon);
  
    // Generate effect description
    FString EffectDesc;
    if (Data->Effect)
    {
        for (const FNeedEffectModification& Mod : Data->Effect->Modifications)
        {
            EffectDesc += FString::Printf(TEXT("%s: +%.0f\n"), 
              *Mod.TargetNeedTag.ToString(), 
                Mod.ValueChange);
        }
    }
    
    EffectDescriptionText->SetText(FText::FromString(EffectDesc));
}
```

---

## Integration with Animation System

### Mood-Based Animations

```cpp
// Animation Blueprint
void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
 Super::NativeUpdateAnimation(DeltaSeconds);
    
    AMyCharacter* Character = Cast<AMyCharacter>(TryGetPawnOwner());
  if (!Character)
  {
        return;
    }
    
    UCharacterNeedsComponent* NeedsComp = Character->FindComponentByClass<UCharacterNeedsComponent>();
    if (!NeedsComp)
    {
return;
    }
    
    // Update mood blend
    float Mood = NeedsComp->GetCurrentMood();
    MoodBlendAlpha = FMath::GetMappedRangeValueClamped(
        FVector2D(-100.0f, 100.0f),  // Input range
        FVector2D(0.0f, 1.0f),       // Output range
        Mood
    );
    
    // Update tired state
    float Energy = NeedsComp->GetNeedValue(FGameplayTag::RequestGameplayTag("Need.Energy"));
    bIsTired = Energy < 30.0f;
}
```

### Idle Animation Variants

```cpp
// Different idle animations based on needs
void UMyAnimInstance::SelectIdleAnimation()
{
  float Hunger = GetNeedValue("Need.Hunger");
    float Energy = GetNeedValue("Need.Energy");
    
    if (Hunger < 20.0f)
    {
        // Clutching stomach animation
        PlayIdleAnimation(IdleHungry);
    }
    else if (Energy < 20.0f)
    {
  // Yawning, slouching animation
        PlayIdleAnimation(IdleTired);
 }
    else
    {
        // Normal idle
        PlayIdleAnimation(IdleNormal);
    }
}
```

---

## Integration with Gameplay Ability System (Future)

### Concept: Gameplay Effects ? Need Effects

```cpp
// Custom Gameplay Effect Execution
class UGameplayEffectExecutionCalc_NeedChange : public UGameplayEffectExecutionCalculation
{
    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override
    {
        AActor* TargetActor = ExecutionParams.GetTargetAbilitySystemComponent()->GetAvatarActor();
        UCharacterNeedsComponent* NeedsComp = TargetActor->FindComponentByClass<UCharacterNeedsComponent>();
        
        if (NeedsComp && NeedEffect)
        {
            NeedsComp->ApplyEffect(NeedEffect, 1);
  }
    }
};
```

---

## Best Practices

### 1. Loose Coupling
? Use events and interfaces
? Direct component references

### 2. Data-Driven Integration
? Configuration through Data Assets
? Hardcoded values in code

### 3. Fallback Behavior
? Graceful degradation if systems missing
? Crashes or errors

### 4. Performance Awareness
? Batch operations
? Per-frame updates

### 5. Clear APIs
? Well-defined interfaces
? Tightly coupled systems

---

## Example: Full Integration Blueprint

```blueprint
// Game Mode Blueprint
Event Begin Play
?? Get Needs Subsystem
?  ?? Initialize with Settings (DA_NeedsSettings)
?? Get Time Subsystem
?  ?? Subscribe to Time Events (automatic)
?? Setup Player Character
   ?? Add Needs Component
   ?? Apply Character Traits (from save data)
   ?? Create Needs UI
   ?? Bind to AI Behavior Tree

// Character Blueprint
Event Begin Play
?? Get Needs Component
?? Bind to Events:
?  ?? On Threshold Changed ? Play Audio/Visual
?  ?? On Mood Changed ? Update Animations
?  ?? On Effect Applied ? Show Notification
?? Register with Quest System

// AI Controller Blueprint
Event Possess
?? Get Needs Component from Pawn
?? Setup Behavior Tree:
?  ?? Decorator: Check Need Critical
?  ?? Task: Satisfy Need
?? Subscribe to Need Events
   ?? On Critical ? Interrupt Current Task
```

---

**System is designed for integration! ??**

Each system maintains loose coupling through events and interfaces, making it easy to integrate with existing game systems without tight dependencies.
