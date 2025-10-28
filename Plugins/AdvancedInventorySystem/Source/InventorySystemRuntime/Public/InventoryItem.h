// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventorySystemTypes.h"
#include "ItemDataAsset.h"
#include "InventoryItem.generated.h"

/**
 * Runtime instance of an item in the inventory
 * Represents a specific instance with unique ID and state
 */
UCLASS(BlueprintType)
class INVENTORYSYSTEMRUNTIME_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItem();

	// ========== CORE DATA ==========

	/** Unique identifier for this specific item instance */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	FInventoryItemId ItemInstanceId;

	/** Reference to the item data asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item")
	TObjectPtr<UItemDataAsset> ItemData;

	/** Stack information for this item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item")
	FInventoryStack Stack;

	/** Custom instance-specific tags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item")
	FGameplayTagContainer InstanceTags;

	/** Custom metadata for this instance (key-value pairs) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item")
	TMap<FName, FString> CustomMetadata;

	/** Whether this item is currently equipped */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	bool bIsEquipped = false;

	/** Last time this item was used (game time) */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Item")
	float LastUsedTime = 0.0f;

	// ========== CREATION ==========

	/**
	 * Create a new inventory item from item data
	 * @param InItemData The item data asset
	 * @param InQuantity Initial quantity
	 * @param Outer The outer object (usually the inventory component)
	 * @return The created inventory item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item", meta = (WorldContext = "WorldContextObject"))
	static UInventoryItem* CreateInventoryItem(UObject* WorldContextObject, UItemDataAsset* InItemData, int32 InQuantity = 1);

	// ========== QUERIES ==========

	/** Get the display name for this item */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	FText GetDisplayName() const;

	/** Get the description for this item */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	FText GetDescription() const;

	/** Check if this item is valid */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	bool IsValid() const;

	/** Check if this item can stack with another item */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	bool CanStackWith(const UInventoryItem* OtherItem) const;

	/** Get the current quantity */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	int32 GetQuantity() const { return Stack.Quantity; }

	/** Get the maximum stack size */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	int32 GetMaxStackSize() const;

	/** Check if the stack is full */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	bool IsStackFull() const;

	/** Get remaining capacity in this stack */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	int32 GetRemainingCapacity() const;

	/** Check if this item can be used right now */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	bool CanBeUsed(AActor* User) const;

	/** Check if this item is on cooldown */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	bool IsOnCooldown(const UWorld* World) const;

	/** Get remaining cooldown time */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	float GetRemainingCooldown(const UWorld* World) const;

	// ========== MODIFICATION ==========

	/**
	 * Add quantity to this stack
	 * @param Amount Amount to add
	 * @return Amount actually added
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	int32 AddQuantity(int32 Amount);

	/**
	 * Remove quantity from this stack
	 * @param Amount Amount to remove
	 * @return Amount actually removed
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	int32 RemoveQuantity(int32 Amount);

	/**
	 * Set the quantity directly
	 * @param NewQuantity New quantity value
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	void SetQuantity(int32 NewQuantity);

	/**
	 * Mark this item as equipped
	 * @param bEquipped Whether the item is equipped
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	void SetEquipped(bool bEquipped);

	/**
	 * Update the last used time
	 * @param World World context for getting time
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	void UpdateLastUsedTime(const UWorld* World);

	// ========== METADATA ==========

	/**
	 * Set custom metadata value
	 * @param Key Metadata key
	 * @param Value Metadata value
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	void SetMetadata(FName Key, const FString& Value);

	/**
	 * Get custom metadata value
	 * @param Key Metadata key
	 * @param OutValue Retrieved value
	 * @return True if the key was found
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory Item")
	bool GetMetadata(FName Key, FString& OutValue) const;

	/**
	 * Remove custom metadata
	 * @param Key Metadata key to remove
	 * @return True if the key was removed
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	bool RemoveMetadata(FName Key);

	// ========== SERIALIZATION ==========

	/** Serialize this item to JSON */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	FString ToJson() const;

	/** Deserialize this item from JSON */
	UFUNCTION(BlueprintCallable, Category = "Inventory Item")
	bool FromJson(const FString& JsonString);
};
