# Needs System - Examples

## Example 1: Basic Setup

### Step 1: Create Need Definitions

**Hunger** (`DA_Need_Hunger`)
```
Tag: Need.Hunger
Display Name: "Hunger"
Initial Value: 100.0
Decay Rate: 5.0 (points per game hour)
Warning Threshold: 30.0
Critical Threshold: 15.0
Mood Weight: 1.0
```

**Energy** (`DA_Need_Energy`)
```
Tag: Need.Energy
Display Name: "Energy"
Initial Value: 100.0
Decay Rate: 10.0
Warning Threshold: 25.0
Critical Threshold: 10.0
Mood Weight: 1.0
```

### Step 2: Create Settings

**Global Settings** (`DA_NeedsSettings`)
```
Available Needs:
  - DA_Need_Hunger
  - DA_Need_Energy

Enable Mood: True
Update Frequency: 10.0
LOD Config:
  Far Distance: 2000.0
  Parked Distance: 5000.0
```

### Step 3: Add Component

Blueprint:
```blueprint
// In Character BP
Components -> Add -> Character Needs Component
```

C++:
```cpp
// In AMyCharacter constructor
NeedsComponent = CreateDefaultSubobject<UCharacterNeedsComponent>(TEXT("NeedsComponent"));
```

---

## Example 2: Food Effects

### Simple Food (Instant)

**Bread** (`DA_Effect_EatBread`)
```
Tag: Need.Effect.EatBread
Display Name: "Eat Bread"
Effect Type: Instant

Modifications:
  - Target: Need.Hunger
    Value: +30.0
```

### Complex Food (Over Time)

**Stew** (`DA_Effect_EatStew`)
```
Tag: Need.Effect.EatStew
Display Name: "Eat Stew"
Effect Type: Over Time
Duration: 2.0 (hours)

Modifications:
  - Target: Need.Hunger
  Value: +50.0 (over 2 hours)
  - Target: Need.Energy
    Value: +20.0 (over 2 hours)

Granted Tags:
  - State.WellFed
```

### Apply in Code

Blueprint:
```blueprint
// On "Eat" action
Get Needs Component (Self) -> Apply Effect (DA_Effect_EatBread, 1)
```

C++:
```cpp
void AMyCharacter::Eat(UNeedEffectAsset* FoodEffect)
{
    if (NeedsComponent)
    {
        NeedsComponent->ApplyEffect(FoodEffect, 1);
    }
}
```

---

## Example 3: Sleep System

### Sleep Effect

**Sleep** (`DA_Effect_Sleep`)
```
Tag: Need.Effect.Sleep
Effect Type: Over Time
Duration: 8.0 (hours)

Modifications:
  - Target: Need.Energy
    Value: +100.0 (over 8 hours)

Granted Tags:
  - State.Sleeping
```

### Well Rested Modifier

**Well Rested** (`DA_Modifier_WellRested`)
```
Tag: Need.Modifier.WellRested
Priority: 10
Duration: 4 hours after waking

Activation Condition:
  Required Tags: State.Awake
  Blocked Tags: State.Tired

Modifications:
  - Target: Need.Energy
    Modify Decay Rate: true
    Operation: Multiply
    Value: 0.5 (slower decay)
```

### Sleep Manager Blueprint

```blueprint
// Function: StartSleep
BEGIN
  // Apply sleep effect
  Apply Effect (DA_Effect_Sleep, 1)
  
  // Suppress hunger while sleeping
  Suppress Need (Need.Hunger, true)
  
  // Wait for sleep to complete (8 hours game time)
  Delay Until Effect Expired
  
  // Add well-rested buff
  Add Modifier (DA_Modifier_WellRested)
  
  // Resume hunger
  Suppress Need (Need.Hunger, false)
END
```

---

## Example 4: Environmental Modifiers

### Hot Weather

**Heat Stress** (`DA_Modifier_HeatStress`)
```
Tag: Need.Modifier.HeatStress
Priority: 5

Activation Condition:
  Required Time: 12:00 - 16:00 (noon)
  Use Time Condition: true

Modifications:
  - Target: Need.Energy
    Modify Decay Rate: true
    Operation: Multiply
    Value: 1.5 (faster decay)
    
  - Target: Need.Comfort
    Modify Decay Rate: true
    Operation: Multiply
    Value: 2.0 (much faster decay)
```

### Apply Based on Location

