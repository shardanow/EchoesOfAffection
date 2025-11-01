# Quest Tracker Widget - Step-by-Step Troubleshooting

## ?? ��������: ������ ���������� ��������� ����� ������ �������

---

## ? ������� �����������

### ��� 1: ����������������� ������

**� ������� debug ����������� � ���!**

```powershell
# ���������������� ������
Build.bat EchoesOfAffectionEditor Win64 Development
```

### ��� 2: ��������� Output Log

**��������� ���� � ��������� Output Log**:

```
// ? ������ (������ ����):
LogTemp: QuestSubsystem v2.0: Initialized successfully
LogTemp: QuestTrackerWidget: NativeConstruct called
LogTemp: QuestTrackerWidget: Quest Subsystem found!
LogTemp: QuestTrackerWidget: Found 1 active quests
LogTemp: QuestTrackerWidget: Auto-tracking quest 'Q_TestQuest'
LogTemp: QuestTrackerWidget: Set title to 'Test Quest'
LogTemp: QuestTrackerWidget: Set objective to 'Talk to NPC (0/1)'

// ? ����� (��������):
LogTemp: QuestTrackerWidget: Quest Subsystem NOT FOUND!
LogTemp: QuestTrackerWidget: Found 0 active quests
LogTemp: QuestTrackerWidget: No active quests to track
LogTemp: QuestTrackerWidget: TextQuestTitle widget is NULL!
```

---

## ?? Fix 1: Quest Subsystem �� ������

### �������:
������ �������� **��** ������������� Quest Subsystem.

### �������:

**������� A: �������� ��������**

```blueprint
// � Player Controller

Event BeginPlay
?? Delay: 1.0  ? �����!
?? Create Widget (WBP_QuestTracker)
    ?? Add to Viewport
```

**������� B: ��������� ������� Subsystem**

```blueprint
Event BeginPlay
?? Custom Event: "TryCreateQuestTracker"
?
?? TryCreateQuestTracker:
    ?? Get Quest Subsystem
    ?? Is Valid?
    ?   ?? TRUE:
    ?   ?   ?? Create Widget ? Add to Viewport
    ?   ?
    ?   ?? FALSE:
    ?    ?? Delay: 0.1
    ?           ?? TryCreateQuestTracker (recursive)
```

---

## ?? Fix 2: ��� �������� �������

### ��������:

**Console**:
```
ShowActiveQuests
```

**Output**:
```
=== ACTIVE QUESTS (0) ===
(No active quests)
```

### �������:

**��������� �������� �����**:

```
Console: StartQuest Q_TestQuest
```

**��� �������� �����**:

1. Content Browser ? Right Click ? Quest System ? Quest Asset
2. ��������: `QA_TestQuest`
3. ���������:
   ```
   Quest ID: Q_TestQuest
   Title: "Test Quest"
   Phase 0:
  ?? Objective 0:
         ?? Title: "Talk to NPC"
       ?? Counters: Target Count = 1
   ```
4. Save
5. Console: `StartQuest Q_TestQuest`

---

## ?? Fix 3: BindWidget �� ��������

### �������� Output Log:

```
LogTemp: QuestTrackerWidget: TextQuestTitle widget is NULL!
```

### �������:
����� �������� � Designer **�� ���������** � C++ �����.

### �������:

**�������� WBP_QuestTracker ? Designer**:

1. �������� Text Block (��� ���������)
2. � **Details** ������:
   ```
   ????????????????????????????????
   ? Name: TextQuestTitle         ? ? ������ ���!
 ????????????????????????????????
   ```

3. �������� ������ Text Block (��� objective)
4. � **Details**:
   ```
   ????????????????????????????????
   ? Name: TextObjective          ? ? ������ ���!
   ????????????????????????????????
   ```

5. (Optional) Progress Bar:
   ```
   ????????????????????????????????
   ? Name: ProgressBar            ? ? ������ ���!
   ????????????????????????????????
   ```

**�����**:
- ? `TextQuestTitle` (���������)
- ? `Text_Quest_Title` (����������� - �������������!)
- ? `textQuestTitle` (����������� - ��������� �����!)
- ? `TextBlock_1` (����������� - ��������� ���!)

**Compile ? Save ? Test**

---

## ?? Fix 4: ������ �� ����������� ��� ���������

### ��������:

1. ��������� �����: `StartQuest Q_TestQuest`
2. �������� objective: `UpdateObjective Q_TestQuest Obj_Main 1`
3. ������ **�� �����������**?

### �������:
Events �� ��������� ��� Subsystem �� ������� �������.

