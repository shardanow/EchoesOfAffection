# Dialogue System - Quick Start Guide

## ?? ������� ����� �� 5 �����

### ��� 1: ������� DialogueAsset

1. � Content Browser ? ��� ? `Dialogue System ? Dialogue Data Asset`
2. �������: `DA_Lianne_FirstMeeting`
3. ������� � ���������:
   - **DialogueId**: `Lianne_FirstMeeting`
   - **DisplayName**: "First meeting with Lianne"
   - **Description**: "Initial conversation when player meets Lianne"

### ��� 2: ������� ���� �������

#### � ������ **Nodes** ��������:

**Node 1: Start**
```
NodeId: Start
NodeType: NpcLine
SpeakerId: Lianne
DialogueText: "Oh, hello! I haven't seen you around here before..."
EmotionTag: Emotion.Neutral
```

**Node 2: Choice_Introduce**
```
NodeId: Choice_Introduce
NodeType: PlayerChoice
ChoiceText: "Hi, I'm new to the village"
EffectExpression: "affinity[Lianne] += 5"
```

**Node 3: Choice_Silent**
```
NodeId: Choice_Silent
NodeType: PlayerChoice
ChoiceText: "..." (stay silent)
EffectExpression: "affinity[Lianne] -= 2"
```

**Node 4: Response_Positive**
```
NodeId: Response_Positive
NodeType: NpcLine
SpeakerId: Lianne
DialogueText: "Welcome! I'm Lianne. Let me know if you need any help!"
ConditionExpression: "affinity[Lianne] >= 0"
EmotionTag: Emotion.Happy
```

**Node 5: Response_Negative**
```
NodeId: Response_Negative
NodeType: NpcLine
SpeakerId: Lianne
DialogueText: "Oh... okay then. *looks away awkwardly*"
ConditionExpression: "affinity[Lianne] < 0"
EmotionTag: Emotion.Sad
```

**Node 6: End**
```
NodeId: End
NodeType: End
```

#### � ������ **Edges** ��������:

```
Start ? Choice_Introduce (Priority: 0)
Start ? Choice_Silent (Priority: 1)

Choice_Introduce ? Response_Positive (Priority: 0)
Choice_Introduce ? Response_Negative (Priority: 1)

Choice_Silent ? Response_Negative (Priority: 0)

Response_Positive ? End
Response_Negative ? End
```

### ��� 3: ��������� NPC

1. ������� Blueprint NPC: `BP_Lianne`
2. �������� ���������: `Dialogue Component`
3. � ���������� ����������:
   - **CharacterId**: `Lianne`
   - **DisplayName**: "Lianne"
   - **DefaultDialogue**: ������� `DA_Lianne_FirstMeeting`
   - **Portrait**: ������������� �������
4. �������� ���������: `Relationship Component`

### ��� 4: ��������� UI

1. ������� Widget Blueprint: `WBP_DialogueMain`
2. ������������ ��: `DialogueWidget` (C++ parent)
3. �������� ��������:
   - Image: `SpeakerPortrait` (�����)
   - TextBlock: `SpeakerName` (��� ���������)
   - RichTextBlock: `DialogueText` (�� ������, ������������)
   - VerticalBox: `ChoicesContainer` (�����)
4. ������� Widget Blueprint: `WBP_Choice`
5. ������������ ��: `ChoiceWidget`
6. �������� ��������:
   - Button: `ChoiceButton`
   - TextBlock: `ChoiceText` (������ ������)
   - TextBlock: `HotkeyText` (����� �� ������, ���������)

### ��� 5: ��������� ��������������

� `BP_PlayerCharacter`:

```cpp
// Blueprint Graph

Event BeginPlay
  ?
[Get Subsystem: DialogueSubsystem]
  ?
[Set as member variable]

Event OnInteract (E pressed)
  ?
[Line Trace �� ������]
  ?
[Get Hit Actor ? Cast to DialogueComponent]
  ?
[If valid] ? [Call: StartDialogue(Self)]
```

### ��� 6: ����!

1. Play In Editor
2. ������� � Lianne
3. ������ E
4. ������� ������� ������
5. ��������� ��������� Affinity � Relationship Component

---

## ?? Checklist ��� ������ �������

- [ ] ������ DialogueAsset
- [ ] ��������� Nodes (������� Start + End)
- [ ] ��������� Edges (��� ���� �������)
- [ ] ��������� Persona (���� ����� AI)
- [ ] � NPC ���� DialogueComponent
- [ ] � NPC ���� RelationshipComponent
- [ ] CharacterId ��������� � Component � � Nodes (SpeakerId)
- [ ] Portrait ��������
- [ ] UI ������ �������� � Level Blueprint
- [ ] ��������� �������� (ValidateGraph())

---

