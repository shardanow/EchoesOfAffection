// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/RelationshipDataAsset.h"
#include "Curves/CurveFloat.h"
#include "RelationshipDimension.generated.h"

/**
 * Defines a single dimension/metric of a relationship
 * Examples: Trust, Friendship, Fear, Respect, Hostility
 * Each dimension has min/max bounds, decay behavior, and presentation settings
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API URelationshipDimension : public URelationshipDataAsset
{
	GENERATED_BODY()

public:
	URelationshipDimension();

	/** Minimum value for this dimension */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
	float MinValue;

	/** Maximum value for this dimension */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
	float MaxValue;

	/** Default/starting value */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dimension")
	float DefaultValue;

	/** Enable automatic decay over time */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decay")
	bool bEnableDecay;

	/** Target value for decay (usually 0 or DefaultValue) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decay", meta = (EditCondition = "bEnableDecay"))
	float DecayTarget;

	/** Decay rate per game hour (positive = decay toward target) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decay", meta = (EditCondition = "bEnableDecay"))
	float DecayRatePerHour;

	/** Curve for non-linear decay (optional, X=current value, Y=decay multiplier) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Decay", meta = (EditCondition = "bEnableDecay"))
	FRuntimeFloatCurve DecayCurve;

	/** Clamp value to min/max bounds */
	UFUNCTION(BlueprintCallable, Category = "Dimension")
	float ClampValue(float Value) const;

	/** Normalize value to 0-1 range */
	UFUNCTION(BlueprintCallable, Category = "Dimension")
	float NormalizeValue(float Value) const;

	/** Calculate decay amount for given time delta */
	UFUNCTION(BlueprintCallable, Category = "Dimension")
	float CalculateDecay(float CurrentValue, float DeltaHours) const;

	/** Apply decay to value */
	UFUNCTION(BlueprintCallable, Category = "Dimension")
	float ApplyDecay(float CurrentValue, float DeltaHours) const;

	/** Check if value is at minimum */
	UFUNCTION(BlueprintCallable, Category = "Dimension")
	bool IsAtMinimum(float Value) const;

	/** Check if value is at maximum */
	UFUNCTION(BlueprintCallable, Category = "Dimension")
	bool IsAtMaximum(float Value) const;

	/** Get range (Max - Min) */
	UFUNCTION(BlueprintCallable, Category = "Dimension")
	float GetRange() const;

protected:
	virtual bool ValidateInternal(TArray<FText>& OutErrors) const override;
};

/**
 * Container for a dimension value with metadata
 * Stores current value and tracks changes over time
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipDimensionValue
{
	GENERATED_BODY()

	/** The dimension this value represents */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimension")
	FGameplayTag DimensionTag;

	/** Current value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimension")
	float Value;

	/** Last modification timestamp (game time in seconds) */
	UPROPERTY(BlueprintReadOnly, Category = "Dimension")
	float LastModifiedTime;

	/** Total accumulated change (for analytics) */
	UPROPERTY(BlueprintReadOnly, Category = "Dimension")
	float AccumulatedChange;

	FRelationshipDimensionValue()
		: Value(0.0f)
		, LastModifiedTime(0.0f)
		, AccumulatedChange(0.0f)
	{
	}

	FRelationshipDimensionValue(FGameplayTag InTag, float InValue)
		: DimensionTag(InTag)
		, Value(InValue)
		, LastModifiedTime(0.0f)
		, AccumulatedChange(0.0f)
	{
	}

	/** Modify value and update metadata */
	void Modify(float Delta, float CurrentGameTime);

	/** Apply decay */
	void ApplyDecay(const URelationshipDimension* Dimension, float DeltaHours, float CurrentGameTime);
};
