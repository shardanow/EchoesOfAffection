// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryQuestRewardListener.h"
#include "InventoryComponent.h"
#include "ItemDataAsset.h"
#include "Engine/AssetManager.h"

// Conditional GameEventBus integration
#if __has_include("GameEventBusSubsystem.h")
	#include "GameEventBusSubsystem.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

DEFINE_LOG_CATEGORY_STATIC(LogInventoryQuestRewards, Log, All);

UInventoryQuestRewardListener::UInventoryQuestRewardListener()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryQuestRewardListener::BeginPlay()
{
	Super::BeginPlay();

#if GAMEEVENTBUS_AVAILABLE
	// Subscribe to GameEvent.Quest.GrantItem
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
	if (EventBus)
	{
		FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Quest.GrantItem"), false);
		if (EventTag.IsValid())
		{
			// Native subscription (more efficient)
			EventSubscriptionHandle = EventBus->SubscribeToEventNative(
				EventTag,
				FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UInventoryQuestRewardListener::HandleGrantItemEvent)
			);

			UE_LOG(LogInventoryQuestRewards, Log, TEXT("? InventoryQuestRewardListener: Subscribed to 'GameEvent.Quest.GrantItem' on %s"), 
				*GetOwner()->GetName());
		}
		else
		{
			UE_LOG(LogInventoryQuestRewards, Error, TEXT("? InventoryQuestRewardListener: EventTag 'GameEvent.Quest.GrantItem' is INVALID!"));
			UE_LOG(LogInventoryQuestRewards, Error, TEXT("   Add to Config/DefaultGameplayTags.ini:"));
			UE_LOG(LogInventoryQuestRewards, Error, TEXT("   +GameplayTagList=(Tag=\"GameEvent.Quest.GrantItem\",DevComment=\"[QuestSystem] Request to grant item to player\")"));
		}
	}
	else
	{
		UE_LOG(LogInventoryQuestRewards, Error, TEXT("? InventoryQuestRewardListener: GameEventBusSubsystem not found!"));
	}
#else
	UE_LOG(LogInventoryQuestRewards, Error, TEXT("? InventoryQuestRewardListener: GameEventBus NOT AVAILABLE!"));
#endif
}

void UInventoryQuestRewardListener::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if GAMEEVENTBUS_AVAILABLE
	// Unsubscribe from events
	if (EventSubscriptionHandle.IsValid())
	{
		UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
		if (EventBus)
		{
			FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Quest.GrantItem"), false);
			EventBus->UnsubscribeNative(EventTag, EventSubscriptionHandle);
			
			UE_LOG(LogInventoryQuestRewards, Log, TEXT("InventoryQuestRewardListener: Unsubscribed from events"));
		}
	}
#endif

	Super::EndPlay(EndPlayReason);
}

void UInventoryQuestRewardListener::HandleGrantItemEvent(const FGameEventPayload& Payload)
{
	UE_LOG(LogInventoryQuestRewards, Warning, TEXT("?? InventoryQuestRewardListener: Received 'GameEvent.Quest.GrantItem'!"));
	UE_LOG(LogInventoryQuestRewards, Warning, TEXT("   ItemId='%s', Amount=%d, QuestId='%s'"),
		*Payload.StringParam.ToString(),
		Payload.IntParam,
		*Payload.StringParam2.ToString());

	// Extract parameters
	FName ItemId = Payload.StringParam;
	int32 Amount = Payload.IntParam;
	FName QuestId = Payload.StringParam2;

	if (ItemId.IsNone() || Amount <= 0)
	{
		UE_LOG(LogInventoryQuestRewards, Error, TEXT("? InventoryQuestRewardListener: Invalid parameters! ItemId='%s', Amount=%d"),
			*ItemId.ToString(), Amount);
		return;
	}

	// Find InventoryComponent on owner
	UInventoryComponent* InventoryComp = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComp)
	{
		UE_LOG(LogInventoryQuestRewards, Error, TEXT("? InventoryQuestRewardListener: Owner '%s' has no InventoryComponent!"),
			*GetOwner()->GetName());
		return;
	}

	// Find ItemDataAsset
	UItemDataAsset* ItemData = FindItemDataAssetByID(ItemId);
	if (!ItemData)
	{
		UE_LOG(LogInventoryQuestRewards, Error, TEXT("? InventoryQuestRewardListener: Failed to find ItemDataAsset for ItemID='%s'"),
			*ItemId.ToString());
		UE_LOG(LogInventoryQuestRewards, Error, TEXT("   Please create an ItemDataAsset with ItemID='%s' in /Game/Items/ or /Game/Data/Items/"),
			*ItemId.ToString());
		return;
	}

	// Add item to inventory
	EInventoryOperationResult Result;
	UInventoryItem* AddedItem = InventoryComp->AddItem(ItemData, Amount, Result);

	if (AddedItem)
	{
		UE_LOG(LogInventoryQuestRewards, Warning, TEXT("? InventoryQuestRewardListener: Successfully added %dx '%s' to inventory (Quest: %s)"),
			Amount, *ItemId.ToString(), *QuestId.ToString());
	}
	else
	{
		UE_LOG(LogInventoryQuestRewards, Warning, TEXT("?? InventoryQuestRewardListener: AddItem returned NULL for '%s' x%d (Result: %d)"),
			*ItemId.ToString(), Amount, (int32)Result);
		UE_LOG(LogInventoryQuestRewards, Warning, TEXT("   Check inventory space, weight limit, or item settings"));
	}
}

UItemDataAsset* UInventoryQuestRewardListener::FindItemDataAssetByID(FName ItemID) const
{
	// Method 1: Try direct path loading (fast)
	TArray<FString> AssetPaths = {
		FString::Printf(TEXT("/Game/Items/ItemData_%s.ItemData_%s"), *ItemID.ToString(), *ItemID.ToString()),
		FString::Printf(TEXT("/Game/Items/%s.%s"), *ItemID.ToString(), *ItemID.ToString()),
		FString::Printf(TEXT("/Game/Data/Items/%s.%s"), *ItemID.ToString(), *ItemID.ToString()),
		FString::Printf(TEXT("/Game/Data/Items/ItemData_%s.ItemData_%s"), *ItemID.ToString(), *ItemID.ToString())
	};

	for (const FString& Path : AssetPaths)
	{
		UItemDataAsset* ItemData = Cast<UItemDataAsset>(StaticLoadObject(UItemDataAsset::StaticClass(), nullptr, *Path));
		if (ItemData)
		{
			UE_LOG(LogInventoryQuestRewards, Log, TEXT("   Loaded ItemDataAsset from path: %s"), *Path);
			return ItemData;
		}
	}

	// Method 2: Search all loaded ItemDataAssets by ItemID property (slower, but more reliable)
	UE_LOG(LogInventoryQuestRewards, Warning, TEXT("   Direct loading failed, searching loaded assets..."));

	for (TObjectIterator<UItemDataAsset> It; It; ++It)
	{
		UItemDataAsset* ItemData = *It;
		if (ItemData && ItemData->IsValidLowLevel() && !ItemData->IsUnreachable() && ItemData->ItemID == ItemID)
		{
			UE_LOG(LogInventoryQuestRewards, Warning, TEXT("   Found ItemDataAsset in memory: %s"), *ItemData->GetPathName());
			return ItemData;
		}
	}

	// Not found
	return nullptr;
}
