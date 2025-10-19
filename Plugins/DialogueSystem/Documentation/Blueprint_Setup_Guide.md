# Blueprint Setup Guide - NPC Memory System

## Быстрый старт

### Шаг 1: Добавление компонентов к NPC

1. Откройте Blueprint вашего NPC (например, `BP_NPC_Villager`)
2. В панели Components добавьте следующие компоненты:
   - `DialogueComponent` (если еще не добавлен)
   - `RelationshipComponent` (если еще не добавлен)
   - `NPCMemoryComponent` ? **НОВЫЙ**

### Шаг 2: Настройка NPCMemoryComponent

В Details панели для NPCMemoryComponent настройте:

**Basic Settings:**
- `Max Memories`: 100 (для обычного NPC), 150 (для важного NPC)
- `Max Topics`: 50

**Decay Settings:**
- `Enable Memory Decay`: ? (рекомендуется)
- `Base Decay Rate`: 0.05 (стандартное забывание)
- `Min Importance Threshold`: 10.0 (забывается только малозначимое)
- `Emotional Memory Bonus`: 0.5 (эмоциональные события помнятся дольше)

**Personality Settings:**
- `Memory Retention Multiplier`: 
  - 0.5 = Плохая память (старик, пьяница)
  - 1.0 = Нормальная память (большинство NPC)
  - 2.0 = Отличная память (ученый, мудрец)
  
- `Negative Bias`:
  - 0.5 = Оптимист (помнит хорошее)
  - 1.0 = Сбалансированный
  - 2.0 = Обидчивый (помнит плохое дольше)

**Performance:**
- `Decay Update Interval`: 60.0 (обновлять decay раз в минуту)

### Шаг 3: Создание DialogueDataAsset с использованием памяти

#### Пример 1: Первая встреча vs Повторная встреча

**Node: "Greeting"** (Start Node)

```
Conditions:
?? Memory Count Check
?  ?? Memory Type: Dialogue Event
?  ?? Min Count: 0
?  ?? Max Count: 0
?
Dialogue Text: "Hello, stranger. I haven't seen you around here before."

Effects:
?? Create Memory
   ?? Memory Type: Dialogue Event
   ?? Description: "First meeting with player"
   ?? Importance: 50.0
   ?? Emotion: Neutral
   ?? Remember Player: true
```

**Node: "Greeting_Repeat"** (Start Node Alternative)

```
Conditions:
?? Memory Count Check
?  ?? Memory Type: Dialogue Event
?  ?? Min Count: 1
?
Dialogue Text: "Oh, hello again! Good to see you."

Effects:
?? Discuss Topic
   ?? Topic Name: "Greeting"
   ?? Interest Modifier: -2.0  // Чуть меньше интереса к повторным приветствиям
```

#### Пример 2: Обещание и его проверка

**Node: "Request_Help"**

```
Dialogue Text: "Could you help me find my lost ring? It's very precious to me."

Choices:
?? "I'll help you" ? [Accept_Promise]
?? "Sorry, I'm busy" ? [Decline_Help]
```

**Node: "Accept_Promise"**

```
Dialogue Text: "Thank you so much! I knew I could count on you!"

Effects:
?? Remember Promise
?  ?? Promise ID: "FindLostRing"
?  ?? Description: "Player promised to find my lost ring"
?  ?? Importance: 90.0
?  ?? Related Tags: Quest.FindRing
?
?? Modify Affinity: +10.0
```

**Node: "Check_Promise"** (В следующей встрече)

```
Conditions:
?? Remembers Event
   ?? Memory ID: "FindLostRing"
   ?? Min Importance: 80.0

Dialogue Text: "Have you found my ring yet?"

[Если игрок принес кольцо - переход к награде]
```

#### Пример 3: Секрет игрока

**Node: "Reveal_Secret"**

```
Player Choice: "I have to tell you something... I'm actually a prince in disguise."

Effects:
?? Remember Secret
?  ?? Secret ID: "PlayerRoyalty"
?  ?? Description: "Player revealed they are royalty"
?  ?? Importance: 95.0
?  ?? Secret Tags: Secret.Identity, Important
?
?? Modify Trust: +20.0
```

**Node: "Reference_Secret"** (Позже в другом диалоге)

```
Conditions:
?? Remembers Event
   ?? Memory ID: "PlayerRoyalty"
   ?? Min Freshness: 0.3

Dialogue Text: "Your secret is safe with me, your highness." [Подмигивает]
```

#### Пример 4: Реакция на подарок

**Node: "Receive_Gift"** (When player gives item)

