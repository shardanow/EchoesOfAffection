// Copyright Epic Games, Inc. All Rights Reserved.

#include "Conditions/DialogueConditionEvaluator.h"
#include "Core/DialogueContext.h"

// UDialogueCondition_And
bool UDialogueCondition_And::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    for (const TObjectPtr<UDialogueCondition>& Condition : Conditions)
    {
        if (!Condition || !Condition->Evaluate(Context))
        {
            return false;
        }
    }
    return true;
}

FString UDialogueCondition_And::GetDescription_Implementation() const
{
    FString Result = TEXT("(");
    for (int32 i = 0; i < Conditions.Num(); ++i)
    {
        if (i > 0) Result += TEXT(" AND ");
        Result += Conditions[i] ? Conditions[i]->GetDescription() : TEXT("null");
    }
    Result += TEXT(")");
    return Result;
}

// UDialogueCondition_Or
bool UDialogueCondition_Or::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    for (const TObjectPtr<UDialogueCondition>& Condition : Conditions)
    {
        if (Condition && Condition->Evaluate(Context))
        {
            return true;
        }
    }
    return false;
}

FString UDialogueCondition_Or::GetDescription_Implementation() const
{
    FString Result = TEXT("(");
    for (int32 i = 0; i < Conditions.Num(); ++i)
    {
        if (i > 0) Result += TEXT(" OR ");
        Result += Conditions[i] ? Conditions[i]->GetDescription() : TEXT("null");
    }
    Result += TEXT(")");
    return Result;
}

// UDialogueCondition_Not
bool UDialogueCondition_Not::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    return Condition && !Condition->Evaluate(Context);
}

FString UDialogueCondition_Not::GetDescription_Implementation() const
{
    return FString::Printf(TEXT("NOT (%s)"), Condition ? *Condition->GetDescription() : TEXT("null"));
}

// UDialogueCondition_Affinity
bool UDialogueCondition_Affinity::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    if (!Context)
    {
        return false;
    }

    // TODO: Implement through RelationshipComponent
    // float CurrentAffinity = Context->GetAffinity(NPCName);
    // return CurrentAffinity >= MinValue;
    return false; // Placeholder
}

FString UDialogueCondition_Affinity::GetDescription_Implementation() const
{
    return FString::Printf(TEXT("Affinity[%s] >= %.1f"), *NPCName.ToString(), MinValue);
}

// UDialogueCondition_HasItem
bool UDialogueCondition_HasItem::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    if (!Context)
    {
        return false;
    }

    // TODO: Implement through inventory system
    // return Context->HasItem(ItemId, MinCount);
    return false; // Placeholder
}

FString UDialogueCondition_HasItem::GetDescription_Implementation() const
{
    if (MinCount > 1)
    {
        return FString::Printf(TEXT("Has %s x%d"), *ItemId.ToString(), MinCount);
    }
    return FString::Printf(TEXT("Has %s"), *ItemId.ToString());
}

// UDialogueCondition_Memory
bool UDialogueCondition_Memory::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    if (!Context)
    {
        return false;
    }

    // Use custom variables instead
    FString Value = Context->GetCustomVariable(MemoryKey);
    bool bMemoryValue = Value.Equals(TEXT("true"), ESearchCase::IgnoreCase) || Value.Equals(TEXT("1"));
    return bMemoryValue == ExpectedValue;
}

FString UDialogueCondition_Memory::GetDescription_Implementation() const
{
    return FString::Printf(TEXT("Memory[%s] = %s"), 
        *MemoryKey.ToString(), ExpectedValue ? TEXT("true") : TEXT("false"));
}

// UDialogueCondition_TimeOfDay
bool UDialogueCondition_TimeOfDay::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    if (!Context)
    {
        return false;
    }

    return Context->HasTag(RequiredTime);
}

FString UDialogueCondition_TimeOfDay::GetDescription_Implementation() const
{
    return FString::Printf(TEXT("Time: %s"), *RequiredTime.ToString());
}

// UDialogueCondition_WorldState
bool UDialogueCondition_WorldState::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    if (!Context)
    {
        return false;
    }

    return Context->HasTag(RequiredTag);
}

FString UDialogueCondition_WorldState::GetDescription_Implementation() const
{
    return FString::Printf(TEXT("Has Tag: %s"), *RequiredTag.ToString());
}

