// Copyright Epic Games, Inc. All Rights Reserved.

#include "Library/DialoguePresentationLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

bool UDialoguePresentationLibrary::QuickActivateDialogueCamera(
	UObject* WorldContextObject,
	AActor* TargetNPC,
	EDialogueCameraFraming Framing)
{
	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return false;
	}

	UDialogueCameraComponent* CameraComp = PresentationComp->GetCameraComponent();
	if (!CameraComp || !TargetNPC)
	{
		return false;
	}

	return CameraComp->ActivateDialogueCameraWithFraming(TargetNPC, Framing);
}

void UDialoguePresentationLibrary::QuickDeactivateDialogueCamera(UObject* WorldContextObject)
{
	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return;
	}

	UDialogueCameraComponent* CameraComp = PresentationComp->GetCameraComponent();
	if (CameraComp && CameraComp->IsDialogueCameraActive())
	{
		CameraComp->DeactivateDialogueCamera();
	}
}

bool UDialoguePresentationLibrary::IsDialogueCameraActive(UObject* WorldContextObject)
{
	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return false;
	}

	UDialogueCameraComponent* CameraComp = PresentationComp->GetCameraComponent();
	return CameraComp && CameraComp->IsDialogueCameraActive();
}

void UDialoguePresentationLibrary::TransitionCameraToTarget(
	UObject* WorldContextObject,
	AActor* NewTarget,
	float BlendTime)
{
	if (!NewTarget)
	{
		return;
	}

	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return;
	}

	PresentationComp->TransitionToNewTarget(NewTarget, BlendTime);
}

bool UDialoguePresentationLibrary::QuickBlockInput(
	UObject* WorldContextObject,
	EInputBlockingMode Mode)
{
	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return false;
	}

	UDialogueInputBlockerComponent* InputBlocker = PresentationComp->GetInputBlockerComponent();
	if (!InputBlocker)
	{
		return false;
	}

	FDialogueInputBlockingSettings Settings;
	Settings.BlockingMode = Mode;
	return InputBlocker->BlockInput(Settings);
}

void UDialoguePresentationLibrary::QuickUnblockInput(UObject* WorldContextObject)
{
	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return;
	}

	UDialogueInputBlockerComponent* InputBlocker = PresentationComp->GetInputBlockerComponent();
	if (InputBlocker && InputBlocker->IsInputBlocked())
	{
		InputBlocker->UnblockInput();
	}
}

bool UDialoguePresentationLibrary::IsInputBlocked(UObject* WorldContextObject)
{
	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return false;
	}

	UDialogueInputBlockerComponent* InputBlocker = PresentationComp->GetInputBlockerComponent();
	return InputBlocker && InputBlocker->IsInputBlocked();
}

bool UDialoguePresentationLibrary::StartDialoguePresentation(
	UObject* WorldContextObject,
	AActor* TargetNPC,
	EDialoguePresentationMode Mode)
{
	if (!TargetNPC)
	{
		return false;
	}

	UDialoguePresentationComponent* PresentationComp = GetOrCreatePresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return false;
	}

	return PresentationComp->ActivatePresentation(TargetNPC, Mode);
}

void UDialoguePresentationLibrary::EndDialoguePresentation(UObject* WorldContextObject)
{
	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return;
	}

	if (PresentationComp->IsPresentationActive())
	{
		PresentationComp->DeactivatePresentation();
	}
}

bool UDialoguePresentationLibrary::IsPresentationActive(UObject* WorldContextObject)
{
	UDialoguePresentationComponent* PresentationComp = GetPlayerPresentationComponent(WorldContextObject);
	if (!PresentationComp)
	{
		return false;
	}

	return PresentationComp->IsPresentationActive();
}

UDialoguePresentationComponent* UDialoguePresentationLibrary::GetOrCreatePresentationComponent(UObject* WorldContextObject)
{
	APawn* PlayerPawn = GetPlayerPawn(WorldContextObject);
	if (!PlayerPawn)
	{
		return nullptr;
	}

	// Try to find existing component
	UDialoguePresentationComponent* PresentationComp = PlayerPawn->FindComponentByClass<UDialoguePresentationComponent>();
	
	if (!PresentationComp)
	{
		// Create new component if not found
		PresentationComp = NewObject<UDialoguePresentationComponent>(PlayerPawn, UDialoguePresentationComponent::StaticClass(), TEXT("DialoguePresentation"));
		if (PresentationComp)
		{
			PresentationComp->RegisterComponent();
		}
	}

	return PresentationComp;
}

FDialogueCameraSettings UDialoguePresentationLibrary::MakeDefaultCameraSettings(EDialogueCameraFraming Framing)
{
	FDialogueCameraSettings Settings;
	Settings.Framing = Framing;

	switch (Framing)
	{
	case EDialogueCameraFraming::CloseUp:
		Settings.Distance = 120.0f;
		Settings.HeightOffset = 70.0f;
		Settings.FOVOverride = 60.0f;
		break;

	case EDialogueCameraFraming::Medium:
		Settings.Distance = 200.0f;
		Settings.HeightOffset = 60.0f;
		Settings.FOVOverride = 70.0f;
		break;

	case EDialogueCameraFraming::Full:
		Settings.Distance = 350.0f;
		Settings.HeightOffset = 50.0f;
		Settings.FOVOverride = 80.0f;
		break;

	case EDialogueCameraFraming::OverShoulder:
		Settings.Distance = 180.0f;
		Settings.HeightOffset = 55.0f;
		Settings.AngleOffset = 45.0f;
		Settings.FOVOverride = 65.0f;
		break;

	case EDialogueCameraFraming::TwoShot:
		Settings.Distance = 300.0f;
		Settings.HeightOffset = 60.0f;
		Settings.AngleOffset = 0.0f;
		Settings.FOVOverride = 75.0f;
		break;

	default:
		break;
	}

	return Settings;
}

