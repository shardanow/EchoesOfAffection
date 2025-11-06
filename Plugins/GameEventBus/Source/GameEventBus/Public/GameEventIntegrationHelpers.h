// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameEventIntegrationHelpers.generated.h"

/**
 * Integration Helpers
 * 
 * 
 * 
 */
UCLASS()
class GAMEEVENTBUS_API UGameEventIntegrationHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//~ Begin Effect Registration Helpers

	/**
	 * Register Quest Effect
	 * 
	 * 
	 * @param WorldContext World context
	 * @param EffectTag Tag , "Effect.Quest.Start")
	 * @param QuestId ID 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Quest", meta = (WorldContext = "WorldContext"))
	static void RegisterQuestStartEffect(
		UObject* WorldContext,
		const FGameplayTag& EffectTag,
		FName QuestId
	);

	/**
	 * Register Dialogue Effect
	 * 
	 * 
	 * @param WorldContext World context
	 * @param EffectTag Tag , "Effect.Dialogue.Start")
	 * @param DialogueId ID 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Dialogue", meta = (WorldContext = "WorldContext"))
	static void RegisterDialogueStartEffect(
		UObject* WorldContext,
		const FGameplayTag& EffectTag,
		FName DialogueId
	);

	/**
	 * Register Item Give Effect
	 * 
	 * 
	 * @param WorldContext World context
	 * @param EffectTag Tag , "Effect.Inventory.GiveItem")
	 * @param ItemId ID 
	 * @param Count 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Inventory", meta = (WorldContext = "WorldContext"))
	static void RegisterItemGiveEffect(
		UObject* WorldContext,
		const FGameplayTag& EffectTag,
		FName ItemId,
		int32 Count = 1
	);

	//~ End Effect Registration Helpers

	//~ Begin Condition Registration Helpers

	/**
	 * Register Quest Active Condition
	 * 
	 * 
	 * @param WorldContext World context
	 * @param ConditionTag Tag , "Condition.Quest.IsActive")
	 * @param QuestId ID 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Quest", meta = (WorldContext = "WorldContext"))
	static void RegisterQuestActiveCondition(
		UObject* WorldContext,
		const FGameplayTag& ConditionTag,
		FName QuestId
	);

	/**
	 * Register Item Count Condition
	 * 
	 * 
	 * @param WorldContext World context
	 * @param ConditionTag Tag , "Condition.Inventory.HasItem")
	 * @param ItemId ID 
	 * @param RequiredCount 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Inventory", meta = (WorldContext = "WorldContext"))
	static void RegisterItemCountCondition(
		UObject* WorldContext,
		const FGameplayTag& ConditionTag,
		FName ItemId,
		int32 RequiredCount = 1
	);

	/**
	 * Register Time Range Condition
	 * 
	 * 
	 * @param WorldContext World context
	 * @param ConditionTag Tag , "Condition.Time.InRange")
	 * @param StartHour )
	 * @param EndHour )
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Time", meta = (WorldContext = "WorldContext"))
	static void RegisterTimeRangeCondition(
		UObject* WorldContext,
		const FGameplayTag& ConditionTag,
		int32 StartHour,
		int32 EndHour
	);

	//~ End Condition Registration Helpers

	//~ Begin Quick Setup Helpers

	/**
	 * Setup Quest-Dialogue Integration
	 * 
	 * 
	 * - 
	 * - 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Setup", meta = (WorldContext = "WorldContext"))
	static void SetupQuestDialogueIntegration(UObject* WorldContext);

	/**
	 * Setup Inventory-Quest Integration
	 * 
	 * 
	 * - 
	 * - 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Setup", meta = (WorldContext = "WorldContext"))
	static void SetupInventoryQuestIntegration(UObject* WorldContext);

	/**
	 * Setup Time-Quest Integration
	 * 
	 * 
	 * - 
	 * - 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Setup", meta = (WorldContext = "WorldContext"))
	static void SetupTimeQuestIntegration(UObject* WorldContext);

	/**
	 * Setup All Integrations
	 * 
	 * 
	 * 
	 * (, )
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Setup", meta = (WorldContext = "WorldContext"))
	static void SetupAllIntegrations(UObject* WorldContext);

	//~ End Quick Setup Helpers

	//~ Begin Debugging

	/**
	 * Print all registered effects and conditions
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Debug", meta = (WorldContext = "WorldContext"))
	static void PrintRegisteredEffectsAndConditions(UObject* WorldContext);

	/**
	 * Test event emission
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Debug", meta = (WorldContext = "WorldContext"))
	static void TestEventEmission(
		UObject* WorldContext,
		const FGameplayTag& EventTag,
		FName StringParam = NAME_None
	);

	//~ End Debugging
};
