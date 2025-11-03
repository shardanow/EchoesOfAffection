// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystems/RelationshipSubsystem.h"
#include "DataAssets/RelationshipDatabase.h"
#include "DataAssets/RelationshipDimension.h"
#include "DataAssets/RelationshipState.h"
#include "DataAssets/RelationshipTrait.h"
#include "DataAssets/RelationshipAction.h"
#include "DataAssets/RelationshipProfile.h"
#include "DataAssets/RelationshipRule.h"
#include "Components/NPCRelationshipComponent.h"
#include "Settings/RelationshipSystemSettings.h"
#include "RelationshipSystemCore.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Conditional include - only if GameEventBus is available
#if __has_include("GameEventBusSubsystem.h")
	#include "GameEventBusSubsystem.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

// FRelationshipData implementation

float FRelationshipData::GetDimensionValue(FGameplayTag DimensionTag) const
{
	if (const FRelationshipDimensionValue* DimValue = Dimensions.Find(DimensionTag))
	{
		return DimValue->Value;
	}
	return 0.0f;
}

const FRelationshipTraitInstance* FRelationshipData::FindTrait(FGameplayTag TraitTag) const
{
	return Traits.FindByPredicate([TraitTag](const FRelationshipTraitInstance& Instance)
	{
		return Instance.TraitTag == TraitTag;
	});
}

bool FRelationshipData::HasTrait(FGameplayTag TraitTag) const
{
	return FindTrait(TraitTag) != nullptr;
}

// URelationshipSubsystem implementation

URelationshipSubsystem::URelationshipSubsystem()
{
}

void URelationshipSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogRelationshipSystem, Log, TEXT("RelationshipSubsystem initialized"));

	// Load and build database
	if (URelationshipDatabase* Database = GetDatabase())
	{
		Database->BuildIndices();
	}
	else
	{
		UE_LOG(LogRelationshipSystem, Warning, TEXT("No Relationship Database configured in Project Settings"));
	}

	// Subscribe to time events
	SubscribeToTimeEvents();
}

void URelationshipSubsystem::Deinitialize()
{
	// Unsubscribe from time events
	UnsubscribeFromTimeEvents();

	ClearAllRelationships();
	RegisteredComponents.Empty();

	UE_LOG(LogRelationshipSystem, Log, TEXT("RelationshipSubsystem deinitialized"));

	Super::Deinitialize();
}

// Component Registration

void URelationshipSubsystem::RegisterComponent(UNPCRelationshipComponent* Component)
{
	if (!Component)
	{
		return;
	}

	RegisteredComponents.AddUnique(Component);
	UE_LOG(LogRelationshipSystem, Verbose, TEXT("Registered component: %s"), *Component->GetOwner()->GetName());
}

void URelationshipSubsystem::UnregisterComponent(UNPCRelationshipComponent* Component)
{
	if (!Component)
	{
		return;
	}

	RegisteredComponents.Remove(Component);
	UE_LOG(LogRelationshipSystem, Verbose, TEXT("Unregistered component: %s"), *Component->GetOwner()->GetName());
}

// Relationship Management

