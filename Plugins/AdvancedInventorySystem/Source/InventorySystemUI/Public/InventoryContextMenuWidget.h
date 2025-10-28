// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryContextMenuWidget.generated.h"

class UItemDataAsset;
class UInventoryItem;
class UInventoryComponent;
class UButton;
class UTextBlock;

/**
 * Base class for inventory item context menu
 * Shows when right-clicking on an item slot
 * 
 * REQUIRED WIDGETS (BindWidget):
 * None - all buttons are optional!
 * 
 * OPTIONAL WIDGETS (BindWidgetOptional):
 * - UseButton (Button) - Use/Consume item
 * - DropButton (Button) - Drop item
 * - SplitButton (Button) - Split stack
 * - InfoButton (Button) - Show item details
 * - CancelButton (Button) - Close menu
 * 
 * All buttons auto-bind and auto-execute actions!
 * Simply add the buttons you want to your Blueprint.
 */
UCLASS(Abstract, Blueprintable)
class INVENTORYSYSTEMUI_API UInventoryContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ========== SETUP ==========

	/**
	 * Show context menu for an item
	 * @param InItemData Item to show menu for
	 * @param InInventoryItem The inventory item instance
	 * @param InInventoryComponent The inventory component
	 * @param ScreenPosition Position to show menu
	 */
	UFUNCTION(BlueprintCallable, Category = "Context Menu")
	void ShowMenu(UItemDataAsset* InItemData, UInventoryItem* InInventoryItem, UInventoryComponent* InInventoryComponent, FVector2D ScreenPosition);

	/**
	 * Hide the context menu
	 */
	UFUNCTION(BlueprintCallable, Category = "Context Menu")
	void HideMenu();

protected:
	virtual void NativeConstruct() override;
	
	/** Handle click outside menu to close it */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/** Update button visibility based on item properties */
	virtual void UpdateButtonVisibility();

	// ========== CONFIGURATION ==========

	/** Show Use button for usable items */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Menu|Options")
	bool bShowUseButton = true;

	/** Show Drop button */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Menu|Options")
	bool bShowDropButton = true;

	/** Show Split button for stackable items */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Menu|Options")
	bool bShowSplitButton = true;

	/** Show Info button */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Menu|Options")
	bool bShowInfoButton = false;

	/** Show Cancel button */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Menu|Options")
	bool bShowCancelButton = true;

	/** Default quantity to drop */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Menu|Options", meta = (ClampMin = "1"))
	int32 DefaultDropQuantity = 1;

	// ========== EVENTS (Blueprint Implementable) ==========

	/**
	 * Called when Use button is clicked
	 * Override to customize behavior
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Context Menu|Events")
	void OnUseClicked();

	/**
	 * Called when Drop button is clicked
	 * Override to customize behavior
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Context Menu|Events")
	void OnDropClicked();

	/**
	 * Called when Split button is clicked
	 * Override to customize behavior
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Context Menu|Events")
	void OnSplitClicked();

	/**
	 * Called when Info button is clicked
	 * Override to customize behavior
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Context Menu|Events")
	void OnInfoClicked();

	/**
	 * Called when any action completes
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Context Menu|Events")
	void OnActionCompleted();

protected:
	// Default implementations
	virtual void OnUseClicked_Implementation();
	virtual void OnDropClicked_Implementation();
	virtual void OnSplitClicked_Implementation();
	virtual void OnInfoClicked_Implementation();

	// Button handlers
	UFUNCTION()
	void HandleUseButtonClicked();

	UFUNCTION()
	void HandleDropButtonClicked();

	UFUNCTION()
	void HandleSplitButtonClicked();

	UFUNCTION()
	void HandleInfoButtonClicked();

	UFUNCTION()
	void HandleCancelButtonClicked();

	// ========== OPTIONAL WIDGETS (BindWidgetOptional) ==========

	/** Use item button */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> UseButton;

	/** Drop item button */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> DropButton;

	/** Split stack button */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SplitButton;

	/** Show info button */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> InfoButton;

	/** Cancel/Close button */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> CancelButton;

	// ========== DATA ==========

	/** Target item data */
	UPROPERTY(BlueprintReadOnly, Category = "Context Menu")
	TObjectPtr<UItemDataAsset> TargetItemData;

	/** Target inventory item instance */
	UPROPERTY(BlueprintReadOnly, Category = "Context Menu")
	TObjectPtr<UInventoryItem> TargetInventoryItem;

	/** Target inventory component */
	UPROPERTY(BlueprintReadOnly, Category = "Context Menu")
	TObjectPtr<UInventoryComponent> TargetInventoryComponent;
};
