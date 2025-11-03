// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/RelationshipDataAsset.h"
#include "DataAssets/RelationshipDimension.h"
#include "GameplayTagContainer.h"
#include "RelationshipProfile.generated.h"

/**
 * Defines a complete relationship configuration for an actor
 * Combines dimensions, initial states, traits, and behavior
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API URelationshipProfile : public URelationshipDataAsset
{
	GENERATED_BODY()

public:
	URelationshipProfile();

	/** Initial dimension values */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile")
	TMap<FGameplayTag, float> InitialDimensionValues;

	/** Initial relationship state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile", meta = (Categories = "Relationship.State"))
	FGameplayTag InitialState;

	/** Traits automatically applied to this profile */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile", meta = (Categories = "Relationship.Trait"))
	FGameplayTagContainer InitialTraits;

	/** Actions available by default (in addition to state-specific actions) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile", meta = (Categories = "Relationship.Action"))
	FGameplayTagContainer AvailableActions;

	/** Dimension decay overrides (empty = use dimension defaults) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile|Advanced")
	TMap<FGameplayTag, float> DecayRateOverrides;

	/** Global dimension modifiers (applied to all interactions) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile|Advanced")
	TMap<FGameplayTag, float> DimensionModifiers;

	/** Can this actor be a relationship subject? (initiate relationships) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile")
	bool bCanBeSubject;

	/** Can this actor be a relationship target? (receive relationships) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile")
	bool bCanBeTarget;

	/** Should this profile's relationships persist across level loads? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profile|Advanced")
	bool bPersistRelationships;

	/** Get initial value for dimension (returns dimension default if not overridden) */
	UFUNCTION(BlueprintCallable, Category = "Profile")
	float GetInitialDimensionValue(FGameplayTag DimensionTag, const class URelationshipDatabase* Database) const;

	/** Get decay rate override for dimension (returns dimension default if not overridden) */
	UFUNCTION(BlueprintCallable, Category = "Profile")
	float GetDecayRateOverride(FGameplayTag DimensionTag, const class URelationshipDatabase* Database) const;

	/** Get dimension modifier (0.0 if not set) */
	UFUNCTION(BlueprintCallable, Category = "Profile")
	float GetDimensionModifier(FGameplayTag DimensionTag) const;

	/** Check if trait is initially active */
	UFUNCTION(BlueprintCallable, Category = "Profile")
	bool HasInitialTrait(FGameplayTag TraitTag) const;

	/** Initialize dimension values from this profile */
	UFUNCTION(BlueprintCallable, Category = "Profile")
	TMap<FGameplayTag, FRelationshipDimensionValue> CreateInitialDimensions(const class URelationshipDatabase* Database, float CurrentGameTime) const;

protected:
	virtual bool ValidateInternal(TArray<FText>& OutErrors) const override;
};
