# DSL Reference - Полный справочник

## ?? Введение

DSL (Domain Specific Language) системы диалогов позволяет авторам писать условия и эффекты в удобном текстовом формате, который затем парсится в runtime.

---

## ?? Conditions DSL

### Базовый синтаксис

```ebnf
condition     ::= expression
expression    ::= term (logical_op term)*
term          ::= "!" atom | atom
atom          ::= comparison | function | boolean

comparison    ::= identifier operator value
identifier    ::= name | name "[" name "]"
operator      ::= "==" | "!=" | ">" | "<" | ">=" | "<="
value         ::= number | string | boolean

logical_op    ::= "&&" | "||"
function      ::= name "(" args ")"
args          ::= arg ("," arg)*
boolean       ::= "true" | "false"
```

### Операторы

| Оператор | Описание | Пример |
|----------|----------|--------|
| `==` | Равно | `time == Evening` |
| `!=` | Не равно | `mood != Angry` |
| `>` | Больше | `affinity > 50` |
| `<` | Меньше | `gold < 100` |
| `>=` | Больше или равно | `romance >= 70` |
| `<=` | Меньше или равно | `trust <= 30` |
| `&&` | Логическое И | `a && b` |
| `||` | Логическое ИЛИ | `a || b` |
| `!` | Логическое НЕ | `!visited(Node_X)` |

### Типы данных

#### 1. **Relationship Attributes**

Синтаксис: `attribute[npc_name] operator value`

**Примеры:**
```cpp
affinity[Lianne] >= 50
trust[Marcus] > 70
romance[Lianne] >= 80
fear[Guard] < 10
respect[Elder] > 60
jealousy[Lianne] != 0
```

**Доступные атрибуты:**
- `affinity` — симпатия (-100 to 100)
- `trust` — доверие (0 to 100)
- `respect` — уважение (0 to 100)
- `romance` — романтика (0 to 100)
- `fear` — страх (0 to 100)
- `jealousy` — ревность (0 to 100)

#### 2. **Inventory Items**

Функция: `hasItem(item_id, min_count?)`

**Примеры:**
```cpp
hasItem(Rose)              // есть хотя бы 1 роза
hasItem(Rose, 5)           // есть минимум 5 роз
hasItem(Sword)             // есть меч
!hasItem(Key)              // нет ключа
```

#### 3. **Gold (Currency)**

Синтаксис: `gold operator value`

**Примеры:**
```cpp
gold >= 100                // есть минимум 100 золота
gold < 50                  // меньше 50 золота
gold == 0                  // совсем нет денег
```

#### 4. **Time of Day**

Синтаксис: `time == tag`

**Примеры:**
```cpp
time == Morning
time == Evening
time == Night
time != Noon
```

**Доступные теги:**
- `Morning` (6:00 - 12:00)
- `Noon` (12:00 - 13:00)
- `Afternoon` (13:00 - 18:00)
- `Evening` (18:00 - 21:00)
- `Night` (21:00 - 6:00)

#### 5. **Location**

Синтаксис: `location == tag`

**Примеры:**
```cpp
location == Village
location == Forest
location != Dungeon
```

#### 6. **Weather**

Синтаксис: `weather == tag`

**Примеры:**
```cpp
weather == Sunny
weather == Rainy
weather != Stormy
```

#### 7. **World State Tags**

Функция: `worldState(tag)`

**Примеры:**
```cpp
worldState(Quest.MainQuest.Completed)
worldState(Event.Festival.Started)
!worldState(War.InProgress)
```

#### 8. **Memory Flags**

Функция: `memory(key)` или `remember(key)`

**Примеры:**
```cpp
memory(FirstMeeting)              // помнит первую встречу
remember(GaveGift)                // помнит, что дарили подарок
!memory(SecretRevealed)           // секрет ещё не раскрыт
```

#### 9. **Visited Nodes**

Функция: `visited(node_id)`

**Примеры:**
```cpp
visited(Node_Intro)               // уже был в этом узле
!visited(Node_Secret)             // ещё не видел этот узел
visited(Node_Quest_Start)         // начал квест
```

#### 10. **Custom Variables**

Синтаксис: `var(key) operator value`

**Примеры:**
```cpp
var(QuestStep) == "2"
var(SecretKnown) == "true"
var(Counter) > 5
```

#### 11. **NPC Mood**

Синтаксис: `mood == tag`

**Примеры:**
```cpp
mood == Happy
mood == Angry
mood != Sad
```

