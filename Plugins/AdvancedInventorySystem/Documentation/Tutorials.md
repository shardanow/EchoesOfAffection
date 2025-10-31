# Tutorials - Advanced Inventory System

Hands-on tutorials to master the inventory system. Each tutorial is self-contained and builds practical features.

---

## Table of Contents

1. [Tutorial 1: Health Potion - Item Creation to Use](#tutorial-1-health-potion---from-creation-to-use)
2. [Tutorial 2: Wearable Armor - Equipment System](#tutorial-2-wearable-armor---equipment-system)
3. [Tutorial 3: General Store - Complete Shop](#tutorial-3-general-store---complete-shop-system)
4. [Tutorial 4: Interactive Inventory UI](#tutorial-4-interactive-inventory-ui)
5. [Tutorial 5: Gift System with Dialogue](#tutorial-5-gift-system-with-dialogue-integration)

---

## Tutorial 1: Health Potion - From Creation to Use

**Goal:** Create a health potion that restores the player's hunger when used.

**Time:** ~15 minutes  
**Difficulty:** ? Beginner

### Step 1: Create Currency Item (Gold)

We need currency for the shop system.

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Choose **ItemDataAsset**
3. Name: `DA_Gold`
4. Open and configure:

```
=== BASIC ===
Item ID: Currency_Gold
Item Name: "Gold Coin"
Item Description: "Universal currency used throughout the realm."
Rarity: Common
Category: Currency

=== STACKING ===
Max Stack Size: 999
Is Stackable: ?
Item Weight: 0.0 (weightless)

=== TRADING ===
Base Value: 1
Can Be Sold: ? (currency can't be sold)
Can Be Bought: ?
Is Currency: ?
```

5. **Save**

### Step 2: Create Health Potion Item

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Choose **ItemDataAsset**
3. Name: `DA_HealthPotion`
4. Open and configure:

```
=== BASIC ===
Item ID: Potion_Health_Small
Item Name: "Health Potion"
Item Description: "A magical red potion that restores your vitality."
Item Icon: [Optional: Import a potion icon texture]
Rarity: Common
Category: Consumable
Item Tags: [Add tag: "Item.Type.Potion"]

=== STACKING ===
Max Stack Size: 10
Is Stackable: ?
Item Weight: 0.2

=== USAGE ===
Is Usable: ?
Consume On Use: ?
Use Cooldown: 1.0 (1 second cooldown)
Use Action Text: "Drink"

=== TRADING ===
Base Value: 15
Can Be Sold: ?
Can Be Bought: ?
Sellback Percentage: 0.5
```

5. **Save**

### Step 3: Create Health Restore Effect

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Choose **ItemEffectDataAsset**
3. Name: `DA_Effect_RestoreHealth`
4. Open and configure:

```
=== BASIC ===
Effect ID: Effect_RestoreHealth
Effect Name: "Restore Health"
Effect Description: "Restores 50 hunger points."

=== TRIGGER ===
Trigger Type: OnUse
Target Type: Self

=== CONDITIONS ===
[Leave empty - no conditions needed]

=== EFFECT TYPES ===
Needs Modifications:
  [0]:
    Need Tag: Needs.Hunger
    Modification Amount: 50.0
    Clamp Value: ?

=== EXECUTION ===
Execution Delay: 0.0
Execution Count: 1
Execution Interval: 0.0
```

5. **Save**

**Note:** If you don't have a Needs System, you can skip the effect for now or create a custom Blueprint effect.

### Step 4: Link Effect to Potion

1. Open `DA_HealthPotion`
2. Scroll to **Item Effects** section
3. Click **+** to add array element
4. Select `DA_Effect_RestoreHealth`
5. **Save**

### Step 5: Add Inventory to Player Character

1. Open your **Character Blueprint** (e.g., `BP_ThirdPersonCharacter`)
2. **Components Panel** ? Click **Add** ? Search `Inventory Component`
3. Add **InventoryComponent**
4. Select it and configure in **Details**:

```
Max Inventory Slots: 20
Max Weight Capacity: 100.0
Auto Stack: ?
Auto Sort: ?
```

5. **Compile & Save**

### Step 6: Test: Add Potion to Inventory

1. In your Character BP's **Event Graph**
2. Add this test code:

```blueprint
Event BeginPlay
  ?
Delay (1.0 seconds) ? Wait for game to fully initialize
  ?
Get Component By Class
  - Component Class: InventoryComponent
  - Return Value ? [InventoryComp variable]
  ?
Add Item
  - Target: [InventoryComp]
  - Item Data: DA_HealthPotion
  - Quantity: 3
  - Out Result ? [Result variable]
  ?
Branch (Condition: Result == Success)
  True ? Print String "Added 3 Health Potions!"
  False ? Print String "Failed to add potion"
```

3. **Compile & Save**
4. **Play In Editor** (PIE)
5. You should see: `"Added 3 Health Potions!"`

### Step 7: Test: Use the Potion

Add input handling to use the potion:

1. In **Event Graph**, add:

```blueprint
Event InputAction [YourUseKey] (e.g., "E")
  ?
Get Component By Class ? InventoryComponent
  ?
Get All Items
  - Return Value ? [ItemsArray]
  ?
ForEachLoop
- Array: [ItemsArray]
  - Array Element ? [CurrentItem]
  - Loop Body ?
  
  Branch (CurrentItem ? GetItemData ? GetItemID == "Potion_Health_Small")
    True ?
      Use Item
      - Item Id: CurrentItem ? ItemInstanceId
        - User: Self
        - Out Result: [Result]
      ?
      Branch (Result == Success)
True ? Print String "Used Health Potion!"
        False ? Print String "Failed to use potion"
      ?
      Break (exit loop)
```

2. **Compile & Save**
3. **Play**, press your use key
4. You should see: `"Used Health Potion!"` and potion count reduces to 2

**Congratulations!** You've created a working consumable item.

---

## Tutorial 2: Wearable Armor - Equipment System

**Goal:** Create an iron helmet that can be equipped and unequipped.

**Time:** ~20 minutes  
**Difficulty:** ?? Intermediate

### Step 1: Define Equipment Slots

1. **Edit ? Project Settings**
2. **Project ? Gameplay Tags**
3. Click **Add New Gameplay Tag** and add:

```
Equipment.Slot.Head
Equipment.Slot.Body
Equipment.Slot.Hands
Equipment.Slot.Feet
Equipment.Slot.MainHand
Equipment.Slot.OffHand
```

4. **Save**

### Step 2: Create Iron Helmet Item

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Choose **ItemDataAsset**
3. Name: `DA_IronHelmet`
4. Configure:

```
=== BASIC ===
Item ID: Armor_Helmet_Iron
Item Name: "Iron Helmet"
Item Description: "A sturdy iron helmet that provides basic protection."
Rarity: Uncommon
Category: Equipment
Item Tags: Item.Type.Armor, Item.EquipSlot.Head

=== STACKING ===
Max Stack Size: 1
Is Stackable: ?
Item Weight: 3.0

=== EQUIPMENT ===
Is Equippable: ?
Equipment Slot: Equipment.Slot.Head

=== TRADING ===
Base Value: 50
Can Be Sold: ?
Can Be Bought: ?
Sellback Percentage: 0.4
```

5. **Save**

### Step 3: Create Defense Boost Effect

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Choose **ItemEffectDataAsset**
3. Name: `DA_Effect_DefenseBoost`
4. Configure:

```
=== BASIC ===
Effect ID: Effect_DefenseBoost_IronHelmet
Effect Name: "Iron Helmet Defense"
Effect Description: "Increases defense while equipped."

=== TRIGGER ===
Trigger Type: WhileEquipped ? Important!
Target Type: Self

=== EFFECT TYPES ===
Tag Modifications:
  [0]:
    Tags To Add: Status.Buff.DefenseBoost
    Tags To Remove: [Empty]
    Duration: 0.0 (permanent while equipped)
```

5. **Save**

### Step 4: Link Effect to Helmet

1. Open `DA_IronHelmet`
2. **Item Effects** ? Click **+**
3. Select `DA_Effect_DefenseBoost`
4. **Save**

### Step 5: Add Item Effect Executor Component

This component executes item effects automatically.

1. Open your **Character Blueprint**
2. **Components Panel** ? Add **Item Effect Executor Component**
3. That's it! It auto-detects inventory events.
4. **Compile & Save**

### Step 6: Create Equipment Test Setup

1. In **Event Graph**, add test code to give helmet:

```blueprint
Event BeginPlay
  ?
Delay (1.0)
  ?
Get Component By Class ? InventoryComponent
  ?
Add Item
  - Item Data: DA_IronHelmet
  - Quantity: 1
  ?
Print String "Added Iron Helmet"
```

### Step 7: Create Equip/Unequip Input

1. In **Event Graph**, add equip logic:

```blueprint
Event InputAction [EquipKey] (e.g., "Q")
  ?
Get Component By Class ? InventoryComponent
  ?
Get All Items
  ?
ForEachLoop
    Array Element ? [Item]
    ?
    Branch (Item ? GetItemData ? Is Equippable == True)
      True ?
        Branch (Item ? bIsEquipped == False)
          True ? Equip Item
            - Item Id: Item ? ItemInstanceId
       - Out Result: [Result]
            ?
            Print String "Equipped!"
     
          False ? Unequip Item
            - Item Id: Item ? ItemInstanceId
    ?
Print String "Unequipped!"
        ?
        Break
```

2. **Compile & Save**

### Step 8: Visualize Equipment (Optional)

If you want to see the helmet on the character:

1. In **Event Graph**, bind to **OnItemEquipped**:

```blueprint
Event BeginPlay
  ?
Get Component ? InventoryComponent
  ?
Bind Event to OnItemEquipped ? HandleItemEquipped

[Custom Event: HandleItemEquipped]
Inputs:
  - Item (InventoryItem Reference)
  - bEquipped (Boolean)
  ?
Branch (bEquipped)
  True ? [Attach helmet mesh to character]
  False ? [Detach/hide helmet mesh]
```

**Example Attach Logic:**

```blueprint
[True Branch]
  ?
Spawn Actor From Class
  - Class: [Your helmet BP actor]
  ?
Attach Actor To Component
  - Parent: GetMesh
  - Socket Name: HeadSocket
```

### Step 9: Test Equipment

1. **Play In Editor**
2. Press equip key ? "Equipped!"
3. Check **GetActiveGameplayTags** on player ? should contain `Status.Buff.DefenseBoost`
4. Press equip key again ? "Unequipped!"
5. Tag should be removed

**Success!** You now have a working equipment system.

---

## Tutorial 3: General Store - Complete Shop System

**Goal:** Create a working shop where players can buy and sell items.

**Time:** ~25 minutes  
**Difficulty:** ?? Intermediate

### Step 1: Create Shop Data Asset

1. **Content Browser** ? Right-click ? **Miscellaneous ? Data Asset**
2. Choose **ShopDataAsset**
3. Name: `DA_Shop_GeneralStore`
4. Configure:

```
=== BASIC INFO ===
Shop Name: "Oakwood General Store"
Shop Description: "The finest goods in all the land!"

=== CURRENCY ===
Currency Item: DA_Gold (created in Tutorial 1)

=== PRICING ===
Price Modifier: 1.0 (normal prices)
Sellback Percentage: 0.5 (50% sellback)

=== BUYING ITEMS ===
Buys Items: ?
Buyable Item Tags: [Empty - buys all]
```

5. **Save**

### Step 2: Add Items to Shop Inventory

Still in `DA_Shop_GeneralStore`:

1. **Shop Inventory** ? Click **+** three times
2. Configure items:

**Item 0: Health Potion**
```
Item: DA_HealthPotion
Purchase Price: 20 (overrides base value of 15)
Stock Quantity: 15
Restocks: ?
Restock Interval: 3600.0 (1 game hour = 3600 seconds)
Restock Quantity: 5
Required Tags: [Empty]
Minimum Level: 0
```

**Item 1: Iron Helmet**
```
Item: DA_IronHelmet
Purchase Price: 75
Stock Quantity: 3
Restocks: ?
Restock Interval: 86400.0 (1 day)
Restock Quantity: 1
```

**Item 2: Bread (New item - create it first)**
```
Item: DA_Bread
Purchase Price: 5
Stock Quantity: 50
Restocks: ?
Restock Interval: 1800.0 (30 minutes)
Restock Quantity: 10
```

3. **Save**

### Step 3: Create Bread Item (Quick)

1. **Content Browser** ? Duplicate `DA_HealthPotion`
2. Rename to `DA_Bread`
3. Edit:

```
Item ID: Food_Bread
Item Name: "Bread"
Item Description: "Fresh baked bread."
Max Stack Size: 20
Base Value: 5
```

4. **Save**

### Step 4: Create Shopkeeper NPC Blueprint

1. **Content Browser** ? Right-click ? **Blueprint Class** ? **Character**
2. Name: `BP_Shopkeeper`
3. Open it
4. **Components** ? Add **Shop Component**
5. Select ShopComponent, configure:

```
Shop Data: DA_Shop_GeneralStore
Is Open: ?
Discount Modifier: 1.0
```

6. Add a **Static Mesh Component** (optional, for visual):
   - Mesh: Any NPC mesh
7. **Compile & Save**

### Step 5: Place Shopkeeper in Level

1. Drag `BP_Shopkeeper` from Content Browser into your level
2. Position near player start
3. **Save Level**

### Step 6: Create Shop UI Widget (Simple Version)

1. **Content Browser** ? **User Interface ? Widget Blueprint**
2. Name: `WBP_ShopUI`
3. **File ? Reparent Blueprint** ? Select **ShopWidgetBase**
4. In **Designer**:

**Add these widgets (exact names!):**

| Widget | Name | Purpose |
|--------|------|---------|
| Scroll Box | `ShopItemsList` | REQUIRED - shop items |
| Scroll Box | `PlayerItemsList` | REQUIRED - player items |
| Text Block | `PlayerGoldText` | Shows player currency |
| Text Block | `ShopNameText` | Shows shop name |
| Button | `CloseButton` | Close shop (auto-binds!) |

**Simple Layout:**

```
Canvas Panel
  ?? Border (Background)
  ?   ?? Vertical Box
  ?       ?? Horizontal Box (Header)
  ?       ?   ?? ShopNameText
?       ?   ?? Spacer
  ?       ?   ?? PlayerGoldText
  ?  ?   ?? CloseButton
  ??? Horizontal Box (Main)
  ?           ?? Vertical Box (Shop Side)
  ?           ?   ?? Text "Shop Inventory"
  ??   ?? ShopItemsList (Scroll Box)
  ?         ?? Vertical Box (Player Side)
  ?               ?? Text "Your Inventory"
  ?        ?? PlayerItemsList (Scroll Box)
```

5. **Compile & Save**

### Step 7: Add Interaction to Open Shop

1. Open `BP_Shopkeeper`
2. **Event Graph**, add:

```blueprint
Event ActorBeginOverlap
  - Other Actor ? [CastTo] ? [YourPlayerCharacter]
  ?
Branch (Cast Success)
  True ?
    Enable Input
      - Player Controller: Get Player Controller
    ?
    Print String "Press E to talk to shopkeeper"

Event ActorEndOverlap
  - Other Actor ? [CastTo] ? [YourPlayerCharacter]
  ?
  Disable Input
  ?
  Print String "" (clear message)
```

3. Add input event:

```blueprint
Event InputAction [InteractKey] (e.g., "E")
  ?
Get Component By Class ? ShopComponent
  ?
Get Player Character ? Cast ? [YourCharacter]
  ?
Get Component By Class ? InventoryComponent (from player)
  ?
Create Widget ? WBP_ShopUI
  ?
Bind To Shop
  - Shop Component: [ShopComponent from shopkeeper]
  - Customer Inventory: [Player's InventoryComponent]
  ?
Show Shop
```

4. **Compile & Save**

### Step 8: Give Player Starting Gold

1. Open your **Player Character Blueprint**
2. **Event Graph**:

```blueprint
Event BeginPlay
  ?
Delay (1.0)
  ?
Get Component ? InventoryComponent
  ?
Add Item
  - Item Data: DA_Gold
  - Quantity: 100
  ?
Print String "You have 100 gold"
```

3. **Compile & Save**

### Step 9: Test the Shop

1. **Play In Editor**
2. Walk to shopkeeper ? "Press E to talk to shopkeeper"
3. Press **E** ? Shop UI opens
4. You should see:
   - Shop items (Health Potion, Iron Helmet, Bread)
   - Your inventory (Gold: 100)
5. Try buying a Health Potion (costs 20 gold)
6. Try selling something back

**Shop system is complete!**

---

## Tutorial 4: Interactive Inventory UI

**Goal:** Create a fully functional inventory UI with tooltips and drag-drop.

**Time:** ~30 minutes  
**Difficulty:** ??? Advanced

### Step 1: Create Tooltip Widget

1. **Content Browser** ? **User Interface ? Widget Blueprint**
2. Name: `WBP_ItemTooltip`
3. **Reparent** to **ItemTooltipWidget**

**Designer Layout:**

```
Canvas Panel
  ?? Border (Name: BackgroundBorder)
      Brush Color: [Bind to Get Rarity Color]
      ?? Vertical Box
          ?? Horizontal Box (Header)
          ? ?? Image (Name: ItemIconImage) ? REQUIRED
          ?   ?? Text (Name: ItemNameText) ? REQUIRED
          ?    Font Size: 18, Bold
  ?? Text (Name: ItemRarityText)
          ?   [Bind Text to Get Item Rarity ? Convert to Text]
      ?? Text (Name: ItemDescriptionText)
    ?   Font Size: 12
   ?   Auto Wrap: ?
          ?? Horizontal Box (Stats)
          ?   ?? Text "Value:"
   ?   ?? Text (Name: ItemValueText)
    ?   ?? Text "Weight:"
          ?   ?? Text (Name: ItemWeightText)
          ?? Text (Name: ItemTypeText)
              [Shows "Usable", "Stackable", etc.]
```

4. **Graph ? Event On Tooltip Updated**:

```blueprint
Event On Tooltip Updated
  ?
Branch (Is Quest Item)
  True ? [Show quest icon]
  
Branch (Is Stackable)
  True ? QuantityText ? Set Text (GetQuantity)
```

5. **Compile & Save**

### Step 2: Create Slot Widget

1. **Content Browser** ? **User Interface ? Widget Blueprint**
2. Name: `WBP_InventorySlot`
3. **Reparent** to **InventorySlotWidget**

**Designer Layout:**

```
Overlay
  ?? Border (Name: SlotBackground)
  ?   Brush Color: (0.1, 0.1, 0.1, 0.8)
  ?   Padding: (2, 2, 2, 2)
  ?   ?? Image (Name: ItemIcon) ? REQUIRED
  ?       Size: 64x64
  ?? Border (Name: RarityBorder)
  ?   [Position at bottom, thin line]
  ?   [Bind Color to item rarity]
  ?? Text (Name: QuantityText)
  ?   Anchor: Bottom-Right
  ?   Text Size: 14
  ?   Shadow: ?
  ?? Image (Name: EquippedIcon)
      Anchor: Top-Right
      [Show "E" icon if equipped]
```

4. **Graph ? Event On Slot Hovered**:

```blueprint
Event On Slot Hovered
  ?
SlotBackground ? Set Brush Color
  - Color: (1, 1, 0, 0.5) - Yellow highlight
```

5. **Graph ? Event On Slot Unhovered**:

```blueprint
Event On Slot Unhovered
  ?
SlotBackground ? Set Brush Color
  - Color: (0.1, 0.1, 0.1, 0.8) - Normal
```

6. **Compile & Save**

### Step 3: Create Context Menu Widget

1. **Content Browser** ? **User Interface ? Widget Blueprint**
2. Name: `WBP_ContextMenu`
3. **Reparent** to **InventoryContextMenuWidget**

**Designer Layout:**

```
Border (Background with shadow)
  ?? Vertical Box
      ?? Button (Name: UseButton)
      ?   ?? Text "Use"
   ?? Button (Name: DropButton)
      ?   ?? Text "Drop"
      ?? Button (Name: SplitButton)
      ?   ?? Text "Split Stack"
      ?? Button (Name: CancelButton)
          ?? Text "Cancel"
```

**Note:** Buttons auto-bind! Just name them correctly.

4. **Compile & Save**

### Step 4: Create Main Inventory Widget

1. **Content Browser** ? **User Interface ? Widget Blueprint**
2. Name: `WBP_PlayerInventory`
3. **Reparent** to **InventoryWidgetBase**

**Designer Layout:**

```
Canvas Panel
  ?? Border (Background, Dark)
      ?? Vertical Box
       ?? Horizontal Box (Header)
          ?   ?? Text (Name: TitleText) "Inventory"
          ?   ?? Spacer
      ?   ?? Text (Name: SlotsText) ? Auto-updates "5 / 20"
          ?   ?? Button (Name: CloseButton) "X"
  ?? Horizontal Box (Weight Bar)
        ?   ?? Text (Name: WeightText) ? Auto-updates
    ?   ?? Progress Bar (Name: WeightBar) ? Auto-updates
          ?   ?? Text (Name: MaxWeightText) ? Auto-updates
     ?? Uniform Grid Panel (Name: InventoryGrid) ? REQUIRED
   ?   Slot Padding: (2, 2, 2, 2)
          ?? Horizontal Box (Footer)
   ?? Button (Name: SortButton)
     ?? Text "Sort"
```

5. **Class Settings** ? Configure:

```
Tooltip Widget Class: WBP_ItemTooltip
Slot Widget Class: WBP_InventorySlot
Context Menu Class: WBP_ContextMenu
Grid Columns: 5
Slot Size: 80.0
```

6. **Compile & Save**

### Step 5: Add Inventory Key Binding

1. **Edit ? Project Settings ? Input**
2. **Action Mappings** ? Add:
   - Name: `ToggleInventory`
   - Key: `I`
3. **Save**

### Step 6: Hook Up Inventory Toggle

1. Open **Player Character Blueprint**
2. **Event Graph**:

```blueprint
Event InputAction ToggleInventory
?
Branch (InventoryWidgetRef ? IsValid)
  True ? [Widget exists]
    InventoryWidgetRef ? Is In Viewport
      ?
      Branch
        True ? Hide Inventory
        False ? Show Inventory
  
  False ? [Create widget first]
    Create Widget ? WBP_PlayerInventory
      ?
      Promote to Variable: InventoryWidgetRef
  ?
      Bind To Inventory
        - Inventory Component: Get Component(InventoryComponent)
  ?
      Show Inventory
```

3. **Compile & Save**

### Step 7: Test Full UI

1. **Play In Editor**
2. Add some items to inventory (from previous tutorials)
3. Press **I** ? Inventory opens
4. **Hover over item** ? Tooltip appears
5. **Right-click item** ? Context menu appears
6. **Click Use** ? Item is used
7. Check **weight bar** updates
8. Check **slots count** updates
9. Press **I** again ? Inventory closes

**Congratulations!** You have a fully functional inventory UI.

---

## Tutorial 5: Gift System with Dialogue Integration

**Goal:** Create a gift system where giving items to NPCs triggers dialogue.

**Time:** ~20 minutes  
**Difficulty:** ?? Intermediate

### Step 1: Create Gift Item

1. Create **ItemDataAsset**: `DA_FlowerBouquet`

```
Item ID: Gift_FlowerBouquet
Item Name: "Flower Bouquet"
Item Description: "A beautiful bouquet of wildflowers."
Rarity: Uncommon
Category: Miscellaneous
Item Tags: Gift.Romantic

Is Stackable: ?
Max Stack Size: 5
Item Weight: 0.3

Can Be Gifted: ?
Can Be Sold: ?
Base Value: 25
```

2. **Save**

### Step 2: Create Gift Effect with Dialogue

1. Create **ItemEffectDataAsset**: `DA_Effect_GiftFlowers`

```
=== BASIC ===
Effect ID: Effect_GiftFlowersDialogue
Effect Name: "Gift Flowers - Trigger Thanks"
Effect Description: "NPC thanks player for the gift."

=== TRIGGER ===
Trigger Type: OnGift ? Important!
Target Type: NPC ? Affects the receiver

=== DIALOGUE TRIGGERS ===
Dialogue Triggers:
  [0]:
    Dialogue Tag: Dialogue.NPC.Generic.ThanksForGift
    Check Conditions: ?

=== TAG MODIFICATIONS ===
Tag Modifications:
  [0]:
 Tags To Add: Relationship.Friendly
    Tags To Remove: [Empty]
    Duration: 0.0 (permanent)
```

2. **Save**

### Step 3: Link Effect to Gift Item

1. Open `DA_FlowerBouquet`
2. **Item Effects** ? Add `DA_Effect_GiftFlowers`
3. **Save**

### Step 4: Create NPC with Dialogue Interface

1. Create Blueprint: `BP_FriendlyNPC` (Actor or Character)
2. **Class Settings** ? **Interfaces** ? Add **DialogueSystemInterface**
3. **Event Graph**, implement dialogue:

```blueprint
Event TriggerDialogue (Interface Message)
  Inputs:
    - Dialogue Tag (GameplayTag)
    - Optional Context (Object)
  ?
  Branch (Dialogue Tag == "Dialogue.NPC.Generic.ThanksForGift")
    True ?
      Print String "Thank you so much for the flowers!"
      ?
      [Optional: Play animation, sound]
    
    False ?
      Print String "Default dialogue..."
```

4. **Compile & Save**

### Step 5: Place NPC in Level

1. Drag `BP_FriendlyNPC` into level
2. Position near player
3. **Save**

### Step 6: Add Gift Interaction

1. Open `BP_FriendlyNPC`
2. **Components** ? Add **Sphere Collision** (for interaction range)
   - Radius: 150
3. **Event Graph**:

```blueprint
Event ActorBeginOverlap
  - Other Actor ? Cast ? [PlayerCharacter]
  ?
  Branch (Cast Success)
    True ?
      Enable Input (Player Controller)
  ?
  Print String "Press G to give gift"
```

4. Add gift input:

```blueprint
Event InputAction [GiftKey] (e.g., "G")
  ?
  Get Player Character ? Cast ? [YourCharacter]
    ?
    Get Component ? InventoryComponent
    ?
    Find Items By Tags
      - Tags: Gift.Romantic
      - Require All: False
      - Return: [GiftItems Array]
    ?
    ForEachLoop (Array: GiftItems)
      - Array Element ? [FirstGift]
      - Loop Body ?
      
      Gift Item
        - Item Id: FirstGift ? ItemInstanceId
        - Receiver: Self (the NPC)
        - Quantity: 1
        - Out Result: [Result]
  ?
      Branch (Result == Success)
  True ? Print String "Gift given!"
      ?
    Break (exit loop after first gift)
```

5. **Compile & Save**

### Step 7: Give Player Flowers

1. Open **Player Character**
2. **Event BeginPlay** ? Add:

```blueprint
Delay (1.0)
  ?
Get Component ? InventoryComponent
  ?
Add Item
  - Item Data: DA_FlowerBouquet
  - Quantity: 3
  ?
Print String "You have flowers to gift!"
```

3. **Compile & Save**

### Step 8: Test Gift System

1. **Play In Editor**
2. Walk to NPC ? "Press G to give gift"
3. Press **G**
4. You should see:
   - "Gift given!"
   - "Thank you so much for the flowers!" (from NPC)
5. Check NPC's tags ? should have `Relationship.Friendly`

**Success!** You now have a working gift and dialogue system.

---

## Bonus: Quick Recipes

### Recipe: Loot Drop on Enemy Death

```blueprint
[Enemy Blueprint ? Event Actor Death]
  ?
Get Random Integer In Range (1, 100)
  ?
Branch (Random Value ? 50) ? 50% drop chance
  True ?
    Spawn Actor From Class
      - Class: [WorldItemActor or custom]
      - Location: GetActorLocation
    ?
    [Set item data on spawned actor]
```

### Recipe: Crafting System (Simple)

```blueprint
[Crafting Table Interaction]
  ?
Check if player has:
  - DA_Wood ? 5
  - DA_IronOre ? 3
  ?
Branch (Has All Ingredients)
  True ?
    Remove Item By Data (DA_Wood, 5)
    Remove Item By Data (DA_IronOre, 3)
    ?
    Add Item (DA_IronSword, 1)
    ?
Print String "Crafted Iron Sword!"
```

### Recipe: Quest Item Detector

```blueprint
[Quest System ? Check for Item]
  ?
Get Component ? InventoryComponent
  ?
Find Items By Tags
  - Tags: Quest.MainQuest.KeyItem
  ?
Branch (Array Length > 0)
  True ? Quest Objective Complete
```

---

## Next Steps

Now that you've completed the tutorials, explore:

- ?? **[Blueprint Reference](BlueprintReference.md)** - Deep dive into all nodes
- ?? **[Data Reference](DataReference.md)** - All Data Asset properties
- ?? **[Examples](Examples.md)** - More ready-to-use patterns
- ? **[FAQ](FAQ_Troubleshooting.md)** - Troubleshooting common issues

---

**© 2024 Game Development Team. All Rights Reserved.**
