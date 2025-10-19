// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffectExecutor.h"
#include "Core/DialogueContext.h"

// UDialogueEffect_ModifyAffinity
void UDialogueEffect_ModifyAffinity::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through RelationshipComponent instead
        // float Current = Context->GetAffinity(NPCName);
        // Context->SetAffinity(NPCName, Current + DeltaValue);
    }
}

FText UDialogueEffect_ModifyAffinity::GetDisplayText_Implementation() const
{
    FString Sign = DeltaValue >= 0 ? TEXT("+") : TEXT("");
    return FText::FromString(FString::Printf(TEXT("Affinity[%s] %s%.1f"), 
        *NPCName.ToString(), *Sign, DeltaValue));
}

// UDialogueEffect_ModifyTrust
void UDialogueEffect_ModifyTrust::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through RelationshipComponent invece
        // float Current = Context->GetTrust(NPCName);
        // Context->SetTrust(NPCName, Current + DeltaValue);
    }
}

FText UDialogueEffect_ModifyTrust::GetDisplayText_Implementation() const
{
    FString Sign = DeltaValue >= 0 ? TEXT("+") : TEXT("");
    return FText::FromString(FString::Printf(TEXT("Trust[%s] %s%.1f"), 
        *NPCName.ToString(), *Sign, DeltaValue));
}

// UDialogueEffect_ModifyInventory
void UDialogueEffect_ModifyInventory::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through inventory system
        // if (DeltaCount > 0)
        // {
        //     Context->AddItem(ItemId, DeltaCount);
        // }
        // else if (DeltaCount < 0)
        // {
        //     Context->RemoveItem(ItemId, FMath::Abs(DeltaCount));
        // }
    }
}

FText UDialogueEffect_ModifyInventory::GetDisplayText_Implementation() const
{
    if (DeltaCount > 0)
    {
        return FText::FromString(FString::Printf(TEXT("Receive: %s x%d"), *ItemId.ToString(), DeltaCount));
    }
    else
    {
        return FText::FromString(FString::Printf(TEXT("Lose: %s x%d"), *ItemId.ToString(), FMath::Abs(DeltaCount)));
    }
}

// UDialogueEffect_ModifyGold
void UDialogueEffect_ModifyGold::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through economy system
        // int32 Current = Context->GetGold();
        // Context->SetGold(Current + DeltaGold);
    }
}

FText UDialogueEffect_ModifyGold::GetDisplayText_Implementation() const
{
    FString Sign = DeltaGold >= 0 ? TEXT("+") : TEXT("");
    return FText::FromString(FString::Printf(TEXT("Gold %s%d"), *Sign, DeltaGold));
}

// UDialogueEffect_SetMemory
void UDialogueEffect_SetMemory::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // Use custom variables instead
        Context->SetCustomVariable(MemoryKey, Value ? TEXT("true") : TEXT("false"));
    }
}

FText UDialogueEffect_SetMemory::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Set Memory[%s] = %s"), 
        *MemoryKey.ToString(), Value ? TEXT("true") : TEXT("false")));
}

// UDialogueEffect_AddWorldStateTag
void UDialogueEffect_AddWorldStateTag::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        Context->AddTag(TagToAdd);
    }
}

FText UDialogueEffect_AddWorldStateTag::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Add Tag: %s"), *TagToAdd.ToString()));
}

// UDialogueEffect_RemoveWorldStateTag
void UDialogueEffect_RemoveWorldStateTag::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        Context->RemoveTag(TagToRemove);
    }
}

FText UDialogueEffect_RemoveWorldStateTag::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Remove Tag: %s"), *TagToRemove.ToString()));
}

// UDialogueEffect_StartQuest
void UDialogueEffect_StartQuest::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through quest system
        // Context->StartQuest(QuestId);
    }
}

FText UDialogueEffect_StartQuest::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Start Quest: %s"), *QuestId.ToString()));
}

// UDialogueEffect_CompleteQuest
void UDialogueEffect_CompleteQuest::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        // TODO: Implement through quest system
        // Context->CompleteQuest(QuestId);
    }
}

FText UDialogueEffect_CompleteQuest::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Complete Quest: %s"), *QuestId.ToString()));
}

