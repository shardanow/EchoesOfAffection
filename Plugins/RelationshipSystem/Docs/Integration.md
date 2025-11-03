# Integration Guide

This guide explains how to integrate the Relationship System with other systems in your project.

## Integration with Dialogue System

The Relationship System is designed to work seamlessly with dialogue systems through events and soft dependencies.

### Affecting Relationships from Dialogue

#### Method 1: Direct Modification (Simple)

In your dialogue choice handling:

```cpp
// C++
void UDialogueChoice::OnChoiceSelected_Implementation()
{
    // Get player's relationship component
    UNPCRelationshipComponent* PlayerRelComp = Player->FindComponentByClass<UNPCRelationshipComponent>();
    
 if (PlayerRelComp)
    {
     // Modify relationship based on choice
        PlayerRelComp->ModifyDimensionValue(DialogueParticipant, 
            FGameplayTag::RequestGameplayTag("Relationship.Dimension.Trust"), 
 10.0f);
    }
}
```

```blueprint
// Blueprint
On Choice Selected:
  Get Relationship Component (from Player)
  -> Modify Dimension Value
     Target: Dialogue Participant
     Dimension Tag: Relationship.Dimension.Trust
     Delta: 10.0
```

#### Method 2: Through Actions (Recommended)

Define relationship actions that correspond to dialogue choices:

1. Create action: `DA_Action_AgreeWithNPC`
2. In dialogue choice, execute action:

```cpp
PlayerRelComp->ExecuteAction(DialogueParticipant, 
    FGameplayTag::RequestGameplayTag("Relationship.Action.AgreeWithNPC"));
```

This approach allows designers to balance relationship effects through Data Assets.

#### Method 3: Event-Based (For Complex Systems)

Use GameEventBus or delegates to decouple systems:

```cpp
// In Dialogue System
OnDialogueChoiceSelected.Broadcast(Player, NPC, ChoiceTag, ChoiceData);

// In custom integration class
void URelationshipDialogueIntegration::OnDialogueChoice(AActor* Player, AActor* NPC, FGameplayTag ChoiceTag, FDialogueChoiceData Data)
{
    // Map dialogue choices to relationship changes
    if (ChoiceTag.MatchesTag("Dialogue.Choice.Agreeable"))
    {
     ModifyRelationship(Player, NPC, "Trust", +5);
        ModifyRelationship(Player, NPC, "Friendship", +3);
    }
  else if (ChoiceTag.MatchesTag("Dialogue.Choice.Confrontational"))
    {
        ModifyRelationship(Player, NPC, "Trust", -10);
        ModifyRelationship(Player, NPC, "Hostility", +5);
    }
}
```

### Gating Dialogue Based on Relationships

#### Check Relationship State

```cpp
bool UDialogueCondition::IsConditionMet_Implementation()
{
    UNPCRelationshipComponent* PlayerRelComp = Player->FindComponentByClass<UNPCRelationshipComponent>();
    
    if (!PlayerRelComp)
    return false;
    
  FGameplayTag CurrentState = PlayerRelComp->GetCurrentState(NPC);
    
    // Only show this dialogue if player is friend or better
    return CurrentState.MatchesAny(FriendStates);
}
```

#### Check Dimension Value

```cpp
bool UDialogueCondition::CheckTrustLevel()
{
    UNPCRelationshipComponent* PlayerRelComp = Player->FindComponentByClass<UNPCRelationshipComponent>();
    
    if (!PlayerRelComp)
        return false;
    
    float TrustValue = PlayerRelComp->GetDimensionValue(NPC, TrustTag);
    
    return TrustValue >= RequiredTrustLevel; // e.g., 50.0
}
```

### Example: Dialogue Memory Effect

If your dialogue system has "Memory Effects", integrate with relationships:

```cpp
// DialogueMemoryEffect.h
UCLASS()
class UDialogueMemoryEffect_ModifyRelationship : public UDialogueMemoryEffect
{
    GENERATED_BODY()
 
public:
    UPROPERTY(EditAnywhere, Category = "Effect")
    FGameplayTag DimensionTag;
    
    UPROPERTY(EditAnywhere, Category = "Effect")
  float DeltaValue;
  
    virtual void ApplyEffect_Implementation(AActor* Subject, AActor* Target) override
    {
        if (UNPCRelationshipComponent* RelComp = Subject->FindComponentByClass<UNPCRelationshipComponent>())
        {
    RelComp->ModifyDimensionValue(Target, DimensionTag, DeltaValue);
}
    }
};
```

## Integration with Quest System

### Quest Rewards Affecting Relationships

