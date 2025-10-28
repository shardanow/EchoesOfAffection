// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InventorySystemTypes.h"
#include "ItemEffectDataAsset.generated.h"

class UItemDataAsset;

/**
 * Condition for effect execution
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FItemEffectCondition
{
	GENERATED_BODY()

	/** Tags that must be present on the target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTagContainer RequiredTags;

	/** Tags that must NOT be present on the target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTagContainer BlockedTags;

	/** Minimum level requirement (0 = no requirement) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0"))
	int32 MinimumLevel = 0;

	/** Maximum level requirement (0 = no requirement) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0"))
	int32 MaximumLevel = 0;

	/** Custom condition check via Blueprint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName CustomConditionName;

	/** Evaluate if all conditions are met */
	bool EvaluateConditions(AActor* Target) const;
};

/**
 * Needs modification effect
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FNeedsModification
{
	GENERATED_BODY()

	/** The need to modify (e.g., "Needs.Hunger") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs")
	FGameplayTag NeedTag;

	/** Amount to modify the need by */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs")
	float ModificationAmount = 0.0f;

	/** Whether to clamp the value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs")
	bool bClampValue = true;
};

/**
 * Dialogue trigger effect
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FDialogueTrigger
{
	GENERATED_BODY()

	/** The dialogue to trigger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FGameplayTag DialogueTag;

	/** Whether to check dialogue conditions before triggering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	bool bCheckConditions = false;
};

/**
 * Tag modification effect
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FTagModification
{
	GENERATED_BODY()

	/** Tags to add to the target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer TagsToAdd;

	/** Tags to remove from the target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer TagsToRemove;

	/** Duration in seconds (0 = permanent) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags", meta = (ClampMin = "0"))
	float Duration = 0.0f;
};

/**
 * Custom effect parameters for Blueprint implementation
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FCustomEffectParameters
{
	GENERATED_BODY()

	/** String parameter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	FString StringParameter;

	/** Float parameter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	float FloatParameter = 0.0f;

	/** Integer parameter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	int32 IntParameter = 0;

	/** Boolean parameter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	bool BoolParameter = false;

	/** Tag parameter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	FGameplayTag TagParameter;

	/** Object parameter */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	TObjectPtr<UObject> ObjectParameter;
};

/**
 * Data asset defining an item effect
 */
UCLASS(BlueprintType)
class INVENTORYSYSTEMCORE_API UItemEffectDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UItemEffectDataAsset();

	// ========== BASIC INFORMATION ==========

	/** Unique identifier for this effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Basic")
	FName EffectID;

	/** Display name of the effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Basic")
	FText EffectName;

	/** Description of what this effect does */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Basic", meta = (MultiLine = true))
	FText EffectDescription;

	// ========== TRIGGER ==========

	/** When this effect should trigger */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Trigger")
	EItemEffectTrigger TriggerType = EItemEffectTrigger::OnUse;

	/** Target type for this effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Trigger")
	EItemEffectTarget TargetType = EItemEffectTarget::Self;

	/** Custom target selector (if TargetType is CustomSelector) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Trigger", meta = (EditCondition = "TargetType == EItemEffectTarget::CustomSelector"))
	TSubclassOf<UObject> CustomTargetSelector;

	// ========== CONDITIONS ==========

	/** Conditions that must be met for this effect to execute */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Conditions")
	FItemEffectCondition ExecutionConditions;

	// ========== EFFECT TYPES ==========

	/** Needs to modify when this effect executes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Types")
	TArray<FNeedsModification> NeedsModifications;

	/** Dialogues to trigger when this effect executes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Types")
	TArray<FDialogueTrigger> DialogueTriggers;

	/** Tag modifications to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Types")
	TArray<FTagModification> TagModifications;

	/** Custom Blueprint effect class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Types")
	TSubclassOf<UObject> CustomEffectClass;

	/** Parameters for custom effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Types")
	FCustomEffectParameters CustomEffectParameters;

	// ========== EXECUTION ==========

	/** Delay before executing the effect (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Execution", meta = (ClampMin = "0"))
	float ExecutionDelay = 0.0f;

	/** Number of times this effect should execute (0 = infinite for WhileOwned/WhileEquipped) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Execution", meta = (ClampMin = "0"))
	int32 ExecutionCount = 1;

	/** Interval between executions for repeating effects (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Execution", meta = (ClampMin = "0"))
	float ExecutionInterval = 0.0f;

	// ========== HELPER FUNCTIONS ==========

	/** Check if conditions are met for execution */
	UFUNCTION(BlueprintPure, Category = "Item Effect")
	bool CanExecute(AActor* Target) const;

	/** Check if this is a persistent effect (WhileOwned or WhileEquipped) */
	UFUNCTION(BlueprintPure, Category = "Item Effect")
	bool IsPersistentEffect() const;

	/** Check if this is a repeating effect */
	UFUNCTION(BlueprintPure, Category = "Item Effect")
	bool IsRepeatingEffect() const;
};