// UDialogueEffect_SetVariable
void UDialogueEffect_SetVariable::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (Context)
    {
        Context->SetCustomVariable(VariableKey, Value);
    }
}

FText UDialogueEffect_SetVariable::GetDisplayText_Implementation() const
{
    return FText::FromString(FString::Printf(TEXT("Set Variable[%s] = %s"), 
        *VariableKey.ToString(), *Value));
}

// UDialogueEffect_Composite
void UDialogueEffect_Composite::Execute_Implementation(UDialogueSessionContext* Context)
{
    for (const TObjectPtr<UDialogueEffect>& Effect : Effects)
    {
        if (Effect)
        {
            Effect->Execute(Context);
        }
    }
}

FText UDialogueEffect_Composite::GetDisplayText_Implementation() const
{
    FString Result = TEXT("Multiple Effects: ");
    for (int32 i = 0; i < Effects.Num() && i < 3; ++i)
    {
        if (i > 0) Result += TEXT(", ");
        Result += Effects[i] ? Effects[i]->GetDisplayText().ToString() : TEXT("null");
    }
    if (Effects.Num() > 3)
    {
        Result += FString::Printf(TEXT(" (+%d more)"), Effects.Num() - 3);
    }
    return FText::FromString(Result);
}

// UDialogueEffectExecutor
UDialogueEffect* UDialogueEffectExecutor::ParseEffect(const FString& EffectString)
{
    // Check cache
    if (TObjectPtr<UDialogueEffect>* Cached = EffectCache.Find(EffectString))
    {
        return *Cached;
    }

    TArray<FString> Tokens = TokenizeExpression(EffectString);
    if (Tokens.Num() == 0)
    {
        return nullptr;
    }

    int32 Index = 0;
    UDialogueEffect* Result = ParseSingleEffect(Tokens, Index);

    if (Result)
    {
        EffectCache.Add(EffectString, Result);
    }

    return Result;
}

void UDialogueEffectExecutor::ExecuteString(const FString& EffectString, UDialogueSessionContext* Context)
{
    TArray<UDialogueEffect*> Effects = ParseEffectList(EffectString);
    for (UDialogueEffect* Effect : Effects)
    {
        ExecuteEffect(Effect, Context);
    }
}

void UDialogueEffectExecutor::ExecuteEffects(const TArray<UDialogueEffect*>& Effects, UDialogueSessionContext* Context)
{
    for (UDialogueEffect* Effect : Effects)
    {
        ExecuteEffect(Effect, Context);
    }
}

void UDialogueEffectExecutor::ExecuteEffect(UDialogueEffect* Effect, UDialogueSessionContext* Context)
{
    if (Effect && Context)
    {
        Effect->Execute(Context);
    }
}

TArray<UDialogueEffect*> UDialogueEffectExecutor::ParseEffectList(const FString& EffectString)
{
    TArray<UDialogueEffect*> Results;
    
    // Split by semicolon
    TArray<FString> EffectStrings;
    EffectString.ParseIntoArray(EffectStrings, TEXT(";"), true);

    for (const FString& SingleEffect : EffectStrings)
    {
        UDialogueEffect* Effect = ParseEffect(SingleEffect.TrimStartAndEnd());
        if (Effect)
        {
            Results.Add(Effect);
        }
    }

    return Results;
}

