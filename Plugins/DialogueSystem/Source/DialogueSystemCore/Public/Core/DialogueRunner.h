// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueContext.h"
#include "DialogueRunner.generated.h"

// Forward declarations
class UDialogueDataAsset;
class UDialogueConditionEvaluator;
class UDialogueEffectExecutor;
class UDialogueSessionContext;

/**
 * Делегаты для событий диалога
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, FName, DialogueId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeEntered, UDialogueNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicesReady, const TArray<UDialogueNode*>&, Choices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoiceSelected, int32, ChoiceIndex, UDialogueNode*, ChoiceNode);

/**
 * Основной класс выполнения диалогов
 * Управляет state machine, переходами, условиями и эффектами
 * 
 * Принципы:
 * - Полностью data-driven (не зависит от конкретных NPC)
 * - Event-based (не использует Tick)
 * - Stateless между сессиями (весь state в UDialogueSessionContext)
 * - Thread-safe для интеграции с AI
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

    /** Диалог завершён */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueEnded OnDialogueEnded;

    /** Вошли в новый узел */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnNodeEntered OnNodeEntered;

    /** Готовы варианты выбора для игрока */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoicesReady OnChoicesReady;

    /** Игрок выбрал вариант */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoiceSelected OnChoiceSelected;

    //~ End Events

protected:
    /** Загруженный asset диалога */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueDataAsset> LoadedDialogue;

    /** Текущий контекст выполнения */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueSessionContext> CurrentContext;

    /** Текущий узел */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> CurrentNode;

    /** История узлов (для кнопки "Назад") */
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDialogueNode>> NodeHistory;

    /** Evaluator условий */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueConditionEvaluator> ConditionEvaluator;

    /** Executor эффектов */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueEffectExecutor> EffectExecutor;

    /** Кэш узлов (NodeId -> Node) */
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDialogueNode>> NodeCache;

    /** Активна ли сессия */
    UPROPERTY(Transient)
    bool bIsActive;

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

    /** Приостановить (без завершения) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void PauseDialogue();

    /** Возобновить */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void ResumeDialogue();

    //~ End Lifecycle

    //~ Begin Navigation

    /** Перейти к узлу по ID */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoToNode(FName NodeId);

    /** Перейти к конкретному узлу (по объекту) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool JumpToNode(UDialogueNode* TargetNode);

    /** Выбрать вариант ответа (по индексу) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool SelectChoice(int32 ChoiceIndex);

    /** Вернуться к предыдущему узлу */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoBack();

    /** Пропустить текущую реплику (auto-advance) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    void Skip();

    //~ End Navigation

    //~ Begin Queries

    /** Получить текущий узел */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueNode* GetCurrentNode() const { return CurrentNode; }

    /** Получить контекст */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueSessionContext* GetContext() const { return CurrentContext; }

    /** Активен ли диалог */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    bool IsActive() const { return bIsActive; }

    /** Получить загруженный DialogueAsset */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueDataAsset* GetLoadedDialogue() const { return LoadedDialogue; }

    /** Получить доступные варианты выбора */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    TArray<UDialogueNode*> GetAvailableChoices() const;

    //~ End Queries

protected:
    //~ Begin Internal Logic

    /** Инициализация кэша узлов */
    void BuildNodeCache();

    /** Обработка входа в узел */
    void ProcessNode(UDialogueNode* Node);

    /** Применить эффекты узла */
    void ApplyNodeEffects(UDialogueNode* Node);

    /** Вычислить следующий узел (с учётом условий) */
    UDialogueNode* ComputeNextNode(UDialogueNode* FromNode);

    /** Собрать доступные выборы (с фильтрацией по условиям) */
    TArray<UDialogueNode*> GatherChoices(UDialogueNode* FromNode) const;

    /** Обработка Random узла */
    UDialogueNode* ProcessRandomNode(UDialogueNode* RandomNode);

    /** Обработка Condition узла */
    UDialogueNode* ProcessConditionNode(UDialogueNode* ConditionNode);

    /** Проверка доступности соединения */
    bool IsConnectionAvailable(const FDialogueConnection& Connection) const;

    /** Автопереход (если нужен) */
    void SetupAutoAdvance(UDialogueNode* Node);

    /** Callback для таймера auto-advance */
    void OnAutoAdvanceTimer();

    //~ End Internal Logic
};
