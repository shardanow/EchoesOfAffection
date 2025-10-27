// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Core/NeedsTypes.h"
#include "NeedModifierAsset.generated.h"

/**
 * Condition for modifier activation
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FModifierCondition
{
	GENERATED_BODY()

	/** Required tags for this modifier to be active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTagContainer RequiredTags;

	/** Blocked by tags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTagContainer BlockedByTags;

	/** Required time of day range (if using time integration) - start hour */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition|Time", meta = (ClampMin = "0", ClampMax = "23"))
	int32 RequiredTimeStart = 0;

	/** Required time of day range - end hour */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition|Time", meta = (ClampMin = "0", ClampMax = "23"))
	int32 RequiredTimeEnd = 23;

	/** Enable time-of-day condition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition|Time")
	bool bUseTimeCondition = false;

	FModifierCondition()
		: RequiredTimeStart(0)
		, RequiredTimeEnd(23)
		, bUseTimeCondition(false)
	{
	}

	/** Check if condition is met */
	bool IsMet(const FGameplayTagContainer& CurrentTags, int32 CurrentHour) const
	{
		// Check tags
		if (!RequiredTags.IsEmpty() && !CurrentTags.HasAll(RequiredTags))
		{
			return false;
		}

		if (!BlockedByTags.IsEmpty() && CurrentTags.HasAny(BlockedByTags))
		{
			return false;
		}

		// Check time
		if (bUseTimeCondition)
		{
			if (RequiredTimeStart <= RequiredTimeEnd)
			{
				// Normal range (e.g., 8-18)
				if (CurrentHour < RequiredTimeStart || CurrentHour > RequiredTimeEnd)
				{
					return false;
				}
			}
			else
			{
				// Overnight range (e.g., 22-6)
				if (CurrentHour < RequiredTimeStart && CurrentHour > RequiredTimeEnd)
				{
					return false;
				}
			}
		}

		return true;
	}
};

/**
 * Single modification to a need's behavior
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FNeedBehaviorModification
{
	GENERATED_BODY()

	/** Which need to modify */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification", meta = (Categories = "Need"))
	FGameplayTag TargetNeedTag;

	/** Modify decay rate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification|Decay")
	bool bModifyDecayRate = false;

	/** Decay rate modification */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification|Decay", meta = (EditCondition = "bModifyDecayRate"))
	ENeedModifierOperation DecayOperation = ENeedModifierOperation::Multiply;

	/** Decay rate value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification|Decay", meta = (EditCondition = "bModifyDecayRate"))
	float DecayValue = 1.0f;

	/** Modify effect power (how strong effects are on this need) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification|Effects")
	bool bModifyEffectPower = false;

	/** Effect power modification */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification|Effects", meta = (EditCondition = "bModifyEffectPower"))
	ENeedModifierOperation EffectPowerOperation = ENeedModifierOperation::Multiply;

	/** Effect power value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification|Effects", meta = (EditCondition = "bModifyEffectPower"))
	float EffectPowerValue = 1.0f;

	/** Cap maximum value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification|Cap")
	bool bCapMaxValue = false;

	/** Maximum value cap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modification|Cap", meta = (EditCondition = "bCapMaxValue", ClampMin = "0.0", ClampMax = "100.0"))
	float MaxValueCap = 100.0f;

	FNeedBehaviorModification()
		: bModifyDecayRate(false)
		, DecayOperation(ENeedModifierOperation::Multiply)
		, DecayValue(1.0f)
		, bModifyEffectPower(false)
		, EffectPowerOperation(ENeedModifierOperation::Multiply)
		, EffectPowerValue(1.0f)
		, bCapMaxValue(false)
		, MaxValueCap(100.0f)
	{
	}

	/** Apply operation */
	float ApplyOperation(float BaseValue, ENeedModifierOperation Operation, float ModValue) const
	{
		switch (Operation)
		{
			case ENeedModifierOperation::Add:
				return BaseValue + ModValue;
			case ENeedModifierOperation::Multiply:
				return BaseValue * ModValue;
			case ENeedModifierOperation::Override:
				return ModValue;
			default:
				return BaseValue;
		}
	}

	/** Apply decay modification */
	float ModifyDecayRate(float BaseDecay) const
	{
		if (!bModifyDecayRate)
		{
			return BaseDecay;
		}
		return ApplyOperation(BaseDecay, DecayOperation, DecayValue);
	}

	/** Apply effect power modification */
	float ModifyEffectPower(float BasePower) const
	{
		if (!bModifyEffectPower)
		{
			return BasePower;
		}
		return ApplyOperation(BasePower, EffectPowerOperation, EffectPowerValue);
	}

	/** Apply value cap */
	float ApplyValueCap(float Value) const
	{
		if (!bCapMaxValue)
		{
			return Value;
		}
		return FMath::Min(Value, MaxValueCap);
	}
};

/**
 * Data Asset defining a modifier that changes need behavior
 * Examples: "Well Rested" buff, "Sick" debuff, "Night Time" environmental
 */
UCLASS(BlueprintType)
class NEEDSCORE_API UNeedModifierAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ========================================
	// IDENTIFICATION
	// ========================================

	/** Unique tag for this modifier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (Categories = "Need.Modifier"))
	FGameplayTag ModifierTag;

	/** Display name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	/** Description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (MultiLine = true))
	FText Description;

	/** Icon for UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	TObjectPtr<UTexture2D> Icon;

	// ========================================
	// PRIORITY
	// ========================================

	/** Priority (higher = applied later, can override lower priority modifiers) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Priority", meta = (ClampMin = "0"))
	int32 Priority = 0;

	// ========================================
	// ACTIVATION CONDITION
	// ========================================

	/** Condition for this modifier to be active */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	FModifierCondition ActivationCondition;

	// ========================================
	// MODIFICATIONS
	// ========================================

	/** List of behavior modifications */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifications")
	TArray<FNeedBehaviorModification> Modifications;

	// ========================================
	// TAGS
	// ========================================

	/** Tags granted while this modifier is active */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer GrantedTags;

	// ========================================
	// HELPERS
	// ========================================

	/** Check if modifier should be active */
	bool ShouldBeActive(const FGameplayTagContainer& CurrentTags, int32 CurrentHour) const
	{
		return ActivationCondition.IsMet(CurrentTags, CurrentHour);
	}

	/** Find modification for a specific need */
	const FNeedBehaviorModification* FindModification(const FGameplayTag& NeedTag) const
	{
		for (const FNeedBehaviorModification& Mod : Modifications)
		{
			if (Mod.TargetNeedTag == NeedTag)
			{
				return &Mod;
			}
		}
		return nullptr;
	}
};
