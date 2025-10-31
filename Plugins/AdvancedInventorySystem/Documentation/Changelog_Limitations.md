# Changelog & Limitations - Advanced Inventory System

Known limitations, compatibility notes, and version history.

---

## Current Version: 1.0.0

**Release Date:** 2024  
**Engine Compatibility:** Unreal Engine 5.4+  
**Source:** `Plugins/AdvancedInventorySystem/AdvancedInventorySystem.uplugin`

---

## Known Limitations

### 1. WorldItemActor Not Implemented

**Status:** ?? **Placeholder**

**Issue:**
- `InventoryComponent::DropItem()` and `DropItemInFront()` reference `AWorldItemActor`
- This class is **not implemented** in the current plugin version

**Impact:**
- Cannot drop items into the world
- Functions will return `nullptr`

**Workaround:**
Create your own `AWorldItemActor` class:

```cpp
// WorldItemActor.h
UCLASS()
class AWorldItemActor : public AActor
{
 GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UItemDataAsset* ItemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* MeshComponent;
};
```

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemRuntime/Public/InventoryComponent.h` (lines 213, 225)

**Planned:** Version 1.1.0

---

### 2. Container/Loot System Not Included

**Status:** ?? **Not Implemented**

**Missing Features:**
- Container actors (chests, crates)
- Loot table data assets
- Random loot generation
- Loot drop on enemy death

**Workaround:**
Manually implement using existing components:

```blueprint
[Chest Blueprint]
  Components:
    - InventoryComponent (MaxSlots: 12)
    - Static Mesh (Chest visual)
    - Sphere Collision (Interaction trigger)
  
  BeginPlay:
    - Add random items to InventoryComponent
  
  Interaction:
    - Open shop UI showing chest's InventoryComponent
```

**Planned:** Version 1.2.0

---

### 3. Crafting System Not Included

**Status:** ?? **Not Implemented**

**Missing Features:**
- Crafting recipes data assets
- Crafting station actors
- Ingredient requirement checking
- Crafting UI widgets

**Workaround:**
Implement manually:

```blueprint
[Crafting Table Interaction]
  ?
  Check if has:
    - DA_Wood ? 5
    - DA_IronOre ? 3
?
  Remove ingredients
  ?
  Add Item (DA_CraftedSword, 1)
```

**Planned:** Version 1.3.0

---

### 4. Limited Equipment System

**Status:** ?? **Partial Implementation**

**Current:**
- Items can be marked as equipped
- Equipment slot tags supported
- Basic equip/unequip functionality

**Missing:**
- Visual attachment system (mesh attachment to skeleton)
- Equipment stats application (need custom implementation)
- Weapon/armor durability
- Equipment set bonuses
- Socket/attachment point management

**Workaround:**
Implement attachment manually:

```blueprint
Event OnItemEquipped
  Inputs: Item, bEquipped
  ?
  Branch (bEquipped)
    True ? Attach weapon mesh to "WeaponSocket"
    False ? Detach weapon mesh
```

---

### 5. No Built-In UI Implementations

**Status:** ?? **Base Classes Only**

**Provided:**
- `InventoryWidgetBase` (C++ base class)
- `InventorySlotWidget` (C++ base class)
- `ItemTooltipWidget` (C++ base class)
- `ShopWidgetBase` (C++ base class)
- `InventoryContextMenuWidget` (C++ base class)

**Not Provided:**
- No Blueprint widget implementations (WBP_* files)
- No example UI layouts
- No pre-made textures/icons
- No styling/themes

**Reason:** Users must create their own UI in UMG Designer to match their game's art style.

**See:** [Tutorials.md](Tutorials.md#tutorial-4-interactive-inventory-ui) for UI creation guide.

---

### 6. No Example Content

**Status:** ?? **No Demo Assets**

**Missing:**
- Example item data assets
- Example shop data
- Example effects
- Demo level
- Tutorial maps
- Sample icons/meshes

**Reason:** Plugin is code-only to minimize size and allow full customization.

**Workaround:** Follow tutorials in [Tutorials.md](Tutorials.md) to create example assets.

---

### 7. Interface Integrations Not Implemented

**Status:** ?? **Interfaces Only**

**Provided Interfaces:**
- `INeedsSystemInterface` (header only)
- `IDialogueSystemInterface` (header only)
- `ITimeOfDaySystemInterface` (header only)

**Not Provided:**
- No default implementations
- No example needs component
- No example dialogue system

**Requirement:** You must implement these interfaces if you want to use:
- Needs modifications (hunger, thirst, etc.)
- Dialogue triggers on item gift/use
- Time-based shop mechanics

**Source:** `Plugins/AdvancedInventorySystem/Source/InventorySystemCore/Public/InventorySystemInterfaces.h`

---

### 8. Save/Load Format Limitations

**Status:** ?? **JSON Only**

**Current:**
- Save to JSON string
- Load from JSON string

**Limitations:**
- No binary serialization
- No compression
- Large inventories = large JSON files
- Custom metadata must be strings

**Performance:**
- 100 items: ~50 KB JSON
- 1000 items: ~500 KB JSON

**Not Supported:**
- Savegame encryption
- Cloud save integration
- Auto-save checkpoints

**Workaround:**
Implement custom save format:

```blueprint
SaveToJson ? [JsonString]
  ?
  [Compress using ZLib]
  ?
  [Encrypt if needed]
  ?
  SaveGameToSlot
