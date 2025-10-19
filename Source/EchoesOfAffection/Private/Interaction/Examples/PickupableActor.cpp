// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/Examples/PickupableActor.h"
#include "Components/StaticMeshComponent.h"

APickupableActor::APickupableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Default settings
	ItemName = FText::FromString("Item");
	bCanBePickedUp = true;
	bDestroyOnPickup = true;
	PickupDistance = 200.0f;
}

void APickupableActor::BeginPlay()
{
	Super::BeginPlay();
}

EInteractionResult APickupableActor::BeginInteract_Implementation(AActor* InteractingActor)
{
	if (!bCanBePickedUp)
	{
		return EInteractionResult::NotInteractable;
	}

	PerformPickup(InteractingActor);
	return EInteractionResult::Success;
}

void APickupableActor::EndInteract_Implementation(AActor* InteractingActor)
{
	// Pickup is instant, nothing to do here
}

FInteractionData APickupableActor::GetInteractionData_Implementation(AActor* InteractingActor) const
{
	FInteractionData Data;
	Data.InteractionName = ItemName;
	Data.InteractionType = EInteractionType::Pickup;
	Data.ActionText = FText::Format(FText::FromString("Press E to Pick Up {0}"), ItemName);
	Data.InteractionDuration = 0.0f; // Instant pickup
	Data.MaxInteractionDistance = PickupDistance;
	Data.bCanInteractMultipleTimes = false;
	Data.bRequiresLineOfSight = true;
	Data.InteractionPriority = 0;

	return Data;
}

bool APickupableActor::CanInteract_Implementation(AActor* InteractingActor) const
{
	return bCanBePickedUp;
}

void APickupableActor::OnInteractionFocused_Implementation(AActor* InteractingActor)
{
	// Apply highlight effect
	if (MeshComponent)
	{
		MeshComponent->SetRenderCustomDepth(true);
		MeshComponent->SetCustomDepthStencilValue(250);
	}
}

void APickupableActor::OnInteractionUnfocused_Implementation(AActor* InteractingActor)
{
	// Remove highlight effect
	if (MeshComponent)
	{
		MeshComponent->SetRenderCustomDepth(false);
	}
}

void APickupableActor::SetPickupEnabled(bool bEnabled)
{
	bCanBePickedUp = bEnabled;
}

void APickupableActor::PerformPickup(AActor* PickedUpBy)
{
	// Call Blueprint event
	BP_OnPickedUp(PickedUpBy);

	// Destroy if configured to do so
	if (bDestroyOnPickup)
	{
		Destroy();
	}
	else
	{
		// Otherwise just disable it
		SetPickupEnabled(false);
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}
