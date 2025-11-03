// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/RelationshipAction.h"
#include "DataAssets/RelationshipDatabase.h"
#include "DataAssets/RelationshipRule.h"
#include "RelationshipSystemCore.h"

// FRelationshipActionEffect implementation

float FRelationshipActionEffect::CalculateFinalValue() const
{
	float FinalValue = BaseValue;

	if (RandomVariance > 0.0f)
	{
		const float Variance = BaseValue * RandomVariance;
		FinalValue += FMath::RandRange(-Variance, Variance);
	}

	return FinalValue;
}

bool FRelationshipActionEffect::RollSuccess() const
{
	return FMath::FRand() <= SuccessProbability;
}

// URelationshipAction implementation

URelationshipAction::URelationshipAction()
	: Cooldown(0.0f)
	, MaxUses(0)
{
	DisplayColor = FLinearColor(1.0f, 0.6f, 0.2f); // Orange default
}

bool URelationshipAction::CanExecute(const TMap<FGameplayTag, float>& DimensionValues, const URelationshipDatabase* Database) const
{
	if (!Database)
	{
		return false;
	}

	// Check costs
	for (const FRelationshipActionCost& Cost : Costs)
	{
		const float* CurrentValue = DimensionValues.Find(Cost.DimensionTag);
		if (!CurrentValue || *CurrentValue < Cost.MinimumValue)
		{
			return false; // Cost not met
		}
	}

	// Check required rules
	for (const FGameplayTag& RuleTag : RequiredRules)
	{
		if (const URelationshipRule* Rule = Database->FindRule(RuleTag))
		{
			if (!Rule->Evaluate(DimensionValues))
			{
				return false; // Rule failed
			}
		}
		else
		{
			UE_LOG(LogRelationshipSystem, Warning, TEXT("Required rule not found: %s"), *RuleTag.ToString());
			return false;
		}
	}

	return true;
}

TArray<FGameplayTag> URelationshipAction::GetReferencedDimensions() const
{
	TSet<FGameplayTag> AllDimensions;

	// Collect from effects
	for (const FRelationshipActionEffect& Effect : Effects)
	{
		if (Effect.DimensionTag.IsValid())
		{
			AllDimensions.Add(Effect.DimensionTag);
		}
	}

	// Collect from costs
	for (const FRelationshipActionCost& Cost : Costs)
	{
		if (Cost.DimensionTag.IsValid())
		{
			AllDimensions.Add(Cost.DimensionTag);
		}
	}

	return AllDimensions.Array();
}

TMap<FGameplayTag, float> URelationshipAction::CalculateTotalEffects() const
{
	TMap<FGameplayTag, float> TotalEffects;

	for (const FRelationshipActionEffect& Effect : Effects)
	{
		if (Effect.DimensionTag.IsValid())
		{
			float& Total = TotalEffects.FindOrAdd(Effect.DimensionTag);
			Total += Effect.BaseValue; // Use base value for calculation
		}
	}

	return TotalEffects;
}

bool URelationshipAction::ValidateInternal(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	// Validate effects
	for (const FRelationshipActionEffect& Effect : Effects)
	{
		if (!Effect.DimensionTag.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Effect has invalid dimension tag")));
			bIsValid = false;
		}

		if (Effect.RandomVariance < 0.0f || Effect.RandomVariance > 1.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("RandomVariance must be between 0 and 1")));
			bIsValid = false;
		}

		if (Effect.SuccessProbability < 0.0f || Effect.SuccessProbability > 1.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("SuccessProbability must be between 0 and 1")));
			bIsValid = false;
		}
	}

	// Validate costs
	for (const FRelationshipActionCost& Cost : Costs)
	{
		if (!Cost.DimensionTag.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Cost has invalid dimension tag")));
			bIsValid = false;
		}

		if (Cost.ConsumedValue < 0.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("ConsumedValue cannot be negative")));
			bIsValid = false;
		}
	}

	// Validate cooldown and max uses
	if (Cooldown < 0.0f)
	{
		OutErrors.Add(FText::FromString(TEXT("Cooldown cannot be negative")));
		bIsValid = false;
	}

	if (MaxUses < 0)
	{
		OutErrors.Add(FText::FromString(TEXT("MaxUses cannot be negative")));
		bIsValid = false;
	}

	return bIsValid;
}

// FRelationshipActionInstance implementation

bool FRelationshipActionInstance::IsOnCooldown(float CurrentGameTime, float CooldownDuration) const
{
	if (CooldownDuration <= 0.0f)
	{
		return false; // No cooldown
	}

	return (CurrentGameTime - LastExecutionTime) < CooldownDuration;
}

bool FRelationshipActionInstance::IsMaxUsesReached(int32 MaxUses) const
{
	if (MaxUses <= 0)
	{
		return false; // Unlimited uses
	}

	return UsageCount >= MaxUses;
}

void FRelationshipActionInstance::RecordExecution(float CurrentGameTime)
{
	UsageCount++;
	LastExecutionTime = CurrentGameTime;
}