bool URelationshipSubsystem::CreateRelationship(AActor* Subject, AActor* Target, URelationshipProfile* SubjectProfile, URelationshipProfile* TargetProfile)
{
	if (!Subject || !Target || Subject == Target)
	{
		UE_LOG(LogRelationshipSystem, Warning, TEXT("Invalid actors for relationship creation"));
		return false;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("No database available"));
		return false;
	}

	// Check if already exists
	if (HasRelationship(Subject, Target))
	{
		UE_LOG(LogRelationshipSystem, Verbose, TEXT("Relationship already exists: %s -> %s"), *Subject->GetName(), *Target->GetName());
		return true;
	}

	// Create new relationship
	FRelationshipKey Key(Subject, Target);
	FRelationshipData& NewRelData = Relationships.Add(Key);
	NewRelData.Subject = Subject;
	NewRelData.Target = Target;
	NewRelData.LastUpdateTime = GetCurrentGameTime();

	// Initialize dimensions from profile
	UE_LOG(LogRelationshipSystem, Log, TEXT("Creating relationship: %s -> %s with Profile: %s"),
		*Subject->GetName(), *Target->GetName(), SubjectProfile ? *SubjectProfile->GetName() : TEXT("DefaultProfile"));

	if (SubjectProfile)
	{
		// Log InitialDimensionValues from profile
		UE_LOG(LogRelationshipSystem, Log, TEXT("  Profile has %d InitialDimensionValues:"), SubjectProfile->InitialDimensionValues.Num());
		for (const auto& Pair : SubjectProfile->InitialDimensionValues)
		{
			UE_LOG(LogRelationshipSystem, Log, TEXT("    %s = %.2f"), *Pair.Key.ToString(), Pair.Value);
		}

		NewRelData.Dimensions = SubjectProfile->CreateInitialDimensions(Database, NewRelData.LastUpdateTime);
		
		// Log created dimensions
		UE_LOG(LogRelationshipSystem, Log, TEXT("  Created %d dimension entries:"), NewRelData.Dimensions.Num());
		for (const auto& Pair : NewRelData.Dimensions)
		{
			UE_LOG(LogRelationshipSystem, Log, TEXT("    %s = %.2f (LastModified: %.2f)"), 
				*Pair.Key.ToString(), Pair.Value.Value, Pair.Value.LastModifiedTime);
		}
	}
	else
	{
		// Use default profile
		if (URelationshipProfile* DefaultProfile = Database->DefaultProfile.LoadSynchronous())
		{
			UE_LOG(LogRelationshipSystem, Log, TEXT("  Using DefaultProfile: %s"), *DefaultProfile->GetName());
			NewRelData.Dimensions = DefaultProfile->CreateInitialDimensions(Database, NewRelData.LastUpdateTime);
		}
		else
		{
			UE_LOG(LogRelationshipSystem, Warning, TEXT("  No DefaultProfile found in Database!"));
		}
	}

	// Initialize state
	FGameplayTag InitialState = SubjectProfile ? SubjectProfile->InitialState : FGameplayTag();
	if (!InitialState.IsValid() && Database->DefaultProfile.IsValid())
	{
		if (URelationshipProfile* DefaultProfile = Database->DefaultProfile.LoadSynchronous())
		{
			InitialState = DefaultProfile->InitialState;
		}
	}

	if (InitialState.IsValid())
	{
		NewRelData.State.SetState(InitialState, NewRelData.LastUpdateTime);
		UE_LOG(LogRelationshipSystem, Log, TEXT("  Set InitialState: %s"), *InitialState.ToString());
	}
	else
	{
		// Find first default state
		TArray<URelationshipState*> AllStates = Database->GetAllStates();
		for (URelationshipState* State : AllStates)
		{
			if (State && State->bIsDefaultState)
			{
				NewRelData.State.SetState(State->GetAssetTag(), NewRelData.LastUpdateTime);
				UE_LOG(LogRelationshipSystem, Log, TEXT("  Set default state: %s"), *State->GetAssetTag().ToString());
				break;
			}
		}
	}

	// Initialize traits
	if (SubjectProfile)
	{
		UE_LOG(LogRelationshipSystem, Log, TEXT("  Adding %d InitialTraits:"), SubjectProfile->InitialTraits.Num());
		for (const FGameplayTag& TraitTag : SubjectProfile->InitialTraits)
		{
			FRelationshipTraitInstance TraitInstance(TraitTag, NewRelData.LastUpdateTime);
			NewRelData.Traits.Add(TraitInstance);
			UE_LOG(LogRelationshipSystem, Log, TEXT("    Added trait: %s"), *TraitTag.ToString());
		}
	}

	UE_LOG(LogRelationshipSystem, Log, TEXT("[OK] Successfully created relationship: %s -> %s (State: %s, %d dimensions, %d traits)"),
		*Subject->GetName(), *Target->GetName(), *NewRelData.State.CurrentState.ToString(), 
		NewRelData.Dimensions.Num(), NewRelData.Traits.Num());

	return true;
}

bool URelationshipSubsystem::RemoveRelationship(AActor* Subject, AActor* Target)
{
	FRelationshipKey Key(Subject, Target);
	return Relationships.Remove(Key) > 0;
}

bool URelationshipSubsystem::HasRelationship(AActor* Subject, AActor* Target) const
{
	FRelationshipKey Key(Subject, Target);
	return Relationships.Contains(Key);
}

bool URelationshipSubsystem::GetRelationshipData(AActor* Subject, AActor* Target, FRelationshipData& OutData) const
{
	if (const FRelationshipData* RelData = GetRelationship(Subject, Target))
	{
		OutData = *RelData;
		return true;
	}
	return false;
}

// Dimension Operations

float URelationshipSubsystem::GetDimensionValue(AActor* Subject, AActor* Target, FGameplayTag DimensionTag) const
{
	if (const FRelationshipData* RelData = GetRelationship(Subject, Target))
	{
		return RelData->GetDimensionValue(DimensionTag);
	}
	return 0.0f;
}

