// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_TurnToFace.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueParticipants.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

// GameEventBus integration (only if enabled)
#if WITH_GAMEEVENTBUS
#include "GameEventBusSubsystem.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogDialogueTurnToFace, Log, All);

UDialogueEffect_TurnToFace::UDialogueEffect_TurnToFace()
{
	bSupportsReverse = false; // Can't undo rotation
}

void UDialogueEffect_TurnToFace::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		UE_LOG(LogDialogueTurnToFace, Error, TEXT("[TURN-TO-FACE] Invalid context"));
		return;
	}

	UE_LOG(LogDialogueTurnToFace, Log, TEXT("[TURN-TO-FACE] Execute called - RotatingParticipant='%s', Target='%s', Mode=%d"),
		*RotatingParticipantId.ToString(),
		*TargetParticipantId.ToString(),
		static_cast<int32>(RotationMode));

	// Resolve rotating actor
	FName RotatingId = RotatingParticipantId;
	if (RotatingId.IsNone())
	{
		UE_LOG(LogDialogueTurnToFace, Error, TEXT("[TURN-TO-FACE] RotatingParticipantId must be specified"));
		UE_LOG(LogDialogueTurnToFace, Error, TEXT("  Example: TurnToFace(Lianne, Player) or TurnToFace(Speaker, Alice) if using parser"));
		return;
	}

	AActor* RotatingActor = FindParticipantActor(Context, RotatingId);
	if (!RotatingActor)
	{
		UE_LOG(LogDialogueTurnToFace, Error, TEXT("[TURN-TO-FACE] Failed to find rotating actor for '%s'"),
			*RotatingId.ToString());
		return;
	}

	// Resolve target actor (nullptr for Forward mode)
	AActor* TargetActor = nullptr;
	if (RotationMode != ETurnToFaceMode::Forward)
	{
		if (TargetParticipantId.IsNone())
		{
			UE_LOG(LogDialogueTurnToFace, Error, TEXT("[TURN-TO-FACE] No target specified for non-Forward mode"));
			return;
		}

		TargetActor = FindParticipantActor(Context, TargetParticipantId);
		if (!TargetActor)
		{
			UE_LOG(LogDialogueTurnToFace, Error, TEXT("[TURN-TO-FACE] Failed to find target actor for '%s'"),
				*TargetParticipantId.ToString());
			return;
		}

		// Validate not trying to turn to self
		if (RotatingActor == TargetActor)
		{
			UE_LOG(LogDialogueTurnToFace, Warning, TEXT("[TURN-TO-FACE] Rotating actor and target are the same (%s), skipping"),
				*RotatingActor->GetName());
			return;
		}
	}

	// Log debug info
	LogTurnToFaceInfo(RotatingActor, TargetActor);

	// Emit GameEventBus event
	EmitTurnToFaceEvent(RotatingActor, TargetActor, Context);
}

FText UDialogueEffect_TurnToFace::GetDisplayText_Implementation() const
{
	FString ModeStr;
	switch (RotationMode)
	{
		case ETurnToFaceMode::Smooth:   ModeStr = TEXT("Smooth"); break;
		case ETurnToFaceMode::Instant:  ModeStr = TEXT("Instant"); break;
		case ETurnToFaceMode::LookAt:   ModeStr = TEXT("LookAt"); break;
		case ETurnToFaceMode::Forward:  ModeStr = TEXT("Forward"); break;
		default: ModeStr = TEXT("Unknown");
	}

	FString RotatingName = RotatingParticipantId.IsNone() ? TEXT("Speaker") : RotatingParticipantId.ToString();
	FString TargetName = TargetParticipantId.IsNone() ? TEXT("None") : TargetParticipantId.ToString();

	if (RotationMode == ETurnToFaceMode::Forward)
	{
		return FText::FromString(FString::Printf(TEXT("Turn %s forward"), *RotatingName));
	}

	FString HeadOnlyStr = bHeadOnly ? TEXT(" (head only)") : TEXT("");
	return FText::FromString(FString::Printf(TEXT("Turn %s to %s (%s, %.1fs)%s"),
		*RotatingName,
		*TargetName,
		*ModeStr,
		RotationDuration,
		*HeadOnlyStr));
}

AActor* UDialogueEffect_TurnToFace::FindParticipantActor(UDialogueSessionContext* Context, FName ParticipantId) const
{
	if (!Context || ParticipantId.IsNone())
	{
		return nullptr;
	}

	UDialogueParticipants* Participants = Context->GetParticipants();
	if (!Participants)
	{
		UE_LOG(LogDialogueTurnToFace, Error, TEXT("[TURN-TO-FACE] No Participants component in context"));
		return nullptr;
	}

	// Try to find actor by PersonaId
	AActor* Actor = Participants->GetActorByPersonaId(ParticipantId);
	if (Actor)
	{
		UE_LOG(LogDialogueTurnToFace, Verbose, TEXT("[TURN-TO-FACE] Found actor '%s' for Persona '%s'"),
			*Actor->GetName(), *ParticipantId.ToString());
		return Actor;
	}

	// Not found
	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("[TURN-TO-FACE] Participant '%s' not found in dialogue"),
		*ParticipantId.ToString());

	return nullptr;
}

