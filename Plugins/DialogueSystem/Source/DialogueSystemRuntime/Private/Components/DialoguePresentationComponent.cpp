// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/DialoguePresentationComponent.h"
#include "Components/DialogueComponent.h"
#include "Core/DialogueRunner.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialoguePresentation, Log, All);

UDialoguePresentationComponent::UDialoguePresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Create sub-components
	CameraComponent = CreateDefaultSubobject<UDialogueCameraComponent>(TEXT("DialogueCameraComponent"));
	InputBlockerComponent = CreateDefaultSubobject<UDialogueInputBlockerComponent>(TEXT("DialogueInputBlockerComponent"));

	// Initialize presentation mode presets
	InitializePresentationModes();
}

void UDialoguePresentationComponent::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe to dialogue events if auto-activation is enabled
	if (bAutoActivateOnDialogueStart || bAutoDeactivateOnDialogueEnd)
	{
		SubscribeToDialogueEvents();
	}
}

void UDialoguePresentationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Ensure presentation is deactivated
	if (bPresentationActive)
	{
		DeactivatePresentation();
	}

	// Unsubscribe from events
	UnsubscribeFromDialogueEvents();

	Super::EndPlay(EndPlayReason);
}

bool UDialoguePresentationComponent::ActivatePresentation(AActor* TargetNPC, EDialoguePresentationMode Mode)
{
	if (!TargetNPC)
	{
		UE_LOG(LogDialoguePresentation, Error, TEXT("ActivatePresentation: TargetNPC is null"));
		return false;
	}

	FDialoguePresentationSettings Settings = GetModeSettings(Mode);
	return ActivatePresentationWithSettings(TargetNPC, Settings);
}

bool UDialoguePresentationComponent::ActivatePresentationWithSettings(AActor* TargetNPC, const FDialoguePresentationSettings& Settings)
{
	if (!TargetNPC)
	{
		UE_LOG(LogDialoguePresentation, Error, TEXT("ActivatePresentationWithSettings: TargetNPC is null"));
		return false;
	}

	if (bPresentationActive)
	{
		UE_LOG(LogDialoguePresentation, Warning, TEXT("Presentation already active, deactivating first"));
		DeactivatePresentation();
	}

	// Store current settings
	CurrentSettings = Settings;

	// Activate camera if enabled
	if (Settings.bEnableCamera && CameraComponent)
	{
		bool bCameraSuccess = false;
		
		if (Settings.PresentationMode == EDialoguePresentationMode::FirstPerson)
		{
			// For first-person, use a different framing
			bCameraSuccess = CameraComponent->ActivateDialogueCameraWithFraming(TargetNPC, EDialogueCameraFraming::CloseUp);
		}
		else
		{
			bCameraSuccess = CameraComponent->ActivateDialogueCamera(TargetNPC, Settings.CameraSettings);
		}

		if (!bCameraSuccess)
		{
			UE_LOG(LogDialoguePresentation, Warning, TEXT("Failed to activate dialogue camera"));
		}
	}

	// Block input if enabled
	if (Settings.bEnableInputBlocking && InputBlockerComponent)
	{
		bool bInputSuccess = InputBlockerComponent->BlockInput(Settings.InputSettings);
		if (!bInputSuccess)
		{
			UE_LOG(LogDialoguePresentation, Warning, TEXT("Failed to block input"));
		}
	}

	// Apply game state changes
	ApplyGameStateChanges(Settings);

	bPresentationActive = true;

	UE_LOG(LogDialoguePresentation, Log, TEXT("Dialogue presentation activated for target: %s with mode: %d"), 
		*TargetNPC->GetName(), (int32)Settings.PresentationMode);

	return true;
}

void UDialoguePresentationComponent::DeactivatePresentation()
{
	if (!bPresentationActive)
	{
		return;
	}

	// Deactivate camera
	if (CameraComponent && CameraComponent->IsDialogueCameraActive())
	{
		CameraComponent->DeactivateDialogueCamera();
	}

	// Unblock input
	if (InputBlockerComponent && InputBlockerComponent->IsInputBlocked())
	{
		InputBlockerComponent->UnblockInput();
	}

	// Restore game state
	RestoreGameState();

	bPresentationActive = false;

	UE_LOG(LogDialoguePresentation, Log, TEXT("Dialogue presentation deactivated"));
}

void UDialoguePresentationComponent::TransitionToNewTarget(AActor* NewTargetNPC, float BlendTime)
{
	if (!bPresentationActive || !NewTargetNPC)
	{
		return;
	}

	// Transition camera to new target
	if (CameraComponent && CameraComponent->IsDialogueCameraActive())
	{
		CameraComponent->TransitionToTarget(NewTargetNPC, BlendTime);
	}

	UE_LOG(LogDialoguePresentation, Log, TEXT("Transitioned presentation to new target: %s"), *NewTargetNPC->GetName());
}

