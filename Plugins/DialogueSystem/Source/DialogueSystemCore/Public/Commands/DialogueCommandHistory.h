// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Commands/DialogueCommand.h"
#include "DialogueCommandHistory.generated.h"

/**
 * Command history entry
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueCommandHistoryEntry
{
	GENERATED_BODY()

	/** The command that was executed */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command History")
	TObjectPtr<UDialogueCommand> Command;

	/** Timestamp when command was executed */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command History")
	FDateTime Timestamp;

	/** Result of execution */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command History")
	EDialogueCommandResult Result;

	/** Index in history */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command History")
	int32 HistoryIndex;

	FDialogueCommandHistoryEntry()
		: Command(nullptr)
		, Timestamp(FDateTime::UtcNow())
		, Result(EDialogueCommandResult::Success)
		, HistoryIndex(0)
	{
	}

	FDialogueCommandHistoryEntry(UDialogueCommand* InCommand, int32 InIndex)
		: Command(InCommand)
		, Timestamp(FDateTime::UtcNow())
		, Result(InCommand ? InCommand->GetMetadata().Result : EDialogueCommandResult::Failed)
		, HistoryIndex(InIndex)
	{
	}
};

/**
 * Manages command history for dialogue session
 * Provides undo/redo functionality and command replay
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommandHistory : public UObject
{
	GENERATED_BODY()

public:
	UDialogueCommandHistory();

	/**
	 * Record a command execution
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command History")
	void RecordCommand(UDialogueCommand* Command);

	/**
	 * Undo the last command
	 * @return true if undo was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command History")
	bool Undo(UDialogueSessionContext* Context);

	/**
	 * Redo the next command in undo stack
	 * @return true if redo was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command History")
	bool Redo(UDialogueSessionContext* Context);

	/**
	 * Check if undo is available
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	bool CanUndo() const;

	/**
	 * Check if redo is available
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	bool CanRedo() const;

	/**
	 * Get command history
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	const TArray<FDialogueCommandHistoryEntry>& GetHistory() const { return History; }

	/**
	 * Get command at specific index
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	FDialogueCommandHistoryEntry GetCommandAt(int32 Index) const;

	/**
	 * Get current position in history (for undo/redo)
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	int32 GetCurrentPosition() const { return CurrentPosition; }

	/**
	 * Get total number of commands in history
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	int32 GetHistorySize() const { return History.Num(); }

	/**
	 * Clear all history
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command History")
	void ClearHistory();

	/**
	 * Replay commands from start to current position
	 * @param Context - Context to replay commands in
	 * @param bStopOnError - Stop replay if command fails
	 * @return Number of commands successfully replayed
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command History")
	int32 ReplayHistory(UDialogueSessionContext* Context, bool bStopOnError = true);

	/**
	 * Export history to string (for debugging/logging)
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	FString ExportHistoryToString() const;

	/**
	 * Get statistics about command history
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	void GetHistoryStats(int32& OutTotalCommands, int32& OutSuccessful, int32& OutFailed, float& OutTotalDuration) const;

	/**
	 * Set maximum history size (0 = unlimited)
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command History")
	void SetMaxHistorySize(int32 InMaxSize);

	/**
	 * Get maximum history size
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command History")
	int32 GetMaxHistorySize() const { return MaxHistorySize; }

protected:
	/** List of executed commands */
	UPROPERTY()
	TArray<FDialogueCommandHistoryEntry> History;

	/** Current position in history (for undo/redo) */
	UPROPERTY()
	int32 CurrentPosition;

	/** Maximum number of commands to keep in history (0 = unlimited) */
	UPROPERTY(EditDefaultsOnly, Category = "Command History")
	int32 MaxHistorySize;

	/** Trim history to max size */
	void TrimHistory();
};

/**
 * Command invoker - manages command execution with history tracking
 * This is the main entry point for executing dialogue commands
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommandInvoker : public UObject
{
	GENERATED_BODY()

public:
	UDialogueCommandInvoker();

	/**
	 * Execute a command
	 * @param Command - Command to execute
	 * @param Context - Dialogue context
	 * @param bRecordInHistory - Whether to record this command in history
	 * @return true if execution succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	bool ExecuteCommand(UDialogueCommand* Command, UDialogueSessionContext* Context, bool bRecordInHistory = true);

	/**
	 * Undo last command
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	bool UndoLastCommand(UDialogueSessionContext* Context);

	/**
	 * Redo next command
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	bool RedoCommand(UDialogueSessionContext* Context);

	/**
	 * Check if undo is available
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	bool CanUndo() const;

	/**
	 * Check if redo is available
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	bool CanRedo() const;

	/**
	 * Get command history
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	UDialogueCommandHistory* GetHistory() const { return CommandHistory; }

	/**
	 * Clear command history
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void ClearHistory();

	/**
	 * Enable/disable command recording
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetRecordingEnabled(bool bEnabled) { bIsRecordingEnabled = bEnabled; }

	/**
	 * Check if recording is enabled
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	bool IsRecordingEnabled() const { return bIsRecordingEnabled; }

	/**
	 * Get last executed command
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	UDialogueCommand* GetLastCommand() const { return LastExecutedCommand; }

	/**
	 * Export command history to string
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	FString ExportHistoryToString() const;

protected:
	/** Command history */
	UPROPERTY()
	TObjectPtr<UDialogueCommandHistory> CommandHistory;

	/** Whether command recording is enabled */
	UPROPERTY()
	bool bIsRecordingEnabled;

	/** Last executed command (for quick access) */
	UPROPERTY()
	TObjectPtr<UDialogueCommand> LastExecutedCommand;
};
