# DialogueSystemUI Module Documentation

## ����� ������

**DialogueSystemUI** � ��� ������ ����������������� ���������� ��� ������� ��������, ��������������� ������� ������� ��� ����������� ��������, ������� ������, ��������� � �������� ������.

## �����������

```
DialogueSystemUI/
??? Public/
?   ??? Widgets/
?   ?   ??? DialogueWidget.h        # ������� ������ �������
?   ?   ??? ChoiceWidget.h          # ������ ������ ������
?   ??? Library/
?       ??? DialoguePresentationLibrary.h  # Blueprint �������
??? Private/
    ??? ...                         # ����������
```

---

## �������� ������

### 1. UDialogueWidget

**����������**: ������� ������ ��� ����������� �������, ������� �����, ��������, ����� ��������� � ������ ������.

#### ������� ��������������:

- **MVVM (Model-View-ViewModel)**: ������ (View) ���������� ������ �� Runner'� (ViewModel)
- **Event-driven**: ��������� �� ������� �� DialogueRunner
- **��������**: ��������� ������� "�������� �������" � ���������

#### ����������� ���������� (Bind Widget):

```cpp
// ������������ ���������� (������ ���� � UMG Blueprint)
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UImage* SpeakerPortrait;  // ������� ����������

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UTextBlock* SpeakerName;  // ��� ����������

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
URichTextBlock* DialogueText;  // ����� ������� (Rich Text ��� ��������������)

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UVerticalBox* ChoicesContainer;  // ��������� ��� ������ ������

// ������������ ����������
UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UCanvasPanel* HistoryPanel;  // ������ ������� (�������������)

UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UImage* EmotionIcon;  // ������ ������ (�������������)

UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UImage* ContinueIndicator;  // ��������� "������� ��� �����������"
```

#### ���������:

```cpp
// ����� ������� ��� ������ ������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
TSubclassOf<UChoiceWidget> ChoiceWidgetClass;

// �������� ������ "�������� �������"
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
bool bEnableTypewriter = true;

// ���������� �������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
bool bShowPortrait = true;

// ���������� ������
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
bool bShowEmotions = true;
```

#### �������� ������:

```cpp
// �������� � Runner
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void BindToRunner(UDialogueRunner* Runner);

// ���������� �� Runner
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void UnbindFromRunner();

// ���������� typewriter (�������� ���� ����� �����)
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void SkipTypewriter();

// ������� ������� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void SelectChoice(int32 Index);
```

#### Blueprint Events (Implementable Events):

```cpp
// �������� ��������� �������
UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
void PlayIntroAnimation();

// �������� ������� �������
UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
void PlayOutroAnimation();
```

---

### �������� DialogueWidget � UMG

**��� 1. �������� Widget Blueprint**:

1. � Content Browser: `Right Click ? User Interface ? Widget Blueprint`
2. ��������: `WBP_DialogueWidget`
3. � Parent Class ��������: `DialogueWidget` (��� ��� ��������� �� ����)

**��� 2. ��������� �������� ��������**:

```
Canvas Panel
??? Background (Image) - ��� �������
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

**�����**: ����� �������� ������ ����� ��������� � ������� � ���� (BindWidget)!

**��� 3. ��������� � Details Panel**:

```
DialogueWidget (Details Panel):
  - Choice Widget Class: WBP_ChoiceButton (��� Blueprint ��� ������ ������)
  - Enable Typewriter: true
  - Show Portrait: true
  - Show Emotions: true
```

**��� 4. ���������� Blueprint Events**:

```blueprintue
// � Event Graph WBP_DialogueWidget:

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

### ������������� DialogueWidget � Blueprint

**������ 1: �������� � ����� ������� ��� ������ �������**:

```blueprintue
// � Actor/PlayerController:

// ����������:
- DialogueWidgetInstance (UDialogueWidget*)
- DialogueWidgetClass (TSubclassOf<UDialogueWidget>) [�� ���������: WBP_DialogueWidget]

// Event: OnDialogueStarted (�� DialogueComponent)
Event OnDialogueStarted
    Runner (��������)
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
    ? Delay (0.5) // ���� ��������� ��������
    ? Remove from Parent (DialogueWidgetInstance)
    ? Set DialogueWidgetInstance = null
```

**������ 2: ������� typewriter ��� �����**:

```blueprintue
// � WBP_DialogueWidget, Event Graph:

Event OnMouseButtonDown (Override)
    ? Branch (bIsTyping)
        True:
            ? Skip Typewriter
        False:
            ? CurrentRunner ? Continue
```

