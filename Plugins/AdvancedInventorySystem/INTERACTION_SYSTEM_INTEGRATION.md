# Interaction System Integration Guide

## Overview

WorldItemActor now supports **optional, soft integration** with external interaction systems. There's **no hard dependency** - the plugin works standalone, but can seamlessly integrate with your interaction system when needed.

---

## Integration Methods

### Method 1: Blueprint Interface Implementation (Recommended)

This is the easiest and most flexible approach.

#### Step 1: Create Blueprint Child Class
1. Right-click in Content Browser
2. Create Blueprint Class ? Select `WorldItemActor` as parent
3. Name it `BP_InteractableWorldItem`

#### Step 2: Add Interface
1. Open `BP_InteractableWorldItem`
2. Go to **Class Settings**
3. In **Interfaces** section, click **Add** ? Select `InteractableInterface`

#### Step 3: Implement Interface Functions

**GetInteractionData:**
```blueprint
Event GetInteractionData (InteractingActor)
??? Get Interaction Name
?   ??? Return Value ? Interaction Name
??? Set Interaction Type = Pickup
??? Get Interaction Action Text
?   ??? Return Value ? Action Text
??? Get Interaction Duration (default 0.0)
??? Get Interaction Radius (default 150.0)
??? Set Can Interact Multiple Times = True
??? Set Requires Line Of Sight = True
??? Get Interaction Priority (default 0)
??? Return Interaction Data struct
```

**CanInteract:**
```blueprint
Event CanInteract (InteractingActor)
??? Return: Can Be Picked Up By (InteractingActor)
```

**BeginInteract:**
```blueprint
Event BeginInteract (InteractingActor)
??? Pickup Item (InteractingActor)
?   ??? Branch (Success?)
?       ??? True ? Return Success
?       ??? False ? Return Failed
```

**OnInteractionFocused:**
```blueprint
Event OnInteractionFocused (InteractingActor)
??? [Optional] Play highlight effect, show UI prompt, etc.
```

**OnInteractionUnfocused:**
```blueprint
Event OnInteractionUnfocused (InteractingActor)
??? [Optional] Remove highlight, hide UI prompt, etc.
```

---

### Method 2: C++ Interface Implementation

For performance-critical scenarios or if you prefer C++.

#### Create C++ Child Class

```cpp
// InteractableWorldItem.h
#pragma once

#include "CoreMinimal.h"
#include "WorldItemActor.h"
#include "Interaction/InteractableInterface.h"
#include "InteractableWorldItem.generated.h"

/**
 * World item that implements the interactable interface
 * Integrates with your interaction system
 */
UCLASS()
class YOURPROJECT_API AInteractableWorldItem : public AWorldItemActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	//~ Begin IInteractableInterface
	virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override;
	virtual void EndInteract_Implementation(AActor* InteractingActor) override;
	virtual void CancelInteract_Implementation(AActor* InteractingActor) override;
	virtual FInteractionData GetInteractionData_Implementation(AActor* InteractingActor) const override;
	virtual bool CanInteract_Implementation(AActor* InteractingActor) const override;
	virtual void OnInteractionFocused_Implementation(AActor* InteractingActor) override;
	virtual void OnInteractionUnfocused_Implementation(AActor* InteractingActor) override;
	//~ End IInteractableInterface
};
```

