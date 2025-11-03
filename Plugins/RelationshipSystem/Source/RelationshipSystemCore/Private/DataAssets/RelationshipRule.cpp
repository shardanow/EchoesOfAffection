// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/RelationshipRule.h"
#include "RelationshipSystemCore.h"

// FRelationshipCondition implementation

bool FRelationshipCondition::Evaluate(float ActualValue, float CompareAgainst) const
{
	switch (Operator)
	{
	case ERelationshipComparisonOp::Equal:
		return FMath::IsNearlyEqual(ActualValue, CompareAgainst, KINDA_SMALL_NUMBER);
	case ERelationshipComparisonOp::NotEqual:
		return !FMath::IsNearlyEqual(ActualValue, CompareAgainst, KINDA_SMALL_NUMBER);
	case ERelationshipComparisonOp::Greater:
		return ActualValue > CompareAgainst;
	case ERelationshipComparisonOp::GreaterOrEqual:
		return ActualValue >= CompareAgainst || FMath::IsNearlyEqual(ActualValue, CompareAgainst, KINDA_SMALL_NUMBER);
	case ERelationshipComparisonOp::Less:
		return ActualValue < CompareAgainst;
	case ERelationshipComparisonOp::LessOrEqual:
		return ActualValue <= CompareAgainst || FMath::IsNearlyEqual(ActualValue, CompareAgainst, KINDA_SMALL_NUMBER);
	}
	return false;
}

// FRelationshipFormula implementation

bool FRelationshipFormula::Parse()
{
	if (Expression.IsEmpty())
	{
		bIsValid = true; // Empty formula is valid
		ParsedTokens.Empty();
		return true;
	}

	FString ParseError;
	bIsValid = FRelationshipFormulaInterpreter::Validate(Expression, ParseError);
	
	if (bIsValid)
	{
		FRelationshipFormulaInterpreter::Parse(Expression, ParsedTokens);
	}
	else
	{
		UE_LOG(LogRelationshipSystem, Warning, TEXT("Formula parse error: %s"), *ParseError);
	}

	return bIsValid;
}

float FRelationshipFormula::Evaluate(const TMap<FGameplayTag, float>& DimensionValues) const
{
	if (!bIsValid || ParsedTokens.Num() == 0)
	{
		return 0.0f;
	}

	return FRelationshipFormulaInterpreter::Evaluate(ParsedTokens, DimensionValues);
}

TArray<FGameplayTag> FRelationshipFormula::GetReferencedDimensions() const
{
	return FRelationshipFormulaInterpreter::ExtractDimensions(Expression);
}

// URelationshipRule implementation

URelationshipRule::URelationshipRule()
	: FormulaComparisonOp(ERelationshipComparisonOp::Greater)
	, FormulaCompareValue(0.0f)
{
	DisplayColor = FLinearColor(0.3f, 1.0f, 0.3f); // Green default
}

bool URelationshipRule::Evaluate(const TMap<FGameplayTag, float>& DimensionValues) const
{
	// Evaluate all conditions (AND logic)
	for (const FRelationshipCondition& Condition : Conditions)
	{
		const float* ActualValue = DimensionValues.Find(Condition.DimensionTag);
		if (!ActualValue)
		{
			UE_LOG(LogRelationshipSystem, Warning, TEXT("Dimension not found: %s"), *Condition.DimensionTag.ToString());
			return false;
		}

		float CompareAgainst = Condition.CompareValue;
		
		// Use dimension value if specified
		if (Condition.CompareDimensionTag.IsValid())
		{
			if (const float* CompareDimValue = DimensionValues.Find(Condition.CompareDimensionTag))
			{
				CompareAgainst = *CompareDimValue;
			}
		}

		if (!Condition.Evaluate(*ActualValue, CompareAgainst))
		{
			return false; // Condition failed
		}
	}

	// Evaluate formula if present
	if (!Formula.Expression.IsEmpty())
	{
		const float FormulaResult = Formula.Evaluate(DimensionValues);
		FRelationshipCondition FormulaCondition;
		FormulaCondition.Operator = FormulaComparisonOp;
		return FormulaCondition.Evaluate(FormulaResult, FormulaCompareValue);
	}

	return true; // All conditions passed
}

