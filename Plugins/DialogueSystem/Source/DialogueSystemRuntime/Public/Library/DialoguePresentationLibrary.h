// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/DialogueCameraComponent.h"
#include "Components/DialogueInputBlockerComponent.h"
#include "Components/DialoguePresentationComponent.h"
#include "DialoguePresentationLibrary.generated.h"

/**
 * Blueprint Function Library for Dialogue Presentation
 * 
 * Provides convenient Blueprint functions for:
 * - Quick camera control
 * - Easy input blocking
 * - Common presentation tasks
 * 
 * Use these functions for simple scenarios or prototyping.
 * For production, use the component API directly for more control.
 */
UCLASS()
class DIALOGUESYSTEMRUNTIME_API UDialoguePresentationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//~ Begin Camera Functions

	/**
	 * Quick activate dialogue camera with default settings
	 * @param WorldContextObject - World context
	 * @param TargetNPC - NPC to focus on
	 * @param Framing - Camera framing preset
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera", meta = (WorldContext = "WorldContextObject"))
	static bool QuickActivateDialogueCamera(
		UObject* WorldContextObject,
		AActor* TargetNPC,
		EDialogueCameraFraming Framing = EDialogueCameraFraming::Medium);

	/**
	 * Quick deactivate dialogue camera
	 * @param WorldContextObject - World context
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera", meta = (WorldContext = "WorldContextObject"))
	static void QuickDeactivateDialogueCamera(UObject* WorldContextObject);

	/**
	 * Check if dialogue camera is active
	 * @param WorldContextObject - World context
	 * @return True if active
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Camera", meta = (WorldContext = "WorldContextObject"))
	static bool IsDialogueCameraActive(UObject* WorldContextObject);

	/**
	 * Transition camera to new target quickly
	 * @param WorldContextObject - World context
	 * @param NewTarget - New NPC to focus on
	 * @param BlendTime - Blend duration
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera", meta = (WorldContext = "WorldContextObject"))
	static void TransitionCameraToTarget(
		UObject* WorldContextObject,
		AActor* NewTarget,
		float BlendTime = 0.5f);

	//~ End Camera Functions

	//~ Begin Input Functions

	/**
	 * Quick block player input
	 * @param WorldContextObject - World context
	 * @param Mode - Blocking mode
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Input", meta = (WorldContext = "WorldContextObject"))
	static bool QuickBlockInput(
		UObject* WorldContextObject,
		EInputBlockingMode Mode = EInputBlockingMode::BlockMovementAndCamera);

	/**
	 * Quick unblock player input
	 * @param WorldContextObject - World context
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Input", meta = (WorldContext = "WorldContextObject"))
	static void QuickUnblockInput(UObject* WorldContextObject);

	/**
	 * Check if input is currently blocked
	 * @param WorldContextObject - World context
	 * @return True if blocked
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Input", meta = (WorldContext = "WorldContextObject"))
	static bool IsInputBlocked(UObject* WorldContextObject);

	//~ End Input Functions

	//~ Begin Presentation Functions

	/**
	 * Start cinematic dialogue presentation
	 * All-in-one function for common use case
	 * 
	 * @param WorldContextObject - World context
	 * @param TargetNPC - NPC to focus on
	 * @param Mode - Presentation mode
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation", meta = (WorldContext = "WorldContextObject"))
	static bool StartDialoguePresentation(
		UObject* WorldContextObject,
		AActor* TargetNPC,
		EDialoguePresentationMode Mode = EDialoguePresentationMode::Cinematic);

	/**
	 * End dialogue presentation
	 * Restores camera and input
	 * 
	 * @param WorldContextObject - World context
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation", meta = (WorldContext = "WorldContextObject"))
	static void EndDialoguePresentation(UObject* WorldContextObject);

	/**
	 * Check if presentation is active
	 * @param WorldContextObject - World context
	 * @return True if active
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Presentation", meta = (WorldContext = "WorldContextObject"))
	static bool IsPresentationActive(UObject* WorldContextObject);

	/**
	 * Get or create presentation component on player
	 * Automatically adds component if not present
	 * 
	 * @param WorldContextObject - World context
	 * @return Presentation component (or nullptr if failed)
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation", meta = (WorldContext = "WorldContextObject"))
	static UDialoguePresentationComponent* GetOrCreatePresentationComponent(UObject* WorldContextObject);

	//~ End Presentation Functions

	//~ Begin Utility Functions

	/**
	 * Create default camera settings
	 * Useful for customizing in Blueprint
	 * 
	 * @param Framing - Base framing preset
	 * @return Camera settings struct
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Utility")
	static FDialogueCameraSettings MakeDefaultCameraSettings(EDialogueCameraFraming Framing);

	/**
	 * Create default input blocking settings
	 * Useful for customizing in Blueprint
	 * 
	 * @param Mode - Base blocking mode
	 * @return Input settings struct
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Utility")
	static FDialogueInputBlockingSettings MakeDefaultInputSettings(EInputBlockingMode Mode);

	/**
	 * Create default presentation settings
	 * Useful for customizing in Blueprint
	 * 
	 * @param Mode - Presentation mode
	 * @return Presentation settings struct
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Utility")
	static FDialoguePresentationSettings MakeDefaultPresentationSettings(EDialoguePresentationMode Mode);

	/**
	 * Get player's presentation component
	 * @param WorldContextObject - World context
	 * @return Component or nullptr
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Utility", meta = (WorldContext = "WorldContextObject"))
	static UDialoguePresentationComponent* GetPlayerPresentationComponent(UObject* WorldContextObject);

	/**
	 * Freeze player character
	 * Quick helper to stop movement/rotation
	 * 
	 * @param WorldContextObject - World context
	 * @param bFreeze - True to freeze, false to unfreeze
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Utility", meta = (WorldContext = "WorldContextObject"))
	static void FreezePlayerCharacter(UObject* WorldContextObject, bool bFreeze = true);

	/**
	 * Set player pawn visibility
	 * Useful for hiding player in cinematic dialogues
	 * 
	 * @param WorldContextObject - World context
	 * @param bVisible - Visibility state
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Utility", meta = (WorldContext = "WorldContextObject"))
	static void SetPlayerPawnVisibility(UObject* WorldContextObject, bool bVisible);

	//~ End Utility Functions

protected:
	/** Helper: Get player pawn */
	static APawn* GetPlayerPawn(UObject* WorldContextObject);

	/** Helper: Get player controller */
	static APlayerController* GetPlayerController(UObject* WorldContextObject);
};
