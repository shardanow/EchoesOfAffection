// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commands/DialogueCommandHistory.h"
#include "Core/DialogueContext.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueCommandHistory, Log, All);

//==============================================================================
// UDialogueCommandHistory
//==============================================================================

UDialogueCommandHistory::UDialogueCommandHistory()
	: CurrentPosition(0)
	, MaxHistorySize(100) // Default: keep last 100 commands
{
}

void UDialogueCommandHistory::RecordCommand(UDialogueCommand* Command)
{
	if (!Command)
	{
		UE_LOG(LogDialogueCommandHistory, Warning, TEXT("Attempted to record null command"));
		return;
	}

	// If we're not at the end of history, remove everything after current position
	// (this happens when user executes new command after undo)
	if (CurrentPosition < History.Num())
	{
		History.RemoveAt(CurrentPosition, History.Num() - CurrentPosition);
	}

	// Add new command to history
	FDialogueCommandHistoryEntry Entry(Command, History.Num());
	History.Add(Entry);

	// Move position to end
	CurrentPosition = History.Num();

	// Trim if necessary
	TrimHistory();

	UE_LOG(LogDialogueCommandHistory, Verbose, TEXT("Recorded command: %s [Index: %d]"),
		*Command->GetCommandTypeName(), Entry.HistoryIndex);
}

bool UDialogueCommandHistory::Undo(UDialogueSessionContext* Context)
{
	if (!CanUndo())
	{
		UE_LOG(LogDialogueCommandHistory, Warning, TEXT("Cannot undo - no commands available"));
		return false;
	}

	if (!Context)
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("Undo - Context is null"));
		return false;
	}

	// Get command to undo (previous command)
	const int32 UndoIndex = CurrentPosition - 1;
	FDialogueCommandHistoryEntry& Entry = History[UndoIndex];

	if (!Entry.Command)
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("Undo - Command is null at index %d"), UndoIndex);
		return false;
	}

	if (!Entry.Command->CanUndo())
	{
		UE_LOG(LogDialogueCommandHistory, Warning, TEXT("Undo - Command does not support undo: %s"),
			*Entry.Command->GetCommandTypeName());
		return false;
	}

	// Perform undo
	const bool bSuccess = Entry.Command->Undo(Context);

	if (bSuccess)
	{
		CurrentPosition--;
		UE_LOG(LogDialogueCommandHistory, Log, TEXT("Undo successful: %s [Position now: %d/%d]"),
			*Entry.Command->GetCommandTypeName(), CurrentPosition, History.Num());
	}
	else
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("Undo failed: %s"),
			*Entry.Command->GetCommandTypeName());
	}

	return bSuccess;
}

bool UDialogueCommandHistory::Redo(UDialogueSessionContext* Context)
{
	if (!CanRedo())
	{
		UE_LOG(LogDialogueCommandHistory, Warning, TEXT("Cannot redo - no commands available"));
		return false;
	}

	if (!Context)
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("Redo - Context is null"));
		return false;
	}

	// Get command to redo (next command)
	FDialogueCommandHistoryEntry& Entry = History[CurrentPosition];

	if (!Entry.Command)
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("Redo - Command is null at index %d"), CurrentPosition);
		return false;
	}

	// Re-execute command
	const bool bSuccess = Entry.Command->Execute(Context);

	if (bSuccess)
	{
		CurrentPosition++;
		UE_LOG(LogDialogueCommandHistory, Log, TEXT("Redo successful: %s [Position now: %d/%d]"),
			*Entry.Command->GetCommandTypeName(), CurrentPosition, History.Num());
	}
	else
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("Redo failed: %s"),
			*Entry.Command->GetCommandTypeName());
	}

	return bSuccess;
}

bool UDialogueCommandHistory::CanUndo() const
{
	return CurrentPosition > 0 && CurrentPosition <= History.Num();
}

bool UDialogueCommandHistory::CanRedo() const
{
	return CurrentPosition < History.Num();
}

