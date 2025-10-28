// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryComponent.h"
#include "InventoryItem.h"
#include "ItemDataAsset.h"
#include "WorldItemActor.h"
#include "Net/UnrealNetwork.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	MaxInventorySlots = 20;
	MaxWeightCapacity = 0.0f;
	bAutoStack = true;
	bAutoSort = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize inventory with empty slots if needed
	if (MaxInventorySlots > 0)
	{
		InventoryItems.Reserve(MaxInventorySlots);
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryItems);
}

// ========== ADD / REMOVE OPERATIONS ==========

UInventoryItem* UInventoryComponent::AddItem(UItemDataAsset* ItemData, int32 Quantity, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	if (!ItemData || Quantity <= 0)
	{
		OutResult = EInventoryOperationResult::Failed_InvalidItem;
		return nullptr;
	}

	// Check if item tags are allowed
	if (!AreItemTagsAllowed(ItemData->ItemTags))
	{
		OutResult = EInventoryOperationResult::Failed_TagRequirementNotMet;
		return nullptr;
	}

	// Check weight limit
	if (MaxWeightCapacity > 0.0f)
	{
		float ItemTotalWeight = ItemData->ItemWeight * Quantity;
		if (!CanAddWeight(ItemTotalWeight))
		{
			OutResult = EInventoryOperationResult::Failed_InsufficientSpace; // Weight counts as space
			return nullptr;
		}
	}

	// Try to stack with existing items first if auto-stacking is enabled
	if (bAutoStack && ItemData->bIsStackable)
	{
		int32 RemainingQuantity = Quantity;
		UInventoryItem* LastModifiedItem = nullptr;

		// Find all existing items of this type
		TArray<UInventoryItem*> ExistingItems = FindItemsByData(ItemData);
		
		for (UInventoryItem* ExistingItem : ExistingItems)
		{
			if (RemainingQuantity <= 0)
			{
				break;
			}

			int32 Added = ExistingItem->AddQuantity(RemainingQuantity);
			RemainingQuantity -= Added;
			
			if (Added > 0)
			{
				LastModifiedItem = ExistingItem;
				
				// Notify item added
				int32 SlotIndex = InventoryItems.IndexOfByKey(ExistingItem);
				OnItemAdded.Broadcast(ExistingItem, Added, SlotIndex);
			}
		}

		// If all quantity was added to existing stacks, we're done
		if (RemainingQuantity <= 0)
		{
			OutResult = EInventoryOperationResult::Success;
			NotifyInventoryChanged();
			return LastModifiedItem;
		}

		// Update quantity to remaining amount
		Quantity = RemainingQuantity;
	}

	// Check if we have space for new item(s)
	if (MaxInventorySlots > 0 && GetUsedSlots() >= MaxInventorySlots)
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientSpace;
		return nullptr;
	}

	// Create new item(s)
	int32 MaxStack = ItemData->GetEffectiveMaxStackSize();
	UInventoryItem* FirstCreatedItem = nullptr;

	while (Quantity > 0)
	{
		// Check for available slot
		int32 EmptySlot = FindEmptySlot();
		if (EmptySlot == INDEX_NONE && MaxInventorySlots > 0)
		{
			OutResult = EInventoryOperationResult::Failed_InsufficientSpace;
			break;
		}

		// Calculate how many to add to this stack
		int32 StackSize = (MaxStack > 0) ? FMath::Min(Quantity, MaxStack) : Quantity;

		// Create new inventory item
		UInventoryItem* NewItem = UInventoryItem::CreateInventoryItem(this, ItemData, StackSize);
		
		if (NewItem)
		{
			// Add to inventory
			int32 NewIndex = InventoryItems.Add(NewItem);
			ItemIdToIndexMap.Add(NewItem->ItemInstanceId, NewIndex);

			if (!FirstCreatedItem)
			{
				FirstCreatedItem = NewItem;
			}

			// Notify item added
			OnItemAdded.Broadcast(NewItem, StackSize, NewIndex);

			Quantity -= StackSize;
		}
		else
		{
			OutResult = EInventoryOperationResult::Failed_Unknown;
			break;
		}
	}

	if (FirstCreatedItem)
	{
		OutResult = EInventoryOperationResult::Success;
		NotifyInventoryChanged();
	}

	return FirstCreatedItem;
}

