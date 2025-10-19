// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDialogueService.generated.h"

// Forward declarations
class UDialogueRunner;
class UDialogueDataAsset;
struct FDialogueSessionSaveData;

/**
 * Service interface for dialogue management
 * Provides decoupling between DialogueComponent and DialogueSubsystem
 * 
 * Benefits:
 * - Dependency Inversion Principle (DIP)
 * - Easy unit testing and mocking
 * - Flexible implementation swapping
 * - Clear contract definition
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UDialogueService : public UInterface
{
    GENERATED_BODY()
};

class DIALOGUESYSTEMCORE_API IDialogueService
{
    GENERATED_BODY()

public:
    /**
     * Start a dialogue between Player and NPC
     * @param DialogueAsset The dialogue data to execute
     * @param Player The player actor
     * @param NPC The NPC actor
     * @return Runner instance or nullptr if failed
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    UDialogueRunner* StartDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC);
    virtual UDialogueRunner* StartDialogue_Implementation(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC) { return nullptr; }

    /**
     * Resume dialogue from saved state
     * @param DialogueAsset The dialogue data
     * @param Player The player actor
     * @param NPC The NPC actor
     * @param SaveData The saved dialogue state
     * @return Runner instance or nullptr if failed
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    UDialogueRunner* ResumeDialogue(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC, const FDialogueSessionSaveData& SaveData);
    virtual UDialogueRunner* ResumeDialogue_Implementation(UDialogueDataAsset* DialogueAsset, AActor* Player, AActor* NPC, const FDialogueSessionSaveData& SaveData) { return nullptr; }

    /**
     * End the active dialogue
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    void EndActiveDialogue();
    virtual void EndActiveDialogue_Implementation() {}

    /**
     * Check if dialogue is currently active
     * @return true if dialogue is running
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    bool IsDialogueActive() const;
    virtual bool IsDialogueActive_Implementation() const { return false; }

    /**
     * Get the active dialogue runner
     * @return Active runner or nullptr
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    UDialogueRunner* GetActiveDialogue() const;
    virtual UDialogueRunner* GetActiveDialogue_Implementation() const { return nullptr; }

    /**
     * Load dialogue asset by ID
     * @param DialogueId The dialogue identifier
     * @return Loaded dialogue asset or nullptr
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue|Service")
    UDialogueDataAsset* LoadDialogue(FName DialogueId);
    virtual UDialogueDataAsset* LoadDialogue_Implementation(FName DialogueId) { return nullptr; }
};
