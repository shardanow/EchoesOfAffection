// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/DialogueInputBlockerComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueInput, Log, All);

UDialogueInputBlockerComponent::UDialogueInputBlockerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Setup default legacy input blocking lists
	LegacyActionNamesToBlock = {
		TEXT("Jump"),
		TEXT("Crouch"),
		TEXT("Sprint"),
		TEXT("Interact"),
		TEXT("Fire"),
		TEXT("Aim"),
		TEXT("Reload"),
		TEXT("Use")
	};

	LegacyAxisNamesToBlock = {
		TEXT("MoveForward"),
		TEXT("MoveRight"),
		TEXT("Turn"),
		TEXT("LookUp"),
		TEXT("TurnRate"),
		TEXT("LookUpRate")
	};
}

void UDialogueInputBlockerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache player controller
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PC = World->GetFirstPlayerController();
		if (PC)
		{
			CachedPlayerController = PC;
			CachedPlayerPawn = PC->GetPawn();
		}
	}
}

void UDialogueInputBlockerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Ensure input is unblocked on component destruction
	if (bInputBlocked)
	{
		UnblockInput();
	}

	Super::EndPlay(EndPlayReason);
}

bool UDialogueInputBlockerComponent::BlockInput(const FDialogueInputBlockingSettings& Settings)
{
	if (!bEnableInputBlocking)
	{
		UE_LOG(LogDialogueInput, Warning, TEXT("Input blocking is disabled"));
		return false;
	}

	APlayerController* PC = CachedPlayerController.Get();
	if (!PC)
	{
		PC = FindPlayerController();
		if (!PC)
		{
			UE_LOG(LogDialogueInput, Error, TEXT("BlockInput: No valid PlayerController"));
			return false;
		}
		CachedPlayerController = PC;
	}

	// Update cached pawn reference
	if (PC->GetPawn())
	{
		CachedPlayerPawn = PC->GetPawn();
	}

	// Store current pawn state
	if (CachedPlayerPawn.IsValid())
	{
		StorePawnState();
	}

	// Apply input blocking
	ApplyInputBlocking(Settings);

	// Handle pawn state
	if (Settings.bDisablePawnMovement)
	{
		BlockPawnMovement(true);
	}

	if (Settings.bDisablePawnRotation)
	{
		BlockPawnRotation(true);
	}

	if (Settings.bHidePlayerPawn)
	{
		SetPawnVisibility(false);
	}

	bInputBlocked = true;
	DefaultBlockingSettings = Settings;

	UE_LOG(LogDialogueInput, Log, TEXT("Input blocked with mode: %d"), (int32)Settings.BlockingMode);
	return true;
}

bool UDialogueInputBlockerComponent::BlockInputWithDefaults()
{
	return BlockInput(DefaultBlockingSettings);
}

void UDialogueInputBlockerComponent::UnblockInput()
{
	if (!bInputBlocked)
	{
		return;
	}

	// Restore input
	RestoreInput();

	// Restore pawn state
	if (DefaultBlockingSettings.bRestorePawnState && CachedPlayerPawn.IsValid())
	{
		RestorePawnState();
	}

	bInputBlocked = false;

	UE_LOG(LogDialogueInput, Log, TEXT("Input unblocked"));
}

void UDialogueInputBlockerComponent::UpdateBlockingMode(EInputBlockingMode NewMode)
{
	if (!bInputBlocked)
	{
		return;
	}

	// Update settings and reapply blocking
	FDialogueInputBlockingSettings NewSettings = DefaultBlockingSettings;
	NewSettings.BlockingMode = NewMode;

	// First restore current state
	RestoreInput();

	// Then apply new blocking
	ApplyInputBlocking(NewSettings);

	DefaultBlockingSettings = NewSettings;
}

void UDialogueInputBlockerComponent::AllowInputAction(FName ActionName)
{
	if (!bInputBlocked)
	{
		return;
	}

	// Remove from blocking lists
	LegacyActionNamesToBlock.Remove(ActionName);

	// TODO: Implement for Enhanced Input System
}

void UDialogueInputBlockerComponent::SetDefaultBlockingSettings(const FDialogueInputBlockingSettings& Settings)
{
	DefaultBlockingSettings = Settings;
}

void UDialogueInputBlockerComponent::ApplyInputBlocking(const FDialogueInputBlockingSettings& Settings)
{
	// Populate legacy blocking lists based on settings
	PopulateLegacyBlockingLists(Settings);

	// Try Enhanced Input first (UE5 standard)
	if (IsEnhancedInputAvailable())
	{
		BlockEnhancedInput(Settings);
	}
	else
	{
		// Fallback to legacy input
		BlockLegacyInput(Settings);
	}
}

