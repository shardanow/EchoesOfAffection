# Camera Control & Input Blocking - Architecture & Best Practices

## Архитектурное решение

### Принципы дизайна

1. **Separation of Concerns**
   - `DialogueCameraComponent` - только камера
   - `DialogueInputBlockerComponent` - только ввод
   - `DialoguePresentationComponent` - координация обоих

2. **Composition over Inheritance**
   - Компоненты можно использовать независимо
   - Гибкая комбинация функциональности
   - Легко расширяется и тестируется

3. **Non-Intrusive Design**
   - Не модифицирует существующий код игрока
   - Сохраняет оригинальное состояние
   - Легко добавить/удалить

4. **Best Practices UE5**
   - Использует ViewTarget систему
   - Поддерживает Enhanced Input
   - Интеграция со Spring Arm
   - Respect PlayerCameraManager

---

## Компонентная архитектура

```
???????????????????????????????????????????????
?     DialoguePresentationComponent           ?
?  (High-level координация)                  ?
?  - Управляет режимами представления         ?
?  - Синхронизирует камеру и ввод            ?
?  - Обрабатывает game state (pause, dilation)?
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

## Паттерны и практики

### 1. State Management

**Проблема**: Нужно восстанавливать состояние после диалога

**Решение**: Store & Restore Pattern
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

**Best Practice**: Всегда восстанавливайте состояние, даже при ошибках

---

### 2. Camera ViewTarget System

**Проблема**: Нужна гладкая смена камеры без конфликтов

**Решение**: Использование стандартной ViewTarget системы UE
```cpp
// Правильно ?
PC->SetViewTargetWithBlend(DialogueCameraActor, BlendTime, BlendFunc);

// Неправильно ? (создаёт конфликты)
PC->SetViewTarget(DialogueCameraActor);  // Без blend
// или
ManualCameraMovement();  // Перемещение камеры вручную
```

**Best Practice**: 
- Используйте `SetViewTargetWithBlend()` для плавности
- Сохраняйте `OriginalViewTarget` для восстановления
- Не манипулируйте камерой PlayerController напрямую

---

### 3. Enhanced Input Integration

**Проблема**: UE5 использует Enhanced Input, но legacy input тоже существует

**Решение**: Dual-Path Support
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
- Всегда поддерживайте оба пути
- Enhanced Input - приоритет
- Legacy - fallback
- Тестируйте с обеими системами

---

### 4. Component Lifecycle Management

**Проблема**: Утечки памяти, компоненты не очищаются

**Решение**: RAII-подобный подход
```cpp
// BeginPlay - инициализация
void BeginPlay() override
{
    Super::BeginPlay();
    CacheReferences();
}

// EndPlay - ВСЕГДА очищайте
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
- Очищайте в `EndPlay()`, не полагайтесь на деструктор
- Проверяйте `IsValid()` перед использованием WeakPtr
- Не оставляйте активные таймеры

---

### 5. Spring Arm for Collision

**Проблема**: Камера проходит сквозь стены

**Решение**: Spring Arm Component
```cpp
SpringArmComponent = NewObject<USpringArmComponent>(...);
SpringArmComponent->TargetArmLength = Settings.Distance;
SpringArmComponent->bDoCollisionTest = Settings.bEnableCollision;

// Attach camera to spring arm
CameraComponent->AttachToComponent(SpringArmComponent, ...);
```

**Best Practice**:
- Всегда используйте Spring Arm для движущихся камер
- Настраивайте collision channels правильно
- Тестируйте в тесных пространствах

---

### 6. Framing Presets

**Проблема**: Художники хотят изменять параметры камеры

**Решение**: Preset System + Data-Driven Config
```cpp
// Presets в коде
void InitializeFramingPresets()
{
    FDialogueCameraSettings CloseUp;
    CloseUp.Distance = 120.0f;
    CloseUp.FOVOverride = 60.0f;
    FramingPresets.Add(EDialogueCameraFraming::CloseUp, CloseUp);
}

// Легко переопределить в Blueprint
SetFramingSettings(EDialogueCameraFraming::CloseUp, MyCustomSettings);
```

**Best Practice**:
- Предоставьте разумные defaults
- Позвольте переопределять в Blueprint/DataAsset
- Документируйте что делает каждый preset

---

### 7. Focus Point System

**Проблема**: Камера смотрит в центр персонажа, а не на голову

**Решение**: Socket-Based Focus
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
- Используйте socket "head" для персонажей
- Предоставьте fallback offset
- Позвольте настраивать в Blueprint

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

### ? Pitfall 1: Не восстанавливается ввод
```cpp
// ПЛОХО
void EndDialogue()
{
    // Забыли разблокировать ввод!
    CameraComponent->DeactivateDialogueCamera();
}

// ХОРОШО
void EndDialogue()
{
    InputBlockerComponent->UnblockInput();  // ?
    CameraComponent->DeactivateDialogueCamera();
}
```

### ? Pitfall 2: Утечка ViewTarget
```cpp
// ПЛОХО - не восстанавливаем оригинал
PC->SetViewTarget(DialogueCameraActor);

// ХОРОШО - сохраняем и восстанавливаем
OriginalViewTarget = PC->GetViewTarget();
PC->SetViewTarget(DialogueCameraActor);
// ... later ...
PC->SetViewTarget(OriginalViewTarget.Get());
```

### ? Pitfall 3: Резкие переходы камеры
```cpp
// ПЛОХО - мгновенное переключение
PC->SetViewTarget(DialogueCameraActor);

// ХОРОШО - плавный переход
PC->SetViewTargetWithBlend(DialogueCameraActor, 0.5f, VTBlend_EaseInOut);
```

---

## Code Review Checklist

При review кода проверяйте:

- [ ] Все компоненты правильно очищаются в `EndPlay()`
- [ ] Используется `WeakObjectPtr` для ссылок на Actors
- [ ] Ввод блокируется И разблокируется
- [ ] Камера восстанавливается к оригинальному ViewTarget
- [ ] Tick отключается когда не нужен
- [ ] Есть fallback для Enhanced/Legacy Input
- [ ] Spring Arm используется для collision detection
- [ ] Настройки доступны в Blueprint
- [ ] Есть разумные defaults
- [ ] Код защищён от nullptr
- [ ] Multiplayer-safe (для локального контроллера)
- [ ] Работает с разными размерами персонажей

---

## Future Improvements

### Возможные расширения:

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

## Заключение

Эта система спроектирована следуя:
- **SOLID принципам**
- **Unreal Engine best practices**
- **AAA game industry standards**
- **Performance-first подходу**

Архитектура модульная, расширяемая и production-ready.

---

**Ссылки:**
- [UE5 Camera System Documentation](https://docs.unrealengine.com/5.0/en-US/camera-system-in-unreal-engine/)
- [Enhanced Input Documentation](https://docs.unrealengine.com/5.0/en-US/enhanced-input-in-unreal-engine/)
- [Best Practices Guide](https://docs.unrealengine.com/5.0/en-US/unreal-engine-programming-and-scripting-best-practices/)
