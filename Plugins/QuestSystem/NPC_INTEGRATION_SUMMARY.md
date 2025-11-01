# Quest System - ����������� �������� �������

## ? ���������� ����������� �����������!

### ��� �������:

1. **UQuestGiverComponent** - ��������� ��� NPC � ��������, ������ ������
2. **������ ������������** �� ���������� � NPC
3. **Blueprint-friendly API** ��� ������ � quest givers

---

## ��� ��� ��������:

### 1. Component-Based ������ (������������ � �������)

```
NPC Actor (BP_NPC_Blacksmith)
?? Mesh Component
?? Quest Giver Component ? ����� ���������!
?  ?? NPC ID: "Blacksmith"
?  ?? Available Quests: [DA_Quest_RepairTools, DA_Quest_ForgeWeapon]
?  ?? Quest Icons (Available, InProgress, Complete)
?  ?? Events (OnQuestAccepted, OnQuestTurnedIn)
?? Box Collision (��� ��������������)
```

### 2. �������� � ���������:

**��� 1: ������� BP NPC**
- ��������, `BP_NPC_Blacksmith`

**��� 2: �������� Quest Giver Component**
```
Components Panel ? Add Component ? Quest Giver Component
```

**��� 3: ��������� � Details**
```
Quest Giver Component:
  NPC ID: Blacksmith (����� �������� Auto Assign)
  Available Quests:
    [0]: /Game/Quests/DA_Quest_RepairHammer
    [1]: /Game/Quests/DA_Quest_ForgeWeapon
```

**��� 4: � Quest Asset ������� Quest Giver**
```
DA_Quest_RepairHammer:
  Quest Giver ID: Blacksmith
  Quest Location: Village_Smithy
```

---

## ������������ ����� �������:

### ? ���������� ��������� � ���������
- Drag & Drop quest assets �� NPC
- ����� ����� ����� ������ � ����
- �� ����� ������ ���

### ? ������������������
- ���� ��������� ? ����� �����
- NPC, �������, �����, ����� - �� ����� ������ ������

### ? �������������� ����������
- ��������� ��� ��������� dependencies
- ���������� ������ ��������� ������
- �������� completed non-repeatable

### ? ����������
- �������������� ������ ��� NPC
- "!" - �������� ����� �����
- "?" - ����� � ��������
- "?" (������) - ����� ����� �����

---

## ������� �������������:

### � Blueprint NPC:

```blueprint
Event OnPlayerPressE:
  ? Get Component Quest Giver Component
  ? Get Available Quests For Player (Player Controller)
  ? For Each Quest:
 ? Show in UI List
  
  ? On UI Quest Clicked:
    ? Quest Giver Component ? Accept Quest
```

### ������������ ���������� ������:

```blueprint
Event OnStoryEvent:
  ? Get Actor BP_Elder
  ? Get Component Quest Giver Component
  ? Available Quests.Add(DA_Quest_SecretMission)
  ? Reload Quests
```

### �������� ������� �������:

```blueprint
Event Tick:
  ? Get Component Quest Giver Component
  ? Has Quest Interaction (Player)
  ? Branch:
      True: Show "E to Talk" prompt
   False: Hide prompt
```

---

## ���������� � ������� ���������:

### � Dialog System:
```
Dialog Node: "Hello!"
  Choices:
    ? "Tell me about quests"
      ? Open Quest List from Quest Giver Component
```

### � UI:
```
WBP_QuestDialog:
  Construct:
    ? Get Quest Giver from Target NPC
    ? Populate Quest List
```

### � Save System:
```
Quest completion ������������� ����������� ����� UQuestSubsystem
```

---

## ������������:

������ �����������:
- **[NPCIntegration.md](NPCIntegration.md)** - ��������� ������������
- **[QuickStart.md](QuickStart.md)** - ������� ����� � ��������
- **[Examples.md](Examples.md)** - ������� �������

---

## ��� ������?

1. **�������� NPC** � Quest Giver Component
2. **��������� Available Quests** � Details
3. **�������� ��������������** (E key)
4. **�������� UI** ��� ����������� �������
5. **�������������!**

---

**�� ������ � �������������!** ??

������ �� ������:
- ? ����������� ������ � NPC ����� ��������
- ? ����������� ��������� ������ �������������
- ? ���������� ���������� ����������
- ? ������������� � dialog/UI ���������
