// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "DialogueCameraComponent.generated.h"

// Forward declarations
class UCameraComponent;
class APlayerController;
class APlayerCameraManager;
class USpringArmComponent;

// NEW v1.17.1: GameEventBus integration
#if WITH_GAMEEVENTBUS
class UGameEventBusSubsystem;
struct FGameEventPayload;
#endif

/**
 * Camera blend modes for dialogue transitions
 */
UENUM(BlueprintType)
enum class EDialogueCameraBlendMode : uint8
{
	/** Linear interpolation */
	Linear		UMETA(DisplayName = "Linear"),
	
	/** Cubic ease in/out */
	Cubic		UMETA(DisplayName = "Cubic"),
	
	/** Ease in (slow start) */
	EaseIn		UMETA(DisplayName = "Ease In"),
	
	/** Ease out (slow end) */
	EaseOut		UMETA(DisplayName = "Ease Out"),
	
	/** Ease in and out */
	EaseInOut	UMETA(DisplayName = "Ease In/Out"),
	
	/** No blending (instant cut) */
	Cut			UMETA(DisplayName = "Cut")
};

/**
 * Camera framing preset for dialogues
 */
UENUM(BlueprintType)
enum class EDialogueCameraFraming : uint8
{
	/** Close-up shot (face focus) */
	CloseUp		UMETA(DisplayName = "Close-Up"),
	
	/** Medium shot (head and shoulders) */
	Medium		UMETA(DisplayName = "Medium Shot"),
	
	/** Full shot (entire character) */
	Full		UMETA(DisplayName = "Full Shot"),
	
	/** Over-the-shoulder shot */
	OverShoulder UMETA(DisplayName = "Over-The-Shoulder"),
	
	/** Two-shot (both participants) */
	TwoShot		UMETA(DisplayName = "Two-Shot"),
	
	/** Custom position */
	Custom		UMETA(DisplayName = "Custom")
};

