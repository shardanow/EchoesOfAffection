# DialogueSystemUI Module Documentation

## Обзор модуля

**DialogueSystemUI** — это модуль пользовательского интерфейса для системы диалогов, предоставляющий готовые виджеты для отображения диалогов, выборов игрока, портретов и анимаций текста.

## Архитектура

```
DialogueSystemUI/
??? Public/
?   ??? Widgets/
?   ?   ??? DialogueWidget.h        # Главный виджет диалога
?   ?   ??? ChoiceWidget.h          # Виджет выбора игрока
?   ??? Library/
?       ??? DialoguePresentationLibrary.h  # Blueprint функции
??? Private/
    ??? ...                         # Реализация
```

---

## Ключевые классы

### 1. UDialogueWidget

**Назначение**: Главный виджет для отображения диалога, включая текст, портреты, имена говорящих и выборы игрока.

#### Паттерн проектирования:

- **MVVM (Model-View-ViewModel)**: Виджет (View) отображает данные из Runner'а (ViewModel)
- **Event-driven**: Реагирует на события от DialogueRunner
- **Анимации**: Поддержка эффекта "печатной машинки" и переходов

#### Связываемые компоненты (Bind Widget):

```cpp
// Обязательные компоненты (должны быть в UMG Blueprint)
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UImage* SpeakerPortrait;  // Портрет говорящего

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UTextBlock* SpeakerName;  // Имя говорящего

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
URichTextBlock* DialogueText;  // Текст диалога (Rich Text для форматирования)

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UVerticalBox* ChoicesContainer;  // Контейнер для кнопок выбора

// Опциональные компоненты
UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UCanvasPanel* HistoryPanel;  // Панель истории (необязательно)

UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UImage* EmotionIcon;  // Иконка эмоции (необязательно)

UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UImage* ContinueIndicator;  // Индикатор "нажмите для продолжения"
```

#### Настройки:

```cpp
// Класс виджета для кнопки выбора
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
TSubclassOf<UChoiceWidget> ChoiceWidgetClass;

// Включить эффект "печатной машинки"
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
bool bEnableTypewriter = true;

// Показывать портрет
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
bool bShowPortrait = true;

// Показывать эмоции
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
bool bShowEmotions = true;
```

#### Основные методы:

```cpp
// Привязка к Runner
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void BindToRunner(UDialogueRunner* Runner);

// Отвязаться от Runner
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void UnbindFromRunner();

// Пропустить typewriter (показать весь текст сразу)
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void SkipTypewriter();

// Выбрать вариант ответа
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void SelectChoice(int32 Index);
```

#### Blueprint Events (Implementable Events):

```cpp
// Анимация появления виджета
UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
void PlayIntroAnimation();

// Анимация скрытия виджета
UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
void PlayOutroAnimation();
```

---

### Создание DialogueWidget в UMG

**Шаг 1. Создание Widget Blueprint**:

1. В Content Browser: `Right Click ? User Interface ? Widget Blueprint`
2. Назовите: `WBP_DialogueWidget`
3. В Parent Class выберите: `DialogueWidget` (или ваш наследник от него)

**Шаг 2. Настройка иерархии виджетов**:

```
Canvas Panel
??? Background (Image) - Фон диалога
??? PortraitContainer (Canvas Panel)
?   ??? SpeakerPortrait (Image) [Name: "SpeakerPortrait"] **REQUIRED**
??? TextContainer (Vertical Box)
?   ??? SpeakerName (Text Block) [Name: "SpeakerName"] **REQUIRED**
?   ??? DialogueText (Rich Text Block) [Name: "DialogueText"] **REQUIRED**
??? ChoicesContainer (Vertical Box) [Name: "ChoicesContainer"] **REQUIRED**
??? BottomBar (Horizontal Box)
    ??? EmotionIcon (Image) [Name: "EmotionIcon"] *Optional*
    ??? ContinueIndicator (Image) [Name: "ContinueIndicator"] *Optional*
```

**Важно**: Имена виджетов должны точно совпадать с именами в коде (BindWidget)!

