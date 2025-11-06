# ?? DIALOGUE SYSTEM + SEQUENCER INTEGRATION GUIDE

## ?? ОБЗОР

Этот документ описывает **полную интеграцию** между DialogueSystem и Level Sequencer для создания кинематографичных диалогов.

---

## ?? АРХИТЕКТУРА

```
DIALOGUE SYSTEM            LEVEL SEQUENCER
?? Logic & Choices    ??    ?? Visual Presentation
?? Conditions & Effects       ?? Camera Work
?? Memory & Relationships          ?? Actor Movement
?? Save/Load        ?? Animations
?? Progression?? Audio

INTEGRATION POINT:
DialogueNode (Event) ? Effect: PlaySequence ? Sequence Plays ? Dialogue Continues
```

---

## ?? QUICK START

### 1. Создать Sequence

```
Content/Sequences/Shots/LS_MyScene.uasset
Duration: 6 seconds
```

### 2. Настроить Sequence

- Add Camera
- Add NPCs (Possessable)
- Set keyframes
- Add animations
- Add audio

### 3. Добавить в Dialogue Node

```
DialogueDataAsset: DA_MyDialogue

Node: cinematic_moment
?? NodeType: Event
?? Text: "[Драматичная сцена разворачивается...]"
?? EffectExpression: "PlaySequence(/Game/Sequences/Shots/LS_MyScene)"
```

### 4. Test

- Start Dialogue
- Reach node `cinematic_moment`
- Sequence plays automatically
- Dialogue continues after

---

## ?? C++ ИНТЕГРАЦИЯ

### UDialogueEffect_PlaySequence

**Файл:** `DialogueSystemCore/Public/Effects/DialogueEffect_PlaySequence.h`

```cpp
UCLASS(BlueprintType, meta = (DisplayName = "Play Sequence"))
class UDialogueEffect_PlaySequence : public UDialogueEffect
{
    GENERATED_BODY()

public:
    /** Level Sequence asset to play */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    TSoftObjectPtr<ULevelSequence> SequenceAsset;

    /** Wait for sequence to complete */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    bool bWaitForCompletion = true;

    /** Restore player camera after sequence ends */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
bool bRestoreCameraAfter = true;
};
```

### Парсер Effect

**Файл:** `DialogueSystemCore/Private/Effects/DialogueEffectParser.cpp`

```cpp
UDialogueEffect* UDialogueEffectParser::ParsePlaySequence(const TArray<FString>& Args)
{
    if (Args.Num() < 1)
    {
        return nullptr;
 }

    UDialogueEffect_PlaySequence* Effect = NewObject<UDialogueEffect_PlaySequence>();
  Effect->SequenceAsset = TSoftObjectPtr<ULevelSequence>(FSoftObjectPath(Args[0]));
    
    // Optional parameters
    if (Args.Num() > 1) Effect->bWaitForCompletion = Args[1].ToBool();
    if (Args.Num() > 2) Effect->bRestoreCameraAfter = Args[2].ToBool();
    
  return Effect;
}
```

---

## ?? СИНТАКСИС ЭФФЕКТА

### Базовый синтаксис

```
PlaySequence(sequencePath)
```

**Пример:**
```
PlaySequence(/Game/Sequences/LS_EllenApproach)
```

### Полный синтаксис

```
PlaySequence(path, waitForCompletion, restoreCamera)
```

**Примеры:**
```
PlaySequence(/Game/Sequences/LS_Kiss, true, true)
PlaySequence(/Game/Sequences/LS_QuickCut, false, false)
PlaySequence(/Game/Sequences/LS_Reveal, true)
```

### Параметры

| Параметр | Тип | Default | Описание |
|----------|-----|---------|----------|
| `path` | String | Required | Путь к Sequence asset |
| `waitForCompletion` | Bool | `true` | Ждать окончания Sequence |
| `restoreCamera` | Bool | `true` | Восстановить камеру после |

---

## ?? ПРАКТИЧЕСКИЕ СЦЕНАРИИ

### Сценарий 1: Простое появление NPC

**Sequence: LS_NPCArrive**
```
Duration: 5s
?? 0-2s: Camera pan to door
?? 2-4s: NPC walks in
?? 4-5s: Camera zoom to NPC face
```

**Dialogue Node:**
```
Node: npc_arrives
?? Text: "[Дверь открывается...]"
?? Effect: "PlaySequence(/Game/Sequences/LS_NPCArrive)"

Next Node: npc_greeting
?? Speaker: "NewNPC"
?? Text: "Привет! Давно не виделись!"
```