bool URelationshipSubsystem::SetDimensionValue(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float NewValue)
{
	FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
	if (!RelData)
	{
		return false;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return false;
	}

	// Get dimension for clamping
	URelationshipDimension* Dimension = Database->FindDimension(DimensionTag);
	if (!Dimension)
	{
		UE_LOG(LogRelationshipSystem, Warning, TEXT("Dimension not found: %s"), *DimensionTag.ToString());
		return false;
	}

	FRelationshipDimensionValue* DimValue = RelData->Dimensions.Find(DimensionTag);
	if (!DimValue)
	{
		// Create new dimension entry
		FRelationshipDimensionValue NewDimValue(DimensionTag, Dimension->ClampValue(NewValue));
		NewDimValue.LastModifiedTime = GetCurrentGameTime();
		RelData->Dimensions.Add(DimensionTag, NewDimValue);
		
		BroadcastDimensionChanged(Subject, Target, DimensionTag, NewValue);
		EvaluateStateTransitions(Subject, Target);
		return true;
	}

	float OldValue = DimValue->Value;
	DimValue->Value = Dimension->ClampValue(NewValue);
	DimValue->LastModifiedTime = GetCurrentGameTime();

	if (!FMath::IsNearlyEqual(OldValue, DimValue->Value))
	{
		BroadcastDimensionChanged(Subject, Target, DimensionTag, DimValue->Value);
		EvaluateStateTransitions(Subject, Target);
	}

	return true;
}

bool URelationshipSubsystem::ModifyDimensionValue(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float Delta)
{
	FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
	if (!RelData)
	{
		return false;
	}

	float ModifiedDelta = ApplyDimensionChangeWithModifiers(RelData, DimensionTag, Delta);
	float CurrentValue = RelData->GetDimensionValue(DimensionTag);
	
	return SetDimensionValue(Subject, Target, DimensionTag, CurrentValue + ModifiedDelta);
}

bool URelationshipSubsystem::ModifyDimensionValues(AActor* Subject, AActor* Target, const TMap<FGameplayTag, float>& DimensionDeltas)
{
	bool bSuccess = true;
	for (const auto& Pair : DimensionDeltas)
	{
		if (!ModifyDimensionValue(Subject, Target, Pair.Key, Pair.Value))
		{
			bSuccess = false;
		}
	}
	return bSuccess;
}

// State Operations

FGameplayTag URelationshipSubsystem::GetCurrentState(AActor* Subject, AActor* Target) const
{
	if (const FRelationshipData* RelData = GetRelationship(Subject, Target))
	{
		return RelData->State.CurrentState;
	}
	return FGameplayTag();
}

bool URelationshipSubsystem::ForceStateChange(AActor* Subject, AActor* Target, FGameplayTag NewState)
{
	FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
	if (!RelData)
	{
		return false;
	}

	FGameplayTag OldState = RelData->State.CurrentState;
	RelData->State.SetState(NewState, GetCurrentGameTime());

	if (OldState != NewState)
	{
		BroadcastStateTransitioned(Subject, Target, OldState, NewState);
	}

	return true;
}

void URelationshipSubsystem::EvaluateStateTransitions(AActor* Subject, AActor* Target)
{
	FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
	if (!RelData)
	{
		return;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return;
	}

	URelationshipState* CurrentState = Database->FindState(RelData->State.CurrentState);
	if (!CurrentState)
	{
		return;
	}

	// Build dimension values for rule evaluation
	TMap<FGameplayTag, float> DimensionValues;
	for (const auto& Pair : RelData->Dimensions)
	{
		DimensionValues.Add(Pair.Key, Pair.Value.Value);
	}

	// Evaluate transitions (sorted by priority)
	TArray<FRelationshipStateTransition> SortedTransitions = CurrentState->GetSortedTransitions();
	
	for (const FRelationshipStateTransition& Transition : SortedTransitions)
	{
		bool bCanTransition = true;

		// Check required rules
		for (const FGameplayTag& RuleTag : Transition.RequiredRules)
		{
			URelationshipRule* Rule = Database->FindRule(RuleTag);
			if (!Rule || !Rule->Evaluate(DimensionValues))
			{
				bCanTransition = false;
				break;
			}
		}

		if (bCanTransition)
		{
			// Check hysteresis
			if (Transition.HysteresisDelay > 0.0f)
			{
				// Update pending transition timer
				RelData->State.UpdatePendingTransition(Transition.TargetState, 0.1f, Transition.HysteresisDelay);
				
				if (RelData->State.IsTransitionReady(Transition.TargetState, Transition.HysteresisDelay))
				{
					ForceStateChange(Subject, Target, Transition.TargetState);
					return;
				}
			}
			else
			{
				// Instant transition
				ForceStateChange(Subject, Target, Transition.TargetState);
				return;
			}
		}
		else
		{
			// Clear pending transition if conditions no longer met
			RelData->State.ClearPendingTransition(Transition.TargetState);
		}
	}
}

// Trait Operations

