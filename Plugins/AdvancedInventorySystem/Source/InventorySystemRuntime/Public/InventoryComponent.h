// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InventorySystemTypes.h"
#include "InventorySystemInterfaces.h"
#include "InventoryComponent.generated.h"

class UInventoryItem;
class UItemDataAsset;

/**
 * Delegate signatures for inventory events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryItemAdded, UInventoryItem*, Item, int32, Quantity, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryItemRemoved, UInventoryItem*, Item, int32, Quantity, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryItemUsed, UInventoryItem*, Item, AActor*, User, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInventoryItemGifted, UInventoryItem*, Item, AActor*, Giver, AActor*, Receiver, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemEquipped, UInventoryItem*, Item, bool, bEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryWeightChanged, float, CurrentWeight, float, MaxWeight);

/**
 * Component that manages an inventory for an actor
 * Event-driven, no Tick, fully data-driven
 */
UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEMRUNTIME_API UInventoryComponent : public UActorComponent, public IInventorySystemSaveInterface
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ========== CONFIGURATION ==========

	/** Maximum number of inventory slots (0 = unlimited) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Configuration", meta = (ClampMin = "0"))
	int32 MaxInventorySlots = 20;

	/** Maximum weight capacity (0 = unlimited, requires weight system) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Configuration", meta = (ClampMin = "0"))
	float MaxWeightCapacity = 0.0f;

	/** Whether to automatically stack items when adding */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Configuration")
	bool bAutoStack = true;

	/** Whether to automatically sort inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Configuration")
	bool bAutoSort = false;

	/** Tags that restrict what items can be added to this inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Configuration")
	FGameplayTagContainer AllowedItemTags;

	/** Tags that prevent items from being added to this inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Configuration")
	FGameplayTagContainer BlockedItemTags;

	// ========== DELEGATES ==========

	/** Called when an item is added to the inventory */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryItemAdded OnItemAdded;

	/** Called when an item is removed from the inventory */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryItemRemoved OnItemRemoved;

	/** Called when an item is used */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryItemUsed OnItemUsed;

	/** Called when an item is gifted */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryItemGifted OnItemGifted;

	/** Called when an item is equipped/unequipped */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryItemEquipped OnItemEquipped;

	/** Called when inventory changes in any way */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

	/** Called when inventory weight changes */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryWeightChanged OnWeightChanged;

	// ========== INVENTORY STORAGE ==========

protected:
	/** Array of inventory items */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory", Replicated)
	TArray<TObjectPtr<UInventoryItem>> InventoryItems;

	/** Map of item IDs to array indices for fast lookup */
	UPROPERTY()
	TMap<FInventoryItemId, int32> ItemIdToIndexMap;

