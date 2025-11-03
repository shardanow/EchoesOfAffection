// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffectExecutor.h"
#include "GameplayTagContainer.h"
#include "DialogueEffect_ModifyRelationship.generated.h"

/**
 * Relationship modification mode
 */
UENUM(BlueprintType)
enum class ERelationshipModificationMode : uint8
{
	/** Execute relationship action */
	ExecuteAction UMETA(DisplayName = "Execute Action"),
	
	/** Directly modify dimension values */
	DirectModify UMETA(DisplayName = "Direct Modify Dimensions"),
	
	/** Add trait to relationship */
	AddTrait UMETA(DisplayName = "Add Trait"),
	
	/** Remove trait from relationship */
	RemoveTrait UMETA(DisplayName = "Remove Trait"),
	
	/** Force state transition */
	ForceStateTransition UMETA(DisplayName = "Force State Transition")
};

/**
 * Dimension modifier struct
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FRelationshipDimensionModifier
{
	GENERATED_BODY()

	/** Dimension to modify */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier", meta=(Categories="Relationship.Dimension"))
	FGameplayTag DimensionTag;

	/** Delta value to apply */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
	float Delta = 0.0f;

	FRelationshipDimensionModifier()
		: Delta(0.0f)
	{}

	FRelationshipDimensionModifier(FGameplayTag InDimensionTag, float InDelta)
		: DimensionTag(InDimensionTag)
		, Delta(InDelta)
	{}
};

/**
 * Effect for modifying RelationshipSystem data
 * 
 * Supports multiple modification modes:
 * 1. Execute Action - Execute relationship action from Data Asset
 * 2. Direct Modify - Directly modify dimension values
 * 3. Add/Remove Trait - Manage relationship traits
 * 4. Force State - Force state transition
 * 
 * Soft Dependency:
 * - Requires RelationshipSystem plugin
 * - Falls back gracefully if unavailable
 */
UCLASS(BlueprintType, meta=(DisplayName="Modify Relationship"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyRelationship : public UDialogueEffect
{
	GENERATED_BODY()

public:
	/** Modification mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship")
	ERelationshipModificationMode Mode = ERelationshipModificationMode::ExecuteAction;

	//~ Begin Mode: Execute Action

	/** Action tag to execute */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Action",
		meta=(EditCondition="Mode==ERelationshipModificationMode::ExecuteAction", 
		      EditConditionHides,
		      Categories="Relationship.Action"))
	FGameplayTag ActionTag;

	//~ End Mode: Execute Action

	//~ Begin Mode: Direct Modify

	/** Dimension modifiers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Dimensions",
		meta=(EditCondition="Mode==ERelationshipModificationMode::DirectModify",
		      EditConditionHides))
	TArray<FRelationshipDimensionModifier> DimensionModifiers;

	//~ End Mode: Direct Modify

	//~ Begin Mode: Add/Remove Trait

	/** Trait tag */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Trait",
		meta=(EditCondition="Mode==ERelationshipModificationMode::AddTrait || Mode==ERelationshipModificationMode::RemoveTrait",
		      EditConditionHides,
		      Categories="Relationship.Trait"))
	FGameplayTag TraitTag;

	//~ End Mode: Add/Remove Trait

	//~ Begin Mode: Force State

	/** Target state tag */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|State",
		meta=(EditCondition="Mode==ERelationshipModificationMode::ForceStateTransition",
		      EditConditionHides,
		   Categories="Relationship.State"))
	FGameplayTag TargetStateTag;

	//~ End Mode: Force State

	//~ Begin Optional Settings

	/** Target participant ID (optional) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Advanced")
	FName TargetParticipantId;

	/** Apply to player's relationship with NPC (instead of NPC to player) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relationship|Advanced")
	bool bApplyToPlayer = false;

	//~ End Optional Settings

public:
	//~ Begin UDialogueEffect Interface
	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
	//~ End UDialogueEffect Interface

protected:
	/** Get relationship component from actor */
	UActorComponent* GetRelationshipComponent(AActor* Actor) const;

	/** Execute relationship action */
	void ExecuteAction(UActorComponent* RelComp, AActor* Target);

	/** Directly modify dimensions */
	void DirectModifyDimensions(UActorComponent* RelComp, AActor* Target);

	/** Add trait */
	void AddTraitToRelationship(UActorComponent* RelComp, AActor* Target);

	/** Remove trait */
	void RemoveTraitFromRelationship(UActorComponent* RelComp, AActor* Target);

	/** Force state transition */
	void ForceState(UActorComponent* RelComp, AActor* Target);
};
