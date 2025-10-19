# DSL Reference - ������ ����������

## ?? ��������

DSL (Domain Specific Language) ������� �������� ��������� ������� ������ ������� � ������� � ������� ��������� �������, ������� ����� �������� � runtime.

---

## ?? Conditions DSL

### ������� ���������

```ebnf
condition     ::= expression
expression    ::= term (logical_op term)*
term          ::= "!" atom | atom
atom          ::= comparison | function | boolean

comparison    ::= identifier operator value
identifier    ::= name | name "[" name "]"
operator      ::= "==" | "!=" | ">" | "<" | ">=" | "<="
value         ::= number | string | boolean

logical_op    ::= "&&" | "||"
function      ::= name "(" args ")"
args          ::= arg ("," arg)*
boolean       ::= "true" | "false"
```

### ���������

| �������� | �������� | ������ |
|----------|----------|--------|
| `==` | ����� | `time == Evening` |
| `!=` | �� ����� | `mood != Angry` |
| `>` | ������ | `affinity > 50` |
| `<` | ������ | `gold < 100` |
| `>=` | ������ ��� ����� | `romance >= 70` |
| `<=` | ������ ��� ����� | `trust <= 30` |
| `&&` | ���������� � | `a && b` |
| `||` | ���������� ��� | `a || b` |
| `!` | ���������� �� | `!visited(Node_X)` |

### ���� ������

#### 1. **Relationship Attributes**

���������: `attribute[npc_name] operator value`

**�������:**
```cpp
affinity[Lianne] >= 50
trust[Marcus] > 70
romance[Lianne] >= 80
fear[Guard] < 10
respect[Elder] > 60
jealousy[Lianne] != 0
```

**��������� ��������:**
- `affinity` � �������� (-100 to 100)
- `trust` � ������� (0 to 100)
- `respect` � �������� (0 to 100)
- `romance` � ��������� (0 to 100)
- `fear` � ����� (0 to 100)
- `jealousy` � �������� (0 to 100)

#### 2. **Inventory Items**

�������: `hasItem(item_id, min_count?)`

**�������:**
```cpp
hasItem(Rose)              // ���� ���� �� 1 ����
hasItem(Rose, 5)           // ���� ������� 5 ���
hasItem(Sword)             // ���� ���
!hasItem(Key)              // ��� �����
```

#### 3. **Gold (Currency)**

���������: `gold operator value`

**�������:**
```cpp
gold >= 100                // ���� ������� 100 ������
gold < 50                  // ������ 50 ������
gold == 0                  // ������ ��� �����
```

#### 4. **Time of Day**

���������: `time == tag`

**�������:**
```cpp
time == Morning
time == Evening
time == Night
time != Noon
```

**��������� ����:**
- `Morning` (6:00 - 12:00)
- `Noon` (12:00 - 13:00)
- `Afternoon` (13:00 - 18:00)
- `Evening` (18:00 - 21:00)
- `Night` (21:00 - 6:00)

#### 5. **Location**

���������: `location == tag`

**�������:**
```cpp
location == Village
location == Forest
location != Dungeon
```

#### 6. **Weather**

���������: `weather == tag`

**�������:**
```cpp
weather == Sunny
weather == Rainy
weather != Stormy
```

#### 7. **World State Tags**

�������: `worldState(tag)`

**�������:**
```cpp
worldState(Quest.MainQuest.Completed)
worldState(Event.Festival.Started)
!worldState(War.InProgress)
```

#### 8. **Memory Flags**

�������: `memory(key)` ��� `remember(key)`

**�������:**
```cpp
memory(FirstMeeting)              // ������ ������ �������
remember(GaveGift)                // ������, ��� ������ �������
!memory(SecretRevealed)           // ������ ��� �� �������
```

#### 9. **Visited Nodes**

�������: `visited(node_id)`

**�������:**
```cpp
visited(Node_Intro)               // ��� ��� � ���� ����
!visited(Node_Secret)             // ��� �� ����� ���� ����
visited(Node_Quest_Start)         // ����� �����
```

#### 10. **Custom Variables**

���������: `var(key) operator value`

**�������:**
```cpp
var(QuestStep) == "2"
var(SecretKnown) == "true"
var(Counter) > 5
```

#### 11. **NPC Mood**

���������: `mood == tag`

**�������:**
```cpp
mood == Happy
mood == Angry
mood != Sad
```

