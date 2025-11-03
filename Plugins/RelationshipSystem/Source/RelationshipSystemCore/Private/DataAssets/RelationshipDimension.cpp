// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/RelationshipDimension.h"
#include "RelationshipSystemCore.h"

URelationshipDimension::URelationshipDimension()
	: MinValue(-100.0f)
	, MaxValue(100.0f)
	, DefaultValue(0.0f)
	, bEnableDecay(false)
	, DecayTarget(0.0f)
	, DecayRatePerHour(1.0f)
{
	DisplayColor = FLinearColor(0.3f, 0.7f, 1.0f); // Light blue default
}

float URelationshipDimension::ClampValue(float Value) const
{
	return FMath::Clamp(Value, MinValue, MaxValue);
}

float URelationshipDimension::NormalizeValue(float Value) const
{
	const float Range = GetRange();
	if (Range > KINDA_SMALL_NUMBER)
	{
		return (Value - MinValue) / Range;
	}
	return 0.5f;
}

float URelationshipDimension::CalculateDecay(float CurrentValue, float DeltaHours) const
{
	if (!bEnableDecay || DeltaHours <= 0.0f)
	{
		return 0.0f;
	}

	// Calculate direction and base decay
	const float Direction = FMath::Sign(DecayTarget - CurrentValue);
	float DecayAmount = DecayRatePerHour * DeltaHours * Direction;

	// Apply curve if available
	if (DecayCurve.GetRichCurveConst() && DecayCurve.GetRichCurveConst()->GetNumKeys() > 0)
	{
		const float CurveMultiplier = DecayCurve.GetRichCurveConst()->Eval(CurrentValue);
		DecayAmount *= CurveMultiplier;
	}

	// Don't overshoot the target
	const float DistanceToTarget = FMath::Abs(DecayTarget - CurrentValue);
	if (FMath::Abs(DecayAmount) > DistanceToTarget)
	{
		DecayAmount = DistanceToTarget * Direction;
	}

	return DecayAmount;
}

float URelationshipDimension::ApplyDecay(float CurrentValue, float DeltaHours) const
{
	const float Decay = CalculateDecay(CurrentValue, DeltaHours);
	return ClampValue(CurrentValue + Decay);
}

bool URelationshipDimension::IsAtMinimum(float Value) const
{
	return FMath::IsNearlyEqual(Value, MinValue, KINDA_SMALL_NUMBER);
}

bool URelationshipDimension::IsAtMaximum(float Value) const
{
	return FMath::IsNearlyEqual(Value, MaxValue, KINDA_SMALL_NUMBER);
}

float URelationshipDimension::GetRange() const
{
	return MaxValue - MinValue;
}

bool URelationshipDimension::ValidateInternal(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	if (MinValue >= MaxValue)
	{
		OutErrors.Add(FText::FromString(FString::Printf(
			TEXT("Invalid range: MinValue (%.2f) must be less than MaxValue (%.2f)"),
			MinValue, MaxValue)));
		bIsValid = false;
	}

	if (DefaultValue < MinValue || DefaultValue > MaxValue)
	{
		OutErrors.Add(FText::FromString(FString::Printf(
			TEXT("DefaultValue (%.2f) is outside valid range [%.2f, %.2f]"),
			DefaultValue, MinValue, MaxValue)));
		bIsValid = false;
	}

	if (bEnableDecay)
	{
		if (DecayTarget < MinValue || DecayTarget > MaxValue)
		{
			OutErrors.Add(FText::FromString(FString::Printf(
				TEXT("DecayTarget (%.2f) is outside valid range [%.2f, %.2f]"),
				DecayTarget, MinValue, MaxValue)));
			bIsValid = false;
		}

		if (DecayRatePerHour < 0.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("DecayRatePerHour cannot be negative")));
			bIsValid = false;
		}
	}

	return bIsValid;
}

void FRelationshipDimensionValue::Modify(float Delta, float CurrentGameTime)
{
	Value += Delta;
	LastModifiedTime = CurrentGameTime;
	AccumulatedChange += FMath::Abs(Delta);
}

void FRelationshipDimensionValue::ApplyDecay(const URelationshipDimension* Dimension, float DeltaHours, float CurrentGameTime)
{
	if (!Dimension || !Dimension->bEnableDecay)
	{
		return;
	}

	const float OldValue = Value;
	Value = Dimension->ApplyDecay(Value, DeltaHours);
	
	if (!FMath::IsNearlyEqual(OldValue, Value))
	{
		LastModifiedTime = CurrentGameTime;
	}
}
