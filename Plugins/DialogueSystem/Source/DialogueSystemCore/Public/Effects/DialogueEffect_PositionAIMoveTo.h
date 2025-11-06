// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffect_PositionParticipant.h"
#include "Navigation/PathFollowingComponent.h"
#include "DialogueEffect_PositionAIMoveTo.generated.h"

/**
 * AI MoveTo Positioning Effect
 * 
 * Uses AI navigation system to move participant to target location.
 * Supports pathfinding, acceptance radius, and async completion.
 * 
 * v1.15: Dialogue Positioning System
 * 
 * Features:
 * - Pathfinding через AI navigation
 * - Async movement с callback
 * - Configurable acceptance radius
 * - Direct movement fallback если AI недоступен
 * 
 * AAA Best Practices:
 * - Используем AAIController::MoveToLocation()
 * - Поддержка async операций
 * - Error handling и fallbacks
 */
UCLASS(BlueprintType, meta = (DisplayName = "Position: AI Move To"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_PositionAIMoveTo : public UDialogueEffect_PositionParticipant
{
	GENERATED_BODY()

public:
	UDialogueEffect_PositionAIMoveTo();

protected:
	//~ Begin UDialogueEffect_PositionParticipant Interface
	virtual void ExecutePositioning(
		AActor* ParticipantActor,
		const FTransform& TargetTransform,
		UDialogueSessionContext* Context
	) override;
	//~ End UDialogueEffect_PositionParticipant Interface

private:
	/** Active AI controller (for tracking movement) */
	UPROPERTY(Transient)
	TWeakObjectPtr<AAIController> ActiveAIController;

	/** Движение завершено callback */
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	/** Выполнить AI движение */
	bool ExecuteAIMovement(
		AAIController* AIController,
		const FVector& TargetLocation,
		float AcceptanceRadius,
		bool bUsePathfinding
	);

	/** Fallback: прямое движение если AI недоступен */
	void ExecuteDirectMovement(
		AActor* ParticipantActor,
		const FTransform& TargetTransform
	);
};
