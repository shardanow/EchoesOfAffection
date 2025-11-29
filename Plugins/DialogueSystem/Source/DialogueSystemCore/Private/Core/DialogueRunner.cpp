// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueRunner.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueStateMachine.h"
#include "Data/DialogueDataAsset.h"
#include "Conditions/DialogueConditionEvaluator.h"
#include "Effects/DialogueEffectExecutor.h"
#include "Effects/DialogueEffect_PlaySequence.h" // NEW v1.13: For sequence cleanup
#include "Effects/DialogueEffect_PositionParticipant.h" // NEW v1.15: For participant positioning
#include "Positioning/SequencePositioningExtractor.h" // NEW v1.16.8: For automatic position extraction
#include "Commands/DialogueCommandHistory.h"
#include "Commands/DialogueCommands.h"
#include "Interfaces/IDialogueParticipant.h" // NEW v1.16.2: For Execute_GetParticipantId
#include "TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h" // NEW v1.16.2: For TActorIterator

// GameEventBus integration (optional)
#if WITH_GAMEEVENTBUS
#include "GameEventBusSubsystem.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogDialogueRunner, Log, All);

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
    
    // ? NEW v1.14: Set owning runner for sequence management
    CurrentContext->SetOwningRunner(this);
    UE_LOG(LogDialogueRunner, Log, TEXT("Set OwningRunner for Context: %s"), *GetName());
    
    CurrentContext->Initialize();
    
    // Initialize participants - use Player and NPC fields instead
    if (InParticipants.Num() > 0)
    {
        CurrentContext->SetPlayer(Cast<AActor>(InParticipants[0]));
    }
    
    if (InParticipants.Num() > 1)
    {
        CurrentContext->SetNPC(Cast<AActor>(InParticipants[1]));
    }
    
    // NEW v1.18.1: Register PersonaIds immediately after setting participants
    // This ensures turn-to-face system can resolve actors by PersonaId
    {
        AActor* Player = CurrentContext->GetPlayer();
        AActor* PrimaryNPC = CurrentContext->GetNPC();
        
        // Auto-resolve Additional Personas from DialogueData
        TMap<FName, AActor*> AdditionalNPCs;
        if (InDialogue && InDialogue->AdditionalPersonas.Num() > 0)
      {
   UWorld* World = GetWorld();
            if (World)
       {
    for (const auto& Pair : InDialogue->AdditionalPersonas)
      {
        FName PersonaId = Pair.Value.PersonaId;
      if (!PersonaId.IsNone())
         {
       AActor* FoundActor = FindActorByPersonaId(World, PersonaId);
         if (FoundActor)
            {
     AdditionalNPCs.Add(PersonaId, FoundActor);
       UE_LOG(LogDialogueRunner, Log, TEXT("StartDialogue: Auto-resolved Additional Persona '%s' -> '%s'"),
 *PersonaId.ToString(), *FoundActor->GetName());
    }
   }
       }
}
      }
        
        RegisterPersonasFromDialogueData(Player, PrimaryNPC, AdditionalNPCs);
     
   UE_LOG(LogDialogueRunner, Log, TEXT("StartDialogue: Registered %d personas for turn-to-face system"), 
     CurrentContext->GetParticipants() ? CurrentContext->GetParticipants()->GetAllPersonaIds().Num() : 0);
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
    
    // ? NEW v1.14: Emit GameEventBus event for ActorScheduleSystem integration
    EmitDialogueStartedEvent(InDialogue->DialogueId, InParticipants);
    
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
    
    // ? NEW v1.14: Stop active sequence FIRST (before broadcasting events)
    // This ensures sequence stops and camera restores BEFORE dialogue cleanup
  UE_LOG(LogDialogueRunner, Warning, TEXT("[DIALOGUE] EndDialogue called"));
    UE_LOG(LogDialogueRunner, Warning, TEXT("[DIALOGUE]   ActiveSequenceEffect: %s"), ActiveSequenceEffect ? TEXT("VALID") : TEXT("NULL"));
    
    if (ActiveSequenceEffect)
    {
        UE_LOG(LogDialogueRunner, Warning, TEXT("[DIALOGUE] Stopping active sequence on dialogue end"));
 ActiveSequenceEffect->StopSequence();
        ActiveSequenceEffect = nullptr;
  }
    else
    {
        UE_LOG(LogDialogueRunner, Warning, TEXT("[DIALOGUE] No active sequence to stop"));
    }
    
    // Broadcast end event (legacy)
    OnDialogueEnded.Broadcast();

    // ? NEW v1.14: Emit GameEventBus event for ActorScheduleSystem integration
    if (LoadedDialogue)
    {
        EmitDialogueEndedEvent(LoadedDialogue->DialogueId);
    }
    
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
    
    // ===== NEW v1.16.9 FIX: Execute positioning logic (handles both manual & auto) =====
    // Always call ExecuteNodePositioning - it checks bEnablePositioning internally
    if (Node->Positioning.bPositionBeforeEffects)
    {
     ExecuteNodePositioning(Node);  // Handles manual OR auto-extraction
    }
    
 // Apply node effects
    ApplyNodeEffects(Node);
    
    // ===== NEW v1.16.9 FIX: Position AFTER effects if configured =====
    if (!Node->Positioning.bPositionBeforeEffects)
    {
        ExecuteNodePositioning(Node);  // Handles manual OR auto-extraction
 }
 
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
        
        // NEW v1.17.1: Emit GameEventBus event for camera
        EmitNodeEnteredEvent(Node);
        
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
	
	// NEW v1.17.1: Emit GameEventBus event for camera system
	EmitNodeEnteredEvent(Node);
	
	// NEW v1.18: Automatic turn-to-face if ListenerId is set
	EmitTurnToFaceEvent(Node);
	
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

//==============================================================================
// Sequence Integration (v1.13)
//==============================================================================

void UDialogueRunner::RegisterActiveSequence(UDialogueEffect_PlaySequence* SequenceEffect)
{
    ActiveSequenceEffect = SequenceEffect;
}

void UDialogueRunner::ClearActiveSequence()
{
 ActiveSequenceEffect = nullptr;
}

//==============================================================================
// GameEventBus Integration (v1.14)
//==============================================================================

void UDialogueRunner::EmitDialogueStartedEvent(FName DialogueId, const TArray<UObject*>& Participants)
{
#if WITH_GAMEEVENTBUS
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS] Cannot emit DialogueStarted: World is null"));
        return;
    }

    UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
    if (!EventBus)
    {
        UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS] Cannot emit DialogueStarted: EventBus is null"));
    return;
    }

    // Prepare payload
    FGameEventPayload Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Started"));
  Payload.StringParam = DialogueId;

    // Set participants as instigator/target (Player and NPC only)
    if (Participants.Num() > 0)
    {
        Payload.InstigatorActor = Cast<AActor>(Participants[0]); // Player
    }
    if (Participants.Num() > 1)
    {
    Payload.TargetActor = Cast<AActor>(Participants[1]); // NPC
    }

    // ? NEW v1.14: Populate AdditionalPersonaIds from DialogueDataAsset
  UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS] ?? Checking for AdditionalPersonas..."));
    UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS]   LoadedDialogue: %s"), LoadedDialogue ? TEXT("VALID") : TEXT("NULL"));
    
