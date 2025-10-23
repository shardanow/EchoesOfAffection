# ��������� UI/UX ������� ��������

## �����

������� �������� ���� ����������� �������� � ����� ������ ����������������� ����� � ����������� �������. ����� ���������� � ������� ������������ ����� ������������� � ��������������� ��������� ��� �������.

## �������� ���������

### 1. DialogueWidget - ������� ������ ��������

#### ����� �������:

**������� ��������**
- ScrollBox ��� ����������� ���������� ������
- �������������� ��������� � ���������� ���������
- ����������� ���������� ����������� ������� (�� ��������� 20)
- ��������� ���������� ��� ������ ������ � NPC

**Typewriter Effect**
- ������� ��������� ������ ������ �� ��������
- ������������� �������� (CharactersPerSecond)
- �������� ������������� typewriter
- ����������� ���������� �������� (SkipTypewriter)
- ��������� ������ ������

**��������**
- IntroAnimation - ��������� ���� �������
- OutroAnimation - ������������ ����
- NewNodeAnimation - ������� � ����� �������
- ChoiceAppearAnimation - ��������� ��������� ������
- ContinueBlinkAnimation - �������� ��������� �����������
- TypingAnimation - �������� ���������

**����� �������**
- TypewriterSound - ���� ������ ��������
- ChoiceHoverSound - ���� ��� ��������� �� �����
- ChoiceSelectSound - ���� ������ �����
- DialogueOpenSound - ���� �������� �������
- DialogueCloseSound - ���� �������� �������

**���������� ���������**
- TypingIndicator - ���������� ����� ����� ����������
- ContinueIndicator - ���������� ����� ����� ������ ��� �����������
- SkipHintText - ��������� � ����������� ����������

#### ������������� � Blueprint:

```cpp
// �������� � Runner
DialogueWidget->BindToRunner(DialogueRunner);

// ���������� �������� typewriter
DialogueWidget->SkipTypewriter();

// ���������� ������
DialogueWidget->ContinueDialogue();

// �������� �������
DialogueWidget->ClearHistory();
```

### 2. ChoiceWidget - ������ ������

#### ����� �������:

**���������� �������**
- Hover overlay � ������� ���������
- ��������� ������ ���������:
  - ConsequenceIcon - ������ ����������� ������
  - LockedIcon - ����������� ��������
  - WarningIcon - ������� ������
- Hotkey display (1, 2, 3...)

**���������������**
- ��������� ������������� ����� (1-9 keys)
- Gamepad support (����� Blueprint)
- Hover sound effects
- Click animations
- ������� ��������� � ��������� ����� ��������

**��������**
- AppearAnimation - ��������� ������
- HoverAnimation - ������ ��� ���������
- UnhoverAnimation - ������ ��� �������� �������
- ClickAnimation - ������ ��� �����
- DisabledAnimation - ��� ����������� ���������

#### �������������:

```cpp
// ���������� ������ ������
ChoiceWidget->SetChoiceData(DialogueNode, ChoiceIndex);

// ������������� �������� ��������� � ���������
ChoiceWidget->PlayAppearAnimation(0.2f);

// ���������� ���� hover
ChoiceWidget->SetHoverSound(HoverSound);

// ��������� �����������
bool bAvailable = ChoiceWidget->IsAvailable();
```

### 3. DialogueHistoryEntryWidget - ������ � �������

���������� ������ ��� ����������� ����� ������� � ������� �������.

#### �������:
- ������� ���������� (���������)
- ��� ���������
- ����� �������
- ������������ ��������� �����
- ��������� ���������� ��� ������ � NPC
- Fade-in �������� ���������

#### �������������:

```cpp
// ������� ������ �������
HistoryEntry->SetHistoryData(
    SpeakerName,
    DialogueText,
    PortraitTexture,
    bIsPlayerChoice
);

// ���������� ��������� �����
HistoryEntry->SetTimestamp(FText::FromString("10:45"));
```

### 4. DialogueEmotionWidget - ������������ ������

��������� ������ ��� ����������� �������������� ��������� ���������.

#### �������:
- ��������� Gameplay Tags ��� ������
- ������ ��� ������ ������
- �������� ��������� (Happy=Yellow, Sad=Blue, Angry=Red, etc.)
- Glow ������� ������ ������
- ������� �������� ����� ��������
- Pulse animation ��� �������� ������
- ��������� Material Instance Dynamic

#### �������������:

```cpp
// ���������� ������
EmotionWidget->SetEmotion(FGameplayTag::RequestGameplayTag("Emotion.Happy"), true);

// �������� ������
EmotionWidget->ClearEmotion(true);

// ��������� ������� ������
FGameplayTag CurrentEmotion = EmotionWidget->GetCurrentEmotion();
```

## ������������ �� ������� UI

### �������� �����

**�������� �����:**
- Background: ������ �������������� ���� (#1A1A1A, 85% opacity)
- Text: ����� (#FFFFFF) ��� ������-����� (#E0E0E0)
- Accent: ����� ����� ��� ������ ��������� (#FFD700 ��� highlighted)
- Borders: ���������� ������� ��� �������� ���������

**������:**
- Happy/Joy: Yellow (#FFD700)
- Sad/Depressed: Blue (#4A90E2)
- Angry/Rage: Red (#E74C3C)
- Fear/Scared: Purple (#9B59B6)
- Surprised/Shock: Orange (#FF8C00)
- Neutral: Gray (#808080)

### �������� � ��������

**��������:**
- Typewriter: 40 ��������/������� (������������)
- Fade in/out: 0.3 �������
- Hover effects: 0.15 �������
- Choice appear delay: 0.1 ������� ����� ��������

**Easing:**
- In animations: EaseOut
- Out animations: EaseIn
- Hover effects: EaseInOut
- Typewriter: Linear

### ������� � �������

**Dialogue Box:**
- Min Width: 600px
- Max Width: 1200px
- Padding: 20px
- Border Radius: 10px

**Portrait:**
- Size: 128x128 (main), 64x64 (history)
- Border: 2px solid � glow
- Margin: 10px

**Choice Buttons:**
- Min Height: 60px
- Padding: 15px horizontal, 10px vertical
- Spacing: 8px ����� ��������
- Border Radius: 5px

### �������� ������

**Typewriter:**
- �������� ���� (50ms)
- ������ ���� ��� ������ ����������
- ��������: ������ 50ms

**UI Sounds:**
- Hover: ������ ����� (100ms)
- Click: ������ ���� (150ms)
- Open/Close: ���� ������ (300ms)

## Blueprint Events

��� ������� ����� Blueprint-����������� ������� ��� ������������:

### DialogueWidget:
- OnNodeChanged
- OnEmotionChanged
- OnTypewriterCompleted

### ChoiceWidget:
- OnChoiceInitialized
- OnChoiceHovered
- OnChoiceUnhovered
- OnChoiceClicked

### DialogueEmotionWidget:
- OnEmotionChanged
- OnEmotionCleared
- OnTransitionStarted
- OnTransitionCompleted

### DialogueHistoryEntryWidget:
- OnHistoryEntryInitialized

## ������ ������ ���������

```cpp
// � GameMode ��� Player Controller

void AMyPlayerController::SetupDialogue()
{
    // ������� ������ �������
    UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(this, DialogueWidgetClass);
    
    // ��������� typewriter
    DialogueWidget->bEnableTypewriter = true;
    DialogueWidget->CharactersPerSecond = 40.0f;
    DialogueWidget->TypewriterSound = TypewriterSound;

    // ��������� �������
    DialogueWidget->bShowHistory = true;
    DialogueWidget->MaxHistoryEntries = 20;
    
    // ��������� �����
 DialogueWidget->ChoiceHoverSound = HoverSound;
    DialogueWidget->ChoiceSelectSound = SelectSound;
    DialogueWidget->DialogueOpenSound = OpenSound;
    DialogueWidget->DialogueCloseSound = CloseSound;
    
    // ��������� ��������
    DialogueWidget->bEnableAnimations = true;
    
    // ��������� � Runner
  DialogueWidget->BindToRunner(DialogueRunner);
    
    // �������� � viewport
    DialogueWidget->AddToViewport();
}
```

## �������������� ����������� ��� ����������

1. **Rich Text Formatting** - ��������� BBCode � ������� ��������
2. **Voice Over Integration** - ������������� � ����� �������
3. **Dynamic Camera** - �������������� ����������� ������
4. **Particle Effects** - ������� ��� ������ � ������ ��������
5. **Branching Visualization** - ����� ��������� ����� �������
6. **Choice Timer** - ����������� ������� �� �����
7. **Quick Time Events** - ����-���� � ��������
8. **Reputation Indicators** - ����� ��������� ���������
9. **Achievement Hints** - ��������� � ������� �����������
10. **Accessibility Options** - ����� ��� ����� � ������������� �������������

## ������������

������������� �������������� ��������� ��������:

1. **������� ������** - ������� typewriter, ������� �����
2. **��������� ������** - ������ ��������� ���� ������
3. **�������** - ��������� ����� ������� ������
4. **������** - ������� ����� ������������� ���������
5. **����������** - ��������� ������ � ����������
6. **�������** - ������ ���������� � ��������
7. **����������� ������** - �������������� � ���������������� ����������
8. **�����** - ���������� ������ ���� �������� ��������

## ������������������

**�����������:**
- Lazy loading ��� ������� ��������
- Object pooling ��� choice widgets
- Minimized redraws �� ����� typewriter
- ����������� �������� ���������
- LOD ��� �������� �� ������ ������

**�������:**
- FPS impact: < 2 FPS � ������ ������
- Memory overhead: ~5MB ��� ������� �� 20 �������
- Load time: < 100ms ��� �������� �������

## ����������

����� ������� �������� ������������ �������, ������������� ���������������� ���� � ���������� ������������� �����. ��������� ����������� ��������� ����� ��������� ���������������� � ������������ ��� ���������� ����� �������.
