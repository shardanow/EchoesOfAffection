// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffectExecutor.h"
#include "GameplayTagContainer.h"
#include "DialogueMemoryEffects.generated.h"

// Forward declarations
class UDialogueSessionContext;

/**
 * Dialogue Effect: Add Memory to NPC
 * 
 * Wrapper around UAddMemoryEffect that works with DialogueSessionContext.
 * Allows adding memories directly from Dialogue Data Assets.
 * 
 * Usage in Data Asset:
 *   DialogueNode ? Effects ? Add Element ? UDialogueEffect_AddMemory
 *   - Memory Tag: "Event.Betrayal"
 *   - Memory Strength: 0.95
 */
UCLASS(DisplayName = "Add Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_AddMemory : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_AddMemory() { bSupportsReverse = false; } // Cannot undo memory creation

	/** Gameplay Tag for the memory (e.g., "Event.Betrayal", "Event.Help") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	/** Strength of the memory (0.0 - 1.0). Higher = more important */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MemoryStrength = 1.0f;

	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Dialogue Effect: Modify Memory Strength
 * 
 * Changes the strength of an existing memory.
 * 
 * Usage:
 *   - Memory Tag: "Event.Help"
 *   - Strength Modifier: 0.2 (increase by 20%)
 *   - Additive: true
 */
UCLASS(DisplayName = "Modify Memory Strength", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_ModifyMemoryStrength : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_ModifyMemoryStrength() { bSupportsReverse = false; }

	/** Gameplay Tag for the memory to modify */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	/** Amount to modify strength by */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float StrengthModifier = 0.1f;

	/** If true, adds modifier. If false, sets to modifier value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bAdditive = true;

	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Dialogue Effect: Remove Memory
 * 
 * Completely removes a memory from NPC.
 * 
 * Usage:
 *   - Memory Tag: "Event.OldPromise"
 */
UCLASS(DisplayName = "Remove Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_RemoveMemory : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_RemoveMemory() { bSupportsReverse = false; }

	/** Gameplay Tag for the memory to remove */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Dialogue Effect: Refresh Memory
 * 
 * "Refreshes" a memory by updating its timestamp and increasing freshness.
 * Simulates NPC "recalling" the memory.
 * 
 * Usage:
 *   - Memory Tag: "Event.Promise"
 */
UCLASS(DisplayName = "Refresh Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_RefreshMemory : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_RefreshMemory() { bSupportsReverse = false; }

	/** Gameplay Tag for the memory to refresh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
};

/**
 * Dialogue Effect: Create Full Memory Entry
 * 
 * Creates a complete, detailed memory with description, importance, emotion, and context tags.
 * Use this for important story moments that need personalized NPC reactions.
 * 
 * Features:
 * - Custom description text (unique per NPC)
 * - Importance level (affects decay rate)
 * - Emotional context (affects NPC behavior)
 * - Multiple context tags for filtering
 * - Optional link to player actor
 * 
 * Usage in Data Asset:
 *   DialogueNode ? Effects ? Add Element ? UDialogueEffect_CreateMemory
 *   - Description: "The traveler betrayed my trust and revealed my secret..."
 *   - Importance: 95.0
 *   - Emotion: Traumatic
 *   - Context Tags: [Event.Betrayal, Location.Village, NPC.Guard]
 * 
 * Example Scenarios:
 * - Major story choices (betrayal, sacrifice, romance)
 * - Quest completions with emotional impact
 * - Promises made/broken
 * - Character-defining moments
 */
UCLASS(DisplayName = "Create Memory (Full)", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_CreateMemory : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_CreateMemory() { bSupportsReverse = false; }

	/** 
	 * Description of what happened (shown in NPC thoughts/UI)
	 * Make it personal and specific to this NPC's perspective.
	 * Example: "This stranger saved my life when bandits attacked the village."
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory", meta = (MultiLine = true))
	FText MemoryDescription;

	/** 
	 * Importance (0-100). Higher values:
	 * - Decay slower (longer retention)
	 * - Less likely to be forgotten completely
	 * - More likely to influence NPC behavior
	 * 
	 * Guidelines:
	 * 0-30:   Minor events (casual conversation)
	 * 31-60:  Significant events (quest help, gift)
	 * 61-90:  Major events (betrayal, rescue, romance)
	 * 91-100: Life-changing (traumatic, transformative)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory", meta = (ClampMin = "0", ClampMax = "100"))
	float Importance = 50.0f;

	/**
	 * Context tags for categorization and filtering.
	 * Use multiple tags to describe the memory from different angles.
	 * 
	 * Recommended tag categories:
	 * - Event.*     (Event.Help, Event.Betrayal, Event.Gift)
	 * - Quest.*     (Quest.VillageDefense.Complete)
	 * - Promise.*   (Promise.KeepSecret, Promise.FindSister)
	 * - Emotion.*   (Emotion.Joyful, Emotion.Angry)
	 * - Location.*  (Location.Village, Location.Forest)
	 * - Character.* (Character.Anna.Met)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	FGameplayTagContainer ContextTags;

	/** 
	 * Should this memory be linked to the Player actor?
	 * If true, the memory will track the player as the related actor.
	 * Useful for: player actions, promises, relationships.
	 * Disable for: observations, environmental memories.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	bool bLinkToPlayer = true;

	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
};
