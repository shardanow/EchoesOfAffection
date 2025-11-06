# ?? SEQUENCER QUICK REFERENCE

## ? БЫСТРЫЙ СТАРТ (5 МИНУТ)

1. **Создать Sequence:**  `Right-click ? Cinematics ? Level Sequence`
2. **Добавить Camera:**   `+ Track ? Camera Actor`
3. **Pilot Camera:**       `Select Camera ? Viewport ? Pilot Camera button`
4. **Move Camera:**         `W/A/S/D + Mouse`
5. **Add Keyframe:**        `Transform ? +` на timeline
6. **Add Actor:**           `+ Track ? Actor to Sequencer`
7. **Play:**`SPACE` или Play button

---

## ?? ОСНОВНЫЕ ГОРЯЧИЕ КЛАВИШИ

| Действие | Клавиша |
|----------|---------|
| **Play/Pause** | `SPACE` |
| **Frame Forward** | `.` (точка) |
| **Frame Backward** | `,` (запятая) |
| **Go to Start** | `Home` |
| **Go to End** | `End` |
| **Add Keyframe** | `Enter` |
| **Toggle Pilot Camera** | `Ctrl + Shift + P` |

---

## ?? CAMERA CONTROL (В PILOT MODE)

| Действие | Управление |
|----------|------------|
| **Move Forward/Back** | `W` / `S` |
| **Move Left/Right** | `A` / `D` |
| **Move Up/Down** | `E` / `Q` |
| **Rotate Camera** | `Right Mouse + Drag` |
| **Pan View** | `Middle Mouse + Drag` |
| **Zoom** | `Mouse Wheel` |

---

## ?? ТИПИЧНЫЕ SHOTS

### Medium Shot (разговор)
```
Distance: 200cm
Height: +60cm (eye level)
Angle: 25° (3/4 view)
FOV: 70°
```

### Close-Up (эмоции)
```
Distance: 120cm
Height: +10cm
Angle: 15°
FOV: 60°
```

### Over-Shoulder (диалог)
```
Distance: 180cm
Height: +15cm
Angle: 45° (behind shoulder)
FOV: 65°
```

### Wide Shot (все видимы)
```
Distance: 300cm
Height: +50cm
Angle: 0° (side view)
FOV: 80°
```

---

## ?? ЧАСТЫЕ ЗАДАЧИ

### Добавить Actor в Sequence
```
+ Track ? Actor to Sequencer ? Select from list
```

### Переместить Actor
```
Select Actor Track ? + ? Transform
Timeline: Set to desired time
World: Move actor
Transform: Click "+"
```

### Добавить Animation
```
Actor Track ? + ? Animation
Right-click ? Add Animation ? Browse
```

### Добавить Audio
```
+ Track ? Audio Track
Drag & Drop: .wav file from Content Browser
```

### Instant Camera Cut
```
+ Track ? Camera Cuts Track
Drag Camera onto track at desired time
```

---

## ?? TROUBLESHOOTING

### Камера не двигается
- ? Check: Pilot Camera mode enabled
- ? Check: Transform track exists
- ? Check: Keyframes added

### Actor не двигается в Sequence
- ? Check: Actor is "Possessable" (NOT Spawnable)
- ? Check: Transform track added
- ? Check: Keyframes on timeline

### После Sequence actor вернулся на старое место
- ?? Expected behavior для Spawnable
- ? Solution: Use "Possessable" вместо Spawnable

### Анимация не проигрывается
- ? Check: Animation track added
- ? Check: Animation asset assigned
- ? Check: Duration stretched on timeline

### Нет звука
- ? Check: Audio Track added
- ? Check: Volume > 0
- ? Check: Audio file valid (.wav)

---

## ?? WORKFLOW ДЛЯ DATING SIM

### Простой диалог (2 персонажа)
```
1. Medium shot на NPC (0-2s)
2. Over-shoulder от Player на NPC (2-4s)
3. Close-up на NPC (emotion) (4-6s)
```

### Появление третьего персонажа
```
1. Medium shot на NPC1 (0-2s)
2. Pan к Player (2-3s)
3. Reveal NPC2 (Wide shot) (3-5s)
4. Close-up на NPC2 (reaction) (5-6s)
```

