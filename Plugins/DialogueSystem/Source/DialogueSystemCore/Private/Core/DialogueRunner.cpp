// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueRunner.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueStateMachine.h"
#include "Data/DialogueDataAsset.h"
#include "Conditions/DialogueConditionEvaluator.h"
#include "Effects/DialogueEffectExecutor.h"
#include "Commands/DialogueCommandHistory.h"
#include "Commands/DialogueCommands.h"
#include "TimerManager.h"
#include "Engine/World.h"

UDialogueRunner::UDialogueRunner()
 : LoadedDialogue(nullptr)
    , CurrentContext(nullptr)
    , CurrentNode(nullptr)
    // NodeHistory not initialized (array default constructor)
    , ConditionEvaluator(nullptr)
    , EffectExecutor(nullptr)
    // NodeCache not initialized (map default constructor)
    , bIsActive(false)
    , StateMachine(nullptr)
    , CommandInvoker(nullptr)
    , bEnableCommandHistory(true)
{
    // Don't create objects here - will be created lazily when needed
}

void UDialogueRunner::StartDialogue(UDialogueDataAsset* InDialogue, const TArray<UObject*>& InParticipants)
{
    if (!InDialogue || InParticipants.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::StartDialogue - Invalid dialogue or no participants"));
        return;
    }

    // Create state machine if needed
if (!StateMachine)
    {
      StateMachine = NewObject<UDialogueStateMachine>(this);
    }

    // Check if we can start dialogue
    if (!CanPerformOperation("StartDialogue"))
  {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::StartDialogue - Cannot start in state: %s"),
            *UEnum::GetValueAsString(StateMachine->GetCurrentState()));
        return;
    }
 
    // End previous dialogue if any
    if (bIsActive)
    {
 EndDialogue();
    }

    // Transition to Loading state
    if (!StateMachine->TransitionTo(EDialogueState::Loading, TEXT("StartDialogue called")))
    {
  UE_LOG(LogTemp, Error, TEXT("DialogueRunner::StartDialogue - Failed to transition to Loading state"));
        return;
  }
    
    LoadedDialogue = InDialogue;
    
    // Create context
    CurrentContext = NewObject<UDialogueSessionContext>(this);
    if (!CurrentContext)
    {
    UE_LOG(LogTemp, Error, TEXT("DialogueRunner::StartDialogue - Failed to create context"));
        StateMachine->TransitionTo(EDialogueState::Error, TEXT("Failed to create context"));
        return;
    }
    
    // Initialize participants - use Player and NPC fields instead
    if (InParticipants.Num() > 0)
    {
        CurrentContext->SetPlayer(Cast<AActor>(InParticipants[0]));
    }
    
    if (InParticipants.Num() > 1)
    {
        CurrentContext->SetNPC(Cast<AActor>(InParticipants[1]));
    }
    
    // Create evaluators
    ConditionEvaluator = NewObject<UDialogueConditionEvaluator>(this);
    EffectExecutor = NewObject<UDialogueEffectExecutor>(this);
    
    // Create command invoker if not already created
    if (!CommandInvoker)
    {
        CommandInvoker = NewObject<UDialogueCommandInvoker>(this);
     if (CommandInvoker)
        {
       CommandInvoker->SetRecordingEnabled(bEnableCommandHistory);
        }
    }
    
    // Build node cache
 BuildNodeCache();
    
    // Find start node
    FName StartNodeId = InDialogue->StartNode;
    if (StartNodeId.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::StartDialogue - No start node defined"));
        StateMachine->TransitionTo(EDialogueState::Error, TEXT("No start node"));
        return;
    }
    
    bIsActive = true; // Keep for backward compatibility
    NodeHistory.Empty();

    // Transition to Active state
    StateMachine->TransitionTo(EDialogueState::Active, TEXT("Dialogue ready"));
    
    // Broadcast start event
 OnDialogueStarted.Broadcast(InDialogue->DialogueId);
    
    // Go to start node
  GoToNode(StartNodeId);
}

void UDialogueRunner::EndDialogue()
{
    // Check if we can end dialogue
    if (!CanPerformOperation("EndDialogue"))
    {
  UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::EndDialogue - Cannot end in state: %s"),
        StateMachine ? *UEnum::GetValueAsString(StateMachine->GetCurrentState()) : TEXT("No StateMachine"));
        return;
    }
    
    // Clear timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
    }

    // Transition to Ended state
    if (StateMachine)
    {
  StateMachine->TransitionTo(EDialogueState::Ended, TEXT("EndDialogue called"));
    }
    
    bIsActive = false; // Keep for backward compatibility
    OnDialogueEnded.Broadcast();
    
    // Cleanup
    CurrentNode = nullptr;
    NodeHistory.Empty();
    NodeCache.Empty();
    LoadedDialogue = nullptr;
    CurrentContext = nullptr;

    // Transition back to Idle
    if (StateMachine)
    {
   StateMachine->TransitionTo(EDialogueState::Idle, TEXT("Cleanup complete"));
    }
}

void UDialogueRunner::PauseDialogue()
{
    // Check if we can pause
    if (!CanPerformOperation("PauseDialogue"))
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::PauseDialogue - Cannot pause in state: %s"),
            StateMachine ? *UEnum::GetValueAsString(StateMachine->GetCurrentState()) : TEXT("No StateMachine"));
    return;
    }
    
    // Transition to Paused state
    if (StateMachine && !StateMachine->TransitionTo(EDialogueState::Paused, TEXT("PauseDialogue called")))
    {
    UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::PauseDialogue - Failed to transition to Paused state"));
        return;
    }
    
    // Clear auto-advance timer
  if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().PauseTimer(AutoAdvanceTimerHandle);
    }
}

void UDialogueRunner::ResumeDialogue()
{
    // Check if we can resume
    if (!CanPerformOperation("ResumeDialogue"))
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::ResumeDialogue - Cannot resume in state: %s"),
   StateMachine ? *UEnum::GetValueAsString(StateMachine->GetCurrentState()) : TEXT("No StateMachine"));
      return;
    }
    
    // Transition back to Active state
    if (StateMachine && !StateMachine->TransitionTo(EDialogueState::Active, TEXT("ResumeDialogue called")))
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::ResumeDialogue - Failed to transition to Active state"));
        return;
    }
    
    // Resume auto-advance timer if it was paused
    if (UWorld* World = GetWorld())
    {
 World->GetTimerManager().UnPauseTimer(AutoAdvanceTimerHandle);
    }
    
    // Re-process current node if needed
    if (CurrentNode)
    {
    // Check if we need to show choices again
        TArray<UDialogueNode*> Choices = GetAvailableChoices();
        if (Choices.Num() > 0)
        {
            OnChoicesReady.Broadcast(Choices);
        }
    }
}

bool UDialogueRunner::GoToNode(FName NodeId)
{
    // Check if we can navigate
    if (!CanPerformOperation("GoToNode"))
  {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::GoToNode - Cannot navigate in state: %s"),
   StateMachine ? *UEnum::GetValueAsString(StateMachine->GetCurrentState()) : TEXT("No StateMachine"));
        return false;
    }

    if (!LoadedDialogue || !CurrentContext)
    {
  return false;
  }
    
    TObjectPtr<UDialogueNode>* NodePtr = NodeCache.Find(NodeId);
    if (!NodePtr || !NodePtr->Get())
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner: Node '%s' not found"), *NodeId.ToString());
     return false;
    }
    
    UDialogueNode* NewNode = NodePtr->Get();
    
    // Transition to Transitioning state
    if (StateMachine)
    {
        StateMachine->TransitionTo(EDialogueState::Transitioning, FString::Printf(TEXT("Navigating to node: %s"), *NodeId.ToString()));
  }
    
    // Add to history
  if (CurrentNode)
    {
        NodeHistory.Add(CurrentNode);
    }
    
    CurrentNode = NewNode;
 
 // Mark as visited
    CurrentContext->MarkNodeVisited(NodeId);
    
    // Process node
    ProcessNode(NewNode);

    // Transition back to Active (unless dialogue ended)
 // FIXED: Don't transition back if we're no longer in Transitioning state
    // (e.g., if ProcessNode called EndDialogue for End nodes)
    if (StateMachine && StateMachine->GetCurrentState() == EDialogueState::Transitioning)
    {
  StateMachine->TransitionTo(EDialogueState::Active, TEXT("Node processed"));
    }
    
    return true;
}

