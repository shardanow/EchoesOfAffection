// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameEventEffectRegistry.generated.h"

// Forward declarations
class UObject;
struct FGameEventPayload;

/**
 * Generic Effect Interface
 * , 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UGenericGameEffect : public UInterface
{
	GENERATED_BODY()
};

class GAMEEVENTBUS_API IGenericGameEffect
{
	GENERATED_BODY()

public:
	/**
	 * Execute the effect
	 * @param WorldContext World context object
	 * @param Payload Event payload with parameters
	 * @return True if effect executed successfully
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Game Effect")
	bool ExecuteEffect(UObject* WorldContext, const FGameEventPayload& Payload);
	virtual bool ExecuteEffect_Implementation(UObject* WorldContext, const FGameEventPayload& Payload) { return false; }

	/**
	 * Get effect display name (for editor/debug)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Game Effect")
	FText GetEffectDisplayName() const;
	virtual FText GetEffectDisplayName_Implementation() const { return FText::GetEmpty(); }

	/**
	 * Can this effect be reversed/undone?
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Game Effect")
	bool CanReverse() const;
	virtual bool CanReverse_Implementation() const { return false; }

	/**
	 * Reverse the effect (undo)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Game Effect")
	bool ReverseEffect(UObject* WorldContext, const FGameEventPayload& Payload);
	virtual bool ReverseEffect_Implementation(UObject* WorldContext, const FGameEventPayload& Payload) { return false; }
};

/**
 * Generic Condition Evaluator Interface
 * , )
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UGenericConditionEvaluator : public UInterface
{
	GENERATED_BODY()
};

class GAMEEVENTBUS_API IGenericConditionEvaluator
{
	GENERATED_BODY()

public:
	/**
	 * Evaluate condition
	 * @param WorldContext World context
	 * @param Payload Event payload with parameters
	 * @return True if condition is met
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Game Condition")
	bool EvaluateCondition(UObject* WorldContext, const FGameEventPayload& Payload) const;
	virtual bool EvaluateCondition_Implementation(UObject* WorldContext, const FGameEventPayload& Payload) const { return false; }

	/**
	 * Get condition display text
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Game Condition")
	FText GetConditionDisplayText() const;
	virtual FText GetConditionDisplayText_Implementation() const { return FText::GetEmpty(); }
};

/**
 * Effect Registration Info
 * 
 */
USTRUCT(BlueprintType)
struct GAMEEVENTBUS_API FEffectRegistration
{
	GENERATED_BODY()

	/** Effect tag identifier (e.g., Effect.Quest.Start, Effect.Dialogue.Modify) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Registration")
	FGameplayTag EffectTag;

	/** Effect class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Registration")
	TSubclassOf<UObject> EffectClass;

	/** Cached instance (created once, reused) */
	UPROPERTY(Transient)
	TObjectPtr<UObject> CachedInstance;

	/** Source plugin/system name (for debugging) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Registration")
	FName SourceSystem;

	FEffectRegistration()
		: EffectClass(nullptr)
		, CachedInstance(nullptr)
		, SourceSystem(NAME_None)
	{
	}
};

/**
 * Condition Registration Info
 */
USTRUCT(BlueprintType)
struct GAMEEVENTBUS_API FConditionRegistration
{
	GENERATED_BODY()

	/** Condition tag identifier (e.g., Condition.Time.InRange, Condition.Affection.Above) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Registration")
	FGameplayTag ConditionTag;

	/** Condition evaluator class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Registration")
	TSubclassOf<UObject> EvaluatorClass;

	/** Cached instance */
	UPROPERTY(Transient)
	TObjectPtr<UObject> CachedInstance;

	/** Source plugin/system name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Registration")
	FName SourceSystem;

	FConditionRegistration()
		: EvaluatorClass(nullptr)
		, CachedInstance(nullptr)
		, SourceSystem(NAME_None)
	{
	}
};

/**
 * Game Event Effect Registry
 * 
 * 
 * 
 * 
 * 
 * - QuestSystem 
 * - DialogueSystem 
 * - TimeSystem 
 * 
 * 
 */
UCLASS(BlueprintType)
class GAMEEVENTBUS_API UGameEventEffectRegistry : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Register effect
	 * @param EffectTag Unique tag for this effect
	 * @param EffectClass Class implementing IGenericGameEffect
	 * @param SourceSystem Name of plugin/system registering this effect
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect Registry")
	void RegisterEffect(const FGameplayTag& EffectTag, TSubclassOf<UObject> EffectClass, FName SourceSystem = NAME_None);

	/**
	 * Register condition evaluator
	 * @param ConditionTag Unique tag for this condition
	 * @param EvaluatorClass Class implementing IGenericConditionEvaluator
	 * @param SourceSystem Name of plugin/system registering this evaluator
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect Registry")
	void RegisterCondition(const FGameplayTag& ConditionTag, TSubclassOf<UObject> EvaluatorClass, FName SourceSystem = NAME_None);

	/**
	 * Execute registered effect
	 * @param EffectTag Tag of registered effect
	 * @param WorldContext World context
	 * @param Payload Effect parameters
	 * @return True if effect was found and executed successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect Registry", meta = (WorldContext = "WorldContext"))
	bool ExecuteEffect(const FGameplayTag& EffectTag, UObject* WorldContext, const FGameEventPayload& Payload);

	/**
	 * Evaluate registered condition
	 * @param ConditionTag Tag of registered condition
	 * @param WorldContext World context
	 * @param Payload Condition parameters
	 * @return True if condition was found and evaluated to true
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect Registry", meta = (WorldContext = "WorldContext"))
	bool EvaluateCondition(const FGameplayTag& ConditionTag, UObject* WorldContext, const FGameEventPayload& Payload) const;

	/**
	 * Check if effect is registered
	 */
	UFUNCTION(BlueprintPure, Category = "Effect Registry")
	bool IsEffectRegistered(const FGameplayTag& EffectTag) const;

	/**
	 * Check if condition is registered
	 */
	UFUNCTION(BlueprintPure, Category = "Effect Registry")
	bool IsConditionRegistered(const FGameplayTag& ConditionTag) const;

	/**
	 * Get all registered effect tags
	 */
	UFUNCTION(BlueprintPure, Category = "Effect Registry")
	TArray<FGameplayTag> GetAllRegisteredEffects() const;

	/**
	 * Get all registered condition tags
	 */
	UFUNCTION(BlueprintPure, Category = "Effect Registry")
	TArray<FGameplayTag> GetAllRegisteredConditions() const;

	/**
	 * Unregister effect
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect Registry")
	void UnregisterEffect(const FGameplayTag& EffectTag);

	/**
	 * Unregister condition
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect Registry")
	void UnregisterCondition(const FGameplayTag& ConditionTag);

	/**
	 * Clear all registrations
	 */
	UFUNCTION(BlueprintCallable, Category = "Effect Registry")
	void ClearAllRegistrations();

protected:
	/** Registered effects */
	UPROPERTY(Transient)
	TMap<FGameplayTag, FEffectRegistration> RegisteredEffects;

	/** Registered conditions */
	UPROPERTY(Transient)
	TMap<FGameplayTag, FConditionRegistration> RegisteredConditions;

private:
	/** Helper: Get or create effect instance */
	UObject* GetOrCreateEffectInstance(FEffectRegistration& Registration);

	/** Helper: Get or create condition instance */
	UObject* GetOrCreateConditionInstance(FConditionRegistration& Registration);
};
