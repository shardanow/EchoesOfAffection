# ?? Quest Actor Component - Complete Guide

## ?? Что это?

**QuestActorComponent** — это универсальный компонент для маркировки акторов, которые участвуют в квестовой системе.

### ?? Главная идея

Вместо разрозненной логики извлечения ID из тегов/имён, теперь **один компонент** управляет всей квестовой идентификацией актора.

---

## ? Ключевые преимущества

| Было | Стало |
|------|-------|
| ID скрыт в GameplayTags или имени | Явное поле `QuestActorId` |
| Логика извлечения разбросана по коду | Централизована в компоненте |
| Только для NPC | Для **любых** акторов (NPC, предметы, локации) |
| Нет метаданных | `TMap<FName, FString>` для кастомных данных |
| Непонятно, откуда ID | Явный источник с логами |

---

## ??? Архитектура

```
QuestActorComponent
?? IDENTITY
?  ?? QuestActorId (FName) - Явный ID
?  ?? bAutoAssignId (bool) - Автоизвлечение из тегов/имени
?  ?? QuestActorTags (FGameplayTagContainer) - Категоризация
?
?? DISPLAY
?  ?? DisplayName (FText) - Для UI
?  ?? Description (FText) - Описание
?  ?? Icon (UTexture2D*) - Иконка
?
?? ROLES
?  ?? bCanGiveQuests - Может выдавать квесты?
?  ?? bCanBeQuestTarget - Может быть целью?
?  ?? bCanEmitQuestEvents - Может эмитить события?
?  ?? bCanBeTracked - Можно отслеживать?
?
?? METADATA
   ?? TMap<FName, FString> - Кастомные данные
```

---

## ?? Быстрый старт

### Шаг 1: Добавить компонент

**В Blueprint:**
```
BP_Lianne:
  ?? Add Component ? Quest Actor Component
```

**В C++:**
```cpp
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
UQuestActorComponent* QuestActor;

AMyNPC::AMyNPC()
{
    QuestActor = CreateDefaultSubobject<UQuestActorComponent>(TEXT("QuestActor"));
}
```

### Шаг 2: Настроить ID

**Вариант А: Автоматически (рекомендуется)**
```
Quest Actor Component:
  ?? bAutoAssignId: ? TRUE
  ?? (QuestActorId пустой - заполнится автоматически)

NPC Actor:
  ?? Gameplay Tags: NPC.Lianne  ? Извлечётся "Lianne"
```

**Вариант Б: Вручную**
```
Quest Actor Component:
  ?? QuestActorId: "Lianne"  ? Явно указан
  ?? bAutoAssignId: ? FALSE
```

### Шаг 3: Настроить роли

```
Quest Actor Component:
  ?? bCanGiveQuests: ? TRUE  ? Если NPC выдаёт квесты
  ?? bCanBeQuestTarget: ? TRUE  ? Если можно взаимодействовать
  ?? bCanEmitQuestEvents: ? TRUE  ? Если эмитит события
  ?? bCanBeTracked: ? TRUE  ? Если показывать в трекере
```

### Шаг 4: (Опционально) Добавить метаданные

```
Quest Actor Component:
  ?? Metadata:
       ?? "Faction" ? "TownGuard"
       ?? "QuestType" ? "MainQuest"
       ?? "RequiredLevel" ? "5"
```

---

## ?? Использование

### C++ API

#### Получить Quest Actor ID

```cpp
// Из компонента
UQuestActorComponent* QuestActor = NPC->FindComponentByClass<UQuestActorComponent>();
if (QuestActor)
{
  FName NpcId = QuestActor->GetQuestActorId();
    UE_LOG(LogTemp, Log, TEXT("NPC ID: %s"), *NpcId.ToString());
}
```

#### Проверить теги

```cpp
if (QuestActor->HasQuestTag(FGameplayTag::RequestGameplayTag("Quest.NPC.Friendly")))
{
    // Дружелюбный NPC
}

if (QuestActor->HasQuestTagMatching(FGameplayTag::RequestGameplayTag("Quest.NPC")))
{
    // Любой тег начинающийся с "Quest.NPC"
}
```

#### Работа с метаданными

```cpp
// Получить
bool bFound;
FString Faction = QuestActor->GetMetadata("Faction", bFound);
if (bFound)
{
    UE_LOG(LogTemp, Log, TEXT("Faction: %s"), *Faction);
}

// Установить
QuestActor->SetMetadata("LastInteraction", "2024-01-15");

// Проверить наличие
if (QuestActor->HasMetadata("QuestGiverType"))
{
    // ...
}
```

#### Эмитить событие от актора

```cpp
QuestActor->EmitQuestEvent(
    FGameplayTag::RequestGameplayTag("GameEvent.Quest.CustomEvent"),
    1, // IntParam
    PlayerActor // Target
);
```

### Blueprint API

