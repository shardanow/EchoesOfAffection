# Interaction System Documentation

## Overview
Профессиональная система взаимодействия с окружающим миром для Unreal Engine 5, разработанная с использованием best practices и готовая к расширению.

## Архитектура

### Core Components

#### 1. **InteractionTypes.h**
Содержит базовые типы и структуры данных:
- `EInteractionType` - Enum типов взаимодействий (Pickup, Talk, Open, Close, etc.)
- `FInteractionData` - Структура с данными о взаимодействии
- `EInteractionResult` - Результат попытки взаимодействия

#### 2. **IInteractableInterface**
Интерфейс для объектов, с которыми можно взаимодействовать:
- `BeginInteract()` - Начало взаимодействия
- `EndInteract()` - Завершение взаимодействия
- `CancelInteract()` - Отмена взаимодействия
- `GetInteractionData()` - Получение данных
- `CanInteract()` - Проверка возможности взаимодействия
- `OnInteractionFocused()` - Фокус на объекте
- `OnInteractionUnfocused()` - Потеря фокуса

#### 3. **UInteractionComponent**
Компонент для актеров, которые могут взаимодействовать (игрок, AI):
- Автоматическое обнаружение интерактивных объектов
- Система трассировки (line/sphere trace)
- Управление фокусом и взаимодействием
- Поддержка мгновенных и продолжительных взаимодействий
- Делегаты для событий

**Делегаты:**
- `OnInteractionStarted` - Начало взаимодействия
- `OnInteractionCompleted` - Успешное завершение
- `OnInteractionCancelled` - Отмена
- `OnInteractableFocused` - Объект в фокусе
- `OnInteractableUnfocused` - Объект потерял фокус

#### 4. **UInteractableComponent**
Компонент для объектов, с которыми можно взаимодействовать:
- Реализует IInteractableInterface
- Готовые Blueprint события
- Автоматическая подсветка (Custom Depth Stencil)
- Ограничение количества взаимодействий
- Enable/Disable функционал

## Примеры использования

### 1. Setup для игрока

#### C++ подход:
```cpp
// В конструкторе вашего Character класса
AYourCharacter::AYourCharacter()
{
    InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
    InteractionComp->TraceDistance = 500.0f;
    InteractionComp->bDrawDebugTrace = true; // Для отладки
}

// Привязать к Input
void AYourCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Enhanced Input или старая система
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AYourCharacter::OnInteractPressed);
}

void AYourCharacter::OnInteractPressed()
{
    if (InteractionComp)
    {
        EInteractionResult Result = InteractionComp->TryInteract();
        // Обработать результат
    }
}
```

#### Blueprint подход:
1. Откройте Blueprint вашего персонажа
2. Добавьте компонент "Interaction Component"
3. Настройте параметры (Trace Distance, Trace Radius, etc.)
4. В Event Graph привяжите Input Action к "Try Interact"

### 2. Создание интерактивного объекта через InteractableComponent

#### Blueprint:
1. Создайте новый Actor Blueprint
2. Добавьте Static Mesh Component
3. Добавьте "Interactable Component"
4. Настройте "Interaction Data":
   - Interaction Name: "Treasure Chest"
   - Interaction Type: "Open"
   - Action Text: "Press E to Open"
   - Max Interaction Distance: 200
5. Реализуйте события:
   - "On Begin Interact" - логика открытия
   - "On End Interact" - завершение
   - "On Focused/Unfocused" - визуальные эффекты

#### C++:
```cpp
// Простой интерактивный объект
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

### 3. Создание кастомного интерактивного актера через интерфейс

```cpp
UCLASS()
class AMyCustomInteractable : public AActor, public IInteractableInterface
{
    GENERATED_BODY()
    
public:
    virtual EInteractionResult BeginInteract_Implementation(AActor* InteractingActor) override
    {
        // Ваша логика
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
        // Кастомная валидация
        return bIsEnabled && !bIsUsed;
    }
    
private:
    bool bIsEnabled = true;
    bool bIsUsed = false;
};
```

## Готовые примеры (Examples)

### 1. **APickupableActor** - Подбираемые предметы
```cpp
// Использование в Blueprint:
// 1. Создайте Blueprint на основе PickupableActor
// 2. Настройте Item Name, Pickup Distance
// 3. Реализуйте событие "On Picked Up"
```

**Пример использования:**
- Создайте BP_HealthPotion : PickupableActor
- В "On Picked Up": добавьте здоровье игроку
- Установите "Destroy On Pickup" = true

### 2. **ADoorActor** - Двери
```cpp
// Использование в Blueprint:
// 1. Создайте Blueprint на основе DoorActor
// 2. Настройте Door Open Angle, Duration
// 3. Реализуйте анимацию в событиях:
//    - "On Door Opening" - начало открытия
//    - "On Door Opened" - дверь открыта
//    - "On Door Closing" - начало закрытия
//    - "On Door Closed" - дверь закрыта
```

**Пример Timeline анимации:**
```
On Door Opening:
  -> Create Timeline (0 to DoorOpenAngle over DoorOpenDuration)
  -> SetRelativeRotation(Door Component)
  -> On Timeline Finished -> Call "On Door Opened"
