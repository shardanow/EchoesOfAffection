// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_PlaySequence.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueRunner.h" // NEW v1.13: For sequence registration
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlayer.h"
#include "MovieScene.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h" // FIX v1.17.3: For physics-safe restoration
#include "Components/CapsuleComponent.h" // FIX v1.17.3: For capsule component access
#include "EngineUtils.h"

// ? GameEventBus integration (only if enabled)
#if WITH_GAMEEVENTBUS
#include "GameEventBusSubsystem.h"
#endif

// ? NEW: ActorSchedule integration - check if actor has active schedule
// Use soft dependency to avoid hard coupling
#include "Components/ActorComponent.h" // For FindComponentByClass

DEFINE_LOG_CATEGORY_STATIC(LogDialogueSequence, Log, All);

UDialogueEffect_PlaySequence::UDialogueEffect_PlaySequence()
{
	bSupportsReverse = false; // Cannot undo sequence playback
	
	// Default playback settings
	PlaybackSettings.bAutoPlay = true;
	PlaybackSettings.bPauseAtEnd = false;
	PlaybackSettings.bDisableMovementInput = true;
	PlaybackSettings.bDisableLookAtInput = true;
	PlaybackSettings.bHidePlayer = false;
	PlaybackSettings.bHideHud = false;
}

