# Dialogue Condition DSL Reference

## �����
DSL (Domain-Specific Language) ��� ������� �������� ������������ ������� � �������� ��������� ��� �������� ������� ���������� ���������.

## ������� ���� �������

### 1. �������� ������� ��������
```
hasItem(ItemName)
hasItem(ItemName, Count)
```

**�������:**
```
hasItem(Rose)           // ���� ����
hasItem(Gold, 100)      // ���� 100 ������
has(Sword)              // �������������� ���������
```

### 2. �������� ��������� (Affinity)
```
affinity[NPCName] >= Value
affinity[NPCName] > Value
affinity[NPCName] <= Value
affinity[NPCName] < Value
affinity[NPCName] == Value
```

**�������:**
```
affinity[Lianne] >= 50   // ��������� � ������� >= 50
affinity[Marcus] < 0     // ��������� � �������� �������������
```

### 3. �������� ������ ������
```
memory(FlagName)
```

**�������:**
```
memory(FirstMeeting)     // ���� "FirstMeeting" ����������
memory(QuestComplete)    // ����� ��������
```

### 4. �������� ��������� ����
```
visited(NodeId)
```

**�������:**
```
visited(Node_001)        // ���� Node_001 ��� �������
!visited(Node_002)       // ���� Node_002 ��� �� �������
```

### 5. �������� ������� �����
```
time == TimeTag
```

**�������:**
```
time == Morning          // ����
time == Evening          // �����
time == Night            // ����
```

### 6. �������� ������� �����
```
tag(TagName)
hasTag(TagName)
```

**�������:**
```
tag(Weather.Rainy)       // ���� �����
hasTag(Quest.Active)     // ���� �������� �����
```

### 7. �������� ��������� ����������
```
variable[VarName] == Value
var[VarName] == Value
```

**�������:**
```
variable[PlayerLevel] == 5      // ������� ������ ����� 5
var[QuestStage] == 3            // ������ ������ 3
variable[PlayerName] == "Hero"  // ��� ������ "Hero"
```

## ���������� ���������

### AND (�)
```
condition1 && condition2
condition1 AND condition2
```

**�������:**
```
hasItem(Rose) && affinity[Lianne] >= 50
memory(FirstMeeting) AND time == Evening
```

### OR (���)
```
condition1 || condition2
condition1 OR condition2
```

**�������:**
```
hasItem(Gold, 100) || hasItem(Silver, 200)
time == Morning OR time == Evening
```

### NOT (��)
```
!condition
NOT condition
```

**�������:**
```
!visited(Node_001)
NOT memory(QuestFailed)
```

## ����������� � ��������
```
(condition1 && condition2) || condition3
```

**�������:**
```
(hasItem(Rose) && time == Evening) || affinity[Lianne] >= 70
!(visited(Node_001) && memory(BadChoice))
```

## ������� �������

### ������ 1: ������������� �����
```
(affinity[Lianne] >= 50 && hasItem(Rose) && time == Evening) || affinity[Lianne] >= 80
```
���������:
- (��������� >= 50 � ���� ���� � �����) ���
- ��������� >= 80

### ������ 2: ��������� ��������
```
memory(QuestStarted) && !visited(QuestLocation) && variable[QuestStage] == 2
```
���������:
- ����� ����� �
- ������� �� �������� �
- ������ ������ = 2

### ������ 3: ������������� �������
```
(hasItem(Gold, 100) || hasItem(Gem, 5)) && 
(affinity[Merchant] >= 30 || tag(Quest.Merchant.Active))
```
���������:
- (���� 100 ������ ��� ���� 5 ����������� ������) �
- (��������� � ��������� >= 30 ��� ������� ����� ��������)

### ������ 4: ��������� � ������������� �������
```
time == Night && affinity[Lianne] >= 60 && 
!memory(Rejected) && variable[Location] == "Garden"
```

## ��������� ����������
1. `()` - ������ (������ ���������)
2. `!`, `NOT` - ���������
3. `&&`, `AND` - ���������� �
4. `||`, `OR` - ���������� ��� (������ ���������)

## ������������� � Blueprint

### ������ �������� �������:
```cpp
if (ConditionEvaluator->EvaluateString("hasItem(Rose)", Context))
{
    // �������� ������������� ������
}
```

### ������ �������� �������:
```cpp
FString ComplexCondition = TEXT(
    "(affinity[Lianne] >= 50 && hasItem(Rose)) || "
    "(affinity[Lianne] >= 70 && time == Evening)"
);

if (ConditionEvaluator->EvaluateString(ComplexCondition, Context))
{
    // ������ ������ ��������
}
```

## Best Practices

### 1. ����������� ����������� �����
```
? memory(CompletedTutorial)
? memory(flag1)
```

### 2. ����������� ��������� �������
```
? (hasItem(Key) && visited(Castle)) || hasItem(MasterKey)
? hasItem(Key) && visited(Castle) || hasItem(MasterKey)  // ������������
```

### 3. ��������� ������� ������� ���������
���������� ����� ������� ������� �� ��������� �������� � ����.

### 4. ��������� ����� ������������ �������
DSL ������ ������������� �������� ������������ ������� �� ������.

## ���������� DSL

��� ���������� ����� ����� �������:

1. �������� ����� ����� `UDialogueCondition_YourType`
2. �������� ������� � `UDialogueConditionEvaluator::ParseAtom()`
3. ���������� `Evaluate_Implementation()` � `GetDescription_Implementation()`
4. �������� ������������ �����

## �������

### �������� ��������� �������� �������:
```cpp
UDialogueCondition* Condition = Evaluator->ParseCondition("hasItem(Rose) && affinity[Lianne] >= 50");
FString Description = Condition->GetDescription();
// �������: "(Has Rose AND Affinity[Lianne] >= 50.0)"
```

### �������� ����������� ��������:
�������� � `DialogueConditionEvaluator.cpp`:
```cpp
#define DEBUG_CONDITION_PARSING 1
```
