// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventorySlotWidget.h"
#include "ItemDataAsset.h"
#include "InventoryItem.h"
#include "InventoryWidgetBase.h"
#include "InventoryComponent.h"
#include "InventoryContextMenuWidget.h"
#include "InventorySystemTypes.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateWidgets();
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	// Highlight slot
	if (SlotBackground)
	{
		SlotBackground->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.3f));
	}

	// Auto-show tooltip if item exists
	if (ItemData)
	{
		// Find parent InventoryWidgetBase by traversing widget tree
		UWidget* CurrentWidget = this;
		while (CurrentWidget)
		{
			UPanelWidget* Panel = CurrentWidget->GetParent();
			if (!Panel)
			{
				break;
			}

			// Check if panel's parent is InventoryWidgetBase
			UInventoryWidgetBase* InventoryWidget = Cast<UInventoryWidgetBase>(Panel->GetTypedOuter<UUserWidget>());
			if (InventoryWidget)
			{
				// Don't show tooltip if context menu is open
				UInventoryContextMenuWidget* ContextMenu = InventoryWidget->GetContextMenu();
				if (ContextMenu && ContextMenu->IsInViewport())
				{
					// Context menu is open, skip tooltip
					break;
				}

				// Get mouse position
				FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
				FVector2D ScreenPosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(GetWorld());

				// Show tooltip
				InventoryWidget->ShowItemTooltip(ItemData, Quantity, ScreenPosition);
				break;
			}

			CurrentWidget = Panel;
		}
	}

	OnSlotHovered();
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	// Remove highlight
	if (SlotBackground)
	{
		SlotBackground->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.1f));
	}

	// Auto-hide tooltip
	UWidget* CurrentWidget = this;
	while (CurrentWidget)
	{
		UPanelWidget* Panel = CurrentWidget->GetParent();
		if (!Panel)
		{
			break;
		}

		UInventoryWidgetBase* InventoryWidget = Cast<UInventoryWidgetBase>(Panel->GetTypedOuter<UUserWidget>());
		if (InventoryWidget)
		{
			InventoryWidget->HideItemTooltip();
			break;
		}

		CurrentWidget = Panel;
	}

	OnSlotUnhovered();
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OnSlotClicked();
		return FReply::Handled();
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		// Get screen position for context menu
		FVector2D ScreenPosition = InMouseEvent.GetScreenSpacePosition();
		
		// Call Blueprint event (can be overridden)
		if (OnSlotRightClicked(ScreenPosition))
		{
			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool UInventorySlotWidget::OnSlotRightClicked_Implementation(const FVector2D& ScreenPosition)
{
	// Auto-show context menu through parent inventory widget
	if (ContextMenuClass && ItemData)
	{
		// Find parent inventory widget to show context menu
		UWidget* CurrentWidget = this;

		while (CurrentWidget)
		{
			UPanelWidget* Panel = CurrentWidget->GetParent();
			if (!Panel)
			{
				break;
			}

			UInventoryWidgetBase* InventoryWidget = Cast<UInventoryWidgetBase>(Panel->GetTypedOuter<UUserWidget>());
			if (InventoryWidget)
			{
				// Use parent widget's context menu management
				InventoryWidget->ShowContextMenu(ItemData, InventoryItem, ScreenPosition);
				return true; // Handled
			}

			CurrentWidget = Panel;
		}

		// Fallback: Direct creation (if no parent inventory widget found)
		// This shouldn't normally happen, but provides backwards compatibility
		UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget: No parent InventoryWidgetBase found, creating context menu directly"));
		
		UInventoryComponent* InventoryComp = nullptr;
		// Try to find inventory component from owner
		if (AActor* Owner = GetOwningPlayerPawn())
		{
			InventoryComp = Owner->FindComponentByClass<UInventoryComponent>();
		}

		if (InventoryComp)
		{
			UInventoryContextMenuWidget* ContextMenu = CreateWidget<UInventoryContextMenuWidget>(GetOwningPlayer(), ContextMenuClass);
			if (ContextMenu)
			{
				ContextMenu->ShowMenu(ItemData, InventoryItem, InventoryComp, ScreenPosition);
				return true;
			}
		}
	}

	// Not handled - Blueprint can override
	return false;
}

void UInventorySlotWidget::SetSlotData(UItemDataAsset* InItemData, int32 InQuantity, int32 InSlotIndex)
{
	ItemData = InItemData;
	InventoryItem = nullptr;
	Quantity = InQuantity;
	SlotIndex = InSlotIndex;

	UpdateWidgets();
	OnSlotDataUpdated();
}

void UInventorySlotWidget::SetInventoryItem(UInventoryItem* InItem, int32 InSlotIndex)
{
	InventoryItem = InItem;
	SlotIndex = InSlotIndex;

	if (InItem && InItem->ItemData)
	{
		ItemData = InItem->ItemData;
		Quantity = InItem->GetQuantity();
	}
	else
	{
		ItemData = nullptr;
		Quantity = 0;
	}

	UpdateWidgets();
	OnSlotDataUpdated();
}

void UInventorySlotWidget::ClearSlot()
{
	ItemData = nullptr;
	InventoryItem = nullptr;
	Quantity = 0;
	SlotIndex = -1;

	UpdateWidgets();
	OnSlotCleared();
}

void UInventorySlotWidget::UpdateWidgets()
{
	if (!ItemData)
	{
		// Empty slot
		if (ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(nullptr);
			ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (QuantityText)
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (RarityBorder)
		{
			RarityBorder->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (EquippedIcon)
		{
			EquippedIcon->SetVisibility(ESlateVisibility::Collapsed);
		}

		return;
	}

	// Update icon
	if (ItemIcon)
	{
		if (ItemData->ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(ItemData->ItemIcon);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// Update quantity
	if (QuantityText)
	{
		if (Quantity > 1)
		{
			FText QuantText = FText::Format(
				FText::FromString("x{0}"),
				FText::AsNumber(Quantity)
			);
			QuantityText->SetText(QuantText);
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Update rarity border color
	if (RarityBorder)
	{
		FLinearColor RarityColor;
		switch (ItemData->Rarity)
		{
		case EItemRarity::Common:
			RarityColor = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);
			break;
		case EItemRarity::Uncommon:
			RarityColor = FLinearColor(0.1f, 1.0f, 0.1f, 1.0f);
			break;
		case EItemRarity::Rare:
			RarityColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);
			break;
		case EItemRarity::Epic:
			RarityColor = FLinearColor(0.64f, 0.21f, 0.93f, 1.0f);
			break;
		case EItemRarity::Legendary:
			RarityColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f);
			break;
		case EItemRarity::Unique:
			RarityColor = FLinearColor(1.0f, 0.84f, 0.0f, 1.0f);
			break;
		default:
			RarityColor = FLinearColor::White;
			break;
		}

		RarityBorder->SetBrushColor(RarityColor);
		RarityBorder->SetVisibility(ESlateVisibility::Visible);
	}

	// Update equipped icon
	if (EquippedIcon && InventoryItem)
	{
		// Check if item is equipped (would need equipment system integration)
		// For now, just hide it
		EquippedIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}
