// Copyright Epic Games, Inc. All Rights Reserved.

#include "Positioning/SequencePositioningExtractor.h"
#include "Channels/MovieSceneDoubleChannel.h"
#include "MovieSceneSection.h"

DEFINE_LOG_CATEGORY(LogSequencePositioning);

FSequencePositioningExtractor::FSequencePositioningExtractor(ULevelSequence* InSequence)
	: Sequence(InSequence)
{
	if (Sequence)
	{
		MovieScene = Sequence->GetMovieScene();
		
		if (!MovieScene)
		{
			UE_LOG(LogSequencePositioning, Warning, TEXT("Sequence '%s' has no MovieScene!"), *Sequence->GetName());
		}
	}
}

bool FSequencePositioningExtractor::GetActorTransformAtTime(const FName& PersonaId, float TimeInSeconds, FTransform& OutTransform) const
{
	if (!IsValid())
	{
		UE_LOG(LogSequencePositioning, Error, TEXT("GetActorTransformAtTime: Invalid sequence!"));
		return false;
	}

	// Find possessable by PersonaId
	const FMovieScenePossessable* Possessable = FindPossessable(PersonaId);
	if (!Possessable)
	{
		UE_LOG(LogSequencePositioning, Warning, TEXT("GetActorTransformAtTime: Possessable '%s' not found in sequence '%s'"), 
			*PersonaId.ToString(), *Sequence->GetName());
		return false;
	}

	// Find 3D transform track for this possessable
	const FGuid& PossessableGuid = Possessable->GetGuid();
	UMovieScene3DTransformTrack* TransformTrack = MovieScene->FindTrack<UMovieScene3DTransformTrack>(PossessableGuid);
	
	if (!TransformTrack)
	{
		UE_LOG(LogSequencePositioning, Warning, TEXT("GetActorTransformAtTime: No 3D Transform track for '%s' in sequence '%s'"), 
			*PersonaId.ToString(), *Sequence->GetName());
		return false;
	}

	// Convert time to frame
	FFrameNumber Frame = TimeToFrame(TimeInSeconds);

	// Extract transform at this frame
	bool bSuccess = ExtractTransformFromTrack(TransformTrack, Frame, OutTransform);
	
	if (bSuccess)
	{
		UE_LOG(LogSequencePositioning, Log, TEXT("Extracted transform for '%s' at time %.2fs: Loc=%s, Rot=%s"), 
			*PersonaId.ToString(), TimeInSeconds, 
			*OutTransform.GetLocation().ToString(), 
			*OutTransform.GetRotation().Rotator().ToString());
	}

	return bSuccess;
}

TMap<FName, FTransform> FSequencePositioningExtractor::GetStartPositions() const
{
	TMap<FName, FTransform> Positions;

	if (!IsValid())
	{
		return Positions;
	}

	UE_LOG(LogSequencePositioning, Log, TEXT("GetStartPositions: Extracting START positions from sequence '%s'"), 
		*Sequence->GetName());

	// Iterate all possessables
	const int32 PossessableCount = MovieScene->GetPossessableCount();
	for (int32 i = 0; i < PossessableCount; ++i)
	{
		const FMovieScenePossessable& Possessable = MovieScene->GetPossessable(i);
		FName PersonaId = FName(Possessable.GetName());

		FTransform Transform;
		if (GetActorTransformAtTime(PersonaId, 0.0f, Transform))
		{
			Positions.Add(PersonaId, Transform);
			
			UE_LOG(LogSequencePositioning, Log, TEXT("  START Position[%s]: Loc=%s, Rot=%s"),
				*PersonaId.ToString(),
				*Transform.GetLocation().ToString(),
				*Transform.GetRotation().Rotator().ToString());
		}
	}

	UE_LOG(LogSequencePositioning, Log, TEXT("GetStartPositions: Extracted %d START positions from sequence '%s'"), 
		Positions.Num(), *Sequence->GetName());

	return Positions;
}

TMap<FName, FTransform> FSequencePositioningExtractor::GetEndPositions() const
{
	TMap<FName, FTransform> Positions;

	if (!IsValid())
	{
		return Positions;
	}

	// FIXED v1.17.1: Use last frame of playback range instead of Duration
	// This ensures we get the ACTUAL end keyframe position
	const FFrameRate TickResolution = MovieScene->GetTickResolution();
	const TRange<FFrameNumber> PlaybackRange = MovieScene->GetPlaybackRange();

	if (!PlaybackRange.HasUpperBound())
	{
		UE_LOG(LogSequencePositioning, Warning, TEXT("GetEndPositions: Sequence has no upper bound!"));
		return Positions;
	}

	// Get the last frame (exclusive upper bound - 1)
	const FFrameNumber EndFrame = PlaybackRange.GetUpperBoundValue() - 1;
	const float EndTime = TickResolution.AsSeconds(EndFrame - PlaybackRange.GetLowerBoundValue());

	UE_LOG(LogSequencePositioning, Log, TEXT("GetEndPositions: Using EndFrame=%d (time=%.2fs) for sequence '%s'"), 
		EndFrame.Value, EndTime, *Sequence->GetName());

	// Iterate all possessables
	const int32 PossessableCount = MovieScene->GetPossessableCount();
	for (int32 i = 0; i < PossessableCount; ++i)
	{
		const FMovieScenePossessable& Possessable = MovieScene->GetPossessable(i);
		FName PersonaId = FName(Possessable.GetName());

		FTransform Transform;
		if (GetActorTransformAtTime(PersonaId, EndTime, Transform))
		{
			Positions.Add(PersonaId, Transform);
			
			UE_LOG(LogSequencePositioning, Log, TEXT("  END Position[%s]: Loc=%s, Rot=%s"),
				*PersonaId.ToString(),
				*Transform.GetLocation().ToString(),
				*Transform.GetRotation().Rotator().ToString());
		}
	}

	UE_LOG(LogSequencePositioning, Log, TEXT("GetEndPositions: Extracted %d END positions from sequence '%s'"), 
		Positions.Num(), *Sequence->GetName());

	return Positions;
}

