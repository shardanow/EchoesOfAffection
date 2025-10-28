# Event & Delegate Reference - Advanced Inventory System

## Complete Event System Documentation

### UInventoryComponent Events

#### OnItemAdded
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnInventoryItemAdded, 
    UInventoryItem*, Item,      // The item that was added
    int32, Quantity, // Quantity added
    int32, SlotIndex            // Slot index where added
);

UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
FOnInventoryItemAdded OnItemAdded;
```

**When Fired**: Item successfully added to inventory

**Usage C++**:
```cpp
InventoryComponent->OnItemAdded.AddDynamic(this, &AMyCharacter::HandleItemAdded);

void AMyCharacter::HandleItemAdded(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("Added %s x%d at slot %d"), 
        *Item->GetDisplayName().ToString(), Quantity, SlotIndex);
}
```

**Usage Blueprint**:
```
Inventory Component ? Bind Event to OnItemAdded
? Custom Event: Item Added
   - Item (Inventory Item)
   - Quantity (Integer)
   - Slot Index (Integer)
```

---

#### OnItemRemoved
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnInventoryItemRemoved, 
  UInventoryItem*, Item,      // The item that was removed
    int32, Quantity,            // Quantity removed
    int32, SlotIndex            // Slot index where it was
);

UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
FOnInventoryItemRemoved OnItemRemoved;
```

**When Fired**: Item successfully removed from inventory

**Usage C++**:
```cpp
InventoryComponent->OnItemRemoved.AddDynamic(this, &AMyCharacter::HandleItemRemoved);

void AMyCharacter::HandleItemRemoved(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("Removed %s x%d from slot %d"), 
        *Item->GetDisplayName().ToString(), Quantity, SlotIndex);
}
```

---

#### OnItemUsed
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnInventoryItemUsed, 
 UInventoryItem*, Item,      // The item that was used
    AActor*, User,            // Actor who used the item
    bool, bSuccess     // Whether use was successful
);

UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
FOnInventoryItemUsed OnItemUsed;
```

**When Fired**: After item use attempt (success or failure)

**Usage C++**:
```cpp
InventoryComponent->OnItemUsed.AddDynamic(this, &AMyCharacter::HandleItemUsed);

void AMyCharacter::HandleItemUsed(UInventoryItem* Item, AActor* User, bool bSuccess)
{
    if (bSuccess)
    {
        // Execute item effects
    if (EffectExecutor)
        {
         EffectExecutor->ExecuteItemEffects(Item, EItemEffectTrigger::OnUse, User);
        }
    }
}
```

**Usage Blueprint**:
```
Event OnItemUsed
? Branch (bSuccess)
 True: Execute Effects
   False: Show "Can't use" message
```

---

#### OnItemGifted
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FOnInventoryItemGifted, 
    UInventoryItem*, Item,      // The item that was gifted
    AActor*, Giver,             // Actor giving the item
    AActor*, Receiver,          // Actor receiving the item
    bool, bSuccess              // Whether gift was successful
);

UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
FOnInventoryItemGifted OnItemGifted;
```

**When Fired**: After gift attempt (success or failure)

**Usage C++**:
```cpp
InventoryComponent->OnItemGifted.AddDynamic(this, &AMyCharacter::HandleItemGifted);

void AMyCharacter::HandleItemGifted(UInventoryItem* Item, AActor* Giver, AActor* Receiver, bool bSuccess)
{
    if (bSuccess)
    {
 // Trigger dialogue or reputation change
        UE_LOG(LogTemp, Log, TEXT("Gifted %s to %s"), 
          *Item->GetDisplayName().ToString(), 
            *Receiver->GetName());
    }
}
```

---

#### OnItemEquipped
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnInventoryItemEquipped, 
UInventoryItem*, Item,      // The item that was equipped/unequipped
  bool, bEquipped    // True if equipped, false if unequipped
);

UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
FOnInventoryItemEquipped OnItemEquipped;
```

**When Fired**: After successful equip/unequip

**Usage C++**:
```cpp
InventoryComponent->OnItemEquipped.AddDynamic(this, &AMyCharacter::HandleItemEquipped);

void AMyCharacter::HandleItemEquipped(UInventoryItem* Item, bool bEquipped)
{
    if (bEquipped)
    {
        // Start WhileEquipped effects
    if (EffectExecutor)
        {
 EffectExecutor->StartPersistentEffects(Item, EItemEffectTrigger::WhileEquipped);
    }
    
        // Update character appearance/stats
    }
    else
    {
   // Stop WhileEquipped effects
        if (EffectExecutor)
        {
    EffectExecutor->StopPersistentEffects(Item->ItemInstanceId);
        }
    }
}
```

**Usage Blueprint**:
```
Event OnItemEquipped
? Branch (bEquipped)
   True: 
  - Start Persistent Effects
     - Attach weapon mesh
     - Update stats
   False:
     - Stop Persistent Effects
     - Detach weapon mesh
     - Restore stats
```

---

#### OnInventoryChanged
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
FOnInventoryChanged OnInventoryChanged;
```