**Шаг 3. Настройка в Details Panel**:

```
DialogueWidget (Details Panel):
  - Choice Widget Class: WBP_ChoiceButton (ваш Blueprint для кнопки выбора)
  - Enable Typewriter: true
  - Show Portrait: true
  - Show Emotions: true
```

**Шаг 4. Реализация Blueprint Events**:

```blueprintue
// В Event Graph WBP_DialogueWidget:

// Event: Play Intro Animation
Event Play Intro Animation
    ? Play Animation [Intro_Anim]
        ? Set Render Opacity (Self, 0.0)
        ? Fade In (0.5 seconds)

// Event: Play Outro Animation
Event Play Outro Animation
    ? Play Animation [Outro_Anim]
        ? Fade Out (0.5 seconds)
        ? Remove from Parent
```

---

### Использование DialogueWidget в Blueprint

**Пример 1: Создание и показ виджета при старте диалога**:

```blueprintue
// В Actor/PlayerController:

// Переменные:
- DialogueWidgetInstance (UDialogueWidget*)
- DialogueWidgetClass (TSubclassOf<UDialogueWidget>) [По умолчанию: WBP_DialogueWidget]

// Event: OnDialogueStarted (от DialogueComponent)
Event OnDialogueStarted
    Runner (параметр)
    ? Branch (DialogueWidgetInstance == null)
        True:
            ? Create Widget [DialogueWidgetClass] ? DialogueWidgetInstance
            ? Add to Viewport (DialogueWidgetInstance)
            ? DialogueWidgetInstance ? Play Intro Animation
    ? DialogueWidgetInstance ? Bind To Runner (Runner)

// Event: OnDialogueEnded
Event OnDialogueEnded
    ? DialogueWidgetInstance ? Unbind From Runner
    ? DialogueWidgetInstance ? Play Outro Animation
    ? Delay (0.5) // Ждем окончания анимации
    ? Remove from Parent (DialogueWidgetInstance)
    ? Set DialogueWidgetInstance = null
```

**Пример 2: Пропуск typewriter при клике**:

```blueprintue
// В WBP_DialogueWidget, Event Graph:

Event OnMouseButtonDown (Override)
    ? Branch (bIsTyping)
        True:
            ? Skip Typewriter
        False:
            ? CurrentRunner ? Continue
```

**Пример 3: Обработка выбора с клавиатуры**:

```blueprintue
// В WBP_DialogueWidget, Event Graph:

Event OnKeyDown (Override)
    Key (параметр)
    ? Switch on EKeys (Key)
        Case "One":
            ? Select Choice (0)
        Case "Two":
            ? Select Choice (1)
        Case "Three":
            ? Select Choice (2)
        Case "Four":
            ? Select Choice (3)
```

---

### Пример использования в C++:

```cpp
// В вашем PlayerController или HUD

#include "Widgets/DialogueWidget.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerController::ShowDialogueWidget(UDialogueRunner* Runner)
{
    if (!DialogueWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidgetClass is not set!"));
        return;
    }

    // Создать виджет
    DialogueWidgetInstance = CreateWidget<UDialogueWidget>(this, DialogueWidgetClass);
    if (DialogueWidgetInstance)
    {
        // Добавить на экран
        DialogueWidgetInstance->AddToViewport();
        
        // Привязать к Runner
        DialogueWidgetInstance->BindToRunner(Runner);
        
        // Воспроизвести анимацию появления
        DialogueWidgetInstance->PlayIntroAnimation();
    }
}

void AMyPlayerController::HideDialogueWidget()
{
    if (DialogueWidgetInstance)
    {
        // Отвязаться от Runner
        DialogueWidgetInstance->UnbindFromRunner();
        
        // Воспроизвести анимацию исчезновения
        DialogueWidgetInstance->PlayOutroAnimation();
        
        // Удалить через 0.5 секунд (после анимации)
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            if (DialogueWidgetInstance)
            {
                DialogueWidgetInstance->RemoveFromParent();
                DialogueWidgetInstance = nullptr;
            }
        }, 0.5f, false);
    }
}
```

---

## 2. UChoiceWidget

**Назначение**: Виджет для отображения одной кнопки выбора игрока.

#### Связываемые компоненты (Bind Widget):

```cpp
// Обязательные
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UButton* ChoiceButton;  // Кнопка

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UTextBlock* ChoiceText;  // Текст выбора

// Опциональные
UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UImage* ConsequenceIcon;  // Иконка последствий (например, +/- отношения)

UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UTextBlock* HotkeyText;  // Текст горячей клавиши (1, 2, 3...)
```

#### Делегат:

```cpp
// Срабатывает при клике на кнопку
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceSelectedSignature, int32, ChoiceIndex);

UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnChoiceSelectedSignature OnChoiceSelected;
```

#### Основные методы:

```cpp
// Инициализировать кнопку с данными выбора
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void InitializeChoice(UDialogueNode* Node, int32 Index);

// Установить данные выбора
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void SetChoiceData(UDialogueNode* Node, int32 Index);

// Получить индекс
UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
int32 GetChoiceIndex() const;

// Получить узел
UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
UDialogueNode* GetNode() const;
```

#### Blueprint Event:

```cpp
// Событие инициализации (для кастомизации)
UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
void OnChoiceInitialized(UDialogueNode* Node, int32 Index);
```

---

### Создание ChoiceWidget в UMG

**Шаг 1. Создание Widget Blueprint**:

1. В Content Browser: `Right Click ? User Interface ? Widget Blueprint`
2. Назовите: `WBP_ChoiceButton`
3. В Parent Class выберите: `ChoiceWidget`

**Шаг 2. Настройка иерархии виджетов**:

```
Button (Name: "ChoiceButton") **REQUIRED**
??? Overlay
    ??? Background (Image) - Фон кнопки
    ??? HotkeyContainer (Canvas Panel) *Optional*
    ?   ??? HotkeyText (Text Block) [Name: "HotkeyText"] *Optional*
    ??? ChoiceText (Text Block) [Name: "ChoiceText"] **REQUIRED**
    ??? ConsequenceIcon (Image) [Name: "ConsequenceIcon"] *Optional*
```

**Шаг 3. Настройка стилей**:

```
ChoiceButton (Details Panel):
  - Style:
      - Normal: BackgroundColor = (R=0.1, G=0.1, B=0.1, A=0.8)
      - Hovered: BackgroundColor = (R=0.2, G=0.2, B=0.2, A=1.0)
      - Pressed: BackgroundColor = (R=0.3, G=0.3, B=0.3, A=1.0)

ChoiceText (Details Panel):
  - Font: Roboto, Size = 16
  - Color: White
  - Justification: Left
  - Padding: (Left=10, Top=5, Right=10, Bottom=5)
```

**Шаг 4. Реализация Blueprint Events**:

```blueprintue
// В Event Graph WBP_ChoiceButton:

// Event: On Choice Initialized
Event On Choice Initialized
    Node (параметр)
    Index (параметр)
    ? Get Choice Text from Node ? ChoiceText_Value
    ? Set Text (ChoiceText, ChoiceText_Value)
    ? Set Text (HotkeyText, ToString(Index + 1))
    
    // Опционально: показать иконку последствий
    ? Parse Effect String (Node.Effect) ? ParsedEffect
    ? Branch (ParsedEffect contains "affinity")
        True ? Set Brush from Texture (ConsequenceIcon, Icon_Heart)
        False ? Set Visibility (ConsequenceIcon, Collapsed)
```

---

### Использование ChoiceWidget в Blueprint

**Пример: Кастомизация кнопки выбора**:

```blueprintue
// В WBP_ChoiceButton, Event Graph:

// Event: On Hovered (Button)
Event On Hovered
    ? Play Animation [Hover_Anim]
    ? Play Sound 2D [SFX_ButtonHover]

// Event: On Unhovered (Button)
Event On Unhovered
    ? Stop Animation [Hover_Anim]

// Event: On Clicked (Button)
Event On Clicked
    ? Play Sound 2D [SFX_ButtonClick]
    ? Play Animation [Click_Anim]

// Event: On Choice Initialized
Event On Choice Initialized
    Node (параметр)
    Index (параметр)
    ? Get Choice Text ? Text
    ? Set Text (ChoiceText, Text)
    ? Set Text (HotkeyText, "[" + ToString(Index + 1) + "]")
    
    // Парсим эффекты для иконки
    ? Get Effect String (Node) ? EffectString
    ? Branch (EffectString contains "affinity")
        True:
            ? Branch (EffectString contains "+=")
                True ? Set Brush (ConsequenceIcon, Icon_Heart_Plus)
                False ? Set Brush (ConsequenceIcon, Icon_Heart_Minus)
            ? Set Visibility (ConsequenceIcon, Visible)
        False:
            ? Set Visibility (ConsequenceIcon, Collapsed)
```

---

## Пошаговые примеры использования

### Пример 1: Полная настройка UI диалога (Blueprint)

**Шаг 1. Создание ChoiceWidget**:

1. Создайте `WBP_ChoiceButton` (как описано выше)
2. Настройте иерархию виджетов
3. Реализуйте `OnChoiceInitialized` event

**Шаг 2. Создание DialogueWidget**:

1. Создайте `WBP_DialogueWidget` (как описано выше)
2. Настройте иерархию виджетов
3. В Details Panel установите `Choice Widget Class = WBP_ChoiceButton`
4. Реализуйте `PlayIntroAnimation` и `PlayOutroAnimation` events

**Шаг 3. Создание анимаций**:

```blueprintue
// В WBP_DialogueWidget, Animations:

// Intro_Anim (длительность: 0.5 сек)
Track 1: Opacity
  - 0.0s: Opacity = 0.0
  - 0.5s: Opacity = 1.0

Track 2: Translation Y
  - 0.0s: Y = -50
  - 0.5s: Y = 0

// Outro_Anim (длительность: 0.5 сек)
Track 1: Opacity
  - 0.0s: Opacity = 1.0
  - 0.5s: Opacity = 0.0

Track 2: Translation Y
  - 0.0s: Y = 0
  - 0.5s: Y = 50
```

**Шаг 4. Интеграция в игру**:

```blueprintue
// В BP_PlayerController, Event Graph:

// Переменные:
- DialogueWidgetInstance (UDialogueWidget*)
- DialogueWidgetClass (TSubclassOf<UDialogueWidget>) [Default: WBP_DialogueWidget]

// Event: On Dialogue Started (от DialogueComponent или Subsystem)
Event On Dialogue Started
    Runner (параметр)
    ? Branch (DialogueWidgetInstance == null)
        True:
            ? Create Widget [DialogueWidgetClass] ? DialogueWidgetInstance
            ? Add to Viewport (DialogueWidgetInstance)
    ? DialogueWidgetInstance ? Bind To Runner (Runner)
    ? DialogueWidgetInstance ? Play Intro Animation
    ? Set Input Mode UI Only (Self)
    ? Show Mouse Cursor (Self, true)

// Event: On Dialogue Ended
Event On Dialogue Ended
    ? DialogueWidgetInstance ? Unbind From Runner
    ? DialogueWidgetInstance ? Play Outro Animation
    ? Delay (0.5)
    ? Remove from Parent (DialogueWidgetInstance)
    ? Set DialogueWidgetInstance = null
    ? Set Input Mode Game Only (Self)
    ? Show Mouse Cursor (Self, false)
```

---

### Пример 2: Эффект "печатной машинки" с настройками

**В C++ (UDialogueWidget::UpdateTypewriter)**:

```cpp
void UDialogueWidget::UpdateTypewriter(float DeltaTime)
{
    if (!bIsTyping || CurrentCharIndex >= FullText.Len())
    {
        bIsTyping = false;
        return;
    }

    // Получить скорость из Subsystem
    UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    float Speed = Subsystem ? Subsystem->GetTypewriterSpeed() : 30.0f;

    // Рассчитать количество символов для отображения
    CurrentCharIndex = FMath::Min(CurrentCharIndex + FMath::RoundToInt(Speed * DeltaTime), FullText.Len());
    
    // Обновить видимый текст
    VisibleText = FullText.Left(CurrentCharIndex);
    
    // Обновить виджет
    if (DialogueText)
    {
        DialogueText->SetText(FText::FromString(VisibleText));
    }

    // Если достигли конца, остановить
    if (CurrentCharIndex >= FullText.Len())
    {
        bIsTyping = false;
        OnTypewriterFinished();
    }
}
```

**В Blueprint (настройка скорости)**:

```blueprintue
// В Settings Menu:

Event OnTypewriterSpeedChanged
    NewSpeed (параметр: float)
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem]
        ? Set Typewriter Speed (NewSpeed)
```

---

### Пример 3: История диалога (опционально)

**Шаг 1. Добавление панели истории**:

```
// В WBP_DialogueWidget, добавить:

HistoryPanel (Canvas Panel) [Name: "HistoryPanel"] *Optional*
??? ScrollBox (Name: "HistoryScrollBox")
    ??? HistoryContainer (Vertical Box) [Name: "HistoryContainer"]
```

**Шаг 2. Реализация добавления в историю**:

```blueprintue
// В WBP_DialogueWidget, Event Graph:

// Переменная:
- HistoryEntries (Array of Text)

// Event: Handle Node Entered (Custom)
Event Handle Node Entered
    Node (параметр)
    ? Get Dialogue Text ? Text
    ? Get Speaker Name ? SpeakerName
    ? Format Text ("{SpeakerName}: {Text}") ? FormattedText
    ? Add (HistoryEntries, FormattedText)
    
    // Создать виджет для истории
    ? Create Widget [WBP_HistoryEntry]
        ? Set Text (FormattedText)
    ? Add Child to Vertical Box (HistoryContainer, Widget)
    
    // Скроллить вниз
    ? Scroll to End (HistoryScrollBox)
```

---

### Пример 4: Анимация портретов и эмоций

**Шаг 1. Настройка эмоций в DialogueNode**:

```cpp
// В Data Asset диалога:

Node[0]:
  - Dialogue Text: "Я так рада тебя видеть!"
  - Speaker Name: "Lianne"
  - Tags: ["Emotion.Happy"]

Node[1]:
  - Dialogue Text: "Я очень расстроена..."
  - Speaker Name: "Lianne"
  - Tags: ["Emotion.Sad"]
```

**Шаг 2. Реализация смены эмоций**:

```blueprintue
// В WBP_DialogueWidget, Event Graph:

// Event: Handle Node Entered
Event Handle Node Entered
    Node (параметр)
    ? Get Tags (Node) ? Tags
    ? For Each Loop (Tags)
        ? Branch (Tag has "Emotion" parent tag)
            True:
                ? Switch on Tag Name
                    Case "Emotion.Happy":
                        ? Set Brush (EmotionIcon, Icon_Happy)
                        ? Play Animation [PortraitBounce]
                    Case "Emotion.Sad":
                        ? Set Brush (EmotionIcon, Icon_Sad)
                        ? Play Animation [PortraitShake]
                    Case "Emotion.Angry":
                        ? Set Brush (EmotionIcon, Icon_Angry)
                        ? Play Animation [PortraitShake]
                    Case "Emotion.Surprised":
                        ? Set Brush (EmotionIcon, Icon_Surprised)
                        ? Play Animation [PortraitZoom]
```

---

### Пример 5: Интеграция с звуком

**Шаг 1. Настройка звуков в DialogueNode**:

```cpp
// В Data Asset диалога:

Node[0]:
  - Dialogue Text: "Привет!"
  - Voice Over: VO_Lianne_Greeting
  - Speaker Name: "Lianne"
```

**Шаг 2. Воспроизведение звуков**:

```blueprintue
// В WBP_DialogueWidget, Event Graph:

// Event: Handle Node Entered
Event Handle Node Entered
    Node (параметр)
    ? Get Voice Over (Node) ? VoiceOver
    ? Branch (VoiceOver != null)
        True:
            ? Stop All Sounds (Audio Component)
            ? Play Sound 2D (VoiceOver)
            ? Set Audio Component (VoiceOver Audio Component)

// Event: Handle Dialogue Ended
Event Handle Dialogue Ended
    ? Stop All Sounds (Audio Component)
```

---

## Расширенные возможности

### 1. Rich Text форматирование

**Настройка Rich Text Block**:

```blueprintue
// В WBP_DialogueWidget, DialogueText (Rich Text Block):

Details Panel:
  - Text Style Set: DT_DialogueTextStyles (Data Table)

// Data Table DT_DialogueTextStyles (структура: FRichTextStyleRow):
[Bold]
  - Font: Roboto-Bold, Size = 18
  - Color: Yellow

[Italic]
  - Font: Roboto-Italic, Size = 16
  - Color: White

[Red]
  - Color: Red
```

**Использование в диалоге**:

```cpp
// В Data Asset:
Node[0]:
  - Dialogue Text: "Ты <Bold>должен</> найти <Red>артефакт</>!"
```

---

### 2. Локализация

**Настройка локализации текста**:

```cpp
// В DialogueNode:
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content", meta = (MultiLine = "true"))
FText DialogueText;  // FText автоматически поддерживает локализацию
```

**Использование String Tables**:

```cpp
// Создайте String Table:
Content/Localization/DialogueStringTable

// В Data Asset:
Node[0]:
  - Dialogue Text: NSLOCTEXT("Dialogue", "Lianne_Greeting", "Привет! Как дела?")
```

---

### 3. Доступность (Accessibility)

**Добавление субтитров**:

```blueprintue
// В WBP_DialogueWidget, добавить:

SubtitlesPanel (Overlay) [Optional]
??? SubtitlesText (Text Block)
    - Font Size: 20
    - Color: White with Black Outline
    - Justification: Center

// Event: Handle Node Entered
Event Handle Node Entered
    Node (параметр)
    ? Get Dialogue Text ? Text
    ? Set Text (SubtitlesText, Text)
    ? Set Visibility (SubtitlesPanel, Visible)
```

**Поддержка клавиатурной навигации**:

```blueprintue
// В WBP_DialogueWidget:

Event OnKeyDown (Override)
    Key (параметр)
    ? Switch on Key
        Case "Up":
            ? Select Previous Choice Button
        Case "Down":
            ? Select Next Choice Button
        Case "Enter":
            ? Click Selected Choice Button
```

---

## Лучшие практики

1. **Используйте Bind Widget**: Всегда используйте `BindWidget` и `BindWidgetOptional` для автоматического связывания виджетов.

2. **Разделяйте логику**: Держите бизнес-логику в C++ (DialogueRunner), а презентацию - в Blueprint (UMG).

3. **Оптимизируйте анимации**: Используйте UMG Animations вместо Blueprints для плавности.

4. **Кэшируйте виджеты**: Не создавайте виджеты каждый раз, переиспользуйте их.

5. **Тестируйте на разных разрешениях**: Используйте Anchors и Size Box для адаптивности.

6. **Поддерживайте доступность**: Добавляйте субтитры, клавиатурную навигацию и настройки шрифтов.

7. **Используйте Rich Text**: Rich Text Block позволяет форматировать текст без создания множества виджетов.

8. **Локализуйте все тексты**: Используйте FText вместо FString для всех пользовательских текстов.

---

## Заключение

**DialogueSystemUI** предоставляет гибкие и настраиваемые виджеты для отображения диалогов. Благодаря паттерну MVVM и event-driven архитектуре, UI легко интегрируется с остальной системой и может быть полностью кастомизирован под стиль вашей игры.

Для получения дополнительной информации см.:
- `01_DialogueSystemCore_Module.md` — Ядро системы
- `02_DialogueSystemRuntime_Module.md` — Runtime компоненты
- `04_DialogueSystemAI_Module.md` — Интеграция с AI
- `05_Complete_Examples.md` — Полные примеры интеграции
