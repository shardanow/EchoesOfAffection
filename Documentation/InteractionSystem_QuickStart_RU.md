# ������� �������������� - ������� �����

## ?? ��� ������ (C++)

### 1. �������� ��������� � ������������ Character:
```cpp
InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));
InteractionComp->TraceDistance = 500.0f;
InteractionComp->TraceRadius = 10.0f;
InteractionComp->bDrawDebugTrace = true; // ��� �������
```

### 2. ��������� � Input:
```cpp
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyCharacter::Interact);
}

void AMyCharacter::Interact()
{
    if (InteractionComp)
    {
        EInteractionResult Result = InteractionComp->TryInteract();
        
        switch(Result)
        {
            case EInteractionResult::Success:
                // �������������� ��������
                break;
            case EInteractionResult::TooFar:
                // ������� ������
                break;
            case EInteractionResult::AlreadyInteracting:
                // ��� ����������������
                break;
        }
    }
}
```

## ?? ��� ������ (Blueprint)

1. �������� BP ���������
2. Add Component -> "Interaction Component"
3. ��������� ��������� � Details
4. Event Graph: Input Action "Interact" -> "Try Interact"
5. ���������� � UI ��� ����������� ���������:
   - "On Interactable Focused" -> �������� UI
   - "On Interactable Unfocused" -> ������ UI

## ?? �������� �������������� ������� (Blueprint)

### ������� 1: ����� ��������� (����� �������)
1. �������� ����� Actor Blueprint
2. �������� Static Mesh
3. Add Component -> "Interactable Component"
4. � Details ��������� Interaction Data:
   ```
   Interaction Name: "������"
   Interaction Type: "Open"
   Action Text: "������� E ����� �������"
   Interaction Duration: 2.0 (�������, 0 = ���������)
   Max Interaction Distance: 200
   ```
5. � Event Graph ����������:
   ```
   Event "On Begin Interact" -> ���� ������ (�������, ���� �������)
   Event "On Focused" -> ������� ��������� (�����������)
   ```

**����������**: ������� ������������� ��������� `InteractableComponent` �� ������, ���� ���� ����� �� ��������� `IInteractableInterface` ��������. ��������� ����� ��������� ��������� ��� ��������������.

### ������� 2: ����������� ������� �������

#### ����������� �������:
1. Create Blueprint -> Parent: PickupableActor
2. ���������:
   - Item Name: "����� ��������"
   - Pickup Distance: 150
   - Destroy On Pickup: true
3. Event "On Picked Up":
   ```
   Cast to MyCharacter -> Add Health -> Destroy
   ```

#### �����:
1. Create Blueprint -> Parent: DoorActor
2. ���������:
   - Door Open Angle: 90
   - Door Open Duration: 1.0
   - Auto Close: true
   - Auto Close Delay: 3.0
3. Event "On Door Opening":
   ```
   Timeline (0 to 90 degrees over 1 second)
   -> Set Relative Rotation (Door Component)
   ```

#### NPC:
1. Create Blueprint -> Parent: NPCActor
2. ���������:
   - NPC Name: "��������"
   - Greeting Text: "������������!"
   - Talk Distance: 250
3. Event "On Dialogue Started":
   ```
   -> Open Dialogue Widget
   -> Pause AI
   ```

## ?? �������� ���������� �������������� ������� (C++)

```cpp
UCLASS()
class AMySwitch : public AActor, public IInteractableInterface
{
    GENERATED_BODY()
    
public:
    // ��� ���������� ��� ��������������
    virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override
    {
        bIsActive = !bIsActive;
        OnSwitchToggled(bIsActive);
        return EInteractionResult::Success;
    }
    
    // ���������� � ��������������
    virtual FInteractionData GetInteractionData_Implementation(AActor* InteractingActor) const override
    {
        FInteractionData Data;
        Data.InteractionName = FText::FromString("���������");
        Data.InteractionType = EInteractionType::Toggle;
        Data.ActionText = FText::FromString(bIsActive ? "E - ���������" : "E - ��������");
        Data.MaxInteractionDistance = 200.0f;
        return Data;
    }
    
    // ����� �� �����������������
    virtual bool CanInteract_Implementation(AActor* InteractingActor) const override
    {
        return bIsEnabled;
    }
    
    // ���������
    virtual void OnInteractionFocused_Implementation(AActor* InteractingActor) override
    {
        MeshComp->SetRenderCustomDepth(true);
    }
    
    virtual void OnInteractionUnfocused_Implementation(AActor* InteractingActor) override
    {
        MeshComp->SetRenderCustomDepth(false);
    }
    
protected:
    UFUNCTION(BlueprintImplementableEvent)
    void OnSwitchToggled(bool bActive);
    
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComp;
    
    bool bIsActive = false;
    bool bIsEnabled = true;
};
```

## ?? ��������� ��������� ��������

### 1. Project Settings:
```
Engine > Rendering > Custom Depth-Stencil Pass = "Enabled with Stencil"
```

