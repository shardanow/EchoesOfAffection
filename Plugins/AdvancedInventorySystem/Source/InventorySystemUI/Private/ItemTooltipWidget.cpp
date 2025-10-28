// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItemTooltipWidget.h"
#include "ItemDataAsset.h"
#include "InventoryItem.h"
#include "InventorySystemTypes.h"
#include "Engine/Texture2D.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Framework/Application/SlateApplication.h"

void UItemTooltipWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateWidgets();
}

void UItemTooltipWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UItemTooltipWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	UpdateWidgets();
}

void UItemTooltipWidget::UpdateTooltipPosition()
{
	// Update position to follow mouse cursor
	if (IsInViewport())
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			// Get current mouse position in viewport space
			FVector2D MousePosition;
			PC->GetMousePosition(MousePosition.X, MousePosition.Y);
			
			// Add small offset so tooltip doesn't cover cursor
			FVector2D TooltipPosition = MousePosition + FVector2D(15.0f, 15.0f);
			
			// Update position
			SetPositionInViewport(TooltipPosition, false);
		}
	}
}

void UItemTooltipWidget::SetItemData(UItemDataAsset* InItemData, int32 InQuantity)
{
	ItemData = InItemData;
	InventoryItem = nullptr;
	Quantity = InQuantity;

	UpdateWidgets();
	OnTooltipUpdated();
}

void UItemTooltipWidget::SetInventoryItem(UInventoryItem* InItem)
{
	InventoryItem = InItem;
	
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
	OnTooltipUpdated();
}

void UItemTooltipWidget::ClearTooltip()
{
	ItemData = nullptr;
	InventoryItem = nullptr;
	Quantity = 0;

	UpdateWidgets();
	OnTooltipCleared();
}

