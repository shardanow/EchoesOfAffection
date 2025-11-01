# Quest Asset Loading - Troubleshooting Guide

## Проблема: Asset Found but Failed to Load

### Симптомы:
```
LogTemp: QuestSubsystem: Attempting to load quest 'Q_Tutorial_First'
LogTemp: QuestSubsystem: Found quest asset data, loading...
LogTemp: QuestSubsystem: Asset not in memory, loading from disk...
LogTemp: Error: QuestSubsystem: Failed to load quest asset 'Q_Tutorial_First'
```

### Причины:

1. **LoadPrimaryAssets() асинхронная** (до версии 1.0.1)
   - `LoadPrimaryAssets()` не блокирует поток
   - Asset не успевает загрузиться перед `GetPrimaryAssetObject()`

2. **Неправильный путь в Asset Manager**
- Asset находится не в указанном пути
   - Путь в Project Settings не совпадает с реальным расположением

---

## ? Решение (версия 1.0.1+)

### Используется StreamableManager.LoadSynchronous():

```cpp
FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
FSoftObjectPath AssetPath = AssetData.GetSoftObjectPath();
UQuestAsset* QuestAsset = Cast<UQuestAsset>(StreamableManager.LoadSynchronous(AssetPath, false));
```

**Преимущества**:
- ? Блокирует поток до полной загрузки
- ? Гарантирует что asset загружен
- ? Работает с любым путём
- ? Fallback на TryLoad() если нужно

---

## Проверка настройки Asset Manager

### 1. ? Asset Manager настроен?
```
Edit ? Project Settings ? Asset Manager
? Quest type добавлен?
? Path указан правильно?
```

**Пример правильной настройки**:
```
Primary Asset Type: Quest
Asset Base Class: /Script/QuestSystemRuntime.QuestAsset
Directories:
  Path: /Game/Game/Core/Subsystems/QuestSystem
  (или /Game/Quests если используете стандартную структуру)
```

### 2. ? Quest ID правильный?
```
Open DA_Quest_First
? Quest ID = Q_Tutorial_First
```

### 3. ? GetPrimaryAssetId() реализован?
```cpp
FPrimaryAssetId UQuestAsset::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(FPrimaryAssetType("Quest"), QuestId);
}
```

### 4. ? Asset Manager обновлён?
```
Restart Editor после изменений в Project Settings
```

---

## Логи для диагностики

### ? Успешная загрузка (v1.0.1+):
```
LogTemp: QuestSubsystem: Attempting to load quest 'Q_Tutorial_First'
LogTemp: QuestSubsystem: Found quest asset data at '/Game/Game/Core/Subsystems/QuestSystem/DA_Quest_First.DA_Quest_First', loading...
LogTemp: QuestSubsystem: Asset not in memory, loading from disk...
LogTemp: QuestSubsystem: Successfully loaded quest asset 'Q_Tutorial_First'
LogTemp: QuestSubsystem: Started quest 'Q_Tutorial_First'
```

### ? Quest не зарегистрирован:
```
LogTemp: Error: Quest 'Q_Tutorial_First' is NOT REGISTERED in Asset Manager!
```
**Решение**: Настроить Asset Manager

### ? Asset не найден (старая версия):
```
LogTemp: Found quest asset data, loading...
LogTemp: Error: Failed to load quest asset 'Q_Tutorial_First'
```
**Решение**: Обновить до версии 1.0.1+ с StreamableManager

---

## Version History

### v1.0.1 - 31.10.2025
- ? **Fixed**: Используется `StreamableManager.LoadSynchronous()`
- ? **Fixed**: Добавлен fallback на `TryLoad()`
- ? **Improved**: Расширенные логи с путями и классами

### v1.0.0
- ? Использовался `LoadPrimaryAssets()` (асинхронная)
- ? Asset не успевал загрузиться

---

## Advanced: Async Loading

Для production рекомендуется async loading:

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

**Проблема решена** в версии **1.0.1** заменой асинхронной загрузки на синхронную через StreamableManager.

Система теперь корректно:
1. Находит quest в Asset Manager ?
2. Загружает с диска синхронно ?
3. Кеширует для быстрого доступа ?
4. Даёт понятные ошибки ?

---

**Дата исправления**: 31.10.2025  
**Версия плагина**: 1.0.1  
**UE Version**: 5.6
