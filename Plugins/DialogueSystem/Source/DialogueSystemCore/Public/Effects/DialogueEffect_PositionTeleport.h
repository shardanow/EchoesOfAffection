// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffect_PositionParticipant.h"
#include "DialogueEffect_PositionTeleport.generated.h"

/**
 * Teleport Positioning Effect
 * 
 * Instantly teleports participant to target location.
 * No pathfinding, no animation - instant position change.
 * 
 * v1.15: Dialogue Positioning System
 * 
 * Features:
 * - Instant teleportation
 * - Optional rotation application
 * - Physics-safe teleport
 * - Optional visual effect spawn point
 * 
 * Use Cases:
 * - Pre-cutscene positioning
 * - Emergency repositioning
 * - Instant scene setup
 */
UCLASS(BlueprintType, meta = (DisplayName = "Position: Teleport"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_PositionTeleport : public UDialogueEffect_PositionParticipant
{
	GENERATED_BODY()

public:
	UDialogueEffect_PositionTeleport();

	/** Spawn visual effect at teleport (optional) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSoftObjectPtr<class UNiagaraSystem> TeleportVFX;

	/** Play sound at teleport (optional) */
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
	/** Execute instant teleport */
	void ExecuteTeleport(
		AActor* ParticipantActor,
		const FTransform& TargetTransform
	);

	/** Spawn visual effects */
	void SpawnTeleportEffects(
		const FVector& Location,
		UWorld* World
	);
};
