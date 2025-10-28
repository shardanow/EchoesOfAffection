# Inventory Slot Widget Guide

**Creating custom inventory slot widgets for item display**

---

## ?? Overview

The **Inventory Slot Widget** represents a single item slot in your inventory grid. 

**System automatically:**
- Creates slots for each item in inventory
- Adds them to `InventoryGrid`
- Updates when inventory changes

**You customize:**
- How slots look (icon, background, borders)
- What info to show (quantity, rarity, equipped status)

---

## ?? Step-by-Step: Create Slot Widget

### Step 1: Create Widget Blueprint

1. **Content Browser** ? `Content/UI/`
2. Right-click ? **User Interface** ? **Widget Blueprint**
3. Name: `WBP_InventorySlot`
4. Open it

### Step 2: Reparent to C++ Class

1. **File** ? **Reparent Blueprint**
2. Search: `InventorySlotWidget`
3. Select and click **Reparent**

### Step 3: Add Required Widget

**?? REQUIRED - Must have:**

| Widget Type | Variable Name | Purpose |
|-------------|---------------|---------|
| **Image** | `ItemIcon` | Item icon display ?? REQUIRED |

**Without this, compilation FAILS!**

### Step 4: Add Recommended Widgets

**? RECOMMENDED:**

| Widget Type | Variable Name | Purpose |
|-------------|---------------|---------|
| **Text Block** | `QuantityText` | Stack quantity ("x5") |
| **Border** | `SlotBackground` | Background/highlight |

### Step 5 (Optional): Add Advanced Widgets

**?? OPTIONAL:**

| Widget Type | Variable Name | Purpose |
|-------------|---------------|---------|
| Border | `RarityBorder` | Rarity color border |
| Image | `EquippedIcon` | Equipped indicator |
| Image | `LockedIcon` | Locked slot indicator |

---

## ?? Layout Examples

### Minimal Layout (1 widget, 1 minute):

```
Overlay (Root) 80x80
?? Image (ItemIcon) ?? REQUIRED
    Size: Fill
```

**Result:** Just shows icon, no quantity/rarity

### Recommended Layout (3 widgets, 3 minutes):

```
Overlay (Root) 80x80
?? Border (SlotBackground) ?
?   Fill entire slot
?   Brush Color: (1, 1, 1, 0.1)
?
?? Image (ItemIcon) ??
?   Size: 64x64, Center
?
?? Text Block (QuantityText) ?
    Anchor: Bottom-Right
    Font Size: 14
  Shadow: Enabled
```

**Result:** Professional looking, shows quantity

### Complete Layout (6 widgets, 5 minutes):

```
Overlay (Root) 80x80
?? Border (RarityBorder) ??
?   Full size, 2px padding
?   Auto-colored by rarity
?
?? Border (SlotBackground) ?
?   Full size
?   Hover highlight
?
?? Image (ItemIcon) ??
?   Center, 64x64
?
?? Text Block (QuantityText) ?
?   Bottom-Right
?   "x5" format
?
?? Image (EquippedIcon) ??
?   Top-Left corner, 16x16
?   Shows if item equipped
?
?? Image (LockedIcon) ??
    Full size, semi-transparent
    Shows if slot locked
```

**Result:** AAA-quality slot with all features

---

## ?? Connect Slot to Inventory

### In WBP_Inventory (parent widget):

**Details Panel:**
```
Slots:
  Slot Widget Class: WBP_InventorySlot  ?? SET THIS!
  Grid Columns: 5
```

**That's it!** System creates slots automatically when you:
1. Call `BindToInventory()`
2. Inventory changes (add/remove items)

---

## ?? Auto-Features

### Automatic Updates:

**ItemIcon:**
- ? Auto-set from ItemData
- ? Auto-shown/hidden if slot empty
- ? Auto-scaled to fit

**QuantityText:**
- ? Auto-formatted "x5"
- ? Auto-hidden if quantity = 1
- ? Auto-updated on stack change

**RarityBorder:**
- ? Auto-colored by rarity:
  - Common: Gray
  - Uncommon: Green
  - Rare: Blue
  - Epic: Purple
  - Legendary: Orange
  - Unique: Gold

**SlotBackground:**
- ? Auto-highlights on mouse hover
- ? Auto-removes highlight on mouse leave

---

## ?? Events You Can Override

### In Blueprint (Event Graph):

**Event OnSlotClicked:**
```blueprint
Event OnSlotClicked
  ?
  ?? Branch (Is Empty?)
   ?? False: (has item)
      ?   ?? Use Item / Drop Item / etc.
      ?
      ?? True: (empty slot)
          ?? Do nothing
```

**Event OnSlotHovered:**
```blueprint
Event OnSlotHovered
  ?
  ?? Get Parent Widget
  ?   Cast to: InventoryWidgetBase
  ?   ?
  ?   ?? Show Item Tooltip
  ?       ItemData: Self.ItemData
  ?       Quantity: Self.Quantity
  ?       ScreenPosition: Get Mouse Position
```

**Event OnSlotUnhovered:**
```blueprint
Event OnSlotUnhovered
  ?
  ?? Get Parent Widget
      Cast to: InventoryWidgetBase
      ?
      ?? Hide Item Tooltip
```

---

## ?? Complete Example

### WBP_InventorySlot Structure:

```
Canvas Panel (Root) 80x80
?? Overlay
 ?? Border (RarityBorder)
    ?   ?? Brush: Solid Color
    ?   ?? Color: Set by C++ (rarity)
    ?   ?? Padding: (2, 2, 2, 2)
    ?
    ?? Border (SlotBackground)
    ?   ?? Brush: Solid Color
    ?   ?? Color: (1, 1, 1, 0.1)
    ?   ?? Hover Tint: (1, 1, 1, 0.3)
    ?
    ?? Horizontal Box (Center)
    ?   ?? Image (ItemIcon) 64x64
    ?       ?? Size: Fill
    ?       ?? Preserve Aspect: true
    ?
    ?? Vertical Box
     ?? Spacer (Fill)
    ?? Horizontal Box
   ?? Spacer (Fill)
    ?? Text Block (QuantityText)
    ?? Font Size: 14
 ?? Shadow: (0, 0, 0, 1)
           ?? Shadow Offset: (1, 1)
```

### Event Graph:

```blueprint
Event OnSlotClicked
  ?
  ?? Print String: "Clicked slot {SlotIndex}"
  ?
  ?? Branch (ItemData valid?)
      ?? True:
      ?? Call: UseItemFromSlot


Event OnSlotHovered
  ?
  ?? Get Owning Player
  ?   ?
  ?   ?? Get Widget of Class
  ?       Class: WBP_Inventory
  ?       ?
  ?       ?? Show Item Tooltip
  ?       ItemData: ItemData
  ? Quantity: Quantity


Event OnSlotUnhovered
  ?
  ?? Get Owning Player
      ?
      ?? Get Widget of Class
    Class: WBP_Inventory
      ?
     ?? Hide Item Tooltip
```

---

## ? How It Works Behind the Scenes

### When Inventory Changes:

```
1. InventoryComponent ? fires OnInventoryChanged
   ?
2. InventoryWidgetBase ? HandleInventoryChanged()
   ?
3. RefreshInventorySlots() called
   ?
4. Clear old slots from InventoryGrid
   ?
5. For each item in inventory:
      - Create WBP_InventorySlot
      - Call SetInventoryItem(item, index)
      - Add to InventoryGrid (row, column)
   ?
6. Slots auto-update their visuals
```

**You don't call any of this manually!** Just:
1. Set `SlotWidgetClass` in WBP_Inventory
2. Call `BindToInventory()`
3. Done!

---

## ?? Grid Layout

### Grid is Uniform Grid Panel:

**Settings in WBP_Inventory:**
- `GridColumns = 5` ? 5 items per row
- Slots added left-to-right, top-to-bottom

**Example with 12 items, 5 columns:**
```
[0] [1] [2] [3] [4]
[5] [6] [7] [8] [9]
[10][11]
```

**Responsive:** Automatically wraps to new rows

---

## ?? Customization Examples

### Example 1: Drag and Drop Support

```blueprint
// In WBP_InventorySlot

Override: OnMouseButtonDown
  ?
  ?? Detect Drag Operation
      ?
 ?? Create Drag Operation
   Payload: Self (slot widget)
 Visual: Duplicate of ItemIcon


Override: OnDrop
  Input: DraggedSlot
  ?
  ?? Swap Items
      Source: DraggedSlot.SlotIndex
      Target: Self.SlotIndex
```

### Example 2: Right-Click Context Menu

```blueprint
Override: OnMouseButtonDown
  ?
  ?? Branch (Right Mouse Button?)
      ?
      ?? True:
   ?
 ?? Create Widget (WBP_ContextMenu)
              ?? Option: Use Item
       ?? Option: Drop Item
   ?? Option: Favorite
    ?? Show at cursor
```

### Example 3: Double-Click to Use

```blueprint
// Variables:
LastClickTime (float)

Event OnSlotClicked
  ?
  ?? Get World Delta Seconds
  ?   Current Time - LastClickTime
  ?   ?
  ?   ?? Branch (< 0.3 seconds?)
  ?       ?
  ?   ?? True: (Double click!)
  ?       ?   ?? Use Item
  ?       ?
  ?     ?? False: (Single click)
  ?       ?? Select Slot
  ?
  ?? Set LastClickTime = CurrentTime
```

---

## ?? Troubleshooting

### Slots Don't Show:

**Check:**
1. `SlotWidgetClass` set in WBP_Inventory? ?
2. `InventoryGrid` exists in WBP_Inventory? ?
3. `BindToInventory()` was called? ?
4. Inventory has items? ?

**Debug:**
```blueprint
After BindToInventory:
  ?? Print String: "Bound to inventory with {ItemCount} items"
```

### Icons Don't Show:

**Check:**
1. `ItemIcon` widget exists in WBP_InventorySlot? ?
2. ItemData has valid `ItemIcon` texture? ?
3. Widget name is exactly `ItemIcon` (case-sensitive)? ?

### Quantity Text Wrong:

**Check:**
1. Widget named `QuantityText` exactly? ?
2. Text block has sufficient size? ?
3. Font size not too large? ?

---

## ?? Related Documentation

- **[WIDGET_COMPONENTS_GUIDE.md](WIDGET_COMPONENTS_GUIDE.md)** - Required widgets
- **[WIDGET_USAGE_QUICK_REF.md](WIDGET_USAGE_QUICK_REF.md)** - How to use in game
- **[TOOLTIP_INTEGRATION_GUIDE.md](TOOLTIP_INTEGRATION_GUIDE.md)** - Connect tooltips
- **[UI_API_REFERENCE.md](UI_API_REFERENCE.md)** - C++ API reference

---

## ? Summary

### To Create Slot Widget:

1. **Create** `WBP_InventorySlot` Blueprint
2. **Reparent** to `InventorySlotWidget`
3. **Add** minimum: `ItemIcon` (Image) ??
4. **Recommended:** + `QuantityText`, `SlotBackground`
5. **Set** in `WBP_Inventory`: `SlotWidgetClass = WBP_InventorySlot`

**Time:** 3-5 minutes  
**Result:** Automatic slot creation and updates! ?

---

**Now your inventory will show items in a beautiful grid!** ??