if (LoadedDialogue)
    {
      UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS]   AdditionalPersonas.Num(): %d"), LoadedDialogue->AdditionalPersonas.Num());
     
        if (LoadedDialogue->AdditionalPersonas.Num() > 0)
     {
            for (const TPair<FName, FDialoguePersonaData>& Pair : LoadedDialogue->AdditionalPersonas)
       {
             FName PersonaId = Pair.Value.PersonaId;
 
      UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS]     PersonaId: '%s' (IsNone: %s)"), 
           *PersonaId.ToString(), PersonaId.IsNone() ? TEXT("YES") : TEXT("NO"));
                
                if (!PersonaId.IsNone())
  {
           Payload.AdditionalPersonaIds.Add(PersonaId.ToString());
          UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS]     ? Added PersonaId='%s' to payload"), 
    *PersonaId.ToString());
     }
       }
   }
    }

    UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS] ?? Final Payload:"));
    UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS]   DialogueId: %s"), *DialogueId.ToString());
    UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS]   AdditionalPersonaIds.Num(): %d"), Payload.AdditionalPersonaIds.Num());
    for (int32 i = 0; i < Payload.AdditionalPersonaIds.Num(); ++i)
    {
        UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS]     [%d] = '%s'"), i, *Payload.AdditionalPersonaIds[i]);
    }

    // Emit event
    EventBus->EmitEvent(Payload, true);  // bLogEvent = true for debugging

#endif // WITH_GAMEEVENTBUS
}

//==============================================================================
// NEW v1.14: GameEventBus Integration - Dialogue Ended Event
//==============================================================================

void UDialogueRunner::EmitDialogueEndedEvent(FName DialogueId)
{
#if WITH_GAMEEVENTBUS
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS] Cannot emit DialogueEnded: World is null"));
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS] Cannot emit DialogueEnded: EventBus is null"));
		return;
	}

	// Prepare payload
	FGameEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.Ended"));
	Payload.StringParam = DialogueId;

	// Set participants if available
	if (CurrentContext)
	{
		Payload.InstigatorActor = CurrentContext->GetPlayer();
		Payload.TargetActor = CurrentContext->GetNPC();
	}
	
	// ? NEW v1.14: Populate AdditionalPersonaIds from DialogueDataAsset (SAME AS STARTED!)
	if (LoadedDialogue)
	{
		if (LoadedDialogue->AdditionalPersonas.Num() > 0)
		{
			for (const TPair<FName, FDialoguePersonaData>& Pair : LoadedDialogue->AdditionalPersonas)
			{
				FName PersonaId = Pair.Value.PersonaId;
				
				if (!PersonaId.IsNone())
				{
					Payload.AdditionalPersonaIds.Add(PersonaId.ToString());
				}
			}
		}
	}

	UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS] ?? Emitting Dialogue.Ended: DialogueId='%s', AdditionalPersonas=%d"), 
		*DialogueId.ToString(), Payload.AdditionalPersonaIds.Num());

	// Emit event
	EventBus->EmitEvent(Payload, true);

#endif // WITH_GAMEEVENTBUS
}

//==============================================================================
// NEW v1.17.1: GameEventBus Integration - Node Entered Event
//==============================================================================

