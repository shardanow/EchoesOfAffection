// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/Examples/DoorActor.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

ADoorActor::ADoorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create root component
	DoorFrameComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	RootComponent = DoorFrameComponent;

	// Create door component
	DoorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	DoorComponent->SetupAttachment(RootComponent);

	// Default settings
	bIsOpen = false;
	bIsLocked = false;
	DoorOpenDuration = 1.0f;
	InteractionDistance = 200.0f;
	DoorOpenAngle = 90.0f;
	bAutoClose = false;
	AutoCloseDelay = 5.0f;
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
}

EInteractionResult ADoorActor::BeginInteract_Implementation(AActor* InteractingActor)
{
	if (bIsLocked)
	{
		BP_OnDoorLockedAttempt(InteractingActor);
		return EInteractionResult::RequirementsNotMet;
	}

	// Start opening/closing animation
	if (bIsOpen)
	{
		BP_OnDoorClosing();
	}
	else
	{
		BP_OnDoorOpening();
	}

	return EInteractionResult::Success;
}

void ADoorActor::EndInteract_Implementation(AActor* InteractingActor)
{
	// Toggle door state
	ToggleDoor();
}

FInteractionData ADoorActor::GetInteractionData_Implementation(AActor* InteractingActor) const
{
	FInteractionData Data;
	Data.InteractionName = FText::FromString("Door");
	Data.InteractionType = bIsOpen ? EInteractionType::Close : EInteractionType::Open;
	Data.ActionText = bIsOpen ? FText::FromString("Press E to Close") : FText::FromString("Press E to Open");
	Data.InteractionDuration = DoorOpenDuration;
	Data.MaxInteractionDistance = InteractionDistance;
	Data.bCanInteractMultipleTimes = true;
	Data.bRequiresLineOfSight = true;
	Data.InteractionPriority = 0;

	return Data;
}

bool ADoorActor::CanInteract_Implementation(AActor* InteractingActor) const
{
	return true; // Can always try to interact (locked doors will fail in BeginInteract)
}

void ADoorActor::OnInteractionFocused_Implementation(AActor* InteractingActor)
{
	// Highlight door
	if (DoorComponent)
	{
		DoorComponent->SetRenderCustomDepth(true);
		DoorComponent->SetCustomDepthStencilValue(bIsLocked ? 251 : 250); // Different color for locked doors
	}
}

void ADoorActor::OnInteractionUnfocused_Implementation(AActor* InteractingActor)
{
	// Remove highlight
	if (DoorComponent)
	{
		DoorComponent->SetRenderCustomDepth(false);
	}
}

void ADoorActor::OpenDoor()
{
	if (bIsOpen || bIsLocked)
	{
		return;
	}

	bIsOpen = true;
	BP_OnDoorOpened();

	// Setup auto-close timer
	if (bAutoClose && AutoCloseDelay > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoCloseTimerHandle, this, &ADoorActor::CloseDoor, AutoCloseDelay, false);
	}
}

void ADoorActor::CloseDoor()
{
	if (!bIsOpen)
	{
		return;
	}

	bIsOpen = false;
	BP_OnDoorClosed();

	// Clear auto-close timer
	if (AutoCloseTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoCloseTimerHandle);
	}
}

void ADoorActor::ToggleDoor()
{
	if (bIsOpen)
	{
		CloseDoor();
	}
	else
	{
		OpenDoor();
	}
}

void ADoorActor::SetLocked(bool bLocked)
{
	bIsLocked = bLocked;
}
