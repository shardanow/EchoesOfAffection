// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogueState.generated.h"

// Forward declarations
class UDialogueNode;

/**
 * Conversation entry in history
 * v1.3.1: Moved from DialogueContext to break circular dependency
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FConversationEntry
{
    GENERATED_BODY()

    /** Speaker name */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
    FText SpeakerName;

    /** Dialogue text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
    FText DialogueText;

    /** Timestamp */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
  FDateTime Timestamp;

    /** Node ID */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
    FName NodeId;

    FConversationEntry()
        : SpeakerName(FText::GetEmpty())
        , DialogueText(FText::GetEmpty())
        , Timestamp(FDateTime::Now())
     , NodeId(NAME_None)
    {}
};

/**
 * Dialogue State Management
 * 
 * Handles:
 * - Current node tracking
 * - Visited nodes history
 * - Conversation history
 * - Session timing
 * 
 * v1.3.1: Extracted from DialogueContext (SRP principle)
 * 
 * Benefits:
 * - Single Responsibility: Only state management
 * - Easier to test
 * - Cleaner separation of concerns
 * - Reusable in different contexts
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueState : public UObject
{
    GENERATED_BODY()

public:
    //==========================================================================
    // Current Node Management
    //==========================================================================

 /** Get current active node */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueNode* GetCurrentNode() const { return CurrentNode.Get(); }

  /** Set current active node */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    void SetCurrentNode(UDialogueNode* Node);

    //==========================================================================
    // Visited Nodes Tracking
    //==========================================================================

    /** Mark node as visited */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    void MarkNodeVisited(FName NodeId);

    /** Check if node was visited */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    bool WasNodeVisited(FName NodeId) const;

    /** Get all visited nodes */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    const TArray<FName>& GetVisitedNodes() const { return VisitedNodes; }

    /** Clear visited nodes history */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    void ClearVisitedNodes();

    //==========================================================================
    // Conversation History
    //==========================================================================

    /** Add entry to conversation history */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    void AddToHistory(const FText& SpeakerName, const FText& DialogueText, FName NodeId);

    /** Get conversation history */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
  const TArray<FConversationEntry>& GetConversationHistory() const { return ConversationHistory; }

    /** Clear conversation history */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    void ClearHistory();

    //==========================================================================
// Session Timing
    //==========================================================================

    /** Get session start time */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    FDateTime GetSessionStartTime() const { return SessionStartTime; }

    /** Get session duration in seconds */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    float GetSessionDuration() const;

    /** Reset session start time */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    void ResetSessionTime();

    //==========================================================================
    // State Management
    //==========================================================================

    /** Reset all state to default */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    void Reset();

    /** Check if state is empty (no active dialogue) */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    bool IsEmpty() const;

protected:
    /** Current active node */
 UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    TWeakObjectPtr<UDialogueNode> CurrentNode;

    /** Visited nodes during this session */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    TArray<FName> VisitedNodes;

    /** Conversation history */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
  TArray<FConversationEntry> ConversationHistory;

    /** Session start time */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    FDateTime SessionStartTime;

    friend class UDialogueSessionContext;
};
