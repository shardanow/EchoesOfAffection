// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Conditions/DialogueConditionEvaluator.h"
#include "GameplayTagContainer.h"
#include "DialogueCondition_CheckRelationship.generated.h"

/**
 * Relationship check mode
 */
UENUM(BlueprintType)
enum class ERelationshipCheckMode : uint8
{
	/** Check dimension value */
	CheckDimension UMETA(DisplayName = "Check Dimension Value"),
	
	/** Check current state */
	CheckState UMETA(DisplayName = "Check Current State"),
	
	/** Check has trait */
	CheckTrait UMETA(DisplayName = "Check Has Trait"),
	
	/** Check state is NOT this */
	CheckNotState UMETA(DisplayName = "Check NOT State"),
	
	/** Check trait is missing */
	CheckNoTrait UMETA(DisplayName = "Check NO Trait")
};

/**
 * Comparison operator for dimension values
 */
UENUM(BlueprintType)
enum class EDialogueRelationshipComparisonOp : uint8
{
	/** Greater than */
	Greater UMETA(DisplayName = ">"),
	
	/** Greater than or equal */
	GreaterOrEqual UMETA(DisplayName = ">="),
	
	/** Less than */
	Less UMETA(DisplayName = "<"),
	
	/** Less than or equal */
	LessOrEqual UMETA(DisplayName = "<="),
	
	/** Equal (with tolerance) */
	Equal UMETA(DisplayName = "="),
	
	/** Not equal */
	NotEqual UMETA(DisplayName = "!=")
};

/**
 * Condition for checking relationship state
 * 
 * Supported checks:
 * - Check dimensions (Trust, Affinity, Romance, etc.)
 * - Check states (Friend, Enemy, Romantic, etc.)
 * - Check traits
 * 
 * Soft Dependency:
 * - Works only if RelationshipSystem plugin is enabled
 * - Falls back to failing condition if unavailable
 */
UCLASS(BlueprintType, meta=(DisplayName="Check Relationship"))
class DIALOGUESYSTEMCORE_API UDialogueCondition_CheckRelationship : public UDialogueCondition
{
	GENERATED_BODY()

public:
	/** Check mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship")
	ERelationshipCheckMode CheckMode = ERelationshipCheckMode::CheckDimension;

	//~ Begin Mode: Check Dimension

	/** Which dimension to check (if Mode = CheckDimension) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Dimension",
		meta=(EditCondition="CheckMode==ERelationshipCheckMode::CheckDimension",
		 EditConditionHides,
		      Categories="Relationship.Dimension"))
	FGameplayTag DimensionTag;

	/** Comparison operator */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Dimension",
		meta=(EditCondition="CheckMode==ERelationshipCheckMode::CheckDimension",
		      EditConditionHides))
	EDialogueRelationshipComparisonOp ComparisonOp = EDialogueRelationshipComparisonOp::GreaterOrEqual;

	/** Threshold value for comparison */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Dimension",
		meta=(EditCondition="CheckMode==ERelationshipCheckMode::CheckDimension",
		      EditConditionHides))
	float ThresholdValue = 50.0f;

	/** Tolerance for Equal comparison */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Dimension",
		meta=(EditCondition="CheckMode==ERelationshipCheckMode::CheckDimension && ComparisonOp==EDialogueRelationshipComparisonOp::Equal",
		      EditConditionHides))
	float EqualityTolerance = 5.0f;

	//~ End Mode: Check Dimension

	//~ Begin Mode: Check State/Trait

	/** Required state/trait tag */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|State",
		meta=(EditCondition="CheckMode==ERelationshipCheckMode::CheckState || CheckMode==ERelationshipCheckMode::CheckNotState || CheckMode==ERelationshipCheckMode::CheckTrait || CheckMode==ERelationshipCheckMode::CheckNoTrait",
		      EditConditionHides,
		      Categories="Relationship.State, Relationship.Trait"))
	FGameplayTag StateOrTraitTag;

	//~ End Mode: Check State/Trait

	//~ Begin Optional Settings

	/** Check player's relationship with NPC (instead of NPC to player) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Advanced")
	bool bCheckPlayerRelationship = false;

	//~ End Optional Settings

public:
	//~ Begin UDialogueCondition Interface
	virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
	virtual FString GetDescription_Implementation() const override;
	//~ End UDialogueCondition Interface

protected:
	/** Get relationship component */
	UActorComponent* GetRelationshipComponent(AActor* Actor) const;

	/** Check dimension */
	bool CheckDimensionValue(UActorComponent* RelComp, AActor* Target) const;

	/** Check state */
	bool CheckCurrentState(UActorComponent* RelComp, AActor* Target) const;

	/** Check trait */
	bool CheckHasTrait(UActorComponent* RelComp, AActor* Target) const;

	/** Perform comparison operation */
	bool PerformComparison(float ActualValue) const;
};
