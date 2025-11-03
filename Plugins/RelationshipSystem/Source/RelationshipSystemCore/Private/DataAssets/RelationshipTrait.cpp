// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/RelationshipTrait.h"
#include "RelationshipSystemCore.h"

URelationshipTrait::URelationshipTrait()
	: bCanStack(false)
	, MaxStacks(1)
{
	DisplayColor = FLinearColor(0.8f, 0.3f, 0.8f); // Purple default
}

FTraitDimensionModifier URelationshipTrait::GetModifierForDimension(FGameplayTag DimensionTag, FGameplayTag ActionTag) const
{
	for (const FTraitDimensionModifier& Modifier : DimensionModifiers)
	{
		if (Modifier.DimensionTag == DimensionTag && Modifier.AppliesToAction(ActionTag))
		{
			return Modifier;
		}
	}
	
	// Return default modifier
	return FTraitDimensionModifier();
}

FTraitEventResponse URelationshipTrait::GetEventResponse(FGameplayTag EventTag) const
{
	for (const FTraitEventResponse& Response : EventResponses)
	{
		if (Response.EventTag.MatchesTagExact(EventTag))
		{
			return Response;
		}
	}
	
	// Return default response
	return FTraitEventResponse();
}

bool URelationshipTrait::ConflictsWith(FGameplayTag OtherTraitTag) const
{
	return ConflictingTraits.HasTagExact(OtherTraitTag);
}

float URelationshipTrait::ApplyModifiers(FGameplayTag DimensionTag, FGameplayTag ActionTag, float BaseValue, int32 StackCount) const
{
	float ModifiedValue = BaseValue;

	for (const FTraitDimensionModifier& Modifier : DimensionModifiers)
	{
		if (Modifier.DimensionTag == DimensionTag && Modifier.AppliesToAction(ActionTag))
		{
			// Apply modifier once per stack
			for (int32 i = 0; i < StackCount; ++i)
			{
				ModifiedValue = Modifier.Apply(ModifiedValue);
			}
		}
	}

	return ModifiedValue;
}

bool URelationshipTrait::ValidateInternal(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	// Validate stack settings
	if (bCanStack && MaxStacks < 1)
	{
		OutErrors.Add(FText::FromString(TEXT("MaxStacks must be at least 1 when stacking is enabled")));
		bIsValid = false;
	}

	// Validate dimension modifiers
	for (const FTraitDimensionModifier& Modifier : DimensionModifiers)
	{
		if (!Modifier.DimensionTag.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Dimension modifier has invalid dimension tag")));
			bIsValid = false;
		}

		if (Modifier.MultiplicativeModifier < 0.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("Multiplicative modifier cannot be negative")));
			bIsValid = false;
		}
	}

	// Validate event responses
	for (const FTraitEventResponse& Response : EventResponses)
	{
		if (!Response.EventTag.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Event response has invalid event tag")));
			bIsValid = false;
		}

		if (Response.ResponseProbability < 0.0f || Response.ResponseProbability > 1.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("Response probability must be between 0 and 1")));
			bIsValid = false;
		}

		if (Response.Cooldown < 0.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("Response cooldown cannot be negative")));
			bIsValid = false;
		}
	}

	// Check for self-conflict
	if (ConflictingTraits.HasTagExact(AssetTag))
	{
		OutErrors.Add(FText::FromString(TEXT("Trait cannot conflict with itself")));
		bIsValid = false;
	}

	// Check for self-requirement
	if (RequiredTraits.HasTagExact(AssetTag))
	{
		OutErrors.Add(FText::FromString(TEXT("Trait cannot require itself")));
		bIsValid = false;
	}

	return bIsValid;
}

// FRelationshipTraitInstance implementation

void FRelationshipTraitInstance::AddStack(const URelationshipTrait* Trait)
{
	if (Trait && Trait->bCanStack && StackCount < Trait->MaxStacks)
	{
		StackCount++;
	}
}

bool FRelationshipTraitInstance::IsEventOnCooldown(FGameplayTag EventTag, float CurrentGameTime) const
{
	if (const float* NextAvailableTime = EventCooldowns.Find(EventTag))
	{
		return CurrentGameTime < *NextAvailableTime;
	}
	return false;
}

void FRelationshipTraitInstance::SetEventCooldown(FGameplayTag EventTag, float Cooldown, float CurrentGameTime)
{
	EventCooldowns.Add(EventTag, CurrentGameTime + Cooldown);
}
