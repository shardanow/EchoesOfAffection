// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueStateMachine.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueStateMachine, Log, All);

UDialogueStateMachine::UDialogueStateMachine()
	: CurrentState(EDialogueState::Idle)
	, PreviousState(EDialogueState::Idle)
	, StateStartTime(FDateTime::UtcNow())
	, MaxHistorySize(100)
{
}

bool UDialogueStateMachine::TransitionTo(EDialogueState NewState, const FString& Reason)
{
	// Same state - no transition needed
	if (CurrentState == NewState)
	{
		UE_LOG(LogDialogueStateMachine, Verbose, TEXT("Already in state: %s"),
			*UEnum::GetValueAsString(NewState));
		return true;
	}

	// Validate transition
	if (!CanTransitionTo(NewState))
	{
		UE_LOG(LogDialogueStateMachine, Warning, 
			TEXT("Invalid transition: %s -> %s (Reason: %s)"),
			*UEnum::GetValueAsString(CurrentState),
			*UEnum::GetValueAsString(NewState),
			*Reason);
		return false;
	}

	// Calculate duration in previous state
	const FDateTime Now = FDateTime::UtcNow();
	const float Duration = static_cast<float>((Now - StateStartTime).GetTotalSeconds());

	// Record state change
	FDialogueStateChange Change(CurrentState, NewState, Duration, Reason);
	StateHistory.Add(Change);

	// Trim history if needed
	TrimHistory();

	// Update state
	const EDialogueState OldState = CurrentState;
	PreviousState = CurrentState;
	CurrentState = NewState;
	StateStartTime = Now;

	// Log transition
	UE_LOG(LogDialogueStateMachine, Log, 
		TEXT("State transition: %s -> %s (%.3fs, Reason: %s)"),
		*UEnum::GetValueAsString(OldState),
		*UEnum::GetValueAsString(NewState),
		Duration,
		Reason.IsEmpty() ? TEXT("None") : *Reason);

	// Notify state changed
	OnStateChanged(OldState, NewState);

	return true;
}

bool UDialogueStateMachine::CanTransitionTo(EDialogueState NewState) const
{
	return IsTransitionValid(CurrentState, NewState);
}

bool UDialogueStateMachine::IsTransitionValid(EDialogueState From, EDialogueState To) const
{
	// Same state is always valid
	if (From == To)
	{
		return true;
	}

	// Define valid transitions
	switch (From)
	{
	case EDialogueState::Idle:
		// Idle can go to Loading or Active
		return (To == EDialogueState::Loading || 
		        To == EDialogueState::Active ||
		        To == EDialogueState::Error);

	case EDialogueState::Loading:
		// Loading can go to Active or Error
		return (To == EDialogueState::Active || 
		To == EDialogueState::Error ||
		 To == EDialogueState::Idle); // Can cancel loading

	case EDialogueState::Active:
		// Active can go to Paused, Transitioning, Ended, or Error
		return (To == EDialogueState::Paused || 
		    To == EDialogueState::Transitioning ||
		 To == EDialogueState::Ended ||
		        To == EDialogueState::Error);

	case EDialogueState::Paused:
		// Paused can go to Active, Ended, or Error
		return (To == EDialogueState::Active || 
		  To == EDialogueState::Ended ||
		     To == EDialogueState::Error);

	case EDialogueState::Transitioning:
		// Transitioning can go to Active, Ended, or Error
		return (To == EDialogueState::Active || 
		    To == EDialogueState::Ended ||
		        To == EDialogueState::Error);

	case EDialogueState::Ended:
		// Ended can go to Idle (for restart)
		return (To == EDialogueState::Idle);

	case EDialogueState::Error:
		// Error can go to Idle (for reset)
		return (To == EDialogueState::Idle);

	default:
		return false;
	}
}

bool UDialogueStateMachine::CanPerformOperation(FName OperationName) const
{
	const TArray<FName> AllowedOps = GetAllowedOperations(CurrentState);
	return AllowedOps.Contains(OperationName);
}

