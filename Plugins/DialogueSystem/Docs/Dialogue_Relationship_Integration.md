# Dialogue ? Relationship Integration Guide

## Overview

Этот гайд объясняет, как использовать новые Effects и Conditions для интеграции DialogueSystem с RelationshipSystem.

---

## ?? Dialogue Effects для Relationship

### 1. **DialogueEffect_ModifyRelationship**

Позволяет напрямую изменять отношения в диалогах.

#### Режимы работы:

##### Mode: Execute Action (рекомендуется)

Выполняет действие, настроенное в Data Asset.

**Пример в Editor:**
```
Node: "Accept Gift"
?? Effects:
    ?? Modify Relationship
        ?? Mode: Execute Action
        ?? Action Tag: Relationship.Action.Gift
```

**В DA_Action_Gift настроено:**
- Trust: +10
- Affinity: +15
- Romance: +5

---

##### Mode: Direct Modify Dimensions

Прямое изменение dimensions без Data Asset.

**Пример:**
```
Node: "Promise to Help"
?? Effects:
    ?? Modify Relationship
        ?? Mode: Direct Modify
      ?? Dimension Modifiers:
       ?? [0] Trust: +25.0
            ?? [1] Affinity: +10.0
            ?? [2] Romance: +5.0
```

---

##### Mode: Add Trait

Добавляет trait к отношениям.

**Пример:**
```
Node: "Reveal Dark Secret"
?? Effects:
 ?? Modify Relationship
   ?? Mode: Add Trait
    ?? Trait Tag: Relationship.Trait.KnowsSecret
```

---

##### Mode: Force State Transition

Принудительно переводит отношения в новое состояние.

**Пример:**
```
Node: "Betray NPC"
?? Effects:
    ?? Modify Relationship
    ?? Mode: Force State
        ?? Target State: Relationship.State.Enemy
```

---

## ?? Dialogue Conditions для Relationship

### 1. **DialogueCondition_CheckRelationship**

Проверяет состояние отношений для условных веток диалога.

#### Режимы проверки:

##### Check Dimension Value

Проверяет значение dimension.

**Пример:**
```
Node: "Romantic Branch" (conditional)
?? Conditions:
    ?? Check Relationship
        ?? Mode: Check Dimension
        ?? Dimension Tag: Relationship.Dimension.Romance
     ?? Comparison: >= (GreaterOrEqual)
        ?? Threshold: 60.0
```

Эта ветка откроется только если Romance >= 60.

---

##### Check Current State

Проверяет текущее состояние.

**Пример:**
```
Node: "Friend Greeting" (conditional)
?? Conditions:
    ?? Check Relationship
   ?? Mode: Check State
        ?? State Tag: Relationship.State.Friend
```

---

##### Check Has Trait

Проверяет наличие trait.

**Пример:**
```
Node: "Reference Secret" (conditional)
?? Conditions:
  ?? Check Relationship
        ?? Mode: Check Trait
        ?? Trait Tag: Relationship.Trait.KnowsSecret
```

---

## ?? Полный пример диалога

### DA_Dialogue_RoseGift

```
Root Node: "Greeting"
?? Text: "Hello there!"
?? Responses:
    ?? [0] "I brought you a rose"
    ?   ?? Effects:
 ?       ?? Create Memory
    ?       ?   ?? Type: ItemGiven
    ?       ?   ?? Description: "Player gave me a rose"
    ?       ?   ?? Importance: 85.0
 ?       ?   ?? Emotion: Joyful
    ?       ?
  ?       ?? Modify Relationship
    ?     ?? Mode: Execute Action
    ?           ?? Action: Relationship.Action.GiftFlower
    ?
    ?   ?? Next Node: "Thanks"
    ?
    ?? [1] "Just passing by"
        ?? Next Node: END

Node: "Thanks"
?? Conditions:
?   ?? Check Relationship
?       ?? Mode: Check Dimension
?       ?? Dimension: Relationship.Dimension.Romance
?       ?? Comparison: >=
?       ?? Threshold: 50.0
?
?? Text (if Romance >= 50): "Oh! This is so sweet of you! *blushes*"
?? Text (else): "Thank you, that's very kind."
```

---

## ?? Memory ? Relationship Auto-Sync (опционально)

### Включить автоматическое влияние памяти на отношения:

**В NPCMemoryComponent:**

```
Details Panel ? Integration
?? Auto Affect Relationships: ? (enabled)
?? Min Importance For Sync: 70.0
```

**Что это делает:**

Когда создаётся важная память (Importance >= 70):
1. NPCMemoryComponent ? BroadcastEvent(GameEventBus)
2. RelationshipSystemBridge ? Слушает событие
3. Читает DA_MemoryMapping
4. Автоматически применяет эффект к отношениям

**DA_MemoryMapping (пример конфигурации):**

