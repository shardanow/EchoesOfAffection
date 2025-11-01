# ?? Quest System - Финальная документация

## ?? Краткое описание

**Quest System v2.0** - полнофункциональная система квестов для Unreal Engine с интеграцией событий через GameEventBus.

## ? Готовые функции

### 1?? Система квестов
- ? Создание квестов через Data Assets
- ? Множественные objectives
- ? Цепочки квестов (dependencies)
- ? Повторяемые квесты
- ? Система наград
- ? Сохранение прогресса

### 2?? Условия квестов
- ? События инвентаря (ItemAcquired, ItemUsed, ItemCrafted, ItemGifted)
- ? События диалогов (DialogueStarted, NodeReached, ChoiceMade)
- ? События квестов (QuestStarted, QuestCompleted, ObjectiveCompleted)
- ? События времени (HourChanged, DayChanged, SeasonChanged)
- ? Фильтрация по EventTag
- ? Фильтрация по ItemId / NpcId / QuestId
- ? Фильтрация по Quantity / Count

### 3?? UI компоненты
- ? Quest Tracker Widget (отслеживание активных квестов)
- ? Quest Giver Component (иконки над NPC, список квестов)
- ? Автоматическое обновление UI при изменении прогресса

### 4?? Интеграции
- ? GameEventBus - глобальная шина событий
- ? Inventory System - события предметов
- ? Dialogue System - события диалогов
- ? Time System - события времени

## ?? Полная документация

### Основные гайды

| Документ | Назначение |
|----------|-----------|
| **NPC_ID_COMPLETE_GUIDE.md** | ?? Всё про NPC ID: где хранится, как извлекается, где используется |
| **QUEST_GIVER_COMPONENT_EXPLAINED.md** | ?? Полное объяснение QuestGiverComponent |
| **QUEST_GIVER_QUICK_REF.md** | ? Краткая справка по QuestGiverComponent |
| **QUEST_CONDITIONS_COMPLETE_REFERENCE.md** | ?? Все типы условий квестов с примерами |
| **QUEST_CONDITIONS_QUICK_REFERENCE.md** | ? Краткая справка по условиям |
| **EVENT_TYPE_VS_EVENT_TAG_EXPLAINED.md** | ?? Разница между EventType и EventTag |
| **EVENT_TAG_FILTER_FIXED.md** | ?? Как работает фильтрация по EventTag |

### История исправлений

| Документ | Что исправлено |
|----------|---------------|
| **NPCID_EXTRACTION_FIXED.md** | ?? Исправление извлечения NPC ID из GameplayTags |
| **CRITICAL_TAG_MISMATCH_FIXED.md** | ?? Исправление несовпадения EventTag |
| **QUEST_EVENT_BRIDGE_FIXED.md** | ?? Исправление QuestEventBridge |
| **STRICT_EVENT_TAG_CHECK_AND_WIDGET_HIDING.md** | ?? Строгая проверка EventTag и скрытие виджета |
| **QUEST_TRACKER_SHOWS_ALL_OBJECTIVES.md** | ?? Отображение всех objectives в трекере |
| **PROGRESS_BAR_FIXED.md** | ?? Исправление прогресс-бара |
| **UFUNCTION_FIXED_FINAL.md** | ?? Исправление UFUNCTION макросов |
| **ADDED_BRIDGE_INIT_LOGGING.md** | ?? Добавление логирования в Bridge |

### Конфигурация

| Файл | Назначение |
|------|-----------|
| **Config/DefaultGameplayTags.ini** | Все GameplayTags для событий |
| **TAGS_FIXED_FINAL.md** | Документация по тегам |

## ?? Быстрый старт

### 1. Создание квеста

```
Content Browser ? Right Click ? Quest System ? Quest Asset
Имя: DA_Quest_MyFirstQuest
```

```
DA_Quest_MyFirstQuest:
  ?? QuestId: Q_MyFirstQuest
  ?? QuestName: "My First Quest"
  ?? QuestDescription: "Complete your first quest!"
  ?? Objectives:
       ?? [0]:
      ?? ObjectiveId: O_TalkToNPC
    ?? Description: "Talk to the NPC"
 ?? RequiredCount: 1
            ?? Conditions:
    ?? [0]:
      ?? EventTag: GameEvent.Dialogue.Started
  ?? NpcId: Lianne
```