void UDialogueRunner::EmitNodeEnteredEvent(UDialogueNode* Node)
{
#if WITH_GAMEEVENTBUS
	if (!Node || !CurrentContext)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		return;
	}

	// Prepare payload
	FGameEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.NodeEntered"));
	Payload.StringParam = Node->NodeId; // Node ID
	
	// NEW v1.17.1: Add speaker info for camera system
	FName SpeakerId = Node->SpeakerId;
	if (!SpeakerId.IsNone())
	{
		// Store SpeakerId in AdditionalPersonaIds[0] for camera to read
		Payload.AdditionalPersonaIds.Add(SpeakerId.ToString());
		
		// Try to resolve speaker actor
		UDialogueParticipants* Participants = CurrentContext->GetParticipants();
		if (Participants)
		{
			AActor* SpeakerActor = Participants->GetActorByPersonaId(SpeakerId);
			if (SpeakerActor)
			{
				Payload.TargetActor = SpeakerActor;
				
				UE_LOG(LogDialogueRunner, Log, TEXT("[GAMEEVENTBUS] Emitting NodeEntered: Node='%s', Speaker='%s', Actor='%s'"),
					*Node->NodeId.ToString(),
					*SpeakerId.ToString(),
					*SpeakerActor->GetName());
			}
			else
			{
				UE_LOG(LogDialogueRunner, Warning, TEXT("[GAMEEVENTBUS] Emitting NodeEntered: Node='%s', Speaker='%s' (actor NOT found)"),
					*Node->NodeId.ToString(),
					*SpeakerId.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogDialogueRunner, Log, TEXT("[GAMEEVENTBUS] Emitting NodeEntered: Node='%s' (no speaker)"),
			*Node->NodeId.ToString());
	}

	// Emit event for camera to listen
	EventBus->EmitEvent(Payload, false);

#endif // WITH_GAMEEVENTBUS
}

//==============================================================================
// NEW v1.18: Automatic Turn-To-Face Event
//==============================================================================

void UDialogueRunner::EmitTurnToFaceEvent(UDialogueNode* Node)
{
	// NEW v1.18: DIAGNOSTIC - Always log method entry
	UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE DEBUG] EmitTurnToFaceEvent called!"));
	UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE DEBUG]   Node: %s"), Node ? *Node->NodeId.ToString() : TEXT("NULL"));
	UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE DEBUG]   Context: %s"), CurrentContext ? TEXT("VALID") : TEXT("NULL"));
	
	if (Node)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE DEBUG]   SpeakerId: %s"), *Node->SpeakerId.ToString());
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE DEBUG]   ListenerId: %s"), *Node->ListenerId.ToString());
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE DEBUG]   ListenerId.IsNone(): %s"), Node->ListenerId.IsNone() ? TEXT("YES") : TEXT("NO"));
	}

#if WITH_GAMEEVENTBUS
	if (!Node || !CurrentContext)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE DEBUG] Early return: Node or Context invalid"));
		return;
	}

	// Check if ListenerId is set (automatic turn-to-face)
	if (Node->ListenerId.IsNone())
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE DEBUG] Early return: ListenerId is None"));
		// No ListenerId - no automatic turn-to-face
		return;
	}

	// Check if SpeakerId is set (who should turn)
	if (Node->SpeakerId.IsNone())
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE] ListenerId set but no SpeakerId - skipping automatic turn"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		return;
	}

	// Resolve speaker actor (who turns)
	UDialogueParticipants* Participants = CurrentContext->GetParticipants();
	if (!Participants)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE] No Participants component - cannot resolve actors"));
		return;
	}

	AActor* SpeakerActor = Participants->GetActorByPersonaId(Node->SpeakerId);
	if (!SpeakerActor)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE] Speaker actor not found for '%s'"), 
			*Node->SpeakerId.ToString());
		return;
	}

	// Resolve listener actor (turn target)
	AActor* ListenerActor = Participants->GetActorByPersonaId(Node->ListenerId);
	if (!ListenerActor)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("[TURN-TO-FACE] Listener actor not found for '%s'"), 
			*Node->ListenerId.ToString());
		return;
	}

	// Prepare payload
	FGameEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.TurnToFace"));
	Payload.InstigatorActor = SpeakerActor;  // Who turns
	Payload.TargetActor = ListenerActor; // Turn target
	Payload.StringParam = FName(TEXT("Smooth"));  // Default mode
	Payload.FloatParam = 0.5f;     // Default duration
	Payload.IntParam = 0;           // bHeadOnly = false

	UE_LOG(LogDialogueRunner, Log, TEXT("[TURN-TO-FACE] Auto turn-to-face: '%s' turns to '%s'"),
		*Node->SpeakerId.ToString(),
		*Node->ListenerId.ToString());
	
	UE_LOG(LogDialogueRunner, Log, TEXT("[TURN-TO-FACE]   SpeakerActor: %s"), 
		*SpeakerActor->GetName());
	UE_LOG(LogDialogueRunner, Log, TEXT("[TURN-TO-FACE]   ListenerActor: %s"), 
		*ListenerActor->GetName());

	// Emit event
	EventBus->EmitEvent(Payload, true); // bLogEvent = true for debugging

	// ===== NEW v1.18.1: Symmetrical turn - Player ? NPC =====
	// If NPC addresses Player, Player should turn to NPC too!
	if (Node->ListenerId == FName("Player") && Node->SpeakerId != FName("Player"))
	{
		UE_LOG(LogDialogueRunner, Log, TEXT("[TURN-TO-FACE] Symmetrical turn: Player turns to '%s'"),
			*Node->SpeakerId.ToString());
		
		// Emit reverse event: Player turns to NPC
		FGameEventPayload PlayerPayload;
		PlayerPayload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.TurnToFace"));
		PlayerPayload.InstigatorActor = ListenerActor;  // Player (who turns)
		PlayerPayload.TargetActor = SpeakerActor;  // NPC (turn target)
		PlayerPayload.StringParam = FName(TEXT("Smooth"));
		PlayerPayload.FloatParam = 0.5f;    // Same duration
		PlayerPayload.IntParam = 0;         // Full body rotation
		
		EventBus->EmitEvent(PlayerPayload, true);
		
		UE_LOG(LogDialogueRunner, Log, TEXT("[TURN-TO-FACE] Player symmetrical turn event emitted"));
	}

#endif // WITH_GAMEEVENTBUS
}

//==============================================================================
// NEW v1.15: Participant Positioning System
//==============================================================================