#### Получить ID

```
Get Quest Actor Id (Target = Quest Actor Component) ? Name
```

#### Проверить роль

```
Branch (Condition = Quest Actor Component ? Can Give Quests)
  ? True: Show Quest Icon
  ? False: Hide Quest Icon
```

#### Метаданные

```
Get Metadata (Quest Actor Component, Key = "Faction")
  ? String (Output)
  ? Found (Bool)
```

---

## ?? Примеры использования

### 1?? NPC Quest Giver

```
BP_Lianne:
  ?? QuestActorComponent:
  ?  ?? QuestActorId: "Lianne"
  ?  ?? bAutoAssignId: true
  ?  ?? QuestActorTags: Quest.NPC, Quest.Friendly, Quest.QuestGiver
  ?  ?? DisplayName: "Лианна"
  ?  ?? Description: "Дружелюбная жительница деревни"
  ?  ?? bCanGiveQuests: TRUE
  ?  ?? bCanBeQuestTarget: TRUE
  ?  ?? bCanEmitQuestEvents: TRUE
  ?
  ?? QuestGiverComponent:
  ?? AvailableQuests: [DA_Quest_TalkToLianne]
```

### 2?? Collectable Item

```
BP_ItemApple:
  ?? QuestActorComponent:
     ?? QuestActorId: "RedApple"
       ?? QuestActorTags: Quest.Item, Quest.Collectable, Quest.Food
       ?? DisplayName: "Красное яблоко"
       ?? Icon: T_Icon_Apple
     ?? bCanGiveQuests: FALSE
       ?? bCanBeQuestTarget: TRUE
       ?? Metadata:
            ?? "Rarity" ? "Common"
     ?? "StackSize" ? "99"
```

### 3?? Quest Location

```
BP_LocationMarker_Library:
  ?? QuestActorComponent:
       ?? QuestActorId: "Library"
     ?? QuestActorTags: Quest.Location, Quest.Town, Quest.Indoor
    ?? DisplayName: "Библиотека"
       ?? bCanBeQuestTarget: TRUE
       ?? bCanBeTracked: TRUE
       ?? Metadata:
            ?? "Zone" ? "Town"
```

### 4?? Quest Object (Interactable)

```
BP_MagicCrystal:
  ?? QuestActorComponent:
       ?? QuestActorId: "MagicCrystal"
       ?? QuestActorTags: Quest.QuestObject, Quest.Unique, Quest.Magical
       ?? DisplayName: "Магический кристалл"
       ?? bCanBeQuestTarget: TRUE
       ?? bCanEmitQuestEvents: TRUE
 ?? Metadata:
  ?? "QuestRequired" ? "Q_FindCrystal"
          ?? "OneTimeUse" ? "true"
```

---

## ?? Интеграция с системами

### DialogueSubsystem

**Автоматически использует QuestActorComponent:**

```cpp
// DialogueSubsystem.cpp - StartDialogue_Implementation()
FName NpcId = ExtractNpcId(NPC);

FName UDialogueSubsystem::ExtractNpcId(AActor* NPC)
{
    // 1?? PRIORITY: QuestActorComponent
    UQuestActorComponent* QuestActor = NPC->FindComponentByClass<UQuestActorComponent>();
    if (QuestActor)
    {
     return QuestActor->GetQuestActorId();
  }
    
    // 2?? FALLBACK: GameplayTags
    // 3?? FALLBACK: Actor Name
}
```

### QuestEventBus

**Проверяет QuestActorComponent для валидации:**

```cpp
// QuestEventBus.cpp - BroadcastEvent()
if (Payload.InstigatorActor)
{
    UQuestActorComponent* QuestActor = Payload.InstigatorActor->FindComponentByClass<UQuestActorComponent>();
  if (QuestActor && !QuestActor->bCanEmitQuestEvents)
    {
UE_LOG(LogTemp, Warning, TEXT("Actor cannot emit events!"));
        return; // Reject event
    }
}

// DoesConditionMatchEvent() - извлекает NPC/Item ID
FName ActualNpcId = Payload.StringParam;
if (ActualNpcId.IsNone() && Payload.TargetActor)
{
 UQuestActorComponent* QuestActor = Payload.TargetActor->FindComponentByClass<UQuestActorComponent>();
    if (QuestActor)
    {
        ActualNpcId = QuestActor->GetQuestActorId();
    }
}
```

---

## ?? Auto-Assign ID Logic

### Приоритет извлечения

```
1?? QuestActorId (если задан вручную)
   ?
2?? GameplayTags (e.g., "NPC.Lianne" ? "Lianne")
   ?
3?? Actor Name (e.g., "BP_Lianne_C_1" ? "Lianne")
   ?
4?? Actor Name as-is (last resort)
```

### Пример логов

