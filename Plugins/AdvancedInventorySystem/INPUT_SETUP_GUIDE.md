# ?? Input Setup ��� WorldItemActor Pickup

## ��������

� ��� ����:
- ? WorldItemActor � ����
- ? InteractionComponent �� Character
- ? �� ������� E ������ �� ������

**�������:** Input �� ������ � `TryInteract()`!

---

## ? ������� (Enhanced Input)

### ��� 1: �������� Input Action

1. **Content Browser ? Right Click ? Input ? Input Action**
   - Name: `IA_Interact`
   - Value Type: `Digital (bool)`

2. **Save**

---

### ��� 2: �������� � Input Mapping Context

1. **�������� ��� IMC** (Input Mapping Context)
   - ������: `IMC_Default` ��� `IMC_Player`

2. **Add Mapping:**
   ```
   Action: IA_Interact
   Key: E (Keyboard)
   Modifiers: None
   Triggers: Pressed
   ```

3. **Save**

---

### ��� 3: Bind � Character Blueprint

**Option A: � Event Graph**

```blueprint
Event BeginPlay
??? Get Player Controller
    ??? Get Enhanced Input Local Player Subsystem
        ??? Add Mapping Context (IMC_Default, Priority: 0)

Setup Player Input Component (Input Component)
??? Enhanced Input Action (IA_Interact)
    ??? Triggered:
    ?   ??? Get Component by Class (InteractionComponent)
    ?       ??? Try Interact
    ??? Completed:
        ??? (nothing - optional cancel logic)
```

**Option B: � C++ (���� ���� Character class)**

```cpp
// YourCharacter.h
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"

UCLASS()
class AYourCharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* InteractAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

private:
    void OnInteract();
};

// YourCharacter.cpp
#include "Interaction/InteractionComponent.h"

void AYourCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
 Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Cast to Enhanced Input Component
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
 // Bind Interact action
        EnhancedInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AYourCharacter::OnInteract);
    }
}

void AYourCharacter::OnInteract()
{
    // Find InteractionComponent
    if (UInteractionComponent* InteractionComp = FindComponentByClass<UInteractionComponent>())
    {
        InteractionComp->TryInteract();
    }
}
```

---

## ?? Debug Checklist

### 1. ��������� ��� Input ��������

**Add Print � Blueprint:**

```blueprint
Enhanced Input Action (IA_Interact) ? Triggered
??? Print String: "Interact pressed!"
```

**��� � C++:**

```cpp
void AYourCharacter::OnInteract()
{
    UE_LOG(LogTemp, Warning, TEXT("Interact button pressed!"));
    
    if (UInteractionComponent* InteractionComp = FindComponentByClass<UInteractionComponent>())
    {
        InteractionComp->TryInteract();
    }
}
```

**Test:** ������� E ? ������ ��������� print!

**���� print �� ����������:**
- Input Mapping Context �� ��������?
- Action �� ������?
- Input Mode ������������?

---

### 2. ��������� ��� InteractionComponent ��������

**Enable Debug Trace:**

```blueprint
Character ? InteractionComponent (Details panel)
??? Draw Debug Trace = True ?
```

**Test:** ���������� �� item
- ? ������ �����/����� = detected
- ? ������� ����� = not detected

---

### 3. ��������� InteractableComponent ������

**Play ? World Outliner ? Your WorldItemActor ? Details**

Components:
```
??? InteractableComponent_Dynamic ?
```

**���� ���:**
- Check Output Log for errors
- `bAutoCreateInteractionComponent = true`?
- InteractableComponent class exists in project?

---

### 4. ��������� ����

**Output Log ������ ����������:**

```
LogTemp: WorldItemActor: Auto-created InteractableComponent for BP_Item_C_0
LogTemp: WorldItemActor: Configured InteractionData for BP_Item_C_0
LogTemp: WorldItemActor: Bound OnBeginInteract delegate for BP_Item_C_0
```

**���� ��� ���� ����� = ��������� �� ��������!**

---

## ?? Alternative: Legacy Input