```
Content/Data/Integration/DA_MemoryRelationshipMapping

Memory Type Actions:
?? [0] Promise ? Relationship.Action.Promise
?? [1] ItemGiven ? Relationship.Action.Gift
?? [2] Secret ? Relationship.Action.ShareSecret
?? [3] PlayerChoice ? (no action, handle manually)

Emotion Mappings:
?? [0] Traumatic
?   ?? Modifiers:
?       ?? Trust: ?-0.5
?       ?? Fear: ?0.3
? ?? Affinity: ?-0.4
?
?? [1] Joyful
    ?? Modifiers:
   ?? Affinity: ?0.4
        ?? Trust: ?0.2
        ?? Romance: ?0.1

Settings:
?? Enable Memory ? Relationship: ?
?? Enable Relationship ? Memory: ?
?? Global Min Importance: 60.0
?? Min Freshness: 0.4
```

---

## ?? Best Practices

### 1. **Используйте Execute Action когда возможно**

? **Хорошо:**
```
Modify Relationship
?? Mode: Execute Action
    ?? Action: Relationship.Action.Gift
```

? **Плохо:**
```
Modify Relationship
?? Mode: Direct Modify
    ?? Trust: +10
    ?? Affinity: +15
    ?? Romance: +5
```

**Почему?** 
- Actions настраиваются в Data Assets
- Легко балансить
- Переиспользование логики

---

### 2. **Создавайте воспоминания о важных событиях**

Всегда добавляйте Create Memory перед Modify Relationship:

```
Effects:
?? [0] Create Memory  ? Сначала память!
?? [1] Modify Relationship  ? Потом эффект
```

---

### 3. **Используйте Conditions для веток**

```
Node: "Flirt" (conditional)
?? Conditions:
    ?? Check Relationship (Romance >= 40)
    ?? Check Relationship (State != Enemy)
```

---

### 4. **Auto-Sync только для фоновых эффектов**

Включайте `bAutoAffectRelationships` только если:
- Много повторяющихся воспоминаний
- Нужны фоновые изменения
- Не критично точное управление

Для **сюжетных** событий лучше явно указывать Effects в диалоге.

---

## ? Soft Dependency

**Важно:** Вся интеграция работает через soft dependency!

- DialogueSystem может работать **без** RelationshipSystem
- Если RelationshipSystem не загружен:
  - Effects просто не выполняются
  - Conditions всегда возвращают `false`
  - Никаких ошибок/крашей

**Проверка в логах:**
```
LogTemp: Verbose: DialogueCondition_CheckRelationship: RelationshipComponent not found (plugin may not be enabled)
```

---

## ?? Тестирование

### Тест 1: Проверка Effect

1. Создайте тестовый диалог
2. Добавьте Modify Relationship effect
3. Запустите диалог
4. Откройте Debug UI RelationshipSystem
5. Проверьте изменения dimensions

### Тест 2: Проверка Condition

1. Установите Trust = 30 через Debug UI
2. Создайте условие: Trust >= 50
3. Ветка должна быть **закрыта**
4. Измените Trust = 60
5. Ветка должна **открыться**

### Тест 3: Auto-Sync

1. Включите `bAutoAffectRelationships` на NPC
2. Создайте память через Effect
3. Проверьте лог:
   ```
   NPCMemory: Broadcast memory event to GameEventBus
   RelationshipBridge: Memory 'Promise' triggered action...
   ```

---

## ?? Troubleshooting

### Effect не работает

**Причины:**
1. RelationshipSystem плагин отключён ? включите
2. NPC не имеет NPCRelationshipComponent ? добавьте
3. ActionTag неверный ? проверьте GameplayTags

### Condition всегда false

**Причины:**
1. RelationshipComponent отсутствует
2. Target не тот actor
3. `bCheckPlayerRelationship` включён/выключён неправильно

### Auto-Sync не работает

**Причины:**
1. `bAutoAffectRelationships = false` ? включите
2. Importance < MinImportanceForSync
3. GameEventBus плагин не загружен
4. Tag "NPC.Memory.Created" не существует в GameplayTags

---

## ?? Summary

| Feature | File | Purpose |
|---------|------|---------|
| **Modify Relationship Effect** | DialogueEffect_ModifyRelationship | Изменить отношения в диалоге |
| **Check Relationship Condition** | DialogueCondition_CheckRelationship | Условные ветки по отношениям |
| **Memory Auto-Sync** | NPCMemoryComponent | Автоматическое влияние памяти |
| **Mapping Config** | DA_MemoryRelationshipMapping | Настройка авто-эффектов |

**Рекомендуемый workflow:**
1. Настройте Actions в Data Assets
2. Используйте Effects в диалогах
3. Добавьте Conditions для веток
4. Опционально: включите Auto-Sync для фона

---

**Happy Relationship Building! ??**
