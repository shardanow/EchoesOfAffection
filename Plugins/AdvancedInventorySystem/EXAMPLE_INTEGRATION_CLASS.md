# Quick Integration Example: C++ Helper Class

## For Your Project (NOT in Plugin)

Place this in your project's Source folder to enable easy interaction system integration.

---

## File: YourProject/Public/Interaction/InteractableWorldItem.h

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldItemActor.h"
#include "Interaction/InteractableInterface.h"
#include "Interaction/InteractionTypes.h"
#include "InteractableWorldItem.generated.h"

/**
 * Example integration class showing how to connect WorldItemActor with your Interaction System
 * 
 * This class lives in YOUR PROJECT (not in the plugin), creating a soft dependency bridge.
 * 
 * Usage:
 * 1. Copy this file to YourProject/Source/YourProject/Public/Interaction/
 * 2. Copy the .cpp to YourProject/Source/YourProject/Private/Interaction/
 * 3. Use this as parent for your Blueprint items (or use directly)
 * 4. The plugin remains independent - you can remove this file anytime
 */
UCLASS()
class ECHOESOFAFFECTION_API AInteractableWorldItem : public AWorldItemActor, public IInteractableInterface
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
	/** Whether to use visual highlight when focused */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Visual")
	bool bEnableOutlineOnFocus = true;

	/** Outline color when focused */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Visual")
	FLinearColor OutlineColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);

	/** Custom depth stencil value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Visual")
	int32 CustomDepthStencilValue = 252;

private:
	/** Internal state tracking */
	bool bIsCurrentlyFocused = false;
};
```

---

## File: YourProject/Private/Interaction/InteractableWorldItem.cpp

```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/InteractableWorldItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

AInteractableWorldItem::AInteractableWorldItem()
{
	// Set defaults for visual feedback
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

	// For instant pickup (InteractionDuration == 0.0)
	if (InteractionDuration <= 0.0f)
	{
		const bool bSuccess = PickupItem(InteractingActor);
		return bSuccess ? EInteractionResult::Success : EInteractionResult::Failed;
	}

	// For hold-to-pickup, return success immediately
	// The actual pickup happens in EndInteract after duration completes
	return EInteractionResult::Success;
}

void AInteractableWorldItem::EndInteract_Implementation(AActor* InteractingActor)
{
	if (!InteractingActor)
	{
		return;
	}

	// This is called after InteractionDuration has elapsed
	// Complete the pickup
	if (InteractionDuration > 0.0f)
	{
		PickupItem(InteractingActor);
	}
}

