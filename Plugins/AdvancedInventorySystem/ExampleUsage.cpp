/**
 * EXAMPLE USAGE GUIDE
 * Advanced Inventory System
 * 
 * This file demonstrates how to integrate and use the Advanced Inventory System
 * in your Unreal Engine project.
 */

// ============================================================================
// EXAMPLE 1: Adding Inventory to a Character
// ============================================================================

// MyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventorySystemInterfaces.h" // For interfaces
#include "MyCharacter.generated.h"

class UInventoryComponent;
class UItemEffectExecutorComponent;
class UInventoryItem;

UCLASS()
class MYPROJECT_API AMyCharacter : public ACharacter, public IInventoryHolder, public INeedsSystemInterface
{
	GENERATED_BODY()

public:
	AMyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UItemEffectExecutorComponent* EffectExecutor;

	// IInventoryHolder interface
	virtual UActorComponent* GetInventoryComponent_Implementation() override;

	// INeedsSystemInterface
	virtual void ModifyNeed_Implementation(const FGameplayTag& NeedTag, float DeltaValue, bool bClamp) override;
	virtual float GetNeedValue_Implementation(const FGameplayTag& NeedTag) const override;

	// Event handlers
	UFUNCTION()
	void OnInventoryItemAdded(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);

	UFUNCTION()
	void OnInventoryItemUsed(UInventoryItem* Item, AActor* User, bool bSuccess);

protected:
	// Needs values (example)
	UPROPERTY()
	TMap<FGameplayTag, float> NeedsMap;
};

// MyCharacter.cpp
#include "MyCharacter.h"
#include "InventoryComponent.h"
#include "ItemEffectExecutorComponent.h"
#include "InventoryItem.h"

AMyCharacter::AMyCharacter()
{
	// Create inventory component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	InventoryComponent->MaxInventorySlots = 20;
	InventoryComponent->bAutoStack = true;

	// Create effect executor
	EffectExecutor = CreateDefaultSubobject<UItemEffectExecutorComponent>(TEXT("EffectExecutor"));

	// Initialize needs
	NeedsMap.Add(FGameplayTag::RequestGameplayTag(FName("Needs.Health")), 100.0f);
	NeedsMap.Add(FGameplayTag::RequestGameplayTag(FName("Needs.Hunger")), 100.0f);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind to inventory events
	InventoryComponent->OnItemAdded.AddDynamic(this, &AMyCharacter::OnInventoryItemAdded);
	InventoryComponent->OnItemUsed.AddDynamic(this, &AMyCharacter::OnInventoryItemUsed);
}

UActorComponent* AMyCharacter::GetInventoryComponent_Implementation()
{
	return InventoryComponent;
}

void AMyCharacter::ModifyNeed_Implementation(const FGameplayTag& NeedTag, float DeltaValue, bool bClamp)
{
	if (float* NeedValue = NeedsMap.Find(NeedTag))
	{
		*NeedValue += DeltaValue;

		if (bClamp)
		{
			*NeedValue = FMath::Clamp(*NeedValue, 0.0f, 100.0f);
		}

		UE_LOG(LogTemp, Log, TEXT("Modified need %s by %.2f, new value: %.2f"), 
			*NeedTag.ToString(), DeltaValue, *NeedValue);
	}
}

float AMyCharacter::GetNeedValue_Implementation(const FGameplayTag& NeedTag) const
{
	if (const float* NeedValue = NeedsMap.Find(NeedTag))
	{
		return *NeedValue;
	}
	return 0.0f;
}

void AMyCharacter::OnInventoryItemAdded(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
	UE_LOG(LogTemp, Log, TEXT("Item added: %s x%d at slot %d"), 
		*Item->GetDisplayName().ToString(), Quantity, SlotIndex);

	// Execute OnAcquire effects
	if (EffectExecutor)
	{
		EffectExecutor->ExecuteItemEffects(Item, EItemEffectTrigger::OnAcquire, this);
	}
}

void AMyCharacter::OnInventoryItemUsed(UInventoryItem* Item, AActor* User, bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Item used: %s"), *Item->GetDisplayName().ToString());

		// Execute OnUse effects
		if (EffectExecutor)
		{
			EffectExecutor->ExecuteItemEffects(Item, EItemEffectTrigger::OnUse, this);
		}
	}
}

