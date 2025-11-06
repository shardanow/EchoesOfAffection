// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "DialogueConditionEvaluator.generated.h"

// Forward declarations
class UDialogueSessionContext;

/**
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DIALOGUESYSTEMCORE_API UDialogueCondition : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Condition")
    bool Evaluate(const UDialogueSessionContext* Context) const;
    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const { return true; }

    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Condition")
    FString GetDescription() const;
    virtual FString GetDescription_Implementation() const { return TEXT("Unknown Condition"); }
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_And : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Instanced, Category = "Condition")
    TArray<TObjectPtr<UDialogueCondition>> Conditions;

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_Or : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Instanced, Category = "Condition")
    TArray<TObjectPtr<UDialogueCondition>> Conditions;

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_Not : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Instanced, Category = "Condition")
    TObjectPtr<UDialogueCondition> Condition;

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_Affinity : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FName NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    float MinValue = 0.0f;

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_HasItem : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    int32 MinCount = 1;

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_Memory : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FName MemoryKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    bool ExpectedValue = true;

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_TimeOfDay : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FGameplayTag RequiredTime; // Time.Morning, Time.Evening, etc

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_WorldState : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FGameplayTag RequiredTag;

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueCondition_CustomVariable : public UDialogueCondition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FName VariableKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FString ExpectedValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    bool bNumericComparison = false;

    virtual bool Evaluate_Implementation(const UDialogueSessionContext* Context) const override;
    virtual FString GetDescription_Implementation() const override;
};

/**
 * 
 * - "affinity[Lianne] >= 50"
 * - "hasItem(Rose) && time == Evening"
 * - "!visited(Node_002) || memory(FirstMeeting)"
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueConditionEvaluator : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Conditions")
    UDialogueCondition* ParseCondition(const FString& ConditionString);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Conditions")
    bool EvaluateString(const FString& ConditionString, const UDialogueSessionContext* Context);

    UFUNCTION(BlueprintCallable, Category = "Dialogue|Conditions")
    bool EvaluateCondition(const UDialogueCondition* Condition, const UDialogueSessionContext* Context);

protected:
    TArray<FString> TokenizeExpression(const FString& Expression);

    UDialogueCondition* ParseExpression(const TArray<FString>& Tokens, int32& Index);

    UDialogueCondition* ParseTerm(const TArray<FString>& Tokens, int32& Index);

    UDialogueCondition* ParseAtom(const TArray<FString>& Tokens, int32& Index);

    /** Helper methods for parsing specific condition types */
    UDialogueCondition* ParseHasItemCondition(const TArray<FString>& Tokens, int32& Index);
    UDialogueCondition* ParseMemoryCondition(const TArray<FString>& Tokens, int32& Index);
    UDialogueCondition* ParseVisitedCondition(const TArray<FString>& Tokens, int32& Index);
    UDialogueCondition* ParseTagCondition(const TArray<FString>& Tokens, int32& Index);
    UDialogueCondition* ParseAffinityCondition(const TArray<FString>& Tokens, int32& Index);
    UDialogueCondition* ParseVariableCondition(const TArray<FString>& Tokens, int32& Index);
    UDialogueCondition* ParseTimeCondition(const TArray<FString>& Tokens, int32& Index);

    /** ??? ???????????? ??????? (??? ???????????) */
    UPROPERTY(Transient)
    TMap<FString, TObjectPtr<UDialogueCondition>> ConditionCache;
};
