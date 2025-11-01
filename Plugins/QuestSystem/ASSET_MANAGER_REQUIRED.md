# ?? �����: ��������� Asset Manager ��� Quest System

## ������ ��������� ������ "Quest asset not found"?

������:
```
LogTemp: Warning: QuestSubsystem: Failed to load quest asset 'Q_Tutorial_First'
LogTemp: Error: QuestSubsystem: Cannot start quest 'Q_Tutorial_First' - asset not found
```

**�������**: Unreal Engine Asset Manager **�� �����** � Quest Assets � �� ����� �� ���������.

---

## ?? ������� ������� (5 �����)

### ��� 1: ������� ����� ��� �������

**Content Browser:**
```
Content ? Right Click ? New Folder ? "Quests"
```

����� �����: `Content/Quests/` (� ������ `/Game/Quests`)

---

### ��� 2: ��������� Asset Manager

**������� A: ����� UI (�������������)**

1. **Edit ? Project Settings**
2. **Game ? Asset Manager**
3. **Primary Asset Types to Scan ? + (Add)**
4. ���������:

```
Primary Asset Type:
  Name: Quest

Asset Base Class:
  /Script/QuestSystemRuntime.QuestAsset

Has Blueprint Classes: ? (unchecked)
Is Editor Only: ? (unchecked)

Directories ? + (Add):
  Path: /Game/Quests
  Wildcards: *.uasset
  Recursive: ? (checked)

Rules:
  Priority: 1
  Chunk ID: -1
  Apply Recursively: ?
  Cook Rule: Always Cook
```

5. **Click "Apply"**
6. **Restart Editor** (�����������!)

---

**������� B: ����� DefaultEngine.ini**

�������� � `Config/DefaultEngine.ini`:

```ini
[/Script/Engine.AssetManagerSettings]
+PrimaryAssetTypesToScan=(PrimaryAssetType="Quest",AssetBaseClass="/Script/QuestSystemRuntime.QuestAsset",bHasBlueprintClasses=False,bIsEditorOnly=False,Directories=((Path="/Game/Quests")),Rules=(Priority=1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
```

������������� ��������.

---

### ��� 3: ������� Quest Asset

**Content/Quests ? Right Click:**
```
Miscellaneous ? Data Asset ? Quest Asset
Name: DA_Quest_FirstQuest
```

**���������:**
```
Quest ID: Q_Tutorial_First
Title: "My First Quest"
Description: "Learn the quest system"
```

**�����**: `Quest ID` ������������ ��� ��������!

---

### ��� 4: ���������

**��������� ���� � ��������� ���:**

? **�����:**
```
LogTemp: QuestSubsystem: Attempting to load quest 'Q_Tutorial_First'
LogTemp: QuestSubsystem: Successfully loaded quest asset 'Q_Tutorial_First' from '/Game/Quests/DA_Quest_FirstQuest'
LogTemp: QuestSubsystem: Started quest 'Q_Tutorial_First'
```

? **������ (Asset Manager �� ��������):**
```
LogTemp: Error: Quest 'Q_Tutorial_First' is NOT REGISTERED in Asset Manager!
LogTemp: Error:   ? Solution: Configure Asset Manager in Project Settings
```

---

## ?? ������������: ������ �������� (��� ������������)

���� �� ������ ����������� Asset Manager ����� ������:

### � Blueprint:

**Event Graph:**
```blueprint
Event Begin Play:
  ? Get Quest Subsystem (World Context: Self)
  ? Load Quest Asset Direct
      Asset Path: "/Game/Quests/DA_Quest_FirstQuest.DA_Quest_FirstQuest"
  ? Start Quest (Quest ID: �������� �� loaded asset)
```

**��� ����� Quest Blueprint Library:**
```blueprint
Event Begin Play:
  ? Load Asset (/Game/Quests/DA_Quest_FirstQuest)
  ? Cast to Quest Asset
  ? Get Quest ID
  ? Quest Subsystem ? Start Quest (Quest ID)
```

---

## ?? Naming Convention

**������������� ���������:**

```
Content/Quests/
?? Tutorial/
?  ?? DA_Quest_FirstSteps.uasset       (Quest ID: Q_Tutorial_FirstSteps)
?  ?? DA_Quest_TalkToNPC.uasset        (Quest ID: Q_Tutorial_TalkToNPC)
?? Main/
?  ?? DA_Quest_MainStory_01.uasset     (Quest ID: Q_MainStory_01)
?? Side/
   ?? DA_Quest_DailyBread.uasset    (Quest ID: Q_DailyBread)
   ?? DA_Quest_FindCat.uasset          (Quest ID: Q_FindCat)
```

**�������:**
- ��� �����: `DA_Quest_[Description]`
- Quest ID ������ asset: `Q_[Description]`
- Quest ID ������������ � ����/Blueprint ��� ��������

---

## ?? Debug: �������� Asset Manager

### ������� 1: Asset Audit

**Window ? Developer Tools ? Asset Audit**

������ ������:
```
Primary Assets:
  Quest:Q_Tutorial_First ? /Game/Quests/DA_Quest_FirstQuest
```

### ������� 2: Console Command

� ��������� (Output Log):
```
AssetManager.DumpLoadedAssets Quest
```

������ �������� ������������������ Quest assets.

---

## ?? ������ ������

### 1. Quest ID �� ���������

```
File: DA_Quest_MyQuest.uasset
Quest ID � asset: Q_DifferentID  ? WRONG!
```

**�������**: Quest ID ������ asset ������ ��������������� ����, ��� �� ����������� � `StartQuest()`

### 2. ����� �� �������

```
Error: Directory '/Game/Quests' does not exist
```

**�������**: ������� `Content/Quests/`

### 3. �� ������������� ��������

Asset Manager ��������� ����������� ������ ����� restart.

**�������**: File ? Exit ? Restart

---

## ? �������� Checklist

����� ������������� �������:

- [ ] ������� ����� `Content/Quests/`
- [ ] Asset Manager �������� (Project Settings)
- [ ] �������� �����������
- [ ] ������ Quest Asset � `/Game/Quests/`
- [ ] Quest ID � asset ��������� � ID � Blueprint/Code
- [ ] Quest Asset �������� ���� �� ���� Phase � Objective

---

## ?? �������������� �������

- [Quick Start Guide](QuickStart.md) - ������ ���� �� �������� ������� ������
- [NPC Integration](NPCIntegration.md) - �������� ������� � NPC
- [Examples](Examples.md) - ������� ������� �������

---

**����� ��������� Asset Manager �� ����� �������� �������������!** ?

Quest Subsystem ������������� ����� � �������� ������ �� �� Quest ID.