/**
 * Camera settings for dialogue
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMRUNTIME_API FDialogueCameraSettings
{
	GENERATED_BODY()

	/** Camera framing preset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	EDialogueCameraFraming Framing = EDialogueCameraFraming::Medium;

	/** Distance from target actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "50", ClampMax = "1000"))
	float Distance = 200.0f;

	/** Height offset from target actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float HeightOffset = 60.0f;

	/** Angle offset (degrees) - horizontal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "-180", ClampMax = "180"))
	float AngleOffset = 25.0f;

	/** Vertical angle adjustment (degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "-45", ClampMax = "45"))
	float PitchAdjustment = -5.0f;

	/** Field of view override (0 = use default) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "0", ClampMax = "170"))
	float FOVOverride = 0.0f;

	/** Focus point offset from actor's location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector FocusPointOffset = FVector(0.0f, 0.0f, 60.0f); // Head height

	/** Use spring arm for camera positioning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bUseSpringArm = true;

	/** Enable camera collision detection */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bEnableCollision = true;

	/** Blend time to this camera position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "0", ClampMax = "5"))
	float BlendTime = 0.5f;

	/** Blend mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	EDialogueCameraBlendMode BlendMode = EDialogueCameraBlendMode::EaseInOut;
};

/**
 * Dialogue Camera Component
 * 
 * v1.17.1: Added automatic camera synchronization after actor teleportation
 * - Listens to GameEvent.Dialogue.ParticipantPositioned
 * - Automatically updates camera when current speaker moves
 * - Configurable delay for smooth repositioning
 * 
 * Handles cinematic camera control during dialogues:
 * - Smooth camera transitions
 * - Multiple framing presets
 * - Automatic focus on speakers
 * - Collision-aware positioning
 * - Restoration of previous camera state
 * 
 * Best Practices:
 * - Uses ViewTarget system for seamless integration
 * - Preserves player camera manager state
 * - Supports both first-person and third-person games
 * - Integrates with Spring Arm for collision
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API UDialogueCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDialogueCameraComponent();

protected:
	//~ Begin Configuration

	/** Default camera settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FDialogueCameraSettings DefaultCameraSettings;

	/** Settings per framing preset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TMap<EDialogueCameraFraming, FDialogueCameraSettings> FramingPresets;

	/** Enable camera control during dialogues */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bEnableCameraControl = true;

	/** Restore original camera on dialogue end */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bRestoreOriginalCamera = true;

	/** Time to restore camera (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "0", ClampMax = "3"))
	float RestoreBlendTime = 0.5f;

	/** Socket name on NPC to focus on (e.g. "head") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FName FocusSocketName = NAME_None;

	/** Dynamically track speaker during dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bTrackSpeaker = false;  

	/** Update frequency for speaker tracking (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "0.01", ClampMax = "2"))
	float TrackingUpdateRate = 0.1f;

	/** NEW v1.17.1: Auto-update camera when participant is positioned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Auto Update")
	bool bAutoUpdateOnPositioning = true;

	/** NEW v1.17.1: Delay before camera update after positioning (allows positioning to settle) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Auto Update", meta = (ClampMin = "0", ClampMax = "2"))
	float PositioningUpdateDelay = 0.2f;

	/** NEW v1.17.1: Blend time for camera repositioning after teleport */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Auto Update", meta = (ClampMin = "0", ClampMax = "3"))
	float PositioningBlendTime = 0.5f;

	/** NEW v1.17.1: Auto-switch camera to current speaker when node changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Auto Update")
	bool bAutoSwitchToSpeaker = true;

	/** NEW v1.17.1: Blend time when switching to new speaker */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Auto Update", meta = (ClampMin = "0", ClampMax = "2"))
	float SpeakerSwitchBlendTime = 0.6f;

	//~ End Configuration

	//~ Begin Runtime State

	/** Cached player controller */
	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerController> CachedPlayerController;

	/** Cached player camera manager */
	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerCameraManager> CachedCameraManager;

	/** Original view target before dialogue */
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> OriginalViewTarget;

	/** Camera actor spawned for dialogue */
	UPROPERTY(Transient)
	TObjectPtr<AActor> DialogueCameraActor;

	/** Camera component on dialogue camera */
	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> DialogueCameraComponent;

	/** Spring arm component (if used) */
	UPROPERTY(Transient)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	/** Current target actor (NPC) */
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> CurrentTarget;

	/** Is camera currently active? */
	UPROPERTY(Transient)
	bool bCameraActive = false;

	/** Is camera currently blending? */
	bool bIsBlending = false;

	/** Blend start time */
	float BlendStartTime = 0.0f;

	/** Blend duration */
	float BlendDuration = 0.0f;

	/** Transform at blend start */
	FTransform BlendStartTransform;

	/** Target transform for blend */
	FTransform BlendTargetTransform;

	/** Current blend function */
	EViewTargetBlendFunction CurrentBlendFunction = VTBlend_Linear;

	/** Start FOV for blending */
	float StartFOV = 90.0f;

	/** Target FOV for blending */
	float TargetFOV = 90.0f;

	/** Timer handle for speaker tracking */
	FTimerHandle TrackingTimerHandle;

	/** Original camera FOV */
	float OriginalFOV = 90.0f;

	/** Original player camera location (for stable tracking) */
	FVector OriginalPlayerLocation = FVector::ZeroVector;

	/** NEW v1.17.1: Timer handle for delayed camera update after positioning */
	FTimerHandle PositioningUpdateTimerHandle;

	/** NEW v1.17.1: GameEventBus subscription handle */
#if WITH_GAMEEVENTBUS
	FDelegateHandle PositioningEventHandle;
	FDelegateHandle NodeEnteredEventHandle;
#endif

	//~ End Runtime State

