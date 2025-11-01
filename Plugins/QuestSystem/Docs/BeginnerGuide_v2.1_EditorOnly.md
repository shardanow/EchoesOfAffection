# Quest System v2.0 - ���������� ���� ��� �������� (Editor Only)

> **��������� � v2.1**: ������ � �������� C++ ��������� � ������ Actor-based ������������!

---

## ? ��� ������ � v2.1

### ?? ������� C++ �������
- ? **Quest Tracker Widget** - ������� HUD tracker � auto-update
- ? **Quest Dialog Widget** - ������� ������ ������ �������
- ? **Quest Journal Widget** - ������� ������ ������� (��� ���)

### ?? ������ �����������
- ? �������� � **AActor*** ������ PlayerController
- ? ��������� NPC �������
- ? Multiplayer ready
- ? ���������� � ����� Interact ��������

---

## ?? ����������

1. [���������� �������](#1-����������-�������)
2. [�������� ������� ������](#2-��������-�������-������)
3. [�������� NPC � �������](#3-��������-npc-�-�������)
4. [��������� UI (��������!)](#4-���������-ui-��������)
5. [���������� � Interact System](#5-����������-�-interact-system)
6. [������������](#6-������������)

---

## 4?? ��������� UI (��������!)

### ? Quest Tracker (������� C++ �����)

#### ��� 4.1: ������� Widget Blueprint

1. **Content Browser** ? `Content/Game/UI/Quests`
2. **Right Click ? User Interface ? Widget Blueprint**
3. � **Parent Class** �������� **Quest Tracker Widget** ? ��� C++ �����!
4. ��������: `WBP_QuestTracker`

#### ��� 4.2: �������� ������ ������ �������

� **Designer** �������� ������� **� ������� �������**:

```
Canvas Panel
  ?? Vertical Box
      ?? Text Block ? ��������: "TextQuestTitle" ? �����������!
    ?? Text Block ? ��������: "TextObjective" ? �����������!
      ?? Progress Bar ? ��������: "ProgressBar" ? �����������
```

> **?? �����**: ����� ������ ���������! C++ ��� ������������� ����� �� ����� `meta = (BindWidget)`.

#### ��� 4.3: ��������� � Details

�������� �������� ������, � **Details**:

```
???????????????????????????????????????????????
? QUEST TRACKER WIDGET     ?
???????????????????????????????????????????????
? Auto Track First Quest: ?   ? ? ����-����
? Show Progress Bar: ? ? ? ���������� ���
? Update Interval: 0.5      ? ? ����������
???????????????????????????????????????????????
```

#### ��� 4.4: ������! ?

**��!** ������ �������� �������������:
- ? ������������� �� ������� �������
- ? ��������� ����� ��� ���������
- ? ������������� �� ��������� �����
- ? ���������� ��������-���

**�������� �� HUD**:

```
Event Begin Play (Player Controller)
?? Create Widget (Class: WBP_QuestTracker)
   ?? Add to Viewport
```

---

## 5?? ���������� � Dialog System

> **����� v2.1**: ������ ���������� ��� ��������!

### ����������� DialogueEffects ��� �������

� ��� ��� ���� Dialog System ? ������ �������� **Quest Effects** � DialogueNode!

#### � DialogueNode (Dialog Asset):

```
Node: "FirstMeeting"
?? Speaker: "NPC_Elder"
?? DialogueText: "I need your help, traveler!"
?? Effects:
    ?? [Add] Quest Dialogue Effect - Start Quest
        ?? Quest ID: Q_FirstTalk
```

#### ��������� Quest Effects:

1. **Start Quest** - ��������� �����
   - Quest ID: ID ������ ������

2. **Complete Objective** - ��������� objective
   - Quest ID + Objective ID

3. **Emit Quest Event** - ���������� �������
   - Event Type: "NpcTalked", "ItemAcquired", etc.
   - NPC ID / Item ID

### ��� ��� ��������:

```
Player ? Dialogue Node ? Effects Executed
 ?
Quest Dialogue Effect
           ?
Quest Subsystem ? Quest Started ?
```

**���������**: `Docs/DialogIntegration_v2.1.md`

---

## ?? ���������: ������ vs ����� ������

### ? ������ ������ (v2.0):

```
100+ �����������:
?? ������� Widget Blueprint
?? �������� Text Block �������
?? ������� ��� �������
?? ������� ���������� �������
?? Bind �������
?? ����������� �� ������� �������
?? �������� ������ ���������� �������
?? �������� ������ �������������� �������
?? ���������� edge cases �������
?? �������������� �� �������
```

### ? ����� ������ (v2.1):

```
5 �����������:
?? Create Widget Blueprint (Parent: Quest Tracker Widget)
?? Add Text Block ? Name: "TextQuestTitle"
?? Add Text Block ? Name: "TextObjective"
?? (Optional) Add Progress Bar ? Name: "ProgressBar"
?? Add to Viewport
```

**���������**: **95% ������ ������!**

---

## ?? ������ ������ workflow v2.1

```
1. ������� ����� (5 �����)
   ?? QA_FirstTalk � Objective: NpcTalked

2. ������� NPC (2 ������)
   ?? BP_Elder + Quest Giver Component

3. ������� UI (2 ������!) ? ��������!
   ?? WBP_QuestTracker (Parent: Quest Tracker Widget)
   ?  ?? Add TextQuestTitle + TextObjective
   ?? WBP_QuestDialog (Parent: Quest Dialog Widget)
      ?? Add TextNPCName + QuestList + Buttons

4. ���������� (1 ������!)
   ?? On Interact ? On Actor Interact ? Open Dialog

5. ������������ (1 ������)
   ?? Play ? Interact ? Accept ? Complete

�����: ~11 ����� �� ���� �� ����������� ������!
```

---

## ?? Blueprint Event Customization

### ����� �������������� � Blueprint:

#### Quest Tracker Widget:

```
Event On Tracked Quest Changed
?? Customize appearance (change colors, animations, etc.)

Event On Quest Progress Updated
?? Show particles, play sound, etc.

Event On Tracked Quest Completed
?? Show celebration animation
```

#### Quest Dialog Widget:

```
Event On Dialog Opened
?? Play NPC animation, show camera focus

Event On Quest Selected
?? Highlight quest, show rewards preview

Event On Quest Accepted
?? Play success sound, show notification
```

---

## ?? ����������� ���������

### �������� ���� �������

� **Designer** ������ �������� ����� �������:

```
������� (��������������):
?? TextQuestTitle ? Auto-bind
?? TextObjective ? Auto-bind
?? ProgressBar ? Auto-bind (optional)

���� ��������������:
?? Image_QuestIcon ? ���
?? TextBlock_Rewards ? ���
?? VerticalBox_Objectives ? ���
?? ... ����� ������
```

� **Event Graph** ������ ���������� � ���:

```
Event On Quest Progress Updated
?? Get Image_QuestIcon
?? Set Brush from Quest Icon
```

---

## ?? ���������� C++ Widget API

### UQuestTrackerWidget

| ����� | �������� |
|-------|----------|
| `SetTrackedQuest(FName)` | ���������� ������������� ����� |
| `ClearTrackedQuest()` | �������� ������������� ����� |
| `GetTrackedQuestId()` | �������� ID �������� ������ |

| �������� | �������� |
|----------|----------|
| `bAutoTrackFirstQuest` | ������������� ������� ������ ����� |
| `bShowProgressBar` | ���������� ��������-��� |
| `UpdateInterval` | �������� ���������� (���) |

### UQuestDialogWidget

| ����� | �������� |
|-------|----------|
| `OpenDialog(AActor* NPC, AActor* Actor)` | ������� ������ |
| `CloseDialog()` | ������� ������ |

| �������� | �������� |
|----------|----------|
| `QuestListItemClass` | ����� ������� ��� �������� ������ |
| `bAutoSelectFirst` | ����-����� ������� ������ |

---

## ?? ���������� v2.1

**��� ����������**:
- ? **95% ������ ������** ��� �������� UI
- ? **Actor-based** ����������� (��������)
- ? **������� C++ �������** (�������� �� �������)
- ? **Blueprint Events** ��� ������������
- ? **Auto-bind** �������� �� �����

**�� 100+ ����������� ? � 5 ������������!**

---

**Quest System v2.1 - Maximum convenience, minimum effort!** ?