```

### 3. **ANPCActor** - NPC для диалогов
```cpp
// Использование в Blueprint:
// 1. Создайте Blueprint на основе NPCActor
// 2. Настройте NPC Name, Greeting Text
// 3. Реализуйте событие "On Dialogue Started":
//    - Откройте UI диалога
//    - Остановите движение NPC
// 4. Реализуйте "On Dialogue Ended":
//    - Закройте UI
//    - Возобновите AI
```

## Настройка подсветки объектов (Post Process Outline)

### 1. В Project Settings:
- Engine > Rendering > Custom Depth-Stencil Pass: "Enabled with Stencil"

### 2. Создайте Post Process Material:
```
Material Domain: Post Process
Blend Mode: Blend

Nodes:
SceneTexture:CustomDepth -> Compare (> 0)
  -> If True: Desired Outline Color
  -> If False: SceneColor
```

### 3. Добавьте Post Process Volume:
- Infinite Extent: true
- Post Process Materials: добавьте созданный материал

### 4. Настройка цветов:
- 250 - обычные интерактивные объекты (зеленый)
- 251 - закрытые/заблокированные (красный)
- 252 - NPC (синий)

## Расширение системы

### Добавление нового типа взаимодействия:

#### 1. Добавьте в InteractionTypes.h:
```cpp
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    // ...existing types...
    Repair    UMETA(DisplayName = "Repair"),  // НОВЫЙ ТИП
};
```

#### 2. Создайте новый класс:
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
    
    // ... остальной код
};
```

### Добавление требований для взаимодействия:

```cpp
// Добавьте в FInteractionData:
UPROPERTY(EditAnywhere, BlueprintReadWrite)
TArray<FName> RequiredItemTags;

// В BeginInteract проверяйте:
virtual bool CanInteract_Implementation(AActor* InteractingActor) const override
{
    if (RequiredItemTags.Num() > 0)
    {
        // Проверьте инвентарь InteractingActor
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

1. **Используйте интерфейс для гибкости:**
   - Любой актер может стать интерактивным, просто реализовав интерфейс
   - Не привязаны к конкретной иерархии классов

2. **Используйте компонент для быстрого прототипирования:**
   - UInteractableComponent идеален для Blueprint-only объектов
   - Быстрая настройка без C++

3. **Делегаты для UI и эффектов:**
   - Подписывайтесь на делегаты InteractionComponent в PlayerController
   - Показывайте UI подсказки при OnInteractableFocused
   - Обновляйте прогресс бары при длительных взаимодействиях

4. **Приоритеты взаимодействий:**
   - Используйте InteractionPriority если несколько объектов в фокусе
   - NPC > Quest Items > Regular Items

5. **Оптимизация:**
   - Настройте TraceTickInterval (по умолчанию 0.1s)
   - Используйте TraceRadius для более удобного взаимодействия
   - Отключайте bDrawDebugTrace в релизе

## Debugging

### Включите отладочную визуализацию:
```cpp
InteractionComponent->bDrawDebugTrace = true;
```

### Логирование:
```cpp
// В InteractableComponent добавьте логи
UE_LOG(LogTemp, Warning, TEXT("Interaction started by %s"), *InteractingActor->GetName());
```

### Частые проблемы:

1. **Объект не обнаруживается:**
   - Проверьте Collision Channel
   - Убедитесь что Trace Distance достаточна
   - Проверьте что объект реализует интерфейс

2. **Подсветка не работает:**
   - Включите Custom Depth в Project Settings
   - Настройте Post Process Material
   - Проверьте что Mesh компонент поддерживает Custom Depth

3. **Взаимодействие не срабатывает:**
   - Проверьте CanInteract возвращает true
   - Убедитесь что InteractionComponent->bInteractionEnabled = true
   - Проверьте расстояние и Line of Sight

## Интеграция с другими системами

### Инвентарь:
```cpp
// В PickupableActor::PerformPickup
if (IInventoryInterface* Inventory = Cast<IInventoryInterface>(PickedUpBy))
{
    Inventory->AddItem(this->ItemData);
}
```

### Quest система:
```cpp
// В BeginInteract
if (IQuestInterface* QuestManager = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
{
    QuestManager->OnObjectInteracted(this, InteractingActor);
}
```

### Save система:
```cpp
// Сохраняйте состояние интерактивных объектов
UPROPERTY(SaveGame)
bool bHasBeenInteractedWith;
```

## Заключение

Эта система обеспечивает:
- ? Гибкость через интерфейсы
- ? Удобство через компоненты
- ? Расширяемость через наследование
- ? Production-ready code
- ? Blueprint и C++ поддержка
- ? Готовые примеры для старта

Начните с готовых примеров (PickupableActor, DoorActor, NPCActor) и расширяйте под свои нужды!
