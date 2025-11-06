// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/DialogueCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueCamera, Log, All);

UDialogueCameraComponent::UDialogueCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Initialize default framing presets in constructor
	InitializeFramingPresets();
}

void UDialogueCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache player controller and camera manager
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC)
		{
			CachedPlayerController = PC;
			CachedCameraManager = PC->PlayerCameraManager;
		}
	}
}

void UDialogueCameraComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Ensure camera is deactivated on component destruction
	// BUT only if not already tearing down world (PIE end, level transition, etc.)
	UWorld* World = GetWorld();
	bool bIsTearingDown = !World || World->bIsTearingDown;
	
	if (bCameraActive && !bIsTearingDown)
	{
		DeactivateDialogueCamera();
	}
	else if (bCameraActive && bIsTearingDown)
	{
		// During teardown, just cleanup without accessing potentially destroyed actors
		UE_LOG(LogDialogueCamera, Log, TEXT("EndPlay during world teardown - skipping full deactivation"));
		
		// Stop blending and timers
		bIsBlending = false;
		SetComponentTickEnabled(false);
		
		if (TrackingTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(TrackingTimerHandle);
			TrackingTimerHandle.Invalidate();
		}
		
		// Destroy camera actor if it still exists
		if (DialogueCameraActor && !DialogueCameraActor->IsActorBeingDestroyed())
		{
			DialogueCameraActor->Destroy();
			DialogueCameraActor = nullptr;
		}
		
		bCameraActive = false;
	}

	Super::EndPlay(EndPlayReason);
}

void UDialogueCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Handle camera blending
	if (bIsBlending && DialogueCameraActor)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		float ElapsedTime = CurrentTime - BlendStartTime;
		
		if (ElapsedTime >= BlendDuration)
		{
			// Blend complete - snap to final position
			DialogueCameraActor->SetActorTransform(BlendTargetTransform);
			bIsBlending = false;
			
			// Set final FOV
			if (DialogueCameraComponent && TargetFOV > 0.0f)
			{
				DialogueCameraComponent->SetFieldOfView(TargetFOV);
			}
			
			// Disable tick if not needed for tracking
			if (!bTrackSpeaker || TrackingUpdateRate <= 0.0f)
			{
				SetComponentTickEnabled(false);
			}
			else
			{
				// Start tracking now that blend is complete
				GetWorld()->GetTimerManager().SetTimer(
					TrackingTimerHandle,
					this,
					&UDialogueCameraComponent::UpdateCameraTracking,
					TrackingUpdateRate,
					true
				);
			}
			
			UE_LOG(LogDialogueCamera, Log, TEXT("Camera blend completed"));
		}
		else
		{
			// Calculate blend alpha based on blend function
			float Alpha = ElapsedTime / BlendDuration;
			Alpha = ApplyBlendFunction(Alpha, CurrentBlendFunction);
			
			// Interpolate transform
			FVector BlendedLocation = FMath::Lerp(
				BlendStartTransform.GetLocation(), 
				BlendTargetTransform.GetLocation(), 
				Alpha
			);
			
			FQuat BlendedRotation = FQuat::Slerp(
				BlendStartTransform.GetRotation(), 
				BlendTargetTransform.GetRotation(), 
				Alpha
			);
			
			DialogueCameraActor->SetActorLocationAndRotation(BlendedLocation, BlendedRotation);
			
			// Smoothly interpolate FOV as well
			if (DialogueCameraComponent && TargetFOV > 0.0f)
			{
				float BlendedFOV = FMath::Lerp(StartFOV, TargetFOV, Alpha);
				DialogueCameraComponent->SetFieldOfView(BlendedFOV);
			}
		}
	}
}