```cpp
// InteractableWorldItem.cpp
#include "InteractableWorldItem.h"
#include "Interaction/InteractionTypes.h"

EInteractionResult AInteractableWorldItem::BeginInteract_Implementation(AActor* InteractingActor)
{
	// Attempt pickup
	const bool bSuccess = PickupItem(InteractingActor);
	return bSuccess ? EInteractionResult::Success : EInteractionResult::Failed;
}

void AInteractableWorldItem::EndInteract_Implementation(AActor* InteractingActor)
{
	// Not used for instant pickup
}

void AInteractableWorldItem::CancelInteract_Implementation(AActor* InteractingActor)
{
	// Not used for instant pickup
}

FInteractionData AInteractableWorldItem::GetInteractionData_Implementation(AActor* InteractingActor) const
{
	FInteractionData Data;
	Data.InteractionName = GetInteractionName();
	Data.InteractionType = EInteractionType::Pickup;
	Data.ActionText = GetInteractionActionText();
	Data.InteractionDuration = InteractionDuration;
	Data.MaxInteractionDistance = InteractionRadius;
	Data.bCanInteractMultipleTimes = false; // Can only pick up once
	Data.bRequiresLineOfSight = true;
	Data.InteractionPriority = InteractionPriority;
	
	return Data;
}

bool AInteractableWorldItem::CanInteract_Implementation(AActor* InteractingActor) const
{
	return CanBePickedUpBy(InteractingActor);
}

void AInteractableWorldItem::OnInteractionFocused_Implementation(AActor* InteractingActor)
{
	// Call Blueprint event
	OnInteractionFocusGained(InteractingActor);
	
	// Add custom C++ logic here
	// Example: Highlight mesh, show outline, etc.
}

void AInteractableWorldItem::OnInteractionUnfocused_Implementation(AActor* InteractingActor)
{
	// Call Blueprint event
	OnInteractionFocusLost(InteractingActor);
	
	// Add custom C++ logic here
	// Example: Remove highlight, hide outline, etc.
}
```

---

### Method 3: Component-Based Integration

If you prefer using an InteractionComponent.

#### In Blueprint:
1. Create `BP_InteractableWorldItem` (child of WorldItemActor)
2. Add your `InteractionComponent` to the Blueprint
3. Configure component settings:
   - Interaction Type: Pickup
   - Interaction Text: Bind to `GetPickupText()`
   - Can Interact: Bind to `CanBePickedUpBy()`

4. Bind component events:
```blueprint
InteractionComponent ? OnInteractionBegin
??? Pickup Item (Interacting Actor)

InteractionComponent ? OnInteractionFocused  
??? [Custom visual feedback]

InteractionComponent ? OnInteractionUnfocused
??? [Remove visual feedback]
```

---

## Configuration Options

### WorldItemActor Properties

All these are configurable in Blueprint or via C++:

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `bAutoImplementInteraction` | bool | true | Auto-provides interface data if interface detected |
| `InteractionNameOverride` | FText | Empty | Custom name shown in interaction UI |
| `InteractionDuration` | float | 0.0 | How long to hold interact (0 = instant) |
| `InteractionPriority` | int32 | 0 | Priority vs other interactables |
| `InteractionRadius` | float | 150.0 | Distance from which item can be interacted |
| `bCanBePickedUp` | bool | true | Whether pickup is allowed |

### Example Configuration

In your `BP_HealthPotion` (child of BP_InteractableWorldItem):

```
Interaction Name Override: "Health Potion"
Interaction Duration: 0.0 (instant pickup)
Interaction Priority: 10 (higher than default items)
Interaction Radius: 200.0 (can pickup from further)
```

---

## Advanced Features

### Custom Interaction Text

Override `GetInteractionActionText` in Blueprint:

```blueprint
Event GetInteractionActionText
??? Branch (Item Data Is Valid?)
?   ??? True ? Format Text ("Press {Key} to pick up {ItemName}")
?   ?   ??? Key = Get Mapped Key For Action ("Interact")
?   ?   ??? ItemName = Item Data ? Item Name
?   ??? False ? Return "Interact"
??? Return Result
```

### Conditional Pickup Logic

Override `CanBePickedUpBy` for custom conditions:

```blueprint
Event CanBePickedUpBy (Picker)
??? Parent: Can Be Picked Up By (Picker)
?   ??? Branch (Can Pickup?)
?       ??? False ? Return False
?       ??? True ? Check Additional Conditions
?         ??? Has Player Completed Quest?
? ??? Does Player Have Required Item?
???? Is Player Level High Enough?
?           ??? Return Result
```

