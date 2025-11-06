// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffectParser.h"
#include "Effects/DialogueEffectExecutor.h"
#include "Effects/DialogueEffect_PlaySequence.h"
#include "Core/DialogueContext.h"

DEFINE_LOG_CATEGORY_STATIC(LogDialogueParser, Log, All);

//==============================================================================
// UDialogueEffectParser Implementation
//==============================================================================

UDialogueEffectParser::UDialogueEffectParser()
	: bEnableCaching(true)
{
}

UDialogueEffect* UDialogueEffectParser::ParseEffect(const FString& EffectString)
{
	if (EffectString.IsEmpty())
	{
		return nullptr;
	}

	// Check cache first
	if (bEnableCaching)
	{
		if (TObjectPtr<UDialogueEffect>* CachedEffect = EffectCache.Find(EffectString))
		{
			UE_LOG(LogDialogueParser, Verbose, TEXT("Cache hit for: %s"), *EffectString);
			return CachedEffect->Get(); // Get raw pointer from TObjectPtr
		}
	}

	// Parse effect
	TArray<FString> Tokens = TokenizeExpression(EffectString);
	UDialogueEffect* ParsedEffect = ParseSingleEffect(Tokens);

	// Cache result
	if (bEnableCaching && ParsedEffect)
	{
		EffectCache.Add(EffectString, ParsedEffect);
		UE_LOG(LogDialogueParser, Verbose, TEXT("Cached effect: %s"), *EffectString);
	}

	return ParsedEffect;
}

TArray<UDialogueEffect*> UDialogueEffectParser::ParseEffectList(const FString& EffectListString)
{
	TArray<UDialogueEffect*> Effects;

	if (EffectListString.IsEmpty())
	{
		return Effects;
	}

	// Split by semicolon
	TArray<FString> EffectStrings;
	EffectListString.ParseIntoArray(EffectStrings, TEXT(";"), true);

	// Parse each effect
	for (const FString& EffectStr : EffectStrings)
	{
		FString TrimmedStr = EffectStr.TrimStartAndEnd();
		if (!TrimmedStr.IsEmpty())
		{
			if (UDialogueEffect* Effect = ParseEffect(TrimmedStr))
			{
				Effects.Add(Effect);
			}
			else
			{
				UE_LOG(LogDialogueParser, Warning, TEXT("Failed to parse effect: %s"), *TrimmedStr);
			}
		}
	}

	UE_LOG(LogDialogueParser, Log, TEXT("Parsed %d effects from list"), Effects.Num());

	return Effects;
}

void UDialogueEffectParser::ClearCache()
{
	EffectCache.Empty();
	UE_LOG(LogDialogueParser, Log, TEXT("Parser cache cleared"));
}

//==============================================================================
// Parsing Implementation
//==============================================================================

TArray<FString> UDialogueEffectParser::TokenizeExpression(const FString& Expression) const
{
	TArray<FString> Tokens;

	// Find function name
	int32 ParenIndex = -1;
	if (Expression.FindChar(TEXT('('), ParenIndex))
	{
		FString FunctionName = Expression.Left(ParenIndex).TrimStartAndEnd();
		Tokens.Add(FunctionName);

		// Extract arguments
		int32 EndParenIndex = -1;
		if (Expression.FindLastChar(TEXT(')'), EndParenIndex))
		{
			FString ArgsString = Expression.Mid(ParenIndex + 1, EndParenIndex - ParenIndex - 1);
			
			// Split by comma
			TArray<FString> Args;
			ArgsString.ParseIntoArray(Args, TEXT(","), false);

			for (FString& Arg : Args)
			{
				Tokens.Add(CleanToken(Arg));
			}
		}
	}

	return Tokens;
}

