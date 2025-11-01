// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryComponentGameEventHelper.h"
#include "InventoryComponent.h"
#include "ItemDataAsset.h"

// Conditional GameEventBus integration
#if __has_include("Integration/InventoryGameEventBridge.h")
	#include "Integration/InventoryGameEventBridge.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

void UInventoryComponentGameEventHelper::EmitItemAcquiredEvent(UInventoryComponent* InventoryComponent, UItemDataAsset* ItemData, int32 Quantity, AActor* Player)
{
	if (!InventoryComponent || !ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ? Invalid params! InventoryComponent=%s, ItemData=%s"),
			InventoryComponent ? TEXT("Valid") : TEXT("NULL"),
			ItemData ? TEXT("Valid") : TEXT("NULL"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ?? Emitting ItemAcquired event: ItemID='%s', Quantity=%d, Player=%s"),
		*ItemData->ItemID.ToString(),
		Quantity,
		Player ? *Player->GetName() : TEXT("NULL"));

#if GAMEEVENTBUS_AVAILABLE
	UInventoryGameEventBridge::EmitItemAcquiredEvent(InventoryComponent, ItemData->ItemID, Quantity, Player);
	UE_LOG(LogTemp, Log, TEXT("[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ? Event emitted to GameEventBridge"));
#else
	UE_LOG(LogTemp, Warning, TEXT("[InventoryComponentGameEventHelper::EmitItemAcquiredEvent] ?? GameEventBus NOT AVAILABLE!"));
#endif
}

void UInventoryComponentGameEventHelper::EmitItemUsedEvent(UInventoryComponent* InventoryComponent, UItemDataAsset* ItemData, int32 Quantity, AActor* User)
{
	if (!InventoryComponent || !ItemData)
	{
		return;
	}

#if GAMEEVENTBUS_AVAILABLE
	UInventoryGameEventBridge::EmitItemUsedEvent(InventoryComponent, ItemData->ItemID, User, nullptr);
#endif
}

void UInventoryComponentGameEventHelper::EmitItemGiftedEvent(UInventoryComponent* InventoryComponent, UItemDataAsset* ItemData, int32 Quantity, AActor* Giver, AActor* Receiver)
{
	if (!InventoryComponent || !ItemData)
	{
		return;
	}

#if GAMEEVENTBUS_AVAILABLE
	UInventoryGameEventBridge::EmitItemGiftedEvent(InventoryComponent, ItemData->ItemID, Quantity, Giver, Receiver);
#endif
}
