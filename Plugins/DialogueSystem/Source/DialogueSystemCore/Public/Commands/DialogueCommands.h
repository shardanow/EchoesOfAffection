// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Commands/DialogueCommand.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueContext.h"
#include "Effects/DialogueEffectExecutor.h"
#include "DialogueCommands.generated.h"

// Forward declarations
class UDialogueNode;
class UDialogueEffectExecutor;
class UDialogueEffect;
struct FDialogueChoice;
struct FDialogueStateSnapshot;

/**
 * Command: Navigate to a specific dialogue node
 * v1.3: Now with full undo support via state snapshots
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommand_NavigateToNode : public UDialogueCommand
{
	GENERATED_BODY()

public:
	UDialogueCommand_NavigateToNode();

	/** Set the target node */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetTargetNode(UDialogueNode* InTargetNode);

	/** Get the target node */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	UDialogueNode* GetTargetNode() const { return TargetNode; }

protected:
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override;
	virtual bool UndoImplementation(UDialogueSessionContext* Context) override;
	virtual bool CanExecuteImplementation(UDialogueSessionContext* Context) const override;
	virtual FString GetDescriptionImplementation() const override;

private:
	/** Target node to navigate to */
	UPROPERTY()
	TObjectPtr<UDialogueNode> TargetNode;

	/** NEW v1.3: State snapshot before navigation (for perfect undo) */
	UPROPERTY()
	FDialogueStateSnapshot StateSnapshot;
};

/**
 * Command: Apply dialogue effects
 * v1.3: Now with full undo support via state snapshots
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommand_ApplyEffects : public UDialogueCommand
{
	GENERATED_BODY()

public:
	UDialogueCommand_ApplyEffects();

	/** Set the effects to apply */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetEffects(const TArray<UDialogueEffect*>& InEffects);

	/** Get the effects */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	const TArray<UDialogueEffect*>& GetEffects() const { return Effects; }

protected:
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override;
	virtual bool UndoImplementation(UDialogueSessionContext* Context) override;
	virtual bool CanExecuteImplementation(UDialogueSessionContext* Context) const override;
	virtual FString GetDescriptionImplementation() const override;

private:
	/** Effects to apply */
	UPROPERTY()
	TArray<TObjectPtr<UDialogueEffect>> Effects;

	/** NEW v1.3: State snapshot before effects (for perfect undo) */
	UPROPERTY()
	FDialogueStateSnapshot StateSnapshot;
};

/**
 * Command: Select a dialogue choice
 * v1.3: Now with full undo support via state snapshots
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommand_SelectChoice : public UDialogueCommand
{
	GENERATED_BODY()

public:
	UDialogueCommand_SelectChoice();

	/** Set the choice index */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetChoiceIndex(int32 InChoiceIndex);

	/** Get the choice index */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	int32 GetChoiceIndex() const { return ChoiceIndex; }

protected:
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override;
	virtual bool UndoImplementation(UDialogueSessionContext* Context) override;
	virtual bool CanExecuteImplementation(UDialogueSessionContext* Context) const override;
	virtual FString GetDescriptionImplementation() const override;

private:
	/** Index of the selected choice */
	UPROPERTY()
	int32 ChoiceIndex;

	/** NEW v1.3: State snapshot before choice (for perfect undo) */
	UPROPERTY()
	FDialogueStateSnapshot StateSnapshot;
};

/**
 * Command: Set context variable
 * Already has full undo support (v1.2)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommand_SetVariable : public UDialogueCommand
{
	GENERATED_BODY()

public:
	UDialogueCommand_SetVariable();

	/** Set variable parameters */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetVariable(FName InVariableName, int32 InValue);

	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetVariableFloat(FName InVariableName, float InValue);

	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetVariableBool(FName InVariableName, bool InValue);

	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetVariableString(FName InVariableName, const FString& InValue);

protected:
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override;
	virtual bool UndoImplementation(UDialogueSessionContext* Context) override;
	virtual FString GetDescriptionImplementation() const override;

private:
	UPROPERTY()
	FName VariableName;

	// Value storage (only one will be used based on type)
	UPROPERTY()
	int32 IntValue;

	UPROPERTY()
	float FloatValue;

	UPROPERTY()
	bool BoolValue;

	UPROPERTY()
	FString StringValue;

	// Old value (for undo)
	UPROPERTY()
	int32 OldIntValue;

	UPROPERTY()
	float OldFloatValue;

	UPROPERTY()
	bool OldBoolValue;

	UPROPERTY()
	FString OldStringValue;

	// Type tracking
	enum class EVariableType : uint8
	{
		Integer,
		Float,
		Bool,
		String
	};

	EVariableType VariableType;
};

/**
 * Command: Start dialogue session
 * No undo support (cannot undo starting dialogue)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommand_StartSession : public UDialogueCommand
{
	GENERATED_BODY()

public:
	UDialogueCommand_StartSession();

	/** Set dialogue asset */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetDialogueAsset(class UDialogueDataAsset* InAsset);

	/** Set participants */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void SetParticipants(const TArray<UObject*>& InParticipants);

protected:
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override;
	virtual bool CanExecuteImplementation(UDialogueSessionContext* Context) const override;
	virtual FString GetDescriptionImplementation() const override;

private:
	UPROPERTY()
	TObjectPtr<class UDialogueDataAsset> DialogueAsset;

	UPROPERTY()
	TArray<TObjectPtr<UObject>> Participants;
};

/**
 * Command: End dialogue session
 * No undo support (cannot undo ending dialogue)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommand_EndSession : public UDialogueCommand
{
	GENERATED_BODY()

public:
	UDialogueCommand_EndSession();

protected:
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override;
	virtual FString GetDescriptionImplementation() const override;
};

/**
 * Composite command: Execute multiple commands in sequence
 * Full undo support (v1.2)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCommand_Composite : public UDialogueCommand
{
	GENERATED_BODY()

public:
	UDialogueCommand_Composite();

	/** Add a sub-command */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void AddCommand(UDialogueCommand* Command);

	/** Get all sub-commands */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Command")
	const TArray<UDialogueCommand*>& GetCommands() const { return Commands; }

	/** Clear all commands */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Command")
	void ClearCommands();

protected:
	virtual bool ExecuteImplementation(UDialogueSessionContext* Context) override;
	virtual bool UndoImplementation(UDialogueSessionContext* Context) override;
	virtual bool CanExecuteImplementation(UDialogueSessionContext* Context) const override;
	virtual FString GetDescriptionImplementation() const override;

private:
	/** List of commands to execute in sequence */
	UPROPERTY()
	TArray<TObjectPtr<UDialogueCommand>> Commands;

	/** Number of commands that were successfully executed (for partial undo) */
	int32 ExecutedCommandCount;
};