UDialogueEffect* UDialogueEffectParser::ParseSingleEffect(const TArray<FString>& Tokens)
{
	if (Tokens.Num() == 0)
	{
		return nullptr;
	}

	FString EffectType = Tokens[0];
	TArray<FString> Args = Tokens;
	Args.RemoveAt(0); // Remove effect type

	// Parse based on type
	if (EffectType.Equals(TEXT("SetVariable"), ESearchCase::IgnoreCase))
	{
		return ParseSetVariable(Args);
	}
	else if (EffectType.Equals(TEXT("SetMemory"), ESearchCase::IgnoreCase))
	{
		return ParseSetMemory(Args);
	}
	else if (EffectType.Equals(TEXT("ModifyAffinity"), ESearchCase::IgnoreCase))
	{
		return ParseModifyAffinity(Args);
	}
	else if (EffectType.Equals(TEXT("ModifyTrust"), ESearchCase::IgnoreCase))
	{
		return ParseModifyTrust(Args);
	}
	else if (EffectType.Equals(TEXT("ModifyInventory"), ESearchCase::IgnoreCase))
	{
		return ParseModifyInventory(Args);
	}
	else if (EffectType.Equals(TEXT("ModifyGold"), ESearchCase::IgnoreCase))
	{
		return ParseModifyGold(Args);
	}
	else if (EffectType.Equals(TEXT("AddTag"), ESearchCase::IgnoreCase))
	{
		return ParseAddTag(Args);
	}
	else if (EffectType.Equals(TEXT("RemoveTag"), ESearchCase::IgnoreCase))
	{
		return ParseRemoveTag(Args);
	}
	else if (EffectType.Equals(TEXT("StartQuest"), ESearchCase::IgnoreCase))
	{
		return ParseStartQuest(Args);
	}
	else if (EffectType.Equals(TEXT("CompleteQuest"), ESearchCase::IgnoreCase))
	{
		return ParseCompleteQuest(Args);
	}
	else if (EffectType.Equals(TEXT("PlaySequence"), ESearchCase::IgnoreCase))
	{
		return ParsePlaySequence(Args);
	}

	UE_LOG(LogDialogueParser, Warning, TEXT("Unknown effect type: %s"), *EffectType);
	return nullptr;
}

//==============================================================================
// Specific Effect Parsers
//==============================================================================

UDialogueEffect* UDialogueEffectParser::ParseSetVariable(const TArray<FString>& Args)
{
	if (Args.Num() < 2)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("SetVariable requires 2 arguments: key, value"));
		return nullptr;
	}

	UDialogueEffect_SetVariable* Effect = NewObject<UDialogueEffect_SetVariable>();
	Effect->VariableKey = FName(*Args[0]);
	Effect->Value = Args[1];

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseSetMemory(const TArray<FString>& Args)
{
	if (Args.Num() < 2)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("SetMemory requires 2 arguments: key, value"));
		return nullptr;
	}

	UDialogueEffect_SetMemory* Effect = NewObject<UDialogueEffect_SetMemory>();
	Effect->MemoryKey = FName(*Args[0]);
	Effect->Value = Args[1].ToBool();

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseModifyAffinity(const TArray<FString>& Args)
{
	if (Args.Num() < 2)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("ModifyAffinity requires 2 arguments: npc, delta"));
		return nullptr;
	}

	UDialogueEffect_ModifyAffinity* Effect = NewObject<UDialogueEffect_ModifyAffinity>();
	Effect->NPCName = FName(*Args[0]);
	Effect->DeltaValue = FCString::Atof(*Args[1]);

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseModifyTrust(const TArray<FString>& Args)
{
	if (Args.Num() < 2)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("ModifyTrust requires 2 arguments: npc, delta"));
		return nullptr;
	}

	UDialogueEffect_ModifyTrust* Effect = NewObject<UDialogueEffect_ModifyTrust>();
	Effect->NPCName = FName(*Args[0]);
	Effect->DeltaValue = FCString::Atof(*Args[1]);

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseModifyInventory(const TArray<FString>& Args)
{
	if (Args.Num() < 2)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("ModifyInventory requires 2 arguments: item, delta"));
		return nullptr;
	}

	UDialogueEffect_ModifyInventory* Effect = NewObject<UDialogueEffect_ModifyInventory>();
	Effect->ItemId = FName(*Args[0]);
	Effect->DeltaCount = FCString::Atoi(*Args[1]);

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseModifyGold(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("ModifyGold requires 1 argument: delta"));
		return nullptr;
	}

	UDialogueEffect_ModifyGold* Effect = NewObject<UDialogueEffect_ModifyGold>();
	Effect->DeltaGold = FCString::Atoi(*Args[0]);

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseAddTag(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("AddTag requires 1 argument: tag"));
		return nullptr;
	}

	UDialogueEffect_AddWorldStateTag* Effect = NewObject<UDialogueEffect_AddWorldStateTag>();
	Effect->TagToAdd = FGameplayTag::RequestGameplayTag(FName(*Args[0]), false);

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseRemoveTag(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("RemoveTag requires 1 argument: tag"));
		return nullptr;
	}

	UDialogueEffect_RemoveWorldStateTag* Effect = NewObject<UDialogueEffect_RemoveWorldStateTag>();
	Effect->TagToRemove = FGameplayTag::RequestGameplayTag(FName(*Args[0]), false);

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseStartQuest(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("StartQuest requires 1 argument: questId"));
		return nullptr;
	}

	UDialogueEffect_StartQuest* Effect = NewObject<UDialogueEffect_StartQuest>();
	Effect->QuestId = FName(*Args[0]);

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParseCompleteQuest(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("CompleteQuest requires 1 argument: questId"));
		return nullptr;
	}

	UDialogueEffect_CompleteQuest* Effect = NewObject<UDialogueEffect_CompleteQuest>();
	Effect->QuestId = FName(*Args[0]);

	return Effect;
}

