# ?? Sequence-Based Automatic Positioning v1.16.8 - Посібник користувача

## ?? Що це таке?

**v1.16.8** додає **автоматичну екстракцію позицій** акторів з LevelSequence! 

Тепер не потрібно вручну налаштовувати позиції в DialogueNode - система автоматично витягує їх з sequence keyframes.

---

## ?? Переваги

### ? ДО (ручне налаштування):
```
DialogueNode:
  ?? Positioning.bEnablePositioning = true
  ?? Positioning.ParticipantPositions[0]
  ?   ?? PersonaId = "Lianne"
  ?   ?? TargetLocation = (1188, -3819, 27)  ? ВРУЧНУ!
  ?   ?? ...
  ?? Effects[0] = PlaySequence(LS_MySequence)
```

### ? ПІСЛЯ (автоматично):
```
DialogueNode:
  ?? Positioning.bEnablePositioning = false  ? ВИМКНУТО!
  ?? Effects[0] = PlaySequence(LS_MySequence)
       ?
  Система АВТОМАТИЧНО витягує позиції з sequence!
```

---

## ?? Як використовувати

### Варіант 1: Автоматична екстракція (РЕКОМЕНДОВАНО)

1. **Створи LevelSequence** з позиціями акторів:
   - Відкрий Sequencer
   - Додай Possessable actors (Lianne, Alice, тощо)
   - Встанови keyframes для Transform
   - Збережи sequence

2. **В DialogueNode:**
   - `Positioning.bEnablePositioning = false` ? **Вимкни ручне позиціонування!**
   - Додай `DialogueEffect_PlaySequence` в Effects
   - Вкажи шлях до sequence

3. **Запусти діалог:**
   - Система автоматично витягне позиції з sequence
   - Логи покажуть екстракцію:
   ```
   LogSequencePositioning: Extracted 2 start positions, 2 end positions
   ExtractPositionsFromSequence: Start Position[Lianne]: Loc=(X, Y, Z)
   ```

### Варіант 2: Ручне налаштування (fallback)

Якщо `Positioning.bEnablePositioning = true`, система використовує ручні налаштування як раніше.

---

## ?? Як працює система

```cpp
// В DialogueRunner::ProcessNode()
if (!Node->Positioning.bEnablePositioning)
{
    // Автоматична екстракція з sequence
    ExtractPositionsFromSequence(Node);
}
else
{
    // Ручне позиціонування
    ExecuteNodePositioning(Node);
}
```

### Алгоритм екстракції:

1. **Знаходить PlaySequence effect** в DialogueNode
2. **Створює FSequencePositioningExtractor** для цього sequence
3. **Витягує Transform для кожного Possessable:**
   - Start positions (frame 0)
   - End positions (last frame)
4. **Логує результати** для debug purposes

---

## ?? Логи та дебаг

### Включи логи:

```ini
; DefaultEngine.ini
[Core.Log]
LogSequencePositioning=Verbose
LogDialogueRunner=Verbose
LogDialoguePositioning=Verbose
```

### Що шукати в Output Log:

? **Успішна екстракція:**
```
LogSequencePositioning: Extracted transform for 'Lianne' at time 0.00s: Loc=(1188, -3819, 27)
ExtractPositionsFromSequence: Extracted 2 start positions, 2 end positions from sequence 'LS_LianneGreeting'
```

? **Помилки:**
```
LogSequencePositioning: Possessable 'Alice' not found in sequence 'LS_MySequence'
LogSequencePositioning: Track has no sections!
```

---

## ?? Приклад використання: lianne_greeting

### 1. Sequence Setup (LS_LianneGreeting)

```
Frame 0:
  Lianne: Loc=(1233, -6274, 96) Rot=(0, 0, 0)
  Alice:  Loc=(948, -4237, 96) Rot=(0, 0, 0)

Frame 120:
  Lianne: Loc=(3989, -2147, 27) Rot=(0, 45, 0)
  Alice:Loc=(3264, -2662, 27) Rot=(0, -30, 0)
```

### 2. DialogueNode Setup

```cpp
Node: node_greeting
  ?? Positioning.bEnablePositioning = false  ? АВТОМАТИЧНО!
  ?? Effects[0] = PlaySequence(/Game/Sequences/LS_LianneGreeting)
```

### 3. Що відбувається:

1. Діалог запускає node_greeting
2. `ProcessNode()` викликає `ExtractPositionsFromSequence()`
3. Система знаходить `PlaySequence` effect
4. Створює `FSequencePositioningExtractor(LS_LianneGreeting)`
5. Витягує позиції:
   - Start: Lianne=(1233, -6274, 96), Alice=(948, -4237, 96)
   - End: Lianne=(3989, -2147, 27), Alice=(3264, -2662, 27)
