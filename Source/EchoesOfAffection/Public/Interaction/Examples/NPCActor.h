// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractableInterface.h"
#include "Interaction/InteractionTypes.h"
#include "NPCActor.generated.h"

/**
 * Delegate for dialogue events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCDialogueStarted, AActor*, TalkingTo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNPCDialogueEnded);

/**
 * Example: Base class for NPCs that can be talked to
 */
UCLASS(Blueprintable)
class ECHOESOFAFFECTION_API ANPCActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ANPCActor();

protected:
	virtual void BeginPlay() override;

public:
	// ========== Interactable Interface ==========
	
	virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override;
	virtual void EndInteract_Implementation(AActor* InteractingActor) override;
	virtual FInteractionData GetInteractionData_Implementation(AActor* InteractingActor) const override;
	virtual bool CanInteract_Implementation(AActor* InteractingActor) const override;
	virtual void OnInteractionFocused_Implementation(AActor* InteractingActor) override;
	virtual void OnInteractionUnfocused_Implementation(AActor* InteractingActor) override;

	// ========== NPC Methods ==========

	/**
	 * Start dialogue with this NPC
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void StartDialogue(AActor* TalkingTo);

	/**
	 * End dialogue with this NPC
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void EndDialogue();

	/**
	 * Check if NPC is currently in dialogue
	 */
	UFUNCTION(BlueprintPure, Category = "NPC")
	bool IsInDialogue() const { return bIsInDialogue; }

	/**
	 * Enable/disable dialogue
	 */
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void SetDialogueEnabled(bool bEnabled);

	// ========== Blueprint Events ==========

	/**
	 * Called when dialogue starts
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "NPC", meta = (DisplayName = "On Dialogue Started"))
	void BP_OnDialogueStarted(AActor* TalkingTo);

	/**
	 * Called when dialogue ends
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "NPC", meta = (DisplayName = "On Dialogue Ended"))
	void BP_OnDialogueEnded();

	/**
	 * Custom check if NPC can talk (e.g., check quest conditions)
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "NPC", meta = (DisplayName = "Can Talk Check"))
	bool BP_CanTalkCheck(AActor* TalkingTo) const;

	// ========== Settings ==========

	/** NPC name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FText NPCName;

	/** Greeting text shown when focused */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FText GreetingText;

	/** Can this NPC be talked to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	bool bCanTalk;

	/** Interaction distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	float TalkDistance;

	/** Mesh component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	class USkeletalMeshComponent* MeshComponent;

	// ========== Delegates ==========

	/** Called when dialogue starts */
	UPROPERTY(BlueprintAssignable, Category = "NPC|Events")
	FOnNPCDialogueStarted OnDialogueStarted;

	/** Called when dialogue ends */
	UPROPERTY(BlueprintAssignable, Category = "NPC|Events")
	FOnNPCDialogueEnded OnDialogueEnded;

protected:
	/** Is NPC currently in dialogue */
	UPROPERTY()
	bool bIsInDialogue;

	/** Actor currently talking to this NPC */
	UPROPERTY()
	AActor* CurrentTalkingActor;
};
