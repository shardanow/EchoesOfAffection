# UI Widget Creation Guide

**Step-by-step guide to create Blueprint Widgets for the Inventory System**

---

## ?? Overview

This guide will help you create **Blueprint Widget Blueprints** based on the C++ base classes provided by the plugin:

- `UInventoryWidgetBase` ? `WBP_Inventory` (Inventory UI)
- `UItemTooltipWidget` ? `WBP_ItemTooltip` (Tooltip)
- `UShopWidgetBase` ? `WBP_Shop` (Shop UI)

---

## ?? Creating the Inventory Widget

### Step 1: Create Widget Blueprint

1. **Content Browser** ? Right-click in `Content/UI/` folder
2. **User Interface** ? **Widget Blueprint**
3. Name it: `WBP_Inventory`
4. Open the Widget Blueprint

### Step 2: Reparent to C++ Class

1. **File** ? **Reparent Blueprint**
2. Search for: `InventoryWidgetBase`
3. Select it and click **Select**

? Now your widget inherits all C++ functionality!

### Step 3: Design the UI

#### Layout Structure (Recommended):

```
Canvas Panel (Root)
?? Border (Background)
    ?? Vertical Box
        ?? Text Block (Title: "Inventory")
  ?? Horizontal Box (Info Bar)
        ?   ?? Text Block ("Weight: ")
        ?   ?? Text Block (WeightValue - Bound)
        ?   ?? Spacer
 ?   ?? Text Block ("Slots: ")
        ?   ?? Text Block (SlotsValue - Bound)
        ?? Uniform Grid Panel (InventoryGrid)
      ?? (Slots will be added dynamically)
```

#### Visual Setup:

**Canvas Panel:**
- Anchors: Full Screen
- Offsets: 0, 0, 0, 0

**Border (Background):**
- Brush Color: Black (A: 0.8)
- Padding: 20, 20, 20, 20

**Title Text:**
- Text: "Inventory"
- Font Size: 36
- Justification: Center

**Uniform Grid Panel:**
- Slot Padding: 5
- Minimum Desired Slot Width: 80
- Minimum Desired Slot Height: 80

### Step 4: Create Inventory Slot Widget

Create a separate widget for slots: `WBP_InventorySlot`

**Structure:**
```
Overlay (Root)
?? Border (Background)
?   ?? Image (ItemIcon)
?? Text Block (Quantity - Bottom Right)
```

**Variables:**
- `ItemData` (ItemDataAsset) - The item in this slot
- `ItemID` (FInventoryItemId) - Instance ID
- `SlotIndex` (Integer) - Slot position
- `Quantity` (Integer) - Stack size

**Events:**
- `OnClicked` ? Use Item
- `OnHovered` ? Show Tooltip
- `OnUnhovered` ? Hide Tooltip

### Step 5: Implement Blueprint Logic

#### Event Graph:

**Event Construct:**
```blueprint
Event Construct
  ?
  ?? Get Owning Player Pawn
  ?   ?
?   ?? Get Component By Class
  ? Class: InventoryComponent
  ?     Output: InventoryRef (Save as variable)
  ?       ?
  ? ?? Bind To Inventory
  ?           Input: InventoryRef
  ?
?? Refresh Inventory (Custom Event - call it)
```

**OnInventoryUpdated (Event):**
```blueprint
Event OnInventoryUpdated (from C++)
  ?
  ?? Refresh Inventory
```

**Custom Event: Refresh Inventory**
```blueprint
Custom Event: Refresh Inventory
  ?
  ?? Clear Children (Uniform Grid Panel)
  ?
  ?? Get All Items (from InventoryRef)
  ?   Output: Items (Array)
  ?   ?
  ?   ?? For Each Loop
  ?    ?
  ?       ?? Create Widget
  ?       ?   Class: WBP_InventorySlot
  ?       ?   Output: SlotWidget
  ?       ?   ?
  ?       ? ?? Set Item Data (on SlotWidget)
  ?   ?   ?   Input: Loop Element
  ??   ?
  ? ?   ?? Add Child to Uniform Grid
  ?       ?       Content: SlotWidget
  ?       ?       Row: (Loop Index / 5)
  ?   ?       Column: (Loop Index MOD 5)
  ?    ?
  ?       ?? Continue Loop
  ?
  ?? Update Weight/Slots Display
```

