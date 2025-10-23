// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueInputBlockerComponent.generated.h"

// Forward declarations
class UEnhancedInputComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class APlayerController;
class APawn;

/**
 * Input mode types (simplified)
 */
UENUM()
enum class EInputMode : uint8
{
	GameOnly,
	UIOnly,
	GameAndUI
};

/**
 * Input blocking strategy
 */
UENUM(BlueprintType)
enum class EInputBlockingMode : uint8
{
	/** Block all input except dialogue actions */
	BlockAll		UMETA(DisplayName = "Block All"),
	
	/** Block only movement input */
	BlockMovement	UMETA(DisplayName = "Block Movement Only"),
	
	/** Block only camera input */
	BlockCamera		UMETA(DisplayName = "Block Camera Only"),
	
	/** Block movement and camera */
	BlockMovementAndCamera UMETA(DisplayName = "Block Movement & Camera"),
	
	/** Allow all input (no blocking) */
	AllowAll		UMETA(DisplayName = "Allow All"),
	
	/** Custom blocking (defined in blueprint) */
	Custom			UMETA(DisplayName = "Custom")
};

/**
 * Input blocking configuration
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FDialogueInputBlockingSettings
{
	GENERATED_BODY()

	/** Blocking mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EInputBlockingMode BlockingMode = EInputBlockingMode::BlockMovementAndCamera;

	/** Block movement actions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "BlockingMode == EInputBlockingMode::Custom", EditConditionHides))
	bool bBlockMovement = true;

	/** Block camera/look actions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "BlockingMode == EInputBlockingMode::Custom", EditConditionHides))
	bool bBlockCamera = true;

	/** Block jump action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "BlockingMode == EInputBlockingMode::Custom", EditConditionHides))
	bool bBlockJump = true;

	/** Block crouch action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "BlockingMode == EInputBlockingMode::Custom", EditConditionHides))
	bool bBlockCrouch = false;

	/** Block sprint action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "BlockingMode == EInputBlockingMode::Custom", EditConditionHides))
	bool bBlockSprint = true;

	/** Block interaction actions (except dialogue) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "BlockingMode == EInputBlockingMode::Custom", EditConditionHides))
	bool bBlockInteraction = true;

	/** Block inventory/menu actions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "BlockingMode == EInputBlockingMode::Custom", EditConditionHides))
	bool bBlockMenus = false;

	/** Block combat actions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (EditCondition = "BlockingMode == EInputBlockingMode::Custom", EditConditionHides))
	bool bBlockCombat = true;

	/** Disable pawn movement completely */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bDisablePawnMovement = true;

	/** Disable pawn rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bDisablePawnRotation = true;

	/** Hide player pawn during dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bHidePlayerPawn = false;

	/** Restore hidden pawn state after dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bRestorePawnState = true;

	/** Show mouse cursor during dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bShowMouseCursor = true;

	/** Enable click events during dialogue (for UI interaction) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bEnableClickEvents = true;

	/** Enable mouse over events during dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	bool bEnableMouseOverEvents = true;
};

/**
 * Dialogue Input Blocker Component
 * 
 * Manages input blocking during dialogues:
 * - Blocks movement and camera input
 * - Preserves dialogue-specific controls
 * - Integrates with Enhanced Input System
 * - Supports custom blocking configurations
 * - Automatically restores input state
 * 
 * Best Practices:
 * - Uses Enhanced Input System for UE5
 * - Falls back to legacy input for compatibility
 * - Preserves input mapping contexts
 * - Non-intrusive blocking (doesn't modify original setup)
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API UDialogueInputBlockerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDialogueInputBlockerComponent();

protected:
	//~ Begin Configuration

	/** Default input blocking settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FDialogueInputBlockingSettings DefaultBlockingSettings;

	/** Enable input blocking during dialogues */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bEnableInputBlocking = true;

	/** Priority for dialogue input mapping context (higher = more priority) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 DialogueInputPriority = 10;

	/** Dialogue-specific input mapping context (optional) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> DialogueInputMappingContext;

	/** Action names to block (legacy input) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Legacy")
	TArray<FName> LegacyActionNamesToBlock;

	/** Axis names to block (legacy input) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Legacy")
	TArray<FName> LegacyAxisNamesToBlock;

	//~ End Configuration

	//~ Begin Runtime State

	/** Cached player controller */
	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerController> CachedPlayerController;

	/** Cached player pawn */
	UPROPERTY(Transient)
