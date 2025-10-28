# API Reference - Advanced Inventory System

## Quick Function Reference

### UInventoryComponent

#### Add/Remove Items
```cpp
// Add item by data asset
UInventoryItem* AddItem(UItemDataAsset* ItemData, int32 Quantity, EInventoryOperationResult& OutResult);

// Add existing inventory item
bool AddInventoryItem(UInventoryItem* Item, EInventoryOperationResult& OutResult);

// Remove by instance ID
int32 RemoveItemById(FInventoryItemId ItemId, int32 Quantity, EInventoryOperationResult& OutResult);

// Remove by item data
int32 RemoveItemByData(UItemDataAsset* ItemData, int32 Quantity, EInventoryOperationResult& OutResult);

// Clear all items
void ClearInventory();
```

#### Query Items
```cpp
// Get all items
TArray<UInventoryItem*> GetAllItems() const;

// Get by ID
UInventoryItem* GetItemById(FInventoryItemId ItemId) const;

// Get by slot
UInventoryItem* GetItemAtSlot(int32 SlotIndex) const;

// Find by data
TArray<UInventoryItem*> FindItemsByData(UItemDataAsset* ItemData) const;

// Find by tags
TArray<UInventoryItem*> FindItemsByTags(FGameplayTagContainer Tags, bool bRequireAll = true) const;

// Check existence
bool HasItem(UItemDataAsset* ItemData, int32 Quantity = 1) const;

// Get quantity
int32 GetItemQuantity(UItemDataAsset* ItemData) const;

// Slot queries
int32 GetUsedSlots() const;
int32 GetFreeSlots() const;
bool IsInventoryFull() const;
bool IsInventoryEmpty() const;
```

#### Use Items
```cpp
// Use item
bool UseItem(FInventoryItemId ItemId, AActor* User, EInventoryOperationResult& OutResult);

// Gift to NPC
bool GiftItem(FInventoryItemId ItemId, AActor* Receiver, int32 Quantity, EInventoryOperationResult& OutResult);

// Drop item
AWorldItemActor* DropItem(FInventoryItemId ItemId, int32 Quantity, FVector DropLocation, EInventoryOperationResult& OutResult);

// Drop in front of owner
AWorldItemActor* DropItemInFront(FInventoryItemId ItemId, int32 Quantity, float DropDistance, EInventoryOperationResult& OutResult);
```

#### Equipment
```cpp
// Equip/Unequip
bool EquipItem(FInventoryItemId ItemId, EInventoryOperationResult& OutResult);
bool UnequipItem(FInventoryItemId ItemId, EInventoryOperationResult& OutResult);

// Query equipped
TArray<UInventoryItem*> GetEquippedItems() const;
UInventoryItem* GetEquippedItemInSlot(FGameplayTag SlotTag) const;
```

#### Organization
```cpp
// Swap items
bool SwapItems(int32 SlotIndexA, int32 SlotIndexB);

// Sort inventory
void SortInventory();

// Validation
bool CanAddItem(UItemDataAsset* ItemData, int32 Quantity) const;
```

#### Save/Load
```cpp
// Save to JSON
bool SaveToJson(FString& OutJsonString);

// Load from JSON
bool LoadFromJson(const FString& JsonString);
```

#### Events
```cpp
FOnInventoryItemAdded OnItemAdded;
FOnInventoryItemRemoved OnItemRemoved;
FOnInventoryItemUsed OnItemUsed;
FOnInventoryItemGifted OnItemGifted;
FOnInventoryItemEquipped OnItemEquipped;
FOnInventoryChanged OnInventoryChanged;
```

---

### UInventoryItem

#### Creation
```cpp
// Create new item
static UInventoryItem* CreateInventoryItem(UObject* WorldContextObject, UItemDataAsset* InItemData, int32 InQuantity = 1);
```

#### Queries
```cpp
// Display info
FText GetDisplayName() const;
FText GetDescription() const;

// Validation
bool IsValid() const;
bool CanStackWith(const UInventoryItem* OtherItem) const;

// Quantity
int32 GetQuantity() const;
int32 GetMaxStackSize() const;
bool IsStackFull() const;
int32 GetRemainingCapacity() const;

// Usage
bool CanBeUsed(AActor* User) const;
bool IsOnCooldown(const UWorld* World) const;
float GetRemainingCooldown(const UWorld* World) const;
```

#### Modification
```cpp
// Quantity
int32 AddQuantity(int32 Amount);
int32 RemoveQuantity(int32 Amount);
void SetQuantity(int32 NewQuantity);

// State
void SetEquipped(bool bEquipped);
void UpdateLastUsedTime(const UWorld* World);
```

#### Metadata
```cpp
// Custom metadata
void SetMetadata(FName Key, const FString& Value);
bool GetMetadata(FName Key, FString& OutValue) const;
bool RemoveMetadata(FName Key);
```

#### Serialization
```cpp
FString ToJson() const;
bool FromJson(const FString& JsonString);
```

---

### AWorldItemActor

#### Initialization
```cpp
// Initialize with item data
void InitializeItem(UItemDataAsset* InItemData, int32 InQuantity = 1);

// Initialize from inventory item
void InitializeFromInventoryItem(UInventoryItem* InventoryItem);
```

