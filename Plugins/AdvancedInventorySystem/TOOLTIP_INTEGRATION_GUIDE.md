# Tooltip Integration Guide

**How to connect your tooltip widget to Inventory and Shop UIs**

---

## ?? Overview

The Advanced Inventory System provides **automatic tooltip support**. You just need to:

1. Create your tooltip widget (WBP_ItemTooltip)
2. Set `TooltipWidgetClass` property in Inventory/Shop widget
3. Call `ShowItemTooltip` / `HideItemTooltip` when needed

**System handles:** Creation, positioning, data binding, cleanup

---

## ?? Step-by-Step Setup

### Step 1: Create Tooltip Widget

Follow **[QUICK_UI_SETUP.md](QUICK_UI_SETUP.md)** to create `WBP_ItemTooltip`

**Minimum required** (2 widgets):
- `ItemNameText` (Text Block) ??
- `ItemIconImage` (Image) ??

### Step 2: Set Tooltip Class in Inventory Widget

1. Open your `WBP_Inventory` (inventory widget)
2. In **Details** panel, find **Tooltip** section
3. Set **Tooltip Widget Class** ? `WBP_ItemTooltip`
4. (Optional) Set **Tooltip Offset** ? (10, 10) default

**That's it for configuration!**

### Step 3: Call Tooltip Methods from Item Slots

When creating item slot widgets, bind hover events:

#### In WBP_InventorySlot (your custom slot widget):

**On Mouse Enter:**
```blueprint
Event OnMouseEnter
  Input: Mouse Event
  ?
  ?? Get Owning Player
  ?   ?
  ?   ?? Get Widget From Viewport
  ?       Class: WBP_Inventory
  ?       Output: InventoryWidget
  ?       ?
  ?    ?? Show Item Tooltip
  ?       Input: ItemData (from this slot)
  ?     Input: Quantity (from this slot)
  ?      Input: Get Mouse Position in Viewport
```

**On Mouse Leave:**
```blueprint
Event OnMouseLeave
  Input: Mouse Event
  ?
  ?? Get Owning Player
      ?
      ?? Get Widget From Viewport
      Class: WBP_Inventory
          Output: InventoryWidget
       ?
        ?? Hide Item Tooltip
```

---

## ?? Shop Widget Setup

### Same Process for Shop:

1. Open `WBP_Shop`
2. Set **Tooltip Widget Class** ? `WBP_ItemTooltip`
3. In shop item widgets, call `ShowItemTooltip` / `HideItemTooltip`

**Both Inventory and Shop can share the same tooltip widget!**

---

## ?? Example: Complete Slot Widget

### WBP_InventorySlot Structure:

```
Overlay (Root)
?? Border (Background)
?   ?? Image (ItemIcon)
?? Text Block (Quantity)
```

### Variables:

```
ItemData (ItemDataAsset) - The item in this slot
Quantity (Integer) - Stack size
ParentInventoryWidget (InventoryWidgetBase) - Reference to parent
```

### Event Graph:

**Event Construct:**
```blueprint
Event Construct
  ?
  ?? Get Parent Widget
      Cast to: WBP_Inventory
    ?
      ?? Set ParentInventoryWidget (save reference)
```

**Event OnMouseEnter:**
```blueprint
Event OnMouseEnter
  Input: Mouse Event (geometry, mouse event)
  ?
  ?? Branch (ItemData is valid?)
  ?   ?
  ?   ?? True:
  ?     ?
  ?       ?? Get Mouse Position in Viewport
  ?    ?   Input: Mouse Event
  ?       ?   Output: Screen Position, Viewport Position
  ?       ?   ?
  ?     ?   ?? ParentInventoryWidget ? Show Item Tooltip
  ?       ?       Input: ItemData
  ?       ?       Input: Quantity
  ?       ?       Input: Screen Position
```