bool UInventoryComponent::AddInventoryItem(UInventoryItem* Item, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	if (!Item || !Item->IsValid())
	{
		OutResult = EInventoryOperationResult::Failed_InvalidItem;
		return false;
	}

	// Check if item tags are allowed
	if (Item->ItemData && !AreItemTagsAllowed(Item->ItemData->ItemTags))
	{
		OutResult = EInventoryOperationResult::Failed_TagRequirementNotMet;
		return false;
	}

	// Try to stack with existing items if auto-stacking is enabled
	if (bAutoStack && Item->ItemData && Item->ItemData->bIsStackable)
	{
		TArray<UInventoryItem*> ExistingItems = FindItemsByData(Item->ItemData);
		
		for (UInventoryItem* ExistingItem : ExistingItems)
		{
			if (ExistingItem->CanStackWith(Item))
			{
				int32 Added = ExistingItem->AddQuantity(Item->GetQuantity());
				
				if (Added > 0)
				{
					int32 SlotIndex = InventoryItems.IndexOfByKey(ExistingItem);
					OnItemAdded.Broadcast(ExistingItem, Added, SlotIndex);
					
					// Remove the added quantity from the source item
					Item->RemoveQuantity(Added);
					
					if (Item->GetQuantity() <= 0)
					{
						OutResult = EInventoryOperationResult::Success;
						NotifyInventoryChanged();
						return true;
					}
				}
			}
		}
	}

	// Check if we have space for the item
	if (MaxInventorySlots > 0 && GetUsedSlots() >= MaxInventorySlots)
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientSpace;
		return false;
	}

	// Add item to inventory
	int32 NewIndex = InventoryItems.Add(Item);
	ItemIdToIndexMap.Add(Item->ItemInstanceId, NewIndex);

	// Notify item added
	OnItemAdded.Broadcast(Item, Item->GetQuantity(), NewIndex);

	OutResult = EInventoryOperationResult::Success;
	NotifyInventoryChanged();
	
	return true;
}

int32 UInventoryComponent::RemoveItemById(FInventoryItemId ItemId, int32 Quantity, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	UInventoryItem* Item = GetItemById(ItemId);
	if (!Item)
	{
		OutResult = EInventoryOperationResult::Failed_ItemNotFound;
		return 0;
	}

	int32 QuantityToRemove = (Quantity <= 0) ? Item->GetQuantity() : FMath::Min(Quantity, Item->GetQuantity());
	int32 ActuallyRemoved = Item->RemoveQuantity(QuantityToRemove);

	if (ActuallyRemoved > 0)
	{
		int32 SlotIndex = InventoryItems.IndexOfByKey(Item);
		
		// Notify item removed
		OnItemRemoved.Broadcast(Item, ActuallyRemoved, SlotIndex);

		// If item quantity is now 0, remove it from inventory
		if (Item->GetQuantity() <= 0)
		{
			ItemIdToIndexMap.Remove(ItemId);
			InventoryItems.RemoveAt(SlotIndex);
			RebuildItemIdMap();
		}

		OutResult = EInventoryOperationResult::Success;
		NotifyInventoryChanged();
	}
	else
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientQuantity;
	}

	return ActuallyRemoved;
}

int32 UInventoryComponent::RemoveItemByData(UItemDataAsset* ItemData, int32 Quantity, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	if (!ItemData || Quantity <= 0)
	{
		OutResult = EInventoryOperationResult::Failed_InvalidItem;
		return 0;
	}

	TArray<UInventoryItem*> Items = FindItemsByData(ItemData);
	if (Items.Num() == 0)
	{
		OutResult = EInventoryOperationResult::Failed_ItemNotFound;
		return 0;
	}

	int32 RemainingToRemove = Quantity;
	int32 TotalRemoved = 0;

	for (UInventoryItem* Item : Items)
	{
		if (RemainingToRemove <= 0)
		{
			break;
		}

		int32 ToRemoveFromThisStack = FMath::Min(RemainingToRemove, Item->GetQuantity());
		
		EInventoryOperationResult RemoveResult;
		int32 Removed = RemoveItemById(Item->ItemInstanceId, ToRemoveFromThisStack, RemoveResult);
		
		TotalRemoved += Removed;
		RemainingToRemove -= Removed;
	}

	if (TotalRemoved > 0)
	{
		OutResult = EInventoryOperationResult::Success;
	}
	else
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientQuantity;
	}

	return TotalRemoved;
}