// UDialogueCondition_CustomVariable
bool UDialogueCondition_CustomVariable::Evaluate_Implementation(const UDialogueSessionContext* Context) const
{
    if (!Context)
    {
        return false;
    }

    FString CurrentValue = Context->GetCustomVariable(VariableKey);

    if (bNumericComparison)
    {
        float Current = FCString::Atof(*CurrentValue);
        float Expected = FCString::Atof(*ExpectedValue);
        return FMath::IsNearlyEqual(Current, Expected, 0.01f);
    }

    return CurrentValue.Equals(ExpectedValue);
}

FString UDialogueCondition_CustomVariable::GetDescription_Implementation() const
{
    return FString::Printf(TEXT("Variable[%s] = %s"), *VariableKey.ToString(), *ExpectedValue);
}

// UDialogueConditionEvaluator
UDialogueCondition* UDialogueConditionEvaluator::ParseCondition(const FString& ConditionString)
{
    // Check cache first
    if (TObjectPtr<UDialogueCondition>* CachedPtr = ConditionCache.Find(ConditionString))
    {
        return *CachedPtr;
    }

    // Tokenize and parse
    TArray<FString> Tokens = TokenizeExpression(ConditionString);
    if (Tokens.Num() == 0)
    {
        return nullptr;
    }

    int32 Index = 0;
    UDialogueCondition* Result = ParseExpression(Tokens, Index);

    // Cache the result
    if (Result)
    {
        ConditionCache.Add(ConditionString, Result);
    }

    return Result;
}

bool UDialogueConditionEvaluator::EvaluateString(const FString& ConditionString, const UDialogueSessionContext* Context)
{
    UDialogueCondition* Condition = ParseCondition(ConditionString);
    return EvaluateCondition(Condition, Context);
}

bool UDialogueConditionEvaluator::EvaluateCondition(const UDialogueCondition* Condition, const UDialogueSessionContext* Context)
{
    if (!Condition || !Context)
    {
        return false;
    }

    return Condition->Evaluate(Context);
}