### 2. Настройка NPC

```
BP_Lianne (Actor Blueprint):
  ?? Gameplay Tags: NPC.Lianne  ? Обязательно!
  ?? QuestGiverComponent:
       ?? bAutoAssignNpcId: true
    ?? AvailableQuests: [DA_Quest_MyFirstQuest]
```

### 3. Проверка в игре

```
1. Запустить игру (PIE)
2. Подойти к NPC (должна появиться иконка !)
3. Начать диалог
4. Проверить логи:
   [LogGameEventBus] ?? EmitEvent: 'GameEvent.Dialogue.Started' | StringParam='Lianne'
   [LogQuestEventBus] ? CONDITION MATCH! StringParam='Lianne' == NpcId='Lianne'
   [LogQuestEventBus] ??? MATCH FOUND! Adding 1 progress
```

## ?? Отладка

### Включить детальные логи

```cpp
// DefaultEngine.ini
[Core.Log]
LogGameEventBus=Verbose
LogQuestEventBus=Verbose
LogQuestBridge=Verbose
LogInventoryEvents=Verbose
LogDialogueSubsystem=Verbose
```

### Типичные проблемы

| Проблема | Причина | Решение |
|----------|---------|---------|
| Квест не получает прогресс | EventTag не совпадает | Проверить точное совпадение тега |
| NPC ID не совпадает | Нет GameplayTag | Добавить тег `NPC.Lianne` |
| Событие не эмитится | Плагин не включён | Включить GameEventBus plugin |
| UI не обновляется | Подписка не создана | Проверить OnQuestProgressChanged |

## ?? Архитектура

```
????????????????????????????????????????????????????
?          QuestSubsystem (Coordinator)          ?
?  ???????????????  ????????????????           ?
?  ?AssetLoader  ?  ?ProgressMgr   ?             ?
?  ?     ?  ?       ?     ?
?  ?LoadQuestAsync????StartQuest   ?      ?
?  ???????????????  ?CompleteQuest?     ?
?       ?GetState  ?             ?
?  ???????????????  ????????????????        ?
?  ? RewardSystem?     ?       ?
?  ?    ?          ?     ?
?  ?GiveReward   ?  ????????????????          ?
?  ???????????????  ?  EventBus  ?             ?
? ?  ?       ?    ?
?  ???????????????  ?OnGameEvent   ?   ?
?  ? SaveManager ?  ?CheckCondition? ?
?  ?  ?  ????????????????    ?
?  ?SaveProgress ?          ?    ?
?  ?LoadProgress ?          ?          ?
?  ???????????????          ?  ?
????????????????????????????????????????????????????
      ?
     ??????????????????????????????????
       ?   ?
    ????????????????????        ??????????????????????
    ? QuestEventBridge ?        ?  GameEventBus      ?
    ?    ?        ?   Subsystem        ?
? Subscribe to:??????????         ?
    ? - Inventory      ? ? EmitEvent()   ?
    ? - Dialogue       ?        ? Subscribe()        ?
    ? - Time       ?        ??????????????????????
    ? - Quest    ?        ?
    ????????????????????      ?
             ?
     ??????????????????????????????????????
              ??
     ???????????????????              ????????????????????
     ?InventoryComponent?      ?DialogueSubsystem?
     ? ?              ?        ?
     ?AddItem() ??????EmitEvent        ?StartDialogue()  ?
     ?UseItem() ??????EmitEvent        ?  ???EmitEvent   ?
     ????????????????????   ???????????????????
```

## ?? Примеры использования

### Пример 1: Квест "Collect Apples"

```
DA_Quest_CollectApples:
  ?? Objective: "Collect 5 apples"
       ?? Condition:
        ?? EventTag: GameEvent.Inventory.ItemAcquired
         ?? ItemId: RedApple
       ?? RequiredCount: 5
```

### Пример 2: Квест "Talk to Lianne"

```
DA_Quest_TalkToLianne:
  ?? Objective: "Talk to Lianne"
     ?? Condition:
 ?? EventTag: GameEvent.Dialogue.Started
            ?? NpcId: Lianne
            ?? RequiredCount: 1
```

