# Complete UI Integration Guide

**Adding Close Buttons, Mouse Cursor, Tooltips, and Context Menus**

---

## ?? Overview

This guide covers **essential UI features** that weren't in basic setup:

1. **Close Buttons** - For Inventory and Shop windows
2. **Mouse Cursor** - Auto-show when UI opens
3. **Tooltips** - Show on item hover in slots
4. **Context Menus** - Use/Drop/Favorite items

---

## 1?? Close Button for Inventory

### Add to WBP_Inventory:

#### Hierarchy:
```
Canvas Panel (Root)
?? Border (Background)
?   ?? Vertical Box
?  ?? Horizontal Box (Header)
?       ?   ?? Text Block ("Inventory")
?    ?   ?? Spacer (Fill)
?       ?   ?? Button (CloseButton) ? ADD THIS!
?       ?       ?? Text Block ("X")
?       ?
?       ?? ... (rest of inventory)
```

#### CloseButton Properties:
```
Style: Set to minimal/flat button
Size: 32x32
Text: "X" or use image (icon_close)
Z-Order: High (always clickable)
```

### Blueprint Logic:

#### Option A: Close and Hide (Recommended for Inventory):

```blueprint
Event Construct
  ?
  ?? Get Widget By Name
      Name: "CloseButton"
    ?
      ?? Bind OnClicked
    ?
          ?? Remove from Parent
              ?? Set Input Mode Game Only
  Show Mouse Cursor: false
```

#### Option B: Auto-Close (if using InventoryWidgetBase property):

**Just add widget named `CloseButton` - system auto-binds!**

But you need to handle input mode manually:

```blueprint
// In Player Controller

Function: CloseInventory
  ?
  ?? InventoryWidget ? Remove from Parent
  ?
  ?? Set Input Mode Game Only
      Show Mouse Cursor: false
```

---

## 2?? Mouse Cursor Auto-Show

### In Player Controller:

```blueprint
Function: OpenInventory
  ?
  ?? InventoryWidget ? Add to Viewport
  ?
  ?? Set Input Mode UI Only  ?? IMPORTANT!
      Widget to Focus: InventoryWidget
      Show Mouse Cursor: true  ?? SHOWS CURSOR!
      Lock Mouse to Viewport: false


Function: CloseInventory
  ?
  ?? InventoryWidget ? Remove from Parent
  ?
  ?? Set Input Mode Game Only  ?? IMPORTANT!
      Show Mouse Cursor: false  ?? HIDES CURSOR!
```

### Input Mode Explanation:

| Mode | Description | Use For |
|------|-------------|---------|
| **Game Only** | No cursor, game controls | Playing game |
| **UI Only** | Cursor visible, no game controls | Inventory/Shop |
| **Game and UI** | Both cursor and game controls | Hybrid (not recommended) |

---

## 3?? Tooltip on Slot Hover

### In WBP_InventorySlot:

#### Event Graph:

```blueprint
Event OnSlotHovered (from C++)
  ?
  ?? Branch (ItemData valid?)
  ?   ?
  ?   ?? True:
  ??
  ?     ?? Get Parent Widget
  ?       ?   Cast to: InventoryWidgetBase
  ?       ?   Output: ParentWidget
  ?       ?   ?
  ?       ?   ?? Get Mouse Position in Viewport
  ?     ?   ?   Output: ScreenPosition
  ?       ?   ?   ?
  ?       ?   ?   ?? ParentWidget ? Show Item Tooltip
  ?  ?   ?  ItemData: Self.ItemData
  ?       ?   ?       Quantity: Self.Quantity
  ?       ?   ?       ScreenPosition: ScreenPosition
  ?       ?
  ?       ?? (Optional) Play Sound: UI_Hover


Event OnSlotUnhovered (from C++)
  ?
  ?? Get Parent Widget
      Cast to: InventoryWidgetBase
      ?
      ?? Hide Item Tooltip
```

### Alternative: Track Mouse Movement

If you want tooltip to follow cursor:

