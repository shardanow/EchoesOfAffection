# ?? ПОЛНОЕ РУКОВОДСТВО: LEVEL SEQUENCER ДЛЯ ДИАЛОГОВ

## ?? **СОДЕРЖАНИЕ**

1. [Подготовка уровня](#1-подготовка-уровня)
2. [Создание Level Sequence](#2-создание-level-sequence)
3. [Настройка камеры](#3-настройка-камеры)
4. [Движение персонажей](#4-движение-персонажей)
5. [Анимации](#5-анимации)
6. [Аудио](#6-аудио)
7. [Интеграция с Dialogue System](#7-интеграция-с-dialogue-system)
8. [Практический пример: "Ellen Подходит"](#8-практический-пример)

---

## 1. ПОДГОТОВКА УРОВНЯ

### 1.1 Разместить акторов в уровне

```
World Outliner ? Place Actors:
?? BP_Lia (Location: X=100, Y=100, Z=100)
?? BP_Ellen (Location: X=-500, Y=200, Z=100) // Off-screen
?? BP_Player (контроллируется игроком)
```

**Важно:** Акторы должны уже существовать в уровне ПЕРЕД созданием Sequence!

---

## 2. СОЗДАНИЕ LEVEL SEQUENCE

### 2.1 Создать папку для Sequences

```
Content Browser ? Right-click Content folder
? New Folder ? Name: "Sequences"
   ?? Subfolder: "Shots"
```

### 2.2 Создать Sequence

```
Right-click в Sequences/Shots/
? Cinematics ? Level Sequence
Name: "LS_EllenApproach"
```

### 2.3 Открыть Sequencer

```
Double-click LS_EllenApproach.uasset
? Sequencer window opens
```

**Окно Sequencer:**
```
??????????????????????????????????????????????
? [+ Track] [Play] [Timeline]       ?
??????????????????????????????????????????????
? Tracks:         Duration: 0:06 ?
? (пусто)        ?
? ?
? Timeline: 0???1???2???3???4???5???6        ?
??????????????????????????????????????????????
```

---

## 3. НАСТРОЙКА КАМЕРЫ

### 3.1 Добавить Camera Actor

```
Sequencer ? + Track ? Camera Actor
? Automatically creates "CineCameraActor" in level
```

### 3.2 Pilot Camera Mode (ручное управление)

```
1. Select CineCameraActor in Sequencer
2. Viewport ? Click "Pilot Camera" button (camera icon)
3. Viewport switches to camera view
```

### 3.3 Позиционировать камеру визуально

```
In Viewport (Pilot Mode):
?? W/A/S/D ? Move camera
?? Right Mouse + Drag ? Rotate camera
?? Scroll Wheel ? Move forward/backward
?? Q/E ? Move up/down
?? Mouse Wheel + Drag ? Pan view
```

### 3.4 Добавить Keyframe

```
Timeline: Set to 0:00s
Sequencer ? CineCameraActor ? Transform ? Click "+"
? ?? Keyframe added!
```

### 3.5 Создать следующий Shot

```
Timeline: Move to 0:02s
Viewport: Reposition camera (new angle)
Transform ? Click "+" (add keyframe)
? Sequencer automatically creates smooth blend!
```

**Результат:**
```
Timeline:
0:00s ? ???smooth interpolation???> 0:02s ?
      Camera Position 1              Camera Position 2
```

### 3.6 Настроить FOV (опционально)

```
CineCameraActor ? Current Camera Settings
?? Filmback ? Sensor Width (mm)
?? Lens ? Focal Length (mm) ? This controls FOV!
?? Or: Field of View (degrees)
```

**Добавить FOV Keyframes:**
```
0:00s: FOV = 70° (Medium shot)
0:04s: FOV = 75° (Wide shot)
0:05s: FOV = 60° (Close-up)
```

---

## 4. ДВИЖЕНИЕ ПЕРСОНАЖЕЙ

### 4.1 Добавить Actor как Possessable

```
Sequencer ? + Track ? Actor to Sequencer
Search: "Ellen"
Select: BP_Ellen_C from World Outliner
? Ellen добавлена в Sequencer
```

**ВАЖНО: Используйте "Possessable", НЕ "Spawnable"!**
- **Possessable** = работает с реальным актором в уровне
- **Spawnable** = создаёт временного актора

### 4.2 Добавить Transform Track

```
BP_Ellen_C ? Click "+" ? Transform
```

### 4.3 Keyframe START Position

```
Timeline: 0:00s

1. Select Ellen в World Outliner
2. В Details Panel ? Transform:
   Location: (X=-500, Y=200, Z=100) // Off-screen
   Rotation: (0, 0, 0)
   
3. Sequencer ? BP_Ellen_C ? Transform ? Click "+"
```

### 4.4 Keyframe END Position

```
Timeline: 0:05s

1. Details Panel ? Transform:
   Location: (X=150, Y=250, Z=100) // Next to Lia
   Rotation: (0, 45, 0) // Turned towards Lia
   
2. Transform ? Click "+"
```

**Sequencer автоматически:**
- ? Интерполирует движение
- ? Ellen плавно двигается из точки A в точку B
- ? Поворачивается во время движения

---

## 5. АНИМАЦИИ

### 5.1 Добавить Animation Track

```
BP_Ellen_C ? Click "+" ? Animation
```

### 5.2 Добавить Walk Animation

```
Timeline: Position at 0:00s

1. Right-click on Animation track
2. Add Animation
3. Browse: Content/Characters/Animations/Walk_Forward
4. Select animation
5. Stretch to 0:00s - 0:05s (5 seconds)
```

### 5.3 Добавить Idle Animation

```
Timeline: Position at 0:05s

1. Add Animation ? Idle_CrossArms
2. Duration: 0:05s - 0:06s (1 second)
```

**Sequencer автоматически blend между анимациями!**

---

## 6. АУДИО

### 6.1 Добавить Audio Track

```
Sequencer ? + Track ? Audio Track
Name: "Master Audio"
```

### 6.2 Drag & Drop аудио файлы

```
Content Browser ? найти Footsteps_Grass.wav
Drag ? drop на Audio Track
Timeline: 0:00s - 0:05s
```

### 6.3 Настроить Volume

```
Right-click на Audio clip ? Properties
?? Volume Multiplier: 0.5
?? Loop: true (для footsteps)
?? Fade In Time: 0.0s
```

### 6.4 Добавить Music

```
New Audio Track: "Music"
Add: Tension_Rising.wav
Timeline: 0:02s
Properties:
?? Volume: 0.3 (background)
?? Fade In: 1.0s
?? Fade Out: 0.5s
```

---

## 7. ИНТЕГРАЦИЯ С DIALOGUE SYSTEM

### 7.1 Использование в Dialogue Node

**DialogueDataAsset:**
```cpp
Node: ellen_arrives (Event Node)
?? NodeType: Event
?? Text: "[За спиной слышны шаги...]"
?? EffectExpression: "PlaySequence(/Game/Sequences/Shots/LS_EllenApproach)"
```

**Что происходит:**
1. Dialogue достигает узла `ellen_arrives`
2. Effect `PlaySequence` запускает Sequence
3. Sequence проигрывается (Ellen двигается, камера переключается)
4. После окончания Sequence диалог продолжается

---

## 8. ПРАКТИЧЕСКИЙ ПРИМЕР: "ELLEN ПОДХОДИТ"

### ?? Checklist

**Подготовка (5 мин):**
- [x] BP_Lia размещена в уровне (X=100, Y=100, Z=100)
- [x] BP_Ellen размещена в уровне (X=-500, Y=200, Z=100)
- [x] Создана папка Content/Sequences/Shots/

**Создание Sequence (30 мин):**
- [x] Создан LS_EllenApproach.uasset
- [x] Добавлена CineCameraActor
- [x] Добавлена BP_Ellen как Possessable
- [x] Добавлена BP_Lia как Possessable

**Shot 1: Lia Medium (0:00-0:02s):**
- [x] Camera позиция: (Distance=200, Angle=25°, Height=160)
- [x] Camera FOV: 70°
- [x] Transform Keyframe на 0:00s

**Shot 2: Over-shoulder (0:02-0:04s):**
- [x] Camera позиция: Behind Lia's shoulder
- [x] Transform Keyframe на 0:02s

**Shot 3: Reveal Ellen (0:04-0:05s):**
- [x] Camera позиция: Wide shot
- [x] FOV: 75°
- [x] Transform Keyframe на 0:04s

**Shot 4: Zoom Ellen (0:05-0:06s):**
- [x] Camera позиция: Close to Ellen
- [x] FOV: 60°
- [x] Transform Keyframe на 0:05s

**Ellen Movement:**
- [x] 0:00s Keyframe: (X=-500, Y=200, Z=100) Off-screen
- [x] 0:05s Keyframe: (X=150, Y=250, Z=100) Next to Lia

**Animations:**
- [x] 0:00-0:05s: Walk_Forward
- [x] 0:05-0:06s: Idle_CrossArms

**Audio:**
- [x] 0:00-0:05s: Footsteps_Grass.wav (loop, volume=0.5)
- [x] 0:02-0:06s: Tension_Rising.wav (fade in, volume=0.3)
- [x] 0:05s: Cloth_Movement.wav

---

## 9. ИНТЕГРАЦИЯ В DIALOGUE NODE

### 9.1 В DialogueDataAsset

```
Content/Dialogue/DA_AwkwardMeeting.uasset

Node: ellen_arrives
?? NodeId: "ellen_arrives"
?? NodeType: Event
?? Text: "[За спиной слышны шаги...]"
?? EffectExpression: "PlaySequence(/Game/Sequences/Shots/LS_EllenApproach)"
```

### 9.2 Тестирование

1. Запустить игру (PIE)
2. Начать диалог с Lia
3. Диалог достигает узла `ellen_arrives`
4. Sequence автоматически проигрывается
5. Ellen двигается, камера переключается
6. После Sequence диалог продолжается

---

## ?? ГОТОВО!

Теперь у вас есть полностью функциональная кинематографичная сцена **БЕЗ КОДА**!

---

## ?? TIPS & TRICKS

### Blend Curves (улучшение плавности)

```
Timeline: Click between two keyframes
? Opens Curve Editor

Adjust curve type:
?? Auto (default)
?? Linear (прямая линия)
?? Constant (instant cut)
?? Cubic (smooth ease)
?? User (custom curve)
```

### Camera Cuts (мгновенные переходы)

Для instant cuts между камерами:

```
+ Track ? Camera Cuts Track

Add shots:
?? 0:00-0:02s: CineCameraActor_1
?? 0:02-0:04s: CineCameraActor_2
?? 0:04-0:06s: CineCameraActor_3
```

### Preview в реальном времени

```
Sequencer ? Press SPACE
или
Click Play button ?
```

---

## ?? ПРОИЗВОДИТЕЛЬНОСТЬ

**Sequencer очень эффективен:**
- ? No runtime overhead (pre-baked)
- ? Async loading sequences
- ? Level streaming friendly
- ? Optimized interpolation

**Dating Sim оптимизация:**
- Sequences обычно короткие (3-10 сек)
- Можно preload перед диалогом
- Minimal memory footprint

---

## ?? ДОПОЛНИТЕЛЬНЫЕ РЕСУРСЫ

**Официальные туториалы:**
- [Sequencer Overview](https://docs.unrealengine.com/5.6/sequencer-overview)
- [Cinematic Creation](https://dev.epicgames.com/community/learning)

**YouTube плейлисты:**
- "UE5 Sequencer Tutorial for Beginners"
- "Dialogue Sequences in Unreal Engine"

---

## ? ИТОГ

**ВЫ МОЖЕТЕ:**
- ? Создавать кинематографичные сцены БЕЗ КОДА
- ? Управлять камерой визуально
- ? Двигать персонажей в реальном времени
- ? Добавлять анимации drag-and-drop
- ? Интегрировать с Dialogue System
- ? Preview в реальном времени
- ? Профессиональное качество

**ЭТО СТАНДАРТ AAA-ИНДУСТРИИ!** ??