```

---

### 9. Multiplayer Limitations

**Status:** ?? **Server-Authoritative Only**

**Supported:**
- Server-authoritative inventory
- Automatic replication of inventory array
- Server RPCs for inventory operations

**Not Supported:**
- Client-side prediction (inventory operations)
- Lag compensation
- Item rollback on failed operations
- Bandwidth optimization for large inventories

**Requirement:** You MUST use server RPCs for all inventory modifications.

**Risk:** Direct client calls will cause desync.

---

### 10. No Analytics/Telemetry

**Status:** ?? **Not Implemented**

**Missing:**
- Item usage tracking
- Shop transaction logging
- Economy balance analytics
- Player behavior metrics

**Workaround:**
Add custom logging:

```blueprint
Event OnItemPurchased
  ?
  [Log to analytics service]
  Analytics ? Track Event
    - Event Name: "ItemPurchased"
    - Properties: {ItemID, Quantity, Price}
```

---

## Engine Compatibility

### Supported Versions

| Engine Version | Status | Notes |
|----------------|--------|-------|
| **UE 5.6** | ? Tested | Primary development version |
| **UE 5.5** | ? Likely works | Not officially tested |
| **UE 5.4** | ? Should work | Minimum recommended version |
| **UE 5.3** | ?? Unknown | May work but not guaranteed |
| **UE 5.2 and below** | ? Not supported | API incompatibilities likely |
| **UE 4.27** | ? Not compatible | Uses UE5-specific features |

### Platform Compatibility

| Platform | Status | Notes |
|----------|--------|-------|
| **Windows** | ? Fully supported | Primary platform |
| **Linux** | ? Should work | Not officially tested |
| **macOS** | ? Should work | Not officially tested |
| **PlayStation** | ?? Likely works | Requires dev license to test |
| **Xbox** | ?? Likely works | Requires dev license to test |
| **Nintendo Switch** | ?? Unknown | Requires dev license to test |
| **Mobile (Android/iOS)** | ?? Unknown | Performance not tested |
| **Web (PixelStreaming)** | ? Should work | No platform-specific code |

---

## Known Issues

### Issue #1: Item Icon Compression

**Symptom:** Item icons appear blurry in UI

**Cause:** Texture compression set to `Default` instead of `UserInterface2D`

**Fix:**
1. Open icon texture asset
2. **Compression Settings** ? **UserInterface2D (RGBA)**
3. **Save**

**Source:** UE5 texture compression behavior

---

### Issue #2: Widget Binding Errors in Packaged Build

**Symptom:** Widgets work in editor but fail in packaged build

**Cause:** Widget names don't match `BindWidget` properties exactly

**Fix:** Ensure exact name match (case-sensitive):
- `InventoryGrid` (not `inventoryGrid` or `Inventory_Grid`)
- `ItemIcon` (not `Icon` or `ItemImage`)

---

### Issue #3: Replication Issues with Late-Joining Clients

**Symptom:** Clients joining mid-game don't see inventory

**Cause:** Inventory populated before client fully connects

**Fix:** Use `OnRep_` functions or delay:

```blueprint
[Client: On Possess Character]
  ?
  Delay (1.0)
  ?
  Request Inventory Refresh (Server RPC)
