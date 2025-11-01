# Quest System - Testing Quick Start

## ?? Как тестировать квесты?

### ? Метод 1: Console Commands (РЕКОМЕНДУЕТСЯ)

#### Шаг 1: Добавить QuestCheatManager

**В вашем PlayerController (Blueprint или C++)**:

```cpp
// MyPlayerController.h
UCLASS()
class AMyPlayerController : public APlayerController
{
  GENERATED_BODY()

public:
    AMyPlayerController();
};

// MyPlayerController.cpp
AMyPlayerController::AMyPlayerController()
{
    // Add Quest Cheat Manager
  CheatClass = UQuestCheatManager::StaticClass();
}
```

**Или в Blueprint**:
```
Class Defaults ? Cheat Class: QuestCheatManager
```

#### Шаг 2: Включить Cheats

В **Project Settings**:
```
Project Settings ? Game ? Allow Cheats: ?
```

Или в консоли:
```
EnableCheats
```

#### Шаг 3: Использовать команды

**Откройте консоль** (~ или `):

```
// Управление квестами
StartQuest Q_MyQuest
CompleteQuest Q_MyQuest
FailQuest Q_MyQuest

// Управление objectives
CompleteObjective Q_MyQuest Obj_TalkToNPC
UpdateObjective Q_MyQuest Obj_Collect 5

// Просмотр информации
ShowActiveQuests
ShowCompletedQuests
ShowQuestInfo Q_MyQuest

// Save/Load
SaveQuests
LoadQuests
ResetQuests      ? УДАЛЯЕТ ВСЕ СОХРАНЕНИЯ!
ShowQuestSaves

// Auto-Save
EnableAutoSave 60
DisableAutoSave

// События
EmitNpcTalked NPC_Elder
EmitItemAcquired Apple 10
```

---

## ?? Где находятся сохранения?

### Windows:
```
C:\Users\<YourUsername>\AppData\Local\EchoesOfAffection\Saved\SaveGames\QuestSaveSlot.sav
```

### Удалить сохранения:

**Вариант 1 - Console**:
```
ResetQuests
```

**Вариант 2 - Вручную**:
1. Закройте игру
2. Удалите файл `QuestSaveSlot.sav`
3. Запустите игру

**Вариант 3 - Проводник**:
```
Win+R ? %LOCALAPPDATA%\EchoesOfAffection\Saved\SaveGames
Удалите QuestSaveSlot.sav
```

---

## ?? Workflow для тестирования

### Тест 1: Новый квест

```
1. ResetQuests        ? Очистить старые сохранения
2. Restart level
3. StartQuest Q_MyQuest
4. ShowActiveQuests     ? Проверить что квест активен
5. (Выполнить действия в игре)
6. ShowQuestInfo Q_MyQuest  ? Проверить прогресс
7. CompleteQuest Q_MyQuest
8. ShowCompletedQuests  ? Проверить завершение
```

### Тест 2: Сохранение/Загрузка

```
1. StartQuest Q_Test
2. UpdateObjective Q_Test Obj_Main 5
3. SaveQuests       ? Сохранить
4. ShowQuestSaves       ? Проверить сохранение
5. Restart level
6. ShowActiveQuests     ? Квест должен восстановиться
7. ShowQuestInfo Q_Test ? Прогресс (5/10) сохранён?
```

### Тест 3: Auto-Save

```
1. EnableAutoSave 30    ? Авто-сохранение каждые 30 сек
2. StartQuest Q_Test
3. (Подождать 30+ секунд)
4. Check Output Log: "QuestSaveManager: Auto-saving..."
5. Restart level
6. ShowActiveQuests     ? Квест восстановлен?
```

---

## ?? Debug Output Log

### Включить подробный лог:

**Console**:
```
Log LogTemp Verbose
```

**Что искать**:
```
LogTemp: QuestSubsystem v2.0: Initialized successfully
LogTemp: QuestSaveManager: Loaded 2 quests from save
LogTemp: Quest 'Q_Test' started from dialogue
LogTemp: Objective 'Obj_Main': 5/10
LogTemp: QuestSaveManager: Auto-saving...
```

---

## ? Quick Commands Reference

| Command | Description |
|---------|-------------|
| `StartQuest <ID>` | Начать квест |
| `CompleteQuest <ID>` | Завершить квест |
| `ShowActiveQuests` | Показать активные |
| `ResetQuests` | **УДАЛИТЬ ВСЕ СОХРАНЕНИЯ** |
| `ShowQuestSaves` | Инфо о сохранениях |
| `EnableAutoSave 60` | Авто-сохранение (60 сек) |
| `EmitNpcTalked <ID>` | Событие "поговорил с NPC" |

---

## ?? Tips

1. **Всегда начинайте с `ResetQuests`** при новом тесте
2. **Используйте `ShowQuestInfo`** для детальной информации
3. **Включите Auto-Save** чтобы не потерять прогресс
4. **Проверяйте Output Log** при проблемах
5. **Bind F1** для Debug Widget (удобнее консоли)

---

## ?? Next Steps

1. Добавьте `QuestCheatManager` в ваш PlayerController
2. Создайте тестовый квест
3. Используйте `StartQuest` для быстрого старта
4. Проверьте сохранение с `ResetQuests` ? Restart ? `ShowActiveQuests`

---

**Happy Testing!** ?