void UDialogueRunner::ExtractAndApplySequencePositions(UDialogueNode* Node, bool bApplyStart, bool bApplyEnd)
{
	UE_LOG(LogDialogueRunner, Log, TEXT("=== ExtractAndApplySequencePositions START for node '%s' ==="), 
		Node ? *Node->NodeId.ToString() : TEXT("NULL"));
	UE_LOG(LogDialogueRunner, Log, TEXT("  bApplyStart=%s, bApplyEnd=%s"), 
		bApplyStart ? TEXT("true") : TEXT("false"), 
		bApplyEnd ? TEXT("true") : TEXT("false"));

	if (!Node || !CurrentContext || !LoadedDialogue)
	{
		UE_LOG(LogDialogueRunner, Error, TEXT("ExtractAndApplySequencePositions: Invalid Node, Context, or Dialogue!"));
		return;
	}

	// Find PlaySequence effect
	UDialogueEffect_PlaySequence* SequenceEffect = nullptr;
	for (UDialogueEffect* Effect : Node->Effects)
	{
		SequenceEffect = Cast<UDialogueEffect_PlaySequence>(Effect);
		if (SequenceEffect)
		{
			UE_LOG(LogDialogueRunner, Log, TEXT("  Found PlaySequence effect"));
			break;
		}
	}

	if (!SequenceEffect)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractAndApplySequencePositions: No PlaySequence effect in node '%s'"), 
			*Node->NodeId.ToString());
		return;
	}

	ULevelSequence* Sequence = SequenceEffect->GetSequence();
	if (!Sequence)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractAndApplySequencePositions: SequenceEffect has no Sequence!"));
		return;
	}

	UE_LOG(LogDialogueRunner, Log, TEXT("  Sequence: '%s'"), *Sequence->GetName());

	// Create extractor
	FSequencePositioningExtractor Extractor(Sequence);
	if (!Extractor.IsValid())
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractAndApplySequencePositions: Failed to create extractor"));
		return;
	}

	// Extract positions
	TMap<FName, FTransform> StartPositions;
	TMap<FName, FTransform> EndPositions;

	if (bApplyStart)
	{
		StartPositions = Extractor.GetStartPositions();
		UE_LOG(LogDialogueRunner, Log, TEXT("  Extracted %d start positions"), StartPositions.Num());
	}

	if (bApplyEnd)
	{
		EndPositions = Extractor.GetEndPositions();
		UE_LOG(LogDialogueRunner, Log, TEXT("  Extracted %d end positions"), EndPositions.Num());
	}

	if (StartPositions.Num() == 0 && EndPositions.Num() == 0)
	{
		UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractAndApplySequencePositions: No positions extracted!"));
		return;
	}

	// Get participants mapping
	UDialogueParticipants* Participants = CurrentContext->GetParticipants();
	if (!Participants)
	{
		UE_LOG(LogDialogueRunner, Error, TEXT("ExtractAndApplySequencePositions: No Participants component!"));
		return;
	}

	// Ensure personas are registered
	if (Participants->GetAllPersonaIds().Num() == 0)
	{
		AActor* Player = CurrentContext->GetPlayer();
		AActor* PrimaryNPC = CurrentContext->GetNPC();
		
		// Auto-resolve Additional Personas from DialogueData
		TMap<FName, AActor*> AdditionalNPCs;
		if (LoadedDialogue && LoadedDialogue->AdditionalPersonas.Num() > 0)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				for (const auto& Pair : LoadedDialogue->AdditionalPersonas)
				{
					FName PersonaId = Pair.Value.PersonaId;
					if (!PersonaId.IsNone())
					{
						AActor* FoundActor = FindActorByPersonaId(World, PersonaId);
						if (FoundActor)
						{
							AdditionalNPCs.Add(PersonaId, FoundActor);
						}
					}
				}
			}
		}

		RegisterPersonasFromDialogueData(Player, PrimaryNPC, AdditionalNPCs);
	}

	// Get positioning mode from DialogueDataAsset (NEW: renamed field)
	EDialoguePositioningMode PositioningMode = LoadedDialogue->SequencePositioningMode;

	// Helper function to find PersonaId for sequence actor name
	auto FindPersonaForSequenceActor = [&](const FName& SequenceActorName) -> FName
	{
		// 1. Try direct match (sequence actor name = PersonaId)
		AActor* DirectActor = Participants->GetActorByPersonaId(SequenceActorName);
		if (DirectActor)
		{
			return SequenceActorName;
		}

		// 2. Try finding by actor name contains PersonaId
		TArray<FName> AllPersonaIds = Participants->GetAllPersonaIds();
		for (const FName& PersonaId : AllPersonaIds)
		{
			AActor* Actor = Participants->GetActorByPersonaId(PersonaId);
			if (Actor)
			{
				// Check if sequence actor name contains persona ID or actor name
				FString SeqActorStr = SequenceActorName.ToString();
				FString PersonaStr = PersonaId.ToString();
				FString ActorNameStr = Actor->GetName();

				if (SeqActorStr.Contains(PersonaStr, ESearchCase::IgnoreCase) ||
					SeqActorStr.Contains(ActorNameStr, ESearchCase::IgnoreCase))
				{
					UE_LOG(LogDialogueRunner, Log, TEXT("    Mapped sequence actor '%s' to PersonaId '%s' (Actor: '%s')"),
						*SequenceActorName.ToString(), *PersonaId.ToString(), *ActorNameStr);
					return PersonaId;
				}
			}
		}

		// 3. Fallback: try finding actor in world with matching name
		UWorld* World = GetWorld();
		if (World)
		{
			FString SeqActorStr = SequenceActorName.ToString();
			for (const FName& PersonaId : AllPersonaIds)
			{
				AActor* Actor = Participants->GetActorByPersonaId(PersonaId);
				if (Actor && Actor->GetName().Contains(SeqActorStr, ESearchCase::IgnoreCase))
				{
					UE_LOG(LogDialogueRunner, Log, TEXT("    Mapped sequence actor '%s' to PersonaId '%s' via actor name"),
						*SequenceActorName.ToString(), *PersonaId.ToString());
					return PersonaId;
				}
			}
		}

		UE_LOG(LogDialogueRunner, Warning, TEXT("    Could not map sequence actor '%s' to any PersonaId"), *SequenceActorName.ToString());
		return NAME_None;
	};

	// Helper function to create and execute positioning effect
	auto ApplyPosition = [&](FName PersonaId, const FTransform& Transform, bool bIsStart)
	{
		if (PersonaId.IsNone())
		{
			return;
		}

		AActor* Actor = Participants->GetActorByPersonaId(PersonaId);
		if (!Actor)
		{
			UE_LOG(LogDialogueRunner, Warning, TEXT("    No actor found for PersonaId '%s'"), *PersonaId.ToString());
			return;
		}

		UE_LOG(LogDialogueRunner, Log, TEXT("  Positioning '%s' (PersonaId: '%s') to %s: Loc=%s, Rot=%s"), 
			*Actor->GetName(), 
			*PersonaId.ToString(),
			bIsStart ? TEXT("START") : TEXT("END"),
			*Transform.GetLocation().ToString(), 
			*Transform.GetRotation().Rotator().ToString());

		// Create positioning config
		FDialogueParticipantPositioning Config;
		Config.ParticipantId = PersonaId;
		Config.LocationSource = EDialogueLocationSource::WorldCoordinates;
		Config.TargetLocation = Transform.GetLocation();
		Config.TargetRotation = Transform.GetRotation().Rotator();
		Config.PositioningMode = PositioningMode;
		Config.bWaitForCompletion = bIsStart ? false : true; // Don't wait for start, wait for end
		Config.bApplyRotation = true;

		// Additional settings based on mode
		if (PositioningMode == EDialoguePositioningMode::ConditionalTeleport)
		{
			Config.TeleportDistanceThreshold = 1000.0f;
		}
		if (PositioningMode == EDialoguePositioningMode::AIMoveTo || 
			PositioningMode == EDialoguePositioningMode::ConditionalTeleport)
		{
			Config.AcceptanceRadius = 50.0f;
			Config.bUsePathfinding = true;
		}

		// Create effect using factory
		UDialogueEffect_PositionParticipant* PositionEffect = 
			UDialoguePositioningEffectFactory::CreateEffect(this, Config);

		if (PositionEffect)
		{
			PositionEffect->Execute(CurrentContext);
		}
	};

	// Apply start positions (before sequence plays)
	if (bApplyStart && StartPositions.Num() > 0)
	{
		UE_LOG(LogDialogueRunner, Log, TEXT("  Applying %d start positions..."), StartPositions.Num());
		
		for (const auto& Pair : StartPositions)
		{
			FName SequenceActorName = Pair.Key;
			FTransform Transform = Pair.Value;

			FName PersonaId = FindPersonaForSequenceActor(SequenceActorName);
			ApplyPosition(PersonaId, Transform, true);
		}
	}

	// Apply end positions (after sequence finishes)
	if (bApplyEnd && EndPositions.Num() > 0)
	{
		UE_LOG(LogDialogueRunner, Log, TEXT("  Storing %d end positions for application after sequence finishes..."), EndPositions.Num());
		
		// Clear previous pending positions
		PendingEndPositions.Empty();

		// Store end positions with mapped PersonaIds
		for (const auto& Pair : EndPositions)
		{
			FName SequenceActorName = Pair.Key;
			FTransform Transform = Pair.Value;

			FName PersonaId = FindPersonaForSequenceActor(SequenceActorName);
			if (!PersonaId.IsNone())
			{
				PendingEndPositions.Add(PersonaId, Transform);
				UE_LOG(LogDialogueRunner, Log, TEXT("    Stored end position for PersonaId '%s': Loc=%s, Rot=%s"),
					*PersonaId.ToString(),
					*Transform.GetLocation().ToString(),
					*Transform.GetRotation().Rotator().ToString());
			}
		}

		UE_LOG(LogDialogueRunner, Log, TEXT("  Stored %d end positions, will be applied after sequence finishes"), PendingEndPositions.Num());
	}

	UE_LOG(LogDialogueRunner, Log, TEXT("=== ExtractAndApplySequencePositions END ==="));
}

