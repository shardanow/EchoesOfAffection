# ?? QUICK FIX INSTRUCTIONS - v1.17.1 Camera Teleportation

## ?? Что было исправлено:

### ? **Проблема:**
- Переключение между спикерами ? работает
- После телепортации камера ? НЕ обновляется
- После сиквенса камера ? смотрит не туда

### ? **Решение:**
- Упрощена логика `UpdateCameraAfterPositioning()`
- Убрана проверка минимальной дельты движения
- Добавлены методы `ValidateCameraPosition()` и `IsActorVisibleFromCamera()`
- Улучшено логирование

---

## ?? Шаги для применения исправления:

### 1. **Закрыть Unreal Editor** (если открыт)
```
File ? Exit
```

### 2. **Скомпилировать изменения**
```powershell
& "D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\Build.bat" EchoesOfAffectionEditor Win64 Development "D:\Game Dev\Projects\UEProjects\EchoesOfAffection\EchoesOfAffection.uproject" -waitmutex
```

**Ожидаемый результат:**
```
Building EchoesOfAffectionEditor...
[1/4] Compile [x64] DialogueCameraComponent.cpp
[2/4] Link [x64] UnrealEditor-DialogueSystemRuntime.lib
[3/4] Link [x64] UnrealEditor-DialogueSystemRuntime.dll
[4/4] WriteMetadata

Result: Succeeded
```

### 3. **Запустить Unreal Editor**

### 4. **Протестировать в PIE**

---

## ?? Тест-кейс для проверки:

### **Сценарий 1: Простая телепортация**
```
1. Запустить диалог с Lianne
2. В одной из нод добавить DialogueEffect_PositionTeleport
3. Lianne телепортируется на 200+ см
4. ? Камера должна ПЛАВНО переместиться к новой позиции
```

**Проверка логов:**
```
LogDialogueCamera: OnParticipantPositioned: Current target 'BP_Lianne' moved to ...
LogDialogueCamera:   Scheduled camera update in 0.20 seconds
(через 0.2 сек:)
LogDialogueCamera: UpdateCameraAfterPositioning: Repositioning camera for 'BP_Lianne'
LogDialogueCamera:   Updated original player location: ...
LogDialogueCamera:   Current camera: ...
LogDialogueCamera:   New camera: ...
LogDialogueCamera:   Blending camera over 0.50 seconds
```

### **Сценарий 2: Сиквенс с движением**
```
1. Запустить диалог с Alice
2. В ноде добавить DialogueEffect_PlaySequence
3. В сиквенсе Alice двигается по Possessable треку
4. ? После окончания сиквенса камера обновляется
```

**Проверка логов:**
```
(сиквенс заканчивается)
LogDialogueCamera: OnParticipantPositioned: Current target 'BP_Alice' moved to ...
LogDialogueCamera: UpdateCameraAfterPositioning: Repositioning camera for 'BP_Alice'
LogDialogueCamera:   Blending camera over 0.50 seconds
```

### **Сценарий 3: Переключение спикеров (должно работать как раньше)**
```
1. Диалог: Lianne говорит ? Player отвечает ? Lianne говорит
2. ? Камера плавно переключается между спикерами
```

**Проверка логов:**
```
LogDialogueCamera: OnNodeEntered: Switching camera to new speaker 'BP_Lianne'
LogDialogueCamera: Smoothly transitioning to new target: BP_Lianne over 0.60 seconds
```

---

## ?? Логи для диагностики:

### **Фильтр в Output Log:**
```
LogDialogueCamera
LogDialogueRunner
```

### **Ключевые строки:**

#### ? **Всё работает:**
```
LogDialogueCamera: OnParticipantPositioned: Current target '...' moved to ...
LogDialogueCamera:   Scheduled camera update in 0.20 seconds
LogDialogueCamera: UpdateCameraAfterPositioning: Repositioning camera for '...'
LogDialogueCamera:   Blending camera over 0.50 seconds
```