## ?? ������: ����������� ������ � �����������

### ��������: "������� ��� Lianne"

```
??????????????
?   Start    ?  "Oh, is that... for me?"
??????????????
       ?
       ???????????????????????????????????????????
       ?                   ?                     ?
  ???????????        ????????????         ????????????
  ?Give Rose?        ?Give Gold ?         ?Give Nothing?
  ? +10 aff ?        ?  +5 aff  ?         ?  -5 aff  ?
  ???????????        ????????????         ????????????
       ?                   ?                     ?
       ?                   ?                     ?
  ???????????        ????????????         ????????????
  ? Shy &   ?        ? Polite   ?         ? Hurt &   ?
  ? Happy   ?        ? Thank    ?         ? Sad      ?
  ? +5 rom  ?        ?          ?         ?          ?
  ???????????        ????????????         ????????????
       ?                   ?                     ?
       ???????????????????????????????????????????
                           ?
                           ?
                      ???????????
                      ?   End   ?
                      ???????????
```

### ��� �����:

**Start:**
```
NodeId: Start
NodeType: NpcLine
DialogueText: "Oh, is that... for me? *notices item in your hand*"
EmotionTag: Emotion.Curious
```

**Choice_GiveRose:**
```
NodeId: Choice_GiveRose
NodeType: PlayerChoice
ChoiceText: "Yes, I picked this rose for you"
ConditionExpression: "hasItem(Rose)"
EffectExpression: "removeItem(Rose); affinity[Lianne] += 10; romance[Lianne] += 5; setMemory(GaveRose)"
ChoiceIcon: Icon_Rose
```

**Choice_GiveGold:**
```
NodeId: Choice_GiveGold
NodeType: PlayerChoice
ChoiceText: "Here, take some gold"
ConditionExpression: "gold >= 50"
EffectExpression: "gold -= 50; affinity[Lianne] += 5"
ChoiceIcon: Icon_Gold
```

**Choice_GiveNothing:**
```
NodeId: Choice_GiveNothing
NodeType: PlayerChoice
ChoiceText: "Actually, never mind"
EffectExpression: "affinity[Lianne] -= 5"
```

**Response_Rose:**
```
NodeId: Response_Rose
NodeType: NpcLine
DialogueText: "A rose? That's... really sweet of you! I love roses... *blushes*"
EmotionTag: Emotion.Shy
PauseAfter: 2.0
```

**Response_Gold:**
```
NodeId: Response_Gold
NodeType: NpcLine
DialogueText: "Oh, thank you! That's very kind, though you didn't have to..."
EmotionTag: Emotion.Grateful
```

**Response_Nothing:**
```
NodeId: Response_Nothing
NodeType: NpcLine
DialogueText: "Oh... I see. *looks down sadly*"
EmotionTag: Emotion.Hurt
```

---

## ?? ����������� �������

### 1. **Conditional Start Nodes**

������ ������ ������� � ����������� �� ���������:

```cpp
// � DialogueAsset
ConditionalStartNodes.Add("affinity[Lianne] >= 70", TEXT("Start_Friend"));
ConditionalStartNodes.Add("affinity[Lianne] < 0", TEXT("Start_Enemy"));
DefaultStartNodeId = TEXT("Start_Neutral");
```

**Start_Friend:**
```
"Hey! Good to see you again! How have you been?"
EmotionTag: Emotion.Happy
```

**Start_Neutral:**
```
"Hello. What can I do for you?"
EmotionTag: Emotion.Neutral
```

**Start_Enemy:**
```
"You again? What do you want?"
EmotionTag: Emotion.Angry
```

### 2. **Random Variations**

�������� ������������� � �������:

```cpp
Node_Random:
  NodeType: Random
  
Edges:
  Node_Random ? Variation_1 (Weight: 50)
  Node_Random ? Variation_2 (Weight: 30)
  Node_Random ? Variation_3 (Weight: 20)
```

**Variation_1:**
```
"The weather is nice today, isn't it?"
```

**Variation_2:**
```
"Have you been to the market yet?"
```

**Variation_3:**
```
"I heard there's a festival coming up!"
```

### 3. **Condition Chains**

����������� �������:

```cpp
ConditionExpression:
  "affinity[Lianne] >= 50 && hasItem(Rose) && time == Evening && !memory(GaveRoseToday)"
```

**������**:
- Affinity ������� 50 (������)
- � ������ ���� ����
- ������ �����
- ��� �� ����� ���� �������

### 4. **Multi-Stage Effects**

������������������ ��������:

```cpp
EffectExpression:
  "affinity[Lianne] += 10; romance[Lianne] += 5; setMemory(ConfessedFeelings); startQuest(DateWithLianne); gold += 100"
```

