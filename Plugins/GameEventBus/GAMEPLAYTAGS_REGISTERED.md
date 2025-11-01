# ?? �����: GameplayTags ���������������� - ��������� ����������!

## ?? �������� ������!

**��������:** GameplayTags �� ���� ������� � ������������
```
Error: Requested Gameplay Tag Effect.Quest.Start was not found
```

**�������:** ? ��� ���� ��������� � `Config/DefaultGameplayTags.ini`

---

## ?? ��� ����� ������� ������

### ��� 1: ������������� Unreal Editor

**�����������!** ���� ����������� ������ ��� ������� ���������.

1. ������ Unreal Editor
2. ������ �����
3. ���� ����� ������������� ���������

---

### ��� 2: (������������) �������� ���� ��� �����������

**���� �� ������ �������������:**

1. ������ **Tools ? Refresh Gameplay Tags**
2. ��� **Project Settings ? Gameplay Tags ? Reimport Gameplay Tags**

---

## ? ��� ���������

### ����: `Config/DefaultGameplayTags.ini`

**����� ���������: 29 �����**

| ��������� | ���������� | ���� |
|-----------|------------|------|
| **Inventory Events** | 7 | ItemAcquired, ItemUsed, ItemGifted, etc. |
| **Quest Events** | 5 | Started, Completed, Failed, etc. |
| **Dialogue Events** | 4 | Started, Ended, NodeReached, etc. |
| **Time Events** | 6 | HourChanged, DayChanged, Sunrise, Sunset, etc. |
| **Weather Events** | 1 | Changed |
| **Effect Tags** | 3 | Quest.Start, Dialogue.Start, Inventory.GiveItem |
| **Condition Tags** | 3 | Quest.IsActive, Inventory.HasItem, Time.InRange |

---

## ?? �������� ����� �����������

### 1. ������� ����
### 2. ������� ����

**������ ����:**
```
LogIntegrationHelpers: RegisterQuestStartEffect: Effect.Quest.Start -> Quest 'None'
LogIntegrationHelpers: Quest-Dialogue integration setup complete
```

**�� ������ ����:**
```
Error: Requested Gameplay Tag Effect.Quest.Start was not found  ?
```

### 3. ������ event history

� �������:
```
GameEventBus.EnableHistory 1
```

### 4. ������� �������

```
GameEventBus.PrintHistory
```

---

## ?? ������� �������������

### ������ 1: Quest � �������������� ����

**Quest Data Asset:**
```
Quest: "Collect Apples"
  Objective:
    Event Tag: "Inventory.Event.ItemAcquired"  ? ������ ��������!
    String Filter: "Apple"
    Target Count: 5
```

### ������ 2: Dialogue ��������� �������

**Dialogue Data Asset:**
```
Node Effect:
  Emit Event: "Quest.Event.Started"  ? ������ ��������!
  String Param: "BringApples"
```

### ������ 3: TimeOfDay ��������� �������

**������������� �������� ����� �����������:**
```
14:00 ? Time.Event.HourChanged (IntParam=14)  ? ����� ���������!
```

---

## ?? ����� ����������� �� ����������!

1. ? Inventory �������
2. ? TimeOfDay �������
3. ? Quest �������
4. ? Dialogue �������
5. ? Effect/Condition ���� (placeholders)

---

## ?? ������������

| ���� | �������� |
|------|----------|
| `Config/DefaultGameplayTags.ini` | **������������ �����** |
| `Plugins/GameEventBus/Documentation/GameplayTags.md` | ������ ������ ����� |
| `Plugins/GameEventBus/QUICK_START.md` | ������� ����� |
| `Plugins/GameEventBus/INTEGRATION_COMPLETE.md` | ���������� |

---

## ?? ������!

**����� ����������� ���������:**
- ? ��� ���� ����� �������
- ? ������� ����� �������������
- ? ������ ����� ����������� �������������

**����������� �������� ����� ������!** ??