bool UDialogueCameraComponent::ActivateDialogueCamera(AActor* TargetActor, const FDialogueCameraSettings& Settings)
{
	if (!bEnableCameraControl)
	{
		UE_LOG(LogDialogueCamera, Warning, TEXT("Camera control is disabled"));
		return false;
	}

	if (!TargetActor)
	{
		UE_LOG(LogDialogueCamera, Error, TEXT("ActivateDialogueCamera: TargetActor is null"));
		return false;
	}

	APlayerController* PC = CachedPlayerController.Get();
	if (!PC)
	{
		UE_LOG(LogDialogueCamera, Error, TEXT("ActivateDialogueCamera: No valid PlayerController"));
		return false;
	}

	// Log detailed information about the call
	UE_LOG(LogDialogueCamera, Log, TEXT("=== ActivateDialogueCamera Called ==="));
	UE_LOG(LogDialogueCamera, Log, TEXT("Target Actor: %s at location %s"), 
		*TargetActor->GetName(), *TargetActor->GetActorLocation().ToCompactString());
	UE_LOG(LogDialogueCamera, Log, TEXT("Settings - Distance: %.1f, HeightOffset: %.1f, AngleOffset: %.1f, BlendMode: %d"),
		Settings.Distance, Settings.HeightOffset, Settings.AngleOffset, (int32)Settings.BlendMode);

	// Store original view target if not already active
	if (!bCameraActive)
	{
		OriginalViewTarget = PC->GetViewTarget();
		if (CachedCameraManager.IsValid())
		{
			OriginalFOV = CachedCameraManager->GetFOVAngle();
			
			// Store original player camera location for stable tracking
			OriginalPlayerLocation = CachedCameraManager->GetCameraLocation();
		}
		UE_LOG(LogDialogueCamera, Log, TEXT("Stored original view target: %s"), 
			OriginalViewTarget.IsValid() ? *OriginalViewTarget->GetName() : TEXT("None"));
		UE_LOG(LogDialogueCamera, Log, TEXT("Stored original player location: %s"), 
			*OriginalPlayerLocation.ToCompactString());
	}

	// Create or reuse dialogue camera actor
	if (!DialogueCameraActor)
	{
		DialogueCameraActor = CreateDialogueCameraActor();
		if (!DialogueCameraActor)
		{
			UE_LOG(LogDialogueCamera, Error, TEXT("Failed to create dialogue camera actor"));
			return false;
		}
		UE_LOG(LogDialogueCamera, Log, TEXT("Created new dialogue camera actor"));
	}

	// Store current target
	CurrentTarget = TargetActor;

	// Calculate target camera transform
	FTransform TargetCameraTransform = CalculateCameraTransform(TargetActor, Settings);
	
	// For smooth blending, start camera at current view location
	EViewTargetBlendFunction BlendFunc = ConvertBlendMode(Settings.BlendMode);
	bool bShouldBlend = (BlendFunc != VTBlend_PreBlended && Settings.BlendTime > 0.0f);
	
	if (bShouldBlend && PC->PlayerCameraManager)
	{
		// Get current camera location and rotation with HIGH PRECISION
		FVector CurrentCameraLocation = PC->PlayerCameraManager->GetCameraLocation();
		FRotator CurrentCameraRotation = PC->PlayerCameraManager->GetCameraRotation();
		
		// IMPORTANT: Also match the FOV immediately to avoid visual pop
		if (DialogueCameraComponent)
		{
			float CurrentFOV = PC->PlayerCameraManager->GetFOVAngle();
			DialogueCameraComponent->SetFieldOfView(CurrentFOV);
		}
		
		// Position dialogue camera EXACTLY at current view location
		DialogueCameraActor->SetActorLocationAndRotation(CurrentCameraLocation, CurrentCameraRotation);
		
		UE_LOG(LogDialogueCamera, Log, TEXT("Starting blend from current camera position: %s (Rot: %s, FOV: %.1f)"), 
			*CurrentCameraLocation.ToCompactString(), 
			*CurrentCameraRotation.ToCompactString(),
			PC->PlayerCameraManager->GetFOVAngle());
		
		// Immediately switch to dialogue camera (no blend yet - camera is at same position)
		PC->SetViewTarget(DialogueCameraActor);
		
		// Now smoothly move the dialogue camera to target position using Tick
		bIsBlending = true;
		BlendStartTime = GetWorld()->GetTimeSeconds();
		BlendDuration = Settings.BlendTime;
		BlendStartTransform = DialogueCameraActor->GetActorTransform();
		BlendTargetTransform = TargetCameraTransform;
		CurrentBlendFunction = BlendFunc;
		
		// Store FOV for smooth blending
		StartFOV = PC->PlayerCameraManager->GetFOVAngle();
		TargetFOV = (Settings.FOVOverride > 0.0f) ? Settings.FOVOverride : OriginalFOV;

		// Enable tick for smooth blending
		SetComponentTickEnabled(true);
		
		UE_LOG(LogDialogueCamera, Log, TEXT("Blending dialogue camera to target over %.2f seconds (Mode: %d)"), 
			Settings.BlendTime, (int32)Settings.BlendMode);
	}
	else
	{
		// Instant positioning - just set transform directly
		DialogueCameraActor->SetActorTransform(TargetCameraTransform);
		PC->SetViewTarget(DialogueCameraActor);
		
		UE_LOG(LogDialogueCamera, Log, TEXT("Instant cut to dialogue camera at: %s"), 
			*TargetCameraTransform.GetLocation().ToCompactString());
	}

	UE_LOG(LogDialogueCamera, Log, TEXT("Target camera position: %s with rotation: %s"),
		*TargetCameraTransform.GetLocation().ToCompactString(), 
		*TargetCameraTransform.Rotator().ToCompactString());

	// Set FOV if overridden
	if (Settings.FOVOverride > 0.0f && DialogueCameraComponent)
	{
		DialogueCameraComponent->SetFieldOfView(Settings.FOVOverride);
		UE_LOG(LogDialogueCamera, Log, TEXT("Set FOV to: %.1f"), Settings.FOVOverride);
	}
	else if (DialogueCameraComponent)
	{
		DialogueCameraComponent->SetFieldOfView(OriginalFOV);
	}

	bCameraActive = true;

	// Start speaker tracking if enabled (but not while blending)
	if (bTrackSpeaker && TrackingUpdateRate > 0.0f && !bIsBlending)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TrackingTimerHandle,
			this,
			&UDialogueCameraComponent::UpdateCameraTracking,
			TrackingUpdateRate,
			true
		);
		UE_LOG(LogDialogueCamera, Log, TEXT("Started camera tracking (update rate: %.2f)"), TrackingUpdateRate);
	}

	UE_LOG(LogDialogueCamera, Log, TEXT("=== Dialogue camera activated successfully ==="));
	return true;
}

