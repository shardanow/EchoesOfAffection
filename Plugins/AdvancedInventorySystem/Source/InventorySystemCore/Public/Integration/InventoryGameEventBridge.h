// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "InventoryGameEventBridge.generated.h"

/**
 * Inventory System -> Game Event Bus Bridge
 * 
 * Упрощает интеграцию системы инвентаря с GameEventBus
 * Предоставляет удобные функции для эмиссии событий
 * 
 * События инвентаря:
 * - Inventory.Event.ItemAcquired - получен предмет
 * - Inventory.Event.ItemUsed - использован предмет
 * - Inventory.Event.ItemDropped - выброшен предмет
 * - Inventory.Event.ItemCrafted - создан предмет
 * - Inventory.Event.ItemSold - продан предмет
 * - Inventory.Event.ItemBought - куплен предмет
 * 
 * Эти события автоматически подхватываются квестовой системой!
 */
UCLASS()
class INVENTORYSYSTEMCORE_API UInventoryGameEventBridge : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Emit event when item is acquired
	 * 
	 * @param WorldContextObject World context
	 * @param ItemId Item identifier
	 * @param Count Number of items acquired
	 * @param Acquirer Actor who acquired the item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitItemAcquiredEvent(
		UObject* WorldContextObject,
		FName ItemId,
		int32 Count = 1,
		AActor* Acquirer = nullptr
	);

	/**
	 * Emit event when item is used
	 * 
	 * @param WorldContextObject World context
	 * @param ItemId Item identifier
	 * @param User Actor who used the item
	 * @param Target Target actor (if any)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitItemUsedEvent(
		UObject* WorldContextObject,
		FName ItemId,
		AActor* User = nullptr,
		AActor* Target = nullptr
	);

	/**
	 * Emit event when item is crafted
	 * 
	 * @param WorldContextObject World context
	 * @param ItemId Item identifier
	 * @param Count Number of items crafted
	 * @param Crafter Actor who crafted the item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitItemCraftedEvent(
		UObject* WorldContextObject,
		FName ItemId,
		int32 Count = 1,
		AActor* Crafter = nullptr
	);

	/**
	 * Emit event when item is given to NPC
	 * 
	 * @param WorldContextObject World context
	 * @param ItemId Item identifier
	 * @param Count Number of items given
	 * @param Giver Actor giving the item
	 * @param Receiver NPC receiving the item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitItemGiftedEvent(
		UObject* WorldContextObject,
		FName ItemId,
		int32 Count,
		AActor* Giver,
		AActor* Receiver
	);

	/**
	 * Emit event when item is dropped
	 * 
	 * @param WorldContextObject World context
	 * @param ItemId Item identifier
	 * @param Count Number of items dropped
	 * @param Dropper Actor who dropped the item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitItemDroppedEvent(
		UObject* WorldContextObject,
		FName ItemId,
		int32 Count,
		AActor* Dropper = nullptr
	);

	/**
	 * Emit generic inventory event with custom parameters
	 * 
	 * @param WorldContextObject World context
	 * @param EventTag Custom event tag (should be under Inventory.Event.*)
	 * @param ItemId Item identifier
	 * @param Count Item count
	 * @param Instigator Instigator actor
	 * @param Target Target actor
	 * @param AdditionalTags Additional tags for filtering
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events", meta = (WorldContext = "WorldContextObject"))
	static void EmitCustomInventoryEvent(
		UObject* WorldContextObject,
		const FGameplayTag& EventTag,
		FName ItemId,
		int32 Count = 1,
		AActor* Instigator = nullptr,
		AActor* Target = nullptr,
		const FGameplayTagContainer& AdditionalTags = FGameplayTagContainer()
	);
};
