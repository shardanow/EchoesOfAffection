# FAQ & Troubleshooting - Advanced Inventory System

Common questions, issues, and solutions.

---

## Table of Contents

1. [General Questions](#general-questions)
2. [Setup & Installation](#setup--installation)
3. [Items & Data Assets](#items--data-assets)
4. [Inventory Component](#inventory-component)
5. [Item Effects](#item-effects)
6. [Shop System](#shop-system)
7. [UI Widgets](#ui-widgets)
8. [Multiplayer](#multiplayer)
9. [Performance](#performance)
10. [Troubleshooting Checklist](#troubleshooting-checklist)

---

## General Questions

### Q: Do I need to know C++ to use this plugin?

**A:** No! The plugin is 100% Blueprint-friendly. All functionality is exposed to Blueprints. C++ knowledge is only needed if you want to extend the core systems.

**Source:** All components and functions are marked `UFUNCTION(BlueprintCallable)` or `BlueprintPure`.

---

### Q: Is this plugin multiplayer-ready?

**A:** Yes, but it uses **server-authoritative** model. You must call inventory functions on the server via RPCs.

**Example:**
```blueprint
[Client: Pickup Item]
?
  Custom Event: ServerPickupItem (Run On Server, Reliable)
    ?
    Add Item (executes on server, replicates to clients)
```

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Public/InventoryComponent.h` - Component has `UPROPERTY(Replicated)` for InventoryItems array.

---

### Q: Can I use this plugin without the Needs/Dialogue systems?

**A:** Yes! The interfaces (`INeedsSystemInterface`, `IDialogueSystemInterface`) are **optional**. If you don't implement them, those effects simply won't execute.

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/InventorySystemInterfaces.h`

---

### Q: Does this plugin use Tick?

**A:** No! The inventory system is **event-driven**. Zero tick overhead.

**Source:** Check `InventoryComponent.h` - no `Tick()` function defined.

---

## Setup & Installation

### Issue: Plugin shows as "Missing" after adding to project

**Symptoms:**
- Project fails to open
- Error: "The following modules are missing or built with a different engine version"

**Solution:**
1. Delete `Intermediate` and `Saved` folders
2. Right-click `.uproject` ? **Generate Visual Studio project files**
3. Build from source (if C++ project)
4. Reopen project

---

### Issue: "Cannot find InventoryComponent in Add Component menu"

**Symptoms:**
- Component doesn't appear in Blueprints

**Solution:**
1. Verify plugin is enabled: **Edit ? Plugins** ? Search "Advanced Inventory System" ? Check **Enabled**
2. Restart editor
3. If still missing, check Content Browser ? **Settings** ? **Show Plugin Content** is enabled

---

### Issue: Compilation errors after installation

**Symptoms:**
- Errors like "Cannot open include file: 'InventoryComponent.h'"

**Solution:**
1. Check your project's `.Build.cs` file includes the modules:
```csharp
PublicDependencyModuleNames.AddRange(new string[] { 
    "Core", "CoreUObject", "Engine", 
    "InventorySystemCore",
    "InventorySystemRuntime"
});
```
2. Regenerate project files
3. Build

---

## Items & Data Assets

### Issue: Items not stacking

**Symptoms:**
- Adding stackable items creates new slots instead of stacking

**Checklist:**
1. ? `Is Stackable` is **checked** in ItemDataAsset
2. ? `Max Stack Size` is **> 1** (or 0 for infinite)
3. ? `Auto Stack` is **checked** in InventoryComponent
4. ? Both items have **same `Item ID`**

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/ItemDataAsset.h` (lines 102-110)

---

### Issue: "Item ID must be unique" warning

**Symptoms:**
- Multiple items with same ID causing conflicts

**Solution:**
Use a naming convention:
```
Category_Subcategory_Name
Examples:
  Potion_Health_Small
  Weapon_Sword_Iron
  Armor_Helmet_Leather
  Currency_Gold
```

**Why:** `Item ID` is used for stacking, saving, and lookups. Duplicates cause undefined behavior.

---

### Issue: Item icon not showing in UI

**Checklist:**
1. ? `Item Icon` is assigned in ItemDataAsset
2. ? Texture is **imported** (not just a file path)
3. ? Texture compression is set to **UserInterface2D** (not Default)
4. ? Widget has `ItemIcon` or `ItemIconImage` widget (exact name required)

**Fix Texture Compression:**
1. Open texture asset
2. **Compression Settings** ? **UserInterface2D (RGBA)**
3. **Save**

---

### Issue: Weight system not working

**Symptoms:**
- `Get Total Weight` always returns 0

**Checklist:**
1. ? `Item Weight` is set in ItemDataAsset (default is 0.0)
2. ? `Max Weight Capacity` is > 0 in InventoryComponent (0 = unlimited weight)

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Public/InventoryComponent.h` (lines 298-347)

---

## Inventory Component

### Issue: Add Item fails with "Insufficient Space"

**Symptoms:**
- `Out Result` returns `Failed_InsufficientSpace`

**Possible Causes:**

**1. Inventory full (slots)**
```blueprint
Get Free Slots ? Returns 0
Solution: Increase Max Inventory Slots or clear inventory
```

**2. Overweight**
```blueprint
Get Total Weight ? Returns value >= Max Weight Capacity
Solution: Increase Max Weight Capacity or reduce item weight
```

**3. Blocked tags**
```blueprint
InventoryComponent ? Blocked Item Tags contains item's tags
Solution: Remove blocking tags or change item tags
```

---

### Issue: Inventory events not firing

**Symptoms:**
- `OnItemAdded`, `OnInventoryChanged` don't trigger

**Checklist:**
1. ? Event is **bound** (not just created)
   ```blueprint
   Bind Event to On Item Added ? [Your Function]
   ```
2. ? Binding happens **before** the item operation
   ```blueprint
   Event BeginPlay
     ?
     Bind Event [FIRST]
     ?
     Add Item [SECOND]
   ```
3. ? Component reference is valid

**Common Mistake:**
```blueprint
? WRONG:
Event On Item Added (not bound, just an event node)

? CORRECT:
Get Component ? Bind Event to On Item Added ? Custom Event
```

---

### Issue: Item removed but not found

**Symptoms:**
- `Remove Item By Data` returns 0 (nothing removed)

**Debug Steps:**
```blueprint
1. Get All Items ? Print count
2. ForEachLoop ? Print Item ? GetItemData ? ItemID
3. Check if expected Item ID is in list
4. Check if Item Data reference matches exactly
```

**Common Cause:** Item Data reference mismatch (created new DA instead of using existing one)

---

## Item Effects

### Issue: Item effects not executing

**Symptoms:**
- Using item doesn't trigger effects

**Checklist:**
1. ? `ItemEffectExecutorComponent` is added to character
2. ? Effect is added to `ItemDataAsset ? Item Effects` array
3. ? Effect `Trigger Type` matches the action (e.g., `OnUse` for using items)
4. ? Effect `Target Type` is `Self` (or correct target)
5. ? Effect conditions are met (tags, level, etc.)

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemEffects/Public/ItemEffectExecutorComponent.h`

---

### Issue: Needs modifications not working

**Symptoms:**
- Effect fires but needs don't change

**Requirements:**
1. ? Target actor implements `INeedsSystemInterface`
2. ? `ModifyNeed` function is implemented
3. ? `Need Tag` matches your needs system tags

**Implementation Example:**
```blueprint
[Character Blueprint - Interface: INeedsSystemInterface]

Function: Modify Need (Interface Message)
  Inputs: NeedTag, DeltaValue, bClamp
  ?
  Get Component ? NeedsComponent
  ?
  Modify Need Value
    - Need: NeedTag
    - Delta: DeltaValue
```

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/InventorySystemInterfaces.h` (lines 40-58)

---

### Issue: Dialogue not triggering

**Similar to needs:**
1. ? NPC implements `IDialogueSystemInterface`
2. ? `TriggerDialogue` function is implemented
3. ? `Dialogue Tag` matches your dialogue system

---

### Issue: WhileEquipped effects stop working

**Symptoms:**
- Passive effects disappear

**Causes:**
1. Item was unequipped
2. Item was removed from inventory
3. `ItemEffectExecutorComponent` was destroyed

**Solution:** Effects auto-stop when item is unequipped. This is intentional behavior.

**Source:** `ItemEffectExecutorComponent::StopPersistentEffects()` is called automatically.

---

## Shop System

### Issue: Purchase fails with "Invalid Item"

**Symptoms:**
- `Purchase Item` returns `Failed_InvalidItem`

**Checklist:**
1. ? Item is in `ShopData ? Shop Inventory` array
2. ? `Shop Data` is assigned to `ShopComponent`
3. ? Item reference matches exactly (not a duplicate DA)

---

### Issue: Player can't afford item but currency is sufficient

**Symptoms:**
- Has enough gold, but purchase fails

**Debug:**
```blueprint
Get Item Quantity (DA_Gold) ? Print
Get Purchase Price (Item, 1, Buyer) ? Print
Compare values
```

**Common Causes:**
1. Currency item is different (e.g., using DA_Silver instead of DA_Gold)
2. Currency is in wrong inventory (check buyer's inventory)
3. Shop `Currency Item` not set in ShopDataAsset

---

### Issue: Shop inventory never restocks

**Checklist:**
1. ? `bRestocks` is **checked** in shop item entry
2. ? `Restock Interval` is > 0
3. ? Game time is passing (not paused)

**Manual Restock:**
```blueprint
ShopComponent ? Restock All
```

---

### Issue: Sellback price is wrong

**Formula Check:**
```
Expected: Item.BaseValue ? ShopData.SellbackPercentage
Actual: GetSellbackPrice(Item, 1, Seller)
```

**Common Issue:** Item `Sellback Percentage` overrides shop's percentage.

**Hierarchy:**
1. Item's `Sellback Percentage` (if set)
2. Shop's `Sellback Percentage` (fallback)

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemTrade/Public/ShopDataAsset.h` (lines 51)

---

## UI Widgets

### Issue: "Failed to find BindWidget" compilation error

**Symptoms:**
- Error: "Could not find widget named 'InventoryGrid'"

**Solution:**
1. Exact widget names matter! Check required names:
- `InventoryWidgetBase` requires: `InventoryGrid`
   - `InventorySlotWidget` requires: `ItemIcon`
   - `ItemTooltipWidget` requires: `ItemNameText`, `ItemIconImage`
   - `ShopWidgetBase` requires: `ShopItemsList`, `PlayerItemsList`

2. Spelling must be exact (case-sensitive)
3. Widget must exist in Designer hierarchy

**Source:** Widget headers have `UPROPERTY(BlueprintReadOnly, meta = (BindWidget))` for required widgets.

---

### Issue: Tooltip not showing

**Checklist:**
1. ? `Tooltip Widget Class` is set in parent widget
2. ? `ShowItemTooltip` is called on mouse hover
3. ? Tooltip widget is added to viewport
4. ? Tooltip has correct Z-Order (appears on top)

**Common Fix:**
```blueprint
[Slot Widget - On Mouse Enter]
  ?
  Get Parent Widget ? Cast to InventoryWidgetBase
    ?
    Show Item Tooltip
      - Item Data: [ItemData]
      - Screen Position: Event ? Position
```

---

### Issue: Context menu appears behind other widgets

**Solution:**
Set **Z-Order** higher:
```blueprint
Create Widget ? WBP_ContextMenu
  ?
  Add to Viewport (Z-Order: 100)
```

Or use **Add to Player Screen** with higher priority.

---

### Issue: Inventory grid slots not filling correctly

**Symptoms:**
- Items appear in wrong slots or grid is empty

**Checklist:**
1. ? `InventoryGrid` is a **Uniform Grid Panel** (not Canvas or Vertical Box)
2. ? `Grid Columns` is set > 0
3. ? `Slot Widget Class` is assigned
4. ? `RefreshInventorySlots` is called after binding

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemUI/Public/InventoryWidgetBase.h` (lines 163-166)

---

## Multiplayer

### Issue: Inventory not replicating to clients

**Requirements:**
1. ? `InventoryComponent` is on a **replicated actor** (Character, PlayerState, etc.)
2. ? Inventory operations run **on server** (via RPC)
3. ? `GetLifetimeReplicatedProps` is called (automatic if using default component)

**Test:**
```blueprint
[Server]
Add Item ? Should replicate to all clients

[Client]
Get All Items ? Should see items added on server
```

**Source:** `InventoryComponent.cpp` implements `GetLifetimeReplicatedProps()`.

---

### Issue: Client can modify inventory directly

**Symptoms:**
- Exploits possible

**Solution:**
Never call inventory functions directly on client. Always use server RPCs:

```blueprint
? WRONG (Client):
Add Item ? Direct call

? CORRECT (Client ? Server):
Custom Event: ServerAddItem (Run On Server, Reliable)
  ?
  [Server validates]
  Add Item
```

---

### Issue: Inventory state desynced between server and client

**Debug:**
```blueprint
Print String "Server Items: " + GetAllItems ? Length (Server)
Print String "Client Items: " + GetAllItems ? Length (Client)
```

**Common Causes:**
1. Client modified inventory directly (bypassed server)
2. Replication didn't complete (network lag)
3. Order of operations issue (client predicted wrong state)

**Fix:** Always use server-authoritative pattern.

---

## Performance

### Q: How many items can the inventory handle?

**A:** Tested with 1000+ items without performance issues. System is event-driven (no Tick), so overhead is minimal.

**Bottlenecks:**
- UI refresh (rendering 1000 slots)
- Serialization (saving 1000 items to JSON)

**Optimization:** Use paging/scrolling for UI if > 100 slots.

---

### Q: Does the system support item pooling?

**A:** Not built-in, but you can implement:
```blueprint
[Item Pool Manager]
  - Pre-create 100 InventoryItem instances
  - Reuse instead of creating new ones
```

---

### Q: Can I use this with Unreal's Gameplay Ability System (GAS)?

**A:** Yes! Item effects can trigger abilities:
```blueprint
[Custom Item Effect]
  ?
  Get Ability System Component
  ?
  Give Ability (MyGameplayAbility)
  ?
  Try Activate Ability
```

---

## Troubleshooting Checklist

### General Debug Steps

1. **Enable logging:**
   ```cpp
   // In C++
   UE_LOG(LogInventory, Warning, TEXT("Item added: %s"), *ItemData->ItemName.ToString());
   ```

   ```blueprint
   // In Blueprint
   Print String ("Debug: " + Message)
   ```

2. **Check references:**
   ```blueprint
   Branch (ItemData ? IsValid)
     False ? Print "ItemData is NULL!"
   ```

3. **Verify component:**
   ```blueprint
   Get Component By Class (InventoryComponent)
     ?
     Branch (Return Value ? IsValid)
       False ? Print "InventoryComponent not found!"
   ```

4. **Test in isolation:**
   - Create empty test level
   - Add only necessary components
   - Test one feature at a time

---

### Quick Diagnostic Table

| Symptom | Likely Cause | Quick Fix |
|---------|--------------|-----------|
| Items not stacking | `Is Stackable = false` | Check ItemDataAsset settings |
| Events not firing | Not bound or bound after operation | Bind in BeginPlay/Construct |
| UI not updating | Widget not refreshed | Call `RefreshInventorySlots()` |
| Effects not working | Missing `ItemEffectExecutorComponent` | Add component to character |
| Shop purchase fails | Wrong currency or insufficient stock | Check `Currency Item` in ShopData |
| Replication broken | Client calling inventory directly | Use server RPCs |
| Weight always 0 | `Item Weight = 0` or `Max Weight = 0` | Set weights in ItemDataAsset |
| Tooltip not showing | `Tooltip Widget Class` not set | Assign in widget class settings |

---

### Still Having Issues?

**Debugging Steps:**

1. **Check plugin version:**
   - `Plugins/AdvancedInventorySystem/AdvancedInventorySystem.uplugin` ? Version: 1.0.0

2. **Verify file paths:**
 - All source files should be in `Plugins/AdvancedInventorySystem/Source/`

3. **Check engine version:**
 - Plugin requires UE 5.4+

4. **Review logs:**
   - Window ? Developer Tools ? Output Log
   - Filter by "Inventory"

5. **Test with example:**
   - Follow [Tutorial 1](Tutorials.md#tutorial-1) step-by-step
   - If tutorial works, issue is in your implementation

---

**Next:** [Changelog & Limitations](Changelog_Limitations.md)

**© 2024 Game Development Team. All Rights Reserved.**