**������ 3: ��������� ������ � ����������**:

```blueprintue
// � WBP_DialogueWidget, Event Graph:

Event OnKeyDown (Override)
    Key (��������)
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

### ������ ������������� � C++:

```cpp
// � ����� PlayerController ��� HUD

#include "Widgets/DialogueWidget.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerController::ShowDialogueWidget(UDialogueRunner* Runner)
{
    if (!DialogueWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidgetClass is not set!"));
        return;
    }

    // ������� ������
    DialogueWidgetInstance = CreateWidget<UDialogueWidget>(this, DialogueWidgetClass);
    if (DialogueWidgetInstance)
    {
        // �������� �� �����
        DialogueWidgetInstance->AddToViewport();
        
        // ��������� � Runner
        DialogueWidgetInstance->BindToRunner(Runner);
        
        // ������������� �������� ���������
        DialogueWidgetInstance->PlayIntroAnimation();
    }
}

void AMyPlayerController::HideDialogueWidget()
{
    if (DialogueWidgetInstance)
    {
        // ���������� �� Runner
        DialogueWidgetInstance->UnbindFromRunner();
        
        // ������������� �������� ������������
        DialogueWidgetInstance->PlayOutroAnimation();
        
        // ������� ����� 0.5 ������ (����� ��������)
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

**����������**: ������ ��� ����������� ����� ������ ������ ������.

#### ����������� ���������� (Bind Widget):

```cpp
// ������������
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UButton* ChoiceButton;  // ������

UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
UTextBlock* ChoiceText;  // ����� ������

// ������������
UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UImage* ConsequenceIcon;  // ������ ����������� (��������, +/- ���������)

UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
UTextBlock* HotkeyText;  // ����� ������� ������� (1, 2, 3...)
```

#### �������:

```cpp
// ����������� ��� ����� �� ������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceSelectedSignature, int32, ChoiceIndex);

UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
FOnChoiceSelectedSignature OnChoiceSelected;
```

#### �������� ������:

```cpp
// ���������������� ������ � ������� ������
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void InitializeChoice(UDialogueNode* Node, int32 Index);

// ���������� ������ ������
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
void SetChoiceData(UDialogueNode* Node, int32 Index);

// �������� ������
UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
int32 GetChoiceIndex() const;

// �������� ����
UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
UDialogueNode* GetNode() const;
```

#### Blueprint Event:

```cpp
// ������� ������������� (��� ������������)
UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
void OnChoiceInitialized(UDialogueNode* Node, int32 Index);
```

---

### �������� ChoiceWidget � UMG

**��� 1. �������� Widget Blueprint**:

1. � Content Browser: `Right Click ? User Interface ? Widget Blueprint`
2. ��������: `WBP_ChoiceButton`
3. � Parent Class ��������: `ChoiceWidget`

**��� 2. ��������� �������� ��������**:

```
Button (Name: "ChoiceButton") **REQUIRED**
??? Overlay
    ??? Background (Image) - ��� ������
    ??? HotkeyContainer (Canvas Panel) *Optional*
    ?   ??? HotkeyText (Text Block) [Name: "HotkeyText"] *Optional*
    ??? ChoiceText (Text Block) [Name: "ChoiceText"] **REQUIRED**
    ??? ConsequenceIcon (Image) [Name: "ConsequenceIcon"] *Optional*
```

**��� 3. ��������� ������**:

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

**��� 4. ���������� Blueprint Events**:

```blueprintue
// � Event Graph WBP_ChoiceButton:

// Event: On Choice Initialized
Event On Choice Initialized
    Node (��������)
    Index (��������)
    ? Get Choice Text from Node ? ChoiceText_Value
    ? Set Text (ChoiceText, ChoiceText_Value)
    ? Set Text (HotkeyText, ToString(Index + 1))
    
    // �����������: �������� ������ �����������
    ? Parse Effect String (Node.Effect) ? ParsedEffect
    ? Branch (ParsedEffect contains "affinity")
        True ? Set Brush from Texture (ConsequenceIcon, Icon_Heart)
        False ? Set Visibility (ConsequenceIcon, Collapsed)
```

---

### ������������� ChoiceWidget � Blueprint

**������: ������������ ������ ������**:

```blueprintue
// � WBP_ChoiceButton, Event Graph:

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
    Node (��������)
    Index (��������)
    ? Get Choice Text ? Text
    ? Set Text (ChoiceText, Text)
    ? Set Text (HotkeyText, "[" + ToString(Index + 1) + "]")
    
    // ������ ������� ��� ������
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

## ��������� ������� �������������

### ������ 1: ������ ��������� UI ������� (Blueprint)

**��� 1. �������� ChoiceWidget**:

1. �������� `WBP_ChoiceButton` (��� ������� ����)
2. ��������� �������� ��������
3. ���������� `OnChoiceInitialized` event

**��� 2. �������� DialogueWidget**:

1. �������� `WBP_DialogueWidget` (��� ������� ����)
2. ��������� �������� ��������
3. � Details Panel ���������� `Choice Widget Class = WBP_ChoiceButton`
4. ���������� `PlayIntroAnimation` � `PlayOutroAnimation` events

**��� 3. �������� ��������**:

```blueprintue
// � WBP_DialogueWidget, Animations:

// Intro_Anim (������������: 0.5 ���)
Track 1: Opacity
  - 0.0s: Opacity = 0.0
  - 0.5s: Opacity = 1.0

Track 2: Translation Y
  - 0.0s: Y = -50
  - 0.5s: Y = 0

// Outro_Anim (������������: 0.5 ���)
Track 1: Opacity
  - 0.0s: Opacity = 1.0
  - 0.5s: Opacity = 0.0

Track 2: Translation Y
  - 0.0s: Y = 0
  - 0.5s: Y = 50
```

**��� 4. ���������� � ����**:

```blueprintue
// � BP_PlayerController, Event Graph:

// ����������:
- DialogueWidgetInstance (UDialogueWidget*)
- DialogueWidgetClass (TSubclassOf<UDialogueWidget>) [Default: WBP_DialogueWidget]

// Event: On Dialogue Started (�� DialogueComponent ��� Subsystem)
Event On Dialogue Started
    Runner (��������)
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

### ������ 2: ������ "�������� �������" � �����������

**� C++ (UDialogueWidget::UpdateTypewriter)**:

```cpp
void UDialogueWidget::UpdateTypewriter(float DeltaTime)
{
    if (!bIsTyping || CurrentCharIndex >= FullText.Len())
    {
        bIsTyping = false;
        return;
    }

    // �������� �������� �� Subsystem
    UDialogueSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
    float Speed = Subsystem ? Subsystem->GetTypewriterSpeed() : 30.0f;

    // ���������� ���������� �������� ��� �����������
    CurrentCharIndex = FMath::Min(CurrentCharIndex + FMath::RoundToInt(Speed * DeltaTime), FullText.Len());
    
    // �������� ������� �����
    VisibleText = FullText.Left(CurrentCharIndex);
    
    // �������� ������
    if (DialogueText)
    {
        DialogueText->SetText(FText::FromString(VisibleText));
    }

    // ���� �������� �����, ����������
    if (CurrentCharIndex >= FullText.Len())
    {
        bIsTyping = false;
        OnTypewriterFinished();
    }
}
```

**� Blueprint (��������� ��������)**:

```blueprintue
// � Settings Menu:

Event OnTypewriterSpeedChanged
    NewSpeed (��������: float)
    ? Get Game Instance
        ? Get Subsystem [DialogueSubsystem]
        ? Set Typewriter Speed (NewSpeed)
```

---

### ������ 3: ������� ������� (�����������)

**��� 1. ���������� ������ �������**:

```
// � WBP_DialogueWidget, ��������:

HistoryPanel (Canvas Panel) [Name: "HistoryPanel"] *Optional*
??? ScrollBox (Name: "HistoryScrollBox")
    ??? HistoryContainer (Vertical Box) [Name: "HistoryContainer"]
```

**��� 2. ���������� ���������� � �������**:

```blueprintue
// � WBP_DialogueWidget, Event Graph:

// ����������:
- HistoryEntries (Array of Text)

// Event: Handle Node Entered (Custom)
Event Handle Node Entered
    Node (��������)
    ? Get Dialogue Text ? Text
    ? Get Speaker Name ? SpeakerName
    ? Format Text ("{SpeakerName}: {Text}") ? FormattedText
    ? Add (HistoryEntries, FormattedText)
    
    // ������� ������ ��� �������
    ? Create Widget [WBP_HistoryEntry]
        ? Set Text (FormattedText)
    ? Add Child to Vertical Box (HistoryContainer, Widget)
    
    // ��������� ����
    ? Scroll to End (HistoryScrollBox)
```

---

### ������ 4: �������� ��������� � ������

**��� 1. ��������� ������ � DialogueNode**:

```cpp
// � Data Asset �������:

Node[0]:
  - Dialogue Text: "� ��� ���� ���� ������!"
  - Speaker Name: "Lianne"
  - Tags: ["Emotion.Happy"]

Node[1]:
  - Dialogue Text: "� ����� ����������..."
  - Speaker Name: "Lianne"
  - Tags: ["Emotion.Sad"]
```

**��� 2. ���������� ����� ������**:

```blueprintue
// � WBP_DialogueWidget, Event Graph:

// Event: Handle Node Entered
Event Handle Node Entered
    Node (��������)
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

### ������ 5: ���������� � ������

**��� 1. ��������� ������ � DialogueNode**:

```cpp
// � Data Asset �������:

Node[0]:
  - Dialogue Text: "������!"
  - Voice Over: VO_Lianne_Greeting
  - Speaker Name: "Lianne"
```

**��� 2. ��������������� ������**:

```blueprintue
// � WBP_DialogueWidget, Event Graph:

// Event: Handle Node Entered
Event Handle Node Entered
    Node (��������)
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

## ����������� �����������

### 1. Rich Text ��������������

**��������� Rich Text Block**:

```blueprintue
// � WBP_DialogueWidget, DialogueText (Rich Text Block):

Details Panel:
  - Text Style Set: DT_DialogueTextStyles (Data Table)

// Data Table DT_DialogueTextStyles (���������: FRichTextStyleRow):
[Bold]
  - Font: Roboto-Bold, Size = 18
  - Color: Yellow

[Italic]
  - Font: Roboto-Italic, Size = 16
  - Color: White

[Red]
  - Color: Red
```

**������������� � �������**:

```cpp
// � Data Asset:
Node[0]:
  - Dialogue Text: "�� <Bold>������</> ����� <Red>��������</>!"
```

---

### 2. �����������

**��������� ����������� ������**:

```cpp
// � DialogueNode:
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content", meta = (MultiLine = "true"))
FText DialogueText;  // FText ������������� ������������ �����������
```

**������������� String Tables**:

```cpp
// �������� String Table:
Content/Localization/DialogueStringTable

// � Data Asset:
Node[0]:
  - Dialogue Text: NSLOCTEXT("Dialogue", "Lianne_Greeting", "������! ��� ����?")
```

---

### 3. ����������� (Accessibility)

**���������� ���������**:

```blueprintue
// � WBP_DialogueWidget, ��������:

SubtitlesPanel (Overlay) [Optional]
??? SubtitlesText (Text Block)
    - Font Size: 20
    - Color: White with Black Outline
    - Justification: Center

// Event: Handle Node Entered
Event Handle Node Entered
    Node (��������)
    ? Get Dialogue Text ? Text
    ? Set Text (SubtitlesText, Text)
    ? Set Visibility (SubtitlesPanel, Visible)
```

**��������� ������������ ���������**:

```blueprintue
// � WBP_DialogueWidget:

Event OnKeyDown (Override)
    Key (��������)
    ? Switch on Key
        Case "Up":
            ? Select Previous Choice Button
        Case "Down":
            ? Select Next Choice Button
        Case "Enter":
            ? Click Selected Choice Button
```

---

## ������ ��������

1. **����������� Bind Widget**: ������ ����������� `BindWidget` � `BindWidgetOptional` ��� ��������������� ���������� ��������.

2. **���������� ������**: ������� ������-������ � C++ (DialogueRunner), � ����������� - � Blueprint (UMG).

3. **������������� ��������**: ����������� UMG Animations ������ Blueprints ��� ���������.

4. **��������� �������**: �� ���������� ������� ������ ���, ��������������� ��.

5. **���������� �� ������ �����������**: ����������� Anchors � Size Box ��� ������������.

6. **������������� �����������**: ���������� ��������, ������������ ��������� � ��������� �������.

7. **����������� Rich Text**: Rich Text Block ��������� ������������� ����� ��� �������� ��������� ��������.

8. **����������� ��� ������**: ����������� FText ������ FString ��� ���� ���������������� �������.

---

## ����������

**DialogueSystemUI** ������������� ������ � ������������� ������� ��� ����������� ��������. ��������� �������� MVVM � event-driven �����������, UI ����� ������������� � ��������� �������� � ����� ���� ��������� �������������� ��� ����� ����� ����.

��� ��������� �������������� ���������� ��.:
- `01_DialogueSystemCore_Module.md` � ���� �������
- `02_DialogueSystemRuntime_Module.md` � Runtime ����������
- `04_DialogueSystemAI_Module.md` � ���������� � AI
- `05_Complete_Examples.md` � ������ ������� ����������
