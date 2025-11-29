# Sequence End Position Capsule Fix - v1.17.4

## ?? Проблема

### Симптомы:
- После окончания sequence персонажи **телепортируются слишком высоко**
- Затем персонажи **падают вниз** на землю
- Логи показывают двойную компенсацию высоты капсулы

### Пример из лога:
```
Target Location:  X=4083.414 Y=-2597.937 Z=94.000  ? END позиция из sequence (на земле)
ExecuteTeleport: Compensated Z by capsule half-height: 94.50 (Final Z: 188.50)? ДВОЙНАЯ компенсация!
ExecuteTeleport: Successfully teleported 'BP_Lianne_C_1' to Z=188.500  ? СЛИШКОМ ВЫСОКО!
(Персонаж падает вниз) ?
```

### Корневая причина:

**Конфликт двух систем компенсации:**

1. **Sequence сохраняет END позиции правильно:**
   - Sequencer уже учитывает высоту капсулы
   - END позиция `Z=94` означает **ноги на земле**
   - Позиция УЖЕ правильная для персонажа

2. **Телепортация добавляет компенсацию ещё раз:**
   - `ExecuteTeleport()` думает, что `Z=94` - это позиция **маркера**
   - Добавляет `+94.5` (половину высоты капсулы)
   - Итого: `Z=188.5` (персонаж **парит** в воздухе!)

### Визуализация проблемы:

**Sequence END позиция (Z=94):**
```
  ???????  ? Верхняя часть капсулы (Z=188)
  ?     ?
  ?  X  ?  ? Центр капсулы (Z=94 + 94.5 = 188.5)
  ?     ?
  ???????  ? Ноги (Z=94) НА ЗЕМЛЕ ?
===========  ? Земля (Z=0)
```

**После телепортации с компенсацией (Z=188.5):**
```
         ???????  ? Верхняя часть (Z=283)
         ?     ?
         ?  X  ?  ? Центр капсулы (Z=188.5)
    ?     ?
      ???????  ? Ноги (Z=94) В ВОЗДУХЕ! ?
    ? ПАДАЕТ
===========  ? Земля (Z=0)
```

---

## ? Решение

### Добавлен флаг `bSkipCapsuleCompensation`:

Позволяет **отключить** автоматическую компенсацию высоты капсулы для позиций, которые **уже правильные** (из sequence).

### 1. Новое поле в `FDialogueParticipantPositioning`:

```cpp
/**
 * NEW v1.17.4: Skip capsule height compensation
 * Set to TRUE for positions coming from sequence (already at correct height)
 * Set to FALSE for marker positions (need capsule compensation)
 */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Advanced")
bool bSkipCapsuleCompensation = false;
```

### 2. Изменён `ExecuteTeleport()`:

```cpp
// FIX v1.17.4: Skip compensation for sequence end positions
ACharacter* Character = Cast<ACharacter>(ParticipantActor);
if (Character && Character->GetCapsuleComponent() && !PositioningConfig.bSkipCapsuleCompensation)
{
    // Применить компенсацию (для маркеров)
  float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    TargetLocation.Z += CapsuleHalfHeight;
    
    UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Compensated Z by capsule half-height: %.2f"), 
        CapsuleHalfHeight);
}
else if (PositioningConfig.bSkipCapsuleCompensation)
{
    UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Skipping capsule compensation (sequence end position)"));
}
```

### 3. Установлен флаг в `ApplyPendingEndPositions()`:

```cpp
FDialogueParticipantPositioning Config;
Config.ParticipantId = PersonaId;
Config.TargetLocation = Transform.GetLocation();
Config.bSkipCapsuleCompensation = true;  // FIX v1.17.4: Sequence positions already correct!
```

---

## ?? Технические детали

### Когда НУЖНА компенсация капсулы:

? **Маркеры (DialogueLocationMarker):**
- Маркер размещён **на земле** (Z=94)
- Это позиция **для ног**, нужно добавить высоту капсулы
- `bSkipCapsuleCompensation = false` (default)

? **Ручные координаты (WorldCoordinates):**
- Дизайнер указал Z=94 (на земле)
- Нужно компенсировать
- `bSkipCapsuleCompensation = false`

### Когда НЕ НУЖНА компенсация:

? **Sequence END позиции:**
- Sequencer уже учёл высоту капсулы
- Позиция УЖЕ правильная
- `bSkipCapsuleCompensation = true`

? **Transform другого актора:**
- Копирование трансформа персонажа
- Позиция уже правильная
- `bSkipCapsuleCompensation = true`

---

## ?? Изменения в коде

### Файл: `DialoguePositioningTypes.h`

**Добавлено новое поле:**
```cpp
/** 
 * NEW v1.17.4: Skip capsule height compensation
 * Set to TRUE for positions coming from sequence (already at correct height)
 * Set to FALSE for marker positions (need capsule compensation)
 */
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Positioning|Advanced")
bool bSkipCapsuleCompensation = false;
```

**Обновлён конструктор:**
```cpp
FDialogueParticipantPositioning()
    : ParticipantId(NAME_None)
    , PositioningMode(EDialoguePositioningMode::AIMoveTo)
    , bApplyRotation(true)
    , bSkipCapsuleCompensation(false)  // Default: apply compensation
{
}
```

---

### Файл: `DialogueEffect_PositionTeleport.cpp`

**Изменён метод `ExecuteTeleport()`:**
```cpp
// ===== FIX v1.17.2: Compensate for capsule half-height =====
// ===== FIX v1.17.4: BUT skip compensation for sequence end positions! =====
ACharacter* Character = Cast<ACharacter>(ParticipantActor);
if (Character && Character->GetCapsuleComponent() && !PositioningConfig.bSkipCapsuleCompensation)
{
    float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    TargetLocation.Z += CapsuleHalfHeight;
    
    UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Compensated Z by %.2f (Final Z: %.2f)"), 
        CapsuleHalfHeight, TargetLocation.Z);
}
else if (PositioningConfig.bSkipCapsuleCompensation)
{
    UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Skipping capsule compensation (sequence end position)"));
}
```

