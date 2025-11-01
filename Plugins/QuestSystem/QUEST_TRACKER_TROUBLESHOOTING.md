# Quest Tracker Widget - Step-by-Step Troubleshooting

## ?? Проблема: Виджет показывает дефолтный текст вместо квестов

---

## ? Чеклист диагностики

### Шаг 1: Перекомпилировать проект

**Я добавил debug логирование в код!**

```powershell
# Перекомпилируйте проект
Build.bat EchoesOfAffectionEditor Win64 Development
```

### Шаг 2: Проверить Output Log

**Запустите игру и проверьте Output Log**:

```
// ? ХОРОШО (должно быть):
LogTemp: QuestSubsystem v2.0: Initialized successfully
LogTemp: QuestTrackerWidget: NativeConstruct called
LogTemp: QuestTrackerWidget: Quest Subsystem found!
LogTemp: QuestTrackerWidget: Found 1 active quests
LogTemp: QuestTrackerWidget: Auto-tracking quest 'Q_TestQuest'
LogTemp: QuestTrackerWidget: Set title to 'Test Quest'
LogTemp: QuestTrackerWidget: Set objective to 'Talk to NPC (0/1)'

// ? ПЛОХО (проблема):
LogTemp: QuestTrackerWidget: Quest Subsystem NOT FOUND!
LogTemp: QuestTrackerWidget: Found 0 active quests
LogTemp: QuestTrackerWidget: No active quests to track
LogTemp: QuestTrackerWidget: TextQuestTitle widget is NULL!
```

---

## ?? Fix 1: Quest Subsystem не найден

### Причина:
Виджет создаётся **ДО** инициализации Quest Subsystem.

### Решение:

**Вариант A: Добавить задержку**

```blueprint
// В Player Controller

Event BeginPlay
?? Delay: 1.0  ? ВАЖНО!
?? Create Widget (WBP_QuestTracker)
    ?? Add to Viewport
```

**Вариант B: Проверять наличие Subsystem**

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

## ?? Fix 2: Нет активных квестов

### Проверка:

**Console**:
```
ShowActiveQuests
```

**Output**:
```
=== ACTIVE QUESTS (0) ===
(No active quests)
```

### Решение:

**Запустите тестовый квест**:

```
Console: StartQuest Q_TestQuest
```

**Или создайте квест**:

1. Content Browser ? Right Click ? Quest System ? Quest Asset
2. Назовите: `QA_TestQuest`
3. Настройте:
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

## ?? Fix 3: BindWidget не работает

### Проверка Output Log:

```
LogTemp: QuestTrackerWidget: TextQuestTitle widget is NULL!
```

### Причина:
Имена виджетов в Designer **НЕ СОВПАДАЮТ** с C++ кодом.

### Решение:

**Откройте WBP_QuestTracker ? Designer**:

1. Выберите Text Block (для заголовка)
2. В **Details** панели:
   ```
   ????????????????????????????????
   ? Name: TextQuestTitle         ? ? ТОЧНОЕ ИМЯ!
 ????????????????????????????????
   ```

3. Выберите второй Text Block (для objective)
4. В **Details**:
   ```
   ????????????????????????????????
   ? Name: TextObjective          ? ? ТОЧНОЕ ИМЯ!
   ????????????????????????????????
   ```

5. (Optional) Progress Bar:
   ```
   ????????????????????????????????
   ? Name: ProgressBar            ? ? ТОЧНОЕ ИМЯ!
   ????????????????????????????????
   ```

**ВАЖНО**:
- ? `TextQuestTitle` (правильно)
- ? `Text_Quest_Title` (неправильно - подчёркивания!)
- ? `textQuestTitle` (неправильно - маленькая буква!)
- ? `TextBlock_1` (неправильно - дефолтное имя!)

**Compile ? Save ? Test**

---

## ?? Fix 4: Виджет не обновляется при прогрессе

### Проверка:

1. Запустите квест: `StartQuest Q_TestQuest`
2. Обновите objective: `UpdateObjective Q_TestQuest Obj_Main 1`
3. Виджет **не обновляется**?

### Причина:
Events не подписаны или Subsystem не передаёт события.

### Решение:

**Проверьте Output Log**:
```
LogTemp: QuestTrackerWidget: Update timer started (0.50 seconds)
LogTemp: QuestProgressManager: Objective 'Obj_Main': 1/10
```

Если таймер работает но виджет не обновляется:

**Принудительное обновление**:

```blueprint
// В WBP_QuestTracker

Event Tick
?? Get Quest Tracker Widget (Self)
?? Refresh Display
```

Или добавьте кнопку:

```blueprint
Button: "Refresh"
Event OnClicked
?? Refresh Display
```

---

## ?? Fix 5: Виджет создаётся но не виден

### Проверка:

**Hierarchy**:
```
WBP_QuestTracker
?? Canvas Panel
    ?? ... (ваши виджеты)
```

### Решение:

**Проверьте Visibility**:

1. Выберите Root Widget (Canvas Panel)
2. Details ? Visibility: **Visible** (НЕ Hidden!)

**Проверьте Z-Order**:

```blueprint
Add to Viewport
?? Z Order: 100  ? Большое значение = поверх всего
```

**Проверьте Position**:

```
Canvas Panel Slot
?? Anchors: Top-Left
?? Position: X = 20, Y = 100 ? Видимые координаты
?? Size: X = 300, Y = 150
```

---

## ?? Fix 6: Текст белый на белом фоне

### Проблема:
Текст есть, но не видно!

### Решение:

**Измените цвет текста**:

```
TextQuestTitle ? Details:
?? Color and Opacity: (1, 1, 0, 1) ? Жёлтый
?? Shadow Offset: (1, 1) ? Тень для читабельности
```

**Добавьте фон**:

```
Canvas Panel
?? Border (background)
    ?? Brush Color: (0, 0, 0, 0.7) ? Полупрозрачный черный
    ?? Content:
        ?? Vertical Box
    ?? TextQuestTitle
       ?? TextObjective
```

---

## ?? Полный Checklist

```
? 1. Перекомпилировать проект (добавлен debug лог)
? 2. Проверить Output Log:
   ? Quest Subsystem найден?
 ? Active quests найдены?
   ? Widget bindings работают?
? 3. Проверить имена виджетов:
   ? TextQuestTitle - ТОЧНОЕ имя
   ? TextObjective - ТОЧНОЕ имя
   ? ProgressBar - опционально
? 4. Добавить задержку в BeginPlay (1 сек)
? 5. Запустить тестовый квест:
   ? Console: StartQuest Q_TestQuest
 ? Или создать QuestAsset
? 6. Проверить видимость:
   ? Visibility: Visible
   ? Z-Order: 100
   ? Color: Жёлтый (контрастный)
? 7. Проверить обновление:
   ? UpdateObjective ? виджет обновляется?
```

---

## ?? Quick Test

**Полный тест за 2 минуты**:

```
1. Console: ResetQuests
2. Restart Level
3. Console: StartQuest Q_TestQuest
4. Output Log: "QuestTrackerWidget: Auto-tracking quest 'Q_TestQuest'"
5. Экран: Должен показать "Test Quest" / "Objective Title"
6. Console: UpdateObjective Q_TestQuest Obj_Main 5
7. Экран: Должен обновиться "5/10"
8. Console: CompleteQuest Q_TestQuest
9. Экран: Должен показать "No active quest" (или следующий квест)
```

**Если всё работает** ? ? Виджет настроен правильно!

**Если НЕ работает** ? Проверьте Output Log для ошибок.

---

## ?? Debug Widget (альтернатива)

Если ничего не помогает, создайте временный Debug Widget:

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

**Покажет ВСЁ** что происходит в реальном времени!

---

## ?? Если ничего не помогло

**Пришлите мне Output Log**:

```
1. Запустите игру
2. Output Log ? Filter: "QuestTracker"
3. Copy All
4. Пришлите сюда
```

Я скажу где проблема! ??

---

**Quest Tracker Troubleshooting Complete!** ????
