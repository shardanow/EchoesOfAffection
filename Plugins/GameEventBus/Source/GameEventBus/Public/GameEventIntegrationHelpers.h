// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameEventIntegrationHelpers.generated.h"

/**
 * Integration Helpers
 * 
 * Упрощённые функции для интеграции систем с GameEventBus
 * Позволяют настраивать взаимодействие систем через Blueprint без кода
 */
UCLASS()
class GAMEEVENTBUS_API UGameEventIntegrationHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//~ Begin Effect Registration Helpers

	/**
	 * Register Quest Effect
	 * Регистрирует эффект для запуска квеста
	 * 
	 * @param WorldContext World context
	 * @param EffectTag Tag для этого эффекта (например, "Effect.Quest.Start")
	 * @param QuestId ID квеста который будет запускаться
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Quest", meta = (WorldContext = "WorldContext"))
	static void RegisterQuestStartEffect(
		UObject* WorldContext,
		const FGameplayTag& EffectTag,
		FName QuestId
	);

	/**
	 * Register Dialogue Effect
	 * Регистрирует эффект для запуска диалога
	 * 
	 * @param WorldContext World context
	 * @param EffectTag Tag для этого эффекта (например, "Effect.Dialogue.Start")
	 * @param DialogueId ID диалога который будет запускаться
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Dialogue", meta = (WorldContext = "WorldContext"))
	static void RegisterDialogueStartEffect(
		UObject* WorldContext,
		const FGameplayTag& EffectTag,
		FName DialogueId
	);

	/**
	 * Register Item Give Effect
	 * Регистрирует эффект для выдачи предмета
	 * 
	 * @param WorldContext World context
	 * @param EffectTag Tag для этого эффекта (например, "Effect.Inventory.GiveItem")
	 * @param ItemId ID предмета
	 * @param Count Количество
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
	 * Регистрирует условие "квест активен"
	 * 
	 * @param WorldContext World context
	 * @param ConditionTag Tag для этого условия (например, "Condition.Quest.IsActive")
	 * @param QuestId ID квеста для проверки
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Quest", meta = (WorldContext = "WorldContext"))
	static void RegisterQuestActiveCondition(
		UObject* WorldContext,
		const FGameplayTag& ConditionTag,
		FName QuestId
	);

	/**
	 * Register Item Count Condition
	 * Регистрирует условие "игрок имеет N предметов"
	 * 
	 * @param WorldContext World context
	 * @param ConditionTag Tag для этого условия (например, "Condition.Inventory.HasItem")
	 * @param ItemId ID предмета
	 * @param RequiredCount Требуемое количество
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
	 * Регистрирует условие "время в диапазоне"
	 * 
	 * @param WorldContext World context
	 * @param ConditionTag Tag для этого условия (например, "Condition.Time.InRange")
	 * @param StartHour Начальный час (0-23)
	 * @param EndHour Конечный час (0-23)
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
	 * Автоматически настраивает связь квестов и диалогов
	 * 
	 * - Регистрирует эффекты запуска квестов из диалогов
	 * - Регистрирует условия проверки квестов в диалогах
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Setup", meta = (WorldContext = "WorldContext"))
	static void SetupQuestDialogueIntegration(UObject* WorldContext);

	/**
	 * Setup Inventory-Quest Integration
	 * Автоматически настраивает связь инвентаря и квестов
	 * 
	 * - Настраивает эмиссию событий получения предметов
	 * - Регистрирует условия проверки предметов для квестов
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Setup", meta = (WorldContext = "WorldContext"))
	static void SetupInventoryQuestIntegration(UObject* WorldContext);

	/**
	 * Setup Time-Quest Integration
	 * Автоматически настраивает связь времени и квестов
	 * 
	 * - Настраивает эмиссию событий времени
	 * - Регистрирует условия проверки времени для квестов
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Setup", meta = (WorldContext = "WorldContext"))
	static void SetupTimeQuestIntegration(UObject* WorldContext);

	/**
	 * Setup All Integrations
	 * Автоматически настраивает ВСЕ интеграции
	 * 
	 * Вызовите эту функцию один раз при старте игры
	 * (например, в GameMode BeginPlay или GameInstance Init)
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Integration|Setup", meta = (WorldContext = "WorldContext"))
	static void SetupAllIntegrations(UObject* WorldContext);

	//~ End Quick Setup Helpers

	//~ Begin Debugging

	/**
	 * Print all registered effects and conditions
	 * Полезно для отладки
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Debug", meta = (WorldContext = "WorldContext"))
	static void PrintRegisteredEffectsAndConditions(UObject* WorldContext);

	/**
	 * Test event emission
	 * Эмитирует тестовое событие для проверки
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Debug", meta = (WorldContext = "WorldContext"))
	static void TestEventEmission(
		UObject* WorldContext,
		const FGameplayTag& EventTag,
		FName StringParam = NAME_None
	);

	//~ End Debugging
};
