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
	if (bCameraActive)
	{
		DeactivateDialogueCamera();
	}

	Super::EndPlay(EndPlayReason);
}

void UDialogueCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Additional smooth tracking can be done here if needed
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

	// Store original view target if not already active
	if (!bCameraActive)
	{
		OriginalViewTarget = PC->GetViewTarget();
		if (CachedCameraManager.IsValid())
		{
			OriginalFOV = CachedCameraManager->GetFOVAngle();
		}
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
	}

	// Store current target
	CurrentTarget = TargetActor;

	// Calculate camera transform
	FTransform CameraTransform = CalculateCameraTransform(TargetActor, Settings);
	DialogueCameraActor->SetActorTransform(CameraTransform);

	// Set FOV if overridden
	if (Settings.FOVOverride > 0.0f && DialogueCameraComponent)
	{
		DialogueCameraComponent->SetFieldOfView(Settings.FOVOverride);
	}
	else if (DialogueCameraComponent)
	{
		DialogueCameraComponent->SetFieldOfView(OriginalFOV);
	}

	// Blend to dialogue camera
	EViewTargetBlendFunction BlendFunc = ConvertBlendMode(Settings.BlendMode);
	
	if (BlendFunc == VTBlend_PreBlended || Settings.BlendTime <= 0.0f)
	{
		// Instant cut
		PC->SetViewTarget(DialogueCameraActor);
	}
	else
	{
		// Smooth blend
		PC->SetViewTargetWithBlend(DialogueCameraActor, Settings.BlendTime, BlendFunc);
	}

	bCameraActive = true;

	// Start speaker tracking if enabled
	if (bTrackSpeaker && TrackingUpdateRate > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TrackingTimerHandle,
			this,
			&UDialogueCameraComponent::UpdateCameraTracking,
			TrackingUpdateRate,
			true
		);
	}

	UE_LOG(LogDialogueCamera, Log, TEXT("Dialogue camera activated for target: %s"), *TargetActor->GetName());
	return true;
}

bool UDialogueCameraComponent::ActivateDialogueCameraWithFraming(AActor* TargetActor, EDialogueCameraFraming Framing)
{
	FDialogueCameraSettings Settings = GetFramingSettings(Framing);
	return ActivateDialogueCamera(TargetActor, Settings);
}

void UDialogueCameraComponent::DeactivateDialogueCamera()
{
	if (!bCameraActive)
	{
		return;
	}

	// Stop tracking timer
	if (TrackingTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TrackingTimerHandle);
		TrackingTimerHandle.Invalidate();
	}

	APlayerController* PC = CachedPlayerController.Get();
	AActor* OriginalTarget = OriginalViewTarget.Get();

	if (PC && bRestoreOriginalCamera)
	{
		if (OriginalTarget)
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
			if (PC->GetPawn())
			{
				PC->SetViewTarget(PC->GetPawn());
				UE_LOG(LogDialogueCamera, Log, TEXT("Restored view to player pawn"));
			}
		}

		// Restore original FOV
		if (CachedCameraManager.IsValid())
		{
			CachedCameraManager->SetFOV(OriginalFOV);
		}
	}

	// Cleanup camera actor
	if (DialogueCameraActor)
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
	CurrentSettings.BlendTime = BlendTime;

	FTransform CameraTransform = CalculateCameraTransform(NewTarget, CurrentSettings);
	
	// Smoothly move camera to new position
	if (DialogueCameraActor)
	{
		// TODO: Implement smooth interpolation here if needed
		// For now, using instant positioning
		DialogueCameraActor->SetActorTransform(CameraTransform);
	}

	UE_LOG(LogDialogueCamera, Log, TEXT("Transitioning camera to new target: %s"), *NewTarget->GetName());
}

