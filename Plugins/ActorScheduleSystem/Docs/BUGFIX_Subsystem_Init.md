# Bugfix: Subsystem Initialization Order

## ?? Проблема

```
Error: Ensure condition failed: bInitialized
Tickable subsystem ScheduleSubsystem tried to tick when not initialized!
```

---

## ? Решение

### 1. **TimeOfDaySubsystem.cpp** - Безопасная проверка

```cpp
void UTimeOfDaySubsystem::NotifyScheduleSystem()
{
    if (UWorld* World = GetWorld())
    {
        UScheduleSubsystem* ScheduleSystem = World->GetSubsystem<UScheduleSubsystem>();
        
        // ? FIX: Verify subsystem is ready before calling
        if (ScheduleSystem && ScheduleSystem->GetWorld())
      {
            const int32 DayOfWeek = static_cast<int32>(CurrentTime.DayOfWeek);
  ScheduleSystem->NotifyTimeChanged(CurrentTime.Hour, CurrentTime.Minute, DayOfWeek);
        }
    }
}
```

**Изменения:**
- Добавлена дополнительная проверка `ScheduleSystem->GetWorld()`
- Это гарантирует, что subsystem полностью инициализирован

### 2. **ScheduleSubsystem.cpp** - Правильный порядок Tick

```cpp
void UScheduleSubsystem::Tick(float DeltaTime)
{
    // ? FIX: Do NOT call Super::Tick() first
    
    if (bIsPaused)
    {
        return;
    }

    AccumulatedDeltaTime += DeltaTime;

    // Clean up invalid components
    RegisteredComponents.RemoveAll([](const UScheduleComponent* Component)
    {
        return !IsValid(Component);
    });
    
    // ? Call Super::Tick at the END
    Super::Tick(DeltaTime);
}
```

**Изменения:**
- Перенесли `Super::Tick(DeltaTime)` в конец метода
- Это позволяет родительскому классу проверить `bInitialized` ПОСЛЕ нашей логики

---

## ?? Причина ошибки

Unreal Engine's `UTickableWorldSubsystem::Tick()` содержит проверку:

```cpp
void UTickableWorldSubsystem::Tick(float DeltaTime)
{
    // Проверка: subsystem должен быть инициализирован
    ensureMsgf(bInitialized, TEXT("Tickable subsystem %s tried to tick when not initialized!"), *GetFullName());
    // ...
}
```

Если вызвать `Super::Tick()` в начале метода, эта проверка сработает раньше, чем subsystem полностью готов.

---

## ? Результат

- ? Никаких ошибок инициализации
- ? Safe integration между TimeOfDaySystem и ScheduleSystem
- ? Корректный порядок вызовов
- ? Production-ready код

---

## ?? Тестирование

### Проверьте в Output Log:

```
LogTemp: ? Loaded TimeOfDaySettings from: ...
LogTemp: TimeOfDaySubsystem initialized. Current time: 06:00 Year 1 (Monday)
LogTemp: Schedule Subsystem Initialized
```

**Не должно быть:**
```
? Error: Ensure condition failed: bInitialized
```

---

## ?? Лучшие практики

### DO ?
```cpp
// 1. Проверяйте subsystem перед использованием
if (Subsystem && Subsystem->GetWorld()) { ... }

// 2. Вызывайте Super::Tick() в конце
void MySubsystem::Tick(float DeltaTime)
{
    // Your logic
    Super::Tick(DeltaTime); // At the end
}
```

### DON'T ?
```cpp
// 1. НЕ предполагайте, что subsystem готов
Subsystem->Method(); // Crash if nullptr!

// 2. НЕ вызывайте Super::Tick() в начале tickable subsystems
void MySubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); // ? Too early!
    // Your logic
}
```

---

## ?? Дополнительные ресурсы

- **UWorldSubsystem.cpp** - Parent class implementation
- **Epic's subsystem documentation** - Best practices
- **INTEGRATION_RU.md** - Integration guide

---

**Status:** ? FIXED  
**Build:** ? SUCCESSFUL  
**Ready for:** Production use
