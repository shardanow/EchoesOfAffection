# ?? QuestGiverComponent - Краткая справка

## Что это?

**UI компонент** для управления квестами от конкретного NPC.

## Для чего нужен?

| Назначение | Описание |
|-----------|----------|
| ?? **Иконки над головой** | Показывает !, ?, ? над NPC в зависимости от статуса квестов |
| ?? **Список квестов** | Возвращает доступные квесты при клике на NPC |
| ? **Приём квестов** | UI для начала квеста от NPC |
| ?? **Сдача квестов** | UI для завершения квеста у NPC |
| ?? **Фильтрация** | Проверяет зависимости и повторяемость квестов |

## Для чего НЕ нужен?

| ? НЕ участвует | Почему |
|----------------|--------|
| События диалогов | DialogueSubsystem сам извлекает NPC ID из GameplayTags |
| Условия квестов | QuestEventBus использует события из GameEventBus |
| Прогресс квестов | QuestProgressManager управляет состоянием |
| Автоматические события | GameEventBridge соединяет события и квесты |

## Основные методы

```cpp
// Получить квесты для UI
TArray<UQuestAsset*> GetAvailableQuestsForPlayer(APlayerController* Player);
TArray<UQuestAsset*> GetActiveQuestsForPlayer(APlayerController* Player);
TArray<UQuestAsset*> GetCompletableQuestsForPlayer(APlayerController* Player);

// Получить иконку над головой
UTexture2D* GetQuestIndicatorIcon(APlayerController* Player);

// Принять квест
bool AcceptQuest(APlayerController* Player, UQuestAsset* QuestAsset);

// Сдать квест
bool TurnInQuest(APlayerController* Player, UQuestAsset* QuestAsset);
```

## Настройка в Blueprint

```
BP_Lianne (Actor)
 ?? QuestGiverComponent
      ?? NpcId: "Lianne" (опционально, авто-заполняется)
      ?? bAutoAssignNpcId: true
      ?? AvailableQuests: [DA_Quest_TalkToLianne, ...]
      ?? QuestAvailableIcon: Texture_YellowExclamation
    ?? QuestInProgressIcon: Texture_GrayQuestion
      ?? QuestCompleteIcon: Texture_GoldCheck
```

## ?? ВАЖНО!

**NpcId в QuestGiverComponent ? NpcId в условиях квестов!**

- **QuestGiverComponent.NpcId** - для UI и отладки
- **Условия квеста (NpcId)** - извлекается из GameplayTags актора NPC

### Правильная настройка NPC:

```
BP_Lianne (Actor)
 ?? GameplayTags: NPC.Lianne  ? Для условий квестов!
 ?? QuestGiverComponent
  ?? NpcId: "Lianne"  ? Для UI (опционально)
```

## Типичный пример использования

```cpp
// Blueprint: OnPlayerInteract
void BP_Lianne::OnPlayerInteract(APlayerController* Player)
{
    UQuestGiverComponent* QuestGiver = FindComponentByClass<UQuestGiverComponent>();
    
    // 1. Показать доступные квесты
    TArray<UQuestAsset*> Available = QuestGiver->GetAvailableQuestsForPlayer(Player);
    if (Available.Num() > 0)
    {
     ShowQuestListUI(Available);
    }
    
    // 2. Показать квесты для сдачи
 TArray<UQuestAsset*> Completable = QuestGiver->GetCompletableQuestsForPlayer(Player);
    if (Completable.Num() > 0)
    {
        ShowTurnInUI(Completable);
    }
}

// Widget над NPC: UpdateQuestIcon
void UpdateIcon()
{
    UTexture2D* Icon = QuestGiver->GetQuestIndicatorIcon(PlayerController);
    QuestIconImage->SetBrushFromTexture(Icon);
    QuestIconImage->SetVisibility(Icon ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
```

## Когда НЕ нужен?

- ? Квест начинается автоматически (через триггер/событие)
- ? Квест основан только на событиях (без UI взаимодействия)
- ? NPC не выдаёт квесты напрямую

## См. также

- `QUEST_GIVER_COMPONENT_EXPLAINED.md` - Полное объяснение
- `NPCID_EXTRACTION_FIXED.md` - Как извлекается NPC ID для событий
- `QUEST_CONDITIONS_COMPLETE_REFERENCE.md` - Условия квестов

---

**TL;DR:** QuestGiverComponent = UI хелпер для квестов от NPC, НЕ система событий! ??
