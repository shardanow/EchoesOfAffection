# Blueprint Reference - Advanced Inventory System

Complete reference for all Blueprint nodes, components, and functions.

---

## Table of Contents

1. [Inventory Component](#inventory-component)
2. [Shop Component](#shop-component)
3. [Item Effect Executor Component](#item-effect-executor-component)
4. [Inventory Item](#inventory-item)
5. [UI Widget Bases](#ui-widget-bases)
6. [Interfaces](#interfaces)
7. [Enumerations](#enumerations)
8. [Structures](#structures)

---

## Inventory Component

**Class:** `UInventoryComponent`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Public/InventoryComponent.h`  
**Add to:** Character, Pawn, or any Actor

### Configuration Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Max Inventory Slots** | Int32 | 20 | Maximum number of slots (0 = unlimited) |
| **Max Weight Capacity** | Float | 0.0 | Maximum weight capacity (0 = unlimited) |
| **Auto Stack** | Bool | True | Automatically stack compatible items |
| **Auto Sort** | Bool | False | Automatically sort inventory on changes |
| **Allowed Item Tags** | GameplayTagContainer | Empty | Only allow items with these tags |
| **Blocked Item Tags** | GameplayTagContainer | Empty | Block items with these tags |

---

### Functions: Adding Items

#### Add Item

```blueprint
Add Item
  Inputs:
  - Item Data (ItemDataAsset Reference)
    - Quantity (Integer)
  Outputs:
    - Return Value (InventoryItem Reference)
    - Out Result (EInventoryOperationResult)
```

**Description:** Adds an item to the inventory by data asset.

**Returns:** The inventory item instance (created or updated)

**Out Result:**
- `Success` - Item added successfully
- `Failed_InsufficientSpace` - No room in inventory
- `Failed_TagRequirementNotMet` - Item tags blocked
- `Failed_InvalidItem` - Item data is invalid

**Example:**
```blueprint
Get Component ? InventoryComponent
  ?
Add Item
  - Item Data: DA_HealthPotion
  - Quantity: 5
  - Out Result: [Result]
  ?
Branch (Result == Success)
  True ? Print "Added 5 potions"
```

#### Add Inventory Item

```blueprint
Add Inventory Item
  Inputs:
    - Item (InventoryItem Reference)
  Outputs:
    - Return Value (Boolean)
    - Out Result (EInventoryOperationResult)
```

**Description:** Adds an existing inventory item instance.

**Use Case:** Transferring items between inventories.

---

### Functions: Removing Items

#### Remove Item By Id

```blueprint
Remove Item By Id
  Inputs:
    - Item Id (FInventoryItemId)
    - Quantity (Integer) ? 0 = remove all
  Outputs:
    - Return Value (Integer) ? Actual quantity removed
  - Out Result (EInventoryOperationResult)
```

**Description:** Removes item by unique instance ID.

**Example:**
```blueprint
Remove Item By Id
  - Item Id: [MyItemId]
  - Quantity: 3
  ?
Print String ("Removed: " + ToString(ReturnValue) + " items")
```

#### Remove Item By Data

```blueprint
Remove Item By Data
  Inputs:
    - Item Data (ItemDataAsset Reference)
    - Quantity (Integer)
  Outputs:
    - Return Value (Integer) ? Actual quantity removed
    - Out Result (EInventoryOperationResult)
```

**Description:** Removes items matching the data asset.

**Note:** Removes from all matching stacks until quantity is met.

**Example:**
```blueprint
Remove Item By Data
  - Item Data: DA_Gold
  - Quantity: 50
  ?
Branch (ReturnValue >= 50)
  True ? Print "Payment successful"
  False ? Print "Insufficient gold"
```

#### Clear Inventory

```blueprint
Clear Inventory
  Inputs: None
  Outputs: None
```

**Description:** Removes ALL items from inventory. No undo!

---

### Functions: Queries

#### Get All Items

```blueprint
Get All Items
  Inputs: None
  Outputs:
    - Return Value (Array of InventoryItem References)
```

**Description:** Returns all items in the inventory.

**Example:**
```blueprint
Get All Items
  ?
ForEachLoop
  - Array Element ? [Item]
  - Print String (Item ? GetDisplayName)
```

#### Get Item By Id

```blueprint
Get Item By Id
  Inputs:
    - Item Id (FInventoryItemId)
  Outputs:
    - Return Value (InventoryItem Reference or null)
```

**Description:** Gets a specific item by its unique ID.

**Returns:** Item instance, or **null** if not found.

#### Get Item At Slot

```blueprint
Get Item At Slot
  Inputs:
    - Slot Index (Integer)
  Outputs:
    - Return Value (InventoryItem Reference or null)
```

**Description:** Gets item at a specific slot index.

**Returns:** Item at that slot, or **null** if slot is empty.

**Example:**
```blueprint
Get Item At Slot
  - Slot Index: 0
  ?
Branch (Return Value ? IsValid)
  True ? Print "Slot 0: " + GetDisplayName
  False ? Print "Slot 0 is empty"
```

#### Find Items By Data

```blueprint
Find Items By Data
  Inputs:
    - Item Data (ItemDataAsset Reference)
  Outputs:
    - Return Value (Array of InventoryItem References)
```

**Description:** Finds all items matching the data asset.

**Use Case:** Finding all instances of a specific item type.

#### Find Items By Tags

```blueprint
Find Items By Tags
  Inputs:
    - Tags (GameplayTagContainer)
    - Require All (Boolean) ? True = must have ALL tags
  Outputs:
    - Return Value (Array of InventoryItem References)
```

**Description:** Finds items with specific tags.

**Example:**
```blueprint
Find Items By Tags
  - Tags: ["Item.Type.Consumable", "Item.Category.Food"]
  - Require All: True
  ?
Returns: All food consumables
```

#### Has Item

```blueprint
Has Item
  Inputs:
    - Item Data (ItemDataAsset Reference)
    - Quantity (Integer)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if inventory contains at least the specified quantity.

**Example:**
```blueprint
Has Item
  - Item Data: DA_Key
  - Quantity: 1
  ?
Branch
  True ? Unlock Door
  False ? Print "You need a key"
```

#### Get Item Quantity

```blueprint
Get Item Quantity
  Inputs:
  - Item Data (ItemDataAsset Reference)
  Outputs:
- Return Value (Integer)
```

**Description:** Returns total quantity of an item across all stacks.

**Example:**
```blueprint
Get Item Quantity
  - Item Data: DA_Gold
  ?
Print String ("You have " + ToString(ReturnValue) + " gold")
```

#### Get Used Slots

```blueprint
Get Used Slots
  Inputs: None
  Outputs:
    - Return Value (Integer)
```

**Description:** Returns number of occupied slots.

#### Get Free Slots

```blueprint
Get Free Slots
  Inputs: None
  Outputs:
    - Return Value (Integer)
```

**Description:** Returns number of free slots.

**Returns:** `MAX_INT` if Max Inventory Slots is 0 (unlimited).

#### Is Inventory Full

```blueprint
Is Inventory Full
  Inputs: None
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if no more items can be added.

#### Is Inventory Empty

```blueprint
Is Inventory Empty
  Inputs: None
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if inventory has zero items.

---

### Functions: Usage

#### Use Item

```blueprint
Use Item
  Inputs:
 - Item Id (FInventoryItemId)
    - User (Actor Reference)
  Outputs:
    - Return Value (Boolean)
    - Out Result (EInventoryOperationResult)
```

**Description:** Uses/consumes an item.

**What Happens:**
1. Checks if item is usable
2. Checks cooldown
3. Executes item effects (trigger: `OnUse`)
4. Reduces quantity if `Consume On Use` is true
5. Updates last used time
6. Fires `OnItemUsed` event

**Example:**
```blueprint
Use Item
  - Item Id: [PotionItemId]
  - User: Self
  - Out Result: [Result]
  ?
Branch (Result == Success)
  True ? Play Animation "Drinking"
  False ? Print "Item on cooldown or not usable"
```

#### Gift Item

```blueprint
Gift Item
  Inputs:
    - Item Id (FInventoryItemId)
    - Receiver (Actor Reference)
    - Quantity (Integer)
  Outputs:
    - Return Value (Boolean)
    - Out Result (EInventoryOperationResult)
```

**Description:** Gifts item to another actor (NPC).

**What Happens:**
1. Checks if item can be gifted
2. Removes item from giver's inventory
3. Executes item effects (trigger: `OnGift`)
4. Fires `OnItemGifted` event

**Receiver Requirements:** Receiver should implement `IDialogueSystemInterface` for dialogue effects.

#### Drop Item

```blueprint
Drop Item
  Inputs:
    - Item Id (FInventoryItemId)
    - Quantity (Integer) ? 0 = drop all
    - Drop Location (Vector)
  Outputs:
    - Return Value (WorldItemActor Reference or null)
    - Out Result (EInventoryOperationResult)
```

**Description:** Drops item into the world as a physical actor.

**Returns:** Spawned world item actor, or **null** if failed.

**Requirements:** Item must have `Can Be Dropped = true` and physical representation configured.

#### Drop Item In Front

```blueprint
Drop Item In Front
  Inputs:
    - Item Id (FInventoryItemId)
    - Quantity (Integer)
    - Drop Distance (Float)
  Outputs:
    - Return Value (WorldItemActor Reference or null)
    - Out Result (EInventoryOperationResult)
```

**Description:** Drops item in front of the owning actor.

**Drop Location:** Calculated as `Actor Forward Vector * Drop Distance`.

---

### Functions: Equipment

#### Equip Item

```blueprint
Equip Item
  Inputs:
    - Item Id (FInventoryItemId)
  Outputs:
    - Return Value (Boolean)
    - Out Result (EInventoryOperationResult)
```

**Description:** Equips an item.

**What Happens:**
1. Checks if item is equippable
2. Unequips item in same slot (if any)
3. Marks item as equipped
4. Executes item effects (trigger: `WhileEquipped`)
5. Fires `OnItemEquipped` event

**Example:**
```blueprint
Equip Item
  - Item Id: [HelmetItemId]
  - Out Result: [Result]
  ?
Branch (Result == Success)
  True ? [Attach helmet mesh to character]
  False ? Print "Cannot equip"
```

#### Unequip Item

```blueprint
Unequip Item
  Inputs:
    - Item Id (FInventoryItemId)
  Outputs:
    - Return Value (Boolean)
    - Out Result (EInventoryOperationResult)
```

**Description:** Unequips an item.

**What Happens:**
1. Marks item as not equipped
2. Stops `WhileEquipped` effects
3. Fires `OnItemEquipped` event with `bEquipped = false`

#### Get Equipped Items

```blueprint
Get Equipped Items
  Inputs: None
Outputs:
    - Return Value (Array of InventoryItem References)
```

**Description:** Returns all currently equipped items.

#### Get Equipped Item In Slot

```blueprint
Get Equipped Item In Slot
  Inputs:
    - Slot Tag (GameplayTag)
  Outputs:
    - Return Value (InventoryItem Reference or null)
```

**Description:** Gets equipped item in a specific slot.

**Example:**
```blueprint
Get Equipped Item In Slot
  - Slot Tag: Equipment.Slot.Head
  ?
Branch (Return Value ? IsValid)
  True ? Print "Wearing: " + GetDisplayName
  False ? Print "No helmet equipped"
```

---

### Functions: Organization

#### Swap Items

```blueprint
Swap Items
  Inputs:
    - Slot Index A (Integer)
    - Slot Index B (Integer)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Swaps two items in inventory.

**Returns:** `true` if swap successful, `false` if slots invalid.

#### Sort Inventory

```blueprint
Sort Inventory
  Inputs: None
  Outputs: None
```

**Description:** Sorts inventory (rarity ? category ? name).

---

### Functions: Weight System

#### Get Total Weight

```blueprint
Get Total Weight
  Inputs: None
  Outputs:
    - Return Value (Float)
```

**Description:** Returns sum of all item weights (item weight ? quantity).

#### Get Remaining Weight Capacity

```blueprint
Get Remaining Weight Capacity
  Inputs: None
  Outputs:
    - Return Value (Float)
```

**Description:** Returns how much more weight can be added.

**Returns:** `MAX_FLOAT` if `Max Weight Capacity` is 0 (unlimited).

#### Get Weight Percentage

```blueprint
Get Weight Percentage
  Inputs: None
  Outputs:
 - Return Value (Float) ? 0.0 to 1.0
```

**Description:** Returns current weight / max weight.

**Use Case:** Bind to a progress bar.

#### Can Add Weight

```blueprint
Can Add Weight
  Inputs:
    - Additional Weight (Float)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if adding this weight would exceed capacity.

#### Is Overweight

```blueprint
Is Overweight
  Inputs: None
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if current weight exceeds max capacity.

#### Get Item Weight (Static)

```blueprint
Get Item Weight
  Inputs:
    - Item Data (ItemDataAsset Reference)
    - Quantity (Integer)
  Outputs:
    - Return Value (Float)
```

**Description:** Calculates weight of a quantity of items.

**Static Function:** Can be called without an inventory component.

**Example:**
```blueprint
Get Item Weight
  - Item Data: DA_IronOre
  - Quantity: 10
  ?
Returns: 50.0 (if iron ore weighs 5.0 each)
```

---

### Functions: Validation

#### Can Add Item

```blueprint
Can Add Item
  Inputs:
    - Item Data (ItemDataAsset Reference)
    - Quantity (Integer)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if item can be added (slots, weight, tags).

**Use Case:** Preview if an action would succeed before attempting it.

**Example:**
```blueprint
Can Add Item
- Item Data: DA_HeavyArmor
  - Quantity: 1
  ?
Branch
  True ? Show "Pick up" prompt
  False ? Show "Inventory full" message
```

---

### Events (Delegates)

Bind to these events to respond to inventory changes.

#### On Item Added

```blueprint
Event On Item Added
  Outputs:
    - Item (InventoryItem Reference)
    - Quantity (Integer)
    - Slot Index (Integer)
```

**Description:** Fired when an item is added.

**Example:**
```blueprint
Event Construct (Widget)
  ?
Get Component ? InventoryComponent
  ?
Bind Event to On Item Added ? HandleItemAdded

[Custom Event: HandleItemAdded]
  - Item, Quantity, SlotIndex
  ?
  Play Sound 2D ? Sound_ItemPickup
  ?
  Refresh UI
```

#### On Item Removed

```blueprint
Event On Item Removed
  Outputs:
    - Item (InventoryItem Reference)
    - Quantity (Integer)
    - Slot Index (Integer)
```

**Description:** Fired when an item is removed.

#### On Item Used

```blueprint
Event On Item Used
  Outputs:
    - Item (InventoryItem Reference)
    - User (Actor Reference)
    - bSuccess (Boolean)
```

**Description:** Fired when an item is used.

**bSuccess:** `true` if use succeeded, `false` if failed (cooldown, etc.).

#### On Item Gifted

```blueprint
Event On Item Gifted
  Outputs:
    - Item (InventoryItem Reference)
    - Giver (Actor Reference)
    - Receiver (Actor Reference)
    - bSuccess (Boolean)
```

**Description:** Fired when an item is gifted.

#### On Item Equipped

```blueprint
Event On Item Equipped
  Outputs:
    - Item (InventoryItem Reference)
    - bEquipped (Boolean)
```

**Description:** Fired when item is equipped or unequipped.

**bEquipped:** `true` = equipped, `false` = unequipped

#### On Inventory Changed

```blueprint
Event On Inventory Changed
  Outputs: None
```

**Description:** Fired on ANY inventory change (add, remove, swap, etc.).

**Use Case:** General refresh trigger.

#### On Weight Changed

```blueprint
Event On Weight Changed
  Outputs:
    - Current Weight (Float)
    - Max Weight (Float)
```

**Description:** Fired when total weight changes.

---

### Save/Load Functions

#### Save To Json

```blueprint
Save To Json
  Inputs: None
  Outputs:
    - Return Value (Boolean)
    - Out Json String (String)
```

**Description:** Serializes inventory to JSON string.

**Example:**
```blueprint
Save To Json
  - Out Json String: [JsonString]
?
Branch (Return Value)
  True ? SaveGameToSlot (JsonString, "InventorySave", 0)
  False ? Print "Save failed"
```

#### Load From Json

```blueprint
Load From Json
  Inputs:
    - Json String (String)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Deserializes inventory from JSON string.

**Warning:** Clears current inventory!

**Example:**
```blueprint
LoadGameFromSlot ("InventorySave", 0)
- Return Value ? [SaveGameObject]
  ?
Get [JsonString] from SaveGameObject
  ?
Load From Json
  - Json String: [JsonString]
  ?
Branch (Return Value)
  True ? Print "Inventory loaded"
  False ? Print "Load failed"
```

---

## Shop Component

**Class:** `UShopComponent`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemTrade/Public/ShopComponent.h`  
**Add to:** NPC Actor, Shop Actor

### Configuration Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Shop Data** | ShopDataAsset | null | The shop data asset defining inventory/prices |
| **Is Open** | Bool | True | Is shop currently open for business? |
| **Discount Modifier** | Float | 1.0 | Price multiplier (0.9 = 10% discount, 1.5 = 50% markup) |

---

### Functions: Transactions

#### Purchase Item

```blueprint
Purchase Item
  Inputs:
    - Item (ItemDataAsset Reference)
    - Quantity (Integer)
    - Buyer (Actor Reference)
  Outputs:
    - Return Value (Boolean)
    - Out Result (EInventoryOperationResult)
```

**Description:** Player purchases item from shop.

**What Happens:**
1. Checks if item is in stock
2. Calculates total price
3. Checks if buyer has enough currency
4. Removes currency from buyer's inventory
5. Adds item to buyer's inventory
6. Reduces shop stock
7. Fires `OnItemPurchased` event

**Out Result:**
- `Success` - Purchase complete
- `Failed_InsufficientQuantity` - Not enough stock
- `Failed_InvalidItem` - Item not in shop
- `Failed_InsufficientSpace` - Buyer inventory full

**Example:**
```blueprint
Purchase Item
  - Item: DA_HealthPotion
  - Quantity: 5
  - Buyer: PlayerCharacter
  - Out Result: [Result]
  ?
Branch (Result == Success)
  True ? Play Sound "Cha-ching!"
  False ? Print "Purchase failed: " + Result
```

#### Sell Item

```blueprint
Sell Item
  Inputs:
    - Item (ItemDataAsset Reference)
    - Quantity (Integer)
 - Seller (Actor Reference)
  Outputs:
    - Return Value (Boolean)
  - Out Result (EInventoryOperationResult)
```

**Description:** Player sells item to shop.

**What Happens:**
1. Checks if shop buys this item type
2. Calculates sellback price
3. Removes item from seller's inventory
4. Adds currency to seller's inventory
5. Fires `OnItemSold` event

**Example:**
```blueprint
Sell Item
  - Item: DA_IronOre
  - Quantity: 10
  - Seller: PlayerCharacter
  - Out Result: [Result]
  ?
Branch (Result == Success)
  True ? Print "Sold for " + GetSellbackPrice(Item, 10)
  False ? Print "Shop doesn't buy this item"
```

---

### Functions: Pricing

#### Get Purchase Price

```blueprint
Get Purchase Price
  Inputs:
    - Item (ItemDataAsset Reference)
    - Quantity (Integer)
 - Buyer (Actor Reference or null)
  Outputs:
    - Return Value (Integer)
```

**Description:** Calculates total purchase price for an item.

**Formula:**
```
FinalPrice = (ItemBaseValue or ShopEntryPrice)
           ? ShopData.PriceModifier
           ? ShopComponent.DiscountModifier
           ? Quantity
```

**Example:**
```blueprint
Get Purchase Price
  - Item: DA_Sword
  - Quantity: 1
  - Buyer: PlayerCharacter
  ?
Print String "Price: " + ToString(ReturnValue) + " gold"
```

#### Get Sellback Price

```blueprint
Get Sellback Price
  Inputs:
    - Item (ItemDataAsset Reference)
    - Quantity (Integer)
    - Seller (Actor Reference or null)
  Outputs:
    - Return Value (Integer)
```

**Description:** Calculates total sellback price.

**Formula:**
```
SellbackPrice = Item.BaseValue
        ? (Item.SellbackPercentage or ShopData.SellbackPercentage)
              ? Quantity
```

---

### Functions: Inventory Queries

#### Get Available Items

```blueprint
Get Available Items
  Inputs: None
  Outputs:
    - Return Value (Array of ItemDataAsset References)
```

**Description:** Returns all items available for purchase.

**Example:**
```blueprint
Get Available Items
  ?
ForEachLoop
  - Array Element ? [ShopItem]
  - Create Widget (ShopItemSlot)
    - Set Item: ShopItem
```

#### Is Item Available

```blueprint
Is Item Available
  Inputs:
    - Item (ItemDataAsset Reference)
    - Quantity (Integer)
    - Buyer (Actor Reference or null)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if item can be purchased.

**Checks:**
- Item is in shop inventory
- Enough stock
- Buyer meets level/tag requirements

#### Get Item Stock

```blueprint
Get Item Stock
  Inputs:
    - Item (ItemDataAsset Reference)
  Outputs:
    - Return Value (Integer)
```

**Description:** Returns current stock quantity.

**Returns:** `-1` if stock is unlimited.

**Example:**
```blueprint
Get Item Stock
  - Item: DA_HealthPotion
  ?
Branch (Return Value == -1)
  True ? Print "Unlimited stock"
  False ? Print "Stock: " + ToString(ReturnValue)
```

#### Will Buy Item

```blueprint
Will Buy Item
  Inputs:
    - Item (ItemDataAsset Reference)
  Outputs:
  - Return Value (Boolean)
```

**Description:** Checks if shop will buy this item type.

**Checks:**
- `ShopData.Buys Items` is true
- Item tags match `ShopData.Buyable Item Tags` (if not empty)
- Item `Can Be Sold` is true

---

### Functions: Restocking

#### Restock All

```blueprint
Restock All
  Inputs: None
  Outputs: None
```

**Description:** Manually restocks all items with `Restocks = true`.

**Use Case:** Admin commands, debug, or timed events.

#### Restock Item

```blueprint
Restock Item
  Inputs:
    - Item (ItemDataAsset Reference)
  Outputs: None
```

**Description:** Manually restocks a specific item.

---

### Events (Delegates)

#### On Item Purchased

```blueprint
Event On Item Purchased
  Outputs:
    - Item (ItemDataAsset Reference)
    - Quantity (Integer)
    - Total Price (Integer)
    - Buyer (Actor Reference)
```

**Description:** Fired when item is purchased from shop.

**Example:**
```blueprint
Bind Event to On Item Purchased ? HandlePurchase

[Custom Event: HandlePurchase]
  ?
  Play Sound 2D ? Sound_Purchase
  ?
  Print String (Buyer?GetName() + " bought " + Quantity + "x " + Item?ItemName)
```

#### On Item Sold

```blueprint
Event On Item Sold
  Outputs:
    - Item (ItemDataAsset Reference)
    - Quantity (Integer)
    - Total Price (Integer)
    - Seller (Actor Reference)
```

**Description:** Fired when item is sold to shop.

#### On Shop Inventory Changed

```blueprint
Event On Shop Inventory Changed
  Outputs: None
```

**Description:** Fired when shop stock changes (purchase, restock).

---

## Item Effect Executor Component

**Class:** `UItemEffectExecutorComponent`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemEffects/Public/ItemEffectExecutorComponent.h`  
**Add to:** Character with Inventory

**Note:** This component works automatically! It listens to inventory events and executes effects.

### Functions

#### Execute Item Effects

```blueprint
Execute Item Effects
  Inputs:
    - Item (InventoryItem Reference)
    - Trigger Type (EItemEffectTrigger)
 - Target (Actor Reference or null)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Manually execute item effects.

**Usually not needed** - the component executes effects automatically.

**Example (Manual Trigger):**
```blueprint
Execute Item Effects
  - Item: [MyInventoryItem]
  - Trigger Type: OnUse
  - Target: Self
```

#### Start Persistent Effects

```blueprint
Start Persistent Effects
  Inputs:
    - Item (InventoryItem Reference)
    - Trigger Type (EItemEffectTrigger)
  Outputs: None
```

**Description:** Starts persistent effects (`WhileOwned`, `WhileEquipped`).

**Auto-called** when items are equipped/acquired.

#### Stop Persistent Effects

```blueprint
Stop Persistent Effects
  Inputs:
    - Item Id (FInventoryItemId)
  Outputs: None
```

**Description:** Stops persistent effects for an item.

**Auto-called** when items are unequipped/lost.

---

## Inventory Item

**Class:** `UInventoryItem`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Public/InventoryItem.h`

**Note:** You don't usually create these manually - they're created by `InventoryComponent.AddItem()`.

### Properties

| Property | Type | Description |
|----------|------|-------------|
| **Item Instance Id** | FInventoryItemId | Unique ID for this instance |
| **Item Data** | ItemDataAsset | Reference to item data |
| **Stack** | FInventoryStack | Stack quantity and max size |
| **Instance Tags** | GameplayTagContainer | Instance-specific tags |
| **Custom Metadata** | Map<Name, String> | Key-value custom data |
| **bIs Equipped** | Bool | Is currently equipped? |
| **Last Used Time** | Float | Game time when last used |

### Functions: Static Creation

#### Create Inventory Item

```blueprint
Create Inventory Item
  Inputs:
    - World Context Object (Object Reference)
    - In Item Data (ItemDataAsset Reference)
 - In Quantity (Integer)
  Outputs:
    - Return Value (InventoryItem Reference)
```

**Description:** Creates a new inventory item instance.

**Use Case:** Advanced scenarios where you need to create items manually.

---

### Functions: Queries

#### Get Display Name

```blueprint
Get Display Name
  Inputs: None
  Outputs:
    - Return Value (Text)
```

**Description:** Returns the display name from item data.

#### Get Description

```blueprint
Get Description
  Inputs: None
Outputs:
    - Return Value (Text)
```

**Description:** Returns the description from item data.

#### Is Valid

```blueprint
Is Valid
  Inputs: None
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if item has valid data.

#### Can Stack With

```blueprint
Can Stack With
  Inputs:
    - Other Item (InventoryItem Reference)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if this item can stack with another.

#### Get Quantity

```blueprint
Get Quantity
  Inputs: None
  Outputs:
    - Return Value (Integer)
```

**Description:** Returns current quantity in stack.

#### Get Max Stack Size

```blueprint
Get Max Stack Size
  Inputs: None
  Outputs:
    - Return Value (Integer)
```

**Description:** Returns max stack size from item data.

#### Is Stack Full

```blueprint
Is Stack Full
  Inputs: None
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if stack is at max capacity.

#### Get Remaining Capacity

```blueprint
Get Remaining Capacity
  Inputs: None
  Outputs:
    - Return Value (Integer)
```

**Description:** Returns how many more items can be added to this stack.

#### Can Be Used

```blueprint
Can Be Used
  Inputs:
    - User (Actor Reference or null)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if item can be used right now (usable, not on cooldown, meets requirements).

#### Is On Cooldown

```blueprint
Is On Cooldown
  Inputs:
    - World (World Reference)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Checks if item is on cooldown.

#### Get Remaining Cooldown

```blueprint
Get Remaining Cooldown
  Inputs:
    - World (World Reference)
  Outputs:
    - Return Value (Float)
```

**Description:** Returns remaining cooldown time in seconds.

---

### Functions: Modification

#### Add Quantity

```blueprint
Add Quantity
  Inputs:
    - Amount (Integer)
  Outputs:
    - Return Value (Integer) ? Amount actually added
```

**Description:** Adds quantity to stack (respects max stack size).

#### Remove Quantity

```blueprint
Remove Quantity
  Inputs:
    - Amount (Integer)
  Outputs:
    - Return Value (Integer) ? Amount actually removed
```

**Description:** Removes quantity from stack.

#### Set Quantity

```blueprint
Set Quantity
Inputs:
    - New Quantity (Integer)
  Outputs: None
```

**Description:** Directly sets quantity (no validation!).

#### Set Equipped

```blueprint
Set Equipped
  Inputs:
    - bEquipped (Boolean)
  Outputs: None
```

**Description:** Marks item as equipped/unequipped.

**Note:** Usually called by `InventoryComponent.EquipItem()`.

---

### Functions: Metadata

#### Set Metadata

```blueprint
Set Metadata
  Inputs:
    - Key (Name)
    - Value (String)
  Outputs: None
```

**Description:** Stores custom metadata on this item instance.

**Example:**
```blueprint
Set Metadata
  - Key: "EnchantmentLevel"
  - Value: "5"
```

#### Get Metadata

```blueprint
Get Metadata
  Inputs:
    - Key (Name)
  Outputs:
 - Return Value (Boolean) ? True if key exists
    - Out Value (String)
```

**Description:** Retrieves metadata value.

**Example:**
```blueprint
Get Metadata
  - Key: "EnchantmentLevel"
  - Out Value: [Value]
  ?
Branch (Return Value)
  True ? Print "Enchant: " + Value
  False ? Print "No enchantment"
```

#### Remove Metadata

```blueprint
Remove Metadata
  Inputs:
    - Key (Name)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Removes metadata key.

---

### Functions: Serialization

#### To Json

```blueprint
To Json
  Inputs: None
  Outputs:
  - Return Value (String)
```

**Description:** Serializes item to JSON string.

#### From Json

```blueprint
From Json
  Inputs:
    - Json String (String)
  Outputs:
    - Return Value (Boolean)
```

**Description:** Deserializes item from JSON string.

---

## UI Widget Bases

See **[User Guide - UI Widgets Section](UserGuide.md#6-ui-widgets)** for detailed widget documentation.

### Inventory Widget Base

**Required Widgets:**
- `InventoryGrid` (Uniform Grid Panel)

**Optional Widgets:**
- `SlotsText`, `WeightText`, `WeightBar`, `SortButton`, `CloseButton`

### Inventory Slot Widget

**Required Widgets:**
- `ItemIcon` (Image)

**Optional Widgets:**
- `QuantityText`, `SlotBackground`, `RarityBorder`, `EquippedIcon`

### Item Tooltip Widget

**Required Widgets:**
- `ItemNameText` (Text Block)
- `ItemIconImage` (Image)

**Optional Widgets:**
- `ItemDescriptionText`, `ItemValueText`, `ItemRarityText`, `BackgroundBorder`

### Context Menu Widget

**Optional Widgets (Auto-Bind):**
- `UseButton`, `DropButton`, `SplitButton`, `InfoButton`, `CancelButton`

### Shop Widget Base

**Required Widgets:**
- `ShopItemsList` (Scroll Box)
- `PlayerItemsList` (Scroll Box)

**Optional Widgets:**
- `PlayerGoldText`, `ShopNameText`, `ShopKeeperText`, `CloseButton`

---

## Interfaces

### IInventoryHolder

**Purpose:** Actors that can hold inventory.

**Function:**
```blueprint
Get Inventory Component (Interface Message)
  Outputs:
    - Return Value (ActorComponent Reference)
```

### INeedsSystemInterface

**Purpose:** Integration with external Needs System.

**Functions:**
```blueprint
Modify Need (Interface Message)
  Inputs:
    - Need Tag (GameplayTag)
 - Delta Value (Float)
    - bClamp (Boolean)

Get Need Value (Interface Message)
  Inputs:
    - Need Tag (GameplayTag)
  Outputs:
    - Return Value (Float)
```

### IDialogueSystemInterface

**Purpose:** Integration with external Dialogue System.

**Functions:**
```blueprint
Trigger Dialogue (Interface Message)
  Inputs:
    - Dialogue Tag (GameplayTag)
  - Optional Context (Object Reference or null)

Check Dialogue Condition (Interface Message)
  Inputs:
    - Dialogue Tag (GameplayTag)
  Outputs:
    - Return Value (Boolean)
```

### IShopPriceModifier

**Purpose:** Custom price calculation logic.

**Functions:**
```blueprint
Calculate Price (Interface Message)
  Inputs:
    - Base Price (Integer)
- Item Data (ItemDataAsset Reference)
 - Buyer (Actor Reference)
    - Quantity (Integer)
  Outputs:
    - Return Value (Integer)

Calculate Sellback Price (Interface Message)
  Inputs:
    - Original Price (Integer)
    - Item Data (ItemDataAsset Reference)
    - Seller (Actor Reference)
    - Quantity (Integer)
  Outputs:
    - Return Value (Integer)
```

---

## Enumerations

### EItemRarity

| Value | Display Name |
|-------|--------------|
| Common | Common |
| Uncommon | Uncommon |
| Rare | Rare |
| Epic | Epic |
| Legendary | Legendary |
| Unique | Unique |

### EItemCategory

| Value | Display Name |
|-------|--------------|
| Consumable | Consumable |
| Equipment | Equipment |
| QuestItem | Quest Item |
| Crafting | Crafting Material |
| Currency | Currency |
| Readable | Readable |
| Key | Key |
| Miscellaneous | Miscellaneous |

### EInventoryOperationResult

| Value | Meaning |
|-------|---------|
| Success | Operation succeeded |
| Failed_ItemNotFound | Item not found in inventory |
| Failed_InsufficientSpace | No room in inventory |
| Failed_InsufficientQuantity | Not enough of the item |
| Failed_InvalidItem | Item data is invalid/null |
| Failed_TagRequirementNotMet | Item tags blocked by inventory |
| Failed_CustomCondition | Custom condition failed |
| Failed_StackLimitReached | Stack limit reached |
| Failed_Unknown | Unknown error |

### EItemEffectTrigger

| Value | When it Fires |
|-------|---------------|
| OnAcquire | When item is added to inventory |
| OnLose | When item is removed from inventory |
| OnUse | When player uses the item |
| OnGift | When item is gifted to an NPC |
| WhileOwned | Continuously while in inventory (persistent) |
| WhileEquipped | Continuously while equipped (persistent) |

### EItemEffectTarget

| Value | Who Receives Effect |
|-------|---------------------|
| Self | The owner/user of the item |
| NPC | The target NPC (for gifts) |
| CustomSelector | Custom target selection logic |

---

## Structures

### FInventoryItemId

**Purpose:** Unique identifier for item instances.

**Fields:**
- `Item Guid` (FGuid) - Unique GUID

**Functions:**
- `Is Valid()` - Checks if GUID is valid
- `ToString()` - Converts to string

### FInventoryStack

**Purpose:** Stack information.

**Fields:**
- `Quantity` (Int32) - Current quantity
- `Max Stack Size` (Int32) - Maximum (0 = no limit)

**Functions:**
- `Can Add Quantity(Amount)` - Can add this many?
- `Get Remaining Capacity()` - How many more can fit?
- `Is Full()` - Is stack at max?
- `Is Empty()` - Is quantity zero?

### FNeedsModification

**Purpose:** Needs effect definition.

**Fields:**
- `Need Tag` (GameplayTag) - Which need to modify
- `Modification Amount` (Float) - Amount to add/subtract
- `bClamp Value` (Bool) - Clamp to valid range?

### FDialogueTrigger

**Purpose:** Dialogue effect definition.

**Fields:**
- `Dialogue Tag` (GameplayTag) - Which dialogue to trigger
- `bCheck Conditions` (Bool) - Check dialogue conditions first?

### FTagModification

**Purpose:** Tag effect definition.

**Fields:**
- `Tags To Add` (GameplayTagContainer) - Tags to add
- `Tags To Remove` (GameplayTagContainer) - Tags to remove
- `Duration` (Float) - Duration in seconds (0 = permanent)

---

**Next:** [Data Reference](DataReference.md) - All Data Assets and properties

**© 2024 Game Development Team. All Rights Reserved.**
