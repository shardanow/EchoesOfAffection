# 🐛 DEBUGGING: ACTOR SCHEDULE + DIALOGUE INTEGRATION

## ❓ **ПРОБЛЕМЫ:**

1. **NPC продолжает двигаться** когда диалог начинается
2. **Schedule не resuming** когда диалог заканчивается

---

## 🔧 **ИСПРАВЛЕНИЯ:**

### **1. Агрессивная остановка движения**

```cpp
void UScheduleComponent::PauseSchedule()
{
    // ✅ Теперь АГРЕССИВНО останавливает движение:
    
    // 1. Pause current action
    IScheduleActionInterface::Execute_PauseAction(...)
    
    // 2. Stop AI movement
    AIController->StopMovement();
    
    // 3. Stop CharacterMovementComponent
    MovementComp->StopMovementImmediately();
}
```

### **2. Детальное логирование**

Добавлено логирование на **каждом этапе**:

```
[GAMEEVENTBUS] EMITTING: GameEvent.Dialogue.Started
   ↓
[EVENT] OnDialogueStarted received
   ↓
[PAUSE] Pausing schedule
   ↓
[PAUSE] Stopped AI movement
   ↓
... диалог идёт ...
   ↓
[GAMEEVENTBUS] EMITTING: GameEvent.Dialogue.Ended
   ↓
[EVENT] OnDialogueEnded received
↓
[RESUME] Resuming schedule
```

---

## 📊 **КАК ОТЛАДИТЬ:**

### **Шаг 1: Проверьте подписку**

Запустите игру и **сразу** смотрите Output Log:

```
[SUBSCRIBE] ============================================
[SUBSCRIBE] Subscribing to Dialogue events for actor: BP_Lia_C_0
[SUBSCRIBE]   Tag 'GameEvent.Dialogue.Started' valid: YES
[SUBSCRIBE]   Subscribed to 'Dialogue.Started' (Handle valid: YES)
[SUBSCRIBE] Tag 'GameEvent.Dialogue.Ended' valid: YES
[SUBSCRIBE]   Subscribed to 'Dialogue.Ended' (Handle valid: YES)
[SUBSCRIBE] Subscription complete!
[SUBSCRIBE] ============================================
```

**✅ Если видите:** Подписка работает!

**❌ Если НЕ видите:**
- GameEventBus не загружен
- ScheduleComponent не создан
- BeginPlay не вызван

---

### **Шаг 2: Начните диалог**

Начните диалог с NPC и смотрите Output Log:

```
[GAMEEVENTBUS] ============================================
[GAMEEVENTBUS] EMITTING: GameEvent.Dialogue.Started
[GAMEEVENTBUS]   DialogueId: greeting_lia
[GAMEEVENTBUS]   Player (Instigator): bp_MyAnimeCharacter_C_0
[GAMEEVENTBUS]   NPC (Target): BP_Lia_C_0
[GAMEEVENTBUS] Event emitted successfully!
[GAMEEVENTBUS] ============================================
```

**✅ Если видите:** DialogueRunner emit работает!

**❌ Если НЕ видите:**
- DialogueRunner не вызывает EmitDialogueStartedEvent
- WITH_GAMEEVENTBUS = 0 (не скомпилировано)
- StartDialogue не вызывается

---

### **Шаг 3: Проверьте получение события**

Сразу после emit должно быть:

```
[EVENT] OnDialogueStarted received for actor: BP_Lia_C_0
[EVENT]   DialogueId: greeting_lia
[EVENT]   InstigatorActor: bp_MyAnimeCharacter_C_0
[EVENT]   TargetActor: BP_Lia_C_0
[EVENT]   bIsInvolvedInDialogue: YES
[EVENT] Dialogue started - pausing schedule for actor: BP_Lia_C_0 (DialogueId: greeting_lia)
```

**✅ Если видите:** Event доходит!

**❌ Если НЕ видите:**
- ScheduleComponent не подписан
- GameEventBus не broadcast'ит
- Tag не совпадает