void UDialogueEffect_PlaySequence::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		UE_LOG(LogDialogueSequence, Error, TEXT("Execute_PlaySequence: Context is null"));
		return;
	}

	// NEW v1.17.1: Cache context for later use in OnSequenceFinished
	CachedContext = Context;

	if (!SequenceAsset.IsValid() && SequenceAsset.IsPending())
	{
		// Try to load synchronously if not loaded yet
		SequenceAsset.LoadSynchronous();
	}

	ULevelSequence* Sequence = SequenceAsset.Get();
	if (!Sequence)
	{
		UE_LOG(LogDialogueSequence, Error, TEXT("Execute_PlaySequence: SequenceAsset is null or failed to load"));
		return;
	}

	UWorld* World = Context->GetWorld();
	if (!World)
	{
		UE_LOG(LogDialogueSequence, Error, TEXT("Execute_PlaySequence: World is null"));
		return;
	}

	// NEW v1.13: Register this sequence with DialogueRunner for cleanup on dialogue end
	UObject* OwningRunner = Context->GetOwningRunner();
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE REGISTER] ?? Attempting to register sequence..."));
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE REGISTER]   OwningRunner: %s"), OwningRunner ? *OwningRunner->GetName() : TEXT("NULL"));
	
	if (OwningRunner)
	{
		// Direct cast instead of reflection - safer and faster
		if (UDialogueRunner* Runner = Cast<UDialogueRunner>(OwningRunner))
		{
			Runner->RegisterActiveSequence(this);
			UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE REGISTER] ? Registered sequence with DialogueRunner: %s"), *Runner->GetName());
		}
		else
		{
			UE_LOG(LogDialogueSequence, Error, TEXT("[SEQUENCE REGISTER] ? OwningRunner is not a DialogueRunner - sequence won't be auto-stopped"));
			UE_LOG(LogDialogueSequence, Error, TEXT("[SEQUENCE REGISTER]    OwningRunner class: %s"), *OwningRunner->GetClass()->GetName());
		}
	}
	else
	{
		UE_LOG(LogDialogueSequence, Error, TEXT("[SEQUENCE REGISTER] ? OwningRunner is NULL - sequence won't be auto-stopped"));
	}

	// Save original actor transforms if needed (для Possessables)
	if (bRestoreActorTransforms)
	{
		OriginalActorTransforms.Empty();
		
		// Get all possessable bindings from sequence
		UMovieScene* MovieScene = Sequence->GetMovieScene();
		if (MovieScene)
		{
			// Iterate through possessables
			for (int32 i = 0; i < MovieScene->GetPossessableCount(); ++i)
			{
				FMovieScenePossessable& Possessable = MovieScene->GetPossessable(i);
				
				// Find actor in world by name/GUID
				// NOTE: This is simplified - real implementation would need proper binding resolution
				FString ActorName = Possessable.GetName();
				
				// Find actor in level
				for (TActorIterator<AActor> It(World); It; ++It)
				{
					AActor* Actor = *It;
					if (Actor && Actor->GetName().Contains(ActorName))
					{
						// Store original transform
						OriginalActorTransforms.Add(Actor, Actor->GetActorTransform());
						UE_LOG(LogDialogueSequence, Log, TEXT("Saved transform for actor: %s at %s"), 
							*Actor->GetName(), *Actor->GetActorLocation().ToString());
						break;
					}
				}
			}
		}
	}

	// NEW v1.13.2: Gather sequence participants and broadcast event
	// This allows external systems (Blueprint/C++) to pause ActorSchedule for sequence actors
	TArray<AActor*> Participants = GatherSequenceParticipants();
	
	// ? NEW v1.14: Cache participants for cleanup on stop
	CachedParticipants.Empty();
	for (AActor* Actor : Participants)
	{
		if (Actor)
		{
			CachedParticipants.Add(Actor);
		}
	}
	
	if (Participants.Num() > 0)
	{
		UE_LOG(LogDialogueSequence, Log, TEXT("Broadcasting OnSequenceParticipantsGathered with %d actors"), Participants.Num());
		OnSequenceParticipantsGathered.Broadcast(Participants);
		
		// ? NEW v1.13.3: Emit GameEventBus event for ActorScheduleSystem integration
		// This pauses sequence participants (like Alice) during cutscene
		EmitSequenceStartedEvent(Participants, Context);
	}

	// Spawn Level Sequence Actor
	FMovieSceneSequencePlaybackSettings Settings = PlaybackSettings;
	
	ALevelSequenceActor* SequenceActor = nullptr;
	ULevelSequencePlayer* Player = ULevelSequencePlayer::CreateLevelSequencePlayer(
		World,
		Sequence,
		Settings,
		SequenceActor
	);

	if (!Player || !SequenceActor)
	{
		UE_LOG(LogDialogueSequence, Error, TEXT("Execute_PlaySequence: Failed to create LevelSequencePlayer"));
		return;
	}

	SpawnedSequenceActor = SequenceActor;
	SequencePlayer = Player;

	// Bind to sequence finished event if we need to wait
	if (bWaitForCompletion)
	{
		Player->OnFinished.AddDynamic(this, &UDialogueEffect_PlaySequence::OnSequenceFinished);
		
		// TODO: Pause dialogue runner until sequence completes
		// This would require extending DialogueRunner to support async effects
		UE_LOG(LogDialogueSequence, Warning, TEXT("bWaitForCompletion is not fully implemented yet - sequence will play but dialogue may continue"));
	}

	// Play sequence
	Player->Play();

	UE_LOG(LogDialogueSequence, Log, TEXT("Playing sequence: %s"), *Sequence->GetName());
}

void UDialogueEffect_PlaySequence::OnSequenceFinished()
{
	UE_LOG(LogDialogueSequence, Warning, TEXT("=== OnSequenceFinished CALLED ==="));
	
	// NEW v1.17.1: Apply pending end positions from DialogueRunner
	bool bAppliedEndPositions = false;
	
	if (CachedContext.IsValid())
	{
		UDialogueSessionContext* Context = CachedContext.Get();
		UObject* OwningRunnerObj = Context->GetOwningRunner();
		
		if (UDialogueRunner* OwningRunner = Cast<UDialogueRunner>(OwningRunnerObj))
		{
			UE_LOG(LogDialogueSequence, Warning, TEXT("  Applying pending end positions after sequence finish..."));
			OwningRunner->ApplyPendingEndPositions();
			bAppliedEndPositions = true;
		}
		else
		{
			UE_LOG(LogDialogueSequence, Warning, TEXT("  No OwningRunner found - end positions not applied"));
		}
	}
	else
	{
		UE_LOG(LogDialogueSequence, Warning, TEXT("  CachedContext invalid - end positions not applied"));
	}

	// Pass flag to CleanupSequence to prevent transform restoration if end positions were applied
	CleanupSequence(bAppliedEndPositions);
	
	UE_LOG(LogDialogueSequence, Warning, TEXT("=== OnSequenceFinished COMPLETE ==="));
}