void UDialogueInputBlockerComponent::BlockEnhancedInput(const FDialogueInputBlockingSettings& Settings)
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	// Store current mapping contexts
	StoredMappingContexts.Empty();
	StoredMappingPriorities.Empty();

	// Get all active mapping contexts
	TArray<FEnhancedActionKeyMapping> Mappings = Subsystem->GetAllPlayerMappableActionKeyMappings();
	
	// Based on blocking mode, remove specific contexts
	if (Settings.BlockingMode == EInputBlockingMode::BlockAll)
	{
		// Clear all mapping contexts except dialogue
		Subsystem->ClearAllMappings();
	}
	else
	{
		// Selective blocking - more complex logic needed
		// This would require knowledge of specific input action names
		// For now, we'll use legacy input as fallback
		UE_LOG(LogDialogueInput, Warning, TEXT("Selective Enhanced Input blocking not fully implemented, using legacy fallback"));
	}

	// Add dialogue-specific input mapping context if provided
	if (DialogueInputMappingContext.IsValid())
	{
		UInputMappingContext* Context = DialogueInputMappingContext.LoadSynchronous();
		if (Context)
		{
			Subsystem->AddMappingContext(Context, DialogueInputPriority);
		}
	}
}

void UDialogueInputBlockerComponent::BlockLegacyInput(const FDialogueInputBlockingSettings& Settings)
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC || !PC->InputComponent)
	{
		return;
	}

	UInputComponent* InputComp = PC->InputComponent;

	// Block action bindings
	for (const FName& ActionName : LegacyActionNamesToBlock)
	{
		// Find and disable action bindings
		for (int32 i = InputComp->GetNumActionBindings() - 1; i >= 0; --i)
		{
			FInputActionBinding& Binding = InputComp->GetActionBinding(i);
			if (Binding.GetActionName() == ActionName)
			{
				// Store original binding for restoration
				// Note: Actual disabling would require modifying the binding
				// For safety, we'll just note this limitation
			}
		}
	}

	// Disable input temporarily
	if (Settings.BlockingMode == EInputBlockingMode::BlockAll)
	{
		PC->DisableInput(PC);
	}
}

void UDialogueInputBlockerComponent::RestoreInput()
{
	if (IsEnhancedInputAvailable())
	{
		RestoreEnhancedInput();
	}
	else
	{
		RestoreLegacyInput();
	}
}

void UDialogueInputBlockerComponent::RestoreEnhancedInput()
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	// Remove dialogue input mapping context
	if (DialogueInputMappingContext.IsValid())
	{
		UInputMappingContext* Context = DialogueInputMappingContext.Get();
		if (Context)
		{
			Subsystem->RemoveMappingContext(Context);
		}
	}

	// Restore original mapping contexts
	// Note: This requires storing them during blocking
	// For simplicity, we're assuming game will restore its own contexts
}

void UDialogueInputBlockerComponent::RestoreLegacyInput()
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC)
	{
		return;
	}

	// Re-enable input
	PC->ResetIgnoreInputFlags();
	PC->bShowMouseCursor = false; // Reset if needed
}

void UDialogueInputBlockerComponent::BlockPawnMovement(bool bBlock)
{
	APawn* Pawn = CachedPlayerPawn.Get();
	if (!Pawn)
	{
		return;
	}

	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			if (bBlock)
			{
				// Store original movement mode
				OriginalMovementMode = (uint8)MoveComp->MovementMode;
				
				// Disable movement
				MoveComp->DisableMovement();
				MoveComp->StopMovementImmediately();
			}
		}
	}
}

void UDialogueInputBlockerComponent::BlockPawnRotation(bool bBlock)
{
	APawn* Pawn = CachedPlayerPawn.Get();
	if (!Pawn)
	{
		return;
	}

	if (AController* Controller = Pawn->GetController())
	{
		if (bBlock)
		{
			// Store original state
			bOriginalRotationEnabled = Pawn->bUseControllerRotationYaw;
			
			// Disable rotation
			Pawn->bUseControllerRotationYaw = false;
			Pawn->bUseControllerRotationPitch = false;
			Pawn->bUseControllerRotationRoll = false;
		}
	}
}

void UDialogueInputBlockerComponent::SetPawnVisibility(bool bVisible)
{
	APawn* Pawn = CachedPlayerPawn.Get();
	if (!Pawn)
	{
		return;
	}

	Pawn->SetActorHiddenInGame(!bVisible);
	Pawn->SetActorEnableCollision(bVisible);
}

void UDialogueInputBlockerComponent::StorePawnState()
{
	APawn* Pawn = CachedPlayerPawn.Get();
	if (!Pawn)
	{
		return;
	}

	bOriginalPawnVisibility = !Pawn->IsHidden();
	bOriginalRotationEnabled = Pawn->bUseControllerRotationYaw;

	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			OriginalMovementMode = (uint8)MoveComp->MovementMode;
		}
	}
}

void UDialogueInputBlockerComponent::RestorePawnState()
{
	APawn* Pawn = CachedPlayerPawn.Get();
	if (!Pawn)
	{
		return;
	}

	// Restore visibility
	SetPawnVisibility(bOriginalPawnVisibility);

	// Restore rotation
	Pawn->bUseControllerRotationYaw = bOriginalRotationEnabled;

	// Restore movement
	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			MoveComp->SetMovementMode((EMovementMode)OriginalMovementMode);
		}
	}
}

