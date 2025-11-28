# Dialogue Camera Tracking Fix - v1.17.1

## Проблема
Камера диалога не отслеживала телепортацию участников и не переключалась на текущего спикера при смене реплик.

## Решение

### 1. GameEventBus Integration
Добавлена интеграция через `GameEventBusSubsystem` для отслеживания событий:
- `GameEvent.Dialogue.ParticipantPositioned` - когда участник телепортируется
- `GameEvent.Dialogue.NodeEntered` - когда меняется реплика (спикер)

### 2. Автоматическое Переключение Камеры
**DialogueCameraComponent** теперь:
- Автоматически переключается на текущего спикера при смене реплик
- Обновляет позицию камеры после телепортации участников
- Валидирует что цель находится в кадре

### 3. Конфигурация (Exposed in Editor)

```cpp
// Автообновление после позиционирования
UPROPERTY(EditAnywhere, Category = "Camera|Auto Update")
bool bAutoUpdateOnPositioning = true;

// Задержка перед обновлением (дает анимации завершиться)
UPROPERTY(EditAnywhere, Category = "Camera|Auto Update")
float PositioningUpdateDelay = 0.2f;

// Время блендинга при репозиционировании
UPROPERTY(EditAnywhere, Category = "Camera|Auto Update")
float PositioningBlendTime = 0.5f;

// Автопереключение на текущего спикера
UPROPERTY(EditAnywhere, Category = "Camera|Auto Update")
bool bAutoSwitchToSpeaker = true;

// Время блендинга при смене спикера
UPROPERTY(EditAnywhere, Category = "Camera|Auto Update")
float SpeakerSwitchBlendTime = 0.6f;
```

### 4. Архитектура (AAA Best Practices)

**Event-Driven Design:**
- ? НЕТ хард-зависимостей между системами
- ? Все через `GameEventBusSubsystem`
- ? Слабая связанность (loose coupling)
- ? Легко тестировать и расширять

**Timing Control:**
- Configurable delays для smooth transitions
- Validation checks перед обновлением камеры
- Smart blending для плавности

## Использование

### В Blueprint
Просто добавьте `DialogueCameraComponent` к актору диалога и настройте:

```
1. Активируйте камеру при старте диалога:
   ActivateDialogueCameraSimple(TargetNPC)

2. Система автоматически:
   - Переключится на спикера при смене реплик
   - Обновит камеру после телепортации
   - Вернет оригинальную камеру при завершении

3. Настройте тайминги в Details panel
```

### В C++
```cpp
// Компонент автоматически подписывается на события в BeginPlay
UDialogueCameraComponent* CameraComp = GetOwner()->FindComponentByClass<UDialogueCameraComponent>();

// При необходимости можно отключить автообновление
CameraComp->bAutoSwitchToSpeaker = false;
CameraComp->bAutoUpdateOnPositioning = false;

// И управлять вручную
CameraComp->TransitionToTarget(NewSpeaker, 0.5f);
```

## Изменённые Файлы

### DialogueCameraComponent
- `Public/Components/DialogueCameraComponent.h` - добавлены properties и методы
- `Private/Components/DialogueCameraComponent.cpp` - реализация GameEventBus подписки

### DialogueRunner
- `Public/Core/DialogueRunner.h` - добавлен `EmitNodeEnteredEvent()`
- `Private/Core/DialogueRunner.cpp` - эмиссия `GameEvent.Dialogue.NodeEntered`

### DialogueEffect_PositionParticipant
- `Public/Effects/DialogueEffect_PositionParticipant.h` - добавлен `EmitParticipantPositionedEvent()`
- `Private/Effects/DialogueEffect_PositionParticipant.cpp` - эмиссия после позиционирования

## Тестирование

1. **Базовый диалог:**
- Запустите диалог
   - Камера должна фокусироваться на NPC

2. **Смена спикера:**
   - Переключите реплику с Player на NPC
   - Камера должна плавно переключиться на NPC
   - Переключите обратно - камера на Player

3. **Телепортация:**
   - Создайте ноду с `DialogueEffect_PositionTeleport`
   - Телепортируйте NPC в другое место
   - Камера должна автоматически обновиться через `PositioningUpdateDelay`

4. **Проверка настроек:**
   - Измените `SpeakerSwitchBlendTime` на 2.0s - переход должен стать медленнее
   - Отключите `bAutoSwitchToSpeaker` - камера не должна переключаться
   - Отключите `bAutoUpdateOnPositioning` - камера не должна следовать за телепортацией

## Известные Ограничения

1. **Требуется GameEventBusSubsystem** - если модуль не доступен (`!WITH_GAMEEVENTBUS`), автообновление не работает
2. **First Frame может быть неточным** - если участники телепортируются ДО активации камеры
3. **Только один DialogueCameraComponent активен** - нельзя иметь несколько активных камер одновременно

## Будущие Улучшения (TODO)

- [ ] Добавить camera shake при телепортации
- [ ] Поддержка multi-camera setups (picture-in-picture)
- [ ] Automatic framing adjustment based on number of visible participants
- [ ] Integration with Cinemachine for advanced camera control

## Debug Tips

Включите логгирование:
```cpp
LogDialogueCamera Log   // Основные события
LogDialogueCamera Verbose // Детальная информация
```

Полезные логи:
- `Subscribed to Dialogue.ParticipantPositioned` - подписка успешна
- `OnParticipantPositioned: Current target moved` - телепортация обнаружена
- `OnNodeEntered: Switching camera to new speaker` - смена спикера
- `Camera position validation FAILED` - цель не в кадре!

---

**Version:** 1.17.1  
**Author:** Senior Game Designer & Technical Architect  
**Date:** 2025-01-XX  
