// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShopWidgetBase.h"
#include "ShopComponent.h"
#include "ShopDataAsset.h"
#include "ItemDataAsset.h"
#include "ItemTooltipWidget.h"
#include "InventoryComponent.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UShopWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind close button if present (only if not already bound)
	if (CloseButton && !CloseButton->OnClicked.IsBound())
	{
		CloseButton->OnClicked.AddDynamic(this, &UShopWidgetBase::HideShop);
	}

	UpdateWidgets();
}

void UShopWidgetBase::NativeDestruct()
{
	// DON'T unbind from shop on destruct!
	// For shop, we DO want to unbind (shop is created fresh each time)
	// But we'll do it in HideShop() instead
	// UnbindFromShop();  // ? REMOVED - will be called in HideShop()
	
	HideItemTooltip();
	Super::NativeDestruct();
}

void UShopWidgetBase::BindToShop(UShopComponent* InShopComponent, UInventoryComponent* InCustomerInventory)
{
	// Unbind from previous shop
	UnbindFromShop();

	BoundShop = InShopComponent;
	CustomerInventory = InCustomerInventory;

	if (BoundShop)
	{
		// Bind to shop events
		BoundShop->OnShopInventoryChanged.AddDynamic(this, &UShopWidgetBase::HandleShopInventoryChanged);
		BoundShop->OnItemPurchased.AddDynamic(this, &UShopWidgetBase::HandleItemPurchased);
		BoundShop->OnItemSold.AddDynamic(this, &UShopWidgetBase::HandleItemSold);

		// Initial update
		HandleShopInventoryChanged();
	}

	if (CustomerInventory)
	{
		// Bind to customer inventory events
		CustomerInventory->OnInventoryChanged.AddDynamic(this, &UShopWidgetBase::HandleCustomerInventoryChanged);

		// Initial update
		HandleCustomerInventoryChanged();
	}

	UpdateWidgets();
}

void UShopWidgetBase::UnbindFromShop()
{
	if (BoundShop)
	{
		// Unbind from shop events
		BoundShop->OnShopInventoryChanged.RemoveDynamic(this, &UShopWidgetBase::HandleShopInventoryChanged);
		BoundShop->OnItemPurchased.RemoveDynamic(this, &UShopWidgetBase::HandleItemPurchased);
		BoundShop->OnItemSold.RemoveDynamic(this, &UShopWidgetBase::HandleItemSold);

		BoundShop = nullptr;
	}

	if (CustomerInventory)
	{
		// Unbind from customer inventory events
		CustomerInventory->OnInventoryChanged.RemoveDynamic(this, &UShopWidgetBase::HandleCustomerInventoryChanged);

		CustomerInventory = nullptr;
	}
}

void UShopWidgetBase::UpdateWidgets()
{
	UpdateShopInfo();
	UpdateCurrencyDisplay();
}

void UShopWidgetBase::UpdateShopInfo()
{
	if (!BoundShop || !BoundShop->ShopData)
	{
		// Clear shop info
		if (ShopNameText)
		{
			ShopNameText->SetText(FText::GetEmpty());
		}
		if (ShopKeeperText)
		{
			ShopKeeperText->SetText(FText::GetEmpty());
		}
		return;
	}

	// Update shop name
	if (ShopNameText)
	{
		ShopNameText->SetText(BoundShop->ShopData->ShopName);
	}

	// Update shop description (used as keeper text)
	if (ShopKeeperText)
	{
		ShopKeeperText->SetText(BoundShop->ShopData->ShopDescription);
	}
}

void UShopWidgetBase::UpdateCurrencyDisplay()
{
	if (!PlayerGoldText)
	{
		return;
	}

	int32 Gold = GetCustomerCurrency();

	FText GoldText = FText::Format(
		FText::FromString("Gold: {0}"),
		FText::AsNumber(Gold)
	);
	PlayerGoldText->SetText(GoldText);

	// Color based on amount
	FLinearColor TextColor = FLinearColor::White;
	if (Gold < 100)
	{
		TextColor = FLinearColor::Red;
	}
	else if (Gold < 500)
	{
		TextColor = FLinearColor::Yellow;
	}
	else
	{
		TextColor = FLinearColor::Green;
	}
	PlayerGoldText->SetColorAndOpacity(FSlateColor(TextColor));
}

bool UShopWidgetBase::PurchaseItem(UItemDataAsset* Item, int32 Quantity)
{
	if (!BoundShop || !CustomerInventory || !Item)
	{
		return false;
	}

	AActor* Customer = CustomerInventory->GetOwner();
	if (!Customer)
	{
		return false;
	}

	EInventoryOperationResult Result;
	bool bSuccess = BoundShop->PurchaseItem(Item, Quantity, Customer, Result);

	if (!bSuccess)
	{
		OnPurchaseFailed(Item, Result);
	}

	return bSuccess;
}

