# Integration Guide - Advanced Inventory System

## Step-by-Step Integration into Your Project

### Phase 1: Plugin Installation (5 minutes)

#### 1.1 Copy Plugin
```
YourProject/
??? Plugins/
    ??? AdvancedInventorySystem/  <- Copy here
```

#### 1.2 Enable in .uproject
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

#### 1.3 Regenerate Project Files
- Right-click `.uproject` ? "Generate Visual Studio project files"
- Or run `GenerateProjectFiles.bat`

#### 1.4 Compile
- Open solution in Visual Studio
- Build ? Build Solution (Ctrl+Shift+B)

---

### Phase 2: Character Setup (10 minutes)

#### 2.1 Add Required Headers

**YourCharacter.h:**
```cpp
#include "InventoryComponent.h"
#include "ItemEffectExecutorComponent.h"
#include "InventorySystemInterfaces.h"

class UInventoryComponent;
class UItemEffectExecutorComponent;
```

#### 2.2 Add Components

**In Header:**
```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
UInventoryComponent* InventoryComponent;

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
UItemEffectExecutorComponent* EffectExecutor;
```

**In Constructor:**
```cpp
AYourCharacter::AYourCharacter()
{
    // Create inventory
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
    InventoryComponent->MaxInventorySlots = 20;
    InventoryComponent->bAutoStack = true;

    // Create effect executor
    EffectExecutor = CreateDefaultSubobject<UItemEffectExecutorComponent>(TEXT("EffectExecutor"));
}
```

#### 2.3 Implement Interfaces (Optional but Recommended)

**In Header:**
```cpp
class YOURPROJECT_API AYourCharacter : public ACharacter, 
    public INeedsSystemInterface,
    public IInventoryHolder
{
    GENERATED_BODY()

public:
  // INeedsSystemInterface
    virtual void ModifyNeed_Implementation(const FGameplayTag& NeedTag, float DeltaValue, bool bClamp) override;
    virtual float GetNeedValue_Implementation(const FGameplayTag& NeedTag) const override;

    // IInventoryHolder
    virtual UActorComponent* GetInventoryComponent_Implementation() override;

protected:
    // Example: Store needs
    UPROPERTY()
    TMap<FGameplayTag, float> NeedsMap;
};
```

**In CPP:**
```cpp
void AYourCharacter::ModifyNeed_Implementation(const FGameplayTag& NeedTag, float DeltaValue, bool bClamp)
{
    if (float* NeedValue = NeedsMap.Find(NeedTag))
    {
   *NeedValue += DeltaValue;
        if (bClamp)
        {
   *NeedValue = FMath::Clamp(*NeedValue, 0.0f, 100.0f);
  }
        
        UE_LOG(LogTemp, Log, TEXT("Need %s: %.2f"), *NeedTag.ToString(), *NeedValue);
    }
}

float AYourCharacter::GetNeedValue_Implementation(const FGameplayTag& NeedTag) const
{
    const float* NeedValue = NeedsMap.Find(NeedTag);
    return NeedValue ? *NeedValue : 0.0f;
}

UActorComponent* AYourCharacter::GetInventoryComponent_Implementation()
{
    return InventoryComponent;
}
```

#### 2.4 Wire Up Events

