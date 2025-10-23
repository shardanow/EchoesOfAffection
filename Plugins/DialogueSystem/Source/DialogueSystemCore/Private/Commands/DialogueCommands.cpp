// Copyright Epic Games, Inc. All Rights Reserved.

#include "Commands/DialogueCommands.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueStateMachine.h"
#include "Data/DialogueDataAsset.h"
#include "Effects/DialogueEffectExecutor.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueCommands, Log, All);

//==============================================================================
// UDialogueCommand_NavigateToNode (v1.3 - Full Undo Support)
//==============================================================================

UDialogueCommand_NavigateToNode::UDialogueCommand_NavigateToNode()
{
	bSupportsUndo = true; // v1.3: Now supports full undo via snapshots
}

void UDialogueCommand_NavigateToNode::SetTargetNode(UDialogueNode* InTargetNode)
{
	TargetNode = InTargetNode;
}

bool UDialogueCommand_NavigateToNode::ExecuteImplementation(UDialogueSessionContext* Context)
{
	if (!TargetNode)
	{
		UE_LOG(LogDialogueCommands, Error, TEXT("NavigateToNode - Target node is null"));
		return false;
	}

	// v1.3: Create snapshot BEFORE navigation
	StateSnapshot = Context->CreateSnapshot(
		(uint8)EDialogueState::Active,
		TArray<UDialogueNode*>() // Empty for now, will be filled by runner
	);

	if (!StateSnapshot.IsValid())
	{
		UE_LOG(LogDialogueCommands, Warning, TEXT("NavigateToNode - Failed to create snapshot, undo may not work perfectly"));
	}

	// Mark node as visited
	Context->MarkNodeVisited(TargetNode->NodeId);

	// Set current node
	Context->SetCurrentNode(TargetNode);

	UE_LOG(LogDialogueCommands, Log, TEXT("Navigated to '%s' (snapshot created: %d bytes)"),
		*TargetNode->NodeId.ToString(), StateSnapshot.GetMemorySize());

	return true;
}

bool UDialogueCommand_NavigateToNode::UndoImplementation(UDialogueSessionContext* Context)
{
	// v1.3: Restore from snapshot
	if (!StateSnapshot.IsValid())
	{
		UE_LOG(LogDialogueCommands, Error, TEXT("NavigateToNode::Undo - No valid snapshot available"));
		return false;
	}

	if (Context->RestoreFromSnapshot(StateSnapshot))
	{
		UE_LOG(LogDialogueCommands, Log, TEXT("NavigateToNode undone successfully"));
		return true;
	}

	UE_LOG(LogDialogueCommands, Error, TEXT("NavigateToNode::Undo - Failed to restore from snapshot"));
	return false;
}

bool UDialogueCommand_NavigateToNode::CanExecuteImplementation(UDialogueSessionContext* Context) const
{
	return TargetNode != nullptr;
}

FString UDialogueCommand_NavigateToNode::GetDescriptionImplementation() const
{
	if (TargetNode)
	{
		return FString::Printf(TEXT("Navigate to node '%s'"), *TargetNode->NodeId.ToString());
	}
	return TEXT("Navigate to node (no target set)");
}

//==============================================================================
// UDialogueCommand_ApplyEffects (v1.3 - Full Undo Support)
//==============================================================================

UDialogueCommand_ApplyEffects::UDialogueCommand_ApplyEffects()
{
	bSupportsUndo = true; // v1.3: Now supports full undo via snapshots
}

void UDialogueCommand_ApplyEffects::SetEffects(const TArray<UDialogueEffect*>& InEffects)
{
	Effects.Empty();
	for (UDialogueEffect* Effect : InEffects)
	{
		Effects.Add(Effect);
	}
}