void UItemTooltipWidget::UpdateWidgets()
{
	if (!ItemData)
	{
		// Clear all widgets
		if (ItemNameText)
		{
			ItemNameText->SetText(FText::GetEmpty());
		}
		if (ItemDescriptionText)
		{
			ItemDescriptionText->SetText(FText::GetEmpty());
		}
		if (ItemIconImage)
		{
			ItemIconImage->SetBrushFromTexture(nullptr);
		}
		if (ItemValueText)
		{
			ItemValueText->SetText(FText::GetEmpty());
		}
		if (ItemWeightText)
		{
			ItemWeightText->SetText(FText::GetEmpty());
		}
		if (ItemRarityText)
		{
			ItemRarityText->SetText(FText::GetEmpty());
		}
		if (ItemCategoryText)
		{
			ItemCategoryText->SetText(FText::GetEmpty());
		}
		if (QuantityText)
		{
			QuantityText->SetText(FText::GetEmpty());
		}
		if (QuestItemIcon)
		{
			QuestItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}

	// Update item name
	if (ItemNameText)
	{
		ItemNameText->SetText(ItemData->ItemName);
		ItemNameText->SetColorAndOpacity(GetRarityColor());
	}

	// Update description
	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(ItemData->ItemDescription);
	}

	// Update icon
	if (ItemIconImage && ItemData->ItemIcon)
	{
		ItemIconImage->SetBrushFromTexture(ItemData->ItemIcon);
	}

	// Update value
	if (ItemValueText)
	{
		FText ValueText = FText::Format(
			FText::FromString("{0} Gold"),
			FText::AsNumber(ItemData->BaseValue)
		);
		ItemValueText->SetText(ValueText);
	}

	// Update weight
	if (ItemWeightText)
	{
		float Weight = GetItemWeight();
		if (Weight > 0.0f)
		{
			FText WeightText = FText::Format(
				FText::FromString("{0} kg"),
				FText::AsNumber(Weight)
			);
			ItemWeightText->SetText(WeightText);
		}
		else
		{
			ItemWeightText->SetText(FText::FromString("0 kg"));
		}
	}

	// Update rarity
	if (ItemRarityText)
	{
		const UEnum* RarityEnum = StaticEnum<EItemRarity>();
		FString RarityString = RarityEnum ? RarityEnum->GetNameStringByValue((int64)ItemData->Rarity) : "Unknown";
		// Remove "EItemRarity::" prefix if present
		RarityString.RemoveFromStart("EItemRarity::");
		ItemRarityText->SetText(FText::FromString(RarityString));
		ItemRarityText->SetColorAndOpacity(GetRarityColor());
	}

	// Update category
	if (ItemCategoryText)
	{
		const UEnum* CategoryEnum = StaticEnum<EItemCategory>();
		FString CategoryString = CategoryEnum ? CategoryEnum->GetNameStringByValue((int64)ItemData->Category) : "Unknown";
		// Remove "EItemCategory::" prefix if present
		CategoryString.RemoveFromStart("EItemCategory::");
		ItemCategoryText->SetText(FText::FromString(CategoryString));
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

	// Update background color
	if (BackgroundBorder)
	{
		FLinearColor RarityColor = GetRarityColor();
		RarityColor.A = 0.3f; // Semi-transparent
		BackgroundBorder->SetBrushColor(RarityColor);
	}

	// Update item type info
	if (ItemTypeText)
	{
		TArray<FString> TypeInfo;
		if (ItemData->bIsStackable) TypeInfo.Add("Stackable");
		if (ItemData->bIsUsable) TypeInfo.Add("Usable");
		if (ItemData->bIsEquippable) TypeInfo.Add("Equippable");
		
		FString TypeString = FString::Join(TypeInfo, TEXT(" • "));
		ItemTypeText->SetText(FText::FromString(TypeString));
	}

	// Update quest item indicator
	if (QuestItemIcon)
	{
		QuestItemIcon->SetVisibility(
			ItemData->bIsQuestItem ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
		);
	}
}

FText UItemTooltipWidget::GetItemName() const
{
	if (ItemData)
	{
		return ItemData->ItemName;
	}
	return FText::GetEmpty();
}

FText UItemTooltipWidget::GetItemDescription() const
{
	if (ItemData)
	{
		return ItemData->ItemDescription;
	}
	return FText::GetEmpty();
}

UTexture2D* UItemTooltipWidget::GetItemIcon() const
{
	if (ItemData)
	{
		return ItemData->ItemIcon;
	}
	return nullptr;
}

EItemRarity UItemTooltipWidget::GetItemRarity() const
{
	if (ItemData)
	{
		return ItemData->Rarity;
	}
	return EItemRarity::Common;
}

EItemCategory UItemTooltipWidget::GetItemCategory() const
{
	if (ItemData)
	{
		return ItemData->Category;
	}
	return EItemCategory::Miscellaneous;
}

int32 UItemTooltipWidget::GetItemValue() const
{
	if (ItemData)
	{
		return ItemData->BaseValue;
	}
	return 0;
}

bool UItemTooltipWidget::IsStackable() const
{
	if (ItemData)
	{
		return ItemData->bIsStackable;
	}
	return false;
}

bool UItemTooltipWidget::IsUsable() const
{
	if (ItemData)
	{
		return ItemData->bIsUsable;
	}
	return false;
}

bool UItemTooltipWidget::IsEquippable() const
{
	if (ItemData)
	{
		return ItemData->bIsEquippable;
	}
	return false;
}

bool UItemTooltipWidget::IsQuestItem() const
{
	if (ItemData)
	{
		return ItemData->bIsQuestItem;
	}
	return false;
}

float UItemTooltipWidget::GetItemWeight() const
{
	if (ItemData)
	{
		// Return total weight (item weight * quantity)
		return ItemData->ItemWeight * Quantity;
	}
	return 0.0f;
}

FLinearColor UItemTooltipWidget::GetRarityColor() const
{
	if (ItemData)
	{
		return GetColorForRarity(ItemData->Rarity);
	}
	return FLinearColor::White;
}

FLinearColor UItemTooltipWidget::GetColorForRarity(EItemRarity Rarity) const
{
	switch (Rarity)
	{
	case EItemRarity::Common:
		return FLinearColor(0.8f, 0.8f, 0.8f, 1.0f); // Gray
	
	case EItemRarity::Uncommon:
		return FLinearColor(0.1f, 1.0f, 0.1f, 1.0f); // Green
	
	case EItemRarity::Rare:
		return FLinearColor(0.0f, 0.5f, 1.0f, 1.0f); // Blue
	
	case EItemRarity::Epic:
		return FLinearColor(0.64f, 0.21f, 0.93f, 1.0f); // Purple
	
	case EItemRarity::Legendary:
		return FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); // Orange
	
	case EItemRarity::Unique:
		return FLinearColor(1.0f, 0.84f, 0.0f, 1.0f); // Gold
	
	default:
		return FLinearColor::White;
	}
}