### 2. Post Process Material (M_OutlineHighlight):
```
Material Domain: Post Process

SceneTexture:CustomDepth 
-> Compare (Greater, 0) 
-> Branch
   True: Your Color (Green for interactables)
   False: SceneColor
-> EmissiveColor
```

### 3. � ���:
- �������� Post Process Volume
- Infinite Extent = true
- Post Process Materials -> �������� M_OutlineHighlight

### ����� �� ��������� (Stencil Value):
- 250 = ������� ������� (�������)
- 251 = ��������������� (�������)  
- 252 = NPC (�����)

## ?? UI ��� ��������������

### Widget Blueprint (WBP_InteractionPrompt):
```
Canvas Panel
  -> Text Block (InteractionName)
  -> Text Block (ActionText)
  -> Progress Bar (��� ���������� ��������������)
```

### � PlayerController ��� HUD:
```cpp
// �������� �� �������
InteractionComp->OnInteractableFocused.AddDynamic(this, &AMyPC::ShowInteractionUI);
InteractionComp->OnInteractableUnfocused.AddDynamic(this, &AMyPC::HideInteractionUI);

void AMyPC::ShowInteractionUI(AActor* Interactable, const FInteractionData& Data)
{
    // �������/�������� Widget
    InteractionWidget->SetInteractionText(Data.ActionText);
    InteractionWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMyPC::HideInteractionUI(AActor* Interactable)
{
    InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
}
```

### ��� ���������� ��������������:
```cpp
// Tick update
void AMyHUD::UpdateInteractionProgress()
{
    if (InteractionComp->IsInteracting())
    {
        float Progress = InteractionComp->GetInteractionProgress();
        ProgressBar->SetPercent(Progress);
    }
}
```

## ?? ���������� �������

### �������� ���������� �������� ��� ��������������:

```cpp
// � FInteractionData ��������:
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName RequiredItemTag;

// � CanInteract ����������:
virtual bool CanInteract_Implementation(AActor* InteractingActor) const override
{
    if (!RequiredItemTag.IsNone())
    {
        IInventoryInterface* Inventory = Cast<IInventoryInterface>(InteractingActor);
        if (!Inventory || !Inventory->HasItem(RequiredItemTag))
        {
            return false;
        }
    }
    return true;
}
```

### �������� �����/�������:

```cpp
UCLASS()
class UInteractableComponent : public UActorComponent
{
    // ��������:
    UPROPERTY(EditAnywhere, Category = "Interaction|Effects")
    USoundBase* InteractSound;
    
    UPROPERTY(EditAnywhere, Category = "Interaction|Effects")
    UParticleSystem* InteractParticle;
    
    virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override
    {
        if (InteractSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, InteractSound, GetOwner()->GetActorLocation());
        }
        
        if (InteractParticle)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InteractParticle, GetOwner()->GetActorLocation());
        }
        
        return Super::BeginInteract_Implementation(InteractingActor);
    }
};
```

## ?? ��������� InteractionComponent

```cpp
// ���������� �����������
TraceDistance = 500.0f;

// ������ ����������� (0 = line trace, >0 = sphere trace)
TraceRadius = 10.0f;

// ��� ����� ��������� ������� (�������)
TraceTickInterval = 0.1f;

// ����� ��������
InteractionTraceChannel = ECC_Visibility;

// ������� (�������/������� �����)
bDrawDebugTrace = true;

// ��������/���������
bInteractionEnabled = true;
```

## ?? �������

### ��������: ������ �� ��������������
- ? �������� `bDrawDebugTrace = true`
- ? ��������� Trace Distance
- ? ��������� ��� ������ ��������� IInteractableInterface
- ? ��������� Collision Channel � Mesh

### ��������: ��������� �� ��������
- ? Project Settings -> Custom Depth �������?
- ? Post Process Volume � ����?
- ? Post Process Material ��������?

### ��������: �������������� �� �����������
- ? `CanInteract` ���������� true?
- ? ��������� Max Interaction Distance
- ? Line of Sight �� �����������?

## ?? ���� �������������� (EInteractionType)

- **Pickup** - ��������� �������
- **Examine** - ���������
- **Talk** - ���������� (NPC)
- **Open** - ������� (�����, ������)
- **Close** - �������
- **Toggle** - ����������� (����, ���������)
- **Use** - ������������ (�����)
- **Activate** - ������������
- **Custom** - ��������� ���

## ? Checklist ��� ������ �������������� �������

1. ? ����� ��������� IInteractableInterface ��� ����� InteractableComponent
2. ? BeginInteract ���������� � �������
3. ? GetInteractionData ���������� ���������� ������
4. ? CanInteract ��������� �������
5. ? OnInteractionFocused/Unfocused ��� ���������� ��������
6. ? Mesh ����� ��������
7. ? Custom Depth ������� ��� ���������

---

**������! ������� � �������� (PickupableActor, DoorActor, NPCActor) � ���������� ��� ���� �����!**