**Event OnMouseLeave:**
```blueprint
Event OnMouseLeave
  ?
  ?? ParentInventoryWidget ? Hide Item Tooltip
```

---

## ? Quick Blueprint Code

### Simplified Version (No Parent Reference):

**OnMouseEnter:**
```
Get Owning Player
  ? Get All Widgets of Class (WBP_Inventory)
    ? Get (index 0)
      ? Show Item Tooltip
```

**OnMouseLeave:**
```
Get Owning Player
  ? Get All Widgets of Class (WBP_Inventory)
    ? Get (index 0)
  ? Hide Item Tooltip
```

**?? Note:** Slower than storing parent reference, but simpler

---

## ?? Advanced: Follow Mouse Cursor

If you want tooltip to follow cursor:

### In WBP_Inventory:

**Event Tick:**
```blueprint
Event Tick
  ?
  ?? Branch (CurrentTooltip is valid?)
  ?   ?
  ?   ?? True:
  ?     ?
  ?       ?? Get Mouse Position in Viewport
  ?       ?   Output: Screen Position
  ?    ?   ?
  ?       ?   ?? CurrentTooltip ? Set Position in Viewport
  ?       ?       Input: Screen Position + TooltipOffset
  ?       ?       Remove DPI Scale: false
```

**?? Performance:** Only use if tooltip needs to follow cursor actively

---

## ?? Tooltip Widget Class Property

### Where to Set It:

**In Blueprint Editor:**

1. Open `WBP_Inventory` or `WBP_Shop`
2. Select root widget (Self)
3. **Details** ? Search: "Tooltip"
4. **Tooltip Widget Class** ? Select `WBP_ItemTooltip`

**Default Values:**

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `TooltipWidgetClass` | `TSubclassOf<ItemTooltipWidget>` | None | Your tooltip BP |
| `TooltipOffset` | `FVector2D` | (10, 10) | Offset from cursor |

---

## ?? Complete Example: Inventory with Tooltip

### WBP_Inventory Setup:

**Details Panel:**
```
Tooltip:
  Tooltip Widget Class: WBP_ItemTooltip
  Tooltip Offset: X=15, Y=15
```

### WBP_InventorySlot Setup:

**Variables:**
```
ItemData: ItemDataAsset (Instance Editable)
Quantity: Integer (Instance Editable)
```

**Event Graph:**
```blueprint
Event OnMouseEnter
  ?
  ?? Get Parent Widget
      Cast to: InventoryWidgetBase
      ?
      ?? Show Item Tooltip
       ItemData: Self.ItemData
 Quantity: Self.Quantity
          ScreenPosition: Get Mouse Position in Viewport


Event OnMouseLeave
  ?
  ?? Get Parent Widget
      Cast to: InventoryWidgetBase
      ?
    ?? Hide Item Tooltip
```

---

## ??? Multiple Tooltips (Advanced)

If you want different tooltips for different contexts:

### Option 1: Different Tooltip Classes

```
WBP_Inventory:
  TooltipWidgetClass: WBP_ItemTooltip_Simple

WBP_Shop:
  TooltipWidgetClass: WBP_ItemTooltip_Detailed
```

### Option 2: Same Tooltip, Different Data

Use `SetInventoryItem()` instead of `SetItemData()` for instance-specific info:

```blueprint
Show Item Tooltip
  ?
  ?? After creation, call:
      CurrentTooltip ? Set Inventory Item
        Input: Actual UInventoryItem instance
```

**Benefits:** Shows enchantments, durability, etc.

---

## ?? Styling Tooltip Position

### Default Behavior:

```
Tooltip appears at: Mouse Position + Offset
```

### Custom Positioning:

**Anchor to Slot:**
```blueprint
Event OnMouseEnter
  ?
  ?? Get Slot Widget Geometry
  ?   Output: Slot Geometry
  ?   ?
  ?   ?? Get Absolute Position
  ?   ?
  ?       ?? Show Item Tooltip
  ?        ScreenPosition: Slot Position + (Slot Width, 0)
```