#### Interaction
```cpp
// Pickup
bool PickupItem(AActor* Picker);

// Queries
FText GetPickupText() const;
bool CanBePickedUpBy(AActor* Picker) const;
```

#### Events (Blueprint)
```cpp
// Implement in Blueprint
OnItemPickedUp(AActor* Picker);
OnPickupFailed(AActor* Picker);
```

---

### UItemEffectExecutorComponent

#### Execute Effects
```cpp
// Execute by trigger type
bool ExecuteItemEffects(UInventoryItem* Item, EItemEffectTrigger TriggerType, AActor* Target = nullptr);

// Execute single effect
bool ExecuteSingleEffect(UItemEffectDataAsset* Effect, UInventoryItem* Item, AActor* Target);
```

#### Persistent Effects
```cpp
// Start persistent effects (WhileOwned/WhileEquipped)
void StartPersistentEffects(UInventoryItem* Item, EItemEffectTrigger TriggerType);

// Stop persistent effects
void StopPersistentEffects(FInventoryItemId ItemId);
void StopAllPersistentEffects();
```

---

### UShopComponent

#### Purchase/Sell
```cpp
// Purchase
bool PurchaseItem(UItemDataAsset* Item, int32 Quantity, AActor* Buyer, EInventoryOperationResult& OutResult);

// Sell
bool SellItem(UItemDataAsset* Item, int32 Quantity, AActor* Seller, EInventoryOperationResult& OutResult);
```

#### Pricing
```cpp
// Get prices
int32 GetPurchasePrice(UItemDataAsset* Item, int32 Quantity, AActor* Buyer) const;
int32 GetSellbackPrice(UItemDataAsset* Item, int32 Quantity, AActor* Seller) const;
```

#### Inventory
```cpp
// Query shop inventory
TArray<UItemDataAsset*> GetAvailableItems() const;
bool IsItemAvailable(UItemDataAsset* Item, int32 Quantity, AActor* Buyer) const;
int32 GetItemStock(UItemDataAsset* Item) const;
bool WillBuyItem(UItemDataAsset* Item) const;
```

#### Restocking
```cpp
// Restock
void RestockAll();
void RestockItem(UItemDataAsset* Item);
```

#### Events
```cpp
FOnItemPurchased OnItemPurchased;
FOnItemSold OnItemSold;
FOnShopInventoryChanged OnShopInventoryChanged;
```

---

### UI Widgets

#### UInventoryWidgetBase
```cpp
// Binding
void BindToInventory(UInventoryComponent* InInventoryComponent);
void UnbindFromInventory();
UInventoryComponent* GetBoundInventory() const;

// Events (Blueprint Implementable)
OnInventoryUpdated();
OnItemAddedToInventory(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);
OnItemRemovedFromInventory(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);
OnItemUsedFromInventory(UInventoryItem* Item, AActor* User, bool bSuccess);
```

#### UShopWidgetBase
```cpp
// Binding
void BindToShop(UShopComponent* InShopComponent, UInventoryComponent* InCustomerInventory);
void UnbindFromShop();

// Actions
bool PurchaseItem(UItemDataAsset* Item, int32 Quantity);
bool SellItem(UItemDataAsset* Item, int32 Quantity);

// Queries
int32 GetPurchasePrice(UItemDataAsset* Item, int32 Quantity) const;
int32 GetSellbackPrice(UItemDataAsset* Item, int32 Quantity) const;
bool CanAffordItem(UItemDataAsset* Item, int32 Quantity) const;
int32 GetCustomerCurrency() const;

// Events (Blueprint Implementable)
OnShopInventoryUpdated();
OnCustomerInventoryUpdated();
OnItemPurchased(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice);
OnItemSold(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice);
OnPurchaseFailed(UItemDataAsset* Item, EInventoryOperationResult Reason);
OnSaleFailed(UItemDataAsset* Item, EInventoryOperationResult Reason);
```

#### UItemTooltipWidget
```cpp
// Setup
void SetItemData(UItemDataAsset* InItemData, int32 InQuantity = 1);
void SetInventoryItem(UInventoryItem* InItem);
void ClearTooltip();

// Getters
FText GetItemName() const;
FText GetItemDescription() const;
UTexture2D* GetItemIcon() const;
EItemRarity GetItemRarity() const;
EItemCategory GetItemCategory() const;
int32 GetQuantity() const;
int32 GetItemValue() const;
bool IsStackable() const;
bool IsUsable() const;
bool IsEquippable() const;
bool IsQuestItem() const;
float GetItemWeight() const;
FLinearColor GetRarityColor() const;

// Events (Blueprint Implementable)
OnTooltipUpdated();
OnTooltipCleared();
```

---

## Interfaces

### INeedsSystemInterface
```cpp
// Modify need value
void ModifyNeed(const FGameplayTag& NeedTag, float DeltaValue, bool bClamp = true);

// Get need value
float GetNeedValue(const FGameplayTag& NeedTag) const;
```