void UDialogueCameraComponent::UpdateCameraFraming(EDialogueCameraFraming NewFraming, float BlendTime)
{
	if (!bCameraActive || !CurrentTarget.IsValid())
	{
		return;
	}

	FDialogueCameraSettings NewSettings = GetFramingSettings(NewFraming);
	NewSettings.BlendTime = BlendTime;

	FTransform CameraTransform = CalculateCameraTransform(CurrentTarget.Get(), NewSettings);
	
	if (DialogueCameraActor)
	{
		DialogueCameraActor->SetActorTransform(CameraTransform);
	}

	// Update FOV if needed
	if (NewSettings.FOVOverride > 0.0f && DialogueCameraComponent)
	{
		DialogueCameraComponent->SetFieldOfView(NewSettings.FOVOverride);
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
	CloseUp.HeightOffset = 70.0f;
	CloseUp.AngleOffset = 15.0f;
	CloseUp.PitchAdjustment = -5.0f;
	CloseUp.FOVOverride = 60.0f;
	FramingPresets.Add(EDialogueCameraFraming::CloseUp, CloseUp);

	// Medium Shot preset
	FDialogueCameraSettings Medium;
	Medium.Framing = EDialogueCameraFraming::Medium;
	Medium.Distance = 200.0f;
	Medium.HeightOffset = 60.0f;
	Medium.AngleOffset = 25.0f;
	Medium.PitchAdjustment = -3.0f;
	Medium.FOVOverride = 70.0f;
	FramingPresets.Add(EDialogueCameraFraming::Medium, Medium);

	// Full Shot preset
	FDialogueCameraSettings Full;
	Full.Framing = EDialogueCameraFraming::Full;
	Full.Distance = 350.0f;
	Full.HeightOffset = 50.0f;
	Full.AngleOffset = 30.0f;
	Full.PitchAdjustment = 0.0f;
	Full.FOVOverride = 80.0f;
	FramingPresets.Add(EDialogueCameraFraming::Full, Full);

	// Over-Shoulder preset
	FDialogueCameraSettings OverShoulder;
	OverShoulder.Framing = EDialogueCameraFraming::OverShoulder;
	OverShoulder.Distance = 180.0f;
	OverShoulder.HeightOffset = 55.0f;
	OverShoulder.AngleOffset = 45.0f;
	OverShoulder.PitchAdjustment = -2.0f;
	OverShoulder.FOVOverride = 65.0f;
	FramingPresets.Add(EDialogueCameraFraming::OverShoulder, OverShoulder);

	// Two-Shot preset
	FDialogueCameraSettings TwoShot;
	TwoShot.Framing = EDialogueCameraFraming::TwoShot;
	TwoShot.Distance = 300.0f;
	TwoShot.HeightOffset = 60.0f;
	TwoShot.AngleOffset = 0.0f; // Side view
	TwoShot.PitchAdjustment = 0.0f;
	TwoShot.FOVOverride = 75.0f;
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
	if (!CameraActor)
	{
		return nullptr;
	}

#if WITH_EDITOR
	CameraActor->SetActorLabel(TEXT("DialogueCamera"));
#endif

	// Create camera component
	DialogueCameraComponent = NewObject<UCameraComponent>(CameraActor, UCameraComponent::StaticClass(), TEXT("DialogueCameraComponent"));
	if (DialogueCameraComponent)
	{
		DialogueCameraComponent->RegisterComponent();
		DialogueCameraComponent->AttachToComponent(CameraActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DialogueCameraComponent->bUsePawnControlRotation = false;
	}

	// Optionally create spring arm for collision
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

			// Attach camera to spring arm
			if (DialogueCameraComponent)
			{
				DialogueCameraComponent->AttachToComponent(SpringArmComponent, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
			}
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

	// Get focus point on target
	FVector FocusPoint = GetFocusPoint(TargetActor);

	// Calculate camera offset from focus point
	FRotator TargetRotation = TargetActor->GetActorRotation();
	
	// Add angle offset (horizontal)
	TargetRotation.Yaw += Settings.AngleOffset;
	
	// Calculate camera position
	FVector CameraOffset = TargetRotation.Vector() * -Settings.Distance; // Negative to place camera in front
	CameraOffset.Z += Settings.HeightOffset;
	
	FVector CameraLocation = FocusPoint + CameraOffset;

	// Calculate camera rotation to look at focus point
	FRotator CameraRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, FocusPoint);
	
	// Apply pitch adjustment
	CameraRotation.Pitch += Settings.PitchAdjustment;

	return FTransform(CameraRotation, CameraLocation, FVector::OneVector);
}

FVector UDialogueCameraComponent::GetFocusPoint(AActor* TargetActor) const
{
	if (!TargetActor)
	{
		return FVector::ZeroVector;
	}

	FVector FocusPoint = TargetActor->GetActorLocation();

	// Try to find socket on skeletal mesh
	if (FocusSocketName != NAME_None)
	{
		if (ACharacter* Character = Cast<ACharacter>(TargetActor))
		{
			if (USkeletalMeshComponent* Mesh = Character->GetMesh())
			{
				if (Mesh->DoesSocketExist(FocusSocketName))
				{
					FocusPoint = Mesh->GetSocketLocation(FocusSocketName);
					return FocusPoint;
				}
			}
		}
	}

	// Fallback: use offset from actor location
	FocusPoint += DefaultCameraSettings.FocusPointOffset;

	return FocusPoint;
}

void UDialogueCameraComponent::UpdateCameraTracking()
{
	if (!bCameraActive || !CurrentTarget.IsValid() || !DialogueCameraActor)
	{
		return;
	}

	// Recalculate camera position for current target
	FDialogueCameraSettings CurrentSettings = GetCurrentCameraSettings();
	FTransform NewTransform = CalculateCameraTransform(CurrentTarget.Get(), CurrentSettings);

	// Smooth interpolation
	FTransform CurrentTransform = DialogueCameraActor->GetActorTransform();
	FTransform BlendedTransform;
	
	float Alpha = FMath::Clamp(TrackingUpdateRate / CurrentSettings.BlendTime, 0.1f, 0.5f);
	
	BlendedTransform.SetLocation(FMath::Lerp(CurrentTransform.GetLocation(), NewTransform.GetLocation(), Alpha));
	BlendedTransform.SetRotation(FQuat::Slerp(CurrentTransform.GetRotation(), NewTransform.GetRotation(), Alpha));
	BlendedTransform.SetScale3D(FVector::OneVector);

	DialogueCameraActor->SetActorTransform(BlendedTransform);
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