TArray<FGameplayTag> URelationshipRule::GetReferencedDimensions() const
{
	TSet<FGameplayTag> AllDimensions;

	// Collect from conditions
	for (const FRelationshipCondition& Condition : Conditions)
	{
		if (Condition.DimensionTag.IsValid())
		{
			AllDimensions.Add(Condition.DimensionTag);
		}
		if (Condition.CompareDimensionTag.IsValid())
		{
			AllDimensions.Add(Condition.CompareDimensionTag);
		}
	}

	// Collect from formula
	if (!Formula.Expression.IsEmpty())
	{
		TArray<FGameplayTag> FormulaDims = Formula.GetReferencedDimensions();
		AllDimensions.Append(FormulaDims);
	}

	return AllDimensions.Array();
}

bool URelationshipRule::ValidateInternal(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	// Validate conditions
	for (const FRelationshipCondition& Condition : Conditions)
	{
		if (!Condition.DimensionTag.IsValid())
		{
			OutErrors.Add(FText::FromString(TEXT("Condition has invalid dimension tag")));
			bIsValid = false;
		}
	}

	// Validate formula
	if (!Formula.Expression.IsEmpty())
	{
		FString ParseError;
		if (!FRelationshipFormulaInterpreter::Validate(Formula.Expression, ParseError))
		{
			OutErrors.Add(FText::FromString(FString::Printf(TEXT("Formula error: %s"), *ParseError)));
			bIsValid = false;
		}
	}

	return bIsValid;
}

#if WITH_EDITOR
void URelationshipRule::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Re-parse formula when it changes
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FRelationshipFormula, Expression))
	{
		Formula.Parse();
	}
}
#endif

// FRelationshipFormulaInterpreter implementation

bool FRelationshipFormulaInterpreter::Parse(const FString& Expression, TArray<FString>& OutTokens)
{
	TArray<FString> InfixTokens;
	Tokenize(Expression, InfixTokens);
	InfixToPostfix(InfixTokens, OutTokens);
	return true;
}

float FRelationshipFormulaInterpreter::Evaluate(const TArray<FString>& Tokens, const TMap<FGameplayTag, float>& DimensionValues)
{
	return EvaluatePostfix(Tokens, DimensionValues);
}

TArray<FGameplayTag> FRelationshipFormulaInterpreter::ExtractDimensions(const FString& Expression)
{
	TArray<FGameplayTag> Dimensions;
	TArray<FString> Tokens;
	Tokenize(Expression, Tokens);

	for (const FString& Token : Tokens)
	{
		if (!IsOperator(Token) && !IsNumber(Token) && Token != TEXT("(") && Token != TEXT(")"))
		{
			// Assume it's a dimension tag
			FGameplayTag Tag = FGameplayTag::RequestGameplayTag(*Token, false);
			if (Tag.IsValid())
			{
				Dimensions.AddUnique(Tag);
			}
		}
	}

	return Dimensions;
}

bool FRelationshipFormulaInterpreter::Validate(const FString& Expression, FString& OutError)
{
	if (Expression.IsEmpty())
	{
		return true;
	}

	TArray<FString> Tokens;
	Tokenize(Expression, Tokens);

	int32 ParenthesisCount = 0;
	for (const FString& Token : Tokens)
	{
		if (Token == TEXT("("))
		{
			ParenthesisCount++;
		}
		else if (Token == TEXT(")"))
		{
			ParenthesisCount--;
			if (ParenthesisCount < 0)
			{
				OutError = TEXT("Mismatched parenthesis");
				return false;
			}
		}
	}

	if (ParenthesisCount != 0)
	{
		OutError = TEXT("Mismatched parenthesis");
		return false;
	}

	return true;
}