FDialogueInputBlockingSettings UDialoguePresentationLibrary::MakeDefaultInputSettings(EInputBlockingMode Mode)
{
	FDialogueInputBlockingSettings Settings;
	Settings.BlockingMode = Mode;

	switch (Mode)
	{
	case EInputBlockingMode::BlockAll:
		Settings.bBlockMovement = true;
		Settings.bBlockCamera = true;
		Settings.bBlockJump = true;
		Settings.bBlockCombat = true;
		Settings.bBlockInteraction = true;
		Settings.bDisablePawnMovement = true;
		Settings.bDisablePawnRotation = true;
		break;

	case EInputBlockingMode::BlockMovement:
		Settings.bBlockMovement = true;
		Settings.bBlockJump = true;
		Settings.bDisablePawnMovement = true;
		break;

	case EInputBlockingMode::BlockCamera:
		Settings.bBlockCamera = true;
		Settings.bDisablePawnRotation = true;
		break;

	case EInputBlockingMode::BlockMovementAndCamera:
		Settings.bBlockMovement = true;
		Settings.bBlockCamera = true;
		Settings.bBlockJump = true;
		Settings.bDisablePawnMovement = true;
		Settings.bDisablePawnRotation = true;
		break;

	default:
		break;
	}

	return Settings;
}

FDialoguePresentationSettings UDialoguePresentationLibrary::MakeDefaultPresentationSettings(EDialoguePresentationMode Mode)
{
	FDialoguePresentationSettings Settings;
	Settings.PresentationMode = Mode;
	Settings.bEnableCamera = true;
	Settings.bEnableInputBlocking = true;

	switch (Mode)
	{
	case EDialoguePresentationMode::Cinematic:
		Settings.CameraSettings = MakeDefaultCameraSettings(EDialogueCameraFraming::Medium);
		Settings.InputSettings = MakeDefaultInputSettings(EInputBlockingMode::BlockMovementAndCamera);
		break;

	case EDialoguePresentationMode::Interactive:
		Settings.bEnableCamera = false;
		Settings.InputSettings = MakeDefaultInputSettings(EInputBlockingMode::BlockMovement);
		Settings.InputSettings.bDisablePawnMovement = false;
		break;

	case EDialoguePresentationMode::Hybrid:
		Settings.CameraSettings = MakeDefaultCameraSettings(EDialogueCameraFraming::Full);
		Settings.InputSettings = MakeDefaultInputSettings(EInputBlockingMode::BlockMovement);
		Settings.InputSettings.bDisablePawnRotation = false;
		break;

	case EDialoguePresentationMode::FirstPerson:
		Settings.CameraSettings = MakeDefaultCameraSettings(EDialogueCameraFraming::CloseUp);
		Settings.InputSettings = MakeDefaultInputSettings(EInputBlockingMode::BlockMovement);
		break;

	default:
		break;
	}

	return Settings;
}

UDialoguePresentationComponent* UDialoguePresentationLibrary::GetPlayerPresentationComponent(UObject* WorldContextObject)
{
	APawn* PlayerPawn = GetPlayerPawn(WorldContextObject);
	if (!PlayerPawn)
	{
		return nullptr;
	}

	return PlayerPawn->FindComponentByClass<UDialoguePresentationComponent>();
}

void UDialoguePresentationLibrary::FreezePlayerCharacter(UObject* WorldContextObject, bool bFreeze)
{
	APawn* PlayerPawn = GetPlayerPawn(WorldContextObject);
	if (!PlayerPawn)
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(PlayerPawn))
	{
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			if (bFreeze)
			{
				MovementComp->DisableMovement();
				MovementComp->StopMovementImmediately();
			}
			else
			{
				MovementComp->SetMovementMode(MOVE_Walking);
			}
		}
	}

	// Disable rotation
	PlayerPawn->bUseControllerRotationYaw = !bFreeze;
	PlayerPawn->bUseControllerRotationPitch = !bFreeze;
}

void UDialoguePresentationLibrary::SetPlayerPawnVisibility(UObject* WorldContextObject, bool bVisible)
{
	APawn* PlayerPawn = GetPlayerPawn(WorldContextObject);
	if (!PlayerPawn)
	{
		return;
	}

	PlayerPawn->SetActorHiddenInGame(!bVisible);
	PlayerPawn->SetActorEnableCollision(bVisible);
}

APawn* UDialoguePresentationLibrary::GetPlayerPawn(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	return PC ? PC->GetPawn() : nullptr;
}

APlayerController* UDialoguePresentationLibrary::GetPlayerController(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return nullptr;
	}

	return World->GetFirstPlayerController();
}
