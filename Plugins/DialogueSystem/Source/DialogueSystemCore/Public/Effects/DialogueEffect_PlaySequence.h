// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Effects/DialogueEffectExecutor.h"
#include "MovieSceneSequencePlayer.h"
#include "DialogueEffect_PlaySequence.generated.h"

// Forward declarations
class ULevelSequence;
class ULevelSequencePlayer;
class ALevelSequenceActor;

/**
 * NEW v1.13.2: Delegate for sequence participants (for ActorSchedule integration)
 * Called when sequence starts to notify external systems about involved actors
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSequenceParticipantsGathered, const TArray<AActor*>&, Participants);

/**
 * Play a Level Sequence during dialogue
 * 
 * Usage in Dialogue Node:
 * EffectExpression: "PlaySequence(/Game/Sequences/LS_EllenApproach)"
 * 
 * Features:
 * - Plays sequence and waits for completion
 * - Restores camera after sequence ends
 * - Pauses dialogue until sequence finishes
 * - NEW v1.13.2: Notifies about Possessable actors for schedule integration
 */
UCLASS(BlueprintType, meta = (DisplayName = "Play Sequence"))
class DIALOGUESYSTEMCORE_API UDialogueEffect_PlaySequence : public UDialogueEffect
{
	GENERATED_BODY()

public:
	UDialogueEffect_PlaySequence();

	/** Level Sequence asset to play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSoftObjectPtr<ULevelSequence> SequenceAsset;

	/** Wait for sequence to complete before continuing dialogue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bWaitForCompletion = true;

	/** Restore player camera after sequence ends */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bRestoreCameraAfter = true;

	/** Restore actor transforms after sequence ends (для Possessables) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bRestoreActorTransforms = true;

	/** Playback settings */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FMovieSceneSequencePlaybackSettings PlaybackSettings;

	/** 
	 * NEW v1.13.2: Event fired when sequence participants are gathered
	 * Use this to pause ActorSchedule for sequence actors via GameEventBus
	 */
	UPROPERTY(BlueprintAssignable, Category = "Effect|Events")
	FOnSequenceParticipantsGathered OnSequenceParticipantsGathered;

	//~ Begin UDialogueEffect Interface
	virtual void Execute_Implementation(UDialogueSessionContext* Context) override;
	virtual FText GetDisplayText_Implementation() const override;
	//~ End UDialogueEffect Interface

	/** Stop sequence playback (called when dialogue ends prematurely) */
	UFUNCTION(BlueprintCallable, Category = "Effect")
	void StopSequence();

	/** Check if sequence is currently playing */
	UFUNCTION(BlueprintPure, Category = "Effect")
	bool IsPlaying() const;

	/** NEW v1.16.8: Get loaded sequence asset */
	UFUNCTION(BlueprintPure, Category = "Effect")
	ULevelSequence* GetSequence() const;

	/** NEW v1.13.2: Gather Possessable actors from this sequence */
	TArray<AActor*> GatherSequenceParticipants() const;

protected:
	/** Spawned sequence actor */
	UPROPERTY(Transient)
	TObjectPtr<ALevelSequenceActor> SpawnedSequenceActor;

	/** Sequence player */
	UPROPERTY(Transient)
	TObjectPtr<ULevelSequencePlayer> SequencePlayer;

	/** Stored actor transforms before sequence (для восстановления) */
	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<AActor>, FTransform> OriginalActorTransforms;

	/** NEW v1.14: Cached sequence participants (for cleanup on stop) */
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> CachedParticipants;

	/** NEW v1.17.1: Cached context reference (for ApplyPendingEndPositions) */
	UPROPERTY(Transient)
	TWeakObjectPtr<UDialogueSessionContext> CachedContext;

	/** Handle sequence finished */
	UFUNCTION()
	void OnSequenceFinished();

	/** Internal cleanup logic */
	void CleanupSequence(bool bSkipTransformRestore = false);

	/** NEW v1.13.3: Emit GameEventBus event when sequence starts */
	void EmitSequenceStartedEvent(const TArray<AActor*>& Participants, class UDialogueSessionContext* Context);

	/** NEW v1.13.3: Emit GameEventBus event when sequence ends */
	void EmitSequenceEndedEvent(const TArray<AActor*>& Participants);
};