```cpp
void AEnvironmentManager::UpdateEnvironmentalModifiers()
{
    for (UCharacterNeedsComponent* Comp : NearbyCharacters)
    {
        FVector Location = Comp->GetOwner()->GetActorLocation();
     
   // Check if in hot zone
 if (IsInHotZone(Location))
   {
      Comp->AddModifier(HeatStressModifier);
        }
    else
  {
          // Remove by instance ID (store when added)
Comp->RemoveModifier(HeatStressInstanceID);
        }
    }
}
```

---

## Example 5: Mood-Based Actions

### Check Mood Before Action

Blueprint:
```blueprint
// Function: CanPerformSocialAction
BEGIN
  Get Current Mood -> Mood Value
  
  IF Mood Value < -20.0 THEN
    Display Message ("Too grumpy to socialize")
    RETURN False
  ENDIF
  
  RETURN True
END
```

C++:
```cpp
bool AMyCharacter::CanPerformSocialAction() const
{
    if (!NeedsComponent)
    {
        return false;
    }
    
    const float Mood = NeedsComponent->GetCurrentMood();
    return Mood >= -20.0f;
}
```

### Mood-Based Animations

```cpp
void AMyCharacter::UpdateMoodAnimation()
{
    if (!NeedsComponent || !AnimInstance)
    {
 return;
    }
    
    const float Mood = NeedsComponent->GetCurrentMood();
    
    if (Mood >= 50.0f)
    {
AnimInstance->SetMoodState(EMoodState::Happy);
    }
    else if (Mood >= 0.0f)
    {
      AnimInstance->SetMoodState(EMoodState::Neutral);
    }
    else if (Mood >= -50.0f)
    {
        AnimInstance->SetMoodState(EMoodState::Sad);
    }
    else
    {
        AnimInstance->SetMoodState(EMoodState::Depressed);
    }
}
```

---

## Example 6: Event Handling

### React to Critical Hunger

Blueprint:
```blueprint
// Event: On Need Threshold Changed
IF NeedTag == Need.Hunger AND NewLevel == Critical THEN
  Display Warning ("You're starving!")
  Apply Visual Effect (HungerVFX)
  Start Timer (DamageFromHunger, 1.0, Looping)
ENDIF

IF NeedTag == Need.Hunger AND NewLevel == Recovered THEN
  Stop Timer (DamageFromHunger)
  Remove Visual Effect (HungerVFX)
ENDIF
```

C++:
```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (NeedsComponent)
    {
      NeedsComponent->OnNeedThresholdChanged.AddDynamic(
            this, &AMyCharacter::HandleThresholdChanged);
    }
}

void AMyCharacter::HandleThresholdChanged(
    FGameplayTag NeedTag,
    ENeedThresholdLevel OldLevel,
    ENeedThresholdLevel NewLevel)
{
    if (NeedTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("Need.Hunger")))
    {
        if (NewLevel == ENeedThresholdLevel::Critical)
   {
      StartStarvationDamage();
      }
        else if (NewLevel == ENeedThresholdLevel::Recovered)
        {
            StopStarvationDamage();
      }
    }
}
```

---

## Example 7: Custom UI

### Create Custom Need Widget

Blueprint Widget (`WBP_CustomNeed`):
```
Hierarchy:
  - Overlay
    - ProgressBar (ValueBar)
    - HorizontalBox
      - Image (Icon)
- TextBlock (Name)
      - TextBlock (Value)
    - Image (WarningIndicator)

Parent Class: NeedDisplayWidget

Event Graph:
  - Event On Value Changed
    - Play Animation (ValueChanged)
    
  - Event On Threshold Changed
    - IF NewThreshold == Critical
      - Play Animation (CriticalPulse, Looping)
    - ELSE
      - Stop Animation (CriticalPulse)
```

### Create Needs Panel

Blueprint Widget (`WBP_NeedsPanel`):
```
Hierarchy:
  - VerticalBox (NeedsContainer)
  - TextBlock (MoodTextBlock)

Parent Class: NeedsPanelWidget

Settings:
  - Need Widget Class: WBP_CustomNeed
  - Auto Update: True
  - Show Mood: True

Event Graph:
  - Event Construct
    - Get Player Character
    - Get Needs Component
    - Set Target Component
```

---

## Example 8: Advanced Modifiers

### Trait System

**Athlete Trait** (`DA_Modifier_Athlete`)
```
Tag: Need.Modifier.Athlete
Priority: 100 (high priority, permanent)

Modifications:
  - Target: Need.Energy
 Modify Decay Rate: true
    Operation: Multiply
    Value: 0.7 (30% slower decay)
    
  - Target: Need.Energy
 Cap Max Value: true
    Max Value: 120.0 (can exceed 100)
```