float FSequencePositioningExtractor::GetSequenceDuration() const
{
	if (!IsValid())
	{
		return 0.0f;
	}

	const FFrameRate TickResolution = MovieScene->GetTickResolution();
	const TRange<FFrameNumber> PlaybackRange = MovieScene->GetPlaybackRange();

	if (PlaybackRange.HasLowerBound() && PlaybackRange.HasUpperBound())
	{
		const FFrameNumber StartFrame = PlaybackRange.GetLowerBoundValue();
		const FFrameNumber EndFrame = PlaybackRange.GetUpperBoundValue();
		const FFrameNumber Duration = EndFrame - StartFrame;

		return TickResolution.AsSeconds(Duration);
	}

	return 0.0f;
}

const FMovieScenePossessable* FSequencePositioningExtractor::FindPossessable(const FName& PersonaId) const
{
	if (!IsValid())
	{
		return nullptr;
	}

	const FString PersonaIdStr = PersonaId.ToString();

	// Search all possessables
	const int32 PossessableCount = MovieScene->GetPossessableCount();
	for (int32 i = 0; i < PossessableCount; ++i)
	{
		const FMovieScenePossessable& Possessable = MovieScene->GetPossessable(i);

		// Check both Name and DisplayName (DisplayName is visible in Sequencer UI)
		if (Possessable.GetName() == PersonaIdStr || 
			Possessable.GetPossessedObjectClass()->GetName() == PersonaIdStr)
		{
			return &Possessable;
		}
	}

	return nullptr;
}

bool FSequencePositioningExtractor::ExtractTransformFromTrack(const UMovieScene3DTransformTrack* Track, FFrameNumber Frame, FTransform& OutTransform) const
{
	if (!Track)
	{
		return false;
	}

	// Get all sections from track
	const TArray<UMovieSceneSection*>& Sections = Track->GetAllSections();
	if (Sections.Num() == 0)
	{
		UE_LOG(LogSequencePositioning, Warning, TEXT("ExtractTransformFromTrack: Track has no sections!"));
		return false;
	}

	// Use first section (typically there's only one section per track)
	UMovieScene3DTransformSection* TransformSection = Cast<UMovieScene3DTransformSection>(Sections[0]);
	if (!TransformSection)
	{
		UE_LOG(LogSequencePositioning, Warning, TEXT("ExtractTransformFromTrack: Section is not a 3D Transform section!"));
		return false;
	}

	// Evaluate channels at frame
	TArrayView<FMovieSceneDoubleChannel*> Channels = TransformSection->GetChannelProxy().GetChannels<FMovieSceneDoubleChannel>();
	
	// Channels order: TranslationX, TranslationY, TranslationZ, RotationX, RotationY, RotationZ, ScaleX, ScaleY, ScaleZ
	if (Channels.Num() < 9)
	{
		UE_LOG(LogSequencePositioning, Warning, TEXT("ExtractTransformFromTrack: Not enough channels (%d < 9)!"), Channels.Num());
		return false;
	}

	// Evaluate each channel at frame
	double Values[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 }; // Default values

	for (int32 i = 0; i < 9; ++i)
	{
		if (Channels[i])
		{
			Channels[i]->Evaluate(Frame, Values[i]);
		}
	}

	// Build transform from evaluated values
	FVector Location(Values[0], Values[1], Values[2]);
	FRotator Rotation(Values[4], Values[5], Values[3]); // Pitch=Y, Yaw=Z, Roll=X
	FVector Scale(Values[6], Values[7], Values[8]);

	OutTransform = FTransform(Rotation.Quaternion(), Location, Scale);
	return true;
}

FFrameNumber FSequencePositioningExtractor::TimeToFrame(float TimeInSeconds) const
{
	if (!IsValid())
	{
		return FFrameNumber(0);
	}

	const FFrameRate TickResolution = MovieScene->GetTickResolution();
	return TickResolution.AsFrameNumber(TimeInSeconds);
}
