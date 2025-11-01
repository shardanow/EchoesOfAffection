# Quest System v2.1 - Dialog Integration Complete! ?

## ?? ��� �������

### ? �������

- ? `UQuestDialogWidget` - �� �����, Dialog System ����� ���� UI
- ? ������� ������������ ����� ���������

### ? ���������

#### 1. Quest Dialogue Effects (������ ����������)

**�����**:
- `QuestDialogueEffects.h/.cpp` - ������� ��� Dialog System

**������**:
```cpp
UQuestDialogueEffect_StartQuest
?? ��������� ����� ��� ��������� DialogueNode
?? ��������: QuestId, QuestAcceptor

UQuestDialogueEffect_CompleteObjective
?? ��������� objective ������
?? ��������: QuestId, ObjectiveId

UQuestDialogueEffect_EmitEvent
?? ���������� quest event (NpcTalked, ItemAcquired, etc.)
?? ��������: EventType, NpcId, ItemId, Count
```

#### 2. ������������

- `DialogIntegration_v2.1.md` - ������ ���� ����������
- ������� `BeginnerGuide_v2.1_EditorOnly.md`

---

## ??? ����������� ����������

### Event-driven ������ (��� ��������)

```
????????????????????????????????????????????
? Dialog System (����������� ������)       ?
????????????????????????????????????????????
? UDialogueNode                  ?
? ?? DialogueText             ?
? ?? Conditions      ?
? ?? Effects: TArray<UDialogueEffect*>     ? ? ����������
?     ?? UDialogueEffect_ModifyAffinity    ?
?     ?? UDialogueEffect_ModifyInventory   ?
?     ?? ... (����������)   ?
????????????????????????????????????????????
         ?
         Execution ����� DialogueRunner
   ?
????????????????????????????????????????????
? Quest System ������������ ���� �������   ?
????????????????????????????????????????????
? UQuestDialogueEffect_StartQuest          ?
? ?? Execute(WorldContext)       ?
? ?? ? QuestSubsystem->StartQuest()    ?
?     ?
? UQuestDialogueEffect_CompleteObjective   ?
? ?? Execute(WorldContext)  ?
? ?? ? QuestSubsystem->CompleteObjective() ?
?             ?
? UQuestDialogueEffect_EmitEvent           ?
? ?? Execute(WorldContext, Actor)          ?
? ?? ? EventBus->EmitEvent()        ?
????????????????????????????????????????????
```

**�������� �������**:
- ? **������ �����������** - ������� �� ����� ���� � ����� ��������
- ? **�������������** - ����� ��������� ����� �������
- ? **�����������������** - Dialog Effects �������� � ������ ���������
- ? **Blueprint-friendly** - �� ������������� � ���������

---

## ?? ������������� (Quick Start)

### � Dialog System Editor

#### ��� 1: ������� DialogueNode

1. �������� ��� **DialogueDataAsset**
2. �������� ������ **DialogueNode**
3. � **Details** ������� **Effects**

#### ��� 2: �������� Quest Effect

```
Effects:
  ?? [Add] Quest Dialogue Effect - Start Quest
    ?? Quest Id: "Q_FirstMeeting"
      ?? Quest Acceptor: [None] ? ���� �� ���������
```

#### ��� 3: ���������

**������!** ��� ��������� ���� ����� ���������� �������������.

---

### ������: ����� ��� ������ �������

```
DialogueNode: "FirstMeeting"
?? Speaker: "NPC_Elder"
?? DialogueText: "Welcome, traveler! I need your help."
?? Conditions: [None]
?? Effects:
    ?? [0] UQuestDialogueEffect_StartQuest
        ?? QuestId: "Q_HelpElder"
```

```
QuestAsset: QA_HelpElder
?? Quest ID: Q_HelpElder
?? Title: "Help the Elder"
?? Phase 0:
    ?? Objective 0:
  ?? Title: "Talk to elder again"
     ?? Condition:
?? Type: NpcTalked
  ?? String Param: NPC_Elder
   ?? Int Param: 2 ? ���������� 2 ����
```

**���������**:
1. ����� ������� � NPC ? DialogueNode ��������������
2. Effect ����������� ? Quest "Q_HelpElder" �����������
3. ����� ������� ����� ? NpcTalked event ? Objective +1
4. ������ ��� ? Objective completed ? Quest completed ?

---

