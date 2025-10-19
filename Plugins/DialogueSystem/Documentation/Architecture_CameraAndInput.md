# Camera Control & Input Blocking - Architecture & Best Practices

## ������������� �������

### �������� �������

1. **Separation of Concerns**
   - `DialogueCameraComponent` - ������ ������
   - `DialogueInputBlockerComponent` - ������ ����
   - `DialoguePresentationComponent` - ����������� �����

2. **Composition over Inheritance**
   - ���������� ����� ������������ ����������
   - ������ ���������� ����������������
   - ����� ����������� � �����������

3. **Non-Intrusive Design**
   - �� ������������ ������������ ��� ������
   - ��������� ������������ ���������
   - ����� ��������/�������

4. **Best Practices UE5**
   - ���������� ViewTarget �������
   - ������������ Enhanced Input
   - ���������� �� Spring Arm
   - Respect PlayerCameraManager

---

## ������������ �����������

```
???????????????????????????????????????????????
?     DialoguePresentationComponent           ?
?  (High-level �����������)                  ?
?  - ��������� �������� �������������         ?
?  - �������������� ������ � ����            ?
?  - ������������ game state (pause, dilation)?
???????????????????????????????????????????????
         ?                   ?
         ?                   ?
???????????????????  ????????????????????????
? DialogueCamera  ?  ? DialogueInputBlocker ?
? Component       ?  ? Component            ?
? - Camera control?  ? - Input blocking     ?
? - Framing       ?  ? - Pawn state        ?
? - Blending      ?  ? - Enhanced Input    ?
???????????????????  ????????????????????????
```

---

## �������� � ��������

### 1. State Management

**��������**: ����� ��������������� ��������� ����� �������

**�������**: Store & Restore Pattern
```cpp
// Store original state
void StorePawnState()
{
    bOriginalPawnVisibility = !Pawn->IsHidden();
    bOriginalRotationEnabled = Pawn->bUseControllerRotationYaw;
    OriginalMovementMode = MovementComp->MovementMode;
}

// Restore on deactivation
void RestorePawnState()
{
    Pawn->SetActorHiddenInGame(!bOriginalPawnVisibility);
    Pawn->bUseControllerRotationYaw = bOriginalRotationEnabled;
    MovementComp->SetMovementMode(OriginalMovementMode);
}
```

**Best Practice**: ������ ���������������� ���������, ���� ��� �������

---

### 2. Camera ViewTarget System

**��������**: ����� ������� ����� ������ ��� ����������

**�������**: ������������� ����������� ViewTarget ������� UE
```cpp
// ��������� ?
PC->SetViewTargetWithBlend(DialogueCameraActor, BlendTime, BlendFunc);

// ����������� ? (������ ���������)
PC->SetViewTarget(DialogueCameraActor);  // ��� blend
// ���
ManualCameraMovement();  // ����������� ������ �������
```

**Best Practice**: 
- ����������� `SetViewTargetWithBlend()` ��� ���������
- ���������� `OriginalViewTarget` ��� ��������������
- �� ������������� ������� PlayerController ��������

---

### 3. Enhanced Input Integration

**��������**: UE5 ���������� Enhanced Input, �� legacy input ���� ����������

**�������**: Dual-Path Support
```cpp
// Check availability
bool IsEnhancedInputAvailable() const
{
    return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()) != nullptr;
}

// Apply appropriate path
if (IsEnhancedInputAvailable())
{
    BlockEnhancedInput(Settings);
}
else
{
    BlockLegacyInput(Settings);
}
```

**Best Practice**:
- ������ ������������� ��� ����
- Enhanced Input - ���������
- Legacy - fallback
- ���������� � ������ ���������

---

### 4. Component Lifecycle Management

**��������**: ������ ������, ���������� �� ���������

**�������**: RAII-�������� ������
```cpp
// BeginPlay - �������������
void BeginPlay() override
{
    Super::BeginPlay();
    CacheReferences();
}

// EndPlay - ������ ��������
void EndPlay(const EEndPlayReason::Type EndPlayReason) override
{
    if (bCameraActive)
    {
        DeactivateDialogueCamera();  // Cleanup
    }
    Super::EndPlay(EndPlayReason);
}
```

**Best Practice**:
- �������� � `EndPlay()`, �� ����������� �� ����������
- ���������� `IsValid()` ����� �������������� WeakPtr
- �� ���������� �������� �������

---

### 5. Spring Arm for Collision

**��������**: ������ �������� ������ �����

**�������**: Spring Arm Component
```cpp
SpringArmComponent = NewObject<USpringArmComponent>(...);
SpringArmComponent->TargetArmLength = Settings.Distance;
SpringArmComponent->bDoCollisionTest = Settings.bEnableCollision;

// Attach camera to spring arm
CameraComponent->AttachToComponent(SpringArmComponent, ...);
```

**Best Practice**:
- ������ ����������� Spring Arm ��� ���������� �����
- ������������ collision channels ���������
- ���������� � ������ �������������

---

### 6. Framing Presets

**��������**: ��������� ����� �������� ��������� ������

**�������**: Preset System + Data-Driven Config
```cpp
// Presets � ����
void InitializeFramingPresets()
{
    FDialogueCameraSettings CloseUp;
    CloseUp.Distance = 120.0f;
    CloseUp.FOVOverride = 60.0f;
    FramingPresets.Add(EDialogueCameraFraming::CloseUp, CloseUp);
}

// ����� �������������� � Blueprint
SetFramingSettings(EDialogueCameraFraming::CloseUp, MyCustomSettings);
```

**Best Practice**:
- ������������ �������� defaults
- ��������� �������������� � Blueprint/DataAsset
- �������������� ��� ������ ������ preset

---

### 7. Focus Point System

**��������**: ������ ������� � ����� ���������, � �� �� ������

**�������**: Socket-Based Focus
```cpp
FVector GetFocusPoint(AActor* TargetActor) const
{
    if (FocusSocketName != NAME_None)
    {
        if (USkeletalMeshComponent* Mesh = Character->GetMesh())
        {
            if (Mesh->DoesSocketExist(FocusSocketName))
            {
                return Mesh->GetSocketLocation(FocusSocketName);
            }
        }
    }
    
    // Fallback
    return TargetActor->GetActorLocation() + FocusPointOffset;
}
```

**Best Practice**:
- ����������� socket "head" ��� ����������
- ������������ fallback offset
- ��������� ����������� � Blueprint

---

## Performance Optimization

### 1. Tick Optimization
```cpp
// ? BAD - Always ticking
PrimaryComponentTick.bCanEverTick = true;

// ? GOOD - Conditional ticking
PrimaryComponentTick.bCanEverTick = true;
PrimaryComponentTick.bStartWithTickEnabled = false;

void ActivateCamera()
{
    SetComponentTickEnabled(true);  // Enable when needed
}

void DeactivateCamera()
{
    SetComponentTickEnabled(false);  // Disable when done
}
```

### 2. Timer-Based Updates
```cpp
// ? BAD - Every frame update
void TickComponent(float DeltaTime, ...)
{
    UpdateCameraPosition();  // Too frequent
}

// ? GOOD - Timer-based updates
void StartTracking()
{
    GetWorld()->GetTimerManager().SetTimer(
        TrackingTimer,
        this,
        &UDialogueCameraComponent::UpdateCameraTracking,
        TrackingUpdateRate,  // e.g., 0.5 seconds
        true
    );
}
```

### 3. Reference Caching
```cpp
// ? GOOD
TWeakObjectPtr<APlayerController> CachedPlayerController;

void BeginPlay()
{
    CachedPlayerController = GetWorld()->GetFirstPlayerController();
}

void SomeFunction()
{
    if (APlayerController* PC = CachedPlayerController.Get())
    {
        // Use cached reference
    }
}
```

