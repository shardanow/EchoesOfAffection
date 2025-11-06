# ?? POSSESSABLE vs SPAWNABLE: КРИТИЧЕСКИ ВАЖНО!

## ?? **ПРОБЛЕМА**

### **Ваша ситуация:**

Акторы в уровне имеют **ДИНАМИЧЕСКИЕ позиции** (гуляют, перемещаются по расписанию):

```
Level (Runtime):
?? BP_Lia: Location = (X=500, Y=300, Z=100) // Гуляет по деревне
?? BP_Ellen: Location = (X=1200, Y=800, Z=100) // В таверне
?? BP_Shopkeeper: Location = (X=200, Y=100, Z=100) // В магазине
```

Но для **Sequence** (диалога) нужны другие позиции:

```
Sequence "LS_AwkwardMeeting":
?? Lia: Location = (X=100, Y=100, Z=100) // Площадь
?? Ellen: Location = (X=-500, Y=200, Z=100) // Off-screen
?? Shopkeeper: НЕ участвует
```

---

## ? **ЧТО ПРОИСХОДИТ С POSSESSABLE**

### **Possessable = "Захват" актора из уровня**

```
Sequencer ? + Track ? Actor to Sequencer (Possessable)
?? Sequencer ЗАХВАТЫВАЕТ существующего актора из Level
?? Изменения в Sequencer НАПРЯМУЮ МЕНЯЮТ актора в уровне
?? После Sequence актор ОСТАЁТСЯ на новой позиции
```

### **Пример проблемы:**

```
BEFORE Sequence:
Level: BP_Lia at (X=500, Y=300, Z=100) // Гуляет

SEQUENCER (Possessable):
Lia Transform ? (X=100, Y=100, Z=100) // Для диалога

AFTER Sequence:
Level: BP_Lia at (X=100, Y=100, Z=100) ? ИЗМЕНИЛОСЬ ГЛОБАЛЬНО!
```

**Результат:**
- ? Lia телепортировалась на площадь после диалога
- ? Её AI schedule сломан
- ? Игрок видит "телепорт"

---

## ? **РЕШЕНИЕ 1: SPAWNABLE ACTORS (РЕКОМЕНДУЮ)**

### **Spawnable = "Временная копия"**

```
Sequencer ? + Track ? Actor ? Spawnable
?? Sequencer СОЗДАЁТ временную копию актора
?? Копия существует ТОЛЬКО во время Sequence
?? После Sequence копия УДАЛЯЕТСЯ
?? Оригинальный актор в уровне НЕ ТРОГАЕТСЯ
```

### **Как использовать:**

#### **1. В Sequencer:**

```
Sequencer window:
+ Track ? Actor ? "Spawnable"
Browse: BP_Lia blueprint
? Creates "Spawned_BP_Lia" ?
```

**Важно:** НЕ "Actor to Sequencer", а именно "Spawnable"!

#### **2. Настроить позицию для Sequence:**

```
Timeline: 0:00s
Spawned_BP_Lia ? Transform:
?? Location: (X=100, Y=100, Z=100) // Только для диалога
?? Rotation: (0, 0, 0)
?? Add Keyframe
```

#### **3. Результат:**

```
DURING Sequence:
?? Оригинальная Lia: (X=500, Y=300, Z=100) // Продолжает гулять
?? Spawned_BP_Lia: (X=100, Y=100, Z=100) // Временная копия для диалога
?? Camera фокусируется на Spawned_BP_Lia

AFTER Sequence:
?? Spawned_BP_Lia: УДАЛЕНА
?? Оригинальная Lia: (X=500, Y=300, Z=100) // Не изменилась!
```

### **Преимущества:**

? Оригинальные акторы не трогаются  
? AI schedule не ломается  
? Нет "телепортов"  
? Идеально для Dating Sim с динамическими NPC  

### **Недостатки:**

?? Spawnable = новый экземпляр (не тот же актор)  
?? State не передаётся (inventory, affinity, etc.)  
?? После Sequence нельзя сразу взаимодействовать с копией  

---

## ? **РЕШЕНИЕ 2: RESTORE TRANSFORMS (для Possessable)**

Если вам НУЖЕН тот же актор (для continuity, state):

