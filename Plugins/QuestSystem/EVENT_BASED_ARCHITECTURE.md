# ?? **EVENT-BASED ARCHITECTURE - NO HARD DEPENDENCIES!**

## ? **��� ����������:**

### **1. ������� ������ ����������� ����� ���������** ?
### **2. ������� event-based ���������� ����� GameEventBus** ?
### **3. Phase Rewards �������� ����� �������** ?

---

## ??? **����� �����������:**

```
????????????????         ????????????????????         ???????????????????
? QuestSystem  ?????????>? GameEventBus   ?????????>? InventorySystem ?
?     ?  emit   ?      ? listen  ?       ?
? Rewards      ?         ? Event Routing    ?         ? Add Items       ?
????????????????         ????????????????????       ???????????????????
```

### **������������:**

1. **��� ������ ������������** - ������� ����� �������� ����������
2. **��������** - ����� ������� ����� ����������� �� �������
3. **�������������** - ����� �������� ����� ���� ������
4. **�������������** - ����� ����������� ������� ��������

---

## ?? **��� ��� ��������:**

### **��� 1: QuestRewardSystem ����� �������**

```cpp
// Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestRewardSystem.cpp
void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
	// Emit event instead of direct call
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Quest.GrantItem"));
	
	UGameEventBusLibrary::EmitItemEvent(
		this,    // WorldContext
		EventTag,    // GameEvent.Quest.GrantItem
		ItemId,      // "RedApple"
		Amount,      // 2
		nullptr,     // Instigator
		true     // bLogEvent
	);
}
```

### **��� 2: GameEventBus �������������� �������**

```
[GameEventBus] Event received: GameEvent.Quest.GrantItem
  - StringParam: RedApple
  - IntParam: 2
  - StringParam2: Q_CollectApples
```

### **��� 3: InventoryQuestRewardListener �������� �������**

```cpp
// Plugins/AdvancedInventorySystem/.../InventoryQuestRewardListener.cpp
void UInventoryQuestRewardListener::HandleGrantItemEvent(const FGameEventPayload& Payload)
{
	FName ItemId = Payload.StringParam;     // "RedApple"
	int32 Amount = Payload.IntParam;        // 2
	
	// Find InventoryComponent
	UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	
	// Find ItemDataAsset
	UItemDataAsset* ItemData = FindItemDataAssetByID(ItemId);
	
	// Add to inventory
	Inventory->AddItem(ItemData, Amount, Result);
}
```

### **��� 4: Item ����������� � ���������**

```
? Successfully added 2x 'RedApple' to inventory (Quest: Q_CollectApples)
```

---

## ?? **���������� �� ���������:**

### **��� 1: ������� ItemDataAsset** (��� ������)

1. Unreal Editor ? `/Game/Items/`
2. Right Click ? Data Asset ? **ItemDataAsset**
3. ��������: `ItemData_RedApple`
4. **ItemID** = `RedApple` ?? **�����!**
5. Save

### **��� 2: �������� InventoryQuestRewardListener � PlayerPawn** ? **�����!**

1. �������� Blueprint ������ ��������� (`BP_MyAnimeCharacter`)
2. **Components Panel** ? **Add Component**
3. �������: **Inventory Quest Reward Listener**
4. �������� ���������
5. **Compile & Save**