---

## Testing Considerations

### 1. Multiplayer Safety
```cpp
// Consider multiplayer scenarios
void ActivateCamera()
{
    // Only run on local client
    if (!IsLocallyControlled())
    {
        return;
    }
    
    // ... rest of activation
}
```

### 2. Edge Cases
- Level transitions during dialogue
- Player death during dialogue
- Multiple simultaneous dialogues
- VR/Split-screen compatibility
- Different character sizes (children, giants)

### 3. Platform Considerations
- Console controller support
- Mouse + Keyboard
- Touch controls (mobile)
- Different aspect ratios

---

## Common Pitfalls & Solutions

### ? Pitfall 1: �� ����������������� ����
```cpp
// �����
void EndDialogue()
{
    // ������ �������������� ����!
    CameraComponent->DeactivateDialogueCamera();
}

// ������
void EndDialogue()
{
    InputBlockerComponent->UnblockInput();  // ?
    CameraComponent->DeactivateDialogueCamera();
}
```

### ? Pitfall 2: ������ ViewTarget
```cpp
// ����� - �� ��������������� ��������
PC->SetViewTarget(DialogueCameraActor);

// ������ - ��������� � ���������������
OriginalViewTarget = PC->GetViewTarget();
PC->SetViewTarget(DialogueCameraActor);
// ... later ...
PC->SetViewTarget(OriginalViewTarget.Get());
```

### ? Pitfall 3: ������ �������� ������
```cpp
// ����� - ���������� ������������
PC->SetViewTarget(DialogueCameraActor);

// ������ - ������� �������
PC->SetViewTargetWithBlend(DialogueCameraActor, 0.5f, VTBlend_EaseInOut);
```

---

## Code Review Checklist

��� review ���� ����������:

- [ ] ��� ���������� ��������� ��������� � `EndPlay()`
- [ ] ������������ `WeakObjectPtr` ��� ������ �� Actors
- [ ] ���� ����������� � ��������������
- [ ] ������ ����������������� � ������������� ViewTarget
- [ ] Tick ����������� ����� �� �����
- [ ] ���� fallback ��� Enhanced/Legacy Input
- [ ] Spring Arm ������������ ��� collision detection
- [ ] ��������� �������� � Blueprint
- [ ] ���� �������� defaults
- [ ] ��� ������� �� nullptr
- [ ] Multiplayer-safe (��� ���������� �����������)
- [ ] �������� � ������� ��������� ����������

---

## Future Improvements

### ��������� ����������:

1. **Advanced Camera Behaviors**
   - Handheld camera shake
   - Look-at constraints
   - Multiple target tracking
   - Cinematic dolly/crane movements

2. **Enhanced Input Integration**
   - Per-dialogue input mapping contexts
   - Dynamic action rebinding
   - Input hints UI

3. **AI Director**
   - Automatic shot selection based on emotion
   - Rule-based camera cuts
   - Attention-based framing

4. **Performance**
   - Camera actor pooling
   - LOD system for distant dialogues
   - Async transition calculations

5. **Accessibility**
   - Customizable motion sickness options
   - Camera zoom controls
   - Alternative input methods

---

## ����������

��� ������� �������������� ������:
- **SOLID ���������**
- **Unreal Engine best practices**
- **AAA game industry standards**
- **Performance-first �������**

����������� ���������, ����������� � production-ready.

---

**������:**
- [UE5 Camera System Documentation](https://docs.unrealengine.com/5.0/en-US/camera-system-in-unreal-engine/)
- [Enhanced Input Documentation](https://docs.unrealengine.com/5.0/en-US/enhanced-input-in-unreal-engine/)
- [Best Practices Guide](https://docs.unrealengine.com/5.0/en-US/unreal-engine-programming-and-scripting-best-practices/)
