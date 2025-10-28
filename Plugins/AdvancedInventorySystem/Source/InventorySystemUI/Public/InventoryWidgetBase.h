// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidgetBase.generated.h"

class UInventoryComponent;
class UInventoryItem;
class UTextBlock;
class UUniformGridPanel;
class UButton;
class UProgressBar;
class UItemTooltipWidget;
class UItemDataAsset;
class UInventorySlotWidget;
class UInventoryContextMenuWidget;

/**
 * Base class for inventory UI widgets
 * Provides API for inventory visualization without enforcing specific layout
 * 
 * REQUIRED WIDGETS (BindWidget - compilation error if missing):
 * - InventoryGrid (Uniform Grid Panel) - Main inventory grid ?? REQUIRED
 * 
 * RECOMMENDED WIDGETS (BindWidgetOptional):
 * - SlotsText (Text Block) - Slots count (e.g. "5 / 20")
 * - WeightText (Text Block) - Current weight display
 * - MaxWeightText (Text Block) - Max weight capacity
 * 
 * OPTIONAL WIDGETS:
 * - WeightBar (Progress Bar) - Weight progress bar
 * - SortButton (Button) - Sort inventory button
 * - TitleText (Text Block) - "Inventory" title
 * 
 * TOOLTIP SETUP:
 * Set TooltipWidgetClass to your tooltip Blueprint (e.g. WBP_ItemTooltip)
 * Tooltip will be shown automatically when hovering over items
 */
UCLASS(Abstract, Blueprintable)
class INVENTORYSYSTEMUI_API UInventoryWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	// ========== BINDING ==========

	/**
	 * Bind to an inventory component
	 * @param InInventoryComponent The inventory component to bind to
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void BindToInventory(UInventoryComponent* InInventoryComponent);

	/**
	 * Unbind from current inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void UnbindFromInventory();

	/**
	 * Get the currently bound inventory component
	 * @return The bound inventory component
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory UI")
	UInventoryComponent* GetBoundInventory() const { return BoundInventory; }

	// ========== UI MANAGEMENT ==========

	/**
	 * Show the inventory UI (adds to viewport, sets input mode)
	 * Call this instead of manually adding to viewport
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void ShowInventory();

	/**
	 * Hide the inventory UI (removes from viewport, restores game input)
	 * Call this to close inventory properly
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void HideInventory();

	/**
	 * Toggle inventory visibility
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void ToggleInventory();

	/**
	 * Check if inventory is currently visible
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory UI")
	bool IsInventoryVisible() const;

	// ========== TOOLTIP ==========

	/**
	 * Show tooltip for an item
	 * @param ItemData The item to show tooltip for
	 * @param Quantity Stack quantity
	 * @param ScreenPosition Position to show tooltip
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void ShowItemTooltip(UItemDataAsset* ItemData, int32 Quantity, FVector2D ScreenPosition);

	/**
	 * Hide the tooltip
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void HideItemTooltip();

	// ========== CONTEXT MENU ==========

	/**
	 * Show context menu for an item slot
	 * @param ItemData The item to show menu for
	 * @param InventoryItem The inventory item instance
	 * @param ScreenPosition Position to show menu
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void ShowContextMenu(UItemDataAsset* ItemData, UInventoryItem* InventoryItem, FVector2D ScreenPosition);

	/**
	 * Hide the context menu
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void HideContextMenu();

	/**
	 * Get the currently active context menu
	 */
	UFUNCTION(BlueprintPure, Category = "Inventory UI")
	UInventoryContextMenuWidget* GetContextMenu() const { return CurrentContextMenu; }

	// ========== CONFIGURATION ==========

	/** Tooltip widget class to use (set this to your WBP_ItemTooltip) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory UI|Tooltip")
	TSubclassOf<UItemTooltipWidget> TooltipWidgetClass;

	/** Offset from mouse cursor when showing tooltip */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory UI|Tooltip")
	FVector2D TooltipOffset = FVector2D(10.0f, 10.0f);

	/** Slot widget class to use for inventory slots (set this to your WBP_InventorySlot) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory UI|Slots")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	/** Number of columns in inventory grid */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory UI|Slots", meta = (ClampMin = "1"))
	int32 GridColumns = 5;

	/** Size of each slot in the grid (width and height) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory UI|Slots", meta = (ClampMin = "32", ClampMax = "256"))
	float SlotSize = 80.0f;

	/** Context menu widget class for item slots (set this to your WBP_ContextMenu) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory UI|Context Menu")
	TSubclassOf<UInventoryContextMenuWidget> ContextMenuClass;

	// ========== EVENTS (Blueprint Implementable) ==========

	/**
	 * Called when inventory data changes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI|Events")
	void OnInventoryUpdated();

	/**
	 * Called when an item is added to inventory
	 * @param Item The item that was added
	 * @param Quantity Number of items added
	 * @param SlotIndex The slot index
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI|Events")
	void OnItemAddedToInventory(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);

	/**
	 * Called when an item is removed from inventory
	 * @param Item The item that was removed
	 * @param Quantity Number of items removed
	 * @param SlotIndex The slot index
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI|Events")
	void OnItemRemovedFromInventory(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);

	/**
	 * Called when an item is used
	 * @param Item The item that was used
	 * @param User The actor who used it
	 * @param bSuccess Whether the use was successful
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI|Events")
	void OnItemUsedFromInventory(UInventoryItem* Item, AActor* User, bool bSuccess);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	/** Handle mouse button down to close context menu if clicked outside */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/** Handle mouse move to update tooltip position */
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/** Update all bound widgets with current data */
	virtual void UpdateWidgets();

	/** Update weight display */
	virtual void UpdateWeightDisplay();

	/** Update slots display */
	virtual void UpdateSlotsDisplay();

	/** Refresh inventory grid with current items */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	virtual void RefreshInventorySlots();

	// ========== REQUIRED WIDGETS (BindWidget - must exist!) ==========

	/** Main inventory grid panel - REQUIRED */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGrid;

	// ========== RECOMMENDED WIDGETS (BindWidgetOptional) ==========

	/** Slots count text (e.g. "5 / 20") */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SlotsText;

	/** Current weight text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WeightText;

	/** Max weight text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MaxWeightText;

	// ========== OPTIONAL WIDGETS ==========

	/** Weight progress bar */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> WeightBar;

	/** Sort button */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SortButton;

	/** Title text */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;

	/** Close button (auto-binds to HideInventory if present) */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

	// ========== DATA ==========

	/** The bound inventory component */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory UI")
	TObjectPtr<UInventoryComponent> BoundInventory;

	/** Currently displayed tooltip widget */
	UPROPERTY()
	TObjectPtr<UItemTooltipWidget> CurrentTooltip;

	/** Currently displayed context menu widget */
	UPROPERTY()
	TObjectPtr<UInventoryContextMenuWidget> CurrentContextMenu;

	/** Created slot widgets */
	UPROPERTY()
	TArray<TObjectPtr<UInventorySlotWidget>> SlotWidgets;

	// ========== DELEGATE CALLBACKS ==========

	UFUNCTION()
	void HandleInventoryChanged();

	UFUNCTION()
	void HandleItemAdded(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);

	UFUNCTION()
	void HandleItemRemoved(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);

	UFUNCTION()
	void HandleItemUsed(UInventoryItem* Item, AActor* User, bool bSuccess);

	UFUNCTION()
	void HandleWeightChanged(float CurrentWeight, float MaxWeight);

	UFUNCTION()
	void HandleSortButtonClicked();
};
