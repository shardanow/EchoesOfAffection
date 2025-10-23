// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/IDialogueContextSerializer.h"

// Forward declarations
class UDialogueSessionContext;
class AActor;
struct FDialogueSessionSaveData;

/**
 * v1.3.1: SaveGame data serializer for persistent storage
 * 
 * Purpose:
 * - Converts dialogue context to/from UE SaveGame format
 * - Handles actor soft references (FSoftObjectPath)
 * - Manages conversation history conversion
 * 
 * Architecture:
 * - Single Responsibility: Only handles SaveGame serialization
 * - Stateless: Can be used as static utility or singleton
 * - Component-aware: Delegates to State/Participants/Variables components
 * 
 * Usage:
 * ```cpp
 * FDialogueSaveDataSerializer Serializer;
 * FDialogueSessionSaveData SaveData = Serializer.ToSaveData(Context, DialogueId);
 * // Save SaveData to disk...
 * // ... later ...
 * Serializer.FromSaveData(Context, SaveData, Player, NPC);
 * ```
 */
class DIALOGUESYSTEMCORE_API FDialogueSaveDataSerializer : public IDialogueSaveDataSerializer
{
public:
    FDialogueSaveDataSerializer() = default;
    virtual ~FDialogueSaveDataSerializer() = default;

    // IDialogueContextSerializer interface
    virtual FString GetSerializerName() const override { return TEXT("SaveDataSerializer"); }

    // IDialogueSaveDataSerializer interface
    virtual FDialogueSessionSaveData ToSaveData(
        const UDialogueSessionContext* Context,
        FName DialogueId
 ) const override;

    virtual bool FromSaveData(
   UDialogueSessionContext* Context,
        const FDialogueSessionSaveData& SaveData,
     AActor* InPlayer,
        AActor* InNPC
  ) const override;

    /**
     * Get singleton instance (optional - can also instantiate directly)
     */
    static const FDialogueSaveDataSerializer& Get()
{
        static FDialogueSaveDataSerializer Instance;
 return Instance;
    }
};
