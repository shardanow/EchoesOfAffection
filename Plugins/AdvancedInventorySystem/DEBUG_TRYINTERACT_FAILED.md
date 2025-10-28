# ?? Debug Guide: TryInteract Returns Failed

## Ситуация

- ? WorldItemActor размещён в мире
- ? Input binding настроен (E вызывает TryInteract)
- ? **TryInteract() возвращает Failed**

---

## ?? Diagnostic Steps

Теперь у вас есть **подробное логирование**. Запустите игру и проверьте **Output Log**.

### Шаг 1: Проверьте что InteractableComponent создан

**Ожидаемый лог при BeginPlay:**

```
LogTemp: WorldItemActor: Auto-created InteractableComponent for BP_Item_C_0
LogTemp: WorldItemActor: Configured InteractionData for BP_Item_C_0
LogTemp: WorldItemActor: Bound OnBeginInteract delegate for BP_Item_C_0
```

**Если этих логов НЕТ:**
- Component не создался
- Проверьте `bAutoCreateInteractionComponent = true`
- Проверьте что `InteractableComponent` class exists
- Проверьте имя: `InteractableComponentClassName = "InteractableComponent"`

---

### Шаг 2: Посмотрите на item и проверьте логи

**Ожидаемое поведение:**
- Debug trace должен быть **зелёным**
- Должен появиться лог о focus

**Если trace КРАСНЫЙ:**
- Item не detected
- Проверьте `TraceDistance` (увеличьте до 1000)
- Проверьте `InteractionTraceChannel`
- Проверьте collision на WorldItemActor

**Если trace ЗЕЛЁНЫЙ но нет focus:**
- `IsActorInteractable()` возвращает false
- Component не создался или interface не работает

---

### Шаг 3: Нажмите E и проверьте логи TryInteract

**Сценарий A: "No focused actor"**

```
LogTemp: Warning: TryInteract FAILED: No focused actor
```

**Причина:** Item не в фокусе  
**Fix:**
- Проверьте что смотрите прямо на item
- Enable `bDrawDebugTrace = true`
- Проверьте что InteractableComponent создан

---

**Сценарий B: "Cached interactable is invalid"**

```
LogTemp: Log: TryInteract: Focused actor = BP_Item_C_0
LogTemp: Error: TryInteract FAILED: Cached interactable is invalid (no interface found)
```

**Причина:** InteractableComponent не найден или не реализует interface  
**Fix:**
1. World Outliner ? BP_Item_C_0 ? Details ? Components
2. Должен быть `InteractableComponent_Dynamic`
3. Если нет - component не создался
4. Проверьте логи BeginPlay (Шаг 1)

---

**Сценарий C: "Validation failed"**

```
LogTemp: Log: TryInteract: CachedInfo valid, Interface = YES, Component = InteractableComponent_Dynamic
LogTemp: Log: TryInteract: InteractionData - Name: Item, Distance: 150.0, RequiresLOS: YES
LogTemp: Warning: TryInteract FAILED: Validation failed
LogTemp: Warning:   - Distance: 523.4 (Max: 150.0)
```

**Причина:** Item слишком далеко или line of sight blocked  
**Fix:**
- **Distance:** Увеличьте `InteractionRadius` на WorldItemActor (default: 150)
- **Line of Sight:** Проверьте что ничто не блокирует view

---

**Сценарий D: "BeginInteract failed"**

```
LogTemp: Log: TryInteract: Validation passed, calling BeginInteract...
LogTemp: Log: TryInteract: BeginInteract returned: 2
LogTemp: Warning: TryInteract: BeginInteract failed with result: 2
```

**Причина:** InteractableComponent::BeginInteract вернул не Success  
**Возможные результаты:**
- `0` = Success
- `1` = Failed
- `2` = NotInteractable
- `3` = RequirementsNotMet
- `4` = AlreadyInteracting

