// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystemTypes.h"
#include "InventorySystemInterfaces.h"
#include "ShopComponent.generated.h"

class UShopDataAsset;
class UItemDataAsset;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnItemPurchased, UItemDataAsset*, Item, int32, Quantity, int32, TotalPrice, AActor*, Buyer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnItemSold, UItemDataAsset*, Item, int32, Quantity, int32, TotalPrice, AActor*, Seller);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopInventoryChanged);

/**
 * Runtime shop inventory tracking
 */
USTRUCT(BlueprintType)
struct FShopInventoryRuntime
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UItemDataAsset> Item;

	UPROPERTY()
	int32 CurrentStock = -1;

	UPROPERTY()
	float LastRestockTime = 0.0f;
};

/**
 * Component that manages shop functionality
 */
UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEMTRADE_API UShopComponent : public UActorComponent, public IShopPriceModifier
{
	GENERATED_BODY()

public:	
	UShopComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ========== CONFIGURATION ==========

	/** Shop data asset defining inventory and prices */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TObjectPtr<UShopDataAsset> ShopData;

	/** Whether this shop is currently open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	bool bIsOpen = true;

	/** Global discount modifier (0.0-1.0, where 0.9 = 10% discount) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Pricing", meta = (ClampMin = "0", ClampMax = "2"))
	float DiscountModifier = 1.0f;

	// ========== DELEGATES ==========

	UPROPERTY(BlueprintAssignable, Category = "Shop|Events")
	FOnItemPurchased OnItemPurchased;

	UPROPERTY(BlueprintAssignable, Category = "Shop|Events")
	FOnItemSold OnItemSold;

	UPROPERTY(BlueprintAssignable, Category = "Shop|Events")
	FOnShopInventoryChanged OnShopInventoryChanged;

	// ========== RUNTIME DATA ==========

protected:
	UPROPERTY()
	TArray<FShopInventoryRuntime> RuntimeInventory;

public:
	// ========== PURCHASE ==========

	/**
	 * Purchase an item from the shop
	 * @param Item The item to purchase
	 * @param Quantity Number of items to purchase
	 * @param Buyer The actor buying the item
	 * @param OutResult Result of the operation
	 * @return True if purchase was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool PurchaseItem(UItemDataAsset* Item, int32 Quantity, AActor* Buyer, EInventoryOperationResult& OutResult);

	/**
	 * Sell an item to the shop
	 * @param Item The item to sell
	 * @param Quantity Number of items to sell
	 * @param Seller The actor selling the item
	 * @param OutResult Result of the operation
	 * @return True if sale was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool SellItem(UItemDataAsset* Item, int32 Quantity, AActor* Seller, EInventoryOperationResult& OutResult);

	// ========== PRICING ==========

	/**
	 * Get the purchase price for an item
	 * @param Item The item to price
	 * @param Quantity Number of items
	 * @param Buyer The actor buying (for dynamic pricing)
	 * @return Total purchase price
	 */
	UFUNCTION(BlueprintPure, Category = "Shop")
	int32 GetPurchasePrice(UItemDataAsset* Item, int32 Quantity, AActor* Buyer) const;

	/**
	 * Get the sellback price for an item
	 * @param Item The item to price
	 * @param Quantity Number of items
	 * @param Seller The actor selling
	 * @return Total sellback price
	 */
	UFUNCTION(BlueprintPure, Category = "Shop")
	int32 GetSellbackPrice(UItemDataAsset* Item, int32 Quantity, AActor* Seller) const;

	// IShopPriceModifier interface
	virtual int32 CalculatePrice_Implementation(int32 BasePrice, const UItemDataAsset* ItemData, AActor* Buyer, int32 Quantity) override;
	virtual int32 CalculateSellbackPrice_Implementation(int32 OriginalPrice, const UItemDataAsset* ItemData, AActor* Seller, int32 Quantity) override;

	// ========== INVENTORY QUERIES ==========

	/**
	 * Get all items available in the shop
	 * @return Array of available items
	 */
	UFUNCTION(BlueprintPure, Category = "Shop")
	TArray<UItemDataAsset*> GetAvailableItems() const;

	/**
	 * Check if an item is available for purchase
	 * @param Item The item to check
	 * @param Quantity Number of items needed
	 * @param Buyer The actor attempting to buy
	 * @return True if the item can be purchased
	 */
	UFUNCTION(BlueprintPure, Category = "Shop")
	bool IsItemAvailable(UItemDataAsset* Item, int32 Quantity, AActor* Buyer) const;

	/**
	 * Get current stock of an item
	 * @param Item The item to check
	 * @return Current stock (-1 = unlimited)
	 */
	UFUNCTION(BlueprintPure, Category = "Shop")
	int32 GetItemStock(UItemDataAsset* Item) const;

	/**
	 * Check if shop will buy an item type
	 * @param Item The item to check
	 * @return True if shop will buy this item
	 */
	UFUNCTION(BlueprintPure, Category = "Shop")
	bool WillBuyItem(UItemDataAsset* Item) const;

	// ========== RESTOCKING ==========

	/**
	 * Manually restock all items
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RestockAll();

	/**
	 * Restock a specific item
	 * @param Item The item to restock
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RestockItem(UItemDataAsset* Item);

protected:
	// ========== INTERNAL ==========

	void InitializeRuntimeInventory();
	FShopInventoryRuntime* FindRuntimeInventory(UItemDataAsset* Item);
	const FShopInventoryRuntime* FindRuntimeInventory(UItemDataAsset* Item) const;
	bool CanAffordPurchase(AActor* Buyer, int32 TotalPrice) const;
	bool ProcessPayment(AActor* Buyer, int32 Amount);
	void ProcessRefund(AActor* Seller, int32 Amount);
	void UpdateStock(UItemDataAsset* Item, int32 Delta);
	void CheckAndProcessRestocks();
};
