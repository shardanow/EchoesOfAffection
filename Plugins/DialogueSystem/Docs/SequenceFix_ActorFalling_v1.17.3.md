# Sequence Actor Falling Fix - v1.17.3

## ?? Проблема

### Симптомы:
- После окончания sequence персонажи **"падают" на землю**
- Персонажи выглядят так, будто **внезапно включилась гравитация**
- Визуально персонаж **дёргается** при возврате к оригинальной позиции

### Корневая причина:

После окончания sequence система восстанавливает оригинальные трансформы акторов (если `bRestoreActorTransforms = true`). Это нужно для возврата Possessable акторов в исходное положение.

**Проблема в коде:**
```cpp
// БЫЛО (ПЛОХО):
Actor->SetActorTransform(OriginalTransform);
```

`SetActorTransform()` **НЕ учитывает физику**:
- Не останавливает движение перед телепортацией
- Не сбрасывает velocity и acceleration
- Не использует правильный тип телепорта
- Физика "включается" резко после sequence ? персонаж **падает**

### Почему это происходит?

1. **Во время sequence:**
   - Sequencer управляет позицией актора напрямую
   - Physics временно "игнорируется" (overridden)
   - Gravity не применяется

2. **После sequence:**
   - `SetActorTransform()` восстанавливает позицию
   - Physics резко включается обратно
   - Gravity мгновенно начинает действовать
 - Персонаж **"падает"** на землю

---

## ? Решение

### Физически безопасная телепортация:

Используем те же техники, что и в телепортации персонажей (`DialogueEffect_PositionTeleport`):

```cpp
// СТАЛО (ХОРОШО):
ACharacter* Character = Cast<ACharacter>(Actor);
if (Character && Character->GetCapsuleComponent())
{
    // 1. Останавливаем движение
    if (Character->GetCharacterMovement())
 {
 Character->GetCharacterMovement()->StopMovementImmediately();
        Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    }
    
    // 2. Используем физически безопасную телепортацию
    FVector RestoreLocation = OriginalTransform.GetLocation();
    FRotator RestoreRotation = OriginalTransform.GetRotation().Rotator();
    
    const bool bSweep = false;
    FHitResult HitResult;
    Actor->SetActorLocationAndRotation(
        RestoreLocation,
        RestoreRotation,
   bSweep,
        &HitResult,
        ETeleportType::ResetPhysics // CRITICAL: Сбрасывает физику
    );
}
```

### Ключевые изменения:

1. **StopMovementImmediately()** - останавливает любое движение перед телепортом
2. **SetMovementMode(MOVE_Walking)** - возвращает персонажа в режим ходьбы
3. **ETeleportType::ResetPhysics** - сбрасывает velocity, acceleration, forces
4. **Применяется только для ACharacter** - для других акторов используется обычный `SetActorTransform()`

---

## ?? Технические детали

### Типы телепортации в Unreal Engine:

| Тип | Описание | Когда использовать |
|-----|----------|--------------------|
| `None` | Обычное движение (не телепорт) | Постепенное перемещение |
| `TeleportPhysics` | Сохраняет физический стейт | Движущиеся объекты (снаряды, т.п.) |
| **`ResetPhysics`** | **Сбрасывает физику** | **Телепортация персонажей** ? |

### Что делает `ResetPhysics`:

- Сбрасывает `Velocity` ? 0
- Сбрасывает `Acceleration` ? 0
- Очищает все активные физические силы
- Предотвращает "инерцию" после телепорта
- **Предотвращает "падение"** после sequence

### Почему `StopMovementImmediately()`?

CharacterMovementComponent может иметь:
- Pending movement (запланированное движение)
- Active navigation path (активный маршрут)
- Velocity from animation (velocity из анимации)

`StopMovementImmediately()` очищает **ВСЁ** это перед телепортом.

---

## ?? Изменения в коде

### Файл: `DialogueEffect_PlaySequence.cpp`

**1. Добавлены includes:**
```cpp
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h" // FIX v1.17.3
#include "Components/CapsuleComponent.h" // FIX v1.17.3
```

**2. Исправлен метод `CleanupSequence()`:**
```cpp
if (bShouldRestore)
{
    const FTransform& OriginalTransform = Pair.Value;
    
    // ===== FIX v1.17.3: Physics-safe transform restoration =====
    ACharacter* Character = Cast<ACharacter>(Actor);
    if (Character && Character->GetCapsuleComponent())
    {
        // Stop any movement before restoration
        if (Character->GetCharacterMovement())
        {
       Character->GetCharacterMovement()->StopMovementImmediately();
            Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
 }
      
        // Use physics-safe teleportation
        FVector RestoreLocation = OriginalTransform.GetLocation();
   FRotator RestoreRotation = OriginalTransform.GetRotation().Rotator();

        const bool bSweep = false;
        FHitResult HitResult;
        Actor->SetActorLocationAndRotation(
    RestoreLocation,
    RestoreRotation,
     bSweep,
          &HitResult,
            ETeleportType::ResetPhysics // CRITICAL: Reset physics
      );
        
      UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] Restored transform (physics-safe) for actor: %s"), *Actor->GetName());
    }
    else
    {
        // For non-characters, use standard transform
        Actor->SetActorTransform(OriginalTransform);
   UE_LOG(LogDialogueSequence, Log, TEXT("[CLEANUP] Restored transform for actor: %s"), *Actor->GetName());
    }
}
```

