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
 * 
 */
USTRUCT(BlueprintType)
struct DIALOGUESYSTEMAI_API FAIDialogueResponse
{
    GENERATED_BODY()

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    bool bSuccess = false;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FText GeneratedText;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FString ErrorMessage;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FGameplayTag SuggestedEmotion;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SuggestedAffinityDelta = 0.0f;
};

/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FAIDialogueResponseDelegate, FAIDialogueResponse, Response);

/**
 * 
 * 
 * 
 * - OpenAI API
 * - Anthropic Claude
 * - Local LLMs (LM Studio, Ollama)
 * - 
 * 
 * 
 * - Async )
 * - 
 * - Rate limiting
 * - Fallback 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class DIALOGUESYSTEMAI_API UAIDialogueProvider : public UObject
{
    GENERATED_BODY()

protected:
    /** API Key () */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    FString APIKey;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    FString Model = TEXT("gpt-4");

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config", meta = (ClampMin = "0.0", ClampMax = "2.0"))
    float Temperature = 0.7f;

    /** Max tokens in response */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Config")
    int32 MaxTokens = 150;

    /** ) */
    UPROPERTY(Transient)
    TMap<FString, FAIDialogueResponse> ResponseCache;

public:
    //~ Begin Public API

    
    UFUNCTION(BlueprintCallable, Category = "AI")
    void GenerateDialogue(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt,
        FAIDialogueResponseDelegate OnComplete);

    
    UFUNCTION(BlueprintCallable, Category = "AI")
    FAIDialogueResponse GenerateDialogueSync(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt);

    
    UFUNCTION(BlueprintPure, Category = "AI")
    virtual bool IsAPIAvailable() const;

    
    UFUNCTION(BlueprintCallable, Category = "AI")
    void ClearCache();

    //~ End Public API

protected:
    //~ Begin Internal API (can be overridden in C++ or BP)

    
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    FString BuildPrompt(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt) const;

    
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    void SendAPIRequest(
        const FString& Prompt,
        const FAIDialogueResponseDelegate& OnComplete);

    
    UFUNCTION(BlueprintNativeEvent, Category = "AI")
    FAIDialogueResponse ParseAPIResponse(const FString& ResponseJson) const;

    //~ End Internal API
};

/**
 * 
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
 * )
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMAI_API UPromptBuilder : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildSystemPrompt(const FDialoguePersonaData& PersonaData);

    
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildContextPrompt(const UDialogueSessionContext* Context);

    
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildFewShotExamples(const FDialoguePersonaData& PersonaData);

    
    UFUNCTION(BlueprintPure, Category = "AI|Prompt")
    static FString BuildFullPrompt(
        const FDialoguePersonaData& PersonaData,
        const UDialogueSessionContext* Context,
        const FString& UserPrompt);
};
