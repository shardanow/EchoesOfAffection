// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryWidgetBase.h"
#include "InventoryComponent.h"
#include "ItemTooltipWidget.h"
#include "InventorySlotWidget.h"
#include "InventoryContextMenuWidget.h"
#include "ItemDataAsset.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"

void UInventoryWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	// Log widget binding state
	UE_LOG(LogTemp, Warning, TEXT("InventoryWidgetBase::NativeConstruct called"));
	UE_LOG(LogTemp, Warning, TEXT("  WeightText: %s"), WeightText ? TEXT("Bound") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("  SlotsText: %s"), SlotsText ? TEXT("Bound") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("  MaxWeightText: %s"), MaxWeightText ? TEXT("Bound") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("  InventoryGrid: %s"), InventoryGrid ? TEXT("Bound") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("  BoundInventory: %s"), BoundInventory ? TEXT("Valid") : TEXT("NULL"));

	// Bind sort button if present (only if not already bound)
	if (SortButton && !SortButton->OnClicked.IsBound())
	{
		SortButton->OnClicked.AddDynamic(this, &UInventoryWidgetBase::HandleSortButtonClicked);
	}

	// Auto-bind close button if present (only if not already bound)
	if (CloseButton && !CloseButton->OnClicked.IsBound())
	{
		CloseButton->OnClicked.AddDynamic(this, &UInventoryWidgetBase::HideInventory);
	}

	UpdateWidgets();
}

void UInventoryWidgetBase::NativeDestruct()
{
	// DON'T unbind from inventory on destruct!
	// Widget may be shown/hidden multiple times
	// UnbindFromInventory();  // ? REMOVED - causes BoundInventory to become NULL
	
	HideItemTooltip();
	Super::NativeDestruct();
}

