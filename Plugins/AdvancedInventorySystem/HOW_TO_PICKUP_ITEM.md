# ?? ��� ������� Item - Quick Guide

## ? �������� �� ������� (v1.7.1)!

**TL;DR:** ������ ������ item � ��! ������������� ��������!

```cpp
InventoryComponent->DropItem(ItemId, 1, DropLocation, Result);
// Item spawned ? InteractableComponent created ? Delegate bound ? Ready to pickup!
```

---

## ?? ��� ��� ��������

### ������������� ��� BeginPlay:

1. **WorldItemActor ������ InteractableComponent** (���� ������� �����)
2. **����������� InteractionData** (��������, �����, ���������, � �.�.)
3. **��������� OnBeginInteract ? HandleInteractionBegin** (�������������!)
4. **HandleInteractionBegin �������� PickupItem()** (������!)

**���������:** Item ����� �������� � interaction system!

---

## ?? ����������� ����������

### 1. � ������ ������ ���� InteractionComponent

```cpp
// � ����� Character Blueprint ��� C++:
Components:
??? InteractionComponent
    ??? Interaction Enabled = True ?
    ??? Trace Distance = 500
    ??? Trace Radius = 10
 ??? Draw Debug Trace = True (��� debug)
```

### 2. Input binding ��������

```blueprint
Input Action "Interact" (E key)
??? InteractionComponent ? Try Interact
```

### 3. � ������ ���� InventoryComponent

```cpp
// ����������� ��� PickupItem()
Components:
??? InventoryComponent
    ??? Max Weight = 100
    ??? Max Slots = 20
    ??? etc.
```

---

## ? ��� ������������� �������������

��� `DropItem()` ��� spawning WorldItemActor:

```cpp
InteractableComponent (auto-created):
??? InteractionData:
?   ??? InteractionName = ItemData?ItemName
?   ??? ActionText = "Press E to pick up {ItemName}"
?   ??? InteractionType = Pickup
?   ??? InteractionDuration = 0.0 (instant)
?   ??? MaxInteractionDistance = 150.0 (�� InteractionRadius)
?   ??? InteractionPriority = 0
?   ??? bCanInteractMultipleTimes = false
?
??? OnBeginInteract ? Bound to HandleInteractionBegin()
    ??? Calls PickupItem()
```

**�� �������������!** ??

---

## ?? ������������

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

## ?? ��������� (�����������)

### �������� ��������� interaction:

```cpp
// � WorldItemActor Defaults ��� ItemDataAsset:
InteractionRadius = 300.0;        // ������ ����� ���������
InteractionDuration = 2.0;          // Hold E 2 sec to pickup
InteractionPriority = 100;          // Higher priority vs other items
InteractionNameOverride = "Magic Sword"; // Custom name
```

### Custom configuration � Blueprint Child:

```blueprint
BP_SpecialItem (child of WorldItemActor):

Event ConfigureInteractionComponent (Comp):
??? Parent: ConfigureInteractionComponent
??? Cast to InteractableComponent
?   ??? Set Max Interaction Count = 1
?   ??? Set Highlight When Focused = True
??? (custom logic)
```

**�� ��� �����������!** �� ��������� �� ��������!

---

## ?? Troubleshooting

### "Item �� �����������"

**Check:**

1. **InteractableComponent ������?**
   ```
   Play ? World Outliner ? Your Item ? Details
? Components ? InteractableComponent_Dynamic ?
   ```

2. **Debug trace ������?**
   ```
   InteractionComponent ? bDrawDebugTrace = true
   Look at item ? Line should be GREEN
   If RED = not detected
   ```

3. **Input ��������?**
   ```
   Press E ? Check Output Log
   Should see: "WorldItemActor: Bound OnBeginInteract delegate"
   ```

4. **Inventory has space?**
   ```
   Open inventory ? Should have free slots
   ```

---

### "InteractableComponent �� ��������"

**Check Output Log:**

```
LogTemp: WorldItemActor: Auto-created InteractableComponent for BP_Item_C_0
```

**���� ���� ���:**

- ����� �� ������: `bAutoCreateInteractionComponent = false`?
- ��� ������������: �� ��������� ���� "InteractableComponent"
- ��������� �� � �������: �������� InteractableComponent class

**Fix:**
```cpp
// If your component has different name:
InteractableComponentClassName = "MyCustomInteractableComp";
```

---

### "Delegate �� ������"

**Check Output Log:**

```
LogTemp: WorldItemActor: Bound OnBeginInteract delegate for BP_Item_C_0
```

**���� ���� ���:**

- Delegate property �� ������: ��������� ��� InteractableComponent ����� `OnBeginInteract` delegate
- Signature ������������: ������ ���� `DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FName, AActor*)`

**��� �� ������ �����������** ���� �� ����������� ����������� InteractableComponent!

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

- [ ] InteractionComponent �� ������ (� Interaction Enabled = true)
- [ ] InventoryComponent �� ������
- [ ] Input binding (E ? TryInteract)
- [ ] ItemDataAsset �������� �� WorldItemActor
- [ ] ItemData?bCanBePickedUp = true
- [ ] Inventory has space
- [ ] bAutoCreateInteractionComponent = true (default)
- [ ] Debug trace ������ ��� ������� �� item

**���� ��� ? = ������ �������� �� �������!** ??

---

## ?? ����

**v1.7.1 = Zero Setup Required!**

```
Drop Item ? Spawns ? Auto-configures ? Works! ?
```

������� Blueprint overrides, ������� ������ ���������, ������� ���������!

**Just works!** ??
