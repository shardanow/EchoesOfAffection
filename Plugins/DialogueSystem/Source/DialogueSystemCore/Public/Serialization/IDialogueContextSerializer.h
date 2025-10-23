// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Forward declarations
class UDialogueSessionContext;
class UDialogueNode;
struct FDialogueStateSnapshot;
struct FDialogueSessionSaveData;

/**
 * v1.3.1: Base interface for dialogue context serialization
 * 
 * Purpose:
 * - Provides abstraction layer for different serialization formats
 * - Enables easy addition of new formats (XML, MessagePack, etc.)
 * - Follows Interface Segregation Principle (ISP)
 * 
 * Architecture Benefits:
 * - SOLID: Open/Closed Principle - add new serializers without modifying existing code
 * - SOLID: Dependency Inversion - Context depends on interface, not concrete implementations
 * - Testability: Each serializer can be unit tested independently
 * - Maintainability: Clear separation of serialization concerns
 */
class DIALOGUESYSTEMCORE_API IDialogueContextSerializer
{
public:
    virtual ~IDialogueContextSerializer() = default;

 /**
     * Get serializer name for logging/debugging
     */
    virtual FString GetSerializerName() const = 0;

    /**
     * Check if serializer supports the given context state
     */
    virtual bool CanSerialize(const UDialogueSessionContext* Context) const
    {
        return Context != nullptr;
    }

    /**
     * Validate serialized data before deserialization
     */
    virtual bool ValidateData() const
    {
  return true;
    }
};

/**
 * Snapshot serialization interface
 */
class DIALOGUESYSTEMCORE_API IDialogueSnapshotSerializer : public IDialogueContextSerializer
{
public:
    /**
     * Create snapshot from context
     * @param Context - Context to snapshot
     * @param DialogueState - Current dialogue state enum
     * @param NodeHistory - Node navigation history
     * @return Snapshot structure
*/
    virtual FDialogueStateSnapshot CreateSnapshot(
        const UDialogueSessionContext* Context,
    uint8 DialogueState,
     const TArray<UDialogueNode*>& NodeHistory
    ) const = 0;

    /**
     * Restore context from snapshot
     * @param Context - Context to restore into
     * @param Snapshot - Snapshot to restore from
     * @return true if successful
     */
    virtual bool RestoreFromSnapshot(
        UDialogueSessionContext* Context,
        const FDialogueStateSnapshot& Snapshot
    ) const = 0;
};

/**
 * SaveGame serialization interface
 */
class DIALOGUESYSTEMCORE_API IDialogueSaveDataSerializer : public IDialogueContextSerializer
{
public:
  /**
     * Serialize context to save game data
   * @param Context - Context to serialize
     * @param DialogueId - ID of the dialogue being saved
     * @return Save game data structure
     */
    virtual FDialogueSessionSaveData ToSaveData(
        const UDialogueSessionContext* Context,
FName DialogueId
    ) const = 0;

    /**
     * Deserialize context from save game data
     * @param Context - Context to restore into
     * @param SaveData - Save data to restore from
     * @param InPlayer - Player actor reference
     * @param InNPC - NPC actor reference
     * @return true if successful
     */
    virtual bool FromSaveData(
    UDialogueSessionContext* Context,
        const FDialogueSessionSaveData& SaveData,
      AActor* InPlayer,
        AActor* InNPC
    ) const = 0;
};

/**
 * JSON serialization interface
 */
class DIALOGUESYSTEMCORE_API IDialogueJsonSerializer : public IDialogueContextSerializer
{
public:
    /**
     * Serialize context to JSON string
     * @param Context - Context to serialize
     * @param OutJson - Output JSON string
     */
    virtual void SaveToJson(
    const UDialogueSessionContext* Context,
        FString& OutJson
    ) const = 0;

    /**
     * Deserialize context from JSON string
   * @param Context - Context to restore into
     * @param Json - JSON string to parse
     * @return true if successful
     */
    virtual bool LoadFromJson(
        UDialogueSessionContext* Context,
        const FString& Json
    ) const = 0;
};
