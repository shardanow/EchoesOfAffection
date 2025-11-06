// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_PositionConditionalTeleport.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UDialogueEffect_PositionConditionalTeleport::UDialogueEffect_PositionConditionalTeleport()
{
	bSupportsReverse = false;
}

void UDialogueEffect_PositionConditionalTeleport::ExecutePositioning(
	AActor* ParticipantActor,
	const FTransform& TargetTransform,
	UDialogueSessionContext* Context)
{
	if (!ParticipantActor)
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("ExecutePositioning (ConditionalTeleport): Invalid participant actor"));
		return;
	}

	const FVector TargetLocation = TargetTransform.GetLocation();
	const float Distance = FVector::Dist(ParticipantActor->GetActorLocation(), TargetLocation);

	UE_LOG(LogDialoguePositioning, Log, TEXT("ExecutePositioning (ConditionalTeleport): Distance=%.2f, Threshold=%.2f"), 
		Distance, PositioningConfig.TeleportDistanceThreshold);

	// Decide between teleport and AI movement
	if (ShouldTeleport(ParticipantActor, TargetLocation))
	{
		UE_LOG(LogDialoguePositioning, Log, TEXT("ExecutePositioning (ConditionalTeleport): Distance > threshold, using TELEPORT"));
		ExecuteTeleport(ParticipantActor, TargetTransform);
	}
	else
	{
		UE_LOG(LogDialoguePositioning, Log, TEXT("ExecutePositioning (ConditionalTeleport): Distance <= threshold, using AI MOVEMENT"));
		ExecuteAIMovement(ParticipantActor, TargetTransform);
	}
}

bool UDialogueEffect_PositionConditionalTeleport::ShouldTeleport(
	AActor* ParticipantActor,
	const FVector& TargetLocation) const
{
	if (!ParticipantActor)
	{
		return true; // Default to teleport if actor invalid
	}

	const FVector CurrentLocation = ParticipantActor->GetActorLocation();
	const float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	return Distance > PositioningConfig.TeleportDistanceThreshold;
}

void UDialogueEffect_PositionConditionalTeleport::ExecuteAIMovement(
	AActor* ParticipantActor,
	const FTransform& TargetTransform)
{
	if (!ParticipantActor)
	{
		return;
	}

	// Get AI controller
	AAIController* AIController = GetAIController(ParticipantActor);
	if (!AIController)
	{
		UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecuteAIMovement: No AI controller, falling back to teleport"));
		ExecuteTeleport(ParticipantActor, TargetTransform);
		return;
	}

	// Prepare move request
	const FVector TargetLocation = TargetTransform.GetLocation();
	const float AcceptanceRadius = PositioningConfig.AcceptanceRadius;
	const bool bUsePathfinding = PositioningConfig.bUsePathfinding;

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
		// Apply rotation if configured
		if (PositioningConfig.bApplyRotation)
		{
			ParticipantActor->SetActorRotation(TargetTransform.GetRotation());
		}

		UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteAIMovement: Movement request successful"));
	}
	else
	{
		UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecuteAIMovement: Movement request failed, falling back to teleport"));
		ExecuteTeleport(ParticipantActor, TargetTransform);
	}
}

void UDialogueEffect_PositionConditionalTeleport::ExecuteTeleport(
	AActor* ParticipantActor,
	const FTransform& TargetTransform)
{
	if (!ParticipantActor)
	{
		return;
	}

	// Prepare teleport parameters
	const FVector TargetLocation = TargetTransform.GetLocation();
	const FRotator TargetRotation = PositioningConfig.bApplyRotation 
		? TargetTransform.GetRotation().Rotator() 
		: ParticipantActor->GetActorRotation();

	// Stop any active movement
	ACharacter* Character = Cast<ACharacter>(ParticipantActor);
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->StopMovementImmediately();
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	// Stop AI movement
	AAIController* AIController = GetAIController(ParticipantActor);
	if (AIController)
	{
		AIController->StopMovement();
	}

	// Execute teleport
	const bool bSweep = false;
	FHitResult HitResult;
	ParticipantActor->SetActorLocationAndRotation(
		TargetLocation,
		TargetRotation,
		bSweep,
		&HitResult,
		ETeleportType::TeleportPhysics
	);

	// Spawn effects
	if (ParticipantActor->GetWorld())
	{
		SpawnTeleportEffects(TargetLocation, ParticipantActor->GetWorld());
	}

	UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Successfully teleported '%s'"), 
		*ParticipantActor->GetName());
}

void UDialogueEffect_PositionConditionalTeleport::SpawnTeleportEffects(
	const FVector& Location,
	UWorld* World)
{
	if (!World)
	{
		return;
	}

	// Spawn VFX
	if (TeleportVFX.IsValid())
	{
		UNiagaraSystem* VFX = TeleportVFX.LoadSynchronous();
		if (VFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				World,
				VFX,
				Location,
				FRotator::ZeroRotator,
				FVector::OneVector,
				true,
				true,
				ENCPoolMethod::AutoRelease
			);
		}
	}

	// Play sound
	if (TeleportSound.IsValid())
	{
		USoundBase* Sound = TeleportSound.LoadSynchronous();
		if (Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				World,
				Sound,
				Location,
				FRotator::ZeroRotator // Add rotation parameter
			);
		}
	}
}