```blueprint
// In WBP_InventorySlot

Variables:
  bIsHovered (Boolean) = false

Event OnSlotHovered
  ?
  ?? Set bIsHovered = true


Event OnSlotUnhovered
  ?
  ?? Set bIsHovered = false


Event Tick (Enable Tick!)
  ?
  ?? Branch (bIsHovered?)
   ?
      ?? True:
          ?
     ?? Get Mouse Position in Viewport
          ?   ?
    ?   ?? Get Parent ? Show Item Tooltip
      ?       (updates position every frame)
```

**?? Performance Warning:** Tick is expensive! Only use if needed.

---

## 4?? Context Menu on Right-Click

### Step 1: Create Context Menu Widget

#### WBP_ItemContextMenu:

```
Canvas Panel (Root) 150x100
?? Border (Background)
    ?? Vertical Box
        ?? Button (UseButton)
        ?   ?? Text ("Use Item")
   ?
        ?? Button (DropButton)
        ?   ?? Text ("Drop")
        ?
        ?? Button (FavoriteButton)
   ?   ?? Text ("Favorite")
      ?
      ?? Button (CancelButton)
  ?? Text ("Cancel")
```

### Step 2: Show Context Menu on Right-Click

#### In WBP_InventorySlot:

```blueprint
Override: OnMouseButtonDown
  Input: Geometry, Mouse Event
  Output: Reply
  ?
  ?? Branch (Mouse Event ? Is Right Mouse Button Down?)
      ?
      ?? True: (Right-click!)
   ?   ?
      ?   ?? Create Widget
      ?   ?   Class: WBP_ItemContextMenu
      ?   ?   Output: ContextMenu
      ?   ?   ?
      ?   ?   ?? Set ContextMenu ? Target Item = Self.ItemData
      ?   ? ?   Set ContextMenu ? Target Slot = Self
      ?   ?   ?   ?
      ?   ?   ?   ?? Add to Viewport
      ?   ?   ?   ?   Z-Order: 999 (top)
      ?   ?   ?   ?   ?
      ??   ?   ?   ?? Set Position in Viewport
      ?   ?   ?   ?   Position: Get Mouse Position
      ?   ?   ?   ?
      ?   ?   ?   ?? Return: FReply::Handled()
      ?   ?
      ?   ?? Return: Handled
      ?
      ?? False:
          ?? Return: Super (default behavior)
```

### Step 3: Context Menu Logic

#### In WBP_ItemContextMenu:

**Variables:**
```
TargetItem (ItemDataAsset)
TargetSlot (InventorySlotWidget)
OwningInventory (InventoryComponent)
```

**Event Construct:**
```blueprint
Event Construct
  ?
  ?? Get Owning Player
  ?   ?
  ?   ?? Get Pawn
  ?       ?
  ?       ?? Get Component By Class
  ?         Class: InventoryComponent
  ?           ?
  ?           ?? Set OwningInventory
  ?
  ?? Bind Button Events
```

**UseButton OnClicked:**
```blueprint
Event OnClicked (UseButton)
  ?
  ?? Branch (TargetItem valid?)
  ?   ?
  ?   ?? True:
  ?       ?
  ?   ?? OwningInventory ? Use Item
  ?       ?   ItemID: TargetItem.ItemID
  ?       ?   User: Get Player Pawn
  ?       ?   ?
  ?       ?   ?? Print String: "Used {ItemName}"
  ?    ?
  ?       ?? Remove from Parent (close menu)
```

**DropButton OnClicked:**
```blueprint
Event OnClicked (DropButton)
  ?
  ?? OwningInventory ? Drop Item
  ?   ItemID: TargetItem.ItemID
  ?   Quantity: 1 (or show input dialog)
  ?   ?
  ?   ?? Print String: "Dropped {ItemName}"
  ?
  ?? Remove from Parent
```

**FavoriteButton OnClicked:**
```blueprint
Event OnClicked (FavoriteButton)
  ?
  ?? TargetSlot ? Toggle Favorite
  ?   (Add custom logic here)
  ?   ?
  ?   ?? Print String: "Toggled favorite"
  ?
  ?? Remove from Parent
```

**CancelButton OnClicked:**
```blueprint
Event OnClicked (CancelButton)
  ?
  ?? Remove from Parent
```

---

## 5?? Shop Close Button

### In WBP_Shop:

Same as Inventory, but **MUST destroy widget** (not just hide):

