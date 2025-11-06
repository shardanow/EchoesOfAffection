// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogueParticipants.generated.h"

// Forward declarations
class AActor;

/**
 * Dialogue Participants Management
 * 
 * Handles:
 * - Player actor reference
 * - NPC actor reference
 * - Additional participants (future)
 * - Base affinity level (relationship data)
 * 
 * v1.3.1: Extracted from DialogueContext (SRP principle)
 * 
 * Benefits:
 * - Single Responsibility: Only participant management
 * - Easy to extend with additional participants
 * - Cleaner API
 * - Reusable in different contexts
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueParticipants : public UObject
{
    GENERATED_BODY()

public:
    //==========================================================================
    // Player Management
    //==========================================================================

    /** Get player actor */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
    AActor* GetPlayer() const { return Player; }

    /** Set player actor */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
    void SetPlayer(AActor* InPlayer);

    /** Check if player is set */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
 bool HasPlayer() const { return Player != nullptr; }

    //==========================================================================
    // NPC Management
    //==========================================================================

    /** Get NPC actor */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
    AActor* GetNPC() const { return NPC; }

    /** Set NPC actor */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
    void SetNPC(AActor* InNPC);

  /** Check if NPC is set */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
    bool HasNPC() const { return NPC != nullptr; }

  //==========================================================================
    // Relationship Data
 //==========================================================================

    /** Get base affinity level between player and NPC */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
    float GetBaseAffinityLevel() const { return BaseAffinityLevel; }

    /** Set base affinity level */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
    void SetBaseAffinityLevel(float Level);

    //==========================================================================
    // Additional Participants (Future)
    // v1.16: Extended for Multi-NPC Dialogue Support
    //==========================================================================

    /** NEW v1.16: Get actor by Persona ID (supports Primary, Additional NPCs, Player) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
    AActor* GetActorByPersonaId(FName PersonaId) const;

    /** NEW v1.16: Set actor for specific Persona ID */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
    void SetActorForPersona(FName PersonaId, AActor* Actor);

    /** NEW v1.16: Get all registered Persona IDs */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
    TArray<FName> GetAllPersonaIds() const;

    /** Add additional participant */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
    void AddAdditionalParticipant(UObject* Participant);

  /** Remove additional participant */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
    void RemoveAdditionalParticipant(UObject* Participant);

    /** Get all additional participants */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
    TArray<UObject*> GetAdditionalParticipants() const;

    //==========================================================================
    // Validation
    //==========================================================================

    /** Check if participants are valid (Player and NPC set) */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
    bool IsValid() const;

    /** Reset all participants */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
    void Reset();

protected:
    /** Player actor */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participants")
    TObjectPtr<AActor> Player;

    /** NPC actor */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participants")
    TObjectPtr<AActor> NPC;

    /** Base affinity level (relationship strength) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participants")
    float BaseAffinityLevel = 0.0f;

    /** Additional participants (for group dialogues, future feature) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participants")
    TArray<TObjectPtr<UObject>> AdditionalParticipants;

    /** NEW v1.16: Persona ID to Actor mapping for multi-NPC dialogues */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Participants")
  TMap<FName, TObjectPtr<AActor>> PersonaActorMap;

  friend class UDialogueSessionContext;
};
