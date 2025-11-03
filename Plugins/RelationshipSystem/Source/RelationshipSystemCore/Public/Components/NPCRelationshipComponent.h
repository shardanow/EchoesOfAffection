// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NPCRelationshipComponent.generated.h"

class URelationshipProfile;
class URelationshipSubsystem;
struct FRelationshipData;

/**
 * Component for actors participating in the relationship system
 * Attach to any actor (Player, NPC, Faction representative) to enable relationships
 * Automatically registers with RelationshipSubsystem on BeginPlay
 */
UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent), Blueprintable)
class RELATIONSHIPSYSTEMCORE_API UNPCRelationshipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNPCRelationshipComponent();

	/** UActorComponent interface */
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ==================== Configuration ====================

	/** Relationship profile for this actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	TSoftObjectPtr<URelationshipProfile> Profile;

	/** Auto-create relationships with nearby actors on BeginPlay? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
	bool bAutoCreateRelationships;

	/** Auto-create radius (if bAutoCreateRelationships is true) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship",
		meta = (EditCondition = "bAutoCreateRelationships", ClampMin = "0.0"))
	float AutoCreateRadius;

	/** Actor tags to filter auto-creation (empty = all actors with RelationshipComponent) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship",
		meta = (EditCondition = "bAutoCreateRelationships"))
	TArray<FName> AutoCreateFilterTags;

	// ==================== Relationship Management ====================

	/** Create relationship with target actor */
	UFUNCTION(BlueprintCallable, Category = "Relationship")
	bool CreateRelationshipWith(AActor* TargetActor);

	/** Remove relationship with target actor */
	UFUNCTION(BlueprintCallable, Category = "Relationship")
	bool RemoveRelationshipWith(AActor* TargetActor);

	/** Check if has relationship with target */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship")
	bool HasRelationshipWith(AActor* TargetActor) const;

	// ==================== Dimension Operations ====================

	/** Get dimension value towards target */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship|Dimensions")
	float GetDimensionValue(AActor* TargetActor, FGameplayTag DimensionTag) const;

	/** Set dimension value towards target */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Dimensions")
	bool SetDimensionValue(AActor* TargetActor, FGameplayTag DimensionTag, float NewValue);

	/** Modify dimension value towards target */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Dimensions")
	bool ModifyDimensionValue(AActor* TargetActor, FGameplayTag DimensionTag, float Delta);

	/** Batch modify multiple dimensions */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Dimensions")
	bool ModifyDimensionValues(AActor* TargetActor, const TMap<FGameplayTag, float>& DimensionDeltas);

	// ==================== State Operations ====================

	/** Get current state towards target */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship|State")
	FGameplayTag GetCurrentState(AActor* TargetActor) const;

	/** Force state change towards target */
	UFUNCTION(BlueprintCallable, Category = "Relationship|State")
	bool ForceStateChange(AActor* TargetActor, FGameplayTag NewState);

	// ==================== Trait Operations ====================

	/** Add trait to relationship with target */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Traits")
	bool AddTrait(AActor* TargetActor, FGameplayTag TraitTag);

	/** Remove trait from relationship with target */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Traits")
	bool RemoveTrait(AActor* TargetActor, FGameplayTag TraitTag);

	/** Check if has trait towards target */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship|Traits")
	bool HasTrait(AActor* TargetActor, FGameplayTag TraitTag) const;

	// ==================== Action Execution ====================

	/** Check if can execute action towards target */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship|Actions")
	bool CanExecuteAction(AActor* TargetActor, FGameplayTag ActionTag) const;

	/** Execute action towards target */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Actions")
	bool ExecuteAction(AActor* TargetActor, FGameplayTag ActionTag);

	/** Get all available actions towards target */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship|Actions")
	TArray<FGameplayTag> GetAvailableActions(AActor* TargetActor) const;

	// ==================== Queries ====================

	/** Get all actors this actor has relationships with (as subject) */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Query")
	TArray<AActor*> GetAllRelationshipTargets() const;

	/** Get all actors that have relationships with this actor (as target) */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Query")
	TArray<AActor*> GetAllRelationshipSubjects() const;

	/** Find targets matching state */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Query")
	TArray<AActor*> FindTargetsByState(FGameplayTag StateTag) const;

	/** Find targets with dimension in range */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Query")
	TArray<AActor*> FindTargetsByDimensionRange(FGameplayTag DimensionTag, float MinValue, float MaxValue) const;

	/** Get best target by dimension (highest value) */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Query")
	AActor* GetBestTargetByDimension(FGameplayTag DimensionTag) const;

	/** Get worst target by dimension (lowest value) */
	UFUNCTION(BlueprintCallable, Category = "Relationship|Query")
	AActor* GetWorstTargetByDimension(FGameplayTag DimensionTag) const;

	// ==================== Utilities ====================

	/** Get loaded profile */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship")
	URelationshipProfile* GetProfile() const;

	/** Get relationship subsystem */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship")
	URelationshipSubsystem* GetRelationshipSubsystem() const;

	/** Can this component be a subject? */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship")
	bool CanBeSubject() const;

	/** Can this component be a target? */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship")
	bool CanBeTarget() const;

protected:
	/** Initialize relationships on begin play */
	void InitializeRelationships();

	/** Auto-create relationships with nearby actors */
	void AutoCreateNearbyRelationships();

	/** Cached profile (loaded on BeginPlay) */
	UPROPERTY(Transient)
	TObjectPtr<URelationshipProfile> CachedProfile;

	/** Cached subsystem reference */
	UPROPERTY(Transient)
	TObjectPtr<URelationshipSubsystem> CachedSubsystem;
};