bool UDialogueRunner::JumpToNode(UDialogueNode* TargetNode)
{
    if (!TargetNode)
    {
   return false;
    }
    
    return GoToNode(TargetNode->NodeId);
}

bool UDialogueRunner::SelectChoice(int32 ChoiceIndex)
{
    // Check if we can select choice
    if (!CanPerformOperation("SelectChoice"))
    {
  UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::SelectChoice - Cannot select choice in state: %s"),
            StateMachine ? *UEnum::GetValueAsString(StateMachine->GetCurrentState()) : TEXT("No StateMachine"));
   return false;
    }

    if (!CurrentNode)
    {
        return false;
    }
    
    TArray<UDialogueNode*> Choices = GetAvailableChoices();
if (!Choices.IsValidIndex(ChoiceIndex))
    {
      return false;
    }
    
    UDialogueNode* SelectedChoice = Choices[ChoiceIndex];
    
    // Broadcast selection event
    OnChoiceSelected.Broadcast(ChoiceIndex, SelectedChoice);
    
    // Apply choice effects
    ApplyNodeEffects(SelectedChoice);
    
    // Navigate to next node
    UDialogueNode* NextNode = ComputeNextNode(SelectedChoice);
    if (NextNode)
 {
        return GoToNode(NextNode->NodeId);
    }
    else
    {
        // End dialogue if no next node
        EndDialogue();
        return true;
    }
}

bool UDialogueRunner::GoBack()
{
    if (!bIsActive || NodeHistory.Num() == 0)
    {
        return false;
    }
    
    UDialogueNode* PreviousNode = NodeHistory.Pop();
    CurrentNode = PreviousNode;
  
    ProcessNode(PreviousNode);

    return true;
}

void UDialogueRunner::Skip()
{
    // Check if we can skip
    if (!CanPerformOperation("Skip"))
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::Skip - Cannot skip in state: %s"),
            StateMachine ? *UEnum::GetValueAsString(StateMachine->GetCurrentState()) : TEXT("No StateMachine"));
        return;
    }

    if (!CurrentNode)
    {
        return;
    }
    
    // Clear auto-advance timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
    }
    
    // Special handling for End nodes - close dialogue when player continues
    if (CurrentNode->NodeType == EDialogueNodeType::End)
    {
      EndDialogue();
        return;
    }
    
    // FIXED: Check for player choices FIRST before auto-advancing
    // If there are choices, we should ALREADY be showing them (from ProcessNode)
    // Don't broadcast again - that would create duplicate choice buttons!
    TArray<UDialogueNode*> Choices = GetAvailableChoices();
    if (Choices.Num() > 0)
    {
     // Choices exist but Skip was called - this means player tried to continue
     // while choices are shown. This is invalid - just ignore the Skip call.
        // The player should select a choice instead.
   UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::Skip - Cannot skip while choices are available. Player must select a choice."));
        return;
    }
    
    // No choices - compute next node based on current node type
    UDialogueNode* NextNode = ComputeNextNode(CurrentNode);
    
    if (NextNode)
 {
        // If we have a single next node, go to it
        GoToNode(NextNode->NodeId);
}
    else
    {
   // No next node and no choices - end dialogue
        EndDialogue();
    }
}

//==============================================================================
// Command Helpers (v1.2)
//==============================================================================

UDialogueCommand* UDialogueRunner::CreateNavigateCommand(UDialogueNode* TargetNode)
{
    UDialogueCommand_NavigateToNode* Cmd = NewObject<UDialogueCommand_NavigateToNode>(this);
    Cmd->SetTargetNode(TargetNode);
 return Cmd;
}

