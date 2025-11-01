# ?? ����������: ������� �������� EventTag + ����������� �������

## ?? �������� #1: ItemUsed ���������� ��� ItemAcquired!

### ��� ����:

```
Objective 1: ������� 5 �����
  Condition: EventTag = GameEvent.Inventory.ItemAcquired ?

Objective 2: ������ 2 ������
  Condition: EventTag = NONE ?  ? ������!
```

**���������:**  
��� ������� ������ ����������� **���** objectives, ������ ��� ������ `EventTag` �������� **����� �������**!

### ������ ���:

```cpp
if (Condition.EventTag.IsValid())
{
    // ��������� ������ ���� ��� �� ������
}
else
{
    // ������ ��� ? ��������� �Ѩ! ?
}
```

---

## ? ������� #1: ������� �������� EventTag

### ����� ���:

```cpp
if (Condition.EventTag.IsValid())
{
    // Exact match (no hierarchy!)
    bool bTagMatches = (Payload.EventTag == Condition.EventTag);
    if (!bTagMatches)
    {
        return false; // ? Mismatch
    }
}
else
{
  // ? EMPTY EventTag NOT ALLOWED!
    UE_LOG(LogTemp, Warning, TEXT("?? Condition.EventTag is EMPTY! Configuration error!"));
 return false; // Reject empty tags
}
```

### ���������:

1. **������ EventTag ? �����������**  
   ������ ����������� ��������� `EventTag` ��� ������� condition!

2. **������ ��������� ������ ��������**  
   ����: `Payload.EventTag.MatchesTag(Condition.EventTag)` (��������� �������� ����)  
   �����: `Payload.EventTag == Condition.EventTag` (������ ������ ����������)

3. **��������� �����������**  
   ```
   [LogTemp] Condition.EventTag = 'GameEvent.Inventory.ItemUsed' (Valid: YES)
   [LogTemp] Payload.EventTag   = 'GameEvent.Inventory.ItemAcquired'
   [LogTemp] ? EventTag mismatch - rejecting
   ```

---

## ? ������� #2: �������������� ������� �������

### ����:

```
��� ������� ? ������ ���������� ������ �����
```

### �����:

```cpp
if (TrackedQuestId.IsNone() || !QuestSubsystem)
{
    // Hide the entire widget
    SetVisibility(ESlateVisibility::Collapsed);
  UE_LOG(LogTemp, Verbose, TEXT("QuestTrackerWidget: No active quest - widget hidden"));
    return;
}

// Show widget when quest is active
SetVisibility(ESlateVisibility::Visible);
```

**������:**
- ��� ������� ? ������ **��������� �����** (Collapsed)
- ���� ����� ? ������ **������������** (Visible)

---

## ?? ��� ��������� ����� � Data Asset:

### ? �����������:

```
Objective 2: ������ 2 ������
  Conditions:
    - EventTag: NONE  ? ������! ����� �����������!
  ItemId: RedApple
      Count: 2
```

### ? ���������:

```
Objective 2: ������ 2 ������
  Conditions:
    - EventTag: GameEvent.Inventory.ItemUsed  ? �����������!
   ItemId: RedApple
      Count: 2
```

---

## ?? ���� ��� �����������:

### ��� 1: Intermediate ��� QuestSystem ��� ������

### ��� 2: ������������ ������

### ��� 3: �������� Data Asset ������

������� **���** objectives � ������ `EventTag` � ���������� ����������:

| ��� �������� | EventTag |
|-------------|----------|
| ������ �������� | `GameEvent.Inventory.ItemAcquired` |
| ������������� �������� | `GameEvent.Inventory.ItemUsed` |
| ����� �������� | `GameEvent.Inventory.ItemCrafted` |
| ������� NPC | `GameEvent.Inventory.ItemGifted` |
| ������ �������� | `GameEvent.Inventory.ItemDropped` |
| ������ � NPC | `GameEvent.Dialogue.Started` |
| ���������� ������ | `GameEvent.Quest.Completed` |

### ��� 4: ��������� Data Asset

### ��� 5: ��������� ����

---

## ?? ��� ������� � Output Log:

### ���������� ������:

```
[QuestEventBus] ?? Condition: EventTag='GameEvent.Inventory.ItemUsed', ItemId='RedApple'
[QuestEventBus] Payload.EventTag = 'GameEvent.Inventory.ItemAcquired'
[QuestEventBus] ? EventTag mismatch - rejecting
```

### ������ ������������ (������ EventTag):

```
[QuestEventBus] ?? Condition.EventTag is EMPTY! Configuration error!
[QuestEventBus] ? Rejecting because empty EventTag would match ANY event!
```

**���� ������ ��� ? ��������� Data Asset!**

---

## ?? ��������� �������:

### ��� �������:

```cpp
SetVisibility(ESlateVisibility::Collapsed);  // ��������� �����
```

�����:
```
???????????????????????
  [����� - ������ �����]
???????????????????????
```

### ���� �����:

```cpp
SetVisibility(ESlateVisibility::Visible);  // �������
```

�����:
```
???????????????????????
  ������ ������
???????????????????????
Collect 5 apples (3/5)
Eat 2 apples (0/2)
???????????????????????
[????????????] 40%
```

---

## ?? ����� ����������:

### 1. ? ������� �������� EventTag
- ������ EventTag ? **�����������**
- ������ ��������� ����� (��� ��������)
- ��������� ����������� ������

### 2. ? ����������� �������
- ��� ������� ? **Collapsed**
- ���� ����� ? **Visible**

### 3. ? �������������� ������ ������������
- ������ objective ������� **���������� EventTag**
- ���������� �������� ������� condition ������� ����������� �� ��

---

## ?? �����: ��������� ��� ������!

����� ���������� **�����������** ��������� ��� Data Assets �������:

1. �������� ������ �����
2. ��� ������� Objective ? Conditions
3. ��������� ��� **EventTag �� ������**
4. ���������

**����� objectives �� ����� �����������!**

---

## ?? ���������:

### ����:
```
? ItemUsed ���������� ��� ItemAcquired (������ EventTag)
? ������ ����������� ���� ��� �������
? ����� ���� ������� "�������������" conditions
```

### �����:
```
? ������� ��������: ������ ������ ���������� �����
? ������ ���������� ����� ��� �������
? ������ EventTag ? ������ ������������ � ����
? ���������� �������� ������� "�������"
```

**������ ������ �������� ��������� � ������!** ?????