void UDialoguePresentationComponent::UpdatePresentationMode(EDialoguePresentationMode NewMode)
{
	if (!bPresentationActive)
	{
		return;
	}

	FDialoguePresentationSettings NewSettings = GetModeSettings(NewMode);
	
	// Update camera
	if (CameraComponent && CameraComponent->IsDialogueCameraActive())
	{
		CameraComponent->UpdateCameraFraming(NewSettings.CameraSettings.Framing, NewSettings.CameraSettings.BlendTime);
	}

	// Update input blocking
	if (InputBlockerComponent && InputBlockerComponent->IsInputBlocked())
	{
		InputBlockerComponent->UpdateBlockingMode(NewSettings.InputSettings.BlockingMode);
	}

	// Update game state
	ApplyGameStateChanges(NewSettings);

	CurrentSettings = NewSettings;

	UE_LOG(LogDialoguePresentation, Log, TEXT("Updated presentation mode to: %d"), (int32)NewMode);
}

void UDialoguePresentationComponent::SetModeSettings(EDialoguePresentationMode Mode, const FDialoguePresentationSettings& Settings)
{
	PresentationModeSettings.Add(Mode, Settings);
}

FDialoguePresentationSettings UDialoguePresentationComponent::GetModeSettings(EDialoguePresentationMode Mode) const
{
	if (const FDialoguePresentationSettings* Settings = PresentationModeSettings.Find(Mode))
	{
		return *Settings;
	}

	// Return default cinematic settings
	FDialoguePresentationSettings DefaultSettings;
	DefaultSettings.PresentationMode = Mode;
	return DefaultSettings;
}

void UDialoguePresentationComponent::InitializePresentationModes()
{
	// Cinematic Mode
	FDialoguePresentationSettings CinematicSettings;
	CinematicSettings.PresentationMode = EDialoguePresentationMode::Cinematic;
	CinematicSettings.bEnableCamera = true;
	CinematicSettings.bEnableInputBlocking = true;
	CinematicSettings.CameraSettings.Framing = EDialogueCameraFraming::Medium;
	CinematicSettings.CameraSettings.BlendTime = 0.5f;
	CinematicSettings.InputSettings.BlockingMode = EInputBlockingMode::BlockMovementAndCamera;
	CinematicSettings.InputSettings.bDisablePawnMovement = true;
	CinematicSettings.InputSettings.bDisablePawnRotation = true;
	CinematicSettings.bPauseGame = false;
	CinematicSettings.TimeDilation = 1.0f;
	PresentationModeSettings.Add(EDialoguePresentationMode::Cinematic, CinematicSettings);

	// Interactive Mode
	FDialoguePresentationSettings InteractiveSettings;
	InteractiveSettings.PresentationMode = EDialoguePresentationMode::Interactive;
	InteractiveSettings.bEnableCamera = false;
	InteractiveSettings.bEnableInputBlocking = true;
	InteractiveSettings.InputSettings.BlockingMode = EInputBlockingMode::BlockMovement;
	InteractiveSettings.InputSettings.bDisablePawnMovement = false; // Allow some movement
	InteractiveSettings.InputSettings.bDisablePawnRotation = false; // Allow looking around
	InteractiveSettings.bPauseGame = false;
	InteractiveSettings.TimeDilation = 1.0f;
	PresentationModeSettings.Add(EDialoguePresentationMode::Interactive, InteractiveSettings);

	// Hybrid Mode
	FDialoguePresentationSettings HybridSettings;
	HybridSettings.PresentationMode = EDialoguePresentationMode::Hybrid;
	HybridSettings.bEnableCamera = true;
	HybridSettings.bEnableInputBlocking = true;
	HybridSettings.CameraSettings.Framing = EDialogueCameraFraming::Full;
	HybridSettings.CameraSettings.BlendTime = 0.3f;
	HybridSettings.InputSettings.BlockingMode = EInputBlockingMode::BlockMovement;
	HybridSettings.InputSettings.bDisablePawnMovement = true;
	HybridSettings.InputSettings.bDisablePawnRotation = false; // Allow camera look
	HybridSettings.bPauseGame = false;
	HybridSettings.TimeDilation = 1.0f;
	PresentationModeSettings.Add(EDialoguePresentationMode::Hybrid, HybridSettings);

	// First-Person Mode
	FDialoguePresentationSettings FirstPersonSettings;
	FirstPersonSettings.PresentationMode = EDialoguePresentationMode::FirstPerson;
	FirstPersonSettings.bEnableCamera = true;
	FirstPersonSettings.bEnableInputBlocking = true;
	FirstPersonSettings.CameraSettings.Framing = EDialogueCameraFraming::CloseUp;
	FirstPersonSettings.CameraSettings.Distance = 100.0f; // Closer for first-person
	FirstPersonSettings.CameraSettings.BlendTime = 0.2f; // Faster blend
	FirstPersonSettings.InputSettings.BlockingMode = EInputBlockingMode::BlockMovement;
	FirstPersonSettings.InputSettings.bDisablePawnMovement = true;
	FirstPersonSettings.InputSettings.bDisablePawnRotation = false;
	FirstPersonSettings.bPauseGame = false;
	FirstPersonSettings.TimeDilation = 1.0f;
	PresentationModeSettings.Add(EDialoguePresentationMode::FirstPerson, FirstPersonSettings);

	// Custom Mode (empty, to be configured by user)
	FDialoguePresentationSettings CustomSettings;
	CustomSettings.PresentationMode = EDialoguePresentationMode::Custom;
	PresentationModeSettings.Add(EDialoguePresentationMode::Custom, CustomSettings);
}

