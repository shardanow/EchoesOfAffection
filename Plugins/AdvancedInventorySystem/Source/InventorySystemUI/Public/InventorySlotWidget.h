// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UItemDataAsset;
class UInventoryItem;
class UImage;
class UTextBlock;
class UBorder;
class UInventoryContextMenuWidget;

/**
 * Base class for inventory slot widgets
 * Represents a single slot in the inventory grid
 * 
 * REQUIRED WIDGETS (BindWidget):
 * - ItemIcon (Image) - Item icon display ?? REQUIRED
 * 
 * RECOMMENDED WIDGETS (BindWidgetOptional):
 * - QuantityText (Text Block) - Stack quantity
 * - SlotBackground (Border) - Slot background/highlight
 * 
 * OPTIONAL WIDGETS:
 * - RarityBorder (Border) - Rarity color indicator
 * - EquippedIcon (Image) - Equipped indicator
 * - LockedIcon (Image) - Locked slot indicator
 * 
 * USAGE:
 * This widget is created dynamically by InventoryWidgetBase
 * You can override in Blueprint to customize appearance and behavior
 */
UCLASS(Abstract, Blueprintable)
class INVENTORYSYSTEMUI_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ========== SETUP ==========

	/**
	 * Initialize slot with item data
	 * @param InItemData The item data to display
	 * @param InQuantity Stack quantity
	 * @param InSlotIndex Slot index in inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	virtual void SetSlotData(UItemDataAsset* InItemData, int32 InQuantity, int32 InSlotIndex);

	/**
	 * Initialize slot with inventory item instance
	 * @param InItem The inventory item instance
	 * @param InSlotIndex Slot index in inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	virtual void SetInventoryItem(UInventoryItem* InItem, int32 InSlotIndex);

	/**
	 * Clear the slot (empty slot)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	virtual void ClearSlot();

	// ========== GETTERS ==========

	/**
	 * Get the item data in this slot
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
	UItemDataAsset* GetItemData() const { return ItemData; }

	/**
	 * Get the inventory item instance
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
	UInventoryItem* GetInventoryItem() const { return InventoryItem; }

	/**
	 * Get the quantity in this slot
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
	int32 GetQuantity() const { return Quantity; }

	/**
	 * Get the slot index
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
	int32 GetSlotIndex() const { return SlotIndex; }

	/**
	 * Check if slot is empty
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory Slot")
	bool IsEmpty() const { return ItemData == nullptr; }

	// ========== CONTEXT MENU ==========

	/** Context menu widget class (auto-shows on right-click) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Slot|Context Menu")
	TSubclassOf<UInventoryContextMenuWidget> ContextMenuClass;

	// ========== EVENTS (Blueprint Implementable) ==========

	/**
	 * Called when slot is clicked
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Slot|Events")
	void OnSlotClicked();

	/**
	 * Called when mouse enters slot (for tooltip)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Slot|Events")
	void OnSlotHovered();

	/**
	 * Called when mouse leaves slot
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Slot|Events")
	void OnSlotUnhovered();

	/**
	 * Called when slot data is updated
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Slot|Events")
	void OnSlotDataUpdated();

	/**
	 * Called when slot is cleared
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Slot|Events")
	void OnSlotCleared();

	/**
	 * Called when right mouse button is clicked on slot (for context menu)
	 * Return true to show context menu, false to ignore
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory Slot|Events")
	bool OnSlotRightClicked(const FVector2D& ScreenPosition);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/** Default implementation - can be overridden in Blueprint */
	virtual bool OnSlotRightClicked_Implementation(const FVector2D& ScreenPosition);

	/** Update all bound widgets with current data */
	virtual void UpdateWidgets();

	// ========== REQUIRED WIDGETS (BindWidget) ==========

	/** Item icon image - REQUIRED */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	// ========== RECOMMENDED WIDGETS (BindWidgetOptional) ==========

	/** Stack quantity text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

	/** Slot background/highlight */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UBorder> SlotBackground;

	// ========== OPTIONAL WIDGETS ==========

	/** Rarity color border */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UBorder> RarityBorder;

	/** Equipped indicator icon */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> EquippedIcon;

	/** Locked slot indicator */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> LockedIcon;

	// ========== DATA ==========

	/** The item data being displayed */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Slot")
	TObjectPtr<UItemDataAsset> ItemData;

	/** The inventory item instance */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Slot")
	TObjectPtr<UInventoryItem> InventoryItem;

	/** Stack quantity */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Slot")
	int32 Quantity = 0;

	/** Slot index in inventory */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Slot")
	int32 SlotIndex = -1;
};