void AInteractableWorldItem::CancelInteract_Implementation(AActor* InteractingActor)
{
	// Called if interaction is interrupted before completion
	// For pickup, we don't need to do anything special
	// Just log for debugging if needed
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

	// Visual feedback - highlight the mesh
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

## Usage Examples

### Example 1: Simple Instant Pickup

In your Blueprint (child of AInteractableWorldItem):

1. Set `Item Data` to your health potion asset
2. Set `Quantity` to 1
3. Set `Interaction Duration` to 0.0 (instant)
4. Done! It will work with your interaction system

### Example 2: Hold-to-Pickup Item

```cpp
// In Blueprint or C++ defaults
InteractionDuration = 2.0f; // Hold for 2 seconds
InteractionPriority = 5;    // Higher priority than default
```

Your interaction system will automatically:
- Show progress bar during the 2 seconds
- Call `EndInteract` when complete
- Call `CancelInteract` if player moves away

### Example 3: Custom Visual Feedback

Override in Blueprint:

```blueprint
Event OnInteractionFocusGained (Interacting Actor)
??? Parent: OnInteractionFocusGained
??? Play Sound (Hover Sound)
??? Spawn Particle Effect (Glow VFX)
??? Show UI Widget (Pickup Prompt)

Event OnInteractionFocusLost (Interacting Actor)
??? Parent: OnInteractionFocusLost
??? Stop Sound
??? Destroy Particle Effect
??? Hide UI Widget
```

---

## Customization Options

### Change Outline Color Based on Rarity

```cpp
void AInteractableWorldItem::BeginPlay()
{
    Super::BeginPlay();
    
    if (ItemData)
    {
// Example: Color based on item rarity
        if (ItemData->ItemTags.HasTag(FGameplayTag::RequestGameplayTag("Item.Rarity.Legendary")))
    {
      OutlineColor = FLinearColor(1.0f, 0.5f, 0.0f); // Orange
  CustomDepthStencilValue = 255;
        }
    else if (ItemData->ItemTags.HasTag(FGameplayTag::RequestGameplayTag("Item.Rarity.Rare")))
        {
      OutlineColor = FLinearColor(0.5f, 0.0f, 1.0f); // Purple
     CustomDepthStencilValue = 254;
        }
        // etc.
    }
}
```

### Add Distance Check

```cpp
bool AInteractableWorldItem::CanInteract_Implementation(AActor* InteractingActor) const
{
 // Parent check
    if (!Super::CanInteract(InteractingActor))
    {
        return false;
    }
    
    // Additional distance validation
    const float Distance = FVector::Distance(GetActorLocation(), InteractingActor->GetActorLocation());
    return Distance <= InteractionRadius;
}
```

### Custom Interaction Text

```cpp
FInteractionData AInteractableWorldItem::GetInteractionData_Implementation(AActor* InteractingActor) const
{
    FInteractionData Data = Super::GetInteractionData(InteractingActor);
    
    // Custom action text based on context
    if (ItemData)
    {
        FString KeyName = "E"; // Get from input mapping
 Data.ActionText = FText::Format(
 FText::FromString("Press {0} to pick up {1}"),
   FText::FromString(KeyName),
            ItemData->ItemName
        );
    }
    
    return Data;
}
```

---

## Integration Checklist

- [ ] Copy InteractableWorldItem.h to YourProject/Public/Interaction/
- [ ] Copy InteractableWorldItem.cpp to YourProject/Private/Interaction/
- [ ] Add "AdvancedInventorySystem" to YourProject.Build.cs dependencies
- [ ] Create Blueprint child class (BP_InteractableWorldItem)
- [ ] Test with existing interaction system
- [ ] Configure outline materials/post-process for custom depth
- [ ] Create item-specific Blueprints (health potion, key, etc.)

---

## Material Setup for Outline

To see the outline effect, you need a post-process material:

1. Create Material (M_Outline_PostProcess)
2. Set Material Domain = Post Process
3. Use Custom Depth node
4. Compare against your stencil value (252)
5. Output colored outline

Or use Unreal's built-in outline system:
- Enable Custom Depth/Stencil in Project Settings
- Enable Post Process Volume in your level
- Configure outline rendering

---

## Why This Design?

? **No Plugin Modification** - Keep plugin clean and updateable
? **Easy to Remove** - Delete these 2 files if you change interaction system
? **Flexible** - Override in Blueprint or C++ as needed
? **Testable** - Can use WorldItemActor directly without interaction system
? **Maintainable** - Clear separation of concerns

The plugin provides the **foundation** (WorldItemActor).
Your project provides the **integration** (AInteractableWorldItem).
Perfect separation! ??

---

## Alternative: Pure Blueprint Approach

Don't want C++ in your project? No problem!

1. Create Blueprint (BP_InteractableWorldItem)
2. Parent Class = WorldItemActor
3. Add Interface = InteractableInterface
4. Implement functions in Blueprint (see INTERACTION_SYSTEM_INTEGRATION.md)

Same result, zero C++ code in your project!

---

## Need More Examples?

Check the comprehensive guide:
`INTERACTION_SYSTEM_INTEGRATION.md`

Or the plugin source:
`WorldItemActor.h` and `WorldItemActor.cpp`

Both show all available functions and events you can use!
