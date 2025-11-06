// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffectExecutor.h"
#include "GameplayTagContainer.h"
#include "DialogueEffect_EmitGameEvent.generated.h"

/**
 * Dialogue Effect: Emit Game Event
 * 
 * 
 * 
 */
UCLASS(BlueprintType, meta = (DisplayName = "Emit Game Event"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_EmitGameEvent : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_EmitGameEvent() 
	{ 
		bSupportsReverse = false; // Events cannot be reversed
	}

	/** Event tag to emit (e.g., "Dialogue.Event.NodeReached", "Quest.Event.ObjectiveComplete") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (Categories = ""))
	FGameplayTag EventTag;

	/** String parameter (DialogueId, QuestId, ItemId, etc) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName StringParam;

	/** Secondary string parameter (NodeId, ObjectiveId, etc) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName StringParam2;

	/** Integer parameter (Count, Amount, Progress, etc) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	int32 IntParam = 0;

	/** Float parameter (Value, Threshold, etc) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float FloatParam = 0.0f;

	/** Additional tags (for filtering, conditions, etc) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTagContainer AdditionalTags;

	/** Use dialogue Player as InstigatorActor? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bUsePlayerAsInstigator = true;

	/** Use dialogue NPC as TargetActor? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bUseNPCAsTarget = true;

	/** Log event emission (for debugging) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Debug")
	bool bLogEvent = false;

	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Dialogue Effect: Emit Multiple Game Events
 * 
 */
UCLASS(BlueprintType, meta = (DisplayName = "Emit Multiple Game Events"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_EmitMultipleGameEvents : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_EmitMultipleGameEvents() 
	{ 
		bSupportsReverse = false;
	}

	/** List of events to emit */
	UPROPERTY(EditAnywhere, Instanced, Category = "Effect")
	TArray<TObjectPtr<UDialogueEffect_EmitGameEvent>> EventsToEmit;

	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
};