**Fix:**
Проверьте InteractableComponent:
- `bIsEnabled = true`?
- `MaxInteractionCount` не достигнут?
- `BP_CanInteractCheck` returns true?

---

**Сценарий E: "BeginInteract Success but PickupItem fails"**

```
LogTemp: Log: TryInteract: BeginInteract returned: 0
LogTemp: Log: TryInteract SUCCESS: Interaction started (Duration: 0.0)
LogTemp: Log: WorldItemActor::PickupItem called by BP_PlayerCharacter_C_0
LogTemp: Warning: WorldItemActor::PickupItem FAILED: CanBePickedUpBy returned false
```

**Причина:** `CanBePickedUpBy()` fails  
**Проверьте sub-логи:**

```
LogTemp: Warning: WorldItemActor::CanBePickedUpBy: No ItemData assigned!
```
? **Fix:** Assign ItemData в Details panel

```
LogTemp: Warning: WorldItemActor::CanBePickedUpBy: ItemData.PhysicalRepresentation.bCanBePickedUp = false
```
? **Fix:** В ItemDataAsset ? Physical Representation ? Can Be Picked Up = True

```
LogTemp: Warning: WorldItemActor::CanBePickedUpBy: Picker has no InventoryComponent
```
? **Fix:** Add InventoryComponent to player Character

```
LogTemp: Warning: WorldItemActor::CanBePickedUpBy: Inventory cannot accept item (full or incompatible)
```
? **Fix:** Clear inventory or increase capacity

---

## ?? Quick Checklist

Пройдите по этому списку:

### Character Setup:
- [ ] InteractionComponent added
- [ ] InteractionComponent ? Interaction Enabled = True
- [ ] InteractionComponent ? Trace Distance = 500+ (increase if needed)
- [ ] InteractionComponent ? Draw Debug Trace = True
- [ ] InventoryComponent added
- [ ] Input binding (E ? TryInteract)

### WorldItemActor Setup:
- [ ] ItemData assigned
- [ ] ItemData ? Physical Representation ? Can Be Picked Up = True
- [ ] bAutoCreateInteractionComponent = True
- [ ] InteractionRadius = 150+ (increase if too far)
- [ ] bCanBePickedUp = True

### Runtime Checks:
- [ ] Output Log shows "Auto-created InteractableComponent"
- [ ] Output Log shows "Configured InteractionData"
- [ ] Output Log shows "Bound OnBeginInteract delegate"
- [ ] Debug trace is GREEN when looking at item
- [ ] No errors in Output Log

---

## ?? Common Fixes

### Fix 1: Increase Interaction Distance

**Problem:** `Distance: 523.4 (Max: 150.0)`

**Solution:**
```
WorldItemActor ? Details
??? Interaction Radius = 500.0 (or higher)
```

---

### Fix 2: ItemData Not Assigned

**Problem:** `No ItemData assigned!`

**Solution:**
```
WorldItemActor ? Details
??? Item Data = DA_YourItem (pick from dropdown)
```

---

### Fix 3: Can't Be Picked Up

**Problem:** `bCanBePickedUp = false` or `ItemData.PhysicalRepresentation.bCanBePickedUp = false`

**Solution:**
```
WorldItemActor ? Details:
??? Can Be Picked Up = True ?

ItemDataAsset ? Physical Representation:
??? Can Be Picked Up = True ?
```

---

### Fix 4: No InventoryComponent

**Problem:** `Picker has no InventoryComponent`

**Solution:**
```
Character Blueprint ? Components:
??? Add Component ? InventoryComponent
```

---

### Fix 5: Inventory Full

**Problem:** `Inventory cannot accept item (full or incompatible)`

**Solution:**
```
Character ? InventoryComponent:
??? Max Weight = 1000 (increase)
??? Max Slots = 50 (increase)
??? Clear some items
```

---

### Fix 6: InteractableComponent Not Created

**Problem:** No logs about component creation