void UDialogueEffect_PlaySequence::CleanupSequence(bool bSkipTransformRestore)
{
	UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] CleanupSequence called (bSkipTransformRestore=%s)"), 
		bSkipTransformRestore ? TEXT("true") : TEXT("false"));
	
	// ? NEW v1.14: Use cached participants instead of gathering again
	TArray<AActor*> Participants;
	for (const TWeakObjectPtr<AActor>& WeakActor : CachedParticipants)
	{
		if (AActor* Actor = WeakActor.Get())
		{
			Participants.Add(Actor);
		}
	}
	
	UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] Found %d cached participants"), Participants.Num());
	
	// ? NEW v1.13.3: Emit sequence ended event BEFORE cleanup
	// This allows ScheduleComponent to resume actors
	if (Participants.Num() > 0)
	{
		EmitSequenceEndedEvent(Participants);
	}

	// NEW v1.13: Unregister from DialogueRunner
	// (Skip - DialogueRunner already cleared ActiveSequenceEffect)

	// Restore actor transforms if requested (для Possessables)
	// NEW v1.17.1: Skip restoration if end positions were applied
	if (bRestoreActorTransforms && !bSkipTransformRestore)
	{
		UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] Restoring %d actor transforms"), OriginalActorTransforms.Num());
		
		for (auto& Pair : OriginalActorTransforms)
		{
			AActor* Actor = Pair.Key.Get();
			if (Actor && !Actor->IsActorBeingDestroyed())
			{
				// ? Check if actor has ScheduleComponent (soft dependency)
				bool bShouldRestore = true;
				
				for (UActorComponent* Comp : Actor->GetComponents())
				{
					if (Comp && Comp->GetClass()->GetName().Contains(TEXT("ScheduleComponent")))
					{
						UFunction* IsPausedFunc = Comp->FindFunction(TEXT("IsSchedulePaused"));
						UFunction* HasActiveFunc = Comp->FindFunction(TEXT("HasActiveAction"));
						
						bool bIsPaused = true;
						bool bHasActiveAction = false;
						
						if (IsPausedFunc)
						{
							Comp->ProcessEvent(IsPausedFunc, &bIsPaused);
						}
						
						if (HasActiveFunc)
						{
							Comp->ProcessEvent(HasActiveFunc, &bHasActiveAction);
						}
						
						if (bHasActiveAction && !bIsPaused)
						{
							bShouldRestore = false;
							UE_LOG(LogDialogueSequence, Warning, 
								TEXT("[CLEANUP] Skipping transform restore for actor: %s (has active schedule)"), 
								*Actor->GetName());
						}
						else
						{
							UE_LOG(LogDialogueSequence, Log, 
								TEXT("[CLEANUP] Actor %s has ScheduleComponent - will restore (active: %s, paused: %s)"), 
								*Actor->GetName(),
								bHasActiveAction ? TEXT("YES") : TEXT("NO"),
								bIsPaused ? TEXT("YES") : TEXT("NO"));
						}
						break;
					}
				}
				
				if (bShouldRestore)
				{
					const FTransform& OriginalTransform = Pair.Value;
					
					// ===== FIX v1.17.3: Physics-safe transform restoration =====
					// Use safe teleportation to prevent "falling" after sequence
					
					// DEBUG: Log actor class for troubleshooting
					UE_LOG(LogDialogueSequence, Warning, TEXT("[CLEANUP DEBUG] Restoring actor: %s (Class: %s)"), 
						*Actor->GetName(), 
						*Actor->GetClass()->GetName());
					
					// For characters, compensate for capsule offset (same as teleport effect)
					ACharacter* Character = Cast<ACharacter>(Actor);
					if (Character && Character->GetCapsuleComponent())
					{
						UE_LOG(LogDialogueSequence, Warning, TEXT("[CLEANUP DEBUG] Actor IS a Character - using physics-safe restoration"));
						
						// Stop any movement before restoration
						if (Character->GetCharacterMovement())
						{
							Character->GetCharacterMovement()->StopMovementImmediately();
							Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
						}
						
						// Use physics-safe teleportation
						FVector RestoreLocation = OriginalTransform.GetLocation();
						FRotator RestoreRotation = OriginalTransform.GetRotation().Rotator();
						
						const bool bSweep = false;
						FHitResult HitResult;
						Actor->SetActorLocationAndRotation(
							RestoreLocation,
							RestoreRotation,
							bSweep,
							&HitResult,
							ETeleportType::ResetPhysics // CRITICAL: Reset physics to prevent falling
						);
						
						UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] Restored transform (physics-safe) for actor: %s"), *Actor->GetName());
					}
					else
					{
						UE_LOG(LogDialogueSequence, Warning, TEXT("[CLEANUP DEBUG] Actor is NOT a Character (or has no capsule) - using standard restoration"));
						
						// For non-characters, use standard transform (no physics issues)
						Actor->SetActorTransform(OriginalTransform);
						UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] Restored transform for actor: %s"), *Actor->GetName());
					}
				}
			}
		}
		OriginalActorTransforms.Empty();
	}
	else if (bSkipTransformRestore)
	{
		UE_LOG(LogDialogueSequence, Warning, TEXT("[CLEANUP] Skipping transform restoration - END positions were applied!"));
		OriginalActorTransforms.Empty(); // Clear but don't restore
	}

	// ? REMOVED: Camera restore (already done in StopSequence)

	// Clean up sequence actor
	if (SpawnedSequenceActor)
	{
		UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] Destroying SpawnedSequenceActor"));
		SpawnedSequenceActor->Destroy();
		SpawnedSequenceActor = nullptr;
	}
	
	SequencePlayer = nullptr;
	CachedParticipants.Empty();
	
	UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] Cleanup complete"));
}

