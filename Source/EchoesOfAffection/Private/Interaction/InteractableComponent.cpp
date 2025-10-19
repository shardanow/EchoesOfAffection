// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/InteractableComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Default settings
	bIsEnabled = true;
	bHighlightWhenFocused = true;
	MaxInteractionCount = -1;
	InteractionCount = 0;
	bIsFocused = false;

	// Default interaction data
	InteractionData.InteractionName = FText::FromString("Interact");
	InteractionData.InteractionType = EInteractionType::Use;
	InteractionData.ActionText = FText::FromString("Press E to Interact");
	InteractionData.InteractionDuration = 0.0f;
	InteractionData.MaxInteractionDistance = 200.0f;
	InteractionData.bCanInteractMultipleTimes = true;
	InteractionData.bRequiresLineOfSight = true;
	InteractionData.InteractionPriority = 0;
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

EInteractionResult UInteractableComponent::BeginInteract_Implementation(AActor* InteractingActor)
{
	if (!bIsEnabled)
	{
		return EInteractionResult::NotInteractable;
	}

	if (HasReachedInteractionLimit())
	{
		return EInteractionResult::RequirementsNotMet;
	}

	InteractionCount++;

	// Broadcast delegate for Blueprint
	OnBeginInteract.Broadcast(InteractingActor);

	return EInteractionResult::Success;
}

void UInteractableComponent::EndInteract_Implementation(AActor* InteractingActor)
{
	// Broadcast delegate for Blueprint
	OnEndInteract.Broadcast(InteractingActor);

	// Disable if reached max interaction count
	if (HasReachedInteractionLimit())
	{
		SetInteractableEnabled(false);
	}
}

void UInteractableComponent::CancelInteract_Implementation(AActor* InteractingActor)
{
	// Revert interaction count since it was cancelled
	if (InteractionCount > 0)
	{
		InteractionCount--;
	}

	// Broadcast delegate for Blueprint
	OnCancelInteract.Broadcast(InteractingActor);
}

FInteractionData UInteractableComponent::GetInteractionData_Implementation(AActor* InteractingActor) const
{
	return InteractionData;
}

bool UInteractableComponent::CanInteract_Implementation(AActor* InteractingActor) const
{
	if (!bIsEnabled)
	{
		return false;
	}

	if (HasReachedInteractionLimit())
	{
		return false;
	}

	// Call Blueprint validation
	return BP_CanInteractCheck(InteractingActor);
}

void UInteractableComponent::OnInteractionFocused_Implementation(AActor* InteractingActor)
{
	bIsFocused = true;

	if (bHighlightWhenFocused)
	{
		ApplyHighlight(true);
	}

	// Broadcast delegate for Blueprint
	OnFocused.Broadcast(InteractingActor);
}

void UInteractableComponent::OnInteractionUnfocused_Implementation(AActor* InteractingActor)
{
	bIsFocused = false;

	if (bHighlightWhenFocused)
	{
		ApplyHighlight(false);
	}

	// Broadcast delegate for Blueprint
	OnUnfocused.Broadcast(InteractingActor);
}

void UInteractableComponent::SetInteractableEnabled(bool bEnabled)
{
	if (bIsEnabled != bEnabled)
	{
		bIsEnabled = bEnabled;
		OnInteractableStateChanged.Broadcast(bIsEnabled);
	}
}

bool UInteractableComponent::BP_CanInteractCheck_Implementation(AActor* InteractingActor) const
{
	return true;
}

void UInteractableComponent::ApplyHighlight_Implementation(bool bHighlight)
{
	// Default implementation: Apply custom depth stencil for post-process outline
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* Component : PrimitiveComponents)
	{
		if (Component)
		{
			Component->SetRenderCustomDepth(bHighlight);
			if (bHighlight)
			{
				Component->SetCustomDepthStencilValue(250); // Can be configured for different highlight colors
			}
		}
	}
}

bool UInteractableComponent::HasReachedInteractionLimit() const
{
	if (MaxInteractionCount < 0)
	{
		return false; // Unlimited
	}

	return InteractionCount >= MaxInteractionCount;
}
