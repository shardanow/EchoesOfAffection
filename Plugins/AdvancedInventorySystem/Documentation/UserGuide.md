# User Guide - Advanced Inventory System

Complete step-by-step instructions for using the Advanced Inventory System in Unreal Engine.

---

## Table of Contents

1. [Creating Items](#1-creating-items)
2. [Setting Up Inventory](#2-setting-up-inventory)
3. [Equipment System](#3-equipment-system)
4. [Item Effects](#4-item-effects)
5. [Shop System](#5-shop-system)
6. [UI Widgets](#6-ui-widgets)
7. [Save & Load](#7-save--load)
8. [Multiplayer](#8-multiplayer)

---

## 1. Creating Items

Items are defined using **ItemDataAsset** files. These are data-only assets that require no code.

### 1.1 Creating an Item Data Asset

**Steps:**

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Select **ItemDataAsset** from the list
3. Name it using convention: `DA_[ItemName]` (e.g., `DA_HealthPotion`)
4. Double-click to open

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/ItemDataAsset.h`

### 1.2 Basic Item Properties

Every item has these core properties:

| Property | Type | Description | Default |
|----------|------|-------------|---------|
| **Item ID** | Name | Unique identifier (MUST be unique!) | Empty |
| **Item Name** | Text | Display name shown in UI | "New Item" |
| **Item Description** | Text | Detailed description (multiline) | "Item description" |
| **Item Icon** | Texture2D | Icon image for UI | None |
| **Rarity** | Enum | Common/Uncommon/Rare/Epic/Legendary/Unique | Common |
| **Category** | Enum | Consumable/Equipment/QuestItem/Crafting/Currency/Readable/Key/Miscellaneous | Miscellaneous |
| **Item Tags** | GameplayTagContainer | Tags for filtering/searching | Empty |

**Important:** `Item ID` MUST be unique across all items! Use descriptive names like `Potion_Health_Small`.

### 1.3 Stacking Properties

Controls how items stack in inventory:

| Property | Type | Description |
|----------|------|-------------|
| **Max Stack Size** | Int32 | Maximum items per stack (0 = infinite) |
| **Is Stackable** | Bool | Can this item stack? |
| **Item Weight** | Float | Weight per single unit (for weight limits) |

**Example: Stackable Consumable**
```
Max Stack Size: 10
Is Stackable: ?
Item Weight: 0.1
```

**Example: Non-Stackable Equipment**
```
Max Stack Size: 1
Is Stackable: ?
Item Weight: 5.0
```

### 1.4 Usage Properties

Controls how items can be used:

| Property | Type | Description |
|----------|------|-------------|
| **Is Usable** | Bool | Can the item be used/consumed? |
| **Consume On Use** | Bool | Is item destroyed after use? |
| **Use Cooldown** | Float | Cooldown in seconds between uses |
| **Use Action Text** | Text | Custom button text (e.g., "Drink", "Eat") |

**Example: Consumable Potion**
```
Is Usable: ?
Consume On Use: ?
Use Cooldown: 0.0
Use Action Text: "Drink"
```

**Example: Reusable Tool**
```
Is Usable: ?
Consume On Use: ?
Use Cooldown: 5.0
Use Action Text: "Use Tool"
```

### 1.5 Equipment Properties

For items that can be equipped:

| Property | Type | Description |
|----------|------|-------------|
| **Is Equippable** | Bool | Can this item be equipped? |
| **Equipment Slot** | GameplayTag | Slot tag (e.g., "Equipment.Slot.Head") |

**Setup Equipment Slots:**

1. **Project Settings** ? **Project ? Gameplay Tags**
2. Add tags:
   ```
   Equipment.Slot.Head
   Equipment.Slot.Body
   Equipment.Slot.Hands
   Equipment.Slot.Feet
   Equipment.Slot.Weapon
   Equipment.Slot.Shield
   ```

**Example: Helmet**
```
Is Equippable: ?
Equipment Slot: Equipment.Slot.Head
```

### 1.6 Trading Properties

For shop and trading systems:

| Property | Type | Description |
|----------|------|-------------|
| **Base Value** | Int32 | Base price in currency |
| **Can Be Sold** | Bool | Can player sell this item? |
| **Can Be Bought** | Bool | Can player buy this from shops? |
| **Can Be Gifted** | Bool | Can player gift this to NPCs? |
| **Sellback Percentage** | Float (0-1) | Percentage of value when selling (0.5 = 50%) |
| **Is Currency** | Bool | Is this item used as currency? |

**Example: Valuable Item**
```
Base Value: 100
Can Be Sold: ?
Can Be Bought: ?
Sellback Percentage: 0.5
```

**Example: Quest Item (No Trading)**
```
Base Value: 0
Can Be Sold: ?
Can Be Bought: ?
Can Be Gifted: ?
```

### 1.7 Quest & Readable Properties

| Property | Type | Description |
|----------|------|-------------|
| **Is Quest Item** | Bool | Is this a quest item? |
| **Quest Tags** | GameplayTagContainer | Associated quest tags |
| **Is Readable** | Bool | Can this item be read (books, notes)? |
| **Readable Content** | Text | Text content for readable items |

**Example: Quest Note**
```
Is Quest Item: ?
Quest Tags: Quest.MainQuest.FindTheKey
Is Readable: ?
Readable Content: "The key is hidden under the old oak tree..."
```

### 1.8 Physical Representation

For dropping items into the world:

| Property | Type | Description |
|----------|------|-------------|
| **Static Mesh** | StaticMesh | Mesh for world representation |
| **Skeletal Mesh** | SkeletalMesh | Alternative skeletal mesh |
| **Actor Class** | TSubclassOf<AActor> | Custom actor to spawn (highest priority) |
| **Location Offset** | Vector | Position offset from spawn point |
| **Rotation Offset** | Rotator | Rotation offset |
| **Scale** | Vector | Mesh scale |
| **Can Be Picked Up** | Bool | Can be picked up from world? |
| **Can Be Dropped** | Bool | Can be dropped into world? |
| **Pickup Interaction Text** | Text | Text shown when hovering |
| **Interaction Radius** | Float | Pickup detection range |
| **Interaction Duration** | Float | Time to hold for pickup (0 = instant) |

**Example: Droppable Item**
```
Static Mesh: SM_Apple_Mesh
Location Offset: (0, 0, 0)
Scale: (1, 1, 1)
Can Be Picked Up: ?
Can Be Dropped: ?
Pickup Interaction Text: "Pick up Apple"
Interaction Radius: 150.0
```

### 1.9 Requirements

Control who can use this item:

| Property | Type | Description |
|----------|------|-------------|
| **Required Tags** | GameplayTagContainer | Tags actor MUST have |
| **Blocked Tags** | GameplayTagContainer | Tags that PREVENT use |
| **Minimum Level** | Int32 | Minimum level to use |

**Example: High-Level Item**
```
Required Tags: Character.Class.Warrior
Blocked Tags: Status.Cursed
Minimum Level: 10
```

---

## 2. Setting Up Inventory

### 2.1 Adding Inventory Component to an Actor

**Blueprint Steps:**

1. Open your **Character Blueprint** or **Pawn Blueprint**
2. **Components Panel** ? Click **Add** button
3. Search for `Inventory Component`
4. Select **InventoryComponent**
5. The component appears in your hierarchy

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Public/InventoryComponent.h`

### 2.2 Configuring Inventory Component

Select the **InventoryComponent** and configure in **Details Panel**:

#### Basic Configuration

| Property | Description | Recommended Value |
|----------|-------------|-------------------|
| **Max Inventory Slots** | Maximum number of slots (0 = unlimited) | 20-40 for players, 10 for NPCs |
| **Max Weight Capacity** | Maximum weight (0 = unlimited) | 100.0 or 0 for no limit |
| **Auto Stack** | Automatically stack compatible items | ? (checked) |
| **Auto Sort** | Automatically sort inventory | ? (usually off) |

#### Tag Filters (Optional)

| Property | Description | Use Case |
|----------|-------------|----------|
| **Allowed Item Tags** | Only items with these tags can be added | Restrict to specific categories |
| **Blocked Item Tags** | Items with these tags cannot be added | Prevent quest items in storage chests |

**Example: Player Inventory**
```
Max Inventory Slots: 30
Max Weight Capacity: 100.0
Auto Stack: ?
Auto Sort: ?
Allowed Item Tags: [Empty - allow all]
Blocked Item Tags: [Empty]
```

**Example: Quest Item Storage**
```
Max Inventory Slots: 10
Max Weight Capacity: 0
Auto Stack: ?
Allowed Item Tags: Quest.Items
```

### 2.3 Adding Items (Blueprint)

#### Method 1: Add by Data Asset

```blueprint
[Your Event]
  ?
Get Component ? InventoryComponent
  ?
Add Item
  - Item Data: DA_HealthPotion
  - Quantity: 1
  - Out Result: [OutResult variable]
  ?
Branch (OutResult == Success)
  True ? [Item added successfully]
  False ? [Show error message]
```

**Blueprint Node:** `Add Item`  
**Parameters:**
- `Item Data` (ItemDataAsset): The item to add
- `Quantity` (Int32): How many to add
- `Out Result` (EInventoryOperationResult): Success/Failure code

**Returns:** `UInventoryItem*` (the created/updated item instance)

#### Method 2: Add Existing Inventory Item

```blueprint
[Your Event]
  ?
Add Inventory Item
  - Item: [YourInventoryItemInstance]
  - Out Result: [OutResult]
```

### 2.4 Removing Items (Blueprint)

#### Remove by Instance ID

```blueprint
[Your Event]
  ?
Remove Item By Id
  - Item Id: [FInventoryItemId]
  - Quantity: 1 (or 0 for all)
  - Out Result: [OutResult]
```

**Returns:** Int32 (number of items actually removed)

#### Remove by Data Asset

```blueprint
[Your Event]
  ?
Remove Item By Data
  - Item Data: DA_HealthPotion
  - Quantity: 3
  - Out Result: [OutResult]
```

#### Clear All Items

```blueprint
[Your Event]
  ?
Clear Inventory
```

### 2.5 Querying Inventory (Blueprint)

#### Get All Items

```blueprint
Get All Items
  ?
Returns: Array of UInventoryItem*
  ?
ForEachLoop
  - Array Element: [Individual Item]
  - Do something with each item
```

#### Check If Has Item

```blueprint
Has Item
  - Item Data: DA_Key
  - Quantity: 1
  ?
Returns: Bool (True if has at least that quantity)
```

#### Get Item Quantity

```blueprint
Get Item Quantity
  - Item Data: DA_Gold
  ?
Returns: Int32 (total quantity across all stacks)
```

#### Find Items by Tags

```blueprint
Find Items By Tags
  - Tags: [GameplayTagContainer with "Consumable.Potion"]
  - Require All: True
  ?
Returns: Array of UInventoryItem*
```

#### Get Item at Slot

```blueprint
Get Item At Slot
  - Slot Index: 5
  ?
Returns: UInventoryItem* (or nullptr if empty)
```

### 2.6 Inventory Events (Delegates)

**Bind to these events in Blueprint:**

#### On Item Added

```blueprint
Event Construct (or BeginPlay)
  ?
Get Component ? InventoryComponent
  ?
Bind Event to On Item Added
  - Event: [Create Custom Event: HandleItemAdded]

[Custom Event: HandleItemAdded]
Inputs:
  - Item (UInventoryItem*)
  - Quantity (Int32)
  - Slot Index (Int32)
```

#### Available Events:

| Event | Parameters | When Fired |
|-------|------------|------------|
| **On Item Added** | Item, Quantity, SlotIndex | Item added to inventory |
| **On Item Removed** | Item, Quantity, SlotIndex | Item removed from inventory |
| **On Item Used** | Item, User, bSuccess | Item was used |
| **On Item Gifted** | Item, Giver, Receiver, bSuccess | Item was gifted to NPC |
| **On Item Equipped** | Item, bEquipped | Item equipped/unequipped |
| **On Inventory Changed** | (none) | Any inventory change |
| **On Weight Changed** | CurrentWeight, MaxWeight | Inventory weight changed |

**Event Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Public/InventoryComponent.h` (lines 29-35)

### 2.7 Weight System

#### Check Weight

```blueprint
Get Total Weight
  ?
Returns: Float (current total weight)

Get Remaining Weight Capacity
  ?
Returns: Float (how much more weight can be added)

Get Weight Percentage
  ?
Returns: Float (0.0 - 1.0, current weight / max weight)

Is Overweight
  ?
Returns: Bool (True if over capacity)
```

#### Calculate Item Weight

```blueprint
Get Item Weight (Static Function)
  - Item Data: DA_Sword
  - Quantity: 1
?
Returns: Float (total weight of that quantity)
```

---

## 3. Equipment System

### 3.1 Setting Up Equipment Slots

**First, define your equipment slots as Gameplay Tags:**

1. **Project Settings** ? **Project ? Gameplay Tags**
2. Click **Add New Gameplay Tag**
3. Add your slots:

```
Equipment.Slot.Head
Equipment.Slot.Body
Equipment.Slot.Hands
Equipment.Slot.Feet
Equipment.Slot.MainHand
Equipment.Slot.OffHand
Equipment.Slot.Ring1
Equipment.Slot.Ring2
Equipment.Slot.Accessory
```

### 3.2 Making an Item Equippable

Open your **ItemDataAsset** (e.g., `DA_IronHelmet`):

```
Is Equippable: ? (checked)
Equipment Slot: Equipment.Slot.Head
```

### 3.3 Equipping Items (Blueprint)

```blueprint
[Equip Button Pressed]
  ?
Get Component ? InventoryComponent
  ?
Equip Item
  - Item Id: [The item's FInventoryItemId]
  - Out Result: [OutResult]
  ?
Branch (OutResult == Success)
  True ? Print String "Equipped!"
  False ? Print String "Failed to equip"
```

**What Happens Automatically:**
1. If another item is in that slot, it's unequipped first
2. The item is marked as equipped
3. `OnItemEquipped` event fires
4. Item effects with trigger `WhileEquipped` are activated

### 3.4 Unequipping Items (Blueprint)

```blueprint
[Unequip Button Pressed]
  ?
Unequip Item
  - Item Id: [FInventoryItemId]
  - Out Result: [OutResult]
```

### 3.5 Querying Equipped Items

#### Get All Equipped Items

```blueprint
Get Equipped Items
  ?
Returns: Array of UInventoryItem*
  ?
ForEachLoop
  - Process each equipped item
```

#### Get Item in Specific Slot

```blueprint
Get Equipped Item In Slot
  - Slot Tag: Equipment.Slot.Head
  ?
Returns: UInventoryItem* (or nullptr if slot is empty)
```

### 3.6 Equipment Example: Sword with Stats

**DA_IronSword:**
```
Item ID: Weapon_Sword_Iron
Item Name: "Iron Sword"
Rarity: Uncommon
Category: Equipment
Is Equippable: ?
Equipment Slot: Equipment.Slot.MainHand
Item Effects: [Add effect that modifies attack stats]
```

You can add custom metadata to store stats:

```blueprint
[When Item is Equipped]
  ?
Get Inventory Item
  ?
Set Metadata
  - Key: "AttackDamage"
  - Value: "15"
```

---

## 4. Item Effects

Item effects allow items to trigger actions when used, acquired, equipped, etc.

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/ItemEffectDataAsset.h`

### 4.1 Creating an Item Effect Data Asset

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Select **ItemEffectDataAsset**
3. Name it: `DA_Effect_[EffectName]` (e.g., `DA_Effect_RestoreHunger`)

### 4.2 Effect Basic Properties

| Property | Type | Description |
|----------|------|-------------|
| **Effect ID** | Name | Unique identifier for this effect |
| **Effect Name** | Text | Display name |
| **Effect Description** | Text | What the effect does |
| **Trigger Type** | Enum | When the effect triggers (see below) |
| **Target Type** | Enum | Who receives the effect (Self/NPC/CustomSelector) |

### 4.3 Trigger Types

| Trigger | When it Fires | Use Case |
|---------|---------------|----------|
| **OnAcquire** | When item is first added to inventory | Welcome gift, curse |
| **OnLose** | When item is removed from inventory | Penalty for dropping quest item |
| **OnUse** | When player uses/consumes the item | Potion restoring health |
| **OnGift** | When item is gifted to an NPC | Trigger dialogue, relationship change |
| **WhileOwned** | Continuously while in inventory | Passive aura effect |
| **WhileEquipped** | Continuously while equipped | Armor stats bonus |

### 4.4 Effect Types

#### A. Needs Modifications

Modify player needs (hunger, thirst, energy, etc.).

**Struct:** `FNeedsModification`

| Field | Type | Description |
|-------|------|-------------|
| **Need Tag** | GameplayTag | Which need to modify (e.g., "Needs.Hunger") |
| **Modification Amount** | Float | Amount to add/subtract (negative = reduce) |
| **Clamp Value** | Bool | Clamp to valid range (usually ?) |

**Example: Food Item Effect**
```
Trigger Type: OnUse
Target Type: Self
Needs Modifications:
  - Need Tag: Needs.Hunger
    Modification Amount: 50.0
    Clamp Value: ?
  - Need Tag: Needs.Energy
 Modification Amount: 10.0
  Clamp Value: ?
```

**Integration Required:** Your character must implement `INeedsSystemInterface`.

#### B. Dialogue Triggers

Trigger dialogue when item is used or gifted.

**Struct:** `FDialogueTrigger`

| Field | Type | Description |
|-------|------|-------------|
| **Dialogue Tag** | GameplayTag | Which dialogue to trigger |
| **Check Conditions** | Bool | Check dialogue conditions first? |

**Example: Gift Effect**
```
Trigger Type: OnGift
Target Type: NPC
Dialogue Triggers:
  - Dialogue Tag: Dialogue.NPC.Shopkeeper.GiftReceived
    Check Conditions: ?
```

**Integration Required:** Your NPC must implement `IDialogueSystemInterface`.

#### C. Tag Modifications

Add or remove gameplay tags from the target.

**Struct:** `FTagModification`

| Field | Type | Description |
|-------|------|-------------|
| **Tags To Add** | GameplayTagContainer | Tags to add to target |
| **Tags To Remove** | GameplayTagContainer | Tags to remove from target |
| **Duration** | Float | Duration in seconds (0 = permanent) |

**Example: Temporary Buff Effect**
```
Trigger Type: OnUse
Target Type: Self
Tag Modifications:
  - Tags To Add: Status.Buff.SpeedBoost
  Tags To Remove: [Empty]
    Duration: 30.0 (30 seconds)
```

#### D. Custom Blueprint Effects

For complex behavior, use custom Blueprint logic.

| Field | Type | Description |
|-------|------|-------------|
| **Custom Effect Class** | TSubclassOf<UObject> | Your custom Blueprint class |
| **Custom Effect Parameters** | FCustomEffectParameters | Parameters to pass |

**Custom Effect Parameters:**
- `String Parameter` (String)
- `Float Parameter` (Float)
- `Int Parameter` (Int32)
- `Bool Parameter` (Bool)
- `Tag Parameter` (GameplayTag)
- `Object Parameter` (UObject*)

**Example: Custom Effect**
```
Trigger Type: OnUse
Custom Effect Class: BP_ExplosionEffect
Custom Effect Parameters:
  Float Parameter: 100.0 (explosion radius)
  Int Parameter: 50 (damage amount)
```

### 4.5 Effect Conditions

Control when effects can execute:

| Field | Type | Description |
|-------|------|-------------|
| **Required Tags** | GameplayTagContainer | Target must have these tags |
| **Blocked Tags** | GameplayTagContainer | Target must NOT have these tags |
| **Minimum Level** | Int32 | Minimum level requirement |
| **Maximum Level** | Int32 | Maximum level requirement |
| **Custom Condition Name** | Name | Blueprint-checkable condition |

**Example: Level-Gated Effect**
```
Required Tags: Character.Class.Mage
Blocked Tags: Status.Silenced
Minimum Level: 5
Maximum Level: 0 (no max)
```

### 4.6 Effect Execution Settings

| Field | Type | Description |
|-------|------|-------------|
| **Execution Delay** | Float | Delay before executing (seconds) |
| **Execution Count** | Int32 | How many times to execute (0 = infinite for persistent) |
| **Execution Interval** | Float | Interval between executions (for repeating) |

**Example: Poison Effect (Repeating)**
```
Trigger Type: OnUse
Target Type: NPC
Execution Count: 10
Execution Interval: 1.0 (every 1 second)
Needs Modifications:
  - Need Tag: Needs.Health
    Modification Amount: -5.0 (damage over time)
```

### 4.7 Adding Effects to Items

Open your **ItemDataAsset** and scroll to **Item Effects**:

```
Item Effects:
  [0]: DA_Effect_RestoreHunger
  [1]: DA_Effect_RestoreThirst
  [2]: DA_Effect_TriggerDialogue
```

Click **+** to add more effects.

### 4.8 Effect Execution (Automatic)

Effects are executed automatically by the **ItemEffectExecutorComponent**.

**You don't need to do anything manually!** The component listens to inventory events and executes effects at the right time.

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemEffects/Public/ItemEffectExecutorComponent.h`

---

## 5. Shop System

### 5.1 Creating a Shop Data Asset

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Select **ShopDataAsset**
3. Name it: `DA_Shop_[ShopName]` (e.g., `DA_Shop_GeneralStore`)

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemTrade/Public/ShopDataAsset.h`

### 5.2 Shop Basic Properties

| Property | Type | Description |
|----------|------|-------------|
| **Shop Name** | Text | Display name of the shop |
| **Shop Description** | Text | Shop description (flavor text) |
| **Currency Item** | ItemDataAsset | The item used as currency (e.g., DA_Gold) |
| **Price Modifier** | Float | Global price multiplier (1.0 = normal, 1.5 = 50% more expensive) |
| **Sellback Percentage** | Float (0-1) | How much players get when selling (0.5 = 50% of value) |
| **Buys Items** | Bool | Does this shop buy items from players? |
| **Buyable Item Tags** | GameplayTagContainer | Tags of items shop will buy (empty = all) |

**Example: General Store**
```
Shop Name: "Oakwood General Store"
Shop Description: "Everything you need for your adventure!"
Currency Item: DA_Gold
Price Modifier: 1.0
Sellback Percentage: 0.5
Buys Items: ?
Buyable Item Tags: [Empty - buys everything]
```

**Example: Weapon Shop (Only Buys Weapons)**
```
Shop Name: "The Blacksmith"
Currency Item: DA_Gold
Price Modifier: 1.2
Sellback Percentage: 0.4
Buys Items: ?
Buyable Item Tags: Item.Type.Weapon, Item.Type.Armor
```

### 5.3 Shop Inventory (Items for Sale)

Add items to **Shop Inventory** array. Each entry is a `FShopItemEntry`:

| Field | Type | Description |
|-------|------|-------------|
| **Item** | ItemDataAsset | The item for sale |
| **Purchase Price** | Int32 | Override price (0 = use item's Base Value) |
| **Stock Quantity** | Int32 | Available stock (-1 = unlimited) |
| **Restocks** | Bool | Does this item restock over time? |
| **Restock Interval** | Float | Seconds between restocks |
| **Restock Quantity** | Int32 | How many items restock each time |
| **Required Tags** | GameplayTagContainer | Player must have these tags to buy |
| **Minimum Level** | Int32 | Minimum level to purchase |

**Example Shop Inventory:**

```
Shop Inventory:
  [0] Health Potion
      Item: DA_HealthPotion
      Purchase Price: 10
      Stock Quantity: 20
    Restocks: ?
      Restock Interval: 3600.0 (1 hour)
      Restock Quantity: 10
  
  [1] Iron Sword
   Item: DA_IronSword
      Purchase Price: 100
  Stock Quantity: 5
      Restocks: ?
      Restock Interval: 86400.0 (24 hours)
      Restock Quantity: 1
  
  [2] Legendary Artifact
      Item: DA_LegendaryRing
      Purchase Price: 10000
      Stock Quantity: 1
      Restocks: ?
 Required Tags: Achievement.CompletedMainQuest
    Minimum Level: 20
```

### 5.4 Adding Shop Component to NPC

1. Open your **NPC Blueprint** (shopkeeper)
2. **Components Panel** ? Add **Shop Component**
3. In **Details Panel**:
   ```
   Shop Data: DA_Shop_GeneralStore
   Is Open: ?
   Discount Modifier: 1.0 (or adjust for sales)
   ```

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemTrade/Public/ShopComponent.h`

### 5.5 Opening a Shop (Blueprint)

```blueprint
[Player Interacts with Shopkeeper]
  ?
Get Component ? ShopComponent (from NPC)
  ?
Create Widget ? WBP_ShopUI
  ?
Bind To Shop
  - Shop Component: [ShopComponent from NPC]
  - Customer Inventory: [Player's InventoryComponent]
  ?
Show Shop
```

### 5.6 Shop Transactions (Blueprint)

#### Purchase Item

```blueprint
[Buy Button Clicked]
  ?
Shop Component ? Purchase Item
  - Item: DA_HealthPotion
- Quantity: 5
  - Buyer: [Player Character]
  - Out Result: [OutResult]
  ?
Branch (OutResult == Success)
  True ? Print "Purchase successful!"
  False ? Print "Purchase failed!"
```

**What Happens:**
1. Checks if player has enough currency
2. Checks if item is in stock
3. Removes currency from player inventory
4. Adds item to player inventory
5. Reduces shop stock
6. Fires `OnItemPurchased` event

#### Sell Item

```blueprint
[Sell Button Clicked]
  ?
Shop Component ? Sell Item
  - Item: DA_IronOre
  - Quantity: 10
  - Seller: [Player Character]
  - Out Result: [OutResult]
  ?
Branch (OutResult == Success)
  True ? Print "Sold!"
  False ? Print "Shop doesn't buy this item"
```

**What Happens:**
1. Checks if shop buys this item type
2. Removes item from player inventory
3. Adds currency to player inventory (based on sellback %)
4. Fires `OnItemSold` event

### 5.7 Shop Pricing (Blueprint)

#### Get Purchase Price

```blueprint
Get Purchase Price
  - Item: DA_Sword
  - Quantity: 1
  - Buyer: [Player]
  ?
Returns: Int32 (total price)
```

**Formula:**
```
FinalPrice = (Item.BaseValue or ShopEntry.PurchasePrice) 
      ? ShopData.PriceModifier 
           ? ShopComponent.DiscountModifier
      ? Quantity
```

#### Get Sellback Price

```blueprint
Get Sellback Price
  - Item: DA_Armor
  - Quantity: 1
  - Seller: [Player]
  ?
Returns: Int32 (total sellback amount)
```

**Formula:**
```
SellbackPrice = Item.BaseValue 
         ? Item.SellbackPercentage 
        ? Quantity
```

### 5.8 Shop Queries (Blueprint)

```blueprint
Get Available Items
  ?
Returns: Array of ItemDataAsset*

Is Item Available
  - Item: DA_Potion
  - Quantity: 5
  - Buyer: [Player]
  ?
Returns: Bool

Get Item Stock
  - Item: DA_Sword
  ?
Returns: Int32 (-1 = unlimited)

Will Buy Item
  - Item: DA_Junk
  ?
Returns: Bool (True if shop accepts this item)
```

### 5.9 Manual Restocking

```blueprint
[Admin Command: Restock All]
  ?
Shop Component ? Restock All

[Admin Command: Restock Specific Item]
  ?
Shop Component ? Restock Item
  - Item: DA_HealthPotion
```

**Note:** Auto-restocking happens automatically based on `Restock Interval` in game time.

### 5.10 Shop Events

Bind to shop events to update UI:

| Event | Parameters | When Fired |
|-------|------------|------------|
| **On Item Purchased** | Item, Quantity, TotalPrice, Buyer | Item purchased |
| **On Item Sold** | Item, Quantity, TotalPrice, Seller | Item sold |
| **On Shop Inventory Changed** | (none) | Shop stock changed |

---

## 6. UI Widgets

### 6.1 Inventory Widget

**Base Class:** `UInventoryWidgetBase`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemUI/Public/InventoryWidgetBase.h`

#### Creating an Inventory Widget

1. **Content Browser** ? Right-click ? **User Interface ? Widget Blueprint**
2. Name it: `WBP_InventoryUI`
3. **File ? Reparent Blueprint**
4. Select **InventoryWidgetBase** as parent class

#### Required Widgets (MUST have these or compilation error!)

Add to Designer with **exact names**:

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Uniform Grid Panel** | `InventoryGrid` | Holds inventory slots (REQUIRED) |

#### Recommended Widgets (Optional but useful)

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Text Block** | `SlotsText` | Shows "5 / 20" used slots |
| **Text Block** | `WeightText` | Shows current weight |
| **Text Block** | `MaxWeightText` | Shows max weight capacity |
| **Progress Bar** | `WeightBar` | Weight progress bar |
| **Button** | `SortButton` | Sort inventory (auto-binds!) |
| **Button** | `CloseButton` | Close inventory (auto-binds!) |
| **Text Block** | `TitleText` | "Inventory" title |

#### Widget Configuration

In **Class Settings** ? **Details Panel**:

| Property | Description | Default |
|----------|-------------|---------|
| **Tooltip Widget Class** | Your tooltip BP (e.g., WBP_ItemTooltip) | None |
| **Slot Widget Class** | Your slot BP (e.g., WBP_InventorySlot) | None |
| **Grid Columns** | Number of columns in grid | 5 |
| **Slot Size** | Size of each slot (px) | 80.0 |
| **Tooltip Offset** | Offset from cursor | (10, 10) |
| **Context Menu Class** | Context menu BP (e.g., WBP_ContextMenu) | None |

#### Blueprint Usage

```blueprint
[Open Inventory Key Pressed]
  ?
Create Widget ? WBP_InventoryUI
  ?
Bind To Inventory
  - Inventory Component: [GetComponentByClass(InventoryComponent)]
  ?
Show Inventory (auto adds to viewport, sets input mode)
```

**Close Inventory:**

```blueprint
[Close Button or ESC Pressed]
  ?
Hide Inventory (auto removes from viewport, restores input)
```

#### Inventory Widget Events (Override in Blueprint)

| Event | When Called | Use For |
|-------|-------------|---------|
| **On Inventory Updated** | Any inventory change | Refresh display |
| **On Item Added To Inventory** | Item added | Play sound, animation |
| **On Item Removed From Inventory** | Item removed | Update UI |
| **On Item Used From Inventory** | Item used | Show feedback |

**Example:**

```blueprint
Event On Item Added To Inventory
  Inputs: Item, Quantity, SlotIndex
  ?
Play Sound 2D ? Sound_ItemPickup
  ?
Print String ? "Added {Item.GetDisplayName()}"
```

### 6.2 Inventory Slot Widget

**Base Class:** `UInventorySlotWidget`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemUI/Public/InventorySlotWidget.h`

#### Creating a Slot Widget

1. **Content Browser** ? Right-click ? **User Interface ? Widget Blueprint**
2. Name it: `WBP_InventorySlot`
3. **File ? Reparent Blueprint** ? **InventorySlotWidget**

#### Required Widgets

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Image** | `ItemIcon` | Item icon display (REQUIRED) |

#### Recommended Widgets

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Text Block** | `QuantityText` | Stack quantity |
| **Border** | `SlotBackground` | Background/highlight |
| **Border** | `RarityBorder` | Rarity color indicator |
| **Image** | `EquippedIcon` | "E" icon for equipped items |
| **Image** | `LockedIcon` | Lock icon for locked slots |

#### Slot Widget Events

| Event | When Called |
|-------|-------------|
| **On Slot Clicked** | Left mouse click |
| **On Slot Hovered** | Mouse enters slot |
| **On Slot Unhovered** | Mouse leaves slot |
| **On Slot Data Updated** | Slot data changed |
| **On Slot Cleared** | Slot cleared |
| **On Slot Right Clicked** | Right mouse click (return true to show context menu) |

**Example: Highlight on Hover**

```blueprint
Event On Slot Hovered
  ?
Slot Background ? Set Brush Color
  - Color: (1, 1, 0, 0.5) - Yellow highlight
```

```blueprint
Event On Slot Unhovered
  ?
Slot Background ? Set Brush Color
  - Color: (1, 1, 1, 0.2) - Normal
```

### 6.3 Item Tooltip Widget

**Base Class:** `UItemTooltipWidget`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemUI/Public/ItemTooltipWidget.h`

#### Creating a Tooltip Widget

1. **Content Browser** ? Right-click ? **User Interface ? Widget Blueprint**
2. Name it: `WBP_ItemTooltip`
3. **File ? Reparent Blueprint** ? **ItemTooltipWidget**

#### Required Widgets

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Text Block** | `ItemNameText` | Item name (REQUIRED) |
| **Image** | `ItemIconImage` | Item icon (REQUIRED) |

#### Recommended Widgets

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Text Block** | `ItemDescriptionText` | Item description |
| **Text Block** | `ItemValueText` | Item value/price |
| **Text Block** | `ItemRarityText` | Rarity name |
| **Border** | `BackgroundBorder` | Background with rarity color |

#### Optional Widgets

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Text Block** | `ItemWeightText` | Item weight |
| **Text Block** | `ItemCategoryText` | Category name |
| **Text Block** | `QuantityText` | Stack quantity |
| **Text Block** | `ItemTypeText` | "Stackable", "Usable", etc. |
| **Image** | `QuestItemIcon` | Quest item indicator |

**The tooltip updates automatically when hovering over items!**

#### Tooltip Events

| Event | When Called |
|-------|-------------|
| **On Tooltip Updated** | Tooltip data changed |
| **On Tooltip Cleared** | Tooltip cleared |

### 6.4 Context Menu Widget

**Base Class:** `UInventoryContextMenuWidget`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemUI/Public/InventoryContextMenuWidget.h`

#### Creating a Context Menu

1. **Content Browser** ? Right-click ? **User Interface ? Widget Blueprint**
2. Name it: `WBP_ContextMenu`
3. **File ? Reparent Blueprint** ? **InventoryContextMenuWidget**

#### Optional Buttons (Auto-Bind!)

Add any of these buttons - they automatically work:

| Button Name (EXACT!) | Action | When Shown |
|----------------------|--------|------------|
| `UseButton` | Use/Consume item | If item is usable |
| `DropButton` | Drop item | Always |
| `SplitButton` | Split stack | If item is stackable |
| `InfoButton` | Show item info | Always |
| `CancelButton` | Close menu | Always |

**Just add the buttons you want!** The system auto-binds them.

#### Context Menu Events (Override)

```blueprint
Event On Use Clicked
?
[Your custom behavior]
  ?
[Base implementation handles Use automatically]
```

### 6.5 Shop Widget

**Base Class:** `UShopWidgetBase`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemUI/Public/ShopWidgetBase.h`

#### Creating a Shop Widget

1. **Content Browser** ? Right-click ? **User Interface ? Widget Blueprint**
2. Name it: `WBP_ShopUI`
3. **File ? Reparent Blueprint** ? **ShopWidgetBase**

#### Required Widgets

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Scroll Box** | `ShopItemsList` | Shop inventory (REQUIRED) |
| **Scroll Box** | `PlayerItemsList` | Player inventory (REQUIRED) |

#### Recommended Widgets

| Widget Type | Name (EXACT!) | Purpose |
|-------------|---------------|---------|
| **Text Block** | `PlayerGoldText` | Player's currency amount |
| **Text Block** | `ShopNameText` | Shop name |
| **Text Block** | `ShopKeeperText` | Shopkeeper name (optional) |
| **Button** | `CloseButton` | Close shop (auto-binds!) |

#### Shop Widget Usage

```blueprint
[Player Opens Shop]
  ?
Create Widget ? WBP_ShopUI
  ?
Bind To Shop
  - Shop Component: [NPC's ShopComponent]
  - Customer Inventory: [Player's InventoryComponent]
  ?
Show Shop
```

#### Shop Widget Events

| Event | Parameters | Use For |
|-------|------------|---------|
| **On Shop Inventory Updated** | - | Refresh shop items |
| **On Customer Inventory Updated** | - | Refresh player items |
| **On Item Purchased** | Item, Quantity, TotalPrice | Play sound, feedback |
| **On Item Sold** | Item, Quantity, TotalPrice | Play sound, feedback |
| **On Purchase Failed** | Item, Reason | Show error message |
| **On Sale Failed** | Item, Reason | Show error message |

---

## 7. Save & Load

### 7.1 Saving Inventory to JSON

```blueprint
[Save Game Event]
  ?
Get Component ? InventoryComponent
  ?
Save To Json
  - Out Json String: [SavedJsonString variable]
  ?
Branch (Return Value == True)
  True ? [Save JsonString to SaveGame file]
  False ? Print "Save failed"
```

**JSON String Example:**
```json
{
  "items": [
    {
      "itemId": "Potion_Health",
   "quantity": 5,
      "instanceId": "A3B2C1D4..."
    }
  ]
}
```

### 7.2 Loading Inventory from JSON

```blueprint
[Load Game Event]
  ?
[Load JsonString from SaveGame file]
  ?
Get Component ? InventoryComponent
  ?
Load From Json
  - Json String: [SavedJsonString]
  ?
Branch (Return Value == True)
  True ? Print "Load successful"
  False ? Print "Load failed"
```

### 7.3 Inventory Item Serialization

Individual items can also be saved:

```blueprint
[Save Single Item]
  ?
Inventory Item ? To Json
  ?
Returns: String (JSON representation)

[Load Single Item]
  ?
Inventory Item ? From Json
  - Json String: [JsonString]
  ?
Returns: Bool (True if successful)
```

---

## 8. Multiplayer

### 8.1 Replication Setup

**InventoryComponent** is already replication-ready!

#### Server-Authority Model

All inventory operations MUST happen on the **Server**:

```blueprint
[Client: Pick Up Item Event]
  ?
Custom Event: ServerPickUpItem (Run On Server, Reliable)
  - ItemData: DA_Apple
  ?
[Server executes]
  ?
Add Item (Server only)
  ?
[Automatically replicates to clients]
```

#### Creating Server RPC

1. Create a **Custom Event** in your Character BP
2. **Details Panel** ? **Replicates** ? **Run on Server**
3. Check **Reliable**
4. Add logic to modify inventory

**Example:**

```blueprint
Custom Event: ServerAddItem (Run On Server, Reliable)
Inputs:
  - Item Data (ItemDataAsset Reference)
  - Quantity (Integer)
  ?
Get Component ? InventoryComponent
  ?
Add Item
    - Item Data: [Passed Item Data]
    - Quantity: [Passed Quantity]
```

### 8.2 Client Prediction

For responsive gameplay, you can show **optimistic UI updates** on the client:

```blueprint
[Client: Use Item Button]
  ?
Play Animation (Drinking potion - immediate feedback)
  ?
Custom Event: ServerUseItem (Run On Server, Reliable)
  - Item Id: [ItemId]
  ?
[Server processes, replicates result]
```

### 8.3 Networking Best Practices

1. **Always call inventory functions on Server**
2. **Listen to events on all clients** (events replicate automatically)
3. **Use OnItemAdded/OnItemRemoved** to update UI on all clients
4. **Don't trust client input** - validate on server

---

## Next Steps

- ?? **[Tutorials](Tutorials.md)** - Hands-on tutorials with step-by-step screenshots
- ?? **[Blueprint Reference](BlueprintReference.md)** - Complete Blueprint node reference
- ?? **[Data Reference](DataReference.md)** - All Data Asset properties explained
- ?? **[Examples](Examples.md)** - Ready-to-use recipes and patterns
- ? **[FAQ & Troubleshooting](FAQ_Troubleshooting.md)** - Common issues and solutions

---

**© 2024 Game Development Team. All Rights Reserved.**
