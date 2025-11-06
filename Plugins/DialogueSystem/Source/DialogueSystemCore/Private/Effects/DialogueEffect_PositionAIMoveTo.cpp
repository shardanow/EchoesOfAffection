// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_PositionAIMoveTo.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UDialogueEffect_PositionAIMoveTo::UDialogueEffect_PositionAIMoveTo()
{
	bSupportsReverse = false;
}

void UDialogueEffect_PositionAIMoveTo::ExecutePositioning(
	AActor* ParticipantActor,
	const FTransform& TargetTransform,
	UDialogueSessionContext* Context)
{
	if (!ParticipantActor)
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("ExecutePositioning (AIMoveTo): Invalid participant actor"));
		return;
	}

	// Get AI controller
	AAIController* AIController = GetAIController(ParticipantActor);
	if (!AIController)
	{
		UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecutePositioning (AIMoveTo): No AI controller found for '%s', using direct movement"), 
			*ParticipantActor->GetName());
		ExecuteDirectMovement(ParticipantActor, TargetTransform);
		return;
	}

	// Execute AI movement
	const FVector TargetLocation = TargetTransform.GetLocation();
	const float AcceptanceRadius = PositioningConfig.AcceptanceRadius;
	const bool bUsePathfinding = PositioningConfig.bUsePathfinding;

	if (!ExecuteAIMovement(AIController, TargetLocation, AcceptanceRadius, bUsePathfinding))
	{
		UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecutePositioning (AIMoveTo): AI movement failed, using direct movement"));
		ExecuteDirectMovement(ParticipantActor, TargetTransform);
		return;
	}

	// Store AI controller for tracking
	ActiveAIController = AIController;

	// Apply rotation if configured
	if (PositioningConfig.bApplyRotation)
	{
		ParticipantActor->SetActorRotation(TargetTransform.GetRotation());
	}

	UE_LOG(LogDialoguePositioning, Log, TEXT("ExecutePositioning (AIMoveTo): Started AI movement for '%s' to %s"), 
		*ParticipantActor->GetName(), *TargetLocation.ToString());
}

bool UDialogueEffect_PositionAIMoveTo::ExecuteAIMovement(
	AAIController* AIController,
	const FVector& TargetLocation,
	float AcceptanceRadius,
	bool bUsePathfinding)
{
	if (!AIController)
	{
		return false;
	}

	// NEW v1.16.4: Force enable CharacterMovementComponent (might be disabled by Schedule)
	APawn* ControlledPawn = AIController->GetPawn();
	if (ControlledPawn)
	{
		// v1.16.7: Log controller info for debugging
		UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecuteAIMovement: Controller='%s' controlling Pawn='%s'"),
			*AIController->GetName(), 
			*ControlledPawn->GetName());

		if (ACharacter* Character = Cast<ACharacter>(ControlledPawn))
		{
			UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
			if (MovementComp)
			{
				// Re-enable movement if it was disabled
				if (!MovementComp->IsActive())
				{
					MovementComp->Activate(true);
					UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteAIMovement: Re-activated CharacterMovementComponent for '%s'"), 
						*Character->GetName());
				}

				// Reset velocity to ensure clean state
				MovementComp->StopMovementImmediately();
				MovementComp->Velocity = FVector::ZeroVector;

				// NEW v1.16.5: Force update movement mode to Walking (might be stuck in None)
				if (MovementComp->MovementMode == MOVE_None)
				{
					MovementComp->SetMovementMode(MOVE_Walking);
					UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecuteAIMovement: Fixed MovementMode from None -> Walking for '%s'"), 
						*Character->GetName());
				}
			}
		}
	}
	else
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("ExecuteAIMovement: AIController '%s' has no pawn!"), 
			*AIController->GetName());
		return false;
	}

	// NEW v1.16.6: Clear PathFollowingComponent state (might be stuck after Schedule pause)
	UPathFollowingComponent* PathFollowing = AIController->GetPathFollowingComponent();
	if (PathFollowing)
	{
		// Abort any existing path following
		PathFollowing->AbortMove(*this, FPathFollowingResultFlags::ForcedScript | FPathFollowingResultFlags::NewRequest);
		UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecuteAIMovement: Cleared PathFollowing state for '%s'"), 
			*AIController->GetName());
	}

	// Prepare move request
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetLocation);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	MoveRequest.SetUsePathfinding(bUsePathfinding);
	MoveRequest.SetAllowPartialPath(true);
	MoveRequest.SetProjectGoalLocation(true);
	MoveRequest.SetNavigationFilter(AIController->GetDefaultNavigationFilterClass());

	// Execute movement
	FPathFollowingRequestResult RequestResult = AIController->MoveTo(MoveRequest);

	if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful ||
		RequestResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		// Subscribe to movement completed delegate if we need to wait
		if (PositioningConfig.bWaitForCompletion)
		{
			AIController->ReceiveMoveCompleted.AddUniqueDynamic(this, &UDialogueEffect_PositionAIMoveTo::OnMoveCompleted);
		}

		UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteAIMovement: Movement request successful (Code: %d) for '%s'"), 
			static_cast<int32>(RequestResult.Code),
			ControlledPawn ? *ControlledPawn->GetName() : TEXT("NULL"));
		return true;
	}
	else
	{
		UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecuteAIMovement: Movement request failed (Code: %d) for '%s'"), 
			static_cast<int32>(RequestResult.Code),
			ControlledPawn ? *ControlledPawn->GetName() : TEXT("NULL"));
		return false;
	}
}

void UDialogueEffect_PositionAIMoveTo::ExecuteDirectMovement(
	AActor* ParticipantActor,
	const FTransform& TargetTransform)
{
	if (!ParticipantActor)
	{
		return;
	}

	// Direct teleport as fallback
	const FVector TargetLocation = TargetTransform.GetLocation();
	const FRotator TargetRotation = PositioningConfig.bApplyRotation 
		? TargetTransform.GetRotation().Rotator() 
		: ParticipantActor->GetActorRotation();

	ParticipantActor->SetActorLocationAndRotation(TargetLocation, TargetRotation, false, nullptr, ETeleportType::TeleportPhysics);

	UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteDirectMovement: Teleported '%s' to %s"), 
		*ParticipantActor->GetName(), *TargetLocation.ToString());
}

void UDialogueEffect_PositionAIMoveTo::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	// Cleanup delegate subscription
	if (ActiveAIController.IsValid())
	{
		ActiveAIController->ReceiveMoveCompleted.RemoveDynamic(this, &UDialogueEffect_PositionAIMoveTo::OnMoveCompleted);
	}

	const FString ResultString = StaticEnum<EPathFollowingResult::Type>()->GetNameStringByValue(static_cast<int64>(Result));
	UE_LOG(LogDialoguePositioning, Log, TEXT("OnMoveCompleted: Movement finished with result: %s"), *ResultString);

	// TODO v1.15.1: Emit event через GameEventBus для продолжения диалога
	// Сейчас DialogueRunner должен сам отслеживать completion если bWaitForCompletion = true
}