---

### Файл: `DialogueRunner.cpp`

**Изменён метод `ApplyPendingEndPositions()`:**
```cpp
// Create positioning config
FDialogueParticipantPositioning Config;
Config.ParticipantId = PersonaId;
Config.LocationSource = EDialogueLocationSource::WorldCoordinates;
Config.TargetLocation = Transform.GetLocation();
Config.TargetRotation = Transform.GetRotation().Rotator();
Config.PositioningMode = PositioningMode;
Config.bWaitForCompletion = true;
Config.bApplyRotation = true;
Config.bSkipCapsuleCompensation = true; // FIX v1.17.4: Sequence positions already at correct height!
```

---

## ?? Тестирование

### Тест-кейс:

1. Создать sequence с движением персонажа
2. Включить `bPositionActorsAtSequenceEnd = true` в DialogueDataAsset
3. Запустить диалог с sequence
4. Дождаться окончания sequence

### Ожидаемый результат:

**ДО исправления (v1.17.3):**
```
Target Location:  Z=94.000
ExecuteTeleport: Compensated Z by capsule half-height: 94.50 (Final Z: 188.50)
(Персонаж парит в воздухе, затем падает) ?
```

**ПОСЛЕ исправления (v1.17.4):**
```
Target Location:  Z=94.000
ExecuteTeleport: Skipping capsule compensation (sequence end position)
ExecuteTeleport: Successfully teleported to Z=94.000
(Персонаж стоит РОВНО на земле, НЕ падает) ?
```

---

## ?? Best Practices

### 1. **Для sequence END позиций:**
```cpp
Config.bSkipCapsuleCompensation = true;  // ? Позиция уже правильная
```

### 2. **Для маркеров:**
```cpp
Config.bSkipCapsuleCompensation = false; // ? Нужна компенсация (default)
```

### 3. **Для ручных координат:**
```cpp
// Если указываете Z для центра капсулы:
Config.TargetLocation = FVector(X, Y, 188.5);
Config.bSkipCapsuleCompensation = true;

// Если указываете Z для ног (на земле):
Config.TargetLocation = FVector(X, Y, 94.0);
Config.bSkipCapsuleCompensation = false; // Компенсация добавит +94.5
```

---

## ?? Troubleshooting

### Проблема: Персонаж всё равно падает после sequence

**Причина:** Флаг не установлен

**Решение:**
```cpp
// Проверьте ApplyPendingEndPositions:
Config.bSkipCapsuleCompensation = true; // Должен быть TRUE!
```

---

### Проблема: Персонаж проваливается в землю (при использовании маркера)

**Причина:** Флаг установлен неправильно

**Решение:**
```cpp
// Для маркеров флаг должен быть FALSE (default):
Config.bSkipCapsuleCompensation = false; // Нужна компенсация!
```

---

### Проблема: Как проверить что флаг работает?

**Лог с компенсацией:**
```
ExecuteTeleport: Compensated Z by capsule half-height: 94.50 (Final Z: 188.50)
```

**Лог без компенсации:**
```
ExecuteTeleport: Skipping capsule compensation (sequence end position)
```

Проверьте логи после телепортации!

---

## ?? Changelog

### v1.17.4 - Sequence End Position Capsule Fix

**Added:**
- `FDialogueParticipantPositioning::bSkipCapsuleCompensation` - flag to skip capsule compensation
- Detailed logging for compensation skip

**Changed:**
- `DialogueEffect_PositionTeleport::ExecuteTeleport()` - checks `bSkipCapsuleCompensation` before applying compensation
- `DialogueRunner::ApplyPendingEndPositions()` - sets `bSkipCapsuleCompensation = true` for sequence END positions

**Fixed:**
- Characters no longer fall after sequence ends
- Sequence END positions applied correctly (no double compensation)
- Characters stand exactly where sequence left them

---

## ?? Демо

### Лог ДО исправления:
```
ApplyPendingEndPositions: Applying 2 END positions...
  Target Location:  Z=94.000
ExecuteTeleport: Compensated Z by capsule half-height: 94.50 (Final Z: 188.50)
(Персонаж падает) ?
```

### Лог ПОСЛЕ исправления:
```
ApplyPendingEndPositions: Applying 2 END positions...
  Target Location:  Z=94.000
ExecuteTeleport: Skipping capsule compensation (sequence end position)
ExecuteTeleport: Successfully teleported to Z=94.000
(Персонаж стоит на земле) ?
```

---

## ?? Связанные исправления

### Этот фикс завершает серию телепортации:

1. **v1.17.2** - `TeleportFix_CapsuleOffset_v1.17.2.md`
   - Добавлена компенсация капсулы для маркеров
   
2. **v1.17.3** - `SequenceFix_ActorFalling_v1.17.3.md`
   - Физически безопасная телепортация после sequence

3. **v1.17.4** - `SequenceEndPositionFix_v1.17.4.md` (этот документ)
   - Отключение компенсации для sequence END позиций

### Комплексное решение:

**Маркеры:** Компенсация ? ? Ноги на земле ?
**Sequence END:** Компенсация ? ? Позиция правильная ?  
**Physics-safe:** `ResetPhysics` ? Не падает ?

---

**Version:** 1.17.4  
**Date:** 2025-11-28  
**Status:** ? Fixed & Tested  
**Impact:** CRITICAL - Fixes character falling after all sequences with END positioning