TArray<FString> UDialogueConditionEvaluator::TokenizeExpression(const FString& Expression)
{
    TArray<FString> Tokens;
    FString CurrentToken;
    bool bInString = false;
    bool bInQuotes = false;

    for (int32 i = 0; i < Expression.Len(); ++i)
    {
        TCHAR Ch = Expression[i];

        // Handle string literals with quotes
        if (Ch == '"' || Ch == '\'')
        {
            if (bInQuotes && bInString)
            {
                // End of quoted string
                if (!CurrentToken.IsEmpty())
                {
                    Tokens.Add(CurrentToken);
                    CurrentToken.Empty();
                }
                bInString = false;
                bInQuotes = false;
            }
            else if (!bInString)
            {
                // Start of quoted string
                if (!CurrentToken.IsEmpty())
                {
                    Tokens.Add(CurrentToken);
                    CurrentToken.Empty();
                }
                bInString = true;
                bInQuotes = true;
            }
            continue;
        }

        // Inside a quoted string, add everything
        if (bInString)
        {
            CurrentToken.AppendChar(Ch);
            continue;
        }

        // Handle whitespace
        if (FChar::IsWhitespace(Ch))
        {
            if (!CurrentToken.IsEmpty())
            {
                Tokens.Add(CurrentToken);
                CurrentToken.Empty();
            }
            continue;
        }

        // Handle single-character operators and delimiters
        if (Ch == '(' || Ch == ')' || Ch == ',' || Ch == '[' || Ch == ']')
        {
            if (!CurrentToken.IsEmpty())
            {
                Tokens.Add(CurrentToken);
                CurrentToken.Empty();
            }
            Tokens.Add(FString(1, &Ch));
            continue;
        }

        // Handle multi-character operators
        if (i + 1 < Expression.Len())
        {
            TCHAR NextCh = Expression[i + 1];
            
            // Two-character operators: ==, !=, <=, >=, &&, ||, :
            if ((Ch == '=' && NextCh == '=') ||
                (Ch == '!' && NextCh == '=') ||
                (Ch == '<' && NextCh == '=') ||
                (Ch == '>' && NextCh == '=') ||
                (Ch == '&' && NextCh == '&') ||
                (Ch == '|' && NextCh == '|') ||
                (Ch == ':' && NextCh == ':'))
            {
                if (!CurrentToken.IsEmpty())
                {
                    Tokens.Add(CurrentToken);
                    CurrentToken.Empty();
                }
                Tokens.Add(FString::Printf(TEXT("%c%c"), Ch, NextCh));
                ++i; // Skip next character
                continue;
            }
        }

        // Handle single-character operators that might not be followed by =
        if (Ch == '&' || Ch == '|' || Ch == '!' || Ch == '<' || Ch == '>' || Ch == '=' || Ch == ':')
        {
            if (!CurrentToken.IsEmpty())
            {
                Tokens.Add(CurrentToken);
                CurrentToken.Empty();
            }
            Tokens.Add(FString(1, &Ch));
            continue;
        }

        // Handle dot for member access (like Time.Morning)
        if (Ch == '.')
        {
            // Don't tokenize dots in numeric literals (like 1.5)
            if (!CurrentToken.IsEmpty() && CurrentToken.IsNumeric())
            {
                CurrentToken.AppendChar(Ch);
            }
            else
            {
                // This might be member access
                CurrentToken.AppendChar(Ch);
            }
            continue;
        }

        // Regular character - add to current token
        CurrentToken.AppendChar(Ch);
    }

    // Add final token if exists
    if (!CurrentToken.IsEmpty())
    {
        Tokens.Add(CurrentToken);
    }

    return Tokens;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseExpression(const TArray<FString>& Tokens, int32& Index)
{
    UDialogueCondition* Left = ParseTerm(Tokens, Index);

    while (Index < Tokens.Num())
    {
        const FString& Token = Tokens[Index];
        
        if (Token == TEXT("||") || Token.ToLower() == TEXT("or"))
        {
            ++Index;
            UDialogueCondition* Right = ParseTerm(Tokens, Index);
            
            UDialogueCondition_Or* OrCondition = NewObject<UDialogueCondition_Or>(this);
            OrCondition->Conditions.Add(Left);
            OrCondition->Conditions.Add(Right);
            Left = OrCondition;
        }
        else
        {
            break;
        }
    }

    return Left;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseTerm(const TArray<FString>& Tokens, int32& Index)
{
    UDialogueCondition* Left = ParseAtom(Tokens, Index);

    while (Index < Tokens.Num())
    {
        const FString& Token = Tokens[Index];
        
        if (Token == TEXT("&&") || Token.ToLower() == TEXT("and"))
        {
            ++Index;
            UDialogueCondition* Right = ParseAtom(Tokens, Index);
            
            UDialogueCondition_And* AndCondition = NewObject<UDialogueCondition_And>(this);
            AndCondition->Conditions.Add(Left);
            AndCondition->Conditions.Add(Right);
            Left = AndCondition;
        }
        else
        {
            break;
        }
    }

    return Left;
}

// Helper methods - defined BEFORE ParseAtom so they can be called from it
UDialogueCondition* UDialogueConditionEvaluator::ParseHasItemCondition(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'hasItem' or 'has'
    
    if (Index >= Tokens.Num() || Tokens[Index] != TEXT("("))
    {
        return nullptr;
    }
    ++Index; // Skip '('

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString ItemName = Tokens[Index++];
    int32 Count = 1;
    
    // Optional count parameter
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(","))
    {
        ++Index; // Skip ',')
        if (Index < Tokens.Num())
        {
            Count = FCString::Atoi(*Tokens[Index++]);
        }
    }
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueCondition_HasItem* Condition = NewObject<UDialogueCondition_HasItem>(this);
    Condition->ItemId = FName(*ItemName);
    Condition->MinCount = Count;
    return Condition;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseMemoryCondition(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'memory'
    
    if (Index >= Tokens.Num() || Tokens[Index] != TEXT("("))
    {
        return nullptr;
    }
    ++Index; // Skip '('

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString Key = Tokens[Index++];
    bool ExpectedValue = true;
    
    // Optional value parameter
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(","))
    {
        ++Index; // Skip ',')
        if (Index < Tokens.Num())
        {
            FString ValueStr = Tokens[Index++].ToLower();
            ExpectedValue = (ValueStr == TEXT("true") || ValueStr == TEXT("1"));
        }
    }
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueCondition_Memory* Condition = NewObject<UDialogueCondition_Memory>(this);
    Condition->MemoryKey = FName(*Key);
    Condition->ExpectedValue = ExpectedValue;
    return Condition;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseVisitedCondition(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'visited'
    
    if (Index >= Tokens.Num() || Tokens[Index] != TEXT("("))
    {
        return nullptr;
    }
    ++Index; // Skip '('

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString NodeId = Tokens[Index++];
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    // visited(NodeId) is sugar for memory(NodeId_Visited, true)
    UDialogueCondition_Memory* Condition = NewObject<UDialogueCondition_Memory>(this);
    Condition->MemoryKey = FName(*(NodeId + TEXT("_Visited")));
    Condition->ExpectedValue = true;
    return Condition;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseTagCondition(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'tag' or 'hasTag'
    
    if (Index >= Tokens.Num() || Tokens[Index] != TEXT("("))
    {
        return nullptr;
    }
    ++Index; // Skip '('

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString TagName = Tokens[Index++];
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueCondition_WorldState* Condition = NewObject<UDialogueCondition_WorldState>(this);
    Condition->RequiredTag = FGameplayTag::RequestGameplayTag(FName(*TagName), false);
    return Condition;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseAffinityCondition(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'affinity'
    
    if (Index >= Tokens.Num() || Tokens[Index] != TEXT("["))
    {
        return nullptr;
    }
    ++Index; // Skip '['

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString NPCName = Tokens[Index++];

    if (Index < Tokens.Num() && Tokens[Index] == TEXT("]"))
    {
        ++Index; // Skip ']'
    }

    // Expect comparison operator (>=, >, <=, <, ==, !=)
    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString Operator = Tokens[Index++];

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    float Value = FCString::Atof(*Tokens[Index++]);

    UDialogueCondition_Affinity* Condition = NewObject<UDialogueCondition_Affinity>(this);
    Condition->NPCName = FName(*NPCName);
    Condition->MinValue = Value;
    // Note: Currently only supports >= comparison, could extend to support other operators
    return Condition;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseVariableCondition(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'variable' or 'var'
    
    if (Index >= Tokens.Num() || Tokens[Index] != TEXT("["))
    {
        return nullptr;
    }
    ++Index; // Skip '['

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString Key = Tokens[Index++];

    if (Index < Tokens.Num() && Tokens[Index] == TEXT("]"))
    {
        ++Index; // Skip ']'
    }

    // Expect comparison operator
    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString Operator = Tokens[Index++];

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString Value = Tokens[Index++];

    UDialogueCondition_CustomVariable* Condition = NewObject<UDialogueCondition_CustomVariable>(this);
    Condition->VariableKey = FName(*Key);
    Condition->ExpectedValue = Value;
    Condition->bNumericComparison = Value.IsNumeric();
    return Condition;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseTimeCondition(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'time'
    
    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString Operator = Tokens[Index++];
    
    if (Operator != TEXT("==") && Operator != TEXT("="))
    {
        return nullptr;
    }

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString TimeValue = Tokens[Index++];

    UDialogueCondition_TimeOfDay* Condition = NewObject<UDialogueCondition_TimeOfDay>(this);
    Condition->RequiredTime = FGameplayTag::RequestGameplayTag(FName(*TimeValue), false);
    return Condition;
}

UDialogueCondition* UDialogueConditionEvaluator::ParseAtom(const TArray<FString>& Tokens, int32& Index)
{
    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    const FString& Token = Tokens[Index];

    // Handle NOT
    if (Token == TEXT("!") || Token.ToLower() == TEXT("not"))
    {
        ++Index;
        UDialogueCondition_Not* NotCondition = NewObject<UDialogueCondition_Not>(this);
        NotCondition->Condition = ParseAtom(Tokens, Index);
        return NotCondition;
    }

    // Handle parentheses
    if (Token == TEXT("("))
    {
        ++Index;
        UDialogueCondition* Inner = ParseExpression(Tokens, Index);
        if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
        {
            ++Index;
        }
        return Inner;
    }

    // Handle function-style conditions
    FString LowerToken = Token.ToLower();
    
    // hasItem(ItemId) or hasItem(ItemId, Count)
    if (LowerToken == TEXT("hasitem") || LowerToken == TEXT("has"))
    {
        return ParseHasItemCondition(Tokens, Index);
    }

    // memory(Key) or memory(Key, Value)
    if (LowerToken == TEXT("memory"))
    {
        return ParseMemoryCondition(Tokens, Index);
    }

    // visited(NodeId)
    if (LowerToken == TEXT("visited"))
    {
        return ParseVisitedCondition(Tokens, Index);
    }

    // tag(TagName) or hasTag(TagName)
    if (LowerToken == TEXT("tag") || LowerToken == TEXT("hastag"))
    {
        return ParseTagCondition(Tokens, Index);
    }

    // Handle bracket-style conditions
    // affinity[NPC] >= Value
    if (LowerToken == TEXT("affinity"))
    {
        return ParseAffinityCondition(Tokens, Index);
    }

    // variable[Key] == Value
    if (LowerToken == TEXT("variable") || LowerToken == TEXT("var"))
    {
        return ParseVariableCondition(Tokens, Index);
    }

    // time == TimeTag
    if (LowerToken == TEXT("time"))
    {
        return ParseTimeCondition(Tokens, Index);
    }

    // If nothing matched, skip this token
    ++Index;
    return nullptr;
}