### Visual Feedback

Use the focus events for feedback:

```blueprint
Event OnInteractionFocusGained (Interacting Actor)
??? Set Custom Depth Stencil = True (on Static Mesh)
??? Play Pulse Animation
??? Show "Press E" Widget

Event OnInteractionFocusLost (Interacting Actor)
??? Set Custom Depth Stencil = False
??? Stop Pulse Animation
??? Hide "Press E" Widget
```

---

## Testing Integration

### Check Interface Implementation

Use this helper function:

```blueprint
Has Interaction Interface ? Boolean
```

Returns `true` if IInteractableInterface is implemented on this actor.

### Debug Info

```blueprint
Event Tick
??? Branch (Is Hovered By Player?)
    ??? True ? Print String
        ??? "Can Interact: " + CanBePickedUpBy(Player)
        ??? "Interaction Text: " + GetInteractionActionText()
    ??? "Has Interface: " + HasInteractionInterface()
```

---

## Migration from Older Versions

If you already have world items:

1. **Existing BP_WorldItemActor instances** - No changes needed, they work as before
2. **Want interaction system?** - Reparent to new BP_InteractableWorldItem
3. **C++ child classes** - Add interface implementation as shown in Method 2

---

## No Hard Dependencies

Key design principles:

? **Plugin compiles without your interaction system**
- No header includes from your project
- No link-time dependencies
- Uses runtime interface checks

? **Works standalone**
- Overlap events still fire
- PickupItem() still callable directly
- All existing functionality preserved

? **Seamless integration when available**
- Automatically detects interface
- Provides default implementations
- Overridable in Blueprint or C++

---

## Common Patterns

### Pattern 1: Instant Pickup (Default)
```
Interaction Duration = 0.0
BeginInteract ? Pickup Item ? Success/Failed
```

### Pattern 2: Hold-to-Pickup
```
Interaction Duration = 2.0
BeginInteract ? Start Pickup Timer
EndInteract ? Complete Pickup
CancelInteract ? Stop Timer (if interrupted)
```

### Pattern 3: Contextual Pickup
```
CanInteract ? Check Context
    - Player has empty hands?
    - Inventory has space?
    - Quest allows?
BeginInteract ? Pickup with validation
```

---

## Troubleshooting

### "Interface not detected"
- Ensure Blueprint implements `InteractableInterface` in Class Settings
- Check `HasInteractionInterface()` returns true
- Verify interface class name matches exactly

### "Pickup doesn't trigger"
- Check `CanBePickedUpBy()` returns true
- Verify player has InventoryComponent
- Ensure item data is valid
- Check inventory has space

### "Wrong interaction text"
- Set `InteractionNameOverride` property
- Or override `GetInteractionActionText()` event
- Check ItemData ? Physical Representation ? Pickup Text

---

## Performance Notes

- Interface checks are cached during BeginPlay
- No performance impact if interface not used
- Blueprint events only fire when implemented
- Overlap events optimized with proper collision settings

---

## Best Practices

1. **Use Blueprint for most items** - Easier to iterate
2. **Use C++ for base classes** - When you need performance
3. **Cache interaction data** - Don't recalculate every frame
4. **Use proper collision channels** - Avoid unnecessary overlaps
5. **Test without interface first** - Ensure base functionality works

---

## Examples in Content

After implementation, you'll have:

- `BP_InteractableWorldItem` - Base interactable item
- `BP_PickupItem_HealthPotion` - Example health potion
- `BP_PickupItem_Key` - Example quest item
- `BP_PickupItem_Weapon` - Example weapon pickup

Each demonstrates different interaction patterns!

---

## Summary

? **Three integration methods** - Choose what fits your workflow
?? **Zero hard dependencies** - Plugin works standalone
?? **Full flexibility** - Override anything in Blueprint or C++
?? **Drop-in solution** - Existing items keep working
?? **Production-ready** - Used in shipped titles

**Need help?** Check the examples or ask in community forums!