void FRelationshipFormulaInterpreter::Tokenize(const FString& Expression, TArray<FString>& OutTokens)
{
	FString CurrentToken;
	
	for (int32 i = 0; i < Expression.Len(); ++i)
	{
		TCHAR Ch = Expression[i];

		if (FChar::IsWhitespace(Ch))
		{
			if (!CurrentToken.IsEmpty())
			{
				OutTokens.Add(CurrentToken);
				CurrentToken.Empty();
			}
			continue;
		}

		if (Ch == '+' || Ch == '-' || Ch == '*' || Ch == '/' || Ch == '(' || Ch == ')')
		{
			if (!CurrentToken.IsEmpty())
			{
				OutTokens.Add(CurrentToken);
				CurrentToken.Empty();
			}
			OutTokens.Add(FString::Chr(Ch));
		}
		else
		{
			CurrentToken.AppendChar(Ch);
		}
	}

	if (!CurrentToken.IsEmpty())
	{
		OutTokens.Add(CurrentToken);
	}
}

float FRelationshipFormulaInterpreter::EvaluatePostfix(const TArray<FString>& PostfixTokens, const TMap<FGameplayTag, float>& DimensionValues)
{
	TArray<float> Stack;

	for (const FString& Token : PostfixTokens)
	{
		if (IsNumber(Token))
		{
			Stack.Push(FCString::Atof(*Token));
		}
		else if (IsOperator(Token))
		{
			if (Stack.Num() < 2)
			{
				UE_LOG(LogRelationshipSystem, Error, TEXT("Formula evaluation error: insufficient operands"));
				return 0.0f;
			}

			float B = Stack.Pop();
			float A = Stack.Pop();
			float Result = 0.0f;

			if (Token == TEXT("+")) Result = A + B;
			else if (Token == TEXT("-")) Result = A - B;
			else if (Token == TEXT("*")) Result = A * B;
			else if (Token == TEXT("/")) Result = (FMath::Abs(B) > KINDA_SMALL_NUMBER) ? A / B : 0.0f;

			Stack.Push(Result);
		}
		else
		{
			// Dimension tag
			FGameplayTag Tag = FGameplayTag::RequestGameplayTag(*Token, false);
			if (const float* Value = DimensionValues.Find(Tag))
			{
				Stack.Push(*Value);
			}
			else
			{
				UE_LOG(LogRelationshipSystem, Warning, TEXT("Dimension not found in formula: %s"), *Token);
				Stack.Push(0.0f);
			}
		}
	}

	return Stack.Num() > 0 ? Stack[0] : 0.0f;
}

void FRelationshipFormulaInterpreter::InfixToPostfix(const TArray<FString>& InfixTokens, TArray<FString>& OutPostfixTokens)
{
	TArray<FString> OperatorStack;

	for (const FString& Token : InfixTokens)
	{
		if (IsNumber(Token) || (!IsOperator(Token) && Token != TEXT("(") && Token != TEXT(")")))
		{
			OutPostfixTokens.Add(Token);
		}
		else if (Token == TEXT("("))
		{
			OperatorStack.Push(Token);
		}
		else if (Token == TEXT(")"))
		{
			while (OperatorStack.Num() > 0 && OperatorStack.Top() != TEXT("("))
			{
				OutPostfixTokens.Add(OperatorStack.Pop());
			}
			if (OperatorStack.Num() > 0)
			{
				OperatorStack.Pop(); // Remove '('
			}
		}
		else if (IsOperator(Token))
		{
			while (OperatorStack.Num() > 0 && IsOperator(OperatorStack.Top()) &&
				   GetPrecedence(OperatorStack.Top()) >= GetPrecedence(Token))
			{
				OutPostfixTokens.Add(OperatorStack.Pop());
			}
			OperatorStack.Push(Token);
		}
	}

	while (OperatorStack.Num() > 0)
	{
		OutPostfixTokens.Add(OperatorStack.Pop());
	}
}

int32 FRelationshipFormulaInterpreter::GetPrecedence(const FString& Op)
{
	if (Op == TEXT("+") || Op == TEXT("-")) return 1;
	if (Op == TEXT("*") || Op == TEXT("/")) return 2;
	return 0;
}

bool FRelationshipFormulaInterpreter::IsOperator(const FString& Token)
{
	return Token == TEXT("+") || Token == TEXT("-") || Token == TEXT("*") || Token == TEXT("/");
}

bool FRelationshipFormulaInterpreter::IsNumber(const FString& Token)
{
	return Token.IsNumeric() || (Token.StartsWith(TEXT("-")) && Token.Mid(1).IsNumeric());
}
