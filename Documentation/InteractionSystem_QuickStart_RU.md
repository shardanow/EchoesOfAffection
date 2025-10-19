# Система взаимодействия - Быстрый старт

## ?? Для игрока (C++)

### 1. Добавьте компонент в конструкторе Character:
```cpp
InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));
InteractionComp->TraceDistance = 500.0f;
InteractionComp->TraceRadius = 10.0f;
InteractionComp->bDrawDebugTrace = true; // Для отладки
```

### 2. Привяжите к Input:
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
                // Взаимодействие началось
                break;
            case EInteractionResult::TooFar:
                // Слишком далеко
                break;
            case EInteractionResult::AlreadyInteracting:
                // Уже взаимодействуете
                break;
        }
    }
}
```

## ?? Для игрока (Blueprint)

1. Откройте BP персонажа
2. Add Component -> "Interaction Component"
3. Настройте параметры в Details
4. Event Graph: Input Action "Interact" -> "Try Interact"
5. Подключите к UI для отображения подсказок:
   - "On Interactable Focused" -> Показать UI
   - "On Interactable Unfocused" -> Скрыть UI

## ?? Создание интерактивного объекта (Blueprint)

### Вариант 1: Через компонент (Самый простой)
1. Создайте новый Actor Blueprint
2. Добавьте Static Mesh
3. Add Component -> "Interactable Component"
4. В Details настройте Interaction Data:
   ```
   Interaction Name: "Сундук"
   Interaction Type: "Open"
   Action Text: "Нажмите E чтобы открыть"
   Interaction Duration: 2.0 (секунды, 0 = мгновенно)
   Max Interaction Distance: 200
   ```
5. В Event Graph реализуйте:
   ```
   Event "On Begin Interact" -> Ваша логика (открыть, дать предмет)
   Event "On Focused" -> Эффекты подсветки (опционально)
   ```

**Примечание**: Система автоматически обнаружит `InteractableComponent` на акторе, даже если актор не реализует `IInteractableInterface` напрямую. Компонент будет корректно обработан при взаимодействии.

### Вариант 2: Используйте готовые примеры

#### Подбираемый предмет:
1. Create Blueprint -> Parent: PickupableActor
2. Настройте:
   - Item Name: "Зелье здоровья"
   - Pickup Distance: 150
   - Destroy On Pickup: true
3. Event "On Picked Up":
   ```
   Cast to MyCharacter -> Add Health -> Destroy
   ```

#### Дверь:
1. Create Blueprint -> Parent: DoorActor
2. Настройте:
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
2. Настройте:
   - NPC Name: "Торговец"
   - Greeting Text: "Здравствуйте!"
   - Talk Distance: 250
3. Event "On Dialogue Started":
   ```
   -> Open Dialogue Widget
   -> Pause AI
   ```

## ?? Создание кастомного интерактивного объекта (C++)

```cpp
UCLASS()
class AMySwitch : public AActor, public IInteractableInterface
{
    GENERATED_BODY()
    
public:
    // Что происходит при взаимодействии
    virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override
    {
        bIsActive = !bIsActive;
        OnSwitchToggled(bIsActive);
        return EInteractionResult::Success;
    }
    
    // Информация о взаимодействии
    virtual FInteractionData GetInteractionData_Implementation(AActor* InteractingActor) const override
    {
        FInteractionData Data;
        Data.InteractionName = FText::FromString("Рубильник");
        Data.InteractionType = EInteractionType::Toggle;
        Data.ActionText = FText::FromString(bIsActive ? "E - Выключить" : "E - Включить");
        Data.MaxInteractionDistance = 200.0f;
        return Data;
    }
    
    // Можно ли взаимодействовать
    virtual bool CanInteract_Implementation(AActor* InteractingActor) const override
    {
        return bIsEnabled;
    }
    
    // Подсветка
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

## ?? Настройка подсветки объектов

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

### 3. В мир:
- Добавьте Post Process Volume
- Infinite Extent = true
- Post Process Materials -> добавьте M_OutlineHighlight

### Цвета по умолчанию (Stencil Value):
- 250 = Обычные объекты (зеленый)
- 251 = Заблокированные (красный)  
- 252 = NPC (синий)

## ?? UI для взаимодействий

### Widget Blueprint (WBP_InteractionPrompt):
```
Canvas Panel
  -> Text Block (InteractionName)
  -> Text Block (ActionText)
  -> Progress Bar (для длительных взаимодействий)
```

### В PlayerController или HUD:
```cpp
// Подписка на события
InteractionComp->OnInteractableFocused.AddDynamic(this, &AMyPC::ShowInteractionUI);
InteractionComp->OnInteractableUnfocused.AddDynamic(this, &AMyPC::HideInteractionUI);

void AMyPC::ShowInteractionUI(AActor* Interactable, const FInteractionData& Data)
{
    // Создать/показать Widget
    InteractionWidget->SetInteractionText(Data.ActionText);
    InteractionWidget->SetVisibility(ESlateVisibility::Visible);
}

void AMyPC::HideInteractionUI(AActor* Interactable)
{
    InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
}
```

### Для длительных взаимодействий:
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

## ?? Расширение системы

### Добавить требование предмета для взаимодействия:

```cpp
// В FInteractionData добавьте:
UPROPERTY(EditAnywhere, BlueprintReadWrite)
FName RequiredItemTag;

// В CanInteract проверяйте:
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

### Добавить звуки/эффекты:

```cpp
UCLASS()
class UInteractableComponent : public UActorComponent
{
    // Добавьте:
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

## ?? Настройки InteractionComponent

```cpp
// Расстояние трассировки
TraceDistance = 500.0f;

// Радиус трассировки (0 = line trace, >0 = sphere trace)
TraceRadius = 10.0f;

// Как часто проверять объекты (секунды)
TraceTickInterval = 0.1f;

// Канал коллизии
InteractionTraceChannel = ECC_Visibility;

// Отладка (красные/зеленые линии)
bDrawDebugTrace = true;

// Включить/выключить
bInteractionEnabled = true;
```

## ?? Отладка

### Проблема: Объект не обнаруживается
- ? Включите `bDrawDebugTrace = true`
- ? Проверьте Trace Distance
- ? Убедитесь что объект реализует IInteractableInterface
- ? Проверьте Collision Channel у Mesh

### Проблема: Подсветка не работает
- ? Project Settings -> Custom Depth включен?
- ? Post Process Volume в мире?
- ? Post Process Material настроен?

### Проблема: Взаимодействие не срабатывает
- ? `CanInteract` возвращает true?
- ? Проверьте Max Interaction Distance
- ? Line of Sight не блокируется?

## ?? Типы взаимодействий (EInteractionType)

- **Pickup** - Подобрать предмет
- **Examine** - Осмотреть
- **Talk** - Поговорить (NPC)
- **Open** - Открыть (дверь, сундук)
- **Close** - Закрыть
- **Toggle** - Переключить (свет, рубильник)
- **Use** - Использовать (общее)
- **Activate** - Активировать
- **Custom** - Кастомный тип

## ? Checklist для нового интерактивного объекта

1. ? Актер реализует IInteractableInterface ИЛИ имеет InteractableComponent
2. ? BeginInteract реализован с логикой
3. ? GetInteractionData возвращает корректные данные
4. ? CanInteract проверяет условия
5. ? OnInteractionFocused/Unfocused для визуальных эффектов
6. ? Mesh имеет коллизию
7. ? Custom Depth включен для подсветки

---

**Готово! Начните с примеров (PickupableActor, DoorActor, NPCActor) и расширяйте под свои нужды!**
