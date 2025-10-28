# Quick UI Setup Guide - With Auto-Binding

**Creating Blueprint Widgets is now MUCH simpler with automatic widget binding!**

---

## ?? How Auto-Binding Works

When you create a Blueprint Widget from C++ base classes, the system **automatically finds and binds** widgets by name.

### Widget Binding Levels:

- ?? **REQUIRED** (`BindWidget`) - **Must exist or compilation ERROR**
- ? **RECOMMENDED** (`BindWidgetOptional`) - Should exist for good UX
- ?? **OPTIONAL** (`BindWidgetOptional`) - Nice to have

?? **See [WIDGET_COMPONENTS_GUIDE.md](WIDGET_COMPONENTS_GUIDE.md) for complete details**

---

## ??? Creating Item Tooltip Widget

### Step 1: Create Widget Blueprint

1. **Content Browser** ? `Content/UI/`
2. Right-click ? **User Interface** ? **Widget Blueprint**
3. Name: `WBP_ItemTooltip`
4. Open it

### Step 2: Reparent to C++ Class

1. **File** ? **Reparent Blueprint**
2. Search: `ItemTooltipWidget`
3. Select and click **Reparent**

### Step 3: Add Widgets (Minimum Required)

**?? REQUIRED - Must have these 2:**

| Widget Type | Variable Name | Purpose |
|-------------|---------------|---------|
| **Text Block** | `ItemNameText` | Item name (auto-colored by rarity) ?? |
| **Image** | `ItemIconImage` | Icon (auto-set from ItemData) ?? |

**Without these 2, compilation will FAIL!**

### Step 4: Add Recommended Widgets

**? RECOMMENDED - Add these 4 for good UX:**

| Widget Type | Variable Name | Purpose |
|-------------|---------------|---------|
| **Text Block** | `ItemDescriptionText` | Description |
| **Text Block** | `ItemValueText` | Value (auto-formatted "50 Gold") |
| **Text Block** | `ItemRarityText` | Rarity (auto-colored) |
| **Border** | `BackgroundBorder` | Background (auto-colored by rarity) |

### Step 5 (Optional): Add Advanced Widgets

**?? OPTIONAL - Advanced features:**

| Widget Type | Variable Name | Purpose |
|-------------|---------------|---------|
| Text Block | `ItemWeightText` | Weight (auto-formatted) |
| Text Block | `ItemCategoryText` | Category |
| Text Block | `QuantityText` | Stack size (auto-hidden if 1) |
| Text Block | `ItemTypeText` | Stackable/Usable/Equippable |
| Image | `QuestItemIcon` | Quest item indicator (auto-hidden) |

### Minimal Layout Example (2 minutes):

```
Canvas Panel (Root)
?? Border
    ?? Horizontal Box
        ?? Image (ItemIconImage) ?? REQUIRED
   ?? Text Block (ItemNameText) ?? REQUIRED
```

### Recommended Layout Example (5 minutes):

```
Canvas Panel (Root)
?? Border (BackgroundBorder) ? Auto-colored!
    ?? Vertical Box (Padding: 10)
        ?? Horizontal Box
   ?   ?? Image (ItemIconImage) 64x64 ?? REQUIRED
        ?   ?? Vertical Box
    ? ?? Text (ItemNameText) Size: 18 ?? REQUIRED
   ?       ?? Text (ItemRarityText) Size: 12 ?
        ?? Text (ItemDescriptionText) Wrap ?
      ?? Horizontal Box
        ?   ?? Text ("Value: ")
        ? ?? Text (ItemValueText) ? Auto-formatted!
        ?? Text (QuantityText) Bottom-Right ?? Auto-hidden!
```

### Step 6: Done!

**Compile (Ctrl+F7):**
- ? Success ? You have minimum requirements!
- ? Error ? Missing required widget (ItemNameText or ItemIconImage)

**That's it!** No Blueprint code needed. The C++ class handles everything:

? Automatic text updates  
? Automatic color based on rarity  
? Automatic icon from ItemData  
? Automatic show/hide for quantity  
? Automatic formatting for value/weight  

### Usage:

```blueprint
Event Construct
  ?
  ?? Set Item Data
      Input: DA_Potion_Health
      Input: Quantity: 5
```

**Everything updates automatically!** ?