���� � ��� �� Enhanced Input:

### � Project Settings:

**Project Settings ? Input ? Action Mappings:**

```
+ Action Mapping
Name: Interact
Key: E (Keyboard)
```

### � Character Blueprint:

```blueprint
Input Action Interact
??? Get Component by Class (InteractionComponent)
    ??? Try Interact
```

### � C++:

```cpp
void AYourCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AYourCharacter::OnInteract);
}

void AYourCharacter::OnInteract()
{
    if (UInteractionComponent* InteractionComp = FindComponentByClass<UInteractionComponent>())
    {
      InteractionComp->TryInteract();
  }
}
```

---

## ?? Complete Setup Example (Blueprint Only)

### BP_PlayerCharacter:

```blueprint
=== Components ===
??? InteractionComponent
?   ??? Interaction Enabled = True
?   ??? Trace Distance = 500
?   ??? Draw Debug Trace = True (for debug)
?   ??? Interaction Trace Channel = Visibility
??? InventoryComponent
    ??? (your inventory settings)

=== Event Graph ===

Event BeginPlay
??? Get Player Controller
?   ??? Enhanced Input Local Player Subsystem
?   ??? Add Mapping Context
?           ??? Mapping Context = IMC_Default
?           ??? Priority = 0
??? Print String: "Player initialized"

Setup Player Input Component (Input Component)
??? Enhanced Input Action (IA_Interact)
    ??? Triggered:
    ?   ??? Print String: "Interact pressed!"
    ?   ??? InteractionComponent ? Try Interact
    ?       ??? Print String: "Result: {ReturnValue}"
    ??? Completed:
        ??? Print String: "Interact released"
```

---

## ?? Input Mapping Context (IMC_Default)

**Content/Input/IMC_Default:**

```
Mappings:
??? IA_Interact
?   ??? Key: E (Keyboard)
?   ??? Modifiers: None
?   ??? Triggers: Pressed
?
??? IA_Move (for movement)
?   ??? ...
?
??? IA_Look (for camera)
    ??? ...
```

---

## ?? Testing Flow

```
1. Start Play
?
2. Check Output Log:
   "Player initialized"
   "WorldItemActor: Auto-created InteractableComponent..."
   ?
3. Look at item
   ? Green debug trace appears
   ?
4. Press E
   ? Output Log: "Interact pressed!"
   ? Output Log: "Result: Success"
   ?
5. Item disappears
   ?
6. Check inventory
   ? Item added ?
```

---

## ?? Common Issues

### "E does nothing"

**Most likely:**
- Input not bound
- IMC not added to subsystem
- Wrong Input Action asset

**Fix:**
Add prints to verify each step works

---

### "Interact pressed but nothing happens"

**Most likely:**
- InteractionComponent not on Character
- InteractionComponent disabled
- Item not in range
- Item no InteractableComponent

**Fix:**
Enable debug trace, check components

---

### "Item detected but can't pickup"

**Most likely:**
- No InventoryComponent on player
- Inventory full
- ItemData not assigned
- bCanBePickedUp = false

**Fix:**
Add logs to `CanBePickedUpBy()`, check inventory

---

## ? Final Checklist

- [ ] IA_Interact Input Action created
- [ ] IMC has IA_Interact mapped to E key
- [ ] Character adds IMC in BeginPlay
- [ ] SetupPlayerInputComponent binds IA_Interact
- [ ] IA_Interact calls InteractionComponent?TryInteract()
- [ ] InteractionComponent on Character
- [ ] InteractionComponent enabled
- [ ] InventoryComponent on Character
- [ ] WorldItemActor has ItemData assigned
- [ ] bAutoCreateInteractionComponent = true
- [ ] Debug trace shows green when looking at item

**If all ? = Should work!** ??

---

## ?? Quick Test

**Minimal test ��� item:**

```blueprint
Input Action IA_Interact ? Triggered
??? Print String: "E pressed!"
```

**If this doesn't work = Input setup problem, NOT item problem!**

Fix input first, then test item pickup.

---

**Ready to test?** ??
