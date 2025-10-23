# Документация DialogueSystem Plugin

Добро пожаловать в документацию плагина **DialogueSystem** для Unreal Engine 5!

## ?? Содержание

### Базовая документация
1. **[DialogueSystemCore Module](01_DialogueSystemCore_Module.md)**  
   Ядро системы: ноды, раннер, условия, эффекты

2. **[DialogueSystemRuntime Module](02_DialogueSystemRuntime_Module.md)**  
   Runtime компоненты: DialogueComponent, камеры, презентация

3. **[DialogueSystemUI Module](03_DialogueSystemUI_Module.md)**  
   UI виджеты и интеграция с интерфейсом

4. **[DialogueSystemAI Module](04_DialogueSystemAI_Module.md)**  
   AI интеграция: отношения, память, эмоции

5. **[Complete Examples](05_Complete_Examples.md)**  
 Полные примеры реализации диалоговых систем

### Новое! События диалогов

6. **[Dialogue Events Guide](06_DialogueEvents_Guide.md)** ? NEW  
   Полное руководство по работе с событиями диалогов в C++ и Blueprint

7. **[Dialogue Events Quick Start](06_DialogueEvents_QuickStart.md)** ? NEW  
   Быстрый старт для Blueprint разработчиков

8. **[Player Dialogue Events Guide](07_Player_Dialogue_Events_Guide.md)** ? NEW  
 Как отловить события диалога из Actor игрока (Blueprint примеры)

### Техническая документация

- **[DSL Parser Documentation](DSL_Parser_Documentation.md)**  
  Документация по парсеру DSL языка для диалогов

---

## ?? Быстрый старт

### Для программистов (C++)
1. Прочитайте [DialogueSystemCore Module](01_DialogueSystemCore_Module.md)
2. Изучите [Dialogue Events Guide](06_DialogueEvents_Guide.md)
3. Посмотрите примеры в [Complete Examples](05_Complete_Examples.md)

### Для дизайнеров (Blueprint)
1. Начните с [Dialogue Events Quick Start](06_DialogueEvents_QuickStart.md)
2. Изучите [DialogueSystemRuntime Module](02_DialogueSystemRuntime_Module.md)
3. Посмотрите [DialogueSystemUI Module](03_DialogueSystemUI_Module.md)

---

## ?? Что нового?

### v1.1 - События диалогов

Добавлены три основных события в `DialogueComponent`:

#### ? OnDialogueStarted
Вызывается при начале диалога
```cpp
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueStartedSignature OnDialogueStarted;
```

#### ? OnDialogueEnded
Вызывается при завершении диалога
```cpp
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueEndedSignature OnDialogueEnded;
```

#### ? OnDialogueNodeChanged
Вызывается при смене реплики (ноды)
```cpp
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnDialogueNodeChangedSignature OnDialogueNodeChanged;
```

**Пример использования:**
```cpp
// C++
DialogueComp->OnDialogueStarted.AddDynamic(this, &AMyNPC::HandleDialogueStarted);
DialogueComp->OnDialogueEnded.AddDynamic(this, &AMyNPC::HandleDialogueEnded);
DialogueComp->OnDialogueNodeChanged.AddDynamic(this, &AMyNPC::HandleNodeChanged);
```

См. [Dialogue Events Guide](06_DialogueEvents_Guide.md) для подробностей.

---

#### ?? Глобальные события через DialogueSubsystem

Добавлены глобальные события для отслеживания ЛЮБЫХ диалогов в игре:

```cpp
// Любой диалог начался (откуда угодно)
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnAnyDialogueStartedSignature OnAnyDialogueStarted;

// Любой диалог закончился
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnAnyDialogueEndedSignature OnAnyDialogueEnded;

// Любая реплика сменилась
UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnAnyDialogueNodeChangedSignature OnAnyDialogueNodeChanged;
```

**Пример для Player Blueprint:**
```cpp
// В BeginPlay
Get Game Instance ? Get Subsystem (DialogueSubsystem)
    ? Bind: OnAnyDialogueStarted ? HandleStart
    ? Bind: OnAnyDialogueEnded ? HandleEnd
    ? Bind: OnAnyDialogueNodeChanged ? HandleNodeChange
```

См. [Player Dialogue Events Guide](07_Player_Dialogue_Events_Guide.md) для полных примеров.

---

## ?? Структура модулей

```
DialogueSystem/
??? DialogueSystemCore/          # Ядро системы (data-driven)
?   ??? DialogueNode       # Узлы диалогов
?   ??? DialogueRunner   # Движок выполнения
?   ??? DialogueConditions      # Система условий
?   ??? DialogueEffects         # Система эффектов
?
??? DialogueSystemRuntime/       # Runtime компоненты
?   ??? DialogueComponent       # Основной компонент NPC
?   ??? DialogueCameraComponent # Камеры для диалогов
?   ??? DialogueSubsystem       # Глобальный менеджер
?   ??? RelationshipComponent   # Отношения с игроком
?
??? DialogueSystemUI/       # UI система
?   ??? DialogueWidget     # Основной виджет диалогов
?   ??? ChoiceButton            # Кнопки выбора
?   ??? DialogueHUD # HUD интеграция
?
??? DialogueSystemAI/     # AI интеграция
    ??? NPCMemoryComponent      # Память NPC
    ??? EmotionSystem           # Эмоции
    ??? BehaviorTree     # BT задачи
```