void UInventoryComponent::ClearInventory()
{
	InventoryItems.Empty();
	ItemIdToIndexMap.Empty();
	NotifyInventoryChanged();
}

// ========== QUERIES ==========

TArray<UInventoryItem*> UInventoryComponent::GetAllItems() const
{
	TArray<UInventoryItem*> Result;
	
	for (UInventoryItem* Item : InventoryItems)
	{
		if (Item && Item->IsValid())
		{
			Result.Add(Item);
		}
	}

	return Result;
}

UInventoryItem* UInventoryComponent::GetItemById(FInventoryItemId ItemId) const
{
	const int32* IndexPtr = ItemIdToIndexMap.Find(ItemId);
	
	if (IndexPtr && InventoryItems.IsValidIndex(*IndexPtr))
	{
		return InventoryItems[*IndexPtr];
	}

	return nullptr;
}

UInventoryItem* UInventoryComponent::GetItemAtSlot(int32 SlotIndex) const
{
	if (InventoryItems.IsValidIndex(SlotIndex))
	{
		return InventoryItems[SlotIndex];
	}

	return nullptr;
}

TArray<UInventoryItem*> UInventoryComponent::FindItemsByData(UItemDataAsset* ItemData) const
{
	TArray<UInventoryItem*> Result;

	if (!ItemData)
	{
		return Result;
	}

	for (UInventoryItem* Item : InventoryItems)
	{
		if (Item && Item->ItemData == ItemData)
		{
			Result.Add(Item);
		}
	}

	return Result;
}

TArray<UInventoryItem*> UInventoryComponent::FindItemsByTags(FGameplayTagContainer Tags, bool bRequireAll) const
{
	TArray<UInventoryItem*> Result;

	for (UInventoryItem* Item : InventoryItems)
	{
		if (!Item || !Item->ItemData)
		{
			continue;
		}

		bool bMatches = false;

		if (bRequireAll)
		{
			bMatches = Item->ItemData->ItemTags.HasAll(Tags);
		}
		else
		{
			bMatches = Item->ItemData->ItemTags.HasAny(Tags);
		}

		if (bMatches)
		{
			Result.Add(Item);
		}
	}

	return Result;
}

bool UInventoryComponent::HasItem(UItemDataAsset* ItemData, int32 Quantity) const
{
	return GetItemQuantity(ItemData) >= Quantity;
}

int32 UInventoryComponent::GetItemQuantity(UItemDataAsset* ItemData) const
{
	if (!ItemData)
	{
		return 0;
	}

	int32 TotalQuantity = 0;

	TArray<UInventoryItem*> Items = FindItemsByData(ItemData);
	for (UInventoryItem* Item : Items)
	{
		TotalQuantity += Item->GetQuantity();
	}

	return TotalQuantity;
}

int32 UInventoryComponent::GetUsedSlots() const
{
	return InventoryItems.Num();
}

int32 UInventoryComponent::GetFreeSlots() const
{
	if (MaxInventorySlots <= 0)
	{
		return MAX_int32; // Unlimited
	}

	return FMath::Max(0, MaxInventorySlots - GetUsedSlots());
}

bool UInventoryComponent::IsInventoryFull() const
{
	if (MaxInventorySlots <= 0)
	{
		return false; // Unlimited inventory
	}

	return GetUsedSlots() >= MaxInventorySlots;
}

bool UInventoryComponent::IsInventoryEmpty() const
{
	return InventoryItems.Num() == 0;
}

// ========== USAGE ==========

bool UInventoryComponent::UseItem(FInventoryItemId ItemId, AActor* User, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	UInventoryItem* Item = GetItemById(ItemId);
	if (!Item || !Item->ItemData)
	{
		OutResult = EInventoryOperationResult::Failed_ItemNotFound;
		OnItemUsed.Broadcast(Item, User, false);
		return false;
	}

	if (!Item->CanBeUsed(User))
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		OnItemUsed.Broadcast(Item, User, false);
		return false;
	}

	// Update last used time
	Item->UpdateLastUsedTime(GetWorld());

	// Consume item if needed
	if (Item->ItemData->bConsumeOnUse)
	{
		EInventoryOperationResult RemoveResult;
		RemoveItemById(ItemId, 1, RemoveResult);
	}

	// Broadcast success
	OutResult = EInventoryOperationResult::Success;
	OnItemUsed.Broadcast(Item, User, true);
	
	return true;
}

