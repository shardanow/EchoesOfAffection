# Пример создания Blueprint виджетов

## WBP_DialogueMain - Главный виджет диалога

### Структура виджета (Hierarchy):

```
CanvasPanel (Root)
??? RootOverlay (Overlay) - необязательно
??? DialogueBoxBorder (Border) - главный контейнер
?   ??? MainContainer (Vertical Box)
?       ??? TopSection (Horizontal Box)
?       ?   ??? SpeakerPortrait (Image) [BINDING REQUIRED]
?       ?   ??? RightSide (Vertical Box)
?   ?     ??? SpeakerName (TextBlock) [BINDING REQUIRED]
?       ?       ??? EmotionIcon (Image) [OPTIONAL]
?       ??? DialogueContent (Horizontal Box)
?       ?   ??? DialogueText (RichTextBlock) [BINDING REQUIRED]
?    ?   ??? TypingIndicator (Image) [OPTIONAL]
?       ??? BottomSection (Horizontal Box)
?       ?   ??? ContinueIndicator (Image) [OPTIONAL]
?       ?   ??? SkipHintText (TextBlock) [OPTIONAL]
?       ??? ChoicesContainer (Vertical Box) [BINDING REQUIRED]
??? HistoryScrollBox (ScrollBox) [OPTIONAL]
?   ??? (Dynamic History Entries)
```

### Настройка виджета:

1. **Parent Class**: DialogueWidget (C++)

2. **Widget Bindings** (Important: exact names!):
   - SpeakerPortrait (Image)
   - SpeakerName (TextBlock)
   - DialogueText (RichTextBlock)
   - ChoicesContainer (Vertical Box)
   
3. **Optional Bindings**:
   - HistoryScrollBox (ScrollBox)
   - EmotionIcon (Image)
   - ContinueIndicator (Image)
   - TypingIndicator (Image)
   - SkipHintText (TextBlock)

4. **Настройки в Details Panel**:
   
   **UI Settings:**
   - Choice Widget Class: WBP_DialogueChoice
   - History Entry Widget Class: WBP_DialogueHistoryEntry
   - Show Portrait: true
   - Show Emotions: true
   - Show History: true
 - Max History Entries: 20
   
   **Typewriter Settings:**
   - Enable Typewriter: true
   - Characters Per Second: 40.0
   - Enable Typewriter Sound: true
   - Typewriter Sound: [выбрать звук]
   - Typewriter Sound Interval: 0.05
   
   **Audio Settings:**
   - Choice Hover Sound: [выбрать звук]
   - Choice Select Sound: [выбрать звук]
   - Dialogue Open Sound: [выбрать звук]
   - Dialogue Close Sound: [выбрать звук]
   
   **Animation Settings:**
   - Enable Animations: true

### Создание анимаций:

1. **IntroAnimation** (0.3s):
   - DialogueBoxBorder: Scale 0.8 -> 1.0 (EaseOut)
   - DialogueBoxBorder: Opacity 0 -> 1.0 (Linear)

2. **OutroAnimation** (0.3s):
   - DialogueBoxBorder: Scale 1.0 -> 0.8 (EaseIn)
   - DialogueBoxBorder: Opacity 1.0 -> 0 (Linear)

3. **NewNodeAnimation** (0.2s):
   - MainContainer: Translation Y +20 -> 0 (EaseOut)
   - MainContainer: Opacity 0.5 -> 1.0 (Linear)

4. **ContinueBlinkAnimation** (1.0s, Loop):
   - ContinueIndicator: Opacity 1.0 -> 0.3 -> 1.0 (Sine wave)

5. **TypingAnimation** (0.5s, Loop):
   - TypingIndicator: Rotation 0 -> 360 (Linear)
   - TypingIndicator: Scale 1.0 -> 1.2 -> 1.0 (Sine)

### Styling (Appearance):

**DialogueBoxBorder:**
- Background Color: #1A1A1A (R:0.1, G:0.1, B:0.1, A:0.85)
- Border Color: #FFD700 (R:1.0, G:0.84, B:0.0, A:1.0)
- Border Width: 2.0
- Padding: 20, 20, 20, 20
- Shadow Offset: 0, 5
- Shadow Color: Black (A:0.5)

**SpeakerPortrait:**
- Size: 128x128
- Border Color: #FFD700
- Border Width: 2
- Tint: White (A:1.0)

**SpeakerName:**
- Font: Bold, Size 24
- Color: #FFD700
- Shadow: Enabled
- Justification: Left

