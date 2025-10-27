// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Core/NeedsTypes.h"
#include "NeedEffectAsset.generated.h"

/**
 * Single need modification in an effect
 */
USTRUCT(BlueprintType)
struct NEEDSCORE_API FNeedEffectModification
{
	GENERATED_BODY()

	/** Which need to affect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (Categories = "Need"))
	FGameplayTag TargetNeedTag;

	/** Value to add/subtract (can be negative) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float ValueChange = 10.0f;

	/** Optional curve for value scaling (X = duration progress 0..1, Y = multiplier) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FRuntimeFloatCurve ValueCurve;

	/** Use curve instead of constant value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bUseCurve = false;

	FNeedEffectModification()
		: ValueChange(10.0f)
		, bUseCurve(false)
	{
	}

	/** Calculate value for a given progress (0..1) */
	float CalculateValue(float Progress) const
	{
		if (bUseCurve && ValueCurve.GetRichCurveConst() && ValueCurve.GetRichCurveConst()->GetNumKeys() > 0)
		{
			const float CurveMultiplier = ValueCurve.GetRichCurveConst()->Eval(Progress);
			return ValueChange * CurveMultiplier;
		}
		return ValueChange;
	}
};

/**
 * Data Asset defining an effect that can be applied to needs
 * Examples: Eating food, sleeping, taking a shower
 */
UCLASS(BlueprintType)
class NEEDSCORE_API UNeedEffectAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ========================================
	// IDENTIFICATION
	// ========================================

	/** Unique tag for this effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (Categories = "Need.Effect"))
	FGameplayTag EffectTag;

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
	// EFFECT TYPE
	// ========================================

	/** How this effect is applied */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Type")
	ENeedEffectType EffectType = ENeedEffectType::Instant;

	/** Duration for OverTime effects (in game hours) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Type", meta = (EditCondition = "EffectType == ENeedEffectType::OverTime", ClampMin = "0.1"))
	float Duration = 1.0f;

	// ========================================
	// MODIFICATIONS
	// ========================================

	/** List of need modifications */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifications")
	TArray<FNeedEffectModification> Modifications;

	// ========================================
	// STACKING
	// ========================================

	/** Can this effect stack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stacking")
	bool bCanStack = false;

	/** Maximum stack count */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stacking", meta = (EditCondition = "bCanStack", ClampMin = "1"))
	int32 MaxStacks = 5;

	/** Stack behavior: Add duration or add power */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stacking", meta = (EditCondition = "bCanStack"))
	bool bStackAddsDuration = true;

	// ========================================
	// TAGS
	// ========================================

	/** Tags granted while this effect is active */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer GrantedTags;

	/** Tags required for this effect to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer RequiredTags;

	/** Tags that block this effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer BlockedByTags;

	// ========================================
	// HELPERS
	// ========================================

	/** Can this effect apply given current tags */
	bool CanApply(const FGameplayTagContainer& CurrentTags) const
	{
		// Check required tags
		if (!RequiredTags.IsEmpty() && !CurrentTags.HasAll(RequiredTags))
		{
			return false;
		}

		// Check blocking tags
		if (!BlockedByTags.IsEmpty() && CurrentTags.HasAny(BlockedByTags))
		{
			return false;
		}

		return true;
	}

	/** Calculate total value change for a modification over duration */
	float CalculateTotalValue(const FNeedEffectModification& Modification) const
	{
		if (EffectType == ENeedEffectType::Instant)
		{
			return Modification.CalculateValue(1.0f);
		}

		// For over time, integrate curve or use total value
		return Modification.CalculateValue(1.0f);
	}
};