bool URelationshipSubsystem::AddTrait(AActor* Subject, AActor* Target, FGameplayTag TraitTag)
{
	FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
	if (!RelData)
	{
		return false;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return false;
	}

	URelationshipTrait* Trait = Database->FindTrait(TraitTag);
	if (!Trait)
	{
		UE_LOG(LogRelationshipSystem, Warning, TEXT("Trait not found: %s"), *TraitTag.ToString());
		return false;
	}

	// Check conflicts
	for (const FGameplayTag& ConflictTag : Trait->ConflictingTraits)
	{
		if (RelData->HasTrait(ConflictTag))
		{
			UE_LOG(LogRelationshipSystem, Warning, TEXT("Trait conflicts with existing trait: %s"), *ConflictTag.ToString());
			return false;
		}
	}

	// Check requirements
	for (const FGameplayTag& RequiredTag : Trait->RequiredTraits)
	{
		if (!RelData->HasTrait(RequiredTag))
		{
			UE_LOG(LogRelationshipSystem, Warning, TEXT("Missing required trait: %s"), *RequiredTag.ToString());
			return false;
		}
	}

	// Check if already has trait
	FRelationshipTraitInstance* ExistingTrait = RelData->Traits.FindByPredicate([TraitTag](const FRelationshipTraitInstance& Instance)
	{
		return Instance.TraitTag == TraitTag;
	});

	if (ExistingTrait)
	{
		// Try to stack
		if (Trait->bCanStack)
		{
			ExistingTrait->AddStack(Trait);
			return true;
		}
		return false; // Already has non-stackable trait
	}

	// Add new trait
	FRelationshipTraitInstance NewTrait(TraitTag, GetCurrentGameTime());
	RelData->Traits.Add(NewTrait);

	BroadcastTraitAdded(Subject, Target, TraitTag);
	return true;
}

bool URelationshipSubsystem::RemoveTrait(AActor* Subject, AActor* Target, FGameplayTag TraitTag)
{
	FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
	if (!RelData)
	{
		return false;
	}

	int32 RemovedCount = RelData->Traits.RemoveAll([TraitTag](const FRelationshipTraitInstance& Instance)
	{
		return Instance.TraitTag == TraitTag;
	});

	if (RemovedCount > 0)
	{
		BroadcastTraitRemoved(Subject, Target, TraitTag);
		return true;
	}

	return false;
}

bool URelationshipSubsystem::HasTrait(AActor* Subject, AActor* Target, FGameplayTag TraitTag) const
{
	if (const FRelationshipData* RelData = GetRelationship(Subject, Target))
	{
		return RelData->HasTrait(TraitTag);
	}
	return false;
}

int32 URelationshipSubsystem::GetTraitStackCount(AActor* Subject, AActor* Target, FGameplayTag TraitTag) const
{
	if (const FRelationshipData* RelData = GetRelationship(Subject, Target))
	{
		if (const FRelationshipTraitInstance* Trait = RelData->FindTrait(TraitTag))
		{
			return Trait->StackCount;
		}
	}
	return 0;
}

// Action Execution

bool URelationshipSubsystem::CanExecuteAction(AActor* Subject, AActor* Target, FGameplayTag ActionTag) const
{
	const FRelationshipData* RelData = GetRelationship(Subject, Target);
	if (!RelData)
	{
		return false;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return false;
	}

	URelationshipAction* Action = Database->FindAction(ActionTag);
	if (!Action)
	{
		return false;
	}

	// Build dimension values
	TMap<FGameplayTag, float> DimensionValues;
	for (const auto& Pair : RelData->Dimensions)
	{
		DimensionValues.Add(Pair.Key, Pair.Value.Value);
	}

	// Check costs and rules
	if (!Action->CanExecute(DimensionValues, Database))
	{
		return false;
	}

	// Check cooldown and max uses
	if (const FRelationshipActionInstance* ActionInst = RelData->Actions.Find(ActionTag))
	{
		if (ActionInst->IsOnCooldown(GetCurrentGameTime(), Action->Cooldown))
		{
			return false;
		}
		if (ActionInst->IsMaxUsesReached(Action->MaxUses))
		{
			return false;
		}
	}

	return true;
}

bool URelationshipSubsystem::ExecuteAction(AActor* Subject, AActor* Target, FGameplayTag ActionTag)
{
	if (!CanExecuteAction(Subject, Target, ActionTag))
	{
		return false;
	}

	FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
	if (!RelData)
	{
		return false;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return false;
	}

	URelationshipAction* Action = Database->FindAction(ActionTag);
	if (!Action)
	{
		return false;
	}

	// Apply costs
	for (const FRelationshipActionCost& Cost : Action->Costs)
	{
		if (Cost.ConsumedValue > 0.0f)
		{
			ModifyDimensionValue(Subject, Target, Cost.DimensionTag, -Cost.ConsumedValue);
		}
	}

	// Apply effects
	for (const FRelationshipActionEffect& Effect : Action->Effects)
	{
		if (Effect.RollSuccess())
		{
			float FinalValue = Effect.CalculateFinalValue();
			ModifyDimensionValue(Subject, Target, Effect.DimensionTag, FinalValue);
		}
	}

	// Record execution
	FRelationshipActionInstance& ActionInst = RelData->Actions.FindOrAdd(ActionTag, FRelationshipActionInstance(ActionTag));
	ActionInst.RecordExecution(GetCurrentGameTime());

	// Trigger events
	for (const FGameplayTag& EventTag : Action->TriggeredEvents)
	{
		// Process trait event responses
		for (const FRelationshipTraitInstance& TraitInst : RelData->Traits)
		{
			URelationshipTrait* Trait = Database->FindTrait(TraitInst.TraitTag);
			if (Trait)
			{
				FTraitEventResponse Response = Trait->GetEventResponse(EventTag);
				if (Response.EventTag.IsValid() && !TraitInst.IsEventOnCooldown(EventTag, GetCurrentGameTime()))
				{
					if (FMath::FRand() <= Response.ResponseProbability)
					{
						ModifyDimensionValues(Subject, Target, Response.DimensionChanges);
						const_cast<FRelationshipTraitInstance&>(TraitInst).SetEventCooldown(EventTag, Response.Cooldown, GetCurrentGameTime());
					}
				}
			}
		}

		// Broadcast to external systems
		SendGameEvent(EventTag, Subject, Target, TMap<FName, FString>());
	}

	BroadcastActionExecuted(Subject, Target, ActionTag);

	UE_LOG(LogRelationshipSystem, Verbose, TEXT("Executed action %s: %s -> %s"),
		*ActionTag.ToString(), *Subject->GetName(), *Target->GetName());

	return true;
}

TArray<FGameplayTag> URelationshipSubsystem::GetAvailableActions(AActor* Subject, AActor* Target) const
{
	TArray<FGameplayTag> AvailableActions;

	const FRelationshipData* RelData = GetRelationship(Subject, Target);
	if (!RelData)
	{
		return AvailableActions;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return AvailableActions;
	}

	// Get current state actions
	if (URelationshipState* CurrentState = Database->FindState(RelData->State.CurrentState))
	{
		for (const FGameplayTag& ActionTag : CurrentState->AvailableActions)
		{
			if (CanExecuteAction(Subject, Target, ActionTag))
			{
				AvailableActions.Add(ActionTag);
			}
		}
	}

	return AvailableActions;
}

// Queries

TArray<FRelationshipData> URelationshipSubsystem::GetRelationshipsAsSubject(AActor* Subject) const
{
	TArray<FRelationshipData> Result;
	
	for (const auto& Pair : Relationships)
	{
		if (Pair.Value.Subject.Get() == Subject)
		{
			Result.Add(Pair.Value);
		}
	}

	return Result;
}

TArray<FRelationshipData> URelationshipSubsystem::GetRelationshipsAsTarget(AActor* Target) const
{
	TArray<FRelationshipData> Result;
	
	for (const auto& Pair : Relationships)
	{
		if (Pair.Value.Target.Get() == Target)
		{
			Result.Add(Pair.Value);
		}
	}

	return Result;
}

TArray<FRelationshipData> URelationshipSubsystem::FindRelationshipsByState(AActor* Subject, FGameplayTag StateTag) const
{
	TArray<FRelationshipData> Result;
	
	for (const auto& Pair : Relationships)
	{
		if (Pair.Value.Subject.Get() == Subject && Pair.Value.State.CurrentState == StateTag)
		{
			Result.Add(Pair.Value);
		}
	}

	return Result;
}

TArray<FRelationshipData> URelationshipSubsystem::FindRelationshipsByDimensionRange(AActor* Subject, FGameplayTag DimensionTag, float MinValue, float MaxValue) const
{
	TArray<FRelationshipData> Result;
	
	for (const auto& Pair : Relationships)
	{
		if (Pair.Value.Subject.Get() == Subject)
		{
			float DimValue = Pair.Value.GetDimensionValue(DimensionTag);
			if (DimValue >= MinValue && DimValue <= MaxValue)
			{
				Result.Add(Pair.Value);
			}
		}
	}

	return Result;
}

// Decay System

void URelationshipSubsystem::ProcessDecay(float DeltaHours)
{
	if (DeltaHours <= 0.0f)
	{
		return;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return;
	}

	float CurrentTime = GetCurrentGameTime();

	for (auto& Pair : Relationships)
	{
		FRelationshipData& RelData = Pair.Value;
		
		if (!RelData.IsValid())
		{
			continue;
		}

		// Process dimension decay
		for (auto& DimPair : RelData.Dimensions)
		{
			URelationshipDimension* Dimension = Database->FindDimension(DimPair.Key);
			if (Dimension && Dimension->bEnableDecay)
			{
				DimPair.Value.ApplyDecay(Dimension, DeltaHours, CurrentTime);
			}
		}

		RelData.LastUpdateTime = CurrentTime;
	}

	// Clean up invalid relationships
	for (auto It = Relationships.CreateIterator(); It; ++It)
	{
		if (!It.Value().IsValid())
		{
			UE_LOG(LogRelationshipSystem, Verbose, TEXT("Removing invalid relationship"));
			It.RemoveCurrent();
		}
	}
}

