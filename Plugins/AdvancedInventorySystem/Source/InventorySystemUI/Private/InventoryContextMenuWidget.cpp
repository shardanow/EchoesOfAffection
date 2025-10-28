// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryContextMenuWidget.h"
#include "ItemDataAsset.h"
#include "InventoryItem.h"
#include "InventoryComponent.h"
#include "InventorySystemTypes.h"
#include "WorldItemActor.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UInventoryContextMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Auto-bind buttons if present (only if not already bound)
	if (UseButton && !UseButton->OnClicked.IsBound())
	{
		UseButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::HandleUseButtonClicked);
	}

	if (DropButton && !DropButton->OnClicked.IsBound())
	{
		DropButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::HandleDropButtonClicked);
	}

	if (SplitButton && !SplitButton->OnClicked.IsBound())
	{
		SplitButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::HandleSplitButtonClicked);
	}

	if (InfoButton && !InfoButton->OnClicked.IsBound())
	{
		InfoButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::HandleInfoButtonClicked);
	}

	if (CancelButton && !CancelButton->OnClicked.IsBound())
	{
		CancelButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::HandleCancelButtonClicked);
	}
}

void UInventoryContextMenuWidget::ShowMenu(UItemDataAsset* InItemData, UInventoryItem* InInventoryItem, UInventoryComponent* InInventoryComponent, FVector2D ScreenPosition)
{
	TargetItemData = InItemData;
	TargetInventoryItem = InInventoryItem;
	TargetInventoryComponent = InInventoryComponent;

	if (!IsInViewport())
	{
		AddToViewport(999); // Very high Z-order
	}

	// IMPROVED: Use current mouse position instead of passed screen position
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		// Get mouse position in viewport space
		FVector2D MousePosition;
		PC->GetMousePosition(MousePosition.X, MousePosition.Y);
		
		// Set position directly - mouse position is already in viewport space
		SetPositionInViewport(MousePosition, false);
		
		UE_LOG(LogTemp, Log, TEXT("Context Menu: Mouse at (%.1f, %.1f)"), MousePosition.X, MousePosition.Y);
	}
	else
	{
		// Fallback - use provided screen position with DPI adjustment
		float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
		FVector2D AdjustedPosition = ScreenPosition / DPIScale;
		SetPositionInViewport(AdjustedPosition, false);
	}

	// Update button visibility based on item properties
	UpdateButtonVisibility();
}

void UInventoryContextMenuWidget::HideMenu()
{
	if (IsInViewport())
	{
		RemoveFromParent();
	}

	// Clear references
	TargetItemData = nullptr;
	TargetInventoryItem = nullptr;
	TargetInventoryComponent = nullptr;
}

FReply UInventoryContextMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// If left mouse button clicked, this is our widget - don't close
	// Parent will handle closing if clicked outside
	return FReply::Handled();
}

void UInventoryContextMenuWidget::UpdateButtonVisibility()
{
	if (!TargetItemData)
	{
		return;
	}

	// Use button - only for usable items
	if (UseButton)
	{
		bool bShouldShow = bShowUseButton && TargetItemData->bIsUsable;
		UseButton->SetVisibility(bShouldShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Drop button - always available if enabled
	if (DropButton)
	{
		DropButton->SetVisibility(bShowDropButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Split button - only for stackable items with quantity > 1
	if (SplitButton)
	{
		bool bCanSplit = bShowSplitButton && TargetItemData->bIsStackable;
		if (TargetInventoryItem)
		{
			bCanSplit = bCanSplit && TargetInventoryItem->GetQuantity() > 1;
		}
		SplitButton->SetVisibility(bCanSplit ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Info button
	if (InfoButton)
	{
		InfoButton->SetVisibility(bShowInfoButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Cancel button
	if (CancelButton)
	{
		CancelButton->SetVisibility(bShowCancelButton ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UInventoryContextMenuWidget::HandleUseButtonClicked()
{
	OnUseClicked();
	OnActionCompleted();
	HideMenu();
}

void UInventoryContextMenuWidget::HandleDropButtonClicked()
{
	OnDropClicked();
	OnActionCompleted();
	HideMenu();
}

void UInventoryContextMenuWidget::HandleSplitButtonClicked()
{
	OnSplitClicked();
	OnActionCompleted();
	HideMenu();
}

void UInventoryContextMenuWidget::HandleInfoButtonClicked()
{
	OnInfoClicked();
	// Don't close menu for info
}

void UInventoryContextMenuWidget::HandleCancelButtonClicked()
{
	HideMenu();
}

void UInventoryContextMenuWidget::OnUseClicked_Implementation()
{
	if (!TargetInventoryComponent || !TargetInventoryItem)
	{
		return;
	}

	// Use the item
	AActor* User = TargetInventoryComponent->GetOwner();
	if (User)
	{
		EInventoryOperationResult Result;
		bool bSuccess = TargetInventoryComponent->UseItem(TargetInventoryItem->ItemInstanceId, User, Result);
		UE_LOG(LogTemp, Log, TEXT("Context Menu: Used item %s, Success: %s"), 
			TargetItemData ? *TargetItemData->ItemName.ToString() : TEXT("NULL"), 
			bSuccess ? TEXT("true") : TEXT("false"));
	}
}

void UInventoryContextMenuWidget::OnDropClicked_Implementation()
{
	if (!TargetInventoryComponent || !TargetInventoryItem)
	{
		return;
	}

	// Drop the item
	int32 QuantityToDrop = DefaultDropQuantity;
	QuantityToDrop = FMath::Min(QuantityToDrop, TargetInventoryItem->GetQuantity());

	// Get drop location (in front of player)
	FVector DropLocation = FVector::ZeroVector;
	if (AActor* Owner = TargetInventoryComponent->GetOwner())
	{
		DropLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.0f;
	}

	EInventoryOperationResult Result;
	AWorldItemActor* DroppedActor = TargetInventoryComponent->DropItem(TargetInventoryItem->ItemInstanceId, QuantityToDrop, DropLocation, Result);
	
	bool bSuccess = (DroppedActor != nullptr);
	UE_LOG(LogTemp, Log, TEXT("Context Menu: Dropped %d of %s, Success: %s"), 
		QuantityToDrop,
		TargetItemData ? *TargetItemData->ItemName.ToString() : TEXT("NULL"), 
		bSuccess ? TEXT("true") : TEXT("false"));
}

void UInventoryContextMenuWidget::OnSplitClicked_Implementation()
{
	// Split stack - default implementation just logs
	// Override in Blueprint to show quantity input dialog
	UE_LOG(LogTemp, Log, TEXT("Context Menu: Split stack requested for %s"), 
		TargetItemData ? *TargetItemData->ItemName.ToString() : TEXT("NULL"));
}

void UInventoryContextMenuWidget::OnInfoClicked_Implementation()
{
	// Show item info - default implementation just logs
	// Override in Blueprint to show detailed item panel
	UE_LOG(LogTemp, Log, TEXT("Context Menu: Info requested for %s"), 
		TargetItemData ? *TargetItemData->ItemName.ToString() : TEXT("NULL"));
}
