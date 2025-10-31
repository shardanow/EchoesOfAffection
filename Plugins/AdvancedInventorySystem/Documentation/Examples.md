# Examples & Recipes - Advanced Inventory System

Ready-to-use code snippets and patterns for common scenarios.

---

## Table of Contents

1. [Basic Inventory Operations](#basic-inventory-operations)
2. [Equipment & Combat](#equipment--combat)
3. [Shop & Trading](#shop--trading)
4. [Quest Systems](#quest-systems)
5. [Loot & Drops](#loot--drops)
6. [UI Patterns](#ui-patterns)
7. [Save/Load Patterns](#saveload-patterns)
8. [Multiplayer Patterns](#multiplayer-patterns)

---

## Basic Inventory Operations

### Recipe: Give Starting Items

```blueprint
[Event BeginPlay - Player Character]
  ?
Delay (1.0) ? Wait for initialization
  ?
Get Component ? InventoryComponent
  ?
Sequence
  ?? Add Item (DA_HealthPotion, 3)
  ?? Add Item (DA_Gold, 100)
  ?? Add Item (DA_BeginnersWeapon, 1)
```

### Recipe: Check and Consume Currency

```blueprint
[Purchase Item Event]
  ?
Get Component ? InventoryComponent
  ?
Get Item Quantity (DA_Gold)
  - Return Value ? [CurrentGold]
  ?
Branch (CurrentGold >= RequiredGold)
  True ?
    Remove Item By Data
      - Item Data: DA_Gold
      - Quantity: RequiredGold
      ?
    [Grant purchased item]
    Print "Purchase successful"
  False ?
    Print "Insufficient gold"
```

### Recipe: Stack or Create New Slot

```blueprint
[Add Item with Smart Stacking]
  ?
Find Items By Data (ItemData)
  - Return: [ExistingStacks]
  ?
ForEachLoop (ExistingStacks)
  - Array Element ? [Stack]
  ?
  Branch (Stack ? IsStackFull == False)
    True ?
      Stack ? AddQuantity (Amount)
   Break (exit loop)
```

### Recipe: Transfer Items Between Inventories

```blueprint
[Transfer Item Between Characters]
  ?
SourceInventory ? Remove Item By Id
  - Item Id: [ItemId]
  - Quantity: [Amount]
  - Out Result ? [RemoveResult]
  ?
Branch (RemoveResult == Success)
  True ?
    TargetInventory ? Add Item
    - Item Data: [ItemData]
    - Quantity: [Amount]
      - Out Result ? [AddResult]
    ?
    Branch (AddResult != Success)
      True ? [Rollback: add back to source]
```

---

## Equipment & Combat

### Recipe: Auto-Equip Best Item

```blueprint
[Auto-Equip Best Armor]
  ?
Get All Items
  ?
Find Items By Tags (Tags: "Item.Type.Armor")
  ?
ForEachLoop
  - Array Element ? [ArmorItem]
  ?
  Get Metadata (ArmorItem, "DefenseRating")
    - Out Value ? [Defense]
    ?
    Branch (Defense > CurrentBestDefense)
     True ? Set CurrentBestDefense, CurrentBestItem
  ?
Equip Item (CurrentBestItem)
```

### Recipe: Equipment Set Bonus

```blueprint
[Check Equipment Set Bonus]
  ?
Get Equipped Items
  ?
Set [SetPieceCount = 0]
  ?
ForEachLoop (EquippedItems)
  - Array Element ? [Item]
  ?
  Branch (Item ? ItemTags ? Has Tag "Set.IronGuardian")
  True ? Increment SetPieceCount
  ?
[After Loop]
Branch (SetPieceCount >= 4)
  True ? Add Tag (Character, "Buff.IronGuardianSetBonus")
  False ? Remove Tag (Character, "Buff.IronGuardianSetBonus")
```

### Recipe: Durability System (via Metadata)

```blueprint
[On Take Damage - Equipped Armor]
  ?
Get Equipped Items
  ?
ForEachLoop
    - Array Element ? [EquippedItem]
  ?
  Get Metadata (EquippedItem, "Durability")
    - Out Value ? [DurabilityString]
 ?
    [Parse to Int] ? [Durability]
    ?
    [Durability - DamageAmount] ? [NewDurability]
    ?
    Branch (NewDurability <= 0)
      True ? Unequip Item, Remove Item
 False ? Set Metadata (EquippedItem, "Durability", NewDurability)
```

### Recipe: Weapon Switching

```blueprint
[Switch Weapon Input]
  ?
Get Equipped Item In Slot (Equipment.Slot.MainHand)
  - Return ? [CurrentWeapon]
  ?
Get All Items
  ?
Find Items By Tags (Tags: "Item.Type.Weapon")
  - Return ? [AllWeapons]
  ?
[Find Next Weapon After CurrentWeapon in AllWeapons]
  ?
Unequip Item (CurrentWeapon)
  ?
Equip Item (NextWeapon)
```

---

## Shop & Trading

### Recipe: Dynamic Shop Prices by Reputation

```blueprint
[Shop Component - Calculate Price (Interface Override)]
  Inputs: BasePrice, ItemData, Buyer, Quantity
  ?
  Get Component (Buyer) ? ReputationComponent
    - Get Reputation ? [Rep]
    ?
  Branch (Rep >= 75)
True ? Discount = 0.9 (10% discount)
      Rep >= 50 ? Discount = 0.95 (5% discount)
      Else ? Discount = 1.0 (no discount)
    ?
    Return: BasePrice ? Discount ? Quantity
```

### Recipe: Bulk Buy/Sell Buttons

```blueprint
[Bulk Buy Button - Buy 10x]
  ?
Get Purchase Price (Item, 10, Buyer)
  - Return ? [TotalPrice]
  ?
[Display Confirmation: "Buy 10x for TotalPrice gold?"]
  ?
On Confirm ?
  Shop Component ? Purchase Item
    - Item: Item
    - Quantity: 10
    - Buyer: Buyer
```

### Recipe: Limited-Time Sale

```blueprint
[Shop Begin Play]
  ?
Set Discount Modifier = 0.75 (25% off)
  ?
Set Timer by Function Name
  - Function Name: "EndSale"
  - Time: 3600.0 (1 hour)
  - Looping: False

[Custom Event: EndSale]
  ?
  Set Discount Modifier = 1.0
  ?
  Print "Sale ended"
```

### Recipe: Barter System (No Currency)

```blueprint
[Barter Trade]
  ?
Player Offers: [OfferedItems Array]
NPC Wants: [WantedItems Array]
  ?
[Calculate total value of OfferedItems]
[Calculate total value of WantedItems]
  ?
Branch (OfferedValue >= WantedValue)
  True ?
    [Remove OfferedItems from Player]
    [Add WantedItems to Player]
    Print "Trade successful"
  False ?
    Print "Insufficient value"
```

---

## Quest Systems

### Recipe: Quest Item Collection Check

```blueprint
[Quest Update Event]
  ?
Get Component ? InventoryComponent
  ?
Has Item (DA_QuestItem_GoldenKey, 1)
  - Return ? [HasKey]
  ?
Branch (HasKey)
  True ? Complete Quest Objective ("FindGoldenKey")
  False ? [Update quest progress]
```

### Recipe: Consume Quest Items on Turn-In

```blueprint
[Quest Turn-In]
  ?
Get Component ? InventoryComponent
  ?
Sequence
  ?? Remove Item By Data (DA_BearPelt, 5)
  ?? Remove Item By Data (DA_WolfFang, 10)
  ?? Remove Item By Data (DA_Herbs, 20)
  ?
[Grant Reward]
  ?
Add Item (DA_Gold, 500)
Add Item (DA_RewardPotion, 3)
```

### Recipe: Required Item for Dialogue Option

```blueprint
[Dialogue Option: "Give Ancient Scroll"]
  ?
Get Component ? InventoryComponent
  ?
Has Item (DA_AncientScroll, 1)
  - Return ? [HasScroll]
  ?
Branch (HasScroll)
  True ? [Show dialogue option "Give scroll"]
  False ? [Hide option or show "You don't have the scroll"]
```

---

## Loot & Drops

### Recipe: Random Loot Drop

```blueprint
[Enemy Death Event]
  ?
Random Float In Range (0.0, 1.0)
  - Return ? [RollValue]
  ?
Sequence [Check loot tables]
  ?? Branch (RollValue ? 0.10) ? 10% rare loot
    ? True ? Spawn Loot (DA_RareItem)
  ?? Branch (RollValue ? 0.40) ? 30% uncommon loot
  ? True ? Spawn Loot (DA_UncommonItem)
  ?? Else ? 60% common loot
      ? Spawn Loot (DA_CommonItem)
```

### Recipe: Weighted Random Loot Table

```blueprint
[Loot Table - Weighted Random]
  ?
[Define Loot Table Array]:
  - DA_Gold (Weight: 50)
  - DA_HealthPotion (Weight: 30)
  - DA_Sword (Weight: 15)
  - DA_LegendaryRing (Weight: 5)
  
  Total Weight = 100
  ?
  Random Integer (1, 100) ? [Roll]
  ?
  Accumulated = 0
  ForEachLoop (LootTable)
    - Accumulated += Item.Weight
    - Branch (Roll ? Accumulated)
        True ? Spawn Item, Break
```

### Recipe: Drop All on Death

```blueprint
[Character Death]
  ?
Get Component ? InventoryComponent
  ?
Get All Items
  - Return ? [AllItems]
  ?
ForEachLoop (AllItems)
    - Array Element ? [Item]
    ?
    Drop Item
      - Item Id: Item ? ItemInstanceId
      - Quantity: 0 (all)
      - Drop Location: GetActorLocation + Random Vector In Cone
```

### Recipe: Container/Chest Inventory

```blueprint
[Chest Actor - BeginPlay]
  ?
Add Component ? InventoryComponent
  - Max Inventory Slots: 12
  - Max Weight Capacity: 0
  ?
[Populate with random loot]
Sequence
  ?? Add Item (DA_Gold, RandomInt(10, 50))
  ?? Add Item (DA_HealthPotion, RandomInt(1, 3))
  ?? 50% Chance ? Add Item (DA_RareGem, 1)
```

---

## UI Patterns

### Recipe: Inventory Weight Progress Bar

```blueprint
[Widget: InventoryWidgetBase - Event On Weight Changed]
  Inputs: CurrentWeight, MaxWeight
  ?
  WeightBar ? Set Percent
    - Percent: CurrentWeight / MaxWeight
    ?
    Branch (CurrentWeight / MaxWeight > 0.9)
      True ? WeightBar ? Set Fill Color (Red)
      Else ? WeightBar ? Set Fill Color (Green)
```

### Recipe: Rarity-Based Color

```blueprint
[Tooltip Widget - Update Rarity Color]
  ?
  Get Item Rarity
    - Return ? [Rarity]
  ?
    Select (Rarity)
      Common ? (1, 1, 1, 1) White
      Uncommon ? (0, 1, 0, 1) Green
 Rare ? (0, 0.5, 1, 1) Blue
Epic ? (0.6, 0, 1, 1) Purple
      Legendary ? (1, 0.5, 0, 1) Orange
      Unique ? (1, 0.8, 0, 1) Gold
    ?
    BackgroundBorder ? Set Brush Color (SelectedColor)
```

### Recipe: Item Comparison Tooltip

```blueprint
[Slot Widget - On Slot Hovered]
  ?
  Get Equipped Item In Slot (Item ? EquipmentSlot)
    - Return ? [EquippedItem]
    ?
  Create Widget ? WBP_ComparisonTooltip
    - Set Item A: [Current Hovered Item]
    - Set Item B: [EquippedItem]
    ?
  [Display stats comparison: Attack, Defense, etc.]
```

### Recipe: Drag-and-Drop Inventory

```blueprint
[Slot Widget - On Mouse Button Down]
  ?
  Detect Drag (Drag Key: Left Mouse Button)
    ?
 On Drag Detected ?
 Create Drag-Drop Operation
        - Payload: [Item Instance Id]
        - Pivot: Drag Pivot Center
        - Visual Widget: [Item Icon]

[Slot Widget - On Drop]
  Inputs: DragDropOperation
  ?
  Get Payload ? [SourceItemId]
  ?
  Swap Items
    - Slot Index A: [Source Slot Index]
    - Slot Index B: [Target Slot Index]
```

---

## Save/Load Patterns

### Recipe: Quick Save Inventory

```blueprint
[Quick Save Button]
  ?
Get Component ? InventoryComponent
  ?
Save To Json
    - Out Json String ? [JsonString]
    ?
  Create Save Game Object
  - Inventory Data: JsonString
    ?
    Save Game To Slot ("QuickSave", 0)
    ?
    Print "Game saved"
```

### Recipe: Auto-Save on Inventory Change

```blueprint
[Event Construct - InventoryWidgetBase]
  ?
Bind Event to On Inventory Changed ? HandleInventoryChanged

[Custom Event: HandleInventoryChanged]
  ?
  [Debounce: only save once per 5 seconds]
  Set Timer by Function Name
    - Function Name: "AutoSave"
    - Time: 5.0
  - Looping: False

[Custom Event: AutoSave]
  ?
  [Save inventory logic]
```

### Recipe: Save Equipped Items State

```blueprint
[Custom Save Data Struct]
struct FSaveData
{
  FString InventoryJson;
  TArray<FInventoryItemId> EquippedItemIds;
}

[Save]
  ?
  Get Equipped Items ? [EquippedItems]
  ?
  ForEachLoop (EquippedItems)
    - Add to EquippedItemIds array
  ?
  Save To Json ? InventoryJson
  ?
  [Save FSaveData to file]

[Load]
  ?
  Load From Json (InventoryJson)
  ?
  ForEachLoop (EquippedItemIds)
    - Equip Item (ItemId)
```

---

## Multiplayer Patterns

### Recipe: Server-Authoritative Add Item

```blueprint
[Client: Pickup Item Event]
  ?
  [Run on Server, Reliable]
  ServerPickUpItem
    - Item Data: DA_Sword
    ?
    [Server executes]
    Get Component ? InventoryComponent
    ?
    Add Item (DA_Sword, 1)
    ?
    [Automatically replicates to all clients]
```

### Recipe: Client Prediction for Use Item

```blueprint
[Client: Use Item Button]
  ?
  [Play local animation immediately]
  Play Animation Montage (DrinkingPotion)
  ?
  [Run on Server, Reliable]
  ServerUseItem
    - Item Id: [ItemId]
    ?
    [Server validates and executes]
    Use Item (ItemId, User)
      - Out Result ? [Result]
      ?
      [Multicast to all clients]
      MulticastItemUsed
        - Item Id: [ItemId]
        - bSuccess: [Result == Success]
```

### Recipe: Drop Item with Replication

```blueprint
[Server: Drop Item]
  ?
  Drop Item
    - Item Id: [ItemId]
    - Quantity: [Amount]
    - Drop Location: [Location]
    - Return ? [SpawnedActor]
    ?
    [SpawnedActor is automatically replicated]
    ?
    [Clients see the dropped item]
```

### Recipe: Synchronized Shop Purchase

```blueprint
[Client: Buy Button Clicked]
  ?
  [Run on Server, Reliable]
  ServerPurchaseItem
    - Item: DA_HealthPotion
    - Quantity: 5
    ?
    [Server executes]
    Shop Component ? Purchase Item
      - Buyer: RequestingClient
      - Out Result ? [Result]
      ?
      Branch (Result == Success)
        True ? [Inventory updated, replicated to client]
        False ? [Client RPC: Show error message]
```

---

## Bonus: Common Patterns

### Pattern: Inventory Full Check Before Pickup

```blueprint
[Before Allowing Pickup]
  ?
  Get Component ? InventoryComponent
  ?
  Can Add Item (Item Data, Quantity)
    - Return ? [CanAdd]
    ?
    Branch (CanAdd)
      True ? [Show "Press E to pick up" prompt]
  False ? [Show "Inventory full" message]
```

### Pattern: Currency Display Widget

```blueprint
[Widget: Currency Display - Event Construct]
  ?
  Get Component ? InventoryComponent
  ?
  Bind Event to On Inventory Changed ? UpdateCurrency

[Custom Event: UpdateCurrency]
  ?
  Get Item Quantity (DA_Gold)
    - Return ? [GoldAmount]
    ?
    CurrencyText ? Set Text (ToString(GoldAmount) + " Gold")
```

### Pattern: Item Hotbar/Quick Slots

```blueprint
[Hotbar Slot 1 Key Pressed]
  ?
  Get Item At Slot (0) ? Slot 0 = Hotbar Slot 1
    - Return ? [Item]
    ?
  Branch (Item ? IsValid AND Item ? ItemData ? IsUsable)
      True ? Use Item (Item ? ItemInstanceId, User: Self)
      False ? Print "No usable item in slot 1"
```

---

**Next:** [FAQ & Troubleshooting](FAQ_Troubleshooting.md) - Common issues and solutions

**© 2024 Game Development Team. All Rights Reserved.**
