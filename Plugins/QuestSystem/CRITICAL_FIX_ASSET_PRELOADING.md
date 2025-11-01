# КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Quest Asset Preloading

## ?? ПРОБЛЕМА:

Quest Assets **НЕ ЗАГРУЖАЮТСЯ** автоматически при восстановлении из SaveGame!

```
LogTemp: QuestSubsystem: Loaded 1 quests from save
LogTemp: QuestTrackerWidget: Quest asset not loaded yet for 'Q_Tutorial_First'
LogTemp: QuestTrackerWidget: Quest asset not loaded yet... (вечно)
```

**Root Cause**: В `LoadInitialData()` квесты загружаются из SaveGame, но **Assets остаются не загруженными**.

---

## ? РЕШЕНИЕ:

### Исправлен файл: `QuestSubsystem.cpp`

**Добавлено в `LoadInitialData()`**:

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

## ?? ЧТО ДЕЛАТЬ:

### 1. Закройте Unreal Editor (Live Coding блокирует компиляцию)

**Ctrl+Alt+F11** или просто закройте редактор.

### 2. Перекомпилируйте проект

```powershell
& 'D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\Build.bat' EchoesOfAffectionEditor Win64 Development "D:\Game Dev\Projects\UEProjects\EchoesOfAffection\EchoesOfAffection.uproject" -WaitMutex
```

### 3. Запустите Unreal Editor

### 4. Проверьте Output Log

**Должно быть**:
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

## ?? Результат:

**РАНЬШЕ**:
```
Quest Tracker показывает:
Loading quest: Q_Tutorial_First
Loading... (forever)
```

**ТЕПЕРЬ**:
```
Quest Tracker показывает:
Tutorial Quest
Talk to NPC (0/1)
```

---

## ?? Если всё ещё не работает:

### Проверка 1: Asset Manager настроен?

```
Project Settings ? Asset Manager ? Primary Asset Types
должен содержать QuestAsset
```

См. `Docs/AssetManagerSetup.md`

### Проверка 2: Quest Asset существует?

```
Content Browser ? Search: "QA_Tutorial_First"
Откройте asset ? Quest ID должен быть "Q_Tutorial_First"
```

### Проверка 3: Console команды

```
Console: ShowQuestInfo Q_Tutorial_First
Output: Должно показать квест

Console: ResetQuests
Restart Level
StartQuest Q_Tutorial_First
```

---

## ?? Файлы изменены:

- ? `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/QuestSubsystem.cpp`
- ? `Plugins/QuestSystem/Source/QuestSystemRuntime/Private/UI/QuestTrackerWidget.cpp`

---

## ?? Техническая деталь:

**Почему assets не загружались**:

1. `QuestSaveManager::LoadQuestProgress()` загружает только **SaveData** (FQuestSaveData)
2. SaveData содержит только **Quest ID**, **не Quest Asset**
3. Quest Assets загружаются **на demand** через `AssetLoader->LoadQuestAssetAsync()`
4. **НО** при восстановлении из save никто не вызывал `LoadQuestAssetAsync()`!

**Решение**: Предзагрузить assets для всех активных квестов при `LoadInitialData()`.

---

**ЗАКРОЙТЕ РЕДАКТОР ? ПЕРЕКОМПИЛИРУЙТЕ ? ТЕСТ!** ???
