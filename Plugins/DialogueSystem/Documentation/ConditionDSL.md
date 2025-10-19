# Dialogue Condition DSL Reference

## Обзор
DSL (Domain-Specific Language) для условий диалогов поддерживает простой и читаемый синтаксис для создания сложных логических выражений.

## Базовые Типы Условий

### 1. Проверка Наличия Предмета
```
hasItem(ItemName)
hasItem(ItemName, Count)
```

**Примеры:**
```
hasItem(Rose)           // Есть роза
hasItem(Gold, 100)      // Есть 100 золота
has(Sword)              // Альтернативный синтаксис
```

### 2. Проверка Отношений (Affinity)
```
affinity[NPCName] >= Value
affinity[NPCName] > Value
affinity[NPCName] <= Value
affinity[NPCName] < Value
affinity[NPCName] == Value
```

**Примеры:**
```
affinity[Lianne] >= 50   // Отношения с Лианной >= 50
affinity[Marcus] < 0     // Отношения с Маркусом отрицательные
```

### 3. Проверка Флагов Памяти
```
memory(FlagName)
```

**Примеры:**
```
memory(FirstMeeting)     // Флаг "FirstMeeting" установлен
memory(QuestComplete)    // Квест завершен
```

### 4. Проверка Посещения Узла
```
visited(NodeId)
```

**Примеры:**
```
visited(Node_001)        // Узел Node_001 был посещен
!visited(Node_002)       // Узел Node_002 еще не посещен
```

### 5. Проверка Времени Суток
```
time == TimeTag
```

**Примеры:**
```
time == Morning          // Утро
time == Evening          // Вечер
time == Night            // Ночь
```

### 6. Проверка Мировых Тегов
```
tag(TagName)
hasTag(TagName)
```

**Примеры:**
```
tag(Weather.Rainy)       // Идет дождь
hasTag(Quest.Active)     // Есть активный квест
```

### 7. Проверка Кастомных Переменных
```
variable[VarName] == Value
var[VarName] == Value
```

**Примеры:**
```
variable[PlayerLevel] == 5      // Уровень игрока равен 5
var[QuestStage] == 3            // Стадия квеста 3
variable[PlayerName] == "Hero"  // Имя игрока "Hero"
```

## Логические Операторы

### AND (И)
```
condition1 && condition2
condition1 AND condition2
```

**Примеры:**
```
hasItem(Rose) && affinity[Lianne] >= 50
memory(FirstMeeting) AND time == Evening
```

### OR (ИЛИ)
```
condition1 || condition2
condition1 OR condition2
```

**Примеры:**
```
hasItem(Gold, 100) || hasItem(Silver, 200)
time == Morning OR time == Evening
```

### NOT (НЕ)
```
!condition
NOT condition
```

**Примеры:**
```
!visited(Node_001)
NOT memory(QuestFailed)
```

## Группировка с Скобками
```
(condition1 && condition2) || condition3
```

**Примеры:**
```
(hasItem(Rose) && time == Evening) || affinity[Lianne] >= 70
!(visited(Node_001) && memory(BadChoice))
```

## Сложные Примеры

### Пример 1: Романтическая сцена
```
(affinity[Lianne] >= 50 && hasItem(Rose) && time == Evening) || affinity[Lianne] >= 80
```
Требуется:
- (Отношения >= 50 И есть роза И вечер) ИЛИ
- Отношения >= 80

### Пример 2: Квестовый прогресс
```
memory(QuestStarted) && !visited(QuestLocation) && variable[QuestStage] == 2
```
Требуется:
- Квест начат И
- Локация не посещена И
- Стадия квеста = 2

### Пример 3: Множественные условия
```
(hasItem(Gold, 100) || hasItem(Gem, 5)) && 
(affinity[Merchant] >= 30 || tag(Quest.Merchant.Active))
```
Требуется:
- (Есть 100 золота ИЛИ есть 5 драгоценных камней) И
- (Отношения с торговцем >= 30 ИЛИ активен квест торговца)

### Пример 4: Временные и эмоциональные условия
```
time == Night && affinity[Lianne] >= 60 && 
!memory(Rejected) && variable[Location] == "Garden"
```

## Приоритет Операторов
1. `()` - Скобки (высший приоритет)
2. `!`, `NOT` - Отрицание
3. `&&`, `AND` - Логическое И
4. `||`, `OR` - Логическое ИЛИ (низший приоритет)

## Использование в Blueprint

### Пример простого условия:
```cpp
if (ConditionEvaluator->EvaluateString("hasItem(Rose)", Context))
{
    // Показать романтический диалог
}
```

### Пример сложного условия:
```cpp
FString ComplexCondition = TEXT(
    "(affinity[Lianne] >= 50 && hasItem(Rose)) || "
    "(affinity[Lianne] >= 70 && time == Evening)"
);

if (ConditionEvaluator->EvaluateString(ComplexCondition, Context))
{
    // Особый диалог доступен
}
```

## Best Practices

### 1. Используйте осмысленные имена
```
? memory(CompletedTutorial)
? memory(flag1)
```

### 2. Группируйте связанные условия
```
? (hasItem(Key) && visited(Castle)) || hasItem(MasterKey)
? hasItem(Key) && visited(Castle) || hasItem(MasterKey)  // Неоднозначно
```

### 3. Избегайте слишком сложных выражений
Разбивайте очень длинные условия на несколько проверок в коде.

### 4. Кэшируйте часто используемые условия
DSL парсер автоматически кэширует распарсенные условия по строке.

## Расширение DSL

Для добавления новых типов условий:

1. Создайте новый класс `UDialogueCondition_YourType`
2. Добавьте парсинг в `UDialogueConditionEvaluator::ParseAtom()`
3. Реализуйте `Evaluate_Implementation()` и `GetDescription_Implementation()`
4. Добавьте документацию здесь

## Отладка

### Получить текстовое описание условия:
```cpp
UDialogueCondition* Condition = Evaluator->ParseCondition("hasItem(Rose) && affinity[Lianne] >= 50");
FString Description = Condition->GetDescription();
// Выведет: "(Has Rose AND Affinity[Lianne] >= 50.0)"
```

### Включить логирование парсинга:
Добавьте в `DialogueConditionEvaluator.cpp`:
```cpp
#define DEBUG_CONDITION_PARSING 1
```