bool UShopWidgetBase::SellItem(UItemDataAsset* Item, int32 Quantity)
{
	if (!BoundShop || !CustomerInventory || !Item)
	{
		return false;
	}

	AActor* Customer = CustomerInventory->GetOwner();
	if (!Customer)
	{
		return false;
	}

	EInventoryOperationResult Result;
	bool bSuccess = BoundShop->SellItem(Item, Quantity, Customer, Result);

	if (!bSuccess)
	{
		OnSaleFailed(Item, Result);
	}

	return bSuccess;
}

int32 UShopWidgetBase::GetPurchasePrice(UItemDataAsset* Item, int32 Quantity) const
{
	if (!BoundShop || !Item)
	{
		return 0;
	}

	AActor* Customer = CustomerInventory ? CustomerInventory->GetOwner() : nullptr;
	return BoundShop->GetPurchasePrice(Item, Quantity, Customer);
}

int32 UShopWidgetBase::GetSellbackPrice(UItemDataAsset* Item, int32 Quantity) const
{
	if (!BoundShop || !Item)
	{
		return 0;
	}

	AActor* Customer = CustomerInventory ? CustomerInventory->GetOwner() : nullptr;
	return BoundShop->GetSellbackPrice(Item, Quantity, Customer);
}

bool UShopWidgetBase::CanAffordItem(UItemDataAsset* Item, int32 Quantity) const
{
	if (!BoundShop || !CustomerInventory || !Item)
	{
		return false;
	}

	int32 Price = GetPurchasePrice(Item, Quantity);
	int32 Currency = GetCustomerCurrency();

	return Currency >= Price;
}

int32 UShopWidgetBase::GetCustomerCurrency() const
{
	if (!BoundShop || !BoundShop->ShopData || !CustomerInventory)
	{
		return 0;
	}

	UItemDataAsset* CurrencyItem = BoundShop->ShopData->CurrencyItem;
	if (!CurrencyItem)
	{
		return 0;
	}

	return CustomerInventory->GetItemQuantity(CurrencyItem);
}

void UShopWidgetBase::HandleShopInventoryChanged()
{
	UpdateWidgets();
	OnShopInventoryUpdated();
}

void UShopWidgetBase::HandleCustomerInventoryChanged()
{
	UpdateCurrencyDisplay();
	OnCustomerInventoryUpdated();
}

void UShopWidgetBase::HandleItemPurchased(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice, AActor* Buyer)
{
	// Only notify if this is our customer
	if (CustomerInventory && Buyer == CustomerInventory->GetOwner())
	{
		OnItemPurchased(Item, Quantity, TotalPrice);
		UpdateCurrencyDisplay();
	}
}

void UShopWidgetBase::HandleItemSold(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice, AActor* Seller)
{
	// Only notify if this is our customer
	if (CustomerInventory && Seller == CustomerInventory->GetOwner())
	{
		OnItemSold(Item, Quantity, TotalPrice);
		UpdateCurrencyDisplay();
	}
}

void UShopWidgetBase::HandleCloseButtonClicked()
{
	// Remove widget from viewport
	RemoveFromParent();
}

void UShopWidgetBase::ShowItemTooltip(UItemDataAsset* ItemData, int32 Quantity, FVector2D ScreenPosition)
{
	if (!TooltipWidgetClass || !ItemData)
	{
		return;
	}

	// Hide existing tooltip
	HideItemTooltip();

	// Create tooltip widget
	UItemTooltipWidget* TooltipWidget = CreateWidget<UItemTooltipWidget>(GetOwningPlayer(), TooltipWidgetClass);
	if (!TooltipWidget)
	{
		return;
	}

	// Set item data
	TooltipWidget->SetItemData(ItemData, Quantity);

	// Add to viewport at cursor position
	TooltipWidget->AddToViewport(100); // High Z-order to be on top

	// Position tooltip
	FVector2D ViewportPosition = ScreenPosition + TooltipOffset;
	TooltipWidget->SetPositionInViewport(ViewportPosition, false);

	// Store reference
	CurrentTooltip = TooltipWidget;
}

void UShopWidgetBase::HideItemTooltip()
{
	if (CurrentTooltip)
	{
		CurrentTooltip.Get()->RemoveFromParent();
		CurrentTooltip = nullptr;
	}
}

void UShopWidgetBase::ShowShop()
{
	// Only add to viewport if not already showing
	if (!IsInViewport())
	{
		AddToViewport(20); // Z-order 20 (higher than inventory)
	}

	// ALWAYS update when showing (even if already in viewport)
	UpdateWidgets();

	// Set input mode to UI with mouse cursor
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UShopWidgetBase::HideShop()
{
	if (IsInViewport())
	{
		// Hide tooltip first
		HideItemTooltip();

		// Unbind from shop (cleanup)
		UnbindFromShop();

		// Remove from viewport (this will destroy the widget if no other references)
		RemoveFromParent();

		// Restore game input mode
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}
	}
}
