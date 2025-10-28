// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySystemTypes.h"
#include "ShopWidgetBase.generated.h"

class UShopComponent;
class UItemDataAsset;
class UInventoryComponent;
class UTextBlock;
class UScrollBox;
class UButton;
class UItemTooltipWidget;

/**
 * Base class for shop UI widgets
 * Provides API for shop visualization without enforcing specific layout
 * 
 * REQUIRED WIDGETS (BindWidget - compilation error if missing):
 * - ShopItemsList (Scroll Box) - Shop inventory items ?? REQUIRED
 * - PlayerItemsList (Scroll Box) - Player inventory items ?? REQUIRED
 * 
 * RECOMMENDED WIDGETS (BindWidgetOptional):
 * - PlayerGoldText (Text Block) - Player's gold amount
 * - ShopNameText (Text Block) - Shop name display
 * 
 * OPTIONAL WIDGETS:
 * - ShopKeeperText (Text Block) - Shop keeper name
 * - CloseButton (Button) - Close shop button
 * 
 * TOOLTIP SETUP:
 * Set TooltipWidgetClass to your tooltip Blueprint (e.g. WBP_ItemTooltip)
 * Tooltip will be shown automatically when hovering over items
 */
UCLASS(Abstract, Blueprintable)
class INVENTORYSYSTEMUI_API UShopWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	// ========== BINDING ==========

	/**
	 * Bind to a shop component
	 * @param InShopComponent The shop component to bind to
	 * @param InCustomerInventory The customer's inventory component
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	virtual void BindToShop(UShopComponent* InShopComponent, UInventoryComponent* InCustomerInventory);

	/**
	 * Unbind from current shop
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	virtual void UnbindFromShop();

	/**
	 * Get the currently bound shop component
	 * @return The bound shop component
	 */
	UFUNCTION(BlueprintPure, Category = "Shop UI")
	UShopComponent* GetBoundShop() const { return BoundShop; }

	/**
	 * Get the customer's inventory component
	 * @return The customer's inventory component
	 */
	UFUNCTION(BlueprintPure, Category = "Shop UI")
	UInventoryComponent* GetCustomerInventory() const { return CustomerInventory; }

	// ========== UI MANAGEMENT ==========

	/**
	 * Show the shop UI (adds to viewport, sets input mode)
	 * Call this instead of manually adding to viewport
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	virtual void ShowShop();

	/**
	 * Hide the shop UI (removes from viewport, restores game input, destroys widget)
	 * Call this to close shop properly
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	virtual void HideShop();

	// ========== TOOLTIP ==========

	/**
	 * Show tooltip for an item
	 * @param ItemData The item to show tooltip for
	 * @param Quantity Stack quantity
	 * @param ScreenPosition Position to show tooltip
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	virtual void ShowItemTooltip(UItemDataAsset* ItemData, int32 Quantity, FVector2D ScreenPosition);

	/**
	 * Hide the tooltip
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	virtual void HideItemTooltip();

	// ========== CONFIGURATION ==========

	/** Tooltip widget class to use (set this to your WBP_ItemTooltip) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop UI|Tooltip")
	TSubclassOf<UItemTooltipWidget> TooltipWidgetClass;

	/** Offset from mouse cursor when showing tooltip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop UI|Tooltip")
	FVector2D TooltipOffset = FVector2D(10.0f, 10.0f);

	// ========== SHOP ACTIONS ==========

	/**
	 * Attempt to purchase an item
	 * @param Item The item to purchase
	 * @param Quantity Number of items to purchase
	 * @return True if purchase was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	bool PurchaseItem(UItemDataAsset* Item, int32 Quantity);

	/**
	 * Attempt to sell an item
	 * @param Item The item to sell
	 * @param Quantity Number of items to sell
	 * @return True if sale was successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Shop UI")
	bool SellItem(UItemDataAsset* Item, int32 Quantity);

	/**
	 * Get purchase price for an item
	 * @param Item The item to price
	 * @param Quantity Number of items
	 * @return Total purchase price
	 */
	UFUNCTION(BlueprintPure, Category = "Shop UI")
	int32 GetPurchasePrice(UItemDataAsset* Item, int32 Quantity) const;

	/**
	 * Get sellback price for an item
	 * @param Item The item to price
	 * @param Quantity Number of items
	 * @return Total sellback price
	 */
	UFUNCTION(BlueprintPure, Category = "Shop UI")
	int32 GetSellbackPrice(UItemDataAsset* Item, int32 Quantity) const;

	/**
	 * Check if customer can afford an item
	 * @param Item The item to check
	 * @param Quantity Number of items
	 * @return True if customer has enough currency
	 */
	UFUNCTION(BlueprintPure, Category = "Shop UI")
	bool CanAffordItem(UItemDataAsset* Item, int32 Quantity) const;

	/**
	 * Get customer's current currency amount
	 * @return Amount of currency
	 */
	UFUNCTION(BlueprintPure, Category = "Shop UI")
	int32 GetCustomerCurrency() const;

	// ========== EVENTS (Blueprint Implementable) ==========

	/**
	 * Called when shop inventory changes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
	void OnShopInventoryUpdated();

	/**
	 * Called when customer inventory changes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
	void OnCustomerInventoryUpdated();

	/**
	 * Called when an item is purchased
	 * @param Item The item that was purchased
	 * @param Quantity Number of items purchased
	 * @param TotalPrice Total price paid
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
	void OnItemPurchased(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice);

	/**
	 * Called when an item is sold
	 * @param Item The item that was sold
	 * @param Quantity Number of items sold
	 * @param TotalPrice Total price received
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
	void OnItemSold(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice);

	/**
	 * Called when a purchase fails
	 * @param Item The item that failed to purchase
	 * @param Reason Failure reason
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
	void OnPurchaseFailed(UItemDataAsset* Item, EInventoryOperationResult Reason);

	/**
	 * Called when a sale fails
	 * @param Item The item that failed to sell
	 * @param Reason Failure reason
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
	void OnSaleFailed(UItemDataAsset* Item, EInventoryOperationResult Reason);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Update all bound widgets with current data */
	virtual void UpdateWidgets();

	/** Update shop info display */
	virtual void UpdateShopInfo();

	/** Update currency display */
	virtual void UpdateCurrencyDisplay();

	// ========== REQUIRED WIDGETS (BindWidget - must exist!) ==========

	/** Shop items list container - REQUIRED */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> ShopItemsList;

	/** Player items list container - REQUIRED */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> PlayerItemsList;

	// ========== RECOMMENDED WIDGETS (BindWidgetOptional) ==========

	/** Player's gold amount text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PlayerGoldText;

	/** Shop name text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ShopNameText;

	// ========== OPTIONAL WIDGETS ==========

	/** Shop keeper name text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ShopKeeperText;

	/** Close shop button */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

	// ========== DATA ==========

	/** The bound shop component */
	UPROPERTY(BlueprintReadOnly, Category = "Shop UI")
	TObjectPtr<UShopComponent> BoundShop;

	/** The customer's inventory component */
	UPROPERTY(BlueprintReadOnly, Category = "Shop UI")
	TObjectPtr<UInventoryComponent> CustomerInventory;

	/** Currently displayed tooltip widget */
	UPROPERTY()
	TObjectPtr<UItemTooltipWidget> CurrentTooltip;

	// ========== DELEGATE CALLBACKS ==========

	UFUNCTION()
	void HandleShopInventoryChanged();

	UFUNCTION()
	void HandleCustomerInventoryChanged();

	UFUNCTION()
	void HandleItemPurchased(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice, AActor* Buyer);

	UFUNCTION()
	void HandleItemSold(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice, AActor* Seller);

	UFUNCTION()
	void HandleCloseButtonClicked();
};
