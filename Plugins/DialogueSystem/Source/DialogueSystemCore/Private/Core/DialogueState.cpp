// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DialogueState.h"
#include "Core/DialogueNode.h"

//==============================================================================
// Current Node Management
//==============================================================================

void UDialogueState::SetCurrentNode(UDialogueNode* Node)
{
    CurrentNode = Node;
}

//==============================================================================
// Visited Nodes Tracking
//==============================================================================

void UDialogueState::MarkNodeVisited(FName NodeId)
{
    VisitedNodes.AddUnique(NodeId);
}

bool UDialogueState::WasNodeVisited(FName NodeId) const
{
    return VisitedNodes.Contains(NodeId);
}

void UDialogueState::ClearVisitedNodes()
{
    VisitedNodes.Empty();
}

//==============================================================================
// Conversation History
//==============================================================================

void UDialogueState::AddToHistory(const FText& SpeakerName, const FText& DialogueText, FName NodeId)
{
    FConversationEntry Entry;
    Entry.SpeakerName = SpeakerName;
    Entry.DialogueText = DialogueText;
    Entry.NodeId = NodeId;
  Entry.Timestamp = FDateTime::Now();
    
    ConversationHistory.Add(Entry);
}

void UDialogueState::ClearHistory()
{
    ConversationHistory.Empty();
}

//==============================================================================
// Session Timing
//==============================================================================

float UDialogueState::GetSessionDuration() const
{
    return (FDateTime::Now() - SessionStartTime).GetTotalSeconds();
}

void UDialogueState::ResetSessionTime()
{
    SessionStartTime = FDateTime::Now();
}

//==============================================================================
// State Management
//==============================================================================

void UDialogueState::Reset()
{
    CurrentNode = nullptr;
    VisitedNodes.Empty();
    ConversationHistory.Empty();
 SessionStartTime = FDateTime::Now();
}

bool UDialogueState::IsEmpty() const
{
    return !CurrentNode.IsValid() && 
           VisitedNodes.Num() == 0 && 
           ConversationHistory.Num() == 0;
}