```
[QuestActorComponent] Auto-assigned QuestActorId from GameplayTags: 'BP_Lianne_C_1' -> 'Lianne'
[QuestActorComponent] QuestActorComponent initialized on 'BP_Lianne_C_1' with ID 'Lianne' (auto-assigned)
[QuestActorComponent]   Roles: QuestGiver, Target, EventEmitter, Trackable
[QuestActorComponent]   Quest Tags: Quest.NPC.Quest.Friendly
```

---

## ?? Валидация и ошибки

### Автоматические проверки в BeginPlay()

```
? Проверка QuestActorId (пусто?)
? Проверка ролей (хотя бы одна включена?)
? Проверка QuestGiverComponent (если bCanGiveQuests=true)
? Проверка DisplayName (пусто?)
```

### Типичные ошибки

| Ошибка | Лог | Решение |
|--------|-----|---------|
| ID пустой | `QuestActorId is None` | Включить `bAutoAssignId` или задать вручную |
| Нет ролей | `No quest roles enabled` | Включить хотя бы одну роль |
| Нет QuestGiver | `bCanGiveQuests=true but no QuestGiverComponent` | Добавить компонент |
| DisplayName пусто | `DisplayName is empty` | Заполнить для UI |

---

## ?? Лучшие практики

### ? DO

1. **Всегда добавляйте компонент** на акторы, участвующие в квестах
2. **Используйте QuestActorTags** для категоризации
3. **Включайте только нужные роли** (не все сразу)
4. **Заполняйте DisplayName** для UI
5. **Используйте метаданные** для кастомной логики

### ? DON'T

1. **Не оставляйте все роли = false** (компонент бесполезен)
2. **Не дублируйте логику** (компонент уже всё делает)
3. **Не игнорируйте логи** (валидация подскажет ошибки)
4. **Не забывайте про fallback** (старая логика работает)

---

## ?? Миграция со старой системы

### Вариант А: Постепенная миграция

```
1. Добавить QuestActorComponent на новые акторы
2. Старые акторы продолжают работать (fallback)
3. Постепенно добавлять компонент на все акторы
4. Удалить fallback логику (опционально)
```

### Вариант Б: Автоматическая миграция

```cpp
// Editor Utility Widget или Commandlet
void MigrateActorsToQuestActorComponent()
{
    TArray<AActor*> AllActors;
    // Найти всех NPC в уровнях
  
    for (AActor* Actor : AllActors)
    {
        if (!Actor->FindComponentByClass<UQuestActorComponent>())
        {
UQuestActorComponent* QuestActor = NewObject<UQuestActorComponent>(Actor);
 QuestActor->bAutoAssignId = true;
     QuestActor->RegisterComponent();
            
            UE_LOG(LogTemp, Log, TEXT("Added QuestActorComponent to %s"), *Actor->GetName());
        }
    }
}
```

---

## ?? Сравнение с альтернативами

| Подход | Плюсы | Минусы |
|--------|-------|--------|
| **GameplayTags only** | Легко | ID скрыт, нет метаданных |
| **Actor Name only** | Без настройки | Ненадёжно, зависит от имён |
| **Hardcoded IDs** | Быстро | Не расширяется, нет UI |
| **QuestActorComponent** ? | Явно, гибко, расширяемо | Нужно добавлять компонент |

---

## ?? Когда использовать

### ? Используйте QuestActorComponent для:

- **NPC** - квестгиверы, цели диалогов
- **Items** - предметы для сбора/использования
- **Locations** - маркеры локаций, триггеры
- **Quest Objects** - интерактивные объекты квестов
- **Enemies** - если нужно убить определённого врага

### ? НЕ используйте для:

- **Player** - обычно не участвует как "quest actor"
- **UI Widgets** - компонент только для AActor
- **Temporary Actors** - если живут <1 секунды

---

## ?? Checklist

Перед добавлением QuestActorComponent:

```
? Актор участвует в квестовой системе?
? Нужен уникальный ID для условий квестов?
? Актор будет эмитить события или быть целью?
? Нужны метаданные для кастомной логики?
? Хотите явную валидацию в Editor?

Если хотя бы 3 "Да" ? добавляйте компонент!
```

---

## ?? Следующие шаги

1. **Изучить примеры** в проекте
2. **Добавить компонент** на свои акторы
3. **Настроить роли** и метаданные
4. **Протестировать** с квестами
5. **Прочитать** интеграцию с другими системами

---

## ?? См. также

- **QUICK_START.md** - Быстрый старт квестовой системы
- **NPC_ID_COMPLETE_GUIDE.md** - Старая система NPC ID (fallback)
- **QUEST_CONDITIONS_COMPLETE_REFERENCE.md** - Как использовать ID в условиях
- **README_FINAL.md** - Полная документация квестовой системы

---

**Версия:** 1.0  
**Дата:** 2024-01-15  
**Автор:** Quest System Team  
**Статус:** ? Production Ready
