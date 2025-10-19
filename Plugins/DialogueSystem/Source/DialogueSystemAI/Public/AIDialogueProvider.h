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
 * ��������� AI ���������
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMAI_API FAIDialogueResponse
{
    GENERATED_BODY()

    /** ������� �� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool bSuccess = false;

    /** ��������������� ����� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FText GeneratedText;

    /** ������ (���� ����) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FString ErrorMessage;

    /** ������������ ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FGameplayTag SuggestedEmotion;

    /** ��������� ���������� (������������) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SuggestedAffinityDelta = 0.0f;
};

/**
 * ������� ��� async ���������
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FAIDialogueResponseDelegate, FAIDialogueResponse, Response);

/**
 * ������� ����� ��� AI ���������� ��������
 * 
 * ������������:
 * - OpenAI API
 * - Anthropic Claude
 * - Local LLMs (LM Studio, Ollama)
 * - ��������� endpoints
 * 
 * �����������:
 * - Async ������� (�� ��������� ����)
 * - ����������� �������
 * - Rate limiting
 * - Fallback �� ��������� �����
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class DIALOGUESYSTEMAI_API UAIDialogueProvider : public UObject
{
    GENERATED_BODY()

protected:
    /** API Key (������ � Project Settings � � .env ����) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    FString APIKey;

    /** ������ (��������: gpt-4, gpt-3.5-turbo) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    FString Model = TEXT("gpt-4");

    /** ����������� (0.0 - 2.0) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config", meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float Temperature = 0.7f;

    /** Max ������� ������ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    int32 MaxTokens = 150;

    /** ��� ������� (hash -> response) */
    UPROPERTY(Transient)
    TMap<FString, FAIDialogueResponse> ResponseCache;

public:
    //~ Begin Public API

    /** ������������� ������� (async) */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void GenerateDialogue(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt,
        FAIDialogueResponseDelegate OnComplete);

    /** ������������� ������� (sync, ��� ������) */
    UFUNCTION(BlueprintCallable, Category = "AI")
    FAIDialogueResponse GenerateDialogueSync(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt);

    /** ��������� ����������� API */
    UFUNCTION(BlueprintPure, Category = "AI")
    virtual bool IsAPIAvailable() const;

    /** �������� ��� */
    UFUNCTION(BlueprintCallable, Category = "AI")
    void ClearCache();

    //~ End Public API

protected:
    //~ Begin Internal API (can be overridden in C++ or BP)

    /** ��������� ������ */
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    FString BuildPrompt(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt) const;

    /** ��������� HTTP ������ */
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    void SendAPIRequest(
        const FString& Prompt,
        const FAIDialogueResponseDelegate& OnComplete);

    /** ������� JSON ������ */
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    FAIDialogueResponse ParseAPIResponse(const FString& ResponseJson) const;

    //~ End Internal API
};

/**
 * ���������� ��� OpenAI API
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
 * ������ ������ (helper)
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMAI_API UPromptBuilder : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** ��������� system prompt */
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildSystemPrompt(const FDialoguePersonaData& PersonaData);

    /** ��������� context prompt (��������, ���������� � �����������, �������� � �.�.) */
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildContextPrompt(const UDialogueSessionContext* Context);

    /** ��������� few-shot ������� */
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildFewShotExamples(const FDialoguePersonaData& PersonaData);

    /** ������ ������ */
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildFullPrompt(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt);
};