```
Effects:
?? Create Memory
?  ?? Memory Type: Item Given
?  ?? Description: "Player gave me beautiful flowers"
?  ?? Importance: 70.0
?  ?? Emotion: Joyful
?  ?? Context Tags: Event.Gift, Item.Flowers
?
?? Modify Affinity: +15.0
?? Modify Romance: +5.0
```

**Node: "Remember_Gift"** (Next dialogue)

```
Conditions:
?? Has Memories With Tags
   ?? Required Tags: Event.Gift, Item.Flowers
   ?? Require All Tags: true
   ?? Min Memories Found: 1

Dialogue Text: "I still have those beautiful flowers you gave me. They brighten my day every time I look at them!"

Effects:
?? Recall Memory
   ?? Memory ID: [Gift Memory ID]  // Укрепляет память
```

#### Пример 5: Травматичное событие

**Node: "Betrayal"**

```
Player Choice: "I told the guards about your plans."

Effects:
?? Create Memory
?  ?? Memory Type: Player Choice
?  ?? Description: "Player betrayed me to the guards"
?  ?? Importance: 95.0
?  ?? Emotion: Traumatic
?  ?? Context Tags: Event.Betrayal, Emotion.Anger
?
?? Modify Affinity: -40.0
?? Modify Trust: -60.0
?? Modify Fear: +30.0
```

**Node: "After_Betrayal"** (Все будущие встречи)

```
Conditions:
?? Has Memories With Tags
   ?? Required Tags: Event.Betrayal
   ?? Min Memories Found: 1

Dialogue Text: "You... Stay away from me. I can never trust you again."

[Блокирует большинство диалогов]
```

### Шаг 4: Динамическая генерация приветствий

Создайте функцию в Blueprint NPC:

**Function: "Generate Dynamic Greeting"**

```blueprint
Get Memory Component ? Get Recent Memories (Count: 1)
?
?? If Array Length > 0
?  ?
?  ?? Switch on Emotion
?     ?? Joyful ? Set Greeting: "It's wonderful to see you again!"
?     ?? Traumatic ? Set Greeting: "What do you want?"
?     ?? Nostalgic ? Set Greeting: "Ah, you remind me of better times..."
?     ?? Default ? Set Greeting: "Hello."
?
?? Else ? Set Greeting: "Hello, stranger."
```

### Шаг 5: Проверка выполнения обещаний

**Function: "Check Broken Promises"**

```blueprint
Get Memory Component ? Get Memories By Type (Type: Promise)
?
?? For Each Memory
   ?
   ?? Check if Quest Completed [Your Quest System Logic]
   ?
   ?? If NOT Completed AND Age > 7 Days
   ?  ?
   ?  ?? Effects:
   ?     ?? Modify Trust: -10.0
   ?     ?? Modify Affinity: -5.0
   ?     ?? Create Memory:
   ?        ?? Type: Player Choice
   ?        ?? Description: "Player broke their promise"
   ?        ?? Emotion: Regretful
   ?        ?? Importance: 80.0
   ?
   ?? If Completed
      ?? Effects:
         ?? Modify Trust: +20.0
         ?? Create Memory:
            ?? Type: Player Choice
            ?? Description: "Player kept their promise"
            ?? Emotion: Positive
            ?? Importance: 75.0
```

## Расширенные примеры

### Пример 6: Система влияния на отношения через память

**Event Graph Node: "Update Relationship Based On Memories"**

```blueprint
Event: Every 60 seconds (Timer)
?
Get Memory Component ? Get Memories About Actor (Player)
?
?? Count Positive Emotions
?? Count Negative Emotions
?
?? Calculate Ratio = (Positive - Negative) / Total
?
?? If Ratio > 0.5
?  ?? Slowly increase Affinity (+1 per minute)
?
?? Else If Ratio < -0.5
   ?? Slowly decrease Affinity (-1 per minute)
```

### Пример 7: Комплексное условие для романтики

**Node: "Romance_Available"**

```
Conditions:
?? Memory Emotional Balance
?  ?? Min Positive Memories: 5
?  ?? Max Negative Memories: 1
?
?? Has Memories With Tags
?  ?? Required Tags: Event.Gift, Event.Compliment
?  ?? Require All Tags: false
?  ?? Min Memories Found: 3
?
?? Topic Discussed
?  ?? Topic Name: "Family"
?  ?? Min Discussion Count: 2
?
?? Affinity Check: >= 75

Dialogue Text: "You know... I've really enjoyed spending time with you..."
```

