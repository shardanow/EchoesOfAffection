// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractableInterface.h"
#include "Interaction/InteractionTypes.h"
#include "PickupableActor.generated.h"

/**
 * Example: Base class for items that can be picked up
 */
UCLASS(Blueprintable)
class ECHOESOFAFFECTION_API APickupableActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	APickupableActor();

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

	// ========== Pickup Methods ==========

	/**
	 * Called when item is picked up (Blueprint implementable)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup", meta = (DisplayName = "On Picked Up"))
	void BP_OnPickedUp(AActor* PickedUpBy);

	/**
	 * Enable/disable this pickup
	 */
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetPickupEnabled(bool bEnabled);

	// ========== Settings ==========

	/** Display name of the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	FText ItemName;

	/** Can this item be picked up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bCanBePickedUp;

	/** Should destroy actor after pickup */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bDestroyOnPickup;

	/** Pickup distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float PickupDistance;

	/** Mesh component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	class UStaticMeshComponent* MeshComponent;

protected:
	/**
	 * Perform the pickup logic
	 */
	virtual void PerformPickup(AActor* PickedUpBy);
};
