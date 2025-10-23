// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commands/DialogueCommand.h"
#include "Core/DialogueContext.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueCommand, Log, All);

UDialogueCommand::UDialogueCommand()
	: bSupportsUndo(false)
{
}

bool UDialogueCommand::Execute(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		UE_LOG(LogDialogueCommand, Error, TEXT("Execute - Context is null"));
		Metadata.Result = EDialogueCommandResult::Failed;
		Metadata.ErrorMessage = TEXT("Context is null");
		return false;
	}

	// Check preconditions
	if (!CanExecute(Context))
	{
		UE_LOG(LogDialogueCommand, Warning, TEXT("Execute - Preconditions not met for command: %s"), 
			*GetCommandTypeName());
		Metadata.Result = EDialogueCommandResult::Failed;
		Metadata.ErrorMessage = TEXT("Preconditions not met");
		return false;
	}

	// Record start time
	const double StartTime = FPlatformTime::Seconds();
	
	// Reset metadata
	Metadata = FDialogueCommandMetadata();
	Metadata.Timestamp = FDateTime::UtcNow();

	// Cache context for potential undo
	if (bSupportsUndo)
	{
		CachedContext = Context;
	}

	// Execute command
	const bool bSuccess = ExecuteImplementation(Context);

	// Record execution duration
	const double EndTime = FPlatformTime::Seconds();
	Metadata.ExecutionDuration = static_cast<float>(EndTime - StartTime);

	// Update result
	Metadata.Result = bSuccess ? EDialogueCommandResult::Success : EDialogueCommandResult::Failed;

	// Log execution
	UE_LOG(LogDialogueCommand, Log, TEXT("Command executed: %s | Result: %s | Duration: %.3fs"),
		*GetCommandTypeName(),
		*UEnum::GetValueAsString(Metadata.Result),
		Metadata.ExecutionDuration);

	return bSuccess;
}

bool UDialogueCommand::Undo(UDialogueSessionContext* Context)
{
	if (!bSupportsUndo)
	{
		UE_LOG(LogDialogueCommand, Warning, TEXT("Undo - Command does not support undo: %s"), 
			*GetCommandTypeName());
		return false;
	}

	if (!Context)
	{
		UE_LOG(LogDialogueCommand, Error, TEXT("Undo - Context is null"));
		return false;
	}

	// Record start time
	const double StartTime = FPlatformTime::Seconds();

	// Execute undo
	const bool bSuccess = UndoImplementation(Context);

	// Record undo duration
	const double EndTime = FPlatformTime::Seconds();
	const float UndoDuration = static_cast<float>(EndTime - StartTime);

	// Log undo
	UE_LOG(LogDialogueCommand, Log, TEXT("Command undone: %s | Success: %s | Duration: %.3fs"),
		*GetCommandTypeName(),
		bSuccess ? TEXT("true") : TEXT("false"),
		UndoDuration);

	return bSuccess;
}

bool UDialogueCommand::CanExecute(UDialogueSessionContext* Context) const
{
	if (!Context)
	{
		return false;
	}

	return CanExecuteImplementation(Context);
}

FString UDialogueCommand::GetDescription() const
{
	FString Description = GetDescriptionImplementation();
	
	if (Description.IsEmpty())
	{
		// Fallback to class name
		Description = GetCommandTypeName();
	}

	return Description;
}

FString UDialogueCommand::GetCommandTypeName() const
{
	return GetClass()->GetName();
}

// Protected virtual implementations (default behavior)

bool UDialogueCommand::ExecuteImplementation(UDialogueSessionContext* Context)
{
	// Default: do nothing
	return true;
}

bool UDialogueCommand::UndoImplementation(UDialogueSessionContext* Context)
{
	// Default: do nothing
	return true;
}

bool UDialogueCommand::CanExecuteImplementation(UDialogueSessionContext* Context) const
{
	// Default: always can execute
	return true;
}

FString UDialogueCommand::GetDescriptionImplementation() const
{
	// Default: return class name
	return GetCommandTypeName();
}

// Blueprint Command Implementation

bool UDialogueCommand_Blueprint::ExecuteImplementation(UDialogueSessionContext* Context)
{
	return ReceiveExecute(Context);
}

bool UDialogueCommand_Blueprint::UndoImplementation(UDialogueSessionContext* Context)
{
	return ReceiveUndo(Context);
}

bool UDialogueCommand_Blueprint::CanExecuteImplementation(UDialogueSessionContext* Context) const
{
	return ReceiveCanExecute(Context);
}

FString UDialogueCommand_Blueprint::GetDescriptionImplementation() const
{
	FString Description = ReceiveGetDescription();
	
	if (Description.IsEmpty())
	{
		return Super::GetDescriptionImplementation();
	}

	return Description;
}
