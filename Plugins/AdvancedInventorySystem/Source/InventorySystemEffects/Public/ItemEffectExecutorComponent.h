// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InventorySystemTypes.h"
#include "ItemEffectExecutorComponent.generated.h"

class UItemEffectDataAsset;
class UInventoryItem;
class UItemDataAsset;

/**
 * Active effect instance tracking
 */
USTRUCT(BlueprintType)
struct FActiveItemEffect
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UItemEffectDataAsset> EffectData;

	UPROPERTY()
	TObjectPtr<AActor> Target;

	UPROPERTY()
	FInventoryItemId SourceItemId;

	UPROPERTY()
	int32 RemainingExecutions = 0;

	UPROPERTY()
	float NextExecutionTime = 0.0f;

	UPROPERTY()
	FTimerHandle TimerHandle;
};

/**
 * Component that executes and manages item effects
 * Handles all effect triggers: OnAcquire, OnLose, OnUse, OnGift, WhileOwned, WhileEquipped
 */
UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEMEFFECTS_API UItemEffectExecutorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UItemEffectExecutorComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// ========== EFFECT EXECUTION ==========

	/**
	 * Execute item effects based on trigger type
	 * @param Item The item whose effects to execute
	 * @param TriggerType The trigger type
	 * @param Target Optional target actor (for NPC effects)
	 * @return True if any effects were executed
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Effects")
	bool ExecuteItemEffects(UInventoryItem* Item, EItemEffectTrigger TriggerType, AActor* Target = nullptr);

	/**
	 * Execute a single effect
	 * @param Effect The effect data to execute
	 * @param Item The source item
	 * @param Target The target actor
	 * @return True if the effect was executed
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Effects")
	bool ExecuteSingleEffect(UItemEffectDataAsset* Effect, UInventoryItem* Item, AActor* Target);

	/**
	 * Start a persistent effect (WhileOwned or WhileEquipped)
	 * @param Item The item with the persistent effect
	 * @param TriggerType The trigger type (should be WhileOwned or WhileEquipped)
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Effects")
	void StartPersistentEffects(UInventoryItem* Item, EItemEffectTrigger TriggerType);

	/**
	 * Stop persistent effects for an item
	 * @param ItemId The item ID whose persistent effects to stop
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Effects")
	void StopPersistentEffects(FInventoryItemId ItemId);

	/**
	 * Stop all persistent effects
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Effects")
	void StopAllPersistentEffects();

protected:
	// ========== ACTIVE EFFECTS ==========

	/** Currently active persistent effects */
	UPROPERTY()
	TArray<FActiveItemEffect> ActiveEffects;

	// ========== EFFECT EXECUTION HELPERS ==========

	/** Execute needs modifications */
	void ExecuteNeedsModifications(UItemEffectDataAsset* Effect, AActor* Target);

	/** Execute dialogue triggers */
	void ExecuteDialogueTriggers(UItemEffectDataAsset* Effect, AActor* Target);

	/** Execute tag modifications */
	void ExecuteTagModifications(UItemEffectDataAsset* Effect, AActor* Target);

	/** Execute custom effect */
	void ExecuteCustomEffect(UItemEffectDataAsset* Effect, AActor* Target, UInventoryItem* SourceItem);

	/** Get target actors for an effect */
	TArray<AActor*> GetEffectTargets(UItemEffectDataAsset* Effect, AActor* DefaultTarget);

	/** Timer callback for repeating effects */
	UFUNCTION()
	void ExecuteRepeatingEffect(int32 EffectIndex);
};
