# Interaction System Documentation

## Overview
���������������� ������� �������������� � ���������� ����� ��� Unreal Engine 5, ������������� � �������������� best practices � ������� � ����������.

## �����������

### Core Components

#### 1. **InteractionTypes.h**
�������� ������� ���� � ��������� ������:
- `EInteractionType` - Enum ����� �������������� (Pickup, Talk, Open, Close, etc.)
- `FInteractionData` - ��������� � ������� � ��������������
- `EInteractionResult` - ��������� ������� ��������������

#### 2. **IInteractableInterface**
��������� ��� ��������, � �������� ����� �����������������:
- `BeginInteract()` - ������ ��������������
- `EndInteract()` - ���������� ��������������
- `CancelInteract()` - ������ ��������������
- `GetInteractionData()` - ��������� ������
- `CanInteract()` - �������� ����������� ��������������
- `OnInteractionFocused()` - ����� �� �������
- `OnInteractionUnfocused()` - ������ ������

#### 3. **UInteractionComponent**
��������� ��� �������, ������� ����� ����������������� (�����, AI):
- �������������� ����������� ������������� ��������
- ������� ����������� (line/sphere trace)
- ���������� ������� � ���������������
- ��������� ���������� � ��������������� ��������������
- �������� ��� �������

**��������:**
- `OnInteractionStarted` - ������ ��������������
- `OnInteractionCompleted` - �������� ����������
- `OnInteractionCancelled` - ������
- `OnInteractableFocused` - ������ � ������
- `OnInteractableUnfocused` - ������ ������� �����

#### 4. **UInteractableComponent**
��������� ��� ��������, � �������� ����� �����������������:
- ��������� IInteractableInterface
- ������� Blueprint �������
- �������������� ��������� (Custom Depth Stencil)
- ����������� ���������� ��������������
- Enable/Disable ����������

## ������� �������������

### 1. Setup ��� ������

#### C++ ������:
```cpp
// � ������������ ������ Character ������
AYourCharacter::AYourCharacter()
{
    InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    InteractionComp->TraceDistance = 500.0f;
    InteractionComp->bDrawDebugTrace = true; // ��� �������
}

// ��������� � Input
void AYourCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Enhanced Input ��� ������ �������
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AYourCharacter::OnInteractPressed);
}

void AYourCharacter::OnInteractPressed()
{
    if (InteractionComp)
    {
        EInteractionResult Result = InteractionComp->TryInteract();
        // ���������� ���������
    }
}
```

#### Blueprint ������:
1. �������� Blueprint ������ ���������
2. �������� ��������� "Interaction Component"
3. ��������� ��������� (Trace Distance, Trace Radius, etc.)
4. � Event Graph ��������� Input Action � "Try Interact"

### 2. �������� �������������� ������� ����� InteractableComponent

#### Blueprint:
1. �������� ����� Actor Blueprint
2. �������� Static Mesh Component
3. �������� "Interactable Component"
4. ��������� "Interaction Data":
   - Interaction Name: "Treasure Chest"
   - Interaction Type: "Open"
   - Action Text: "Press E to Open"
   - Max Interaction Distance: 200
5. ���������� �������:
   - "On Begin Interact" - ������ ��������
   - "On End Interact" - ����������
   - "On Focused/Unfocused" - ���������� �������

#### C++:
```cpp
// ������� ������������� ������
UCLASS()
class AMyInteractable : public AActor
{
    GENERATED_BODY()
    
public:
    AMyInteractable()
    {
        MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
        RootComponent = MeshComp;
        
        InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
        InteractableComp->InteractionData.InteractionName = FText::FromString("Object");
        InteractableComp->InteractionData.InteractionType = EInteractionType::Use;
    }
    
protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;
    
    UPROPERTY(VisibleAnywhere)
    UInteractableComponent* InteractableComp;
};
```

### 3. �������� ���������� �������������� ������ ����� ���������

```cpp
UCLASS()
class AMyCustomInteractable : public AActor, public IInteractableInterface
{
    GENERATED_BODY()
    
public:
    virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override
    {
        // ���� ������
        UE_LOG(LogTemp, Warning, TEXT("Interacted by %s"), *InteractingActor->GetName());
        return EInteractionResult::Success;
    }
    
    virtual FInteractionData GetInteractionData_Implementation(AActor* InteractingActor) const override
    {
        FInteractionData Data;
        Data.InteractionName = FText::FromString("Custom Object");
        Data.InteractionType = EInteractionType::Custom;
        Data.MaxInteractionDistance = 300.0f;
        return Data;
    }
    
    virtual bool CanInteract_Implementation(AActor* InteractingActor) const override
    {
        // ��������� ���������
        return bIsEnabled && !bIsUsed;
    }
    
private:
    bool bIsEnabled = true;
    bool bIsUsed = false;
};
```

## ������� ������� (Examples)

### 1. **APickupableActor** - ����������� ��������
```cpp
// ������������� � Blueprint:
// 1. �������� Blueprint �� ������ PickupableActor
// 2. ��������� Item Name, Pickup Distance
// 3. ���������� ������� "On Picked Up"
```

**������ �������������:**
- �������� BP_HealthPotion : PickupableActor
- � "On Picked Up": �������� �������� ������
- ���������� "Destroy On Pickup" = true

### 2. **ADoorActor** - �����
```cpp
// ������������� � Blueprint:
// 1. �������� Blueprint �� ������ DoorActor
// 2. ��������� Door Open Angle, Duration
// 3. ���������� �������� � ��������:
//    - "On Door Opening" - ������ ��������
//    - "On Door Opened" - ����� �������
//    - "On Door Closing" - ������ ��������
//    - "On Door Closed" - ����� �������
```

