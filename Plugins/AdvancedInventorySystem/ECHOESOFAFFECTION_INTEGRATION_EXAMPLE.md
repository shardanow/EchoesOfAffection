# Example: Integrating WorldItemActor with Your Interaction System

## For Your Project: EchoesOfAffection

This is a practical example specifically for your project.

---

## ?? File Location

Place this in your project (NOT in plugin):

```
Source/
??? EchoesOfAffection/
    ??? Public/
    ???? Items/
    ?    ??? InteractableWorldItem.h
    ??? Private/
        ??? Items/
            ??? InteractableWorldItem.cpp
```

---

## ?? InteractableWorldItem.h

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldItemActor.h"
#include "Interaction/InteractableInterface.h"
#include "InteractableWorldItem.generated.h"

/**
 * World item integrated with EchoesOfAffection interaction system
 * 
 * This class bridges the AdvancedInventorySystem plugin with your Interaction system.
 * It's in YOUR project, not in the plugin, maintaining clean separation.
 * 
 * Usage:
 * 1. Use this as base class for all pickupable items
 * 2. Or create Blueprint children for specific item types
 * 3. Configure properties in Blueprint as needed
 */
UCLASS()
class ECHOESOFAFFECTION_API AInteractableWorldItem 
    : public AWorldItemActor
    , public IInteractableInterface
{
    GENERATED_BODY()

public:
    AInteractableWorldItem();

    //~ Begin IInteractableInterface
    virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override;
  virtual void EndInteract_Implementation(AActor* InteractingActor) override;
  virtual void CancelInteract_Implementation(AActor* InteractingActor) override;
    virtual FInteractionData GetInteractionData_Implementation(AActor* InteractingActor) const override;
    virtual bool CanInteract_Implementation(AActor* InteractingActor) const override;
    virtual void OnInteractionFocused_Implementation(AActor* InteractingActor) override;
    virtual void OnInteractionUnfocused_Implementation(AActor* InteractingActor) override;
    //~ End IInteractableInterface

protected:
    /** Visual feedback configuration */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Visual")
    bool bEnableOutlineOnFocus = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Visual")
    FLinearColor OutlineColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Visual")
    int32 CustomDepthStencilValue = 252;

private:
    bool bIsCurrentlyFocused = false;
};
```

---

## ?? InteractableWorldItem.cpp

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/InteractableWorldItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

AInteractableWorldItem::AInteractableWorldItem()
{
    bEnableOutlineOnFocus = true;
    OutlineColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);
    CustomDepthStencilValue = 252;
    bIsCurrentlyFocused = false;
}

EInteractionResult AInteractableWorldItem::BeginInteract_Implementation(AActor* InteractingActor)
{
    if (!InteractingActor)
    {
        return EInteractionResult::Failed;
    }

    // Instant pickup (InteractionDuration == 0)
    if (InteractionDuration <= 0.0f)
    {
        const bool bSuccess = PickupItem(InteractingActor);
  return bSuccess ? EInteractionResult::Success : EInteractionResult::Failed;
    }

    // Hold-to-pickup (InteractionDuration > 0)
// Return Success to allow interaction system to start timer
    // Actual pickup happens in EndInteract
    return EInteractionResult::Success;
}

void AInteractableWorldItem::EndInteract_Implementation(AActor* InteractingActor)
{
  if (!InteractingActor)
    {
        return;
    }

    // Called after InteractionDuration elapsed
    if (InteractionDuration > 0.0f)
    {
    PickupItem(InteractingActor);
    }
}

void AInteractableWorldItem::CancelInteract_Implementation(AActor* InteractingActor)
{
    // Interaction cancelled (player moved away, etc.)
    UE_LOG(LogTemp, Log, TEXT("Pickup cancelled for %s"), *GetName());
}

FInteractionData AInteractableWorldItem::GetInteractionData_Implementation(AActor* InteractingActor) const
{
    FInteractionData Data;
    
    // Use WorldItemActor's helper functions
    Data.InteractionName = GetInteractionName();
    Data.InteractionType = EInteractionType::Pickup;
    Data.ActionText = GetInteractionActionText();
    Data.InteractionDuration = InteractionDuration;
    Data.MaxInteractionDistance = InteractionRadius;
Data.bCanInteractMultipleTimes = false; // Can only pickup once
    Data.bRequiresLineOfSight = true;
    Data.InteractionPriority = InteractionPriority;
    
    return Data;
}

bool AInteractableWorldItem::CanInteract_Implementation(AActor* InteractingActor) const
{
    // Use WorldItemActor's logic
    return CanBePickedUpBy(InteractingActor);
}

void AInteractableWorldItem::OnInteractionFocused_Implementation(AActor* InteractingActor)
{
    if (bIsCurrentlyFocused)
    {
        return; // Already focused
    }

    bIsCurrentlyFocused = true;

    // Visual feedback - outline
    if (bEnableOutlineOnFocus)
    {
        if (StaticMeshComponent && StaticMeshComponent->IsVisible())
        {
            StaticMeshComponent->SetRenderCustomDepth(true);
         StaticMeshComponent->SetCustomDepthStencilValue(CustomDepthStencilValue);
        }

        if (SkeletalMeshComponent && SkeletalMeshComponent->IsVisible())
        {
   SkeletalMeshComponent->SetRenderCustomDepth(true);
         SkeletalMeshComponent->SetCustomDepthStencilValue(CustomDepthStencilValue);
        }
    }

    // Call Blueprint event for additional custom logic
    OnInteractionFocusGained(InteractingActor);
}

void AInteractableWorldItem::OnInteractionUnfocused_Implementation(AActor* InteractingActor)
{
    if (!bIsCurrentlyFocused)
    {
        return; // Not focused
    }

    bIsCurrentlyFocused = false;

    // Remove visual feedback
    if (bEnableOutlineOnFocus)
    {
 if (StaticMeshComponent)
    {
            StaticMeshComponent->SetRenderCustomDepth(false);
        }

        if (SkeletalMeshComponent)
  {
      SkeletalMeshComponent->SetRenderCustomDepth(false);
      }
    }

    // Call Blueprint event
    OnInteractionFocusLost(InteractingActor);
}
```

