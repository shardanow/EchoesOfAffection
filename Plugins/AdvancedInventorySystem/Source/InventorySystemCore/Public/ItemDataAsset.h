// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InventorySystemTypes.h"
#include "ItemDataAsset.generated.h"

class UTexture2D;
class UStaticMesh;
class USkeletalMesh;
class UItemEffectDataAsset;

/**
 * Physical representation configuration for items
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FItemPhysicalRepresentation
{
	GENERATED_BODY()

	/** Static mesh to use for the physical representation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	TObjectPtr<UStaticMesh> StaticMesh;

	/** Skeletal mesh to use for the physical representation (alternative to static mesh) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	/** Actor class to spawn as physical representation (highest priority) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	TSubclassOf<AActor> ActorClass;

	/** Relative location offset for the mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	FVector LocationOffset = FVector::ZeroVector;

	/** Relative rotation offset for the mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	FRotator RotationOffset = FRotator::ZeroRotator;

	/** Scale for the mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	FVector Scale = FVector::OneVector;

	/** Whether this item can be picked up from the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	bool bCanBePickedUp = true;

	/** Whether this item can be dropped into the world */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	bool bCanBeDropped = true;

	/** Custom pickup interaction text */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation")
	FText PickupInteractionText = FText::FromString("Pick up");

	/** Interaction radius for pickup detection */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation|Interaction", meta = (ClampMin = "0"))
	float InteractionRadius = 150.0f;

	/** Interaction duration in seconds (0 = instant pickup) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation|Interaction", meta = (ClampMin = "0"))
	float InteractionDuration = 0.0f;

	/** Interaction priority (higher = picked first when multiple items overlap) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physical Representation|Interaction")
	int32 InteractionPriority = 0;

	bool HasPhysicalRepresentation() const
	{
		return ActorClass != nullptr || StaticMesh != nullptr || SkeletalMesh != nullptr;
	}
};

/**
 * Primary data asset defining an item's properties
 */
UCLASS(BlueprintType)
class INVENTORYSYSTEMCORE_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UItemDataAsset();

	//~ Begin UPrimaryDataAsset Interface
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	//~ End UPrimaryDataAsset Interface

	// ========== BASIC INFORMATION ==========

	/** Unique identifier for this item type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Basic")
	FName ItemID;

	/** Display name of the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Basic")
	FText ItemName;

	/** Detailed description of the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Basic", meta = (MultiLine = true))
	FText ItemDescription;

	/** Icon for UI display */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Basic")
	TObjectPtr<UTexture2D> ItemIcon;

	/** Item rarity level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Basic")
	EItemRarity Rarity = EItemRarity::Common;

	/** Item category */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Basic")
	EItemCategory Category = EItemCategory::Miscellaneous;

	/** Gameplay tags associated with this item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Basic")
	FGameplayTagContainer ItemTags;

	// ========== STACKING ==========

	/** Maximum stack size for this item (0 = infinite stacking) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Stacking", meta = (ClampMin = "0"))
	int32 MaxStackSize = 1;

	/** Whether this item can be stacked with other instances */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Stacking")
	bool bIsStackable = false;

	/** Weight of a single item unit (used for inventory weight limit) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Stacking", meta = (ClampMin = "0"))
	float ItemWeight = 0.0f;

	// ========== PHYSICAL REPRESENTATION ==========

	/** Configuration for physical world representation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Physical")
	FItemPhysicalRepresentation PhysicalRepresentation;

	// ========== USAGE ==========

	/** Whether this item can be used/consumed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Usage")
	bool bIsUsable = false;

	/** Whether the item is consumed when used */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Usage")
	bool bConsumeOnUse = false;

	/** Cooldown in seconds between uses */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Usage", meta = (ClampMin = "0"))
	float UseCooldown = 0.0f;

	/** Custom use action text */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Usage")
	FText UseActionText = FText::FromString("Use");

	// ========== EQUIPMENT ==========

	/** Whether this item can be equipped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment")
	bool bIsEquippable = false;

	/** Equipment slot tag (if equippable) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Equipment", meta = (EditCondition = "bIsEquippable"))
	FGameplayTag EquipmentSlot;

	// ========== QUEST / READABLE ==========

	/** Whether this is a quest item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Quest")
	bool bIsQuestItem = false;

	/** Associated quest tags */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Quest", meta = (EditCondition = "bIsQuestItem"))
	FGameplayTagContainer QuestTags;

	/** Whether this item can be read (notes, books, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Readable")
	bool bIsReadable = false;

	/** Text content for readable items */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Readable", meta = (EditCondition = "bIsReadable", MultiLine = true))
	FText ReadableContent;

	// ========== TRADING ==========

	/** Base value/price of the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Trading", meta = (ClampMin = "0"))
	int32 BaseValue = 0;

	/** Whether this item can be sold */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Trading")
	bool bCanBeSold = true;

	/** Whether this item can be bought from shops */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Trading")
	bool bCanBeBought = true;

	/** Whether this item can be gifted to NPCs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Trading")
	bool bCanBeGifted = true;

	/** Sellback percentage (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Trading", meta = (ClampMin = "0", ClampMax = "1"))
	float SellbackPercentage = 0.5f;

	/** Whether this item is currency */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Trading")
	bool bIsCurrency = false;

	// ========== EFFECTS ==========

	/** Item effects that trigger on various events */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Effects")
	TArray<TObjectPtr<UItemEffectDataAsset>> ItemEffects;

	// ========== REQUIREMENTS ==========

	/** Required tags to use/equip this item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Requirements")
	FGameplayTagContainer RequiredTags;

	/** Blocked tags that prevent use/equip */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Requirements")
	FGameplayTagContainer BlockedTags;

	/** Minimum level required to use this item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Requirements", meta = (ClampMin = "0"))
	int32 MinimumLevel = 0;

	// ========== HELPER FUNCTIONS ==========

	/** Check if this item can stack with another */
	UFUNCTION(BlueprintPure, Category = "Item")
	bool CanStackWith(const UItemDataAsset* OtherItem) const;

	/** Get the effective stack size considering the max stack size */
	UFUNCTION(BlueprintPure, Category = "Item")
	int32 GetEffectiveMaxStackSize() const;

	/** Check if actor meets requirements to use this item */
	UFUNCTION(BlueprintPure, Category = "Item")
	bool MeetsRequirements(AActor* Actor) const;

	/** Get all effects of a specific trigger type */
	UFUNCTION(BlueprintPure, Category = "Item")
	TArray<UItemEffectDataAsset*> GetEffectsByTrigger(EItemEffectTrigger TriggerType) const;
};
