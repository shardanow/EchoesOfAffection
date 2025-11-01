// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryComponentGameEventHelper.generated.h"

class UInventoryComponent;
class UItemDataAsset;
class AActor;

/**
 * Helper library for InventoryComponent GameEventBus integration
 * This file contains utility functions to emit events from InventoryComponent
 */
UCLASS()
class INVENTORYSYSTEMRUNTIME_API UInventoryComponentGameEventHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Emit ItemAcquired event through GameEventBus (if available)
	 * Call this when items are added to inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events")
	static void EmitItemAcquiredEvent(UInventoryComponent* InventoryComponent, UItemDataAsset* ItemData, int32 Quantity, AActor* Player);

	/**
	 * Emit ItemUsed event through GameEventBus (if available)
	 * Call this when items are used/consumed
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events")
	static void EmitItemUsedEvent(UInventoryComponent* InventoryComponent, UItemDataAsset* ItemData, int32 Quantity, AActor* User);

	/**
	 * Emit ItemGifted event through GameEventBus (if available)
	 * Call this when items are gifted to NPCs
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Events")
	static void EmitItemGiftedEvent(UInventoryComponent* InventoryComponent, UItemDataAsset* ItemData, int32 Quantity, AActor* Giver, AActor* Receiver);
};