// ============================================================================
// EXAMPLE 2: Creating Item Data Assets (Blueprint or C++)
// ============================================================================

/*
In Blueprint:
1. Right-click Content Browser ? Miscellaneous ? Data Asset
2. Choose UItemDataAsset
3. Name it "DA_HealthPotion"
4. Configure properties:
   - ItemID: "HealthPotion"
   - ItemName: "Health Potion"
   - ItemDescription: "Restores 50 health"
   - Rarity: Common
   - Category: Consumable
   - bIsStackable: true
   - MaxStackSize: 20
   - bIsUsable: true
   - bConsumeOnUse: true
   - BaseValue: 25

5. Create Effect Data Asset:
   - Right-click ? Miscellaneous ? Data Asset ? UItemEffectDataAsset
   - Name it "Effect_RestoreHealth"
   - Configure:
     * TriggerType: OnUse
     * TargetType: Self
     * NeedsModifications:
       - NeedTag: "Needs.Health"
       - ModificationAmount: 50.0
       - bClampValue: true

6. Add effect to item:
   - Open DA_HealthPotion
   - Add Effect_RestoreHealth to ItemEffects array
*/

// ============================================================================
// EXAMPLE 3: Using Inventory in Blueprint
// ============================================================================

/*
// Add item to inventory
Get Inventory Component ? Add Item
- Item Data: DA_HealthPotion
- Quantity: 5

// Use item
Get Inventory Component ? Get Item At Slot (SlotIndex: 0)
? Use Item (Item ID: ReturnValue.ItemInstanceId, User: Self)

// Check if has item
Get Inventory Component ? Has Item
- Item Data: DA_HealthPotion
- Quantity: 1

// Get item quantity
Get Inventory Component ? Get Item Quantity
- Item Data: DA_HealthPotion

// Equip item
Get Inventory Component ? Equip Item
- Item ID: (from item)

// Listen to events
Event BeginPlay ? Get Inventory Component
? Bind Event to On Item Added
? In event: Print String (Item.DisplayName + " added!")
*/

// ============================================================================
// EXAMPLE 4: World Item Spawning and Pickup
// ============================================================================

// Spawn a world item
void AMyGameMode::SpawnLoot(FVector Location)
{
	// Load item data
	UItemDataAsset* ItemData = LoadObject<UItemDataAsset>(
		nullptr, 
		TEXT("/Game/Items/DA_HealthPotion")
	);

	if (ItemData)
	{
		// Spawn world item
		FActorSpawnParameters SpawnParams;
		AWorldItemActor* WorldItem = GetWorld()->SpawnActor<AWorldItemActor>(
			AWorldItemActor::StaticClass(),
			Location,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (WorldItem)
		{
			WorldItem->InitializeItem(ItemData, 3); // 3 potions
		}
	}
}

// Pickup interaction (in player controller or character)
void AMyCharacter::InteractWithWorldItem(AWorldItemActor* WorldItem)
{
	if (WorldItem && WorldItem->CanBePickedUpBy(this))
	{
		WorldItem->PickupItem(this);
	}
}

// ============================================================================
// EXAMPLE 5: Shop System
// ============================================================================

// MyMerchant.h
#pragma once

#include "GameFramework/Actor.h"
#include "MyMerchant.generated.h"

class UShopComponent;

UCLASS()
class AMyMerchant : public AActor
{
	GENERATED_BODY()

public:
	AMyMerchant();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UShopComponent* ShopComponent;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void OpenShop(AActor* Customer);
};

// MyMerchant.cpp
#include "MyMerchant.h"
#include "ShopComponent.h"

AMyMerchant::AMyMerchant()
{
	ShopComponent = CreateDefaultSubobject<UShopComponent>(TEXT("Shop"));
	ShopComponent->bIsOpen = true;
}

void AMyMerchant::OpenShop(AActor* Customer)
{
	// Show shop UI
	// UI will interact with ShopComponent
}

// ============================================================================
// EXAMPLE 6: Creating Shop Data Asset
// ============================================================================

/*
In Blueprint:
1. Right-click ? Miscellaneous ? Data Asset ? UShopDataAsset
2. Name it "DA_GeneralStore"
3. Configure:
   - ShopName: "General Store"
   - ShopDescription: "Buy and sell common items"
   - CurrencyItem: DA_GoldCoin
   - PriceModifier: 1.0
   - SellbackPercentage: 0.5
   - bBuysItems: true

4. Add shop inventory entries:
   Entry[0]:
   - Item: DA_HealthPotion
   - PurchasePrice: 50 (or 0 to use item's base value)
   - StockQuantity: 10 (-1 for unlimited)
   - bRestocks: true
 - RestockInterval: 3600.0 (1 hour in seconds)
   - RestockQuantity: 5

5. Assign to merchant:
   - Open merchant blueprint
   - Select Shop Component
   - Set ShopData to DA_GeneralStore
*/

// ============================================================================
// EXAMPLE 7: Buying and Selling
// ============================================================================

// Buy item (Blueprint or C++)
void BuyHealthPotion(AMyMerchant* Merchant, AMyCharacter* Player)
{
	UItemDataAsset* PotionData = /* Load DA_HealthPotion */;
	
	EInventoryOperationResult Result;
	bool bSuccess = Merchant->ShopComponent->PurchaseItem(
		PotionData,
		5, // quantity
		Player,
		Result
	);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Purchase successful!"));
	}
}