void URelationshipSubsystem::ProcessRelationshipDecay(AActor* Subject, AActor* Target, float DeltaHours)
{
	FRelationshipData* RelData = GetRelationshipMutable(Subject, Target);
	if (!RelData || DeltaHours <= 0.0f)
	{
		return;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return;
	}

	float CurrentTime = GetCurrentGameTime();

	for (auto& DimPair : RelData->Dimensions)
	{
		URelationshipDimension* Dimension = Database->FindDimension(DimPair.Key);
		if (Dimension && Dimension->bEnableDecay)
		{
			DimPair.Value.ApplyDecay(Dimension, DeltaHours, CurrentTime);
		}
	}

	RelData->LastUpdateTime = CurrentTime;
}

// Database Access

URelationshipDatabase* URelationshipSubsystem::GetDatabase() const
{
	if (DatabaseOverride)
	{
		return DatabaseOverride;
	}

	const URelationshipSystemSettings* Settings = GetDefault<URelationshipSystemSettings>();
	if (Settings && Settings->Database.IsValid())
	{
		return Settings->Database.LoadSynchronous();
	}

	return nullptr;
}

void URelationshipSubsystem::SetDatabaseOverride(URelationshipDatabase* NewDatabase)
{
	DatabaseOverride = NewDatabase;
	
	if (NewDatabase)
	{
		NewDatabase->BuildIndices();
	}
}

// Debug & Utilities

int32 URelationshipSubsystem::GetRelationshipCount() const
{
	return Relationships.Num();
}

void URelationshipSubsystem::ClearAllRelationships()
{
	Relationships.Empty();
	UE_LOG(LogRelationshipSystem, Warning, TEXT("All relationships cleared"));
}

void URelationshipSubsystem::ValidateAllRelationships()
{
	int32 InvalidCount = 0;
	int32 TotalCount = Relationships.Num();

	for (const auto& Pair : Relationships)
	{
		if (!Pair.Value.IsValid())
		{
			InvalidCount++;
			UE_LOG(LogRelationshipSystem, Warning, TEXT("Invalid relationship found (actors destroyed)"));
		}
	}

	UE_LOG(LogRelationshipSystem, Log, TEXT("Validation complete: %d/%d relationships valid"), TotalCount - InvalidCount, TotalCount);
}

// Private Helpers

FRelationshipData* URelationshipSubsystem::GetOrCreateRelationship(AActor* Subject, AActor* Target)
{
	if (!Subject || !Target || Subject == Target)
	{
		return nullptr;
	}

	FRelationshipKey Key(Subject, Target);
	
	if (FRelationshipData* Existing = Relationships.Find(Key))
	{
		return Existing;
	}

	// Auto-create if components are registered
	// This will be handled by RelationshipComponent

	return nullptr;
}

FRelationshipData* URelationshipSubsystem::GetRelationshipMutable(AActor* Subject, AActor* Target)
{
	if (!Subject || !Target)
	{
		return nullptr;
	}

	FRelationshipKey Key(Subject, Target);
	return Relationships.Find(Key);
}

const FRelationshipData* URelationshipSubsystem::GetRelationship(AActor* Subject, AActor* Target) const
{
	if (!Subject || !Target)
	{
		return nullptr;
	}

	FRelationshipKey Key(Subject, Target);
	return Relationships.Find(Key);
}

float URelationshipSubsystem::ApplyDimensionChangeWithModifiers(FRelationshipData* RelData, FGameplayTag DimensionTag, float BaseDelta, FGameplayTag ActionTag)
{
	if (!RelData)
	{
		return BaseDelta;
	}

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return BaseDelta;
	}

	float ModifiedDelta = BaseDelta;

	// Apply trait modifiers
	for (const FRelationshipTraitInstance& TraitInst : RelData->Traits)
	{
		URelationshipTrait* Trait = Database->FindTrait(TraitInst.TraitTag);
		if (Trait)
		{
			ModifiedDelta = Trait->ApplyModifiers(DimensionTag, ActionTag, ModifiedDelta, TraitInst.StackCount);
		}
	}

	// Apply state modifiers
	if (URelationshipState* CurrentState = Database->FindState(RelData->State.CurrentState))
	{
		float StateModifier = CurrentState->GetDimensionModifier(DimensionTag);
		ModifiedDelta *= (1.0f + StateModifier);
	}

	return ModifiedDelta;
}