TArray<FString> UDialogueEffectExecutor::TokenizeExpression(const FString& Expression)
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
            
            // Two-character operators: +=, -=, *=, /=, ==, :: 
            if ((Ch == '+' && NextCh == '=') ||
                (Ch == '-' && NextCh == '=') ||
                (Ch == '*' && NextCh == '=') ||
                (Ch == '/' && NextCh == '=') ||
                (Ch == '=' && NextCh == '=') ||
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

        // Handle single assignment operator
        if (Ch == '=')
        {
            if (!CurrentToken.IsEmpty())
            {
                Tokens.Add(CurrentToken);
                CurrentToken.Empty();
            }
            Tokens.Add(TEXT("="));
            continue;
        }

        // Handle arithmetic operators (not followed by =)
        if (Ch == '+' || Ch == '-' || Ch == '*' || Ch == '/')
        {
            // Check if it's a unary minus for negative numbers
            if (Ch == '-' && CurrentToken.IsEmpty() && 
                (Tokens.Num() == 0 || Tokens.Last() == TEXT("(") || Tokens.Last() == TEXT(",")))
            {
                // This might be a negative number
                CurrentToken.AppendChar(Ch);
            }
            else
            {
                if (!CurrentToken.IsEmpty())
                {
                    Tokens.Add(CurrentToken);
                    CurrentToken.Empty();
                }
                Tokens.Add(FString(1, &Ch));
            }
            continue;
        }

        // Handle dot for member access or decimal numbers
        if (Ch == '.')
        {
            if (!CurrentToken.IsEmpty() && CurrentToken.IsNumeric())
            {
                // Part of a numeric literal
                CurrentToken.AppendChar(Ch);
            }
            else
            {
                // Member access
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

// Helper methods - defined BEFORE ParseSingleEffect so they can be called from it
UDialogueEffect* UDialogueEffectExecutor::ParseGiveItemEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'giveItem' or 'give'
    
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
        ++Index; // Skip ','
        if (Index < Tokens.Num())
        {
            Count = FCString::Atoi(*Tokens[Index++]);
        }
    }
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueEffect_ModifyInventory* Effect = NewObject<UDialogueEffect_ModifyInventory>(this);
    Effect->ItemId = FName(*ItemName);
    Effect->DeltaCount = FMath::Abs(Count);
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseRemoveItemEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'removeItem' or 'remove'
    
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
        ++Index; // Skip ','
        if (Index < Tokens.Num())
        {
            Count = FCString::Atoi(*Tokens[Index++]);
        }
    }
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueEffect_ModifyInventory* Effect = NewObject<UDialogueEffect_ModifyInventory>(this);
    Effect->ItemId = FName(*ItemName);
    Effect->DeltaCount = -FMath::Abs(Count);
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseMemoryEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'setMemory' or 'memory'
    
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
    bool Value = true;
    
    // Optional value parameter
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(","))
    {
        ++Index; // Skip ','
        if (Index < Tokens.Num())
        {
            FString ValueStr = Tokens[Index++].ToLower();
            Value = (ValueStr == TEXT("true") || ValueStr == TEXT("1"));
        }
    }
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueEffect_SetMemory* Effect = NewObject<UDialogueEffect_SetMemory>(this);
    Effect->MemoryKey = FName(*Key);
    Effect->Value = Value;
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseQuestEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'startQuest' or 'quest'
    
    if (Index >= Tokens.Num() || Tokens[Index] != TEXT("("))
    {
        return nullptr;
    }
    ++Index; // Skip '('

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString QuestId = Tokens[Index++];
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueEffect_StartQuest* Effect = NewObject<UDialogueEffect_StartQuest>(this);
    Effect->QuestId = FName(*QuestId);
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseCompleteQuestEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'completeQuest'
    
    if (Index >= Tokens.Num() || Tokens[Index] != TEXT("("))
    {
        return nullptr;
    }
    ++Index; // Skip '('

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString QuestId = Tokens[Index++];
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueEffect_CompleteQuest* Effect = NewObject<UDialogueEffect_CompleteQuest>(this);
    Effect->QuestId = FName(*QuestId);
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseAddTagEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'addTag'
    
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

    UDialogueEffect_AddWorldStateTag* Effect = NewObject<UDialogueEffect_AddWorldStateTag>(this);
    Effect->TagToAdd = FGameplayTag::RequestGameplayTag(FName(*TagName), false);
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseRemoveTagEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'removeTag'
    
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

    UDialogueEffect_RemoveWorldStateTag* Effect = NewObject<UDialogueEffect_RemoveWorldStateTag>(this);
    Effect->TagToRemove = FGameplayTag::RequestGameplayTag(FName(*TagName), false);
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseSetVariableEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'setVariable' or 'var'
    
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
    FString Value;
    
    // Required value parameter
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(","))
    {
        ++Index; // Skip ','
        if (Index < Tokens.Num())
        {
            Value = Tokens[Index++];
        }
    }
    
    if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
    {
        ++Index; // Skip ')'
    }

    UDialogueEffect_SetVariable* Effect = NewObject<UDialogueEffect_SetVariable>(this);
    Effect->VariableKey = FName(*Key);
    Effect->Value = Value;
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseAffinityEffect(const TArray<FString>& Tokens, int32& Index)
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

    // Expect operator (+=, -=, or =)
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

    // Handle -= operator by negating the value
    if (Operator == TEXT("-="))
    {
        Value = -Value;
    }

    UDialogueEffect_ModifyAffinity* Effect = NewObject<UDialogueEffect_ModifyAffinity>(this);
    Effect->NPCName = FName(*NPCName);
    Effect->DeltaValue = Value;
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseTrustEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'trust'
    
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

    // Expect operator (+=, -=, or =)
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

    // Handle -= operator by negating the value
    if (Operator == TEXT("-="))
    {
        Value = -Value;
    }

    UDialogueEffect_ModifyTrust* Effect = NewObject<UDialogueEffect_ModifyTrust>(this);
    Effect->NPCName = FName(*NPCName);
    Effect->DeltaValue = Value;
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseGoldEffect(const TArray<FString>& Tokens, int32& Index)
{
    ++Index; // Skip 'gold'
    
    // Expect operator (+=, -=, or =)
    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    FString Operator = Tokens[Index++];

    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    int32 Value = FCString::Atoi(*Tokens[Index++]);

    // Handle -= operator by negating the value
    if (Operator == TEXT("-="))
    {
        Value = -Value;
    }

    UDialogueEffect_ModifyGold* Effect = NewObject<UDialogueEffect_ModifyGold>(this);
    Effect->DeltaGold = Value;
    return Effect;
}