**Доступные настроения:**
- `Neutral`, `Happy`, `Sad`, `Angry`, `Fearful`, `Excited`, `Bored`, `Anxious`

### Комплексные примеры

#### Пример 1: Друзья и время
```cpp
affinity[Lianne] >= 50 && time == Evening
```
**Описание**: Lianne друг (50+) И сейчас вечер

#### Пример 2: Есть подарок или деньги
```cpp
hasItem(Rose) || gold >= 100
```
**Описание**: Есть роза ИЛИ минимум 100 золота

#### Пример 3: Квест и локация
```cpp
worldState(Quest.FindFlower.Active) && location == Forest && !memory(FoundFlower)
```
**Описание**: Квест активен И в лесу И ещё не нашёл цветок

#### Пример 4: Романтический сценарий
```cpp
romance[Lianne] >= 70 && affinity[Lianne] >= 80 && time == Evening && !visited(Node_Confession)
```
**Описание**: Высокая романтика (70+) И симпатия (80+) И вечер И ещё не признавался

#### Пример 5: Негативный путь
```cpp
affinity[Lianne] < 0 && (visited(Node_Insult) || visited(Node_Betrayal))
```
**Описание**: Отрицательная симпатия И (оскорблял ИЛИ предал)

#### Пример 6: Сложная проверка ресурсов
```cpp
(hasItem(Rose, 3) && gold >= 50) || (hasItem(Diamond) && trust[Lianne] > 60)
```
**Описание**: (3 розы И 50 золота) ИЛИ (алмаз И доверие 60+)

---

## ? Effects DSL

### Базовый синтаксис

```ebnf
effects       ::= statement (";" statement)*
statement     ::= assignment | function_call

assignment    ::= identifier operator value
identifier    ::= name | name "[" name "]"
operator      ::= "=" | "+=" | "-="
value         ::= number | string | boolean

function_call ::= name "(" args ")"
args          ::= arg ("," arg)*
```

### Операторы

| Оператор | Описание | Пример |
|----------|----------|--------|
| `=` | Присвоить | `gold = 100` |
| `+=` | Увеличить | `affinity += 10` |
| `-=` | Уменьшить | `gold -= 50` |

### Типы эффектов

#### 1. **Relationship Modifications**

**Синтаксис:** `attribute[npc_name] operator value`

**Примеры:**
```cpp
affinity[Lianne] += 10
trust[Marcus] -= 5
romance[Lianne] += 15
fear[Guard] += 20
respect[Elder] = 100
jealousy[Lianne] += 30
```

**Рекомендации:**
- Небольшие изменения: ±5 для рутинных действий
- Средние изменения: ±10-15 для важных выборов
- Большие изменения: ±20+ для критических событий
- Прямое присвоение (`=`) использовать редко

#### 2. **Inventory Modifications**

**Добавить предмет:** `giveItem(item_id, count?)`
**Удалить предмет:** `takeItem(item_id, count?)` или `removeItem(item_id, count?)`

**Примеры:**
```cpp
giveItem(Rose)              // дать 1 розу
giveItem(Rose, 5)           // дать 5 роз
takeItem(Sword)             // забрать меч
removeItem(Key, 1)          // удалить 1 ключ
```

#### 3. **Gold Modifications**

**Синтаксис:** `gold operator value`

**Примеры:**
```cpp
gold += 100                 // дать 100 золота
gold -= 50                  // забрать 50 золота
gold = 0                    // обнулить (для квестов)
```

#### 4. **Memory Flags**

**Установить:** `setMemory(key)` или `setMemory(key, value)`
**Удалить:** `clearMemory(key)`

**Примеры:**
```cpp
setMemory(FirstMeeting)
setMemory(GaveGift, true)
setMemory(SecretRevealed)
clearMemory(OldFlag)
```

#### 5. **World State Tags**

**Добавить:** `addWorldState(tag)`
**Удалить:** `removeWorldState(tag)`

**Примеры:**
```cpp
addWorldState(Quest.MainQuest.Started)
addWorldState(Event.Festival.InProgress)
removeWorldState(War.Active)
```

#### 6. **Quest Management**

**Начать:** `startQuest(quest_id)`
**Завершить:** `completeQuest(quest_id)`
**Провалить:** `failQuest(quest_id)`

**Примеры:**
```cpp
startQuest(HelpLianne)
completeQuest(FindFlower)
failQuest(RescueMission)
```

#### 7. **Custom Variables**

**Синтаксис:** `setVar(key, value)`

**Примеры:**
```cpp
setVar(QuestStep, "2")
setVar(SecretKnown, "true")
setVar(Counter, "5")
```