```blueprint
CloseButton OnClicked
  ?
  ?? Unbind from Shop (clean up references)
  ?
  ?? Remove from Parent (destroys widget)
  ?
  ?? Set Input Mode Game Only
      Show Mouse Cursor: false
```

**?? Important:** Shop widgets are created fresh each time, so destroy them!

---

## ?? Complete Example: Player Controller

### Variables:
```
InventoryWidget (WBP_Inventory)
CurrentShopWidget (WBP_Shop)
bIsUIOpen (Boolean)
```

### Event BeginPlay:
```blueprint
Event BeginPlay
  ?
  ?? Create Inventory Widget
      ?
 ?? Bind To Inventory
      ?
      ?? Set InventoryWidget
   (Don't add to viewport yet!)
```

### Input: Toggle Inventory (Tab or I):
```blueprint
Input Action: ToggleInventory
  ?
  ?? Branch (InventoryWidget in viewport?)
      ?
      ?? True: (Close inventory)
      ?   ?
    ?   ?? Remove from Parent
      ?   ?
      ?   ?? Set Input Mode Game Only
      ?   ?   Show Mouse Cursor: false  ??
      ?   ?
      ?   ?? Set bIsUIOpen = false
      ?
      ?? False: (Open inventory)
     ?
          ?? Add to Viewport
 ?
      ?? Set Input Mode UI Only  ??
          ?   Widget to Focus: InventoryWidget
  ?   Show Mouse Cursor: true  ??
 ?
          ?? Set bIsUIOpen = true
```

### Function: Open Shop:
```blueprint
Function: OpenShop
  Input: ShopActor
  ?
  ?? Create Widget (WBP_Shop)
  ?   ?
  ?   ?? Bind To Shop
  ?   ?   ShopComponent: from ShopActor
  ?   ?   PlayerInventory: Get from Player
  ?   ?   ?
  ?   ?   ?? Add to Viewport
  ?   ?   ?   ?
  ?   ?   ?   ?? Set Input Mode UI Only  ??
  ?   ?   ?   ?   Show Mouse Cursor: true  ??
  ?   ? ?   ?   ?
  ?   ?   ?   ?   ?? Set CurrentShopWidget
  ?   ?   ?   ? Set bIsUIOpen = true
```

### Input: Close UI (ESC):
```blueprint
Input Action: Cancel/Escape
  ?
  ?? Branch (CurrentShopWidget valid?)
  ?   ?
  ?   ?? True: (Shop is open)
  ?   ?   ?
  ?   ??? CurrentShopWidget ? Remove from Parent
  ?   ?   ?   ?
  ?   ?   ?   ?? Set CurrentShopWidget = null
  ?   ? ?   ?
  ?   ?   ?   ?? Close UI (Set Input Mode Game, etc.)
  ?   ?
  ?   ?? False: (Shop not open)
  ? ?
  ?  ?? Branch (InventoryWidget in viewport?)
  ?     ?
  ?           ?? True: (Inventory is open)
  ?               ?
  ?          ?? Close Inventory
  ?
  ?? Set Input Mode Game Only
      Show Mouse Cursor: false
      Set bIsUIOpen = false
```

---

## ?? Styling Tips

### Close Button:

**Hover Effect:**
```
Normal: Color (1, 1, 1, 0.5)
Hovered: Color (1, 0.2, 0.2, 1.0) - Red
Pressed: Color (0.8, 0, 0, 1.0) - Dark Red
```

**With Icon:**
```
Use Image Brush
Icon: T_UI_Close_Icon
Tint: White (normal), Red (hovered)
```

### Context Menu:

**Background:**
```
Border:
  Brush: Solid Color (0, 0, 0, 0.9) - Dark semi-transparent
  Padding: (5, 5, 5, 5)
  Shadow: Enabled (soft drop shadow)
```

**Buttons:**
```
Style: Minimal
Hover: Slight highlight (1, 1, 1, 0.2)
Text: Left-aligned
Icon: Optional (icon_use, icon_drop, etc.)
```

---

## ?? Advanced: Close on Click Outside

### Detect Click Outside UI:

```blueprint
// In WBP_Inventory or WBP_Shop

Override: OnMouseButtonDown
  Input: Geometry, Mouse Event
  ?
  ?? Branch (Is Left Mouse Button?)
?
      ?? True:
          ?
          ?? Is Mouse Over This Widget?
          ?   Input: Geometry, Mouse Event
   ?   Output: bIsOver
       ?   ?
          ?   ?? Branch (NOT bIsOver?)
   ?       ?
   ?       ?? True: (Clicked outside!)
        ?        ?
   ?        ?? Close UI
```

**?? Note:** This can be annoying! Use with caution.

---

## ?? Input Mode Cheat Sheet

| Scenario | Input Mode | Show Cursor | Focus |
|----------|------------|-------------|-------|
| **Playing** | Game Only | false | None |
| **Inventory Open** | UI Only | **true** ? | InventoryWidget |
| **Shop Open** | UI Only | **true** ? | ShopWidget |
| **Dialogue** | UI Only | true | DialogueWidget |
| **Pause Menu** | UI Only | true | PauseWidget |

---

## ?? Troubleshooting

### Cursor Doesn't Show:

**Check:**
1. `Set Input Mode UI Only` called? ?
2. `Show Mouse Cursor = true`? ?
3. Not immediately overridden by Game Only? ?

**Debug:**
```blueprint
Open Inventory:
  ?? Print String: "Input Mode: UI, Cursor: true"
```

### Cursor Doesn't Hide:

**Check:**
1. `Set Input Mode Game Only` called when closing? ?
2. `Show Mouse Cursor = false`? ?

### Tooltip Doesn't Show:

**Check:**
1. `TooltipWidgetClass` set in Inventory? ?
2. `OnSlotHovered` event firing? ?
3. ItemData valid in slot? ?

**Debug:**
```blueprint
Event OnSlotHovered:
  ?? Print String: "Hovering slot with {ItemName}"
```

### Context Menu Shows in Wrong Place:

**Fix:** Use `Set Position in Viewport` with mouse position:

```blueprint
Get Mouse Position in Viewport
  ?
  ?? ContextMenu ? Set Position in Viewport
      Position: Mouse Position + Offset(10, 10)
```

---

## ? Complete Integration Checklist

### For Inventory:

- [ ] **Close Button** added to WBP_Inventory
- [ ] **Open Inventory** sets Input Mode UI Only + Shows Cursor
- [ ] **Close Inventory** sets Input Mode Game Only + Hides Cursor
- [ ] **Tooltip** shows on slot hover (OnSlotHovered)
- [ ] **Tooltip** hides on slot unhover (OnSlotUnhovered)
- [ ] **Context Menu** shows on right-click
- [ ] **Context Menu** has Use/Drop/Cancel buttons
- [ ] **ESC key** closes UI

### For Shop:

- [ ] **Close Button** added to WBP_Shop
- [ ] **Open Shop** sets Input Mode UI Only + Shows Cursor
- [ ] **Close Shop** sets Input Mode Game Only + Hides Cursor
- [ ] **Shop** destroys widget on close (not just hide!)
- [ ] **Tooltip** works in shop slots too

---

## ?? Related Documentation

- **[WIDGET_USAGE_QUICK_REF.md](WIDGET_USAGE_QUICK_REF.md)** - Widget lifecycle
- **[TOOLTIP_INTEGRATION_GUIDE.md](TOOLTIP_INTEGRATION_GUIDE.md)** - Tooltip details
- **[SLOT_WIDGET_GUIDE.md](SLOT_WIDGET_GUIDE.md)** - Slot customization

---

## ?? Quick Summary

### Must-Have Features:

1. **Close Button**
   - Add Button widget
 - OnClicked ? Remove from Parent + Set Input Mode Game

2. **Mouse Cursor**
   - Open: `Set Input Mode UI Only` + `Show Cursor = true`
   - Close: `Set Input Mode Game Only` + `Show Cursor = false`

3. **Tooltip**
   - In Slot: `OnSlotHovered` ? `Show Item Tooltip`
   - In Slot: `OnSlotUnhovered` ? `Hide Item Tooltip`

4. **Context Menu**
   - Right-click ? Create WBP_ItemContextMenu
   - Buttons: Use, Drop, Cancel

**Time:** ~30 minutes to add all features  
**Result:** Professional, complete UI! ??

---

**Now your inventory and shop will be fully featured!** ?

