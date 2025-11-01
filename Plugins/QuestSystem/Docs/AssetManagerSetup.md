# Quest System - Asset Manager Setup Guide

## Проблема: Quest Asset Not Found

Если вы видите ошибку:
```
LogTemp: Warning: QuestSubsystem: Failed to load quest asset 'Q_Tutorial_First'
LogTemp: Error: QuestSubsystem: Cannot start quest 'Q_Tutorial_First' - asset not found
```

**Причина**: Asset Manager не настроен для Primary Assets типа "Quest"

---

## Решение: Настроить Asset Manager

### Вариант 1: Через Project Settings (Рекомендуется)

1. **Edit ? Project Settings ? Game ? Asset Manager**

2. **Primary Asset Types to Scan ? Add (+)**

3. Настроить:
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

4. **Save** и **Restart Editor**

---

### Вариант 2: Через DefaultEngine.ini (Альтернатива)

Добавить в `Config/DefaultEngine.ini`:

```ini
[/Script/Engine.AssetManagerSettings]
+PrimaryAssetTypesToScan=(PrimaryAssetType="Quest",AssetBaseClass="/Script/QuestSystemRuntime.QuestAsset",bHasBlueprintClasses=False,bIsEditorOnly=False,Directories=((Path="/Game/Quests")),SpecificAssets=(),Rules=(Priority=1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
```

**Важно**: Путь `/Game/Quests` должен существовать!

---

## Альтернатива: Прямая загрузка (Временное решение)

Если не хотите настраивать Asset Manager прямо сейчас:

### В Blueprint или C++:

**Blueprint (Level Blueprint):**
```
Event Begin Play:
  ? Load Asset (DA_Quest_FirstQuest)
  ? Cast to Quest Asset
  ? Start Quest (Quest ID: получить из загруженного asset)
```

**C++ (для тестирования):**
```cpp
// Прямая загрузка по пути
FSoftObjectPath QuestPath(TEXT("/Game/Quests/DA_Quest_FirstQuest.DA_Quest_FirstQuest"));
UQuestAsset* QuestAsset = Cast<UQuestAsset>(QuestPath.TryLoad());

if (QuestAsset)
{
    // Использовать напрямую
    QuestSystem->StartQuest(QuestAsset->QuestId);
}
```

---

## Проверка настройки

### 1. Создать папку Quests

**Content Browser:**
```
Right Click ? New Folder ? "Quests"
```

Путь должен быть: `/Game/Quests` (в проекте это `Content/Quests/`)

### 2. Создать Quest Asset

```
Content/Quests ? Right Click ? Miscellaneous ? Data Asset
? Select: Quest Asset
? Name: DA_Quest_FirstQuest
```

### 3. Настроить Quest ID

**Открыть DA_Quest_FirstQuest:**
```
Quest ID: Q_Tutorial_First
Title: "My First Quest"
... (остальные параметры)
```

**ВАЖНО**: Quest ID в asset должен совпадать с ID при вызове StartQuest!

### 4. Проверить Asset Manager

**Window ? Developer Tools ? Asset Audit**

Должен показать: `/Game/Quests/DA_Quest_FirstQuest` как Primary Asset

---

## Debug: Проверка загрузки

Добавить лог в `QuestSubsystem::LoadQuestAsset`:

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

## Частые ошибки

### ? Quest ID не совпадает с именем файла

**Проблема:**
```
File: DA_Quest_FirstQuest
Quest ID в asset: Q_MyQuest  ? НЕПРАВИЛЬНО!
```

**Решение:**
Asset Manager использует Quest ID, не имя файла. Убедитесь что они совпадают или настройте правильно.

### ? Папка Quests не существует

```
Error: Directory '/Game/Quests' does not exist
```

**Решение:**
Создать папку `Content/Quests` в проекте.

### ? Asset Manager не обновился

**Решение:**
1. Перезапустить редактор
2. **Tools ? Rescan Sources**
3. **File ? Refresh All**

---

## Рекомендации

### ? Структура файлов:

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
Quest ID внутри: Q_[QuestName]

Пример:
File: DA_Quest_RepairHammer
Quest ID: Q_RepairHammer
```

---

## Итого

1. **Настроить Asset Manager** в Project Settings
2. **Создать папку** `/Game/Quests`
3. **Создать Quest Asset** с правильным Quest ID
4. **Перезапустить редактор**
5. **Протестировать** загрузку

После этого квесты будут загружаться автоматически! ?