---

## ?? Тестирование

### Тест-кейс:

1. Создать sequence с Possessable персонажем
2. В sequence переместить персонажа (Transform track)
3. Включить `bRestoreActorTransforms = true` в эффекте
4. Запустить диалог с sequence
5. Дождаться окончания sequence

### Ожидаемый результат:

**ДО исправления:**
```
[CLEANUP] Restored transform for actor: BP_Lianne_C_1
(персонаж "падает" на землю после восстановления) ?
```

**ПОСЛЕ исправления:**
```
[CLEANUP] Restored transform (physics-safe) for actor: BP_Lianne_C_1
(персонаж плавно возвращается в исходную позицию, НЕ падает) ?
```

---

## ?? Best Practices

### 1. **Всегда используйте ResetPhysics для телепортации персонажей:**
```cpp
// Правильно:
Actor->SetActorLocationAndRotation(
    Location,
    Rotation,
    false, // No sweep
    nullptr,
ETeleportType::ResetPhysics
);

// Неправильно:
Actor->SetActorTransform(Transform); // Может вызвать падение
```

### 2. **Останавливайте движение ПЕРЕД телепортом:**
```cpp
// Правильный порядок:
Character->GetCharacterMovement()->StopMovementImmediately();
// THEN teleport
Actor->SetActorLocationAndRotation(...);
```

### 3. **Проверяйте тип актора:**
```cpp
// Правильно:
ACharacter* Character = Cast<ACharacter>(Actor);
if (Character)
{
    // Physics-safe teleport для персонажей
}
else
{
    // Standard transform для других акторов
}
```

### 4. **Не используйте sweep при телепортации:**
```cpp
const bool bSweep = false; // ВАЖНО: sweep=false для телепорта
Actor->SetActorLocationAndRotation(Location, Rotation, bSweep, ...);
```

---

## ?? Troubleshooting

### Проблема: Персонаж всё равно падает

**Причина 1:** CharacterMovementComponent в неправильном режиме
```cpp
// Проверьте:
Character->GetCharacterMovement()->MovementMode
```
**Решение:** Принудительно установите `MOVE_Walking` перед телепортом

---

**Причина 2:** Физика не сброшена
```cpp
// Проверьте тип телепорта:
ETeleportType::ResetPhysics // Должен быть ResetPhysics, НЕ TeleportPhysics
```

---

**Причина 3:** Движение не остановлено
```cpp
// Добавьте:
Character->GetCharacterMovement()->StopMovementImmediately();
// ПЕРЕД телепортом
```

---

### Проблема: Персонаж "дёргается" при восстановлении

**Причина:** Слишком резкий переход

**Решение:** Добавьте небольшую задержку или blend:
```cpp
// Опция 1: Задержка
FTimerHandle TimerHandle;
GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Actor, Transform]()
{
    RestoreActorTransform(Actor, Transform);
}, 0.1f, false);

// Опция 2: Плавное движение (не телепорт)
// Используйте AIController->MoveToLocation() вместо телепорта
```

---

## ?? Changelog

### v1.17.3 - Sequence Actor Falling Fix

**Changed:**
- `DialogueEffect_PlaySequence::CleanupSequence()` - physics-safe transform restoration
- Используется `ETeleportType::ResetPhysics` вместо `SetActorTransform()`
- Добавлена остановка движения перед восстановлением

**Fixed:**
- Персонажи больше НЕ падают после окончания sequence
- Персонажи плавно возвращаются в исходную позицию
- Физика корректно сбрасывается при восстановлении

**Added:**
- `#include "GameFramework/CharacterMovementComponent.h"`
- `#include "Components/CapsuleComponent.h"`
- Проверка типа актора (Character vs другие)

---

## ?? Демо

### Лог ДО исправления:
```
[CLEANUP] Restoring 1 actor transforms
[CLEANUP] Restored transform for actor: BP_Lianne_C_1
(Персонаж падает на землю) ?
```

### Лог ПОСЛЕ исправления:
```
[CLEANUP] Restoring 1 actor transforms
[CLEANUP] Restored transform (physics-safe) for actor: BP_Lianne_C_1
(Персонаж плавно возвращается в исходную позицию) ?
```

---

## ?? Связанные исправления

### Этот фикс связан с:

1. **v1.17.2** - `TeleportFix_CapsuleOffset_v1.17.2.md`
   - Исправление проваливания в землю при телепортации
 - Та же техника `ResetPhysics` используется здесь

2. **v1.17.1** - `DialogueCamera_TeleportFix_v1.17.1.md`
   - Исправление камеры после телепортации
   - Комплексное решение для телепортации в диалогах

### Общий принцип:

**ВСЕ телепортации персонажей должны:**
1. Останавливать движение
2. Использовать `ResetPhysics`
3. Проверять тип актора
4. Логировать результат

---

**Version:** 1.17.3  
**Date:** 2025-11-28  
**Status:** ? Fixed & Tested  
**Impact:** CRITICAL - Affects all sequences with bRestoreActorTransforms=true
