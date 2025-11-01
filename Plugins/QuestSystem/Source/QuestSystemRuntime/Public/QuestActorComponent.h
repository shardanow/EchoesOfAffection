// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "QuestActorComponent.generated.h"

/**
 * Quest Actor Component v1.0
 * 
 * Marks an actor as participant in the quest system and provides unified quest-related identification.
 * 
 * Key Features:
 * - Unified quest actor identification (auto-extract from tags or actor name)
 * - Quest-specific gameplay tags for categorization
 * - Metadata storage for quest-related data
 * - Role flags (quest giver, target, event emitter)
 * - Automatic validation and logging
 * 
 * Usage:
 * 1. Add to any actor that participates in quests (NPCs, items, locations, quest objects)
 * 2. Set QuestActorId manually or use bAutoAssignId
 * 3. Add QuestActorTags for categorization (Quest.NPC, Quest.Item, etc.)
 * 4. Configure role flags as needed
 * 
 * Benefits:
 * - Single source of truth for quest actor identity
 * - Easy to find quest-participating actors
 * - Extensible metadata system
 * - Blueprint and C++ friendly
 */
UCLASS(ClassGroup=(Quest), meta=(BlueprintSpawnableComponent, DisplayName="Quest Actor Component"))
class QUESTSYSTEMRUNTIME_API UQuestActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestActorComponent();

	// === CORE IDENTIFICATION ===
	
	/**
	 * Unique ID of this actor in the quest system.
	 * Used for quest conditions (NpcId, ItemId, LocationId, etc.)
	 * If empty and bAutoAssignId is true, will be extracted from GameplayTags or actor name.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Identity")
	FName QuestActorId;
	
	/**
	 * Automatically extract QuestActorId from:
	 * 1. GameplayTags (e.g., "NPC.Lianne" -> "Lianne")
	 * 2. Actor name (e.g., "BP_Lianne_C_1" -> "Lianne")
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Identity")
	bool bAutoAssignId;
	
	/**
	 * Quest-specific tags for categorization and filtering.
	 * Examples: Quest.NPC, Quest.Item, Quest.Location, Quest.QuestGiver, Quest.Friendly, etc.
	 * Note: This is separate from actor's GameplayTags (which are used for NPC ID extraction)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Identity", meta=(Categories="Quest"))
	FGameplayTagContainer QuestActorTags;
	
	// === DISPLAY INFO ===
	
	/**
	 * Localized display name for UI.
	 * Used in quest logs, notifications, dialogue UI, etc.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Display")
	FText DisplayName;
	
	/**
	 * Localized description for UI.
	 * Used in quest details, tooltips, etc.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Display")
	FText Description;
	
	/**
	 * Optional icon for UI representation.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Display")
	TObjectPtr<UTexture2D> Icon;
	
	// === QUEST ROLES ===
	
	/**
	 * Can this actor give quests to players?
	 * If true, QuestGiverComponent should also be present.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Roles")
	bool bCanGiveQuests;
	
	/**
	 * Can this actor be a target of quest objectives?
	 * Examples: Talk to NPC, collect item, reach location, interact with object
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Roles")
	bool bCanBeQuestTarget;
	
	/**
	 * Can this actor emit quest-related events?
	 * If false, events from this actor will be ignored by quest system.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Roles")
	bool bCanEmitQuestEvents;
	
	/**
	 * Can this actor be tracked by quest tracker UI?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Roles")
	bool bCanBeTracked;
	
	// === METADATA ===
	
	/**
	 * Custom key-value metadata for quest-specific data.
	 * Examples:
	 * - "Faction" -> "TownGuard"
	 * - "QuestGiverType" -> "MainQuest"
	 * - "Rarity" -> "Legendary"
	 * - "RequiredLevel" -> "5"
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Actor|Metadata")
	TMap<FName, FString> Metadata;
	
	// === API: GETTERS ===
	
	/**
	 * Get the final quest actor ID (from QuestActorId or auto-extracted).
	 * This is the ID used in quest conditions.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	FName GetQuestActorId() const;
	
	/**
	 * Get display name or fallback to quest actor ID.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	FText GetDisplayNameOrId() const;
	
	/**
	 * Check if this actor has a specific quest tag.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	bool HasQuestTag(FGameplayTag Tag) const;
	
	/**
	 * Check if this actor has any tag matching the given parent tag.
	 * Example: HasQuestTagMatching("Quest.NPC") returns true for "Quest.NPC.Friendly"
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	bool HasQuestTagMatching(FGameplayTag ParentTag) const;
	
	/**
	 * Get all quest tags.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	FGameplayTagContainer GetQuestTags() const { return QuestActorTags; }
	
	// === API: METADATA ===
	
	/**
	 * Get metadata value by key.
	 * @param Key Metadata key
	 * @param bFound Output parameter indicating if key was found
	 * @return Metadata value or empty string if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	FString GetMetadata(FName Key, bool& bFound) const;
	
	/**
	 * Set metadata value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Actor")
	void SetMetadata(FName Key, const FString& Value);
	
	/**
	 * Remove metadata by key.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Actor")
	void RemoveMetadata(FName Key);
	
	/**
	 * Check if metadata key exists.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	bool HasMetadata(FName Key) const;
	
	/**
	 * Get all metadata keys.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	TArray<FName> GetMetadataKeys() const;
	
	// === API: EVENTS ===
	
	/**
	 * Emit a quest event from this actor.
	 * Automatically uses this actor as Instigator and fills in QuestActorId.
	 * @param EventTag Event tag (e.g., "GameEvent.Quest.CustomEvent")
	 * @param IntParam Optional integer parameter
	 * @param Target Optional target actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Actor")
	void EmitQuestEvent(FGameplayTag EventTag, int32 IntParam = 0, AActor* Target = nullptr);
	
	// === API: VALIDATION ===
	
	/**
	 * Validate component configuration.
	 * Checks for common mistakes and logs warnings.
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Actor")
	void ValidateConfiguration();
	
	/**
	 * Check if component is properly configured.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Actor")
	bool IsValidConfiguration() const;

protected:
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	/**
	 * Auto-extract quest actor ID from GameplayTags or actor name.
	 */
	void AutoAssignQuestActorId();
	
	/**
	 * Extract ID from GameplayTags (e.g., "NPC.Lianne" -> "Lianne")
	 */
	FName ExtractIdFromGameplayTags() const;
	
	/**
	 * Extract ID from actor name (e.g., "BP_Lianne_C_1" -> "Lianne")
	 */
	FName ExtractIdFromActorName() const;
	
	/**
	 * Validate settings and log warnings for common mistakes.
	 */
	void ValidateSettings();

private:
	/** Cached final ID (after auto-assignment) */
	UPROPERTY(Transient)
	FName CachedQuestActorId;
	
	/** Flag indicating if ID was auto-assigned */
	UPROPERTY(Transient)
	bool bWasIdAutoAssigned;
};
