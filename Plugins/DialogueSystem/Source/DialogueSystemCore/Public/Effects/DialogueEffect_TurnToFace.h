// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffectExecutor.h"
#include "DialogueEffect_TurnToFace.generated.h"

/**
 * Turn-To-Face Rotation Mode
 * 
 * v1.18: Different rotation behaviors
 */
UENUM(BlueprintType)
enum class ETurnToFaceMode : uint8
{
	/** Smooth interpolated rotation over RotationDuration */
	Smooth UMETA(DisplayName = "Smooth"),
	
	/** Instant snap to target rotation */
	Instant UMETA(DisplayName = "Instant"),
	
	/** Look at target continuously (head/eyes tracking) - emits event but rotation handled by subscriber */
	LookAt UMETA(DisplayName = "Look At"),
	
	/** Reset to forward direction (0, 0, 0) */
	Forward UMETA(DisplayName = "Forward")
};

/**
 * Turn-To-Face Effect - v1.18
 * 
 * Rotates participant to face target participant.
 * Emits GameEventBus event for loose coupling - actual rotation handled by subscribers.
 * 
 * Features:
 * - Multiple rotation modes (Smooth, Instant, LookAt, Forward)
 * - Configurable rotation speed
 * - Optional head-only rotation
 * - GameEventBus integration (no hard dependencies)
 * - Works with positioning system
 * 
 * DLS Syntax:
 *   TurnToFace(Alice)         // Smooth rotation to Alice
 *   TurnToFace(Player, Instant)        // Instant snap to Player
 *   TurnToFace(Lianne, Smooth=1.5)     // 1.5 second rotation
 *   TurnToFace(Forward)// Reset to forward direction
 *   TurnToFace(Alice, HeadOnly)// Only rotate head (if supported)
 * 
 * AAA Best Practices:
 * - Soft coupling через GameEventBus
 * - Event-driven architecture
 * - No dependencies on animation/IK systems
 * - Flexible subscription model
 * - Debug logging для troubleshooting
 */
UCLASS(BlueprintType, meta = (DisplayName = "Turn To Face"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_TurnToFace : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_TurnToFace();

	/** 
	 * Who should turn (PersonaId or empty for current speaker)
	 * 
	 * Examples:
	 *   - Empty = Current speaker (SpeakerId from node)
	 *   - "Lianne" = Specific NPC
	 *   - "Player" = Player character
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName RotatingParticipantId;
	
	/** 
	 * Who to turn towards (PersonaId, "Player", or "Forward" for reset)
	 * 
	 * Examples:
	 * - "Player" = Turn to player
	 *   - "Alice" = Turn to Alice NPC
	 *   - "Forward" = Reset to forward direction
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName TargetParticipantId;
	
	/** Rotation mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	ETurnToFaceMode RotationMode = ETurnToFaceMode::Smooth;
	
	/** Rotation duration (seconds) - ignored for Instant mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = "0.1", ClampMax = "5.0", UIMin = "0.1", UIMax = "5.0"))
	float RotationDuration = 0.5f;
	
	/** Only rotate head/upper body (if supported by character rig) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bHeadOnly = false;
	
	/** Wait for rotation to complete before continuing dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bWaitForCompletion = false;

	//~ Begin UDialogueEffect Interface
	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
	//~ End UDialogueEffect Interface

protected:
	/** 
	 * Find participant actor by ID
	 * 
	 * @param Context - Dialogue session context
	 * @param ParticipantId - PersonaId to find (or "Player")
	 * @return Found actor or nullptr
	 */
	AActor* FindParticipantActor(UDialogueSessionContext* Context, FName ParticipantId) const;

	/** 
	 * Emit GameEventBus event for turn-to-face
	 * 
	 * @param RotatingActor - Actor that should rotate
	 * @param TargetActor - Target to face (nullptr for Forward mode)
	 * @param Context - Dialogue session context
	 */
	void EmitTurnToFaceEvent(AActor* RotatingActor, AActor* TargetActor, UDialogueSessionContext* Context);

	/** 
	 * Calculate target rotation for actor to face target
	 * 
	 * @param Actor - Actor that rotates
	 * @param Target - Target to face
	 * @return Target rotation (yaw only, preserving pitch/roll)
	 */
	FRotator CalculateTargetRotation(AActor* Actor, AActor* Target) const;

	/** Log turn-to-face debug info */
	void LogTurnToFaceInfo(AActor* RotatingActor, AActor* TargetActor) const;
};