#### ? **Проблема:**
```
LogDialogueCamera: OnParticipantPositioned: ... ?
LogDialogueCamera:   Scheduled camera update...  ?
(ТИШИНА - UpdateCameraAfterPositioning не вызывается)  ?
```

#### ?? **Предупреждение (не критично):**
```
LogDialogueCamera: Warning: Camera position validation FAILED - target may not be visible!
```
*Это означает что цель за пределами 73° конуса камеры. Не критично, но можно подкрутить AngleOffset.*

---

## ?? Настройки (если нужно подкрутить):

В **DialogueCameraComponent** (Details Panel):

### **Camera | Auto Update:**
```cpp
bAutoUpdateOnPositioning = true   // Включить автообновление
PositioningUpdateDelay = 0.2f     // Задержка перед обновлением (сек)
PositioningBlendTime = 0.5f       // Время блендинга (0 = instant)
```

### **Рекомендации:**
- **Если камера дёргается:** Увеличь `PositioningUpdateDelay` до 0.3-0.5s
- **Если камера медленно догоняет:** Уменьши `PositioningBlendTime` до 0.2-0.3s
- **Если нужен instant snap:** Установи `PositioningBlendTime = 0.0f`

---

## ?? Troubleshooting:

### **Проблема: Камера всё равно не обновляется**

**Проверь:**
1. Открыт ли Output Log (`Window ? Developer Tools ? Output Log`)
2. Видишь ли строку `OnParticipantPositioned`? 
   - Если НЕТ ? событие не эмитируется (проблема в DialogueRunner)
   - Если ДА ? читай дальше
3. Видишь ли строку `Scheduled camera update`?
   - Если НЕТ ? таймер не запустился (проблема в OnParticipantPositioned)
   - Если ДА ? читай дальше
4. Видишь ли строку `UpdateCameraAfterPositioning`?
   - Если НЕТ ? таймер был очищен или World teardown

**Решение:** Перезапусти редактор и попробуй ещё раз.

---

### **Проблема: Warning "target may not be visible"**

**Причина:** Камера смотрит под углом > 73° от цели.

**Решение:** Настрой параметры камеры:
```cpp
// В DialogueDataAsset или через Blueprint:
Settings.AngleOffset = 20.0f;  // Уменьши для более фронтального вида
Settings.Distance = 180.0f;    // Уменьши для более близкого кадра
```

---

### **Проблема: Камера дёргается**

**Причина:** Конфликт между `UpdateCameraTracking` и `UpdateCameraAfterPositioning`.

**Решение:** Отключи постоянное отслеживание:
```cpp
bTrackSpeaker = false;         // В Details Panel
TrackingUpdateRate = 0.0f;     // Или установи в 0
```

---

## ?? Файлы изменены:

1. `DialogueCameraComponent.cpp` - исправлен метод `UpdateCameraAfterPositioning()`
2. `DialogueCameraComponent.cpp` - добавлены методы `ValidateCameraPosition()` и `IsActorVisibleFromCamera()`
3. `DialogueRunner.cpp` - добавлен вызов `EmitNodeEnteredEvent()` в `ProcessNode()`

---

## ?? Документация:

- **Полная документация:** `DialogueCamera_TeleportFix_v1.17.1.md`
- **Quick Start:** `DialogueCamera_QuickStart.md`

---

## ? Checklist после применения:

- [ ] Редактор закрыт
- [ ] Код скомпилирован успешно
- [ ] Редактор запущен
- [ ] Протестирован сценарий 1 (телепортация)
- [ ] Протестирован сценарий 2 (сиквенс)
- [ ] Протестирован сценарий 3 (переключение спикеров)
- [ ] Логи содержат `UpdateCameraAfterPositioning`
- [ ] Камера корректно следует за акторами

---

**Version:** 1.17.1  
**Date:** 2025-11-07  
**Status:** Ready to Test ??
