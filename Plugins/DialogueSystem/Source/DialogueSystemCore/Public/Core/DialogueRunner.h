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

/**
 * ������� ��� �������
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, FName, DialogueId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeEntered, UDialogueNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicesReady, const TArray<UDialogueNode*>&, Choices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoiceSelected, int32, ChoiceIndex, UDialogueNode*, ChoiceNode);

/** NEW v1.3: State change event */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueStateChanged, EDialogueState, OldState, EDialogueState, NewState);

/**
 * �������� ����� ���������� ��������
 * ��������� state machine, ���������, ������� � �������
 * 
 * Features (v1.3):
 * - State Machine ��� ���������� ����������
 * - Command Pattern ��� ���� ��������
 * - Undo/Redo ���������
 * - Command History tracking
 * - Replay ����������������
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueRunner : public UObject
{
    GENERATED_BODY()

public:
    UDialogueRunner();
    
    //~ Begin Events

    /** ������ ����� */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueStarted OnDialogueStarted;

    /** ������ �������� */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueEnded OnDialogueEnded;

    /** ���� � ����� ��� */
  UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnNodeEntered OnNodeEntered;

    /** �������� ������� ������ ��� ������ */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoicesReady OnChoicesReady;

    /** ����� ������ ������� */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoiceSelected OnChoiceSelected;

  /** NEW v1.3: ��������� ������� ���������� */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueStateChanged OnStateChanged;

    //~ End Events

protected:
    /** ����������� asset ������� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueDataAsset> LoadedDialogue;

    /** ������� ������� �������� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueSessionContext> CurrentContext;

    /** ������� ��� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> CurrentNode;

    /** ������� ����� (��� ������ "�����") */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDialogueNode>> NodeHistory;

    /** Evaluator ������� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueConditionEvaluator> ConditionEvaluator;

    /** Executor �������� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueEffectExecutor> EffectExecutor;

    /** ��� ����� (NodeId -> Node) */
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDialogueNode>> NodeCache;

    /** DEPRECATED v1.3: Use StateMachine instead */
    UPROPERTY(Transient)
    bool bIsActive;

    /** NEW v1.3: State Machine ��� ���������� ���������� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueStateMachine> StateMachine;

    /** Command invoker ��� ���������� ��������� (v1.2) */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueCommandInvoker> CommandInvoker;

    /** �������� ������ ������ � ������� */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dialogue|Commands")
    bool bEnableCommandHistory;

    /** Handle ��� ������� auto-advance */
    FTimerHandle AutoAdvanceTimerHandle;

public:
    //~ Begin Lifecycle

    /** ������ ������ */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void StartDialogue(UDialogueDataAsset* InDialogue, const TArray<UObject*>& InParticipants);

    /** ��������� ������ */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void EndDialogue();

    /** ����� (��� ����������) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void PauseDialogue();

    /** ���������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void ResumeDialogue();

    //~ End Lifecycle

    //~ Begin Navigation

    /** ������� � ���� �� ID */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoToNode(FName NodeId);

 /** ������� � ����������� ���� (�� ������) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool JumpToNode(UDialogueNode* TargetNode);

  /** ������� ������� ������� (�� �������) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool SelectChoice(int32 ChoiceIndex);

    /** ��������� � ����������� ���� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoBack();

    /** ���������� ������� ������� (auto-advance) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    void Skip();

  //~ End Navigation

    //~ Begin Queries

    /** �������� ������� ��� */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueNode* GetCurrentNode() const { return CurrentNode; }

    /** �������� �������� */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueSessionContext* GetContext() const { return CurrentContext; }

    /** DEPRECATED v1.3: Use GetCurrentState() instead */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State", meta = (DeprecatedFunction, DeprecationMessage = "Use GetCurrentState() instead"))
    bool IsActive() const;

    /** NEW v1.3: Get current dialogue state */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    EDialogueState GetCurrentState() const;

    /** NEW v1.3: Get state machine */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
 UDialogueStateMachine* GetStateMachine() const { return StateMachine; }

    /** �������� ����������� DialogueAsset */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueDataAsset* GetLoadedDialogue() const { return LoadedDialogue; }

    /** �������� ��������� �������� ������� */
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

protected:
 //~ Begin Internal Logic

    /** ������������� ���� ����� */
    void BuildNodeCache();

    /** ��������� ���� */
    void ProcessNode(UDialogueNode* Node);

    /** ��������� ������� ���� */
    void ApplyNodeEffects(UDialogueNode* Node);

    /** ��������� ��������� ��� */
    UDialogueNode* ComputeNextNode(UDialogueNode* FromNode);

    /** ������� �������� ������� (� ������ �������) */
    TArray<UDialogueNode*> GatherChoices(UDialogueNode* FromNode) const;

/** ��������� Random ���� */
    UDialogueNode* ProcessRandomNode(UDialogueNode* RandomNode);

    /** ��������� Condition ���� */
    UDialogueNode* ProcessConditionNode(UDialogueNode* ConditionNode);

    /** �������� ����������� ���������� */
    bool IsConnectionAvailable(const FDialogueConnection& Connection) const;

    /** �������������� ����������� (���� �����) */
    void SetupAutoAdvance(UDialogueNode* Node);

    /** Callback ��� ������� auto-advance */
    void OnAutoAdvanceTimer();

    /** NEW v1.3: Check if operation is allowed in current state */
    bool CanPerformOperation(FName OperationName) const;

    /** NEW v1.3: Callback when state changes */
    void OnStateMachineStateChanged(EDialogueState OldState, EDialogueState NewState);

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
