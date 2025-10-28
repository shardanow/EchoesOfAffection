# Widget Usage Quick Reference

**How to use Inventory and Shop widgets in your game**

---

## ?? Inventory Widget Setup

### Step 1: Create Widget Once (Player Controller BeginPlay)

```blueprint
Event BeginPlay
  ?
  ?? Create Widget
  ?   Class: WBP_Inventory
  ?   Owning Player: Self
  ?   Output: InventoryWidget
  ?   ?
  ?   ?? Get Pawn
  ?   ?   ?
  ?   ?   ?? Get Component By Class
  ?   ?       Class: InventoryComponent
  ?   ?       Output: PlayerInventory
  ?   ?       ?
  ?   ?    ?? InventoryWidget ? Bind To Inventory  ?? CRITICAL!
  ?   ?           Input: PlayerInventory
  ?   ?
  ?   ?? Set InventoryWidget (save variable)
  ?       (Don't add to viewport yet!)
```

### Step 2: Toggle Visibility (Input Action)

```blueprint
Input Action: ToggleInventory (E key or Tab)
  ?
  ?? Branch (InventoryWidget ? Is in Viewport?)
  ?
      ?? True: (Already open - close it)
      ?   ?
      ? ?? InventoryWidget ? Remove from Parent
      ?   ?
      ?   ?? Set Input Mode Game Only
      ?       Show Mouse Cursor: false
      ?
  ?? False: (Closed - open it)
          ?
          ?? InventoryWidget ? Add to Viewport
          ? Z-Order: 10
          ?
          ?? Set Input Mode UI Only
      Show Mouse Cursor: true
```

**That's it!** Widget auto-updates when inventory changes.

---

## ?? Shop Widget Setup

### Key Difference: Shop is CREATED when opened, DESTROYED when closed

### Step 1: Open Shop (Called from NPC or Shop Trigger)

```blueprint
Function: OpenShop
  Input: ShopComponent (from NPC/Shop Actor)
  ?
  ?? Create Widget
  ?   Class: WBP_Shop
  ?   Owning Player: Get Player Controller
  ?   Output: ShopWidget
  ?   ?
  ?   ?? Get Player Controller
  ?   ?   ?
  ?   ?   ?? Get Pawn
  ?   ?       ?
  ? ?       ?? Get Component By Class
  ?   ?           Class: InventoryComponent
  ?   ?         Output: PlayerInventory
  ?   ?           ?
  ?   ?           ?? ShopWidget ? Bind To Shop  ?? CRITICAL!
  ?   ?         Input: ShopComponent (from NPC)
  ?   ?      Input: PlayerInventory
  ?   ?
  ?   ?? Add to Viewport
  ?   ?   Z-Order: 20 (higher than inventory)
  ?   ?
  ?   ?? Set Input Mode UI Only
  ?   ?   Show Mouse Cursor: true
  ?   ?
  ?   ?? Set CurrentShopWidget (save variable)
```

### Step 2: Close Shop

**Option A: Auto-Close (if CloseButton exists)**
```
CloseButton in WBP_Shop auto-calls Remove from Parent
System handles cleanup automatically
```

**Option B: Manual Close**
```blueprint
Function: CloseShop
  ?
  ?? Branch (CurrentShopWidget is valid?)
      ?
 ?? True:
      ?
          ?? CurrentShopWidget ? Remove from Parent
    ?
    ?? Set CurrentShopWidget = null
  ?
      ?? Set Input Mode Game Only
  Show Mouse Cursor: false
```

---

## ?? Complete Example: Player Controller

### Variables:

```
InventoryWidget (WBP_Inventory) - Instance Editable: false
CurrentShopWidget (WBP_Shop) - Instance Editable: false
PlayerInventoryComponent (InventoryComponent) - cached reference
```

### Event BeginPlay:

```blueprint
Event BeginPlay
  ?
  ?? Get Pawn
  ?   ?
  ?   ?? Get Component By Class
  ?       Class: InventoryComponent
  ?       ?
  ?       ?? Set PlayerInventoryComponent (cache it)
  ?
  ?? Create Widget
      Class: WBP_Inventory
  ?
      ?? Bind To Inventory
      ?   Input: PlayerInventoryComponent
      ?
      ?? Set InventoryWidget (save)
```

### Input: Toggle Inventory

```blueprint
Input Action: ToggleInventory
  ?
  ?? Branch (InventoryWidget in viewport?)
      ?? True: Close Inventory (remove from parent, game input)
      ?? False: Open Inventory (add to viewport, UI input)
```

### Function: Open Shop

```blueprint
Function: OpenShop
  Input: ShopActor (Actor)
  ?
  ?? Get Component from Actor
  ?   Actor: ShopActor
  ?   Class: ShopComponent
  ?   Output: ShopComp
  ?   ?
  ?   ?? Branch (ShopComp valid?)
  ?       ?
  ?     ?? True:
  ?           ?
  ?      ?? Close Inventory First (if open)
?    ?
  ?           ?? Create Widget (WBP_Shop)
  ?        ?   ?
?           ?   ?? Bind To Shop
  ?       ?   ?   Input: ShopComp
  ?   ?   ?   Input: PlayerInventoryComponent
  ?           ?   ?
  ?           ?   ?? Add to Viewport
  ?           ?   ?
  ?           ?   ?? Set CurrentShopWidget
  ?         ?
  ?      ?? Set Input Mode UI Only
```

### Function: Close Shop