---

### **Шаг 4: Проверьте паузу**

Сразу после OnDialogueStarted должно быть:

```
[PAUSE] Pausing schedule for actor: BP_Lia_C_0
[PAUSE]   Current action paused: Move to location
[PAUSE]   Stopped AI movement
[PAUSE]   Stopped CharacterMovementComponent
```

**✅ Если видите:** Пауза работает!

**❌ Если НЕ видите:**
- PauseSchedule не вызывается
- Логика в OnDialogueStarted не проходит

---

### **Шаг 5: Закончите диалог**

Закончите диалог и смотрите Output Log:

```
[GAMEEVENTBUS] ============================================Поня
[GAMEEVENTBUS] EMITTING: GameEvent.Dialogue.Ended
[GAMEEVENTBUS] Player (Instigator): bp_MyAnimeCharacter_C_0
[GAMEEVENTBUS]   NPC (Target): BP_Lia_C_0
[GAMEEVENTBUS] Event emitted successfully!
[GAMEEVENTBUS] ============================================
```

**✅ Если видите:** DialogueRunner emit работает!

---

### **Шаг 6: Проверьте получение Ended**

```
[EVENT] OnDialogueEnded received for actor: BP_Lia_C_0
[EVENT]   InstigatorActor: bp_MyAnimeCharacter_C_0
[EVENT]   TargetActor: BP_Lia_C_0
[EVENT]   bIsPaused: YES
[EVENT]   bWasInvolvedInDialogue: YES
[EVENT] Dialogue ended - resuming schedule for actor: BP_Lia_C_0 from position: X=100.000 Y=100.000 Z=100.000
```

**✅ Если видите:** Event доходит!

**❌ Если НЕ видите:**
- OnDialogueEnded не вызывается
- bIsPaused = false (не был запаузен!)

---

### **Шаг 7: Проверьте resume**

```
[RESUME] Resuming schedule for actor: BP_Lia_C_0 (was paused: YES)
[RESUME]   Current action resumed: Move to location
[RESUME]   Re-evaluating schedule...
[SCHEDULE] EvaluateSchedule for BP_Lia_C_0:
[RESUME]   Done!
```

**✅ Если видите:** Resume работает!

---

## 🐛 **ТИПИЧНЫЕ ПРОБЛЕМЫ:**

### **Problem 1: "bIsPaused: NO" когда должен быть YES**

```
[EVENT] OnDialogueEnded received
[EVENT]   bIsPaused: NO  ← ❌ ПРОБЛЕМА!
```

**Причина:** PauseSchedule не был вызван

**Решение:** Проверьте OnDialogueStarted и убедитесь что:
- bIsInvolvedInDialogue = YES
- PauseSchedule вызывается

---

### **Problem 2: NPC продолжает двигаться после PauseSchedule**

```
[PAUSE] Pausing schedule
[PAUSE]   Current action paused: Move to location
[PAUSE]   Stopped AI movement  ← но NPC всё ещё двигается!
```

**Причина:** AI движение не остановлено корректно

**Решение:**
1. Проверьте что NPC имеет AIController
2. Проверьте что CharacterMovementComponent существует
3. Попробуйте более агрессивную остановку:
   ```cpp
   AIController->StopMovement();
   AIController->GetPathFollowingComponent()->AbortMove(...);
   ```

---

### **Problem 3: Events не доходят (нет [EVENT] логов)**

```
[GAMEEVENTBUS] Event emitted successfully!
... ничего дальше ...
```

**Причина:** ScheduleComponent не подписан на события

**Решение:**
1. Проверьте что ScheduleComponent создан
2. Проверьте что BeginPlay вызван
3. Проверьте логи [SUBSCRIBE]
4. Проверьте что WITH_GAMEEVENTBUS = 1

---

### **Problem 4: Tags не совпадают**

