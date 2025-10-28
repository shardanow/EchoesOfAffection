# Interaction System Integration Guide

**How to integrate WorldItemActor with your Interaction System**

---

## ?? Overview

`WorldItemActor` is designed to be **flexible**. You have two options for integration:

### Option A: Blueprint Integration (Recommended)
- Create Blueprint child class
- Add `InteractableComponent` or implement `IInteractableInterface`
- Connect to `PickupItem()` function
- **No C++ code needed!**

### Option B: C++ Extension
- Create C++ child class inheriting from `AWorldItemActor`
- Implement `IInteractableInterface`
- Custom pickup logic

---

## ? Option A: Blueprint Integration (Recommended)

### Step 1: Create Blueprint Class

1. **Content Browser** ? `Content/Items/`
2. Right-click `WorldItemActor` (C++ class)
3. **Create Blueprint class based on WorldItemActor**
4. Name: `BP_WorldItem_Interactable`

### Step 2: Add Interactable Component

1. Open `BP_WorldItem_Interactable`
2. **Add Component** ? Search: `InteractableComponent`
3. Click **Add Component**

> **Note:** If your Interaction System uses a component approach

### Step 3: Implement Interface (Alternative)

If your system uses `IInteractableInterface` instead:

1. **Class Settings** ? **Implemented Interfaces**
2. Click **Add** ? Search: `InteractableInterface`
3. Click **Add**

### Step 4: Implement Interaction Logic

#### Event Graph:

```blueprint
Event BeginInteract (from IInteractableInterface)
  Input: Interacting Actor
  Output: Interaction Result
  ?
  ?? Can Be Picked Up By?
  ?   Input: Interacting Actor
  ?   Output: Can Pickup?
  ?   ?
  ?   ?? Branch (Can Pickup?)
  ?       ?
  ?       ?? True:
  ?       ?   ?
  ?       ?   ?? Pickup Item
  ?       ?   Input: Interacting Actor
  ?       ?       Output: Success?
  ?       ?     ?
  ?    ?       ?? Branch (Success?)
  ?       ?           ?
  ?       ?   ?? True:
  ?       ?           ?   ?? Return: EInteractionResult::Success
  ?     ?           ?
  ?       ?           ?? False:
  ?       ? ?? Return: EInteractionResult::Failed
  ?       ?
  ?       ?? False:
  ?   ?? Return: EInteractionResult::NotInteractable
```

### Step 5: Implement Get Interaction Data

```blueprint
Event GetInteractionData (from IInteractableInterface)
  Input: Interacting Actor
  Output: Interaction Data
  ?
  ?? Make Interaction Data
  ?   Interaction Name: [ItemData ? Item Name]
  ?   Interaction Type: Pickup
  ?   Action Text: [Get Pickup Text]
  ?   Interaction Duration: 0.0
  ?   Max Interaction Distance: [Interaction Radius]
  ? Can Interact Multiple Times: false
  ?   Requires Line Of Sight: true
  ?   Interaction Priority: 0
  ?   ?
  ?   ?? Return: Interaction Data
```

### Step 6: Implement Can Interact

```blueprint
Event CanInteract (from IInteractableInterface)
  Input: Interacting Actor
  Output: Can Interact? (bool)
  ?
  ?? Return: Can Be Picked Up By?
  Input: Interacting Actor
```

### Step 7: Add Visual Feedback (Optional)

```blueprint
Event OnInteractionFocused (from IInteractableInterface)
  Input: Interacting Actor
  ?
  ?? Get Static Mesh Component
  ?   ?
  ?   ?? Set Render Custom Depth
  ?       Enabled: true
  ?       ?
  ?       ?? Set Custom Depth Stencil Value
  ?         Value: 250


Event OnInteractionUnfocused (from IInteractableInterface)
  Input: Interacting Actor
  ?
  ?? Get Static Mesh Component
      ?
  ?? Set Render Custom Depth
    Enabled: false
```

### Step 8: Done!

Now use `BP_WorldItem_Interactable` instead of `WorldItemActor` on your levels.

---

## ?? Example: Glowing Effect on Focus

```blueprint
Variables:
  - OutlineMaterial (Material Instance Dynamic)
  - bIsHighlighted (Boolean)

Event BeginPlay
  ?
  ?? Get Static Mesh Component
?
      ?? Create Dynamic Material Instance
  Source Material: M_Item_Outline
   ?
          ?? Save as: OutlineMaterial


Event OnInteractionFocused
  ?
  ?? Set bIsHighlighted: true
  ?
?? Get Static Mesh Component
  ?   ?? Set Material (1, OutlineMaterial)
  ?
  ?? Timeline: Pulse Glow
      ?
      ?? OutlineMaterial ? Set Scalar Parameter
          Name: "GlowIntensity"
          Value: [Timeline Float Track]


Event OnInteractionUnfocused
  ?
  ?? Set bIsHighlighted: false
  ?
  ?? Get Static Mesh Component
      ?? Set Material (1, null)
```

---

## ??? Option B: C++ Extension

### Step 1: Create C++ Child Class

**WorldItemActor_Interactable.h:**

