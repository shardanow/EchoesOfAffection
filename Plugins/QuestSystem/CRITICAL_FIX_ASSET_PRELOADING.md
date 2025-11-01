# ����������� �����������: Quest Asset Preloading

## ?? ��������:

Quest Assets **�� �����������** ������������� ��� �������������� �� SaveGame!

```
LogTemp: QuestSubsystem: Loaded 1 quests from save
LogTemp: QuestTrackerWidget: Quest asset not loaded yet for 'Q_Tutorial_First'
LogTemp: QuestTrackerWidget: Quest asset not loaded yet... (�����)
```

**Root Cause**: � `LoadInitialData()` ������ ����������� �� SaveGame, �� **Assets �������� �� ������������**.

---

## ? �������:

### ��������� ����: `QuestSubsystem.cpp`

**��������� � `LoadInitialData()`**:

```cpp
// Preload quest assets for active quests
TArray<FQuestSaveData> ActiveQuests = CurrentSaveGame->GetQuestsByState(EQuestState::Active);
for (const FQuestSaveData& QuestData : ActiveQuests)
{
    if (!QuestData.QuestId.IsNone())
    {
        UE_LOG(LogTemp, Log, TEXT("QuestSubsystem: Preloading asset for active quest '%s'"), *QuestData.QuestId.ToString());
      
        // Load asynchronously (no callback needed - will be cached)
  FOnQuestAssetLoaded EmptyCallback;
        AssetLoader->LoadQuestAssetAsync(QuestData.QuestId, EmptyCallback);
    }
}
```

---

## ?? ��� ������:

### 1. �������� Unreal Editor (Live Coding ��������� ����������)

**Ctrl+Alt+F11** ��� ������ �������� ��������.

### 2. ���������������� ������

```powershell
& 'D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\Build.bat' EchoesOfAffectionEditor Win64 Development "D:\Game Dev\Projects\UEProjects\EchoesOfAffection\EchoesOfAffection.uproject" -WaitMutex
```

### 3. ��������� Unreal Editor

### 4. ��������� Output Log

**������ ����**:
```
LogTemp: QuestSubsystem: Loaded 1 quests from save
LogTemp: QuestSubsystem: Preloading asset for active quest 'Q_Tutorial_First'
... (async loading) ...
LogTemp: QuestTrackerWidget: NativeConstruct called
LogTemp: QuestTrackerWidget: Found 1 active quests
LogTemp: QuestTrackerWidget: Retrying refresh for 'Q_Tutorial_First'
LogTemp: QuestTrackerWidget: Set title to 'Tutorial Quest'
LogTemp: QuestTrackerWidget: Set objective to '...'
```

---

## ?? ���������:

**������**:
```
Quest Tracker ����������:
Loading quest: Q_Tutorial_First
Loading... (forever)
```

**������**:
```
Quest Tracker ����������:
Tutorial Quest
Talk to NPC (0/1)
```

---

## ?? ���� �� ��� �� ��������:

### �������� 1: Asset Manager ��������?

```
Project Settings ? Asset Manager ? Primary Asset Types
������ ��������� QuestAsset
```

��. `Docs/AssetManagerSetup.md`

### �������� 2: Quest Asset ����������?

```
Content Browser ? Search: "QA_Tutorial_First"
�������� asset ? Quest ID ������ ���� "Q_Tutorial_First"
```

### �������� 3: Console �������

```
Console: ShowQuestInfo Q_Tutorial_First
Output: ������ �������� �����

Console: ResetQuests
Restart Level
StartQuest Q_Tutorial_First
```

---

## ?? ����� ��������:

- ? `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestSubsystem.cpp`
- ? `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/UI/QuestTrackerWidget.cpp`

---

## ?? ����������� ������:

**������ assets �� �����������**:

1. `QuestSaveManager::LoadQuestProgress()` ��������� ������ **SaveData** (FQuestSaveData)
2. SaveData �������� ������ **Quest ID**, **�� Quest Asset**
3. Quest Assets ����������� **�� demand** ����� `AssetLoader->LoadQuestAssetAsync()`
4. **��** ��� �������������� �� save ����� �� ������� `LoadQuestAssetAsync()`!

**�������**: ������������� assets ��� ���� �������� ������� ��� `LoadInitialData()`.

---

**�������� �������� ? ���������������� ? ����!** ???