void UDialogueRunner::ExtractPositionsFromSequence(UDialogueNode* Node)
{
	UE_LOG(LogDialogueRunner, Warning, TEXT("=== ExtractPositionsFromSequence CALLED for node '%s' ==="), 
    Node ? *Node->NodeId.ToString() : TEXT("NULL"));
  
    if (!Node || !CurrentContext)
    {
        UE_LOG(LogDialogueRunner, Error, TEXT("ExtractPositionsFromSequence: Invalid Node or Context!"));
        return;
    }

    UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractPositionsFromSequence: Node->Effects.Num() = %d"), Node->Effects.Num());

    // Find PlaySequence effect in node effects
    UDialogueEffect_PlaySequence* SequenceEffect = nullptr;
  for (int32 i = 0; i < Node->Effects.Num(); ++i)
    {
   UDialogueEffect* Effect = Node->Effects[i];
        UE_LOG(LogDialogueRunner, Warning, TEXT("  Effect[%d]: %s"), i, Effect ? *Effect->GetClass()->GetName() : TEXT("NULL"));
 
        SequenceEffect = Cast<UDialogueEffect_PlaySequence>(Effect);
   if (SequenceEffect)
        {
    UE_LOG(LogDialogueRunner, Warning, TEXT("  ? Found PlaySequence effect at index %d!"), i);
            break;
        }
    }

    // No sequence - skip automatic extraction
    if (!SequenceEffect)
    {
        UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractPositionsFromSequence: No PlaySequence effect found in node '%s'"), 
   *Node->NodeId.ToString());
        return;
    }

    ULevelSequence* Sequence = SequenceEffect->GetSequence();
    if (!Sequence)
    {
      UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractPositionsFromSequence: SequenceEffect has no Sequence!"));
     return;
    }

    UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractPositionsFromSequence: Found sequence '%s'"), *Sequence->GetName());

    // Create extractor
    FSequencePositioningExtractor Extractor(Sequence);
    if (!Extractor.IsValid())
    {
UE_LOG(LogDialogueRunner, Warning, TEXT("ExtractPositionsFromSequence: Failed to create extractor for sequence '%s'"), 
      *Sequence->GetName());
 return;
    }

 // Extract start/end positions
    TMap<FName, FTransform> StartPositions = Extractor.GetStartPositions();
    TMap<FName, FTransform> EndPositions = Extractor.GetEndPositions();

    UE_LOG(LogDialogueRunner, Warning, TEXT("=== ExtractPositionsFromSequence: Extracted %d start positions, %d end positions from sequence '%s' ==="),
        StartPositions.Num(), EndPositions.Num(), *Sequence->GetName());

    // TODO v1.16.8: Apply extracted positions to Node->Positioning automatically
    // For now, just log what we found

    for (const auto& Pair : StartPositions)
    {
        UE_LOG(LogDialogueRunner, Warning, TEXT("  Start Position[%s]: Loc=%s, Rot=%s"),
          *Pair.Key.ToString(),
      *Pair.Value.GetLocation().ToString(),
            *Pair.Value.GetRotation().Rotator().ToString());
    }

    for (const auto& Pair : EndPositions)
    {
        UE_LOG(LogDialogueRunner, Warning, TEXT("  End Position[%s]: Loc=%s, Rot=%s"),
            *Pair.Key.ToString(),
      *Pair.Value.GetLocation().ToString(),
 *Pair.Value.GetRotation().Rotator().ToString());
    }
    
    UE_LOG(LogDialogueRunner, Warning, TEXT("=== ExtractPositionsFromSequence COMPLETE ==="));
}

