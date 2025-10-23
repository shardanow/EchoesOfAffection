// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogueEffectParser.generated.h"

// Forward declarations
class UDialogueEffect;
class UDialogueSessionContext;

/**
 * Parser for dialogue effect DSL (Domain Specific Language)
 * 
 * Separates parsing logic from execution (SRP principle)
 * 
 * Supported syntax:
 * - SetVariable(key, value)
 * - ModifyAffinity(npc, delta)
 * - AddTag(tag)
 * - Multiple effects: "effect1(); effect2(); effect3()"
 * 
 * v1.3.1 - Architecture improvement: Separated from Executor
 * 
 * Example usage:
 * 
 *   UDialogueEffectParser* Parser = NewObject<UDialogueEffectParser>();
 *   UDialogueEffect* Effect = Parser->ParseEffect("SetVariable(gold, 100)");
 *   TArray<UDialogueEffect*> Effects = Parser->ParseEffectList("AddTag(quest_started); ModifyAffinity(Lianne, 10)");
 */
UCLASS(BlueprintType)
class DIALOGUESYSTEMCORE_API UDialogueEffectParser : public UObject
{
	GENERATED_BODY()

public:
	UDialogueEffectParser();

	/**
	 * Parse single effect from string
	 * @param EffectString - DSL string (e.g., "SetVariable(gold, 100)")
	 * @return Parsed effect or nullptr if parsing failed
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Parser")
	UDialogueEffect* ParseEffect(const FString& EffectString);

	/**
	 * Parse multiple effects from string (separated by semicolon)
	 * @param EffectListString - DSL string (e.g., "Effect1(); Effect2(); Effect3()")
	 * @return Array of parsed effects
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Parser")
	TArray<UDialogueEffect*> ParseEffectList(const FString& EffectListString);

	/**
	 * Clear parser cache
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Parser")
	void ClearCache();

	/**
	 * Get cache statistics
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Parser")
	int32 GetCacheSize() const { return EffectCache.Num(); }

	/**
	 * Enable/disable caching
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Parser")
	void SetCachingEnabled(bool bEnabled) { bEnableCaching = bEnabled; }

	/**
	 * Check if caching is enabled
	 */
	UFUNCTION(BlueprintPure, Category = "Dialogue|Parser")
	bool IsCachingEnabled() const { return bEnableCaching; }

private:
	/**
	 * Tokenize expression into array of strings
	 * Example: "SetVariable(key, value)" -> ["SetVariable", "key", "value"]
	 */
	TArray<FString> TokenizeExpression(const FString& Expression) const;

	/**
	 * Parse single effect from tokens
	 */
	UDialogueEffect* ParseSingleEffect(const TArray<FString>& Tokens);

	/**
	 * Parse specific effect types
	 */
	UDialogueEffect* ParseSetVariable(const TArray<FString>& Args);
	UDialogueEffect* ParseSetMemory(const TArray<FString>& Args);
	UDialogueEffect* ParseModifyAffinity(const TArray<FString>& Args);
	UDialogueEffect* ParseModifyTrust(const TArray<FString>& Args);
	UDialogueEffect* ParseModifyInventory(const TArray<FString>& Args);
	UDialogueEffect* ParseModifyGold(const TArray<FString>& Args);
	UDialogueEffect* ParseAddTag(const TArray<FString>& Args);
	UDialogueEffect* ParseRemoveTag(const TArray<FString>& Args);
	UDialogueEffect* ParseStartQuest(const TArray<FString>& Args);
	UDialogueEffect* ParseCompleteQuest(const TArray<FString>& Args);

	/**
	 * Helper: Trim whitespace and quotes from string
	 */
	FString CleanToken(const FString& Token) const;

	/** Effect cache (for performance) */
	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UDialogueEffect>> EffectCache;

	/** Enable/disable caching */
	UPROPERTY(Transient)
	bool bEnableCaching;
};

/**
 * Blueprint library for parser utilities
 */
UCLASS()
class DIALOGUESYSTEMCORE_API UDialogueEffectParserLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Create parser instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Parser", meta = (WorldContext = "WorldContextObject"))
	static UDialogueEffectParser* CreateParser(UObject* WorldContextObject);

	/**
	 * Parse and execute effect string
	 * Convenience function that combines parsing and execution
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Parser")
	static bool ParseAndExecuteEffect(const FString& EffectString, UDialogueSessionContext* Context);

	/**
	 * Parse and execute multiple effects
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue|Parser")
	static bool ParseAndExecuteEffects(const FString& EffectListString, UDialogueSessionContext* Context);
};
