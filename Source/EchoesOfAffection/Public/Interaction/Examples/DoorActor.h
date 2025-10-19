// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/InteractableInterface.h"
#include "Interaction/InteractionTypes.h"
#include "DoorActor.generated.h"

/**
 * Example: Base class for doors that can be opened/closed
 */
UCLASS(Blueprintable)
class ECHOESOFAFFECTION_API ADoorActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ADoorActor();

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

	// ========== Door Methods ==========

	/**
	 * Open the door
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor();

	/**
	 * Close the door
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor();

	/**
	 * Toggle door state
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void ToggleDoor();

	/**
	 * Check if door is open
	 */
	UFUNCTION(BlueprintPure, Category = "Door")
	bool IsOpen() const { return bIsOpen; }

	/**
	 * Lock/unlock the door
	 */
	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetLocked(bool bLocked);

	/**
	 * Check if door is locked
	 */
	UFUNCTION(BlueprintPure, Category = "Door")
	bool IsLocked() const { return bIsLocked; }

	// ========== Blueprint Events ==========

	/**
	 * Called when door starts opening
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Door Opening"))
	void BP_OnDoorOpening();

	/**
	 * Called when door finishes opening
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Door Opened"))
	void BP_OnDoorOpened();

	/**
	 * Called when door starts closing
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Door Closing"))
	void BP_OnDoorClosing();

	/**
	 * Called when door finishes closing
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Door Closed"))
	void BP_OnDoorClosed();

	/**
	 * Called when trying to open a locked door
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Door", meta = (DisplayName = "On Door Locked Attempt"))
	void BP_OnDoorLockedAttempt(AActor* InteractingActor);

	// ========== Settings ==========

	/** Door frame mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* DoorFrameComponent;

	/** Door mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	class UStaticMeshComponent* DoorComponent;

	/** Is the door currently open */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	bool bIsOpen;

	/** Is the door locked */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bIsLocked;

	/** Time it takes to open/close the door */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorOpenDuration;

	/** Interaction distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float InteractionDistance;

	/** Door open angle (in degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float DoorOpenAngle;

	/** Should the door close automatically */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bAutoClose;

	/** Time before auto-closing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (EditCondition = "bAutoClose"))
	float AutoCloseDelay;

protected:
	FTimerHandle AutoCloseTimerHandle;
};
