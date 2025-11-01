# Quest System - Asset Manager Setup Guide

## ��������: Quest Asset Not Found

���� �� ������ ������:
```
LogTemp: Warning: QuestSubsystem: Failed to load quest asset 'Q_Tutorial_First'
LogTemp: Error: QuestSubsystem: Cannot start quest 'Q_Tutorial_First' - asset not found
```

**�������**: Asset Manager �� �������� ��� Primary Assets ���� "Quest"

---

## �������: ��������� Asset Manager

### ������� 1: ����� Project Settings (�������������)

1. **Edit ? Project Settings ? Game ? Asset Manager**

2. **Primary Asset Types to Scan ? Add (+)**

3. ���������:
   ```
   Primary Asset Type: Quest
   Asset Base Class: /Script/QuestSystemRuntime.QuestAsset
   Has Blueprint Classes: false
   Is Editor Only: false
   
   Directories:
     [0]:
       Path: /Game/Quests
       Sub Path: ""
  Wildcards: ["*.uasset"]
     Recursive: true
   
   Rules:
     Priority: 1
     Chunk ID: -1
Cook Rule: Always Cook
   ```

4. **Save** � **Restart Editor**

---

### ������� 2: ����� DefaultEngine.ini (������������)

�������� � `Config/DefaultEngine.ini`:

```ini
[/Script/Engine.AssetManagerSettings]
+PrimaryAssetTypesToScan=(PrimaryAssetType="Quest",AssetBaseClass="/Script/QuestSystemRuntime.QuestAsset",bHasBlueprintClasses=False,bIsEditorOnly=False,Directories=((Path="/Game/Quests")),SpecificAssets=(),Rules=(Priority=1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
```

**�����**: ���� `/Game/Quests` ������ ������������!

---

## ������������: ������ �������� (��������� �������)

���� �� ������ ����������� Asset Manager ����� ������:

### � Blueprint ��� C++:

**Blueprint (Level Blueprint):**
```
Event Begin Play:
  ? Load Asset (DA_Quest_FirstQuest)
  ? Cast to Quest Asset
  ? Start Quest (Quest ID: �������� �� ������������ asset)
```

**C++ (��� ������������):**
```cpp
// ������ �������� �� ����
FSoftObjectPath QuestPath(TEXT("/Game/Quests/DA_Quest_FirstQuest.DA_Quest_FirstQuest"));
UQuestAsset* QuestAsset = Cast<UQuestAsset>(QuestPath.TryLoad());

if (QuestAsset)
{
    // ������������ ��������
    QuestSystem->StartQuest(QuestAsset->QuestId);
}
```

---

## �������� ���������

### 1. ������� ����� Quests

**Content Browser:**
```
Right Click ? New Folder ? "Quests"
```

���� ������ ����: `/Game/Quests` (� ������� ��� `Content/Quests/`)

### 2. ������� Quest Asset

```
Content/Quests ? Right Click ? Miscellaneous ? Data Asset
? Select: Quest Asset
? Name: DA_Quest_FirstQuest
```

### 3. ��������� Quest ID

**������� DA_Quest_FirstQuest:**
```
Quest ID: Q_Tutorial_First
Title: "My First Quest"
... (��������� ���������)
```

**�����**: Quest ID � asset ������ ��������� � ID ��� ������ StartQuest!

### 4. ��������� Asset Manager

**Window ? Developer Tools ? Asset Audit**

������ ��������: `/Game/Quests/DA_Quest_FirstQuest` ��� Primary Asset

---

## Debug: �������� ��������

�������� ��� � `QuestSubsystem::LoadQuestAsset`:

```cpp
UQuestAsset* UQuestSubsystem::LoadQuestAsset(FName QuestId)
{
    // Check cache first
    if (const TObjectPtr<UQuestAsset>* CachedAsset = QuestAssetCache.Find(QuestId))
    {
        return CachedAsset->Get();
    }

    UE_LOG(LogTemp, Log, TEXT("QuestSubsystem: Attempting to load quest '%s'"), *QuestId.ToString());

    // Try to load via Asset Manager
    FPrimaryAssetId AssetId(FPrimaryAssetType("Quest"), QuestId);
    
    UE_LOG(LogTemp, Log, TEXT("QuestSubsystem: Primary Asset ID: %s"), *AssetId.ToString());

    UAssetManager& AssetManager = UAssetManager::Get();
    
    // Check if asset is registered
    FPrimaryAssetData AssetData;
    if (!AssetManager.GetPrimaryAssetData(AssetId, AssetData))
  {
        UE_LOG(LogTemp, Error, TEXT("QuestSubsystem: Asset '%s' NOT REGISTERED in Asset Manager!"), *QuestId.ToString());
        UE_LOG(LogTemp, Error, TEXT("  Please configure Asset Manager in Project Settings"));
        return nullptr;
    }

    // Synchronous load
    UQuestAsset* QuestAsset = Cast<UQuestAsset>(AssetManager.GetPrimaryAssetObject(AssetId));
    
    if (QuestAsset)
    {
    QuestAssetCache.Add(QuestId, QuestAsset);
        UE_LOG(LogTemp, Log, TEXT("QuestSubsystem: Successfully loaded quest asset '%s'"), *QuestId.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("QuestSubsystem: Failed to load quest asset '%s'"), *QuestId.ToString());
    }

    return QuestAsset;
}
```

---

## ������ ������

### ? Quest ID �� ��������� � ������ �����

**��������:**
```
File: DA_Quest_FirstQuest
Quest ID � asset: Q_MyQuest  ? �����������!
```

**�������:**
Asset Manager ���������� Quest ID, �� ��� �����. ��������� ��� ��� ��������� ��� ��������� ���������.

### ? ����� Quests �� ����������

```
Error: Directory '/Game/Quests' does not exist
```

**�������:**
������� ����� `Content/Quests` � �������.

### ? Asset Manager �� ���������

**�������:**
1. ������������� ��������
2. **Tools ? Rescan Sources**
3. **File ? Refresh All**

---

## ������������

### ? ��������� ������:

```
Content/
?? Quests/
   ?? Tutorial/
   ?  ?? DA_Quest_FirstSteps.uasset
   ?  ?? DA_Quest_TalkToNPC.uasset
   ?? Main/
   ?  ?? DA_Quest_MainStory_01.uasset
   ?? Side/
      ?? DA_Quest_DailyBread.uasset
      ?? DA_Quest_FindCat.uasset
```

### ? Naming Convention:

```
File Name: DA_Quest_[QuestName]
Quest ID ������: Q_[QuestName]

������:
File: DA_Quest_RepairHammer
Quest ID: Q_RepairHammer
```

---

## �����

1. **��������� Asset Manager** � Project Settings
2. **������� �����** `/Game/Quests`
3. **������� Quest Asset** � ���������� Quest ID
4. **������������� ��������**
5. **��������������** ��������

����� ����� ������ ����� ����������� �������������! ?
