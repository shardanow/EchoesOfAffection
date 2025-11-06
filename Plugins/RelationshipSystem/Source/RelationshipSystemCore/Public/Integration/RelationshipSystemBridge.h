// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"

// TEMPORARY: Bridge disabled - no generated.h needed
// #include "RelationshipSystemBridge.generated.h"

// Forward declarations
class URelationshipSubsystem;
class UMemoryToRelationshipMapping;

// TEMPORARY: Bridge commented out until GameEventBus integration is complete
// TODO: Uncomment when FGameEvent struct is available from GameEventBus plugin

#if 0 // Disabled for now

/**
 * Bridge for integration between systems via GameEventBus
 * 
 * Features:
 * 1. NPCMemory to RelationshipSystem (memory affects relationship)
 * 2. RelationshipSystem to NPCMemory (relationship creates memories)
 * 3. Data-driven configuration via UMemoryToRelationshipMapping
 * 
 * Soft dependency:
 * - Requires GameEventBus runtime
 * - Works with NPCMemoryComponent
 * - Configured via Data Assets
 */
UCLASS()
class RELATIONSHIPSYSTEMCORE_API URelationshipSystemBridge : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

protected:
	/** Is GameEventBus available */
	bool bGameEventBusAvailable = false;

	/** Cached relationship subsystem */
	UPROPERTY(Transient)
	TObjectPtr<URelationshipSubsystem> RelationshipSubsystem;

	/** Mapping configuration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Integration", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMemoryToRelationshipMapping> MappingConfig;

	/** Time since last sync */
	float TimeSinceLastSync = 0.0f;

protected:
	/** Setup event listeners */
	void SetupEventListeners();

	/** Cleanup event listeners */
	void CleanupEventListeners();

	//~ Begin GameEventBus Listeners

	/** Handle memory event */
	UFUNCTION()
	void OnMemoryEventReceived(const FGameEvent& Event);

	//~ End GameEventBus Listeners

	//~ Begin RelationshipSubsystem Listeners

	/** Handle dimension change */
	UFUNCTION()
	void OnDimensionChanged(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float NewValue);

	/** Handle state transition */
	UFUNCTION()
	void OnStateTransitioned(AActor* Subject, AActor* Target, FGameplayTag OldState, FGameplayTag NewState);

	/** Handle trait added */
	UFUNCTION()
	void OnTraitAdded(AActor* Subject, AActor* Target, FGameplayTag TraitTag);

	/** Handle action executed */
	UFUNCTION()
	void OnActionExecuted(AActor* Subject, AActor* Target, FGameplayTag ActionTag);

	//~ End RelationshipSubsystem Listeners

protected:
	//~ Begin Memory to Relationship Logic

	/**
	 * Convert memory to relationship change
	 * Uses MappingConfig for data-driven logic
	 */
	void ConvertMemoryToRelationshipChange(
		AActor* NPC, 
		AActor* Target,
		const FString& MemoryType, 
		float Importance,
		float Freshness,
		const FString& Emotion);

	/**
	 * Apply emotion modifiers to dimensions
	 */
	void ApplyEmotionModifiers(
		AActor* NPC,
		AActor* Target, 
		const FString& Emotion,
		float Importance);

	//~ End Memory to Relationship Logic

	//~ Begin Relationship to Memory Logic

	/**
	 * Create memory from relationship change
	 */
	void CreateMemoryFromRelationshipChange(
		AActor* NPC,
		AActor* Target,
		FGameplayTag DimensionTag,
		float NewValue);

	/**
	 * Create memory from state transition
	 */
	void CreateMemoryFromStateTransition(
		AActor* NPC,
		AActor* Target,
		FGameplayTag OldState,
		FGameplayTag NewState);

	/**
	 * Calculate memory importance based on relationship
	 */
	float CalculateMemoryImportance(FGameplayTag DimensionTag, float Value) const;

	/**
	 * Determine emotion for memory
	 */
	FString DetermineMemoryEmotion(FGameplayTag DimensionTag, float Value) const;

	/**
	 * Generate memory description
	 */
	FText GenerateMemoryDescription(
		FGameplayTag DimensionTag, 
		float Value,
		AActor* Target) const;

	//~ End Relationship to Memory Logic

	//~ Begin GameEventBus Integration

	/**
	 * Broadcast event to other systems
	 */
	void BroadcastRelationshipEvent(
		FGameplayTag EventTag,
		AActor* Subject,
		AActor* Target,
		const TMap<FName, FString>& Payload);

	//~ End GameEventBus Integration

public:
	/** Check if GameEventBus is available */
	UFUNCTION(BlueprintPure, Category = "Relationship System|Integration")
	bool IsGameEventBusAvailable() const;

	/** Set mapping config */
	UFUNCTION(BlueprintCallable, Category = "Relationship System|Integration")
	void SetMappingConfig(UMemoryToRelationshipMapping* NewConfig) { MappingConfig = NewConfig; }

	/** Get mapping config */
	UFUNCTION(BlueprintPure, Category = "Relationship System|Integration")
	UMemoryToRelationshipMapping* GetMappingConfig() const { return MappingConfig; }
};

#endif // Disabled Bridge