**Solution A:** Check class name
```cpp
WorldItemActor ? Details:
??? Interactable Component Class Name = "InteractableComponent"
```

**Solution B:** Verify component exists in project
```
Content Browser ? Search: "InteractableComponent"
Should find: C++ Class
```

**Solution C:** Manually add component (temporary workaround)
```
WorldItemActor Blueprint ? Components:
??? Add Component ? InteractableComponent

Event Graph:
Event BeginPlay
??? Get Component (InteractableComponent)
    ??? Bind Event to OnBeginInteract
 ??? Custom Event: DoPickup
     
Custom Event DoPickup (Interacting Actor)
??? PickupItem (Interacting Actor)
```

---

## ?? Complete Log Example (Success)

```
=== BeginPlay ===
LogTemp: WorldItemActor: Auto-created InteractableComponent for BP_HealthPotion_C_0
LogTemp: WorldItemActor: Configured InteractionData for BP_HealthPotion_C_0
LogTemp: WorldItemActor: Bound OnBeginInteract delegate for BP_HealthPotion_C_0

=== Looking at Item ===
(Green debug trace appears)

=== Pressing E ===
LogTemp: Log: TryInteract: Focused actor = BP_HealthPotion_C_0
LogTemp: Log: TryInteract: CachedInfo valid, Interface = YES, Component = InteractableComponent_Dynamic
LogTemp: Log: TryInteract: InteractionData - Name: Health Potion, Distance: 150.0, RequiresLOS: YES
LogTemp: Log: TryInteract: Validation passed, calling BeginInteract...
LogTemp: Log: TryInteract: BeginInteract returned: 0
LogTemp: Log: TryInteract SUCCESS: Interaction started (Duration: 0.0)
LogTemp: Log: WorldItemActor::PickupItem called by BP_PlayerCharacter_C_0
LogTemp: Log: WorldItemActor::PickupItem: Found InventoryComponent, attempting to add item...
LogTemp: Log: WorldItemActor::PickupItem SUCCESS: Item added to inventory

=== Item Disappears ===
? Success!
```

---

## ?? Complete Log Example (Failure - No ItemData)

```
=== BeginPlay ===
LogTemp: WorldItemActor: Auto-created InteractableComponent for BP_Item_C_0
LogTemp: WorldItemActor: Configured InteractionData for BP_Item_C_0
LogTemp: WorldItemActor: Bound OnBeginInteract delegate for BP_Item_C_0

=== Pressing E ===
LogTemp: Log: TryInteract: Focused actor = BP_Item_C_0
LogTemp: Log: TryInteract: CachedInfo valid, Interface = YES, Component = InteractableComponent_Dynamic
LogTemp: Log: TryInteract: InteractionData - Name: Item, Distance: 150.0, RequiresLOS: YES
LogTemp: Log: TryInteract: Validation passed, calling BeginInteract...
LogTemp: Log: TryInteract: BeginInteract returned: 0
LogTemp: Log: TryInteract SUCCESS: Interaction started (Duration: 0.0)
LogTemp: Log: WorldItemActor::PickupItem called by BP_PlayerCharacter_C_0
LogTemp: Error: WorldItemActor::CanBePickedUpBy: No ItemData assigned!
LogTemp: Warning: WorldItemActor::PickupItem FAILED: CanBePickedUpBy returned false

? Fix: Assign ItemData!
```

---

## ?? Next Steps

1. **Run game**
2. **Open Output Log** (Window ? Developer Tools ? Output Log)
3. **Look at item**
4. **Press E**
5. **Read logs**
6. **Apply fix based on error message**
7. **Test again**

**Logs tell you EXACTLY what's wrong!** ??

---

## ?? Pro Tip

**Copy all logs and paste here if still stuck!**

Include:
- BeginPlay logs
- TryInteract logs
- PickupItem logs
- Any errors/warnings

We can diagnose exact problem! ??
