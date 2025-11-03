// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RelationshipDatabase.generated.h"

class URelationshipDimension;
class URelationshipState;
class URelationshipTrait;
class URelationshipRule;
class URelationshipAction;
class URelationshipProfile;

/**
 * Central database containing all relationship system configuration
 * This is the master Data Asset referenced in Project Settings
 * Provides indexed access to all relationship elements
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API URelationshipDatabase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	URelationshipDatabase();

	/** All available dimensions (Trust, Friendship, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Database")
	TArray<TSoftObjectPtr<URelationshipDimension>> Dimensions;

	/** All available relationship states (Stranger, Friend, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Database")
	TArray<TSoftObjectPtr<URelationshipState>> States;

	/** All available traits (Forgiving, Suspicious, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Database")
	TArray<TSoftObjectPtr<URelationshipTrait>> Traits;

	/** All available rules (conditions and formulas) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Database")
	TArray<TSoftObjectPtr<URelationshipRule>> Rules;

	/** All available actions (Talk, Gift, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Database")
	TArray<TSoftObjectPtr<URelationshipAction>> Actions;

	/** All available actor profiles */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Database")
	TArray<TSoftObjectPtr<URelationshipProfile>> Profiles;

	/** Default profile for actors without explicit configuration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Database")
	TSoftObjectPtr<URelationshipProfile> DefaultProfile;

	/** Build lookup indices for fast access (called at startup and hot-reload) */
	UFUNCTION(BlueprintCallable, Category = "Database")
	void BuildIndices();

	/** Clear all indices */
	UFUNCTION(BlueprintCallable, Category = "Database")
	void ClearIndices();

	/** Find dimension by tag (O(1) lookup) */
	UFUNCTION(BlueprintCallable, Category = "Database")
	URelationshipDimension* FindDimension(FGameplayTag Tag) const;

	/** Find state by tag */
	UFUNCTION(BlueprintCallable, Category = "Database")
	URelationshipState* FindState(FGameplayTag Tag) const;

	/** Find trait by tag */
	UFUNCTION(BlueprintCallable, Category = "Database")
	URelationshipTrait* FindTrait(FGameplayTag Tag) const;

	/** Find rule by tag */
	UFUNCTION(BlueprintCallable, Category = "Database")
	URelationshipRule* FindRule(FGameplayTag Tag) const;

	/** Find action by tag */
	UFUNCTION(BlueprintCallable, Category = "Database")
	URelationshipAction* FindAction(FGameplayTag Tag) const;

	/** Find profile by tag */
	UFUNCTION(BlueprintCallable, Category = "Database")
	URelationshipProfile* FindProfile(FGameplayTag Tag) const;

	/** Validate entire database */
	UFUNCTION(BlueprintCallable, Category = "Validation")
	bool ValidateDatabase(TArray<FText>& OutErrors) const;

	/** Get all dimensions (loaded) */
	UFUNCTION(BlueprintCallable, Category = "Database")
	TArray<URelationshipDimension*> GetAllDimensions() const;

	/** Get all states (loaded) */
	UFUNCTION(BlueprintCallable, Category = "Database")
	TArray<URelationshipState*> GetAllStates() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:
	/** Fast lookup maps (built from arrays) */
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<URelationshipDimension>> DimensionMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<URelationshipState>> StateMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<URelationshipTrait>> TraitMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<URelationshipRule>> RuleMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<URelationshipAction>> ActionMap;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<URelationshipProfile>> ProfileMap;

	/** Load and index specific asset type */
	template<typename T>
	void BuildIndexForType(const TArray<TSoftObjectPtr<T>>& Assets, TMap<FGameplayTag, TObjectPtr<T>>& OutMap);
};
