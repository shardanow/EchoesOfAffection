// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Data/DialogueDataAsset.h"  // For FDialoguePersonaData
#include "AIDialogueProvider.generated.h"

// Forward declarations
class UDialogueSessionContext;

/**
 * Результат AI генерации
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMAI_API FAIDialogueResponse
{
    GENERATED_BODY()

    /** Успешно ли */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool bSuccess = false;

    /** Сгенерированный текст */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FText GeneratedText;

    /** Ошибка (если есть) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FString ErrorMessage;

    /** Предложенная эмоция */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FGameplayTag SuggestedEmotion;

    /** Изменение аффинности (предложенное) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SuggestedAffinityDelta = 0.0f;
};

/**
 * Делегат для async генерации
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FAIDialogueResponseDelegate, FAIDialogueResponse, Response);

/**
 * Базовый класс для AI провайдера диалогов
 * 
 * Поддерживает:
 * - OpenAI API
 * - Anthropic Claude
 * - Local LLMs (LM Studio, Ollama)
 * - Кастомные endpoints
 * 
 * Архитектура:
 * - Async запросы (не блокируют игру)
 * - Кэширование ответов
 * - Rate limiting
 * - Fallback на статичный текст
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class DIALOGUESYSTEMAI_API UAIDialogueProvider : public UObject
{
    GENERATED_BODY()

protected:
    /** API Key (смотри в Project Settings и в .env файл) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    FString APIKey;

    /** Модель (например: gpt-4, gpt-3.5-turbo) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    FString Model = TEXT("gpt-4");

    /** Температура (0.0 - 2.0) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config", meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float Temperature = 0.7f;

    /** Max токенов ответа */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    int32 MaxTokens = 150;

    /** Кэш ответов (hash -> response) */
    UPROPERTY(Transient)
    TMap<FString, FAIDialogueResponse> ResponseCache;

public:
    //~ Begin Public API

    /** Сгенерировать реплику (async) */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void GenerateDialogue(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt,
        FAIDialogueResponseDelegate OnComplete);

    /** Сгенерировать реплику (sync, для тестов) */
    UFUNCTION(BlueprintCallable, Category = "AI")
    FAIDialogueResponse GenerateDialogueSync(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt);

    /** Проверить доступность API */
    UFUNCTION(BlueprintPure, Category = "AI")
    virtual bool IsAPIAvailable() const;

    /** Очистить кэш */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void ClearCache();

    //~ End Public API

protected:
    //~ Begin Internal API (can be overridden in C++ or BP)

    /** Построить промпт */
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    FString BuildPrompt(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt) const;

    /** Отправить HTTP запрос */
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    void SendAPIRequest(
        const FString& Prompt,
        const FAIDialogueResponseDelegate& OnComplete);

    /** Парсинг JSON ответа */
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    FAIDialogueResponse ParseAPIResponse(const FString& ResponseJson) const;

    //~ End Internal API
};

/**
 * Реализация для OpenAI API
 */
UCLASS()
class DIALOGUESYSTEMAI_API UAIDialogueProvider_OpenAI : public UAIDialogueProvider
{
    GENERATED_BODY()

protected:
    virtual FString BuildPrompt_Implementation(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt) const override;

    virtual void SendAPIRequest_Implementation(
        const FString& Prompt,
        const FAIDialogueResponseDelegate& OnComplete) override;

    virtual FAIDialogueResponse ParseAPIResponse_Implementation(const FString& ResponseJson) const override;
};

/**
 * Промпт билдер (helper)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMAI_API UPromptBuilder : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Построить system prompt */
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildSystemPrompt(const FDialoguePersonaData& PersonaData);

    /** Построить context prompt (например, информацию о предыстории, ситуации и т.д.) */
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildContextPrompt(const UDialogueSessionContext* Context);

    /** Построить few-shot примеры */
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildFewShotExamples(const FDialoguePersonaData& PersonaData);

    /** Полный промпт */
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildFullPrompt(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt);
};