void UDialogueEffect_PlaySequence::StopSequence()
{
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] ============================================"));
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] StopSequence called - force stopping sequence playback"));

	// ? FIX: Stop playback FIRST before any cleanup
	if (SequencePlayer)
	{
		UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] Stopping SequencePlayer..."));
		SequencePlayer->Stop();
		SequencePlayer->OnFinished.RemoveAll(this);
		UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] SequencePlayer stopped"));
	}
	else
	{
		UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] No SequencePlayer to stop"));
	}

	// ? FIX: Restore camera IMMEDIATELY (before cleanup)
	if (bRestoreCameraAfter)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			APlayerController* PC = World->GetFirstPlayerController();
			if (PC && PC->PlayerCameraManager)
			{
				APawn* PlayerPawn = PC->GetPawn();
				if (PlayerPawn)
				{
					UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] Restoring camera to player pawn: %s"), *PlayerPawn->GetName());
					PC->SetViewTargetWithBlend(PlayerPawn, 0.5f); // Quick blend back
					UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] Camera restored!"));
				}
				else
				{
					UE_LOG(LogDialogueSequence, Error, TEXT("[SEQUENCE STOP] No PlayerPawn to restore camera to!"));
				}
			}
		}
	}

	// Then cleanup resources
	CleanupSequence(false); // Force cleanup without skipping restoration on manual stop
	
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] Cleanup complete!"));
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE STOP] ============================================"));
}

bool UDialogueEffect_PlaySequence::IsPlaying() const
{
	return SequencePlayer != nullptr && SequencePlayer->IsPlaying();
}

// NEW v1.16.8: Get loaded sequence asset
ULevelSequence* UDialogueEffect_PlaySequence::GetSequence() const
{
	// Try to get loaded asset first
	if (SequenceAsset.IsValid())
	{
		return SequenceAsset.Get();
	}
	
	// If not loaded but path is valid, try to load synchronously
	if (SequenceAsset.IsPending())
	{
		return SequenceAsset.LoadSynchronous();
	}
	
	return nullptr;
}

FText UDialogueEffect_PlaySequence::GetDisplayText_Implementation() const
{
	if (SequenceAsset.IsValid())
	{
		return FText::FromString(FString::Printf(TEXT("Play Sequence: %s"), *SequenceAsset->GetName()));
	}
	return FText::FromString(TEXT("Play Sequence (None)"));
}