public:
	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	//~ Begin Public API

	/**
	 * Activate dialogue camera targeting the specified NPC
	 * @param TargetActor - The NPC to focus on
	 * @param Settings - Camera settings (optional, uses default if null)
	 * @return True if camera was activated successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera")
	bool ActivateDialogueCamera(AActor* TargetActor, const FDialogueCameraSettings& Settings);

	/**
	 * Activate dialogue camera with preset framing
	 * @param TargetActor - The NPC to focus on
	 * @param Framing - The framing preset to use
	 * @return True if camera was activated successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera")
	bool ActivateDialogueCameraWithFraming(AActor* TargetActor, EDialogueCameraFraming Framing);

	/**
	 * Simple activation with default Medium shot settings
	 * @param TargetActor - The NPC to focus on
	 * @return True if camera was activated successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera", meta = (DisplayName = "Activate Dialogue Camera (Simple)"))
	bool ActivateDialogueCameraSimple(AActor* TargetActor);

	/**
	 * Deactivate dialogue camera and restore original view
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera")
	void DeactivateDialogueCamera();

	/**
	 * Smoothly transition to a new target actor
	 * @param NewTarget - The new NPC to focus on
	 * @param BlendTime - Time to blend (optional)
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera")
	void TransitionToTarget(AActor* NewTarget, float BlendTime = 0.5f);

	/**
	 * Update camera framing on the fly
	 * @param NewFraming - The new framing preset
	 * @param BlendTime - Time to blend
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera")
	void UpdateCameraFraming(EDialogueCameraFraming NewFraming, float BlendTime = 0.3f);

	/**
	 * Check if dialogue camera is currently active
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Camera")
	bool IsDialogueCameraActive() const { return bCameraActive; }

	/**
	 * Get current camera settings
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Camera")
	FDialogueCameraSettings GetCurrentCameraSettings() const;

	/**
	 * Get settings for a specific framing preset
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Camera")
	FDialogueCameraSettings GetFramingSettings(EDialogueCameraFraming Framing) const;

	/**
	 * Set settings for a specific framing preset
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Camera")
	void SetFramingSettings(EDialogueCameraFraming Framing, const FDialogueCameraSettings& Settings);

	//~ End Public API

protected:
	/** Initialize default framing presets */
	void InitializeFramingPresets();

	/** Create dialogue camera actor */
	AActor* CreateDialogueCameraActor();

	/** Destroy dialogue camera actor */
	void DestroyDialogueCameraActor();

	/** Calculate camera position for target actor */
	FTransform CalculateCameraTransform(AActor* TargetActor, const FDialogueCameraSettings& Settings) const;

	/** Calculate camera position using specific player location (for stable framing) */
	FTransform CalculateCameraTransformWithPlayerLocation(AActor* TargetActor, const FDialogueCameraSettings& Settings, const FVector& PlayerLocation) const;

	/** Get focus point on target actor */
	FVector GetFocusPoint(AActor* TargetActor) const;

	/** Update camera position to track speaker */
	void UpdateCameraTracking();

	/** Convert blend mode to engine blend function */
	static EViewTargetBlendFunction ConvertBlendMode(EDialogueCameraBlendMode BlendMode);

	/** Apply blend function to alpha value (0-1) */
	float ApplyBlendFunction(float Alpha, EViewTargetBlendFunction BlendFunc) const;

	/** Find player camera manager */
	APlayerCameraManager* FindPlayerCameraManager() const;

	/** NEW v1.17.1: Subscribe to GameEventBus events */
	void SubscribeToGameEvents();

	/** NEW v1.17.1: Unsubscribe from GameEventBus events */
	void UnsubscribeFromGameEvents();

	/** NEW v1.17.1: Handle participant positioned event */
	void OnParticipantPositioned(const FGameEventPayload& Payload);

	/** NEW v1.17.1: Handle node entered event (speaker change) */
	void OnNodeEntered(const FGameEventPayload& Payload);

	/** NEW v1.17.1: Delayed camera update after positioning */
	void UpdateCameraAfterPositioning();

	/** NEW v1.17.1: Validate camera position (ensure target is in frame) */
	bool ValidateCameraPosition(const FTransform& CameraTransform, AActor* TargetActor) const;

	/** NEW v1.17.1: Check if actor is visible from camera position */
	bool IsActorVisibleFromCamera(const FTransform& CameraTransform, AActor* TargetActor, float& OutVisibilityScore) const;
};
