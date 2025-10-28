// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemTooltipWidget.generated.h"

class UItemDataAsset;
class UInventoryItem;
class UTextBlock;
class UImage;
class UBorder;

/**
 * Base class for item tooltip widgets
 * Displays item information when hovering over items in UI
 * 
 * REQUIRED WIDGETS (BindWidget - compilation error if missing):
 * - ItemNameText (Text Block) - Item name ?? REQUIRED
 * - ItemIconImage (Image) - Item icon ?? REQUIRED
 * 
 * RECOMMENDED WIDGETS (BindWidgetOptional):
 * - ItemDescriptionText (Text Block) - Item description
 * - ItemValueText (Text Block) - Item value
 * - ItemRarityText (Text Block) - Rarity name
 * - BackgroundBorder (Border) - Background with rarity color
 * 
 * OPTIONAL WIDGETS:
 * - ItemWeightText (Text Block) - Item weight
 * - ItemCategoryText (Text Block) - Category name
 * - QuantityText (Text Block) - Stack quantity
 * - ItemTypeText (Text Block) - Stackable/Usable/Equippable info
 * - QuestItemIcon (Image) - Quest item indicator
 */
UCLASS(Abstract, Blueprintable)
class INVENTORYSYSTEMUI_API UItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ========== SETUP ==========

	/**
	 * Set the item data to display in tooltip
	 * @param InItemData The item data asset
	 * @param InQuantity The quantity of items (for stacks)
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Tooltip")
	virtual void SetItemData(UItemDataAsset* InItemData, int32 InQuantity = 1);

	/**
	 * Set the item instance to display in tooltip
	 * @param InItem The inventory item instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Tooltip")
	virtual void SetInventoryItem(UInventoryItem* InItem);

	/**
	 * Clear the tooltip
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Tooltip")
	virtual void ClearTooltip();

	// ========== GETTERS ==========

	/**
	 * Get the current item data
	 * @return The item data asset
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	UItemDataAsset* GetItemData() const { return ItemData; }

	/**
	 * Get the current inventory item instance
	 * @return The inventory item
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	UInventoryItem* GetInventoryItem() const { return InventoryItem; }

	/**
	 * Get the item name
	 * @return Item display name
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	FText GetItemName() const;

	/**
	 * Get the item description
	 * @return Item description
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	FText GetItemDescription() const;

	/**
	 * Get the item icon
	 * @return Item icon texture
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	class UTexture2D* GetItemIcon() const;

	/**
	 * Get the item rarity
	 * @return Item rarity level
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	EItemRarity GetItemRarity() const;

	/**
	 * Get the item category
	 * @return Item category
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	EItemCategory GetItemCategory() const;

	/**
	 * Get the item quantity
	 * @return Number of items in stack
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	int32 GetQuantity() const { return Quantity; }

	/**
	 * Get the item value
	 * @return Base value of the item
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	int32 GetItemValue() const;

	/**
	 * Check if item is stackable
	 * @return True if item can stack
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	bool IsStackable() const;

	/**
	 * Check if item is usable
	 * @return True if item can be used
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	bool IsUsable() const;

	/**
	 * Check if item is equippable
	 * @return True if item can be equipped
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	bool IsEquippable() const;

	/**
	 * Check if item is a quest item
	 * @return True if this is a quest item
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	bool IsQuestItem() const;

	/**
	 * Get item weight (if weight system is used)
	 * @return Item weight value
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	float GetItemWeight() const;

	/**
	 * Get rarity color for UI display
	 * @return Color based on rarity
	 */
	UFUNCTION(BlueprintPure, Category = "Item Tooltip")
	FLinearColor GetRarityColor() const;
	
	/**
	 * Update tooltip position to follow cursor
	 * Called by parent inventory widget on mouse move
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Tooltip")
	void UpdateTooltipPosition();

	// ========== EVENTS (Blueprint Implementable) ==========

	/**
	 * Called when tooltip data is updated
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item Tooltip|Events")
	void OnTooltipUpdated();

	/**
	 * Called when tooltip is cleared
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Item Tooltip|Events")
	void OnTooltipCleared();

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;

	/** Update all bound widgets with current data */
	virtual void UpdateWidgets();
	
	// ========== REQUIRED WIDGETS (BindWidget - must exist!) ==========

	/** Item name text - REQUIRED */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameText;

	/** Item icon image - REQUIRED */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> ItemIconImage;

	// ========== RECOMMENDED WIDGETS (BindWidgetOptional) ==========

	/** Item description text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemDescriptionText;

	/** Item value text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemValueText;

	/** Item rarity text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemRarityText;

	/** Background border (for rarity color) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UBorder> BackgroundBorder;

	// ========== OPTIONAL WIDGETS ==========

	/** Item weight text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemWeightText;

	/** Item category text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemCategoryText;

	/** Stack quantity text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

	/** Item type info text (optional) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemTypeText;

	/** Quest item indicator (optional) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> QuestItemIcon;

	// ========== DATA ==========

	/** The item data being displayed */
	UPROPERTY(BlueprintReadOnly, Category = "Item Tooltip")
	TObjectPtr<UItemDataAsset> ItemData;

	/** The inventory item instance (optional, for instance-specific data) */
	UPROPERTY(BlueprintReadOnly, Category = "Item Tooltip")
	TObjectPtr<UInventoryItem> InventoryItem;

	/** Current quantity being displayed */
	UPROPERTY(BlueprintReadOnly, Category = "Item Tooltip")
	int32 Quantity = 1;

	/** Get rarity color based on rarity enum */
	FLinearColor GetColorForRarity(EItemRarity Rarity) const;
};