bool UInventoryComponent::GiftItem(FInventoryItemId ItemId, AActor* Receiver, int32 Quantity, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	UInventoryItem* Item = GetItemById(ItemId);
	if (!Item || !Item->ItemData)
	{
		OutResult = EInventoryOperationResult::Failed_ItemNotFound;
		OnItemGifted.Broadcast(Item, GetOwner(), Receiver, false);
		return false;
	}

	if (!Item->ItemData->bCanBeGifted)
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		OnItemGifted.Broadcast(Item, GetOwner(), Receiver, false);
		return false;
	}

	if (!Receiver)
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		OnItemGifted.Broadcast(Item, GetOwner(), Receiver, false);
		return false;
	}

	// Get receiver's inventory component
	UInventoryComponent* ReceiverInventory = Cast<UInventoryComponent>(
		Receiver->GetComponentByClass(UInventoryComponent::StaticClass())
	);

	if (!ReceiverInventory)
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		OnItemGifted.Broadcast(Item, GetOwner(), Receiver, false);
		return false;
	}

	// Check quantity
	int32 QuantityToGift = FMath::Min(Quantity, Item->GetQuantity());
	if (QuantityToGift <= 0)
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientQuantity;
		OnItemGifted.Broadcast(Item, GetOwner(), Receiver, false);
		return false;
	}

	// Try to add to receiver's inventory
	EInventoryOperationResult AddResult;
	UInventoryItem* GiftedItem = ReceiverInventory->AddItem(Item->ItemData, QuantityToGift, AddResult);

	if (AddResult == EInventoryOperationResult::Success && GiftedItem)
	{
		// Remove from this inventory
		EInventoryOperationResult RemoveResult;
		RemoveItemById(ItemId, QuantityToGift, RemoveResult);

		OutResult = EInventoryOperationResult::Success;
		OnItemGifted.Broadcast(Item, GetOwner(), Receiver, true);
		return true;
	}

	OutResult = AddResult;
	OnItemGifted.Broadcast(Item, GetOwner(), Receiver, false);
	return false;
}

AWorldItemActor* UInventoryComponent::DropItem(FInventoryItemId ItemId, int32 Quantity, FVector DropLocation, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	UInventoryItem* Item = GetItemById(ItemId);
	if (!Item || !Item->ItemData)
	{
		OutResult = EInventoryOperationResult::Failed_ItemNotFound;
		return nullptr;
	}

	// Check if item can be dropped
	if (!Item->ItemData->PhysicalRepresentation.bCanBeDropped)
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		return nullptr;
	}

	// Determine quantity to drop
	int32 QuantityToDrop = (Quantity <= 0) ? Item->GetQuantity() : FMath::Min(Quantity, Item->GetQuantity());
	
	if (QuantityToDrop <= 0)
	{
		OutResult = EInventoryOperationResult::Failed_InsufficientQuantity;
		return nullptr;
	}

	// Get world
	UWorld* World = GetWorld();
	if (!World)
	{
		OutResult = EInventoryOperationResult::Failed_Unknown;
		return nullptr;
	}

	// Spawn world item actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AWorldItemActor* WorldItem = World->SpawnActor<AWorldItemActor>(
		AWorldItemActor::StaticClass(),
		DropLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (!WorldItem)
	{
		OutResult = EInventoryOperationResult::Failed_Unknown;
		return nullptr;
	}

	// Initialize the world item with our item data
	WorldItem->InitializeItem(Item->ItemData, QuantityToDrop);
	
	// Transfer instance data
	WorldItem->InstanceTags = Item->InstanceTags;
	WorldItem->CustomMetadata = Item->CustomMetadata;

	// Remove from inventory
	int32 Removed = RemoveItemById(ItemId, QuantityToDrop, OutResult);
	
	if (OutResult == EInventoryOperationResult::Success && Removed == QuantityToDrop)
	{
		return WorldItem;
	}

	// If removal failed, destroy the spawned actor and return failure
	WorldItem->Destroy();
	OutResult = EInventoryOperationResult::Failed_Unknown;
	return nullptr;
}

