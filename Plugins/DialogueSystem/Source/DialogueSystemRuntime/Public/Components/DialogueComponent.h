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

/**
 * Компонент диалога для NPC
 * Отвечает за:
 * - Хранение доступных диалогов
 * - Интеграцию с интеракцией
 * - Связь с Relationship Component
 * - Управление состоянием диалога
 */
UCLASS(ClassGroup=(Dialogue), meta=(BlueprintSpawnableComponent))
class DIALOGUESYSTEMRUNTIME_API UDialogueComponent : public UActorComponent, public IDialogueParticipant
{
    GENERATED_BODY()

public:
    UDialogueComponent();

protected:
    //~ Begin Configuration

    /** Основной диалог NPC (fallback) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSoftObjectPtr<UDialogueDataAsset> DefaultDialogue;

    /** Дополнительные диалоги (по условиям/контексту) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TMap<FName, TSoftObjectPtr<UDialogueDataAsset>> ConditionalDialogues;

    /** ID персонажа (для системы отношений) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName CharacterId;

    /** Отображаемое имя персонажа */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText DisplayName;

    /** Портрет для UI */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSoftObjectPtr<UTexture2D> Portrait;

    /** Можно ли начать диалог сейчас */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool bCanStartDialogue = true;

    /** Дистанция взаимодействия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    float InteractionDistance = 300.0f;

    //~ End Configuration

    //~ Begin Runtime State

    /** Текущий Runner (если диалог активен) */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueRunner> ActiveRunner;

    /** Relationship Component (cached) */
    UPROPERTY(Transient)
    TObjectPtr<URelationshipComponent> RelationshipComp;

    /** Последний Context */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueSessionContext> LastContext;

    /** Cooldown до следующего диалога */
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

    /** Начать диалог с этим NPC */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    bool StartDialogue(AActor* Initiator);

    /** Выбрать подходящий диалог на основе контекста */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    UDialogueDataAsset* SelectDialogue(const UDialogueSessionContext* Context);

    /** Получить Relationship Component */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    URelationshipComponent* GetRelationshipComponent() const { return RelationshipComp; }

    /** Получить активный Runner */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    UDialogueRunner* GetActiveRunner() const { return ActiveRunner; }

    /** Диалог активен? */
    UFUNCTION(BlueprintPure, Category = "Dialogue")
    bool IsInDialogue() const { return ActiveRunner != nullptr; }

    /** Установить cooldown */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SetDialogueCooldown(float Seconds) { DialogueCooldown = Seconds; }

    //~ End Public API

protected:
    /** Callback: диалог завершён */
    UFUNCTION()
    void HandleDialogueEnded();
};
