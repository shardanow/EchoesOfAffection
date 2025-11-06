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

	LegacyActionNamesToBlock = {
		TEXT("Jump"), TEXT("Crouch"), TEXT("Sprint"), TEXT("Interact"),
		TEXT("Fire"), TEXT("Aim"), TEXT("Reload"), TEXT("Use")
	};

	LegacyAxisNamesToBlock = {
		TEXT("MoveForward"), TEXT("MoveRight"), TEXT("Turn"), TEXT("LookUp"),
		TEXT("TurnRate"), TEXT("LookUpRate")
	};
}

void UDialogueInputBlockerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Don't cache PC here - get it dynamically when needed
	// This is important when component is on NPC and controls player input
}

void UDialogueInputBlockerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ?? ???????? ?????????????? ???? ??? ????????????
	// Skip unblock during PIE end or quit to avoid accessing destroyed objects
	if (bInputBlocked && 
		EndPlayReason != EEndPlayReason::EndPlayInEditor && 
		EndPlayReason != EEndPlayReason::Quit)
	{
		UnblockInput();
	}
	else if (bInputBlocked)
	{
		// During PIE end/quit, just mark as unblocked without restoration
		UE_LOG(LogDialogueInput, Log, TEXT("EndPlay during PIE shutdown - skipping input restoration"));
		bInputBlocked = false;
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

	if (PC->GetPawn())
	{
		CachedPlayerPawn = PC->GetPawn();
	}

	if (CachedPlayerPawn.IsValid())
	{
		StorePawnState();
	}

	// Store original mouse/UI state
	bOriginalShowMouseCursor = PC->bShowMouseCursor;
	bOriginalEnableClickEvents = PC->bEnableClickEvents;
	bOriginalEnableMouseOverEvents = PC->bEnableMouseOverEvents;

	// Determine current input mode (simplified detection)
	if (PC->bShowMouseCursor)
	{
		OriginalInputMode = PC->bEnableClickEvents ? EInputMode::GameAndUI : EInputMode::GameOnly;
	}
	else
	{
		OriginalInputMode = EInputMode::GameOnly;
	}

	// Apply mouse cursor settings
	if (Settings.bShowMouseCursor)
	{
		PC->bShowMouseCursor = true;
		UE_LOG(LogDialogueInput, Verbose, TEXT("Mouse cursor shown"));
	}

	if (Settings.bEnableClickEvents)
	{
		PC->bEnableClickEvents = true;
		UE_LOG(LogDialogueInput, Verbose, TEXT("Click events enabled"));
	}

	if (Settings.bEnableMouseOverEvents)
	{
		PC->bEnableMouseOverEvents = true;
		UE_LOG(LogDialogueInput, Verbose, TEXT("Mouse over events enabled"));
	}

	// Set input mode for dialogue (UI + Game for clicking dialogue options)
	if (Settings.bShowMouseCursor)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		UE_LOG(LogDialogueInput, Verbose, TEXT("Set input mode to Game And UI for dialogue"));
	}

	ApplyInputBlocking(Settings);

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
		UE_LOG(LogDialogueInput, Warning, TEXT("UnblockInput called but input is not blocked"));
		return;
	}

	// Check if world is tearing down (PIE end, level transition, etc.)
	UWorld* World = GetWorld();
	bool bIsTearingDown = !World || World->bIsTearingDown;
	
	// During teardown, skip restoration to avoid accessing destroyed objects
	if (bIsTearingDown)
	{
		UE_LOG(LogDialogueInput, Log, TEXT("UnblockInput: Skipping restoration during world teardown"));
		bInputBlocked = false;
		return;
	}


	APlayerController* PC = FindPlayerController();
	if (!PC)
	{
		UE_LOG(LogDialogueInput, Warning, TEXT("UnblockInput: No valid PlayerController found - possibly during shutdown"));
		bInputBlocked = false;
		return;
	}
	
	UE_LOG(LogDialogueInput, Log, TEXT("=== UnblockInput START for PC: %s ==="), *PC->GetName());
	
	// CRITICAL: Clear accumulated input
	if (PC->PlayerInput)
	{
		PC->PlayerInput->FlushPressedKeys();
		UE_LOG(LogDialogueInput, Verbose, TEXT("Flushed pressed keys"));
	}
	else
	{
		UE_LOG(LogDialogueInput, Error, TEXT("PlayerInput is NULL!"));
	}
	
	// Restore mouse cursor state
	PC->bShowMouseCursor = bOriginalShowMouseCursor;
	PC->bEnableClickEvents = bOriginalEnableClickEvents;
	PC->bEnableMouseOverEvents = bOriginalEnableMouseOverEvents;
	
	UE_LOG(LogDialogueInput, Verbose, TEXT("Restored mouse cursor state - Show: %d, Click: %d, MouseOver: %d"),
		bOriginalShowMouseCursor, bOriginalEnableClickEvents, bOriginalEnableMouseOverEvents);
	
	// CRITICAL FIX: Reset input mode to Game Only
	// This removes UI focus and returns control to the game
	if (!bOriginalShowMouseCursor)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		UE_LOG(LogDialogueInput, Verbose, TEXT("Set input mode to Game Only"));
	}
	else
	{
		// If cursor was originally visible, use GameAndUI mode
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		UE_LOG(LogDialogueInput, Verbose, TEXT("Set input mode to Game And UI"));
	}
	

	UE_LOG(LogDialogueInput, Log, TEXT("BEFORE Restore - MoveIgnored: %d, LookIgnored: %d"), 
		PC->IsMoveInputIgnored(), PC->IsLookInputIgnored());

	RestoreInput();


	UE_LOG(LogDialogueInput, Log, TEXT("AFTER Restore - MoveIgnored: %d, LookIgnored: %d"), 
		PC->IsMoveInputIgnored(), PC->IsLookInputIgnored());

	if (DefaultBlockingSettings.bRestorePawnState && CachedPlayerPawn.IsValid())
	{
		RestorePawnState();
		UE_LOG(LogDialogueInput, Verbose, TEXT("Restored pawn state"));
	}

	// Additional cleanup: reset controller rotation input
	PC->ResetIgnoreLookInput();
	PC->ResetIgnoreMoveInput();
	
	UE_LOG(LogDialogueInput, Log, TEXT("AFTER Reset - MoveIgnored: %d, LookIgnored: %d"), 
		PC->IsMoveInputIgnored(), PC->IsLookInputIgnored());
	

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		UE_LOG(LogDialogueInput, Log, TEXT("Enhanced Input Subsystem exists"));
	}
	else
	{
		UE_LOG(LogDialogueInput, Warning, TEXT("Enhanced Input Subsystem is NULL!"));
	}

	bInputBlocked = false;

	UE_LOG(LogDialogueInput, Log, TEXT("=== UnblockInput END - Success ==="));
}

