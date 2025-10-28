# Blueprint Examples - Advanced Inventory System

**Готовые решения для копирования в ваши Blueprints**

---

## ?? Содержание

1. [Основные операции](#основные-операции)
2. [UI взаимодействие](#ui-взаимодействие)
3. [Игровые механики](#игровые-механики)
4. [Интеграция с другими системами](#интеграция-с-другими-системами)

---

## Основные операции

### ?? Подбор предмета игроком

**В WorldItemActor** (уже реализовано):

```
Event OnInteract
  Input: Interacting Actor (Actor)
  ?
  ?? Get Component By Class
  ?   Input: Interacting Actor
  ?   Class: InventoryComponent
  ?   ?
  ?   ?? Is Valid?
  ?       ?
  ?    ?? True:
  ?    ?   ?
  ?       ?   ?? Add Item
  ?       ?   ?   Input: Item Data = [Self.ItemData]
  ?       ?   ?   Input: Quantity = [Self.Quantity]
  ?       ? ?   Output: Result
  ?       ?   ?   ?
  ?       ??   ?? Branch (Result == Success?)
  ?    ?   ?       ?
  ??   ?       ?? True:
  ?       ?   ?       ?   ?? Play Sound 2D (PickupSound)
  ?       ?   ?     ?   ?? Destroy Actor
  ?       ?   ?       ?   ?? Print String ("Picked up!")
  ?       ?   ?       ?
  ? ?   ?       ?? False:
  ?       ?   ?           ?? Print String ("Inventory full!")
  ?       ? ?
  ?       ?   ?? Return
  ?       ?
  ?       ?? False:
  ?     ?? Print String ("No inventory component!")
```

---

### ?? Использование предмета по нажатию клавиши

**В PlayerCharacter Blueprint:**

```
Event: Input Action (Hotkey 1)
  ?
  ?? Get Component By Class
  ?   Class: InventoryComponent
  ?   ?
  ?   ?? Get All Items
  ?    ?
  ?     ?? For Each Loop
  ?        ?
  ?         ?? Break InventoryItem
  ?           ?   Output: ItemData, ItemInstanceId, Quantity
  ?    ?   ?
  ?           ?   ?? Branch (ItemData == DA_Potion_Health?)
  ?           ?  ?
  ?       ?       ?? True:
  ?      ?       ?   ?
  ?      ?       ?   ?? Use Item
  ?           ? ?       Input: ItemId = ItemInstanceId
  ? ?       ?       Input: User = Self
  ?        ?       ?       Output: Result
  ?    ?       ?       ?
  ?   ?       ?       ?? Branch (Success?)
  ?           ?       ?  ?
  ?    ?       ?           ?? True:
  ?           ?       ?           ?   ?? Play Montage (DrinkPotion)
  ?    ?       ?           ?
  ? ?  ?           ?? False:
  ?  ?       ?   ?? Print String ("Can't use")
  ?        ?       ?
  ?         ?       ?? False:
  ?           ?           ?? (Continue loop)
```

**Упрощенная версия (если известен Item ID):**

```
Hotkey 1 Pressed
  ?
  ?? Use Item
      Input: Item ID = [сохранённый ID из UI]
   Input: User = Self
      Output: Result
```

---

### ?? Выбрасывание предмета

```
Event: Drop Button Clicked
  Input: Item ID (FInventoryItemId)
  ?
  ?? Get Inventory Component
  ?   ?
  ?   ?? Drop Item In Front
  ?   Input: Item ID
  ?       Input: Quantity = 1
  ?       Input: Drop Distance = 200.0
  ?       Output: Result
  ?       Output: Spawned Actor
  ?       ?
  ?     ?? Branch (Success?)
  ?           ?
  ?  ?? True:
  ?           ?   ?? Play Sound 2D (DropSound)
  ?    ?
  ?           ?? False:
  ?     ?? Print String ("Can't drop quest item")
```

---

### ?? Передача предмета другому игроку

```
Event: Gift Item to NPC
  Input: Receiver (Actor)
  Input: Item Data (ItemDataAsset)
  ?
  ?? Get Inventory Component (Self)
  ?   ?
  ?   ?? Find Items By Data
  ?     Input: Item Data
  ?       Output: Found Items (Array)
  ?       ?
  ? ?? Branch (Array Length > 0?)
  ?           ?
  ?           ?? True:
  ?        ?   ?
  ?           ?   ?? Get (Index 0)
  ?           ?   ?   Output: First Item
  ?           ?   ?   ?
  ?     ?   ?   ?? Break InventoryItem
  ?           ?   ?       Output: ItemInstanceId
  ?   ??       ?
  ?           ?   ?   ?? Gift Item
  ? ?   ?           Input: Item ID = ItemInstanceId
  ?  ?   ?           Input: Receiver = Receiver
  ??   ?  Input: Quantity = 1
  ?           ??           Output: Result
  ?  ?   ?           ?
  ??   ?           ?? Branch (Success?)
  ? ?   ?       ?
  ?     ?   ?          ?? True:
  ?           ?   ?       ?   ?? Print String ("Gift sent!")
  ?    ?   ?               ?
  ?    ?   ?      ?? False:
  ?           ?   ?         ?? Print String ("Can't gift")
  ? ?   ?
  ?       ?   ?? Return
  ?           ?
  ? ?? False:
  ?    ?? Print String ("You don't have this item")
```

---

## UI взаимодействие

### ?? Обновление UI инвентаря

**В Widget (WBP_Inventory):**

```
Event Construct
  ?
  ?? Get Owning Player Pawn
  ?   ?
  ?   ?? Get Component By Class
  ?       Class: InventoryComponent
  ?       Output: InventoryRef
  ?       ?
  ?       ?? Save as Variable: InventoryRef
  ?       ?
  ?       ?? Bind Event to OnInventoryChanged
  ?           Event: RefreshInventoryUI
  ?
  ?? RefreshInventoryUI (call)


Custom Event: RefreshInventoryUI
  ?
  ?? Clear Children (UniformGridPanel)
  ?
  ?? Get All Items (from InventoryRef)
  ?   Output: Items Array
  ?   ?
  ?   ?? For Each Loop
  ?       Loop Body:
  ?         Input: Array Element (UInventoryItem)
  ?         Input: Array Index
?      ?
  ?    ?? Create Widget
  ?         ?   Class: WBP_InventorySlot
  ?         ?   Output: SlotWidget
  ?         ?   ?
  ?         ? ?? Set Item Data (SlotWidget)
  ?         ?   ?   Input: InventoryItem = Array Element
  ?         ?   ?
  ?     ?   ?? Add Child to Uniform Grid
  ?         ?   Input: Content = SlotWidget
  ?         ?  Input: Row = (Index / 5)
  ?  ?       Input: Column = (Index % 5)
  ?         ?
  ?       ?? (Continue)
```

---

### ?? Drag and Drop слотов

**В WBP_InventorySlot:**

#### On Mouse Button Down:

```
Override: On Mouse Button Down
  Input: My Geometry
  Input: Mouse Event
  ?
  ?? Detect Drag
      Input: Drag Key = Left Mouse Button
 Output: Drag Drop Operation
      ?
      ?? Create Drag Drop Operation
     Class: UDragDropOperation
     ?
      ?? Set Payload
          ?   Input: ItemID = [Self.ItemID]
        ?   Input: SlotIndex = [Self.SlotIndex]
   ?
      ?? Set Default Drag Visual
           Input: Widget = [Self Widget]
```

#### On Drop:

```
Override: On Drop
  Input: My Geometry
  Input: Pointer Event
  Input: Operation (UDragDropOperation)
  ?
  ?? Get Payload
  ?   Output: Source ItemID
?   Output: Source SlotIndex
  ?   ?
  ?   ?? Get Inventory Component
  ?    ?
  ? ?? Swap Items
  ? Input: Slot A = Source SlotIndex
  ?         Input: Slot B = Self.SlotIndex
  ?      ?
  ?           ?? Return true (Handled)
```

---

### ?? Tooltip при наведении

**В WBP_InventorySlot:**

```
Override: On Mouse Enter
  ?
  ?? Branch (ItemData Is Valid?)
  ?   ?
  ?   ?? True:
  ?       ?
  ?    ?? Create Widget
  ?           Class: WBP_ItemTooltip
  ?           Output: TooltipWidget
  ?     ?
  ?           ?? Set Tooltip Data
  ?        ?   Input: Item Name = [ItemData.ItemName]
  ?           ?   Input: Description = [ItemData.Description]
  ?           ?   Input: Value = [ItemData.BaseValue]
  ?           ?
  ?           ?? Add to Viewport
  ?            Input: Widget = TooltipWidget
  ?         Input: Z-Order = 100


Override: On Mouse Leave
  ?
  ?? Remove from Parent (TooltipWidget)
```

---

## Игровые механики

### ?? Крафтинг предметов

**В PlayerCharacter:**

```
Custom Event: Craft Item
  Input: Recipe (CraftingRecipe)
  ?
  ?? For Each (Recipe.Required Items)
  ?   ?
  ?   ?? Has Item?
  ?       Input: ItemData = Loop Item Data
  ?       Input: Quantity = Loop Required Amount
  ?       Output: Has?
  ?       ?
  ?       ?? Branch (Has?)
  ?           ?
  ?           ?? False:
  ?      ?   ?? Print String ("Missing materials")
  ?  ?   ?? Return
  ?      ?
  ?  ?? True: Continue
  ?
  ?? All materials checked - Remove them:
  ?   ?
  ?   ?? For Each (Recipe.Required Items)
  ?       ?
  ?       ?? Remove Item By Data
  ? Input: ItemData = Loop Data
  ?       Input: Quantity = Loop Amount
  ?
  ?? Add Result Item
?
      ?? Add Item
          Input: ItemData = Recipe.Result Item
        Input: Quantity = Recipe.Result Quantity
          ?
    ?? Print String ("Crafted: {ResultName}")
```

---

### ?? Проверка квестового предмета

**В Quest Manager:**

```
Function: Check Quest Requirements
  Input: Quest ID (Name)
  Output: Can Complete? (bool)
  ?
  ?? Get Quest Data (by ID)
  ?   Output: Quest Data
  ?   ?
  ?   ?? For Each (Quest.Required Items)
  ?       ?
  ??? Has Item?
  ?      Input: ItemData = Loop Item
  ?           Input: Quantity = Loop Quantity
  ?           Output: Has?
  ? ?
  ?           ?? Branch (Has?)
  ?       ?
  ?       ?? True: Continue
  ?  ?
  ?     ?? False:
  ? ?? Return false
  ?
  ?? Return true
```

---

### ?? Автоматическое поднятие предметов (магнит)

**В PlayerCharacter (Tick):**

```
Event Tick
  ?
  ?? Sphere Overlap Actors
  ?   Input: World Context = Self
  ?   Input: Sphere Pos = Self Location
  ?   Input: Radius = 300.0
  ?   Input: Object Types = [WorldDynamic]
  ?   Input: Actor Class Filter = WorldItemActor
  ?   Output: Out Actors
  ?   ?
  ?   ?? For Each (Out Actors)
  ??
  ?       ?? Cast to WorldItemActor
  ?       ?   Output: WorldItem
  ? ?   ?
  ?       ?   ?? Branch (Cast Success?)
  ?       ?       ?
  ?       ?       ?? True:
  ?       ??
  ?       ? ?? Get Inventory Component
  ? ?        ?   ?
  ?       ?     ?   ?? Can Add Item?
  ?       ??       Input: WorldItem.ItemData
  ?   ?           ?       Input: WorldItem.Quantity
  ?       ?  ?  Output: Can?
  ?       ?           ?     ?
  ?       ?       ?       ?? Branch (Can?)
  ?       ?           ??
  ?       ?      ?        ?? True:
  ?       ?        ?        ?
  ?  ?           ?    ?? Lerp (Actor to Actor)
  ?       ?  ?          ?   Start: WorldItem Location
  ?       ?      ?           ?   End: Self Location
  ? ?           ?   ?   Alpha: 0.1
  ?       ?      ?       ?   ?
  ?       ?  ?          ?   ?? Set Actor Location
  ?     ?       ?               ?
  ?       ?    ?            ?? Branch (Distance < 50?)
  ?       ?           ?       ?
  ?       ?     ?    ?? True:
  ?     ?           ?             ?
  ?       ? ?           ?? Interact (auto pickup)
```

---

### ?? Система веса инвентаря

**Custom Function: Get Current Weight**

```
Function: Get Current Weight
  Output: Total Weight (float)
  ?
  ?? Get All Items
  ?   Output: Items
  ?   ?
  ? ?? For Each Loop
  ?       ?
?       ?? Get Item Data ? Physical Weight
  ?       ?   Output: Item Weight
  ?       ?
  ?       ?? Multiply (Item Weight * Quantity)
  ?  ?   Output: Slot Weight
  ?       ?
  ?       ?? Add (Total Weight + Slot Weight)
  ?           Output: New Total
  ?
  ?? Return Total Weight
```

**Check before adding:**

```
Before Add Item
  ?
  ?? Get Current Weight
  ?   Output: Current
  ?   ?
  ?   ?? Get Max Weight Capacity
  ?   ?   Output: Max
  ?   ?   ?
  ?   ?   ?? Get New Item Weight
  ?   ?   ?   Output: Adding Weight
  ?   ?   ?   ?
  ?   ??   ?? Add (Current + Adding)
  ?   ?   ?       Output: Future Weight
  ?   ?   ?       ?
  ?   ?   ?     ?? Branch (Future <= Max?)
  ?   ?   ?   ?
  ?   ?   ?           ?? True: Allow Add
  ?   ?   ?           ?
  ?   ?   ?       ?? False:
  ?   ?   ?             ?? Print String ("Too heavy!")
```

---

## Интеграция с другими системами

### ?? Сохранение инвентаря (Save Game)

**В Game Instance:**

```
Function: Save Game
  ?
  ?? Get Player Character
  ?   ?
  ?   ?? Get Inventory Component
  ?       ?
  ?  ?? Save To Json
  ?     Output: Json String
  ?           ?
  ?           ?? Create Save Game Object
  ?        Class: MySaveGame
  ?    ?
  ?   ?? Set Inventory Data
  ?       ?   Input: Json String
  ?          ?
  ?   ?? Save Game to Slot
  ?           Input: Slot Name = "Slot1"
  ?                Input: User Index = 0


Function: Load Game
  ?
  ?? Load Game from Slot
  ?   Input: Slot Name = "Slot1"
  ?   Output: Save Game Object
  ??
  ?   ?? Cast to MySaveGame
  ?  Output: SaveData
  ?    ?
  ?       ?? Get Inventory Data
  ?     Output: Json String
  ?     ?
  ?     ?? Get Player Character
  ?        ?
  ?   ?? Get Inventory Component
  ?       ?
  ?        ?? Load From Json
  ?             Input: Json String
```

---

### ?? Интеграция с Needs System

**При использовании еды:**

```
OnItemUsed Event
  Input: Item (UInventoryItem)
  Input: User (AActor)
  Input: Success (bool)
  ?
?? Branch (Success?)
  ?
      ?? True:
          ?
          ?? Branch (Item has tag "Item.Consumable.Food"?)
         ?
?? True:
   ?
       ?? Get Component (NeedsComponent)
          ?
     ?? Modify Need
          Input: Need Tag = "Need.Hunger"
        Input: Amount = 30.0
 Input: Operation = Add
```

---

### ?? Интеграция с Dialogue System

**Проверка предмета в диалоге:**

```
Dialogue Condition: Has Quest Item
  ?
  ?? Get Speaker Actor
  ?   ?
  ?   ?? Get Inventory Component
  ?       ?
  ?     ?? Has Item?
  ?           Input: ItemData = DA_Quest_Letter
  ?       Input: Quantity = 1
  ?      Output: Has?
  ?  ?
  ?           ?? Return Has
```

**Получение награды в диалоге:**

```
Dialogue Effect: Give Reward
  ?
  ?? Get Player Actor
  ?   ?
  ?   ?? Get Inventory Component
  ?    ?
  ?       ?? Add Item
  ?           Input: ItemData = DA_Reward_Gold
  ?           Input: Quantity = 100
```

---

### ?? Интеграция с Time of Day System

**Автоматическая порча еды:**

```
Event: On Hour Changed (from TimeOfDayComponent)
  Input: New Hour (int)
  ?
  ?? Get All Items
      ?
      ?? For Each Loop
    ?
       ?? Branch (Item has tag "Item.Perishable"?)
              ?
      ?? True:
                  ?
 ?? Get Custom Metadata
         ?   Key: "FreshnessLevel"
    ?   Output: Freshness (float)
   ?   ?
     ?   ?? Subtract (Freshness - 1.0)
      ?   ?   Output: New Freshness
    ?   ??
             ?   ?   ?? Branch (New Freshness <= 0?)
    ?   ?       ?
       ?   ?       ?? True:
        ?   ?       ?   ?
       ?   ?       ?   ?? Remove Item (spoiled)
     ?   ?       ?       ?? Print String ("Food spoiled!")
          ?   ?       ?
      ?   ?       ?? False:
         ?   ?           ?
 ?   ? ?? Set Custom Metadata
       ?   ?     Key: "FreshnessLevel"
        ?   ?    Value: New Freshness
```

---

## ?? Продвинутые сценарии

### ?? Система амуниции для оружия

```
Event: Fire Weapon
  ?
  ?? Get Equipped Item (Slot = "MainHand")
  ?   Output: Weapon Item
  ?   ?
  ?   ?? Get Custom Metadata (Weapon)
  ?       Key: "CurrentAmmo"
  ?       Output: Ammo Count
  ?       ?
  ?       ?? Branch (Ammo Count > 0?)
  ?         ?
  ? ?? True:
  ?           ?   ?
  ?       ?   ?? Fire Weapon Logic...
  ?     ?   ?
  ?    ?   ?? Set Metadata
  ?           ?       Key: "CurrentAmmo"
  ?        ?       Value: (Ammo - 1)
  ?           ?
  ?    ?? False:
  ?        ?
  ?             ?? Find Items By Data
  ?        Input: DA_Ammo_Bullets
  ?         Output: Ammo Items
?       ?
  ?       ?? Branch (Has ammo?)
  ?       ?
  ?      ?? True:
  ?         ?   ?
  ?            ?   ?? Reload Weapon
  ?        ?       ?? Remove Item (Bullets, 30)
  ?         ?       ?? Set Metadata ("CurrentAmmo", 30)
  ?        ?
  ?         ?? False:
  ?        ?? Print String ("Out of ammo!")
```

---

### ?? Случайный лут из контейнера

```
Event: Open Chest
  ?
  ?? Get Loot Table (Chest)
  ?   Output: Loot Entries (Array)
  ?   ?
  ?   ?? For Each (Loot Entry)
  ?    ?
  ?       ?? Random Float (0.0 to 1.0)
  ?       ?   Output: Roll
  ?       ?   ?
  ?       ?   ?? Branch (Roll <= Entry.DropChance?)
  ?       ?   ?
  ?       ?       ?? True:
  ?     ?           ?
  ?       ?           ?? Random Integer (Min, Max Quantity)
  ?       ?       ?   Output: Quantity
  ?       ?           ? ?
  ?       ?     ?   ?? Add Item
  ?       ?     ?       Input: Entry.ItemData
  ?  ?         ?       Input: Quantity
  ??  ?       ?
  ?       ?      ?       ?? Print String ("Found {Item}!")
```

---

### ?? Комбинирование предметов (алхимия)

```
Custom Event: Combine Items
  Input: Item A (ItemDataAsset)
  Input: Item B (ItemDataAsset)
  ?
  ?? Get Combination Recipe
  ?   Input: A, B
  ?   Output: Recipe (DataAsset or None)
  ?   ?
  ?   ?? Branch (Recipe Is Valid?)
  ?       ?
  ?       ?? True:
  ?       ?   ?
  ?       ?   ?? Remove Item (Item A, 1)
  ? ?   ?? Remove Item (Item B, 1)
  ?       ?   ?
  ?       ?   ?? Add Item
  ?       ?       Input: Recipe.Result Item
  ?       ?    Input: Quantity = 1
  ?       ?       ?
  ?       ?       ?? Play Animation (Crafting)
  ?       ?     ?? Print String ("Created {Result}!")
  ?       ?
  ?    ?? False:
  ?           ?? Print String ("Can't combine these items")
```

---

## ?? Полезные макросы

### Macro: Get Player Inventory

```
Macro: Get Player Inventory
  Output: Inventory Component
  ?
  ?? Get Player Character
  ?
      ?? Get Component By Class
   Class: InventoryComponent
  Output: Inventory
```

Использование:

```
Any Event
  ?
  ?? Get Player Inventory (Macro)
    Output: Inventory
      ?
      ?? Add Item...
```

---

### Macro: Show Notification

```
Macro: Show Notification
  Input: Message (Text)
  Input: Duration (float) = 3.0
  ?
  ?? Create Widget
      Class: WBP_Notification
      ?
?? Set Text (Message)
      ?? Add to Viewport
      ?
  ?? Delay (Duration)
        ?
          ?? Remove from Parent
```

---

## ?? Заключение

Эти примеры покрывают большинство сценариев использования системы инвентаря. Вы можете копировать их напрямую в свои Blueprints и адаптировать под свои нужды.

**Помните:**
- Всегда проверяйте результаты операций (Success/Failure)
- Используйте события для обновления UI
- Кешируйте ссылки на компоненты
- Тестируйте edge cases (пустой инвентарь, максимальный вес и т.д.)

---

**Happy Game Development!** ??

