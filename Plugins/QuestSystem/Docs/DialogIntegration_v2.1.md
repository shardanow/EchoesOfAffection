# Quest System ? Dialog System Integration Guide

## ? ������ Event-driven ���������� ��� ��������

---

## ?? ����������

1. [����� ����������](#�����-����������)
2. [Quest Dialogue Effects](#quest-dialogue-effects)
3. [������������� � Dialog System](#�������������-�-dialog-system)
4. [�������](#�������)
5. [Blueprint Setup](#blueprint-setup)

---

## 1?? ����� ����������

### �����������

```
Dialog System (�����������)
  ?
  UDialogueNode
  ?? Effects: TArray<UDialogueEffect*>
  ?? Execute Effects ��� ��������� ����
      ?
Quest System (������������ ���� �������)
  ?? UQuestDialogueEffect_StartQuest
  ?? UQuestDialogueEffect_CompleteObjective
  ?? UQuestDialogueEffect_EmitEvent
```

**������������**:
- ? **��� ��������** - ������� ����������
- ? **Event-driven** - ������ �����������
- ? **�����������** - ����� ��������� ����� �������
- ? **Blueprint-friendly** - �������� �� ���������

---

## 2?? Quest Dialogue Effects

### UQuestDialogueEffect_StartQuest

**����������**: ��������� ����� ����� dialogue node ��������������.

**��������**:
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName QuestId; // ID ������ ��� �������

UPROPERTY(EditAnywhere, BlueprintReadWrite)
AActor* QuestAcceptor; // (Optional) ��� ��������� �����
```

**�������������**:
- �������� � DialogueNode.Effects
- ���������� QuestId
- ����� ���������� ������������� ��� ��������� ����

---

### UQuestDialogueEffect_CompleteObjective

**����������**: ��������� objective ������.

**��������**:
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName QuestId; // ID ������

UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName ObjectiveId; // ID objective ��� ����������
```

**�������������**:
- ������� ��� "talk to NPC" objectives
- �������������� ���������� ��� �������

---

### UQuestDialogueEffect_EmitEvent

**����������**: ���������� quest event (��� �������� objectives).

**��������**:
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName EventType; // "NpcTalked", "ItemAcquired", etc.

UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName NpcId; // ��� NpcTalked

UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName ItemId; // ��� ItemAcquired/ItemUsed

UPROPERTY(EditAnywhere, BlueprintReadWrite)
int32 Count; // ���������� (��� ���������)
```

**�������������**:
- ��� event-based objectives
- ����� ������������� �� ������� � ������������� �����������

---

## 3?? ������������� � Dialog System

### � Dialog Asset (Editor)

#### ��� 1: ������� DialogueNode

1. �������� ��� **DialogueDataAsset**
2. �������� ������ **DialogueNode**
3. � **Details** ������ ������� **Effects**

#### ��� 2: �������� Quest Effect

```
Effects:
  [0]: UQuestDialogueEffect_StartQuest
    ?? Quest Id: "Q_FirstMeeting"
    ?? Quest Acceptor: [None] ? ���� �� ���������
```

#### ��� 3: ���������

**������!** ��� ��������� ���� ���� ����� ���������� �������������.

---

### � Blueprint DialogueNode

```blueprint
Event OnNodeEntered
?? For Each (Effects)
?   ?? Cast to QuestDialogueEffect_StartQuest
?       ?? Execute (WorldContext: Self)
?? [Quest started automatically]
```

---

## 4?? �������

### ������ 1: ������� ����� ��� �������

**��������**: ����� ������� ������� � NPC ? ������������� ���������� �����.

**DialogueNode ���������**:
```
Node: "FirstMeeting"
?? Speaker: "NPC_Elder"
?? DialogueText: "Welcome, traveler! I need your help."
?? Effects:
    ?? [0] UQuestDialogueEffect_StartQuest
 ?? QuestId: "Q_HelpElder"
```

**QuestAsset ���������**:
```
Quest: QA_HelpElder
?? Quest ID: Q_HelpElder
?? Title: "Help the Elder"
?? Phase 0:
    ?? Objective 0:
        ?? Objective ID: Obj_TalkMore
        ?? Title: "Talk to elder again"
        ?? Condition:
     ?? Type: NpcTalked
            ?? String Param: NPC_Elder
?? Int Param: 2 ? ����� 2 ���� ����������
```

---

### ������ 2: ���������� objective ����� ������

**��������**: � ������ ����� "������� ����������", ���� �� objectives - "���������� � ���������".

**DialogueNode ���������**:
```
Node: "MerchantInfo"
?? Speaker: "NPC_Merchant"
?? DialogueText: "I saw suspicious people near the forest..."
?? Condition: Quest.Active("Q_GatherInfo") ? ������ ���� ����� �������
?? Effects:
    ?? [0] UQuestDialogueEffect_CompleteObjective
     ?? QuestId: "Q_GatherInfo"
        ?? ObjectiveId: "Obj_TalkMerchant"
```

**QuestAsset ���������**:
```
Quest: QA_GatherInfo
?? Quest ID: Q_GatherInfo
?? Phase 0:
 ?? Objective 0: Obj_TalkMerchant
    ?   ?? Title: "Talk to merchant"
    ?   ?? Style: EventBased
    ?? Objective 1: Obj_TalkGuard
   ?? Title: "Talk to guard"
     ?? Style: EventBased
```

---

### ������ 3: Event-based ��������

**��������**: ����� ����� ������� NPC ? quest objective �����������.

**DialogueNode ���������**:
```
Node: "GiveItem"
?? Speaker: "NPC_Blacksmith"
?? DialogueText: "Ah, the iron ore! Thank you!"
?? Condition: HasItem("IronOre")
?? Effects:
    ?? [0] UDialogueEffect_ModifyInventory
    ?   ?? ItemId: "IronOre"
    ?   ?? DeltaCount: -1 ? ������ �������
    ?? [1] UQuestDialogueEffect_EmitEvent
        ?? EventType: "ItemUsed"
        ?? ItemId: "IronOre"
        ?? Count: 1
```

**QuestAsset ���������**:
```
Quest: QA_HelpBlacksmith
?? Quest ID: Q_HelpBlacksmith
?? Phase 0:
    ?? Objective 0:
        ?? Objective ID: Obj_BringOre
        ?? Title: "Bring iron ore to blacksmith"
        ?? Style: EventBased
        ?? Condition:
 ?? Type: ItemUsed
            ?? String Param: IronOre
            ?? Int Param: 1
```

---

## 5?? Blueprint Setup

### � DialogueRunner

**���� ����������� custom DialogueRunner**:

```blueprint
Event OnNodeEntered (Node: UDialogueNode)
?? Get Effects (from Node)
?? For Each (Effect)
?   ?? Cast to QuestDialogueEffect_StartQuest
?   ?   ?? Success? ? Execute (WorldContext: Self)
?   ?? Cast to QuestDialogueEffect_CompleteObjective
?   ?   ?? Success? ? Execute (WorldContext: Self)
?   ?? Cast to QuestDialogueEffect_EmitEvent
?       ?? Success? ? Execute (WorldContext: Self, Instigator: Player)
?? Continue dialogue...
```

**���� ����������� ���������� DialogueRunner**:
- ������ ������ �� �����!
- Effects ����������� ������������� ����� `UDialogueEffectExecutor`

---

## 6?? ���������� �������

### �������� custom Quest Effect

#### ��� 1: ������� �����

```cpp
// MyQuestEffect.h
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "My Quest Effect"))
class UMyQuestDialogueEffect : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName MyParameter;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void Execute(UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "Quest")
    FText GetDisplayText() const;
};
```

#### ��� 2: ����������� Execute()

```cpp
// MyQuestEffect.cpp
void UMyQuestDialogueEffect::Execute(UObject* WorldContextObject)
{
    // �������� Quest Subsystem
    UQuestSubsystem* QuestSys = UQuestSubsystem::Get(WorldContextObject);
    if (!QuestSys) return;

    // ���� ������
    // ...
}
```

#### ��� 3: ������������ � Dialog System

```
DialogueNode ? Effects ? Add:
  ?? UMyQuestDialogueEffect
      ?? MyParameter: "Value"
```

---

## 7?? Troubleshooting

### ? ����� �� �����������

**�������**:
1. ��������� `QuestId` ��������� � Quest Asset
2. ��������� Quest Subsystem ���������������
3. �������� Output Log ? ������ ����:
   ```
   LogTemp: Quest 'Q_HelpElder' started from dialogue
   ```

---

### ? Objective �� �����������

**�������**:
1. ��������� ����� ������� (`IsQuestActive()`)
2. ��������� `QuestId` � `ObjectiveId` ����������
3. ��������� objective �� ��� ��������

---

### ? Events �� ��������

**�������**:
1. ��������� `EventType` ���������� ("NpcTalked", "ItemAcquired", etc.)
2. ��������� `NpcId` ��� `ItemId` ��������� � Quest Condition
3. ��������� Event Bus ���������������

---

## 8?? Best Practices

### ? DO:

1. **����������� EmitEvent** ��� ��������:
   ```
   UQuestDialogueEffect_EmitEvent ������ CompleteObjective
   ```
   ����� ��� ����� ��� ������ � ��������.

2. **���������� �������** � DialogueNode:
   ```
 Condition: Quest.Active("Q_MyQuest")
   ```
   ����� ������ ���������� ������ ����� �����.

3. **���������** � custom effects:
   ```cpp
   UE_LOG(LogTemp, Log, TEXT("MyEffect executed"));
   ```

### ? DON'T:

1. **�� ����������** Quest System � Dialog System
2. **�� ���������� ������** - ����������� �������
3. **�� ���������** ��� null-checks

---

## ?? ����������

**Quest ? Dialog ���������� ����� Effects**:
- ? ������ ����������� (no hardcode)
- ? Event-driven (������ �����������)
- ? Blueprint-friendly (����� ������������)
- ? ����������� (����� ��������� �������)

**������� ����������, �� ������� �������� ������!**

---

**Quest System v2.1 + Dialog System - Perfect Integration!** ?
