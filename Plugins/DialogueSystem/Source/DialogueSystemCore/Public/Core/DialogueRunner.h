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
 * �������� ��� ������� �������
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, FName, DialogueId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeEntered, UDialogueNode*, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicesReady, const TArray<UDialogueNode*>&, Choices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoiceSelected, int32, ChoiceIndex, UDialogueNode*, ChoiceNode);

/**
 * �������� ����� ���������� ��������
 * ��������� state machine, ����������, ��������� � ���������
 * 
 * ��������:
 * - ��������� data-driven (�� ������� �� ���������� NPC)
 * - Event-based (�� ���������� Tick)
 * - Stateless ����� �������� (���� state � UDialogueSessionContext)
 * - Thread-safe ��� ���������� � AI
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

    /** ����� � ����� ���� */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnNodeEntered OnNodeEntered;

    /** ������ �������� ������ ��� ������ */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoicesReady OnChoicesReady;

    /** ����� ������ ������� */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoiceSelected OnChoiceSelected;

    //~ End Events

protected:
    /** ����������� asset ������� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueDataAsset> LoadedDialogue;

    /** ������� �������� ���������� */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueSessionContext> CurrentContext;

    /** ������� ���� */
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

    /** ������� �� ������ */
    UPROPERTY(Transient)
    bool bIsActive;

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

    /** ������������� (��� ����������) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void PauseDialogue();

    /** ����������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Control")
    void ResumeDialogue();

    //~ End Lifecycle

    //~ Begin Navigation

    /** ������� � ���� �� ID */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool GoToNode(FName NodeId);

    /** ������� � ����������� ���� (�� �������) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Navigation")
    bool JumpToNode(UDialogueNode* TargetNode);

    /** ������� ������� ������ (�� �������) */
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

    /** �������� ������� ���� */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueNode* GetCurrentNode() const { return CurrentNode; }

    /** �������� �������� */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueSessionContext* GetContext() const { return CurrentContext; }

    /** ������� �� ������ */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    bool IsActive() const { return bIsActive; }

    /** �������� ����������� DialogueAsset */
    UFUNCTION(BlueprintPure, Category = "Dialogue|State")
    UDialogueDataAsset* GetLoadedDialogue() const { return LoadedDialogue; }

    /** �������� ��������� �������� ������ */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|State")
    TArray<UDialogueNode*> GetAvailableChoices() const;

    //~ End Queries

protected:
    //~ Begin Internal Logic

    /** ������������� ���� ����� */
    void BuildNodeCache();

    /** ��������� ����� � ���� */
    void ProcessNode(UDialogueNode* Node);

    /** ��������� ������� ���� */
    void ApplyNodeEffects(UDialogueNode* Node);

    /** ��������� ��������� ���� (� ������ �������) */
    UDialogueNode* ComputeNextNode(UDialogueNode* FromNode);

    /** ������� ��������� ������ (� ����������� �� ��������) */
    TArray<UDialogueNode*> GatherChoices(UDialogueNode* FromNode) const;

    /** ��������� Random ���� */
    UDialogueNode* ProcessRandomNode(UDialogueNode* RandomNode);

    /** ��������� Condition ���� */
    UDialogueNode* ProcessConditionNode(UDialogueNode* ConditionNode);

    /** �������� ����������� ���������� */
    bool IsConnectionAvailable(const FDialogueConnection& Connection) const;

    /** ����������� (���� �����) */
    void SetupAutoAdvance(UDialogueNode* Node);

    /** Callback ��� ������� auto-advance */
    void OnAutoAdvanceTimer();

    //~ End Internal Logic
};
