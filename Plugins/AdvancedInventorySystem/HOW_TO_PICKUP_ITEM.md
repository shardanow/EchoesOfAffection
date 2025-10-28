# ?? Как Поднять Item - Quick Guide

## ? РАБОТАЕТ ИЗ КОРОБКИ (v1.7.1)!

**TL;DR:** Просто дропай item и всё! Автоматически работает!

```cpp
InventoryComponent->DropItem(ItemId, 1, DropLocation, Result);
// Item spawned ? InteractableComponent created ? Delegate bound ? Ready to pickup!
```

---

## ?? Как Это Работает

### Автоматически при BeginPlay:

1. **WorldItemActor создаёт InteractableComponent** (если находит класс)
2. **Настраивает InteractionData** (название, текст, дистанция, и т.д.)
3. **Связывает OnBeginInteract ? HandleInteractionBegin** (автоматически!)
4. **HandleInteractionBegin вызывает PickupItem()** (готово!)

**Результат:** Item сразу работает с interaction system!

---

## ?? Минимальные Требования

### 1. У игрока должен быть InteractionComponent

```cpp
// В вашем Character Blueprint или C++:
Components:
??? InteractionComponent
    ??? Interaction Enabled = True ?
    ??? Trace Distance = 500
    ??? Trace Radius = 10
 ??? Draw Debug Trace = True (для debug)
```

### 2. Input binding настроен

```blueprint
Input Action "Interact" (E key)
??? InteractionComponent ? Try Interact
```

### 3. У игрока есть InventoryComponent

```cpp
// Обязательно для PickupItem()
Components:
??? InventoryComponent
    ??? Max Weight = 100
    ??? Max Slots = 20
    ??? etc.
```

---

## ? Что Настраивается Автоматически

При `DropItem()` или spawning WorldItemActor:

```cpp
InteractableComponent (auto-created):
??? InteractionData:
?   ??? InteractionName = ItemData?ItemName
?   ??? ActionText = "Press E to pick up {ItemName}"
?   ??? InteractionType = Pickup
?   ??? InteractionDuration = 0.0 (instant)
?   ??? MaxInteractionDistance = 150.0 (из InteractionRadius)
?   ??? InteractionPriority = 0
?   ??? bCanInteractMultipleTimes = false
?
??? OnBeginInteract ? Bound to HandleInteractionBegin()
    ??? Calls PickupItem()
```

**Всё автоматически!** ??

---

## ?? Тестирование

### Quick Test:

```cpp
// 1. Drop item from inventory
InventoryComponent->DropItem(ItemId, 1, PlayerLocation + Forward * 200, Result);

// 2. Turn around and look at it
// ? Should see green debug trace (if bDrawDebugTrace = true)
// ? Should see outline (if configured)

// 3. Press E
// ? Item disappears
// ? Item added back to inventory
// ? Success! ?
```

---

## ?? Настройка (Опционально)

### Изменить параметры interaction:

```cpp
// В WorldItemActor Defaults или ItemDataAsset:
InteractionRadius = 300.0;        // Дальше можно подобрать
InteractionDuration = 2.0;          // Hold E 2 sec to pickup
InteractionPriority = 100;          // Higher priority vs other items
InteractionNameOverride = "Magic Sword"; // Custom name
```

### Custom configuration в Blueprint Child:

```blueprint
BP_SpecialItem (child of WorldItemActor):

Event ConfigureInteractionComponent (Comp):
??? Parent: ConfigureInteractionComponent
??? Cast to InteractableComponent
?   ??? Set Max Interaction Count = 1
?   ??? Set Highlight When Focused = True
??? (custom logic)
```

**Но это опционально!** По умолчанию всё работает!

---

## ?? Troubleshooting

### "Item не поднимается"

**Check:**

1. **InteractableComponent создан?**
   ```
   Play ? World Outliner ? Your Item ? Details
? Components ? InteractableComponent_Dynamic ?
   ```

2. **Debug trace зелёный?**
   ```
   InteractionComponent ? bDrawDebugTrace = true
   Look at item ? Line should be GREEN
   If RED = not detected
   ```

3. **Input работает?**
   ```
   Press E ? Check Output Log
   Should see: "WorldItemActor: Bound OnBeginInteract delegate"
   ```

4. **Inventory has space?**
   ```
   Open inventory ? Should have free slots
   ```

---

### "InteractableComponent не создался"

**Check Output Log:**

```
LogTemp: WorldItemActor: Auto-created InteractableComponent for BP_Item_C_0
```

**Если лога нет:**

