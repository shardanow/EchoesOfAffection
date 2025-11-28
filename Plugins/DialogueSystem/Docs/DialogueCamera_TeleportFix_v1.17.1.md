# Dialogue Camera Teleportation Fix - v1.17.1

## ?? Проблема

### Симптомы:
- ? Переключение камеры между спикерами работает отлично
- ? После телепортации актора камера **НЕ обновляет** позицию
- ? После проигрывания сиквенса камера **смотрит не туда**

### Корневая причина:

В методе `UpdateCameraAfterPositioning` была **слишком сложная логика** с несколькими проблемами:

#### 1. **Проверка минимальной дельты движения:**
```cpp
// СТАРЫЙ КОД (ПЛОХО):
float LocationDelta = FVector::Dist(CurrentTransform.GetLocation(), NewTransform.GetLocation());
const float MinLocationDelta = 5.0f; // 5cm threshold

if (LocationDelta < MinLocationDelta && RotationDelta < MinRotationDelta)
{
    return; // ? Не обновляет камеру!
}
```

**Проблема:** После телепортации актор перемещается на 200+ см, но камера уже **следовала за ним в реальном времени** через `UpdateCameraTracking()`. Поэтому дельта между **текущей** и **новой** позицией камеры была маленькой (<5см), и обновление **не происходило**.

#### 2. **Использование OriginalPlayerLocation для расчёта:**
```cpp
// СТАРЫЙ КОД (ПЛОХО):
FVector FocusPoint2D = FocusPoint;
FVector OriginalPlayerLocation2D = OriginalPlayerLocation;

FVector NPCToPlayer2D = (OriginalPlayerLocation2D - FocusPoint2D).GetSafeNormal();
```

**Проблема:** Логика рассчитывала позицию камеры относительно **оригинальной** позиции игрока (при старте диалога), а не **текущей**. Это создавало **неправильный** угол обзора после телепортации.

#### 3. **Излишняя интерполяция с малым Alpha:**
```cpp
// СТАРЫЙ КОД (ПЛОХО):
float Alpha = 0.05f; // Очень медленная интерполяция
BlendedTransform.SetLocation(FMath::Lerp(CurrentTransform.GetLocation(), NewTransform.GetLocation(), Alpha));
```

**Проблема:** Камера двигалась **слишком медленно** (5% за кадр), и не успевала "догнать" актора после телепортации.

---

## ? Решение

### Упрощённая логика `UpdateCameraAfterPositioning`:

```cpp
void UDialogueCameraComponent::UpdateCameraAfterPositioning()
{
    // 1. Обновить OriginalPlayerLocation = текущая позиция камеры
    OriginalPlayerLocation = CachedCameraManager->GetCameraLocation();
    
    // 2. Пересчитать NEW camera transform
    FTransform NewCameraTransform = CalculateCameraTransform(CurrentTarget.Get(), Settings);
 
    // 3. Валидация (опционально)
    ValidateCameraPosition(NewCameraTransform, CurrentTarget.Get());
  
    // 4. ВСЕГДА применить новую позицию (без проверок дельты!)
    if (PositioningBlendTime > 0.0f)
    {
        // Плавный blend с EaseInOut
     StartBlending(NewCameraTransform, PositioningBlendTime);
    }
    else
    {
        // Мгновенное обновление
        DialogueCameraActor->SetActorTransform(NewCameraTransform);
    }
}
```

### Ключевые изменения:

1. **Убрана проверка минимальной дельты** - камера **всегда** обновляется
2. **Используется текущая позиция камеры** для расчёта нового фрейминга
3. **Стандартный блендинг** через систему Tick (не custom interpolation)
4. **Логирование** на каждом шаге для отладки

---

## ?? Логи для проверки

### ДО исправления:
```
LogDialogueCamera: OnParticipantPositioned: Current target 'BP_Lianne' moved to ...
LogDialogueCamera:   Scheduled camera update in 0.20 seconds
(ТИШИНА - метод не вызывается или не обновляет камеру)
```

### ПОСЛЕ исправления:
```
LogDialogueCamera: OnParticipantPositioned: Current target 'BP_Lianne' moved to ...
LogDialogueCamera:   Scheduled camera update in 0.20 seconds
(через 0.2 сек...)
LogDialogueCamera: UpdateCameraAfterPositioning: Repositioning camera for 'BP_Lianne'
LogDialogueCamera:   Updated original player location: X=... Y=... Z=...
LogDialogueCamera:   Current camera: X=... Y=... Z=...
LogDialogueCamera:   New camera: X=... Y=... Z=...
LogDialogueCamera:   Blending camera over 0.50 seconds
```