### **Новая опция в DialogueEffect_PlaySequence:**

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
bool bRestoreActorTransforms = true;
```

### **Как работает:**

```
1. BEFORE Sequence:
   ?? Сохраняет оригинальные позиции всех Possessables
   ?? OriginalActorTransforms[BP_Lia] = (X=500, Y=300, Z=100)

2. DURING Sequence:
   ?? Possessable двигается как настроено
   ?? BP_Lia at (X=100, Y=100, Z=100)

3. AFTER Sequence:
   ?? Восстанавливает оригинальные позиции
   ?? BP_Lia restored to (X=500, Y=300, Z=100)
```

### **Использование:**

```
DialogueNode (Event):
Effect: "PlaySequence(/Game/Sequences/LS_Meeting, true, true, true)"
    ?     ?     ?
   wait  restore restore
         camera  actors
```

### **Параметры:**

| Параметр | Тип | Default | Описание |
|----------|-----|---------|----------|
| `path` | String | Required | Путь к Sequence |
| `waitForCompletion` | Bool | `true` | Ждать окончания |
| `restoreCamera` | Bool | `true` | Восстановить камеру |
| `restoreActors` | Bool | `true` | ? NEW! Восстановить позиции |

### **Преимущества:**

? Тот же актор (state сохраняется)  
? Позиции восстанавливаются автоматически  
? AI schedule не ломается  

### **Недостатки:**

?? Актор "телепортируется" на время Sequence (может быть заметно)  
?? Требует дополнительной обработки  

---

## ?? **КАКОЙ ВАРИАНТ ВЫБРАТЬ?**

### **Dating Sim / Visual Novel:**

| Сценарий | Рекомендация |
|----------|--------------|
| **Статичные диалоги** (персонажи не двигаются) | ? **SPAWNABLE** |
| **Кинематографичные сцены** (чистая визуализация) | ? **SPAWNABLE** |
| **Диалог с передачей предметов** (нужен тот же актор) | ? Possessable + **Restore** |
| **Сцена с изменением состояния** (affinity, etc.) | ? Possessable + **Restore** |
| **Простые камерные переходы** | ? **SPAWNABLE** |

### **RPG / Open World:**

| Сценарий | Рекомендация |
|----------|--------------|
| **NPC с AI Patrol** | ? **SPAWNABLE** |
| **Dynamic World** (NPC перемещаются) | ? **SPAWNABLE** |
| **Quest cutscenes** | ? **SPAWNABLE** |
| **Boss intro** (нужна та же entity) | ? Possessable + **Restore** |

---

## ?? **ПРАКТИЧЕСКИЕ ПРИМЕРЫ**

### **Пример 1: Dating Sim - Confession Scene (SPAWNABLE)**

```
Сцена: Признание на площади

Level (Runtime):
?? Lia: Гуляет по деревне (dynamic location)
?? Player: Контролируется игроком

Sequencer "LS_Confession":
?? Spawnable: Spawned_BP_Lia
?  ?? Location: Площадь (X=100, Y=100, Z=100)
?? Camera: Романтичный shot
?? Audio: Romantic music

DialogueNode:
Effect: "PlaySequence(/Game/Sequences/LS_Confession)"

Result:
?? Оригинальная Lia продолжает гулять где-то
?? Spawned_BP_Lia появляется на площади
?? Sequence проигрывается
?? После Sequence: spawned copy удаляется, dialogue продолжается с ОРИГИНАЛЬНОЙ Lia
```

**Проблема:** После Sequence игрок говорит с оригинальной Lia (которая может быть далеко!)

**Решение:** Телепортировать оригинальную Lia на площадь ПОСЛЕ Sequence:

```
DialogueNode (Event):
?? Effect 1: "PlaySequence(/Game/Sequences/LS_Confession)"
?? Effect 2: "TeleportActor(Lia, 100, 100, 100)" ? После Sequence
```

---

### **Пример 2: Visual Novel - Character Arrives (SPAWNABLE)**

```
Сцена: Ellen входит в комнату

Level (Runtime):
?? Ellen: В таверне (X=1200, Y=800, Z=100)
?? Player: В комнате (X=0, Y=0, Z=100)

Sequencer "LS_EllenArrive":
?? Spawnable: Spawned_BP_Ellen
?  ?? 0:00s: Off-screen (X=-500, Y=200, Z=100)
?  ?? 0:05s: In room (X=150, Y=100, Z=100)
?? Camera: Door ? Ellen
?? Animation: Walk_Forward