AWorldItemActor* UInventoryComponent::DropItemInFront(FInventoryItemId ItemId, int32 Quantity, float DropDistance, EInventoryOperationResult& OutResult)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		OutResult = EInventoryOperationResult::Failed_Unknown;
		return nullptr;
	}

	// Calculate drop location in front of owner
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector OwnerForward = Owner->GetActorForwardVector();
	FVector DropLocation = OwnerLocation + (OwnerForward * DropDistance);

	// Trace down to find ground
	UWorld* World = GetWorld();
	if (World)
	{
		FHitResult HitResult;
		FVector TraceStart = DropLocation + FVector(0.0f, 0.0f, 100.0f);
		FVector TraceEnd = DropLocation - FVector(0.0f, 0.0f, 1000.0f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);

		if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			DropLocation = HitResult.Location;
		}
	}

	return DropItem(ItemId, Quantity, DropLocation, OutResult);
}

// ========== EQUIPMENT ==========

bool UInventoryComponent::EquipItem(FInventoryItemId ItemId, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	UInventoryItem* Item = GetItemById(ItemId);
	if (!Item || !Item->ItemData)
	{
		OutResult = EInventoryOperationResult::Failed_ItemNotFound;
		return false;
	}

	if (!Item->ItemData->bIsEquippable)
	{
		OutResult = EInventoryOperationResult::Failed_CustomCondition;
		return false;
	}

	if (Item->bIsEquipped)
	{
		OutResult = EInventoryOperationResult::Success; // Already equipped
		return true;
	}

	// Check if there's already an item equipped in this slot
	if (Item->ItemData->EquipmentSlot.IsValid())
	{
		UInventoryItem* CurrentEquippedItem = GetEquippedItemInSlot(Item->ItemData->EquipmentSlot);
		if (CurrentEquippedItem && CurrentEquippedItem != Item)
		{
			// Unequip the current item
			EInventoryOperationResult UnequipResult;
			UnequipItem(CurrentEquippedItem->ItemInstanceId, UnequipResult);
		}
	}

	// Equip the item
	Item->SetEquipped(true);

	OutResult = EInventoryOperationResult::Success;
	OnItemEquipped.Broadcast(Item, true);
	NotifyInventoryChanged();

	return true;
}

bool UInventoryComponent::UnequipItem(FInventoryItemId ItemId, EInventoryOperationResult& OutResult)
{
	OutResult = EInventoryOperationResult::Failed_Unknown;

	UInventoryItem* Item = GetItemById(ItemId);
	if (!Item)
	{
		OutResult = EInventoryOperationResult::Failed_ItemNotFound;
		return false;
	}

	if (!Item->bIsEquipped)
	{
		OutResult = EInventoryOperationResult::Success; // Already unequipped
		return true;
	}

	// Unequip the item
	Item->SetEquipped(false);

	OutResult = EInventoryOperationResult::Success;
	OnItemEquipped.Broadcast(Item, false);
	NotifyInventoryChanged();

	return true;
}

TArray<UInventoryItem*> UInventoryComponent::GetEquippedItems() const
{
	TArray<UInventoryItem*> Result;

	for (UInventoryItem* Item : InventoryItems)
	{
		if (Item && Item->bIsEquipped)
		{
			Result.Add(Item);
		}
	}

	return Result;
}

UInventoryItem* UInventoryComponent::GetEquippedItemInSlot(FGameplayTag SlotTag) const
{
	if (!SlotTag.IsValid())
	{
		return nullptr;
	}

	for (UInventoryItem* Item : InventoryItems)
	{
		if (Item && Item->bIsEquipped && Item->ItemData)
		{
			if (Item->ItemData->EquipmentSlot.MatchesTagExact(SlotTag))
			{
				return Item;
			}
		}
	}

	return nullptr;
}

// ========== ORGANIZATION ==========

bool UInventoryComponent::SwapItems(int32 SlotIndexA, int32 SlotIndexB)
{
	if (!InventoryItems.IsValidIndex(SlotIndexA) || !InventoryItems.IsValidIndex(SlotIndexB))
	{
		return false;
	}

	if (SlotIndexA == SlotIndexB)
	{
		return true; // No swap needed
	}

	// Swap the items
	InventoryItems.Swap(SlotIndexA, SlotIndexB);

	// Rebuild index map
	RebuildItemIdMap();

	NotifyInventoryChanged();

	return true;
}