TWeakObjectPtr<APawn> CachedPlayerPawn;

	/** Is input currently blocked? */
	UPROPERTY(Transient)
	bool bInputBlocked = false;

	/** Original pawn visibility */
	bool bOriginalPawnVisibility = true;

	/** Original movement mode */
	uint8 OriginalMovementMode = 0;

	/** Original pawn rotation enabled state */
	bool bOriginalRotationEnabled = true;

	/** Stored input mapping contexts (for restoration) */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UInputMappingContext>> StoredMappingContexts;

	/** Stored mapping context priorities */
	UPROPERTY(Transient)
	TArray<int32> StoredMappingPriorities;

	/** Original pawn pitch rotation enabled state */
	bool bOriginalPitchRotationEnabled = false;

	/** Original pawn roll rotation enabled state */
	bool bOriginalRollRotationEnabled = false;

	/** Original mouse cursor visibility */
	bool bOriginalShowMouseCursor = false;

	/** Original click events enabled state */
	bool bOriginalEnableClickEvents = false;

	/** Original mouse over events enabled state */
	bool bOriginalEnableMouseOverEvents = false;

	/** Original input mode (to restore after dialogue) */
	EInputMode OriginalInputMode = EInputMode::GameOnly;

	//~ End Runtime State

public:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

	//~ Begin Public API

	/**
	 * Block input with specified settings
	 * @param Settings - Input blocking configuration
	 * @return True if input was blocked successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Input")
	bool BlockInput(const FDialogueInputBlockingSettings& Settings);

	/**
	 * Block input with default settings
	 * @return True if input was blocked successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Input")
	bool BlockInputWithDefaults();

	/**
	 * Unblock input and restore original state
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Input")
	void UnblockInput();

	/**
	 * Check if input is currently blocked
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Input")
	bool IsInputBlocked() const { return bInputBlocked; }

	/**
	 * Update blocking mode on the fly
	 * @param NewMode - The new blocking mode
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Input")
	void UpdateBlockingMode(EInputBlockingMode NewMode);

	/**
	 * Temporarily allow specific input action
	 * @param ActionName - Name of the action to allow
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Input")
	void AllowInputAction(FName ActionName);

	/**
	 * Get current blocking settings
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Input")
	FDialogueInputBlockingSettings GetCurrentBlockingSettings() const { return DefaultBlockingSettings; }

	/**
	 * Set default blocking settings
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Input")
	void SetDefaultBlockingSettings(const FDialogueInputBlockingSettings& Settings);

	//~ End Public API

protected:
	/** Apply input blocking based on settings */
	void ApplyInputBlocking(const FDialogueInputBlockingSettings& Settings);

	/** Block Enhanced Input System actions */
	void BlockEnhancedInput(const FDialogueInputBlockingSettings& Settings);

	/** Block legacy input actions */
	void BlockLegacyInput(const FDialogueInputBlockingSettings& Settings);

	/** Restore input to original state */
	void RestoreInput();

	/** Restore Enhanced Input System state */
	void RestoreEnhancedInput();

	/** Restore legacy input state */
	void RestoreLegacyInput();

	/** Block pawn movement */
	void BlockPawnMovement(bool bBlock);

	/** Block pawn rotation */
	void BlockPawnRotation(bool bBlock);

	/** Set pawn visibility */
	void SetPawnVisibility(bool bVisible);

	/** Store current pawn state */
	void StorePawnState();

	/** Restore pawn state */
	void RestorePawnState();

	/** Populate legacy action/axis names based on mode */
	void PopulateLegacyBlockingLists(const FDialogueInputBlockingSettings& Settings);

	/** Find player controller */
	APlayerController* FindPlayerController() const;

	/** Check if Enhanced Input System is available */
	bool IsEnhancedInputAvailable() const;
};
