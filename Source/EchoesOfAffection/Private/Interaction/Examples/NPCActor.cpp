// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/Examples/NPCActor.h"
#include "Components/SkeletalMeshComponent.h"

ANPCActor::ANPCActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create skeletal mesh component
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Default settings
	NPCName = FText::FromString("NPC");
	GreetingText = FText::FromString("Hello!");
	bCanTalk = true;
	TalkDistance = 250.0f;
	bIsInDialogue = false;
	CurrentTalkingActor = nullptr;
}

void ANPCActor::BeginPlay()
{
	Super::BeginPlay();
}

EInteractionResult ANPCActor::BeginInteract_Implementation(AActor* InteractingActor)
{
	if (!bCanTalk)
	{
		return EInteractionResult::NotInteractable;
	}

	if (bIsInDialogue)
	{
		return EInteractionResult::AlreadyInteracting;
	}

	if (!BP_CanTalkCheck(InteractingActor))
	{
		return EInteractionResult::RequirementsNotMet;
	}

	StartDialogue(InteractingActor);
	return EInteractionResult::Success;
}

void ANPCActor::EndInteract_Implementation(AActor* InteractingActor)
{
	// ? FIXED: Automatically end dialogue when interaction ends
	if (bIsInDialogue && CurrentTalkingActor == InteractingActor)
	{
		EndDialogue();
	}
}

FInteractionData ANPCActor::GetInteractionData_Implementation(AActor* InteractingActor) const
{
	FInteractionData Data;
	Data.InteractionName = NPCName;
	Data.InteractionType = EInteractionType::Talk;
	Data.ActionText = FText::Format(FText::FromString("Press E to Talk to {0}"), NPCName);
	Data.InteractionDuration = 0.0f; // Instant to start dialogue
	Data.MaxInteractionDistance = TalkDistance;
	Data.bCanInteractMultipleTimes = true;
	Data.bRequiresLineOfSight = true;
	Data.InteractionPriority = 1; // Higher priority than regular objects

	return Data;
}

bool ANPCActor::CanInteract_Implementation(AActor* InteractingActor) const
{
	if (!bCanTalk)
	{
		return false;
	}

	if (bIsInDialogue)
	{
		return false;
	}

	return BP_CanTalkCheck(InteractingActor);
}

void ANPCActor::OnInteractionFocused_Implementation(AActor* InteractingActor)
{
	// Could display greeting text in UI here
	if (MeshComponent)
	{
		MeshComponent->SetRenderCustomDepth(true);
		MeshComponent->SetCustomDepthStencilValue(252); // Different color for NPCs
	}
}

void ANPCActor::OnInteractionUnfocused_Implementation(AActor* InteractingActor)
{
	if (MeshComponent)
	{
		MeshComponent->SetRenderCustomDepth(false);
	}
}

void ANPCActor::StartDialogue(AActor* TalkingTo)
{
	if (bIsInDialogue || !bCanTalk)
	{
		return;
	}

	bIsInDialogue = true;
	CurrentTalkingActor = TalkingTo;

	// Call Blueprint event
	BP_OnDialogueStarted(TalkingTo);

	// Broadcast delegate
	OnDialogueStarted.Broadcast(TalkingTo);
}

void ANPCActor::EndDialogue()
{
	if (!bIsInDialogue)
	{
		return;
	}

	bIsInDialogue = false;
	CurrentTalkingActor = nullptr;

	// Call Blueprint event
	BP_OnDialogueEnded();

	// Broadcast delegate
	OnDialogueEnded.Broadcast();
}

void ANPCActor::SetDialogueEnabled(bool bEnabled)
{
	bCanTalk = bEnabled;
}

bool ANPCActor::BP_CanTalkCheck_Implementation(AActor* TalkingTo) const
{
	return true;
}