**In BeginPlay:**
```cpp
void AYourCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize needs
    NeedsMap.Add(FGameplayTag::RequestGameplayTag("Needs.Health"), 100.0f);
    NeedsMap.Add(FGameplayTag::RequestGameplayTag("Needs.Hunger"), 100.0f);

    // Bind inventory events
    if (InventoryComponent)
    {
        InventoryComponent->OnItemAdded.AddDynamic(this, &AYourCharacter::OnItemAdded);
        InventoryComponent->OnItemUsed.AddDynamic(this, &AYourCharacter::OnItemUsed);
        InventoryComponent->OnItemEquipped.AddDynamic(this, &AYourCharacter::OnItemEquipped);
    }
}

void AYourCharacter::OnItemAdded(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("Added: %s x%d"), *Item->GetDisplayName().ToString(), Quantity);

    // Execute OnAcquire effects
    if (EffectExecutor)
    {
 EffectExecutor->ExecuteItemEffects(Item, EItemEffectTrigger::OnAcquire, this);
    }
}

void AYourCharacter::OnItemUsed(UInventoryItem* Item, AActor* User, bool bSuccess)
{
    if (bSuccess && EffectExecutor)
    {
        // Execute OnUse effects
     EffectExecutor->ExecuteItemEffects(Item, EItemEffectTrigger::OnUse, this);
    }
}

void AYourCharacter::OnItemEquipped(UInventoryItem* Item, bool bEquipped)
{
    if (bEquipped && EffectExecutor)
    {
  // Start WhileEquipped effects
        EffectExecutor->StartPersistentEffects(Item, EItemEffectTrigger::WhileEquipped);
    }
    else if (EffectExecutor)
    {
      // Stop WhileEquipped effects
        EffectExecutor->StopPersistentEffects(Item->ItemInstanceId);
    }
}
```

---

### Phase 3: Create Content (15 minutes)

#### 3.1 Setup Gameplay Tags

1. **Edit ? Project Settings ? Gameplay Tags**
2. Add tag source or manually add tags:

```
Items.Category.Consumable
Items.Category.Equipment
Items.Category.Quest

Items.Rarity.Common
Items.Rarity.Rare
Items.Rarity.Legendary

Needs.Health
Needs.Hunger
Needs.Thirst
Needs.Energy

Equipment.Slot.Head
Equipment.Slot.Body
Equipment.Slot.Hands

Dialogue.Greeting
Dialogue.Quest.Start
```

#### 3.2 Create Currency Item

1. **Content Browser ? Right-click ? Miscellaneous ? Data Asset**
2. Select **UItemDataAsset**
3. Name: `DA_GoldCoin`
4. Configure:
```
ItemID: GoldCoin
ItemName: Gold Coin
ItemDescription: Standard currency
Category: Currency
bIsStackable: true
MaxStackSize: 999
bIsCurrency: true
BaseValue: 1
```

#### 3.3 Create Health Potion

1. Create **UItemDataAsset**: `DA_HealthPotion`
2. Configure:
```
ItemID: HealthPotion
ItemName: Health Potion
ItemDescription: Restores 50 health
Category: Consumable
bIsStackable: true
MaxStackSize: 20
bIsUsable: true
bConsumeOnUse: true
BaseValue: 25
```

3. Create **UItemEffectDataAsset**: `Effect_RestoreHealth`
```
EffectID: RestoreHealth
EffectName: Restore Health
TriggerType: OnUse
TargetType: Self

NeedsModifications[0]:
  NeedTag: Needs.Health
  ModificationAmount: 50.0
  bClampValue: true
```

4. In `DA_HealthPotion`, add `Effect_RestoreHealth` to **ItemEffects** array

#### 3.4 Create Weapon (Optional)

1. Create **UItemDataAsset**: `DA_IronSword`
2. Configure:
```
ItemID: IronSword
ItemName: Iron Sword
ItemDescription: A sturdy iron blade
Category: Equipment
bIsEquippable: true
EquipmentSlot: Equipment.Slot.Weapon
bIsUsable: false
bCanBeSold: true
BaseValue: 100

PhysicalRepresentation:
  StaticMesh: [Your sword mesh]
  bCanBePickedUp: true
  bCanBeDropped: true
```

---

### Phase 4: Create Shop (10 minutes)

#### 4.1 Create Shop Data Asset

1. **Content Browser ? Right-click ? Miscellaneous ? Data Asset**
2. Select **UShopDataAsset**
3. Name: `DA_GeneralStore`
4. Configure:
```
ShopName: "General Store"
ShopDescription: "Buy and sell common goods"
CurrencyItem: DA_GoldCoin
PriceModifier: 1.0
SellbackPercentage: 0.5
bBuysItems: true

ShopInventory[0]:
  Item: DA_HealthPotion
  PurchasePrice: 50 (or 0 to use BaseValue)
  StockQuantity: 10 (-1 for unlimited)
  bRestocks: true
  RestockInterval: 3600.0  // 1 hour
  RestockQuantity: 5

ShopInventory[1]:
  Item: DA_IronSword
  PurchasePrice: 150
  StockQuantity: 2
  bRestocks: false
```