**When Fired**: Any time inventory state changes

**Usage C++**:
```cpp
InventoryComponent->OnInventoryChanged.AddDynamic(this, &AMyCharacter::HandleInventoryChanged);

void AMyCharacter::HandleInventoryChanged()
{
    // Refresh UI
    if (InventoryWidget)
    {
        InventoryWidget->RefreshDisplay();
    }
    
    // Trigger save
    SaveInventory();
}
```

**Usage Blueprint**:
```
Event OnInventoryChanged
? Refresh Inventory UI
? Auto-save (if enabled)
? Update weight display
```

---

### UShopComponent Events

#### OnItemPurchased
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FOnItemPurchased, 
    UItemDataAsset*, Item,    // Item that was purchased
    int32, Quantity,     // Quantity purchased
    int32, TotalPrice,       // Total price paid
    AActor*, Buyer         // Actor who bought
);

UPROPERTY(BlueprintAssignable, Category = "Shop|Events")
FOnItemPurchased OnItemPurchased;
```

**When Fired**: After successful purchase

**Usage C++**:
```cpp
ShopComponent->OnItemPurchased.AddDynamic(this, &AShopkeeper::HandlePurchase);

void AShopkeeper::HandlePurchase(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice, AActor* Buyer)
{
    UE_LOG(LogTemp, Log, TEXT("Sold %d x %s for %d gold"), 
        Quantity, *Item->ItemName.ToString(), TotalPrice);
  
    // Trigger dialogue
    // Update reputation
}
```

---

#### OnItemSold
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FOnItemSold, 
    UItemDataAsset*, Item,      // Item that was sold
    int32, Quantity,        // Quantity sold
    int32, TotalPrice,          // Total price received
    AActor*, Seller         // Actor who sold
);

UPROPERTY(BlueprintAssignable, Category = "Shop|Events")
FOnItemSold OnItemSold;
```

**When Fired**: After successful sale to shop

**Usage C++**:
```cpp
ShopComponent->OnItemSold.AddDynamic(this, &AShopkeeper::HandleSale);

void AShopkeeper::HandleSale(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice, AActor* Seller)
{
    UE_LOG(LogTemp, Log, TEXT("Bought %d x %s for %d gold"), 
        Quantity, *Item->ItemName.ToString(), TotalPrice);
}
```

---

#### OnShopInventoryChanged
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopInventoryChanged);

UPROPERTY(BlueprintAssignable, Category = "Shop|Events")
FOnShopInventoryChanged OnShopInventoryChanged;
```

**When Fired**: Shop stock changes (purchase, restock, etc.)

**Usage C++**:
```cpp
ShopComponent->OnShopInventoryChanged.AddDynamic(this, &UShopWidget::RefreshShopDisplay);
```

---

### UI Widget Events (Blueprint Implementable)

#### UInventoryWidgetBase

```cpp
// Called when inventory data changes
UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI|Events")
void OnInventoryUpdated();

// Called when an item is added
UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI|Events")
void OnItemAddedToInventory(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);

// Called when an item is removed
UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI|Events")
void OnItemRemovedFromInventory(UInventoryItem* Item, int32 Quantity, int32 SlotIndex);

// Called when an item is used
UFUNCTION(BlueprintImplementableEvent, Category = "Inventory UI|Events")
void OnItemUsedFromInventory(UInventoryItem* Item, AActor* User, bool bSuccess);
```

**Blueprint Implementation**:
```
Event OnInventoryUpdated
? Clear Item Slots
? For Each Item in Inventory
   - Create Slot Widget
   - Set Item Data
   - Add to Grid Panel

Event OnItemAddedToInventory
? Play Add Animation
? Update Slot Display
? Play Sound Effect
```

#### UShopWidgetBase

```cpp
// Called when shop inventory changes
UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
void OnShopInventoryUpdated();

// Called when customer inventory changes
UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
void OnCustomerInventoryUpdated();

// Called when item is purchased
UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
void OnItemPurchased(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice);

// Called when item is sold
UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
void OnItemSold(UItemDataAsset* Item, int32 Quantity, int32 TotalPrice);

// Called when purchase fails
UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
void OnPurchaseFailed(UItemDataAsset* Item, EInventoryOperationResult Reason);

// Called when sale fails
UFUNCTION(BlueprintImplementableEvent, Category = "Shop UI|Events")
void OnSaleFailed(UItemDataAsset* Item, EInventoryOperationResult Reason);
```

**Blueprint Implementation**:
```
Event OnItemPurchased
? Play Purchase Sound
? Show Confirmation Message
? Play Gold Decrease Animation

Event OnPurchaseFailed
? Branch (Reason)
   InsufficientSpace: "Inventory Full!"
   CustomCondition: "Not Enough Gold!"
? Play Error Sound
```

#### UItemTooltipWidget

```cpp
// Called when tooltip is updated
UFUNCTION(BlueprintImplementableEvent, Category = "Item Tooltip|Events")
void OnTooltipUpdated();