**Update Info Display:**
```blueprint
Custom Event: UpdateInfoDisplay
  ?
  ?? Get Current Weight
  ?   Output: CurrentWeight
  ?   ?
  ?   ?? Set Text (WeightValueText)
  ?       Text: Format("{0} / {1}", CurrentWeight, MaxWeight)
  ?
  ?? Get Slot Count
      Output: UsedSlots, MaxSlots
      ?
      ?? Set Text (SlotsValueText)
          Text: Format("{0} / {1}", UsedSlots, MaxSlots)
```

### Step 6: Test the Widget

1. Add to viewport in Player Controller or HUD
2. Press Play
3. Add items to inventory ? UI should update automatically!

---

## ??? Creating the Tooltip Widget

### Step 1: Create Widget Blueprint

1. **Content Browser** ? `Content/UI/`
2. **User Interface** ? **Widget Blueprint**
3. Name: `WBP_ItemTooltip`
4. **Reparent** to `ItemTooltipWidget`

### Step 2: Design the Tooltip

#### Layout:

```
Overlay (Root)
?? Border (Background with shadow)
    ?? Vertical Box (Padding: 10)
    ?? Horizontal Box (Header)
 ?   ?? Image (ItemIcon - 48x48)
        ?   ?? Vertical Box
        ?       ?? Text Block (ItemName - Large, Bold)
     ?       ?? Text Block (ItemCategory - Small, Italic)
      ?
        ?? Spacer (Height: 5)
   ?
        ?? Text Block (ItemDescription - Wrap Text)
        ?
        ?? Spacer (Height: 5)
        ?
        ?? Horizontal Box (Stats Row 1)
        ?   ?? Text Block ("Value: ")
 ?   ?? Text Block (ValueText - Bound)
        ?
        ?? Horizontal Box (Stats Row 2)
        ?   ?? Text Block ("Weight: ")
?   ?? Text Block (WeightText - Bound)
        ?
  ?? Horizontal Box (Rarity)
    ?? Text Block ("Rarity: ")
            ?? Text Block (RarityText - Color Bound to Rarity)
```

### Step 3: Bind Data

#### Event Pre Construct:

```blueprint
Event Pre Construct
  ?
  ?? Branch (Is Item Data Valid?)
  ?   ?
  ?   ?? True:
  ?       ?
  ?       ?? Get Item Name
  ?       ?   ?? Set Text (ItemNameText)
  ?    ?
  ?       ?? Get Item Description
  ?       ?   ?? Set Text (DescriptionText)
  ?       ?
  ?       ?? Get Item Icon
  ?    ?   ?? Set Brush from Texture (IconImage)
  ?       ?
  ?       ?? Get Item Value
  ?       ?   ?? Set Text (ValueText)
  ?       ?       Format: "{0} Gold"
  ?       ?
  ?       ?? Get Item Weight
  ?       ?   ?? Set Text (WeightText)
  ?    ?
  ?       ?? Get Rarity Color
  ?       ?   ?
?       ?   ?? Set Color and Opacity (RarityText)
  ?       ?
  ?       ?? Get Item Rarity
  ??? Set Text (RarityText)
  ?       Convert Enum to Text
```

### Step 4: Helper Functions

Create these as **Functions** in the widget:

**Function: Show Tooltip**
```blueprint
Function: ShowTooltip
  Input: ItemData (ItemDataAsset)
  Input: Position (Vector2D)
  ?
  ?? Set Item Data
  ?   Input: ItemData
  ?
  ?? Add to Viewport
  ?   Z-Order: 100
  ?
  ?? Set Position in Viewport
      Position: Position + Offset(10, 10)
```

**Function: Hide Tooltip**
```blueprint
Function: HideTooltip
  ?
  ?? Remove from Parent
```

---

## ?? Creating the Shop Widget

### Step 1: Create Widget Blueprint

1. **Content Browser** ? `Content/UI/`
2. **Widget Blueprint** ? `WBP_Shop`
3. **Reparent** to `ShopWidgetBase`

### Step 2: Design the Shop UI

#### Layout:

```
Canvas Panel (Root)
?? Border (Background)
?? Horizontal Box
        ?? Vertical Box (Shop Inventory - 50%)
        ?   ?? Text Block ("Shop Inventory")
        ?   ?? Scroll Box
        ?   ?   ?? Vertical Box (ShopItemsList)
        ?   ? ?? (Items added dynamically)
        ?   ?? Text Block ("Shop Keeper: {Name}")
        ?
        ?? Spacer (Width: 20)
   ?
        ?? Vertical Box (Player Inventory - 50%)
          ?? Text Block ("Your Inventory")
      ?? Scroll Box
        ?   ?? Vertical Box (PlayerItemsList)
       ?       ?? (Items added dynamically)
    ?? Horizontal Box (Currency Display)
                ?? Text Block ("Gold: ")
     ?? Text Block (GoldAmountText - Bound)
```

### Step 3: Create Shop Item Widget

Create `WBP_ShopItem`:

```
Horizontal Box (Root)
?? Image (ItemIcon - 64x64)
?? Vertical Box (Item Info)
?   ?? Text Block (ItemName)
?   ?? Text Block (ItemDescription - Small)
?   ?? Horizontal Box
?    ?? Text Block ("Price: ")
?       ?? Text Block (PriceText)
?? Spacer
?? Button (BuyButton / SellButton)
    ?? Text Block ("Buy" / "Sell")
```

**Variables:**
- `ItemData` (ItemDataAsset)
- `Price` (Integer)
- `IsBuyMode` (Boolean) - true = buy, false = sell
- `Quantity` (Integer)

### Step 4: Implement Shop Logic

#### Event Construct:

```blueprint
Event Construct
  ?
  ?? Custom Event: InitializeShop
      Input: ShopComponent
      Input: PlayerInventory
    ?
      ?? Bind To Shop
      ?   Input: ShopComponent
      ?   Input: PlayerInventory
      ?
      ?? Refresh Shop Display
```

**OnShopInventoryUpdated (Event):**
```blueprint
Event OnShopInventoryUpdated
  ?
  ?? Refresh Shop Items
```

**Custom Event: Refresh Shop Items**
```blueprint
Custom Event: RefreshShopItems
  ?
  ?? Clear Children (ShopItemsList)
  ?
  ?? Get Shop Component ? Get Shop Data
  ?   Output: ShopData
  ?   ?
  ?   ?? For Each (ShopData.ShopInventory)
  ?       ?
  ?       ?? Create Widget (WBP_ShopItem)
  ?   ?   Output: ShopItemWidget
  ?       ?   ?
  ?       ?   ?? Set Item Data (Loop Element.ItemData)
  ?       ?   ?? Set Price (Loop Element.Price)
  ?       ?   ?? Set Is Buy Mode (True)
  ?    ?   ?? Bind OnBuyClicked
  ?       ?   ?   ?? Purchase Item
  ?       ?   ?
  ?       ?   ?? Add Child (ShopItemsList)
  ?  ?
  ?  ?? Continue
  ?
  ?? Refresh Player Items (similar for sell)
```

**On Buy Button Clicked (in WBP_ShopItem):**
```blueprint
Event OnClicked (BuyButton)
  ?
  ?? Get Parent Widget
  ?   Cast to: WBP_Shop
  ??
  ?   ?? Purchase Item
  ?       Input: ItemData
  ?       Input: Quantity (from spinner/input)
  ?       Output: Success
  ?   ?
  ?     ?? Branch (Success?)
  ?       ?
  ?           ?? True:
  ?           ?   ?? Play Sound (Purchase Sound)
  ?         ?   ?? Show Notification ("Purchased!")
  ?           ?
  ?      ?? False:
  ?   ?? Show Error ("Not enough gold!")
```

**Update Currency Display:**
```blueprint
Custom Event: UpdateCurrency
  ?
  ?? Get Customer Currency
      Output: Gold
      ?
      ?? Set Text (GoldAmountText)
       Text: Format("{0}", Gold)
```

---

## ?? Styling Tips

### Color Schemes:

**Rarity Colors:**
- Common: `(0.8, 0.8, 0.8)` - Gray
- Uncommon: `(0.2, 1.0, 0.2)` - Green
- Rare: `(0.2, 0.5, 1.0)` - Blue
- Epic: `(0.8, 0.2, 1.0)` - Purple
- Legendary: `(1.0, 0.6, 0.0)` - Orange