### �������:

**��������� Output Log**:
```
LogTemp: QuestTrackerWidget: Update timer started (0.50 seconds)
LogTemp: QuestProgressManager: Objective 'Obj_Main': 1/10
```

���� ������ �������� �� ������ �� �����������:

**�������������� ����������**:

```blueprint
// � WBP_QuestTracker

Event Tick
?? Get Quest Tracker Widget (Self)
?? Refresh Display
```

��� �������� ������:

```blueprint
Button: "Refresh"
Event OnClicked
?? Refresh Display
```

---

## ?? Fix 5: ������ �������� �� �� �����

### ��������:

**Hierarchy**:
```
WBP_QuestTracker
?? Canvas Panel
    ?? ... (���� �������)
```

### �������:

**��������� Visibility**:

1. �������� Root Widget (Canvas Panel)
2. Details ? Visibility: **Visible** (�� Hidden!)

**��������� Z-Order**:

```blueprint
Add to Viewport
?? Z Order: 100  ? ������� �������� = ������ �����
```

**��������� Position**:

```
Canvas Panel Slot
?? Anchors: Top-Left
?? Position: X = 20, Y = 100 ? ������� ����������
?? Size: X = 300, Y = 150
```

---

## ?? Fix 6: ����� ����� �� ����� ����

### ��������:
����� ����, �� �� �����!

### �������:

**�������� ���� ������**:

```
TextQuestTitle ? Details:
?? Color and Opacity: (1, 1, 0, 1) ? Ƹ����
?? Shadow Offset: (1, 1) ? ���� ��� �������������
```

**�������� ���**:

```
Canvas Panel
?? Border (background)
    ?? Brush Color: (0, 0, 0, 0.7) ? �������������� ������
    ?? Content:
        ?? Vertical Box
    ?? TextQuestTitle
       ?? TextObjective
```

---

## ?? ������ Checklist

```
? 1. ����������������� ������ (�������� debug ���)
? 2. ��������� Output Log:
   ? Quest Subsystem ������?
 ? Active quests �������?
   ? Widget bindings ��������?
? 3. ��������� ����� ��������:
   ? TextQuestTitle - ������ ���
   ? TextObjective - ������ ���
   ? ProgressBar - �����������
? 4. �������� �������� � BeginPlay (1 ���)
? 5. ��������� �������� �����:
   ? Console: StartQuest Q_TestQuest
 ? ��� ������� QuestAsset
? 6. ��������� ���������:
   ? Visibility: Visible
   ? Z-Order: 100
   ? Color: Ƹ���� (�����������)
? 7. ��������� ����������:
   ? UpdateObjective ? ������ �����������?
```

---

## ?? Quick Test

**������ ���� �� 2 ������**:

```
1. Console: ResetQuests
2. Restart Level
3. Console: StartQuest Q_TestQuest
4. Output Log: "QuestTrackerWidget: Auto-tracking quest 'Q_TestQuest'"
5. �����: ������ �������� "Test Quest" / "Objective Title"
6. Console: UpdateObjective Q_TestQuest Obj_Main 5
7. �����: ������ ���������� "5/10"
8. Console: CompleteQuest Q_TestQuest
9. �����: ������ �������� "No active quest" (��� ��������� �����)
```

**���� �� ��������** ? ? ������ �������� ���������!

**���� �� ��������** ? ��������� Output Log ��� ������.

---

## ?? Debug Widget (������������)

���� ������ �� ��������, �������� ��������� Debug Widget:

```blueprint
// WBP_QuestTrackerDebug

Canvas Panel
?? Vertical Box
    ?? Text: "=== QUEST TRACKER DEBUG ==="
    ?
    ?? Text Block: "SubsystemStatus"
    ?   ?? Bind: Get Quest Subsystem ? Is Valid? ? "Found" : "NOT FOUND"
    ?
    ?? Text Block: "ActiveQuestsCount"
    ?   ?? Bind: Get Active Quests ? Length
    ?
    ?? Text Block: "TrackedQuestId"
    ?   ?? Bind: Get Tracked Quest Id ? To String
    ?
    ?? Button: "Refresh"
        ?? OnClicked: Refresh Display
```

**������� �Ѩ** ��� ���������� � �������� �������!

---

## ?? ���� ������ �� �������

**�������� ��� Output Log**:

```
1. ��������� ����
2. Output Log ? Filter: "QuestTracker"
3. Copy All
4. �������� ����
```

� ����� ��� ��������! ??

---

**Quest Tracker Troubleshooting Complete!** ????
