// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueContext.h"
#include "Core/DialogueStateMachine.h"
#include "Commands/DialogueCommandHistory.h"
#include "DialogueRunner.generated.h"

// Forward declarations
class UDialogueDataAsset;
class UDialogueConditionEvaluator;
class UDialogueEffectExecutor;
class UDialogueSessionContext;
class UDialogueCommandInvoker;
class UDialogueCommand;
class UDialogueEffect;
class UDialogueStateMachine;
class UDialogueEffect_PlaySequence; // NEW v1.13: For sequence cleanup

/**
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, FName, DialogueId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeEntered, UDialogueNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicesReady, const TArray<UDialogueNode*>&, Choices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoiceSelected, int32, ChoiceIndex, UDialogueNode*, ChoiceNode);

/** NEW v1.3: State change event */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueStateChanged, EDialogueState, OldState, EDialogueState, NewState);

/**
 * 
 * Features (v1.3):
 * - Command History tracking
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueRunner : public UObject
{
    GENERATED_BODY()

public:
    UDialogueRunner();
    
    //~ Begin Events

    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueStarted OnDialogueStarted;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueEnded OnDialogueEnded;

  UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnNodeEntered OnNodeEntered;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoicesReady OnChoicesReady;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoiceSelected OnChoiceSelected;

    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueStateChanged OnStateChanged;

    //~ End Events

protected:
    UPROPERTY(Transient)
    TObjectPtr<UDialogueDataAsset> LoadedDialogue;

    UPROPERTY(Transient)
    TObjectPtr<UDialogueSessionContext> CurrentContext;

    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> CurrentNode;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UDialogueNode>> NodeHistory;

    UPROPERTY(Transient)
    TObjectPtr<UDialogueConditionEvaluator> ConditionEvaluator;

    UPROPERTY(Transient)
    TObjectPtr<UDialogueEffectExecutor> EffectExecutor;

    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDialogueNode>> NodeCache;

    /** DEPRECATED v1.3: Use StateMachine instead */
    UPROPERTY(Transient)
    bool bIsActive;

    UPROPERTY(Transient)
    TObjectPtr<UDialogueStateMachine> StateMachine;

    UPROPERTY(Transient)
    TObjectPtr<UDialogueCommandInvoker> CommandInvoker;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dialogue|Commands")
    bool bEnableCommandHistory;

  FTimerHandle AutoAdvanceTimerHandle;

    /** Active sequence effect (if any) - used for cleanup on dialogue end */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueEffect_PlaySequence> ActiveSequenceEffect;

    /** NEW v1.17.0: Pending end positions to apply after sequence finishes */
    UPROPERTY(Transient)
    TMap<FName, FTransform> PendingEndPositions;

public:
    //~ Begin Lifecycle

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void StartDialogue(UDialogueDataAsset* InDialogue, const TArray<UObject*>& InParticipants);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void EndDialogue();

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void PauseDialogue();

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void ResumeDialogue();

    //~ End Lifecycle

    //~ Begin Navigation

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoToNode(FName NodeId);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool JumpToNode(UDialogueNode* TargetNode);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool SelectChoice(int32 ChoiceIndex);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoBack();

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    void Skip();

    /** Continue to next node in dialogue flow (calls Skip internally) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    void Continue() { Skip(); }

  //~ End Navigation

    //~ Begin Queries

    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueNode* GetCurrentNode() const { return CurrentNode; }

    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueSessionContext* GetContext() const { return CurrentContext; }

    /** DEPRECATED v1.3: Use GetCurrentState() instead */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State", meta = (DeprecatedFunction, DeprecationMessage = "Use GetCurrentState() instead"))
    bool IsActive() const;

    /** NEW v1.3: Get current dialogue state */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    EDialogueState GetCurrentState() const;

    /** NEW v1.15: Get loaded dialogue data */
  UFUNCTION(BlueprintPure, Category = "Dialogue")
    UDialogueDataAsset* GetLoadedDialogue() const { return LoadedDialogue; }

    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    TArray<UDialogueNode*> GetAvailableChoices() const;

  //~ End Queries

 //~ Begin Command Pattern (v1.2)

    /**
 * Execute a dialogue command
   * @param Command - Command to execute
     * @param bRecordInHistory - Whether to record in history (default: true)
     * @return true if execution succeeded
 */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Commands")
    bool ExecuteCommand(UDialogueCommand* Command, bool bRecordInHistory = true);

    /**
     * Undo last command
     * @return true if undo succeeded
  */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Commands")
    bool UndoLastCommand();

    /**
     * Redo next command
     * @return true if redo succeeded
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Commands")
    bool RedoCommand();

    /**
     * Check if undo is available
     */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Commands")
    bool CanUndo() const;

    /**
     * Check if redo is available
     */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Commands")
    bool CanRedo() const;

    /**
  * Get command invoker
  */
  UFUNCTION(BlueprintPure, Category = "Dialogue|Commands")
    UDialogueCommandInvoker* GetCommandInvoker() const { return CommandInvoker; }

    /**
     * Get command history as string (for debugging)
     */
