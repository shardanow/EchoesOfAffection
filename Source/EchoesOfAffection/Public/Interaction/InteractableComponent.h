// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionTypes.h"
#include "InteractableInterface.h"
#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableStateChanged, bool, bIsEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableBeginInteract, AActor*, InteractingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableEndInteract, AActor*, InteractingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableCancelInteract, AActor*, InteractingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableComponentGainedFocus, AActor*, InteractingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableComponentLostFocus, AActor*, InteractingActor);

/**
 * Component that makes an actor interactable
 * Add this to any actor you want to be interactable
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType)
class ECHOESOFAFFECTION_API UInteractableComponent : public UActorComponent, public IInteractableInterface
{
	GENERATED_BODY()

public:	
	UInteractableComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ========== Interactable Interface Implementation ==========

	virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override;
	virtual void EndInteract_Implementation(AActor* InteractingActor) override;
	virtual void CancelInteract_Implementation(AActor* InteractingActor) override;
	virtual FInteractionData GetInteractionData_Implementation(AActor* InteractingActor) const override;
	virtual bool CanInteract_Implementation(AActor* InteractingActor) const override;
	virtual void OnInteractionFocused_Implementation(AActor* InteractingActor) override;
	virtual void OnInteractionUnfocused_Implementation(AActor* InteractingActor) override;

	// ========== Public Methods ==========

	/**
	 * Enable/disable this interactable
	 */
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void SetInteractableEnabled(bool bEnabled);

	/**
	 * Check if this interactable is enabled
	 */
	UFUNCTION(BlueprintPure, Category = "Interactable")
	bool IsInteractableEnabled() const { return bIsEnabled; }

	/**
	 * Get number of times this has been interacted with
	 */
	UFUNCTION(BlueprintPure, Category = "Interactable")
	int32 GetInteractionCount() const { return InteractionCount; }

	/**
	 * Reset interaction count
	 */
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void ResetInteractionCount() { InteractionCount = 0; }

	/**
	 * Set custom interaction data
	 */
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void SetInteractionData(const FInteractionData& NewData) { InteractionData = NewData; }

	// ========== Settings ==========

	/** Interaction data configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	FInteractionData InteractionData;

	/** Is this interactable currently enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	bool bIsEnabled;

	/** Should highlight the actor when focused (requires implementation) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable|Visual")
	bool bHighlightWhenFocused;

	/** Maximum number of times this can be interacted with (-1 = unlimited) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	int32 MaxInteractionCount;

	// ========== Delegates ==========

	/** Called when interactable state changes */
	UPROPERTY(BlueprintAssignable, Category = "Interactable|Events")
	FOnInteractableStateChanged OnInteractableStateChanged;

	/** Called when interaction begins */
	UPROPERTY(BlueprintAssignable, Category = "Interactable|Events")
	FInteractableBeginInteract OnBeginInteract;

	/** Called when interaction ends */
	UPROPERTY(BlueprintAssignable, Category = "Interactable|Events")
	FInteractableEndInteract OnEndInteract;

	/** Called when interaction is cancelled */
	UPROPERTY(BlueprintAssignable, Category = "Interactable|Events")
	FInteractableCancelInteract OnCancelInteract;

	/** Called when this interactable is focused */
	UPROPERTY(BlueprintAssignable, Category = "Interactable|Events")
	FInteractableComponentGainedFocus OnFocused;

	/** Called when this interactable loses focus */
	UPROPERTY(BlueprintAssignable, Category = "Interactable|Events")
	FInteractableComponentLostFocus OnUnfocused;

	// ========== Blueprint Overridable Functions ==========

	/**
	 * Custom validation for interaction (Blueprint Event)
	 * Return false to prevent interaction
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Interactable|Events", meta = (DisplayName = "Can Interact Check"))
	bool BP_CanInteractCheck(AActor* InteractingActor) const;

protected:
	/**
	 * Apply highlight effect (override for custom highlighting)
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Interactable|Visual")
	void ApplyHighlight(bool bHighlight);

	/**
	 * Check if interaction limit has been reached
	 */
	bool HasReachedInteractionLimit() const;

private:
	/** Number of times this has been interacted with */
	UPROPERTY()
	int32 InteractionCount;

	/** Is currently focused */
	bool bIsFocused;
};