**��������� ����������:**
- `Neutral`, `Happy`, `Sad`, `Angry`, `Fearful`, `Excited`, `Bored`, `Anxious`

### ����������� �������

#### ������ 1: ������ � �����
```cpp
affinity[Lianne] >= 50 && time == Evening
```
**��������**: Lianne ���� (50+) � ������ �����

#### ������ 2: ���� ������� ��� ������
```cpp
hasItem(Rose) || gold >= 100
```
**��������**: ���� ���� ��� ������� 100 ������

#### ������ 3: ����� � �������
```cpp
worldState(Quest.FindFlower.Active) && location == Forest && !memory(FoundFlower)
```
**��������**: ����� ������� � � ���� � ��� �� ����� ������

#### ������ 4: ������������� ��������
```cpp
romance[Lianne] >= 70 && affinity[Lianne] >= 80 && time == Evening && !visited(Node_Confession)
```
**��������**: ������� ��������� (70+) � �������� (80+) � ����� � ��� �� �����������

#### ������ 5: ���������� ����
```cpp
affinity[Lianne] < 0 && (visited(Node_Insult) || visited(Node_Betrayal))
```
**��������**: ������������� �������� � (��������� ��� ������)

#### ������ 6: ������� �������� ��������
```cpp
(hasItem(Rose, 3) && gold >= 50) || (hasItem(Diamond) && trust[Lianne] > 60)
```
**��������**: (3 ���� � 50 ������) ��� (����� � ������� 60+)

---

## ? Effects DSL

### ������� ���������

```ebnf
effects       ::= statement (";" statement)*
statement     ::= assignment | function_call

assignment    ::= identifier operator value
identifier    ::= name | name "[" name "]"
operator      ::= "=" | "+=" | "-="
value         ::= number | string | boolean

function_call ::= name "(" args ")"
args          ::= arg ("," arg)*
```

### ���������

| �������� | �������� | ������ |
|----------|----------|--------|
| `=` | ��������� | `gold = 100` |
| `+=` | ��������� | `affinity += 10` |
| `-=` | ��������� | `gold -= 50` |

### ���� ��������

#### 1. **Relationship Modifications**

**���������:** `attribute[npc_name] operator value`

**�������:**
```cpp
affinity[Lianne] += 10
trust[Marcus] -= 5
romance[Lianne] += 15
fear[Guard] += 20
respect[Elder] = 100
jealousy[Lianne] += 30
```

**������������:**
- ��������� ���������: �5 ��� �������� ��������
- ������� ���������: �10-15 ��� ������ �������
- ������� ���������: �20+ ��� ����������� �������
- ������ ���������� (`=`) ������������ �����

#### 2. **Inventory Modifications**

**�������� �������:** `giveItem(item_id, count?)`
**������� �������:** `takeItem(item_id, count?)` ��� `removeItem(item_id, count?)`

**�������:**
```cpp
giveItem(Rose)              // ���� 1 ����
giveItem(Rose, 5)           // ���� 5 ���
takeItem(Sword)             // ������� ���
removeItem(Key, 1)          // ������� 1 ����
```

#### 3. **Gold Modifications**

**���������:** `gold operator value`

**�������:**
```cpp
gold += 100                 // ���� 100 ������
gold -= 50                  // ������� 50 ������
gold = 0                    // �������� (��� �������)
```

#### 4. **Memory Flags**

**����������:** `setMemory(key)` ��� `setMemory(key, value)`
**�������:** `clearMemory(key)`

**�������:**
```cpp
setMemory(FirstMeeting)
setMemory(GaveGift, true)
setMemory(SecretRevealed)
clearMemory(OldFlag)
```

#### 5. **World State Tags**

**��������:** `addWorldState(tag)`
**�������:** `removeWorldState(tag)`

**�������:**
```cpp
addWorldState(Quest.MainQuest.Started)
addWorldState(Event.Festival.InProgress)
removeWorldState(War.Active)
```

#### 6. **Quest Management**

**������:** `startQuest(quest_id)`
**���������:** `completeQuest(quest_id)`
**���������:** `failQuest(quest_id)`

**�������:**
```cpp
startQuest(HelpLianne)
completeQuest(FindFlower)
failQuest(RescueMission)
```

#### 7. **Custom Variables**

**���������:** `setVar(key, value)`

**�������:**
```cpp
setVar(QuestStep, "2")
setVar(SecretKnown, "true")
setVar(Counter, "5")
```

