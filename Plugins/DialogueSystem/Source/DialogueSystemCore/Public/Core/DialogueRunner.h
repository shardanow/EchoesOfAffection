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
 * События для диалога
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, FName, DialogueId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeEntered, UDialogueNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicesReady, const TArray<UDialogueNode*>&, Choices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoiceSelected, int32, ChoiceIndex, UDialogueNode*, ChoiceNode);

/** NEW v1.3: State change event */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueStateChanged, EDialogueState, OldState, EDialogueState, NewState);

/**
 * Основной класс управления диалогом
 * Реализует state machine, навигацию, условия и эффекты
 * 
 * Features (v1.3):
 * - State Machine для управления состоянием
 * - Command Pattern для всех действий
 * - Undo/Redo поддержка
 * - Command History tracking
 * - Replay функциональность
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueRunner : public UObject
{
    GENERATED_BODY()

public:
    UDialogueRunner();
    
    //~ Begin Events

    /** Диалог начат */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueStarted OnDialogueStarted;

    /** Диалог завершен */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueEnded OnDialogueEnded;

    /** Вход в новый нод */
  UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnNodeEntered OnNodeEntered;

    /** Варианты ответов готовы для выбора */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoicesReady OnChoicesReady;

    /** Игрок выбрал реплику */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoiceSelected OnChoiceSelected;

  /** NEW v1.3: Состояние диалога изменилось */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueStateChanged OnStateChanged;

    //~ End Events

protected:
    /** Загруженный asset диалога */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueDataAsset> LoadedDialogue;

    /** Текущий игровой контекст */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueSessionContext> CurrentContext;

    /** Текущий нод */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> CurrentNode;

    /** История нодов (для кнопки "назад") */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDialogueNode>> NodeHistory;

    /** Evaluator условий */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueConditionEvaluator> ConditionEvaluator;

    /** Executor эффектов */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueEffectExecutor> EffectExecutor;

    /** Кеш нодов (NodeId -> Node) */
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDialogueNode>> NodeCache;

    /** DEPRECATED v1.3: Use StateMachine instead */
    UPROPERTY(Transient)
    bool bIsActive;

    /** NEW v1.3: State Machine для управления состоянием */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueStateMachine> StateMachine;

    /** Command invoker для управления командами (v1.2) */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueCommandInvoker> CommandInvoker;

    /** Включить запись команд в историю */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dialogue|Commands")
    bool bEnableCommandHistory;

    /** Handle для таймера auto-advance */
    FTimerHandle AutoAdvanceTimerHandle;

public:
    //~ Begin Lifecycle

    /** Начать диалог */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void StartDialogue(UDialogueDataAsset* InDialogue, const TArray<UObject*>& InParticipants);

    /** Завершить диалог */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void EndDialogue();

    /** Пауза (для сохранения) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void PauseDialogue();

    /** Продолжить */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void ResumeDialogue();

    //~ End Lifecycle

    //~ Begin Navigation

    /** Перейти к ноду по ID */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoToNode(FName NodeId);

 /** Перейти к конкретному ноду (по ссылке) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool JumpToNode(UDialogueNode* TargetNode);

  /** Выбрать игровую реплику (по индексу) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool SelectChoice(int32 ChoiceIndex);

    /** Вернуться к предыдущему ноду */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoBack();

    /** Пропустить текущую реплику (auto-advance) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    void Skip();

  //~ End Navigation

    //~ Begin Queries

    /** Получить текущий нод */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueNode* GetCurrentNode() const { return CurrentNode; }

    /** Получить контекст */
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

    /** Получить загруженный DialogueAsset */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueDataAsset* GetLoadedDialogue() const { return LoadedDialogue; }

    /** Получить доступные варианты ответов */
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

    /** Инициализация кеша нодов */
    void BuildNodeCache();

    /** Обработка ноды */
    void ProcessNode(UDialogueNode* Node);

    /** Применить эффекты ноды */
    void ApplyNodeEffects(UDialogueNode* Node);

    /** Вычислить следующий нод */
    UDialogueNode* ComputeNextNode(UDialogueNode* FromNode);

    /** Собрать варианты ответов (с учетом условий) */
    TArray<UDialogueNode*> GatherChoices(UDialogueNode* FromNode) const;

/** Обработка Random ноды */
    UDialogueNode* ProcessRandomNode(UDialogueNode* RandomNode);

    /** Обработка Condition ноды */
    UDialogueNode* ProcessConditionNode(UDialogueNode* ConditionNode);

    /** Проверка доступности соединения */
    bool IsConnectionAvailable(const FDialogueConnection& Connection) const;

    /** Автоматическое продолжение (если нужно) */
    void SetupAutoAdvance(UDialogueNode* Node);

    /** Callback для таймера auto-advance */
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
