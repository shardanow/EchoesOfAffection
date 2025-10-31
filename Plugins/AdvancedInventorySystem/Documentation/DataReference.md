# Data Reference - Advanced Inventory System

Complete reference for all Data Assets, their properties, and default values.

---

## Table of Contents

1. [Item Data Asset](#item-data-asset)
2. [Item Effect Data Asset](#item-effect-data-asset)
3. [Shop Data Asset](#shop-data-asset)
4. [Structures Reference](#structures-reference)

---

## Item Data Asset

**Class:** `UItemDataAsset`  
**Base Class:** `UPrimaryDataAsset`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/ItemDataAsset.h`

**Create:** Content Browser ? Miscellaneous ? Data Asset ? ItemDataAsset

---

### Basic Information

| Property | Type | Default | Description | Required? |
|----------|------|---------|-------------|-----------|
| **Item ID** | FName | Empty | Unique identifier (MUST be unique!) | ? YES |
| **Item Name** | FText | "New Item" | Display name shown in UI | ? YES |
| **Item Description** | FText | "Item description" | Detailed description (multiline) | Recommended |
| **Item Icon** | UTexture2D* | null | Icon texture for UI display | Recommended |
| **Rarity** | EItemRarity | Common | Rarity level (Common/Uncommon/Rare/Epic/Legendary/Unique) | No |
| **Category** | EItemCategory | Miscellaneous | Item category (Consumable/Equipment/QuestItem/etc.) | No |
| **Item Tags** | FGameplayTagContainer | Empty | Gameplay tags for filtering/searching | No |

**Important Notes:**
- `Item ID` **MUST be globally unique** across all items
- Use descriptive IDs like `Potion_Health_Small`, not `Item001`
- `Item Name` and `Item Description` are localized (support multiple languages)

**Example:**
```
Item ID: Weapon_Sword_Iron
Item Name: "Iron Sword"
Item Description: "A sturdy iron blade forged by the village blacksmith."
Rarity: Uncommon
Category: Equipment
Item Tags: Item.Type.Weapon, Item.WeaponType.Melee
```

---

### Stacking Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Max Stack Size** | int32 | 1 | Maximum items per stack (0 = infinite) |
| **Is Stackable** | bool | false | Can this item stack with others? |
| **Item Weight** | float | 0.0 | Weight per single unit |

**Rules:**
- If `Is Stackable = false`, item cannot stack regardless of `Max Stack Size`
- If `Is Stackable = true` and `Max Stack Size = 0`, items stack infinitely
- Weight is per-unit: Total weight = `Item Weight ? Quantity`

**Examples:**

**Stackable Consumable:**
```
Max Stack Size: 10
Is Stackable: ?
Item Weight: 0.1
```

**Non-Stackable Equipment:**
```
Max Stack Size: 1
Is Stackable: ?
Item Weight: 5.0
```

**Infinite Stackable Currency:**
```
Max Stack Size: 0
Is Stackable: ?
Item Weight: 0.0
```

---

### Usage Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Is Usable** | bool | false | Can the item be used/consumed? |
| **Consume On Use** | bool | false | Is item destroyed after use? |
| **Use Cooldown** | float | 0.0 | Cooldown in seconds between uses |
| **Use Action Text** | FText | "Use" | Custom button text (e.g., "Drink", "Read") |

**Combinations:**

| Is Usable | Consume On Use | Use Case |
|-----------|----------------|----------|
| ? | ? | Consumable potion (destroyed after use) |
| ? | ? | Reusable tool (infinite uses) |
| ? | ? | Passive item (just held in inventory) |

**Examples:**

**Consumable Potion:**
```
Is Usable: ?
Consume On Use: ?
Use Cooldown: 0.0
Use Action Text: "Drink"
```

**Reusable Lantern:**
```
Is Usable: ?
Consume On Use: ?
Use Cooldown: 1.0
Use Action Text: "Toggle Light"
```

---

### Equipment Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Is Equippable** | bool | false | Can this item be equipped? |
| **Equipment Slot** | FGameplayTag | Empty | Slot tag (e.g., "Equipment.Slot.Head") |

**Setup Steps:**
1. Enable `Is Equippable`
2. Define equipment slots in **Project Settings ? Gameplay Tags**
3. Assign slot tag to item

**Slot Tag Examples:**
```
Equipment.Slot.Head
Equipment.Slot.Body
Equipment.Slot.Hands
Equipment.Slot.Feet
Equipment.Slot.MainHand
Equipment.Slot.OffHand
Equipment.Slot.Ring1
Equipment.Slot.Accessory
```

**Example:**
```
Is Equippable: ?
Equipment Slot: Equipment.Slot.Head
```

---

### Quest & Readable Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Is Quest Item** | bool | false | Is this a quest item? |
| **Quest Tags** | FGameplayTagContainer | Empty | Associated quest tags |
| **Is Readable** | bool | false | Can this item be read (books, notes)? |
| **Readable Content** | FText | Empty | Text content for readable items (multiline) |

**Quest Item Example:**
```
Is Quest Item: ?
Quest Tags: Quest.MainQuest.FindTheArtifact
```

**Book Example:**
```
Is Readable: ?
Readable Content: "Chapter 1: The Beginning\n\nOnce upon a time..."
```

---

### Trading Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Base Value** | int32 | 0 | Base price/value in currency |
| **Can Be Sold** | bool | true | Can player sell this to shops? |
| **Can Be Bought** | bool | true | Can player buy this from shops? |
| **Can Be Gifted** | bool | true | Can player gift this to NPCs? |
| **Sellback Percentage** | float | 0.5 | Percentage of value when selling (0.0-1.0) |
| **Is Currency** | bool | false | Is this item used as currency? |

**Trading Rules:**
- Currency items (`Is Currency = true`) typically have `Can Be Sold = false`
- Quest items often have `Can Be Sold = false` to prevent accidental sales
- `Sellback Percentage` of 0.5 means you get 50% of value when selling

**Examples:**

**Normal Item:**
```
Base Value: 100
Can Be Sold: ?
Can Be Bought: ?
Sellback Percentage: 0.5 (get 50 gold when selling)
```

**Currency (Gold):**
```
Base Value: 1
Can Be Sold: ?
Can Be Bought: ?
Is Currency: ?
```

**Quest Item:**
```
Base Value: 0
Can Be Sold: ?
Can Be Bought: ?
Can Be Gifted: ?
```

---

### Physical Representation

Used when dropping items into the world.

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Static Mesh** | UStaticMesh* | null | Mesh for world representation |
| **Skeletal Mesh** | USkeletalMesh* | null | Alternative skeletal mesh |
| **Actor Class** | TSubclassOf<AActor> | null | Custom actor to spawn (highest priority) |
| **Location Offset** | FVector | (0,0,0) | Position offset from spawn point |
| **Rotation Offset** | FRotator | (0,0,0) | Rotation offset |
| **Scale** | FVector | (1,1,1) | Mesh scale |
| **Can Be Picked Up** | bool | true | Can be picked up from world? |
| **Can Be Dropped** | bool | true | Can be dropped into world? |
| **Pickup Interaction Text** | FText | "Pick up" | Text shown when hovering |
| **Interaction Radius** | float | 150.0 | Pickup detection range (cm) |
| **Interaction Duration** | float | 0.0 | Time to hold for pickup (0 = instant) |
| **Interaction Priority** | int32 | 0 | Priority when multiple items overlap |

**Priority System:**
- **Actor Class** (if set, overrides meshes)
- **Skeletal Mesh** (if Actor Class not set)
- **Static Mesh** (fallback)

**Example:**
```
Static Mesh: SM_HealthPotion
Location Offset: (0, 0, 5)
Scale: (1, 1, 1)
Can Be Picked Up: ?
Can Be Dropped: ?
Pickup Interaction Text: "Pick up Health Potion"
Interaction Radius: 150.0
Interaction Duration: 0.0 (instant)
```

---

### Requirements

Control who can use/equip this item.

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Required Tags** | FGameplayTagContainer | Empty | Actor MUST have these tags |
| **Blocked Tags** | FGameplayTagContainer | Empty | Actor must NOT have these tags |
| **Minimum Level** | int32 | 0 | Minimum level to use (0 = no requirement) |

**Example: High-Level Warrior Sword:**
```
Required Tags: Character.Class.Warrior
Blocked Tags: Status.Cursed, Status.Disarmed
Minimum Level: 10
```

---

### Item Effects

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Item Effects** | TArray<UItemEffectDataAsset*> | Empty | Effects that trigger on various events |

**Add effects by:**
1. Creating **ItemEffectDataAsset** (see next section)
2. Adding to this array

**Example:**
```
Item Effects:
  [0]: DA_Effect_RestoreHealth
  [1]: DA_Effect_BuffStrength
  [2]: DA_Effect_TriggerDialogue
```

---

### Helper Functions (Blueprint-Callable)

#### Can Stack With

```cpp
bool CanStackWith(const UItemDataAsset* OtherItem) const;
```

**Returns:** `true` if items can stack together.

**Checks:**
- Both items are stackable
- Both have same `Item ID`

#### Get Effective Max Stack Size

```cpp
int32 GetEffectiveMaxStackSize() const;
```

**Returns:** Actual max stack size (handles infinite stacking).

**Logic:**
```
If not stackable: return 1
If stackable and MaxStackSize > 0: return MaxStackSize
If stackable and MaxStackSize == 0: return MAX_INT
```

#### Meets Requirements

```cpp
bool MeetsRequirements(AActor* Actor) const;
```

**Returns:** `true` if actor meets requirements to use this item.

**Checks:**
- Required tags
- Blocked tags
- Minimum level (requires custom interface)

#### Get Effects By Trigger

```cpp
TArray<UItemEffectDataAsset*> GetEffectsByTrigger(EItemEffectTrigger TriggerType) const;
```

**Returns:** All effects matching the trigger type.

**Example:**
```cpp
TArray<UItemEffectDataAsset*> UseEffects = ItemData->GetEffectsByTrigger(EItemEffectTrigger::OnUse);
```

---

## Item Effect Data Asset

**Class:** `UItemEffectDataAsset`  
**Base Class:** `UDataAsset`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/ItemEffectDataAsset.h`

**Create:** Content Browser ? Miscellaneous ? Data Asset ? ItemEffectDataAsset

---

### Basic Information

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Effect ID** | FName | Empty | Unique identifier for this effect |
| **Effect Name** | FText | Empty | Display name |
| **Effect Description** | FText | Empty | What the effect does (multiline) |

---

### Trigger Settings

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Trigger Type** | EItemEffectTrigger | OnUse | When this effect triggers |
| **Target Type** | EItemEffectTarget | Self | Who receives the effect |
| **Custom Target Selector** | TSubclassOf<UObject> | null | Custom target selection class |

**Trigger Types:**

| Value | When Fired | Persistent? |
|-------|------------|-------------|
| OnAcquire | Item added to inventory | No |
| OnLose | Item removed from inventory | No |
| OnUse | Player uses item | No |
| OnGift | Item gifted to NPC | No |
| WhileOwned | Continuously while in inventory | Yes |
| WhileEquipped | Continuously while equipped | Yes |

**Target Types:**

| Value | Target Actor |
|-------|--------------|
| Self | Owner/user of the item |
| NPC | The target NPC (for gifts) |
| CustomSelector | Use `Custom Target Selector` class |

---

### Execution Conditions

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Required Tags** | FGameplayTagContainer | Empty | Target must have these tags |
| **Blocked Tags** | FGameplayTagContainer | Empty | Target must NOT have these tags |
| **Minimum Level** | int32 | 0 | Minimum level requirement |
| **Maximum Level** | int32 | 0 | Maximum level requirement (0 = no max) |
| **Custom Condition Name** | FName | Empty | Blueprint-checkable condition |

---

### Effect Types

#### Needs Modifications

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Needs Modifications** | TArray<FNeedsModification> | Empty | Modify player needs |

**FNeedsModification Struct:**

| Field | Type | Description |
|-------|------|-------------|
| Need Tag | FGameplayTag | Which need (e.g., "Needs.Hunger") |
| Modification Amount | float | Amount to add/subtract |
| bClamp Value | bool | Clamp to valid range? |

**Example:**
```
Needs Modifications:
  [0]:
    Need Tag: Needs.Hunger
    Modification Amount: 50.0
    bClamp Value: ?
  [1]:
    Need Tag: Needs.Thirst
    Modification Amount: 30.0
    bClamp Value: ?
```

#### Dialogue Triggers

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Dialogue Triggers** | TArray<FDialogueTrigger> | Empty | Trigger dialogues |

**FDialogueTrigger Struct:**

| Field | Type | Description |
|-------|------|-------------|
| Dialogue Tag | FGameplayTag | Which dialogue to trigger |
| bCheck Conditions | bool | Check dialogue conditions first? |

**Example:**
```
Dialogue Triggers:
  [0]:
    Dialogue Tag: Dialogue.NPC.Shopkeeper.Thanks
    bCheck Conditions: ?
```

#### Tag Modifications

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Tag Modifications** | TArray<FTagModification> | Empty | Add/remove tags from target |

**FTagModification Struct:**

| Field | Type | Description |
|-------|------|-------------|
| Tags To Add | FGameplayTagContainer | Tags to add |
| Tags To Remove | FGameplayTagContainer | Tags to remove |
| Duration | float | Duration in seconds (0 = permanent) |

**Example:**
```
Tag Modifications:
  [0]:
    Tags To Add: Status.Buff.SpeedBoost
    Tags To Remove: [Empty]
    Duration: 30.0 (30 seconds)
```

#### Custom Effect

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Custom Effect Class** | TSubclassOf<UObject> | null | Custom Blueprint effect class |
| **Custom Effect Parameters** | FCustomEffectParameters | Empty | Parameters to pass |

**FCustomEffectParameters Struct:**

| Field | Type | Description |
|-------|------|-------------|
| String Parameter | FString | String value |
| Float Parameter | float | Float value |
| Int Parameter | int32 | Integer value |
| Bool Parameter | bool | Boolean value |
| Tag Parameter | FGameplayTag | Tag value |
| Object Parameter | UObject* | Object reference |

---

### Execution Settings

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Execution Delay** | float | 0.0 | Delay before executing (seconds) |
| **Execution Count** | int32 | 1 | How many times to execute (0 = infinite for persistent) |
| **Execution Interval** | float | 0.0 | Interval between executions (seconds) |

**Examples:**

**Instant One-Time:**
```
Execution Delay: 0.0
Execution Count: 1
Execution Interval: 0.0
```

**Delayed Damage:**
```
Execution Delay: 2.0 (2 second delay)
Execution Count: 1
Execution Interval: 0.0
```

**Poison (Damage Over Time):**
```
Execution Delay: 0.0
Execution Count: 10 (10 ticks)
Execution Interval: 1.0 (every 1 second)
Total Duration: 10 seconds
```

**Persistent Aura:**
```
Trigger Type: WhileEquipped
Execution Delay: 0.0
Execution Count: 0 (infinite)
Execution Interval: 1.0 (every 1 second)
```

---

### Helper Functions

#### Can Execute

```cpp
bool CanExecute(AActor* Target) const;
```

**Returns:** `true` if conditions are met for execution.

#### Is Persistent Effect

```cpp
bool IsPersistentEffect() const;
```

**Returns:** `true` if trigger is `WhileOwned` or `WhileEquipped`.

#### Is Repeating Effect

```cpp
bool IsRepeatingEffect() const;
```

**Returns:** `true` if `Execution Interval > 0`.

---

## Shop Data Asset

**Class:** `UShopDataAsset`  
**Base Class:** `UDataAsset`  
**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemTrade/Public/ShopDataAsset.h`

**Create:** Content Browser ? Miscellaneous ? Data Asset ? ShopDataAsset

---

### Basic Information

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Shop Name** | FText | Empty | Display name of the shop |
| **Shop Description** | FText | Empty | Shop description (flavor text) |

---

### Currency & Pricing

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Currency Item** | UItemDataAsset* | null | The item used as currency |
| **Price Modifier** | float | 1.0 | Global price multiplier (1.0 = normal, 1.5 = 50% markup) |
| **Sellback Percentage** | float | 0.5 | How much players get when selling (0.0-1.0) |

**Price Calculation:**
```
Purchase Price = (Item Base Value or Shop Entry Price) 
  ? ShopData.Price Modifier 
  ? ShopComponent.Discount Modifier
```

**Sellback Price:**
```
Sellback = Item.Base Value ? Sellback Percentage
```

---

### Buying Policy

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Buys Items** | bool | true | Does shop buy items from players? |
| **Buyable Item Tags** | FGameplayTagContainer | Empty | Tags of items shop will buy (empty = all) |

**Logic:**
- If `Buys Items = false`, shop won't buy anything
- If `Buyable Item Tags` is empty, shop buys all items
- If `Buyable Item Tags` has tags, shop only buys items with matching tags

---

### Shop Inventory

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Shop Inventory** | TArray<FShopItemEntry> | Empty | Items available for purchase |

**FShopItemEntry Struct:**

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| Item | UItemDataAsset* | null | The item for sale |
| Purchase Price | int32 | 0 | Override price (0 = use item's Base Value) |
| Stock Quantity | int32 | -1 | Available stock (-1 = unlimited) |
| bRestocks | bool | false | Does this item restock? |
| Restock Interval | float | 86400.0 | Seconds between restocks (86400 = 24 hours) |
| Restock Quantity | int32 | 1 | How many items restock each time |
| Required Tags | FGameplayTagContainer | Empty | Player must have these tags to buy |
| Minimum Level | int32 | 0 | Minimum level to purchase |

**Example:**

```
Shop Inventory:
  [0]: Health Potion
      Item: DA_HealthPotion
  Purchase Price: 20
   Stock Quantity: 15
      bRestocks: ?
    Restock Interval: 3600.0 (1 hour)
      Restock Quantity: 5
  
  [1]: Legendary Sword
      Item: DA_LegendarySword
      Purchase Price: 10000
      Stock Quantity: 1
      bRestocks: ?
 Required Tags: Achievement.DragonSlayer
      Minimum Level: 20
```

---

## Structures Reference

### FInventoryItemId

**Purpose:** Unique identifier for item instances.

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/InventorySystemTypes.h` (lines 13-48)

**Fields:**
- `Item Guid` (FGuid) - Unique GUID

**Functions:**
- `IsValid()` ? bool
- `ToString()` ? FString
- `operator==`, `operator!=`

---

### FInventoryStack

**Purpose:** Stack information.

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/InventorySystemTypes.h` (lines 50-96)

**Fields:**
- `Quantity` (int32, default: 1) - Current quantity
- `Max Stack Size` (int32, default: 0) - Maximum (0 = no limit)

**Functions:**
- `CanAddQuantity(int32 Amount)` ? bool
- `GetRemainingCapacity()` ? int32
- `IsFull()` ? bool
- `IsEmpty()` ? bool

---

### FItemPhysicalRepresentation

**Purpose:** Physical representation configuration.

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/ItemDataAsset.h` (lines 17-68)

**Fields:** See [Physical Representation section](#physical-representation) above.

---

### FItemEffectCondition

**Purpose:** Condition for effect execution.

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/ItemEffectDataAsset.h` (lines 17-44)

**Fields:** See [Execution Conditions section](#execution-conditions) above.

---

**Next:** [Examples](Examples.md) - Ready-to-use recipes and patterns

**© 2024 Game Development Team. All Rights Reserved.**