bool UDialogueCameraComponent::ActivateDialogueCameraWithFraming(AActor* TargetActor, EDialogueCameraFraming Framing)
{
	FDialogueCameraSettings Settings = GetFramingSettings(Framing);
	return ActivateDialogueCamera(TargetActor, Settings);
}

bool UDialogueCameraComponent::ActivateDialogueCameraSimple(AActor* TargetActor)
{
	// Use Medium shot preset by default
	return ActivateDialogueCameraWithFraming(TargetActor, EDialogueCameraFraming::Medium);
}

void UDialogueCameraComponent::DeactivateDialogueCamera()
{
	if (!bCameraActive)
	{
		return;
	}

	// Stop blending
	bIsBlending = false;
	SetComponentTickEnabled(false);

	// Stop tracking timer
	UWorld* World = GetWorld();
	if (World && TrackingTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(TrackingTimerHandle);
		TrackingTimerHandle.Invalidate();
	}

	// Check if world is tearing down (PIE end, level transition, etc.)
	bool bIsTearingDown = !World || World->bIsTearingDown;
	
	APlayerController* PC = CachedPlayerController.Get();
	AActor* OriginalTarget = OriginalViewTarget.Get();

	// Only restore camera if not tearing down and we have valid references
	if (!bIsTearingDown && PC && bRestoreOriginalCamera)
	{
		if (OriginalTarget && !OriginalTarget->IsActorBeingDestroyed())
		{
			// Blend back to original view target
			if (RestoreBlendTime > 0.0f)
			{
				PC->SetViewTargetWithBlend(OriginalTarget, RestoreBlendTime, VTBlend_EaseInOut);
			}
			else
			{
				PC->SetViewTarget(OriginalTarget);
			}

			UE_LOG(LogDialogueCamera, Log, TEXT("Restored original view target: %s"), *OriginalTarget->GetName());
		}
		else
		{
			// Fallback to player pawn
			APawn* PlayerPawn = PC->GetPawn();
			if (PlayerPawn && !PlayerPawn->IsActorBeingDestroyed())
			{
				PC->SetViewTarget(PlayerPawn);
				UE_LOG(LogDialogueCamera, Log, TEXT("Restored view to player pawn"));
			}
		}

		// Restore original FOV
		if (CachedCameraManager.IsValid())
		{
			CachedCameraManager->SetFOV(OriginalFOV);
		}
	}
	else if (bIsTearingDown)
	{
		UE_LOG(LogDialogueCamera, Log, TEXT("Skipping camera restoration during world teardown"));
	}

	// Cleanup camera actor
	if (DialogueCameraActor && !DialogueCameraActor->IsActorBeingDestroyed())
	{
		DestroyDialogueCameraActor();
	}

	bCameraActive = false;
	CurrentTarget.Reset();
	OriginalViewTarget.Reset();

	UE_LOG(LogDialogueCamera, Log, TEXT("Dialogue camera deactivated"));
}

void UDialogueCameraComponent::TransitionToTarget(AActor* NewTarget, float BlendTime)
{
	if (!bCameraActive || !NewTarget)
	{
		return;
	}

	CurrentTarget = NewTarget;

	FDialogueCameraSettings CurrentSettings = GetCurrentCameraSettings();
	
	// IMPORTANT: Update OriginalPlayerLocation for new target to prevent camera drift
	if (CachedPlayerController.IsValid() && CachedPlayerController->PlayerCameraManager)
	{
		OriginalPlayerLocation = CachedPlayerController->PlayerCameraManager->GetCameraLocation();
		UE_LOG(LogDialogueCamera, Log, TEXT("Updated original player location for new target: %s"), 
			*OriginalPlayerLocation.ToCompactString());
	}
	
	FTransform TargetTransform = CalculateCameraTransform(NewTarget, CurrentSettings);
	
	// Smoothly move camera to new position
	if (DialogueCameraActor)
	{
		if (BlendTime > 0.0f)
		{
			// SMOOTH TRANSITION
			bIsBlending = true;
			BlendStartTime = GetWorld()->GetTimeSeconds();
			BlendDuration = BlendTime;
			BlendStartTransform = DialogueCameraActor->GetActorTransform();
			BlendTargetTransform = TargetTransform;
			CurrentBlendFunction = VTBlend_EaseInOut;
			
			// FOV stays the same during target transition
			if (DialogueCameraComponent)
			{
				StartFOV = DialogueCameraComponent->FieldOfView;
				TargetFOV = StartFOV;
			}
			
			SetComponentTickEnabled(true);
			
			UE_LOG(LogDialogueCamera, Log, TEXT("Smoothly transitioning to new target: %s over %.2f seconds"), 
				*NewTarget->GetName(), BlendTime);
		}
		else
		{
			// Instant transition
			DialogueCameraActor->SetActorTransform(TargetTransform);
			UE_LOG(LogDialogueCamera, Log, TEXT("Instantly transitioned to new target: %s"), *NewTarget->GetName());
		}
	}
}

