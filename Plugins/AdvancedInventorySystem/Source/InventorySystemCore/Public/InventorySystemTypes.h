// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventorySystemTypes.generated.h"

/**
 * Represents a unique identifier for an item instance
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryItemId
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FGuid ItemGuid;

	FInventoryItemId()
		: ItemGuid(FGuid::NewGuid())
	{
	}

	explicit FInventoryItemId(const FGuid& InGuid)
		: ItemGuid(InGuid)
	{
	}

	bool IsValid() const { return ItemGuid.IsValid(); }
	
	bool operator==(const FInventoryItemId& Other) const
	{
		return ItemGuid == Other.ItemGuid;
	}

	bool operator!=(const FInventoryItemId& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FInventoryItemId& ItemId)
	{
		return GetTypeHash(ItemId.ItemGuid);
	}

	FString ToString() const
	{
		return ItemGuid.ToString();
	}
};

/**
 * Stack information for an inventory item
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryStack
{
	GENERATED_BODY()

	/** Current quantity in this stack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "0"))
	int32 Quantity = 1;

	/** Maximum stack size (0 = no limit from this stack's perspective) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "0"))
	int32 MaxStackSize = 0;

	FInventoryStack()
		: Quantity(1)
		, MaxStackSize(0)
	{
	}

	FInventoryStack(int32 InQuantity, int32 InMaxStackSize = 0)
		: Quantity(InQuantity)
		, MaxStackSize(InMaxStackSize)
	{
	}

	bool CanAddQuantity(int32 Amount) const
	{
		if (MaxStackSize <= 0) return true; // No limit
		return (Quantity + Amount) <= MaxStackSize;
	}

	int32 GetRemainingCapacity() const
	{
		if (MaxStackSize <= 0) return MAX_int32; // No limit
		return FMath::Max(0, MaxStackSize - Quantity);
	}

	bool IsFull() const
	{
		if (MaxStackSize <= 0) return false;
		return Quantity >= MaxStackSize;
	}

	bool IsEmpty() const
	{
		return Quantity <= 0;
	}
};

/**
 * Item rarity levels
 */
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common UMETA(DisplayName = "Common"),
	Uncommon UMETA(DisplayName = "Uncommon"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary"),
	Unique UMETA(DisplayName = "Unique")
};

/**
 * Item category types
 */
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Consumable UMETA(DisplayName = "Consumable"),
	Equipment UMETA(DisplayName = "Equipment"),
	QuestItem UMETA(DisplayName = "Quest Item"),
	Crafting UMETA(DisplayName = "Crafting Material"),
	Currency UMETA(DisplayName = "Currency"),
	Readable UMETA(DisplayName = "Readable"),
	Key UMETA(DisplayName = "Key"),
	Miscellaneous UMETA(DisplayName = "Miscellaneous")
};

/**
 * Result codes for inventory operations
 */
UENUM(BlueprintType)
enum class EInventoryOperationResult : uint8
{
	Success UMETA(DisplayName = "Success"),
	Failed_ItemNotFound UMETA(DisplayName = "Item Not Found"),
	Failed_InsufficientSpace UMETA(DisplayName = "Insufficient Space"),
	Failed_InsufficientQuantity UMETA(DisplayName = "Insufficient Quantity"),
	Failed_InvalidItem UMETA(DisplayName = "Invalid Item"),
	Failed_TagRequirementNotMet UMETA(DisplayName = "Tag Requirement Not Met"),
	Failed_CustomCondition UMETA(DisplayName = "Custom Condition Failed"),
	Failed_StackLimitReached UMETA(DisplayName = "Stack Limit Reached"),
	Failed_Unknown UMETA(DisplayName = "Unknown Error")
};

/**
 * Effect trigger types
 */
UENUM(BlueprintType)
enum class EItemEffectTrigger : uint8
{
	OnAcquire UMETA(DisplayName = "On Acquire"),
	OnLose UMETA(DisplayName = "On Lose"),
	OnUse UMETA(DisplayName = "On Use"),
	OnGift UMETA(DisplayName = "On Gift"),
	WhileOwned UMETA(DisplayName = "While Owned"),
	WhileEquipped UMETA(DisplayName = "While Equipped")
};

/**
 * Effect target types
 */
UENUM(BlueprintType)
enum class EItemEffectTarget : uint8
{
	Self UMETA(DisplayName = "Self (Owner)"),
	NPC UMETA(DisplayName = "NPC/Target"),
	CustomSelector UMETA(DisplayName = "Custom Selector")
};

/**
 * Price calculation type
 */
UENUM(BlueprintType)
enum class EPriceCalculationType : uint8
{
	Fixed UMETA(DisplayName = "Fixed Price"),
	BaseWithModifier UMETA(DisplayName = "Base Price with Modifier"),
	DynamicFormula UMETA(DisplayName = "Dynamic Formula")
};