```blueprint
Function: CloseShop
  ?
  ?? Branch (CurrentShopWidget valid?)
      ?
      ?? True:
          ?? Remove from Parent
          ?? Set = null
          ?? Set Input Mode Game Only
```

---

## ?? Lifecycle Comparison

### Inventory Widget:

```
BeginPlay: Create ? Bind ? Store
Game: Toggle visibility (Add/Remove from viewport)
EndPlay: Destroyed automatically
```

**Lifetime:** Entire game session  
**Memory:** Always in memory (one instance)  
**Updates:** Automatic when inventory changes

### Shop Widget:

```
Open Shop: Create ? Bind ? Add to viewport
Use Shop: Buy/Sell items
Close Shop: Remove from viewport ? Destroy
```

**Lifetime:** Only while shop is open  
**Memory:** Created/destroyed each time  
**Updates:** Automatic while open

---

## ?? Common Mistakes

### ? DON'T:

1. **Forget to Bind:**
   ```blueprint
   Create Widget ? Add to Viewport
   // ? Widget won't update!
   ```
   **Fix:** Always call `Bind To Inventory` or `Bind To Shop`

2. **Create Inventory Every Time:**
   ```blueprint
   Toggle Inventory:
     Create Widget ? ... 
   // ? Memory leak! Creates new widget each time
   ```
   **Fix:** Create once in BeginPlay, reuse

3. **Keep Shop Widget Alive:**
   ```blueprint
   Create Shop Widget in BeginPlay
   // ? Wastes memory, shop not always needed
   ```
   **Fix:** Create when opening shop, destroy when closing

4. **Wrong Input Mode:**
```blueprint
   Open Inventory without setting Input Mode UI
   // ? Can't click buttons!
   ```
   **Fix:** Always set Input Mode UI Only when showing widget

### ? DO:

1. **Cache Components:**
   ```blueprint
   BeginPlay: Get InventoryComponent ? Save as variable
   Later: Use cached variable (faster!)
   ```

2. **Proper Cleanup:**
   ```blueprint
   Close Shop: Remove from viewport
   // Widget auto-destroyed if no references
   ```

3. **Check Validity:**
 ```blueprint
   Before opening shop:
     Branch (ShopComponent valid?)
 ```

---

## ?? Quick Cheat Sheet

### Inventory:

| Action | When | How |
|--------|------|-----|
| **Create** | BeginPlay (once) | `CreateWidget` ? `BindToInventory` |
| **Open** | Input action | `AddToViewport` + UI input mode |
| **Close** | Input action | `RemoveFromParent` + Game input |
| **Auto-Update** | Always | Automatic after `BindToInventory` |

### Shop:

| Action | When | How |
|--------|------|-----|
| **Create** | NPC interaction | `CreateWidget` ? `BindToShop` |
| **Open** | Same time as create | `AddToViewport` + UI input mode |
| **Close** | Button or ESC | `RemoveFromParent` + Game input |
| **Auto-Update** | While open | Automatic after `BindToShop` |

---

## ?? Advanced: Both Open at Once

### Can you have Inventory + Shop open simultaneously?

**Yes!** For "sell to shop" functionality:

```blueprint
Open Shop:
  ?
  ?? Create/Open Shop Widget (Z-order: 20)
  ?
  ?? Open Inventory Widget (Z-order: 10)
  // Inventory shows below shop
```

**Layout:**
```
Screen:
  ?? Shop Widget (Right side, Z:20)
  ?? Inventory Widget (Left side, Z:10)
```

**To sell item:**
1. Click item in Inventory
2. Drag to Shop (or click Sell button)
3. Shop's `SellItem()` called automatically

---

## ?? Example: NPC Shop

### NPC Actor:

**Components:**
- `ShopComponent` - configured with shop data

**Blueprint:**
```blueprint
Event BeginInteract (from IInteractableInterface)
  Input: Interacting Actor (Player)
  ?
  ?? Get Player Controller
      ?
      ?? Call: OpenShop
     Input: Self (NPC has ShopComponent)
```

### Player Controller:

**Function: OpenShop** (defined above)

**Result:** Player interacts with NPC ? Shop opens!

---

## ?? UI/UX Best Practices

### Input Handling:

```blueprint
// Close on ESC key
Input Action: Cancel/Escape
  ?
  ?? Branch (CurrentShopWidget valid?)
  ?   ?? True: CloseShop
  ?
  ?? Branch (InventoryWidget in viewport?)
      ?? True: Close Inventory
```

### Focus Management:

```blueprint
Open Inventory:
  ?
  ?? Add to Viewport
  ?
  ?? Set UI Focus to InventoryWidget
      // Auto-select first item slot
```

### Prevent Double-Open:

```blueprint
Toggle Inventory:
  ?
  ?? Branch (CurrentShopWidget valid?)
  ?   ?? True: Return (don't open inventory while shop open)
  ?
  ?? Continue (toggle inventory)
```

---

## ? Summary

### Inventory Widget:
- ? Create **once** in BeginPlay
- ? **Bind** to InventoryComponent
- ? **Toggle** visibility with input
- ? **Reuse** same instance
- ? **Auto-updates** when inventory changes

### Shop Widget:
- ? Create **when needed** (NPC interaction)
- ? **Bind** to ShopComponent + PlayerInventory
- ? **Destroy** when closed
- ? **Don't keep alive** when not in use
- ? **Auto-updates** while open

### Key Difference:
**Inventory** = Create once, toggle visibility  
**Shop** = Create on-demand, destroy after use

---

**Time to implement:** ~20 minutes  
**Complexity:** Medium (multiple bind calls)  
**Result:** Professional inventory + shop system! ??

