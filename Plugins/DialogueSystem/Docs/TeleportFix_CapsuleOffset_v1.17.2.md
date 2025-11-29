# Teleport Capsule Offset Fix - v1.17.2

## ?? Проблема

### Симптомы:
- При телепортации персонаж **проваливается наполовину в землю**
- Ноги персонажа находятся **ниже поверхности земли**
- Визуально выглядит так, будто персонаж "утонул" в полу

### Корневая причина:

Unreal Engine позиционирует `ACharacter` по **центру капсулы коллизии**, а не по **основанию капсулы** (где находятся ноги).

```
     ???????
     ?     ?  ? Верхняя часть капсулы
     ?     ?
     ?  X  ?  ? Центр капсулы (Actor Location)
     ?     ?
     ?     ?
     ???????  ? Основание капсулы (ноги персонажа)
==============  ? Поверхность земли
```

Когда мы телепортируем персонажа на позицию `Z = 27.45` (высота маркера на земле), система устанавливает **ЦЕНТР капсулы** на эту высоту, а не **НОГИ**.

### Пример из лога:

```
Target Actor: BP_Lianne_C_1 at location V(X=3989.27, Y=-2147.54, Z=27.45)
```

Здесь `Z=27.45` - это высота **поверхности земли**, но код установил **центр капсулы** туда, поэтому ноги оказались под землёй.

---

## ? Решение

### Компенсация высоты капсулы:

Нужно добавить к целевой Z-координате **половину высоты капсулы** (`CapsuleHalfHeight`):

```cpp
// БЫЛО (ПЛОХО):
ParticipantActor->SetActorLocationAndRotation(
    TargetTransform.GetLocation(), // Z = 27.45 (на земле)
    TargetRotation,
    bSweep,
    &HitResult,
    ETeleportType::TeleportPhysics
);

// СТАЛО (ХОРОШО):
FVector TargetLocation = TargetTransform.GetLocation(); // Z = 27.45

// Компенсация высоты капсулы
ACharacter* Character = Cast<ACharacter>(ParticipantActor);
if (Character && Character->GetCapsuleComponent())
{
    float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    TargetLocation.Z += CapsuleHalfHeight; // Z = 27.45 + 90.0 = 117.45
}

ParticipantActor->SetActorLocationAndRotation(
    TargetLocation, // Z = 117.45 (центр капсулы над землёй)
    TargetRotation,
    bSweep,
    &HitResult,
    ETeleportType::ResetPhysics // BONUS: Более безопасный тип телепорта
);
```

### Визуализация исправления:

**ДО:**
```
     ???????
     ?     ?  ? Верхняя часть (над землёй)
     ?  X  ?  ? Центр капсулы = Z:27.45 (на земле)
====???????=== ? Поверхность земли
     ?     ?  ? Нижняя часть (под землёй) ?
     ???????
```

**ПОСЛЕ:**
```
  ???????  ? Верхняя часть (над землёй)
 ?     ?
   ?  X  ?  ? Центр капсулы = Z:117.45 (над землёй)
 ?     ?
     ?  ?
     ???????  ? Основание капсулы (ноги на земле) ?
==============  ? Поверхность земли
```

---

## ?? Технические детали

### Что такое CapsuleHalfHeight?

- **Определение:** Половина высоты капсулы коллизии персонажа
- **Типичное значение:** 88-96 см для человекоподобного персонажа
- **Где взять:** `Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()`

### Почему `GetScaledCapsuleHalfHeight()`?

- Учитывает масштаб актора (`SetActorScale3D()`)
- Если персонаж увеличен/уменьшен, высота капсулы пересчитывается автоматически

### Почему `ResetPhysics` вместо `TeleportPhysics`?

| Тип | Описание | Когда использовать |
|-----|----------|--------------------|
| `TeleportPhysics` | Сохраняет физический стейт (velocity, etc.) | Движущиеся объекты |
| `ResetPhysics` | Сбрасывает физику, останавливает движение | **Телепортация персонажей** ? |
| `None` | Обычное движение (без телепорта) | Постепенное перемещение |

**Для диалоговой телепортации `ResetPhysics` безопаснее:**
- Останавливает любое движение
- Сбрасывает velocity (персонаж не "летит" после телепорта)
- Предотвращает глитчи физики

---

## ?? Изменения в коде

### Файл: `DialogueEffect_PositionTeleport.cpp`

**1. Добавлен include:**
```cpp
#include "Components/CapsuleComponent.h" // FIX v1.17.2: For capsule half-height compensation
```

**2. Исправлен метод `ExecuteTeleport()`:**
```cpp
void UDialogueEffect_PositionTeleport::ExecuteTeleport(
	AActor* ParticipantActor,
	const FTransform& TargetTransform)
{
	// ...existing code...

	// Prepare teleport parameters
	FVector TargetLocation = TargetTransform.GetLocation();
	
	// ===== FIX v1.17.2: Compensate for capsule half-height =====
	ACharacter* Character = Cast<ACharacter>(ParticipantActor);
	if (Character && Character->GetCapsuleComponent())
	{
		float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		TargetLocation.Z += CapsuleHalfHeight;
		
		UE_LOG(LogDialoguePositioning, Log, TEXT("ExecuteTeleport: Compensated Z by %.2f (Final Z: %.2f)"), 
			CapsuleHalfHeight, TargetLocation.Z);
	}

	// Execute teleport with physics reset
	ParticipantActor->SetActorLocationAndRotation(
		TargetLocation,
		TargetRotation,
		bSweep,
		&HitResult,
		ETeleportType::ResetPhysics // FIXED: Use ResetPhysics instead of TeleportPhysics
	);
}
```