// Called when tooltip is cleared
UFUNCTION(BlueprintImplementableEvent, Category = "Item Tooltip|Events")
void OnTooltipCleared();
```

---

## Event Flow Examples

### Example 1: Item Usage Flow

```
Player ? UseItem()
? InventoryComponent validates
? OnItemUsed event fires
   ? HandleItemUsed()
      ? ItemEffectExecutor executes effects
         ? ModifyNeed() via interface
            ? Health increases
   ? UI updates via OnInventoryUpdated
   ? If consumable, item removed
      ? OnItemRemoved event fires
```

### Example 2: Purchase Flow

```
Player ? PurchaseItem()
? ShopComponent validates
   - Check stock
   - Check currency
   - Check requirements
? If success:
   - Remove currency from buyer
   - Add item to buyer inventory
      ? OnItemAdded event fires (buyer inventory)
   - Update shop stock
   ? OnItemPurchased event fires
   ? UI shows confirmation
      ? Shopkeeper dialogue
   ? OnShopInventoryChanged event fires
      ? UI refreshes shop display
```

### Example 3: Equipment Flow

```
Player ? EquipItem()
? InventoryComponent validates
? Unequip current item in slot (if any)
   ? OnItemEquipped(Item, false) fires
  ? Stop WhileEquipped effects
? Equip new item
   ? OnItemEquipped(Item, true) fires
      ? Start WhileEquipped effects
       ? Repeating effect timers start
      ? Update character mesh
      ? Update stats
? OnInventoryChanged fires
   ? UI refreshes
```

---

## Best Practices

### 1. Bind Events in BeginPlay
```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (InventoryComponent)
    {
        InventoryComponent->OnItemAdded.AddDynamic(this, &AMyCharacter::HandleItemAdded);
 InventoryComponent->OnItemUsed.AddDynamic(this, &AMyCharacter::HandleItemUsed);
    // ... etc
 }
}
```

### 2. Unbind on Destroy
```cpp
void AMyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (InventoryComponent)
    {
        InventoryComponent->OnItemAdded.RemoveDynamic(this, &AMyCharacter::HandleItemAdded);
    // ... etc
    }
    
    Super::EndPlay(EndPlayReason);
}
```

### 3. Check for nullptr
```cpp
void AMyCharacter::HandleItemAdded(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
    if (!Item || !Item->ItemData)
    {
return;
    }
    
    // Process event...
}
```

### 4. Use Lambda for Simple Cases
```cpp
InventoryComponent->OnItemAdded.AddDynamic(this, [](UInventoryItem* Item, int32 Qty, int32 Slot)
{
    UE_LOG(LogTemp, Log, TEXT("Item added!"));
});
```

### 5. Separate UI Logic
```cpp
// DON'T do gameplay logic in UI events
Event OnItemAdded (in UI widget)
? ? Modify player stats
? ? Execute effects
? ? Update visual display only

// DO use gameplay events for logic
Event OnItemAdded (in Character)
? ? Execute effects
? ? Modify stats
? ? Trigger dialogue
```

---

## Event Chaining Example

Complete flow for using a health potion:

```cpp
// 1. Player presses use button
void AMyCharacter::OnUseItemPressed()
{
    UInventoryItem* SelectedItem = GetSelectedItem();
    
    // 2. Call UseItem
    EInventoryOperationResult Result;
    bool bUsed = InventoryComponent->UseItem(SelectedItem->ItemInstanceId, this, Result);
}

// 3. OnItemUsed event fires
void AMyCharacter::HandleItemUsed(UInventoryItem* Item, AActor* User, bool bSuccess)
{
    if (!bSuccess) return;
    
    // 4. Execute effects
    if (EffectExecutor)
    {
        EffectExecutor->ExecuteItemEffects(Item, EItemEffectTrigger::OnUse, this);
  }
    
    // 5. Effects modify needs
    // ? ModifyNeed() called via INeedsSystemInterface
    // ? Health increases
    
    // 6. If consumable, item is removed
    // ? OnItemRemoved event fires
}

// 7. OnItemRemoved event
void AMyCharacter::HandleItemRemoved(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
    // Update UI
    // Play sound
}

// 8. OnInventoryChanged event
void AMyCharacter::HandleInventoryChanged()
{
  // Refresh full inventory display
    // Auto-save
}
```

---

## Debugging Events

### Enable Event Logging
```cpp
void AMyCharacter::HandleItemAdded(UInventoryItem* Item, int32 Quantity, int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("[EVENT] OnItemAdded: %s x%d at slot %d"), 
        *Item->GetDisplayName().ToString(), Quantity, SlotIndex);
    
    // Stack trace for debugging
    UE_LOG(LogTemp, Log, TEXT("Callstack:"));
    FDebug::DumpStackTraceToLog();
}
```

### Check Event Binding
```cpp
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (InventoryComponent)
    {
        int32 NumBindings = InventoryComponent->OnItemAdded.GetAllObjects().Num();
        UE_LOG(LogTemp, Log, TEXT("OnItemAdded has %d bindings"), NumBindings);
    }
}
```

---

**For more examples, see ExampleUsage.cpp in the plugin directory.**