**Glutton Trait** (`DA_Modifier_Glutton`)
```
Tag: Need.Modifier.Glutton
Priority: 100

Modifications:
  - Target: Need.Hunger
    Modify Effect Power: true
    Operation: Multiply
    Value: 1.3 (food 30% more effective)

  - Target: Need.Hunger
    Modify Decay Rate: true
 Operation: Multiply
    Value: 1.2 (20% faster decay)
```

### Apply Traits on Character Creation

```cpp
void AMyCharacter::ApplyTraits(const TArray<UNeedModifierAsset*>& Traits)
{
    if (!NeedsComponent)
    {
        return;
    }
    
    for (UNeedModifierAsset* Trait : Traits)
    {
        NeedsComponent->AddModifier(Trait);
    }
}
```

---

## Example 9: Save/Load System

```cpp
USTRUCT()
struct FNeedsSaveData
{
    GENERATED_BODY()
    
    UPROPERTY()
    TMap<FGameplayTag, float> NeedValues;
    
    UPROPERTY()
    TArray<FActiveNeedEffect> ActiveEffects;
    
    UPROPERTY()
    TArray<FActiveNeedModifier> ActiveModifiers;
    
    UPROPERTY()
    float CurrentMood;
};

// Save
FNeedsSaveData UMyCharacter::SaveNeedsData() const
{
    FNeedsSaveData Data;
    
    if (NeedsComponent)
    {
      for (const FGameplayTag& Tag : NeedsComponent->GetAllNeedTags())
    {
        Data.NeedValues.Add(Tag, NeedsComponent->GetNeedValue(Tag));
        }
     
      Data.CurrentMood = NeedsComponent->GetCurrentMood();
 // Store active effects/modifiers...
    }
    
    return Data;
}

// Load
void UMyCharacter::LoadNeedsData(const FNeedsSaveData& Data)
{
    if (!NeedsComponent)
    {
        return;
    }
    
    for (const auto& Pair : Data.NeedValues)
    {
        NeedsComponent->SetNeedValue(Pair.Key, Pair.Value);
    }
    
    // Restore active effects/modifiers...
}
```

---

## Example 10: Debug Visualization

### Enable Debug in Runtime

Blueprint:
```blueprint
// Console Command: "needs.debug 1"
Get Needs Subsystem -> Set Debug Visualization (True)
```

C++:
```cpp
// Console command
static FAutoConsoleCommand CVarNeedsDebug(
    TEXT("needs.debug"),
    TEXT("Toggle needs debug visualization"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
    {
        if (UWorld* World = GEngine->GetWorldFromContextObject(GEngine->GameViewport))
    {
if (UNeedsSubsystem* Subsystem = World->GetSubsystem<UNeedsSubsystem>())
  {
  const bool bEnable = Args.Num() > 0 ? FCString::Atoi(*Args[0]) != 0 : true;
          Subsystem->SetDebugVisualization(bEnable);
    }
        }
    })
);
```

### Custom Debug Display

```cpp
void AMyCharacter::DisplayNeedsDebug()
{
    if (!NeedsComponent)
    {
        return;
  }
    
    for (const FGameplayTag& Tag : NeedsComponent->GetAllNeedTags())
    {
        const float Value = NeedsComponent->GetNeedValue(Tag);
  const ENeedThresholdLevel Threshold = NeedsComponent->GetNeedThreshold(Tag);
        
FString DebugText = FString::Printf(
    TEXT("%s: %.1f [%s]"),
            *Tag.ToString(),
         Value,
    *StaticEnum<ENeedThresholdLevel>()->GetValueAsString(Threshold)
        );
        
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugText);
    }
    
    const float Mood = NeedsComponent->GetCurrentMood();
    FString MoodText = FString::Printf(TEXT("Mood: %.1f"), Mood);
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, MoodText);
}
```

---

## Best Practices Summary

1. **Use Data Assets** - Keep all configuration in data assets
2. **Event-Driven** - React to threshold changes, not polling
3. **Batch Effects** - Apply multiple effects at once when possible
4. **LOD Awareness** - Consider LOD when designing systems
5. **Handle-Based** - Use handles for external references
6. **Time Integration** - Leverage Time System for realistic simulation
7. **Modifiers for Traits** - Use permanent modifiers for character traits
8. **Effects for Actions** - Use effects for temporary actions (eating, sleeping)
9. **UI Listeners** - Use listener interface for reactive UI
10. **Debug Tools** - Implement debug visualization for development
