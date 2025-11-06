// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffect_PositionParticipant.h"
#include "DialogueEffect_PositionConditionalTeleport.generated.h"

/**
 * Conditional Teleport Positioning Effect
 * 
 * Smart positioning that chooses between AI movement and teleport based on distance.
 * If distance > threshold: Teleport
 * If distance <= threshold: AI Move To
 * 
 * v1.15: Dialogue Positioning System
 * 
 * Features:
 * - Distance-based mode selection
 * - Combines AI movement and teleport
 * - Configurable threshold
 * - Best of both worlds
 * 
 * Use Cases:
 * - Natural movement for nearby positions
 * - Instant teleport for far positions
 * - Adaptive positioning
 * - Player-friendly repositioning
 * 
 * AAA Best Practice:
 * - Prevent long pathfinding delays
 * - Keep NPCs responsive
 * - Smooth experience для игрока
 */
UCLASS(BlueprintType, meta = (DisplayName = "Position: Smart Teleport"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_PositionConditionalTeleport : public UDialogueEffect_PositionParticipant
{
	GENERATED_BODY()

public:
	UDialogueEffect_PositionConditionalTeleport();

	/** Visual effect for teleport (only when teleporting) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSoftObjectPtr<class UNiagaraSystem> TeleportVFX;

	/** Sound for teleport (only when teleporting) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSoftObjectPtr<class USoundBase> TeleportSound;

protected:
	//~ Begin UDialogueEffect_PositionParticipant Interface
	virtual void ExecutePositioning(
		AActor* ParticipantActor,
		const FTransform& TargetTransform,
		UDialogueSessionContext* Context
	) override;
	//~ End UDialogueEffect_PositionParticipant Interface

private:
	/** Determine if should teleport based on distance */
	bool ShouldTeleport(
		AActor* ParticipantActor,
		const FVector& TargetLocation
	) const;

	/** Execute AI movement (similar to AIMoveTo effect) */
	void ExecuteAIMovement(
		AActor* ParticipantActor,
		const FTransform& TargetTransform
	);

	/** Execute teleport (similar to Teleport effect) */
	void ExecuteTeleport(
		AActor* ParticipantActor,
		const FTransform& TargetTransform
	);

	/** Spawn teleport VFX/SFX */
	void SpawnTeleportEffects(
		const FVector& Location,
		UWorld* World
	);
};