### Пример 8: Реакция на повторяющиеся темы

**Node: "Topic_Politics"**

```
Conditions:
?? Topic Discussed
   ?? Topic Name: "Politics"
   ?? Min Discussion Count: 0
   ?? Max Discussion Count: 0

Dialogue Text: "Politics? It's a complicated topic..."

Effects:
?? Discuss Topic
   ?? Topic Name: "Politics"
   ?? Interest Modifier: -5.0
```

**Node: "Topic_Politics_Repeat"**

```
Conditions:
?? Topic Discussed
   ?? Topic Name: "Politics"
   ?? Min Discussion Count: 2

Dialogue Text: "Can we talk about something else? Politics bore me."

Effects:
?? Discuss Topic
   ?? Topic Name: "Politics"
   ?? Interest Modifier: -10.0
```

## Советы по дизайну

### 1. Баланс важности

```
0-30:   Малозначимые события (обычные приветствия)
31-60:  Средней важности (интересные разговоры)
61-85:  Важные события (подарки, помощь)
86-100: Критические события (обещания, секреты, предательства)
```

### 2. Использование тегов

Организуйте теги иерархически:

```
Event
?? Event.Gift
?  ?? Event.Gift.Flower
?  ?? Event.Gift.Jewelry
?? Event.Help
?? Event.Betrayal

Emotion
?? Emotion.Happy
?? Emotion.Sad
?? Emotion.Angry

Topic
?? Topic.Family
?? Topic.Politics
?? Topic.Romance

Quest
?? Quest.Main
?? Quest.Side
```

### 3. Комбинирование систем

```
Подарок ? Memory + Affinity + Romance
Обещание ? Memory + Trust
Секрет ? Memory + Trust + [Unlock new dialogues]
Предательство ? Memory + Fear + Affinity decrease + [Block dialogues]
```

### 4. Тестирование

Создайте debug функцию для тестирования:

**Function: "Debug Memory System"**

```blueprint
Print String: "=== NPC Memory Debug ==="

Get Memory Component
?? Get Memory Count
?  ?? Print: "Total Memories: [Count]"
?
?? Get All Memories
?  ?? For Each
?     ?? Print: "[Type] - [Description] - Importance: [Value]"
?
?? Get All Topics
   ?? For Each
      ?? Print: "Topic: [Name] - Interest: [Value] - Count: [Count]"
```

## Частые проблемы и решения

### Проблема: Память заполняется слишком быстро

**Решение:**
- Уменьшите `Max Memories`
- Увеличьте `Base Decay Rate`
- Используйте `Prune Old Memories` периодически

### Проблема: Воспоминания забываются слишком быстро

**Решение:**
- Увеличьте `Memory Retention Multiplier`
- Уменьшите `Base Decay Rate`
- Увеличьте важность событий
- Используйте эмоциональную окраску

### Проблема: NPC не помнит важные события

**Решение:**
- Проверьте что `Enable Memory Decay` не отключает важные события
- Увеличьте `Importance` события (>= 70)
- Используйте `Recall Memory` в диалогах для укрепления памяти

### Проблема: Производительность

**Решение:**
- Увеличьте `Decay Update Interval`
- Уменьшите `Max Memories`
- Рассмотрите отключение `Enable Memory Decay` для фоновых NPC

## Интеграция с Quest System

Пример интеграции с вашей квест-системой:

**Quest Begin:**
```blueprint
On Quest Started
?? NPC Memory Component ? Create Memory
   ?? Type: Quest Event
   ?? Description: "Started [Quest Name]"
   ?? Importance: 70.0
   ?? MetaData: {"QuestId": "[Quest ID]"}
```

**Quest Complete:**
```blueprint
On Quest Completed
?? NPC Memory Component ? Find Memories With Tags (Quest.[QuestId])
   ?? Modify Memory Importance: +20.0
   ?? Create New Memory:
      ?? Type: Player Choice
      ?? Description: "Completed [Quest Name]"
      ?? Emotion: Joyful
      ?? Importance: 85.0
```

**Quest Failed:**
```blueprint
On Quest Failed
?? NPC Memory Component ? Create Memory
   ?? Type: Player Choice
   ?? Description: "Failed [Quest Name]"
   ?? Emotion: Negative
   ?? Importance: 70.0
   ?? Effect: Modify Trust: -15.0
```

## Заключение

NPCMemoryComponent - это мощный инструмент для создания живых, запоминающихся NPC. Экспериментируйте с различными комбинациями памяти, эмоций и отношений для создания уникальных персонажей!
