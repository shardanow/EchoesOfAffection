// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ShopDataAsset.generated.h"

class UItemDataAsset;

/**
 * Shop item entry defining what's available for purchase
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMTRADE_API FShopItemEntry
{
	GENERATED_BODY()

	/** The item for sale */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TObjectPtr<UItemDataAsset> Item;

	/** Purchase price (overrides item's base value if > 0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop", meta = (ClampMin = "0"))
	int32 PurchasePrice = 0;

	/** Stock quantity (-1 = unlimited) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	int32 StockQuantity = -1;

	/** Whether this item restocks */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	bool bRestocks = false;

	/** Restock interval in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop", meta = (EditCondition = "bRestocks", ClampMin = "0"))
	float RestockInterval = 86400.0f; // 24 hours default

	/** Restock quantity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop", meta = (EditCondition = "bRestocks", ClampMin = "1"))
	int32 RestockQuantity = 1;

	/** Required tags to purchase this item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	FGameplayTagContainer RequiredTags;

	/** Minimum level to purchase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop", meta = (ClampMin = "0"))
	int32 MinimumLevel = 0;
};

/**
 * Data asset defining a shop's inventory and pricing
 */
UCLASS(BlueprintType)
class INVENTORYSYSTEMTRADE_API UShopDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UShopDataAsset();

	/** Shop name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	FText ShopName;

	/** Shop description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop", meta = (MultiLine = true))
	FText ShopDescription;

	/** Items available in this shop */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	TArray<FShopItemEntry> ShopInventory;

	/** Currency item required for purchases */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Currency")
	TObjectPtr<UItemDataAsset> CurrencyItem;

	/** Global price modifier (multiplier) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Pricing", meta = (ClampMin = "0"))
	float PriceModifier = 1.0f;

	/** Sellback percentage for items sold to this shop */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Pricing", meta = (ClampMin = "0", ClampMax = "1"))
	float SellbackPercentage = 0.5f;

	/** Whether this shop buys items from players */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	bool bBuysItems = true;

	/** Tags of items this shop will buy (empty = buys all) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop", meta = (EditCondition = "bBuysItems"))
	FGameplayTagContainer BuyableItemTags;
};