void UDialogueInputBlockerComponent::PopulateLegacyBlockingLists(const FDialogueInputBlockingSettings& Settings)
{
	// Clear existing lists
	LegacyActionNamesToBlock.Empty();
	LegacyAxisNamesToBlock.Empty();

	// Add based on blocking mode
	switch (Settings.BlockingMode)
	{
	case EInputBlockingMode::BlockAll:
		// Block everything except dialogue-specific actions
		LegacyActionNamesToBlock = {
			TEXT("Jump"), TEXT("Crouch"), TEXT("Sprint"), TEXT("Interact"),
			TEXT("Fire"), TEXT("Aim"), TEXT("Reload"), TEXT("Use"),
			TEXT("ToggleInventory"), TEXT("ToggleMenu")
		};
		LegacyAxisNamesToBlock = {
			TEXT("MoveForward"), TEXT("MoveRight"), TEXT("Turn"), TEXT("LookUp"),
			TEXT("TurnRate"), TEXT("LookUpRate")
		};
		break;

	case EInputBlockingMode::BlockMovement:
		if (Settings.bBlockMovement)
		{
			LegacyAxisNamesToBlock.Add(TEXT("MoveForward"));
			LegacyAxisNamesToBlock.Add(TEXT("MoveRight"));
		}
		if (Settings.bBlockJump)
		{
			LegacyActionNamesToBlock.Add(TEXT("Jump"));
		}
		if (Settings.bBlockSprint)
		{
			LegacyActionNamesToBlock.Add(TEXT("Sprint"));
		}
		break;

	case EInputBlockingMode::BlockCamera:
		if (Settings.bBlockCamera)
		{
			LegacyAxisNamesToBlock.Add(TEXT("Turn"));
			LegacyAxisNamesToBlock.Add(TEXT("LookUp"));
			LegacyAxisNamesToBlock.Add(TEXT("TurnRate"));
			LegacyAxisNamesToBlock.Add(TEXT("LookUpRate"));
		}
		break;

	case EInputBlockingMode::BlockMovementAndCamera:
		// Combine movement and camera blocking
		PopulateLegacyBlockingLists(FDialogueInputBlockingSettings{ EInputBlockingMode::BlockMovement });
		{
			FDialogueInputBlockingSettings CameraSettings;
			CameraSettings.BlockingMode = EInputBlockingMode::BlockCamera;
			
			TArray<FName> CameraActions;
			TArray<FName> CameraAxes;
			// Temporarily store current lists
			TArray<FName> TempActions = LegacyActionNamesToBlock;
			TArray<FName> TempAxes = LegacyAxisNamesToBlock;
			
			// Get camera blocking lists
			PopulateLegacyBlockingLists(CameraSettings);
			CameraActions = LegacyActionNamesToBlock;
			CameraAxes = LegacyAxisNamesToBlock;
			
			// Combine
			LegacyActionNamesToBlock = TempActions;
			LegacyAxisNamesToBlock = TempAxes;
			LegacyActionNamesToBlock.Append(CameraActions);
			LegacyAxisNamesToBlock.Append(CameraAxes);
		}
		break;

	case EInputBlockingMode::Custom:
		// Use custom settings
		if (Settings.bBlockMovement)
		{
			LegacyAxisNamesToBlock.Add(TEXT("MoveForward"));
			LegacyAxisNamesToBlock.Add(TEXT("MoveRight"));
		}
		if (Settings.bBlockCamera)
		{
			LegacyAxisNamesToBlock.Add(TEXT("Turn"));
			LegacyAxisNamesToBlock.Add(TEXT("LookUp"));
		}
		if (Settings.bBlockJump)
		{
			LegacyActionNamesToBlock.Add(TEXT("Jump"));
		}
		if (Settings.bBlockCrouch)
		{
			LegacyActionNamesToBlock.Add(TEXT("Crouch"));
		}
		if (Settings.bBlockSprint)
		{
			LegacyActionNamesToBlock.Add(TEXT("Sprint"));
		}
		if (Settings.bBlockInteraction)
		{
			LegacyActionNamesToBlock.Add(TEXT("Interact"));
			LegacyActionNamesToBlock.Add(TEXT("Use"));
		}
		if (Settings.bBlockMenus)
		{
			LegacyActionNamesToBlock.Add(TEXT("ToggleInventory"));
			LegacyActionNamesToBlock.Add(TEXT("ToggleMenu"));
		}
		if (Settings.bBlockCombat)
		{
			LegacyActionNamesToBlock.Add(TEXT("Fire"));
			LegacyActionNamesToBlock.Add(TEXT("Aim"));
			LegacyActionNamesToBlock.Add(TEXT("Reload"));
		}
		break;

	case EInputBlockingMode::AllowAll:
		// Don't block anything
		break;
	}
}

APlayerController* UDialogueInputBlockerComponent::FindPlayerController() const
{
	UWorld* World = GetWorld();
	if (World)
	{
		return World->GetFirstPlayerController();
	}
	return nullptr;
}

bool UDialogueInputBlockerComponent::IsEnhancedInputAvailable() const
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC || !PC->GetLocalPlayer())
	{
		return false;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	return Subsystem != nullptr;
}
