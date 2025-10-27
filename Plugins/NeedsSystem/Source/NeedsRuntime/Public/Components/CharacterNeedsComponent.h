// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Core/NeedsTypes.h"
#include "Core/NeedsInterface.h"
#include "Data/NeedDefinitionAsset.h"
#include "Data/NeedEffectAsset.h"
#include "Data/NeedModifierAsset.h"
#include "CharacterNeedsComponent.generated.h"

// Forward declarations
class UNeedsSubsystem;

// ========================================
// DELEGATES
// ========================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNeedValueChanged, FGameplayTag, NeedTag, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNeedThresholdChanged, FGameplayTag, NeedTag, ENeedThresholdLevel, OldLevel, ENeedThresholdLevel, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMoodChanged, float, OldMood, float, NewMood);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectApplied, FActiveNeedEffect, Effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEffectExpired, FActiveNeedEffect, Effect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModifierAdded, FActiveNeedModifier, Modifier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModifierRemoved, FActiveNeedModifier, Modifier);

/**
 * Internal need data (runtime state + definition)
 */
USTRUCT()
struct FNeedData
{
	GENERATED_BODY()

	/** Need definition */
	UPROPERTY()
	TObjectPtr<UNeedDefinitionAsset> Definition;

	/** Runtime state */
	UPROPERTY()
	FNeedRuntimeState State;

	FNeedData()
		: Definition(nullptr)
	{
	}
};

/**
 * Character Needs Component
 * 
 * Attach this component to any Actor to give it needs simulation.
 * Completely data-driven, event-driven, no Tick().
 * 
 * Features:
 * - Data-driven need definitions
 * - Decay/recovery simulation
 * - Mood calculation
 * - Effects and modifiers
 * - LOD support
 * - Event-driven architecture
 * - Handle-based API
 */
UCLASS(ClassGroup=(Gameplay), meta=(BlueprintSpawnableComponent), Blueprintable)
class NEEDSRUNTIME_API UCharacterNeedsComponent : public UActorComponent, public INeedsAccessor, public INeedsMutator
{
	GENERATED_BODY()

public:
	UCharacterNeedsComponent();

	// ========================================
	// LIFECYCLE
	// ========================================

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ========================================
	// CONFIGURATION
	// ========================================

	/** Override needs (if empty, uses settings from subsystem) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs|Configuration")
	TArray<TObjectPtr<UNeedDefinitionAsset>> OverrideNeeds;

	/** Use custom LOD distances (if false, uses global settings) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs|LOD")
	bool bUseCustomLOD = false;

	/** Custom LOD configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs|LOD", meta = (EditCondition = "bUseCustomLOD"))
	FNeedLODConfig CustomLODConfig;

	// ========================================
	// INeedsAccessor INTERFACE
	// ========================================

	virtual float GetNeedValue_Implementation(const FGameplayTag& NeedTag) const override;
	virtual FNeedHandle GetNeedHandle_Implementation(const FGameplayTag& NeedTag) const override;
	virtual TArray<FNeedHandle> GetAllNeedHandles_Implementation() const override;
	virtual float GetCurrentMood_Implementation() const override;
	virtual bool HasNeed_Implementation(const FGameplayTag& NeedTag) const override;
	virtual ENeedThresholdLevel GetNeedThreshold_Implementation(const FGameplayTag& NeedTag) const override;
	virtual bool IsNeedSuppressed_Implementation(const FGameplayTag& NeedTag) const override;
	virtual bool IsNeedFrozen_Implementation(const FGameplayTag& NeedTag) const override;

	// ========================================
	// INeedsMutator INTERFACE
	// ========================================

	virtual void SetNeedValue_Implementation(const FGameplayTag& NeedTag, float Value) override;
	virtual void ModifyNeedValue_Implementation(const FGameplayTag& NeedTag, float Delta) override;
	virtual void ApplyEffect_Implementation(UNeedEffectAsset* Effect, int32 StackCount) override;
	virtual void AddModifier_Implementation(UNeedModifierAsset* Modifier) override;
	virtual void RemoveModifier_Implementation(int32 InstanceID) override;
	virtual void SuppressNeed_Implementation(const FGameplayTag& NeedTag, bool bSuppress) override;
	virtual void FreezeNeed_Implementation(const FGameplayTag& NeedTag, bool bFreeze) override;
	virtual void RegisterListener_Implementation(const TScriptInterface<INeedsListener>& Listener) override;
	virtual void UnregisterListener_Implementation(const TScriptInterface<INeedsListener>& Listener) override;

	// ========================================
	// BLUEPRINT API
	// ========================================

	/** Get need value - Blueprint friendly */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DisplayName = "Get Need Value"))
	float BP_GetNeedValue(const FGameplayTag& NeedTag) const;

	/** Set need value - Blueprint friendly */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DisplayName = "Set Need Value"))
	void BP_SetNeedValue(const FGameplayTag& NeedTag, float Value);

	/** Modify need value - Blueprint friendly */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DisplayName = "Modify Need Value"))
	void BP_ModifyNeedValue(const FGameplayTag& NeedTag, float Delta);

	/** Apply effect - Blueprint friendly */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DisplayName = "Apply Effect"))
	void BP_ApplyEffect(UNeedEffectAsset* Effect, int32 StackCount = 1);

	/** Add modifier - Blueprint friendly */
	UFUNCTION(BlueprintCallable, Category = "Needs", meta = (DisplayName = "Add Modifier"))
	void BP_AddModifier(UNeedModifierAsset* Modifier);

	/** Get current mood - Blueprint friendly */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs", meta = (DisplayName = "Get Current Mood"))
	float BP_GetCurrentMood() const;

	/** Get all need tags */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs")
	TArray<FGameplayTag> GetAllNeedTags() const;

	/** Get gameplay tags (дл€ интеграции с другими системами) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Needs")
	FGameplayTagContainer GetGameplayTags() const { return GameplayTags; }

	// ========================================
	// EVENTS (MULTICAST DELEGATES)
	// ========================================

	UPROPERTY(BlueprintAssignable, Category = "Needs|Events")
	FOnNeedValueChanged OnNeedValueChanged;

	UPROPERTY(BlueprintAssignable, Category = "Needs|Events")
	FOnNeedThresholdChanged OnNeedThresholdChanged;

	UPROPERTY(BlueprintAssignable, Category = "Needs|Events")
	FOnMoodChanged OnMoodChanged;

	UPROPERTY(BlueprintAssignable, Category = "Needs|Events")
	FOnEffectApplied OnEffectApplied;

	UPROPERTY(BlueprintAssignable, Category = "Needs|Events")
	FOnEffectExpired OnEffectExpired;

	UPROPERTY(BlueprintAssignable, Category = "Needs|Events")
	FOnModifierAdded OnModifierAdded;

	UPROPERTY(BlueprintAssignable, Category = "Needs|Events")
	FOnModifierRemoved OnModifierRemoved;

	// ========================================
	// INTERNAL UPDATE (вызываетс€ Subsystem)
	// ========================================

	/** Update needs simulation (called by subsystem) */
	void UpdateNeeds(float DeltaGameHours, ENeedLODLevel CurrentLOD);

	/** Get current LOD level */
	ENeedLODLevel GetCurrentLOD() const { return CurrentLODLevel; }

	/** Get unique component ID */
	int32 GetComponentID() const { return ComponentID; }

	/** Get LOD config (public access for subsystem) */
	const FNeedLODConfig& GetLODConfig() const;

