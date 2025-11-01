# ?? ������ ��������� ���� �������� � ��������� � �������

## ? �������������� ��������:

### 1. ? **ItemAcquired** (������ ��������)
**��������!** ��������� ��������������.

**������ ������:**
```
Condition: ItemId='RedApple', Count=5
? ��������� 5 �����
```

**�������:**
- ������ �������� � �����
- ��������� �������� �� NPC
- ������� ��������
- ��������� �������

---

### 2. ? **ItemUsed** (������������� ��������)
**��������!** ������ � �������������.

**������ ������:**
```
Condition: EventTag='GameEvent.Inventory.ItemUsed', ItemId='HealthPotion', Count=3
? ����������� 3 ����� ��������
```

**�������:**
- ������������� consumable (���, �����)
- ���������� �����������
- ��������� ��������� ��������

---

### 3. ? **ItemCrafted** (����� ��������)
**��������!** ������ � �������������.

**������ ������:**
```
Condition: EventTag='GameEvent.Inventory.ItemCrafted', ItemId='IronSword', Count=1
? ��������� �������� ���
```

**�������:**
- ����� �� ��������
- ������� ���
- �������� �����

---

### 4. ? **ItemGifted** (������� �������� NPC)
**��������!** ������ � �������������.

**������ ������:**
```
Condition: EventTag='GameEvent.Inventory.ItemGifted', ItemId='Flowers', NpcId='Alice', Count=1
? �������� ����� �����
```

**�������:**
- ������� ������� NPC
- �������� ���������� ��������
- ���������� �����/������

**�������������:**
- ������������ **����������� NPC** ����� `Payload.TargetActor`
- ����� ����������� ������� **������������ NPC**

---

### 5. ? **ItemDropped** (������ ��������)
**��������!** ������ � �������������.

**������ ������:**
```
Condition: EventTag='GameEvent.Inventory.ItemDropped', ItemId='CursedAmulet', Count=1
? ���������� �� ���������� �������
```

**�������:**
- ������ �������� �� ���������
- ���������� �������� � ����
- ���������� ������

---

### 6. ? **CustomInventoryEvent** (��������� �������)
**��������!** ��� ����������� ������.

**������:**
```cpp
FGameplayTag CustomTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory.ItemEnchanted"));
UInventoryGameEventBridge::EmitCustomInventoryEvent(
    WorldContextObject,
    CustomTag,
    FName("MagicSword"),
    1,
    Player,
    Enchanter,
    AdditionalTags
);
```

**�������������:**
- ������ �������������� � ����������
- ����������� ��������� (���������, �����������)
- ��������� ������ ���������

---

## ?? ���������� � �������:

### ��������� Objective Condition:

```cpp
struct FObjectiveCondition
{
    FGameplayTag EventTag;  // ��� ������� (ItemUsed, ItemCrafted, etc.)
 FName ItemId;   // ID ��������
    FName NpcId;     // ID NPC (��� ItemGifted)
    int32 Count;   // ����������
};
```

### ������� ������� �������:

#### ����� "�������"
```
Objective 1: �������� 10 ����
  - EventTag: GameEvent.Inventory.ItemAcquired
  - ItemId: Herb
  - Count: 10

Objective 2: ��������� 3 ����� ��������
  - EventTag: GameEvent.Inventory.ItemCrafted
  - ItemId: HealthPotion
  - Count: 3

Objective 3: ����������� �����
  - EventTag: GameEvent.Inventory.ItemUsed
  - ItemId: HealthPotion
  - Count: 1
```

#### ����� "������� ��� �����"
```
Objective 1: �������� �����
  - EventTag: GameEvent.Inventory.ItemAcquired
  - ItemId: Flowers
  - Count: 1

Objective 2: �������� ����� �����
  - EventTag: GameEvent.Inventory.ItemGifted
  - ItemId: Flowers
  - NpcId: Alice
  - Count: 1
```

#### ����� "��������"
```
Objective 1: ���������� �� ���������� ��������
  - EventTag: GameEvent.Inventory.ItemDropped
  - ItemId: CursedAmulet
  - Count: 1
```

---

## ?? ��� ������������ � ����� ����:

### 1. ��� Use/Consume ���������:

```cpp
// � ����� Item/Potion ������ ��� �������������:
void AHealthPotion::Use(AActor* User)
{
    // ���� ������ �������������
    RestoreHealth(User);
  
    // ����������� ������� ��� �������
    UInventoryGameEventBridge::EmitItemUsedEvent(
     this,
        FName("HealthPotion"),
        User,
    nullptr  // Target (���� �����)
    );
}
```

### 2. ��� ������:

```cpp
// � ����� ������� ������:
void UCraftingComponent::CraftItem(FName ItemId, int32 Count, AActor* Crafter)
{
    // ���� ������ ������
    AddItemToInventory(ItemId, Count);
  
    // ����������� ������� ��� �������
    UInventoryGameEventBridge::EmitItemCraftedEvent(
        this,
        ItemId,
        Count,
    Crafter
    );
}
```

### 3. ��� ������� NPC:

```cpp
// � ����� Gift/Trade �������:
void UGiftComponent::GiveGiftToNPC(FName ItemId, AActor* Giver, AActor* NPCReceiver)
{
    // ���� ������ ��������
    RemoveItemFromInventory(ItemId);
    
    // ����������� ������� ��� �������
    UInventoryGameEventBridge::EmitItemGiftedEvent(
     this,
    ItemId,
        1,
        Giver,
        NPCReceiver  // �����! ����� ����� ��������� ����������� NPC
    );
}
```

### 4. ��� �������:

```cpp
// � InventoryComponent ��� �������:
void UInventoryComponent::DropItem(FName ItemId, int32 Count, AActor* Dropper)
{
    // ���� ������ �������
    SpawnItemInWorld(ItemId, Count);
    
    // ����������� ������� ��� �������
    UInventoryGameEventBridge::EmitItemDroppedEvent(
        this,
      ItemId,
        Count,
     Dropper
    );
}
```

---

## ?? ������������:

### ��� 1: ������������ ������
Intermediate ��� AdvancedInventorySystem ��� ������.

### ��� 2: �������� �������� �����

� Data Asset �������� ����� � ������� ������ objectives:

```
Phase 1:
  - Objective: �������� 3 ������
    Condition: EventTag=GameEvent.Inventory.ItemAcquired, ItemId=RedApple, Count=3

Phase 2:
  - Objective: ����������� ������
    Condition: EventTag=GameEvent.Inventory.ItemUsed, ItemId=RedApple, Count=1
```

### ��� 3: �������� ������� � ����

```cpp
// ���� � ������� ��� Blueprint:
UInventoryGameEventBridge::EmitItemUsedEvent(this, FName("RedApple"), PlayerCharacter, nullptr);
```

### ��� 4: ��������� Output Log

```
[LogInventoryEvents] [InventoryGameEventBridge::EmitItemUsedEvent] ?? Called! ItemId='RedApple'
[LogInventoryEvents] [InventoryGameEventBridge] ? EventTag valid: 'GameEvent.Inventory.ItemUsed'
[LogInventoryEvents] [InventoryGameEventBridge] ?? Item Used event emitted to GameEventBus!
    ??
[LogQuestBridge] ?? Received game event: 'GameEvent.Inventory.ItemUsed'
    ??
[QuestEventBus] ??? MATCH FOUND!
    ??
[QuestProgressManager] ? Objective completed!
```

---

## ?? �����:

### ? ��������� �������������� 5 ��������:
1. ? **Acquire** � ������/���������
2. ? **Use** � �������������/�����������
3. ? **Craft** � �����/��������
4. ? **Gift** � ������� NPC
5. ? **Drop** � ������/����������

### ? ��� �������:
- ��������� ����������� ����� `GameEventBus`
- ����� ���������� ���� `GameEvent.Inventory.*`
- ��������� ���������� ��� �������
- ��������� � `QuestEventBridge`
- �������������� `QuestEventBus`

### ? �������������� �����������:
- **������������ ����������� NPC** ��� �������
- **��������� �������** ����� `EmitCustomInventoryEvent`
- **Target Actor** ��� ������� � ����� �����������
- **Additional Tags** ��� ����������� ������

**�Ѩ ������ � �������������!** ?????