- Класс не найден: `bAutoCreateInteractionComponent = false`?
- Имя неправильное: По умолчанию ищет "InteractableComponent"
- Компонент не в проекте: Добавьте InteractableComponent class

**Fix:**
```cpp
// If your component has different name:
InteractableComponentClassName = "MyCustomInteractableComp";
```

---

### "Delegate не связан"

**Check Output Log:**

```
LogTemp: WorldItemActor: Bound OnBeginInteract delegate for BP_Item_C_0
```

**Если лога нет:**

- Delegate property не найден: Убедитесь что InteractableComponent имеет `OnBeginInteract` delegate
- Signature неправильная: Должна быть `DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FName, AActor*)`

**Это НЕ должно происходить** если вы используете стандартный InteractableComponent!

---

### "PickupItem fails"

**Check `CanBePickedUpBy()`:**

```cpp
// Add logs to diagnose:
bool AWorldItemActor::CanBePickedUpBy(AActor* Picker) const
{
    if (!bCanBePickedUp)
    {
        UE_LOG(LogTemp, Warning, TEXT("bCanBePickedUp = false"));
        return false;
    }
    
    if (!ItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("No ItemData!"));
     return false;
    }
    
    // ... etc
}
```

**Common issues:**
- No ItemData assigned
- ItemData?PhysicalRepresentation.bCanBePickedUp = false
- Inventory full
- Player no InventoryComponent

---

## ?? Full Example Flow

```
1. Player opens inventory
2. Right-click item ? Drop
3. InventoryComponent?DropItem()
    ?
4. WorldItemActor spawned at location
  ?
5. BeginPlay() ? TryCreateInteractableComponent()
    ?
6. FindObject("InteractableComponent") ? Found!
    ?
7. NewObject<InteractableComponent>() ? Created
    ?
8. RegisterComponent() ? Active
    ?
9. ConfigureInteractionComponent()
??? Set InteractionData via reflection
  ??? Bind OnBeginInteract ? HandleInteractionBegin
    ?
10. Item ready! ??
    ?
11. Player looks at item
    ?
12. InteractionComponent trace ? Found InteractableComponent
    ?
13. OnInteractionFocused ? Outline appears
    ?
14. Player presses E
    ?
15. InteractionComponent?TryInteract()
    ?
16. InteractableComponent?BeginInteract()
    ?
17. OnBeginInteract.Broadcast(Player)
    ?
18. HandleInteractionBegin(Player) called
    ?
19. PickupItem(Player)
    ??? Check CanBePickedUpBy() ?
    ??? InventoryComponent?AddItem() ?
    ??? Destroy() item
    ?
20. Item picked up! ?
```

---

## ?? Advanced: Override Behavior

### Custom PickupItem logic:

```cpp
// In your custom WorldItemActor child:
bool AMySpecialItem::PickupItem(AActor* Picker) override
{
    // Custom pre-pickup logic
    if (bRequiresQuest && !PlayerHasQuest(Picker))
    {
        ShowMessage("You need to complete the quest first!");
        return false;
    }
    
    // Call parent
    bool bSuccess = Super::PickupItem(Picker);
    
    // Custom post-pickup logic
    if (bSuccess)
    {
        PlaySound(PickupSound);
   SpawnParticles(PickupFX);
        UnlockAchievement("FirstPickup");
    }
    
    return bSuccess;
}
```

### Custom HandleInteractionBegin:

```cpp
// Override the callback
void AMySpecialItem::HandleInteractionBegin(AActor* InteractingActor) override
{
    // Custom interaction logic
    if (ShouldShowDialogue())
    {
        ShowDialogue(InteractingActor);
        // Don't pickup yet
    }
    else
    {
        // Normal pickup
  PickupItem(InteractingActor);
    }
}
```

---

## ? Final Checklist

- [ ] InteractionComponent на игроке (с Interaction Enabled = true)
- [ ] InventoryComponent на игроке
- [ ] Input binding (E ? TryInteract)
- [ ] ItemDataAsset назначен на WorldItemActor
- [ ] ItemData?bCanBePickedUp = true
- [ ] Inventory has space
- [ ] bAutoCreateInteractionComponent = true (default)
- [ ] Debug trace зелёный при взгляде на item

**Если все ? = Должно работать из коробки!** ??

---

## ?? Итог

**v1.7.1 = Zero Setup Required!**

```
Drop Item ? Spawns ? Auto-configures ? Works! ?
```

Никаких Blueprint overrides, никакой ручной настройки, никаких делегатов!

**Just works!** ??