UDialogueEffect* UDialogueEffectExecutor::ParseSingleEffect(const TArray<FString>& Tokens, int32& Index)
{
    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    const FString& Token = Tokens[Index];
    FString LowerToken = Token.ToLower();

    // Handle function-style effects
    // giveItem(ItemId, Count) or give(ItemId, Count)
    if (LowerToken == TEXT("giveitem") || LowerToken == TEXT("give"))
    {
        return ParseGiveItemEffect(Tokens, Index);
    }

    // removeItem(ItemId, Count) or remove(ItemId, Count)
    if (LowerToken == TEXT("removeitem") || LowerToken == TEXT("remove"))
    {
        return ParseRemoveItemEffect(Tokens, Index);
    }

    // setMemory(Key, Value) or memory(Key, Value)
    if (LowerToken == TEXT("setmemory") || LowerToken == TEXT("memory"))
    {
        return ParseMemoryEffect(Tokens, Index);
    }

    // startQuest(QuestId) or quest(QuestId)
    if (LowerToken == TEXT("startquest") || LowerToken == TEXT("quest"))
    {
        return ParseQuestEffect(Tokens, Index);
    }

    // completeQuest(QuestId)
    if (LowerToken == TEXT("completequest"))
    {
        return ParseCompleteQuestEffect(Tokens, Index);
    }

    // addTag(TagName)
    if (LowerToken == TEXT("addtag"))
    {
        return ParseAddTagEffect(Tokens, Index);
    }

    // removeTag(TagName)
    if (LowerToken == TEXT("removetag"))
    {
        return ParseRemoveTagEffect(Tokens, Index);
    }

    // setVariable(Key, Value) or var(Key, Value)
    if (LowerToken == TEXT("setvariable") || LowerToken == TEXT("var"))
    {
        return ParseSetVariableEffect(Tokens, Index);
    }

    // Handle bracket-style effects
    // affinity[NPC] += Value
    if (LowerToken == TEXT("affinity"))
    {
        return ParseAffinityEffect(Tokens, Index);
    }

    // trust[NPC] += Value
    if (LowerToken == TEXT("trust"))
    {
        return ParseTrustEffect(Tokens, Index);
    }

    // gold += Value
    if (LowerToken == TEXT("gold"))
    {
        return ParseGoldEffect(Tokens, Index);
    }

    // If nothing matched, skip this token
    ++Index;
    return nullptr;
}
