// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShopDataAsset.h"

UShopDataAsset::UShopDataAsset()
{
	ShopName = FText::FromString("New Shop");
	ShopDescription = FText::FromString("Shop description");
	PriceModifier = 1.0f;
	SellbackPercentage = 0.5f;
	bBuysItems = true;
}