#### 4.2 Create Merchant NPC

**In Blueprint or C++:**
```cpp
class YOURPROJECT_API AMerchantNPC : public ACharacter
{
    GENERATED_BODY()

public:
    AMerchantNPC();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
 UShopComponent* ShopComponent;
};

AMerchantNPC::AMerchantNPC()
{
    ShopComponent = CreateDefaultSubobject<UShopComponent>(TEXT("Shop"));
    ShopComponent->bIsOpen = true;
}
```

**In Blueprint:**
- Add Shop Component
- Set ShopData to `DA_GeneralStore`

#### 4.3 Create Interaction

**In Player Controller or Character:**
```cpp
void AYourPlayerController::InteractWithMerchant(AMerchantNPC* Merchant)
{
    if (Merchant && Merchant->ShopComponent)
    {
        // Open shop UI
        OpenShopUI(Merchant->ShopComponent);
    }
}
```

---

### Phase 5: Create UI (20 minutes)

#### 5.1 Create Inventory UI Widget

**Create Blueprint Widget: WBP_Inventory**

1. **Inherit from**: `InventoryWidgetBase`
2. **Add UI elements**: Grid Panel, Item Slots, etc.
3. **Implement Events**:

```
Event OnInventoryUpdated:
  - Refresh all item slots

Event OnItemAddedToInventory:
  - Play add animation
  - Update slot display

Event OnItemRemovedFromInventory:
  - Play remove animation
  - Update slot display
```

4. **In PlayerController BeginPlay**:
```cpp
void AYourPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Create inventory widget
    if (InventoryWidgetClass)
    {
     InventoryWidget = CreateWidget<UInventoryWidgetBase>(this, InventoryWidgetClass);
        
        // Bind to player's inventory
     AYourCharacter* Character = Cast<AYourCharacter>(GetPawn());
      if (Character && Character->InventoryComponent)
        {
            InventoryWidget->BindToInventory(Character->InventoryComponent);
        }
    }
}

void AYourPlayerController::ToggleInventory()
{
    if (InventoryWidget)
    {
        if (InventoryWidget->IsInViewport())
  {
 InventoryWidget->RemoveFromParent();
 }
else
      {
            InventoryWidget->AddToViewport();
     }
    }
}
```

#### 5.2 Create Shop UI Widget

**Create Blueprint Widget: WBP_Shop**

1. **Inherit from**: `ShopWidgetBase`
2. **Add UI elements**: Shop items list, player inventory, currency display
3. **Implement Events**:

```
Event OnShopInventoryUpdated:
  - Refresh shop items list

Event OnCustomerInventoryUpdated:
  - Refresh player items list
  - Update currency display

Event OnItemPurchased:
  - Play purchase animation
  - Show confirmation message

Event OnPurchaseFailed:
  - Show error message (insufficient funds, etc.)
```

#### 5.3 Create Tooltip Widget

**Create Blueprint Widget: WBP_ItemTooltip**

1. **Inherit from**: `ItemTooltipWidget`
2. **Add UI elements**: Item name, description, icon, stats, etc.
3. **Implement Event**:

```
Event OnTooltipUpdated:
  - Set item name (GetItemName)
  - Set item description (GetItemDescription)
  - Set item icon (GetItemIcon)
  - Set rarity color (GetRarityColor)
  - Show/hide based on item properties
```

---

### Phase 6: Testing (10 minutes)

#### 6.1 Add Test Items to Player

