# Quest System - Testing Quick Start

## ?? ��� ����������� ������?

### ? ����� 1: Console Commands (�������������)

#### ��� 1: �������� QuestCheatManager

**� ����� PlayerController (Blueprint ��� C++)**:

```cpp
// MyPlayerController.h
UCLASS()
class AMyPlayerController : public APlayerController
{
  GENERATED_BODY()

public:
    AMyPlayerController();
};

// MyPlayerController.cpp
AMyPlayerController::AMyPlayerController()
{
    // Add Quest Cheat Manager
  CheatClass = UQuestCheatManager::StaticClass();
}
```

**��� � Blueprint**:
```
Class Defaults ? Cheat Class: QuestCheatManager
```

#### ��� 2: �������� Cheats

� **Project Settings**:
```
Project Settings ? Game ? Allow Cheats: ?
```

��� � �������:
```
EnableCheats
```

#### ��� 3: ������������ �������

**�������� �������** (~ ��� `):

```
// ���������� ��������
StartQuest Q_MyQuest
CompleteQuest Q_MyQuest
FailQuest Q_MyQuest

// ���������� objectives
CompleteObjective Q_MyQuest Obj_TalkToNPC
UpdateObjective Q_MyQuest Obj_Collect 5

// �������� ����������
ShowActiveQuests
ShowCompletedQuests
ShowQuestInfo Q_MyQuest

// Save/Load
SaveQuests
LoadQuests
ResetQuests      ? ������� ��� ����������!
ShowQuestSaves

// Auto-Save
EnableAutoSave 60
DisableAutoSave

// �������
EmitNpcTalked NPC_Elder
EmitItemAcquired Apple 10
```

---

## ?? ��� ��������� ����������?

### Windows:
```
C:\Users\<YourUsername>\AppData\Local\EchoesOfAffection\Saved\SaveGames\QuestSaveSlot.sav
```

### ������� ����������:

**������� 1 - Console**:
```
ResetQuests
```

**������� 2 - �������**:
1. �������� ����
2. ������� ���� `QuestSaveSlot.sav`
3. ��������� ����

**������� 3 - ���������**:
```
Win+R ? %LOCALAPPDATA%\EchoesOfAffection\Saved\SaveGames
������� QuestSaveSlot.sav
```

---

## ?? Workflow ��� ������������

### ���� 1: ����� �����

```
1. ResetQuests        ? �������� ������ ����������
2. Restart level
3. StartQuest Q_MyQuest
4. ShowActiveQuests     ? ��������� ��� ����� �������
5. (��������� �������� � ����)
6. ShowQuestInfo Q_MyQuest  ? ��������� ��������
7. CompleteQuest Q_MyQuest
8. ShowCompletedQuests  ? ��������� ����������
```

### ���� 2: ����������/��������

```
1. StartQuest Q_Test
2. UpdateObjective Q_Test Obj_Main 5
3. SaveQuests       ? ���������
4. ShowQuestSaves       ? ��������� ����������
5. Restart level
6. ShowActiveQuests     ? ����� ������ ��������������
7. ShowQuestInfo Q_Test ? �������� (5/10) �������?
```

### ���� 3: Auto-Save

```
1. EnableAutoSave 30    ? ����-���������� ������ 30 ���
2. StartQuest Q_Test
3. (��������� 30+ ������)
4. Check Output Log: "QuestSaveManager: Auto-saving..."
5. Restart level
6. ShowActiveQuests     ? ����� ������������?
```

---

## ?? Debug Output Log

### �������� ��������� ���:

**Console**:
```
Log LogTemp Verbose
```

**��� ������**:
```
LogTemp: QuestSubsystem v2.0: Initialized successfully
LogTemp: QuestSaveManager: Loaded 2 quests from save
LogTemp: Quest 'Q_Test' started from dialogue
LogTemp: Objective 'Obj_Main': 5/10
LogTemp: QuestSaveManager: Auto-saving...
```

---

## ? Quick Commands Reference

| Command | Description |
|---------|-------------|
| `StartQuest <ID>` | ������ ����� |
| `CompleteQuest <ID>` | ��������� ����� |
| `ShowActiveQuests` | �������� �������� |
| `ResetQuests` | **������� ��� ����������** |
| `ShowQuestSaves` | ���� � ����������� |
| `EnableAutoSave 60` | ����-���������� (60 ���) |
| `EmitNpcTalked <ID>` | ������� "��������� � NPC" |

---

## ?? Tips

1. **������ ��������� � `ResetQuests`** ��� ����� �����
2. **����������� `ShowQuestInfo`** ��� ��������� ����������
3. **�������� Auto-Save** ����� �� �������� ��������
4. **���������� Output Log** ��� ���������
5. **Bind F1** ��� Debug Widget (������� �������)

---

## ?? Next Steps

1. �������� `QuestCheatManager` � ��� PlayerController
2. �������� �������� �����
3. ����������� `StartQuest` ��� �������� ������
4. ��������� ���������� � `ResetQuests` ? Restart ? `ShowActiveQuests`

---

**Happy Testing!** ?