void URelationshipSubsystem::BroadcastDimensionChanged(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float NewValue)
{
	UE_LOG(LogRelationshipSystem, Log, TEXT("BroadcastDimensionChanged: %s -> %s, Dimension=%s, Value=%.2f"),
		*GetNameSafe(Subject), *GetNameSafe(Target), *DimensionTag.ToString(), NewValue);

	OnDimensionChanged.Broadcast(Subject, Target, DimensionTag, NewValue);
	
	// FIXED: Use correct event tag that UI listens to
	FGameplayTag DimensionChangedEventTag = FGameplayTag::RequestGameplayTag(FName("Relationship.DimensionChanged"), false);
	
	if (!DimensionChangedEventTag.IsValid())
	{
		UE_LOG(LogRelationshipSystem, Warning, TEXT("Event tag 'Relationship.DimensionChanged' not found in GameplayTags!"));
		// Try alternate tag
		DimensionChangedEventTag = FGameplayTag::RequestGameplayTag(FName("Relationship.Event.DimensionChanged"), false);
	}

	if (DimensionChangedEventTag.IsValid())
	{
		// Send to GameEventBus if available
		TMap<FName, FString> Payload;
		Payload.Add("Dimension", DimensionTag.ToString());
		Payload.Add("Value", FString::SanitizeFloat(NewValue));
		SendGameEvent(DimensionChangedEventTag, Subject, Target, Payload);
	}
	else
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("Could not find 'Relationship.DimensionChanged' or 'Relationship.Event.DimensionChanged' GameplayTag!"));
	}
}

void URelationshipSubsystem::BroadcastStateTransitioned(AActor* Subject, AActor* Target, FGameplayTag OldState, FGameplayTag NewState)
{
	OnStateTransitioned.Broadcast(Subject, Target, OldState, NewState);
	
	TMap<FName, FString> Payload;
	Payload.Add("OldState", OldState.ToString());
	Payload.Add("NewState", NewState.ToString());
	SendGameEvent(FGameplayTag::RequestGameplayTag("Relationship.Event.StateTransitioned"), Subject, Target, Payload);
}

void URelationshipSubsystem::BroadcastTraitAdded(AActor* Subject, AActor* Target, FGameplayTag TraitTag)
{
	OnTraitAdded.Broadcast(Subject, Target, TraitTag);
	
	TMap<FName, FString> Payload;
	Payload.Add("Trait", TraitTag.ToString());
	SendGameEvent(FGameplayTag::RequestGameplayTag("Relationship.Event.TraitAdded"), Subject, Target, Payload);
}

void URelationshipSubsystem::BroadcastTraitRemoved(AActor* Subject, AActor* Target, FGameplayTag TraitTag)
{
	OnTraitRemoved.Broadcast(Subject, Target, TraitTag);
	
	TMap<FName, FString> Payload;
	Payload.Add("Trait", TraitTag.ToString());
	SendGameEvent(FGameplayTag::RequestGameplayTag("Relationship.Event.TraitRemoved"), Subject, Target, Payload);
}

void URelationshipSubsystem::BroadcastActionExecuted(AActor* Subject, AActor* Target, FGameplayTag ActionTag)
{
	OnActionExecuted.Broadcast(Subject, Target, ActionTag);
	
	TMap<FName, FString> Payload;
	Payload.Add("Action", ActionTag.ToString());
	SendGameEvent(FGameplayTag::RequestGameplayTag("Relationship.Event.ActionExecuted"), Subject, Target, Payload);
}

float URelationshipSubsystem::GetCurrentGameTime() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetTimeSeconds();
	}
	return 0.0f;
}

// ===========================================================================================
// Time System Integration via GameEventBus
// ===========================================================================================

void URelationshipSubsystem::SubscribeToTimeEvents()
{
#if GAMEEVENTBUS_AVAILABLE
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();
		if (!EventBus)
		{
			UE_LOG(LogRelationshipSystem, Warning, TEXT("RelationshipSubsystem: GameEventBusSubsystem not found, cannot subscribe to time events"));
			return;
		}

		// Subscribe to time events
		FGameplayTag HourChangedTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.HourChanged"), false);
		FGameplayTag DayChangedTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.DayChanged"), false);

		if (HourChangedTag.IsValid())
		{
			TimeEventHourChangedHandle = EventBus->SubscribeToEventNative(
				HourChangedTag,
				FGameEventNativeDelegate::FDelegate::CreateUObject(this, &URelationshipSubsystem::OnGameTimeHourChanged)
			);
			UE_LOG(LogRelationshipSystem, Log, TEXT("RelationshipSubsystem: Subscribed to Time.Event.HourChanged"));
		}

		if (DayChangedTag.IsValid())
		{
			TimeEventDayChangedHandle = EventBus->SubscribeToEventNative(
				DayChangedTag,
				FGameEventNativeDelegate::FDelegate::CreateUObject(this, &URelationshipSubsystem::OnGameTimeDayChanged)
			);
			UE_LOG(LogRelationshipSystem, Log, TEXT("RelationshipSubsystem: Subscribed to Time.Event.DayChanged"));
		}
	}
#else
	UE_LOG(LogRelationshipSystem, Warning, TEXT("RelationshipSubsystem: GameEventBus not available, time-based decay will use RealTime"));
#endif
}

