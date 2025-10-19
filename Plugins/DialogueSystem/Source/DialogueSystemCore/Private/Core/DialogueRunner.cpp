// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueRunner.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueNode.h"
#include "Data/DialogueDataAsset.h"
#include "Conditions/DialogueConditionEvaluator.h"
#include "Effects/DialogueEffectExecutor.h"
#include "TimerManager.h"
#include "Engine/World.h"

UDialogueRunner::UDialogueRunner()
    : bIsActive(false)
    , CurrentNode(nullptr)
    , CurrentContext(nullptr)
    , LoadedDialogue(nullptr)
    , ConditionEvaluator(nullptr)
    , EffectExecutor(nullptr)
{
}

void UDialogueRunner::StartDialogue(UDialogueDataAsset* InDialogue, const TArray<UObject*>& InParticipants)
{
    if (!InDialogue || InParticipants.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::StartDialogue - Invalid dialogue or no participants"));
        return;
    }
    
    // End previous dialogue if any
    if (bIsActive)
    {
        EndDialogue();
    }
    
    LoadedDialogue = InDialogue;
    
    // Create context
    CurrentContext = NewObject<UDialogueSessionContext>(this);
    if (!CurrentContext)
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueRunner::StartDialogue - Failed to create context"));
        return;
    }
    
    // Initialize participants - use Player and NPC fields instead
    if (InParticipants.Num() > 0)
    {
        CurrentContext->Player = Cast<AActor>(InParticipants[0]);
    }
    if (InParticipants.Num() > 1)
    {
        CurrentContext->NPC = Cast<AActor>(InParticipants[1]);
    }
    
    // Create evaluators
    ConditionEvaluator = NewObject<UDialogueConditionEvaluator>(this);
    EffectExecutor = NewObject<UDialogueEffectExecutor>(this);
    
    // Build node cache
    BuildNodeCache();
    
    // Find start node
    FName StartNodeId = InDialogue->StartNode;
    if (StartNodeId.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueRunner::StartDialogue - No start node defined"));
        return;
    }
    
    bIsActive = true;
    NodeHistory.Empty();
    
    // Broadcast start event
    OnDialogueStarted.Broadcast(InDialogue->DialogueId);
    
    // Go to start node
    GoToNode(StartNodeId);
}

void UDialogueRunner::EndDialogue()
{
    if (!bIsActive)
    {
        return;
    }
    
    // Clear timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
    }
    
    bIsActive = false;
    OnDialogueEnded.Broadcast();
    
    // Cleanup
    CurrentNode = nullptr;
    NodeHistory.Empty();
    NodeCache.Empty();
    LoadedDialogue = nullptr;
    CurrentContext = nullptr;
}

void UDialogueRunner::PauseDialogue()
{
    if (!bIsActive)
    {
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
    if (!bIsActive)
    {
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
    if (!bIsActive || !LoadedDialogue || !CurrentContext)
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
    
    // Add to history
    if (CurrentNode)
    {
        NodeHistory.Add(CurrentNode);
    }
    
    CurrentNode = NewNode;
    
    // Mark as visited
    CurrentContext->VisitedNodes.Add(NodeId);
    
    // Process node
    ProcessNode(NewNode);
    
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
    if (!bIsActive || !CurrentNode)
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
    if (!bIsActive || !CurrentNode)
    {
        return;
    }
    
    // Clear auto-advance timer
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AutoAdvanceTimerHandle);
    }
    
    // Compute next node based on current node type
    UDialogueNode* NextNode = ComputeNextNode(CurrentNode);
    
    if (NextNode)
    {
        // If we have a single next node, go to it
        GoToNode(NextNode->NodeId);
    }
    else
    {
        // If no direct next node, check for choices
        TArray<UDialogueNode*> Choices = GetAvailableChoices();
        if (Choices.Num() > 0)
        {
            // Show choices to player
            OnChoicesReady.Broadcast(Choices);
        }
        else
        {
            // No choices and no next node - end dialogue
            EndDialogue();
        }
    }
}

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
        EndDialogue();
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
