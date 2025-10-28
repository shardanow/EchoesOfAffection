# Advanced Inventory System - User Guide

**Руководство для Game Designers**  
Версия: 1.0  
Для Unreal Engine 5.6+

---

## ?? Содержание

1. [Введение](#введение)
2. [Создание предметов (Items)](#создание-предметов-items)
3. [Создание эффектов предметов](#создание-эффектов-предметов)
4. [Размещение предметов на уровне](#размещение-предметов-на-уровне)
5. [Настройка взаимодействия с предметами](#настройка-взаимодействия-с-предметами)
6. [Работа с инвентарем](#работа-с-инвентарем)
7. [Использование предметов](#использование-предметов)
8. [Создание магазинов](#создание-магазинов)
9. [UI и визуализация](#ui-и-визуализация)
10. [Частые сценарии](#частые-сценарии)

---

## Введение

**Advanced Inventory System** - это полнофункциональная система инвентаря для Unreal Engine, которая позволяет создавать предметы, эффекты, магазины и управлять ними полностью в редакторе без написания кода.

### Основные возможности

- ? **Data-driven подход** - всё настраивается через Data Assets
- ? **Эффекты предметов** - здоровье, характеристики, теги
- ? **Система взаимодействия** - подбор предметов с уровня
- ? **Торговля** - магазины, покупка/продажа
- ? **Полная интеграция** - работает с системой Interaction

---

## Создание предметов (Items)

### Шаг 1: Создание Item Data Asset

1. **Откройте Content Browser**
2. Перейдите в папку `Content/Items/` (или создайте её)
3. **ПКМ** ? **Miscellaneous** ? **Data Asset**
4. Выберите класс **`ItemDataAsset`**
5. Назовите файл, например: `DA_Potion_Health`

![Создание Data Asset](https://via.placeholder.com/600x200/4CAF50/FFFFFF?text=Right+Click+?+Data+Asset)

### Шаг 2: Настройка базовых свойств

Откройте созданный Data Asset и заполните поля:

#### ?? Identity (Идентификация)

| Свойство | Описание | Пример |
|----------|----------|---------|
| **Item ID** | Уникальный идентификатор | `HealthPotion` |
| **Item Name** | Отображаемое имя | `Health Potion` |
| **Item Description** | Описание предмета | `Restores 50 HP` |
| **Icon** | Иконка для UI | Выберите текстуру |
| **Mesh** | 3D модель | Выберите Static Mesh |

#### ?? Classification (Классификация)

| Свойство | Значение | Описание |
|----------|----------|----------|
| **Category** | `Consumable` | Расходуемый предмет |
| **Rarity** | `Common` / `Rare` / `Legendary` | Редкость |
| **Item Tags** | `Item.Consumable.Potion` | Теги для фильтрации |

#### ?? Stacking (Стакирование)

```
? Is Stackable: true
 Max Stack Size: 99
```

#### ?? Usage (Использование)

```
? Is Usable: true
? Consume On Use: true
   Use Cooldown: 0.0
```

### Шаг 3: Настройка экономики

#### ?? Economy

```
Base Value: 50         // Базовая цена
? Can Be Sold: true
? Can Be Bought: true
? Can Be Gifted: true
Sellback Percentage: 0.5  // 50% при продаже
```

### Пример готового предмета

**DA_Potion_Health** (Зелье здоровья):
- Item ID: `HealthPotion`
- Name: `Health Potion`
- Description: `Restores 50 HP instantly`
- Category: `Consumable`
- Rarity: `Common`
- Stackable: ? (99)
- Base Value: 50 золота
- Icon: T_Icon_Potion_Red

---

## Создание эффектов предметов

Эффекты определяют, что происходит при использовании предмета.

### Шаг 1: Создание Effect Data Asset

1. **Content Browser** ? **ПКМ** ? **Data Asset**
2. Выберите **`ItemEffectDataAsset`**
3. Назовите: `EFF_Heal_50HP`

### Шаг 2: Настройка типа эффекта

#### ?? Effect Type (Тип эффекта)

Доступные типы:

| Тип | Описание | Когда использовать |
|-----|----------|-------------------|
| **Modify Stat** | Изменяет характеристику | Здоровье, мана, выносливость |
| **Add Tag** | Добавляет тег | Баффы, дебаффы, состояния |
| **Remove Tag** | Удаляет тег | Снятие эффектов |
| **Trigger Event** | Запускает событие | Кастомная логика |

### Шаг 3: Настройка Modify Stat эффекта

Для зелья здоровья:

```
Effect Type: Modify Stat
Stat Name: "Health"
Modification Type: Add
Modification Value: 50.0
```

#### Примеры других эффектов:

**Зелье маны:**
```
Effect Type: Modify Stat
Stat Name: "Mana"
Modification Type: Add
Modification Value: 30.0
```

**Баф силы:**
```
Effect Type: Add Tag
Tag To Add: "Buff.Strength"
Duration: 60.0  // 1 минута
```

**Антидот (снятие яда):**
```
Effect Type: Remove Tag
Tag To Remove: "Debuff.Poison"
```

### Шаг 4: Привязка эффекта к предмету

1. Откройте **Item Data Asset** (DA_Potion_Health)
2. Найдите секцию **Effects**
3. Добавьте элемент в массив **Item Effects**
4. Выберите созданный **EFF_Heal_50HP**
5. Установите **Trigger Type**: `On Use`

#### Триггеры эффектов:

| Trigger | Когда срабатывает |
|---------|-------------------|
| **On Use** | При использовании предмета |
| **On Equip** | При экипировке |
| **On Unequip** | При снятии |
| **On Pickup** | При подборе |
| **On Drop** | При выбрасывании |

---

## Размещение предметов на уровне

### Метод 1: World Item Actor

#### Шаг 1: Размещение актора

1. Откройте ваш уровень
2. **Place Actors** ? поиск: `WorldItemActor`
3. Перетащите на уровень
4. Или **Modes** ? **All Classes** ? `WorldItemActor`

#### Шаг 2: Настройка предмета

Выберите размещённый актор и в **Details**:

```
Item Data: DA_Potion_Health     // Ваш Item Data Asset
Initial Quantity: 1 // Количество
Instance Tags: (опционально)  // Уникальные теги
```

#### Шаг 3: Настройка визуализации

**Важно! Есть 2 способа задать меш предмета:**
##### Способ A: Через Item Data Asset (рекомендуется)

1. Откройте **Item Data Asset** (DA_Potion_Health)
2. Найдите секцию **Physical Representation**
3. Установите:

```
Physical Representation:
  ?? Static Mesh: SM_Potion_Bottle
  ?? Location Offset: (0, 0, 0)
  ?? Rotation Offset: (0, 0, 0)
  ?? Scale: (1, 1, 1)
  ? Can Be Picked Up: true
```

При этом способе меш **автоматически** применится ко всем WorldItemActor с этим Item Data.

##### Способ B: Вручную на акторе

Если в Item Data Asset **не указан меш**, можно задать его напрямую на акторе:

1. Выберите WorldItemActor на уровне
2. В Details найдите **Static Mesh Component**
3. Установите **Static Mesh**: `SM_Potion_Bottle`

**?? Внимание:**
- Если в Item Data Asset **есть меш** ? он переопределит ручную настройку
- Если в Item Data Asset **нет меша** ? сохранится меш, установленный вручную

#### Настройка collision:

```
Static Mesh Component:
  ?? Collision Enabled: Query Only
  ?? Object Type: WorldDynamic
  ?? Collision Presets: BlockAllDynamic
```

### Метод 2: Blueprint spawn

Для динамического спавна создайте Blueprint:

1. Создайте **Actor Blueprint**: `BP_ItemSpawner`
2. Добавьте ноду **Spawn Actor from Class**
3. Class: `WorldItemActor`
4. После спавна вызовите `Initialize Item`:

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

## Настройка взаимодействия с предметами

### Интеграция с Interaction System

Ваш проект уже имеет систему взаимодействия. Настроим подбор предметов.

#### Шаг 1: Настройка WorldItemActor

`WorldItemActor` уже реализует **`IInteractableInterface`**.

В настройках актора:

```
Interactable Component:
  ? Can Be Interacted With: true
  Interaction Prompt: "Press E to pick up {ItemName}"
  Interaction Time: 0.0  // Мгновенный подбор
```

#### Шаг 2: Настройка персонажа

Убедитесь, что у вашего персонажа есть:

1. **Interaction Component** (уже есть в проекте)
2. **Inventory Component**

Добавление Inventory Component:

1. Откройте Blueprint персонажа (BP_PlayerCharacter)
2. **Add Component** ? `InventoryComponent`
3. Настройте:

```
Max Inventory Slots: 20
Max Weight Capacity: 0.0  // Без ограничения веса
? Auto Stack: true
? Auto Sort: false
```

#### Шаг 3: Blueprint логика подбора

В **WorldItemActor** уже реализовано событие `OnInteract`:

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

**Это уже работает из коробки!** Просто разместите WorldItemActor на уровне.

### Кастомизация подбора

Если нужно изменить логику:

1. Создайте **Blueprint** наследник `WorldItemActor`: `BP_WorldItem_Custom`
2. Override функцию `OnInteract`
3. Добавьте кастомную логику:

```blueprint
OnInteract
  ?
  ?? Play Sound 2D (Pickup Sound)
  ?
  ?? Spawn Emitter at Location (Particle Effect)
  ?
  ?? Parent: OnInteract  // Вызов базовой логики
  ?
  ?? Show Notification ("Picked up {ItemName}")
```

---

## Работа с инвентарем

### Добавление предметов

#### Через Blueprint:

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

#### Пример: Квестовая награда

```blueprint
Event OnQuestComplete
  ?
  ?? Add Item
       ?? Item Data: DA_Reward_Gold
       ?? Quantity: 100
       ?? Result ? Print String ("Received 100 gold!")
```

### Удаление предметов

```blueprint
??????????????????
? Remove Item    ?
? By Data        ?
? Item: DA_...   ?
? Quantity: 1    ?
??????????????????
```

### Проверка наличия

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

### Получение всех предметов

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

## Использование предметов

### Способ 1: Из Blueprint

```blueprint
Event Use Potion Hotkey
  ?
  ?? Find Item by Data (DA_Potion_Health)
  ?    ?
  ?    ?? Use Item
  ?     ?? Item ID: [найденный ID]
  ?         ?? User: Self
  ?         ?? Result
  ?   ?
  ?      ?? Branch (Success?)
  ?       ?? True: Play Animation
```

### Способ 2: Из UI

1. Создайте **Widget Blueprint**: `WBP_InventorySlot`
2. Добавьте **Button**
3. В `OnClicked`:

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

### Эффекты при использовании

Когда игрок использует предмет:

1. **Inventory Component** вызывает `Use Item`
2. Проверяется `Can Be Used`
3. Срабатывают **эффекты** с триггером `On Use`
4. Если `Consume On Use = true`, предмет удаляется
5. Срабатывает событие `OnItemUsed`

### Подписка на события использования

В **персонаже**:

```blueprint
Event BeginPlay
  ?
  ?? Bind Event to OnItemUsed
       ?
       ?? Event
 ?? Item: [использованный предмет]
            ?? User: [кто использовал]
  ?? Success: [успешно?]
          ?
             ?? Branch
      ?? True:  Play Sound + Animation
       ?? False: Show Error Message
```

---

## Выбрасывание и другие действия

### Drop Item (Выбросить)

```blueprint
Drop Item
  ?? Item ID: [ID предмета]
  ?? Quantity: 1
  ?? Drop Location: [позиция персонажа + forward]
  ?? Out Result
       ?
       ?? Returns: WorldItemActor (spawned)
```

**Автоматическая функция** `Drop Item In Front`:
- Высчитывает позицию впереди персонажа
- Делает трейс вниз до земли
- Спавнит WorldItemActor
- Передаёт все данные предмета

### Gift Item (Подарить)

```blueprint
Gift Item
  ?? Item ID: [ID]
  ?? Receiver: [другой актор]
  ?? Quantity: 1
  ?? Result
```

Проверки:
- ? `Can Be Gifted = true`
- ? У получателя есть Inventory Component
- ? У получателя есть место

### Swap Items (Поменять местами)

```blueprint
Swap Items
  ?? Slot Index A: 0
?? Slot Index B: 5
```

Используется для UI drag-and-drop.

### Sort Inventory

```blueprint
Sort Inventory
```

Сортирует автоматически по:
1. Категории
2. Редкости (выше ? раньше)
3. Имени (алфавит)

---

## Создание магазинов

### Шаг 1: Создание Shop Data Asset

1. **Content Browser** ? **Data Asset**
2. Класс: **`ShopDataAsset`**
3. Название: `DA_Shop_GeneralStore`

### Шаг 2: Настройка магазина

```
Shop ID: "GeneralStore"
Shop Name: "General Store"
Shop Description: "Buy and sell items"
Shop Keeper Name: "Merchant Tom"

Shop Inventory (массив предметов):
  [0] Stock Item:
      ?? Item Data: DA_Potion_Health
      ?? Stock Quantity: 10
    ?? Price: 50
      ?? Unlimited Stock: false
   ?? Restock Time: 3600.0  // 1 час
   
  [1] Stock Item:
      ?? Item Data: DA_Potion_Mana
      ?? Stock Quantity: 5
      ?? Price: 75
      ?? Unlimited Stock: false
      
  [2] Stock Item:
      ?? Item Data: DA_Bread
      ?? Price: 10
      ?? Unlimited Stock: true  // Бесконечный товар
```

### Настройки цен:

```
Buy Price Multiplier: 1.0    // Множитель цены покупки
Sell Price Multiplier: 0.5   // Игрок продаёт за 50%
? Can Buy From Player: true
? Can Sell To Player: true
```

### Шаг 3: Создание Shop Component

1. Откройте Blueprint NPC торговца: `BP_Merchant`
2. **Add Component** ? `ShopComponent`
3. Настройки:

```
Shop Data: DA_Shop_GeneralStore
? Auto Initialize: true
```

### Шаг 4: Настройка взаимодействия

В **BP_Merchant**:

```blueprint
Event OnInteract (from Interactable)
  ?
  ?? Open Shop UI
       ?? Shop Component: [Self.ShopComponent]
       ?? Player: [Interacting Actor]
```

---

## Покупка и продажи

### Покупка предмета

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

**Автоматические проверки:**
- ? Достаточно ли денег у игрока?
- ? Есть ли товар в наличии?
- ? Есть ли место в инвентаре?

### Продажа предмета

```blueprint
Sell Item
  ?? Shop Component: [Shop]
  ?? Item ID: [ID из инвентаря игрока]
  ?? Quantity: 1
  ?? Seller: PlayerCharacter
  ?? Out Result
```

**Автоматически:**
- ? Удаляет из инвентаря игрока
- ? Добавляет деньги игроку
- ? Добавляет товар в магазин (если скупает)

### Получение цены

```blueprint
Get Buy Price
?? Item Data: DA_Sword
  ?? Returns: int32 (цена покупки)

Get Sell Price
  ?? Item Data: DA_Sword
  ?? Returns: int32 (цена продажи)
```

---

## UI и визуализация

### Создание базового Inventory UI

#### Шаг 1: Widget Blueprint

1. Создайте **Widget**: `WBP_Inventory`
2. Добавьте **Uniform Grid Panel**
3. В **Graph**:

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

#### Шаг 2: Slot Widget

Создайте `WBP_InventorySlot`:

**Компоненты:**
- **Image** (иконка предмета)
- **Text** (название)
- **Text** (количество)
- **Button** (клик для использования)

**Переменные:**
- `Item Data` (ItemDataAsset)
- `Item ID` (FInventoryItemId)
- `Quantity` (int32)

**Логика:**

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

### Создание Shop UI

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

**Логика Buy Button:**

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

## Частые сценарии

### ?? Сценарий 1: Квестовый предмет

**Цель:** Создать ключ, который нельзя продать/выбросить.

1. Создайте `DA_Quest_Key`
2. Настройки:

```
Item ID: "QuestKey"
Item Name: "Old Key"
? Is Quest Item: true
Category: QuestItem
? Can Be Sold: false
? Can Be Dropped: false
? Can Be Gifted: false
```

3. Используйте в Blueprint двери:

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

### ?? Сценарий 2: Еда с эффектом сытости

**Цель:** Хлеб восстанавливает голод и даёт временный бафф.

1. **Создайте эффект сытости:**

`EFF_Food_Satiety`:
```
Effect Type: Modify Stat
Stat Name: "Hunger"
Modification Type: Add
Modification Value: 30.0
```

2. **Создайте бафф:**

`EFF_Buff_WellFed`:
```
Effect Type: Add Tag
Tag To Add: "Buff.WellFed"
Duration: 300.0  // 5 минут
```

3. **Создайте предмет:**

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

### ?? Сценарий 3: Оружие с экипировкой

**Цель:** Меч, который даёт +10 к атаке при экипировке.

1. **Создайте эффект:**

`EFF_Sword_AttackBonus`:
```
Effect Type: Modify Stat
Stat Name: "Attack"
Modification Type: Add
Modification Value: 10.0
```

2. **Создайте предмет:**

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

3. **В Blueprint персонажа:**

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

### ?? Сценарий 4: Магазин с разными ценами

**Цель:** В городе дешевле, в деревне дороже.

Создайте 2 магазина:

`DA_Shop_City`:
```
Shop ID: "CityShop"
Buy Price Multiplier: 1.0
Sell Price Multiplier: 0.6  // Лучше продажа
```

`DA_Shop_Village`:
```
Shop ID: "VillageShop"
Buy Price Multiplier: 1.2   // Дороже покупка
Sell Price Multiplier: 0.4  // Хуже продажа
```

---

### ?? Сценарий 5: Ограниченные товары

**Цель:** Редкий товар появляется раз в день.

`DA_Shop_Rare`:
```
Stock Items:
  [0] Rare Sword
      ?? Stock Quantity: 1
      ?? Price: 1000
      ?? ? Unlimited Stock: false
   ?? Restock Time: 86400.0  // 24 часа
      ?? Restock Amount: 1
```

После покупки товар исчезнет и появится через 24 часа игрового времени.

---

## ?? Советы и Best Practices

### ? Организация Content

Рекомендуемая структура папок:

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

| Тип | Префикс | Пример |
|-----|---------|--------|
| Item Data Asset | `DA_` | `DA_Potion_Health` |
| Effect | `EFF_` | `EFF_Heal_50HP` |
| Shop | `DA_Shop_` | `DA_Shop_General` |
| Widget | `WBP_` | `WBP_Inventory` |
| Blueprint Actor | `BP_` | `BP_WorldItem` |

### ? Тестирование

1. **Добавьте debug команды:**

```blueprint
Console Command: "GiveItem HealthPotion 10"
  ?
  ?? Add Item (DA_Potion_Health, 10)
```

2. **Используйте Print String** для отладки:

```blueprint
OnItemAdded
  ?
  ?? Print String ("Added: {ItemName} x{Quantity}")
```

### ? Производительность

- Используйте **Object Pooling** для WorldItemActor (множество предметов)
- Ограничивайте **Max Inventory Slots** разумным числом (20-50)
- Для большого магазина используйте **виртуализацию** списка (UMG Scroll Box)

---

## ?? Чеклист готовности системы

Перед релизом проверьте:

- [ ] Все Item Data Assets имеют уникальные Item ID
- [ ] У всех предметов заполнены Name и Description
- [ ] Иконки и модели назначены
- [ ] Эффекты протестированы
- [ ] Магазины имеют корректные цены
- [ ] UI отображает информацию правильно
- [ ] Сохранение/загрузка инвентаря работает
- [ ] Подбор предметов с уровня работает
- [ ] Использование предметов применяет эффекты
- [ ] Торговля работает корректно

---

## ?? Решение проблем

### Предмет не подбирается

? **Проверьте:**
- WorldItemActor имеет Collision
- У персонажа есть Inventory Component
- Item Data Asset назначен в WorldItemActor
- Interaction Component работает

### Эффект не применяется

? **Проверьте:**
- Effect добавлен в Item Effects
- Trigger Type = On Use
- Effect Type настроен правильно
- В Item Data: `Is Usable = true`

### Магазин не работает

? **Проверьте:**
- Shop Component добавлен к NPC
- Shop Data Asset назначен
- У игрока есть деньги (валюта)
- Товар в наличии (Stock Quantity > 0)

---

## ?? Дополнительные ресурсы

- **QUICKSTART.md** - Быстрый старт для программистов
- **INTEGRATION_GUIDE.md** - Техническая документация
- **API_REFERENCE.md** - Справочник по C++ API
- **EXAMPLES.md** - Готовые примеры и шаблоны

---

## ?? Заключение

Теперь вы знаете как:

? Создавать предметы и эффекты  
? Размещать предметы на уровне  
? Настраивать взаимодействие  
? Работать с инвентарем  
? Создавать магазины  
? Настраивать UI  

**Успехов в создании игры!** ??

---

**Advanced Inventory System** v1.0  
© 2024 Game Development Team
