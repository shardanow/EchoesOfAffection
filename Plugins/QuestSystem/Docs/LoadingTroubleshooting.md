# Quest Asset Loading - Troubleshooting Guide

## ��������: Asset Found but Failed to Load

### ��������:
```
LogTemp: QuestSubsystem: Attempting to load quest 'Q_Tutorial_First'
LogTemp: QuestSubsystem: Found quest asset data, loading...
LogTemp: QuestSubsystem: Asset not in memory, loading from disk...
LogTemp: Error: QuestSubsystem: Failed to load quest asset 'Q_Tutorial_First'
```

### �������:

1. **LoadPrimaryAssets() �����������** (�� ������ 1.0.1)
   - `LoadPrimaryAssets()` �� ��������� �����
   - Asset �� �������� ����������� ����� `GetPrimaryAssetObject()`

2. **������������ ���� � Asset Manager**
- Asset ��������� �� � ��������� ����
   - ���� � Project Settings �� ��������� � �������� �������������

---

## ? ������� (������ 1.0.1+)

### ������������ StreamableManager.LoadSynchronous():

```cpp
FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
FSoftObjectPath AssetPath = AssetData.GetSoftObjectPath();
UQuestAsset* QuestAsset = Cast<UQuestAsset>(StreamableManager.LoadSynchronous(AssetPath, false));
```

**������������**:
- ? ��������� ����� �� ������ ��������
- ? ����������� ��� asset ��������
- ? �������� � ����� ����
- ? Fallback �� TryLoad() ���� �����

---

## �������� ��������� Asset Manager

### 1. ? Asset Manager ��������?
```
Edit ? Project Settings ? Asset Manager
? Quest type ��������?
? Path ������ ���������?
```

**������ ���������� ���������**:
```
Primary Asset Type: Quest
Asset Base Class: /Script/QuestSystemRuntime.QuestAsset
Directories:
  Path: /Game/Game/Core/Subsystems/QuestSystem
  (��� /Game/Quests ���� ����������� ����������� ���������)
```

### 2. ? Quest ID ����������?
```
Open DA_Quest_First
? Quest ID = Q_Tutorial_First
```

### 3. ? GetPrimaryAssetId() ����������?
```cpp
FPrimaryAssetId UQuestAsset::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(FPrimaryAssetType("Quest"), QuestId);
}
```

### 4. ? Asset Manager �������?
```
Restart Editor ����� ��������� � Project Settings
```

---

## ���� ��� �����������

### ? �������� �������� (v1.0.1+):
```
LogTemp: QuestSubsystem: Attempting to load quest 'Q_Tutorial_First'
LogTemp: QuestSubsystem: Found quest asset data at '/Game/Game/Core/Subsystems/QuestSystem/DA_Quest_First.DA_Quest_First', loading...
LogTemp: QuestSubsystem: Asset not in memory, loading from disk...
LogTemp: QuestSubsystem: Successfully loaded quest asset 'Q_Tutorial_First'
LogTemp: QuestSubsystem: Started quest 'Q_Tutorial_First'
```

### ? Quest �� ���������������:
```
LogTemp: Error: Quest 'Q_Tutorial_First' is NOT REGISTERED in Asset Manager!
```
**�������**: ��������� Asset Manager

### ? Asset �� ������ (������ ������):
```
LogTemp: Found quest asset data, loading...
LogTemp: Error: Failed to load quest asset 'Q_Tutorial_First'
```
**�������**: �������� �� ������ 1.0.1+ � StreamableManager

---

## Version History

### v1.0.1 - 31.10.2025
- ? **Fixed**: ������������ `StreamableManager.LoadSynchronous()`
- ? **Fixed**: �������� fallback �� `TryLoad()`
- ? **Improved**: ����������� ���� � ������ � ��������

### v1.0.0
- ? ������������� `LoadPrimaryAssets()` (�����������)
- ? Asset �� ������� �����������

---

## Advanced: Async Loading

��� production ������������� async loading:

```cpp
void UQuestSubsystem::LoadQuestAssetAsync(FName QuestId, FOnQuestLoaded OnLoaded)
{
    FPrimaryAssetId AssetId(FPrimaryAssetType("Quest"), QuestId);
    UAssetManager& AssetManager = UAssetManager::Get();
    
    FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
    FAssetData AssetData;
    
    if (!AssetManager.GetPrimaryAssetData(AssetId, AssetData))
  {
        OnLoaded.ExecuteIfBound(nullptr);
        return;
    }
    
    FSoftObjectPath AssetPath = AssetData.GetSoftObjectPath();
    
    StreamableManager.RequestAsyncLoad(
        AssetPath,
     [this, QuestId, OnLoaded, AssetPath]()
  {
         UQuestAsset* Asset = Cast<UQuestAsset>(AssetPath.ResolveObject());
            if (Asset)
          {
    QuestAssetCache.Add(QuestId, Asset);
       }
            OnLoaded.ExecuteIfBound(Asset);
        }
    );
}
```

---

## Summary

**�������� ������** � ������ **1.0.1** ������� ����������� �������� �� ���������� ����� StreamableManager.

������� ������ ���������:
1. ������� quest � Asset Manager ?
2. ��������� � ����� ��������� ?
3. �������� ��� �������� ������� ?
4. ��� �������� ������ ?

---

**���� �����������**: 31.10.2025  
**������ �������**: 1.0.1  
**UE Version**: 5.6