void UDialogueRunner::ExecuteNodePositioning(UDialogueNode* Node)
{
	if (!Node || !CurrentContext || !LoadedDialogue)
	{
		return;
	}

	const FDialogueNodePositioning& Positioning = Node->Positioning;

	// NEW v1.17.0: Determine if we should apply sequence-based positioning
	bool bShouldExtractFromSequence = false;
	bool bApplyStart = false;
	bool bApplyEnd = false;

	// Check if sequence positioning is enabled (either globally or per-node override)
	if (Positioning.bEnablePositioning && Positioning.bOverrideSequencePositioning)
	{
		// Per-node override takes precedence
		bShouldExtractFromSequence = true;
		bApplyStart = Positioning.bPositionAtSequenceStart;
		bApplyEnd = Positioning.bPositionAtSequenceEnd;
		UE_LOG(LogDialogueRunner, Log, TEXT("ExecuteNodePositioning: Using per-node sequence positioning (Start=%s, End=%s)"),
			bApplyStart ? TEXT("true") : TEXT("false"),
			bApplyEnd ? TEXT("true") : TEXT("false"));
	}
	else if (LoadedDialogue->bExtractPositionsFromSequence)
	{
		// Global setting from DialogueDataAsset
		bShouldExtractFromSequence = true;
		bApplyStart = LoadedDialogue->bPositionActorsAtSequenceStart;
		bApplyEnd = LoadedDialogue->bPositionActorsAtSequenceEnd;
		UE_LOG(LogDialogueRunner, Log, TEXT("ExecuteNodePositioning: Using global sequence positioning (Start=%s, End=%s)"),
			bApplyStart ? TEXT("true") : TEXT("false"),
			bApplyEnd ? TEXT("true") : TEXT("false"));
	}

	// Apply sequence-based positioning if enabled
	if (bShouldExtractFromSequence && (bApplyStart || bApplyEnd))
	{
		ExtractAndApplySequencePositions(Node, bApplyStart, bApplyEnd);
		
		// If sequence positioning is active and no manual positioning configured, return early
		if (!Positioning.bEnablePositioning || (Positioning.bEnablePositioning && Positioning.bOverrideSequencePositioning))
		{
			// Sequence positioning is handling everything, skip manual positioning
			return;
		}
	}

	// Manual positioning logic (v1.15 - v1.17)
	// Only execute if manual positioning is explicitly enabled AND not overridden by sequence positioning
	if (!Positioning.bEnablePositioning || Positioning.bOverrideSequencePositioning)
	{
		return; // No manual positioning configured
	}

	// NEW v1.16: Auto-register personas if not yet registered
	if (CurrentContext->GetParticipants())
	{
		TArray<FName> ExistingPersonas = CurrentContext->GetParticipants()->GetAllPersonaIds();
		if (ExistingPersonas.Num() == 0)
		{
			// No personas registered yet - register them now
			AActor* Player = CurrentContext->GetPlayer();
			AActor* PrimaryNPC = CurrentContext->GetNPC();
			
			// NEW v1.16.2: Auto-resolve Additional Personas from DialogueData
			TMap<FName, AActor*> AdditionalNPCs;
			if (LoadedDialogue && LoadedDialogue->AdditionalPersonas.Num() > 0)
			{
				UWorld* World = GetWorld();
				if (World)
				{
					// Try to find actors for each Additional Persona in the world
					for (const auto& Pair : LoadedDialogue->AdditionalPersonas)
					{
						FName PersonaId = Pair.Value.PersonaId;
						if (!PersonaId.IsNone())
						{
							// Search for actor with matching CharacterId (DialogueComponent)
							AActor* FoundActor = FindActorByPersonaId(World, PersonaId);
							if (FoundActor)
							{
								AdditionalNPCs.Add(PersonaId, FoundActor);
								UE_LOG(LogDialogueRunner, Log, TEXT("ExecuteNodePositioning: Auto-resolved Additional Persona '%s' -> '%s'"),
									*PersonaId.ToString(), *FoundActor->GetName());
							}
							else
							{
								UE_LOG(LogDialogueRunner, Warning, TEXT("ExecuteNodePositioning: Could not find actor for Additional Persona '%s'"),
									*PersonaId.ToString());
							}
						}
					}
				}
			}

			RegisterPersonasFromDialogueData(Player, PrimaryNPC, AdditionalNPCs);
		}
	}

	UE_LOG(LogDialogueRunner, Log, TEXT("ExecuteNodePositioning: Processing manual positioning for node '%s'"), *Node->NodeId.ToString());

	// Get primary persona ID from dialogue data
	FName PrimaryPersonaId = LoadedDialogue->PrimaryPersona.PersonaId;

	// Create positioning effects using factory
	TArray<UDialogueEffect_PositionParticipant*> PositioningEffects = 
		UDialoguePositioningEffectFactory::CreateEffectsForNode(
			this,
			Positioning,
			PrimaryPersonaId
		);

	// Execute all positioning effects
	for (UDialogueEffect_PositionParticipant* Effect : PositioningEffects)
	{
		if (Effect)
		{
			Effect->Execute(CurrentContext);
		}
	}

	UE_LOG(LogDialogueRunner, Log, TEXT("ExecuteNodePositioning: Executed %d manual positioning effects"), PositioningEffects.Num());
}