FDialogueCommandHistoryEntry UDialogueCommandHistory::GetCommandAt(int32 Index) const
{
	if (History.IsValidIndex(Index))
	{
		return History[Index];
	}

	UE_LOG(LogDialogueCommandHistory, Warning, TEXT("GetCommandAt - Invalid index: %d"), Index);
	return FDialogueCommandHistoryEntry();
}

void UDialogueCommandHistory::ClearHistory()
{
	History.Empty();
	CurrentPosition = 0;

	UE_LOG(LogDialogueCommandHistory, Log, TEXT("History cleared"));
}

int32 UDialogueCommandHistory::ReplayHistory(UDialogueSessionContext* Context, bool bStopOnError)
{
	if (!Context)
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("ReplayHistory - Context is null"));
		return 0;
	}

	int32 SuccessfulCommands = 0;

	UE_LOG(LogDialogueCommandHistory, Log, TEXT("Replaying %d commands..."), CurrentPosition);

	for (int32 i = 0; i < CurrentPosition; ++i)
	{
		FDialogueCommandHistoryEntry& Entry = History[i];

		if (!Entry.Command)
		{
			UE_LOG(LogDialogueCommandHistory, Warning, TEXT("ReplayHistory - Null command at index %d"), i);
			if (bStopOnError)
			{
				break;
			}
			continue;
		}

		if (Entry.Command->Execute(Context))
		{
			SuccessfulCommands++;
		}
		else
		{
			UE_LOG(LogDialogueCommandHistory, Error, TEXT("ReplayHistory - Command failed: %s"),
				*Entry.Command->GetCommandTypeName());

			if (bStopOnError)
			{
				break;
			}
		}
	}

	UE_LOG(LogDialogueCommandHistory, Log, TEXT("Replay complete: %d/%d commands succeeded"),
		SuccessfulCommands, CurrentPosition);

	return SuccessfulCommands;
}

FString UDialogueCommandHistory::ExportHistoryToString() const
{
	FString Result = FString::Printf(TEXT("Command History [%d commands, position: %d]\n"), 
		History.Num(), CurrentPosition);
	Result += TEXT("=====================================\n");

	for (int32 i = 0; i < History.Num(); ++i)
	{
		const FDialogueCommandHistoryEntry& Entry = History[i];

		const FString Marker = (i < CurrentPosition) ? TEXT("?") : TEXT("?");
		const FString ResultStr = UEnum::GetValueAsString(Entry.Result);

		Result += FString::Printf(TEXT("%s [%d] %s | %s | %.3fs\n"),
			*Marker,
			i,
			Entry.Command ? *Entry.Command->GetDescription() : TEXT("NULL"),
			*ResultStr,
			Entry.Command ? Entry.Command->GetMetadata().ExecutionDuration : 0.0f);
	}

	return Result;
}

void UDialogueCommandHistory::GetHistoryStats(int32& OutTotalCommands, int32& OutSuccessful, 
	int32& OutFailed, float& OutTotalDuration) const
{
	OutTotalCommands = History.Num();
	OutSuccessful = 0;
	OutFailed = 0;
	OutTotalDuration = 0.0f;

	for (const FDialogueCommandHistoryEntry& Entry : History)
	{
		if (Entry.Command)
		{
			const FDialogueCommandMetadata& Metadata = Entry.Command->GetMetadata();
			
			if (Metadata.Result == EDialogueCommandResult::Success)
			{
				OutSuccessful++;
			}
			else if (Metadata.Result == EDialogueCommandResult::Failed)
			{
				OutFailed++;
			}

			OutTotalDuration += Metadata.ExecutionDuration;
		}
	}
}

void UDialogueCommandHistory::SetMaxHistorySize(int32 InMaxSize)
{
	MaxHistorySize = FMath::Max(0, InMaxSize);
	TrimHistory();

	UE_LOG(LogDialogueCommandHistory, Log, TEXT("Max history size set to: %d"), MaxHistorySize);
}

