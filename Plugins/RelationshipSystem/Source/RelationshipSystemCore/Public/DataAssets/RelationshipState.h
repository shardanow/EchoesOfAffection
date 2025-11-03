// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/RelationshipDataAsset.h"
#include "GameplayTagContainer.h"
#include "RelationshipState.generated.h"

class URelationshipRule;

/**
 * Transition between relationship states
 * Defines conditions for moving from one state to another
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipStateTransition
{
	GENERATED_BODY()

	/** Target state tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition", meta = (Categories = "Relationship.State"))
	FGameplayTag TargetState;

	/** Rules that must be satisfied for transition (AND logic) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition", meta = (Categories = "Relationship.Rule"))
	FGameplayTagContainer RequiredRules;

	/** Priority (higher = evaluated first) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	int32 Priority;

	/** Hysteresis: must remain valid for this many seconds before transitioning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	float HysteresisDelay;

	/** Display name for editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	FText DisplayName;

	FRelationshipStateTransition()
		: Priority(0)
		, HysteresisDelay(0.0f)
	{
	}
};

/**
 * Represents a stage of relationship (Stranger, Acquaintance, Friend, Enemy, etc.)
 * States define behavior, available actions, and transition conditions
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API URelationshipState : public URelationshipDataAsset
{
	GENERATED_BODY()

public:
	URelationshipState();

	/** Is this the default/initial state? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDefaultState;

	/** Transitions to other states */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transitions")
	TArray<FRelationshipStateTransition> Transitions;

	/** Actions available in this state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State", meta = (Categories = "Relationship.Action"))
	FGameplayTagContainer AvailableActions;

	/** Traits automatically applied in this state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State", meta = (Categories = "Relationship.Trait"))
	FGameplayTagContainer AutoTraits;

	/** Dimension modifiers applied while in this state (e.g., +10% Trust gain) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	TMap<FGameplayTag, float> DimensionModifiers;

	/** Custom gameplay tags associated with this state (for external queries) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State")
	FGameplayTagContainer StateTags;

	/** Sort transitions by priority (descending) */
	UFUNCTION(BlueprintCallable, Category = "State")
	void SortTransitionsByPriority();

	/** Get transitions sorted by priority */
	UFUNCTION(BlueprintCallable, Category = "State")
	TArray<FRelationshipStateTransition> GetSortedTransitions() const;

	/** Check if action is available in this state */
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsActionAvailable(FGameplayTag ActionTag) const;

	/** Get dimension modifier for specific dimension (0.0 if not found) */
	UFUNCTION(BlueprintCallable, Category = "State")
	float GetDimensionModifier(FGameplayTag DimensionTag) const;

protected:
	virtual bool ValidateInternal(TArray<FText>& OutErrors) const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

/**
 * Runtime state instance with transition tracking
 * Tracks hysteresis timers for smooth state transitions
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipStateInstance
{
	GENERATED_BODY()

	/** Current active state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	FGameplayTag CurrentState;

	/** Time entered current state (game time in seconds) */
	UPROPERTY(BlueprintReadOnly, Category = "State")
	float StateEntryTime;

	/** Pending transitions with hysteresis timers */
	UPROPERTY()
	TMap<FGameplayTag, float> PendingTransitions;

	FRelationshipStateInstance()
		: StateEntryTime(0.0f)
	{
	}

	/** Set new state and clear pending transitions */
	void SetState(FGameplayTag NewState, float CurrentGameTime);

	/** Update pending transition timer */
	void UpdatePendingTransition(FGameplayTag TargetState, float DeltaTime, float HysteresisDelay);

	/** Clear pending transition */
	void ClearPendingTransition(FGameplayTag TargetState);

	/** Check if transition is ready (hysteresis satisfied) */
	bool IsTransitionReady(FGameplayTag TargetState, float HysteresisDelay) const;

	/** Get time spent in current state */
	float GetTimeInState(float CurrentGameTime) const;
};
