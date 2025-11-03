// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/RelationshipProfile.h"
#include "DataAssets/RelationshipDatabase.h"
#include "DataAssets/RelationshipDimension.h"
#include "RelationshipSystemCore.h"

URelationshipProfile::URelationshipProfile()
	: bCanBeSubject(true)
	, bCanBeTarget(true)
	, bPersistRelationships(true)
{
	DisplayColor = FLinearColor(0.6f, 0.6f, 1.0f); // Light blue default
}

float URelationshipProfile::GetInitialDimensionValue(FGameplayTag DimensionTag, const URelationshipDatabase* Database) const
{
	// Check for override in profile
	if (const float* OverrideValue = InitialDimensionValues.Find(DimensionTag))
	{
		return *OverrideValue;
	}

	// Fall back to dimension default
	if (Database)
	{
		if (const URelationshipDimension* Dimension = Database->FindDimension(DimensionTag))
		{
			return Dimension->DefaultValue;
		}
	}

	return 0.0f;
}

float URelationshipProfile::GetDecayRateOverride(FGameplayTag DimensionTag, const URelationshipDatabase* Database) const
{
	// Check for override in profile
	if (const float* OverrideRate = DecayRateOverrides.Find(DimensionTag))
	{
		return *OverrideRate;
	}

	// Fall back to dimension default
	if (Database)
	{
		if (const URelationshipDimension* Dimension = Database->FindDimension(DimensionTag))
		{
			return Dimension->DecayRatePerHour;
		}
	}

	return 0.0f;
}

float URelationshipProfile::GetDimensionModifier(FGameplayTag DimensionTag) const
{
	if (const float* Modifier = DimensionModifiers.Find(DimensionTag))
	{
		return *Modifier;
	}
	return 0.0f;
}

bool URelationshipProfile::HasInitialTrait(FGameplayTag TraitTag) const
{
	return InitialTraits.HasTagExact(TraitTag);
}

TMap<FGameplayTag, FRelationshipDimensionValue> URelationshipProfile::CreateInitialDimensions(
	const URelationshipDatabase* Database, float CurrentGameTime) const
{
	TMap<FGameplayTag, FRelationshipDimensionValue> Result;

	if (!Database)
	{
		return Result;
	}

	// Get all dimensions from database
	TArray<URelationshipDimension*> AllDimensions = Database->GetAllDimensions();

	for (URelationshipDimension* Dimension : AllDimensions)
	{
		if (!Dimension)
		{
			continue;
		}

		FGameplayTag DimTag = Dimension->GetAssetTag();
		float InitialValue = GetInitialDimensionValue(DimTag, Database);
		
		// Clamp to dimension bounds
		InitialValue = Dimension->ClampValue(InitialValue);

		FRelationshipDimensionValue DimValue(DimTag, InitialValue);
		DimValue.LastModifiedTime = CurrentGameTime;

		Result.Add(DimTag, DimValue);
	}

	// Add any profile-specific dimensions not in database (for flexibility)
	for (const auto& Pair : InitialDimensionValues)
	{
		if (!Result.Contains(Pair.Key))
		{
			FRelationshipDimensionValue DimValue(Pair.Key, Pair.Value);
			DimValue.LastModifiedTime = CurrentGameTime;
			Result.Add(Pair.Key, DimValue);
		}
	}

	return Result;
}

bool URelationshipProfile::ValidateInternal(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	// Validate initial state
	if (!InitialState.IsValid())
	{
		OutErrors.Add(FText::FromString(TEXT("Initial state is not set")));
		// Not critical, system can handle this
	}

	// Validate dimension values
	for (const auto& Pair : InitialDimensionValues)
	{
		if (!Pair.Key.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Invalid dimension tag in initial values")));
			bIsValid = false;
		}
	}

	// Validate decay overrides
	for (const auto& Pair : DecayRateOverrides)
	{
		if (!Pair.Key.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Invalid dimension tag in decay overrides")));
			bIsValid = false;
		}

		if (Pair.Value < 0.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("Decay rate cannot be negative")));
			bIsValid = false;
		}
	}

	// Validate dimension modifiers
	for (const auto& Pair : DimensionModifiers)
	{
		if (!Pair.Key.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Invalid dimension tag in modifiers")));
			bIsValid = false;
		}
	}

	// Check that actor can be at least subject or target
	if (!bCanBeSubject && !bCanBeTarget)
	{
		OutErrors.Add(FText::FromString(TEXT("Profile must allow actor to be either subject or target")));
		bIsValid = false;
	}

	return bIsValid;
}