```cpp
// In Quest Reward System
void UQuestRewardSystem::GiveReward(AActor* Player, FQuestReward Reward)
{
    // ... other rewards ...
    
    if (Reward.RelationshipChanges.Num() > 0)
    {
        UNPCRelationshipComponent* PlayerRelComp = Player->FindComponentByClass<UNPCRelationshipComponent>();
        
        for (auto& Change : Reward.RelationshipChanges)
        {
            PlayerRelComp->ModifyDimensionValue(
        Change.TargetNPC,
    Change.DimensionTag,
     Change.Value
       );
        }
    }
}
```

### Quest Availability Based on Relationships

```cpp
bool UQuest::IsAvailable(AActor* Player)
{
    // ... other conditions ...
    
    // Check relationship requirements
    for (auto& Requirement : RelationshipRequirements)
    {
        UNPCRelationshipComponent* RelComp = Player->FindComponentByClass<UNPCRelationshipComponent>();
        
        if (!RelComp)
            return false;
     
        float DimValue = RelComp->GetDimensionValue(Requirement.NPC, Requirement.DimensionTag);
        
        if (DimValue < Requirement.MinValue || DimValue > Requirement.MaxValue)
  return false;
    }
    
    return true;
}
```

### Example Quest Data Structure

```cpp
USTRUCT(BlueprintType)
struct FQuestRelationshipRequirement
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere)
    AActor* NPC;
  
    UPROPERTY(EditAnywhere, meta = (Categories = "Relationship.Dimension"))
    FGameplayTag DimensionTag;
    
    UPROPERTY(EditAnywhere)
    float MinValue;
    
    UPROPERTY(EditAnywhere)
    float MaxValue;
};

UCLASS()
class UQuestAsset : public UDataAsset
{
    GENERATED_BODY()
    
public:
    // ... quest data ...
    
    UPROPERTY(EditAnywhere, Category = "Requirements")
    TArray<FQuestRelationshipRequirement> RelationshipRequirements;
    
    UPROPERTY(EditAnywhere, Category = "Rewards")
  TMap<FGameplayTag, float> RelationshipRewards; // Dimension -> Value
};
```

## Integration with Combat System

### Affecting Relationships Through Combat

```cpp
// On combat start
void UCombatComponent::OnCombatStarted(AActor* Attacker, AActor* Defender)
{
    if (UNPCRelationshipComponent* RelComp = Attacker->FindComponentByClass<UNPCRelationshipComponent>())
  {
        // Attacking someone makes you hostile
        RelComp->ModifyDimensionValue(Defender, HostilityTag, +50.0f);
        RelComp->ModifyDimensionValue(Defender, TrustTag, -30.0f);
    }
}

// On ally defense
void UCombatComponent::OnAllyDefended(AActor* Defender, AActor* Ally)
{
    if (UNPCRelationshipComponent* RelComp = Ally->FindComponentByClass<UNPCRelationshipComponent>())
    {
        // Defending ally improves relationship
        RelComp->ModifyDimensionValue(Defender, TrustTag, +20.0f);
        RelComp->ModifyDimensionValue(Defender, FriendshipTag, +15.0f);
    }
}
```

### AI Behavior Based on Relationships

```cpp
// In AI controller
void AAIController::UpdateHostility()
{
    UNPCRelationshipComponent* RelComp = GetPawn()->FindComponentByClass<UNPCRelationshipComponent>();
    
    if (!RelComp)
return;
    
    // Check relationship with player
  FGameplayTag CurrentState = RelComp->GetCurrentState(PlayerCharacter);
    
    if (CurrentState.MatchesTag("Relationship.State.Enemy"))
    {
        // Attack on sight
        SetHostile(true);
    }
    else if (CurrentState.MatchesTag("Relationship.State.Friend"))
    {
      // Don't attack, help if needed
   SetHostile(false);
        ConsiderHelping();
    }
}
```

## Integration with Save System

### Automatic Saving

The Relationship System provides its own save/load helpers, but you can integrate with your project's save system:

```cpp
// In your SaveGame class
UCLASS()
class UMyGameSaveGame : public USaveGame
{
    GENERATED_BODY()
    
public:
    // ... other save data ...
    
    // Embed relationship save data
    UPROPERTY()
    TObjectPtr<URelationshipSaveGame> RelationshipData;
    
    void SaveGame()
    {
        // ... save other data ...
        
      // Save relationships
        RelationshipData = NewObject<URelationshipSaveGame>();
        if (URelationshipSubsystem* Subsystem = GetSubsystem())
{
            RelationshipData->SerializeFromSubsystem(Subsystem);
   }
    }
    
    void LoadGame(UWorld* World)
    {
     // ... load other data ...
     
        // Load relationships
     if (RelationshipData)
 {
         if (URelationshipSubsystem* Subsystem = GetSubsystem())
            {
                RelationshipData->DeserializeToSubsystem(Subsystem, World);
   }
        }
    }
};
```

### Selective Persistence