### IDialogueSystemInterface
```cpp
// Trigger dialogue
void TriggerDialogue(const FGameplayTag& DialogueTag, UObject* OptionalContext = nullptr);

// Check dialogue condition
bool CheckDialogueCondition(const FGameplayTag& DialogueTag) const;
```

### ITimeOfDaySystemInterface
```cpp
// Get current time
float GetCurrentTimeOfDay() const;
int32 GetCurrentDay() const;
bool IsTimeInRange(float StartHour, float EndHour) const;
```

### IItemEffectTargetSelector
```cpp
// Select targets for effect
bool SelectTargets(const UItemDataAsset* ItemData, AActor* Instigator, TArray<AActor*>& OutTargets);
```

### IShopPriceModifier
```cpp
// Calculate purchase price
int32 CalculatePrice(int32 BasePrice, const UItemDataAsset* ItemData, AActor* Buyer, int32 Quantity);

// Calculate sellback price
int32 CalculateSellbackPrice(int32 OriginalPrice, const UItemDataAsset* ItemData, AActor* Seller, int32 Quantity);
```

### IInventorySystemSaveInterface
```cpp
// Save to JSON
bool SaveToJson(FString& OutJsonString);

// Load from JSON
bool LoadFromJson(const FString& JsonString);
```

### IInventoryHolder
```cpp
// Get inventory component
UActorComponent* GetInventoryComponent();
```

---

## Enumerations

### EInventoryOperationResult
```cpp
Success         // Operation successful
Failed_ItemNotFound        // Item not found in inventory
Failed_InsufficientSpace   // Not enough inventory space
Failed_InsufficientQuantity // Not enough items
Failed_InvalidItem   // Invalid item or item data
Failed_TagRequirementNotMet // Tag requirements not met
Failed_CustomCondition     // Custom condition failed
Failed_StackLimitReached   // Stack limit reached
Failed_Unknown             // Unknown error
```

### EItemEffectTrigger
```cpp
OnAcquire      // When item is added to inventory
OnLose     // When item is removed from inventory
OnUse          // When item is used
OnGift       // When item is gifted
WhileOwned     // While item is in inventory (persistent)
WhileEquipped  // While item is equipped (persistent)
```

### EItemEffectTarget
```cpp
Self        // Effect targets the owner
NPC            // Effect targets an NPC
CustomSelector // Effect uses custom target selection
```

### EItemRarity
```cpp
Common      // Common item (Gray)
Uncommon    // Uncommon item (Green)
Rare           // Rare item (Blue)
Epic   // Epic item (Purple)
Legendary      // Legendary item (Orange)
Unique   // Unique item (Gold)
```

### EItemCategory
```cpp
Consumable     // Consumable items (food, potions, etc.)
Equipment      // Equipment items (weapons, armor, etc.)
QuestItem      // Quest items
Crafting       // Crafting materials
Currency       // Currency items
Readable       // Readable items (books, notes, etc.)
Key   // Key items
Miscellaneous  // Miscellaneous items
```

---

## Common Patterns

### Pattern: Add Item and Handle Result
```cpp
EInventoryOperationResult Result;
UInventoryItem* Item = InventoryComponent->AddItem(ItemData, 5, Result);

if (Result == EInventoryOperationResult::Success)
{
    UE_LOG(LogTemp, Log, TEXT("Added %s x5"), *Item->GetDisplayName().ToString());
}
else
{
    UE_LOG(LogTemp, Warning, TEXT("Failed to add item: %d"), (int32)Result);
}
```

### Pattern: Use Item with Effects
```cpp
// Execute item usage
EInventoryOperationResult Result;
bool bUsed = InventoryComponent->UseItem(ItemId, this, Result);

if (bUsed && EffectExecutor)
{
    // Get the item
    UInventoryItem* Item = InventoryComponent->GetItemById(ItemId);
    
    // Execute OnUse effects
EffectExecutor->ExecuteItemEffects(Item, EItemEffectTrigger::OnUse, this);
}
```

### Pattern: Equip Item with Persistent Effects
```cpp
// Equip
EInventoryOperationResult Result;
bool bEquipped = InventoryComponent->EquipItem(ItemId, Result);

if (bEquipped && EffectExecutor)
{
    UInventoryItem* Item = InventoryComponent->GetItemById(ItemId);
    
    // Start WhileEquipped effects
    EffectExecutor->StartPersistentEffects(Item, EItemEffectTrigger::WhileEquipped);
}

// Later, when unequipping
EffectExecutor->StopPersistentEffects(ItemId);
InventoryComponent->UnequipItem(ItemId, Result);
```

### Pattern: Save/Load Inventory
```cpp
// Save
FString SavePath = FPaths::ProjectSavedDir() / TEXT("Saves/Inventory.json");
FString JsonString;

if (InventoryComponent->SaveToJson(JsonString))
{
    FFileHelper::SaveStringToFile(JsonString, *SavePath);
}

// Load
FString JsonString;
if (FFileHelper::LoadFileToString(JsonString, *SavePath))
{
    InventoryComponent->LoadFromJson(JsonString);
}
```

---

**For more examples, see ExampleUsage.cpp in the plugin directory.**
