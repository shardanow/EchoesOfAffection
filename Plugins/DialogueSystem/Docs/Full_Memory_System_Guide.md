# ?? Full Memory System - Complete Guide

> **Version:** v1.5 - Full Memory Support  
> **Status:** ? Compiled Successfully  
> **Date:** 2025-01-23

---

## ? **Что Реализовано:**

Теперь в системе **5 классов** для работы с воспоминаниями:

```
Plugins/DialogueSystem/Source/DialogueSystemCore/Effects/

? UDialogueEffect_AddMemory             // Простое (только тег)
? UDialogueEffect_CreateMemory    // ?? Полное (текст + детали)
? UDialogueEffect_ModifyMemoryStrength
? UDialogueEffect_RemoveMemory
? UDialogueEffect_RefreshMemory
```

---

## ?? **Два Подхода к Воспоминаниям:**

### **1. Простой (`AddMemory`) - для флагов и быстрых событий**

```cpp
UDialogueEffect_AddMemory
?? MemoryTag: FGameplayTag    // "Event.Help"
?? MemoryStrength: float       // 0.85
```

**Когда использовать:**
- ? Флаги событий ("Player met Anna")
- ? Бинарные факты ("Village saved")
- ? Быстрые прототипы
- ? Простые условия диалогов

**Пример:**
```
DialogueNode: "Nice to meet you!"
?? Effect: AddMemory
   ?? Tag: Character.Anna.Met
   ?? Strength: 1.0
```

---

### **2. Полный (`CreateMemory`) - для сюжетных моментов** ??

```cpp
UDialogueEffect_CreateMemory
?? Description: FText          // "The traveler saved my life..."
?? Importance: float     // 90.0 (0-100)
?? ContextTags: TagContainer   // [Event.Rescue, Location.Village]
?? bLinkToPlayer: bool         // true
```

**Когда использовать:**
- ? Важные сюжетные моменты
- ? Эмоциональные реакции NPC
- ? Персонализированные воспоминания
- ? Система репутации/отношений
- ? Квесты с последствиями

**Пример:**
```
DialogueNode: "You betrayed me!"
?? Effect: CreateMemory
   ?? Description: "This scoundrel revealed my secret to the guards. I trusted them, and they stabbed me in the back. I will never forgive this."
   ?? Importance: 95.0
   ?? Context Tags:
   ?  ?? Event.Betrayal
   ?  ?? Location.Village
   ?  ?? NPC.Guard
   ?? Link To Player: ? Yes
```

---

## ?? **Практические Примеры:**

### **Пример 1: Предательство (Полная Детализация)**

```
DialogueNode: "I told the guards about your secret"
?? Effects:
   ?? CreateMemory (Full)
   ?  ?? Description: "The traveler I trusted betrayed me to the guards. They revealed the location of my hideout. Because of them, my friends were arrested. I will never trust outsiders again."
   ?  ?? Importance: 95.0
   ?  ?? Context Tags:
   ?  ?  ?? Event.Betrayal
   ?  ?  ?? Location.Hideout
   ?  ?  ?? NPC.Guard
   ?  ?  ?? Emotion.Traumatic
   ?  ?? Link To Player: ? Yes
   ?
 ?? ModifyAffinity
      ?? NPC: CurrentNPC
      ?? Delta: -60
```

**Результат:**
```cpp
FNPCMemoryEntry
{
Description: "The traveler I trusted betrayed me...",
    Importance: 95.0,
    ContextTags: [Event.Betrayal, Location.Hideout, NPC.Guard, Emotion.Traumatic],
    RelatedActor: Player,
    Timestamp: 2025-01-23 23:25:00,
    Freshness: 100%
}
```

**Decay:**
- Importance 95 ? decay на 98% медленнее
- Emotion.Traumatic ? decay еще на 50% медленнее
- **Итог:** NPC будет помнить 10+ лет игрового времени!

---

### **Пример 2: Спасение Деревни (Героический Поступок)**

```
DialogueNode: "Thank you for saving us!"
?? Effects:
   ?? CreateMemory (Full)
   ?  ?? Description: "This brave hero defended our village when bandits attacked. They risked their life for people they barely knew. We owe them everything."
   ?  ?? Importance: 90.0
   ?  ?? Context Tags:
   ?  ?  ?? Event.Rescue
   ?  ?  ?? Event.Combat
   ?  ?  ?? Location.Village
   ?  ?  ?? Enemy.Bandits
   ?  ?  ?? Emotion.Grateful
   ?  ?? Link To Player: ? Yes
   ?
   ?? AddMemory (Simple)
   ?  ?? Tag: Quest.VillageDefense.Complete
   ?  ?? Strength: 1.0
   ?
   ?? ModifyAffinity
      ?? Delta: +50
```

**Два воспоминания созданы:**

1. **Полное воспоминание** (CreateMemory):
   - Персонализированный текст
   - Высокая важность
   - Множество тегов для условий
   - Связано с игроком

2. **Простое воспоминание** (AddMemory):
   - Флаг квеста для условий
   - Быстрая проверка в диалогах