#### 8. **NPC Mood**

**���������:** `setMood(mood_tag)`

**�������:**
```cpp
setMood(Happy)
setMood(Angry)
setMood(Sad)
```

#### 9. **Events**

**Trigger �������:** `triggerEvent(event_id)`

**�������:**
```cpp
triggerEvent(Cutscene_Intro)
triggerEvent(Battle_Start)
triggerEvent(Achievement_Unlocked)
```

### ����������� �������

#### ������ 1: ������� �������
```cpp
takeItem(Rose, 1); affinity[Lianne] += 10; romance[Lianne] += 5; setMemory(GaveRose)
```
**��� ����������:**
1. ������� 1 ���� �� ���������
2. +10 � ��������
3. +5 � ���������
4. ��������� �������

#### ������ 2: ������� ��������
```cpp
gold -= 50; giveItem(Sword); setMemory(BoughtSword)
```

#### ������ 3: ������ ������
```cpp
startQuest(HelpLianne); affinity[Lianne] += 5; addWorldState(Quest.HelpLianne.Active)
```

#### ������ 4: ������
```cpp
affinity[Lianne] -= 20; trust[Lianne] -= 15; setMood(Angry); setMemory(Betrayed)
```

#### ������ 5: ������� �� �����
```cpp
completeQuest(FindFlower); gold += 200; affinity[Lianne] += 15; giveItem(Amulet); triggerEvent(QuestComplete_FindFlower)
```

#### ������ 6: ������������� ��������
```cpp
romance[Lianne] = 100; affinity[Lianne] = 100; triggerEvent(Cutscene_Confession); startQuest(DateWithLianne); setMemory(Lovers)
```

---

## ?? ������� � ����������

### ��� �������� ������

#### 1. Tokenization
```cpp
Input: "affinity[Lianne] >= 50 && hasItem(Rose)"

Tokens:
  ["affinity", "[", "Lianne", "]", ">=", "50", "&&", "hasItem", "(", "Rose", ")"]
```

#### 2. AST Construction
```cpp
AndNode
??? ComparisonNode (affinity[Lianne] >= 50)
??? FunctionNode (hasItem(Rose))
```

#### 3. Evaluation
```cpp
bool Result = AST->Evaluate(Context);
// true ���� affinity >= 50 � ���� ����
```

### �����������

������ �������� ����������:

```cpp
// ������ ���: ��������
Condition1 = Parse("affinity[Lianne] >= 50");

// ������ ���: ������ �� ����
Condition2 = Parse("affinity[Lianne] >= 50"); // same object
```

**������������:**
- ��� ���������� ��������
- ������ ��������� ������
- ������� ����������

---

## ?? Best Practices

### ? DO

#### 1. ������������ ��������� �������
```cpp
// ������
affinity[Lianne] += 10; romance[Lianne] += 5; setMemory(Date)

// ����� (������ ����)
Node1: affinity[Lianne] += 10
Node2: romance[Lianne] += 5
Node3: setMemory(Date)
```

#### 2. ������������ �������� ����� ������
```cpp
// ������
setMemory(Player_GaveFlowerToLianne)

// �����
setMemory(flag1)
```

#### 3. ��������� ������� �������� ����� ��������������
```cpp
// ������
Condition: hasItem(Rose)
Effect: takeItem(Rose); affinity += 10

// ����� (����� ���� ������)
Effect: takeItem(Rose); affinity += 10
```

#### 4. ������������ ������ ��� �������
```cpp
// ������ (����)
(affinity >= 50 && hasItem(Rose)) || gold >= 100

// ������ (������� �� ����������)
affinity >= 50 && hasItem(Rose) || gold >= 100
```

### ? DON'T

#### 1. �� ��������� ����������� �����������
```cpp
// �����
Node A: Condition "visited(B)"
Node B: Condition "visited(A)"
```

#### 2. �� ������������ ���������� �����
```cpp
// �����
affinity[Lianne] += 37

// ������
affinity[Lianne] += 10 // Small gift bonus
```

#### 3. �� ������ ������� ������� ��������
```cpp
// ����� (������� �����)
affinity += 10; romance += 5; gold -= 50; giveItem(Rose); takeItem(Gold); startQuest(Q1); completeQuest(Q2); setMemory(M1); setMemory(M2); triggerEvent(E1)

// ������ (��������� �� ����)
Node1 Effects: affinity += 10; romance += 5
Node2 Effects: gold -= 50; giveItem(Rose)
Node3 Effects: startQuest(Q1)
```

