// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataAssets/RelationshipDimension.h"
#include "DataAssets/RelationshipState.h"
#include "DataAssets/RelationshipTrait.h"
#include "DataAssets/RelationshipAction.h"
#include "GameplayTagContainer.h"
#include "RelationshipSubsystem.generated.h"

// Forward declarations
class URelationshipDatabase;
class URelationshipComponent;
class URelationshipProfile;
struct FGameEventPayload;

/**
 * Complete relationship data between two actors
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipData
{
	GENERATED_BODY()

	/** Subject actor (who has the relationship) */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship")
	TWeakObjectPtr<AActor> Subject;

	/** Target actor (relationship towards) */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship")
	TWeakObjectPtr<AActor> Target;

	/** Current dimension values */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship")
	TMap<FGameplayTag, FRelationshipDimensionValue> Dimensions;

	/** Current state */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship")
	FRelationshipStateInstance State;

	/** Active traits */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship")
	TArray<FRelationshipTraitInstance> Traits;

	/** Action usage tracking */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship")
	TMap<FGameplayTag, FRelationshipActionInstance> Actions;

	/** Last update time (for decay calculations) */
	UPROPERTY(BlueprintReadOnly, Category = "Relationship")
	float LastUpdateTime;

	FRelationshipData()
		: LastUpdateTime(0.0f)
	{
	}

	/** Check if relationship is valid (actors still exist) */
	bool IsValid() const
	{
		return Subject.IsValid() && Target.IsValid();
	}

	/** Get dimension value (0.0 if not found) */
	float GetDimensionValue(FGameplayTag DimensionTag) const;

	/** Get trait instance (nullptr if not found) */
	const FRelationshipTraitInstance* FindTrait(FGameplayTag TraitTag) const;

	/** Check if trait is active */
	bool HasTrait(FGameplayTag TraitTag) const;
};

/**
 * Unique key for relationship lookup
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipKey
{
	GENERATED_BODY()

	/** Subject actor */
	UPROPERTY()
	TWeakObjectPtr<AActor> Subject;

	/** Target actor */
	UPROPERTY()
	TWeakObjectPtr<AActor> Target;

	FRelationshipKey()
	{
	}

	FRelationshipKey(AActor* InSubject, AActor* InTarget)
		: Subject(InSubject)
		, Target(InTarget)
	{
	}

	bool operator==(const FRelationshipKey& Other) const
	{
		return Subject == Other.Subject && Target == Other.Target;
	}

	bool IsValid() const
	{
		return Subject.IsValid() && Target.IsValid();
	}

	friend uint32 GetTypeHash(const FRelationshipKey& Key)
	{
		return HashCombine(GetTypeHash(Key.Subject), GetTypeHash(Key.Target));
	}
};

/**
 * Event delegates for relationship changes
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDimensionChanged, AActor*, Subject, AActor*, Target, FGameplayTag, DimensionTag, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnStateTransitioned, AActor*, Subject, AActor*, Target, FGameplayTag, OldState, FGameplayTag, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTraitAdded, AActor*, Subject, AActor*, Target, FGameplayTag, TraitTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTraitRemoved, AActor*, Subject, AActor*, Target, FGameplayTag, TraitTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActionExecuted, AActor*, Subject, AActor*, Target, FGameplayTag, ActionTag);

/**
 * Global subsystem managing all relationships in the game
 * Provides O(1) lookups, event-driven updates, and centralized management
 */