//==============================================================================
// NEW v1.16: Multi-NPC Dialogue Support - Persona Registration
//==============================================================================

void UDialogueRunner::RegisterPersonasFromDialogueData(
	AActor* Player,
	AActor* PrimaryNPC,
	const TMap<FName, AActor*>& AdditionalNPCs)
{
	if (!CurrentContext || !LoadedDialogue)
	{
		return;
	}

	UDialogueParticipants* Participants = CurrentContext->GetParticipants();
	if (!Participants)
	{
		UE_LOG(LogDialogueRunner, Error, TEXT("RegisterPersonasFromDialogueData: No Participants component"));
		return;
	}

	// Register Primary Persona (NPC)
	FName PrimaryPersonaId = LoadedDialogue->PrimaryPersona.PersonaId;
	if (!PrimaryPersonaId.IsNone() && PrimaryNPC)
	{
		Participants->SetActorForPersona(PrimaryPersonaId, PrimaryNPC);
		UE_LOG(LogDialogueRunner, Log, TEXT("RegisterPersonas: Primary Persona '%s' -> '%s'"), 
			*PrimaryPersonaId.ToString(), *PrimaryNPC->GetName());
	}

	// Register Player
	if (Player)
	{
		Participants->SetActorForPersona(FName("Player"), Player);
		UE_LOG(LogDialogueRunner, Log, TEXT("RegisterPersonas: Player registered as 'Player'"));
	}

	// Register Additional NPCs
	for (const auto& Pair : AdditionalNPCs)
	{
		FName PersonaId = Pair.Key;
		AActor* Actor = Pair.Value;

		if (!PersonaId.IsNone() && Actor)
		{
			Participants->SetActorForPersona(PersonaId, Actor);
			UE_LOG(LogDialogueRunner, Log, TEXT("RegisterPersonas: Additional Persona '%s' -> '%s'"), 
				*PersonaId.ToString(), *Actor->GetName());
		}
	}

	// Log summary
	TArray<FName> AllPersonaIds = Participants->GetAllPersonaIds();
	FString PersonaList;
	for (const FName& Id : AllPersonaIds)
	{
		if (!PersonaList.IsEmpty())
		{
			PersonaList += TEXT(", ");
		}
		PersonaList += Id.ToString();
	}
	UE_LOG(LogDialogueRunner, Log, TEXT("RegisterPersonas: Total %d personas registered: %s"), 
		AllPersonaIds.Num(), *PersonaList);
}

//==============================================================================
// NEW v1.16.2: Helper - Find Actor by PersonaId
//==============================================================================

AActor* UDialogueRunner::FindActorByPersonaId(UWorld* World, FName PersonaId)
{
	if (!World || PersonaId.IsNone())
	{
		return nullptr;
	}

	// 1. PRIORITY: Search for component implementing IDialogueParticipant with matching CharacterId
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor || Actor->IsPendingKillPending())
		{
			continue;
		}

		// Check all components for IDialogueParticipant interface
		TArray<UActorComponent*> Components = Actor->GetComponentsByInterface(UDialogueParticipant::StaticClass());
		for (UActorComponent* Component : Components)
		{
			if (Component && Component->GetClass()->ImplementsInterface(UDialogueParticipant::StaticClass()))
			{
				// Use IDialogueParticipant interface to get CharacterId
				FName ActorCharacterId = IDialogueParticipant::Execute_GetParticipantId(Component);
				if (ActorCharacterId == PersonaId)
				{
					UE_LOG(LogDialogueRunner, Log, TEXT("FindActorByPersonaId: Found actor '%s' via IDialogueParticipant.CharacterId '%s'"),
						*Actor->GetName(), *PersonaId.ToString());
					return Actor;
				}
			}
		}
	}

	// 2. FALLBACK: Check actor tags
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		if (Actor->Tags.Contains(PersonaId))
		{
			UE_LOG(LogDialogueRunner, Log, TEXT("FindActorByPersonaId: Found actor '%s' by tag '%s'"),
				*Actor->GetName(), *PersonaId.ToString());
			return Actor;
		}
	}

	// 3. LAST FALLBACK: Check if actor name contains PersonaId
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		FString ActorName = Actor->GetName();
		FString PersonaIdStr = PersonaId.ToString();
		if (ActorName.Contains(PersonaIdStr, ESearchCase::IgnoreCase))
		{
			UE_LOG(LogDialogueRunner, Log, TEXT("FindActorByPersonaId: Found actor '%s' by name match '%s'"),
				*ActorName, *PersonaIdStr);
			return Actor;
		}
	}

	UE_LOG(LogDialogueRunner, Warning, TEXT("FindActorByPersonaId: Could not find actor for PersonaId '%s'. Solutions:"), *PersonaId.ToString());
	UE_LOG(LogDialogueRunner, Warning, TEXT("  1. Add DialogueComponent to actor and set CharacterId = '%s'"), *PersonaId.ToString());
	UE_LOG(LogDialogueRunner, Warning, TEXT("  2. Add Actor Tag '%s' in Details panel ? Tags"), *PersonaId.ToString());
	UE_LOG(LogDialogueRunner, Warning, TEXT("  3. Ensure actor name contains '%s'"), *PersonaId.ToString());
	return nullptr;
}