---

### Сценарий 2: Эмоциональная реакция

**Sequence: LS_ShockedReaction**
```
Duration: 3s
?? 0-1s: Quick zoom to NPC face
?? 1-2s: NPC plays "Shocked" animation
?? 2-3s: Camera shake + dramatic music
```

**Dialogue Node:**
```
Node: reveal_secret
?? Speaker: "Player"
?? Text: "Я знаю твой секрет..."
?? Next: npc_shocked

Node: npc_shocked (Event)
?? Text: "[NPC выглядит потрясённым]"
?? Effect: "PlaySequence(/Game/Sequences/LS_ShockedReaction)"
?? Next: npc_responds

Node: npc_responds
?? Speaker: "NPC"
?? Text: "Как... как ты узнал?!"
```

---

### Сценарий 3: Критический выбор с визуализацией

**Sequence: LS_ChoicePresentation**
```
Duration: 4s
?? 0-2s: Wide shot (all characters visible)
?? 2-3s: Camera splits (show both options)
?? 3-4s: Pause for player choice
```

**Dialogue Node:**
```
Node: critical_choice_setup (Event)
?? Text: "[Момент истины...]"
?? Effect: "PlaySequence(/Game/Sequences/LS_ChoicePresentation, true)"
?? Next: critical_choice

Node: critical_choice (PlayerChoice)
?? Choice A: "Поддержать Lia"
?? Choice B: "Поддержать Ellen"
```

---

### Сценарий 4: Кульминация (множественные Sequences)

**Sequences:**
- `LS_Tension_Build` (10s)
- `LS_Decision_Made` (5s)
- `LS_Consequences` (8s)

**Dialogue Flow:**
```
Node: tension_start (Event)
?? Effect: "PlaySequence(/Game/Sequences/LS_Tension_Build)"
?? Next: player_decision

Node: player_decision (PlayerChoice)
?? Choice: "Сознаться"
?  ?? Next: confession_seq
?? Choice: "Солгать"
   ?? Next: lie_seq

Node: confession_seq (Event)
?? Effect: "PlaySequence(/Game/Sequences/LS_Decision_Made)"
?? Next: consequences_good

Node: lie_seq (Event)
?? Effect: "PlaySequence(/Game/Sequences/LS_Decision_Made)"
?? Next: consequences_bad

Node: consequences_good (Event)
?? Effect: "PlaySequence(/Game/Sequences/LS_Consequences)"
?? Next: resolution

Node: consequences_bad (Event)
?? Effect: "PlaySequence(/Game/Sequences/LS_Consequences)"
?? Next: bad_ending
```

---

## ?? BEST PRACTICES

### 1. Naming Convention

```
Content/Sequences/
?? Shots/
?  ?? LS_SceneName_ShotNumber.uasset
?  ?? Example: LS_AwkwardMeeting_01.uasset
?? Master/
   ?? LS_SceneName_Master.uasset
   ?? Example: LS_AwkwardMeeting_Master.uasset
```

### 2. Duration Guidelines

| Тип сцены | Рекомендуемая длина |
|-----------|---------------------|
| Простое действие | 2-4s |
| Появление NPC | 4-6s |
| Эмоциональная реакция | 3-5s |
| Критический момент | 6-10s |
| Кульминация | 10-15s |

### 3. Camera Workflow

```
1. Start: Wide shot (context)
2. Middle: Medium shot (focus on speaker)
3. End: Close-up (emotion)
```

### 4. Audio Layering

```
Audio Tracks:
?? Voice (priority 1)
?? SFX (priority 2)
?? Music (priority 3, low volume)
```

---

## ? ПРОИЗВОДИТЕЛЬНОСТЬ

### Preloading Sequences

**В Dialogue Start:**
```cpp
void UDialogueRunner::StartDialogue(...)
{
    // Preload upcoming sequences
    TArray<FName> SequencesToPreload = {
        "/Game/Sequences/LS_EllenApproach",
        "/Game/Sequences/LS_CriticalChoice"
    };
    
    PreloadSequences(SequencesToPreload);
}
```

### Memory Management

```cpp
void UDialogueEffect_PlaySequence::OnSequenceFinished()
{
    // Cleanup after sequence
    if (SpawnedSequenceActor)
    {
        SpawnedSequenceActor->Destroy();
  SpawnedSequenceActor = nullptr;
    }
    SequencePlayer = nullptr;
}
```

