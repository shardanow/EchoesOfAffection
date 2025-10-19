// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/DialogueNode.h"
#include "DialogueDataAsset.generated.h"

// Forward declarations
class UDialogueSessionContext;

/**
 * Данные персоны (личности) NPC
 * Используется для модификации стилей общения и AI промптов
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMCORE_API FDialoguePersonaData
{
    GENERATED_BODY()

    /** ID персоны */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    FName PersonaId;

    /** Отображаемое имя */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    FText DisplayName;

    /** Краткое описание личности */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona", meta = (MultiLine = true))
    FText Description;

    /** Основные черты характера (теги) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persona")
    FGameplayTagContainer PersonalityTraits; // Trait.Shy, Trait.Confident, etc

    /** AI Prompt (system message) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (MultiLine = true))
    FString AISystemPrompt;

    /** Примеры реплик для Few-Shot Learning */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TArray<FString> ExamplePhrases;

    /** Модификаторы отношений */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float PositiveSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float NegativeSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship")
    float ForgivenessRate = 1.0f;
};

/**
 * Главный Asset диалога
 * Содержит весь граф узлов, рёбер и метаданные
 * 
 * Data-driven подход: всё редактируется в DataTable или Editor,
 * а runtime только читает и выполняет
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    //~ Begin Metadata

    /** Уникальный ID диалога */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FName DialogueId;

    /** Отображаемое название (для редактора) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FText DisplayName;

    /** Описание диалога */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata", meta = (MultiLine = true))
    FText Description;

    /** Теги для фильтрации/поиска */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    FGameplayTagContainer DialogueTags;

    /** Минимальный уровень отношений для доступа */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    float MinAffinityRequired = -100.0f;

    /** Версия диалога (для миграции) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Metadata")
    int32 Version = 1;

    //~ End Metadata

    //~ Begin Graph Data

    /** Все узлы диалога */
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Graph")
    TArray<TObjectPtr<UDialogueNode>> Nodes;

    /** ID стартового узла (по умолчанию) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Graph")
    FName StartNode;

    //~ End Graph Data

    //~ Begin Persona Data

    /** Данные персоны главного участника */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persona")
    FDialoguePersonaData PrimaryPersona;

    /** Дополнительные персоны (для групповых диалогов) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persona")
    TMap<FName, FDialoguePersonaData> AdditionalPersonas;

    //~ End Persona Data

    //~ Begin Localization

    /** Namespace для локализации */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Localization")
    FString LocalizationNamespace;

    /** Поддерживаемые языки */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Localization")
    TArray<FString> SupportedCultures;

    //~ End Localization

    //~ Begin AI Integration

    /** Разрешить AI генерацию для этого диалога */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    bool bAllowAIGeneration = false;

    /** Температура для AI (креативность) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float AITemperature = 0.7f;

    /** Max токенов для ответа */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    int32 AIMaxTokens = 150;

    //~ End AI Integration

public:
    //~ Begin API

    /** Найти узел по ID */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    UDialogueNode* FindNode(FName NodeId) const;

    /** Получить исходящие рёбра узла */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    TArray<FDialogueEdgeData> GetOutgoingEdges(FName FromNodeId) const;

    /** Получить стартовый узел с учётом контекста */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    FName GetStartNode(const UDialogueSessionContext* Context) const;

    /** Валидация графа (для редактора) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    bool ValidateGraph(TArray<FString>& OutErrors) const;

    /** Экспорт в JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    FString ExportToJson() const;

    /** Импорт из JSON */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Data")
    bool ImportFromJson(const FString& Json);

    //~ End API

protected:
    //~ Begin UObject Interface
    virtual void PostLoad() override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    //~ End UObject Interface

    /** Построить индекс для быстрого поиска */
    void RebuildIndex();

    /** Индекс узлов */
    UPROPERTY(Transient)
    TMap<FName, int32> NodeIndex;
};
