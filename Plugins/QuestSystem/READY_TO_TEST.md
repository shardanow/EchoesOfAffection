# ?? **��������� ���������� - EVENT-BASED �����������!**

## ? **��� ����������:**

### **1. Phase Rewards ������ �������� ��� ��������� ����** ?
### **2. ������� ������ ����������� ����� ���������** ?
### **3. ������� event-based ���������� ����� GameEventBus** ?

---

## ??? **����� �����������:**

```
QuestSystem ??(emit event)??> GameEventBus ??(route)??> InventorySystem
     ?           ?       ?
Rewards System          Event Routing              Listener Component
```

**������� ƨ����� ������������!** ������� �������� ����� �������.

?? **�����������:** `EVENT_BASED_ARCHITECTURE.md`

---

## ?? **��������� ����:**

### **��� 1: ������� ItemDataAsset ��� RedApple**

1. �������� **Unreal Editor**
2. � **Content Browser** ? `/Game/Items/` (�������� ����� ���� � ���)
3. **Right Click** ? **Miscellaneous** ? **Data Asset**
4. �������� �����: **`ItemDataAsset`**
5. ��������: **`ItemData_RedApple`** (��� ������ `RedApple`)
6. �������� asset � ����������:
   - **Item ID** = `RedApple` (�����: ������ ��������� � `ItemId` � quest reward!)
   - **Item Name** = `Red Apple`
   - **Item Description** = `A delicious red apple`
   - **Max Stack Size** = `99`
   - **Is Stackable** = `true`
   - **Item Icon** = (�������� �������� ������, ���� ����)
   - **Category** = `Food` (��� `Miscellaneous`)
   - **Rarity** = `Common`

7. **Save**

### **��� 2: �������� InventoryQuestRewardListener � PlayerPawn** ? **�����!**

1. �������� Blueprint ������ ��������� (����., `BP_MyAnimeCharacter`)
2. **Components Panel** ? **Add Component**
3. �������: **Inventory Quest Reward Listener**
4. �������� ���������
5. **Compile & Save**

**?? ��� ����� ���������� ������� �� ����� ����������� � ���������!**

### **��� 3: ���������, ��� PlayerPawn ����� InventoryComponent**

1. � ��� �� Blueprint ��������� ������� `InventoryComponent`
2. ���� ��� ��� - ��������: **Add Component** ? **Inventory Component**
3. **Compile** � **Save**

### **��� 4: ������� ������ ����������**

```powershell
Remove-Item "Saved\SaveGames\QuestSaveSlot.sav" -Force
```

### **��� 5: ��������� ���� � ��������������**

1. **Play In Editor** (PIE)
2. ������� ����� `Q_CollectApples`
3. ��������� ������ ���� (�������� ������)
4. ���������� � **Lianne** (��� �������� ������ ����)
5. **��������� ���������** - ������ ���������� **2? RedApple**!

---

## ?? **��������� ����:**

```
[XX:XX:XX] ? Objective 'O_TalkToLianne' completed in quest 'Q_CollectApples'
[XX:XX:XX] ?? Last phase 'P_ReturnApples' completed - broadcasting phase change for rewards
[XX:XX:XX] ?? QuestSubsystem::OnPhaseChangedInternal
[XX:XX:XX] ?? Processing phase completion rewards for 'P_ReturnApples'
[XX:XX:XX]    Reward[0]: Type=0, Amount=2, ItemId='RedApple'
[XX:XX:XX] QuestRewardSystem: Grant item 'RedApple' x2 (Quest: Q_CollectApples)
[XX:XX:XX] ? QuestRewardSystem: Emitted 'GameEvent.Quest.GrantItem' for ItemId='RedApple', Amount=2
[XX:XX:XX] ?? InventoryQuestRewardListener: Received 'GameEvent.Quest.GrantItem'!
[XX:XX:XX]    ItemId='RedApple', Amount=2, QuestId='Q_CollectApples'
[XX:XX:XX] ? InventoryQuestRewardListener: Successfully added 2x 'RedApple' to inventory
```

---

## ?? **��������� �������� � �������:**

### **�������� 1: ItemDataAsset �� ������**

```
ERROR: Failed to find ItemDataAsset for ItemID='RedApple'
```

**�������:**
- �������� ItemDataAsset ��� ������� � ��� 1
- ���������, ��� **ItemID** property = `RedApple` (��������� � quest reward!)

### **�������� 2: InventoryQuestRewardListener �� ����������**

```
ERROR: EventTag 'GameEvent.Quest.GrantItem' is INVALID!
```

**�������:**
- **������������� Unreal Editor** (���� ����������� ��� ������)
- ��������� `Config/DefaultGameplayTags.ini`

### **�������� 3: ��� ����� �� listener ����������**

```
(������ �� InventoryQuestRewardListener)
```

**�������:**
- ���������, ��� �� �������� **InventoryQuestRewardListener** � ������ PlayerPawn Blueprint
- **Compile & Save** Blueprint

### **�������� 4: AddItem ������ NULL**

```
?? AddItem returned NULL (Result: X)
```

**�������:**
- ��������� ��������� ��������� (Max Capacity, Weight Limit)
- ���������, ��� � ��������� ���� ��������� �����

---

## ?? **��� ������:**

����� ��������� ������������ �� ������:

1. **������� ��������������� ����** (���� ��� ������� ������)
2. **�������� ������ �������** � Phase Rewards
3. **��������� Quest Rewards** (RewardsOnComplete) ��� ������ ������ �� ���������� ����� ������
4. **�������� ������ ���� ������:**
   - Currency
 - Stats/Needs
   - Affection
   - Dialog branches
   - Recipes
   - Tags

---

## ?? **������������:**

- `EVENT_BASED_ARCHITECTURE.md` - **��������� �������� ����� �����������** ?
- `BUG_FIXED.md` - ������ �������� ����� ������� � �������
- `FINAL_FIX.md` - ������ ����������� Phase Rewards
- `INVENTORY_INTEGRATION.md` - ������ ������ (� ������ ������������)

---

**?? ������� ������� � ������ - EVENT-BASED!** ??

? Phase Rewards �������� ��� ���� ���
? ��� ������ ������������ ����� ���������
? Event-driven architecture ����� GameEventBus
? Items ����������� � ��������� ����� �������
? ����� ��������� � �����������

**�������� INVENTORYQUESTREWARDLISTENER � PLAYERPAWN � ����������!** ?????