#### 8. **NPC Mood**

**Синтаксис:** `setMood(mood_tag)`

**Примеры:**
```cpp
setMood(Happy)
setMood(Angry)
setMood(Sad)
```

#### 9. **Events**

**Trigger событие:** `triggerEvent(event_id)`

**Примеры:**
```cpp
triggerEvent(Cutscene_Intro)
triggerEvent(Battle_Start)
triggerEvent(Achievement_Unlocked)
```

### Комплексные примеры

#### Пример 1: Дарение подарка
```cpp
takeItem(Rose, 1); affinity[Lianne] += 10; romance[Lianne] += 5; setMemory(GaveRose)
```
**Что происходит:**
1. Удалить 1 розу из инвентаря
2. +10 к симпатии
3. +5 к романтике
4. Запомнить событие

#### Пример 2: Покупка предмета
```cpp
gold -= 50; giveItem(Sword); setMemory(BoughtSword)
```

#### Пример 3: Начало квеста
```cpp
startQuest(HelpLianne); affinity[Lianne] += 5; addWorldState(Quest.HelpLianne.Active)
```

#### Пример 4: Провал
```cpp
affinity[Lianne] -= 20; trust[Lianne] -= 15; setMood(Angry); setMemory(Betrayed)
```

#### Пример 5: Награда за квест
```cpp
completeQuest(FindFlower); gold += 200; affinity[Lianne] += 15; giveItem(Amulet); triggerEvent(QuestComplete_FindFlower)
```

#### Пример 6: Романтическая концовка
```cpp
romance[Lianne] = 100; affinity[Lianne] = 100; triggerEvent(Cutscene_Confession); startQuest(DateWithLianne); setMemory(Lovers)
```

---

## ?? Парсинг и выполнение

### Как работает парсер

#### 1. Tokenization
```cpp
Input: "affinity[Lianne] >= 50 && hasItem(Rose)"

Tokens:
  ["affinity", "[", "Lianne", "]", ">=", "50", "&&", "hasItem", "(", "Rose", ")"]
```

#### 2. AST Construction
```cpp
AndNode
??? ComparisonNode (affinity[Lianne] >= 50)
??? FunctionNode (hasItem(Rose))
```

#### 3. Evaluation
```cpp
bool Result = AST->Evaluate(Context);
// true если affinity >= 50 И есть роза
```

### Кэширование

Парсер кэширует результаты:

```cpp
// Первый раз: парсится
Condition1 = Parse("affinity[Lianne] >= 50");

// Второй раз: берётся из кэша
Condition2 = Parse("affinity[Lianne] >= 50"); // same object
```

**Преимущества:**
- Нет повторного парсинга
- Меньше аллокаций памяти
- Быстрее выполнение

---

## ?? Best Practices

### ? DO

#### 1. Группировать связанные эффекты
```cpp
// Хорошо
affinity[Lianne] += 10; romance[Lianne] += 5; setMemory(Date)

// Плохо (разные узлы)
Node1: affinity[Lianne] += 10
Node2: romance[Lianne] += 5
Node3: setMemory(Date)
```

#### 2. Использовать понятные имена памяти
```cpp
// Хорошо
setMemory(Player_GaveFlowerToLianne)

// Плохо
setMemory(flag1)
```

#### 3. Проверять наличие ресурсов перед использованием
```cpp
// Хорошо
Condition: hasItem(Rose)
Effect: takeItem(Rose); affinity += 10

// Плохо (может быть ошибка)
Effect: takeItem(Rose); affinity += 10
```

#### 4. Использовать скобки для ясности
```cpp
// Хорошо (явно)
(affinity >= 50 && hasItem(Rose)) || gold >= 100

// Неясно (зависит от приоритета)
affinity >= 50 && hasItem(Rose) || gold >= 100
```

### ? DON'T

#### 1. Не создавать циклические зависимости
```cpp
// Плохо
Node A: Condition "visited(B)"
Node B: Condition "visited(A)"
```

#### 2. Не использовать магические числа
```cpp
// Плохо
affinity[Lianne] += 37

// Хорошо
affinity[Lianne] += 10 // Small gift bonus
```

#### 3. Не делать эффекты слишком сложными
```cpp
// Плохо (слишком много)
affinity += 10; romance += 5; gold -= 50; giveItem(Rose); takeItem(Gold); startQuest(Q1); completeQuest(Q2); setMemory(M1); setMemory(M2); triggerEvent(E1)

// Хорошо (разделить на узлы)
Node1 Effects: affinity += 10; romance += 5
Node2 Effects: gold -= 50; giveItem(Rose)
Node3 Effects: startQuest(Q1)
```