Only save important relationships:

```cpp
void UMyGameSaveGame::SaveImportantRelationships()
{
    URelationshipSubsystem* Subsystem = GetSubsystem();
    
    // Get all relationships
    TArray<FRelationshipData> AllRelationships = Subsystem->GetRelationshipsAsSubject(Player);
    
    for (const FRelationshipData& RelData : AllRelationships)
    {
   // Only save if relationship has changed significantly
        bool bShouldSave = false;
     
        for (const auto& DimPair : RelData.Dimensions)
        {
    if (FMath::Abs(DimPair.Value.Value) > 10.0f)
   {
  bShouldSave = true;
  break;
   }
        }
        
        if (bShouldSave)
        {
            // Add to save data
        }
    }
}
```

## Custom Event Integration

### Creating Custom Event Handlers

```cpp
UCLASS()
class URelationshipEventHandler : public UObject
{
    GENERATED_BODY()
    
public:
    void Initialize(URelationshipSubsystem* Subsystem)
    {
        // Bind to relationship events
        Subsystem->OnDimensionChanged.AddDynamic(this, &URelationshipEventHandler::OnDimensionChanged);
        Subsystem->OnStateTransitioned.AddDynamic(this, &URelationshipEventHandler::OnStateChanged);
    }
    
    UFUNCTION()
    void OnDimensionChanged(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float NewValue)
    {
      // Custom logic when dimension changes
        UE_LOG(LogTemp, Log, TEXT("%s's %s towards %s changed to %f"),
            *Subject->GetName(),
      *DimensionTag.ToString(),
     *Target->GetName(),
            NewValue);
        
// Trigger achievements, UI updates, etc.
  CheckForAchievements(Subject, DimensionTag, NewValue);
        UpdateUI(Subject, Target);
    }
    
 UFUNCTION()
    void OnStateChanged(AActor* Subject, AActor* Target, FGameplayTag OldState, FGameplayTag NewState)
    {
        // Custom logic when state transitions
        UE_LOG(LogTemp, Log, TEXT("%s's relationship with %s: %s -> %s"),
            *Subject->GetName(),
         *Target->GetName(),
*OldState.ToString(),
     *NewState.ToString());
        
        // Unlock new dialogue, quests, etc.
        UnlockContentForState(Subject, Target, NewState);
    }
};
```

## Soft Integration Pattern

If you want to keep systems completely decoupled, use this pattern:

```cpp
// In a bridge/adapter class
UCLASS()
class URelationshipSystemBridge : public UGameInstanceSubsystem
{
    GENERATED_BODY()
    
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override
    {
        Super::Initialize(Collection);
     
  // Find relationship subsystem
      RelationshipSubsystem = GetGameInstance()->GetSubsystem<URelationshipSubsystem>();
        
        // Find other subsystems
        DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
     QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
        
     // Wire up connections if both systems exist
    if (RelationshipSubsystem && DialogueSubsystem)
        {
            ConnectDialogueSystem();
        }
        
        if (RelationshipSubsystem && QuestSubsystem)
        {
        ConnectQuestSystem();
        }
    }
    
private:
    UPROPERTY()
  TObjectPtr<URelationshipSubsystem> RelationshipSubsystem;
    
    UPROPERTY()
    TObjectPtr<UDialogueSubsystem> DialogueSubsystem;
    
    UPROPERTY()
    TObjectPtr<UQuestSubsystem> QuestSubsystem;
    
    void ConnectDialogueSystem()
    {
        // Bind dialogue events to relationship changes
        DialogueSubsystem->OnChoiceSelected.AddDynamic(this, &URelationshipSystemBridge::OnDialogueChoice);
    }
 
    void ConnectQuestSystem()
    {
// Bind quest events to relationship changes
        QuestSubsystem->OnQuestCompleted.AddDynamic(this, &URelationshipSystemBridge::OnQuestCompleted);
    }
};
```

This keeps each system independent while still allowing integration when both are present.

## Best Practices

1. **Use Tags Consistently**: Define all relationship tags in DefaultGameplayTags.ini
2. **Event-Driven**: Don't poll relationships every frame, use events
3. **Batch Updates**: Modify multiple dimensions at once when possible
4. **Cache Components**: Store RelationshipComponent references instead of searching every time
5. **Soft Dependencies**: Use `FModuleManager::Get().IsModuleLoaded()` to check for optional systems
6. **Designer Empowerment**: Expose relationship effects through Data Assets, not code

## Debugging Integration

Use these console commands to debug integration:

```
RelationshipSystem.Debug 1// Enable debug logging
RelationshipSystem.ShowAllRelationships    // Display all active relationships
RelationshipSystem.DumpRelationship Player NPC_Friendly // Dump specific relationship
```

Create debug UI widgets to visualize relationships during development.