void URelationshipSubsystem::UnsubscribeFromTimeEvents()
{
#if GAMEEVENTBUS_AVAILABLE
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();
		if (!EventBus)
		{
			return;
		}

		FGameplayTag HourChangedTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.HourChanged"), false);
		FGameplayTag DayChangedTag = FGameplayTag::RequestGameplayTag(FName("Time.Event.DayChanged"), false);

		if (HourChangedTag.IsValid() && TimeEventHourChangedHandle.IsValid())
		{
			EventBus->UnsubscribeNative(HourChangedTag, TimeEventHourChangedHandle);
			TimeEventHourChangedHandle.Reset();
			UE_LOG(LogRelationshipSystem, Verbose, TEXT("RelationshipSubsystem: Unsubscribed from Time.Event.HourChanged"));
		}

		if (DayChangedTag.IsValid() && TimeEventDayChangedHandle.IsValid())
		{
			EventBus->UnsubscribeNative(DayChangedTag, TimeEventDayChangedHandle);
			TimeEventDayChangedHandle.Reset();
			UE_LOG(LogRelationshipSystem, Verbose, TEXT("RelationshipSubsystem: Unsubscribed from Time.Event.DayChanged"));
		}
	}
#endif
}

void URelationshipSubsystem::OnGameTimeHourChanged(const FGameEventPayload& Payload)
{
#if GAMEEVENTBUS_AVAILABLE
	int32 NewHour = Payload.IntParam;
	
	UE_LOG(LogRelationshipSystem, Verbose, TEXT("RelationshipSubsystem: Game hour changed to %d, processing hourly decay"), NewHour);

	// Process relationship decay for 1 game hour
	ProcessDecay(1.0f);
#endif
}

void URelationshipSubsystem::OnGameTimeDayChanged(const FGameEventPayload& Payload)
{
#if GAMEEVENTBUS_AVAILABLE
	int32 NewDay = Payload.IntParam;
	
	UE_LOG(LogRelationshipSystem, Log, TEXT("RelationshipSubsystem: Game day changed to %d, processing daily relationship maintenance"), NewDay);

	// Additional processing on day change
	ProcessDailyRelationshipMaintenance();
#endif
}

void URelationshipSubsystem::ProcessDailyRelationshipMaintenance()
{
	UE_LOG(LogRelationshipSystem, Verbose, TEXT("RelationshipSubsystem: Running daily relationship maintenance"));

	URelationshipDatabase* Database = GetDatabase();
	if (!Database)
	{
		return;
	}

	int32 ExpiredTraitsCount = 0;
	float CurrentTime = GetCurrentGameTime();

	// Process all relationships
	for (auto& Pair : Relationships)
	{
		FRelationshipData& RelData = Pair.Value;
		
		if (!RelData.IsValid())
		{
			continue;
		}

		// Check state transitions (could have changed over the day)
		EvaluateStateTransitions(RelData.Subject.Get(), RelData.Target.Get());
	}

	// Clean up invalid relationships
	ValidateAndCleanupRelationships();
}

void URelationshipSubsystem::ValidateAndCleanupRelationships()
{
	int32 RemovedCount = 0;

	for (auto It = Relationships.CreateIterator(); It; ++It)
	{
		if (!It.Value().IsValid())
		{
			UE_LOG(LogRelationshipSystem, Verbose, TEXT("Removing invalid relationship (actors destroyed)"));
			It.RemoveCurrent();
			RemovedCount++;
		}
	}

	if (RemovedCount > 0)
	{
		UE_LOG(LogRelationshipSystem, Log, TEXT("RelationshipSubsystem: Cleaned up %d invalid relationships"), RemovedCount);
	}
}

bool URelationshipSubsystem::IsGameEventBusAvailable() const
{
#if GAMEEVENTBUS_AVAILABLE
	return FModuleManager::Get().IsModuleLoaded("GameEventBus");
#else
	return false;
#endif
}

void URelationshipSubsystem::SendGameEvent(FGameplayTag EventTag, AActor* Subject, AActor* Target, const TMap<FName, FString>& Payload)
{
#if GAMEEVENTBUS_AVAILABLE
	if (!IsGameEventBusAvailable())
	{
		return;
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();
		if (!EventBus)
		{
			return;
		}

		// Create event payload
		FGameEventPayload EventPayload(EventTag);
		EventPayload.InstigatorActor = Subject;
		EventPayload.TargetActor = Target;

		// Add parameters
		if (const FString* DimensionStr = Payload.Find("Dimension"))
		{
			EventPayload.StringParam = FName(**DimensionStr);
		}
		if (const FString* ValueStr = Payload.Find("Value"))
		{
			EventPayload.FloatParam = FCString::Atof(**ValueStr);
		}

		// Emit event
		EventBus->EmitEvent(EventPayload, false);

		UE_LOG(LogRelationshipSystem, Verbose, TEXT("GameEvent: %s from %s to %s"),
			*EventTag.ToString(),
			Subject ? *Subject->GetName() : TEXT("None"),
			Target ? *Target->GetName() : TEXT("None"));
	}
#else
	UE_LOG(LogRelationshipSystem, Verbose, TEXT("GameEvent skipped (GameEventBus not available): %s"),
		*EventTag.ToString());
#endif
}
