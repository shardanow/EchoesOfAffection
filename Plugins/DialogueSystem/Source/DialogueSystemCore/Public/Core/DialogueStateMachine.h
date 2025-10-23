// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogueStateMachine.generated.h"

/**
 * Dialogue state enumeration
 * Represents all possible states of a dialogue session
 */
UENUM(BlueprintType)
enum class EDialogueState : uint8
{
	/** Dialogue is not running */
	Idle           UMETA(DisplayName = "Idle"),
	
	/** Loading dialogue asset asynchronously */
	Loading        UMETA(DisplayName = "Loading"),
	
	/** Dialogue is active and running */
	Active      UMETA(DisplayName = "Active"),
	
	/** Dialogue is temporarily paused */
	Paused         UMETA(DisplayName = "Paused"),
	
	/** Transitioning between dialogue nodes */
	Transitioning  UMETA(DisplayName = "Transitioning"),
	
	/** Dialogue has ended */
	Ended        UMETA(DisplayName = "Ended"),
	
	/** Error state */
	Error          UMETA(DisplayName = "Error")
};

/**
 * Record of a state change
 * Used for debugging and history tracking
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueStateChange
{
	GENERATED_BODY()

	/** Previous state */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|State")
	EDialogueState FromState;

	/** New state */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|State")
	EDialogueState ToState;

	/** When the transition occurred */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|State")
	FDateTime Timestamp;

	/** How long we were in the previous state */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|State")
	float Duration;

	/** Reason for transition (optional, for debugging) */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|State")
	FString Reason;

	FDialogueStateChange()
		: FromState(EDialogueState::Idle)
		, ToState(EDialogueState::Idle)
		, Timestamp(FDateTime::UtcNow())
		, Duration(0.0f)
	{
	}

	FDialogueStateChange(EDialogueState InFrom, EDialogueState InTo, float InDuration, const FString& InReason = TEXT(""))
		: FromState(InFrom)
		, ToState(InTo)
		, Timestamp(FDateTime::UtcNow())
		, Duration(InDuration)
		, Reason(InReason)
	{
	}
};

/**
 * State machine for dialogue system
 * Manages state transitions and validates operations
 * 
 * Features:
 * - State validation
 * - Transition history
 * - Operation permission checking
 * - Thread-safe state changes
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueStateMachine : public UObject
{
	GENERATED_BODY()

public:
	UDialogueStateMachine();

	/**
	 * Get current dialogue state
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	EDialogueState GetCurrentState() const { return CurrentState; }

	/**
	 * Get previous dialogue state
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	EDialogueState GetPreviousState() const { return PreviousState; }

	/**
	 * Attempt to transition to a new state
	 * @param NewState - Target state
	 * @param Reason - Reason for transition (for debugging)
	 * @return true if transition was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
	bool TransitionTo(EDialogueState NewState, const FString& Reason = TEXT(""));

	/**
	 * Check if transition to state is valid
	 * @param NewState - Target state to check
	 * @return true if transition is allowed
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	bool CanTransitionTo(EDialogueState NewState) const;

	/**
	 * Check if an operation is allowed in current state
	 * @param OperationName - Name of operation to check
	 * @return true if operation is allowed
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	bool CanPerformOperation(FName OperationName) const;

	/**
	 * Get time spent in current state
	 * @return Duration in seconds
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	float GetTimeInCurrentState() const;

	/**
	 * Get state transition history
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	const TArray<FDialogueStateChange>& GetStateHistory() const { return StateHistory; }

	/**
	 * Get state change at specific index
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	FDialogueStateChange GetStateChangeAt(int32 Index) const;

	/**
	 * Export state history to string (for debugging)
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	FString ExportHistoryToString() const;

	/**
	 * Clear state history
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
	void ClearHistory();

	/**
	 * Reset to idle state
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
	void Reset();

	/**
	 * Set maximum history size
	 * @param InMaxSize - Maximum number of state changes to keep (0 = unlimited)
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
	void SetMaxHistorySize(int32 InMaxSize);

	/**
	 * Get maximum history size
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|State")
	int32 GetMaxHistorySize() const { return MaxHistorySize; }

protected:
	/**
	 * Validate if transition is allowed
	 * Override to add custom validation logic
	 */
	virtual bool IsTransitionValid(EDialogueState From, EDialogueState To) const;

	/**
	 * Called when state changes (override for custom behavior)
	 */
	virtual void OnStateChanged(EDialogueState OldState, EDialogueState NewState);

	/** Current state */
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EDialogueState CurrentState;

	/** Previous state */
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EDialogueState PreviousState;

	/** When we entered the current state */
	UPROPERTY()
	FDateTime StateStartTime;

	/** History of state changes */
	UPROPERTY()
	TArray<FDialogueStateChange> StateHistory;

	/** Maximum history size (0 = unlimited) */
	UPROPERTY(EditDefaultsOnly, Category = "State")
	int32 MaxHistorySize;

private:
	/** Trim history to max size */
	void TrimHistory();

	/** Get allowed operations for a state */
	TArray<FName> GetAllowedOperations(EDialogueState State) const;
};