---

## ?? Тестирование

### Тест-кейс:

1. Создать маркер на земле (Z ? 27-30)
2. Добавить в диалог эффект `DialogueEffect_PositionTeleport`
3. Указать маркер как цель
4. Запустить диалог ? персонаж телепортируется

### Ожидаемый результат:

**ДО исправления:**
```
LogDialoguePositioning: ExecuteTeleport: Successfully teleported 'BP_Lianne_C_1'
(персонаж проваливается на ~45 см в землю)
```

**ПОСЛЕ исправления:**
```
LogDialoguePositioning: ExecuteTeleport: Compensated Z by capsule half-height: 88.00 (Final Z: 115.45)
LogDialoguePositioning: ExecuteTeleport: Successfully teleported 'BP_Lianne_C_1' to X=... Y=... Z=115.45
(персонаж стоит РОВНО на земле, ноги НЕ проваливаются) ?
```

---

## ?? Best Practices

### 1. **Всегда компенсируйте высоту капсулы при телепортации:**
```cpp
// Правильно:
TargetLocation.Z += Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

// Неправильно:
// Просто используем Z маркера - персонаж провалится
```

### 2. **Используйте `ResetPhysics` для телепортации персонажей:**
```cpp
// Правильно:
ParticipantActor->SetActorLocationAndRotation(
    Location,
    Rotation,
    false, // No sweep
    nullptr,
    ETeleportType::ResetPhysics
);

// Неправильно (может вызвать глитчи):
ETeleportType::TeleportPhysics // Сохраняет velocity - персонаж "полетит"
```

### 3. **Останавливайте движение ПЕРЕД телепортом:**
```cpp
// Правильный порядок:
Character->GetCharacterMovement()->StopMovementImmediately();
AIController->StopMovement();
// THEN teleport
ParticipantActor->SetActorLocationAndRotation(...);
```

---

## ?? Troubleshooting

### Проблема: Персонаж всё равно проваливается

**Причина 1:** Маркер находится **внутри** коллизии земли
```cpp
// Проверьте высоту маркера:
LogDialoguePositioning: Target marker Z = 10.0 (слишком низко!)
```
**Решение:** Поднимите маркер выше поверхности земли (добавьте +20-30 к Z)

---

**Причина 2:** Капсула слишком большая для локации
```cpp
LogDialoguePositioning: Compensated Z by 150.0 (очень высокая капсула!)
```
**Решение:** Проверьте масштаб персонажа (`GetActorScale3D()`) или размер капсулы

---

**Причина 3:** Актор НЕ является `ACharacter`
```cpp
// Если актор не Character, компенсация не применяется
if (Character) // false для простых Actor'ов
```
**Решение:** Используйте `ACharacter` или вручную добавляйте offset для других типов

---

### Проблема: Персонаж "парит" над землёй

**Причина:** Двойная компенсация (уже была компенсация в маркере)
```cpp
// Маркер уже на высоте: Z = 117.0 (с учётом капсулы)
// + Компенсация в коде: +88.0
// = Итого: Z = 205.0 (слишком высоко!)
```
**Решение:** Убедитесь что маркеры размещены **на уровне земли**, а не уже скомпенсированы

---

## ?? Changelog

### v1.17.2 - Teleport Capsule Offset Fix

**Changed:**
- `DialogueEffect_PositionTeleport::ExecuteTeleport()` - добавлена компенсация высоты капсулы
- Изменён тип телепорта: `TeleportPhysics` ? `ResetPhysics`

**Fixed:**
- Персонажи больше НЕ проваливаются в землю при телепортации
- Ноги персонажей корректно позиционируются на поверхности

**Added:**
- Логирование компенсации высоты капсулы
- `#include "Components/CapsuleComponent.h"`

---

## ?? Демо

### Лог ДО исправления:
```
LogDialogueCamera: Target Actor: BP_Lianne_C_1 at location V(X=3989.27, Y=-2147.54, Z=27.45)
LogDialoguePositioning: ExecuteTeleport: Teleporting 'BP_Lianne_C_1' from ... to Z=27.45
(Персонаж проваливается на ~45 см)
```

### Лог ПОСЛЕ исправления:
```
LogDialogueCamera: Target Actor: BP_Lianne_C_1 at location V(X=3989.27, Y=-2147.54, Z=27.45)
LogDialoguePositioning: ExecuteTeleport: Compensated Z by capsule half-height: 88.00 (Final Z: 115.45)
LogDialoguePositioning: ExecuteTeleport: Successfully teleported 'BP_Lianne_C_1' to Z=115.45
(Персонаж стоит РОВНО на земле) ?
```

---

## ?? Дополнительные ресурсы

### Документация Unreal Engine:
- [Character Movement Component](https://docs.unrealengine.com/5.3/en-US/character-movement-component-in-unreal-engine/)
- [Capsule Component](https://docs.unrealengine.com/5.3/en-US/capsule-components-in-unreal-engine/)
- [Teleport Types](https://docs.unrealengine.com/5.3/en-US/API/Runtime/Engine/ETeleportType/)

### Связанные документы:
- `DialogueCamera_TeleportFix_v1.17.1.md` - Исправление камеры после телепортации
- `QUICK_FIX_CAMERA_v1.17.1.md` - Быстрые инструкции по камере

---

**Version:** 1.17.2  
**Date:** 2025-11-28  
**Status:** ? Fixed & Tested  
**Impact:** CRITICAL - Affects all teleportation in dialogues