protected:
	// ========================================
	// INTERNAL STATE
	// ========================================

	/** Map of needs (Tag -> Data) */
	UPROPERTY()
	TMap<FGameplayTag, FNeedData> Needs;

	/** Current mood state */
	UPROPERTY()
	FMoodState MoodState;

	/** Active effects */
	UPROPERTY()
	TArray<FActiveNeedEffect> ActiveEffects;

	/** Active modifiers */
	UPROPERTY()
	TArray<FActiveNeedModifier> ActiveModifiers;

	/** Registered listeners */
	UPROPERTY()
	TArray<TScriptInterface<INeedsListener>> RegisteredListeners;

	/** Current gameplay tags (from effects/modifiers) */
	UPROPERTY()
	FGameplayTagContainer GameplayTags;

	/** Current LOD level */
	UPROPERTY()
	ENeedLODLevel CurrentLODLevel = ENeedLODLevel::Near;

	/** Unique component ID */
	UPROPERTY()
	int32 ComponentID = 0;

	/** Reference to subsystem */
	UPROPERTY()
	TObjectPtr<UNeedsSubsystem> CachedSubsystem;

	/** Next instance ID for effects/modifiers */
	int32 NextInstanceID = 1;

	// ========================================
	// INTERNAL METHODS
	// ========================================

	/** Initialize needs from definitions */
	void InitializeNeeds();

	/** Get need data (const) */
	const FNeedData* GetNeedData(const FGameplayTag& NeedTag) const;

	/** Get need data (mutable) */
	FNeedData* GetNeedDataMutable(const FGameplayTag& NeedTag);

	/** Update single need */
	void UpdateNeed(const FGameplayTag& NeedTag, FNeedData& NeedData, float DeltaGameHours);

	/** Apply decay to need */
	void ApplyDecay(FNeedData& NeedData, float DeltaGameHours);

	/** Update mood */
	void UpdateMood();

	/** Update active effects */
	void UpdateEffects(float DeltaGameHours);

	/** Update modifiers (check conditions) */
	void UpdateModifiers();

	/** Update gameplay tags */
	void UpdateGameplayTags();

	/** Calculate modified decay rate for a need */
	float CalculateModifiedDecayRate(const FGameplayTag& NeedTag, float BaseDecayRate) const;

	/** Calculate modified effect power for a need */
	float CalculateModifiedEffectPower(const FGameplayTag& NeedTag, float BasePower) const;

	/** Check threshold and broadcast if changed */
	void CheckThreshold(const FGameplayTag& NeedTag, FNeedData& NeedData);

	/** Notify listeners */
	void NotifyNeedValueChanged(const FGameplayTag& NeedTag, float OldValue, float NewValue);
	void NotifyNeedThresholdChanged(const FGameplayTag& NeedTag, ENeedThresholdLevel OldLevel, ENeedThresholdLevel NewLevel);
	void NotifyMoodChanged(float OldMood, float NewMood);
	void NotifyEffectApplied(const FActiveNeedEffect& Effect);
	void NotifyEffectExpired(const FActiveNeedEffect& Effect);
	void NotifyModifierAdded(const FActiveNeedModifier& Modifier);
	void NotifyModifierRemoved(const FActiveNeedModifier& Modifier);

	/** Get subsystem */
	UNeedsSubsystem* GetSubsystem() const;

	/** Get current hour (for modifier conditions) */
	int32 GetCurrentHour() const;
};