void UDialogueInputBlockerComponent::UpdateBlockingMode(EInputBlockingMode NewMode)
{
	if (!bInputBlocked)
	{
		return;
	}

	FDialogueInputBlockingSettings NewSettings = DefaultBlockingSettings;
	NewSettings.BlockingMode = NewMode;

	RestoreInput();
	ApplyInputBlocking(NewSettings);

	DefaultBlockingSettings = NewSettings;
}

void UDialogueInputBlockerComponent::AllowInputAction(FName ActionName)
{
	if (!bInputBlocked)
	{
		return;
	}

	LegacyActionNamesToBlock.Remove(ActionName);
}

void UDialogueInputBlockerComponent::SetDefaultBlockingSettings(const FDialogueInputBlockingSettings& Settings)
{
	DefaultBlockingSettings = Settings;
}

void UDialogueInputBlockerComponent::ApplyInputBlocking(const FDialogueInputBlockingSettings& Settings)
{
	PopulateLegacyBlockingLists(Settings);

	if (IsEnhancedInputAvailable())
	{
		BlockEnhancedInput(Settings);
	}
	else
	{
		BlockLegacyInput(Settings);
	}
}

void UDialogueInputBlockerComponent::BlockEnhancedInput(const FDialogueInputBlockingSettings& Settings)
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC || !PC->GetLocalPlayer())
	{
		UE_LOG(LogDialogueInput, Error, TEXT("BlockEnhancedInput: Invalid PC or LocalPlayer"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem)
	{
		UE_LOG(LogDialogueInput, Error, TEXT("BlockEnhancedInput: No Enhanced Input Subsystem"));
		return;
	}

	UE_LOG(LogDialogueInput, Log, TEXT("=== BlockEnhancedInput START ==="));




	if (DialogueInputMappingContext.IsValid())
	{
		UInputMappingContext* Context = DialogueInputMappingContext.LoadSynchronous();
		if (Context)
		{
			const int32 HighPriority = 1000;
			
			UE_LOG(LogDialogueInput, Log, TEXT("Adding dialogue context: %s with priority %d"), 
				*Context->GetName(), HighPriority);
			
			Subsystem->AddMappingContext(Context, HighPriority);
			
			UE_LOG(LogDialogueInput, Log, TEXT("? Dialogue input mapping context added successfully"));
		}
		else
		{
			UE_LOG(LogDialogueInput, Error, TEXT("Failed to load DialogueInputMappingContext!"));
			BlockLegacyInput(Settings);
		}
	}
	else
	{

		UE_LOG(LogDialogueInput, Warning, TEXT("No dialogue input mapping context - using legacy blocking"));
		BlockLegacyInput(Settings);
	}
	
	UE_LOG(LogDialogueInput, Log, TEXT("=== BlockEnhancedInput END ==="));
}

void UDialogueInputBlockerComponent::BlockLegacyInput(const FDialogueInputBlockingSettings& Settings)
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC || !PC->InputComponent)
	{
		return;
	}

	if (Settings.BlockingMode == EInputBlockingMode::BlockAll)
	{
		PC->DisableInput(PC);
	}
	else
	{
		if (Settings.bBlockMovement || Settings.BlockingMode == EInputBlockingMode::BlockMovement ||
			Settings.BlockingMode == EInputBlockingMode::BlockMovementAndCamera)
		{
			PC->SetIgnoreMoveInput(true);
		}

		if (Settings.bBlockCamera || Settings.BlockingMode == EInputBlockingMode::BlockCamera ||
			Settings.BlockingMode == EInputBlockingMode::BlockMovementAndCamera)
		{
			PC->SetIgnoreLookInput(true);
		}
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
	if (!PC || !PC->GetLocalPlayer())
	{
		UE_LOG(LogDialogueInput, Error, TEXT("RestoreEnhancedInput: Invalid PC or LocalPlayer"));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem)
	{
		UE_LOG(LogDialogueInput, Error, TEXT("RestoreEnhancedInput: No Enhanced Input Subsystem"));
		return;
	}

	UE_LOG(LogDialogueInput, Log, TEXT("=== RestoreEnhancedInput START ==="));


	if (DialogueInputMappingContext.IsValid())
	{
		UInputMappingContext* Context = DialogueInputMappingContext.Get();
		if (Context)
		{
			UE_LOG(LogDialogueInput, Log, TEXT("Removing dialogue context: %s"), *Context->GetName());
			Subsystem->RemoveMappingContext(Context);
			UE_LOG(LogDialogueInput, Log, TEXT("Removed dialogue input mapping context"));
		}
		else
		{
			UE_LOG(LogDialogueInput, Warning, TEXT("DialogueInputMappingContext is valid but Get() returned NULL"));
		}
	}
	else
	{

		UE_LOG(LogDialogueInput, Warning, TEXT("No dialogue input mapping context - restoring legacy input"));
		RestoreLegacyInput();
	}

	StoredMappingContexts.Empty();
	StoredMappingPriorities.Empty();
	
	UE_LOG(LogDialogueInput, Log, TEXT("=== RestoreEnhancedInput END - Original contexts should be active ==="));
}

