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
}
