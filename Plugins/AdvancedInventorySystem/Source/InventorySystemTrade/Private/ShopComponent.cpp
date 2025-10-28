// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShopComponent.h"
#include "ShopDataAsset.h"
#include "ItemDataAsset.h"
#include "InventoryComponent.h"
#include "Engine/World.h"

UShopComponent::UShopComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsOpen = true;
	DiscountModifier = 1.0f;
}

void UShopComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeRuntimeInventory();
}

void UShopComponent::InitializeRuntimeInventory()
{
	RuntimeInventory.Empty();

	if (!ShopData)
	{
		return;
	}

	for (const FShopItemEntry& Entry : ShopData->ShopInventory)
	{
		if (!Entry.Item)
		{
			continue;
		}

		FShopInventoryRuntime& Runtime = RuntimeInventory.AddDefaulted_GetRef();
		Runtime.Item = Entry.Item;
		Runtime.CurrentStock = Entry.StockQuantity;
		Runtime.LastRestockTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	}
}

bool UShopComponent::PurchaseItem(UItemDataAsset* Item, int32 Quantity, AActor* Buyer, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	if (!bIsOpen || !ShopData || !Item || !Buyer || Quantity <= 0)
	{
		OutResult = EInventoryOperationResult::Failed_InvalidItem;
		return false;
	}

	// Check if item is available
	if (!IsItemAvailable(Item, Quantity, Buyer))
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientQuantity;
		return false;
	}

	// Calculate price
	int32 TotalPrice = GetPurchasePrice(Item, Quantity, Buyer);

	// Check if buyer can afford it
	if (!CanAffordPurchase(Buyer, TotalPrice))
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		return false;
	}

	// Get buyer's inventory
	UInventoryComponent* BuyerInventory = Cast<UInventoryComponent>(
		Buyer->GetComponentByClass(UInventoryComponent::StaticClass())
	);

	if (!BuyerInventory)
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		return false;
	}

	// Check if buyer has space
	if (!BuyerInventory->CanAddItem(Item, Quantity))
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientSpace;
		return false;
	}

	// Process payment
	if (!ProcessPayment(Buyer, TotalPrice))
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		return false;
	}

	// Add item to buyer's inventory
	UInventoryItem* AddedItem = BuyerInventory->AddItem(Item, Quantity, OutResult);
	
	if (OutResult != EInventoryOperationResult::Success || !AddedItem)
	{
		// Refund if item add failed
		ProcessRefund(Buyer, TotalPrice);
		return false;
	}

	// Update stock
	UpdateStock(Item, -Quantity);

	// Broadcast event
	OnItemPurchased.Broadcast(Item, Quantity, TotalPrice, Buyer);
	OnShopInventoryChanged.Broadcast();

	OutResult = EInventoryOperationResult::Success;
	return true;
}

bool UShopComponent::SellItem(UItemDataAsset* Item, int32 Quantity, AActor* Seller, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	if (!bIsOpen || !ShopData || !Item || !Seller || Quantity <= 0)
	{
		OutResult = EInventoryOperationResult::Failed_InvalidItem;
		return false;
	}

	// Check if shop buys this item
	if (!WillBuyItem(Item))
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		return false;
	}

	// Get seller's inventory
	UInventoryComponent* SellerInventory = Cast<UInventoryComponent>(
		Seller->GetComponentByClass(UInventoryComponent::StaticClass())
	);

	if (!SellerInventory)
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		return false;
	}

	// Check if seller has the items
	if (!SellerInventory->HasItem(Item, Quantity))
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientQuantity;
		return false;
	}

	// Calculate sellback price
	int32 TotalPrice = GetSellbackPrice(Item, Quantity, Seller);

	// Remove items from seller's inventory
	int32 Removed = SellerInventory->RemoveItemByData(Item, Quantity, OutResult);
	
	if (OutResult != EInventoryOperationResult::Success || Removed != Quantity)
	{
		return false;
	}

	// Give currency to seller
	ProcessRefund(Seller, TotalPrice);

	// Broadcast event
	OnItemSold.Broadcast(Item, Quantity, TotalPrice, Seller);

	OutResult = EInventoryOperationResult::Success;
	return true;
}

