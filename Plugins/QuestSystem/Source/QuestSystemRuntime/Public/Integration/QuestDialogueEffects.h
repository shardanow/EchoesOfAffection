// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestDialogueEffects.generated.h"

// Forward declarations
class UDialogueEffect;
class UDialogueSessionContext;
class UQuestAsset;

/**
 * Quest Dialogue Effect - Start Quest
 * 
 * Usage in Dialog System:
 * 1. Add this effect to DialogueNode
 * 2. Set Quest ID
 * 3. Effect will be executed when node is processed
 * 
 * Integration:
 * - This class is in Quest System plugin
 * - Inherits from UDialogueEffect (Dialog System)
 * - No hardcoded dependencies
 */
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "Start Quest"))
class QUESTSYSTEMRUNTIME_API UQuestDialogueEffect_StartQuest : public UObject
{
    GENERATED_BODY()

public:
    /** Quest ID to start */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName QuestId;

    /** Optional: Actor who accepts the quest (if null, uses player from context) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    TObjectPtr<AActor> QuestAcceptor;

    /**
 * Execute effect
     * Called by Dialog System when node is processed
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void Execute(UObject* WorldContextObject);

    /**
     * Get display text for UI
     */
    UFUNCTION(BlueprintPure, Category = "Quest")
    FText GetDisplayText() const;

protected:
    /** Get quest subsystem from world */
    class UQuestSubsystem* GetQuestSubsystem(UObject* WorldContextObject) const;

private:
    /** Callback for quest start */
    UFUNCTION()
    void OnQuestStartedCallback(bool bSuccess, class UQuestAsset* QuestAsset);
};

/**
 * Quest Dialogue Effect - Complete Quest Objective
 * 
 * Usage:
 * - Completes specific objective when dialogue node is processed
 * - Useful for "talk to NPC" objectives
 */
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "Complete Quest Objective"))
class QUESTSYSTEMRUNTIME_API UQuestDialogueEffect_CompleteObjective : public UObject
{
    GENERATED_BODY()

public:
    /** Quest ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName QuestId;

    /** Objective ID to complete */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName ObjectiveId;

  /**
     * Execute effect
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void Execute(UObject* WorldContextObject);

  /**
     * Get display text for UI
     */
    UFUNCTION(BlueprintPure, Category = "Quest")
    FText GetDisplayText() const;

protected:
    class UQuestSubsystem* GetQuestSubsystem(UObject* WorldContextObject) const;
};

/**
 * Quest Dialogue Effect - Emit Quest Event
 * 
 * Usage:
 * - Emits custom quest event (e.g., "NpcTalked", "ItemGiven")
 * - Quest objectives can listen to these events
 */
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "Emit Quest Event"))
class QUESTSYSTEMRUNTIME_API UQuestDialogueEffect_EmitEvent : public UObject
{
    GENERATED_BODY()

public:
    /** Event type (e.g., "NpcTalked") */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName EventType;

    /** NPC ID (for NpcTalked events) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
  FName NpcId;

    /** Optional: Item ID (for item-related events) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName ItemId;

    /** Optional: Count (for counting events) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 Count = 1;

    /**
     * Execute effect
     */
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void Execute(UObject* WorldContextObject, AActor* Instigator);

    /**
   * Get display text for UI
     */
UFUNCTION(BlueprintPure, Category = "Quest")
    FText GetDisplayText() const;

protected:
    class UQuestSubsystem* GetQuestSubsystem(UObject* WorldContextObject) const;
};