void UDialogueEffect_TurnToFace::EmitTurnToFaceEvent(AActor* RotatingActor, AActor* TargetActor, UDialogueSessionContext* Context)
{
#if WITH_GAMEEVENTBUS
	UWorld* World = Context->GetWorld();
	if (!World)
	{
		UE_LOG(LogDialogueTurnToFace, Warning, TEXT("[TURN-TO-FACE] Cannot emit event: World is null"));
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		UE_LOG(LogDialogueTurnToFace, Warning, TEXT("[TURN-TO-FACE] Cannot emit event: EventBus is null"));
		return;
	}

	// Prepare payload
	FGameEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.TurnToFace"));
	Payload.InstigatorActor = RotatingActor;  // Who turns
	Payload.TargetActor = TargetActor;      // Turn target (nullptr for Forward)
	
	// Pack mode as string for easy blueprint parsing
	FString ModeStr;
	switch (RotationMode)
	{
		case ETurnToFaceMode::Smooth:   ModeStr = TEXT("Smooth"); break;
		case ETurnToFaceMode::Instant:  ModeStr = TEXT("Instant"); break;
		case ETurnToFaceMode::LookAt:   ModeStr = TEXT("LookAt"); break;
		case ETurnToFaceMode::Forward:  ModeStr = TEXT("Forward"); break;
	}
	Payload.StringParam = FName(*ModeStr);
	Payload.FloatParam = RotationDuration;
	Payload.IntParam = bHeadOnly ? 1 : 0;

	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("[GAMEEVENTBUS] Emitting Dialogue.TurnToFace:"));
	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("  RotatingActor: %s"), 
		RotatingActor ? *RotatingActor->GetName() : TEXT("None"));
	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("  TargetActor: %s"), 
		TargetActor ? *TargetActor->GetName() : TEXT("None (Forward)"));
	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("  Mode: %s"), *ModeStr);
	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("  Duration: %.2f"), RotationDuration);
	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("  HeadOnly: %s"), bHeadOnly ? TEXT("YES") : TEXT("NO"));

	// Emit event
	EventBus->EmitEvent(Payload, true); // bLogEvent = true for debugging

	UE_LOG(LogDialogueTurnToFace, Log, TEXT("[GAMEEVENTBUS] TurnToFace event emitted successfully"));
#else
	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("[TURN-TO-FACE] GameEventBus not available, turn-to-face event NOT emitted"));
	UE_LOG(LogDialogueTurnToFace, Warning, TEXT("  Recompile DialogueSystemCore with WITH_GAMEEVENTBUS=1 to enable events"));
#endif
}

FRotator UDialogueEffect_TurnToFace::CalculateTargetRotation(AActor* Actor, AActor* Target) const
{
	if (!Actor || !Target)
	{
		return FRotator::ZeroRotator;
	}

	// Calculate direction from actor to target (2D only - yaw)
	FVector DirectionToTarget = (Target->GetActorLocation() - Actor->GetActorLocation()).GetSafeNormal2D();
	
	// Convert to rotation (yaw only)
	FRotator TargetRotation = DirectionToTarget.Rotation();
	
	// Preserve pitch and roll from current rotation
	FRotator CurrentRotation = Actor->GetActorRotation();
	TargetRotation.Pitch = CurrentRotation.Pitch;
	TargetRotation.Roll = CurrentRotation.Roll;

	return TargetRotation;
}

void UDialogueEffect_TurnToFace::LogTurnToFaceInfo(AActor* RotatingActor, AActor* TargetActor) const
{
	if (!RotatingActor)
	{
		return;
	}

	UE_LOG(LogDialogueTurnToFace, Log, TEXT("[TURN-TO-FACE] Rotation Info:"));
	UE_LOG(LogDialogueTurnToFace, Log, TEXT("  Rotating Actor: %s"), *RotatingActor->GetName());
	UE_LOG(LogDialogueTurnToFace, Log, TEXT("    Current Location: %s"), *RotatingActor->GetActorLocation().ToString());
	UE_LOG(LogDialogueTurnToFace, Log, TEXT("    Current Rotation: %s"), *RotatingActor->GetActorRotation().ToString());

	if (TargetActor)
	{
		UE_LOG(LogDialogueTurnToFace, Log, TEXT("  Target Actor: %s"), *TargetActor->GetName());
		UE_LOG(LogDialogueTurnToFace, Log, TEXT("    Target Location: %s"), *TargetActor->GetActorLocation().ToString());

		// Calculate target rotation
		FRotator TargetRotation = CalculateTargetRotation(RotatingActor, TargetActor);
		UE_LOG(LogDialogueTurnToFace, Log, TEXT("    Target Rotation: %s"), *TargetRotation.ToString());

		// Calculate rotation delta
		FRotator CurrentRotation = RotatingActor->GetActorRotation();
		float YawDelta = FMath::Abs(FRotator::NormalizeAxis(TargetRotation.Yaw - CurrentRotation.Yaw));
		UE_LOG(LogDialogueTurnToFace, Log, TEXT("    Yaw Delta: %.2f degrees"), YawDelta);
	}
	else
	{
		UE_LOG(LogDialogueTurnToFace, Log, TEXT("  Target: Forward (0, 0, 0)"));
	}

	UE_LOG(LogDialogueTurnToFace, Log, TEXT("  Mode: %d"), static_cast<int32>(RotationMode));
	UE_LOG(LogDialogueTurnToFace, Log, TEXT("  Duration: %.2f seconds"), RotationDuration);
	UE_LOG(LogDialogueTurnToFace, Log, TEXT("  HeadOnly: %s"), bHeadOnly ? TEXT("YES") : TEXT("NO"));
}
