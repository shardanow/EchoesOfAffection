# UI Widgets C++ API Quick Reference

**Quick reference for Blueprint Widget development**

---

## ?? UInventoryWidgetBase

**Base class for inventory UI widgets**

### Setup Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `BindToInventory` | Bind to an inventory component | `UInventoryComponent*` |
| `UnbindFromInventory` | Unbind from current inventory | - |
| `GetBoundInventory` | Get currently bound inventory | Returns `UInventoryComponent*` |

### Blueprint Implementable Events

Override these in your Blueprint Widget:

| Event | When Called | Parameters |
|-------|-------------|------------|
| `OnInventoryUpdated` | Any inventory change | - |
| `OnItemAddedToInventory` | Item added | `Item`, `Quantity`, `SlotIndex` |
| `OnItemRemovedFromInventory` | Item removed | `Item`, `Quantity`, `SlotIndex` |
| `OnItemUsedFromInventory` | Item used | `Item`, `User`, `bSuccess` |

### Usage Example

```blueprint
Event Construct
  ?
  ?? Get Owning Player Pawn
      ?? Get Component (InventoryComponent)
          ?? Bind To Inventory
```

---

## ??? UItemTooltipWidget

**Base class for item tooltip widgets**

### Setup Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `SetItemData` | Set item to display | `ItemDataAsset`, `Quantity` (optional) |
| `SetInventoryItem` | Set from inventory item | `UInventoryItem*` |
| `ClearTooltip` | Clear tooltip content | - |

### Data Getters (Pure)

All these are `BlueprintPure` - use directly in bindings:

| Getter | Returns | Description |
|--------|---------|-------------|
| `GetItemName` | `FText` | Display name |
| `GetItemDescription` | `FText` | Description text |
| `GetItemIcon` | `UTexture2D*` | Icon texture |
| `GetItemRarity` | `EItemRarity` | Rarity level |
| `GetItemCategory` | `EItemCategory` | Category |
| `GetQuantity` | `int32` | Stack size |
| `GetItemValue` | `int32` | Base value |
| `GetItemWeight` | `float` | Weight |
| `GetRarityColor` | `FLinearColor` | Color for rarity |
| `IsStackable` | `bool` | Can stack? |
| `IsUsable` | `bool` | Can use? |
| `IsEquippable` | `bool` | Can equip? |
| `IsQuestItem` | `bool` | Is quest item? |

### Blueprint Implementable Events

| Event | When Called |
|-------|-------------|
| `OnTooltipUpdated` | Data changed |
| `OnTooltipCleared` | Tooltip cleared |

### Usage Example

```blueprint
Event Pre Construct
  ?
  ?? Get Item Name
  ? ?? Set Text (TitleText)
  ?
  ?? Get Rarity Color
  ?   ?? Set Color (Border)
  ?
  ?? Get Item Icon
      ?? Set Brush from Texture (IconImage)
```

---

## ?? UShopWidgetBase

**Base class for shop UI widgets**

### Setup Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `BindToShop` | Bind to shop and customer | `ShopComponent`, `CustomerInventory` |
| `UnbindFromShop` | Unbind from current shop | - |
| `GetBoundShop` | Get bound shop | Returns `UShopComponent*` |
| `GetCustomerInventory` | Get customer inventory | Returns `UInventoryComponent*` |

### Shop Actions

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `PurchaseItem` | Buy an item | `ItemData`, `Quantity` | `bool` (success) |
| `SellItem` | Sell an item | `ItemData`, `Quantity` | `bool` (success) |
| `GetPurchasePrice` | Get buy price | `ItemData`, `Quantity` | `int32` (price) |
| `GetSellbackPrice` | Get sell price | `ItemData`, `Quantity` | `int32` (price) |
| `CanAffordItem` | Check affordability | `ItemData`, `Quantity` | `bool` |
| `GetCustomerCurrency` | Get player money | - | `int32` |

### Blueprint Implementable Events

| Event | When Called | Parameters |
|-------|-------------|------------|
| `OnShopInventoryUpdated` | Shop stock changed | - |
| `OnCustomerInventoryUpdated` | Player inventory changed | - |
| `OnItemPurchased` | Item bought | `Item`, `Quantity`, `TotalPrice` |
| `OnItemSold` | Item sold | `Item`, `Quantity`, `TotalPrice` |
| `OnPurchaseFailed` | Buy failed | `Item`, `Reason` |
| `OnSaleFailed` | Sell failed | `Item`, `Reason` |

### Usage Example

```blueprint
Event OnBuyButtonClicked
  ?
  ?? Purchase Item
      Input: ItemData
      Input: Quantity
      Output: Success
      ?
      ?? Branch (Success?)
          ?? True:  Show Success Message
   ?? False: Show Error Message
```

---

## ?? Common Patterns

### Pattern: Data Binding

Use getters in **Bind** functions for text/images:

```blueprint
Text Block ? Bind ? Create Binding
  ?
  ?? Get Item Name (from parent widget)
      ?? Return Value
```

### Pattern: Event Handling

Override events to react to changes:

```blueprint
Event OnInventoryUpdated
  ?
  ?? Custom Event: RefreshInventoryDisplay
```

### Pattern: Widget Communication

Parent widget controls child widgets:

```blueprint
In Parent (WBP_Inventory):

For Each (Items)
  ?
  ?? Create Widget (WBP_Slot)
  ?   Output: SlotWidget
  ?   ?
  ?   ?? Set Item Data (SlotWidget)
  ?   ?   Input: Loop Item
  ?   ?
  ?   ?? Add Child (Grid)
  ?  Input: SlotWidget
```

---

## ?? Debugging Tips

### Check Bindings

```blueprint
Print String:
  "Inventory Bound: {0}"
  Format: GetBoundInventory ? Is Valid?
```

### Verify Events

```blueprint
Event OnInventoryUpdated
  ?
  ?? Print String ("Inventory Updated!")
```

### Monitor Data

```blueprint
Event Tick (Only during development!)
  ?
  ?? Get Customer Currency
      ?? Set Text (Debug Text)
```

---

## ? Performance Tips

### 1. Don't Tick for UI Updates

? **Bad:**
```blueprint
Event Tick
  ?? Update All UI Elements
```

? **Good:**
```blueprint
Event OnInventoryUpdated
  ?? Update Only Changed Elements
```

### 2. Cache References

? **Bad:**
```blueprint
Every Frame:
  Get Owning Player ? Get Component ? Get Item
```

? **Good:**
```blueprint
Event Construct:
  Get Component ? Save as Variable (InventoryRef)

Later:
  Use InventoryRef (cached)
```

### 3. Use Data Bindings Wisely

? **Bad:** Bind complex calculations
```blueprint
Bind: Calculate Total Weight (loops all items)
```

? **Good:** Update on change
```blueprint
Event OnInventoryUpdated
  ?? Calculate Total Weight (once)
      ?? Set Text (cached result)
```

### 4. Pool Widgets

For large lists (100+ items), consider widget pooling:

```blueprint
Slot Widgets Pool (Array)
  - Reuse existing widgets
  - Only create new if pool empty
  - Return to pool when not visible
```

---

## ?? Related Documentation

- **[UI_WIDGET_CREATION_GUIDE.md](UI_WIDGET_CREATION_GUIDE.md)** - Full widget creation guide
- **[BLUEPRINT_EXAMPLES.md](BLUEPRINT_EXAMPLES.md)** - Blueprint code examples
- **[USER_GUIDE.md](USER_GUIDE.md)** - Game designer guide

---

**Quick Reference v1.0**  
For Advanced Inventory System