![Add Component](https://via.placeholder.com/600x300?text=Add+Inventory+Quest+Reward+Listener)

### **��� 3: ���������, ��� InventoryComponent ���� ����**

1. � ��� �� Blueprint ��������� ������� **Inventory Component**
2. ���� ��� ��� - ��������: **Add Component** ? **Inventory Component**
3. **Compile & Save**

### **��� 4: ������� ������ ����������**

```powershell
Remove-Item "Saved\SaveGames\QuestSaveSlot.sav" -Force
```

### **��� 5: ��������� ���� � ��������������**

1. **Play In Editor** (PIE)
2. ������� ����� `Q_CollectApples`
3. �������� ������ (������ ����)
4. ���������� � **Lianne** (������ ���� ����������)
5. **��������� ���������** - ������ ���������� **2? RedApple**!

---

## ?? **��������� ����:**

```
[XX:XX:XX] ? Objective 'O_TalkToLianne' completed in quest 'Q_CollectApples'
[XX:XX:XX] ?? Last phase 'P_ReturnApples' completed - broadcasting phase change for rewards
[XX:XX:XX] ?? QuestSubsystem::OnPhaseChangedInternal - Quest 'Q_CollectApples': 'P_ReturnApples' ? 'None'
[XX:XX:XX] ?? Processing phase completion rewards for 'P_ReturnApples'
[XX:XX:XX] ??? QuestRewardSystem::ProcessQuestRewards called
[XX:XX:XX]    Reward[0]: Type=0, Amount=2, ItemId='RedApple'
[XX:XX:XX] ?? Granting reward: Type=0
[XX:XX:XX] QuestRewardSystem: Grant item 'RedApple' x2 (Quest: Q_CollectApples)
[XX:XX:XX] ? QuestRewardSystem: Emitted 'GameEvent.Quest.GrantItem' for ItemId='RedApple', Amount=2
[XX:XX:XX] ?? InventoryQuestRewardListener: Received 'GameEvent.Quest.GrantItem'!
[XX:XX:XX]    ItemId='RedApple', Amount=2, QuestId='Q_CollectApples'
[XX:XX:XX]    Loaded ItemDataAsset from path: /Game/Items/ItemData_RedApple
[XX:XX:XX] ? InventoryQuestRewardListener: Successfully added 2x 'RedApple' to inventory (Quest: Q_CollectApples)
```

---

## ?? **��������� ��������:**

### **1. InventoryQuestRewardListener �� ���������� �� �������**

```
ERROR: EventTag 'GameEvent.Quest.GrantItem' is INVALID!
```

**�������:**
- ������������� Unreal Editor (���� ����������� ��� ������)
- ��������� `Config/DefaultGameplayTags.ini` - ������ ���� ���:
  ```ini
  +GameplayTagList=(Tag="GameEvent.Quest.GrantItem",DevComment="[QuestSystem] Request to grant item reward to player")
  ```

### **2. ��������� �� �������� � PlayerPawn**

```
(��� ����� �� InventoryQuestRewardListener ������)
```

**�������:**
- �������� `InventoryQuestRewardListener` component � ������ PlayerPawn Blueprint
- **Compile & Save**

### **3. ItemDataAsset �� ������**

```
ERROR: Failed to find ItemDataAsset for ItemID='RedApple'
```

**�������:**
- �������� ItemDataAsset ��� ������� � ��� 1
- ���������, ��� **ItemID property** = `RedApple`

---

## ?? **����������� ������:**

### **����� �����:**

1. **`QuestRewardSystem.cpp`** - ������ ��� ������� �������
2. **`InventoryQuestRewardListener.h/cpp`** - ����� ���������-���������
3. **`DefaultGameplayTags.ini`** - �������� ��� `GameEvent.Quest.GrantItem`

### **�����������:**

```
QuestSystem:
  - GameEventBus (�����������, ����� #if __has_include)
  
InventorySystem:
  - GameEventBus (�����������, ����� #if __has_include)

������ ������������ ����� QuestSystem � InventorySystem ���! ?
```

### **Event Flow:**

```
QuestProgressManager
  ??> OnObjectiveCompleted
       ??> OnQuestPhaseChanged.Broadcast
 ??> QuestSubsystem::OnPhaseChangedInternal
  ??> QuestRewardSystem::ProcessQuestRewards
   ??> QuestRewardSystem::GrantReward
 ??> OnGrantItem_Implementation
          ??> UGameEventBusLibrary::EmitItemEvent
       ??> GameEventBusSubsystem::EmitEvent
   ??> InventoryQuestRewardListener::HandleGrantItemEvent
       ??> InventoryComponent::AddItem
        ??> Item ��������! ?
```

---

## ?? **����:**

### **��� ����������:**

| �� | ����� |
|----|-------|
| QuestRewardSystem ? **InventoryComponent** (������ �����) ? | QuestRewardSystem ? **GameEventBus** ? InventoryQuestRewardListener ? |
| Ƹ����� ����������� ����� ��������� | ������ ������������� ����� ������� |
| ������ ����������� �������� | ����� ����������� � ��������� |

### **������������ ����� �������:**

1. ? **�����������** - ������� �������� ����������
2. ? **�������������** - ����� �������� ����� ���� ������
3. ? **��������** - ����� ������� ����� ����������� �� �������
4. ? **�������������** - ����� ����������� ��������
5. ? **������������������** - native C++ event delegates

---

## ?? **�������������:**

### **��� �������� ���� ���������� �������:**

```cpp
// � ����� ����������
void UMyCustomComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Quest.GrantItem"));
	
	EventBus->SubscribeToEventNative(
		EventTag,
		FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UMyCustomComponent::HandleEvent)
	);
}

void UMyCustomComponent::HandleEvent(const FGameEventPayload& Payload)
{
	// ���� ������
}
```

### **��� �������� ����� ��� �������:**

1. �������� ����� enum � `EQuestRewardType`
2. �������� case � `QuestRewardSystem::GrantReward()`
3. Emit ��������������� �������
4. �������� listener component � ������� �������

---

**?? ������� ��������� ������������!** ??

? Event-based architecture
? ��� ������ ������������
? Phase Rewards ��������
? Items ����������� � ���������

**�������� ��������� � PLAYERPAWN � ����������!** ?????