TArray<FName> UDialogueStateMachine::GetAllowedOperations(EDialogueState State) const
{
	TArray<FName> AllowedOps;

	switch (State)
	{
	case EDialogueState::Idle:
		AllowedOps.Add("StartDialogue");
		break;

	case EDialogueState::Loading:
		AllowedOps.Add("CancelLoad");
		break;

	case EDialogueState::Active:
		AllowedOps.Add("GoToNode");
		AllowedOps.Add("SelectChoice");
		AllowedOps.Add("ExecuteCommand");
		AllowedOps.Add("UndoCommand");
		AllowedOps.Add("PauseDialogue");
		AllowedOps.Add("EndDialogue");
		AllowedOps.Add("Skip");
		break;

	case EDialogueState::Paused:
		AllowedOps.Add("ResumeDialogue");
		AllowedOps.Add("UndoCommand");
		AllowedOps.Add("EndDialogue");
		break;

	case EDialogueState::Transitioning:
		AllowedOps.Add("ExecuteCommand");
		// Very limited operations during transition
		break;

	case EDialogueState::Ended:
		AllowedOps.Add("StartDialogue"); // Can start a new dialogue
		break;

	case EDialogueState::Error:
		AllowedOps.Add("Reset");
		break;

	default:
		break;
	}

	return AllowedOps;
}

float UDialogueStateMachine::GetTimeInCurrentState() const
{
	const FDateTime Now = FDateTime::UtcNow();
	return static_cast<float>((Now - StateStartTime).GetTotalSeconds());
}

FDialogueStateChange UDialogueStateMachine::GetStateChangeAt(int32 Index) const
{
	if (StateHistory.IsValidIndex(Index))
	{
		return StateHistory[Index];
	}

	UE_LOG(LogDialogueStateMachine, Warning, TEXT("Invalid history index: %d"), Index);
	return FDialogueStateChange();
}

FString UDialogueStateMachine::ExportHistoryToString() const
{
	FString Result = FString::Printf(TEXT("Dialogue State History [%d changes]\n"), StateHistory.Num());
	Result += TEXT("Current State: ") + UEnum::GetValueAsString(CurrentState) + TEXT("\n");
	Result += FString::Printf(TEXT("Time in State: %.3fs\n"), GetTimeInCurrentState());
	Result += TEXT("=====================================\n");

	for (int32 i = 0; i < StateHistory.Num(); ++i)
	{
		const FDialogueStateChange& Change = StateHistory[i];

		Result += FString::Printf(TEXT("[%d] %s -> %s (%.3fs)"),
			i,
			*UEnum::GetValueAsString(Change.FromState),
			*UEnum::GetValueAsString(Change.ToState),
			Change.Duration);

		if (!Change.Reason.IsEmpty())
		{
			Result += FString::Printf(TEXT(" | Reason: %s"), *Change.Reason);
		}

		Result += TEXT("\n");
	}

	return Result;
}

void UDialogueStateMachine::ClearHistory()
{
	StateHistory.Empty();
	UE_LOG(LogDialogueStateMachine, Log, TEXT("State history cleared"));
}

void UDialogueStateMachine::Reset()
{
	if (TransitionTo(EDialogueState::Idle, TEXT("Manual Reset")))
	{
		UE_LOG(LogDialogueStateMachine, Log, TEXT("State machine reset to Idle"));
	}
}

void UDialogueStateMachine::SetMaxHistorySize(int32 InMaxSize)
{
	MaxHistorySize = FMath::Max(0, InMaxSize);
	TrimHistory();

	UE_LOG(LogDialogueStateMachine, Log, TEXT("Max history size set to: %d"), MaxHistorySize);
}

void UDialogueStateMachine::OnStateChanged(EDialogueState OldState, EDialogueState NewState)
{
	// Override in derived classes for custom behavior
}

void UDialogueStateMachine::TrimHistory()
{
	if (MaxHistorySize <= 0 || StateHistory.Num() <= MaxHistorySize)
	{
		return;
	}

	// Remove oldest entries
	const int32 NumToRemove = StateHistory.Num() - MaxHistorySize;
	StateHistory.RemoveAt(0, NumToRemove);

	UE_LOG(LogDialogueStateMachine, Verbose, TEXT("Trimmed %d state changes from history"), NumToRemove);
}