void UDialogueCameraComponent::UpdateCameraFraming(EDialogueCameraFraming NewFraming, float BlendTime)
{
	if (!bCameraActive || !CurrentTarget.IsValid())
	{
		return;
	}

	FDialogueCameraSettings NewSettings = GetFramingSettings(NewFraming);
	
	// IMPORTANT: Store new settings as current so tracking uses them
	DefaultCameraSettings = NewSettings;
	
	// Calculate transform using ORIGINAL player location (not current camera!)
	FTransform TargetTransform = CalculateCameraTransformWithPlayerLocation(CurrentTarget.Get(), NewSettings, OriginalPlayerLocation);
	
	if (DialogueCameraActor)
	{
		if (BlendTime > 0.0f)
		{
			// SMOOTH TRANSITION using the same blend system as activation
			bIsBlending = true;
			BlendStartTime = GetWorld()->GetTimeSeconds();
			BlendDuration = BlendTime;
			BlendStartTransform = DialogueCameraActor->GetActorTransform();
			BlendTargetTransform = TargetTransform;
			CurrentBlendFunction = VTBlend_EaseInOut; // Smooth ease for framing changes
			
			// Store FOV for smooth blending
			if (DialogueCameraComponent)
			{
				StartFOV = DialogueCameraComponent->FieldOfView;
				TargetFOV = (NewSettings.FOVOverride > 0.0f) ? NewSettings.FOVOverride : OriginalFOV;
			}
			
			// Enable tick for blending
			SetComponentTickEnabled(true);
			
			UE_LOG(LogDialogueCamera, Log, TEXT("Smoothly transitioning to %d framing over %.2f seconds (using original player location)"), 
				(int32)NewFraming, BlendTime);
		}
		else
		{
			// Instant change
			DialogueCameraActor->SetActorTransform(TargetTransform);
			
			// Update FOV immediately
			if (NewSettings.FOVOverride > 0.0f && DialogueCameraComponent)
			{
				DialogueCameraComponent->SetFieldOfView(NewSettings.FOVOverride);
			}
			
			UE_LOG(LogDialogueCamera, Log, TEXT("Instantly changed to %d framing"), (int32)NewFraming);
		}
	}
}

FDialogueCameraSettings UDialogueCameraComponent::GetCurrentCameraSettings() const
{
	return DefaultCameraSettings;
}

FDialogueCameraSettings UDialogueCameraComponent::GetFramingSettings(EDialogueCameraFraming Framing) const
{
	if (const FDialogueCameraSettings* Settings = FramingPresets.Find(Framing))
	{
		return *Settings;
	}
	return DefaultCameraSettings;
}

void UDialogueCameraComponent::SetFramingSettings(EDialogueCameraFraming Framing, const FDialogueCameraSettings& Settings)
{
	FramingPresets.Add(Framing, Settings);
}