UDialogueEffect* UDialogueEffectParser::ParsePlaySequence(const TArray<FString>& Args)
{
	if (Args.Num() < 1)
	{
		UE_LOG(LogDialogueParser, Error, TEXT("PlaySequence requires 1 argument: sequencePath"));
		return nullptr;
	}

	UDialogueEffect_PlaySequence* Effect = NewObject<UDialogueEffect_PlaySequence>();
	
	// Parse sequence asset path
	FString SequencePath = Args[0];
	Effect->SequenceAsset = TSoftObjectPtr<ULevelSequence>(FSoftObjectPath(SequencePath));

	// Optional: bWaitForCompletion (default: true)
	if (Args.Num() > 1)
	{
		Effect->bWaitForCompletion = Args[1].ToBool();
	}

	// Optional: bRestoreCameraAfter (default: true)
	if (Args.Num() > 2)
	{
		Effect->bRestoreCameraAfter = Args[2].ToBool();
	}

	UE_LOG(LogDialogueParser, Log, TEXT("Parsed PlaySequence effect: %s"), *SequencePath);

	return Effect;
}

FString UDialogueEffectParser::CleanToken(const FString& Token) const
{
	FString Cleaned = Token.TrimStartAndEnd();

	// Remove quotes
	if (Cleaned.StartsWith(TEXT("\"")) && Cleaned.EndsWith(TEXT("\"")))
	{
		Cleaned = Cleaned.Mid(1, Cleaned.Len() - 2);
	}
	else if (Cleaned.StartsWith(TEXT("'")) && Cleaned.EndsWith(TEXT("'")))
	{
		Cleaned = Cleaned.Mid(1, Cleaned.Len() - 2);
	}

	return Cleaned;
}

//==============================================================================
// Blueprint Library Implementation
//==============================================================================

UDialogueEffectParser* UDialogueEffectParserLibrary::CreateParser(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	return NewObject<UDialogueEffectParser>(WorldContextObject);
}

bool UDialogueEffectParserLibrary::ParseAndExecuteEffect(const FString& EffectString, UDialogueSessionContext* Context)
{
	if (!Context)
	{
		return false;
	}

	UDialogueEffectParser* Parser = NewObject<UDialogueEffectParser>();
	UDialogueEffect* Effect = Parser->ParseEffect(EffectString);

	if (!Effect)
	{
		return false;
	}

	UDialogueEffectExecutor* Executor = NewObject<UDialogueEffectExecutor>();
	Executor->ExecuteEffect(Effect, Context);

	return true;
}

bool UDialogueEffectParserLibrary::ParseAndExecuteEffects(const FString& EffectListString, UDialogueSessionContext* Context)
{
	if (!Context)
	{
		return false;
	}

	UDialogueEffectParser* Parser = NewObject<UDialogueEffectParser>();
	TArray<UDialogueEffect*> Effects = Parser->ParseEffectList(EffectListString);

	if (Effects.Num() == 0)
	{
		return false;
	}

	UDialogueEffectExecutor* Executor = NewObject<UDialogueEffectExecutor>();
	Executor->ExecuteEffects(Effects, Context);

	return true;
}
