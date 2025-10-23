// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogueCommand.generated.h"

// Forward declarations
class UDialogueSessionContext;
class UDialogueNode;

/**
 * Result of command execution
 */
UENUM(BlueprintType)
enum class EDialogueCommandResult : uint8
{
	Success     UMETA(DisplayName = "Success"),
	Failed      UMETA(DisplayName = "Failed"),
	PartialSuccess  UMETA(DisplayName = "Partial Success"),
	Cancelled       UMETA(DisplayName = "Cancelled")
};

/**
 * Command execution metadata
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialogueCommandMetadata
{
	GENERATED_BODY()

	/** Unique ID of this command execution */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command")
	FGuid ExecutionId;

	/** Timestamp when command was executed */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command")
	FDateTime Timestamp;

	/** Result of command execution */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command")
	EDialogueCommandResult Result;

	/** Error message if command failed */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command")
	FString ErrorMessage;

	/** Duration of command execution in seconds */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue|Command")
	float ExecutionDuration;

	FDialogueCommandMetadata()
		: ExecutionId(FGuid::NewGuid())
		, Timestamp(FDateTime::UtcNow())
		, Result(EDialogueCommandResult::Success)
		, ExecutionDuration(0.0f)
	{
	}
};

/**
 * Base interface for all dialogue commands
 * Implements Command Pattern for dialogue actions
 * 
 * Benefits:
 * - Undo/Redo support
 * - Command history tracking
 * - Replay functionality
 * - Better debugging and logging
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class DIALOGUESYSTEMCORE_API UDialogueCommand : public UObject
{
	GENERATED_BODY()

public:
	UDialogueCommand();

	/**
	 * Execute the command
	 * @param Context - Dialogue session context
	 * @return true if execution succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	bool Execute(UDialogueSessionContext* Context);

	/**
	 * Undo the command (reverse its effects)
	 * @param Context - Dialogue session context
	 * @return true if undo succeeded
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	bool Undo(UDialogueSessionContext* Context);

	/**
	 * Check if this command can be executed
	 * @param Context - Dialogue session context
	 * @return true if command can be executed
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	bool CanExecute(UDialogueSessionContext* Context) const;

	/**
	 * Check if this command can be undone
	 * @return true if command supports undo
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	bool CanUndo() const { return bSupportsUndo; }

	/**
	 * Get human-readable description of what this command does
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	FString GetDescription() const;

	/**
	 * Get execution metadata
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	const FDialogueCommandMetadata& GetMetadata() const { return Metadata; }

	/**
	 * Get command type name (for logging/debugging)
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	FString GetCommandTypeName() const;

protected:
	/**
	 * Override this to implement command execution logic
	 */
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context);

	/**
	 * Override this to implement undo logic
	 */
	virtual bool UndoImplementation(UDialogueSessionContext* Context);

	/**
	 * Override this to implement precondition checks
	 */
	virtual bool CanExecuteImplementation(UDialogueSessionContext* Context) const;

	/**
	 * Override this to provide custom description
	 */
	virtual FString GetDescriptionImplementation() const;

	/** Whether this command supports undo operation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Command")
	bool bSupportsUndo;

	/** Metadata about command execution */
	UPROPERTY(BlueprintReadOnly, Category = "Command")
	FDialogueCommandMetadata Metadata;

	/** Cached context for undo (if needed) */
	UPROPERTY(Transient)
	TObjectPtr<UDialogueSessionContext> CachedContext;
};

/**
 * Blueprint implementable command
 * Allows designers to create custom commands in Blueprint
 */
UCLASS(Blueprintable, BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommand_Blueprint : public UDialogueCommand
{
	GENERATED_BODY()

public:
	/** Blueprint event for execute logic */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|Command", meta = (DisplayName = "Execute"))
	bool ReceiveExecute(UDialogueSessionContext* Context);

	/** Blueprint event for undo logic */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|Command", meta = (DisplayName = "Undo"))
	bool ReceiveUndo(UDialogueSessionContext* Context);

	/** Blueprint event for can execute check */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|Command", meta = (DisplayName = "Can Execute"))
	bool ReceiveCanExecute(UDialogueSessionContext* Context) const;

	/** Blueprint event for description */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|Command", meta = (DisplayName = "Get Description"))
	FString ReceiveGetDescription() const;

protected:
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override;
	virtual bool UndoImplementation(UDialogueSessionContext* Context) override;
	virtual bool CanExecuteImplementation(UDialogueSessionContext* Context) const override;
	virtual FString GetDescriptionImplementation() const override;
};