---

## ?? Update Build.cs

Add to `EchoesOfAffection.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[] 
{ 
    "Core", 
    "CoreUObject", 
    "Engine", 
    "InputCore",
    "AdvancedInventorySystem" // Add this
});
```

---

## ?? Create Blueprint Examples

### 1. BP_InteractableWorldItem (Base)

**Parent Class:** AInteractableWorldItem
**Purpose:** Base class for all items

**No additional logic needed** - Just use as parent!

---

### 2. BP_HealthPotion

**Parent Class:** BP_InteractableWorldItem

**Configuration:**
```
Item Data: DA_HealthPotion
Quantity: 1
Interaction Duration: 0.0 (instant)
Interaction Priority: 10
Interaction Name Override: (leave empty - uses ItemData name)
Enable Outline On Focus: true
Outline Color: (0.0, 1.0, 0.0) - Green
```

**Optional Override (Event Graph):**

```blueprint
Event OnInteractionFocusGained
??? Parent: OnInteractionFocusGained
??? Play Sound at Location
?   ??? Sound: SFX_Item_Hover
??? Spawn Emitter Attached
    ??? Template: P_Item_Glow
    ??? Attach to: Static Mesh Component
```

---

### 3. BP_QuestKey

**Parent Class:** BP_InteractableWorldItem

**Configuration:**
```
Item Data: DA_QuestKey
Quantity: 1
Interaction Duration: 0.5 (slight delay for drama)
Interaction Priority: 100 (very high)
Interaction Name Override: "Ancient Temple Key"
Outline Color: (1.0, 0.8, 0.0) - Gold
```

**Override CanInteract (Event Graph):**

```blueprint
Event CanInteract (Interacting Actor)
??? Parent: CanInteract (Interacting Actor) ? Can Pickup?
??? Branch (Can Pickup?)
?   ??? False ? Return False
?   ??? True ? Continue
??? Get Player Controller
??? Cast to MyPlayerController
??? Has Started Quest? ("TempleQuest")
??? Return Result
```

This restricts pickup to players who have started the quest!

---

### 4. BP_WeaponPickup

**Parent Class:** BP_InteractableWorldItem

**Configuration:**
```
Item Data: DA_Sword
Quantity: 1
Interaction Duration: 1.0 (hold to equip)
Interaction Priority: 50
Interaction Name Override: (empty - uses weapon name from data)
Outline Color: (0.0, 0.5, 1.0) - Blue
```

**Override GetInteractionActionText (Event Graph):**

```blueprint
Event GetInteractionActionText
??? Get Item Data ? Item Name
??? Format Text: "Hold E to equip {0}"
?   ??? 0 = Item Name
??? Return Formatted Text
```

---

