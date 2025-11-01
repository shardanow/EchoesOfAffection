# ?? **��������� ������ - EVENT-BASED ARCHITECTURE**

## ? **�������� ������:**

�� ��������� ��������: **�� ������ ���� ������ ������������ ����� ���������!**

### **��:**
```cpp
QuestRewardSystem::OnGrantItem()
{
	// ������ ����� InventoryComponent ?
	UInventoryComponent* Inventory = Player->GetComponent<UInventoryComponent>();
	Inventory->AddItem(ItemData, Amount);
}
```
**��������:** QuestSystem ? InventorySystem (������ �����)

### **�����:**
```cpp
QuestRewardSystem::OnGrantItem()
{
	// ������� ������� ����� GameEventBus ?
	UGameEventBusLibrary::EmitItemEvent(EventTag, ItemId, Amount);
}

// � ������ �������:
InventoryQuestRewardListener::HandleGrantItemEvent(Payload)
{
	// ��������� ������� � ���������� item
	Inventory->AddItem(ItemData, Amount);
}
```
**�������:** QuestSystem ? **GameEventBus** ? InventorySystem (������ ����� ����� �������)

---

## ??? **�����������:**

```
????????????????????
?  QuestSystem    ?
?     ?
? QuestRewardSystem?
?   GrantReward()  ?
????????????????????
 ? emit
         ? GameEvent.Quest.GrantItem
   ?
????????????????????
? GameEventBus ?
?          ?
?  Event Routing  ?
?  Tag Matching   ?
????????????????????
         ? broadcast
         ?
????????????????????
? InventorySystem ?
?  ?
? InventoryQuestRewardListener ?
?   HandleGrantItemEvent()      ?
?        ?                ?
?   InventoryComponent?
?      AddItem()  ?
???????????????????????????????
```

### **������������:**

1. ? **��� ������ ������������** - ������� ����������
2. ? **�����������** - ����� �������/��������� ����� ������
3. ? **�������������** - ����� �������� ����� �����������
4. ? **�������������** - ����� ����������� ��������
5. ? **������������������** - native C++ delegates

---

## ?? **��� ����������:**

### **�����:**

#### **QuestSystem:**
- ?? `QuestRewardSystem.cpp` - ������ ������ ������� ������ ������ �������
- ?? `QuestProgressManager.cpp` - ���������� ������ Phase Rewards ��� ��������� ����
- ?? `EVENT_BASED_ARCHITECTURE.md` - ����������� ������������
- ?? `READY_TO_TEST.md` - ���������� ����������

#### **InventorySystem:**
- ? `InventoryQuestRewardListener.h` - **����� ���������-���������**
- ? `InventoryQuestRewardListener.cpp` - ���������� listener

#### **Config:**
- ?? `DefaultGameplayTags.ini` - �������� ��� `GameEvent.Quest.GrantItem`

---

## ?? **��������� ��� - ���������:**

### **?? ���������� �����:**

**�������� `InventoryQuestRewardListener` � ������ PlayerPawn Blueprint!**

��� ����� ���������� ������� ����� �������������, �� ����� �� �� �������!

### **��� ��������:**

1. �������� `BP_MyAnimeCharacter` (��� ��� PlayerPawn)
2. **Add Component** ? **Inventory Quest Reward Listener**
3. **Compile & Save**

### **�����:**

1. �������� `ItemData_RedApple` asset (ItemID = "RedApple")
2. ������� ����������: `Remove-Item "Saved\SaveGames\QuestSaveSlot.sav" -Force`
3. ��������� ����
4. ��������� ����� (���������� � Lianne)
5. **��������� ���������!**

---

## ?? **��������� ���������:**

```
[QuestRewardSystem] Grant item 'RedApple' x2
[QuestRewardSystem] ? Emitted 'GameEvent.Quest.GrantItem'
[GameEventBus] Broadcasting event to 1 listeners
[InventoryQuestRewardListener] ?? Received event!
[InventoryQuestRewardListener] ? Successfully added 2x 'RedApple' to inventory
```

**� ���������:** 2? RedApple ?

---

## ?? **������������:**

- **`EVENT_BASED_ARCHITECTURE.md`** - ������ ����������� ������������
- **`READY_TO_TEST.md`** - ��������� ����������
- **`BUG_FIXED.md`** - �������� ������������ �������

---

## ?? **����:**

### **����������:**
? Phase Rewards �������� ��� ��������� ����
? ������� ������ ����������� ����� ���������
? ������� event-based ����������
? Items ����������� � ��������� ����� �������

### **�����������:**
? ������ ����� ����� GameEventBus
? ������ �����������
? ����� ��������� � �����������

### **���������:**
?? **�������� `InventoryQuestRewardListener` � PlayerPawn!**
?? ������� ItemDataAsset � ���������� ItemID
?? ������������� Editor (��� �������� ����� �����)

---

**?? ������� ������ � ������������!** ??

**�������� ��������� � ���������!** ?????