**������ Timeline ��������:**
```
On Door Opening:
  -> Create Timeline (0 to DoorOpenAngle over DoorOpenDuration)
  -> SetRelativeRotation(Door Component)
  -> On Timeline Finished -> Call "On Door Opened"
```

### 3. **ANPCActor** - NPC ��� ��������
```cpp
// ������������� � Blueprint:
// 1. �������� Blueprint �� ������ NPCActor
// 2. ��������� NPC Name, Greeting Text
// 3. ���������� ������� "On Dialogue Started":
//    - �������� UI �������
//    - ���������� �������� NPC
// 4. ���������� "On Dialogue Ended":
//    - �������� UI
//    - ����������� AI
```

## ��������� ��������� �������� (Post Process Outline)

### 1. � Project Settings:
- Engine > Rendering > Custom Depth-Stencil Pass: "Enabled with Stencil"

### 2. �������� Post Process Material:
```
Material Domain: Post Process
Blend Mode: Blend

Nodes:
SceneTexture:CustomDepth -> Compare (> 0)
  -> If True: Desired Outline Color
  -> If False: SceneColor
```

### 3. �������� Post Process Volume:
- Infinite Extent: true
- Post Process Materials: �������� ��������� ��������

### 4. ��������� ������:
- 250 - ������� ������������� ������� (�������)
- 251 - ��������/��������������� (�������)
- 252 - NPC (�����)

## ���������� �������

### ���������� ������ ���� ��������������:

#### 1. �������� � InteractionTypes.h:
```cpp
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    // ...existing types...
    Repair    UMETA(DisplayName = "Repair"),  // ����� ���
};
```

#### 2. �������� ����� �����:
```cpp
UCLASS()
class ARepairableActor : public AActor, public IInteractableInterface
{
    GENERATED_BODY()
    
public:
    virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override
    {
        if (bIsDestroyed)
        {
            // Repair logic
            RepairProgress += 0.1f;
            if (RepairProgress >= 1.0f)
            {
                bIsDestroyed = false;
                OnRepaired();
            }
            return EInteractionResult::Success;
        }
        return EInteractionResult::RequirementsNotMet;
    }
    
    // ... ��������� ���
};
```

### ���������� ���������� ��� ��������������:

```cpp
// �������� � FInteractionData:
UPROPERTY(EditAnywhere, BlueprintReadWrite)
TArray<FName> RequiredItemTags;

// � BeginInteract ����������:
virtual bool CanInteract_Implementation(AActor* InteractingActor) const override
{
    if (RequiredItemTags.Num() > 0)
    {
        // ��������� ��������� InteractingActor
        IInventoryInterface* Inventory = Cast<IInventoryInterface>(InteractingActor);
        if (!Inventory || !Inventory->HasItemsWithTags(RequiredItemTags))
        {
            return false;
        }
    }
    return true;
}
```

## Best Practices

1. **����������� ��������� ��� ��������:**
   - ����� ����� ����� ����� �������������, ������ ���������� ���������
   - �� ��������� � ���������� �������� �������

2. **����������� ��������� ��� �������� ����������������:**
   - UInteractableComponent ������� ��� Blueprint-only ��������
   - ������� ��������� ��� C++

3. **�������� ��� UI � ��������:**
   - �������������� �� �������� InteractionComponent � PlayerController
   - ����������� UI ��������� ��� OnInteractableFocused
   - ���������� �������� ���� ��� ���������� ���������������

4. **���������� ��������������:**
   - ����������� InteractionPriority ���� ��������� �������� � ������
   - NPC > Quest Items > Regular Items

5. **�����������:**
   - ��������� TraceTickInterval (�� ��������� 0.1s)
   - ����������� TraceRadius ��� ����� �������� ��������������
   - ���������� bDrawDebugTrace � ������

## Debugging

### �������� ���������� ������������:
```cpp
InteractionComponent->bDrawDebugTrace = true;
```

### �����������:
```cpp
// � InteractableComponent �������� ����
UE_LOG(LogTemp, Warning, TEXT("Interaction started by %s"), *InteractingActor->GetName());
```

### ������ ��������:

1. **������ �� ��������������:**
   - ��������� Collision Channel
   - ��������� ��� Trace Distance ����������
   - ��������� ��� ������ ��������� ���������

2. **��������� �� ��������:**
   - �������� Custom Depth � Project Settings
   - ��������� Post Process Material
   - ��������� ��� Mesh ��������� ������������ Custom Depth

3. **�������������� �� �����������:**
   - ��������� CanInteract ���������� true
   - ��������� ��� InteractionComponent->bInteractionEnabled = true
   - ��������� ���������� � Line of Sight

## ���������� � ������� ���������

### ���������:
```cpp
// � PickupableActor::PerformPickup
if (IInventoryInterface* Inventory = Cast<IInventoryInterface>(PickedUpBy))
{
    Inventory->AddItem(this->ItemData);
}
```

### Quest �������:
```cpp
// � BeginInteract
if (IQuestInterface* QuestManager = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
{
    QuestManager->OnObjectInteracted(this, InteractingActor);
}
```

### Save �������:
```cpp
// ���������� ��������� ������������� ��������
UPROPERTY(SaveGame)
bool bHasBeenInteractedWith;
```

## ����������

��� ������� ������������:
- ? �������� ����� ����������
- ? �������� ����� ����������
- ? ������������� ����� ������������
- ? Production-ready code
- ? Blueprint � C++ ���������
- ? ������� ������� ��� ������

������� � ������� �������� (PickupableActor, DoorActor, NPCActor) � ���������� ��� ���� �����!
