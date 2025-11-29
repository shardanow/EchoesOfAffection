// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DialoguePositioningTypes.generated.h"

/**
 * Positioning Mode - How to move participant to target location
 * 
 * v1.15: Dialogue participant positioning system
 */
UENUM(BlueprintType)
enum class EDialoguePositioningMode : uint8
{
	/** Use AI navigation to move to location (pathfinding) */
	AIMoveTo UMETA(DisplayName = "AI Move To"),

	/** Instantly teleport to location (no animation) */
	Teleport UMETA(DisplayName = "Teleport"),

	/** Teleport if distance > threshold, otherwise AI move */
	ConditionalTeleport UMETA(DisplayName = "Conditional Teleport")
};

/**
 * Location Source Type - How to determine target location
 * 
 * v1.15: Flexible location specification
 */
UENUM(BlueprintType)
enum class EDialogueLocationSource : uint8
{
	/** Manual world coordinates */
	WorldCoordinates UMETA(DisplayName = "World Coordinates"),

	/** Find LocationMarker actor by GameplayTag */
	MarkerTag UMETA(DisplayName = "Marker Tag"),

	/** Use transform from another actor */
	ActorTransform UMETA(DisplayName = "Actor Transform")
};

/**
 * Single Participant Positioning Data
 * 
 * Определяет как и куда позиционировать одного участника диалога.
 * 
 * v1.15: Dialogue positioning system
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueParticipantPositioning
{
	GENERATED_BODY()

	/** Participant ID (Persona name from DialogueDataAsset) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning")
	FName ParticipantId;

	/** How to move participant to location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning")
	EDialoguePositioningMode PositioningMode = EDialoguePositioningMode::AIMoveTo;

	/** How to determine target location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning")
	EDialogueLocationSource LocationSource = EDialogueLocationSource::MarkerTag;

	// ===== Location Source Parameters =====

	/** Manual world location (used if LocationSource = WorldCoordinates) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Location", meta = (EditCondition = "LocationSource == EDialogueLocationSource::WorldCoordinates", EditConditionHides))
	FVector TargetLocation = FVector::ZeroVector;

	/** Manual world rotation (used if LocationSource = WorldCoordinates) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Location", meta = (EditCondition = "LocationSource == EDialogueLocationSource::WorldCoordinates", EditConditionHides))
	FRotator TargetRotation = FRotator::ZeroRotator;

	/** GameplayTag to find LocationMarker (e.g., "Location.Dialogue.Lianne.Front") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Location", meta = (EditCondition = "LocationSource == EDialogueLocationSource::MarkerTag", EditConditionHides, Categories = "Location"))
	FGameplayTag MarkerTag;

	/** Actor to use transform from (used if LocationSource = ActorTransform) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Location", meta = (EditCondition = "LocationSource == EDialogueLocationSource::ActorTransform", EditConditionHides))
	TSoftObjectPtr<AActor> TargetActor;

	// ===== Positioning Mode Parameters =====

	/** Distance threshold for ConditionalTeleport mode (teleport if distance > this value) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Behavior", meta = (EditCondition = "PositioningMode == EDialoguePositioningMode::ConditionalTeleport", EditConditionHides, ClampMin = "100.0", ClampMax = "10000.0", UIMin = "100.0", UIMax = "10000.0"))
	float TeleportDistanceThreshold = 1000.0f;

	/** Wait for movement to complete before continuing dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Behavior")
	bool bWaitForCompletion = true;

	/** Acceptance radius for AI movement (how close to get) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Behavior", meta = (EditCondition = "PositioningMode == EDialoguePositioningMode::AIMoveTo || PositioningMode == EDialoguePositioningMode::ConditionalTeleport", EditConditionHides, ClampMin = "1.0", ClampMax = "500.0", UIMin = "1.0", UIMax = "500.0"))
	float AcceptanceRadius = 50.0f;

	/** Use pathfinding (true) or direct path (false) for AI movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Behavior", meta = (EditCondition = "PositioningMode == EDialoguePositioningMode::AIMoveTo || PositioningMode == EDialoguePositioningMode::ConditionalTeleport", EditConditionHides))
	bool bUsePathfinding = true;

	/** Apply rotation when positioning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Behavior")
	bool bApplyRotation = true;
	
	/** 
	 * NEW v1.17.4: Skip capsule height compensation
	 * Set to TRUE for positions coming from sequence (already at correct height)
	 * Set to FALSE for marker positions (need capsule compensation)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Advanced")
	bool bSkipCapsuleCompensation = false;

	FDialogueParticipantPositioning()
		: ParticipantId(NAME_None)
		, PositioningMode(EDialoguePositioningMode::AIMoveTo)
		, bApplyRotation(true)
		, bSkipCapsuleCompensation(false)
	{
	}
};

/**
 * Node Positioning Configuration
 * 
 * Manages positioning for all participants in a dialogue node.
 * Supports both manual positioning AND sequence-based auto-positioning.
 * 
 * v1.15: Multi-participant positioning
 * v1.17: Sequence-based auto-extraction
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueNodePositioning
{
	GENERATED_BODY()

	/** Enable positioning for this node (either manual OR sequence-based) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning")
	bool bEnablePositioning = false;

	/** 
	 * Override global sequence positioning settings for THIS NODE ONLY
	 * 
	 * If enabled, uses per-node settings instead of DialogueDataAsset settings
	 * Useful when one node needs different positioning behavior
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence Positioning", 
		meta = (EditCondition = "bEnablePositioning", EditConditionHides))
	bool bOverrideSequencePositioning = false;

	/** Position actors to sequence START frame before sequence plays (per-node override) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence Positioning", 
		meta = (EditCondition = "bEnablePositioning && bOverrideSequencePositioning", EditConditionHides))
	bool bPositionAtSequenceStart = true;

	/** Position actors to sequence END frame after sequence finishes (per-node override) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence Positioning", 
		meta = (EditCondition = "bEnablePositioning && bOverrideSequencePositioning", EditConditionHides))
	bool bPositionAtSequenceEnd = false;

	/** 
	 * MANUAL POSITIONING: Position Primary Persona (main NPC)
	 * 
	 * Only used when sequence positioning is DISABLED
	 * Allows manual coordinate/marker-based positioning
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manual Positioning", 
		meta = (EditCondition = "bEnablePositioning && !bOverrideSequencePositioning", EditConditionHides))
	FDialogueParticipantPositioning PrimaryPersonaPositioning;

	/** 
	 * MANUAL POSITIONING: Additional Personas (other NPCs)
	 * 
	 * Only used when sequence positioning is DISABLED
	 * Key = PersonaId from DialogueDataAsset
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manual Positioning", 
		meta = (EditCondition = "bEnablePositioning && !bOverrideSequencePositioning", EditConditionHides))
	TMap<FName, FDialogueParticipantPositioning> AdditionalPersonasPositioning;

	/** Position actors BEFORE or AFTER applying node effects */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning", 
		meta = (EditCondition = "bEnablePositioning", EditConditionHides))
	bool bPositionBeforeEffects = true;

	FDialogueNodePositioning()
		: bEnablePositioning(false)
		, bOverrideSequencePositioning(false)
		, bPositionAtSequenceStart(true)
		, bPositionAtSequenceEnd(false)
		, bPositionBeforeEffects(true)
	{
	}
};