bool UDialogueCommand_ApplyEffects::ExecuteImplementation(UDialogueSessionContext* Context)
{
	if (Effects.Num() == 0)
	{
		UE_LOG(LogDialogueCommands, Warning, TEXT("ApplyEffects - No effects to apply"));
		return true; // Not an error, just nothing to do
	}

	// v1.3: Create snapshot BEFORE applying effects
	StateSnapshot = Context->CreateSnapshot(
		(uint8)EDialogueState::Active,
		TArray<UDialogueNode*>()
	);

	if (!StateSnapshot.IsValid())
	{
		UE_LOG(LogDialogueCommands, Warning, TEXT("ApplyEffects - Failed to create snapshot, undo may not work perfectly"));
	}

	// Apply effects using the effect executor
	UDialogueEffectExecutor* Executor = NewObject<UDialogueEffectExecutor>();
	for (UDialogueEffect* Effect : Effects)
	{
		if (Effect)
		{
			Executor->ExecuteEffect(Effect, Context);
		}
	}

	UE_LOG(LogDialogueCommands, Log, TEXT("Applied %d effects (snapshot: %d bytes)"), 
		Effects.Num(), StateSnapshot.GetMemorySize());

	return true;
}

bool UDialogueCommand_ApplyEffects::UndoImplementation(UDialogueSessionContext* Context)
{
	// v1.3 Phase 5: HYBRID APPROACH - Try reversal first, then fallback to snapshot
	
	// Check if all effects support reversal
	bool bAllReversible = true;
	for (const TObjectPtr<UDialogueEffect>& Effect : Effects)
	{
		if (!Effect || !Effect->SupportsReverse())
		{
			bAllReversible = false;
			break;
		}
	}
	
	if (bAllReversible)
	{
		// Use effect reversal (fast and efficient!)
		UDialogueEffectExecutor* Executor = NewObject<UDialogueEffectExecutor>();
		Executor->ReverseEffects(Effects, Context);
		
		UE_LOG(LogDialogueCommands, Log, TEXT("ApplyEffects undone via reversal (%d effects)"), Effects.Num());
		return true;
	}
	else
	{
		// Fallback to snapshot restoration
		if (!StateSnapshot.IsValid())
		{
			UE_LOG(LogDialogueCommands, Error, TEXT("ApplyEffects::Undo - No valid snapshot available and effects not reversible"));
			return false;
		}

		if (Context->RestoreFromSnapshot(StateSnapshot))
		{
			UE_LOG(LogDialogueCommands, Log, TEXT("ApplyEffects undone via snapshot (some effects not reversible)"));
			return true;
		}

		UE_LOG(LogDialogueCommands, Error, TEXT("ApplyEffects::Undo - Failed to restore from snapshot"));
		return false;
	}
}

bool UDialogueCommand_ApplyEffects::CanExecuteImplementation(UDialogueSessionContext* Context) const
{
	return Effects.Num() > 0;
}

FString UDialogueCommand_ApplyEffects::GetDescriptionImplementation() const
{
	return FString::Printf(TEXT("Apply %d dialogue effects"), Effects.Num());
}

//==============================================================================
// UDialogueCommand_SelectChoice (v1.3 - Full Undo Support)
//==============================================================================

UDialogueCommand_SelectChoice::UDialogueCommand_SelectChoice()
	: ChoiceIndex(-1)
{
	bSupportsUndo = true; // v1.3: Now supports full undo via snapshots
}

void UDialogueCommand_SelectChoice::SetChoiceIndex(int32 InChoiceIndex)
{
	ChoiceIndex = InChoiceIndex;
}

bool UDialogueCommand_SelectChoice::ExecuteImplementation(UDialogueSessionContext* Context)
{
	// v1.3: Create snapshot BEFORE selecting choice
	StateSnapshot = Context->CreateSnapshot(
		(uint8)EDialogueState::Active,
		TArray<UDialogueNode*>()
	);

	if (!StateSnapshot.IsValid())
	{
		UE_LOG(LogDialogueCommands, Warning, TEXT("SelectChoice - Failed to create snapshot, undo may not work perfectly"));
	}

	// For now, just mark as executed
	// Actual choice logic should be handled by DialogueRunner

	UE_LOG(LogDialogueCommands, Log, TEXT("Selected choice %d (snapshot: %d bytes)"), 
		ChoiceIndex, StateSnapshot.GetMemorySize());

	return true;
}

