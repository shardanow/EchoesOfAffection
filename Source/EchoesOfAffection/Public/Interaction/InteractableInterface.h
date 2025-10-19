// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionTypes.h"
#include "InteractableInterface.generated.h"

class AActor;
class UInteractionComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that can be interacted with
 */
class ECHOESOFAFFECTION_API IInteractableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Called when interaction begins
	 * @param InteractingActor - The actor initiating the interaction
	 * @return Result of the interaction attempt
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	EInteractionResult BeginInteract(AActor* InteractingActor);

	/**
	 * Called when interaction ends (for duration-based interactions)
	 * @param InteractingActor - The actor that was interacting
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EndInteract(AActor* InteractingActor);

	/**
	 * Called to cancel an ongoing interaction
	 * @param InteractingActor - The actor that was interacting
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void CancelInteract(AActor* InteractingActor);

	/**
	 * Get interaction data for this interactable
	 * @param InteractingActor - The actor requesting interaction data
	 * @return Interaction data structure
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FInteractionData GetInteractionData(AActor* InteractingActor) const;

	/**
	 * Check if this object can be interacted with
	 * @param InteractingActor - The actor requesting to interact
	 * @return True if interaction is allowed
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* InteractingActor) const;

	/**
	 * Called when this interactable is focused/highlighted
	 * @param InteractingActor - The actor focusing on this object
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionFocused(AActor* InteractingActor);

	/**
	 * Called when this interactable loses focus
	 * @param InteractingActor - The actor that was focusing on this object
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionUnfocused(AActor* InteractingActor);
};
