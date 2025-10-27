// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Core/NeedsTypes.h"
#include "NeedsInterface.generated.h"

// ========================================
// MANAGER INTERFACE
// Центральный интерфейс для управления системой потребностей (реализуется Subsystem)
// ========================================

UINTERFACE(MinimalAPI, Blueprintable)
class UNeedsManager : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for the central Needs Manager (implemented by Subsystem)
 * Provides batch update orchestration and global state management
 */
class NEEDSCORE_API INeedsManager
{
	GENERATED_BODY()

public:
	/** Get current game time in hours (для интеграции с Time System через события) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Manager")
	float GetCurrentGameTimeHours() const;

	/** Register a needs component for batch updates */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Manager")
	void RegisterNeedsComponent(UActorComponent* Component);

	/** Unregister a needs component */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Manager")
	void UnregisterNeedsComponent(UActorComponent* Component);

	/** Get camera location for LOD calculations */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Manager")
	FVector GetCameraLocation() const;
};

// ========================================
// LISTENER INTERFACE
// Интерфейс для объектов, которые хотят слушать события потребностей
// ========================================

UINTERFACE(MinimalAPI, Blueprintable)
class UNeedsListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that want to listen to needs events
 * Can be implemented by Actors, Components, Widgets, etc.
 */
class NEEDSCORE_API INeedsListener
{
	GENERATED_BODY()

public:
	/** Called when a need value changes */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Events")
	void OnNeedValueChanged(const FNeedHandle& NeedHandle, float OldValue, float NewValue);

	/** Called when a need crosses a threshold */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Events")
	void OnNeedThresholdChanged(const FNeedHandle& NeedHandle, ENeedThresholdLevel OldLevel, ENeedThresholdLevel NewLevel);

	/** Called when mood changes significantly */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Events")
	void OnMoodChanged(float OldMood, float NewMood);

	/** Called when an effect is applied */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Events")
	void OnEffectApplied(const FActiveNeedEffect& Effect);

	/** Called when an effect expires */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Events")
	void OnEffectExpired(const FActiveNeedEffect& Effect);

	/** Called when a modifier is added */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Events")
	void OnModifierAdded(const FActiveNeedModifier& Modifier);

	/** Called when a modifier is removed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs Events")
	void OnModifierRemoved(const FActiveNeedModifier& Modifier);
};

// ========================================
// NEEDS ACCESSOR INTERFACE
// Интерфейс для доступа к потребностям (реализуется Component)
// ========================================

UINTERFACE(MinimalAPI, Blueprintable)
class UNeedsAccessor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for accessing needs data (implemented by Component)
 * Provides read-only access to current state
 */
class NEEDSCORE_API INeedsAccessor
{
	GENERATED_BODY()

public:
	/** Get need value by tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	float GetNeedValue(const FGameplayTag& NeedTag) const;

	/** Get need handle by tag */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	FNeedHandle GetNeedHandle(const FGameplayTag& NeedTag) const;

	/** Get all active needs */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	TArray<FNeedHandle> GetAllNeedHandles() const;

	/** Get current mood */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	float GetCurrentMood() const;

	/** Check if need exists */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	bool HasNeed(const FGameplayTag& NeedTag) const;

	/** Get need threshold level */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	ENeedThresholdLevel GetNeedThreshold(const FGameplayTag& NeedTag) const;

	/** Is need suppressed */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	bool IsNeedSuppressed(const FGameplayTag& NeedTag) const;

	/** Is need frozen */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	bool IsNeedFrozen(const FGameplayTag& NeedTag) const;
};

// ========================================
// NEEDS MUTATOR INTERFACE
// Интерфейс для изменения потребностей (реализуется Component)
// ========================================

UINTERFACE(MinimalAPI, Blueprintable)
class UNeedsMutator : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for modifying needs (implemented by Component)
 * Provides write access and effect application
 */
class NEEDSCORE_API INeedsMutator
{
	GENERATED_BODY()

public:
	/** Set need value directly */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void SetNeedValue(const FGameplayTag& NeedTag, float Value);

	/** Modify need value by delta */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void ModifyNeedValue(const FGameplayTag& NeedTag, float Delta);

	/** Apply effect to needs */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void ApplyEffect(UNeedEffectAsset* Effect, int32 StackCount = 1);

	/** Add modifier */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void AddModifier(UNeedModifierAsset* Modifier);

	/** Remove modifier by instance ID */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void RemoveModifier(int32 InstanceID);

	/** Suppress need (stop decay) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void SuppressNeed(const FGameplayTag& NeedTag, bool bSuppress);

	/** Freeze need (no changes at all) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void FreezeNeed(const FGameplayTag& NeedTag, bool bFreeze);

	/** Register listener for events */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void RegisterListener(const TScriptInterface<INeedsListener>& Listener);

	/** Unregister listener */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Needs")
	void UnregisterListener(const TScriptInterface<INeedsListener>& Listener);
};
