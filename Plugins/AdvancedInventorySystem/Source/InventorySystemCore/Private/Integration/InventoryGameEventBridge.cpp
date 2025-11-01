// Copyright Epic Games, Inc. All Rights Reserved.

#include "Integration/InventoryGameEventBridge.h"

// Conditional include - only if GameEventBus is available
#if __has_include("GameEventBusSubsystem.h")
	#include "GameEventBusSubsystem.h"
	#include "GameEventBusLibrary.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

DEFINE_LOG_CATEGORY_STATIC(LogInventoryEvents, Log, All);

void UInventoryGameEventBridge::EmitItemAcquiredEvent(UObject* WorldContextObject, FName ItemId, int32 Count, AActor* Acquirer)
{
#if GAMEEVENTBUS_AVAILABLE
	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge::EmitItemAcquiredEvent] ?? Called! ItemId='%s', Count=%d, Acquirer=%s"),
		*ItemId.ToString(),
		Count,
		Acquirer ? *Acquirer->GetName() : TEXT("NULL"));

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory.ItemAcquired"), false);
	
	if (!EventTag.IsValid())
	{
		UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? EventTag 'GameEvent.Inventory.ItemAcquired' is INVALID! Check DefaultGameplayTags.ini"));
		return;
	}

	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ? EventTag valid: '%s'"), *EventTag.ToString());

	UGameEventBusLibrary::EmitItemEvent(WorldContextObject, EventTag, ItemId, Count, Acquirer, false);
	
	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ?? Event emitted to GameEventBus!"));
#else
	UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? GameEventBus NOT AVAILABLE!"));
#endif
}

void UInventoryGameEventBridge::EmitItemUsedEvent(UObject* WorldContextObject, FName ItemId, AActor* User, AActor* Target)
{
#if GAMEEVENTBUS_AVAILABLE
	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge::EmitItemUsedEvent] ?? Called! ItemId='%s', User=%s, Target=%s"),
		*ItemId.ToString(),
		User ? *User->GetName() : TEXT("NULL"),
		Target ? *Target->GetName() : TEXT("NULL"));

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory.ItemUsed"), false);
	
	if (!EventTag.IsValid())
	{
		UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? EventTag 'GameEvent.Inventory.ItemUsed' is INVALID! Check DefaultGameplayTags.ini"));
		return;
	}

	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ? EventTag valid: '%s'"), *EventTag.ToString());

	FGameEventPayload Payload = UGameEventBusLibrary::MakeItemPayload(EventTag, ItemId, 1, User);
	Payload.TargetActor = Target;
	
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (EventBus)
	{
		EventBus->EmitEvent(Payload, false);
		UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ?? Item Used event emitted to GameEventBus!"));
	}
	else
	{
		UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? EventBus is NULL!"));
	}
#else
	UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? GameEventBus NOT AVAILABLE!"));
#endif
}

void UInventoryGameEventBridge::EmitItemCraftedEvent(UObject* WorldContextObject, FName ItemId, int32 Count, AActor* Crafter)
{
#if GAMEEVENTBUS_AVAILABLE
	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge::EmitItemCraftedEvent] ?? Called! ItemId='%s', Count=%d, Crafter=%s"),
		*ItemId.ToString(),
		Count,
		Crafter ? *Crafter->GetName() : TEXT("NULL"));

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory.ItemCrafted"), false);
	
	if (!EventTag.IsValid())
	{
		UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? EventTag 'GameEvent.Inventory.ItemCrafted' is INVALID! Check DefaultGameplayTags.ini"));
		return;
	}

	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ? EventTag valid: '%s'"), *EventTag.ToString());

	UGameEventBusLibrary::EmitItemEvent(WorldContextObject, EventTag, ItemId, Count, Crafter, false);
	
	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ?? Item Crafted event emitted to GameEventBus!"));
#else
	UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? GameEventBus NOT AVAILABLE!"));
#endif
}

void UInventoryGameEventBridge::EmitItemGiftedEvent(UObject* WorldContextObject, FName ItemId, int32 Count, AActor* Giver, AActor* Receiver)
{
#if GAMEEVENTBUS_AVAILABLE
	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge::EmitItemGiftedEvent] ?? Called! ItemId='%s', Count=%d, Giver=%s, Receiver=%s"),
		*ItemId.ToString(),
		Count,
		Giver ? *Giver->GetName() : TEXT("NULL"),
		Receiver ? *Receiver->GetName() : TEXT("NULL"));

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory.ItemGifted"), false);
	
	if (!EventTag.IsValid())
	{
		UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? EventTag 'GameEvent.Inventory.ItemGifted' is INVALID! Check DefaultGameplayTags.ini"));
		return;
	}

	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ? EventTag valid: '%s'"), *EventTag.ToString());

	FGameEventPayload Payload = UGameEventBusLibrary::MakeItemPayload(EventTag, ItemId, Count, Giver);
	Payload.TargetActor = Receiver;
	
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (EventBus)
	{
		EventBus->EmitEvent(Payload, false);
		UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ?? Item Gifted event emitted to GameEventBus!"));
	}
	else
	{
		UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? EventBus is NULL!"));
	}
#else
	UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? GameEventBus NOT AVAILABLE!"));
#endif
}

void UInventoryGameEventBridge::EmitItemDroppedEvent(UObject* WorldContextObject, FName ItemId, int32 Count, AActor* Dropper)
{
#if GAMEEVENTBUS_AVAILABLE
	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge::EmitItemDroppedEvent] ?? Called! ItemId='%s', Count=%d, Dropper=%s"),
		*ItemId.ToString(),
		Count,
		Dropper ? *Dropper->GetName() : TEXT("NULL"));

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory.ItemDropped"), false);
	
	if (!EventTag.IsValid())
	{
		UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? EventTag 'GameEvent.Inventory.ItemDropped' is INVALID! Check DefaultGameplayTags.ini"));
		return;
	}

	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ? EventTag valid: '%s'"), *EventTag.ToString());

	UGameEventBusLibrary::EmitItemEvent(WorldContextObject, EventTag, ItemId, Count, Dropper, false);
	
	UE_LOG(LogInventoryEvents, Log, TEXT("[InventoryGameEventBridge] ?? Item Dropped event emitted to GameEventBus!"));
#else
	UE_LOG(LogInventoryEvents, Error, TEXT("[InventoryGameEventBridge] ? GameEventBus NOT AVAILABLE!"));
#endif
}

void UInventoryGameEventBridge::EmitCustomInventoryEvent(
	UObject* WorldContextObject,
	const FGameplayTag& EventTag,
	FName ItemId,
	int32 Count,
	AActor* Instigator,
	AActor* Target,
	const FGameplayTagContainer& AdditionalTags)
{
#if GAMEEVENTBUS_AVAILABLE
	if (!EventTag.IsValid())
	{
		UE_LOG(LogInventoryEvents, Warning, TEXT("EmitCustomInventoryEvent: Invalid EventTag"));
		return;
	}

	FGameEventPayload Payload = UGameEventBusLibrary::MakeItemPayload(EventTag, ItemId, Count, Instigator);
	Payload.TargetActor = Target;
	Payload.AdditionalTags = AdditionalTags;
	
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (EventBus)
	{
		EventBus->EmitEvent(Payload, false);
		UE_LOG(LogInventoryEvents, Verbose, TEXT("Custom Inventory Event: %s (Item: %s)"), 
			*EventTag.ToString(), 
			*ItemId.ToString());
	}
#else
	UE_LOG(LogInventoryEvents, Warning, TEXT("GameEventBus not available - custom inventory event not emitted"));
#endif
}
