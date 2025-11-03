// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/RelationshipState.h"
#include "RelationshipSystemCore.h"

URelationshipState::URelationshipState()
	: bIsDefaultState(false)
{
	DisplayColor = FLinearColor(1.0f, 0.8f, 0.3f); // Golden default
}

void URelationshipState::SortTransitionsByPriority()
{
	Transitions.Sort([](const FRelationshipStateTransition& A, const FRelationshipStateTransition& B)
	{
		return A.Priority > B.Priority; // Descending order
	});
}

TArray<FRelationshipStateTransition> URelationshipState::GetSortedTransitions() const
{
	TArray<FRelationshipStateTransition> Sorted = Transitions;
	Sorted.Sort([](const FRelationshipStateTransition& A, const FRelationshipStateTransition& B)
	{
		return A.Priority > B.Priority;
	});
	return Sorted;
}

bool URelationshipState::IsActionAvailable(FGameplayTag ActionTag) const
{
	return AvailableActions.HasTagExact(ActionTag);
}

float URelationshipState::GetDimensionModifier(FGameplayTag DimensionTag) const
{
	if (const float* Modifier = DimensionModifiers.Find(DimensionTag))
	{
		return *Modifier;
	}
	return 0.0f;
}

bool URelationshipState::ValidateInternal(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	// Check for circular transitions
	TSet<FGameplayTag> VisitedStates;
	VisitedStates.Add(AssetTag);

	for (const FRelationshipStateTransition& Transition : Transitions)
	{
		if (!Transition.TargetState.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Transition has invalid target state")));
			bIsValid = false;
			continue;
		}

		// Check for self-transition
		if (Transition.TargetState == AssetTag)
		{
			OutErrors.Add(FText::FromString(FString::Printf(
				TEXT("Self-transition detected in state %s"),
				*AssetTag.ToString())));
			// Not critical, allow self-transitions for some use cases
		}

		// Validate hysteresis
		if (Transition.HysteresisDelay < 0.0f)
		{
			OutErrors.Add(FText::FromString(TEXT("HysteresisDelay cannot be negative")));
			bIsValid = false;
		}
	}

	// Validate dimension modifiers
	for (const auto& Pair : DimensionModifiers)
	{
		if (!Pair.Key.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Invalid dimension tag in modifiers")));
			bIsValid = false;
		}
	}

	return bIsValid;
}

#if WITH_EDITOR
void URelationshipState::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Auto-sort transitions when they change
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(URelationshipState, Transitions))
	{
		SortTransitionsByPriority();
	}
}
#endif

// FRelationshipStateInstance implementation

void FRelationshipStateInstance::SetState(FGameplayTag NewState, float CurrentGameTime)
{
	CurrentState = NewState;
	StateEntryTime = CurrentGameTime;
	PendingTransitions.Empty();
}

void FRelationshipStateInstance::UpdatePendingTransition(FGameplayTag TargetState, float DeltaTime, float HysteresisDelay)
{
	if (!PendingTransitions.Contains(TargetState))
	{
		PendingTransitions.Add(TargetState, 0.0f);
	}

	PendingTransitions[TargetState] += DeltaTime;
}

void FRelationshipStateInstance::ClearPendingTransition(FGameplayTag TargetState)
{
	PendingTransitions.Remove(TargetState);
}

bool FRelationshipStateInstance::IsTransitionReady(FGameplayTag TargetState, float HysteresisDelay) const
{
	if (const float* ElapsedTime = PendingTransitions.Find(TargetState))
	{
		return *ElapsedTime >= HysteresisDelay;
	}
	return HysteresisDelay <= 0.0f; // No hysteresis means instant transition
}

float FRelationshipStateInstance::GetTimeInState(float CurrentGameTime) const
{
	return FMath::Max(0.0f, CurrentGameTime - StateEntryTime);
}
