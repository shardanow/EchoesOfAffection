// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "DialogueContext.generated.h"

// Forward declarations
class AActor;
class UDialogueNode;
struct FDialogueSessionSaveData;

/**
 * Запись разговора
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FConversationEntry
{
    GENERATED_BODY()

    /** Имя говорящего */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
    FText SpeakerName;

    /** Текст реплики */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
    FText DialogueText;

    /** Временная метка */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation")
    FDateTime Timestamp;

    /** ID узла */
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
 * Контекст всей сессии (рантайм)
 * 
 * Используется для Condition/Effect вычисления
 * Содержит:
 * - Player/NPC references
 * - Relationship data
 * - Quest state
 * - Inventory
 * - Все доступные переменные
 * - Историю разговора
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueSessionContext : public UObject
{
    GENERATED_BODY()

public:
    //~ Begin Participants

    /** Игрок */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    TObjectPtr<AActor> Player;

    /** NPC */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    TObjectPtr<AActor> NPC;

    //~ End Participants

    //~ Begin Relationship

    /** Начальное значение отношений между актерами */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    float BaseAffinityLevel = 0.0f;

    //~ End Relationship

    //~ Begin Conversation State

    /** Текущий выполняемый узел */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Context")
    TWeakObjectPtr<UDialogueNode> CurrentNode;

    /** История разговора */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Context")
    TArray<FConversationEntry> ConversationHistory;

    /** Посещенные узлы (для защиты циклов) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Context")
    TArray<FName> VisitedNodes;

    /** Время начала сессии */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Context")
    FDateTime SessionStartTime;

    //~ End Conversation State

    //~ Begin Custom Variables

    /** Пользовательские переменные (для Conditions/Effects) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    TMap<FName, FString> CustomVariables;

    /** Теги, установленные во время диалога */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context")
    FGameplayTagContainer ActiveTags;

    //~ End Custom Variables

public:
    //~ Begin UObject Interface
    virtual void BeginDestroy() override;
    //~ End UObject Interface

    //~ Begin Public API

    /** Получить пользовательскую переменную */
    UFUNCTION(BlueprintPure, Category = "Context")
    FString GetCustomVariable(FName VariableName, const FString& DefaultValue = TEXT("")) const;

    /** Установить пользовательскую переменную */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void SetCustomVariable(FName VariableName, const FString& Value);

    /** Добавить тег */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void AddTag(FGameplayTag Tag);

    /** Удалить тег */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void RemoveTag(FGameplayTag Tag);

    /** Проверить наличие тега */
    UFUNCTION(BlueprintPure, Category = "Context")
    bool HasTag(FGameplayTag Tag) const;

    /** Добавить в историю */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void AddToHistory(const FText& SpeakerName, const FText& DialogueText, FName NodeId);

    /** Отметить посещенный узел */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void MarkNodeVisited(FName NodeId);

    /** Проверка узла был посещен */
    UFUNCTION(BlueprintPure, Category = "Context")
    bool WasNodeVisited(FName NodeId) const;

    //~ End Public API

    //~ Begin Save/Load API

    /** Преобразовать в SaveData структуру */
    UFUNCTION(BlueprintCallable, Category = "Context|SaveGame")
    FDialogueSessionSaveData ToSaveData(FName DialogueId) const;

    /** Восстановить из SaveData структуры */
    UFUNCTION(BlueprintCallable, Category = "Context|SaveGame")
    bool FromSaveData(const FDialogueSessionSaveData& SaveData, AActor* InPlayer, AActor* InNPC);

    /** Сериализация в JSON (legacy) */
    UFUNCTION(BlueprintCallable, Category = "Context")
    void SaveToJson(FString& OutJson) const;

    /** Десериализация из JSON (legacy) */
    UFUNCTION(BlueprintCallable, Category = "Context")
    bool LoadFromJson(const FString& Json);

    //~ End Save/Load API
};