UDialogueCommand* UDialogueRunner::CreateApplyEffectsCommand(const TArray<UDialogueEffect*>& Effects)
{
    UDialogueCommand_ApplyEffects* Cmd = NewObject<UDialogueCommand_ApplyEffects>(this);
    Cmd->SetEffects(Effects);
    return Cmd;
}

UDialogueCommand* UDialogueRunner::CreateSelectChoiceCommand(int32 ChoiceIndex)
{
    UDialogueCommand_SelectChoice* Cmd = NewObject<UDialogueCommand_SelectChoice>(this);
    Cmd->SetChoiceIndex(ChoiceIndex);
    return Cmd;
}

//==============================================================================
// State Machine Integration (v1.3)
//==============================================================================

bool UDialogueRunner::IsActive() const
{
    // Deprecated - use GetCurrentState() instead
    if (StateMachine)
    {
        const EDialogueState State = StateMachine->GetCurrentState();
        return (State == EDialogueState::Active || 
     State == EDialogueState::Transitioning ||
         State == EDialogueState::Paused);
    }

    return bIsActive; // Fallback to old bool
}

EDialogueState UDialogueRunner::GetCurrentState() const
{
    if (StateMachine)
    {
        return StateMachine->GetCurrentState();
    }

    // Fallback if no state machine
    return bIsActive ? EDialogueState::Active : EDialogueState::Idle;
}

bool UDialogueRunner::CanPerformOperation(FName OperationName) const
{
 if (!StateMachine)
    {
        // If no state machine, use old behavior
     if (OperationName == "StartDialogue")
  {
      return !bIsActive;
        }
        return bIsActive;
    }

    return StateMachine->CanPerformOperation(OperationName);
}

void UDialogueRunner::OnStateMachineStateChanged(EDialogueState OldState, EDialogueState NewState)
{
    // Broadcast state change event
    OnStateChanged.Broadcast(OldState, NewState);

    // Log state change
    UE_LOG(LogTemp, Log, TEXT("DialogueRunner: State changed %s -> %s"),
     *UEnum::GetValueAsString(OldState),
 *UEnum::GetValueAsString(NewState));
}

//==============================================================================
// Internal Logic (restored from original)
//==============================================================================

TArray<UDialogueNode*> UDialogueRunner::GetAvailableChoices() const
{
    return GatherChoices(CurrentNode);
}

void UDialogueRunner::BuildNodeCache()
{
    NodeCache.Empty();
    
    if (!LoadedDialogue)
    {
        return;
    }
    
    for (UDialogueNode* Node : LoadedDialogue->Nodes)
    {
        if (Node)
        {
         NodeCache.Add(Node->NodeId, Node);
        }
 }
}

void UDialogueRunner::ProcessNode(UDialogueNode* Node)
{
  if (!Node || !CurrentContext)
    {
  return;
  }
    
 // Apply node effects
    ApplyNodeEffects(Node);
    
    // Handle special node types
    switch (Node->NodeType)
    {
    case EDialogueNodeType::Random:
        {
     UDialogueNode* RandomNext = ProcessRandomNode(Node);
     if (RandomNext)
  {
       GoToNode(RandomNext->NodeId);
     return;
      }
  }
        break;
        
    case EDialogueNodeType::Condition:
  {
       UDialogueNode* ConditionalNext = ProcessConditionNode(Node);
   if (ConditionalNext)
            {
           GoToNode(ConditionalNext->NodeId);
       return;
  }
 }
        break;
   
    case EDialogueNodeType::End:
        // Broadcast event first so UI can show the end node text
        OnNodeEntered.Broadcast(Node);
        
// CRITICAL: Transition back to Active
        // This allows UI to process the End node normally (show text, wait for input)
 if (StateMachine && StateMachine->GetCurrentState() == EDialogueState::Transitioning)
      {
            StateMachine->TransitionTo(EDialogueState::Active, TEXT("End node - awaiting player confirmation"));
        }
     
        // DON'T auto-close - let UI show continue indicator
        // Player will manually close dialogue by clicking or pressing button
  // The dialogue will end when player calls Continue()
        
     // Optional: Setup auto-close timer if configured
      // This respects Node->bAutoAdvance and Node->AutoAdvanceDelay
    SetupAutoAdvance(Node);
      
        return;
  
    default:
     break;
    }
    
    // Broadcast node entered event
    OnNodeEntered.Broadcast(Node);
    
    // Check for choices
 TArray<UDialogueNode*> Choices = GetAvailableChoices();
    if (Choices.Num() > 0)
    {
     OnChoicesReady.Broadcast(Choices);
    }
    else
 {
 // Setup auto-advance if no choices
SetupAutoAdvance(Node);
  }
}

