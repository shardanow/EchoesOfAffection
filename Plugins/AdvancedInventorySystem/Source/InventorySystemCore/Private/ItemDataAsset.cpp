// Copyright Epic Games, Inc. All Rights Reserved.

#include "ItemDataAsset.h"
#include "ItemEffectDataAsset.h"
#include "InventorySystemCore.h"

UItemDataAsset::UItemDataAsset()
{
	// Set default values
	ItemName = FText::FromString("New Item");
	ItemDescription = FText::FromString("Item description");
	MaxStackSize = 1;
	bIsStackable = false;
	bIsUsable = false;
	bConsumeOnUse = false;
	UseCooldown = 0.0f;
	bIsEquippable = false;
	bIsQuestItem = false;
	bIsReadable = false;
	BaseValue = 0;
	bCanBeSold = true;
	bCanBeBought = true;
	bCanBeGifted = true;
	SellbackPercentage = 0.5f;
	bIsCurrency = false;
	MinimumLevel = 0;
	Rarity = EItemRarity::Common;
	Category = EItemCategory::Miscellaneous;
}

FPrimaryAssetId UItemDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("Item", ItemID);
}

bool UItemDataAsset::CanStackWith(const UItemDataAsset* OtherItem) const
{
	if (!OtherItem || !bIsStackable || !OtherItem->bIsStackable)
	{
		return false;
	}

	// Must be the same item
	return ItemID == OtherItem->ItemID;
}

int32 UItemDataAsset::GetEffectiveMaxStackSize() const
{
	if (!bIsStackable)
	{
		return 1;
	}

	return MaxStackSize > 0 ? MaxStackSize : MAX_int32;
}

bool UItemDataAsset::MeetsRequirements(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	// Check for required tags
	if (RequiredTags.Num() > 0)
	{
		// Actor needs to implement a tag interface or component
		// For now, we'll return true if no tag system is present
		// This should be extended with proper tag checking
	}

	// Check for blocked tags
	if (BlockedTags.Num() > 0)
	{
		// Similar to above, needs proper implementation
	}

	// Minimum level check would require a level interface
	// For now, we assume it's met

	return true;
}

TArray<UItemEffectDataAsset*> UItemDataAsset::GetEffectsByTrigger(EItemEffectTrigger TriggerType) const
{
	TArray<UItemEffectDataAsset*> Result;
	
	for (UItemEffectDataAsset* Effect : ItemEffects)
	{
		if (Effect && Effect->TriggerType == TriggerType)
		{
			Result.Add(Effect);
		}
	}

	return Result;
}