bool UDialogueCommand_SelectChoice::UndoImplementation(UDialogueSessionContext* Context)
{
	// v1.3: Restore from snapshot
	if (!StateSnapshot.IsValid())
	{
		UE_LOG(LogDialogueCommands, Error, TEXT("SelectChoice::Undo - No valid snapshot available"));
		return false;
	}

	if (Context->RestoreFromSnapshot(StateSnapshot))
	{
		UE_LOG(LogDialogueCommands, Log, TEXT("SelectChoice undone successfully"));
		return true;
	}

	UE_LOG(LogDialogueCommands, Error, TEXT("SelectChoice::Undo - Failed to restore from snapshot"));
	return false;
}

bool UDialogueCommand_SelectChoice::CanExecuteImplementation(UDialogueSessionContext* Context) const
{
	return ChoiceIndex >= 0;
}

FString UDialogueCommand_SelectChoice::GetDescriptionImplementation() const
{
	return FString::Printf(TEXT("Select dialogue choice %d"), ChoiceIndex);
}

//==============================================================================
// UDialogueCommand_SetVariable
//==============================================================================

UDialogueCommand_SetVariable::UDialogueCommand_SetVariable()
	: IntValue(0)
	, FloatValue(0.0f)
	, BoolValue(false)
	, OldIntValue(0)
	, OldFloatValue(0.0f)
	, OldBoolValue(false)
	, VariableType(EVariableType::String) // Default to string
{
	bSupportsUndo = true;
}

void UDialogueCommand_SetVariable::SetVariable(FName InVariableName, int32 InValue)
{
	VariableName = InVariableName;
	IntValue = InValue;
	StringValue = FString::FromInt(InValue);
	VariableType = EVariableType::Integer;
}

void UDialogueCommand_SetVariable::SetVariableFloat(FName InVariableName, float InValue)
{
	VariableName = InVariableName;
	FloatValue = InValue;
	StringValue = FString::SanitizeFloat(InValue);
	VariableType = EVariableType::Float;
}

void UDialogueCommand_SetVariable::SetVariableBool(FName InVariableName, bool InValue)
{
	VariableName = InVariableName;
	BoolValue = InValue;
	StringValue = InValue ? TEXT("true") : TEXT("false");
	VariableType = EVariableType::Bool;
}

void UDialogueCommand_SetVariable::SetVariableString(FName InVariableName, const FString& InValue)
{
	VariableName = InVariableName;
	StringValue = InValue;
	VariableType = EVariableType::String;
}

bool UDialogueCommand_SetVariable::ExecuteImplementation(UDialogueSessionContext* Context)
{
	// Store old value for undo (using CustomVariables API)
	OldStringValue = Context->GetCustomVariable(VariableName);

	// Set new value
	Context->SetCustomVariable(VariableName, StringValue);

	UE_LOG(LogDialogueCommands, Log, TEXT("Set variable '%s' = '%s' (was '%s')"),
		*VariableName.ToString(), *StringValue, *OldStringValue);

	return true;
}

bool UDialogueCommand_SetVariable::UndoImplementation(UDialogueSessionContext* Context)
{
	// Restore old value
	Context->SetCustomVariable(VariableName, OldStringValue);

	UE_LOG(LogDialogueCommands, Log, TEXT("Reverted variable '%s'"), *VariableName.ToString());

	return true;
}

FString UDialogueCommand_SetVariable::GetDescriptionImplementation() const
{
	return FString::Printf(TEXT("Set variable '%s' = '%s'"), 
		*VariableName.ToString(), *StringValue);
}

//==============================================================================
// UDialogueCommand_StartSession
//==============================================================================

UDialogueCommand_StartSession::UDialogueCommand_StartSession()
{
	bSupportsUndo = false; // Cannot undo starting a session
}

void UDialogueCommand_StartSession::SetDialogueAsset(UDialogueDataAsset* InAsset)
{
	DialogueAsset = InAsset;
}

void UDialogueCommand_StartSession::SetParticipants(const TArray<UObject*>& InParticipants)
{
	Participants.Empty();
	for (UObject* Participant : InParticipants)
	{
		Participants.Add(Participant);
	}
}