UCLASS()
class RELATIONSHIPSYSTEMCORE_API URelationshipSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	URelationshipSubsystem();

	/** USubsystem implementation */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ==================== Component Registration ====================

	/** Register component (called automatically on BeginPlay) */
	UFUNCTION(BlueprintCallable, Category = "Relationships")
	void RegisterComponent(UNPCRelationshipComponent* Component);

	/** Unregister component (called automatically on EndPlay) */
	UFUNCTION(BlueprintCallable, Category = "Relationships")
	void UnregisterComponent(UNPCRelationshipComponent* Component);

	// ==================== Relationship Management ====================

	/** Create or get existing relationship */
	UFUNCTION(BlueprintCallable, Category = "Relationships")
	bool CreateRelationship(AActor* Subject, AActor* Target, URelationshipProfile* SubjectProfile, URelationshipProfile* TargetProfile);

	/** Remove relationship */
	UFUNCTION(BlueprintCallable, Category = "Relationships")
	bool RemoveRelationship(AActor* Subject, AActor* Target);

	/** Check if relationship exists */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships")
	bool HasRelationship(AActor* Subject, AActor* Target) const;

	/** Get relationship data (read-only) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships")
	bool GetRelationshipData(AActor* Subject, AActor* Target, FRelationshipData& OutData) const;

	// ==================== Dimension Operations ====================

	/** Get dimension value */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships|Dimensions")
	float GetDimensionValue(AActor* Subject, AActor* Target, FGameplayTag DimensionTag) const;

	/** Set dimension value (absolute) */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Dimensions")
	bool SetDimensionValue(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float NewValue);

	/** Modify dimension value (relative) */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Dimensions")
	bool ModifyDimensionValue(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float Delta);

	/** Batch modify multiple dimensions at once */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Dimensions")
	bool ModifyDimensionValues(AActor* Subject, AActor* Target, const TMap<FGameplayTag, float>& DimensionDeltas);

	// ==================== State Operations ====================

	/** Get current state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships|State")
	FGameplayTag GetCurrentState(AActor* Subject, AActor* Target) const;

	/** Force state change (bypasses transitions) */
	UFUNCTION(BlueprintCallable, Category = "Relationships|State")
	bool ForceStateChange(AActor* Subject, AActor* Target, FGameplayTag NewState);

	/** Evaluate state transitions (called internally after dimension changes) */
	void EvaluateStateTransitions(AActor* Subject, AActor* Target);

	// ==================== Trait Operations ====================

	/** Add trait to relationship */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Traits")
	bool AddTrait(AActor* Subject, AActor* Target, FGameplayTag TraitTag);

	/** Remove trait from relationship */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Traits")
	bool RemoveTrait(AActor* Subject, AActor* Target, FGameplayTag TraitTag);

	/** Check if relationship has trait */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships|Traits")
	bool HasTrait(AActor* Subject, AActor* Target, FGameplayTag TraitTag) const;

	/** Get trait stack count */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships|Traits")
	int32 GetTraitStackCount(AActor* Subject, AActor* Target, FGameplayTag TraitTag) const;

	// ==================== Action Execution ====================

	/** Check if action can be executed */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships|Actions")
	bool CanExecuteAction(AActor* Subject, AActor* Target, FGameplayTag ActionTag) const;

	/** Execute action */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Actions")
	bool ExecuteAction(AActor* Subject, AActor* Target, FGameplayTag ActionTag);

	/** Get available actions for relationship in current state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships|Actions")
	TArray<FGameplayTag> GetAvailableActions(AActor* Subject, AActor* Target) const;

	// ==================== Queries ====================

	/** Get all relationships where actor is subject */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Query")
	TArray<FRelationshipData> GetRelationshipsAsSubject(AActor* Subject) const;

	/** Get all relationships where actor is target */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Query")
	TArray<FRelationshipData> GetRelationshipsAsTarget(AActor* Target) const;

	/** Find relationships matching state */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Query")
	TArray<FRelationshipData> FindRelationshipsByState(AActor* Subject, FGameplayTag StateTag) const;

	/** Find relationships with dimension in range */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Query")
	TArray<FRelationshipData> FindRelationshipsByDimensionRange(AActor* Subject, FGameplayTag DimensionTag, float MinValue, float MaxValue) const;

	// ==================== Decay System ====================

	/** Process decay for all relationships (call periodically, not every frame) */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Decay")
	void ProcessDecay(float DeltaHours);

	/** Process decay for specific relationship */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Decay")
	void ProcessRelationshipDecay(AActor* Subject, AActor* Target, float DeltaHours);

	// ==================== Database Access ====================

	/** Get database (from project settings or override) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships")
	URelationshipDatabase* GetDatabase() const;

	/** Set database override (temporary, not saved) */
	UFUNCTION(BlueprintCallable, Category = "Relationships")
	void SetDatabaseOverride(URelationshipDatabase* NewDatabase);

	// ==================== Events ====================

	UPROPERTY(BlueprintAssignable, Category = "Relationships|Events")
	FOnDimensionChanged OnDimensionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Relationships|Events")
	FOnStateTransitioned OnStateTransitioned;

	UPROPERTY(BlueprintAssignable, Category = "Relationships|Events")
	FOnTraitAdded OnTraitAdded;

	UPROPERTY(BlueprintAssignable, Category = "Relationships|Events")
	FOnTraitRemoved OnTraitRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Relationships|Events")
	FOnActionExecuted OnActionExecuted;

	// ==================== Debug & Utilities ====================

	/** Get total number of active relationships */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships|Debug")
	int32 GetRelationshipCount() const;

	/** Clear all relationships (use with caution!) */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Debug")
	void ClearAllRelationships();

	/** Validate all relationships and log issues */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Debug")
	void ValidateAllRelationships();