void UInventoryComponent::SortInventory()
{
	// Sort by: Category, Rarity, Name
	InventoryItems.Sort([](const UInventoryItem& A, const UInventoryItem& B)
	{
		if (!A.ItemData || !B.ItemData)
		{
			return false;
		}

		// First by category
		if (A.ItemData->Category != B.ItemData->Category)
		{
			return (uint8)A.ItemData->Category < (uint8)B.ItemData->Category;
		}

		// Then by rarity
		if (A.ItemData->Rarity != B.ItemData->Rarity)
		{
			return (uint8)A.ItemData->Rarity > (uint8)B.ItemData->Rarity; // Higher rarity first
		}

		// Finally by name
		return A.ItemData->ItemName.CompareTo(B.ItemData->ItemName) < 0;
	});

	// Rebuild index map
	RebuildItemIdMap();

	NotifyInventoryChanged();
}

// ========== VALIDATION ==========

bool UInventoryComponent::CanAddItem(UItemDataAsset* ItemData, int32 Quantity) const
{
	if (!ItemData || Quantity <= 0)
	{
		return false;
	}

	// Check tags
	if (!AreItemTagsAllowed(ItemData->ItemTags))
	{
		return false;
	}

	// Check weight limit
	if (MaxWeightCapacity > 0.0f)
	{
		float ItemTotalWeight = ItemData->ItemWeight * Quantity;
		if (!CanAddWeight(ItemTotalWeight))
		{
			return false;
		}
	}

	// If stackable, check if we can add to existing stacks
	if (bAutoStack && ItemData->bIsStackable)
	{
		TArray<UInventoryItem*> ExistingItems = FindItemsByData(ItemData);
		int32 AvailableStackSpace = 0;

		for (UInventoryItem* Item : ExistingItems)
		{
			AvailableStackSpace += Item->GetRemainingCapacity();
		}

		if (AvailableStackSpace >= Quantity)
		{
			return true;
		}

		Quantity -= AvailableStackSpace;
	}

	// Check if we have enough free slots for remaining quantity
	int32 FreeSlots = GetFreeSlots();
	if (FreeSlots == MAX_int32)
	{
		return true; // Unlimited
	}

	int32 MaxStack = ItemData->GetEffectiveMaxStackSize();
	int32 SlotsNeeded = (MaxStack > 0) ? FMath::CeilToInt((float)Quantity / MaxStack) : 1;

	return FreeSlots >= SlotsNeeded;
}

// ========== SAVE/LOAD ==========

bool UInventoryComponent::SaveToJson_Implementation(FString& OutJsonString)
{
	TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();

	// Save configuration
	RootObject->SetNumberField(TEXT("MaxSlots"), MaxInventorySlots);
	RootObject->SetNumberField(TEXT("MaxWeight"), MaxWeightCapacity);
	RootObject->SetBoolField(TEXT("AutoStack"), bAutoStack);
	RootObject->SetBoolField(TEXT("AutoSort"), bAutoSort);

	// Save items
	TArray<TSharedPtr<FJsonValue>> ItemsArray;
	for (UInventoryItem* Item : InventoryItems)
	{
		if (Item && Item->IsValid())
		{
			FString ItemJson = Item->ToJson();
			
			TSharedPtr<FJsonObject> ItemObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ItemJson);
			
			if (FJsonSerializer::Deserialize(Reader, ItemObject) && ItemObject.IsValid())
			{
				ItemsArray.Add(MakeShared<FJsonValueObject>(ItemObject));
			}
		}
	}
	RootObject->SetArrayField(TEXT("Items"), ItemsArray);

	// Serialize to string
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJsonString);
	return FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
}

bool UInventoryComponent::LoadFromJson_Implementation(const FString& JsonString)
{
	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		return false;
	}

	// Clear existing inventory
	ClearInventory();

	// Load configuration
	RootObject->TryGetNumberField(TEXT("MaxSlots"), MaxInventorySlots);
	RootObject->TryGetNumberField(TEXT("MaxWeight"), MaxWeightCapacity);
	RootObject->TryGetBoolField(TEXT("AutoStack"), bAutoStack);
	RootObject->TryGetBoolField(TEXT("AutoSort"), bAutoSort);

	// Load items
	const TArray<TSharedPtr<FJsonValue>>* ItemsArray;
	if (RootObject->TryGetArrayField(TEXT("Items"), ItemsArray))
	{
		for (const TSharedPtr<FJsonValue>& ItemValue : *ItemsArray)
		{
			TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();
			if (!ItemObject.IsValid())
			{
				continue;
			}

			// Convert back to JSON string for item deserialization
			FString ItemJson;
			TSharedRef<TJsonWriter<>> ItemWriter = TJsonWriterFactory<>::Create(&ItemJson);
			FJsonSerializer::Serialize(ItemObject.ToSharedRef(), ItemWriter);

			// Create item and deserialize
			UInventoryItem* NewItem = NewObject<UInventoryItem>(this);
			if (NewItem && NewItem->FromJson(ItemJson))
			{
				// Note: ItemData needs to be loaded from asset manager
				// This is a simplified version
				if (NewItem->IsValid())
				{
					int32 NewIndex = InventoryItems.Add(NewItem);
					ItemIdToIndexMap.Add(NewItem->ItemInstanceId, NewIndex);
				}
			}
		}
	}

	NotifyInventoryChanged();

	return true;
}

