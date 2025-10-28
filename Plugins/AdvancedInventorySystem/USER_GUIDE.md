# Advanced Inventory System - User Guide

**����������� ��� Game Designers**  
������: 1.0  
��� Unreal Engine 5.6+

---

## ?? ����������

1. [��������](#��������)
2. [�������� ��������� (Items)](#��������-���������-items)
3. [�������� �������� ���������](#��������-��������-���������)
4. [���������� ��������� �� ������](#����������-���������-��-������)
5. [��������� �������������� � ����������](#���������-��������������-�-����������)
6. [������ � ����������](#������-�-����������)
7. [������������� ���������](#�������������-���������)
8. [�������� ���������](#��������-���������)
9. [UI � ������������](#ui-�-������������)
10. [������ ��������](#������-��������)

---

## ��������

**Advanced Inventory System** - ��� ������������������� ������� ��������� ��� Unreal Engine, ������� ��������� ��������� ��������, �������, �������� � ��������� ���� ��������� � ��������� ��� ��������� ����.

### �������� �����������

- ? **Data-driven ������** - �� ������������� ����� Data Assets
- ? **������� ���������** - ��������, ��������������, ����
- ? **������� ��������������** - ������ ��������� � ������
- ? **��������** - ��������, �������/�������
- ? **������ ����������** - �������� � �������� Interaction

---

## �������� ��������� (Items)

### ��� 1: �������� Item Data Asset

1. **�������� Content Browser**
2. ��������� � ����� `Content/Items/` (��� �������� �)
3. **���** ? **Miscellaneous** ? **Data Asset**
4. �������� ����� **`ItemDataAsset`**
5. �������� ����, ��������: `DA_Potion_Health`

![�������� Data Asset](https://via.placeholder.com/600x200/4CAF50/FFFFFF?text=Right+Click+?+Data+Asset)

### ��� 2: ��������� ������� �������

�������� ��������� Data Asset � ��������� ����:

#### ?? Identity (�������������)

| �������� | �������� | ������ |
|----------|----------|---------|
| **Item ID** | ���������� ������������� | `HealthPotion` |
| **Item Name** | ������������ ��� | `Health Potion` |
| **Item Description** | �������� �������� | `Restores 50 HP` |
| **Icon** | ������ ��� UI | �������� �������� |
| **Mesh** | 3D ������ | �������� Static Mesh |

#### ?? Classification (�������������)

| �������� | �������� | �������� |
|----------|----------|----------|
| **Category** | `Consumable` | ����������� ������� |
| **Rarity** | `Common` / `Rare` / `Legendary` | �������� |
| **Item Tags** | `Item.Consumable.Potion` | ���� ��� ���������� |

#### ?? Stacking (������������)

```
? Is Stackable: true
 Max Stack Size: 99
```

#### ?? Usage (�������������)

```
? Is Usable: true
? Consume On Use: true
   Use Cooldown: 0.0
```

### ��� 3: ��������� ���������

#### ?? Economy

```
Base Value: 50         // ������� ����
? Can Be Sold: true
? Can Be Bought: true
? Can Be Gifted: true
Sellback Percentage: 0.5  // 50% ��� �������
```

### ������ �������� ��������

**DA_Potion_Health** (����� ��������):
- Item ID: `HealthPotion`
- Name: `Health Potion`
- Description: `Restores 50 HP instantly`
- Category: `Consumable`
- Rarity: `Common`
- Stackable: ? (99)
- Base Value: 50 ������
- Icon: T_Icon_Potion_Red

---

## �������� �������� ���������

������� ����������, ��� ���������� ��� ������������� ��������.

### ��� 1: �������� Effect Data Asset

1. **Content Browser** ? **���** ? **Data Asset**
2. �������� **`ItemEffectDataAsset`**
3. ��������: `EFF_Heal_50HP`

### ��� 2: ��������� ���� �������

#### ?? Effect Type (��� �������)

��������� ����:

| ��� | �������� | ����� ������������ |
|-----|----------|-------------------|
| **Modify Stat** | �������� �������������� | ��������, ����, ������������ |
| **Add Tag** | ��������� ��� | �����, �������, ��������� |
| **Remove Tag** | ������� ��� | ������ �������� |
| **Trigger Event** | ��������� ������� | ��������� ������ |

### ��� 3: ��������� Modify Stat �������

��� ����� ��������:

```
Effect Type: Modify Stat
Stat Name: "Health"
Modification Type: Add
Modification Value: 50.0
```

#### ������� ������ ��������:

**����� ����:**
```
Effect Type: Modify Stat
Stat Name: "Mana"
Modification Type: Add
Modification Value: 30.0
```

**��� ����:**
```
Effect Type: Add Tag
Tag To Add: "Buff.Strength"
Duration: 60.0  // 1 ������
```

**������� (������ ���):**
```
Effect Type: Remove Tag
Tag To Remove: "Debuff.Poison"
```

### ��� 4: �������� ������� � ��������

1. �������� **Item Data Asset** (DA_Potion_Health)
2. ������� ������ **Effects**
3. �������� ������� � ������ **Item Effects**
4. �������� ��������� **EFF_Heal_50HP**
5. ���������� **Trigger Type**: `On Use`

#### �������� ��������:

| Trigger | ����� ����������� |
|---------|-------------------|
| **On Use** | ��� ������������� �������� |
| **On Equip** | ��� ���������� |
| **On Unequip** | ��� ������ |
| **On Pickup** | ��� ������� |
| **On Drop** | ��� ������������ |

---

## ���������� ��������� �� ������

### ����� 1: World Item Actor

#### ��� 1: ���������� ������

1. �������� ��� �������
2. **Place Actors** ? �����: `WorldItemActor`
3. ���������� �� �������
4. ��� **Modes** ? **All Classes** ? `WorldItemActor`

#### ��� 2: ��������� ��������

�������� ����������� ����� � � **Details**:

```
Item Data: DA_Potion_Health     // ��� Item Data Asset
Initial Quantity: 1 // ����������
Instance Tags: (�����������)  // ���������� ����
```

#### ��� 3: ��������� ������������

**�����! ���� 2 ������� ������ ��� ��������:**
##### ������ A: ����� Item Data Asset (�������������)

1. �������� **Item Data Asset** (DA_Potion_Health)
2. ������� ������ **Physical Representation**
3. ����������:

```
Physical Representation:
  ?? Static Mesh: SM_Potion_Bottle
  ?? Location Offset: (0, 0, 0)
  ?? Rotation Offset: (0, 0, 0)
  ?? Scale: (1, 1, 1)
  ? Can Be Picked Up: true
```

��� ���� ������� ��� **�������������** ���������� �� ���� WorldItemActor � ���� Item Data.

##### ������ B: ������� �� ������

���� � Item Data Asset **�� ������ ���**, ����� ������ ��� �������� �� ������:

1. �������� WorldItemActor �� ������
2. � Details ������� **Static Mesh Component**
3. ���������� **Static Mesh**: `SM_Potion_Bottle`

**?? ��������:**
- ���� � Item Data Asset **���� ���** ? �� ������������� ������ ���������
- ���� � Item Data Asset **��� ����** ? ���������� ���, ������������� �������

#### ��������� collision:

```
Static Mesh Component:
  ?? Collision Enabled: Query Only
  ?? Object Type: WorldDynamic
  ?? Collision Presets: BlockAllDynamic
```

### ����� 2: Blueprint spawn

��� ������������� ������ �������� Blueprint:

1. �������� **Actor Blueprint**: `BP_ItemSpawner`
2. �������� ���� **Spawn Actor from Class**
3. Class: `WorldItemActor`
4. ����� ������ �������� `Initialize Item`:

```
       ?????????????????????
Begin Play ???????? Spawn Actor    ?
         ? Class: WorldItem  ?
    ?????????????????????
              ?
   ?
                  ?????????????????????
      ? Initialize Item   ?
                  ? Item Data: DA_... ?
         ? Quantity: 1       ?
        ?????????????????????
```

---

## ��������� �������������� � ����������

### ���������� � Interaction System

��� ������ ��� ����� ������� ��������������. �������� ������ ���������.

#### ��� 1: ��������� WorldItemActor

`WorldItemActor` ��� ��������� **`IInteractableInterface`**.

� ���������� ������:

```
Interactable Component:
  ? Can Be Interacted With: true
  Interaction Prompt: "Press E to pick up {ItemName}"
  Interaction Time: 0.0  // ���������� ������
```

#### ��� 2: ��������� ���������

���������, ��� � ������ ��������� ����:

1. **Interaction Component** (��� ���� � �������)
2. **Inventory Component**

���������� Inventory Component:

1. �������� Blueprint ��������� (BP_PlayerCharacter)
2. **Add Component** ? `InventoryComponent`
3. ���������:

```
Max Inventory Slots: 20
Max Weight Capacity: 0.0  // ��� ����������� ����
? Auto Stack: true
? Auto Sort: false
```

#### ��� 3: Blueprint ������ �������

� **WorldItemActor** ��� ����������� ������� `OnInteract`:

```blueprint
Event OnInteract
  ?
  ?? Get Interacting Actor
  ?    ?
  ?    ?? Get Component By Class
  ?       ?? Class: InventoryComponent
  ?       ?
  ?     ?? Add Item
  ?               ?? Item Data: [ItemData]
  ?        ?? Quantity: [Quantity]
  ?                   ?? Out Result: [Result]
  ?          ?
  ?        ?? Branch (Success?)
  ?      ?? True: Destroy Actor
  ?      ?? False: Show Error Message
```

**��� ��� �������� �� �������!** ������ ���������� WorldItemActor �� ������.

### ������������ �������

���� ����� �������� ������:

1. �������� **Blueprint** ��������� `WorldItemActor`: `BP_WorldItem_Custom`
2. Override ������� `OnInteract`
3. �������� ��������� ������:

```blueprint
OnInteract
  ?
  ?? Play Sound 2D (Pickup Sound)
  ?
  ?? Spawn Emitter at Location (Particle Effect)
  ?
  ?? Parent: OnInteract  // ����� ������� ������
  ?
  ?? Show Notification ("Picked up {ItemName}")
```

---

## ������ � ����������

### ���������� ���������

#### ����� Blueprint:

```blueprint
??????????????????
? Get Component  ?
? Inventory Comp ?
??????????????????
        ?
        ?
??????????????????
?   Add Item     ?
? Item Data: ... ?
? Quantity: 1    ?
??????????????????
        ?
        ?
??????????????????
?    Branch      ?
?  (Success?)    ?
??????????????????
```

#### ������: ��������� �������

```blueprint
Event OnQuestComplete
  ?
  ?? Add Item
       ?? Item Data: DA_Reward_Gold
       ?? Quantity: 100
       ?? Result ? Print String ("Received 100 gold!")
```

### �������� ���������

```blueprint
??????????????????
? Remove Item    ?
? By Data        ?
? Item: DA_...   ?
? Quantity: 1    ?
??????????????????
```

### �������� �������

```blueprint
??????????????????
?   Has Item?    ?
? Item: DA_Key   ?
? Quantity: 1    ?
??????????????????
        ?
      ?
??????????????????
?  Branch      ?
? Can open door? ?
??????????????????
```

### ��������� ���� ���������

```blueprint
??????????????????
? Get All Items  ?
??????????????????
   ?
     ?
??????????????????
?   For Each     ?
? Show in UI     ?
??????????????????
```

---

## ������������� ���������

### ������ 1: �� Blueprint

```blueprint
Event Use Potion Hotkey
  ?
  ?? Find Item by Data (DA_Potion_Health)
  ?    ?
  ?    ?? Use Item
  ?     ?? Item ID: [��������� ID]
  ?         ?? User: Self
  ?         ?? Result
  ?   ?
  ?      ?? Branch (Success?)
  ?       ?? True: Play Animation
```

### ������ 2: �� UI

1. �������� **Widget Blueprint**: `WBP_InventorySlot`
2. �������� **Button**
3. � `OnClicked`:

```blueprint
OnClicked
  ?
  ?? Get Owning Player
   ?
       ?? Get Component (Inventory)
     ?
      ?? Use Item
     ?? Item ID: [SlotItemID]
             ?? User: PlayerCharacter
```

### ������� ��� �������������

����� ����� ���������� �������:

1. **Inventory Component** �������� `Use Item`
2. ����������� `Can Be Used`
3. ����������� **�������** � ��������� `On Use`
4. ���� `Consume On Use = true`, ������� ���������
5. ����������� ������� `OnItemUsed`

### �������� �� ������� �������������

� **���������**:

```blueprint
Event BeginPlay
  ?
  ?? Bind Event to OnItemUsed
       ?
       ?? Event
 ?? Item: [�������������� �������]
            ?? User: [��� �����������]
  ?? Success: [�������?]
          ?
             ?? Branch
      ?? True:  Play Sound + Animation
       ?? False: Show Error Message
```

---

## ������������ � ������ ��������

### Drop Item (���������)

```blueprint
Drop Item
  ?? Item ID: [ID ��������]
  ?? Quantity: 1
  ?? Drop Location: [������� ��������� + forward]
  ?? Out Result
       ?
       ?? Returns: WorldItemActor (spawned)
```

**�������������� �������** `Drop Item In Front`:
- ����������� ������� ������� ���������
- ������ ����� ���� �� �����
- ������� WorldItemActor
- ������� ��� ������ ��������

### Gift Item (��������)

```blueprint
Gift Item
  ?? Item ID: [ID]
  ?? Receiver: [������ �����]
  ?? Quantity: 1
  ?? Result
```

��������:
- ? `Can Be Gifted = true`
- ? � ���������� ���� Inventory Component
- ? � ���������� ���� �����

### Swap Items (�������� �������)

```blueprint
Swap Items
  ?? Slot Index A: 0
?? Slot Index B: 5
```

������������ ��� UI drag-and-drop.

### Sort Inventory

```blueprint
Sort Inventory
```

��������� ������������� ��:
1. ���������
2. �������� (���� ? ������)
3. ����� (�������)

---

## �������� ���������

### ��� 1: �������� Shop Data Asset

1. **Content Browser** ? **Data Asset**
2. �����: **`ShopDataAsset`**
3. ��������: `DA_Shop_GeneralStore`

### ��� 2: ��������� ��������

```
Shop ID: "GeneralStore"
Shop Name: "General Store"
Shop Description: "Buy and sell items"
Shop Keeper Name: "Merchant Tom"

Shop Inventory (������ ���������):
  [0] Stock Item:
      ?? Item Data: DA_Potion_Health
      ?? Stock Quantity: 10
    ?? Price: 50
      ?? Unlimited Stock: false
   ?? Restock Time: 3600.0  // 1 ���
   
  [1] Stock Item:
      ?? Item Data: DA_Potion_Mana
      ?? Stock Quantity: 5
      ?? Price: 75
      ?? Unlimited Stock: false
      
  [2] Stock Item:
      ?? Item Data: DA_Bread
      ?? Price: 10
      ?? Unlimited Stock: true  // ����������� �����
```

### ��������� ���:

```
Buy Price Multiplier: 1.0    // ��������� ���� �������
Sell Price Multiplier: 0.5   // ����� ������ �� 50%
? Can Buy From Player: true
? Can Sell To Player: true
```

### ��� 3: �������� Shop Component

1. �������� Blueprint NPC ��������: `BP_Merchant`
2. **Add Component** ? `ShopComponent`
3. ���������:

```
Shop Data: DA_Shop_GeneralStore
? Auto Initialize: true
```

### ��� 4: ��������� ��������������

� **BP_Merchant**:

```blueprint
Event OnInteract (from Interactable)
  ?
  ?? Open Shop UI
       ?? Shop Component: [Self.ShopComponent]
       ?? Player: [Interacting Actor]
```

---

## ������� � �������

### ������� ��������

```blueprint
Buy Item
  ?? Shop Component: [Merchant.ShopComponent]
  ?? Item Data: DA_Potion_Health
  ?? Quantity: 2
  ?? Buyer: PlayerCharacter
  ?? Out Result
       ?
       ?? Branch (Success?)
            ?? True:  Play Sound + Update UI
            ?? False: Show "Not enough gold"
```

**�������������� ��������:**
- ? ���������� �� ����� � ������?
- ? ���� �� ����� � �������?
- ? ���� �� ����� � ���������?

### ������� ��������

```blueprint
Sell Item
  ?? Shop Component: [Shop]
  ?? Item ID: [ID �� ��������� ������]
  ?? Quantity: 1
  ?? Seller: PlayerCharacter
  ?? Out Result
```

**�������������:**
- ? ������� �� ��������� ������
- ? ��������� ������ ������
- ? ��������� ����� � ������� (���� �������)

### ��������� ����

```blueprint
Get Buy Price
?? Item Data: DA_Sword
  ?? Returns: int32 (���� �������)

Get Sell Price
  ?? Item Data: DA_Sword
  ?? Returns: int32 (���� �������)
```

---

## UI � ������������

### �������� �������� Inventory UI

#### ��� 1: Widget Blueprint

1. �������� **Widget**: `WBP_Inventory`
2. �������� **Uniform Grid Panel**
3. � **Graph**:

```blueprint
Event Construct
  ?
  ?? Get Owning Player
       ?
       ?? Get Component (Inventory)
         ?
   ?? Get All Items
        ?
     ?? For Each Loop
              ?
  ?? Create Widget (WBP_InventorySlot)
             ?
           ?? Add to Grid Panel
```

#### ��� 2: Slot Widget

�������� `WBP_InventorySlot`:

**����������:**
- **Image** (������ ��������)
- **Text** (��������)
- **Text** (����������)
- **Button** (���� ��� �������������)

**����������:**
- `Item Data` (ItemDataAsset)
- `Item ID` (FInventoryItemId)
- `Quantity` (int32)

**������:**

```blueprint
Event Pre Construct
  ?
  ?? Set Icon (Item Data ? Icon)
  ?? Set Text (Item Data ? Item Name)
?? Set Quantity Text (Quantity)
  
OnClicked (Button)
  ?
  ?? Use Item (Item ID)
```

### �������� Shop UI

`WBP_Shop`:

**Layout:**
```
???????????????????????????????????????
?  Shop Name: General Store       ?
???????????????????????????????????????
?  Shop Items  ?  Player Inventory ?
??          ?
?  [Item 1]    ?  [Item A]            ?
?  [Item 2]    ?  [Item B]            ?
?  ...         ?  ...           ?
?       ?           ?
?  [Buy] [Sell]?  Gold: 1000         ?
???????????????????????????????????????
```

**������ Buy Button:**

```blueprint
OnClicked (Buy)
  ?
  ?? Buy Item
       ?? Shop Component
       ?? Selected Item
       ?? Player
        ?
      ?? Refresh UI
```

---

## ������ ��������

### ?? �������� 1: ��������� �������

**����:** ������� ����, ������� ������ �������/���������.

1. �������� `DA_Quest_Key`
2. ���������:

```
Item ID: "QuestKey"
Item Name: "Old Key"
? Is Quest Item: true
Category: QuestItem
? Can Be Sold: false
? Can Be Dropped: false
? Can Be Gifted: false
```

3. ����������� � Blueprint �����:

```blueprint
Event OnInteract
  ?
  ?? Has Item?
       ?? Item: DA_Quest_Key
       ?? Branch
            ?? True:  Open Door + Remove Item
    ?? False: Show "Door is locked"
```

---

### ?? �������� 2: ��� � �������� �������

**����:** ���� ��������������� ����� � ��� ��������� ����.

1. **�������� ������ �������:**

`EFF_Food_Satiety`:
```
Effect Type: Modify Stat
Stat Name: "Hunger"
Modification Type: Add
Modification Value: 30.0
```

2. **�������� ����:**

`EFF_Buff_WellFed`:
```
Effect Type: Add Tag
Tag To Add: "Buff.WellFed"
Duration: 300.0  // 5 �����
```

3. **�������� �������:**

`DA_Food_Bread`:
```
Item Name: "Bread"
Category: Consumable
? Is Usable: true
? Consume On Use: true

Item Effects:
  [0] EFF_Food_Satiety (Trigger: On Use)
  [1] EFF_Buff_WellFed (Trigger: On Use)
```

---

### ?? �������� 3: ������ � �����������

**����:** ���, ������� ��� +10 � ����� ��� ����������.

1. **�������� ������:**

`EFF_Sword_AttackBonus`:
```
Effect Type: Modify Stat
Stat Name: "Attack"
Modification Type: Add
Modification Value: 10.0
```

2. **�������� �������:**

`DA_Weapon_Sword`:
```
Item Name: "Iron Sword"
Category: Weapon
? Is Equippable: true
Equipment Slot: "Equipment.Slot.MainHand"

Item Effects:
  [0] EFF_Sword_AttackBonus
Trigger Type: On Equip
```

3. **� Blueprint ���������:**

```blueprint
OnItemEquipped
  ?
  ?? Update Weapon Mesh
  ?? Play Equip Sound
  
OnItemUnequipped
  ?
  ?? Hide Weapon Mesh
```

---

### ?? �������� 4: ������� � ������� ������

**����:** � ������ �������, � ������� ������.

�������� 2 ��������:

`DA_Shop_City`:
```
Shop ID: "CityShop"
Buy Price Multiplier: 1.0
Sell Price Multiplier: 0.6  // ����� �������
```

`DA_Shop_Village`:
```
Shop ID: "VillageShop"
Buy Price Multiplier: 1.2   // ������ �������
Sell Price Multiplier: 0.4  // ���� �������
```

---

### ?? �������� 5: ������������ ������

**����:** ������ ����� ���������� ��� � ����.

`DA_Shop_Rare`:
```
Stock Items:
  [0] Rare Sword
      ?? Stock Quantity: 1
      ?? Price: 1000
      ?? ? Unlimited Stock: false
   ?? Restock Time: 86400.0  // 24 ����
      ?? Restock Amount: 1
```

����� ������� ����� �������� � �������� ����� 24 ���� �������� �������.

---

## ?? ������ � Best Practices

### ? ����������� Content

������������� ��������� �����:

```
Content/
??? Items/
?   ??? Consumables/
?   ?   ??? DA_Potion_Health
?   ?   ??? DA_Food_Bread
?   ?   ??? ...
?   ??? Weapons/
?   ?   ??? DA_Sword_Iron
?   ?   ??? ...
?   ??? Armor/
?   ??? Quest/
?
??? Effects/
?   ??? Stats/
?   ?   ??? EFF_Heal_50HP
?   ?   ??? ...
?   ??? Buffs/
?
??? Shops/
?   ??? DA_Shop_General
?   ??? DA_Shop_Blacksmith
?   ??? ...
?
??? UI/
    ??? WBP_Inventory
    ??? WBP_Shop
    ??? ...
```

### ? Naming Conventions

| ��� | ������� | ������ |
|-----|---------|--------|
| Item Data Asset | `DA_` | `DA_Potion_Health` |
| Effect | `EFF_` | `EFF_Heal_50HP` |
| Shop | `DA_Shop_` | `DA_Shop_General` |
| Widget | `WBP_` | `WBP_Inventory` |
| Blueprint Actor | `BP_` | `BP_WorldItem` |

### ? ������������

1. **�������� debug �������:**

```blueprint
Console Command: "GiveItem HealthPotion 10"
  ?
  ?? Add Item (DA_Potion_Health, 10)
```

2. **����������� Print String** ��� �������:

```blueprint
OnItemAdded
  ?
  ?? Print String ("Added: {ItemName} x{Quantity}")
```

### ? ������������������

- ����������� **Object Pooling** ��� WorldItemActor (��������� ���������)
- ������������� **Max Inventory Slots** �������� ������ (20-50)
- ��� �������� �������� ����������� **�������������** ������ (UMG Scroll Box)

---

## ?? ������� ���������� �������

����� ������� ���������:

- [ ] ��� Item Data Assets ����� ���������� Item ID
- [ ] � ���� ��������� ��������� Name � Description
- [ ] ������ � ������ ���������
- [ ] ������� ��������������
- [ ] �������� ����� ���������� ����
- [ ] UI ���������� ���������� ���������
- [ ] ����������/�������� ��������� ��������
- [ ] ������ ��������� � ������ ��������
- [ ] ������������� ��������� ��������� �������
- [ ] �������� �������� ���������

---

## ?? ������� �������

### ������� �� �����������

? **���������:**
- WorldItemActor ����� Collision
- � ��������� ���� Inventory Component
- Item Data Asset �������� � WorldItemActor
- Interaction Component ��������

### ������ �� �����������

? **���������:**
- Effect �������� � Item Effects
- Trigger Type = On Use
- Effect Type �������� ���������
- � Item Data: `Is Usable = true`

### ������� �� ��������

? **���������:**
- Shop Component �������� � NPC
- Shop Data Asset ��������
- � ������ ���� ������ (������)
- ����� � ������� (Stock Quantity > 0)

---

## ?? �������������� �������

- **QUICKSTART.md** - ������� ����� ��� �������������
- **INTEGRATION_GUIDE.md** - ����������� ������������
- **API_REFERENCE.md** - ���������� �� C++ API
- **EXAMPLES.md** - ������� ������� � �������

---

## ?? ����������

������ �� ������ ���:

? ��������� �������� � �������  
? ��������� �������� �� ������  
? ����������� ��������������  
? �������� � ����������  
? ��������� ��������  
? ����������� UI  

**������� � �������� ����!** ??

---

**Advanced Inventory System** v1.0  
� 2024 Game Development Team