void UDialogueInputBlockerComponent::RestoreLegacyInput()
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!PC)
	{
		return;
	}

	PC->ResetIgnoreInputFlags();
	PC->EnableInput(PC);
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
				OriginalMovementMode = (uint8)MoveComp->MovementMode;
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

	if (bBlock)
	{
		bOriginalRotationEnabled = Pawn->bUseControllerRotationYaw;
		Pawn->bUseControllerRotationYaw = false;
		Pawn->bUseControllerRotationPitch = false;
		Pawn->bUseControllerRotationRoll = false;
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

	UE_LOG(LogDialogueInput, Verbose, TEXT("Stored pawn state - Visibility: %s, Rotation: %s, MovementMode: %d"),
		bOriginalPawnVisibility ? TEXT("Visible") : TEXT("Hidden"),
		bOriginalRotationEnabled ? TEXT("Enabled") : TEXT("Disabled"),
		OriginalMovementMode);
}

void UDialogueInputBlockerComponent::RestorePawnState()
{
	APawn* Pawn = CachedPlayerPawn.Get();
	if (!Pawn)
	{
		return;
	}

	SetPawnVisibility(bOriginalPawnVisibility);

	Pawn->bUseControllerRotationYaw = bOriginalRotationEnabled;

	if (bOriginalRotationEnabled)
	{
		Pawn->bUseControllerRotationPitch = false;
		Pawn->bUseControllerRotationRoll = false;
	}

	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			MoveComp->SetMovementMode((EMovementMode)OriginalMovementMode);
		}
	}

	UE_LOG(LogDialogueInput, Verbose, TEXT("Restored pawn state - Visibility: %s, Rotation: %s, MovementMode: %d"),
		bOriginalPawnVisibility ? TEXT("Visible") : TEXT("Hidden"),
		bOriginalRotationEnabled ? TEXT("Enabled") : TEXT("Disabled"),
		OriginalMovementMode);
}

void UDialogueInputBlockerComponent::PopulateLegacyBlockingLists(const FDialogueInputBlockingSettings& Settings)
{
	LegacyActionNamesToBlock.Empty();
	LegacyAxisNamesToBlock.Empty();

	switch (Settings.BlockingMode)
	{
	case EInputBlockingMode::BlockAll:
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
		PopulateLegacyBlockingLists(FDialogueInputBlockingSettings{ EInputBlockingMode::BlockMovement });
		{
			FDialogueInputBlockingSettings CameraSettings;
			CameraSettings.BlockingMode = EInputBlockingMode::BlockCamera;

			TArray<FName> TempActions = LegacyActionNamesToBlock;
			TArray<FName> TempAxes = LegacyAxisNamesToBlock;

			PopulateLegacyBlockingLists(CameraSettings);
			TArray<FName> CameraActions = LegacyActionNamesToBlock;
			TArray<FName> CameraAxes = LegacyAxisNamesToBlock;

			LegacyActionNamesToBlock = TempActions;
			LegacyAxisNamesToBlock = TempAxes;
			LegacyActionNamesToBlock.Append(CameraActions);
			LegacyAxisNamesToBlock.Append(CameraAxes);
		}
		break;

	case EInputBlockingMode::Custom:
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
		break;
	}
}

APlayerController* UDialogueInputBlockerComponent::FindPlayerController() const
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return nullptr;
	}
	
	return World->GetFirstPlayerController();
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