---

### **Пример 3: Разные NPC - Разные Воспоминания**

#### **Anna (добрый NPC):**

```
DialogueNode: "You helped me!"
?? Effect: CreateMemory
   ?? Description: "This kind stranger helped me find my lost daughter. I don't know what I would have done without them. They have a good heart."
   ?? Importance: 85.0
   ?? Tags: [Event.Help, Quest.FindDaughter, Emotion.Grateful]
   ?? Link To Player: ? Yes
```

#### **Guard (нейтральный NPC):**

```
DialogueNode: "Job done"
?? Effect: CreateMemory
   ?? Description: "Mercenary completed the patrol mission. Acceptable performance. Nothing special."
?? Importance: 40.0
   ?? Tags: [Event.Help, Job.Patrol]
   ?? Link To Player: ? Yes
```

#### **Bandit Leader (враждебный NPC):**

```
DialogueNode: "You killed my men!"
?? Effect: CreateMemory
   ?? Description: "This dog slaughtered my best fighters. They will pay for this. I will hunt them down and make them suffer."
   ?? Importance: 90.0
   ?? Tags: [Event.Combat, Enemy.Player, Emotion.Vengeful]
   ?? Link To Player: ? Yes
```

**Одно событие - три совершенно разных воспоминания!**

---

### **Пример 4: Обещание (Promise System)**

```
DialogueNode: "I promise I'll find your sister"
?? Effects:
   ?? CreateMemory (Full)
?  ?? Description: "The traveler promised to find my sister Sarah who went missing in the forest. They looked me in the eyes and gave their word. I choose to believe them."
   ?  ?? Importance: 80.0
   ?  ?? Tags:
   ?  ?  ?? Promise.FindSister
 ?  ?  ?? Character.Sarah
   ?  ?  ?? Location.Forest
   ?  ?  ?? Emotion.Hopeful
   ?  ?? Link To Player: ? Yes
   ?
   ?? StartQuest
      ?? QuestID: FindSarahQuest
```

**Позже, если квест провален:**

```
DialogueNode: "You lied to me..."
?? Effects:
   ?? CreateMemory (Full)
   ?  ?? Description: "They promised to find Sarah, but they never did. Either they forgot, or they lied from the start. My sister is still missing because of their broken promise."
   ?  ?? Importance: 85.0
   ?  ?? Tags:
   ?  ??? Promise.Broken
   ?  ?  ?? Character.Sarah
   ?  ?  ?? Emotion.Betrayed
   ?  ?? Link To Player: ? Yes
   ?
   ?? ModifyTrust
      ?? Delta: -70
```

---

## ?? **Сравнение Подходов:**

| Аспект | AddMemory (Simple) | CreateMemory (Full) |
|--------|-------------------|---------------------|
| **Описание** | ? Нет (только тег) | ? Кастомный текст |
| **Важность** | ?? Через strength | ? Прямой параметр (0-100) |
| **Теги** | ?? 1 тег | ? Множество тегов |
| **Связь с актором** | ? Нет | ? Player/другой NPC |
| **Персонализация** | ? Одинаково | ? Уникально для NPC |
| **UI/Debug** | ? Только ID | ? Читаемый текст |
| **Скорость setup** | ??? Быстро | ?? Средне |
| **Лучше для** | Флаги, условия | Сюжет, эмоции |

---

## ?? **Guidelines: Когда Что Использовать**

### **Используй `AddMemory` (Simple):**

```
? Флаги встреч:     "Player met Anna"
? Квестовые флаги:  "Quest started/completed"
? Локации:    "Player visited village"
? Простые условия:  "if HasMemoryWithTag(Character.Anna.Met)"
? Прототипирование: Быстрая проверка идеи
```

### **Используй `CreateMemory` (Full):**

```
? Важные выборы:      Предательство, жертва, спасение
? Эмоциональные моменты: Романтика, конфликты, примирение
? Обещания: Данные/нарушенные обещания
? Квесты с последствиями: Успех/провал влияет на отношения
? Персонализация NPC: Разные NPC видят событие по-разному
? Система репутации:  Детальный учет действий игрока
```

---

## ?? **Параметры CreateMemory (Детально):**

### **Description (FText):**

```
? ПЛОХО:  "Player helped"
? ХОРОШО: "This brave stranger defended our village when bandits attacked at dawn. They fought alongside the guards and saved countless lives. We will remember this day forever."
```

**Советы:**
- Пиши от лица NPC (первое лицо)
- Добавляй детали (где, когда, как)
- Включай эмоции NPC
- Делай уникальным для каждого NPC

---

### **Importance (0-100):**

```
0-30:   Мелочи
 - Casual conversation
        - Small favors
     - Routine interactions

31-60:  Значимые события
   - Quest help
        - Gifts
        - Minor conflicts

61-90:  Важные моменты
        - Betrayal/rescue
        - Major quest outcomes
        - Character development

91-100: Жизнеопределяющие
      - Traumatic events
        - Life-saving actions
        - Romance milestones
```