void UDialogueCameraComponent::InitializeFramingPresets()
{
	// Close-Up preset
	FDialogueCameraSettings CloseUp;
	CloseUp.Framing = EDialogueCameraFraming::CloseUp;
	CloseUp.Distance = 120.0f;
	CloseUp.HeightOffset = 10.0f;  // 
	CloseUp.AngleOffset = 15.0f;
	CloseUp.PitchAdjustment = -5.0f;
	CloseUp.FOVOverride = 60.0f;
	CloseUp.bUseSpringArm = false;
	CloseUp.bEnableCollision = false;
	FramingPresets.Add(EDialogueCameraFraming::CloseUp, CloseUp);

	// Medium Shot preset
	FDialogueCameraSettings Medium;
	Medium.Framing = EDialogueCameraFraming::Medium;
	Medium.Distance = 200.0f;
	Medium.HeightOffset = 20.0f;  // 
	Medium.AngleOffset = 25.0f;
	Medium.PitchAdjustment = -3.0f;
	Medium.FOVOverride = 70.0f;
	Medium.bUseSpringArm = false;
	Medium.bEnableCollision = false;
	FramingPresets.Add(EDialogueCameraFraming::Medium, Medium);

	// Full Shot preset
	FDialogueCameraSettings Full;
	Full.Framing = EDialogueCameraFraming::Full;
	Full.Distance = 350.0f;
	Full.HeightOffset = 30.0f;  // 
	Full.AngleOffset = 30.0f;
	Full.PitchAdjustment = 0.0f;
	Full.FOVOverride = 80.0f;
	Full.bUseSpringArm = false;
	Full.bEnableCollision = false;
	FramingPresets.Add(EDialogueCameraFraming::Full, Full);

	// Over-Shoulder preset
	FDialogueCameraSettings OverShoulder;
	OverShoulder.Framing = EDialogueCameraFraming::OverShoulder;
	OverShoulder.Distance = 180.0f;
	OverShoulder.HeightOffset = 15.0f;  // 
	OverShoulder.AngleOffset = 45.0f;
	OverShoulder.PitchAdjustment = -2.0f;
	OverShoulder.FOVOverride = 65.0f;
	OverShoulder.bUseSpringArm = false;
	OverShoulder.bEnableCollision = false;
	FramingPresets.Add(EDialogueCameraFraming::OverShoulder, OverShoulder);

	// Two-Shot preset
	FDialogueCameraSettings TwoShot;
	TwoShot.Framing = EDialogueCameraFraming::TwoShot;
	TwoShot.Distance = 300.0f;
	TwoShot.HeightOffset = 25.0f;  // 
	TwoShot.AngleOffset = 0.0f; // Side view
	TwoShot.PitchAdjustment = 0.0f;
	TwoShot.FOVOverride = 75.0f;
	TwoShot.bUseSpringArm = false;
	TwoShot.bEnableCollision = false;
	FramingPresets.Add(EDialogueCameraFraming::TwoShot, TwoShot);

	// Set default to Medium
	DefaultCameraSettings = Medium;
}

AActor* UDialogueCameraComponent::CreateDialogueCameraActor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	// Spawn camera actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* CameraActor = World->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	USceneComponent* Root = NewObject<USceneComponent>(CameraActor, TEXT("DialogueRoot"));
	Root->RegisterComponent();
	CameraActor->SetRootComponent(Root);
	if (!CameraActor)
	{
		return nullptr;
	}

#if WITH_EDITOR
	CameraActor->SetActorLabel(TEXT("DialogueCamera"));
