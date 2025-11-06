# ?? Level Sequencer Integration для Dialogue System

## ?? Quick Start (5 минут)

### 1. Создайте Sequence

```
Content Browser ? Right-click
? Cinematics ? Level Sequence
Name: "LS_MyFirstScene"
```

### 2. Добавьте Camera

```
Sequencer ? + Track ? Camera Actor
Viewport ? Click "Pilot Camera"
Move camera with W/A/S/D + Mouse
Timeline 0:00s ? Click "+" (add keyframe)
```

### 3. Используйте в Dialogue

```
DialogueDataAsset ? Create Event Node:
EffectExpression: "PlaySequence(/Game/Sequences/LS_MyFirstScene)"
```

### 4. Готово!

```
Play (PIE) ? Start Dialogue ? Sequence проигрывается автоматически
```

---

## ?? Документация

- **[Полное руководство](Sequencer_Tutorial_Complete.md)** - Пошаговое создание сцен
- **[Быстрая справка](Sequencer_QuickRef.md)** - Горячие клавиши и частые задачи
- **[Интеграция](Sequencer_Dialogue_Integration.md)** - Связь с Dialogue System
- **[Summary](Sequencer_SUMMARY.md)** - Общий обзор возможностей

---

## ?? C++ API

### UDialogueEffect_PlaySequence

```cpp
// Файл: DialogueSystemCore/Public/Effects/DialogueEffect_PlaySequence.h

UCLASS()
class UDialogueEffect_PlaySequence : public UDialogueEffect
{
    UPROPERTY(EditAnywhere)
    TSoftObjectPtr<ULevelSequence> SequenceAsset;
    
    UPROPERTY(EditAnywhere)
    bool bWaitForCompletion = true;
    
    UPROPERTY(EditAnywhere)
    bool bRestoreCameraAfter = true;
};
```

### Syntax

```
PlaySequence(path)
PlaySequence(path, waitForCompletion)
PlaySequence(path, waitForCompletion, restoreCamera)
```

### Examples

```
"PlaySequence(/Game/Sequences/LS_EllenApproach)"
"PlaySequence(/Game/Sequences/LS_Kiss, true, true)"
"PlaySequence(/Game/Sequences/LS_QuickCut, false)"
```

---

## ?? Use Cases

### Dating Sim - Romantic Scene
```
Sequence: 6 seconds
?? 0-2s: Zoom to NPC face
?? 2-4s: NPC blushes (facial animation)
?? 4-6s: Confession text appears
```

### Visual Novel - Character Arrival
```
Sequence: 8 seconds
?? 0-3s: Door opens (camera pan)
?? 3-6s: Character walks in
?? 6-8s: Camera zoom to face
```

### RPG - Critical Choice
```
Sequence: 5 seconds
?? 0-2s: Wide shot (all characters visible)
?? 2-3s: Camera split
?? 3-5s: Pause for player choice
```

---

## ? Benefits

- ? **No Code Required** - Полностью визуальные инструменты
- ? **Real-time Preview** - Instant feedback
- ? **AAA Quality** - Профессиональные результаты
- ? **Fast Iteration** - Быстрые изменения
- ? **Artist-Friendly** - Для художников/дизайнеров
- ? **Industry Standard** - Используют все AAA-студии

---

## ??? Installation

### Dependencies

```csharp
// DialogueSystemCore.Build.cs

PublicDependencyModuleNames.AddRange(new string[] {
    "LevelSequence",
    "MovieScene"
});
```

### Files Added

```
Plugins/DialogueSystem/Source/DialogueSystemCore/
?? Public/Effects/DialogueEffect_PlaySequence.h
?? Private/Effects/DialogueEffect_PlaySequence.cpp
?? Updated: DialogueEffectParser.h/cpp
```

---

## ?? Learn More

**Official Unreal Docs:**
- [Sequencer Overview](https://docs.unrealengine.com/5.6/sequencer-overview)
- [Cinematic Creation](https://dev.epicgames.com/community/learning)

**Video Tutorials:**
- "UE5 Sequencer Quick Start" (YouTube)
- "Dialogue Cinematics Tutorial" (YouTube)

---

## ?? Support

**Questions?** Check:
1. `Sequencer_QuickRef.md` - Common issues
2. `Sequencer_Tutorial_Complete.md` - Step-by-step guide
3. `Sequencer_Dialogue_Integration.md` - Advanced usage

---

## ?? Result

**You can now create AAA-quality cinematic dialogues WITHOUT CODE!** ??

---

**Version:** 1.12  
**Author:** Dialogue System Team  
**License:** MIT