UFUNCTION(BlueprintPure, Category = "Dialogue|Commands")
    FString GetCommandHistoryAsString() const;

    /**
     * Clear command history
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Commands")
    void ClearCommandHistory();

    /**
     * Enable/disable command recording
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Commands")
    void SetCommandRecordingEnabled(bool bEnabled);

    /**
     * Check if command recording is enabled
     */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Commands")
    bool IsCommandRecordingEnabled() const { return bEnableCommandHistory; }

    //~ End Command Pattern

    /** NEW v1.13: Register active sequence effect for cleanup on dialogue end */
    void RegisterActiveSequence(UDialogueEffect_PlaySequence* SequenceEffect);

    /** NEW v1.13: Clear active sequence reference */
    void ClearActiveSequence();

protected:
    //~ Begin Internal Logic

    void BuildNodeCache();

    void ProcessNode(UDialogueNode* Node);

  void ApplyNodeEffects(UDialogueNode* Node);

    UDialogueNode* ComputeNextNode(UDialogueNode* FromNode);

    TArray<UDialogueNode*> GatherChoices(UDialogueNode* FromNode) const;

    UDialogueNode* ProcessRandomNode(UDialogueNode* RandomNode);

    UDialogueNode* ProcessConditionNode(UDialogueNode* ConditionNode);

    bool IsConnectionAvailable(const FDialogueConnection& Connection) const;

    void SetupAutoAdvance(UDialogueNode* Node);

    void OnAutoAdvanceTimer();

    /** NEW v1.3: Check if operation is allowed in current state */
    bool CanPerformOperation(FName OperationName) const;

    /** NEW v1.3: Callback when state changes */
    void OnStateMachineStateChanged(EDialogueState OldState, EDialogueState NewState);

 /** NEW v1.13: Emit GameEventBus event for dialogue start (soft integration with ActorScheduleSystem) */
    void EmitDialogueStartedEvent(FName DialogueId, const TArray<UObject*>& Participants);

    /** NEW v1.13: Emit GameEventBus event for dialogue end */
    void EmitDialogueEndedEvent(FName DialogueId);

    /** NEW v1.17.1: Emit GameEventBus event when node is entered (for camera system) */
    void EmitNodeEnteredEvent(UDialogueNode* Node);

    /** NEW v1.13.2: Gather all sequence participants from all dialogue nodes */
    TArray<AActor*> GatherAllSequenceParticipants() const;

    /** NEW v1.15: Execute node positioning for participants */
  void ExecuteNodePositioning(UDialogueNode* Node);

    /** NEW v1.16: Register all personas from dialogue data into Participants mapping */
 void RegisterPersonasFromDialogueData(AActor* Player, AActor* PrimaryNPC, const TMap<FName, AActor*>& AdditionalNPCs);

    /** NEW v1.16.2: Find actor in world by PersonaId (searches DialogueComponent with matching CharacterId) */
    AActor* FindActorByPersonaId(UWorld* World, FName PersonaId);

    /** NEW v1.16.8: Extract positioning data from sequence automatically */
    void ExtractPositionsFromSequence(UDialogueNode* Node);

    /** NEW v1.17.0: Extract and apply sequence positions to actors */
    void ExtractAndApplySequencePositions(UDialogueNode* Node, bool bApplyStart, bool bApplyEnd);

public:
    /** NEW v1.17.0: Apply pending end positions (called after sequence finishes) */
    void ApplyPendingEndPositions();

protected:
    //~ End Internal Logic

    //~ Begin Command Helpers (v1.2)

    /**
  * Create a navigate command
     */
    UDialogueCommand* CreateNavigateCommand(UDialogueNode* TargetNode);

    /**
     * Create an apply effects command
     */
    UDialogueCommand* CreateApplyEffectsCommand(const TArray<UDialogueEffect*>& Effects);

    /**
     * Create a select choice command
     */
    UDialogueCommand* CreateSelectChoiceCommand(int32 ChoiceIndex);

  //~ End Command Helpers
};
