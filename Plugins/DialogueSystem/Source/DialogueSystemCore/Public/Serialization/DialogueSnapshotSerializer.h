// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/IDialogueContextSerializer.h"

// Forward declarations
class UDialogueSessionContext;
class UDialogueNode;
struct FDialogueStateSnapshot;

/**
 * v1.3.1: Snapshot serializer for undo/redo functionality
 * 
 * Purpose:
 * - Creates lightweight snapshots of dialogue state
 * - Enables perfect undo/redo support
 * - Captures all component state efficiently
 * 
 * Architecture:
 * - Single Responsibility: Only handles snapshot serialization
 * - Stateless: Can be used as static utility or singleton
 * - Component-aware: Delegates to State/Participants/Variables components
 * 
 * Usage:
 * ```cpp
 * FDialogueSnapshotSerializer Serializer;
 * FDialogueStateSnapshot Snapshot = Serializer.CreateSnapshot(Context, State, History);
 * // ... later ...
 * Serializer.RestoreFromSnapshot(Context, Snapshot);
 * ```
 */
class DIALOGUESYSTEMCORE_API FDialogueSnapshotSerializer : public IDialogueSnapshotSerializer
{
public:
    FDialogueSnapshotSerializer() = default;
    virtual ~FDialogueSnapshotSerializer() = default;

    // IDialogueContextSerializer interface
    virtual FString GetSerializerName() const override { return TEXT("SnapshotSerializer"); }

    // IDialogueSnapshotSerializer interface
    virtual FDialogueStateSnapshot CreateSnapshot(
        const UDialogueSessionContext* Context,
        uint8 DialogueState,
     const TArray<UDialogueNode*>& NodeHistory
    ) const override;

    virtual bool RestoreFromSnapshot(
        UDialogueSessionContext* Context,
        const FDialogueStateSnapshot& Snapshot
    ) const override;

    /**
     * Get singleton instance (optional - can also instantiate directly)
     */
    static const FDialogueSnapshotSerializer& Get()
    {
        static FDialogueSnapshotSerializer Instance;
        return Instance;
    }
};
