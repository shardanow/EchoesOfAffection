// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Core/NeedsTypes.h"
#include "NeedDefinitionAsset.generated.h"

/**
 * Data Asset defining a single need (Hunger, Energy, etc.)
 * Полностью data-driven конфигурация потребности
 */
UCLASS(BlueprintType)
class NEEDSCORE_API UNeedDefinitionAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ========================================
	// IDENTIFICATION
	// ========================================

	/** Unique tag identifying this need */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (Categories = "Need"))
	FGameplayTag NeedTag;

	/** Display name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FText DisplayName;

	/** Description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", meta = (MultiLine = true))
	FText Description;

	/** Icon for UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	TObjectPtr<UTexture2D> Icon;

	/** Color for UI visualization */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity")
	FLinearColor DisplayColor = FLinearColor::White;

	// ========================================
	// INITIAL STATE
	// ========================================

	/** Starting value (0..100) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initial State", meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float InitialValue = 100.0f;

	/** Start suppressed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initial State")
	bool bStartSuppressed = false;

	/** Start frozen */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initial State")
	bool bStartFrozen = false;

	// ========================================
	// DECAY CONFIGURATION
	// ========================================

	/** Decay configuration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decay")
	FNeedDecayConfig DecayConfig;

	// ========================================
	// THRESHOLD CONFIGURATION
	// ========================================

	/** Threshold configuration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds")
	FNeedThresholdConfig ThresholdConfig;

	// ========================================
	// MOOD CONTRIBUTION
	// ========================================

	/** Weight of this need in mood calculation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MoodWeight = 1.0f;

	/** Optional mood contribution curve (X = need value 0..1, Y = mood contribution -1..1) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
	FRuntimeFloatCurve MoodContributionCurve;

	/** Use curve for mood contribution */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
	bool bUseMoodCurve = false;

	// ========================================
	// VALIDATION
	// ========================================

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		// Validate thresholds
		ThresholdConfig.CriticalThreshold = FMath::Clamp(ThresholdConfig.CriticalThreshold, 0.0f, 100.0f);
		ThresholdConfig.WarningThreshold = FMath::Clamp(ThresholdConfig.WarningThreshold, ThresholdConfig.CriticalThreshold, 100.0f);
		ThresholdConfig.RecoveredThreshold = FMath::Clamp(ThresholdConfig.RecoveredThreshold, ThresholdConfig.WarningThreshold, 100.0f);
	}

	// ========================================
	// HELPERS
	// ========================================

	/** Calculate mood contribution for a value */
	float CalculateMoodContribution(float Value) const
	{
		if (bUseMoodCurve && MoodContributionCurve.GetRichCurveConst() && MoodContributionCurve.GetRichCurveConst()->GetNumKeys() > 0)
		{
			const float NormalizedValue = Value / 100.0f;
			return MoodContributionCurve.GetRichCurveConst()->Eval(NormalizedValue) * MoodWeight;
		}

		// Linear: value 0 = -1.0 mood, value 100 = +1.0 mood
		const float NormalizedValue = (Value - 50.0f) / 50.0f; // -1..1
		return NormalizedValue * MoodWeight;
	}
};
