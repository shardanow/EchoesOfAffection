// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/DialogueComponent.h"
#include "Interfaces/IDialogueParticipant.h"
#include "Subsystems/DialogueSubsystem.h"
#include "Components/RelationshipComponent.h"
#include "Core/DialogueRunner.h"
#include "Core/DialogueContext.h"
#include "Data/DialogueDataAsset.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"

UDialogueComponent::UDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDialogueComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache Relationship Component
    AActor* Owner = GetOwner();
    if (Owner)
    {
        RelationshipComp = Owner->FindComponentByClass<URelationshipComponent>();
    }
}

void UDialogueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update cooldown
    if (DialogueCooldown > 0.0f)
    {
        DialogueCooldown -= DeltaTime;
    }
}

UTexture2D* UDialogueComponent::GetParticipantPortrait_Implementation() const
{
    return Portrait.LoadSynchronous();
}

bool UDialogueComponent::CanStartDialogue_Implementation(AActor* Initiator) const
{
    // Default: can always start dialogue
    if (!bCanStartDialogue)
    {
        return false;
    }

    if (DialogueCooldown > 0.0f)
    {
        return false;
    }

    if (IsInDialogue())
    {
        return false;
    }

    // Check distance
    if (Initiator && GetOwner())
    {
        float Distance = FVector::Distance(GetOwner()->GetActorLocation(), Initiator->GetActorLocation());
        if (Distance > InteractionDistance)
        {
            return false;
        }
    }

    return true;
}

void UDialogueComponent::OnDialogueStarted_Implementation(UDialogueRunner* Runner)
{
    // Override in blueprints
    ActiveRunner = Runner;
    SetComponentTickEnabled(true);
}

void UDialogueComponent::OnDialogueEnded_Implementation()
{
    // Override in blueprints
    ActiveRunner = nullptr;
    SetComponentTickEnabled(false);
}

bool UDialogueComponent::StartDialogue(AActor* Initiator)
{
    if (!CanStartDialogue(Initiator))
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    UDialogueSubsystem* DialogueSubsystem = World->GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    if (!DialogueSubsystem)
    {
        return false;
    }

    // Create context
    UDialogueSessionContext* Context = NewObject<UDialogueSessionContext>();
    Context->Player = Initiator;
    Context->NPC = GetOwner();

    // Select dialogue
    UDialogueDataAsset* SelectedDialogue = SelectDialogue(Context);
    if (!SelectedDialogue)
    {
        return false;
    }

    // Start dialogue through subsystem
    UDialogueRunner* Runner = DialogueSubsystem->StartDialogue(SelectedDialogue, Initiator, GetOwner());
    if (Runner)
    {
        ActiveRunner = Runner;
        LastContext = Context;
        
        // Bind to end event
        Runner->OnDialogueEnded.AddDynamic(this, &UDialogueComponent::HandleDialogueEnded);
        OnDialogueStarted(Runner);
        
        return true;
    }

    return false;
}

UDialogueDataAsset* UDialogueComponent::SelectDialogue(const UDialogueSessionContext* Context)
{
    // Try conditional dialogues first
    for (const TPair<FName, TSoftObjectPtr<UDialogueDataAsset>>& Pair : ConditionalDialogues)
    {
        UDialogueDataAsset* ConditionalDialogue = Pair.Value.LoadSynchronous();
        if (ConditionalDialogue && RelationshipComp)
        {
            // Check if this dialogue's conditions are met
            float CurrentAffinity = RelationshipComp->GetAffinity();
            if (CurrentAffinity >= ConditionalDialogue->MinAffinityRequired)
            {
                return ConditionalDialogue;
            }
        }
    }

    // Fallback to default
    return DefaultDialogue.LoadSynchronous();
}

void UDialogueComponent::HandleDialogueEnded()
{
    if (ActiveRunner)
    {
        ActiveRunner->OnDialogueEnded.RemoveDynamic(this, &UDialogueComponent::HandleDialogueEnded);
        ActiveRunner = nullptr;
    }

    OnDialogueEnded();
}
