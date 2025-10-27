// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/NeedsTypes.h"
#include "NeedDefinitionAsset.h"
#include "NeedsSettings.generated.h"

/**
 * Global settings for the Needs System
 * Central configuration asset
 */
UCLASS(BlueprintType)
class NEEDSCORE_API UNeedsSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ========================================
	// NEEDS DEFINITIONS
	// ========================================

	/** List of all available needs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Needs")
	TArray<TObjectPtr<UNeedDefinitionAsset>> AvailableNeeds;

	// ========================================
	// MOOD CALCULATION
	// ========================================

	/** Enable mood calculation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
	bool bEnableMood = true;

	/** Mood calculation curve (optional) - X = weighted average of needs (0..1), Y = final mood (-1..1) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
	FRuntimeFloatCurve MoodCalculationCurve;

	/** Use curve for mood calculation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood")
	bool bUseMoodCurve = false;

	/** Mood change threshold for broadcasting events (prevents spam) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mood", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float MoodChangeThreshold = 5.0f;

	// ========================================
	// LOD CONFIGURATION
	// ========================================

	/** LOD configuration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LOD")
	FNeedLODConfig LODConfig;

	// ========================================
	// UPDATE FREQUENCY
	// ========================================

	/** Update frequency (updates per second) - 0 = every frame */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (ClampMin = "0.0", ClampMax = "60.0"))
	float UpdateFrequency = 10.0f;

	/** Batch size for component updates */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta = (ClampMin = "1", ClampMax = "100"))
	int32 BatchSize = 10;

	// ========================================
	// TIME INTEGRATION
	// ========================================

	/** Use game time from TimeOfDaySystem (if available) instead of real time */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Time Integration")
	bool bUseGameTime = true;

	/** Real time scale: how many game hours pass per real hour (only used when NOT using TimeOfDaySystem) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Time Integration", meta = (EditCondition = "!bUseGameTime", ClampMin = "0.01"))
	float RealTimeScale = 1.0f;

	// ========================================
	// DEBUG
	// ========================================

	/** Enable debug visualization */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bEnableDebugVisualization = false;

	/** Debug text size */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug", meta = (EditCondition = "bEnableDebugVisualization"))
	float DebugTextSize = 1.5f;

	// ========================================
	// HELPERS
	// ========================================

	/** Find need definition by tag */
	UNeedDefinitionAsset* FindNeedDefinition(const FGameplayTag& NeedTag) const
	{
		for (UNeedDefinitionAsset* Need : AvailableNeeds)
		{
			if (Need && Need->NeedTag == NeedTag)
			{
				return Need;
			}
		}
		return nullptr;
	}

	/** Get all need tags */
	TArray<FGameplayTag> GetAllNeedTags() const
	{
		TArray<FGameplayTag> Tags;
		for (UNeedDefinitionAsset* Need : AvailableNeeds)
		{
			if (Need && Need->NeedTag.IsValid())
			{
				Tags.Add(Need->NeedTag);
			}
		}
		return Tags;
	}

	/** Calculate mood from need values */
	float CalculateMood(const TMap<FGameplayTag, float>& NeedValues) const
	{
		if (!bEnableMood || AvailableNeeds.Num() == 0)
		{
			return 0.0f;
		}

		float TotalWeight = 0.0f;
		float WeightedSum = 0.0f;

		// Calculate weighted average of mood contributions
		for (UNeedDefinitionAsset* Need : AvailableNeeds)
		{
			if (!Need || !Need->NeedTag.IsValid())
			{
				continue;
			}

			const float* ValuePtr = NeedValues.Find(Need->NeedTag);
			if (!ValuePtr)
			{
				continue;
			}

			const float Contribution = Need->CalculateMoodContribution(*ValuePtr);
			WeightedSum += Contribution;
			TotalWeight += Need->MoodWeight;
		}

		if (TotalWeight <= 0.0f)
		{
			return 0.0f;
		}

		// Normalize to -1..1 range
		const float NormalizedMood = WeightedSum / TotalWeight;

		// Apply optional curve
		if (bUseMoodCurve && MoodCalculationCurve.GetRichCurveConst() && MoodCalculationCurve.GetRichCurveConst()->GetNumKeys() > 0)
		{
			// Convert -1..1 to 0..1 for curve input
			const float CurveInput = (NormalizedMood + 1.0f) * 0.5f;
			const float CurveOutput = MoodCalculationCurve.GetRichCurveConst()->Eval(CurveInput);
			return FMath::Clamp(CurveOutput * 100.0f, -100.0f, 100.0f);
		}

		// Convert to -100..100 range
		return FMath::Clamp(NormalizedMood * 100.0f, -100.0f, 100.0f);
	}
};