## ?? ��������� Quest Effects

### 1. Start Quest

**����������**: ��������� �����.

**���������**:
- `Quest Id` - ID ������ (FName)
- `Quest Acceptor` - (Optional) Actor ����������� �����

**������**:
```
Effect: Start Quest
?? Quest Id: "Q_MainStory_01"
```

---

### 2. Complete Objective

**����������**: ��������� ���������� objective.

**���������**:
- `Quest Id` - ID ������
- `Objective Id` - ID objective

**������**:
```
Effect: Complete Objective
?? Quest Id: "Q_GatherInfo"
?? Objective Id: "Obj_TalkMerchant"
```

**����� ������������**: ��� "talk to NPC" objectives.

---

### 3. Emit Quest Event

**����������**: ���������� ������� ��� quest objectives.

**���������**:
- `Event Type` - ��� ("NpcTalked", "ItemAcquired", "ItemUsed")
- `Npc Id` - ID NPC (��� NpcTalked)
- `Item Id` - ID �������� (��� Item events)
- `Count` - ����������

**������ 1 - NPC Talked**:
```
Effect: Emit Quest Event
?? Event Type: "NpcTalked"
?? Npc Id: "NPC_Merchant"
?? Count: 1
```

**������ 2 - Item Given**:
```
Effect: Emit Quest Event
?? Event Type: "ItemUsed"
?? Item Id: "IronOre"
?? Count: 1
```

---

## ?? ���������� �������

### �������� custom Quest Effect

```cpp
// MyCustomQuestEffect.h
UCLASS(BlueprintType, EditInlineNew, meta = (DisplayName = "My Quest Effect"))
class UMyCustomQuestEffect : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FName MyParameter;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void Execute(UObject* WorldContextObject)
  {
        UQuestSubsystem* QuestSys = /* Get subsystem */;
        // ���� ������...
    }

    UFUNCTION(BlueprintPure, Category = "Quest")
    FText GetDisplayText() const
    {
        return FText::FromString(TEXT("My Custom Effect"));
    }
};
```

**�������������**:
1. Compile project
2. Open DialogueNode
3. Add Effect ? **My Quest Effect**
4. Set parameters

---

## ?? ������ ���������

### Dialog System �� ������� �� Quest System

```cpp
// ? WRONG (� Dialog System):
#include "QuestSubsystem.h" // ���!

// ? CORRECT (� Quest System):
#include "Effects/DialogueEffectExecutor.h" // ������ ����������
```

Dialog System ����� ������ ��� **UDialogueEffect** (����������).

Quest System ������������ ���� ���������� �������.

### ������� ���������� Effects

Effects ����������� **� ������� ����������**:

```
Effects:
  [0] ModifyInventory (give item)
  [1] StartQuest      (start quest)
  [2] EmitEvent    (notify quest)
```

���� ����� ������ ������� - �������� �������.

---

## ?? ���������: Old vs New

### ? ������ ������ (�������):

```cpp
// � Dialog System:
if (QuestSystem && QuestId != NAME_None)
{
    QuestSystem->StartQuest(QuestId); // ������ �����������!
}
```

**��������**:
- ? Dialog ������� �� Quest
- ? ������ ��������� Quest ��� ������
- ? ������ ���������
- ? ��������� SRP

### ? ����� ������ (event-driven):

```cpp
// Dialog System:
for (UDialogueEffect* Effect : Effects)
{
    Effect->Execute(Context); // ����������!
}

// Quest System (��������):
void UQuestDialogueEffect_StartQuest::Execute(Context)
{
    QuestSystem->StartQuest(QuestId); // ����������
}
```

**������������**:
- ? ������ �����������
- ? ����� ��������� Quest System
- ? ������������� (�������� ����� �������)
- ? ���������� SRP

---

## ?? ���������� v2.1

**Quest + Dialog ����������**:
- ? **Event-driven** architecture
- ? **��� ��������** ����� ���������
- ? **Blueprint-friendly** ���������
- ? **�����������** ����� custom effects
- ? **����������������** �����������

**��� ����������� ������� �������� ������ ��� ������ ������������!**

---

**������������**:
- `DialogIntegration_v2.1.md` - ������ ����
- `BeginnerGuide_v2.1_EditorOnly.md` - ��� ��������
- `ARCHITECTURE_V2.md` - �����������

**Quest System v2.1 - Professional Integration!** ?
