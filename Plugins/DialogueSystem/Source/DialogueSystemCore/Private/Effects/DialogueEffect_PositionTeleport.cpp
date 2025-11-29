// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_PositionTeleport.h"
#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h" // FIX v1.17.2: For capsule half-height compensation

UDialogueEffect_PositionTeleport::UDialogueEffect_PositionTeleport()
{
	bSupportsReverse = false;
}

void UDialogueEffect_PositionTeleport::ExecutePositioning(
	AActor* ParticipantActor,
	const FTransform& TargetTransform,
	UDialogueSessionContext* Context)
{
	if (!ParticipantActor)
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("ExecutePositioning (Teleport): Invalid participant actor"));
		return;
	}

	UE_LOG(LogDialoguePositioning, Log, TEXT("ExecutePositioning (Teleport): Teleporting '%s' from %s to %s"), 
		*ParticipantActor->GetName(),
		*ParticipantActor->GetActorLocation().ToString(),
		*TargetTransform.GetLocation().ToString());

	// Execute teleport
	ExecuteTeleport(ParticipantActor, TargetTransform);

	// Spawn visual/audio effects
	if (ParticipantActor->GetWorld())
	{
		SpawnTeleportEffects(TargetTransform.GetLocation(), ParticipantActor->GetWorld());
	}
}

void UDialogueEffect_PositionTeleport::ExecuteTeleport(
	AActor* ParticipantActor,
	const FTransform& TargetTransform)
{
	if (!ParticipantActor)
	{
		return;
	}

	// Prepare teleport parameters
	FVector TargetLocation = TargetTransform.GetLocation();
	const FRotator TargetRotation = PositioningConfig.bApplyRotation 
		? TargetTransform.GetRotation().Rotator() 
		: ParticipantActor->GetActorRotation();

	// ===== FIX v1.17.2: Compensate for capsule half-height =====
	// Characters are positioned at capsule CENTER, not at feet!
	// We need to offset Z by capsule half-height to place feet on ground.
	// ===== FIX v1.17.4: BUT skip compensation for sequence end positions! =====
	ACharacter* Character = Cast<ACharacter>(ParticipantActor);
	if (Character && Character->GetCapsuleComponent() && !PositioningConfig.bSkipCapsuleCompensation)
	{
		float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		TargetLocation.Z += CapsuleHalfHeight;
		
		UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Compensated Z by capsule half-height: %.2f (Final Z: %.2f)"), 
			CapsuleHalfHeight, TargetLocation.Z);
	}
	else if (PositioningConfig.bSkipCapsuleCompensation)
	{
		UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Skipping capsule compensation (sequence end position)"));
	}

	// Stop any active movement before teleporting
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->StopMovementImmediately();
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	// Stop AI movement if active
	AAIController* AIController = GetAIController(ParticipantActor);
	if (AIController)
	{
		AIController->StopMovement();
	}

	// Execute teleport with physics reset (safer than TeleportPhysics)
	const bool bSweep = false; // No sweep - instant teleport
	FHitResult HitResult;
	ParticipantActor->SetActorLocationAndRotation(
		TargetLocation,
		TargetRotation,
		bSweep,
		&HitResult,
		ETeleportType::ResetPhysics // FIXED: Use ResetPhysics instead of TeleportPhysics
	);

	UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Successfully teleported '%s' to %s"), 
		*ParticipantActor->GetName(),
		*TargetLocation.ToString());
}

void UDialogueEffect_PositionTeleport::SpawnTeleportEffects(
	const FVector& Location,
	UWorld* World)
{
	if (!World)
	{
		return;
	}

	// Spawn Niagara VFX if configured
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

			UE_LOG(LogDialoguePositioning, Log, TEXT("SpawnTeleportEffects: Spawned VFX at %s"), *Location.ToString());
		}
	}

	// Play sound if configured
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

			UE_LOG(LogDialoguePositioning, Log, TEXT("SpawnTeleportEffects: Played sound at %s"), *Location.ToString());
		}
	}
}
