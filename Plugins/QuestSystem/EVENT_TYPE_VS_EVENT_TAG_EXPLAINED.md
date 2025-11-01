# ?? EventType vs EventTag: � ��� �������?

## ?? ������: ��� ���� ����� EventType dropdown ���� �� ���������� EventTag?

�������� ������! ������� �������:

---

## ?? EventType (Enum Dropdown)

### ��� ���:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
EConditionEventType EventType = EConditionEventType::Custom;
```

**Enum** � ���������������� ������ �������:

```cpp
enum class EConditionEventType : uint8
{
    OnItemAcquired,      // Item Acquired
    OnItemUsed,          // Item Used  
    OnItemGifted,        // Item Gifted
    OnNpcTalked,   // NPC Talked
    OnDialogChoice,      // Dialog Choice
    OnKillActor,  // Kill Actor
    OnEnterArea,         // Enter Area
    OnNeedChanged,       // Need Changed
    OnAffectionChanged,  // Affection Changed
    OnTimeReached,     // Time Reached
    OnDayChanged,        // Day Changed
    OnWeatherChanged,    // Weather Changed
    Custom     // Custom Event (���������� EventTag)
};
```

### ��� ����:

1. **�������� ��� ����������** � ������� ����� �� ������ ��������������� �������
2. **���������� ���������** � ����� ����� ��� ������� ��� �������� �����
3. **���������** � ����� ����������/�������� ���� � ����������� �� ����
4. **Legacy support** � ������ ������ ����� ������������ enum

---

## ??? EventTag (Gameplay Tag)

### ��� ���:

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (Categories = "GameEvent"))
FGameplayTag EventTag;
```

**Gameplay Tag** ��� ������� �������� �������:

- `GameEvent.Inventory.ItemAcquired`
- `GameEvent.Inventory.ItemUsed`
- `GameEvent.Dialogue.Started`
- `GameEvent.Quest.Completed`

### ��� ����:

1. **��������** � ����� ��������� ����� ��������� �������
2. **��������** � ���� ������������ ����������� (`GameEvent.Inventory.*`)
3. **����������** � ������������ � GameEventBus ��� �������� �������
4. **Extensibility** � ����� ��������� ����� ������� ��� ��������� ����

---

## ?? ��� ��� �������� ������?

### ������� 1: ������������ EventType (��������)

**������ ������** (�� ���������� � GameEventBus):

```
EventType: OnItemAcquired  ? �������� �� dropdown
EventTag: [�����]
ItemId: RedApple
```

**��������:** �� �������� � ����� �������� GameEventBus!

---

### ������� 2: ������������ EventTag (�������������)

**����� ������** (� GameEventBus):

```
EventType: Custom  ? ������ Custom!
EventTag: GameEvent.Inventory.ItemAcquired  ? ������ ���
ItemId: RedApple
```

**��������:** GameEventBus ��������� ������� � ������ `GameEvent.*`

---

## ?? ���������:

| �������������� | EventType (Enum) | EventTag (Tag) |
|----------------|------------------|----------------|
| **���** | ��������������� ������ | ����� Gameplay Tag |
| **��������** | ��������� enum | ����������� |
| **����������** | �� �������� � GameEventBus ? | �������� � GameEventBus ? |
| **������������** | ������� dropdown | ����� ����� ��� ���� |
| **Extensibility** | ������� ��������� ���� | �������� ���� � ������ |
| **������** | **Legacy / Deprecated** | **����������** |

---

## ? ������������:

### ����������� ������ EventTag!

**���������� ���������:**

```
EventType: Custom  ? �����������, �������� Custom
EventTag: GameEvent.Inventory.ItemAcquired  ? �����!
ItemId: RedApple
Count: 5
```

### ������ EventType �� ��� ����?

1. **�������� �������������** � ������ ������ ����� ��� ������������
2. **������� ���������** � ����� �������� �������������� EventTag ��� ������ EventType
3. **UI ���������** � ����������� ���������� ����������� ���� � ����������� �� ����

---

## ?? ��������� ������� ���������:

����� �������� ������ ������� **������������� ��������� EventTag** ��� ������ EventType:

```cpp
// � ������������ ���������:
if (EventType == EConditionEventType::OnItemAcquired)
{
    EventTag = FGameplayTag::RequestGameplayTag("GameEvent.Inventory.ItemAcquired");
}
else if (EventType == EConditionEventType::OnItemUsed)
{
    EventTag = FGameplayTag::RequestGameplayTag("GameEvent.Inventory.ItemUsed");
}
// ...
```

�� **������** ��� �� �����������, �������:

---

## ?? �����:

### EventType � ��� ���:
- **������� dropdown** � ���������������� ������ �������
- **Legacy feature** ��� ������ �������
- **���������� ���������** ��� ����������

### EventTag � ��� ���:
- **�������� ���** ������� ������������ ��������
- **������������ ����** ��� ������ � GameEventBus
- **������ �������** ��� ����� ��������� �������

### ��� ������������:
? **EventTag** � ������!
? **EventType** � ����������� (�������� `Custom`)

---

## ?? ������ ����������� ������:

```
Quest: "Collect Apples"

Objective 1: "Collect 5 apples"
  Conditions:
    [0]:
      EventType: Custom  ? ����������
   EventTag: GameEvent.Inventory.ItemAcquired  ? ����������!
   ItemId: RedApple
      Count: 5

Objective 2: "Eat 2 apples"
  Conditions:
    [0]:
      EventType: Custom  ? ����������
      EventTag: GameEvent.Inventory.ItemUsed  ? ����������!
      ItemId: RedApple
Count: 2
```

**������ ����������!** ???

---

## ?? �������������� ������������

- **[������ ���������� �� Conditions](QUEST_CONDITIONS_COMPLETE_REFERENCE.md)** � ��� ��������� ���� �������
- **[������� �������](QUEST_CONDITIONS_QUICK_REFERENCE.md)** � ������� � �������

**�Ѩ �������!** ???