#### 4. Не забывать проверять пограничные случаи
```cpp
// Плохо (что если gold < 50?)
gold -= 50

// Хорошо
Condition: gold >= 50
Effect: gold -= 50
```

---

## ?? Тестирование условий

### Unit Test пример

```cpp
TEST(ConditionEvaluator, Affinity_Comparison)
{
    // Setup
    UDialogueContext* Context = NewObject<UDialogueContext>();
    Context->SetAffinity(TEXT("Lianne"), 75.0f);
    
    // Test
    bool Result1 = Evaluator->EvaluateString("affinity[Lianne] >= 50", Context);
    bool Result2 = Evaluator->EvaluateString("affinity[Lianne] >= 100", Context);
    
    // Assert
    EXPECT_TRUE(Result1);   // 75 >= 50 ?
    EXPECT_FALSE(Result2);  // 75 >= 100 ?
}
```

### Runtime проверка

```cpp
// В консоли
dialogue.test "affinity[Lianne] >= 50"

// Output:
// [DialogueSystem] Condition result: true
// [DialogueSystem] - affinity[Lianne] = 75.0
// [DialogueSystem] - comparison: 75.0 >= 50.0
```

---

## ?? Примеры из практики

### Кейс 1: Система свиданий

**Требования:**
- Romance >= 70
- Affinity >= 60
- Вечер
- Не в отношениях с кем-то ещё

**Condition:**
```cpp
romance[Lianne] >= 70 && affinity[Lianne] >= 60 && time == Evening && !memory(InRelationship)
```

**Effect (при согласии):**
```cpp
romance[Lianne] += 10; setMemory(OnDateWithLianne); triggerEvent(DateCutscene); startQuest(FirstDate)
```

### Кейс 2: Система измен

**Требования:**
- В отношениях с Lianne
- Флиртуешь с другой

**Condition:**
```cpp
memory(InRelationshipWithLianne) && romance[Sarah] > 30
```

**Effect (если поймали):**
```cpp
romance[Lianne] -= 50; affinity[Lianne] -= 30; trust[Lianne] -= 40; jealousy[Lianne] += 80; setMood(Angry); setMemory(CheatedOnLianne); clearMemory(InRelationshipWithLianne)
```

### Кейс 3: Прощение

**Требования:**
- Предал раньше
- Принёс дорогой подарок
- Прошло время

**Condition:**
```cpp
memory(CheatedOnLianne) && hasItem(Diamond) && var(DaysSinceBetrayal) >= 7
```

**Effect (при прощении):**
```cpp
takeItem(Diamond); affinity[Lianne] += 20; trust[Lianne] += 10; clearMemory(CheatedOnLianne); setMemory(Forgiven); setMood(Sad)
```

---

## ?? Debugging DSL

### Syntax Errors

```cpp
// Error: Missing closing bracket
affinity[Lianne >= 50

// Error: Invalid operator
affinity[Lianne] === 50

// Error: Missing parenthesis
hasItem(Rose

// Error: Invalid function name
hasItemm(Rose)
```

### Runtime Errors

```cpp
// Error: NPC not found
affinity[UnknownNPC] >= 50
// ? Will log warning and return false

// Error: Item not in inventory
takeItem(NonExistentItem)
// ? Will log warning and fail gracefully
```

### Валидация

```cpp
// Validate before save
TArray<FString> Errors;
bool bValid = Evaluator->ValidateCondition("affinity[Lianne] >= 50", Errors);

if (!bValid)
{
    for (const FString& Error : Errors)
    {
        UE_LOG(LogDialogue, Error, TEXT("%s"), *Error);
    }
}
```

---

## ?? Расширение DSL

### Добавить новую функцию

1. Создать класс условия:

```cpp
UCLASS()
class UDialogueCondition_CustomFunction : public UDialogueCondition
{
    GENERATED_BODY()

public:
    virtual bool Evaluate_Implementation(const UDialogueContext* Context) const override
    {
        // Ваша логика
        return true;
    }
};
```

2. Зарегистрировать в парсере:

```cpp
void UDialogueConditionEvaluator::RegisterBuiltinFunctions()
{
    FunctionRegistry.Add(TEXT("myFunction"), UDialogueCondition_CustomFunction::StaticClass());
}
```

3. Использовать:

```cpp
Condition: "myFunction(arg1, arg2)"
```

---

**Версия**: 1.0
**Последнее обновление**: 2024