**Влияние Importance:**
- Decay rate (чем выше - тем медленнее забывается)
- Приоритет в AI (более важные влияют сильнее)
- Вероятность упоминания в диалогах

---

### **Context Tags:**

```
Рекомендованная структура:

Event.*     - Что произошло
?? Event.Help
?? Event.Betrayal
?? Event.Combat
?? Event.Gift
?? Event.Rescue

Quest.*     - Связь с квестами
?? Quest.[QuestID].Start
?? Quest.[QuestID].Complete
?? Quest.[QuestID].Failed

Promise.*   - Обещания
?? Promise.KeepSecret
?? Promise.FindSister
?? Promise.Revenge
?? Promise.Broken

Emotion.*   - Эмоциональный контекст
?? Emotion.Joyful
?? Emotion.Traumatic
?? Emotion.Grateful
?? Emotion.Vengeful
?? Emotion.Hopeful

Location.*  - Где произошло
?? Location.Village
?? Location.Forest
?? Location.Cave

Character.* - Кто участвовал
?? Character.[Name].Met
?? Character.[Name].Friend
?? Character.[Name].Enemy
```

**Используй 3-5 тегов на воспоминание для лучшей фильтрации!**

---

### **Link To Player:**

```
? TRUE:  Действия игрока, обещания, отношения
? FALSE: Наблюдения, мысли NPC, окружение
```

**Примеры:**
- ? Link: "Player saved me"
- ? Link: "Player betrayed my trust"
- ? No Link: "I saw a beautiful sunset"
- ? No Link: "The village was attacked"

---

## ?? **Использование в Dialogue Conditions:**

### **С Simple Memory:**

```cpp
Condition: HasMemoryWithTag("Event.Betrayal")

// В диалоге:
if (HasMemoryWithTag(Event.Betrayal))
{
    ShowDialogue("You... how dare you come back here!");
}
```

### **С Full Memory:**

```cpp
// Проверка важности
Condition: HasMemoryWithImportance("Event.Betrayal", 80.0f)

// Проверка множества тегов
Condition: HasMemoryWithAllTags([Event.Help, Location.Village])

// Проверка связи с игроком
Condition: HasMemoryAboutPlayer("Event.Rescue")
```

---

## ?? **Workflow: Создание Воспоминания**

### **1. Определи Тип:**

```
Это флаг события? ? AddMemory
Это важный сюжетный момент? ? CreateMemory
```

### **2. Напиши Описание (для CreateMemory):**

```
Вопросы для помощи:
- Как NPC видит это событие?
- Какие детали запомнятся NPC?
- Какие эмоции испытывает NPC?
- Как это изменит NPC?
```

### **3. Установи Importance:**

```
Спроси себя:
- Забудет ли NPC это через неделю? ? 20-40
- Вспомнит ли через месяц? ? 50-70
- Никогда не забудет? ? 80-100
```

### **4. Добавь Теги:**

```
Minimum 3 тега:
1. Event.* (что произошло)
2. Location.* или Character.* (контекст)
3. Emotion.* (как NPC это воспринял)
```

### **5. Тестируй:**

```
1. Создай воспоминание в диалоге
2. Проверь в debug UI (если есть)
3. Проверь условия диалогов
4. Тестируй decay со временем
```

---

## ?? **Следующие Шаги:**

### **1. Создай Gameplay Tags:**

```
Project Settings ? GameplayTags

Event.*
?? Event.Help
?? Event.Betrayal
?? Event.Gift
?? Event.Rescue
?? Event.Combat

Quest.*
?? Quest.VillageDefense.Complete
?? ...

Promise.*
?? Promise.FindSister
?? ...

Emotion.*
?? Emotion.Joyful
?? Emotion.Traumatic
?? ...
```

### **2. Создай Dialogue Data Asset:**

```
Content Browser ? Right Click
? Dialogue ? Dialogue Data Asset

Add DialogueNode
? Effects ? Add Element
? Choose: CreateMemory или AddMemory
```

### **3. Настрой Параметры:**

```
CreateMemory:
?? Description: "Персонализированный текст"
?? Importance: 70.0
?? Context Tags: [Event.Help, Location.Village]
?? Link To Player: ?
```

---

## ?? **Итог:**

### **Теперь У Тебя Есть:**

```
? 5 классов для работы с воспоминаниями
? Простой подход (AddMemory) для флагов
? Полный подход (CreateMemory) для сюжета
? Персонализированные описания для каждого NPC
? Система важности и decay
? Множественные теги для фильтрации
? Связь воспоминаний с акторами
```

### **Используй:**

- **AddMemory** - для 70% случаев (флаги, условия)
- **CreateMemory** - для 30% случаев (важные моменты)

**Комбинируй оба подхода для максимальной гибкости!**

---

**Документ подготовлен:** 2025-01-23  
**Версия:** 1.0  
**Статус:** ? Complete & Compiled

**Вывод:** Полная система воспоминаний с текстом, важностью и тегами реализована и готова к использованию! ????
