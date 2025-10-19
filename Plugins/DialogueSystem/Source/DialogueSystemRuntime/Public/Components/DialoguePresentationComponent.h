// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/DialogueCameraComponent.h"
#include "Components/DialogueInputBlockerComponent.h"
#include "DialoguePresentationComponent.generated.h"

// Forward declarations
class UDialogueRunner;
class UDialogueComponent;

/**
 * Presentation mode for dialogues
 */
UENUM(BlueprintType)
enum class EDialoguePresentationMode : uint8
{
	/** Cinematic mode (camera + full input blocking) */
	Cinematic		UMETA(DisplayName = "Cinematic"),
	
	/** Interactive mode (no camera, minimal input blocking) */
	Interactive		UMETA(DisplayName = "Interactive"),
	
	/** Hybrid mode (camera but allow some movement) */
	Hybrid			UMETA(DisplayName = "Hybrid"),
	
	/** First-person mode (minimal camera movement, focus on NPC) */
	FirstPerson		UMETA(DisplayName = "First-Person"),
	
	/** Custom mode (defined by settings) */
	Custom			UMETA(DisplayName = "Custom")
};

/**
 * Complete dialogue presentation settings
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FDialoguePresentationSettings
{
	GENERATED_BODY()

	/** Presentation mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
	EDialoguePresentationMode PresentationMode = EDialoguePresentationMode::Cinematic;

	/** Camera settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Camera")
	FDialogueCameraSettings CameraSettings;

	/** Input blocking settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Input")
	FDialogueInputBlockingSettings InputSettings;

	/** Enable camera control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Camera")
	bool bEnableCamera = true;

	/** Enable input blocking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Input")
	bool bEnableInputBlocking = true;

	/** Pause game during dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
	bool bPauseGame = false;

	/** Slow down time during dialogue (0.0 = pause, 1.0 = normal) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation", meta = (ClampMin = "0", ClampMax = "1"))
	float TimeDilation = 1.0f;
};

/**
 * Dialogue Presentation Component
 * 
 * High-level component that manages all presentation aspects of dialogues:
 * - Integrates camera control
 * - Manages input blocking
 * - Coordinates with DialogueComponent
 * - Provides preset modes for common scenarios
 * - Handles game state (pause, time dilation)
 * 
 * This is the main component you should add to your player character
 * or game mode to handle dialogue presentation.
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API UDialoguePresentationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDialoguePresentationComponent();

protected:
	//~ Begin Configuration

	/** Default presentation mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
	EDialoguePresentationMode DefaultPresentationMode = EDialoguePresentationMode::Cinematic;

	/** Settings for each presentation mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
	TMap<EDialoguePresentationMode, FDialoguePresentationSettings> PresentationModeSettings;

	/** Auto-activate presentation on dialogue start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
	bool bAutoActivateOnDialogueStart = true;

	/** Auto-deactivate presentation on dialogue end */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
	bool bAutoDeactivateOnDialogueEnd = true;

	/** Socket on player to attach camera (for first-person mode) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation|Camera")
	FName PlayerCameraSocket = TEXT("head");

	//~ End Configuration

	//~ Begin Sub-Components

	/** Camera control component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<UDialogueCameraComponent> CameraComponent;

	/** Input blocker component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<UDialogueInputBlockerComponent> InputBlockerComponent;

	//~ End Sub-Components

	//~ Begin Runtime State

	/** Is presentation currently active? */
	UPROPERTY(Transient)
	bool bPresentationActive = false;

	/** Current presentation settings */
	UPROPERTY(Transient)
	FDialoguePresentationSettings CurrentSettings;

	/** Cached dialogue component */
	UPROPERTY(Transient)
	TWeakObjectPtr<UDialogueComponent> CachedDialogueComponent;

	/** Original time dilation */
	float OriginalTimeDilation = 1.0f;

	/** Was game paused before dialogue? */
	bool bWasGamePaused = false;

	//~ End Runtime State

public:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface

	//~ Begin Public API

	/**
	 * Activate dialogue presentation
	 * @param TargetNPC - The NPC to focus on
	 * @param Mode - Presentation mode to use
	 * @return True if presentation was activated successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation")
	bool ActivatePresentation(AActor* TargetNPC, EDialoguePresentationMode Mode);

	/**
	 * Activate presentation with custom settings
	 * @param TargetNPC - The NPC to focus on
	 * @param Settings - Custom presentation settings
	 * @return True if presentation was activated successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation")
	bool ActivatePresentationWithSettings(AActor* TargetNPC, const FDialoguePresentationSettings& Settings);

	/**
	 * Deactivate dialogue presentation and restore original state
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation")
	void DeactivatePresentation();

	/**
	 * Transition to new NPC (change camera target)
	 * @param NewTargetNPC - The new NPC to focus on
	 * @param BlendTime - Time to blend camera
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation")
	void TransitionToNewTarget(AActor* NewTargetNPC, float BlendTime = 0.5f);

	/**
	 * Update presentation mode on the fly
	 * @param NewMode - The new presentation mode
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation")
	void UpdatePresentationMode(EDialoguePresentationMode NewMode);

	/**
	 * Check if presentation is currently active
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Presentation")
	bool IsPresentationActive() const { return bPresentationActive; }

	/**
	 * Get camera component
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Presentation")
	UDialogueCameraComponent* GetCameraComponent() const { return CameraComponent; }

	/**
	 * Get input blocker component
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Presentation")
	UDialogueInputBlockerComponent* GetInputBlockerComponent() const { return InputBlockerComponent; }

	/**
	 * Set settings for a presentation mode
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Presentation")
	void SetModeSettings(EDialoguePresentationMode Mode, const FDialoguePresentationSettings& Settings);

	/**
	 * Get settings for a presentation mode
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Presentation")
	FDialoguePresentationSettings GetModeSettings(EDialoguePresentationMode Mode) const;

	//~ End Public API

protected:
	/** Initialize presentation mode presets */
	void InitializePresentationModes();

	/** Apply game state changes (pause, time dilation) */
	void ApplyGameStateChanges(const FDialoguePresentationSettings& Settings);

	/** Restore original game state */
	void RestoreGameState();

	/** Handle dialogue started event */
	UFUNCTION()
	void OnDialogueStarted(UDialogueRunner* Runner);

	/** Handle dialogue ended event */
	UFUNCTION()
	void OnDialogueEnded();

	/** Find dialogue component on owner */
	UDialogueComponent* FindDialogueComponent() const;

	/** Subscribe to dialogue events */
	void SubscribeToDialogueEvents();

	/** Unsubscribe from dialogue events */
	void UnsubscribeFromDialogueEvents();
};
