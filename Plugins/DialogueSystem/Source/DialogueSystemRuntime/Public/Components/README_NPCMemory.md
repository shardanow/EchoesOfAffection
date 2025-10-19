# NPC Memory Component

## ?? Краткое описание

**NPCMemoryComponent** - это модульный компонент долгосрочной памяти для NPC в Unreal Engine 5, разработанный для создания живых и запоминающихся персонажей в RPG и симуляторах отношений.

## ? Ключевые возможности

- ?? **Долгосрочная память** - NPC помнят важные события, выборы игрока и темы разговоров
- ?? **Система забывания** - Память естественно деградирует со временем (с учетом важности и эмоций)
- ?? **Эмоциональная окраска** - События с сильными эмоциями помнятся дольше
- ?? **Персонализация** - Настройка памяти под личность NPC (хорошая/плохая память, оптимист/пессимист)
- ?? **Интеллектуальный поиск** - Релевантный поиск воспоминаний по контексту
- ??? **Отслеживание тем** - Автоматическое отслеживание обсужденных тем и интереса к ним
- ?? **Интеграция** - Полная интеграция с RelationshipComponent и DialogueSystem
- ?? **Сохранение** - Полная поддержка сохранения/загрузки через JSON

## ?? Файлы компонента

```
Plugins/DialogueSystem/Source/DialogueSystemRuntime/
??? Public/Components/
?   ??? NPCMemoryComponent.h          # Основной компонент
?   ??? NPCMemoryExamples.h           # Примеры использования
?   ??? NPCMemoryTestHelpers.h        # Тестовые утилиты
??? Private/Components/
    ??? NPCMemoryComponent.cpp        # Реализация

Plugins/DialogueSystem/Source/DialogueSystemCore/
??? Public/Conditions/
?   ??? MemoryConditions.h            # Условия для диалогов
??? Public/Effects/
    ??? MemoryEffects.h               # Эффекты для диалогов

Plugins/DialogueSystem/Documentation/
??? NPCMemoryComponent_Guide.md       # Полное руководство
??? Blueprint_Setup_Guide.md          # Blueprint примеры
```

## ?? Быстрый старт

### 1. Добавление компонента

```cpp
// В конструкторе вашего NPC
MemoryComponent = CreateDefaultSubobject<UNPCMemoryComponent>(TEXT("MemoryComponent"));
```

Или через Blueprint:
1. Откройте Blueprint NPC
2. Добавьте компонент `NPCMemoryComponent`
3. Настройте параметры в Details панели

### 2. Создание воспоминания

```cpp
// Простой способ
MemoryComponent->CreateMemory(
    EMemoryType::DialogueEvent,
    FText::FromString("Player helped defend the village"),
    75.0f,                        // Важность
    EMemoryEmotion::Joyful,
    PlayerActor
);
```

### 3. Поиск воспоминаний

```cpp
// Поиск релевантных воспоминаний для текущего контекста
FGameplayTagContainer ContextTags;
ContextTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Topic.Family")));

TArray<FNPCMemoryEntry> Memories = MemoryComponent->FindRelevantMemories(
    ContextTags,
    PlayerActor,
    5
);
```

### 4. Использование в диалогах

В DialogueNode добавьте условие:
```
Condition: Remembers Event
- Memory ID: "PlayerSavedMe"
- Min Importance: 70.0
```

И эффект:
```
Effect: Create Memory
- Memory Type: Player Choice
- Description: "Player promised to help"
- Importance: 90.0
- Emotion: Nostalgic
```

## ?? Типы воспоминаний

| Тип | Описание | Типичная важность |
|-----|----------|-------------------|
| DialogueEvent | События из диалогов | 30-50 |
| PlayerChoice | Важные выборы игрока | 60-85 |
| ItemGiven | Подарки и предметы | 50-75 |
| QuestEvent | Квестовые события | 60-80 |
| WorldEvent | Важные мировые события | 50-70 |
| Promise | Обещания | 85-95 |
| Secret | Секреты | 90-100 |
| PersonalMemory | Личные воспоминания | 40-80 |

## ?? Эмоции

| Эмоция | Влияние на память |
|--------|-------------------|
| Neutral | Стандартное забывание |
| Positive | +20% к устойчивости |
| Negative | +30% с учетом Negative Bias |
| Bittersweet | +25% к устойчивости |
| Traumatic | +50% к устойчивости (помнится очень долго) |
| Joyful | +30% к устойчивости |
| Regretful | +25% к устойчивости |
| Nostalgic | +20% к устойчивости |

## ?? Основные параметры

### Memory Storage
- `Max Memories` (100) - Максимальное количество воспоминаний
- `Max Topics` (50) - Максимальное количество тем

### Memory Decay
- `Enable Memory Decay` (true) - Включить автоматическое забывание
- `Base Decay Rate` (0.05) - Базовая скорость забывания (за день)
- `Min Importance Threshold` (10.0) - Минимальная важность для сохранения
- `Emotional Memory Bonus` (0.5) - Бонус для эмоциональных событий

### Personality
- `Memory Retention Multiplier` (1.0) - Множитель качества памяти
  - 0.5 = Плохая память
  - 1.0 = Нормальная память
  - 2.0 = Отличная память
  
- `Negative Bias` (1.0) - Склонность помнить негативное
  - 0.5 = Оптимист
  - 1.0 = Сбалансированный
  - 2.0 = Обидчивый

### Performance
- `Decay Update Interval` (60.0) - Частота обновления decay (секунды)

## ?? Интеграция

### С RelationshipComponent

```cpp
// Создаем память о предательстве
FNPCMemoryEntry Memory;
Memory.Type = EMemoryType::PlayerChoice;
Memory.Description = FText::FromString("Player betrayed my trust");
Memory.Emotion = EMemoryEmotion::Traumatic;
MemoryComponent->AddMemory(Memory);

// Влияем на отношения
RelationshipComponent->ModifyAffinity(-40.0f);
RelationshipComponent->ModifyTrust(-60.0f);
```

### С DialogueSystem

Используйте Memory Conditions и Effects в DialogueNodes для:
- Проверки выполнения обещаний
- Упоминания прошлых событий
- Блокировки повторяющихся разговоров
- Динамической генерации диалогов

### С Quest System

```cpp
// При начале квеста
OnQuestStarted.AddDynamic([&](FName QuestId) {
    MemoryComponent->CreateMemory(
        EMemoryType::Promise,
        FText::Format(FText::FromString("Agreed to help with {0}"), QuestId),
        90.0f
    );
});

// При завершении квеста
OnQuestCompleted.AddDynamic([&](FName QuestId) {
    // Найти обещание и создать позитивную память
});
```

## ?? Примеры использования

### Динамическое приветствие

```cpp
TArray<FNPCMemoryEntry> RecentMemories = MemoryComponent->GetRecentMemories(1);
if (RecentMemories.Num() > 0)
{
    switch (RecentMemories[0].Emotion)
    {
        case EMemoryEmotion::Joyful:
            Greeting = "It's wonderful to see you again!";
            break;
        case EMemoryEmotion::Traumatic:
            Greeting = "What do you want?";
            break;
        default:
            Greeting = "Hello.";
    }
}
```

### Проверка обещаний

```cpp
TArray<FNPCMemoryEntry> Promises = MemoryComponent->GetMemoriesByType(EMemoryType::Promise);
for (const FNPCMemoryEntry& Promise : Promises)
{
    FTimespan Age = FDateTime::Now() - Promise.Timestamp;
    if (Age.GetTotalDays() > 7.0f)
    {
        // Игрок не выполнил обещание - снижаем доверие
        RelationshipComponent->ModifyTrust(-10.0f);
    }
}
```

## ?? Тестирование

```cpp
// Создать тестовый набор воспоминаний
UNPCMemoryTestHelpers::CreateTestMemories(WorldContext, NPC, Player);

// Вывести все воспоминания в лог
UNPCMemoryTestHelpers::PrintMemories(NPC);

// Тест релевантности
UNPCMemoryTestHelpers::TestRelevance(NPC, Player);

// Тест производительности
UNPCMemoryTestHelpers::PerformanceTest(NPC, 1000);
```

## ?? Документация

Полная документация доступна в:
- `Documentation/NPCMemoryComponent_Guide.md` - Детальное руководство
- `Documentation/Blueprint_Setup_Guide.md` - Blueprint примеры

## ?? Blueprint API

### Основные функции

```
Create Memory
?? Memory Type
?? Description
?? Importance
?? Emotion
?? Related Actor

Get Memory
?? Memory ID

Find Relevant Memories
?? Context Tags
?? Related Actor
?? Max Results

Discuss Topic
?? Topic Name
?? Interest Modifier
```

### Условия для диалогов

- `Remembers Event` - Проверяет наличие воспоминания
- `Memory Count Check` - Проверяет количество воспоминаний
- `Topic Discussed` - Проверяет обсуждение темы
- `Has Memories With Tags` - Поиск по тегам
- `Memory Emotional Balance` - Проверяет эмоциональный баланс

### Эффекты для диалогов

- `Create Memory` - Создает воспоминание
- `Remember Promise` - Записывает обещание
- `Remember Secret` - Записывает секрет
- `Discuss Topic` - Отмечает обсуждение темы
- `Forget Memory` - Удаляет воспоминание
- `Recall Memory` - Укрепляет память

## ? Производительность

- **Рекомендуемый лимит памяти**: 50-150 воспоминаний на NPC
- **Update interval**: 60 секунд для большинства случаев
- **Complexity**: O(n) для большинства операций, O(n log n) для сортировки

Для оптимизации:
- Используйте `Decay Update Interval` >= 60 секунд
- Ограничьте `Max Memories` разумным значением
- Для фоновых NPC отключите `Enable Memory Decay`

## ?? Отладка

```cpp
// Вывод статистики
UE_LOG(LogTemp, Log, TEXT("Total Memories: %d"), MemoryComponent->GetMemoryCount());
UE_LOG(LogTemp, Log, TEXT("Topics: %d"), MemoryComponent->GetAllTopics().Num());

// Детальная информация
for (const FNPCMemoryEntry& Memory : MemoryComponent->GetAllMemories())
{
    UE_LOG(LogTemp, Log, TEXT("%s - Importance: %.1f, Freshness: %.2f"),
        *Memory.Description.ToString(),
        Memory.Importance,
        Memory.Freshness);
}
```

## ?? Вклад

Компонент является частью модульной системы диалогов и спроектирован для легкого расширения:

1. Добавьте новые типы воспоминаний в `EMemoryType`
2. Создайте кастомные Conditions/Effects
3. Расширьте систему релевантности
4. Добавьте интеграцию с вашими системами

## ?? Лицензия

Copyright Epic Games, Inc. All Rights Reserved.

## ?? Авторы

Разработано как часть модульной системы диалогов для проекта EchoesOfAffection.

## ?? Планы развития

- [ ] Machine Learning для предсказания интересов NPC
- [ ] Автоматическая генерация диалогов на основе памяти
- [ ] Система ассоциаций между воспоминаниями
- [ ] Визуальный редактор памяти в Editor
- [ ] Профилирование и дополнительная оптимизация

## ?? Поддержка

При возникновении проблем:
1. Проверьте логи (LogTemp)
2. Используйте Test Helpers для отладки
3. Обратитесь к полной документации
4. Проверьте примеры использования

---

**Создавайте незабываемых NPC! ???**