### Пример 3: Квест "Complete Another Quest"

```
DA_Quest_ChainedQuest:
  ?? Dependencies: [Q_TalkToLianne]  ? Сначала нужно завершить этот
  ?? Objective: "Complete Lianne's first quest"
       ?? Condition:
            ?? EventTag: GameEvent.Quest.Completed
  ?? QuestId: Q_TalkToLianne
   ?? RequiredCount: 1
```

## ?? Тестирование

### Консольные команды

```cpp
// Начать квест
QuestSubsystem->StartQuestAsync(FName("Q_MyQuest"), OnStartedDelegate);

// Завершить квест
QuestSubsystem->CompleteQuest(FName("Q_MyQuest"));

// Получить состояние квеста
EQuestState State = QuestSubsystem->GetProgressManager()->GetQuestState(FName("Q_MyQuest"));

// Получить прогресс objective
int32 Progress = QuestSubsystem->GetProgressManager()->GetObjectiveProgress(FName("Q_MyQuest"), FName("O_Objective1"));
```

### Blueprint Testing

```cpp
// Event BeginPlay
void ATestActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Get Quest Subsystem
    UQuestSubsystem* QuestSystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    
    // Start test quest
    QuestSystem->StartQuestAsync(
        FName("Q_TestQuest"),
        FOnQuestStartedDelegate::CreateUObject(this, &ATestActor::OnQuestStarted)
    );
}

void ATestActor::OnQuestStarted(bool bSuccess, UQuestAsset* QuestAsset)
{
    if (bSuccess)
    {
UE_LOG(LogTemp, Log, TEXT("Test quest started: %s"), *QuestAsset->QuestName.ToString());
    }
}
```

## ?? Связанные системы

### Inventory System
- `Plugins/AdvancedInventorySystem/ALL_INVENTORY_OPERATIONS_SUPPORTED.md`
- События: ItemAcquired, ItemUsed, ItemCrafted, ItemGifted, ItemDropped

### Dialogue System
- `Plugins/DialogueSystem/FINAL_SUMMARY_ALL_EVENTS.md`
- `Plugins/DialogueSystem/ALL_DIALOGUE_EVENTS_COMPLETE.md`
- События: DialogueStarted, DialogueEnded, NodeReached, ChoiceMade

### GameEventBus
- `Plugins/GameEventBus/README.md`
- `Plugins/GameEventBus/CRITICAL_TAG_MISMATCH_FIXED.md`
- Глобальная шина событий для всех систем

## ?? Changelog

### v2.0 (Current)
- ? Полная интеграция с GameEventBus
- ? Поддержка всех типов событий
- ? Исправлено извлечение NPC ID
- ? Исправлена фильтрация по EventTag
- ? Quest Tracker Widget
- ? Quest Giver Component
- ? Детальное логирование
- ? Полная документация

### v1.0
- Базовая система квестов
- Objectives и Conditions
- Система наград
- Сохранение прогресса

## ?? Roadmap

### Планируется в будущем
- [ ] Квесты по времени (time-limited)
- [ ] Секретные objectives
- [ ] Случайные objectives (random selection)
- [ ] Квесты от нескольких NPC
- [ ] Групповые квесты
- [ ] PvP квесты
- [ ] Achievements integration

## ?? Поддержка

### При возникновении проблем:

1. **Проверить логи** - включить Verbose логирование
2. **Проверить теги** - EventTag должен точно совпадать
3. **Проверить NPC ID** - добавить GameplayTag `NPC.{Name}`
4. **Проверить плагины** - GameEventBus должен быть включён
5. **Прочитать документацию** - см. раздел "Полная документация"

### Часто задаваемые вопросы

**Q: Квест не начинается**
A: Проверьте Dependencies - возможно, требуется завершить другой квест

**Q: Условие не срабатывает**
A: Проверьте точное совпадение EventTag и ItemId/NpcId/QuestId

**Q: UI не обновляется**
A: Убедитесь, что виджет подписан на OnQuestProgressChanged

**Q: Событие не эмитится**
A: Проверьте, что GameEventBus plugin включён в .uproject

---

**Quest System v2.0** - Production Ready! ??

**Created by:** Game Development Team  
**Last Updated:** 2025-01-31  
**Version:** 2.0.0