**UI Colors:**
- Background: `(0.1, 0.1, 0.1, 0.9)` - Dark
- Selected Slot: `(0.3, 0.6, 1.0, 0.5)` - Blue
- Hover: `(1.0, 1.0, 1.0, 0.2)` - White

### Fonts:

- Title: Size 36, Bold
- Item Name: Size 18, Bold
- Description: Size 14, Regular
- Price: Size 16, Bold

---

## ?? Adding Widgets to HUD

### Method 1: Player Controller

In your **Player Controller Blueprint**:

```blueprint
Event BeginPlay
  ?
  ?? Create Widget
      Class: WBP_Inventory
      Output: InventoryWidget (save as variable)
?
      ?? Add to Viewport
          Z-Order: 0
```

**Toggle Inventory (Input Action):**
```blueprint
Input Action: ToggleInventory
  ?
  ?? Branch (Is in Viewport?)
  ?   ?
  ?   ?? True:
  ?   ?   ?? Remove from Parent (InventoryWidget)
  ?   ?       ?? Set Input Mode Game Only
  ?   ?
  ?   ?? False:
  ?       ?? Add to Viewport (InventoryWidget)
  ??? Set Input Mode UI Only
```

### Method 2: HUD Class

Create `BP_InventoryHUD` (inherits from HUD):

```blueprint
Event BeginPlay
  ?
  ?? Create Widget (WBP_Inventory)
  ?   ?? Add to Viewport (Hidden)
  ?
  ?? Create Widget (WBP_ItemTooltip)
  ?   ?? Save as TooltipWidget
  ?
  ?? Create Widget (WBP_Shop)
  ?? Save as ShopWidget
```

---

## ?? Common Patterns

### Pattern 1: Slot Click to Use

```blueprint
In WBP_InventorySlot:

Event OnClicked
  ?
  ?? Get Owning Player Pawn
  ?   ?
  ?   ?? Get Component (InventoryComponent)
  ?       ?
  ?  ?? Use Item
  ? Input: ItemID
  ?           Input: User = Owning Pawn
  ?    Output: Result
  ?        ?
  ?           ?? Branch (Success?)
  ? ?? True: Play Animation
```

### Pattern 2: Drag and Drop

```blueprint
Override: On Mouse Button Down
  ?
  ?? Detect Drag
      Drag Key: Left Mouse Button
    ?
      ?? Create Drag Drop Operation
     Payload: ItemID + SlotIndex
      Visual: Clone of ItemIcon


Override: On Drop
  Input: Operation
  ?
  ?? Get Payload ? SourceSlotIndex
  ?
  ?? Swap Items
      Input: SourceSlot, TargetSlot (this)
```

### Pattern 3: Right-Click Context Menu

```blueprint
Override: On Mouse Button Down
  Input: Mouse Event
  ?
  ?? Branch (Is Right Mouse Button?)
      ?
      ?? True:
          ?
          ?? Create Widget (WBP_ContextMenu)
?
    ?? Add Option ("Use")
     ?? Add Option ("Drop")
            ?? Add Option ("Info")
        ?
       ?? Show at Mouse Position
```

---

## ? Testing Checklist

Before finalizing your widgets, test:

- [ ] Inventory updates when items added/removed
- [ ] Tooltip shows correct information
- [ ] Tooltip follows mouse cursor
- [ ] Shop buy/sell works correctly
- [ ] Currency updates after transactions
- [ ] Drag and drop (if implemented)
- [ ] Context menus (if implemented)
- [ ] Widgets close properly
- [ ] No memory leaks (widgets removed from viewport)
- [ ] Performance is acceptable (60fps with full inventory)

---

## ?? Next Steps

1. **Create the widgets** following this guide
2. **Style them** to match your game's aesthetic
3. **Test thoroughly** with different items
4. **Add polish:**
   - Animations (fade in/out)
   - Sound effects
   - Particle effects
   - Screen shake on purchase
5. **Optimize** if needed (widget pooling for large inventories)

---

## ?? Additional Resources

- **C++ Base Classes:**
  - `InventoryWidgetBase.h`
  - `ItemTooltipWidget.h`
  - `ShopWidgetBase.h`

- **Examples:**
  - See `BLUEPRINT_EXAMPLES.md` for code snippets
  - See `USER_GUIDE.md` for usage scenarios

---

**Happy Widget Creating!** ??

