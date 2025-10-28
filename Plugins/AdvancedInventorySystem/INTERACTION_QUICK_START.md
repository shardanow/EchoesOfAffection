# ?? WorldItemActor + Interaction System - Quick Start

**Version:** 1.7.0  
**Time to Integrate:** 5-30 minutes  
**Difficulty:** Easy to Intermediate

---

## ?? ��� ���?

WorldItemActor ������ ������������ **������������ ����������** � ����� Interaction System **��� ������ ������������**.

### ? ��� ��������:
- ? ������ ������������� �������� �� �������
- ? ��� ������� ���������� (Blueprint, C++, Component)
- ? �������������� ����������� IInteractableInterface
- ? ������� ��� ���������� �������� �����
- ? ������� overhead ���� ���������� �� ������������

---

## ? Quick Start (3 ����)

### ��� 1: �������� �����

**Option A - Blueprint (5 �����, ����� �����):**
```
������� BP child ? Add IInteractableInterface ? Implement events
```
?? [������ Blueprint ����](INTERACTION_SYSTEM_INTEGRATION.md#method-1-blueprint-interface-recommended)

**Option B - C++ Helper (10 �����, production-ready):**
```cpp
class AInteractableWorldItem 
    : public AWorldItemActor
  , public IInteractableInterface
{
    // Implement interface...
};
```
?? [������� C++ ���](EXAMPLE_INTEGRATION_CLASS.md)

**Option C - Component (3 ������, ��������):**
```
Add InteractionComponent ? Bind events ? Done
```
?? [Component ����](INTERACTION_SYSTEM_INTEGRATION.md#method-3-component-based)

---

### ��� 2: ���������� ����������

#### Blueprint ������:

**Event GetInteractionData:**
```blueprint
??? Make Interaction Data Struct
    ??? Name = Get Interaction Name
    ??? Type = Pickup
    ??? Action Text = Get Interaction Action Text
    ??? Duration = Interaction Duration (variable)
    ??? Max Distance = Interaction Radius (variable)
    ??? Priority = Interaction Priority (variable)
    ??? Return
```

**Event BeginInteract:**
```blueprint
Pickup Item (Interacting Actor)
??? Branch
    ??? True ? Return EInteractionResult::Success
    ??? False ? Return EInteractionResult::Failed
```

**Event CanInteract:**
```blueprint
Can Be Picked Up By (Interacting Actor)
??? Return (bool)
```

#### C++ ������:

```cpp
EInteractionResult AInteractableWorldItem::BeginInteract_Implementation(AActor* InteractingActor)
{
    bool bSuccess = PickupItem(InteractingActor);
    return bSuccess ? EInteractionResult::Success : EInteractionResult::Failed;
}

FInteractionData AInteractableWorldItem::GetInteractionData_Implementation(AActor* InteractingActor) const
{
    FInteractionData Data;
    Data.InteractionName = GetInteractionName();
    Data.InteractionType = EInteractionType::Pickup;
    Data.ActionText = GetInteractionActionText();
    Data.InteractionDuration = InteractionDuration;
    Data.MaxInteractionDistance = InteractionRadius;
    Data.InteractionPriority = InteractionPriority;
    return Data;
}

bool AInteractableWorldItem::CanInteract_Implementation(AActor* InteractingActor) const
{
    return CanBePickedUpBy(InteractingActor);
}
```

---

### ��� 3: �������� ���������� �������� ����� (�����������)

**� Blueprint:**

```blueprint
Event OnInteractionFocusGained
??? Set Render Custom Depth = True (Static Mesh)
??? Set Custom Depth Stencil Value = 252
??? Create Widget ? WBP_PickupPrompt

Event OnInteractionFocusLost
??? Set Render Custom Depth = False
??? Remove Widget
```

**� C++:**

```cpp
void AInteractableWorldItem::OnInteractionFocused_Implementation(AActor* InteractingActor)
{
    if (StaticMeshComponent)
 {
        StaticMeshComponent->SetRenderCustomDepth(true);
        StaticMeshComponent->SetCustomDepthStencilValue(252);
    }
OnInteractionFocusGained(InteractingActor); // BP event
}
```

---

## ?? ����� �������� WorldItemActor

�������� � Details panel:

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| **Auto Implement Interaction** | bool | true | Auto-provide data if interface detected |
| **Interaction Name Override** | Text | Empty | Custom name (overrides ItemData name) |
| **Interaction Duration** | float | 0.0 | Hold time (0 = instant) |
| **Interaction Priority** | int32 | 0 | Priority vs other interactables |
| **Interaction Radius** | float | 150.0 | Inherited from previous version |

---

## ?? ������� ������������

### Instant Pickup (�� ���������)
```
Interaction Duration = 0.0
```
���������: ����� E ? ����� ��������

### Hold-to-Pickup
```
Interaction Duration = 2.0
```
���������: ������� E 2 ������� ? progress bar ? ��������

### High Priority Item (Quest Key)
```
Interaction Priority = 100
Interaction Name Override = "Ancient Key"
```
���������: ��������� ���� ������� items, custom ��������

### Long Range Pickup
```
Interaction Radius = 300.0
```
���������: ����� ��������� ��������

---

## ?? ��������� �������

### Blueprint Callable:

```cpp
// Get interaction name
FText GetInteractionName()

// Get action text ("Press E to pick up...")
FText GetInteractionActionText()

// Check if has IInteractableInterface
bool HasInteractionInterface()

// Check if can be picked up
bool CanBePickedUpBy(AActor* Picker)

// Attempt pickup
bool PickupItem(AActor* Picker)
```

### Blueprint Events (Override):

```cpp
// Called when focused
Event OnInteractionFocusGained(AActor* InteractingActor)

// Called when unfocused
Event OnInteractionFocusLost(AActor* InteractingActor)

// Override interaction name
Event GetInteractionName() ? FText

// Override action text
Event GetInteractionActionText() ? FText
```

---

## ?? ������������

### Quick Reference
?? **[INTERACTION_QUICK_REF.md](INTERACTION_QUICK_REF.md)** - ������� �������

### Full Guides
?? **[INTERACTION_SYSTEM_INTEGRATION.md](INTERACTION_SYSTEM_INTEGRATION.md)** - ������ ���� ���� �������  
?? **[EXAMPLE_INTEGRATION_CLASS.md](EXAMPLE_INTEGRATION_CLASS.md)** - ������� C++ �����  
?? **[INTERACTION_IMPLEMENTATION_SUMMARY.md](INTERACTION_IMPLEMENTATION_SUMMARY.md)** - ����������� ������

### Changelog
?? **[v1.7.0_INTERACTION_INTEGRATION.md](v1.7.0_INTERACTION_INTEGRATION.md)** - ��� ������ � v1.7.0

---

## ? FAQ

### Q: ����� �� �������������� ������?
**A:** ���! ������ �������� as-is. ���������� �������� � ����� �������.

### Q: ����� �� �������� ��� IInteractableInterface?
**A:** ��! ��� ������ ������� (PickupItem, overlap events) �������� ��� ������.

### Q: ����� ����� �����?
**A:** 
- **Blueprint** - ��� �������� ���������������� � ����������
- **C++** - ��� production � performance-critical code
- **Component** - ���� ��� ����������� component-based ������

### Q: ����� �� ��������� ������?
**A:** ��! ��������, C++ ������� ����� + Blueprint overrides ��� ����������� items.

### Q: ����� performance impact?
**A:** ����������� (~0.1ms ��� ������ ��������, <0.001ms �����). ���� ��������� �� ������������ - zero overhead.

### Q: ���������� � ����������� ��������?
**A:** ��! 100% backwards compatible. Zero breaking changes.

---

## ?? Common Issues

### "Interface not detected"
**Solution:** 
- Check Blueprint Class Settings ? Interfaces ? IInteractableInterface added
- Verify `HasInteractionInterface()` returns true

### "Can't pickup item"
**Solution:**
- Check `CanBePickedUpBy()` returns true
- Ensure Player has InventoryComponent
- Verify inventory has space
- Check ItemData is valid

### "No visual feedback"
**Solution:**
- Implement `OnInteractionFocusGained/Lost` events
- Setup Post Process material for custom depth
- Enable Custom Depth in Project Settings

---

## ?? Visual Setup

### Outline Effect (Post Process)

1. **Create Post Process Material:**
   - Material Domain = Post Process
   - Blend Location = After Tonemapping

2. **Setup Nodes:**
```
Scene Texture: Custom Depth
    ?
Custom Depth Stencil Value = 252?
    ?? True ? Colored Outline
    ?? False ? Transparent
```

3. **Add to Level:**
   - Place Post Process Volume
   - Set Unbound = True
   - Add material to Post Process Materials array

4. **Enable in Project Settings:**
   - Rendering ? Custom Depth-Stencil Pass = Enabled with Stencil

---

## ? Checklist

Integration checklist:

- [ ] ������ ����� ���������� (Blueprint/C++/Component)
- [ ] ������ child class WorldItemActor
- [ ] ����������� interface functions ��� bind component events
- [ ] ��������� ���������� �������� ����� (outline, widget, etc.)
- [ ] ��������� �������� (duration, priority, radius)
- [ ] ������������� pickup flow
- [ ] ������ ���� �� ���� example item (health potion, key, etc.)
- [ ] ��������������� ��� �������

---

## ?? Next Steps

1. **������������� ������� pickup:**
   ```
   Spawn item ? Walk to item ? Press interact ? Item picked up
   ```

2. **�������� ���������� �������� �����:**
   ```
   Look at item ? Outline appears ? "Press E" widget shows
   ```

3. **�������� �������� items:**
   ```
   BP_HealthPotion (instant pickup)
   BP_QuestKey (hold-to-pickup, high priority)
   BP_Weapon (custom interaction text)
   ```

4. **�������������� ��� �������:**
   ```
   - ����� ����� ������������
   - ��� ������ code/blueprints
   - Best practices
   ```

---

## ?? Tips & Tricks

### Tip 1: Custom Interaction Text per Item Type

```blueprint
Event GetInteractionActionText
??? Switch on Enum (Item Type)
    ?? Health Potion ? "Drink Potion (E)"
    ?? Quest Item ? "Take Key (E)"
    ?? Weapon ? "Equip Weapon (E)"
```

### Tip 2: Conditional Pickup based on Quest

```cpp
bool AQuestKey::CanInteract_Implementation(AActor* InteractingActor) const
{
    if (!Super::CanInteract(InteractingActor))
        return false;
    
    // Additional check
    AMyPlayerController* PC = Cast<AMyPlayerController>(InteractingActor->GetInstigatorController());
    return PC && PC->HasStartedQuest("TempleQuest");
}
```

### Tip 3: Rarity-Based Outline Color

```cpp
void AWorldItemRare::BeginPlay()
{
    Super::BeginPlay();
    
    if (ItemData->Rarity == ERarity::Legendary)
     OutlineColor = FLinearColor::Orange;
    else if (ItemData->Rarity == ERarity::Rare)
        OutlineColor = FLinearColor::Purple;
}
```

---

## ?? You're Ready!

�� ������ ��� ����������. �������� ����� � �������� ���������������� �����!

**Estimated time:** 5-30 ����� � ����������� �� ���������

**Questions?** ������ ������ ������������ ���� ��� community forum!

**Happy integrating! ??**