int32 UShopComponent::GetPurchasePrice(UItemDataAsset* Item, int32 Quantity, AActor* Buyer) const
{
	if (!Item || !ShopData || Quantity <= 0)
	{
		return 0;
	}

	// Find shop entry for this item
	int32 BasePrice = Item->BaseValue;
	
	for (const FShopItemEntry& Entry : ShopData->ShopInventory)
	{
		if (Entry.Item == Item)
		{
			if (Entry.PurchasePrice > 0)
			{
				BasePrice = Entry.PurchasePrice;
			}
			break;
		}
	}

	// Apply modifiers
	float FinalPrice = static_cast<float>(BasePrice);
	FinalPrice *= ShopData->PriceModifier;
	FinalPrice *= DiscountModifier;

	// Call price modifier interface if implemented
	if (Implements<UShopPriceModifier>())
	{
		FinalPrice = static_cast<float>(Execute_CalculatePrice(const_cast<UShopComponent*>(this), 
			static_cast<int32>(FinalPrice), Item, Buyer, Quantity));
	}

	return FMath::Max(0, static_cast<int32>(FinalPrice * Quantity));
}

int32 UShopComponent::GetSellbackPrice(UItemDataAsset* Item, int32 Quantity, AActor* Seller) const
{
	if (!Item || !ShopData || Quantity <= 0)
	{
		return 0;
	}

	int32 BasePrice = Item->BaseValue;
	float SellbackPercent = Item->SellbackPercentage;

	// Use shop's sellback percentage if specified
	if (ShopData->SellbackPercentage > 0.0f)
	{
		SellbackPercent = ShopData->SellbackPercentage;
	}

	float FinalPrice = static_cast<float>(BasePrice) * SellbackPercent;

	// Call sellback modifier interface if implemented
	if (Implements<UShopPriceModifier>())
	{
		FinalPrice = static_cast<float>(Execute_CalculateSellbackPrice(const_cast<UShopComponent*>(this), 
			static_cast<int32>(FinalPrice), Item, Seller, Quantity));
	}

	return FMath::Max(0, static_cast<int32>(FinalPrice * Quantity));
}

int32 UShopComponent::CalculatePrice_Implementation(int32 BasePrice, const UItemDataAsset* ItemData, AActor* Buyer, int32 Quantity)
{
	// Default implementation - can be overridden
	return BasePrice;
}

int32 UShopComponent::CalculateSellbackPrice_Implementation(int32 OriginalPrice, const UItemDataAsset* ItemData, AActor* Seller, int32 Quantity)
{
	// Default implementation - can be overridden
	return OriginalPrice;
}

TArray<UItemDataAsset*> UShopComponent::GetAvailableItems() const
{
	TArray<UItemDataAsset*> Result;

	if (!ShopData)
	{
		return Result;
	}

	for (const FShopInventoryRuntime& Runtime : RuntimeInventory)
	{
		if (Runtime.Item && (Runtime.CurrentStock != 0))
		{
			Result.Add(Runtime.Item);
		}
	}

	return Result;
}

bool UShopComponent::IsItemAvailable(UItemDataAsset* Item, int32 Quantity, AActor* Buyer) const
{
	if (!Item || !ShopData || Quantity <= 0)
	{
		return false;
	}

	// Find shop entry
	const FShopItemEntry* Entry = ShopData->ShopInventory.FindByPredicate([Item](const FShopItemEntry& E) {
		return E.Item == Item;
	});

	if (!Entry)
	{
		return false;
	}

	// Check requirements
	if (Entry->RequiredTags.Num() > 0)
	{
		// Would need tag checking on buyer
	}

	if (Entry->MinimumLevel > 0)
	{
		// Would need level checking on buyer
	}

	// Check stock
	const FShopInventoryRuntime* Runtime = FindRuntimeInventory(Item);
	if (Runtime)
	{
		if (Runtime->CurrentStock >= 0 && Runtime->CurrentStock < Quantity)
		{
			return false;
		}
	}

	return true;
}

int32 UShopComponent::GetItemStock(UItemDataAsset* Item) const
{
	const FShopInventoryRuntime* Runtime = FindRuntimeInventory(Item);
	return Runtime ? Runtime->CurrentStock : 0;
}

bool UShopComponent::WillBuyItem(UItemDataAsset* Item) const
{
	if (!ShopData || !ShopData->bBuysItems || !Item)
	{
		return false;
	}

	// Check if item tags match buyable tags
	if (ShopData->BuyableItemTags.Num() > 0)
	{
		return Item->ItemTags.HasAny(ShopData->BuyableItemTags);
	}

	return true;
}