DialogueNode:
Effect: "PlaySequence(/Game/Sequences/LS_EllenArrive)"

Result:
?? Оригинальная Ellen остаётся в таверне
?? Spawned_BP_Ellen "заходит" в комнату (визуально)
?? После Sequence: dialogue продолжается (Ellen теперь "в комнате" логически)
```

**Note:** После Sequence нужно **логически** отметить, что Ellen теперь в комнате:

```
DialogueNode (Event):
?? Effect 1: "PlaySequence(/Game/Sequences/LS_EllenArrive)"
?? Effect 2: "SetMemory(EllenInRoom, true)"
?? Effect 3: "TeleportActor(Ellen, 150, 100, 100)" ? Телепорт оригинала
```

---

### **Пример 3: Item Exchange (POSSESSABLE + RESTORE)**

```
Сцена: Lia передаёт игроку розу

Level (Runtime):
?? Lia: Гуляет (X=500, Y=300, Z=100)
?? Player: (X=0, Y=0, Z=100)

Sequencer "LS_RoseGift":
?? Possessable: BP_Lia ? ТОТ ЖЕ АКТОР!
?  ?? Location: (X=100, Y=100, Z=100) // Рядом с Player
?? Item "Rose" attached to Lia's hand
?? Animation: Give_Item
?? Effect: AddInventory(Player, Rose, 1)

DialogueNode:
Effect: "PlaySequence(/Game/Sequences/LS_RoseGift, true, true, true)"
            ?
      restore actors

Result:
?? Lia телепортируется на (X=100, Y=100, Z=100)
?? Sequence проигрывается (роза передаётся)
?? Inventory updated (роза у Player)
?? Lia восстанавливается на (X=500, Y=300, Z=100)
```

**Преимущество:** Тот же актор ? item attachment работает корректно!

---

## ??? **BEST PRACTICES**

### **1. Используйте SPAWNABLE по умолчанию**

```
? Dating Sim кинематографика ? SPAWNABLE
? Visual Novel сцены ? SPAWNABLE
? Простые диалоги ? SPAWNABLE
```

### **2. Используйте POSSESSABLE только когда нужно:**

```
? Item transfer (передача предметов)
? State change during sequence (изменение состояния)
? Attachment (привязка объектов к актору)
? Continuity (нужен ТОТ ЖЕ актор после)
```

### **3. Всегда включайте Restore для Possessable:**

```
PlaySequence(path, true, true, true)
        ?
      ВСЕГДА true для dynamic NPCs!
```

### **4. Телепортируйте актора ПОСЛЕ Sequence (если нужно):**

```
DialogueNode (Event):
?? Effect 1: "PlaySequence(/Game/Sequences/LS_MyScene)"
?? Effect 2: "TeleportActor(NPC, targetX, targetY, targetZ)"
```

---

## ?? **TROUBLESHOOTING**

### **Issue: Актор телепортировался после Sequence**

**Причина:** Используете Possessable без Restore  
**Решение:**
```
PlaySequence(path, true, true, true) ? Включите restore
```

### **Issue: После Sequence актор не на том месте**

**Причина:** Используете Spawnable, но нужен Possessable  
**Решение:** Переключите на Possessable + Restore

### **Issue: AI Schedule сломан после Sequence**

**Причина:** Possessable изменил позицию глобально  
**Решение:**
1. Используйте Spawnable
2. ИЛИ включите Restore
3. ИЛИ сбросьте AI после Sequence

### **Issue: State (inventory) не передался после Sequence**

**Причина:** Spawnable = новый экземпляр  
**Решение:**
1. Используйте Possessable (с Restore)
2. ИЛИ применяйте state changes через Effects, НЕ через Sequence

---

## ? **ИТОГ**

### **SPAWNABLE:**
- ? Временная копия актора
- ? Оригинал не трогается
- ? Идеально для кинематографики
- ?? State не передаётся

### **POSSESSABLE + RESTORE:**
- ? Тот же актор
- ? State сохраняется
- ? Позиции восстанавливаются
- ?? Актор телепортируется на время

### **Рекомендация для Dating Sim:**
**SPAWNABLE в 90% случаев + телепорт актора после Sequence при необходимости!**

---

**Version:** 1.12  
**Author:** Dialogue System Team  
**Last Updated:** 2024