**Result:** Tooltip appears to the right of slot

### Prevent Off-Screen:

```blueprint
After Show Item Tooltip:
  ?
  ?? Get Viewport Size
  ?   Output: Viewport Size
  ?   ?
  ?   ?? Get Tooltip Widget Size
  ?   ?   ?
  ?   ?   ?? Branch (Tooltip goes off-screen?)
  ?   ?       ?
  ?   ?       ?? True:
  ?   ?           ?? Adjust Position
  ?   ?           (move left or up)
```

---

## ?? Troubleshooting

### Tooltip Doesn't Show:

**Check:**
1. `TooltipWidgetClass` is set in WBP_Inventory?
2. `ItemData` is valid when calling ShowItemTooltip?
3. `WBP_ItemTooltip` has required widgets (ItemNameText, ItemIconImage)?
4. Mouse events are enabled on slot widget?

**Debug:**
```blueprint
Show Item Tooltip
  ?
  ?? Print String: "Showing tooltip for: {ItemName}"
```

### Tooltip Shows But Empty:

**Check:**
1. ItemData is set correctly?
2. WBP_ItemTooltip has correct widget names?
3. Call `SetItemData()` before showing?

### Tooltip Doesn't Hide:

**Check:**
1. `OnMouseLeave` event is firing?
2. Calling `HideItemTooltip()` correctly?

**Debug:**
```blueprint
Event OnMouseLeave
  ?
  ?? Print String: "Mouse left slot"
?? Hide Item Tooltip
```

### Tooltip Position Wrong:

**Check:**
1. Using `Get Mouse Position in Viewport` (not `Get Mouse Position in Geometry`)?
2. `TooltipOffset` is reasonable (not 1000, 1000)?

---

## ?? Best Practices

### ? DO:

- ? Set `TooltipWidgetClass` in parent widget (Inventory/Shop)
- ? Call `HideItemTooltip` on `OnMouseLeave`
- ? Check `ItemData` is valid before showing
- ? Use same tooltip class for Inventory and Shop (consistency)

### ? DON'T:

- ? Create tooltip manually with `CreateWidget` (system handles it)
- ? Forget to call `HideItemTooltip` (causes tooltip to stick)
- ? Call `ShowItemTooltip` every frame (performance!)
- ? Set tooltip Z-order too low (won't show on top)

---

## ?? Performance Tips

### Optimize Slot Widgets:

1. **Cache Parent Reference:**
   ```
   Store parent in Event Construct
 Reuse in mouse events
   ```

2. **Avoid Finding Every Time:**
   ```
   ? Get All Widgets of Class (every mouse event)
   ? Store reference once
   ```

3. **Check Validity:**
   ```
   Only show tooltip if ItemData valid
   Early exit if no data
   ```

---

## ?? Related Documentation

- **[QUICK_UI_SETUP.md](QUICK_UI_SETUP.md)** - Create tooltip widget
- **[WIDGET_COMPONENTS_GUIDE.md](WIDGET_COMPONENTS_GUIDE.md)** - Required widgets
- **[UI_WIDGET_CREATION_GUIDE.md](UI_WIDGET_CREATION_GUIDE.md)** - Full UI guide
- **[UI_API_REFERENCE.md](UI_API_REFERENCE.md)** - API reference

---

## ? Summary

### To Connect Tooltip:

1. **Create** `WBP_ItemTooltip` (2 required widgets minimum)
2. **Set** `TooltipWidgetClass` in `WBP_Inventory` Details
3. **Call** `ShowItemTooltip` on `OnMouseEnter`
4. **Call** `HideItemTooltip` on `OnMouseLeave`

**Time:** ~10 minutes total  
**Difficulty:** Easy  
**Result:** Professional tooltip system! ?

---

**Now your tooltips will show automatically when hovering over items!** ??

