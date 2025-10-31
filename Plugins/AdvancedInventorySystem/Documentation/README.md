# Advanced Inventory System

**Version:** 1.0.0  
**Engine:** Unreal Engine 5.4+  
**Category:** Gameplay  

## Overview

The **Advanced Inventory System** is a production-ready, data-driven inventory plugin for Unreal Engine. It provides a complete, modular architecture for managing items, trading, effects, and UI components without requiring any C++ knowledge for basic usage.

### Key Features

- ? **100% Blueprint-Friendly** - Full functionality accessible via Blueprints
- ?? **Data-Driven Design** - Define items, shops, and effects as Data Assets
- ?? **Event-Driven Architecture** - No Tick overhead, responds to events only
- ?? **Modular Plugin System** - Five independent modules you can use separately
- ?? **Item Effects System** - Trigger effects on use, gift, equip, or ownership
- ?? **Trading & Shops** - Full shop system with dynamic pricing
- ?? **Stacking & Weight** - Optional weight limit and stack management
- ?? **Save/Load Support** - Built-in JSON serialization
- ?? **Multiplayer Ready** - Replicated inventory component
- ?? **UI Widgets Included** - Base widget classes for inventory, shops, tooltips

---

## Quick Feature List

| Feature | Status | Module |
|---------|--------|--------|
| Item Data Assets | ? | Core |
| Inventory Component | ? | Runtime |
| Stacking System | ? | Runtime |
| Weight Limit System | ? | Runtime |
| Equipment System | ? | Runtime |
| Item Effects (6 trigger types) | ? | Effects |
| Needs System Integration | ? | Effects |
| Dialogue System Integration | ? | Effects |
| Shop Component | ? | Trade |
| Dynamic Pricing | ? | Trade |
| Shop Inventory & Restocking | ? | Trade |
| Currency System | ? | Trade |
| Inventory Widget Base | ? | UI |
| Shop Widget Base | ? | UI |
| Tooltip Widget | ? | UI |
| Context Menu Widget | ? | UI |
| Slot Widget | ? | UI |
| Save/Load (JSON) | ? | Core |
| Replication Support | ? | Runtime |

---

## System Requirements

### Minimum Requirements

- **Unreal Engine:** 5.4 or later
- **C++ Compiler:** Not required for Blueprint users
- **Build Configuration:** Development or Shipping

### Optional Integrations

The plugin includes optional interfaces for integration with:
- **Needs System** (hunger, thirst, etc.) - via `INeedsSystemInterface`
- **Dialogue System** - via `IDialogueSystemInterface`
- **Time of Day System** - via `ITimeOfDaySystemInterface`

These are **not required** to use the plugin.

---

## Installation

### Step 1: Add Plugin to Your Project

1. **Option A: Marketplace Installation** *(if applicable)*
   - Install from the Unreal Marketplace
   - Enable the plugin in your project

2. **Option B: Manual Installation**
   - Copy the `AdvancedInventorySystem` folder to your project's `Plugins` directory
   - If `Plugins` folder doesn't exist, create it at the project root
   - Your path should be: `YourProject/Plugins/AdvancedInventorySystem/`

### Step 2: Enable the Plugin

1. Open your project in Unreal Editor
2. Go to **Edit ? Plugins**
3. Search for "Advanced Inventory System"
4. Check the **Enabled** checkbox
5. Click **Restart Now**

### Step 3: Verify Installation

After restart, verify the plugin is loaded:

1. Go to **Content Browser**
2. Enable **Show Plugin Content** (Settings ? Show Plugin Content)
3. You should see **Advanced Inventory System Content** folder

---

## Quick Start (5 Minutes)

This guide gets you up and running with a basic inventory system.

### 1. Create Your First Item (Data Asset)

1. In **Content Browser**, right-click ? **Miscellaneous ? Data Asset**
2. Select **ItemDataAsset**
3. Name it `DA_Apple`
4. Open the asset and configure:

```
Item ID: Apple
Item Name: "Apple"
Item Description: "A delicious red apple. Restores hunger."
Rarity: Common
Category: Consumable
Max Stack Size: 10
Is Stackable: ? (checked)
Is Usable: ? (checked)
Consume On Use: ? (checked)
Base Value: 5
Can Be Sold: ? (checked)
Item Weight: 0.1
```

5. **Save** the asset

### 2. Add Inventory to Your Character

1. Open your **Character Blueprint** (e.g., `BP_ThirdPersonCharacter`)
2. In the **Components** panel, click **Add** ? search for `Inventory Component`
3. Add the **Inventory Component**
4. Configure the component:

```
Max Inventory Slots: 20
Max Weight Capacity: 100.0 (or 0 for unlimited)
Auto Stack: ? (checked)
```

5. **Compile and Save**

### 3. Test Adding Items (Blueprint)

1. In your Character Blueprint's **Event Graph**, add this test code:

```blueprint
Event BeginPlay
  ?
Get Component (InventoryComponent)
  ?
Add Item (ItemData: DA_Apple, Quantity: 5)
  ?
Print String (Result: Success or Failed)
```

2. **Play In Editor** - you should see "Success" printed

### 4. Create a Simple Inventory UI

1. Right-click in Content Browser ? **User Interface ? Widget Blueprint**
2. Name it `WBP_InventoryUI`
3. Set **Parent Class** to `InventoryWidgetBase`
4. In the **Designer**:
   - Add a **Uniform Grid Panel** (name it exactly `InventoryGrid` - required!)
   - Optionally add:
     - **Text Block** named `SlotsText` (shows "5 / 20")
     - **Text Block** named `WeightText` (shows current weight)
     - **Button** named `CloseButton` (auto-binds to close)

5. In **Class Settings**, set:
   - **Grid Columns:** 5
   - **Slot Size:** 80

6. **Compile and Save**

### 5. Show the Inventory UI

In your Character Blueprint:

```blueprint
Event [YourInputKey] (e.g., "I" for Inventory)
  ?
Create Widget (Class: WBP_InventoryUI)
  ?
Bind To Inventory (Inventory Component: Self ? GetComponentByClass(InventoryComponent))
  ?
Show Inventory
```

**That's it!** Press your inventory key in-game to see your items.

---

## Module Architecture

The plugin is split into **5 independent modules**:

| Module | Purpose | Dependencies |
|--------|---------|--------------|
| **InventorySystemCore** | Base types, Data Assets, Interfaces | None |
| **InventorySystemRuntime** | Inventory & Item components | Core |
| **InventorySystemEffects** | Item effect execution | Core, Runtime |
| **InventorySystemTrade** | Shop & trading | Core, Runtime |
| **InventorySystemUI** | UI widget base classes | Core, Runtime |

**Source Locations:**
- `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/`
- `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/`
- `Plugins/AdvancedInventorySystem/Source/InventorySystemEffects/`
- `Plugins/AdvancedInventorySystem/Source/InventorySystemTrade/`
- `Plugins/AdvancedInventorySystem/Source/InventorySystemUI/`

---

## What's Next?

?? **[User Guide](UserGuide.md)** - Complete step-by-step instructions  
?? **[Tutorials](Tutorials.md)** - 5 hands-on tutorials  
?? **[Blueprint Reference](BlueprintReference.md)** - All Blueprint nodes explained  
?? **[Data Reference](DataReference.md)** - All Data Assets and their properties  
?? **[Examples](Examples.md)** - Ready-to-use recipes  
?? **[FAQ & Troubleshooting](FAQ_Troubleshooting.md)** - Common issues and solutions  

---

## Support & Community

- **Documentation:** See the `/Documentation/` folder in the plugin
- **Issues:** Report bugs via your project's issue tracker
- **Feature Requests:** Contact the development team

---

## License

Copyright Epic Games, Inc. All Rights Reserved.

---

## Credits

**Created By:** Game Development Team  
**Version:** 1.0.0  
**Last Updated:** 2024