TArray<AActor*> UDialogueEffect_PlaySequence::GatherSequenceParticipants() const
{
	TArray<AActor*> Participants;

	ULevelSequence* Sequence = SequenceAsset.Get();
	if (!Sequence)
	{
		return Participants;
	}

	UMovieScene* MovieScene = Sequence->GetMovieScene();
	if (!MovieScene)
	{
		return Participants;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return Participants;
	}

	// Gather Possessable actors (actors in level that sequence will control)
	for (int32 i = 0; i < MovieScene->GetPossessableCount(); ++i)
	{
		FMovieScenePossessable& Possessable = MovieScene->GetPossessable(i);
		FString ActorName = Possessable.GetName();

		// Find matching actor in world
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor && Actor->GetName().Contains(ActorName))
			{
				Participants.AddUnique(Actor);
				UE_LOG(LogDialogueSequence, Log, TEXT("Found sequence participant: %s"), *Actor->GetName());
				break;
			}
		}
	}

	return Participants;
}

//==============================================================================
// GameEventBus Integration (v1.13.3)
//==============================================================================

void UDialogueEffect_PlaySequence::EmitSequenceStartedEvent(const TArray<AActor*>& Participants, UDialogueSessionContext* Context)
{
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] EmitSequenceStartedEvent called with %d participants"), Participants.Num());
	
#if WITH_GAMEEVENTBUS
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] WITH_GAMEEVENTBUS=1 - GameEventBus available!"));
	
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] Cannot emit event: World is null"));
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] GameEventBus subsystem not found"));
		return;
	}

	// Create event payload
	FGameEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.Started"));
	Payload.StringParam = SequenceAsset.IsValid() ? SequenceAsset->GetFName() : NAME_None;
	
	// Set main dialogue participants if available
	if (Context)
	{
		Payload.InstigatorActor = Context->GetPlayer();
		Payload.TargetActor = Context->GetNPC();
	}
	
	// ? Add sequence participants (like Alice!)
	Payload.AdditionalActors = Participants;

	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] ============================================"));
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] EMITTING: GameEvent.Sequence.Started"));
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT]   Sequence: %s"), *Payload.StringParam.ToString());
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT]   Player (Instigator): %s"), 
		Payload.InstigatorActor.IsValid() ? *Payload.InstigatorActor->GetName() : TEXT("None"));
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT]   NPC (Target): %s"), 
		Payload.TargetActor.IsValid() ? *Payload.TargetActor->GetName() : TEXT("None"));
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT]   Sequence Participants: %d"), Participants.Num());
	for (AActor* Actor : Participants)
	{
		if (Actor)
		{
			UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT]     - %s"), *Actor->GetName());
		}
	}

	// Emit event
	EventBus->EmitEvent(Payload, true);
	
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] Event emitted successfully!"));
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] ============================================"));
#else
	UE_LOG(LogDialogueSequence, Error, TEXT("[SEQUENCE EVENT] WITH_GAMEEVENTBUS=0 - GameEventBus NOT AVAILABLE! Events will NOT be emitted!"));
	UE_LOG(LogDialogueSequence, Error, TEXT("[SEQUENCE EVENT] You need to recompile DialogueSystemCore module with GameEventBus dependency!"));
#endif
}

void UDialogueEffect_PlaySequence::EmitSequenceEndedEvent(const TArray<AActor*>& Participants)
{
#if WITH_GAMEEVENTBUS
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		return;
	}

	// Create event payload
	FGameEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Sequence.Ended"));
	Payload.StringParam = SequenceAsset.IsValid() ? SequenceAsset->GetFName() : NAME_None;
	Payload.AdditionalActors = Participants;

	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] EMITTING: GameEvent.Sequence.Ended"));
	EventBus->EmitEvent(Payload, true);
	UE_LOG(LogDialogueSequence, Warning, TEXT("[SEQUENCE EVENT] Sequence.Ended event emitted!"));
#else
	// No-op if GameEventBus not available
#endif
}