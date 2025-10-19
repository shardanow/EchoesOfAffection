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
        // TODO: Implement through RelationshipComponent instead
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

    for (int32 i = 0; i < Expression.Len(); ++i)
    {
        TCHAR Ch = Expression[i];

        if (Ch == '"')
        {
            bInString = !bInString;
            continue;
        }

        if (bInString)
        {
            CurrentToken.AppendChar(Ch);
            continue;
        }

        if (FChar::IsWhitespace(Ch))
        {
            if (!CurrentToken.IsEmpty())
            {
                Tokens.Add(CurrentToken);
                CurrentToken.Empty();
            }
            continue;
        }

        // Handle operators and delimiters
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

        // Handle operators +=, -=, etc
        if ((Ch == '+' || Ch == '-' || Ch == '*' || Ch == '/') && 
            i + 1 < Expression.Len() && Expression[i + 1] == '=')
        {
            if (!CurrentToken.IsEmpty())
            {
                Tokens.Add(CurrentToken);
                CurrentToken.Empty();
            }
            Tokens.Add(FString::Chr(Ch) + TEXT("="));
            ++i;
            continue;
        }

        // Handle assignment =
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

        CurrentToken.AppendChar(Ch);
    }

    if (!CurrentToken.IsEmpty())
    {
        Tokens.Add(CurrentToken);
    }

    return Tokens;
}

UDialogueEffect* UDialogueEffectExecutor::ParseSingleEffect(const TArray<FString>& Tokens, int32& Index)
{
    if (Index >= Tokens.Num())
    {
        return nullptr;
    }

    const FString& Token = Tokens[Index];

    // affinity[NPC] += Value
    if (Token.ToLower() == TEXT("affinity"))
    {
        ++Index;
        if (Index < Tokens.Num() && Tokens[Index] == TEXT("["))
        {
            ++Index;
            if (Index < Tokens.Num())
            {
                FString NPCName = Tokens[Index++];
                
                if (Index < Tokens.Num() && Tokens[Index] == TEXT("]"))
                {
                    ++Index;
                }

                if (Index < Tokens.Num() && Tokens[Index] == TEXT("+="))
                {
                    ++Index;
                    if (Index < Tokens.Num())
                    {
                        float Value = FCString::Atof(*Tokens[Index++]);

                        UDialogueEffect_ModifyAffinity* Effect = NewObject<UDialogueEffect_ModifyAffinity>(this);
                        Effect->NPCName = FName(*NPCName);
                        Effect->DeltaValue = Value;
                        return Effect;
                    }
                }
            }
        }
    }

    // gold += Value
    if (Token.ToLower() == TEXT("gold"))
    {
        ++Index;
        if (Index < Tokens.Num() && Tokens[Index] == TEXT("+="))
        {
            ++Index;
            if (Index < Tokens.Num())
            {
                int32 Value = FCString::Atoi(*Tokens[Index++]);

                UDialogueEffect_ModifyGold* Effect = NewObject<UDialogueEffect_ModifyGold>(this);
                Effect->DeltaGold = Value;
                return Effect;
            }
        }
    }

    // giveItem(ItemId, Count)
    if (Token.ToLower() == TEXT("giveitem") || Token.ToLower() == TEXT("give"))
    {
        ++Index;
        if (Index < Tokens.Num() && Tokens[Index] == TEXT("("))
        {
            ++Index;
            if (Index < Tokens.Num())
            {
                FString ItemName = Tokens[Index++];
                int32 Count = 1;
                
                if (Index < Tokens.Num() && Tokens[Index] == TEXT(","))
                {
                    ++Index;
                    if (Index < Tokens.Num())
                    {
                        Count = FCString::Atoi(*Tokens[Index++]);
                    }
                }
                
                if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
                {
                    ++Index;
                }

                UDialogueEffect_ModifyInventory* Effect = NewObject<UDialogueEffect_ModifyInventory>(this);
                Effect->ItemId = FName(*ItemName);
                Effect->DeltaCount = Count;
                return Effect;
            }
        }
    }

    // setMemory(Key, Value)
    if (Token.ToLower() == TEXT("setmemory") || Token.ToLower() == TEXT("memory"))
    {
        ++Index;
        if (Index < Tokens.Num() && Tokens[Index] == TEXT("("))
        {
            ++Index;
            if (Index < Tokens.Num())
            {
                FString Key = Tokens[Index++];
                bool Value = true;
                
                if (Index < Tokens.Num() && Tokens[Index] == TEXT(","))
                {
                    ++Index;
                    if (Index < Tokens.Num())
                    {
                        Value = Tokens[Index++].ToBool();
                    }
                }
                
                if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
                {
                    ++Index;
                }

                UDialogueEffect_SetMemory* Effect = NewObject<UDialogueEffect_SetMemory>(this);
                Effect->MemoryKey = FName(*Key);
                Effect->Value = Value;
                return Effect;
            }
        }
    }

    // startQuest(QuestId)
    if (Token.ToLower() == TEXT("startquest") || Token.ToLower() == TEXT("quest"))
    {
        ++Index;
        if (Index < Tokens.Num() && Tokens[Index] == TEXT("("))
        {
            ++Index;
            if (Index < Tokens.Num())
            {
                FString QuestId = Tokens[Index++];
                
                if (Index < Tokens.Num() && Tokens[Index] == TEXT(")"))
                {
                    ++Index;
                }

                UDialogueEffect_StartQuest* Effect = NewObject<UDialogueEffect_StartQuest>(this);
                Effect->QuestId = FName(*QuestId);
                return Effect;
            }
        }
    }

    // If nothing else, skip
    ++Index;
    return nullptr;
}