**��� ���������**:
1. +10 Affinity
2. +5 Romance
3. ���� ������ ����������
4. ����� �����
5. ������� 100 ������

### 5. **One-Time Events**

�������, ������� ������������ ������ ���:

```cpp
Node_FirstMeeting:
  bOneTimeOnly: true
  ConditionExpression: "!visited(Node_FirstMeeting)"
```

**����������**:
- Cutscene �������
- ��������� ��������
- ���������� �������

---

## ?? ������ � �������� � ����������

### ������� ������

| EmotionTag | �������� ���� | ���� UI | ������ |
|------------|---------------|---------|--------|
| Emotion.Neutral | Default | White | ?? |
| Emotion.Happy | Smile_01 | Yellow | ?? |
| Emotion.Sad | Frown_01 | Blue | ?? |
| Emotion.Angry | Angry_01 | Red | ?? |
| Emotion.Shy | Blush_01 | Pink | ?? |
| Emotion.Surprised | Shocked_01 | Orange | ?? |
| Emotion.Curious | Think_01 | Purple | ?? |

### ��������� Presentation Data

```cpp
Node_Example:
  Presentation:
    EmotionTag: Emotion.Shy
    Intensity: Strong
    AnimationName: Blush_Intense
    VoiceAudio: VO_Lianne_Shy_01
    PauseAfter: 1.5
    bFocusCamera: true
    CameraDistance: 150.0
```

**���������**:
- NPC ���������� (��������)
- ������ ����������� �� 150 units
- �������������� �����
- ����� ������� � ����� 1.5 ���

---

## ?? Debugging Tips

### Console Commands

```cpp
// �������� ������� Context
dialogue.dumpcontext

// �������� �������� ����
dialogue.showcurrentnode

// ���������� � ����
dialogue.jumptonode NodeId

// ���������� Affinity
dialogue.setaffinity Lianne 100

// �������� ������
dialogue.clearmemory

// Reload �������
dialogue.reload
```

### Debug Draw

� `DialogueRunner::ProcessNode()`:

```cpp
#if !UE_BUILD_SHIPPING
    if (CVarDialogueDebug.GetValueOnGameThread())
    {
        DrawDebugString(
            GetWorld(),
            NPC->GetActorLocation() + FVector(0, 0, 100),
            *CurrentNode->Data.NodeId.ToString(),
            nullptr,
            FColor::Green,
            5.0f
        );
    }
#endif
```

### Validation Errors

��������� ���������:

```cpp
TArray<FString> Errors;
bool bValid = DialogueAsset->ValidateGraph(Errors);

if (!bValid)
{
    for (const FString& Error : Errors)
    {
        UE_LOG(LogDialogue, Error, TEXT("%s"), *Error);
    }
}
```

**�������� ������**:
- "Node 'X' has no outgoing edges"
- "Edge references non-existent node 'Y'"
- "Circular dependency detected: X ? Y ? X"
- "Start node not found"

---

## ?? Performance Monitoring

### Stat Commands

```cpp
stat dialogue
stat dialogueai
stat dialogueui
```

### Profiling Markers

```cpp
SCOPE_CYCLE_COUNTER(STAT_DialogueRunner_ProcessNode);

void UDialogueRunner::ProcessNode(UDialogueNode* Node)
{
    SCOPE_CYCLE_COUNTER(STAT_DialogueRunner_ProcessNode);
    
    // ... logic ...
}
```

### Memory Tracking

```cpp
// ����� ������� ����
int32 CacheSize = NodeCache.Num() * sizeof(UDialogueNode*);
UE_LOG(LogDialogue, Log, TEXT("Node cache size: %d KB"), CacheSize / 1024);
```

---

## ?? Localization Setup

### 1. ��������� Localization Dashboard

1. `Window ? Localization Dashboard`
2. ������� ����� `Dialogue` target
3. �������� ��������: `en`, `ru`, `ja`, � �.�.

### 2. �������� ������ ��� �����������

```cpp
// � DataTable
DialogueText: NSLOCTEXT("Dialogue", "Lianne_FirstMeeting_Start", "Hello! I haven't seen you around here before...")
```

### 3. Gather & Compile

```cpp
// Command line
UnrealEditor-Cmd.exe ProjectName -Run=GatherText -Config=Config/Localization/Dialogue.ini
```

### 4. ��������� � ��������������� ������

```
Content/Localization/Dialogue/ru/Dialogue.po:

msgid "Hello! I haven't seen you around here before..."
msgstr "������! � �� ������ ���� ����� ������..."
```

---

**��������� ����**:
- ��������� `/Docs/DSL_Reference.md` ��� ������� ���������� DSL
- ��������� `/Docs/AI_Integration.md` ��� AI ���������
- ���������� `/Content/Examples/` ��� ������� ��������

**������**: 1.0