bool UDialogueCommand_StartSession::ExecuteImplementation(UDialogueSessionContext* Context)
{
	if (!DialogueAsset)
	{
		UE_LOG(LogDialogueCommands, Error, TEXT("StartSession - DialogueAsset is null"));
		return false;
	}

	// For now, just mark as successful
	// Actual initialization should be handled by DialogueRunner
	UE_LOG(LogDialogueCommands, Log, TEXT("Started dialogue session: '%s'"),
		*DialogueAsset->DialogueId.ToString());

	return true;
}

bool UDialogueCommand_StartSession::CanExecuteImplementation(UDialogueSessionContext* Context) const
{
	return DialogueAsset != nullptr;
}

FString UDialogueCommand_StartSession::GetDescriptionImplementation() const
{
	if (DialogueAsset)
	{
		return FString::Printf(TEXT("Start dialogue '%s'"), 
			*DialogueAsset->DialogueId.ToString());
	}
	return TEXT("Start dialogue session");
}

//==============================================================================
// UDialogueCommand_EndSession
//==============================================================================

UDialogueCommand_EndSession::UDialogueCommand_EndSession()
{
	bSupportsUndo = false; // Don't support undo for ending session
}

bool UDialogueCommand_EndSession::ExecuteImplementation(UDialogueSessionContext* Context)
{
	// For now, just mark as executed
	// The actual ending logic should be handled by DialogueRunner

	UE_LOG(LogDialogueCommands, Log, TEXT("Ended dialogue session"));

	return true;
}

FString UDialogueCommand_EndSession::GetDescriptionImplementation() const
{
	return TEXT("End dialogue session");
}

//==============================================================================
// UDialogueCommand_Composite
//==============================================================================

UDialogueCommand_Composite::UDialogueCommand_Composite()
	: ExecutedCommandCount(0)
{
	bSupportsUndo = true;
}

void UDialogueCommand_Composite::AddCommand(UDialogueCommand* Command)
{
	if (Command)
	{
		Commands.Add(Command);
	}
}

void UDialogueCommand_Composite::ClearCommands()
{
	Commands.Empty();
	ExecutedCommandCount = 0;
}

bool UDialogueCommand_Composite::ExecuteImplementation(UDialogueSessionContext* Context)
{
	ExecutedCommandCount = 0;
	bool bAllSucceeded = true;

	for (UDialogueCommand* Command : Commands)
	{
		if (!Command)
		{
			UE_LOG(LogDialogueCommands, Warning, TEXT("Composite - Null command in list"));
			continue;
		}

		if (!Command->Execute(Context))
		{
			UE_LOG(LogDialogueCommands, Error, TEXT("Composite - Command failed: %s"),
				*Command->GetCommandTypeName());
			bAllSucceeded = false;
			break;
		}

		ExecutedCommandCount++;
	}

	UE_LOG(LogDialogueCommands, Log, TEXT("Composite executed %d/%d commands"),
		ExecutedCommandCount, Commands.Num());

	return bAllSucceeded;
}

bool UDialogueCommand_Composite::UndoImplementation(UDialogueSessionContext* Context)
{
	// Undo in reverse order, only up to ExecutedCommandCount
	for (int32 i = ExecutedCommandCount - 1; i >= 0; --i)
	{
		if (UDialogueCommand* Command = Commands[i])
		{
			if (Command->CanUndo())
			{
				Command->Undo(Context);
			}
		}
	}

	UE_LOG(LogDialogueCommands, Log, TEXT("Composite undone: %d commands"), ExecutedCommandCount);

	ExecutedCommandCount = 0;
	return true;
}

bool UDialogueCommand_Composite::CanExecuteImplementation(UDialogueSessionContext* Context) const
{
	// All sub-commands must be executable
	for (const UDialogueCommand* Command : Commands)
	{
		if (!Command || !Command->CanExecute(Context))
		{
			return false;
		}
	}

	return Commands.Num() > 0;
}

FString UDialogueCommand_Composite::GetDescriptionImplementation() const
{
	return FString::Printf(TEXT("Execute %d commands"), Commands.Num());
}