---

## ?? Ключевые концепции

### 1. Data-Driven Architecture
Вся логика диалогов хранится в **DialogueDataAsset**, без привязки к конкретным NPC.

### 2. Event-Based System
Система использует события (делегаты) для уведомления о изменениях состояния.

### 3. Dependency Injection
Компоненты используют интерфейсы (`IDialogueService`, `IDialogueParticipant`) для декаплинга.

### 4. Thread-Safe Design
Безопасно для использования с AI и асинхронными операциями.

---

## ?? Типичные сценарии использования

### Сценарий 1: Простой NPC с одним диалогом
```cpp
// 1. Добавить DialogueComponent к NPC
// 2. Установить DefaultDialogue в Details
// 3. Подписаться на события (опционально)
```

### Сценарий 2: NPC с несколькими диалогами по условиям
```cpp
// 1. Добавить DialogueComponent и RelationshipComponent
// 2. Заполнить ConditionalDialogues мапу
// 3. Система автоматически выберет подходящий диалог
```

### Сценарий 3: Диалог с озвучкой и анимациями
```cpp
// 1. Подписаться на OnDialogueNodeChanged
// 2. В обработчике воспроизводить VoiceClip из ноды
// 3. Триггерить анимации на основе EmotionTag
```

### Сценарий 4: Кастомная камера для диалогов
```cpp
// 1. Добавить DialogueCameraComponent
// 2. Подписаться на OnDialogueStarted/Ended
// 3. Управлять blend между камерами
```

---

## ??? Инструменты разработки

### Blueprint
- Все компоненты доступны из Blueprint
- Все события можно прослушивать через Event Graph
- Полная поддержка Blueprint виджетов

### C++
- Чистый C++ API
- Полная поддержка интерфейсов
- Thread-safe операции

### Visual Scripting (опционально)
- Можно использовать вместо Blueprint
- Те же самые ноды и события

---

## ?? Примеры интеграции

### С EnhancedInput
```cpp
// Привязать E к началу диалога
InputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, 
  this, &APlayer::TryStartDialogue);
```

### С Behavior Tree
```cpp
// Создать BT Task для диалога
UBTTask_StartDialogue::ExecuteTask()
{
    DialogueComponent->StartDialogue(PlayerCharacter);
}
```

### С Save System
```cpp
// Сохранить состояние диалогов
DialogueSubsystem->SaveDialogueState(SaveGame);
DialogueSubsystem->LoadDialogueState(SaveGame);
```

---

## ?? Отладка

### Включить логирование
```cpp
// В DefaultEngine.ini
[Core.Log]
LogDialogueComponent=Verbose
LogDialogueRunner=Verbose
LogDialogueSubsystem=Verbose
```

### Использовать Print String в Blueprint
```
On Dialogue Started ??? Print String "Dialogue Started!"
On Dialogue Ended ??? Print String "Dialogue Ended!"
```

### Проверить актуальность нод
```cpp
if (ensure(NewNode))
{
    UE_LOG(LogTemp, Log, TEXT("Node: %s"), *NewNode->GetDialogueText().ToString());
}
```

---

## ?? Поддержка

### Issues
- Создайте issue в репозитории GitHub
- Приложите логи и скриншоты
- Опишите шаги воспроизведения

### Pull Requests
- Следуйте code style проекта
- Добавьте тесты (если возможно)
- Обновите документацию

### Вопросы
- Проверьте [Complete Examples](05_Complete_Examples.md)
- Посмотрите [Dialogue Events Guide](06_DialogueEvents_Guide.md)
- Спросите в Discord/форуме проекта

---

## ?? Changelog

### v1.1.0 (2025-01-22)
- ? Добавлены события диалогов: OnDialogueStarted, OnDialogueEnded, OnDialogueNodeChanged
- ?? Добавлены глобальные события в DialogueSubsystem: OnAnyDialogueStarted, OnAnyDialogueEnded, OnAnyDialogueNodeChanged
- ?? Новая документация по событиям (3 новых гайда)
- ?? Специальное руководство для Player Blueprint
- ?? Исправлены memory leaks при завершении диалога
- ? Оптимизирована подписка на события Runner

### v1.0.0 (Initial Release)
- ?? Первый релиз DialogueSystem
- ? Полная реализация Core, Runtime, UI, AI модулей
- ?? Базовая документация

---

## ?? Лицензия

Copyright Epic Games, Inc. All Rights Reserved.

Смотрите LICENSE файл в корне проекта.

---

## ?? Благодарности

Спасибо всем контрибьюторам и тестировщикам проекта!

---

**Последнее обновление:** 22 января 2025  
**Версия документации:** 1.1.0  
**Версия плагина:** 1.1.0