6. **Поки що тільки логує** (v1.16.8 = logging-only)

---

## ?? API Reference

### FSequencePositioningExtractor

```cpp
// Create extractor
FSequencePositioningExtractor Extractor(MySequence);

// Get transform at specific time
FTransform Transform;
bool bSuccess = Extractor.GetActorTransformAtTime(
    FName("Lianne"),  // PersonaId
    0.0f,    // Time in seconds
    Transform         // Output
);

// Get all start/end positions
TMap<FName, FTransform> StartPositions = Extractor.GetStartPositions();
TMap<FName, FTransform> EndPositions = Extractor.GetEndPositions();

// Get sequence duration
float Duration = Extractor.GetSequenceDuration();
```

### DialogueRunner Integration

```cpp
// Automatic extraction (called internally)
void UDialogueRunner::ExtractPositionsFromSequence(UDialogueNode* Node);

// Manual positioning (existing)
void UDialogueRunner::ExecuteNodePositioning(UDialogueNode* Node);
```

---

## ?? Налаштування

### В DialogueNode (Blueprint/DataAsset):

| Property | Value | Опис |
|----------|-------|------|
| `Positioning.bEnablePositioning` | `false` | Використовує sequence positions |
| `Positioning.bEnablePositioning` | `true` | Використовує ручні positions |

---

## ?? Обмеження поточної версії (v1.16.8)

### ?? Logging-Only Implementation

**Поточна версія тільки ЛОГУЄ позиції**, не застосовує їх автоматично!

**Чому?** Потрібно прийняти дизайн рішення:

#### Option A: Dynamic Effect Creation
```cpp
// Автоматично створювати PositionAIMoveTo effects
for (auto& Pair : StartPositions)
{
    UDialogueEffect_PositionAIMoveTo* Effect = NewObject<...>();
    Effect->TargetLocation = Pair.Value.GetLocation();
    Effect->Execute(Context);
}
```
? Простіше у використанні  
? Більше runtime overhead

#### Option B: Query-Based Approach
```cpp
// Modify DialogueEffect_PositionAIMoveTo
if (Sequence)
{
    FSequencePositioningExtractor Extractor(Sequence);
    Extractor.GetActorTransformAtTime(PersonaId, Time, OutTransform);
}
```
? Більш гнучкий  
? Потрібна зміна в effect

---

## ?? Troubleshooting

### Проблема: "Possessable 'Alice' not found"

**Рішення:** Перевір що:
1. Actor доданий як **Possessable** (не Spawnable!)
2. Ім'я в sequence співпадає з PersonaId
3. Actor існує в рівні під час діалогу

### Проблема: "Track has no sections"

**Рішення:**
1. Відкрий sequence в Sequencer
2. Переконайся що є **Transform Track** для актора
3. Додай хоча б 2 keyframes (start + end)

### Проблема: "No positions extracted"

**Рішення:**
1. Перевір `Positioning.bEnablePositioning = false`
2. Переконайся що є `PlaySequence` effect в node
3. Дивись Output Log (LogSequencePositioning)

---

## ?? Наступні кроки

1. **Тестуй logging** - переконайся що позиції екстрактуються правильно
2. **Вибери підхід** - Dynamic vs Query-Based
3. **Імплементуй auto-positioning** - застосування позицій автоматично
4. **Документуй best practices** - як структурувати sequences

---

## ?? Додаткова документація

- `DialoguePositioning_v1.15_Guide.md` - Повний гайд по позиціонуванню
- `Sequencer_Tutorial_Complete.md` - Sequencer integration tutorial
- `Sequencer_Possessable_vs_Spawnable.md` - CRITICAL: Actor handling

---

**Version:** 1.16.8  
**Date:** 2025-01-06  
**Status:** ? Compiled & Ready for Testing  
**Next:** Choose auto-positioning approach & implement

---

## ?? Швидкий старт

```bash
# 1. Compile проект
Build.bat EchoesOfAffectionEditor Win64 Development

# 2. Запусти редактор
UnrealEditor.exe EchoesOfAffection.uproject

# 3. Play діалог lianne_greeting

# 4. Дивись Output Log:
Window ? Developer Tools ? Output Log
Filter: "LogSequencePositioning"
```

**Очікуваний результат:**
```
LogSequencePositioning: Extracted 2 start positions, 2 end positions
ExtractPositionsFromSequence: Start Position[Lianne]: Loc=(...)
ExtractPositionsFromSequence: End Position[Lianne]: Loc=(...)
```

? Якщо бачиш ці логи - **система працює!** ??