```cpp
#pragma once

#include "CoreMinimal.h"
#include "WorldItemActor.h"
#include "Interaction/InteractableInterface.h"
#include "WorldItemActor_Interactable.generated.h"

UCLASS()
class YOURPROJECT_API AWorldItemActor_Interactable : public AWorldItemActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// IInteractableInterface
	virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override;
	virtual void EndInteract_Implementation(AActor* InteractingActor) override;
	virtual void CancelInteract_Implementation(AActor* InteractingActor) override;
	virtual FInteractionData GetInteractionData_Implementation(AActor* InteractingActor) const override;
	virtual bool CanInteract_Implementation(AActor* InteractingActor) const override;
	virtual void OnInteractionFocused_Implementation(AActor* InteractingActor) override;
	virtual void OnInteractionUnfocused_Implementation(AActor* InteractingActor) override;
};
```

**WorldItemActor_Interactable.cpp:**

```cpp
#include "WorldItemActor_Interactable.h"
#include "Interaction/InteractionTypes.h"

EInteractionResult AWorldItemActor_Interactable::BeginInteract_Implementation(AActor* InteractingActor)
{
	if (!CanInteract_Implementation(InteractingActor))
	{
		return EInteractionResult::NotInteractable;
	}

	bool bSuccess = PickupItem(InteractingActor);
	return bSuccess ? EInteractionResult::Success : EInteractionResult::Failed;
}

void AWorldItemActor_Interactable::EndInteract_Implementation(AActor* InteractingActor)
{
	// Pickup is instant
}

void AWorldItemActor_Interactable::CancelInteract_Implementation(AActor* InteractingActor)
{
	// Pickup is instant
}

FInteractionData AWorldItemActor_Interactable::GetInteractionData_Implementation(AActor* InteractingActor) const
{
	FInteractionData Data;
	
	if (ItemData)
	{
		Data.InteractionName = ItemData->ItemName;
		Data.InteractionType = EInteractionType::Pickup;
		Data.ActionText = GetPickupText();
		Data.InteractionDuration = 0.0f;
		Data.MaxInteractionDistance = InteractionRadius;
		Data.bCanInteractMultipleTimes = false;
		Data.bRequiresLineOfSight = true;
		Data.InteractionPriority = 0;
	}
	
	return Data;
}

bool AWorldItemActor_Interactable::CanInteract_Implementation(AActor* InteractingActor) const
{
	return CanBePickedUpBy(InteractingActor);
}

void AWorldItemActor_Interactable::OnInteractionFocused_Implementation(AActor* InteractingActor)
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(true);
		StaticMeshComponent->SetCustomDepthStencilValue(250);
	}
}

void AWorldItemActor_Interactable::OnInteractionUnfocused_Implementation(AActor* InteractingActor)
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRenderCustomDepth(false);
	}
}
```

### Step 2: Use in Project

Replace `AWorldItemActor` with `AWorldItemActor_Interactable` in your spawning code.

---

## ?? Testing

### Test Checklist:

- [ ] Item highlights when player looks at it
- [ ] Interaction prompt shows (via GetInteractionData)
- [ ] Press E ? item picked up
- [ ] Item added to inventory
- [ ] World actor destroyed/hidden
- [ ] Pickup sound/effects play (if implemented)

### Debug:

```blueprint
Event BeginInteract
  ?
  ?? Print String
      Text: "Attempting pickup: {ItemName}"
      ?
      ?? Pickup Item
          ?
          ?? Branch (Success?)
              ?? True: Print String ("Pickup successful!")
    ?? False: Print String ("Pickup failed!")
```

---

## ?? Which Option to Choose?

| Scenario | Recommendation |
|----------|----------------|
| **Simple pickup** | Option A (Blueprint) |
| **Custom visuals** | Option A (Blueprint) |
| **Performance critical** | Option B (C++) |
| **Complex logic** | Option B (C++) |
| **Prototype/iteration** | Option A (Blueprint) |
| **Final/shipping** | Either (both are production-ready) |

---

## ?? Tips

### Tip 1: Reusable Blueprint

Create `BP_WorldItem_Interactable` as a base class, then:
- `BP_WorldItem_Potion` ? inherits from it
- `BP_WorldItem_Weapon` ? inherits from it
- etc.

All automatically work with Interaction System!

### Tip 2: Data Table

Use Data Table to configure all items:

```
Items Data Table (DT_Items):
  - HealthPotion ? DA_Potion_Health
  - ManaSword ? DA_Weapon_Sword
  - etc.
```

Spawn from table:

```blueprint
Function: Spawn Item From Table
  Input: Item ID (Name)
  ?
  ?? Get Data Table Row
  ?   Table: DT_Items
  ?   Row: Item ID
  ?   Output: Item Data
  ?   ?
  ?   ?? Spawn Actor
  ?       Class: BP_WorldItem_Interactable
  ?   ?
  ?       ?? Initialize Item
  ?         ItemData: [from table]
```

### Tip 3: Pooling

For many items (100+), use object pooling:

```blueprint
Item Pool (Array of BP_WorldItem_Interactable):
  - Pre-spawn 50 actors
  - Hide them
  - Reuse when needed (SetActorLocation, InitializeItem)
  - Return to pool when picked up
```

---

## ?? Related Documentation

- **WorldItemActor.h** - C++ class reference
- **USER_GUIDE.md** § Creating Items
- **BLUEPRINT_EXAMPLES.md** - More Blueprint examples

---

## ?? Common Issues

### Item doesn't highlight?

? Check:
- InteractableInterface is implemented
- OnInteractionFocused is implemented
- Custom Depth is enabled in Project Settings

### PickupItem not called?

? Check:
- BeginInteract calls PickupItem()
- InteractionComponent is on player
- Item has collision enabled

### Can't find InteractableInterface?

? Make sure:
- Your Interaction System module is loaded
- Include path is correct
- Module dependency added to `.Build.cs`

---

**Happy Integrating!** ??