public:
	// ========== ADD / REMOVE OPERATIONS ==========

	/**
	 * Add an item to the inventory by item data
	 * @param ItemData The item data to add
	 * @param Quantity Number of items to add
	 * @param OutResult Result of the operation
	 * @return The inventory item that was added/updated
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UInventoryItem* AddItem(UItemDataAsset* ItemData, int32 Quantity, EInventoryOperationResult& OutResult);

	/**
	 * Add an existing inventory item
	 * @param Item The item instance to add
	 * @param OutResult Result of the operation
	 * @return True if the item was added successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddInventoryItem(UInventoryItem* Item, EInventoryOperationResult& OutResult);

	/**
	 * Remove an item from the inventory by instance ID
	 * @param ItemId The item instance ID to remove
	 * @param Quantity Number of items to remove (0 = all)
	 * @param OutResult Result of the operation
	 * @return Number of items actually removed
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItemById(FInventoryItemId ItemId, int32 Quantity, EInventoryOperationResult& OutResult);

	/**
	 * Remove items by item data
	 * @param ItemData The item data to remove
	 * @param Quantity Number of items to remove
	 * @param OutResult Result of the operation
	 * @return Number of items actually removed
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItemByData(UItemDataAsset* ItemData, int32 Quantity, EInventoryOperationResult& OutResult);

	/**
	 * Remove all items from the inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearInventory();

	// ========== QUERIES ==========

	/**
	 * Get all items in the inventory
	 * @return Array of inventory items
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UInventoryItem*> GetAllItems() const;

	/**
	 * Get an item by its instance ID
	 * @param ItemId The item instance ID
	 * @return The inventory item, or nullptr if not found
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryItem* GetItemById(FInventoryItemId ItemId) const;

	/**
	 * Get an item at a specific slot index
	 * @param SlotIndex The slot index
	 * @return The inventory item, or nullptr if slot is empty
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryItem* GetItemAtSlot(int32 SlotIndex) const;

	/**
	 * Find all items of a specific type
	 * @param ItemData The item data to find
	 * @return Array of matching inventory items
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UInventoryItem*> FindItemsByData(UItemDataAsset* ItemData) const;

	/**
	 * Find all items with specific tags
	 * @param Tags The tags to search for
	 * @param bRequireAll Whether all tags must be present
	 * @return Array of matching inventory items
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UInventoryItem*> FindItemsByTags(FGameplayTagContainer Tags, bool bRequireAll = true) const;

	/**
	 * Check if the inventory contains a specific item
	 * @param ItemData The item data to check
	 * @param Quantity Minimum quantity required
	 * @return True if the inventory contains the item with at least the specified quantity
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(UItemDataAsset* ItemData, int32 Quantity = 1) const;

	/**
	 * Get the total quantity of a specific item
	 * @param ItemData The item data to count
	 * @return Total quantity of the item
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemQuantity(UItemDataAsset* ItemData) const;

	/**
	 * Get the number of used slots
	 * @return Number of slots with items
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetUsedSlots() const;

	/**
	 * Get the number of free slots
	 * @return Number of available slots
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetFreeSlots() const;

	/**
	 * Check if the inventory is full
	 * @return True if no more items can be added
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryFull() const;

	/**
	 * Check if the inventory is empty
	 * @return True if there are no items
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsInventoryEmpty() const;

	// ========== USAGE ==========

	/**
	 * Use an item from the inventory
	 * @param ItemId The item instance ID to use
	 * @param User The actor using the item
	 * @param OutResult Result of the operation
	 * @return True if the item was used successfully
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(FInventoryItemId ItemId, AActor* User, EInventoryOperationResult& OutResult);

	/**
	 * Gift an item to another actor
	 * @param ItemId The item instance ID to gift
	 * @param Receiver The actor receiving the gift
	 * @param Quantity Number of items to gift
	 * @param OutResult Result of the operation
	 * @return True if the gift was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool GiftItem(FInventoryItemId ItemId, AActor* Receiver, int32 Quantity, EInventoryOperationResult& OutResult);

	/**
	 * Drop an item from inventory into the world
	 * @param ItemId The item instance ID to drop
	 * @param Quantity Number of items to drop (0 = all)
	 * @param DropLocation Where to drop the item in the world
	 * @param OutResult Result of the operation
	 * @return The spawned world item actor, or nullptr if failed
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	class AWorldItemActor* DropItem(FInventoryItemId ItemId, int32 Quantity, FVector DropLocation, EInventoryOperationResult& OutResult);

	/**
	 * Drop item in front of the owner actor
	 * @param ItemId The item instance ID to drop
	 * @param Quantity Number of items to drop
	 * @param DropDistance Distance in front of owner to drop
	 * @param OutResult Result of the operation
	 * @return The spawned world item actor
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AWorldItemActor* DropItemInFront(FInventoryItemId ItemId, int32 Quantity, float DropDistance, EInventoryOperationResult& OutResult);

	// ========== EQUIPMENT ==========

	/**
	 * Equip an item
	 * @param ItemId The item instance ID to equip
	 * @param OutResult Result of the operation
	 * @return True if the item was equipped
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool EquipItem(FInventoryItemId ItemId, EInventoryOperationResult& OutResult);

	/**
	 * Unequip an item
	 * @param ItemId The item instance ID to unequip
	 * @param OutResult Result of the operation
	 * @return True if the item was unequipped
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UnequipItem(FInventoryItemId ItemId, EInventoryOperationResult& OutResult);

	/**
	 * Get all equipped items
	 * @return Array of equipped items
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<UInventoryItem*> GetEquippedItems() const;

	/**
	 * Get equipped item in a specific slot
	 * @param SlotTag The equipment slot tag
	 * @return The equipped item, or nullptr if slot is empty
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryItem* GetEquippedItemInSlot(FGameplayTag SlotTag) const;

	// ========== ORGANIZATION ==========

	/**
	 * Swap two items in the inventory
	 * @param SlotIndexA First slot index
	 * @param SlotIndexB Second slot index
	 * @return True if the swap was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapItems(int32 SlotIndexA, int32 SlotIndexB);

	/**
	 * Sort the inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SortInventory();

	// ========== WEIGHT SYSTEM ==========

	/**
	 * Get the total weight of all items in the inventory
	 * @return Total weight of all items (item weight * quantity)
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
	float GetTotalWeight() const;

	/**
	 * Get the remaining weight capacity
	 * @return How much more weight can be added (MaxWeightCapacity - CurrentWeight)
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
	float GetRemainingWeightCapacity() const;

	/**
	 * Get the weight capacity percentage (0.0 - 1.0)
	 * @return CurrentWeight / MaxWeightCapacity
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
	float GetWeightPercentage() const;

	/**
	 * Check if adding a specific weight would exceed capacity
	 * @param AdditionalWeight Weight to check
	 * @return True if the weight can be added without exceeding capacity
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
	bool CanAddWeight(float AdditionalWeight) const;

	/**
	 * Check if the inventory is overweight
	 * @return True if current weight exceeds max capacity
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
	bool IsOverweight() const;

	/**
	 * Get the weight of a specific item type
	 * @param ItemData The item data to check
	 * @param Quantity Number of items
	 * @return Total weight (item weight * quantity)
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Weight")
	static float GetItemWeight(UItemDataAsset* ItemData, int32 Quantity = 1);

	// ========== VALIDATION ==========

	/**
	 * Check if an item can be added to the inventory
	 * @param ItemData The item data to check
	 * @param Quantity Number of items to add
	 * @return True if the item can be added
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool CanAddItem(UItemDataAsset* ItemData, int32 Quantity) const;

	// ========== SAVE/LOAD ==========

	/** Save inventory to JSON string */
	virtual bool SaveToJson_Implementation(FString& OutJsonString) override;

	/** Load inventory from JSON string */
	virtual bool LoadFromJson_Implementation(const FString& JsonString) override;

	// ========== REPLICATION ==========
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// ========== INTERNAL HELPERS ==========

	/** Find the first empty slot */
	int32 FindEmptySlot() const;

	/** Find a slot with a specific item that can stack */
	int32 FindStackableSlot(UItemDataAsset* ItemData) const;

	/** Rebuild the item ID to index map */
	void RebuildItemIdMap();

	/** Check if item tags are allowed */
	bool AreItemTagsAllowed(const FGameplayTagContainer& ItemTags) const;

	/** Notify that inventory has changed */
	void NotifyInventoryChanged();
};
