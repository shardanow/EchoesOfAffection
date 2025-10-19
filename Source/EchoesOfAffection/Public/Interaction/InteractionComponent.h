// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionTypes.h"
#include "InteractableInterface.h"
#include "InteractionComponent.generated.h"

class UCameraComponent;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionStarted, AActor*, InteractableActor, const FInteractionData&, InteractionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionCompleted, AActor*, InteractableActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionCancelled, AActor*, InteractableActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractableFocused, AActor*, InteractableActor, const FInteractionData&, InteractionData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableUnfocused, AActor*, InteractableActor);

/**
 * Component that allows an actor to interact with interactable objects
 * Handles detection, focusing, and triggering of interactions
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType)
class ECHOESOFAFFECTION_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ========== Interaction Methods ==========
	
	/**
	 * Attempt to start interaction with currently focused object
	 * @return Result of the interaction attempt
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	EInteractionResult TryInteract();

	/**
	 * Cancel current ongoing interaction
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CancelInteraction();

	/**
	 * Check if currently interacting with something
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsInteracting() const { return bIsInteracting; }

	/**
	 * Get the actor currently being focused on
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedActor() const { return FocusedActor; }

	/**
	 * Get the actor currently being interacted with
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetInteractingActor() const { return InteractingActor; }

	/**
	 * Get interaction data of currently focused object
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FInteractionData GetFocusedInteractionData() const { return FocusedInteractionData; }

	/**
	 * Get remaining interaction time (for duration-based interactions)
	 */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetInteractionProgress() const;

	// ========== Settings ==========

	/** Enable/disable interaction detection */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Settings")
	bool bInteractionEnabled;

	/** Trace distance for finding interactables */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Settings")
	float TraceDistance;

	/** Radius for sphere trace (0 = line trace) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Settings")
	float TraceRadius;

	/** How often to check for interactables (in seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Settings")
	float TraceTickInterval;

	/** Debug: Draw interaction traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug")
	bool bDrawDebugTrace;

	/** Collision channel for interaction traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Settings")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel;

	// ========== Delegates ==========

	/** Called when interaction starts */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionStarted OnInteractionStarted;

	/** Called when interaction completes successfully */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionCompleted OnInteractionCompleted;

	/** Called when interaction is cancelled */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractionCancelled OnInteractionCancelled;

	/** Called when an interactable is focused */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractableFocused OnInteractableFocused;

	/** Called when an interactable loses focus */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnInteractableUnfocused OnInteractableUnfocused;

protected:
	/**
	 * Find interactable objects in front of the actor
	 */
	void FindInteractables();

	/**
	 * Update currently focused interactable
	 */
	void UpdateFocusedInteractable(AActor* NewFocusedActor);

	/**
	 * Complete current interaction
	 */
	void CompleteInteraction();

	/**
	 * Get camera for traces (override for custom camera setup)
	 */
	virtual bool GetTraceStartAndEnd(FVector& OutStart, FVector& OutEnd, FVector& OutDirection);

	/**
	 * Check if actor implements interactable interface
	 */
	bool IsActorInteractable(AActor* Actor) const;

	/**
	 * Validate interaction distance and line of sight
	 */
	bool ValidateInteraction(AActor* InteractableActor, const FInteractionData& InteractionData);

private:
	/** Currently focused interactable actor */
	UPROPERTY()
	AActor* FocusedActor;

	/** Actor currently being interacted with */
	UPROPERTY()
	AActor* InteractingActor;

	/** Data of currently focused interaction */
	FInteractionData FocusedInteractionData;

	/** Is an interaction currently in progress */
	bool bIsInteracting;

	/** Timer for interaction duration */
	float CurrentInteractionTime;

	/** Timer for trace interval */
	float TraceTimer;

	/** Duration of current interaction */
	float InteractionDuration;
};