---

## ?? ADVANCED TECHNIQUES

### 1. Dynamic Sequence Selection

**Выбор Sequence на основе Relationship:**

```
Node: dynamic_reaction (Event)
?? Condition: "affinity[Lia] >= 50"
?? Effect True: "PlaySequence(/Game/Sequences/LS_HappyReaction)"
?? Effect False: "PlaySequence(/Game/Sequences/LS_SadReaction)"
```

### 2. Sequence Chains

**Последовательные Sequences:**

```
Node: sequence_chain_1 (Event)
?? Effect: "PlaySequence(/Game/Sequences/LS_Part1)"
?? Next: sequence_chain_2

Node: sequence_chain_2 (Event)
?? Effect: "PlaySequence(/Game/Sequences/LS_Part2)"
?? Next: dialogue_continues
```

### 3. Interactive Sequences

**Sequence с выбором внутри:**

```
Sequence: LS_InteractiveChoice
?? 0-3s: Build tension
?? 3-4s: Pause + Show UI
?? Player makes choice
?? 4-6s: Show consequence (varies by choice)
```

---

## ?? TROUBLESHOOTING

### Issue: Sequence не проигрывается

**Check:**
1. ? Path correct: `/Game/Sequences/...`
2. ? Sequence asset exists
3. ? Effect syntax правильный
4. ? No typos в названии

### Issue: Камера не возвращается

**Solution:**
```
PlaySequence(path, true, true)  // Ensure 3rd param = true
```

### Issue: Dialogue continues во время Sequence

**Solution:**
```
PlaySequence(path, true)  // Ensure 2nd param = true
```

### Issue: Actor не на месте после Sequence

**Check:**
1. ? Actor is Possessable (NOT Spawnable)
2. ? Transform properly keyframed
3. ? Sequence duration правильная

---

## ?? ПРИМЕРЫ ПРОЕКТОВ

### Dating Sim - "Romantic Confession"

```
Sequences:
?? LS_Approach (NPC подходит)
?? LS_EyeContact (взгляды встречаются)
?? LS_Confession (признание)
?? LS_Response (реакция)

Dialogue Flow:
1. player_initiates ? LS_Approach
2. tension_builds ? LS_EyeContact
3. confession_moment ? LS_Confession
4. npc_responds ? LS_Response
```

### Visual Novel - "Mystery Reveal"

```
Sequences:
?? LS_DoorOpens (дверь открывается)
?? LS_ShadowFigure (тень появляется)
?? LS_FaceReveal (лицо видно)
?? LS_ShockedReaction (реакция)

Dialogue Flow:
1. suspense_builds ? LS_DoorOpens
2. narrator_describes ? LS_ShadowFigure
3. reveal_moment ? LS_FaceReveal
4. characters_react ? LS_ShockedReaction
```

---

## ? CHECKLIST

**Перед интеграцией:**
- [ ] Sequence создан и настроен
- [ ] Tested в Sequencer (preview works)
- [ ] Duration оптимальная
- [ ] Audio добавлено
- [ ] Path к Sequence известен

**Интеграция:**
- [ ] Dialogue Node создан (Event type)
- [ ] Effect syntax правильный
- [ ] Path в Effect правильный
- [ ] Parameters настроены (wait, restore)

**Тестирование:**
- [ ] Dialogue запускается
- [ ] Sequence проигрывается
- [ ] Camera returns (если нужно)
- [ ] Actor на правильном месте
- [ ] Audio работает
- [ ] No errors в log

---

## ?? ИТОГ

**ВЫ МОЖЕТЕ ТЕПЕРЬ:**
- ? Создавать кинематографичные диалоги БЕЗ КОДА
- ? Интегрировать Sequencer с Dialogue System
- ? Управлять камерой визуально
- ? Двигать персонажей плавно
- ? Создавать эмоциональные сцены
- ? Профессиональное качество AAA

**ЭТО МОЩНЕЙШИЙ ИНСТРУМЕНТ ДЛЯ DATING SIM!** ??

---

**Следующие шаги:**
1. Создайте первую тестовую Sequence (5 минут)
2. Интегрируйте с Dialogue System (10 минут)
3. Протестируйте в игре (PIE)
4. Создайте библиотеку Sequences для проекта
5. Enjoy professional cinematics! ??
