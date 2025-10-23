// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/IDialogueContextSerializer.h"

// Forward declarations
class UDialogueSessionContext;

/**
 * v1.3.1: JSON serializer for human-readable persistence
 * 
 * Purpose:
 * - Converts dialogue context to/from JSON format
 * - Enables debugging and manual editing
 * - Supports export/import for external tools
 * 
 * Architecture:
 * - Single Responsibility: Only handles JSON serialization
 * - Stateless: Can be used as static utility or singleton
 * - Component-aware: Delegates to State/Participants/Variables components
 * 
 * JSON Structure:
 * ```json
 * {
 *   "CustomVariables": { "key": "value", ... },
 *   "VisitedNodes": ["NodeId1", "NodeId2", ...],
 *   "History": [
 *     { "Speaker": "...", "Text": "...", "NodeId": "...", "Timestamp": "..." },
 *   ...
 *   ],
 *   "ActiveTags": ["Tag.Gameplay.Dialogue", ...],
 *   "BaseAffinity": 50.0,
 *   "SessionStart": "2024-01-01T12:00:00",
 *   "CurrentNode": "NodeId"
 * }
 * ```
 * 
 * Usage:
 * ```cpp
 * FDialogueJsonSerializer Serializer;
 * FString Json;
 * Serializer.SaveToJson(Context, Json);
 * // Save Json to file...
 * // ... later ...
 * Serializer.LoadFromJson(Context, Json);
 * ```
 */
class DIALOGUESYSTEMCORE_API FDialogueJsonSerializer : public IDialogueJsonSerializer
{
public:
    FDialogueJsonSerializer() = default;
    virtual ~FDialogueJsonSerializer() = default;

  // IDialogueContextSerializer interface
    virtual FString GetSerializerName() const override { return TEXT("JsonSerializer"); }

    // IDialogueJsonSerializer interface
    virtual void SaveToJson(
        const UDialogueSessionContext* Context,
        FString& OutJson
  ) const override;

 virtual bool LoadFromJson(
        UDialogueSessionContext* Context,
        const FString& Json
    ) const override;

    /**
     * Get singleton instance (optional - can also instantiate directly)
     */
    static const FDialogueJsonSerializer& Get()
    {
      static FDialogueJsonSerializer Instance;
        return Instance;
    }
};
