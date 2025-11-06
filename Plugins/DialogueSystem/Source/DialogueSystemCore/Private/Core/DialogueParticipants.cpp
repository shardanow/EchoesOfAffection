// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueParticipants.h"
#include "GameFramework/Actor.h"

//==============================================================================
// Player Management
//==============================================================================

void UDialogueParticipants::SetPlayer(AActor* InPlayer)
{
    Player = InPlayer;
}

//==============================================================================
// NPC Management
//==============================================================================

void UDialogueParticipants::SetNPC(AActor* InNPC)
{
    NPC = InNPC;
}

//==============================================================================
// Relationship Data
//==============================================================================

void UDialogueParticipants::SetBaseAffinityLevel(float Level)
{
    BaseAffinityLevel = Level;
}

//==============================================================================
// Additional Participants
//==============================================================================

void UDialogueParticipants::AddAdditionalParticipant(UObject* Participant)
{
    if (Participant)
    {
        AdditionalParticipants.AddUnique(Participant);
    }
}

void UDialogueParticipants::RemoveAdditionalParticipant(UObject* Participant)
{
    AdditionalParticipants.Remove(Participant);
}

TArray<UObject*> UDialogueParticipants::GetAdditionalParticipants() const
{
    TArray<UObject*> Result;
    for (const TObjectPtr<UObject>& Obj : AdditionalParticipants)
    {
        if (Obj)
        {
            Result.Add(Obj.Get());
        }
    }
    return Result;
}

//==============================================================================
// Validation
//==============================================================================

bool UDialogueParticipants::IsValid() const
{
    return Player != nullptr && NPC != nullptr;
}

void UDialogueParticipants::Reset()
{
    Player = nullptr;
    NPC = nullptr;
    BaseAffinityLevel = 0.0f;
    AdditionalParticipants.Empty();
    PersonaActorMap.Empty(); // v1.16
}

//==============================================================================
// v1.16: Multi-NPC Dialogue Support
//==============================================================================

AActor* UDialogueParticipants::GetActorByPersonaId(FName PersonaId) const
{
    if (PersonaId.IsNone())
    {
        return nullptr;
    }

    // Check PersonaActorMap first (primary source of truth)
    if (const TObjectPtr<AActor>* Found = PersonaActorMap.Find(PersonaId))
    {
        return Found->Get();
    }

    // Not found
    return nullptr;
}

void UDialogueParticipants::SetActorForPersona(FName PersonaId, AActor* Actor)
{
    if (PersonaId.IsNone() || !Actor)
    {
        return;
    }

    PersonaActorMap.Add(PersonaId, Actor);

    // Keep legacy NPC reference in sync (for backward compatibility)
    // Assume first registered persona is the primary NPC
    if (!NPC)
    {
        NPC = Actor;
    }
}

TArray<FName> UDialogueParticipants::GetAllPersonaIds() const
{
    TArray<FName> PersonaIds;
    PersonaActorMap.GetKeys(PersonaIds);
    return PersonaIds;
}
