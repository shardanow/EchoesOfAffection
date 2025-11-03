// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/RelationshipDataAsset.h"
#include "GameplayTagContainer.h"
#include "RelationshipAction.generated.h"

/**
 * Effect applied when action is executed
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipActionEffect
{
	GENERATED_BODY()

	/** Target dimension */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (Categories = "Relationship.Dimension"))
	FGameplayTag DimensionTag;

	/** Base value change */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float BaseValue;

	/** Random variance (+/- percentage, 0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RandomVariance;

	/** Success probability (0-1, 1.0 = always succeeds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SuccessProbability;

	FRelationshipActionEffect()
		: BaseValue(0.0f)
		, RandomVariance(0.0f)
		, SuccessProbability(1.0f)
	{
	}

	/** Calculate final value with variance */
	float CalculateFinalValue() const;

	/** Check if effect succeeds */
	bool RollSuccess() const;
};

/**
 * Cost required to execute action
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipActionCost
{
	GENERATED_BODY()

	/** Dimension required */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost", meta = (Categories = "Relationship.Dimension"))
	FGameplayTag DimensionTag;

	/** Minimum value required */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost")
	float MinimumValue;

	/** Value consumed (0 = no consumption, just requirement) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost")
	float ConsumedValue;

	FRelationshipActionCost()
		: MinimumValue(0.0f)
		, ConsumedValue(0.0f)
	{
	}
};

/**
 * Defines an action that can be performed to affect relationships
 * Examples: Talk, Gift, Insult, Help, Betray
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API URelationshipAction : public URelationshipDataAsset
{
	GENERATED_BODY()

public:
	URelationshipAction();

	/** Effects applied when action succeeds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TArray<FRelationshipActionEffect> Effects;

	/** Costs to execute this action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	TArray<FRelationshipActionCost> Costs;

	/** Rules that must be satisfied to execute (availability gates) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action", meta = (Categories = "Relationship.Rule"))
	FGameplayTagContainer RequiredRules;

	/** Cooldown after execution (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	float Cooldown;

	/** Maximum uses (0 = unlimited) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
	int32 MaxUses;

	/** Events triggered when action is executed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Events", meta = (Categories = "Relationship.Event"))
	FGameplayTagContainer TriggeredEvents;

	/** Animation/visual identifier for UI/gameplay */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	FName AnimationTag;

	/** Sound to play on execution */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<USoundBase> ExecutionSound;

	/** Check if action can be executed (costs, rules) */
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool CanExecute(const TMap<FGameplayTag, float>& DimensionValues, const class URelationshipDatabase* Database) const;

	/** Get all dimension tags referenced by this action */
	UFUNCTION(BlueprintCallable, Category = "Action")
	TArray<FGameplayTag> GetReferencedDimensions() const;

	/** Calculate total dimension change (sum of all effects) */
	UFUNCTION(BlueprintCallable, Category = "Action")
	TMap<FGameplayTag, float> CalculateTotalEffects() const;

protected:
	virtual bool ValidateInternal(TArray<FText>& OutErrors) const override;
};

/**
 * Runtime action instance tracking usage
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipActionInstance
{
	GENERATED_BODY()

	/** Action tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	FGameplayTag ActionTag;

	/** Times used */
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	int32 UsageCount;

	/** Last execution time (game time in seconds) */
	UPROPERTY(BlueprintReadOnly, Category = "Action")
	float LastExecutionTime;

	FRelationshipActionInstance()
		: UsageCount(0)
		, LastExecutionTime(0.0f)
	{
	}

	FRelationshipActionInstance(FGameplayTag InTag)
		: ActionTag(InTag)
		, UsageCount(0)
		, LastExecutionTime(0.0f)
	{
	}

	/** Check if action is on cooldown */
	bool IsOnCooldown(float CurrentGameTime, float CooldownDuration) const;

	/** Check if max uses reached */
	bool IsMaxUsesReached(int32 MaxUses) const;

	/** Record execution */
	void RecordExecution(float CurrentGameTime);
};