//==============================================================================
// NEW v1.17.0: Apply Pending End Positions
//==============================================================================

void UDialogueRunner::ApplyPendingEndPositions()
{
	UE_LOG(LogDialogueRunner, Warning, TEXT("=== ApplyPendingEndPositions CALLED ==="));
	UE_LOG(LogDialogueRunner, Warning, TEXT("  PendingEndPositions.Num() = %d"), PendingEndPositions.Num());

	if (PendingEndPositions.Num() == 0)
	{
		UE_LOG(LogDialogueRunner, Log, TEXT("ApplyPendingEndPositions: No pending end positions to apply"));
		return;
	}

	if (!CurrentContext || !LoadedDialogue)
	{
		UE_LOG(LogDialogueRunner, Error, TEXT("ApplyPendingEndPositions: Invalid context or dialogue!"));
		return;
	}

	UDialogueParticipants* Participants = CurrentContext->GetParticipants();
	if (!Participants)
	{
		UE_LOG(LogDialogueRunner, Error, TEXT("ApplyPendingEndPositions: No Participants component!"));
		return;
	}

	EDialoguePositioningMode PositioningMode = LoadedDialogue->SequencePositioningMode;

	UE_LOG(LogDialogueRunner, Warning, TEXT("ApplyPendingEndPositions: Applying %d END positions with mode '%s'..."), 
		PendingEndPositions.Num(),
		*UEnum::GetValueAsString(PositioningMode));

	int32 AppliedCount = 0;

	for (const auto& Pair : PendingEndPositions)
	{
		FName PersonaId = Pair.Key;
		FTransform Transform = Pair.Value;

		AActor* Actor = Participants->GetActorByPersonaId(PersonaId);
		if (!Actor)
		{
			UE_LOG(LogDialogueRunner, Warning, TEXT("  [SKIP] No actor found for PersonaId '%s'"), *PersonaId.ToString());
			continue;
		}

		UE_LOG(LogDialogueRunner, Warning, TEXT("  [APPLYING] '%s' (PersonaId: '%s')"), 
			*Actor->GetName(), 
			*PersonaId.ToString());
		UE_LOG(LogDialogueRunner, Warning, TEXT("    Current Location: %s"), *Actor->GetActorLocation().ToString());
		UE_LOG(LogDialogueRunner, Warning, TEXT("    Target Location:  %s"), *Transform.GetLocation().ToString());
		UE_LOG(LogDialogueRunner, Warning, TEXT("    Target Rotation:  %s"), *Transform.GetRotation().Rotator().ToString());

		// Create positioning config
		FDialogueParticipantPositioning Config;
		Config.ParticipantId = PersonaId;
		Config.LocationSource = EDialogueLocationSource::WorldCoordinates;
		Config.TargetLocation = Transform.GetLocation();
		Config.TargetRotation = Transform.GetRotation().Rotator();
		Config.PositioningMode = PositioningMode;
		Config.bWaitForCompletion = true; // Wait for end positioning
		Config.bApplyRotation = true;
		Config.bSkipCapsuleCompensation = true; // FIX v1.17.4: Sequence positions already at correct height!

		// Additional settings based on mode
		if (PositioningMode == EDialoguePositioningMode::ConditionalTeleport)
		{
			Config.TeleportDistanceThreshold = 1000.0f;
		}
		if (PositioningMode == EDialoguePositioningMode::AIMoveTo || 
			PositioningMode == EDialoguePositioningMode::ConditionalTeleport)
		{
			Config.AcceptanceRadius = 50.0f;
			Config.bUsePathfinding = true;
		}

		// Create effect using factory
		UDialogueEffect_PositionParticipant* PositionEffect = 
			UDialoguePositioningEffectFactory::CreateEffect(this, Config);

		if (PositionEffect)
		{
			PositionEffect->Execute(CurrentContext);
			AppliedCount++;
			UE_LOG(LogDialogueRunner, Warning, TEXT("    [SUCCESS] Effect executed"));
		}
		else
		{
			UE_LOG(LogDialogueRunner, Error, TEXT("    [FAILED] Could not create positioning effect"));
		}
	}

	// Clear pending positions after application
	PendingEndPositions.Empty();

	UE_LOG(LogDialogueRunner, Warning, TEXT("=== ApplyPendingEndPositions COMPLETE: Applied %d/%d positions ==="), 
		AppliedCount, PendingEndPositions.Num());
}