private:
	/** All active relationships (indexed by subject-target pair) */
	UPROPERTY()
	TMap<FRelationshipKey, FRelationshipData> Relationships;

	/** Registered components (for reverse lookups) */
	UPROPERTY()
	TArray<TWeakObjectPtr<UNPCRelationshipComponent>> RegisteredComponents;

	/** Database override (temporary) */
	UPROPERTY(Transient)
	TObjectPtr<URelationshipDatabase> DatabaseOverride;

	/** Delegate handles for time event subscriptions */
	FDelegateHandle TimeEventHourChangedHandle;
	FDelegateHandle TimeEventDayChangedHandle;

	/** Get or create relationship data */
	FRelationshipData* GetOrCreateRelationship(AActor* Subject, AActor* Target);

	/** Get relationship data (mutable) */
	FRelationshipData* GetRelationshipMutable(AActor* Subject, AActor* Target);

	/** Get relationship data (const) */
	const FRelationshipData* GetRelationship(AActor* Subject, AActor* Target) const;

	/** Apply dimension change with trait modifiers */
	float ApplyDimensionChangeWithModifiers(FRelationshipData* RelData, FGameplayTag DimensionTag, float BaseDelta, FGameplayTag ActionTag = FGameplayTag());

	/** Evaluate state transition with hysteresis */
	void EvaluateTransitionsWithHysteresis(FRelationshipData* RelData, float DeltaTime);

	/** Broadcast events to external systems */
	void BroadcastDimensionChanged(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float NewValue);
	void BroadcastStateTransitioned(AActor* Subject, AActor* Target, FGameplayTag OldState, FGameplayTag NewState);
	void BroadcastTraitAdded(AActor* Subject, AActor* Target, FGameplayTag TraitTag);
	void BroadcastTraitRemoved(AActor* Subject, AActor* Target, FGameplayTag TraitTag);
	void BroadcastActionExecuted(AActor* Subject, AActor* Target, FGameplayTag ActionTag);

	/** Get current game time in seconds */
	float GetCurrentGameTime() const;

	/** Check if GameEventBus is available */
	bool IsGameEventBusAvailable() const;

	/** Send event to GameEventBus if available */
	void SendGameEvent(FGameplayTag EventTag, AActor* Subject, AActor* Target, const TMap<FName, FString>& Payload);

	//~ Begin Time System Integration

	/** Subscribe to time events from GameEventBus */
	void SubscribeToTimeEvents();

	/** Unsubscribe from time events */
	void UnsubscribeFromTimeEvents();

	/** Called when game hour changes */
	void OnGameTimeHourChanged(const FGameEventPayload& Payload);

	/** Called when game day changes */
	void OnGameTimeDayChanged(const FGameEventPayload& Payload);

	/** Daily relationship maintenance (expire traits, validate state) */
	void ProcessDailyRelationshipMaintenance();

	/** Validate and cleanup invalid relationships */
	void ValidateAndCleanupRelationships();

	//~ End Time System Integration
};