void UDialoguePresentationComponent::ApplyGameStateChanges(const FDialoguePresentationSettings& Settings)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Store original state
	OriginalTimeDilation = World->GetWorldSettings()->TimeDilation;
	bWasGamePaused = UGameplayStatics::IsGamePaused(World);

	// Apply pause if requested
	if (Settings.bPauseGame && !bWasGamePaused)
	{
		UGameplayStatics::SetGamePaused(World, true);
	}

	// Apply time dilation if different from current
	if (!FMath::IsNearlyEqual(Settings.TimeDilation, OriginalTimeDilation))
	{
		World->GetWorldSettings()->SetTimeDilation(Settings.TimeDilation);
	}
}

void UDialoguePresentationComponent::RestoreGameState()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Restore pause state
	if (UGameplayStatics::IsGamePaused(World) && !bWasGamePaused)
	{
		UGameplayStatics::SetGamePaused(World, false);
	}

	// Restore time dilation
	if (!FMath::IsNearlyEqual(World->GetWorldSettings()->TimeDilation, OriginalTimeDilation))
	{
		World->GetWorldSettings()->SetTimeDilation(OriginalTimeDilation);
	}
}

void UDialoguePresentationComponent::OnDialogueStarted(UDialogueRunner* Runner)
{
	if (!bAutoActivateOnDialogueStart || !Runner)
	{
		return;
	}

	// Try to get target NPC from dialogue runner's context
	AActor* TargetNPC = nullptr;
	

	UDialogueSessionContext* Context = Runner->GetContext();
	if (Context)
	{
		AActor* Owner = GetOwner();
		

		if (Owner == Context->GetPlayer())
		{
			TargetNPC = Context->GetNPC();
		}
		else if (Owner == Context->GetNPC())
		{
			TargetNPC = Context->GetPlayer();
		}
	}
		

	if (TargetNPC && CameraComponent)
	{
		FDialogueCameraSettings DefaultSettings;
		CameraComponent->ActivateDialogueCamera(TargetNPC, DefaultSettings);
	}
	
	// Fallback: try to get from cached dialogue component
	if (!TargetNPC && CachedDialogueComponent.IsValid())
	{
		TargetNPC = CachedDialogueComponent->GetOwner();
	}

	if (TargetNPC)
	{
		bool bSuccess = ActivatePresentation(TargetNPC, DefaultPresentationMode);
		if (bSuccess)
		{
			UE_LOG(LogDialoguePresentation, Log, TEXT("Auto-activated presentation for target: %s"), *TargetNPC->GetName());
		}
		else
		{
			UE_LOG(LogDialoguePresentation, Warning, TEXT("Failed to auto-activate presentation for target: %s"), *TargetNPC->GetName());
		}
	}
	else
	{
		UE_LOG(LogDialoguePresentation, Warning, TEXT("Could not determine target NPC for auto-activation"));
	}
}

void UDialoguePresentationComponent::OnDialogueEnded()
{
	if (!bAutoDeactivateOnDialogueEnd)
	{
		return;
	}

	DeactivatePresentation();
}

UDialogueComponent* UDialoguePresentationComponent::FindDialogueComponent() const
{
	// Try to find on owner
	AActor* Owner = GetOwner();
	if (Owner)
	{
		return Owner->FindComponentByClass<UDialogueComponent>();
	}

	return nullptr;
}

void UDialoguePresentationComponent::SubscribeToDialogueEvents()
{
	UDialogueComponent* DialogueComp = FindDialogueComponent();
	if (DialogueComp)
	{
		CachedDialogueComponent = DialogueComp;

		// Subscribe to dialogue events
		DialogueComp->OnDialogueStarted.AddDynamic(this, &UDialoguePresentationComponent::OnDialogueStarted);
		DialogueComp->OnDialogueEnded.AddDynamic(this, &UDialoguePresentationComponent::OnDialogueEnded);
		
		UE_LOG(LogDialoguePresentation, Log, TEXT("Subscribed to dialogue events from component: %s"), 
			*DialogueComp->GetName());
	}
	else
	{
		UE_LOG(LogDialoguePresentation, Warning, TEXT("Could not find DialogueComponent to subscribe to events"));
	}
}

void UDialoguePresentationComponent::UnsubscribeFromDialogueEvents()
{
	if (CachedDialogueComponent.IsValid())
	{
		// Unsubscribe from dialogue events
		CachedDialogueComponent->OnDialogueStarted.RemoveDynamic(this, &UDialoguePresentationComponent::OnDialogueStarted);
		CachedDialogueComponent->OnDialogueEnded.RemoveDynamic(this, &UDialoguePresentationComponent::OnDialogueEnded);
		
		UE_LOG(LogDialoguePresentation, Log, TEXT("Unsubscribed from dialogue events"));
	}
}