void UDialogueRunner::ApplyNodeEffects(UDialogueNode* Node)
{
if (!Node || !EffectExecutor || !CurrentContext)
    {
   return;
    }
    
    // Execute node effects if any
    if (Node->Effects.Num() > 0)
    {
        EffectExecutor->ExecuteEffects(Node->Effects, CurrentContext);
    }
}

UDialogueNode* UDialogueRunner::ComputeNextNode(UDialogueNode* FromNode)
{
    if (!FromNode || !LoadedDialogue)
    {
        return nullptr;
    }
    
    // Get connections from this node
    for (const FDialogueConnection& Connection : FromNode->Connections)
    {
      if (IsConnectionAvailable(Connection))
        {
  if (TObjectPtr<UDialogueNode>* TargetNode = NodeCache.Find(Connection.TargetNodeId))
     {
     return TargetNode->Get();
       }
        }
    }
    
    return nullptr;
}

TArray<UDialogueNode*> UDialogueRunner::GatherChoices(UDialogueNode* FromNode) const
{
    TArray<UDialogueNode*> Result;
    
    if (!FromNode || !LoadedDialogue || !ConditionEvaluator || !CurrentContext)
    {
        return Result;
    }
    
    // Get connections from this node
    for (const FDialogueConnection& Connection : FromNode->Connections)
    {
   if (IsConnectionAvailable(Connection))
        {
            if (const TObjectPtr<UDialogueNode>* TargetNode = NodeCache.Find(Connection.TargetNodeId))
            {
                // Only include player choice nodes
   if (TargetNode->Get() && TargetNode->Get()->NodeType == EDialogueNodeType::PlayerChoice)
            {
               Result.Add(TargetNode->Get());
   }
            }
        }
    }
    
    return Result;
}

UDialogueNode* UDialogueRunner::ProcessRandomNode(UDialogueNode* RandomNode)
{
    if (!RandomNode || !LoadedDialogue)
    {
   return nullptr;
    }
    
    // Filter available connections
    TArray<FDialogueConnection> AvailableConnections;
    for (const FDialogueConnection& Connection : RandomNode->Connections)
    {
  if (IsConnectionAvailable(Connection))
        {
   AvailableConnections.Add(Connection);
     }
    }
    
    if (AvailableConnections.Num() == 0)
    {
        return nullptr;
    }
    
    // Select random connection
    int32 RandomIndex = FMath::RandRange(0, AvailableConnections.Num() - 1);
    if (TObjectPtr<UDialogueNode>* TargetNode = NodeCache.Find(AvailableConnections[RandomIndex].TargetNodeId))
    {
        return TargetNode->Get();
    }
    
    return nullptr;
}

UDialogueNode* UDialogueRunner::ProcessConditionNode(UDialogueNode* ConditionNode)
{
    if (!ConditionNode || !LoadedDialogue)
    {
        return nullptr;
    }
    
    // Return first connection that passes condition
    for (const FDialogueConnection& Connection : ConditionNode->Connections)
    {
        if (IsConnectionAvailable(Connection))
   {
            if (TObjectPtr<UDialogueNode>* TargetNode = NodeCache.Find(Connection.TargetNodeId))
            {
       return TargetNode->Get();
  }
        }
    }
    
    return nullptr;
}

bool UDialogueRunner::IsConnectionAvailable(const FDialogueConnection& Connection) const
{
    if (!ConditionEvaluator || !CurrentContext)
    {
        return true; // If no evaluator, allow all connections
    }
    
    // Empty condition means always available
    if (!Connection.Condition)
    {
        return true;
  }
    
    return ConditionEvaluator->EvaluateCondition(Connection.Condition, CurrentContext);
}