**In BeginPlay or via console command:**
```cpp
void AYourCharacter::AddTestItems()
{
    if (!InventoryComponent) return;

    // Load items
    UItemDataAsset* HealthPotion = LoadObject<UItemDataAsset>(
      nullptr, TEXT("/Game/Items/DA_HealthPotion"));
    UItemDataAsset* GoldCoin = LoadObject<UItemDataAsset>(
        nullptr, TEXT("/Game/Items/DA_GoldCoin"));

    // Add to inventory
    EInventoryOperationResult Result;
    InventoryComponent->AddItem(HealthPotion, 5, Result);
    InventoryComponent->AddItem(GoldCoin, 100, Result);
}
```

#### 6.2 Test Item Usage

```cpp
// Console command
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

#### 6.3 Test Shopping

Place merchant in level and interact:
```cpp
void TestPurchase(AMerchantNPC* Merchant)
{
    UItemDataAsset* Potion = LoadObject<UItemDataAsset>(nullptr, TEXT("/Game/Items/DA_HealthPotion"));
    
    EInventoryOperationResult Result;
    bool bSuccess = Merchant->ShopComponent->PurchaseItem(Potion, 1, this, Result);
    
    UE_LOG(LogTemp, Log, TEXT("Purchase: %s"), bSuccess ? TEXT("Success") : TEXT("Failed"));
}
```

---

### Phase 7: Save/Load Integration (5 minutes)

#### 7.1 Save Game Class

```cpp
UCLASS()
class USaveGame_Inventory : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString InventoryData;
};
```

#### 7.2 Save Function

```cpp
void AYourCharacter::SaveInventory()
{
    if (!InventoryComponent) return;

USaveGame_Inventory* SaveGame = Cast<USaveGame_Inventory>(
        UGameplayStatics::CreateSaveGameObject(USaveGame_Inventory::StaticClass()));

    if (SaveGame)
    {
        InventoryComponent->SaveToJson(SaveGame->InventoryData);
        UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("InventorySlot"), 0);
    }
}
```

#### 7.3 Load Function

```cpp
void AYourCharacter::LoadInventory()
{
    if (!InventoryComponent) return;

    USaveGame_Inventory* SaveGame = Cast<USaveGame_Inventory>(
        UGameplayStatics::LoadGameFromSlot(TEXT("InventorySlot"), 0));

    if (SaveGame && !SaveGame->InventoryData.IsEmpty())
 {
        InventoryComponent->LoadFromJson(SaveGame->InventoryData);
    }
}
```

---

## Common Integration Issues

### Issue 1: Items Not Stacking
**Solution**: Set `bIsStackable = true` and `MaxStackSize > 1` in ItemDataAsset

### Issue 2: Effects Not Working
**Solution**: 
- Verify `ItemEffectExecutorComponent` is added
- Check interface implementation
- Ensure effects are added to item's ItemEffects array

### Issue 3: Shop Can't Buy Items
**Solution**:
- Set CurrencyItem in ShopDataAsset
- Add currency to player inventory
- Set bIsOpen = true in ShopComponent

### Issue 4: Save/Load Not Working
**Solution**:
- Ensure ItemData assets are loaded properly after deserialization
- Check JSON string is valid
- Verify save game slot exists

---

## Performance Optimization Tips

1. **Use Object Pooling** for WorldItemActors
2. **Limit Slot Count** based on game needs
3. **Batch UI Updates** when adding multiple items
4. **Cache** frequently accessed items
5. **Unload** unused item textures

---

## Blueprint Integration Tips

### Getting Inventory Component from Any Actor
```
Get Actor Component by Class ? InventoryComponent class
```

### Checking if Actor Has Inventory
```
Does Implement Interface ? InventoryHolder
If true ? Execute GetInventoryComponent
```

### Listening to Events
```
Event BeginPlay
? Get Inventory Component
? Bind Event to OnItemAdded
? Custom Event: Handle Item Added
```

---

## Next Steps

1. ? Create more items (food, weapons, armor)
2. ? Design shop UIs
3. ? Implement world item spawning
4. ? Add quest integration
5. ? Create crafting recipes (if needed)
6. ? Extend with custom effects

---

**Integration Complete!** ??

Your project now has a fully functional inventory system with effects, trading, and save/load support.

For more examples, see `ExampleUsage.cpp` in the plugin directory.
For API reference, see `API_REFERENCE.md`.