---

## ?? Настройки

Всё управляется через **Details Panel** в `DialogueCameraComponent`:

### **Реакция на телепортацию:**
```cpp
UPROPERTY(EditAnywhere, Category = "Camera|Auto Update")
bool bAutoUpdateOnPositioning = true;  // Включить автообновление

UPROPERTY(EditAnywhere, Category = "Camera|Auto Update")
float PositioningUpdateDelay = 0.2f;   // Задержка перед обновлением

UPROPERTY(EditAnywhere, Category = "Camera|Auto Update")
float PositioningBlendTime = 0.5f;     // Время блендинга (0 = instant)
```

### **Рекомендуемые значения:**

| Сценарий | Delay | Blend Time | Описание |
|----------|-------|------------|----------|
| **Телепорт без анимации** | 0.0s | 0.3s | Быстрое переключение |
| **Телепорт с fade** | 0.2s | 0.5s | Стандартная задержка |
| **После сиквенса** | 0.3s | 0.7s | Даём время на "осмотреться" |
| **Мгновенный рывок** | 0.0s | 0.0s | Debug режим |

---

## ?? Тест-кейс

### 1. **Простая телепортация:**
```
1. Диалог стартует - камера на Lianne
2. Нода с DialogueEffect_PositionTeleport - Lianne телепортируется
3. ? Камера плавно (0.5s) переезжает к новой позиции Lianne
```

### 2. **Сиквенс с движением:**
```
1. Диалог стартует - камера на Alice
2. Нода с DialogueEffect_PlaySequence - Alice двигается в сиквенсе
3. ? После окончания сиквенса камера обновляется через 0.2s
```

### 3. **Множественная телепортация:**
```
1. Камера на Lianne
2. Lianne телепортируется ? камера следует
3. Переключение на Alice ? камера переключается
4. Alice телепортируется ? камера следует
5. ? Все переходы плавные и корректные
```

---

## ?? Troubleshooting

### Проблема: Камера всё равно не обновляется

**Проверь логи:**
```
LogDialogueCamera: OnParticipantPositioned: ... ? (событие получено)
LogDialogueCamera:   Scheduled camera update...  ? (таймер запущен)
LogDialogueCamera: UpdateCameraAfterPositioning: ... ? (метод не вызвался!)
```

**Решение:** Таймер был очищен или World teardown. Проверь:
- `PositioningUpdateTimerHandle.IsValid()` - должен быть true
- `GetWorld()` - не null
- Редактор не в режиме PIE shutdown

---

### Проблема: Камера дёргается

**Причина:** Конфликт между `UpdateCameraTracking` и `UpdateCameraAfterPositioning`

**Решение:** Отключи `bTrackSpeaker` или увеличь `PositioningUpdateDelay`:
```cpp
bTrackSpeaker = false;         // Отключить постоянное отслеживание
PositioningUpdateDelay = 0.3f;    // Больше времени на оседание
```

---

### Проблема: Валидация говорит "target not visible"

**Лог:**
```
LogDialogueCamera: Warning: Camera position validation FAILED - target may not be visible!
```

**Причина:** Dot product < 0.3 (цель за пределами 73° конуса)

**Решение:** Настрой `AngleOffset` и `Distance` в Settings:
```cpp
Settings.AngleOffset = 25.0f;  // Уменьши для более фронтального вида
Settings.Distance = 200.0f;    // Уменьши для более близкого кадра
```

---

## ?? Changelog

### v1.17.1 - Camera Teleportation Fix

**Changed:**
- `UpdateCameraAfterPositioning()` упрощён и исправлен
- Убрана проверка минимальной дельты движения
- Используется текущая позиция камеры вместо оригинальной для расчёта
- Стандартный блендинг через Tick вместо custom interpolation

**Fixed:**
- Камера не обновлялась после телепортации актора
- Камера смотрела не туда после проигрывания сиквенса
- Слишком медленная интерполяция (Alpha=0.05)

**Added:**
- Детальное логирование каждого шага обновления
- Валидация позиции камеры перед применением

---

## ?? Итог

**Было:**
- Сложная логика с проверками дельты
- Неправильное использование OriginalPlayerLocation
- Медленная интерполяция

**Стало:**
- Простая логика: всегда обновляй камеру
- Используй текущую позицию для расчёта
- Стандартный блендинг через систему Tick

**Результат:** Камера корректно следует за актором после телепортации и сиквенса! ???

---

**Version:** 1.17.1  
**Date:** 2025-11-07  
**Status:** ? Fixed & Tested
