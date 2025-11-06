// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameEventBusSubsystem.h"
#include "GameEventEffectRegistry.h"
#include "GameEventBusLibrary.generated.h"

/**
 * Blueprint Function Library 
 * 
 * 
 */
UCLASS()
class GAMEEVENTBUS_API UGameEventBusLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//~ Begin Quick Event Emission Helpers

	/**
	 * Emit simple event (just tag)
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitSimpleEvent(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		bool bLogEvent = false
	);

	/**
	 * Emit event with string parameter
	 * , NpcId, QuestId 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitEventWithString(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		FName StringParam,
		bool bLogEvent = false
	);

	/**
	 * Emit event with int parameter
	 * , Amount 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitEventWithInt(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		int32 IntParam,
		bool bLogEvent = false
	);

	/**
	 * Emit event with actors
	 * 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitEventWithActors(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		AActor* InstigatorActor,
		AActor* TargetActor,
		bool bLogEvent = false
	);

	/**
	 * Emit item-related event
	 * Shortcut 
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Items", meta = (WorldContext = "WorldContextObject"))
	static void EmitItemEvent(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		FName ItemId,
		int32 Count = 1,
		AActor* Instigator = nullptr,
		bool bLogEvent = false
	);

	/**
	 * Emit dialogue-related event
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Dialogue", meta = (WorldContext = "WorldContextObject"))
	static void EmitDialogueEvent(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		FName DialogueId,
		FName NodeId = NAME_None,
		AActor* Player = nullptr,
		AActor* NPC = nullptr,
		bool bLogEvent = false
	);

	/**
	 * Emit quest-related event
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Quest", meta = (WorldContext = "WorldContextObject"))
	static void EmitQuestEvent(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		FName QuestId,
		FName ObjectiveId = NAME_None,
		int32 Progress = 0,
		bool bLogEvent = false
	);

	/**
	 * Emit time-related event
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Time", meta = (WorldContext = "WorldContextObject"))
	static void EmitTimeEvent(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		int32 Hour,
		int32 Day = 0,
		bool bLogEvent = false
	);

	//~ End Quick Event Emission Helpers

	//~ Begin Registry Access

	/**
	 * Get Effect Registry instance
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events|Registry", meta = (WorldContext = "WorldContextObject"))
	static UGameEventEffectRegistry* GetEffectRegistry(UObject* WorldContextObject);

	/**
	 * Execute registered effect
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Registry", meta = (WorldContext = "WorldContextObject"))
	static bool ExecuteRegisteredEffect(
		UObject* WorldContextObject,
		const FGameplayTag& EffectTag,
		const FGameEventPayload& Payload
	);

	/**
	 * Evaluate registered condition
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Registry", meta = (WorldContext = "WorldContextObject"))
	static bool EvaluateRegisteredCondition(
		UObject* WorldContextObject,
		const FGameplayTag& ConditionTag,
		const FGameEventPayload& Payload
	);

	//~ End Registry Access

	//~ Begin Payload Builders

	/**
	 * Create payload for item events
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events|Payload")
	static FGameEventPayload MakeItemPayload(
		const FGameplayTag& EventTag,
		FName ItemId,
		int32 Count = 1,
		AActor* Instigator = nullptr
	);

	/**
	 * Create payload for dialogue events
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events|Payload")
	static FGameEventPayload MakeDialoguePayload(
		const FGameplayTag& EventTag,
		FName DialogueId,
		FName NodeId,
		AActor* Player = nullptr,
		AActor* NPC = nullptr
	);

	/**
	 * Create payload for quest events
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events|Payload")
	static FGameEventPayload MakeQuestPayload(
		const FGameplayTag& EventTag,
		FName QuestId,
		FName ObjectiveId = NAME_None,
		int32 Progress = 0
	);

	/**
	 * Create payload for time events
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events|Payload")
	static FGameEventPayload MakeTimePayload(
		const FGameplayTag& EventTag,
		int32 Hour,
		int32 Day = 0
	);

	//~ End Payload Builders

	//~ Begin Utility

	/**
	 * Get GameEventBus subsystem
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events", meta = (WorldContext = "WorldContextObject"))
	static UGameEventBusSubsystem* GetGameEventBus(UObject* WorldContextObject);

	//~ End Utility
};