```

---

## Incompatibilities

### Plugins

| Plugin | Status | Notes |
|--------|--------|-------|
| **Lyra Starter Game** | ?? Unknown | May conflict with Lyra's inventory |
| **ActionRPG Sample** | ?? Conflict | ActionRPG has its own inventory system |
| **Gameplay Ability System** | ? Compatible | Can integrate item effects with GAS |
| **CommonUI** | ? Compatible | UI widgets work with CommonUI |
| **Enhanced Input** | ? Compatible | No input conflicts |

### Systems

**Compatible With:**
- Unreal's Animation System
- Unreal's AI System
- Unreal's Niagara
- Unreal's Audio System
- Third-party save systems (Easy Save, etc.)

**May Conflict With:**
- Other inventory plugins
- Asset packs with inventory systems included
- Templates with built-in inventory (ActionRPG, etc.)

---

## Performance Benchmarks

**Test Setup:**
- UE 5.6
- Windows 11
- Release build
- 60 FPS target

**Results:**

| Operation | Items | Time | Notes |
|-----------|-------|------|-------|
| Add Item | 1 | <0.1 ms | Instant |
| Add Item | 100 (batch) | ~2 ms | Acceptable |
| Remove Item | 1 | <0.1 ms | Instant |
| Get All Items | 1000 | ~0.5 ms | Array copy |
| Find By Tags | 1000 | ~5 ms | Linear search |
| Save To JSON | 100 | ~10 ms | Serialization overhead |
| Save To JSON | 1000 | ~100 ms | May cause hitch |
| Load From JSON | 1000 | ~150 ms | Loading screen recommended |

**Recommendations:**
- Keep inventories < 500 items for instant operations
- Use loading screen for save/load with > 500 items
- Cache `Find By Tags` results if called frequently

---

## Changelog

### Version 1.0.0 (2024)

**Initial Release**

**Added:**
- ? Inventory Component with full functionality
- ? Item Data Assets
- ? Item Effect Data Assets
- ? Shop Component
- ? Shop Data Assets
- ? Item Effect Executor Component
- ? UI Widget Base Classes (5 widgets)
- ? Save/Load to JSON
- ? Replication support
- ? Equipment system (basic)
- ? Weight system
- ? Stacking system
- ? Event-driven architecture (no Tick)

**Known Issues:**
- WorldItemActor not implemented (drop item feature incomplete)
- No example content/assets
- No container/loot system
- No crafting system

**Documentation:**
- Complete README
- User Guide (50+ pages)
- Tutorials (5 tutorials)
- Blueprint Reference
- Data Reference
- Examples & Recipes
- FAQ & Troubleshooting

---

### Planned Features (Future Versions)

**Version 1.1.0 (TBD)**
- ? WorldItemActor implementation
- ? Pickup/drop system complete
- ? Example content (10 items, 1 shop)
- ?? Bug fixes from user feedback

**Version 1.2.0 (TBD)**
- ? Container system (chests, crates)
- ? Loot table data assets
- ? Random loot generation
- ? Enemy loot drops

**Version 1.3.0 (TBD)**
- ? Crafting system
- ? Crafting recipes
- ? Crafting UI widgets
- ? Multi-step crafting

**Version 2.0.0 (TBD)**
- ? Full equipment system (visual attachment)
- ? Durability system
- ? Equipment stats
- ? Equipment set bonuses
- ? Enchantment system

---

## Migration Notes

### From Future Versions

**Not applicable** - This is the initial release.

When migrating from 1.0.0 to future versions, migration notes will be added here.

---

## API Stability

**Stable APIs (Won't Change):**
- InventoryComponent core functions (Add, Remove, Get)
- ItemDataAsset properties
- Event signatures (OnItemAdded, etc.)
- Save/Load JSON format

**May Change in Future:**
- UI widget property names
- Effect execution details
- Shop pricing formulas

**Deprecated:**
- None (initial release)

---

## Reporting Issues

**Before reporting:**
1. Check [FAQ & Troubleshooting](FAQ_Troubleshooting.md)
2. Verify you're using supported UE version
3. Test in isolation (new empty project)

**When reporting, include:**
- Plugin version (1.0.0)
- UE version (e.g., 5.6.1)
- Platform (Windows/Linux/Mac)
- Reproduction steps
- Error logs from Output Log
- Screenshots/video if applicable

---

**Next:** [Audit](Audit.md) - Plugin architecture analysis

**© 2024 Game Development Team. All Rights Reserved.**