// ========== INTERNAL HELPERS ==========

int32 UInventoryComponent::FindEmptySlot() const
{
	if (MaxInventorySlots <= 0)
	{
		return InventoryItems.Num(); // Unlimited, return next available index
	}

	if (InventoryItems.Num() < MaxInventorySlots)
	{
		return InventoryItems.Num(); // Next available slot
	}

	return INDEX_NONE; // No empty slots
}

int32 UInventoryComponent::FindStackableSlot(UItemDataAsset* ItemData) const
{
	if (!ItemData || !ItemData->bIsStackable)
	{
		return INDEX_NONE;
	}

	for (int32 i = 0; i < InventoryItems.Num(); ++i)
	{
		UInventoryItem* Item = InventoryItems[i];
		if (Item && Item->ItemData == ItemData && !Item->IsStackFull())
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void UInventoryComponent::RebuildItemIdMap()
{
	ItemIdToIndexMap.Empty();

	for (int32 i = 0; i < InventoryItems.Num(); ++i)
	{
		UInventoryItem* Item = InventoryItems[i];
		if (Item)
		{
			ItemIdToIndexMap.Add(Item->ItemInstanceId, i);
		}
	}
}

bool UInventoryComponent::AreItemTagsAllowed(const FGameplayTagContainer& ItemTags) const
{
	// Check blocked tags
	if (BlockedItemTags.Num() > 0 && ItemTags.HasAny(BlockedItemTags))
	{
		return false;
	}

	// Check allowed tags (if any are specified, item must have at least one)
	if (AllowedItemTags.Num() > 0 && !ItemTags.HasAny(AllowedItemTags))
	{
		return false;
	}

	return true;
}

void UInventoryComponent::NotifyInventoryChanged()
{
	OnInventoryChanged.Broadcast();
	
	// Also broadcast weight change
	if (MaxWeightCapacity > 0.0f)
	{
		OnWeightChanged.Broadcast(GetTotalWeight(), MaxWeightCapacity);
	}
}

// ========== WEIGHT SYSTEM ==========

float UInventoryComponent::GetTotalWeight() const
{
	float TotalWeight = 0.0f;

	for (const UInventoryItem* Item : InventoryItems)
	{
		if (Item && Item->ItemData)
		{
			TotalWeight += Item->ItemData->ItemWeight * Item->GetQuantity();
		}
	}

	return TotalWeight;
}

float UInventoryComponent::GetRemainingWeightCapacity() const
{
	if (MaxWeightCapacity <= 0.0f)
	{
		return FLT_MAX; // Unlimited
	}

	return FMath::Max(0.0f, MaxWeightCapacity - GetTotalWeight());
}

float UInventoryComponent::GetWeightPercentage() const
{
	if (MaxWeightCapacity <= 0.0f)
	{
		return 0.0f; // Unlimited weight = 0% usage
	}

	float CurrentWeight = GetTotalWeight();
	return FMath::Clamp(CurrentWeight / MaxWeightCapacity, 0.0f, 1.0f);
}

bool UInventoryComponent::CanAddWeight(float AdditionalWeight) const
{
	if (MaxWeightCapacity <= 0.0f)
	{
		return true; // Unlimited weight
	}

	float CurrentWeight = GetTotalWeight();
	return (CurrentWeight + AdditionalWeight) <= MaxWeightCapacity;
}

bool UInventoryComponent::IsOverweight() const
{
	if (MaxWeightCapacity <= 0.0f)
	{
		return false; // Unlimited weight
	}

	return GetTotalWeight() > MaxWeightCapacity;
}

float UInventoryComponent::GetItemWeight(UItemDataAsset* ItemData, int32 Quantity)
{
	if (!ItemData || Quantity <= 0)
	{
		return 0.0f;
	}

	return ItemData->ItemWeight * Quantity;
}