### Критический выбор (choice scene)
```
1. Wide shot (все видимы)
2. Pause на timeline
3. Choice UI появляется
```

---

## ?? DIALOGUE INTEGRATION

### В DialogueDataAsset:
```
Node: my_cinematic_node
?? NodeType: Event
?? EffectExpression: "PlaySequence(/Game/Sequences/LS_MySequence)"
```

### Поддерживаемые параметры:
```
PlaySequence(path)
PlaySequence(path, waitForCompletion)
PlaySequence(path, waitForCompletion, restoreCamera)

Examples:
"PlaySequence(/Game/Sequences/LS_EllenApproach)"
"PlaySequence(/Game/Sequences/LS_Kiss, true, true)"
```

---

## ?? PRO TIPS

1. **Используйте Possessable** для постоянных NPC
2. **Добавляйте Camera Cuts** для instant transitions
3. **Настраивайте Blend Curves** для smooth movement
4. **Используйте Folders** для организации треков
5. **Preview часто** (SPACE) во время работы
6. **Сохраняйте регулярно** (Ctrl + S)
7. **Называйте Sequences понятно**: `LS_SceneName_ShotNumber`

---

## ?? CHECKLIST ПЕРЕД ПУБЛИКАЦИЕЙ

- [ ] Все keyframes добавлены
- [ ] Camera smooth transitions настроены
- [ ] Animations assigned и stretched
- [ ] Audio добавлено и volume правильный
- [ ] Duration sequence правильная
- [ ] Preview работает корректно
- [ ] Integrated в DialogueDataAsset
- [ ] Tested в игре (PIE)

---

## ?? СТАНДАРТНЫЕ TIMING

| Тип Sequence | Длительность |
|--------------|--------------|
| **Простая реплика** | 2-4s |
| **Эмоциональная сцена** | 4-8s |
| **Появление NPC** | 5-10s |
| **Критический выбор** | 3-6s |
| **Кульминация** | 10-15s |

---

## ?? ОПТИМИЗАЦИЯ

### Performance Tips:
- ? Keep sequences short (<15s)
- ? Preload sequences before dialogue
- ? Use LODs для distant actors
- ? Limit simultaneous sequences (1-2 max)

### Memory Tips:
- ? Unload sequences after use
- ? Use Level Streaming для больших сцен
- ? Compress audio (Ogg Vorbis)

---

## ?? ДОПОЛНИТЕЛЬНЫЕ МАТЕРИАЛЫ

**Полное руководство:**
- `Sequencer_Tutorial_Complete.md`

**Официальная документация:**
- [Sequencer Overview](https://docs.unrealengine.com/5.6/sequencer-overview)

**YouTube:**
- "UE5 Sequencer Quick Start"
- "Dating Sim Cinematics Tutorial"

---

**ПОМНИТЕ:** Sequencer - это мощнейший инструмент для создания кинематографичных сцен БЕЗ КОДА! ??

---

## ?? **ИТОГОВЫЙ ОТВЕТ: ЧТО МОЖНО ЧЕРЕЗ DA**

### ? **ЧТО РЕАЛЬНО МОЖНО (БЕЗ КОДА):**

---

## ?? **КРИТИЧЕСКИ ВАЖНО: POSSESSABLE vs SPAWNABLE**

### **Если акторы динамические (гуляют, перемещаются):**

**ПРОБЛЕМА:**
```
Level: Lia at (X=500, Y=300) // Гуляет
Sequencer: Lia at (X=100, Y=100) // Для диалога
? Possessable ГЛОБАЛЬНО изменит позицию Lia!
```

**РЕШЕНИЕ 1: SPAWNABLE (рекомендую)**
```
Sequencer ? + Track ? Actor ? "Spawnable"
? Временная копия
? Оригинал не трогается
? Идеально для Dating Sim
```

**РЕШЕНИЕ 2: POSSESSABLE + RESTORE**
```
PlaySequence(path, true, true, true)
?
         restore actors
? Позиции восстанавливаются после
? Тот же актор (state сохраняется)
```

**?? Подробнее:** `Sequencer_Possessable_vs_Spawnable.md`