**DialogueText:**
- Font: Regular, Size 18
- Color: White
- Line Height Percentage: 1.2
- Justification: Left
- Auto Wrap: true

**ContinueIndicator:**
- Size: 32x32
- Tint: #FFD700
- Render Opacity: 1.0

---

## WBP_DialogueChoice - Виджет выбора

### Структура:

```
Button (ChoiceButton) [BINDING REQUIRED]
??? Border (ChoiceBorder)
    ??? Overlay
        ??? HoverOverlay (Image) [OPTIONAL]
        ??? Content (Horizontal Box)
          ??? HotkeyText (TextBlock) [OPTIONAL]
            ??? ChoiceText (RichTextBlock) [BINDING REQUIRED]
    ??? Icons (Horizontal Box)
              ??? ConsequenceIcon (Image) [OPTIONAL]
       ??? LockedIcon (Image) [OPTIONAL]
    ??? WarningIcon (Image) [OPTIONAL]
```

### Настройка:

1. **Parent Class**: ChoiceWidget (C++)

2. **Required Bindings**:
   - ChoiceButton (Button)
 - ChoiceText (RichTextBlock)

3. **Optional Bindings**:
   - ChoiceBorder (Border)
   - HoverOverlay (Image)
   - HotkeyText (TextBlock)
   - ConsequenceIcon (Image)
   - LockedIcon (Image)
   - WarningIcon (Image)

4. **Settings**:
   - Enable Hover Effects: true
   - Enable Animations: true
   - Show Hotkeys: true

### Анимации:

1. **AppearAnimation** (0.2s):
   - Root: Translation X -50 -> 0 (EaseOut)
   - Root: Opacity 0 -> 1.0 (Linear)

2. **HoverAnimation** (0.15s):
   - ChoiceBorder: Scale 1.0 -> 1.05 (EaseOut)
   - HoverOverlay: Opacity 0 -> 0.3 (Linear)

3. **UnhoverAnimation** (0.15s):
   - ChoiceBorder: Scale 1.05 -> 1.0 (EaseIn)
   - HoverOverlay: Opacity 0.3 -> 0 (Linear)

4. **ClickAnimation** (0.1s):
   - Root: Scale 1.0 -> 0.95 -> 1.0 (EaseInOut)

5. **DisabledAnimation** (0.3s):
   - Root: Color Tint -> Gray (Linear)
   - Root: Opacity -> 0.5 (Linear)

### Styling:

**ChoiceButton:**
- Normal: Transparent
- Hovered: Transparent (handled by HoverOverlay)
- Pressed: Transparent

**ChoiceBorder:**
- Background: #2A2A2A (R:0.16, G:0.16, B:0.16, A:0.9)
- Border Color: #505050
- Border Width: 1.0
- Padding: 15, 10, 15, 10

**HoverOverlay:**
- Color: #FFD700 (A:0.0 by default)
- Blend Mode: Additive

**ChoiceText:**
- Font: Regular, Size 16
- Color: White
- Auto Wrap: true

**HotkeyText:**
- Font: Bold, Size 20
- Color: #FFD700
- Margin: 0, 0, 10, 0

**Icons:**
- Size: 24x24
- Spacing: 5px
- ConsequenceIcon: Yellow tint
- LockedIcon: Red tint
- WarningIcon: Orange tint

---

## WBP_DialogueHistoryEntry - Запись истории

### Структура:

```
Border (EntryBorder)
??? Horizontal Box
    ??? SpeakerPortrait (Image) [OPTIONAL]
    ??? Content (Vertical Box)
        ??? SpeakerName (TextBlock) [BINDING REQUIRED]
 ??? Bottom (Horizontal Box)
      ??? DialogueText (RichTextBlock) [BINDING REQUIRED]
         ??? Timestamp (TextBlock) [OPTIONAL]
```

### Настройка:

1. **Parent Class**: DialogueHistoryEntryWidget (C++)

2. **Bindings**:
   - SpeakerName (TextBlock)
   - DialogueText (RichTextBlock)
   - SpeakerPortrait (Image) [Optional]
   - Timestamp (TextBlock) [Optional]
   - EntryBorder (Border) [Optional]

### Анимации:

1. **FadeInAnimation** (0.3s):
   - Root: Opacity 0 -> 1.0 (Linear)
   - Root: Translation Y -10 -> 0 (EaseOut)

### Styling:

**EntryBorder:**
- Background: #1A1A1A (A:0.5)
- Padding: 10, 8, 10, 8
- Margin: 0, 0, 0, 5

**SpeakerPortrait:**
- Size: 48x48
- Margin: 0, 0, 10, 0

**SpeakerName:**
- Font: Bold, Size 14
- Color: #FFD700

**DialogueText:**
- Font: Regular, Size 12
- Color: #CCCCCC
- Auto Wrap: true

**Timestamp:**
- Font: Italic, Size 10
- Color: #808080
- Margin: 5, 0, 0, 0

---

## WBP_DialogueEmotion - Виджет эмоций

### Структура:

```
Overlay
??? EmotionGlow (Image) [OPTIONAL]
??? EmotionIcon (Image) [BINDING REQUIRED]
```

### Настройка:

1. **Parent Class**: DialogueEmotionWidget (C++)

2. **Bindings**:
   - EmotionIcon (Image)
   - EmotionGlow (Image) [Optional]

3. **Settings**:
   - Emotion Icons: [Map Gameplay Tags to Textures]
   - Emotion Colors: [Map Gameplay Tags to Colors]
   - Transition Duration: 0.3
   - Enable Animations: true
   - Enable Glow: true

### Анимации:

1. **AppearAnimation** (0.3s):
   - Root: Scale 0 -> 1.0 (EaseOut, Bounce)
   - Root: Opacity 0 -> 1.0 (Linear)

2. **DisappearAnimation** (0.2s):
   - Root: Scale 1.0 -> 0 (EaseIn)
   - Root: Opacity 1.0 -> 0 (Linear)

3. **EmotionChangeAnimation** (0.3s):
   - EmotionIcon: Rotation 0 -> 360 (EaseInOut)
   - EmotionIcon: Scale 1.0 -> 1.3 -> 1.0 (EaseInOut)

4. **PulseAnimation** (1.5s, Loop):
   - EmotionGlow: Scale 1.0 -> 1.2 -> 1.0 (Sine)
   - EmotionGlow: Opacity 0.5 -> 1.0 -> 0.5 (Sine)

### Styling:

**EmotionIcon:**
- Size: 64x64
- Render Transform Pivot: 0.5, 0.5

**EmotionGlow:**
- Size: 80x80
- Blend Mode: Additive
- Opacity: 0.5

---

## Blueprint Event Implementations

### В WBP_DialogueMain:

```blueprint
// Event Graph

Event OnNodeChanged(Node)
??? Play Sound at Location (Node Voice)
??? Set Camera Focus (Node Speaker)
??? Update Background (Node Location)

Event OnEmotionChanged(Emotion Tag)
??? Spawn Particle Effect (Emotion Particles)
??? Trigger Camera Shake (Emotion Intensity)

Event OnTypewriterCompleted()
??? Play Sound (Completion Sound)
??? Enable Continue Hint
```

### В WBP_DialogueChoice:

```blueprint
Event OnChoiceHovered()
??? Play Hover Animation
??? Update Tooltip (Show Consequences)
??? Highlight Related Choices

Event OnChoiceClicked()
??? Play Click Animation
??? Disable All Choices
??? Send Analytics Event
```

## Использование в игре

```blueprint
// В Player Controller или Game Mode

Event BeginPlay
??? Create Widget (WBP_DialogueMain)
 ??? Add to Viewport (Z-Order: 10)

Function StartDialogue(Dialogue Asset)
??? Get or Create Dialogue Runner
??? Set Dialogue Asset
??? Bind Widget to Runner
??? Run Dialogue
??? Set Input Mode (UI Only)

Function EndDialogue()
??? Unbind Widget from Runner
??? Remove Widget from Viewport
??? Set Input Mode (Game Only)
```

## Советы по оптимизации

1. **Widget Pooling**: Используйте Object Pool для ChoiceWidgets
2. **Lazy Loading**: Загружайте портреты асинхронно
3. **LOD**: Отключайте анимации на слабом железе
4. **Batch Updates**: Обновляйте UI пакетами, а не каждый кадр
5. **Visibility**: Используйте Collapsed вместо Hidden для скрытых элементов

## Дополнительные ресурсы

- Примеры звуков: `Content/Sounds/UI/Dialogue/`
- Примеры текстур: `Content/Textures/UI/Dialogue/`
- Примеры материалов: `Content/Materials/UI/Dialogue/`
- Примеры анимаций: `Content/Animations/UI/Dialogue/`
