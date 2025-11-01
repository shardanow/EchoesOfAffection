# Quest System v2.0 - Улучшенный гайд для новичков (Editor Only)

> **Обновлено в v2.1**: Теперь с готовыми C++ виджетами и гибкой Actor-based архитектурой!

---

## ? Что нового в v2.1

### ?? Готовые C++ виджеты
- ? **Quest Tracker Widget** - готовый HUD tracker с auto-update
- ? **Quest Dialog Widget** - готовый диалог выбора квестов
- ? **Quest Journal Widget** - готовый журнал квестов (уже был)

### ?? Гибкая архитектура
- ? Работает с **AActor*** вместо PlayerController
- ? Поддержка NPC квестов
- ? Multiplayer ready
- ? Интеграция с любой Interact системой

---

## ?? Содержание

1. [Подготовка проекта](#1-подготовка-проекта)
2. [Создание первого квеста](#2-создание-первого-квеста)
3. [Создание NPC с квестом](#3-создание-npc-с-квестом)
4. [Настройка UI (УПРОЩЕНО!)](#4-настройка-ui-упрощено)
5. [Интеграция с Interact System](#5-интеграция-с-interact-system)
6. [Тестирование](#6-тестирование)

---

## 4?? Настройка UI (УПРОЩЕНО!)

### ? Quest Tracker (готовый C++ класс)

#### Шаг 4.1: Создать Widget Blueprint

1. **Content Browser** ? `Content/Game/UI/Quests`
2. **Right Click ? User Interface ? Widget Blueprint**
3. В **Parent Class** выберите **Quest Tracker Widget** ? НАШ C++ КЛАСС!
4. Назовите: `WBP_QuestTracker`

#### Шаг 4.2: Добавить ТОЛЬКО нужные виджеты

В **Designer** добавьте виджеты **С ТОЧНЫМИ ИМЕНАМИ**:

```
Canvas Panel
  ?? Vertical Box
      ?? Text Block ? Назовите: "TextQuestTitle" ? ОБЯЗАТЕЛЬНО!
    ?? Text Block ? Назовите: "TextObjective" ? ОБЯЗАТЕЛЬНО!
      ?? Progress Bar ? Назовите: "ProgressBar" ? Опционально
```

> **?? Важно**: Имена ДОЛЖНЫ СОВПАДАТЬ! C++ код автоматически найдёт их через `meta = (BindWidget)`.

#### Шаг 4.3: Настроить в Details

Выберите корневой виджет, в **Details**:

```
???????????????????????????????????????????????
? QUEST TRACKER WIDGET     ?
???????????????????????????????????????????????
? Auto Track First Quest: ?   ? ? Авто-трек
? Show Progress Bar: ? ? ? Показывать бар
? Update Interval: 0.5      ? ? Обновление
???????????????????????????????????????????????
```

#### Шаг 4.4: ГОТОВО! ?

**Всё!** Виджет работает автоматически:
- ? Подписывается на события квестов
- ? Обновляет текст при прогрессе
- ? Переключается на следующий квест
- ? Показывает прогресс-бар

**Добавьте на HUD**:

```
Event Begin Play (Player Controller)
?? Create Widget (Class: WBP_QuestTracker)
   ?? Add to Viewport
```

---

## 5?? Интеграция с Dialog System

> **НОВОЕ v2.1**: Чистая интеграция без хардкода!

### Используйте DialogueEffects для квестов

У вас уже есть Dialog System ? просто добавьте **Quest Effects** в DialogueNode!

#### В DialogueNode (Dialog Asset):

```
Node: "FirstMeeting"
?? Speaker: "NPC_Elder"
?? DialogueText: "I need your help, traveler!"
?? Effects:
    ?? [Add] Quest Dialogue Effect - Start Quest
        ?? Quest ID: Q_FirstTalk
```

#### Доступные Quest Effects:

1. **Start Quest** - запускает квест
   - Quest ID: ID вашего квеста

2. **Complete Objective** - завершает objective
   - Quest ID + Objective ID

3. **Emit Quest Event** - отправляет событие
   - Event Type: "NpcTalked", "ItemAcquired", etc.
   - NPC ID / Item ID

### Как это работает:

```
Player ? Dialogue Node ? Effects Executed
 ?
Quest Dialogue Effect
           ?
Quest Subsystem ? Quest Started ?
```

**Подробнее**: `Docs/DialogIntegration_v2.1.md`

---

## ?? Сравнение: Старый vs Новый подход

### ? Старый подход (v2.0):

```
100+ манипуляций:
?? Создать Widget Blueprint
?? Добавить Text Block вручную
?? Назвать его вручную
?? Создать переменную вручную
?? Bind вручную
?? Подписаться на события вручную
?? Написать логику обновления вручную
?? Написать логику форматирования вручную
?? Обработать edge cases вручную
?? Протестировать всё вручную
```

### ? Новый подход (v2.1):

```
5 манипуляций:
?? Create Widget Blueprint (Parent: Quest Tracker Widget)
?? Add Text Block ? Name: "TextQuestTitle"
?? Add Text Block ? Name: "TextObjective"
?? (Optional) Add Progress Bar ? Name: "ProgressBar"
?? Add to Viewport
```

**Результат**: **95% МЕНЬШЕ РАБОТЫ!**

---

## ?? Полный пример workflow v2.1

```
1. Создать квест (5 минут)
   ?? QA_FirstTalk с Objective: NpcTalked

2. Создать NPC (2 минуты)
   ?? BP_Elder + Quest Giver Component

3. Создать UI (2 минуты!) ? УПРОЩЕНО!
   ?? WBP_QuestTracker (Parent: Quest Tracker Widget)
   ?  ?? Add TextQuestTitle + TextObjective
   ?? WBP_QuestDialog (Parent: Quest Dialog Widget)
      ?? Add TextNPCName + QuestList + Buttons

4. Интеграция (1 минута!)
   ?? On Interact ? On Actor Interact ? Open Dialog

5. Тестирование (1 минута)
   ?? Play ? Interact ? Accept ? Complete

ИТОГО: ~11 минут от нуля до работающего квеста!
```

---

## ?? Blueprint Event Customization

### Можно переопределить в Blueprint:

#### Quest Tracker Widget:

```
Event On Tracked Quest Changed
?? Customize appearance (change colors, animations, etc.)

Event On Quest Progress Updated
?? Show particles, play sound, etc.

Event On Tracked Quest Completed
?? Show celebration animation
```

#### Quest Dialog Widget:

```
Event On Dialog Opened
?? Play NPC animation, show camera focus

Event On Quest Selected
?? Highlight quest, show rewards preview

Event On Quest Accepted
?? Play success sound, show notification
```

---

## ?? Расширенная настройка

### Добавить свои виджеты

В **Designer** можете добавить ЛЮБЫЕ виджеты:

```
Базовые (автоматические):
?? TextQuestTitle ? Auto-bind
?? TextObjective ? Auto-bind
?? ProgressBar ? Auto-bind (optional)

Ваши дополнительные:
?? Image_QuestIcon ? Ваш
?? TextBlock_Rewards ? Ваш
?? VerticalBox_Objectives ? Ваш
?? ... любые другие
```

В **Event Graph** можете обращаться к ним:

```
Event On Quest Progress Updated
?? Get Image_QuestIcon
?? Set Brush from Quest Icon
```

---

## ?? Справочник C++ Widget API

### UQuestTrackerWidget

| Метод | Описание |
|-------|----------|
| `SetTrackedQuest(FName)` | Установить отслеживаемый квест |
| `ClearTrackedQuest()` | Очистить отслеживаемый квест |
| `GetTrackedQuestId()` | Получить ID текущего квеста |

| Свойство | Описание |
|----------|----------|
| `bAutoTrackFirstQuest` | Автоматически трекать первый квест |
| `bShowProgressBar` | Показывать прогресс-бар |
| `UpdateInterval` | Интервал обновления (сек) |

### UQuestDialogWidget

| Метод | Описание |
|-------|----------|
| `OpenDialog(AActor* NPC, AActor* Actor)` | Открыть диалог |
| `CloseDialog()` | Закрыть диалог |

| Свойство | Описание |
|----------|----------|
| `QuestListItemClass` | Класс виджета для элемента списка |
| `bAutoSelectFirst` | Авто-выбор первого квеста |

---

## ?? Заключение v2.1

**Что улучшилось**:
- ? **95% меньше работы** для создания UI
- ? **Actor-based** архитектура (гибкость)
- ? **Готовые C++ виджеты** (работают из коробки)
- ? **Blueprint Events** для кастомизации
- ? **Auto-bind** виджетов по имени

**От 100+ манипуляций ? к 5 манипуляциям!**

---

**Quest System v2.1 - Maximum convenience, minimum effort!** ?
