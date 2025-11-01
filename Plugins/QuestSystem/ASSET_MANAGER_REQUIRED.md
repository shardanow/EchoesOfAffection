# ?? ВАЖНО: Настройка Asset Manager для Quest System

## Почему возникает ошибка "Quest asset not found"?

Ошибка:
```
LogTemp: Warning: QuestSubsystem: Failed to load quest asset 'Q_Tutorial_First'
LogTemp: Error: QuestSubsystem: Cannot start quest 'Q_Tutorial_First' - asset not found
```

**Причина**: Unreal Engine Asset Manager **не знает** о Quest Assets и не может их загрузить.

---

## ?? Быстрое решение (5 минут)

### Шаг 1: Создать папку для квестов

**Content Browser:**
```
Content ? Right Click ? New Folder ? "Quests"
```

Папка будет: `Content/Quests/` (в движке `/Game/Quests`)

---

### Шаг 2: Настроить Asset Manager

**Вариант A: Через UI (Рекомендуется)**

1. **Edit ? Project Settings**
2. **Game ? Asset Manager**
3. **Primary Asset Types to Scan ? + (Add)**
4. Заполнить:

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
6. **Restart Editor** (обязательно!)

---

**Вариант B: Через DefaultEngine.ini**

Добавить в `Config/DefaultEngine.ini`:

```ini
[/Script/Engine.AssetManagerSettings]
+PrimaryAssetTypesToScan=(PrimaryAssetType="Quest",AssetBaseClass="/Script/QuestSystemRuntime.QuestAsset",bHasBlueprintClasses=False,bIsEditorOnly=False,Directories=((Path="/Game/Quests")),Rules=(Priority=1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
```

Перезапустить редактор.

---

### Шаг 3: Создать Quest Asset

**Content/Quests ? Right Click:**
```
Miscellaneous ? Data Asset ? Quest Asset
Name: DA_Quest_FirstQuest
```

**Настроить:**
```
Quest ID: Q_Tutorial_First
Title: "My First Quest"
Description: "Learn the quest system"
```

**ВАЖНО**: `Quest ID` используется для загрузки!

---

### Шаг 4: Проверить

**Запустить игру и проверить лог:**

? **Успех:**
```
LogTemp: QuestSubsystem: Attempting to load quest 'Q_Tutorial_First'
LogTemp: QuestSubsystem: Successfully loaded quest asset 'Q_Tutorial_First' from '/Game/Quests/DA_Quest_FirstQuest'
LogTemp: QuestSubsystem: Started quest 'Q_Tutorial_First'
```

? **Ошибка (Asset Manager не настроен):**
```
LogTemp: Error: Quest 'Q_Tutorial_First' is NOT REGISTERED in Asset Manager!
LogTemp: Error:   ? Solution: Configure Asset Manager in Project Settings
```

---

## ?? Альтернатива: Прямая загрузка (для тестирования)

Если не хотите настраивать Asset Manager прямо сейчас:

### В Blueprint:

**Event Graph:**
```blueprint
Event Begin Play:
  ? Get Quest Subsystem (World Context: Self)
  ? Load Quest Asset Direct
      Asset Path: "/Game/Quests/DA_Quest_FirstQuest.DA_Quest_FirstQuest"
  ? Start Quest (Quest ID: получить из loaded asset)
```

**Или через Quest Blueprint Library:**
```blueprint
Event Begin Play:
  ? Load Asset (/Game/Quests/DA_Quest_FirstQuest)
  ? Cast to Quest Asset
  ? Get Quest ID
  ? Quest Subsystem ? Start Quest (Quest ID)
```

---

## ?? Naming Convention

**Рекомендуемая структура:**

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

**Правило:**
- Имя файла: `DA_Quest_[Description]`
- Quest ID внутри asset: `Q_[Description]`
- Quest ID используется в коде/Blueprint для загрузки

---

## ?? Debug: Проверка Asset Manager

### Вариант 1: Asset Audit

**Window ? Developer Tools ? Asset Audit**

Должны видеть:
```
Primary Assets:
  Quest:Q_Tutorial_First ? /Game/Quests/DA_Quest_FirstQuest
```

### Вариант 2: Console Command

В редакторе (Output Log):
```
AssetManager.DumpLoadedAssets Quest
```

Должно показать зарегистрированные Quest assets.

---

## ?? Частые ошибки

### 1. Quest ID не совпадает

```
File: DA_Quest_MyQuest.uasset
Quest ID в asset: Q_DifferentID  ? WRONG!
```

**Решение**: Quest ID внутри asset должен соответствовать тому, что вы используете в `StartQuest()`

### 2. Папка не создана

```
Error: Directory '/Game/Quests' does not exist
```

**Решение**: Создать `Content/Quests/`

### 3. Не перезапустили редактор

Asset Manager изменения применяются только после restart.

**Решение**: File ? Exit ? Restart

---

## ? Итоговый Checklist

Перед тестированием квестов:

- [ ] Создана папка `Content/Quests/`
- [ ] Asset Manager настроен (Project Settings)
- [ ] Редактор перезапущен
- [ ] Создан Quest Asset в `/Game/Quests/`
- [ ] Quest ID в asset совпадает с ID в Blueprint/Code
- [ ] Quest Asset содержит хотя бы один Phase с Objective

---

## ?? Дополнительные ресурсы

- [Quick Start Guide](QuickStart.md) - Полный гайд по созданию первого квеста
- [NPC Integration](NPCIntegration.md) - Привязка квестов к NPC
- [Examples](Examples.md) - Примеры готовых квестов

---

**После настройки Asset Manager всё будет работать автоматически!** ?

Quest Subsystem автоматически найдёт и загрузит квесты по их Quest ID.