void UShopComponent::RestockAll()
{
	if (!ShopData)
	{
		return;
	}

	for (FShopInventoryRuntime& Runtime : RuntimeInventory)
	{
		RestockItem(Runtime.Item);
	}

	OnShopInventoryChanged.Broadcast();
}

void UShopComponent::RestockItem(UItemDataAsset* Item)
{
	if (!ShopData || !Item)
	{
		return;
	}

	// Find shop entry
	const FShopItemEntry* Entry = ShopData->ShopInventory.FindByPredicate([Item](const FShopItemEntry& E) {
		return E.Item == Item;
	});

	if (!Entry || !Entry->bRestocks)
	{
		return;
	}

	// Find runtime inventory
	FShopInventoryRuntime* Runtime = FindRuntimeInventory(Item);
	if (!Runtime)
	{
		return;
	}

	// Restock
	if (Runtime->CurrentStock >= 0)
	{
		Runtime->CurrentStock += Entry->RestockQuantity;
	}

	Runtime->LastRestockTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
}

FShopInventoryRuntime* UShopComponent::FindRuntimeInventory(UItemDataAsset* Item)
{
	return RuntimeInventory.FindByPredicate([Item](const FShopInventoryRuntime& R) {
		return R.Item == Item;
	});
}

const FShopInventoryRuntime* UShopComponent::FindRuntimeInventory(UItemDataAsset* Item) const
{
	return RuntimeInventory.FindByPredicate([Item](const FShopInventoryRuntime& R) {
		return R.Item == Item;
	});
}

bool UShopComponent::CanAffordPurchase(AActor* Buyer, int32 TotalPrice) const
{
	if (!Buyer || !ShopData || !ShopData->CurrencyItem)
	{
		return TotalPrice == 0;
	}

	UInventoryComponent* BuyerInventory = Cast<UInventoryComponent>(
		Buyer->GetComponentByClass(UInventoryComponent::StaticClass())
	);

	if (!BuyerInventory)
	{
		return false;
	}

	return BuyerInventory->GetItemQuantity(ShopData->CurrencyItem) >= TotalPrice;
}

bool UShopComponent::ProcessPayment(AActor* Buyer, int32 Amount)
{
	if (!Buyer || !ShopData || !ShopData->CurrencyItem || Amount <= 0)
	{
		return Amount == 0;
	}

	UInventoryComponent* BuyerInventory = Cast<UInventoryComponent>(
		Buyer->GetComponentByClass(UInventoryComponent::StaticClass())
	);

	if (!BuyerInventory)
	{
		return false;
	}

	EInventoryOperationResult Result;
	int32 Removed = BuyerInventory->RemoveItemByData(ShopData->CurrencyItem, Amount, Result);

	return Removed == Amount && Result == EInventoryOperationResult::Success;
}

void UShopComponent::ProcessRefund(AActor* Seller, int32 Amount)
{
	if (!Seller || !ShopData || !ShopData->CurrencyItem || Amount <= 0)
	{
		return;
	}

	UInventoryComponent* SellerInventory = Cast<UInventoryComponent>(
		Seller->GetComponentByClass(UInventoryComponent::StaticClass())
	);

	if (!SellerInventory)
	{
		return;
	}

	EInventoryOperationResult Result;
	SellerInventory->AddItem(ShopData->CurrencyItem, Amount, Result);
}

void UShopComponent::UpdateStock(UItemDataAsset* Item, int32 Delta)
{
	FShopInventoryRuntime* Runtime = FindRuntimeInventory(Item);
	if (Runtime && Runtime->CurrentStock >= 0)
	{
		Runtime->CurrentStock = FMath::Max(0, Runtime->CurrentStock + Delta);
	}
}

void UShopComponent::CheckAndProcessRestocks()
{
	if (!ShopData || !GetWorld())
	{
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();

	for (int32 i = 0; i < ShopData->ShopInventory.Num(); ++i)
	{
		const FShopItemEntry& Entry = ShopData->ShopInventory[i];
		
		if (!Entry.bRestocks || !Entry.Item)
		{
			continue;
		}

		FShopInventoryRuntime* Runtime = FindRuntimeInventory(Entry.Item);
		if (!Runtime)
		{
			continue;
		}

		float TimeSinceRestock = CurrentTime - Runtime->LastRestockTime;
		
		if (TimeSinceRestock >= Entry.RestockInterval)
		{
			RestockItem(Entry.Item);
		}
	}
}
