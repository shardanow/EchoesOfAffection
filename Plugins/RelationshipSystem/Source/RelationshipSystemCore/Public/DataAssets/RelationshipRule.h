// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/RelationshipDataAsset.h"
#include "GameplayTagContainer.h"
#include "RelationshipRule.generated.h"

class URelationshipComponent;

/**
 * Comparison operators for rule conditions
 */
UENUM(BlueprintType)
enum class ERelationshipComparisonOp : uint8
{
	Equal,
	NotEqual,
	Greater,
	GreaterOrEqual,
	Less,
	LessOrEqual
};

/**
 * Single condition in a rule
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipCondition
{
	GENERATED_BODY()

	/** Dimension to check */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (Categories = "Relationship.Dimension"))
	FGameplayTag DimensionTag;

	/** Comparison operator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	ERelationshipComparisonOp Operator;

	/** Value to compare against (can be literal or reference another dimension) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	float CompareValue;

	/** Use another dimension's value instead of literal (optional) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (Categories = "Relationship.Dimension"))
	FGameplayTag CompareDimensionTag;

	FRelationshipCondition()
		: Operator(ERelationshipComparisonOp::Greater)
		, CompareValue(0.0f)
	{
	}

	/** Evaluate this condition */
	bool Evaluate(float ActualValue, float CompareAgainst) const;
};

/**
 * Formula for calculating values
 * Simple expression language: "Trust * 0.5 + Friendship - 10"
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FRelationshipFormula
{
	GENERATED_BODY()

	/** Formula expression (e.g., "Trust * 0.5 + Friendship - 10") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Formula", meta = (MultiLine = true))
	FString Expression;

	/** Cached parsed tokens (for performance) */
	UPROPERTY(Transient)
	TArray<FString> ParsedTokens;

	/** Is formula valid? */
	UPROPERTY(Transient)
	bool bIsValid;

	FRelationshipFormula()
		: bIsValid(false)
	{
	}

	/** Parse and validate formula */
	bool Parse();

	/** Evaluate formula with given dimension values */
	float Evaluate(const TMap<FGameplayTag, float>& DimensionValues) const;

	/** Get all dimension tags referenced in formula */
	TArray<FGameplayTag> GetReferencedDimensions() const;
};

/**
 * Defines a rule for evaluating relationship conditions
 * Used for state transitions, action availability, quest gates, etc.
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API URelationshipRule : public URelationshipDataAsset
{
	GENERATED_BODY()

public:
	URelationshipRule();

	/** Conditions that must all be true (AND logic) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	TArray<FRelationshipCondition> Conditions;

	/** Optional formula for complex calculations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule")
	FRelationshipFormula Formula;

	/** If using formula, this is the comparison operator for result */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule", meta = (EditCondition = "Formula.Expression != \"\""))
	ERelationshipComparisonOp FormulaComparisonOp;

	/** If using formula, compare result against this value */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rule", meta = (EditCondition = "Formula.Expression != \"\""))
	float FormulaCompareValue;

	/** Evaluate rule against dimension values */
	UFUNCTION(BlueprintCallable, Category = "Rule")
	bool Evaluate(const TMap<FGameplayTag, float>& DimensionValues) const;

	/** Get all dimensions referenced by this rule */
	UFUNCTION(BlueprintCallable, Category = "Rule")
	TArray<FGameplayTag> GetReferencedDimensions() const;

protected:
	virtual bool ValidateInternal(TArray<FText>& OutErrors) const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

/**
 * Formula interpreter/evaluator
 * Handles parsing and evaluation of simple mathematical expressions
 */
class RELATIONSHIPSYSTEMCORE_API FRelationshipFormulaInterpreter
{
public:
	/** Parse expression into tokens */
	static bool Parse(const FString& Expression, TArray<FString>& OutTokens);

	/** Evaluate parsed tokens with dimension values */
	static float Evaluate(const TArray<FString>& Tokens, const TMap<FGameplayTag, float>& DimensionValues);

	/** Extract dimension tags from expression */
	static TArray<FGameplayTag> ExtractDimensions(const FString& Expression);

	/** Validate expression syntax */
	static bool Validate(const FString& Expression, FString& OutError);

private:
	/** Tokenize expression */
	static void Tokenize(const FString& Expression, TArray<FString>& OutTokens);

	/** Evaluate postfix expression */
	static float EvaluatePostfix(const TArray<FString>& PostfixTokens, const TMap<FGameplayTag, float>& DimensionValues);

	/** Convert infix to postfix (Shunting-yard algorithm) */
	static void InfixToPostfix(const TArray<FString>& InfixTokens, TArray<FString>& OutPostfixTokens);

	/** Get operator precedence */
	static int32 GetPrecedence(const FString& Op);

	/** Check if token is operator */
	static bool IsOperator(const FString& Token);

	/** Check if token is number */
	static bool IsNumber(const FString& Token);
};
