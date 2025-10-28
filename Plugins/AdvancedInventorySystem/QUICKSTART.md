# Advanced Inventory System - Quick Start Guide

## Installation

### 1. Enable Plugin

**Option A: Via .uproject file**
```json
{
    "Plugins": [
        {
 "Name": "AdvancedInventorySystem",
            "Enabled": true
 }
 ]
}
```

**Option B: Via Editor**
1. Edit ? Plugins
2. Search: "Advanced Inventory System"
3. Check "Enabled"
4. Restart Editor

### 2. Enable Gameplay Tags

1. Edit ? Project Settings
2. Project ? Gameplay Tags
3. Add tag source (optional): Create `Config/Tags/InventoryTags.ini`

## Basic Setup (5 Minutes)

### Step 1: Add Inventory to Character

**In C++ Header:**
```cpp
#include "InventoryComponent.h"

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
UInventoryComponent* InventoryComponent;
```

**In C++ Constructor:**
```cpp
InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
InventoryComponent->MaxInventorySlots = 20;
InventoryComponent->bAutoStack = true;
```

**Or in Blueprint:**
1. Open Character Blueprint
2. Add Component ? Inventory Component
3. Set Max Inventory Slots = 20
4. Set Auto Stack = true

### Step 2: Create Your First Item

1. **Right-click** in Content Browser
2. **Miscellaneous ? Data Asset**
3. Select **UItemDataAsset**
4. Name it `DA_HealthPotion`

**Configure:**
```
ItemID: HealthPotion
ItemName: Health Potion
ItemDescription: Restores 50 health points
ItemIcon: [Your icon texture]
Rarity: Common
Category: Consumable
bIsStackable: true
MaxStackSize: 20
bIsUsable: true
bConsumeOnUse: true
BaseValue: 25
```

### Step 3: Create Item Effect

1. **Right-click** in Content Browser
2. **Miscellaneous ? Data Asset**
3. Select **UItemEffectDataAsset**
4. Name it `Effect_RestoreHealth`

**Configure:**
```
EffectID: RestoreHealth
EffectName: Restore Health
TriggerType: OnUse
TargetType: Self
```

**Add Needs Modification:**
```
NeedTag: Needs.Health
ModificationAmount: 50.0
bClampValue: true
```

### Step 4: Link Effect to Item

1. Open `DA_HealthPotion`
2. Find **Item Effects** array
3. Add element
4. Set to `Effect_RestoreHealth`

### Step 5: Implement Needs System Interface

**In Character Header:**
```cpp
#include "InventorySystemInterfaces.h"

class AMyCharacter : public ACharacter, public INeedsSystemInterface
{
    // Implement interface
    virtual void ModifyNeed_Implementation(
  const FGameplayTag& NeedTag, 
        float DeltaValue, 
        bool bClamp) override;
    
    virtual float GetNeedValue_Implementation(
   const FGameplayTag& NeedTag) const override;
};
```

**In Character CPP:**
```cpp
void AMyCharacter::ModifyNeed_Implementation(
    const FGameplayTag& NeedTag, 
float DeltaValue, 
 bool bClamp)
{
    if (NeedTag.MatchesTag(FGameplayTag::RequestGameplayTag("Needs.Health")))
    {
        CurrentHealth += DeltaValue;
        if (bClamp)
        {
     CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
        }
    }
}

float AMyCharacter::GetNeedValue_Implementation(const FGameplayTag& NeedTag) const
{
    if (NeedTag.MatchesTag(FGameplayTag::RequestGameplayTag("Needs.Health")))
    {
        return CurrentHealth;
    }
    return 0.0f;
}
```

### Step 6: Add Effect Executor

**In Character Header:**
```cpp
#include "ItemEffectExecutorComponent.h"

UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
UItemEffectExecutorComponent* EffectExecutor;
```

**In Character Constructor:**
```cpp
EffectExecutor = CreateDefaultSubobject<UItemEffectExecutorComponent>(TEXT("EffectExecutor"));
```

### Step 7: Handle Item Usage

**In Character CPP:**
```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Bind to item used event
 InventoryComponent->OnItemUsed.AddDynamic(this, &AMyCharacter::OnInventoryItemUsed);
}

void AMyCharacter::OnInventoryItemUsed(UInventoryItem* Item, AActor* User, bool bSuccess)
{
    if (bSuccess && EffectExecutor)
    {
    // Execute OnUse effects
      EffectExecutor->ExecuteItemEffects(Item, EItemEffectTrigger::OnUse, this);
    }
}
```

## Testing (2 Minutes)

### Add Item via Console

1. Press **~** (tilde) to open console
2. Type: `ke * AddTestItem`

**Create AddTestItem function:**
```cpp
UFUNCTION(Exec, Category = "Inventory")
void AddTestItem()
{
 UItemDataAsset* PotionData = LoadObject<UItemDataAsset>(
        nullptr, 
        TEXT("/Game/Items/DA_HealthPotion")
    );
    
    EInventoryOperationResult Result;
    InventoryComponent->AddItem(PotionData, 5, Result);
}
```

### Use Item via Console