FReply UInventoryWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Close context menu if it's open and click is outside of it
	if (CurrentContextMenu && CurrentContextMenu->IsInViewport())
	{
		// Check if click was on the context menu itself
		FVector2D LocalMousePos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		
		// Get context menu geometry
		FGeometry ContextMenuGeometry = CurrentContextMenu->GetCachedGeometry();
		FVector2D ContextMenuScreenPos = ContextMenuGeometry.GetAbsolutePosition();
		FVector2D ContextMenuSize = ContextMenuGeometry.GetAbsoluteSize();
		
		FVector2D MouseScreenPos = InMouseEvent.GetScreenSpacePosition();
		
		// Check if click is outside context menu bounds
		bool bClickedOutside = 
			MouseScreenPos.X < ContextMenuScreenPos.X ||
			MouseScreenPos.X > ContextMenuScreenPos.X + ContextMenuSize.X ||
			MouseScreenPos.Y < ContextMenuScreenPos.Y ||
			MouseScreenPos.Y > ContextMenuScreenPos.Y + ContextMenuSize.Y;
		
		if (bClickedOutside)
		{
			HideContextMenu();
			return FReply::Handled(); // Consume the click
		}
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UInventoryWidgetBase::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Update tooltip position if tooltip is showing
	// EVENT-DRIVEN: Only called when mouse actually moves!
	// Don't update tooltip if context menu is open
	if (CurrentTooltip && CurrentTooltip->IsInViewport() && 
		!(CurrentContextMenu && CurrentContextMenu->IsInViewport()))
	{
		CurrentTooltip->UpdateTooltipPosition();
	}
	
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UInventoryWidgetBase::BindToInventory(UInventoryComponent* InInventoryComponent)
{
	// Unbind from previous inventory
	UnbindFromInventory();

	BoundInventory = InInventoryComponent;

	if (BoundInventory)
	{
		// Bind to inventory events
		BoundInventory->OnInventoryChanged.AddDynamic(this, &UInventoryWidgetBase::HandleInventoryChanged);
		BoundInventory->OnItemAdded.AddDynamic(this, &UInventoryWidgetBase::HandleItemAdded);
		BoundInventory->OnItemRemoved.AddDynamic(this, &UInventoryWidgetBase::HandleItemRemoved);
		BoundInventory->OnItemUsed.AddDynamic(this, &UInventoryWidgetBase::HandleItemUsed);
		
		// Bind to weight changed event if weight system is enabled
		if (BoundInventory->MaxWeightCapacity > 0.0f)
		{
			BoundInventory->OnWeightChanged.AddDynamic(this, &UInventoryWidgetBase::HandleWeightChanged);
		}

		// Initial update
		HandleInventoryChanged();
	}
}

void UInventoryWidgetBase::UnbindFromInventory()
{
	if (BoundInventory)
	{
		// Unbind from inventory events
		BoundInventory->OnInventoryChanged.RemoveDynamic(this, &UInventoryWidgetBase::HandleInventoryChanged);
		BoundInventory->OnItemAdded.RemoveDynamic(this, &UInventoryWidgetBase::HandleItemAdded);
		BoundInventory->OnItemRemoved.RemoveDynamic(this, &UInventoryWidgetBase::HandleItemRemoved);
		BoundInventory->OnItemUsed.RemoveDynamic(this, &UInventoryWidgetBase::HandleItemUsed);
		
		// Unbind weight event if it was bound
		if (BoundInventory->OnWeightChanged.IsBound())
		{
			BoundInventory->OnWeightChanged.RemoveDynamic(this, &UInventoryWidgetBase::HandleWeightChanged);
		}

		BoundInventory = nullptr;
	}
}

void UInventoryWidgetBase::ShowInventory()
{
	// Only add to viewport if not already showing
	if (!IsInViewport())
	{
		AddToViewport(10); // Z-order 10
	}

	// ALWAYS refresh when showing (even if already in viewport)
	RefreshInventorySlots();
	UpdateWidgets();

	// Set input mode to UI with game input allowed
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		// FIXED: Use GameAndUI mode to allow toggle while inventory is open
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UInventoryWidgetBase::HideInventory()
{
	if (IsInViewport())
	{
		// Hide tooltip first
		HideItemTooltip();

		// Hide context menu
		HideContextMenu();

		// Remove from viewport
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

void UInventoryWidgetBase::ToggleInventory()
{
	if (IsInViewport())
	{
		HideInventory();
	}
	else
	{
		ShowInventory();
	}
}

bool UInventoryWidgetBase::IsInventoryVisible() const
{
	return IsInViewport();
}

void UInventoryWidgetBase::UpdateWidgets()
{
	if (!BoundInventory)
	{
		// Clear all widgets
		if (WeightText)
		{
			WeightText->SetText(FText::GetEmpty());
		}
		if (MaxWeightText)
		{
			MaxWeightText->SetText(FText::GetEmpty());
		}
		if (SlotsText)
		{
			SlotsText->SetText(FText::GetEmpty());
		}
		if (WeightBar)
		{
			WeightBar->SetPercent(0.0f);
		}
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidgetBase::UpdateWidgets - No BoundInventory!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryWidgetBase::UpdateWidgets - Updating display widgets"));

	UpdateWeightDisplay();
	UpdateSlotsDisplay();

	// Update title if present
	if (TitleText)
	{
		TitleText->SetText(FText::FromString("Inventory"));
	}

	// Log widget state
	UE_LOG(LogTemp, Log, TEXT("  WeightText: %s"), WeightText ? TEXT("Valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  SlotsText: %s"), SlotsText ? TEXT("Valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  MaxWeightText: %s"), MaxWeightText ? TEXT("Valid") : TEXT("NULL"));
}

void UInventoryWidgetBase::UpdateWeightDisplay()
{
	if (!BoundInventory)
	{
		return;
	}

	// Get weight data from inventory using new weight system
	float CurrentWeight = BoundInventory->GetTotalWeight();
	float MaxWeight = BoundInventory->MaxWeightCapacity;

	// Update weight text
	if (WeightText)
	{
		FText WeightFormatted = FText::Format(
			FText::FromString("{0} kg"),
			FText::AsNumber(FMath::RoundToFloat(CurrentWeight * 10.0f) / 10.0f) // Round to 1 decimal
		);
		WeightText->SetText(WeightFormatted);
	}

	// Update max weight text
	if (MaxWeightText)
	{
		if (MaxWeight > 0.0f)
		{
			FText MaxWeightFormatted = FText::Format(
				FText::FromString("{0} kg"),
				FText::AsNumber(FMath::RoundToFloat(MaxWeight * 10.0f) / 10.0f)
			);
			MaxWeightText->SetText(MaxWeightFormatted);
		}
		else
		{
			MaxWeightText->SetText(FText::FromString("?")); // Unlimited weight
		}
	}

	// Update weight progress bar
	if (WeightBar)
	{
		if (MaxWeight > 0.0f)
		{
			float Percent = BoundInventory->GetWeightPercentage();
			WeightBar->SetPercent(Percent);

			// Color based on weight percentage
			FLinearColor BarColor;
			if (Percent >= 1.0f)
			{
				BarColor = FLinearColor(1.0f, 0.0f, 0.0f); // Red - Overweight!
			}
			else if (Percent > 0.9f)
			{
				BarColor = FLinearColor(1.0f, 0.5f, 0.0f); // Orange - Almost full
			}
			else if (Percent > 0.7f)
			{
				BarColor = FLinearColor(1.0f, 1.0f, 0.0f); // Yellow - Getting full
			}
			else
			{
				BarColor = FLinearColor(0.0f, 1.0f, 0.0f); // Green - Good
			}
			WeightBar->SetFillColorAndOpacity(BarColor);
		}
		else
		{
			// Unlimited weight - hide bar or show empty
			WeightBar->SetPercent(0.0f);
			WeightBar->SetFillColorAndOpacity(FLinearColor::Gray);
		}
	}
}

void UInventoryWidgetBase::UpdateSlotsDisplay()
{
	if (!BoundInventory)
	{
		return;
	}

	// Get all items to count used slots
	TArray<UInventoryItem*> Items = BoundInventory->GetAllItems();
	int32 UsedSlots = Items.Num();
	int32 MaxSlots = BoundInventory->MaxInventorySlots;

	if (SlotsText)
	{
		FText SlotsFormatted = FText::Format(
			FText::FromString("{0} / {1}"),
			FText::AsNumber(UsedSlots),
			FText::AsNumber(MaxSlots)
		);
		SlotsText->SetText(SlotsFormatted);
	}
}

void UInventoryWidgetBase::RefreshInventorySlots()
{
	if (!InventoryGrid || !BoundInventory)
	{
		return;
	}

	// Clear existing slot widgets (ONLY slots, not other widgets!)
	// Store widgets that are NOT our slot widgets
	TArray<UWidget*> OtherWidgets;
	for (UWidget* Child : InventoryGrid->GetAllChildren())
	{
		// Check if this is one of our slot widgets
		if (!SlotWidgets.Contains(Cast<UInventorySlotWidget>(Child)))
		{
			// This is NOT our slot - keep it
			OtherWidgets.Add(Child);
		}
	}

	// Clear ALL children
	InventoryGrid->ClearChildren();
	SlotWidgets.Empty();

	// Re-add other widgets that were there
	for (UWidget* Widget : OtherWidgets)
	{
		InventoryGrid->AddChild(Widget);
	}

	if (!SlotWidgetClass)
	{
		// No slot widget class set - inventory won't show items
		// This is OK - user can populate grid manually in Blueprint
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidgetBase: No SlotWidgetClass set! Items won't be shown."));
		return;
	}

	// Get all items
	TArray<UInventoryItem*> Items = BoundInventory->GetAllItems();

	UE_LOG(LogTemp, Log, TEXT("InventoryWidgetBase: Refreshing inventory slots. Item count: %d"), Items.Num());

	// Create slots for each item
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		UInventoryItem* Item = Items[i];
		if (!Item)
		{
			continue;
		}

		// Create slot widget
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
		if (!SlotWidget)
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryWidgetBase: Failed to create slot widget for item %d"), i);
			continue;
		}

		// Set slot data
		SlotWidget->SetInventoryItem(Item, i);

		// Add to grid
		int32 Row = i / GridColumns;
		int32 Column = i % GridColumns;
		
		// Add to uniform grid (will use slot widget's size from Blueprint)
		InventoryGrid->AddChildToUniformGrid(SlotWidget, Row, Column);

		// Store reference
		SlotWidgets.Add(SlotWidget);
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryWidgetBase: Created %d slot widgets"), SlotWidgets.Num());
}

void UInventoryWidgetBase::HandleInventoryChanged()
{
	RefreshInventorySlots();
	UpdateWidgets();
	OnInventoryUpdated();
}

void UInventoryWidgetBase::HandleItemAdded(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
	OnItemAddedToInventory(Item, Quantity, SlotIndex);
	UpdateWidgets();
}

void UInventoryWidgetBase::HandleItemRemoved(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
	OnItemRemovedFromInventory(Item, Quantity, SlotIndex);
	UpdateWidgets();
}

void UInventoryWidgetBase::HandleItemUsed(UInventoryItem* Item, AActor* User, bool bSuccess)
{
	OnItemUsedFromInventory(Item, User, bSuccess);
}

void UInventoryWidgetBase::HandleSortButtonClicked()
{
	if (BoundInventory)
	{
		BoundInventory->SortInventory();
	}
}

void UInventoryWidgetBase::ShowItemTooltip(UItemDataAsset* ItemData, int32 Quantity, FVector2D ScreenPosition)
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

void UInventoryWidgetBase::HideItemTooltip()
{
	if (CurrentTooltip)
	{
		CurrentTooltip.Get()->RemoveFromParent();
		CurrentTooltip = nullptr;
	}
}

void UInventoryWidgetBase::ShowContextMenu(UItemDataAsset* ItemData, UInventoryItem* InventoryItem, FVector2D ScreenPosition)
{
	if (!ContextMenuClass || !ItemData || !BoundInventory)
	{
		return;
	}

	// Hide tooltip when context menu is shown
	HideItemTooltip();

	// Hide existing context menu first
	HideContextMenu();

	// Create context menu if needed
	if (!CurrentContextMenu)
	{
		CurrentContextMenu = CreateWidget<UInventoryContextMenuWidget>(GetOwningPlayer(), ContextMenuClass);
		if (!CurrentContextMenu)
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryWidgetBase: Failed to create context menu widget!"));
			return;
		}
	}

	// Show menu
	CurrentContextMenu->ShowMenu(ItemData, InventoryItem, BoundInventory, ScreenPosition);
}

void UInventoryWidgetBase::HideContextMenu()
{
	if (CurrentContextMenu)
	{
		CurrentContextMenu->HideMenu();
		// Don't destroy - keep for reuse
	}
}

void UInventoryWidgetBase::HandleWeightChanged(float CurrentWeight, float MaxWeight)
{
	// Update weight display when weight changes
	UpdateWeightDisplay();
}