void UDialogueCommandHistory::TrimHistory()
{
	if (MaxHistorySize <= 0 || History.Num() <= MaxHistorySize)
	{
		return;
	}

	// Remove oldest commands
	const int32 NumToRemove = History.Num() - MaxHistorySize;
	History.RemoveAt(0, NumToRemove);

	// Adjust current position
	CurrentPosition = FMath::Max(0, CurrentPosition - NumToRemove);

	UE_LOG(LogDialogueCommandHistory, Verbose, TEXT("Trimmed %d commands from history"), NumToRemove);
}

//==============================================================================
// UDialogueCommandInvoker
//==============================================================================

UDialogueCommandInvoker::UDialogueCommandInvoker()
	: CommandHistory(nullptr)
	, bIsRecordingEnabled(true)
	, LastExecutedCommand(nullptr)
{
	// Don't create CommandHistory here - will be created lazily when needed
}

bool UDialogueCommandInvoker::ExecuteCommand(UDialogueCommand* Command, UDialogueSessionContext* Context, bool bRecordInHistory)
{
	if (!Command)
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("ExecuteCommand - Command is null"));
		return false;
	}

	if (!Context)
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("ExecuteCommand - Context is null"));
		return false;
	}

	// Lazy initialize CommandHistory
	if (!CommandHistory)
	{
		CommandHistory = NewObject<UDialogueCommandHistory>(this);
		if (!CommandHistory)
		{
			UE_LOG(LogDialogueCommandHistory, Error, TEXT("ExecuteCommand - Failed to create CommandHistory"));
			// Continue without history
		}
	}

	// Execute command
	const bool bSuccess = Command->Execute(Context);

	if (bSuccess)
	{
		// Record in history if enabled and history exists
		if (bRecordInHistory && bIsRecordingEnabled && CommandHistory)
		{
			CommandHistory->RecordCommand(Command);
		}

		// Store last executed command
		LastExecutedCommand = Command;

		UE_LOG(LogDialogueCommandHistory, Log, TEXT("Command executed successfully: %s"),
			*Command->GetCommandTypeName());
	}
	else
	{
		UE_LOG(LogDialogueCommandHistory, Error, TEXT("Command execution failed: %s"),
			*Command->GetCommandTypeName());
	}

	return bSuccess;
}

bool UDialogueCommandInvoker::UndoLastCommand(UDialogueSessionContext* Context)
{
	// Lazy initialize CommandHistory
	if (!CommandHistory)
	{
		CommandHistory = NewObject<UDialogueCommandHistory>(this);
		if (!CommandHistory)
		{
			UE_LOG(LogDialogueCommandHistory, Error, TEXT("UndoLastCommand - Failed to create CommandHistory"));
			return false;
		}
	}

	return CommandHistory->Undo(Context);
}

bool UDialogueCommandInvoker::RedoCommand(UDialogueSessionContext* Context)
{
	// Lazy initialize CommandHistory
	if (!CommandHistory)
	{
		CommandHistory = NewObject<UDialogueCommandHistory>(this);
		if (!CommandHistory)
		{
			UE_LOG(LogDialogueCommandHistory, Error, TEXT("RedoCommand - Failed to create CommandHistory"));
			return false;
		}
	}

	return CommandHistory->Redo(Context);
}

bool UDialogueCommandInvoker::CanUndo() const
{
	return CommandHistory && CommandHistory->CanUndo();
}

bool UDialogueCommandInvoker::CanRedo() const
{
	return CommandHistory && CommandHistory->CanRedo();
}

void UDialogueCommandInvoker::ClearHistory()
{
	if (CommandHistory)
	{
		CommandHistory->ClearHistory();
	}

	LastExecutedCommand = nullptr;

	UE_LOG(LogDialogueCommandHistory, Log, TEXT("Command history cleared"));
}

FString UDialogueCommandInvoker::ExportHistoryToString() const
{
	if (!CommandHistory)
	{
		return TEXT("No command history available (not initialized yet)");
	}

	return CommandHistory->ExportHistoryToString();
}