#endif

	// If using spring arm, create it as the intermediate component
	if (DefaultCameraSettings.bUseSpringArm)
	{
		SpringArmComponent = NewObject<USpringArmComponent>(CameraActor, USpringArmComponent::StaticClass(), TEXT("DialogueSpringArm"));
		if (SpringArmComponent)
		{
			SpringArmComponent->RegisterComponent();
			SpringArmComponent->AttachToComponent(CameraActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			SpringArmComponent->TargetArmLength = DefaultCameraSettings.Distance;
			SpringArmComponent->bDoCollisionTest = DefaultCameraSettings.bEnableCollision;
			SpringArmComponent->bUsePawnControlRotation = false;
			SpringArmComponent->bInheritPitch = false;
			SpringArmComponent->bInheritYaw = false;
			SpringArmComponent->bInheritRoll = false;
			SpringArmComponent->bEnableCameraLag = false;
			SpringArmComponent->bEnableCameraRotationLag = false;
			
			// Create camera and attach to spring arm
			DialogueCameraComponent = NewObject<UCameraComponent>(CameraActor, UCameraComponent::StaticClass(), TEXT("DialogueCameraComponent"));
			if (DialogueCameraComponent)
			{
				DialogueCameraComponent->RegisterComponent();
				DialogueCameraComponent->AttachToComponent(SpringArmComponent, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
				DialogueCameraComponent->bUsePawnControlRotation = false;
			}
		}
	}
	else
	{
		// Create camera component directly on actor
		DialogueCameraComponent = NewObject<UCameraComponent>(CameraActor, UCameraComponent::StaticClass(), TEXT("DialogueCameraComponent"));
		if (DialogueCameraComponent)
		{
			DialogueCameraComponent->RegisterComponent();
			DialogueCameraComponent->AttachToComponent(CameraActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			DialogueCameraComponent->bUsePawnControlRotation = false;
		}
	}

	return CameraActor;
}

void UDialogueCameraComponent::DestroyDialogueCameraActor()
{
	if (DialogueCameraActor)
	{
		DialogueCameraActor->Destroy();
		DialogueCameraActor = nullptr;
	}

	DialogueCameraComponent = nullptr;
	SpringArmComponent = nullptr;
}

FTransform UDialogueCameraComponent::CalculateCameraTransform(AActor* TargetActor, const FDialogueCameraSettings& Settings) const
{
	if (!TargetActor)
	{
		return FTransform::Identity;
	}

	// Get focus point on target NPC (e.g., head position)
	FVector FocusPoint = GetFocusPoint(TargetActor);

	if (Settings.bUseSpringArm && SpringArmComponent)
	{
		// Get the actor's forward vector (facing direction)
		FRotator ActorRotation = TargetActor->GetActorRotation();
		
		// With SpringArm:
		// 1. Position camera actor AT the focus point
		// 2. Rotate it so SpringArm extends in the desired direction
		// 3. SpringArm will automatically push camera backwards by TargetArmLength
		
		// Calculate desired camera angle (where we want camera to be, relative to NPC)
		FRotator DesiredCameraAngle = ActorRotation;
		DesiredCameraAngle.Yaw += Settings.AngleOffset;
		
		// SpringArm extends BACKWARD from its rotation
		// So if we want camera at angle X, we rotate SpringArm to angle X
		// and it will push camera backwards along that direction
		FRotator SpringArmRotation = DesiredCameraAngle;
		SpringArmRotation.Pitch = Settings.PitchAdjustment;
		
		// Update SpringArm length dynamically
		if (SpringArmComponent)
		{
			SpringArmComponent->TargetArmLength = Settings.Distance;
		}
		
		// Position: Focus point with height offset
		FVector CameraActorLocation = FocusPoint;
		CameraActorLocation.Z += Settings.HeightOffset;
		
		UE_LOG(LogDialogueCamera, Log, TEXT("SpringArm Mode - Focus: %s, Actor Loc: %s, Rotation: %s, ArmLength: %.1f"),
			*FocusPoint.ToCompactString(), *CameraActorLocation.ToCompactString(), 
			*SpringArmRotation.ToCompactString(), Settings.Distance);
		
		return FTransform(SpringArmRotation, CameraActorLocation, FVector::OneVector);
	}
	else
	{
		// Manual positioning without SpringArm
		// Position camera between player and NPC, but in HORIZONTAL plane
		
		// Get player CAMERA location (not pawn location!) for better first-person support
		FVector PlayerLocation = FVector::ZeroVector;
		if (CachedPlayerController.IsValid())
		{
			if (CachedPlayerController->PlayerCameraManager)
			{
				// Use camera location for first-person games
				PlayerLocation = CachedPlayerController->PlayerCameraManager->GetCameraLocation();
			}
			else if (CachedPlayerController->GetPawn())
			{
				// Fallback to pawn location for third-person
				PlayerLocation = CachedPlayerController->GetPawn()->GetActorLocation();
			}
		}
		
		return CalculateCameraTransformWithPlayerLocation(TargetActor, Settings, PlayerLocation);
	}
}

FTransform UDialogueCameraComponent::CalculateCameraTransformWithPlayerLocation(AActor* TargetActor, const FDialogueCameraSettings& Settings, const FVector& PlayerLocation) const
{
	if (!TargetActor)
	{
		return FTransform::Identity;
	}

	// Get focus point on target NPC
	FVector FocusPoint = GetFocusPoint(TargetActor);

	if (Settings.bUseSpringArm && SpringArmComponent)
	{
		// SpringArm mode
		FRotator ActorRotation = TargetActor->GetActorRotation();
		FRotator DesiredCameraAngle = ActorRotation;
		DesiredCameraAngle.Yaw += Settings.AngleOffset;
		
		FRotator SpringArmRotation = DesiredCameraAngle;
		SpringArmRotation.Pitch = Settings.PitchAdjustment;
		
		if (SpringArmComponent)
		{
			SpringArmComponent->TargetArmLength = Settings.Distance;
		}
		
		FVector CameraActorLocation = FocusPoint;
		CameraActorLocation.Z += Settings.HeightOffset;
		
		UE_LOG(LogDialogueCamera, Log, TEXT("SpringArm Mode - Focus: %s, Actor Loc: %s, Rotation: %s, ArmLength: %.1f"),
			*FocusPoint.ToCompactString(), *CameraActorLocation.ToCompactString(), 
			*SpringArmRotation.ToCompactString(), Settings.Distance);
		
		return FTransform(SpringArmRotation, CameraActorLocation, FVector::OneVector);
	}
	else
	{
		// Manual positioning without SpringArm
		// Position camera between player and NPC, but in HORIZONTAL plane
		
		// Work in HORIZONTAL plane only (ignore Z difference between player and NPC)
		FVector FocusPoint2D = FocusPoint;
		FocusPoint2D.Z = 0.0f;
		
		FVector PlayerLocation2D = PlayerLocation;
		PlayerLocation2D.Z = 0.0f;
		
		// Calculate horizontal direction from NPC to Player
		FVector NPCToPlayer2D = (PlayerLocation2D - FocusPoint2D).GetSafeNormal();
		
		// If player is at same position, use NPC's forward vector as fallback
		if (NPCToPlayer2D.IsNearlyZero())
		{
			FRotator ActorRotation = TargetActor->GetActorRotation();
			FVector ForwardVector = ActorRotation.Vector();
			NPCToPlayer2D = FVector(ForwardVector.X, ForwardVector.Y, 0.0f).GetSafeNormal();
		}
		
		// Create rotation from horizontal NPC-to-Player vector
		FRotator BaseRotation = NPCToPlayer2D.Rotation();
		
		// Apply angle offset (rotate around Z axis)
		FRotator CameraAngle = BaseRotation;
		CameraAngle.Yaw += Settings.AngleOffset;
		
		// Get horizontal direction for camera placement
		FVector CameraDirection2D = CameraAngle.Vector();
		CameraDirection2D.Z = 0.0f; // Ensure it's horizontal
		CameraDirection2D.Normalize();
		
		// Position camera at distance along horizontal direction
		FVector CameraLocation = FocusPoint + (CameraDirection2D * Settings.Distance);
		
		// Add height offset RELATIVE TO FOCUS POINT (not relative to ground)
		CameraLocation.Z = FocusPoint.Z + Settings.HeightOffset;
		
		// Make camera look at focus point
		FRotator CameraRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, FocusPoint);
		
		// Apply pitch adjustment
		CameraRotation.Pitch += Settings.PitchAdjustment;
		
		UE_LOG(LogDialogueCamera, Log, TEXT("Manual Mode - Focus: %s (Z=%.1f), PlayerLoc: %s"),
			*FocusPoint.ToCompactString(), FocusPoint.Z, *PlayerLocation.ToCompactString());
		UE_LOG(LogDialogueCamera, Log, TEXT("NPCToPlayer2D: %s, CameraDir2D: %s"),
			*NPCToPlayer2D.ToCompactString(), *CameraDirection2D.ToCompactString());
		UE_LOG(LogDialogueCamera, Log, TEXT("Camera: Loc=%s (Z=%.1f), Rot=%s"),
			*CameraLocation.ToCompactString(), CameraLocation.Z, *CameraRotation.ToCompactString());
		
		return FTransform(CameraRotation, CameraLocation, FVector::OneVector);
	}
}

FVector UDialogueCameraComponent::GetFocusPoint(AActor* TargetActor) const
{
	if (!TargetActor)
	{
		return FVector::ZeroVector;
	}

	FVector FocusPoint = TargetActor->GetActorLocation();

	// Try to find socket on skeletal mesh (e.g., "head" socket)
	if (FocusSocketName != NAME_None)
	{
		if (ACharacter* Character = Cast<ACharacter>(TargetActor))
		{
			if (USkeletalMeshComponent* Mesh = Character->GetMesh())
			{
				if (Mesh->DoesSocketExist(FocusSocketName))
				{
					FocusPoint = Mesh->GetSocketLocation(FocusSocketName);
					UE_LOG(LogDialogueCamera, Verbose, TEXT("Using socket '%s' for focus point: %s"),
						*FocusSocketName.ToString(), *FocusPoint.ToCompactString());
					return FocusPoint;
				}
				else
				{
					UE_LOG(LogDialogueCamera, Warning, TEXT("Socket '%s' not found on character '%s'"),
						*FocusSocketName.ToString(), *TargetActor->GetName());
				}
			}
		}
	}

	// Fallback: use offset from actor location (default head height)
	// Apply full offset, not just Z
	FocusPoint += DefaultCameraSettings.FocusPointOffset;

	UE_LOG(LogDialogueCamera, Verbose, TEXT("Using actor location with offset for focus point: %s"),
		*FocusPoint.ToCompactString());

	return FocusPoint;
}

void UDialogueCameraComponent::UpdateCameraTracking()
{
	if (!bCameraActive || !CurrentTarget.IsValid() || !DialogueCameraActor)
	{
		return;
	}

	// IMPORTANT: Don't recalculate based on current player camera position!
	// Use the ORIGINAL player position from when dialogue started to maintain stable framing
	
	// Get current settings
	FDialogueCameraSettings CurrentSettings = GetCurrentCameraSettings();
	
	// Calculate new transform based on NPC movement ONLY
	// Using the same PlayerLocation we used initially (stored in OriginalPlayerLocation)
	FVector FocusPoint = GetFocusPoint(CurrentTarget.Get());
	
	// Work in HORIZONTAL plane only
	FVector FocusPoint2D = FocusPoint;
	FocusPoint2D.Z = 0.0f;
	
	FVector OriginalPlayerLocation2D = OriginalPlayerLocation;
	OriginalPlayerLocation2D.Z = 0.0f;
	
	// Calculate horizontal direction from NPC to ORIGINAL player position
	FVector NPCToPlayer2D = (OriginalPlayerLocation2D - FocusPoint2D).GetSafeNormal();
	
	if (NPCToPlayer2D.IsNearlyZero())
	{
		FRotator ActorRotation = CurrentTarget->GetActorRotation();
		FVector ForwardVector = ActorRotation.Vector();
		NPCToPlayer2D = FVector(ForwardVector.X, ForwardVector.Y, 0.0f).GetSafeNormal();
	}
	
	// Create rotation from horizontal NPC-to-Player vector
	FRotator BaseRotation = NPCToPlayer2D.Rotation();
	
	// Apply angle offset
	FRotator CameraAngle = BaseRotation;
	CameraAngle.Yaw += CurrentSettings.AngleOffset;
	
	// Get horizontal direction for camera placement
	FVector CameraDirection2D = CameraAngle.Vector();
	CameraDirection2D.Z = 0.0f;
	CameraDirection2D.Normalize();
	
	// Position camera at distance along horizontal direction
	FVector NewLocation = FocusPoint + (CameraDirection2D * CurrentSettings.Distance);
	NewLocation.Z = FocusPoint.Z + CurrentSettings.HeightOffset;
	
	// Make camera look at focus point
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(NewLocation, FocusPoint);
	NewRotation.Pitch += CurrentSettings.PitchAdjustment;
	
	FTransform NewTransform(NewRotation, NewLocation, FVector::OneVector);

	// Get current camera transform
	FTransform CurrentTransform = DialogueCameraActor->GetActorTransform();
	
	// Check if target has moved significantly enough to warrant an update
	float LocationDelta = FVector::Dist(CurrentTransform.GetLocation(), NewTransform.GetLocation());
	float RotationDelta = CurrentTransform.GetRotation().AngularDistance(NewTransform.GetRotation());
	
	// Only update if movement is significant (reduces jitter from small movements)
	const float MinLocationDelta = 5.0f; // 5cm threshold
	const float MinRotationDelta = FMath::DegreesToRadians(2.0f); // 2 degree threshold
	
	if (LocationDelta < MinLocationDelta && RotationDelta < MinRotationDelta)
	{
		// Movement too small, skip update
		return;
	}

	// Smooth interpolation with very small alpha for gentle tracking
	FTransform BlendedTransform;
	
	// Use much smaller alpha for smoother tracking (0.05 = very smooth)
	float Alpha = 0.05f;
	
	BlendedTransform.SetLocation(FMath::Lerp(CurrentTransform.GetLocation(), NewTransform.GetLocation(), Alpha));
	BlendedTransform.SetRotation(FQuat::Slerp(CurrentTransform.GetRotation(), NewTransform.GetRotation(), Alpha));
	BlendedTransform.SetScale3D(FVector::OneVector);

	DialogueCameraActor->SetActorTransform(BlendedTransform);
	
	UE_LOG(LogDialogueCamera, Verbose, TEXT("Camera tracking updated - LocationDelta: %.2f, RotationDelta: %.2f"), 
		LocationDelta, FMath::RadiansToDegrees(RotationDelta));
}

EViewTargetBlendFunction UDialogueCameraComponent::ConvertBlendMode(EDialogueCameraBlendMode BlendMode)
{
	switch (BlendMode)
	{
	case EDialogueCameraBlendMode::Linear:
		return VTBlend_Linear;
	case EDialogueCameraBlendMode::Cubic:
		return VTBlend_Cubic;
	case EDialogueCameraBlendMode::EaseIn:
		return VTBlend_EaseIn;
	case EDialogueCameraBlendMode::EaseOut:
		return VTBlend_EaseOut;
	case EDialogueCameraBlendMode::EaseInOut:
		return VTBlend_EaseInOut;
	case EDialogueCameraBlendMode::Cut:
		return VTBlend_PreBlended;
	default:
		return VTBlend_Linear;
	}
}

APlayerCameraManager* UDialogueCameraComponent::FindPlayerCameraManager() const
{
	if (CachedCameraManager.IsValid())
	{
		return CachedCameraManager.Get();
	}

	UWorld* World = GetWorld();
	if (World)
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			return PC->PlayerCameraManager;
		}
	}

	return nullptr;
}

float UDialogueCameraComponent::ApplyBlendFunction(float Alpha, EViewTargetBlendFunction BlendFunc) const
{
	// Apply easing function to alpha
	switch (BlendFunc)
	{
	case VTBlend_Linear:
		return Alpha; // No modification
		
	case VTBlend_Cubic:
		// Cubic ease in/out
		if (Alpha < 0.5f)
		{
			return 4.0f * Alpha * Alpha * Alpha;
		}
		else
		{
			float F = ((2.0f * Alpha) - 2.0f);
			return 0.5f * F * F * F + 1.0f;
		}
		
	case VTBlend_EaseIn:
		// Quadratic ease in
		return Alpha * Alpha;
		
	case VTBlend_EaseOut:
		// Quadratic ease out
		return Alpha * (2.0f - Alpha);
		
	case VTBlend_EaseInOut:
		// Quadratic ease in/out
		if (Alpha < 0.5f)
		{
			return 2.0f * Alpha * Alpha;
		}
		else
		{
			return (-2.0f * Alpha * Alpha) + (4.0f * Alpha) - 1.0f;
		}
		
	default:
		return Alpha;
	}
}
