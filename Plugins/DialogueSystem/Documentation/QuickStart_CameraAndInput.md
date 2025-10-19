# Camera Control & Input Blocking - Quick Start

## Для новичков (Blueprint)

### Шаг 1: Добавьте компонент
1. Откройте Blueprint вашего персонажа игрока
2. Нажмите "Add Component"
3. Найдите "Dialogue Presentation Component"
4. Добавьте его

### Шаг 2: Настройте
В Details панели компонента:
- **Default Presentation Mode**: Выберите "Cinematic"
- Оставьте остальные настройки по умолчанию

### Шаг 3: Используйте
В Event Graph (например, при взаимодействии с NPC):

```
[Event] On Interact With NPC
   ?
Start Dialogue Presentation (from DialoguePresentationLibrary)
   ?? Target NPC: [Ваш NPC Actor]
   ?? Mode: Cinematic
   ?? ?
   [Ваша логика диалога]
   ?? ?
   End Dialogue Presentation (from DialoguePresentationLibrary)
```

**Готово!** Теперь при диалоге камера автоматически переключится на NPC, а управление заблокируется.

---

## Для опытных разработчиков (C++)

### Быстрая интеграция

```cpp
// PlayerCharacter.h
UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
UDialoguePresentationComponent* PresentationComponent;

// PlayerCharacter.cpp (Constructor)
PresentationComponent = CreateDefaultSubobject<UDialoguePresentationComponent>(TEXT("DialoguePresentation"));

// Использование
void APlayerCharacter::StartDialogue(AActor* NPC)
{
    PresentationComponent->ActivatePresentation(NPC, EDialoguePresentationMode::Cinematic);
    // ... ваша логика диалога
}

void APlayerCharacter::EndDialogue()
{
    PresentationComponent->DeactivatePresentation();
}
```

---

## Частые сценарии

### 1. Интерактивный диалог (игрок может двигаться)
```cpp
// Blueprint
Mode: Interactive

// C++
PresentationComponent->ActivatePresentation(NPC, EDialoguePresentationMode::Interactive);
```

### 2. First-Person диалог
```cpp
// Blueprint
Mode: First-Person

// C++
PresentationComponent->ActivatePresentation(NPC, EDialoguePresentationMode::FirstPerson);
```

### 3. Смена камеры во время диалога
```cpp
// Blueprint
Transition To New Target (from PresentationComponent)
   ?? New Target NPC: [Другой персонаж]
   ?? Blend Time: 0.5

// C++
PresentationComponent->TransitionToNewTarget(OtherNPC, 0.5f);
```

### 4. Кастомные настройки камеры
```cpp
// Blueprint
1. Make Default Camera Settings (from DialoguePresentationLibrary)
   ?? Framing: Close-Up
2. Set параметры (Distance, Height, FOV, etc.)
3. Activate Dialogue Camera (from CameraComponent)
   ?? Target Actor: [NPC]
   ?? Settings: [Ваши настройки]

// C++
FDialogueCameraSettings CustomSettings;
CustomSettings.Framing = EDialogueCameraFraming::CloseUp;
CustomSettings.Distance = 150.0f;
CustomSettings.FOVOverride = 55.0f;
CameraComponent->ActivateDialogueCamera(NPC, CustomSettings);
```

---

## Решение проблем

### Камера не активируется?
- Проверьте, что компонент добавлен на персонажа игрока
- Убедитесь, что `bEnableCamera = true`
- Проверьте, что NPC - валидный Actor

### Ввод не блокируется?
- Проверьте, что `bEnableInputBlocking = true`
- Для UE5 убедитесь, что Enhanced Input настроен
- Проверьте режим блокировки (Mode)

### Камера дергается?
- Увеличьте `Blend Time` (попробуйте 0.5-1.0)
- Включите `bEnableCollision = true`
- Убедитесь, что `bUseSpringArm = true`

---

## Следующие шаги

?? Полное руководство: `CameraAndInput_Guide.md`
?? Примеры Blueprint: `Content/DialogueSystem/Examples/`
?? API документация: Смотрите комментарии в `.h` файлах

---

## Поддержка

Если возникли вопросы:
1. Проверьте `CameraAndInput_Guide.md` (подробная документация)
2. Изучите примеры в `Content/DialogueSystem/Examples/`
3. Проверьте логи: категория `LogDialogueCamera` и `LogDialogueInput`

---

**Совет**: Начните с режима "Cinematic" для диалогов и "Interactive" для случайных разговоров.