## ?? Usage in Level

### Place in World

1. Drag BP_HealthPotion into level
2. Position as needed
3. That's it! Your interaction system will detect it automatically

### Spawn at Runtime

```cpp
// C++
FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 100);
AInteractableWorldItem* Item = GetWorld()->SpawnActor<AInteractableWorldItem>(
    BP_HealthPotion, 
    SpawnLocation, 
    FRotator::ZeroRotator
);
Item->InitializeItem(HealthPotionData, 1);
```

```blueprint
// Blueprint
Spawn Actor from Class
??? Class: BP_HealthPotion
??? Spawn Transform: Make Transform (Location, Rotation, Scale)
??? Collision Handling Override: Always Spawn
    ??? Initialize Item
      ??? Item Data: DA_HealthPotion
        ??? Quantity: 1
```

---

## ? Testing Checklist

### Basic Functionality
- [ ] Item spawns correctly
- [ ] Outline appears when looking at item
- [ ] "Press E" prompt shows (from your interaction system)
- [ ] Pressing E picks up item
- [ ] Item added to inventory
- [ ] Item removed from world

### Visual Feedback
- [ ] Outline color matches configuration
- [ ] Outline disappears when looking away
- [ ] Optional effects (sound, particles) work

### Edge Cases
- [ ] Can't pickup with full inventory (shows message)
- [ ] Can't pickup quest item without quest (if applicable)
- [ ] Multiple items nearby - priority works correctly
- [ ] Network replication (if multiplayer)

---

## ?? Troubleshooting

### Issue: Outline not visible

**Solution:**
1. Check Project Settings ? Rendering ? Custom Depth-Stencil Pass = Enabled with Stencil
2. Add Post Process Volume to level (Unbound = true)
3. Create/add post process material for outline rendering

### Issue: Interaction not detected

**Solution:**
1. Verify `HasInteractionInterface()` returns true (add debug print)
2. Check Interaction Radius (should be >0, default 150)
3. Ensure your interaction system is checking for IInteractableInterface

### Issue: Can't pickup item

**Solution:**
1. Check player has InventoryComponent
2. Verify inventory has space
3. Check `CanBePickedUpBy()` returns true
4. Ensure ItemData is valid

---

## ?? Next Steps

1. **Compile project** - Should compile without errors
2. **Create blueprints** - Use examples above
3. **Test in level** - Verify all functionality
4. **Add more items** - Reuse AInteractableWorldItem
5. **Customize** - Override events in Blueprint as needed

---

## ?? Tips

### Tip 1: Color Code by Rarity

```cpp
void AInteractableWorldItem::BeginPlay()
{
    Super::BeginPlay();
    
    if (ItemData)
    {
        // Example with tags
        if (ItemData->ItemTags.HasTag(FGameplayTag::RequestGameplayTag("Item.Rarity.Legendary")))
    {
            OutlineColor = FLinearColor(1.0f, 0.5f, 0.0f); // Orange
        }
        else if (ItemData->ItemTags.HasTag(FGameplayTag::RequestGameplayTag("Item.Rarity.Rare")))
        {
      OutlineColor = FLinearColor(0.5f, 0.0f, 1.0f); // Purple
        }
        // etc.
    }
}
```

### Tip 2: Custom Pickup Sound

```blueprint
Event OnItemPickedUp (Picker)
??? Parent: OnItemPickedUp
??? Get Item Data ? Item Tags
??? Has Tag? ("Item.Type.Potion")
?   ??? True ? Play Sound (SFX_Potion_Pickup)
?   ??? False ? Has Tag? ("Item.Type.Weapon")
?       ??? True ? Play Sound (SFX_Weapon_Equip)
?   ??? False ? Play Sound (SFX_Generic_Pickup)
```

### Tip 3: Animated Pickup

```blueprint
Event BeginInteract (Interacting Actor)
??? Branch (Interaction Duration > 0?)
?   ??? True ? Play Animation Montage (AM_Pickup)
??? Parent: BeginInteract (Interacting Actor)
??? Return Result
```

---

## ?? You're All Set!

Your project now has seamless integration between:
- ? AdvancedInventorySystem plugin
- ? Your Interaction System
- ? Clean separation (no plugin modification)
- ? Flexible and extensible

**Questions?** Check the plugin documentation:
- INTERACTION_QUICK_START.md
- INTERACTION_SYSTEM_INTEGRATION.md
- EXAMPLE_INTEGRATION_CLASS.md

**Happy developing! ??**
