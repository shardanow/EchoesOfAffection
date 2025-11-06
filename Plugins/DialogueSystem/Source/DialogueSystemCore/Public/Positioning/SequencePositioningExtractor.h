// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/DialoguePositioningTypes.h"
#include "LevelSequence.h"
#include "MovieScene.h"
#include "MovieScenePossessable.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "SequencePositioningExtractor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSequencePositioning, Log, All);

/**
 * v1.16.8: Utility for extracting actor positions from LevelSequence
 * 
 * Purpose:
 * - Automatically get actor positions from sequence start/end frames
 * - Eliminates need for manual position setup in DialogueNode
 * - Ensures consistency between sequence animation and positioning
 * 
 * Usage:
 *   FSequencePositioningExtractor Extractor(MySequence);
 *   FTransform StartPos = Extractor.GetActorTransformAtTime("Lianne", 0.0f);
 *   FTransform EndPos = Extractor.GetActorTransformAtTime("Lianne", Extractor.GetSequenceDuration());
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FSequencePositioningExtractor
{
	GENERATED_BODY()

public:
	FSequencePositioningExtractor() = default;
	explicit FSequencePositioningExtractor(ULevelSequence* InSequence);

	/**
	 * Extract actor transform at specific time in sequence
	 * @param PersonaId - Actor identifier (Possessable name or DisplayName)
	 * @param TimeInSeconds - Time position in sequence (0.0 = start, Duration = end)
	 * @param OutTransform - Extracted transform if found
	 * @return true if transform was extracted successfully
	 */
	bool GetActorTransformAtTime(const FName& PersonaId, float TimeInSeconds, FTransform& OutTransform) const;

	/**
	 * Get all actor transforms at sequence start (frame 0)
	 * @return Map of PersonaId -> Transform at sequence start
	 */
	TMap<FName, FTransform> GetStartPositions() const;

	/**
	 * Get all actor transforms at sequence end (last frame)
	 * @return Map of PersonaId -> Transform at sequence end
	 */
	TMap<FName, FTransform> GetEndPositions() const;

	/**
	 * Get sequence duration in seconds
	 */
	float GetSequenceDuration() const;

	/**
	 * Check if sequence is valid and has data
	 */
	bool IsValid() const { return Sequence != nullptr && MovieScene != nullptr; }

private:
	/** Find possessable by PersonaId (checks both GUID name and DisplayName) */
	const FMovieScenePossessable* FindPossessable(const FName& PersonaId) const;

	/** Extract transform from 3D transform track at specific frame */
	bool ExtractTransformFromTrack(const UMovieScene3DTransformTrack* Track, FFrameNumber Frame, FTransform& OutTransform) const;

	/** Convert time in seconds to frame number */
	FFrameNumber TimeToFrame(float TimeInSeconds) const;

private:
	UPROPERTY()
	TObjectPtr<ULevelSequence> Sequence = nullptr;

	UPROPERTY()
	TObjectPtr<UMovieScene> MovieScene = nullptr;
};