void UDialogueRunner::SetupAutoAdvance(UDialogueNode* Node)
{
  if (!Node)
    {
     return;
    }
    
    UWorld* World = GetWorld();
    if (!World)
{
        return;
 }
    
    // Clear existing timer
    World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
    
    // Set new timer if auto-advance is enabled
    if (Node->bAutoAdvance && Node->AutoAdvanceDelay > 0.0f)
    {
        World->GetTimerManager().SetTimer(
     AutoAdvanceTimerHandle,
   this,
        &UDialogueRunner::OnAutoAdvanceTimer,
            Node->AutoAdvanceDelay,
  false
        );
    }
}

void UDialogueRunner::OnAutoAdvanceTimer()
{
    // Auto-advance to next node or show choices
    Skip();
}

//==============================================================================
// Command Pattern Implementation (v1.2)
//==============================================================================

bool UDialogueRunner::ExecuteCommand(UDialogueCommand* Command, bool bRecordInHistory)
{
    // Lazy initialize CommandInvoker
    if (!CommandInvoker)
    {
        CommandInvoker = NewObject<UDialogueCommandInvoker>(this);
        if (!CommandInvoker)
        {
 UE_LOG(LogTemp, Error, TEXT("DialogueRunner::ExecuteCommand - Failed to create CommandInvoker"));
            return false;
        }
        CommandInvoker->SetRecordingEnabled(bEnableCommandHistory);
    }

    if (!CurrentContext)
    {
   UE_LOG(LogTemp, Error, TEXT("DialogueRunner::ExecuteCommand - No active context"));
        return false;
    }

    return CommandInvoker->ExecuteCommand(Command, CurrentContext, bRecordInHistory);
}

bool UDialogueRunner::UndoLastCommand()
{
    // Lazy initialize CommandInvoker
    if (!CommandInvoker)
    {
        CommandInvoker = NewObject<UDialogueCommandInvoker>(this);
if (!CommandInvoker)
        {
       UE_LOG(LogTemp, Error, TEXT("DialogueRunner::UndoLastCommand - Failed to create CommandInvoker"));
       return false;
        }
        CommandInvoker->SetRecordingEnabled(bEnableCommandHistory);
    }

    if (!CurrentContext)
    {
    UE_LOG(LogTemp, Error, TEXT("DialogueRunner::UndoLastCommand - No active context"));
        return false;
  }

    return CommandInvoker->UndoLastCommand(CurrentContext);
}

bool UDialogueRunner::RedoCommand()
{
    // Lazy initialize CommandInvoker
    if (!CommandInvoker)
    {
     CommandInvoker = NewObject<UDialogueCommandInvoker>(this);
        if (!CommandInvoker)
 {
    UE_LOG(LogTemp, Error, TEXT("DialogueRunner::RedoCommand - Failed to create CommandInvoker"));
    return false;
        }
        CommandInvoker->SetRecordingEnabled(bEnableCommandHistory);
 }

    if (!CurrentContext)
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueRunner::RedoCommand - No active context"));
        return false;
    }

    return CommandInvoker->RedoCommand(CurrentContext);
}

bool UDialogueRunner::CanUndo() const
{
if (!CommandInvoker)
    {
        return false;
    }

    return CommandInvoker->CanUndo();
}

bool UDialogueRunner::CanRedo() const
{
    if (!CommandInvoker)
{
        return false;
    }

    return CommandInvoker->CanRedo();
}

FString UDialogueRunner::GetCommandHistoryAsString() const
{
    if (!CommandInvoker)
    {
        return TEXT("No command history available (invoker not initialized yet)");
    }

    return CommandInvoker->ExportHistoryToString();
}

void UDialogueRunner::ClearCommandHistory()
{
    if (CommandInvoker)
  {
        CommandInvoker->ClearHistory();
    }
}

void UDialogueRunner::SetCommandRecordingEnabled(bool bEnabled)
{
  bEnableCommandHistory = bEnabled;

    if (CommandInvoker)
    {
        CommandInvoker->SetRecordingEnabled(bEnabled);
    }
}