// Sell item
void SellSword(AMyMerchant* Merchant, AMyCharacter* Player)
{
	UItemDataAsset* SwordData = /* Load DA_IronSword */;
	
	EInventoryOperationResult Result;
	bool bSuccess = Merchant->ShopComponent->SellItem(
		SwordData,
		1, // quantity
		Player,
		Result
	);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Sale successful!"));
	}
}

// ============================================================================
// EXAMPLE 8: Save and Load Inventory
// ============================================================================

// Save inventory to file
void SavePlayerInventory(AMyCharacter* Player, const FString& FilePath)
{
	FString JsonString;
	if (Player->InventoryComponent->SaveToJson(JsonString))
	{
		FFileHelper::SaveStringToFile(JsonString, *FilePath);
		UE_LOG(LogTemp, Log, TEXT("Inventory saved to %s"), *FilePath);
	}
}

// Load inventory from file
void LoadPlayerInventory(AMyCharacter* Player, const FString& FilePath)
{
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		if (Player->InventoryComponent->LoadFromJson(JsonString))
		{
			UE_LOG(LogTemp, Log, TEXT("Inventory loaded from %s"), *FilePath);
		}
	}
}

// ============================================================================
// EXAMPLE 9: Custom Item Effect
// ============================================================================

// Create custom effect class (Blueprint or C++)
UCLASS(Blueprintable)
class UMyCustomEffect : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Effects")
	void ExecuteEffect(AActor* Target, UInventoryItem* SourceItem)
	{
		// Custom effect logic
		UE_LOG(LogTemp, Log, TEXT("Custom effect executed on %s"), *Target->GetName());
		
		// Example: Spawn particles, play sound, etc.
	}
};

/*
Then in Effect Data Asset:
- CustomEffectClass: UMyCustomEffect
- CustomEffectParameters: Configure as needed
*/

// ============================================================================
// EXAMPLE 10: Persistent Effects (WhileOwned/WhileEquipped)
// ============================================================================

/*
Create Effect Data Asset for "Regeneration Ring":
- TriggerType: WhileEquipped
- TargetType: Self
- ExecutionInterval: 5.0 (every 5 seconds)
- ExecutionCount: 0 (infinite)
- NeedsModifications:
  * NeedTag: "Needs.Health"
  * ModificationAmount: 5.0

This will restore 5 health every 5 seconds while the ring is equipped.

In code:
*/
void EquipRing(UInventoryItem* RingItem)
{
	// Equip the item
	EInventoryOperationResult Result;
	InventoryComponent->EquipItem(RingItem->ItemInstanceId, Result);

	// Start persistent effects
	if (Result == EInventoryOperationResult::Success && EffectExecutor)
	{
		EffectExecutor->StartPersistentEffects(RingItem, EItemEffectTrigger::WhileEquipped);
	}
}

void UnequipRing(UInventoryItem* RingItem)
{
	// Stop persistent effects
	if (EffectExecutor)
	{
		EffectExecutor->StopPersistentEffects(RingItem->ItemInstanceId);
	}

	// Unequip the item
	EInventoryOperationResult Result;
	InventoryComponent->UnequipItem(RingItem->ItemInstanceId, Result);
}

// ============================================================================
// That's it! The plugin is ready to use in your project.
// Check the README.md for more detailed information.
// ============================================================================
