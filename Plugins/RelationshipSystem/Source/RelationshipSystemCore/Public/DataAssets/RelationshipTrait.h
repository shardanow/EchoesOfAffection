// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/RelationshipDataAsset.h"
#include "GameplayTagContainer.h"
#include "RelationshipTrait.generated.h"

/**
 * Dimension modifier applied by a trait
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FTraitDimensionModifier
{
	GENERATED_BODY()

	/** Target dimension */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier", meta = (Categories = "Relationship.Dimension"))
	FGameplayTag DimensionTag;

	/** Additive modifier (e.g., +5 per interaction) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	float AdditiveModifier;

	/** Multiplicative modifier (e.g., 1.5x = +50% effectiveness) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier")
	float MultiplicativeModifier;

	/** Only apply to specific actions (empty = all actions) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Modifier", meta = (Categories = "Relationship.Action"))
	FGameplayTagContainer FilterActions;

	FTraitDimensionModifier()
		: AdditiveModifier(0.0f)
		, MultiplicativeModifier(1.0f)
	{
	}

	/** Check if this modifier applies to given action */
	bool AppliesToAction(FGameplayTag ActionTag) const
	{
		return FilterActions.IsEmpty() || FilterActions.HasTagExact(ActionTag);
	}

	/** Apply modifier to value */
	float Apply(float BaseValue) const
	{
		return (BaseValue * MultiplicativeModifier) + AdditiveModifier;
	}
};

/**
 * Event response definition for traits
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FTraitEventResponse
{
	GENERATED_BODY()

	/** Event tag to respond to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event", meta = (Categories = "Relationship.Event"))
	FGameplayTag EventTag;

	/** Dimension modifications when event fires */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TMap<FGameplayTag, float> DimensionChanges;

	/** Probability of response (0-1, 1.0 = always) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ResponseProbability;

	/** Cooldown between responses (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	float Cooldown;

	FTraitEventResponse()
		: ResponseProbability(1.0f)
		, Cooldown(0.0f)
	{
	}
};

/**
 * Defines a behavioral trait that modifies relationship interactions
 * Examples: Forgiving (reduces negative impact), Suspicious (slower trust gain),
 *           Loyal (bonus to friendship), Vengeful (remembers slights)
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API URelationshipTrait : public URelationshipDataAsset
{
	GENERATED_BODY()

public:
	URelationshipTrait();

	/** Can multiple instances of this trait stack? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait")
	bool bCanStack;

	/** Maximum stack count (only relevant if bCanStack = true) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait", meta = (EditCondition = "bCanStack", ClampMin = "1"))
	int32 MaxStacks;

	/** Dimension modifiers applied by this trait */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifiers")
	TArray<FTraitDimensionModifier> DimensionModifiers;

	/** Event responses (reactive behavior) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Events")
	TArray<FTraitEventResponse> EventResponses;

	/** Traits that conflict with this one (mutually exclusive) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait", meta = (Categories = "Relationship.Trait"))
	FGameplayTagContainer ConflictingTraits;

	/** Traits required for this trait to function */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trait", meta = (Categories = "Relationship.Trait"))
	FGameplayTagContainer RequiredTraits;

	/** Get modifier for specific dimension and action */
	UFUNCTION(BlueprintCallable, Category = "Trait")
	FTraitDimensionModifier GetModifierForDimension(FGameplayTag DimensionTag, FGameplayTag ActionTag) const;

	/** Get event response for specific event */
	UFUNCTION(BlueprintCallable, Category = "Trait")
	FTraitEventResponse GetEventResponse(FGameplayTag EventTag) const;

	/** Check if trait conflicts with another */
	UFUNCTION(BlueprintCallable, Category = "Trait")
	bool ConflictsWith(FGameplayTag OtherTraitTag) const;

	/** Apply all dimension modifiers to a value */
	UFUNCTION(BlueprintCallable, Category = "Trait")
	float ApplyModifiers(FGameplayTag DimensionTag, FGameplayTag ActionTag, float BaseValue, int32 StackCount = 1) const;

protected:
	virtual bool ValidateInternal(TArray<FText>& OutErrors) const override;
};

/**
 * Runtime trait instance with stack tracking
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipTraitInstance
{
	GENERATED_BODY()

	/** Trait tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trait")
	FGameplayTag TraitTag;

	/** Current stack count */
	UPROPERTY(BlueprintReadOnly, Category = "Trait")
	int32 StackCount;

	/** Time acquired (game time in seconds) */
	UPROPERTY(BlueprintReadOnly, Category = "Trait")
	float AcquiredTime;

	/** Event cooldowns (EventTag -> next available time) */
	UPROPERTY()
	TMap<FGameplayTag, float> EventCooldowns;

	FRelationshipTraitInstance()
		: StackCount(1)
		, AcquiredTime(0.0f)
	{
	}

	FRelationshipTraitInstance(FGameplayTag InTag, float InTime)
		: TraitTag(InTag)
		, StackCount(1)
		, AcquiredTime(InTime)
	{
	}

	/** Add stack */
	void AddStack(const URelationshipTrait* Trait);

	/** Check if event is on cooldown */
	bool IsEventOnCooldown(FGameplayTag EventTag, float CurrentGameTime) const;

	/** Put event on cooldown */
	void SetEventCooldown(FGameplayTag EventTag, float Cooldown, float CurrentGameTime);
};
