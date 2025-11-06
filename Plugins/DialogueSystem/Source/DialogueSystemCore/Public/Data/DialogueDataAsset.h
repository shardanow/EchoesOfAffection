// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/DialogueNode.h"
#include "DialogueDataAsset.generated.h"

// Forward declarations
class UDialogueSessionContext;

/**
 * NPC Persona data (character personality)
 * Used for AI generation and character behavior
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialoguePersonaData
{
    GENERATED_BODY()

    /** Persona ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
FName PersonaId;

    /** Display name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    FText DisplayName;

    /** Short character description */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona", meta = (MultiLine = true))
    FText Description;

    /** Personality trait tags */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    FGameplayTagContainer PersonalityTraits; // Trait.Shy, Trait.Confident, etc

    /** AI Prompt (system message) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (MultiLine = true))
    FString AISystemPrompt;

    /** Example phrases for Few-Shot Learning */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TArray<FString> ExamplePhrases;

    /** Relationship sensitivity */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float PositiveSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float NegativeSensitivity = 1.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float ForgivenessRate = 1.0f;
};

/**
 * Dialogue Data Asset
 * Contains all nodes, edges and metadata for a dialogue
 * 
 * Data-driven approach: designed for DataTable or Editor use,
 * runtime creates instances with state
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    //~ Begin Metadata

    /** Unique dialogue ID */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FName DialogueId;

    /** Display name (for navigation) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FText DisplayName;

    /** Dialogue description */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata", meta = (MultiLine = true))
    FText Description;

    /** Tags for filtering/searching */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FGameplayTagContainer DialogueTags;

  /** Minimum affinity required to start */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    float MinAffinityRequired = -100.0f;

    /** Data version (for migration) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    int32 Version = 1;

    //~ End Metadata

    //~ Begin Graph Data

    /** All dialogue nodes */
UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Graph")
    TArray<TObjectPtr<UDialogueNode>> Nodes;

  /** Starting node ID (default) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graph")
    FName StartNode;

    //~ End Graph Data

    //~ Begin Persona Data

    /** Primary speaker persona data */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persona")
    FDialoguePersonaData PrimaryPersona;

    /** Additional personas (for multi-NPC dialogues) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persona")
    TMap<FName, FDialoguePersonaData> AdditionalPersonas;

    //~ End Persona Data

    //~ Begin Localization

/** Namespace for localization */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Localization")
    FString LocalizationNamespace;

    /** Supported cultures */
 UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Localization")
    TArray<FString> SupportedCultures;

  //~ End Localization

    //~ Begin AI Integration

    /** Allow AI generation for new nodes */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    bool bAllowAIGeneration = false;

    /** Temperature for AI (creativity) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float AITemperature = 0.7f;

    /** Max tokens for response */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    int32 AIMaxTokens = 150;

    //~ End AI Integration

    //~ Begin Sequence-Based Auto-Positioning (v1.17.0)

    /** 
     * Automatically extract and apply actor positions from Level Sequence keyframes
     * 
     * When enabled, system will:
     * - Extract actor positions from sequence animation tracks
     * - Position actors BEFORE sequence plays (start frame)
     * - Position actors AFTER sequence finishes (end frame)
     * 
     * This eliminates manual position setup for sequences!
*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequence Positioning")
    bool bExtractPositionsFromSequence = false;

    /** Position actors to sequence START frame (frame 0) BEFORE sequence plays */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequence Positioning", 
        meta = (EditCondition = "bExtractPositionsFromSequence", EditConditionHides))
    bool bPositionActorsAtSequenceStart = true;

    /** Position actors to sequence END frame (last frame) AFTER sequence finishes */
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequence Positioning", 
        meta = (EditCondition = "bExtractPositionsFromSequence", EditConditionHides))
    bool bPositionActorsAtSequenceEnd = false;

    /** How to move actors to extracted positions (Teleport = instant, AIMoveTo = pathfinding) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sequence Positioning", 
        meta = (EditCondition = "bExtractPositionsFromSequence", EditConditionHides))
    EDialoguePositioningMode SequencePositioningMode = EDialoguePositioningMode::Teleport;

    //~ End Sequence-Based Auto-Positioning

public:
    //~ Begin API

 /** Find node by ID */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    UDialogueNode* FindNode(FName NodeId) const;

    /** Get outgoing edges from node */
UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    TArray<FDialogueEdgeData> GetOutgoingEdges(FName FromNodeId) const;

    /** Get starting node for given context */
 UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    FName GetStartNode(const UDialogueSessionContext* Context) const;

    /** Validate graph (for debugging) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    bool ValidateGraph(TArray<FString>& OutErrors) const;

    /** Export to JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    FString ExportToJson() const;

    /** Import from JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    bool ImportFromJson(const FString& Json);

    //~ End API

protected:
    //~ Begin UObject Interface
    virtual void PostLoad() override;
#if WITH_EDITOR
virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
 //~ End UObject Interface

  /** Rebuild search index */
    void RebuildIndex();

    /** Node lookup cache */
    UPROPERTY(Transient)
    TMap<FName, int32> NodeIndex;
};