```
[SUBSCRIBE]   Tag 'GameEvent.Dialogue.Started' valid: NO  ← ❌ ПРОБЛЕМА!
```

**Причина:** Tag не зарегистрирован в DefaultGameplayTags.ini

**Решение:**
1. Откройте `Config/DefaultGameplayTags.ini`
2. Убедитесь что есть:
   ```ini
   +GameplayTagList=(Tag="GameEvent.Dialogue.Started",...)
   +GameplayTagList=(Tag="GameEvent.Dialogue.Ended",...)
   ```
3. Перезапустите редактор

---

## ✅ **EXPECTED OUTPUT (полный успешный flow):**

```
// ========== STARTUP ==========
[SUBSCRIBE] ============================================
[SUBSCRIBE] Subscribing to Dialogue events for actor: BP_Lia_C_0
[SUBSCRIBE]   Tag 'GameEvent.Dialogue.Started' valid: YES
[SUBSCRIBE]   Subscribed to 'Dialogue.Started' (Handle valid: YES)
[SUBSCRIBE]   Tag 'GameEvent.Dialogue.Ended' valid: YES
[SUBSCRIBE]   Subscribed to 'Dialogue.Ended' (Handle valid: YES)
[SUBSCRIBE] Subscription complete!
[SUBSCRIBE] ============================================

// ========== DIALOGUE START ==========
[GAMEEVENTBUS] ============================================
[GAMEEVENTBUS] EMITTING: GameEvent.Dialogue.Started
[GAMEEVENTBUS]   DialogueId: greeting_lia
[GAMEEVENTBUS]   Player (Instigator): bp_MyAnimeCharacter_C_0
[GAMEEVENTBUS]   NPC (Target): BP_Lia_C_0
[GAMEEVENTBUS] Event emitted successfully!
[GAMEEVENTBUS] ============================================

[EVENT] OnDialogueStarted received for actor: BP_Lia_C_0
[EVENT]   DialogueId: greeting_lia
[EVENT]   InstigatorActor: bp_MyAnimeCharacter_C_0
[EVENT]   TargetActor: BP_Lia_C_0
[EVENT]   bIsInvolvedInDialogue: YES
[EVENT] Dialogue started - pausing schedule for actor: BP_Lia_C_0

[PAUSE] Pausing schedule for actor: BP_Lia_C_0
[PAUSE]   Current action paused: Move to location
[PAUSE]   Stopped AI movement
[PAUSE]   Stopped CharacterMovementComponent

// ========== DIALOGUE ENDS ==========
[GAMEEVENTBUS] ============================================
[GAMEEVENTBUS] EMITTING: GameEvent.Dialogue.Ended
[GAMEEVENTBUS]   Player (Instigator): bp_MyAnimeCharacter_C_0
[GAMEEVENTBUS]   NPC (Target): BP_Lia_C_0
[GAMEEVENTBUS] Event emitted successfully!
[GAMEEVENTBUS] ============================================

[EVENT] OnDialogueEnded received for actor: BP_Lia_C_0
[EVENT]   InstigatorActor: bp_MyAnimeCharacter_C_0
[EVENT]   TargetActor: BP_Lia_C_0
[EVENT]   bIsPaused: YES
[EVENT]   bWasInvolvedInDialogue: YES
[EVENT] Dialogue ended - resuming schedule

[RESUME] Resuming schedule for actor: BP_Lia_C_0 (was paused: YES)
[RESUME]   Current action resumed: Move to location
[RESUME]   Re-evaluating schedule...
[SCHEDULE] EvaluateSchedule for BP_Lia_C_0:
[RESUME]   Done!
```

---

## 🚀 **СЛЕДУЮЩИЕ ШАГИ:**

1. **Скомпилируйте проект**
2. **Запустите игру**
3. **Смотрите Output Log**
4. **Найдите где ломается** (используя логи выше)
5. **Сообщите мне точные логи** - я помогу исправить!

---

**Version:** 1.13-debug  
**Last Updated:** 2024