#### 4. �� �������� ��������� ����������� ������
```cpp
// ����� (��� ���� gold < 50?)
gold -= 50

// ������
Condition: gold >= 50
Effect: gold -= 50
```

---

## ?? ������������ �������

### Unit Test ������

```cpp
TEST(ConditionEvaluator, Affinity_Comparison)
{
    // Setup
    UDialogueContext* Context = NewObject<UDialogueContext>();
    Context->SetAffinity(TEXT("Lianne"), 75.0f);
    
    // Test
    bool Result1 = Evaluator->EvaluateString("affinity[Lianne] >= 50", Context);
    bool Result2 = Evaluator->EvaluateString("affinity[Lianne] >= 100", Context);
    
    // Assert
    EXPECT_TRUE(Result1);   // 75 >= 50 ?
    EXPECT_FALSE(Result2);  // 75 >= 100 ?
}
```

### Runtime ��������

```cpp
// � �������
dialogue.test "affinity[Lianne] >= 50"

// Output:
// [DialogueSystem] Condition result: true
// [DialogueSystem] - affinity[Lianne] = 75.0
// [DialogueSystem] - comparison: 75.0 >= 50.0
```

---

## ?? ������� �� ��������

### ���� 1: ������� ��������

**����������:**
- Romance >= 70
- Affinity >= 60
- �����
- �� � ���������� � ���-�� ���

**Condition:**
```cpp
romance[Lianne] >= 70 && affinity[Lianne] >= 60 && time == Evening && !memory(InRelationship)
```

**Effect (��� ��������):**
```cpp
romance[Lianne] += 10; setMemory(OnDateWithLianne); triggerEvent(DateCutscene); startQuest(FirstDate)
```

### ���� 2: ������� �����

**����������:**
- � ���������� � Lianne
- ��������� � ������

**Condition:**
```cpp
memory(InRelationshipWithLianne) && romance[Sarah] > 30
```

**Effect (���� �������):**
```cpp
romance[Lianne] -= 50; affinity[Lianne] -= 30; trust[Lianne] -= 40; jealousy[Lianne] += 80; setMood(Angry); setMemory(CheatedOnLianne); clearMemory(InRelationshipWithLianne)
```

### ���� 3: ��������

**����������:**
- ������ ������
- ����� ������� �������
- ������ �����

**Condition:**
```cpp
memory(CheatedOnLianne) && hasItem(Diamond) && var(DaysSinceBetrayal) >= 7
```

**Effect (��� ��������):**
```cpp
takeItem(Diamond); affinity[Lianne] += 20; trust[Lianne] += 10; clearMemory(CheatedOnLianne); setMemory(Forgiven); setMood(Sad)
```

---

## ?? Debugging DSL

### Syntax Errors

```cpp
// Error: Missing closing bracket
affinity[Lianne >= 50

// Error: Invalid operator
affinity[Lianne] === 50

// Error: Missing parenthesis
hasItem(Rose

// Error: Invalid function name
hasItemm(Rose)
```

### Runtime Errors

```cpp
// Error: NPC not found
affinity[UnknownNPC] >= 50
// ? Will log warning and return false

// Error: Item not in inventory
takeItem(NonExistentItem)
// ? Will log warning and fail gracefully
```

### ���������

```cpp
// Validate before save
TArray<FString> Errors;
bool bValid = Evaluator->ValidateCondition("affinity[Lianne] >= 50", Errors);

if (!bValid)
{
    for (const FString& Error : Errors)
    {
        UE_LOG(LogDialogue, Error, TEXT("%s"), *Error);
    }
}
```

---

## ?? ���������� DSL

### �������� ����� �������

1. ������� ����� �������:

```cpp
UCLASS()
class UDialogueCondition_CustomFunction : public UDialogueCondition
{
    GENERATED_BODY()

public:
    virtual bool Evaluate_Implementation(const UDialogueContext* Context) const override
    {
        // ���� ������
        return true;
    }
};
```

2. ���������������� � �������:

```cpp
void UDialogueConditionEvaluator::RegisterBuiltinFunctions()
{
    FunctionRegistry.Add(TEXT("myFunction"), UDialogueCondition_CustomFunction::StaticClass());
}
```

3. ������������:

```cpp
Condition: "myFunction(arg1, arg2)"
```

---

**������**: 1.0
**��������� ����������**: 2024