```cpp
UFUNCTION(Exec, Category = "Inventory")
void UseFirstItem()
{
    TArray<UInventoryItem*> Items = InventoryComponent->GetAllItems();
    if (Items.Num() > 0)
    {
        EInventoryOperationResult Result;
      InventoryComponent->UseItem(Items[0]->ItemInstanceId, this, Result);
    }
}
```

## Common Recipes

### Recipe 1: Pickup World Item

```cpp
void AMyCharacter::InteractWithItem(AWorldItemActor* WorldItem)
{
 if (WorldItem && WorldItem->CanBePickedUpBy(this))
    {
        WorldItem->PickupItem(this);
    }
}
```

### Recipe 2: Drop Item

```cpp
void AMyCharacter::DropCurrentItem()
{
    TArray<UInventoryItem*> Items = InventoryComponent->GetAllItems();
    if (Items.Num() > 0)
    {
        EInventoryOperationResult Result;
        InventoryComponent->DropItemInFront(
          Items[0]->ItemInstanceId, 
            1,   // quantity
    200.0f, // drop distance
      Result
        );
    }
}
```

### Recipe 3: Equip/Unequip

```cpp
void AMyCharacter::ToggleEquipItem(FInventoryItemId ItemId)
{
    UInventoryItem* Item = InventoryComponent->GetItemById(ItemId);
    if (Item)
    {
  EInventoryOperationResult Result;
        if (Item->bIsEquipped)
        {
            InventoryComponent->UnequipItem(ItemId, Result);
   
            // Stop persistent effects
            if (EffectExecutor)
        {
EffectExecutor->StopPersistentEffects(ItemId);
            }
        }
        else
        {
            InventoryComponent->EquipItem(ItemId, Result);
          
            // Start persistent effects
        if (Result == EInventoryOperationResult::Success && EffectExecutor)
 {
          EffectExecutor->StartPersistentEffects(Item, EItemEffectTrigger::WhileEquipped);
            }
        }
    }
}
```

### Recipe 4: Gift to NPC

```cpp
void AMyCharacter::GiftItemToNPC(AActor* NPC, FInventoryItemId ItemId)
{
    EInventoryOperationResult Result;
    bool bSuccess = InventoryComponent->GiftItem(ItemId, NPC, 1, Result);
    
    if (bSuccess)
{
     UE_LOG(LogTemp, Log, TEXT("Gift successful!"));
    }
}
```

### Recipe 5: Save/Load Inventory

```cpp
// Save
void SaveInventory()
{
 FString JsonString;
    if (InventoryComponent->SaveToJson(JsonString))
    {
        FString FilePath = FPaths::ProjectSavedDir() / TEXT("inventory.json");
        FFileHelper::SaveStringToFile(JsonString, *FilePath);
    }
}

// Load
void LoadInventory()
{
    FString FilePath = FPaths::ProjectSavedDir() / TEXT("inventory.json");
    FString JsonString;
    if (FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        InventoryComponent->LoadFromJson(JsonString);
    }
}
```

## Blueprint Quick Start

### Add Item (Blueprint)

1. Get Inventory Component
2. Call **Add Item**
- Item Data: `DA_HealthPotion`
   - Quantity: 5
3. Branch on Result == Success

### Use Item (Blueprint)

1. Get Inventory Component
2. Call **Get Item At Slot** (Index 0)
3. Get **Item Instance Id** from result
4. Call **Use Item**
   - Item Id: [from above]
   - User: Self
5. Branch on Success

### Listen to Events (Blueprint)

1. Event BeginPlay
2. Get Inventory Component
3. Bind Event to **On Item Added**
4. In bound event: Print String with Item Name

## Next Steps

1. **Create More Items** - Food, weapons, armor, quest items
2. **Add Shop System** - Create `UShopDataAsset`, add `ShopComponent` to merchants
3. **Create UI** - Extend `UInventoryWidgetBase`, `UShopWidgetBase`, `UItemTooltipWidget`
4. **Add Effects** - Buffs, debuffs, stat modifications
5. **Implement Custom Selectors** - For AoE effects, party members, etc.

## Troubleshooting

**Problem: Item effects not working**
- Solution: Make sure `ItemEffectExecutorComponent` is added to character
- Solution: Verify interface implementation (`INeedsSystemInterface`)
- Solution: Check effect is added to item's `ItemEffects` array

**Problem: Items not stacking**
- Solution: Set `bIsStackable = true` in ItemData
- Solution: Set `MaxStackSize > 1`
- Solution: Verify `bAutoStack = true` in InventoryComponent

**Problem: Can't drop items**
- Solution: Set `bCanBeDropped = true` in item's `PhysicalRepresentation`
- Solution: Ensure item has valid physical representation (mesh)

**Problem: Shop not working**
- Solution: Set `CurrencyItem` in ShopDataAsset
- Solution: Add currency to player inventory
- Solution: Check `bIsOpen = true` in ShopComponent

## Support

- **Documentation**: See `README.md` and `ExampleUsage.cpp`
- **Examples**: Full code examples in `Plugins/AdvancedInventorySystem/ExampleUsage.cpp`
- **Issues**: Report bugs via project repository

---

**You're ready to go! Start creating your inventory system!** ??
