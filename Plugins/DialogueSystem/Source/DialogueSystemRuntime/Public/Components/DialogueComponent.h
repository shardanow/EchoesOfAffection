// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IDialogueParticipant.h"
#include "DialogueComponent.generated.h"

// Forward declarations
class UDialogueRunner;
class UDialogueDataAsset;
class UDialogueSessionContext;
class URelationshipComponent;
class UNPCMemoryComponent;
class IDialogueService;
class UDialogueNode;

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStartedSignature, UDialogueRunner*, Runner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEndedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueNodeChangedSignature, UDialogueNode*, NewNode, UDialogueNode*, PreviousNode);

/**
 * ��������� ������� ��� NPC
 * �������� ��:
 * - �������� ��������� ��������
 * - ���������� � �����������
 * - ����� � Relationship Component
 * - ���������� ���������� �������
 * 
 * Uses Dependency Injection via IDialogueService interface
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API UDialogueComponent : public UActorComponent, public IDialogueParticipant
{
    GENERATED_BODY()

public:
    UDialogueComponent();

    //~ Begin Delegates

    /** Called when dialogue starts */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueStartedSignature OnDialogueStarted;

    /** Called when dialogue ends */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueEndedSignature OnDialogueEnded;

    /** Called when dialogue node changes (new line/response) */
    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnDialogueNodeChangedSignature OnDialogueNodeChanged;

    //~ End Delegates

protected:
    //~ Begin Configuration

    /** �������� ������ NPC (fallback) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSoftObjectPtr<UDialogueDataAsset> DefaultDialogue;

    /** �������������� ������� (�� ��������/���������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, TSoftObjectPtr<UDialogueDataAsset>> ConditionalDialogues;

    /** ID ��������� (��� ������� ���������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName CharacterId;

    /** ������������ ��� ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText DisplayName;

    /** ������� ��� UI */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSoftObjectPtr<UTexture2D> Portrait;

    /** ����� �� ������ ������ ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bCanStartDialogue = true;

    /** ��������� �������������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    float InteractionDistance = 300.0f;

    //~ End Configuration

    //~ Begin Runtime State

    /** ������� Runner (���� ������ �������) */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueRunner> ActiveRunner;

    /** ���������� ���� (��� ������������ ���������) */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> PreviousNode;

    /** Relationship Component (cached) */
    UPROPERTY(Transient)
    TObjectPtr<URelationshipComponent> RelationshipComp;

    /** Memory Component (cached) */
    UPROPERTY(Transient)
    TObjectPtr<UNPCMemoryComponent> MemoryComp;

    /** ��������� Context */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueSessionContext> LastContext;

    /** Cooldown �� ���������� ������� */
    UPROPERTY(Transient)
    float DialogueCooldown = 0.0f;

    //~ End Runtime State

public:
    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface

    //~ Begin IDialogueParticipant Interface
    virtual FName GetParticipantId_Implementation() const override { return CharacterId; }
    virtual FText GetParticipantDisplayName_Implementation() const override { return DisplayName; }
    virtual UTexture2D* GetParticipantPortrait_Implementation() const override;
    virtual bool CanStartDialogue_Implementation(AActor* Initiator) const override;
    virtual void OnDialogueStarted_Implementation(UDialogueRunner* Runner) override;
    virtual void OnDialogueEnded_Implementation() override;
    //~ End IDialogueParticipant Interface

    //~ Begin Public API

    /** ������ ������ � ���� NPC */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool StartDialogue(AActor* Initiator);

    /** ������� ���������� ������ �� ������ ��������� */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    UDialogueDataAsset* SelectDialogue(const UDialogueSessionContext* Context);

    /** �������� Relationship Component */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    URelationshipComponent* GetRelationshipComponent() const { return RelationshipComp; }

    /** �������� �������� Runner */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    UDialogueRunner* GetActiveRunner() const { return ActiveRunner; }

    /** ������ �������? */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    bool IsInDialogue() const { return ActiveRunner != nullptr; }

    /** ���������� cooldown */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SetDialogueCooldown(float Seconds) { DialogueCooldown = Seconds; }

    /** �������� Memory Component */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    UNPCMemoryComponent* GetMemoryComponent() const { return MemoryComp; }

    //~ End Public API

protected:
    /**
     * �������� ������ �������� ����� ��������� ������������
     * ������������ �������������� �� ������ �����������
     * @return ��������� DialogueService ��� nullptr
     */
    IDialogueService* GetDialogueService() const;

    /** Callback: ������ �������� */
    UFUNCTION()
    void HandleDialogueEnded();

    /** Callback: ������� �� ����� ���� */
    UFUNCTION()
    void HandleNodeEntered(UDialogueNode* NewNode);
};
