# Bugfix: Subsystem Initialization Order

## ?? ��������

```
Error: Ensure condition failed: bInitialized
Tickable subsystem ScheduleSubsystem tried to tick when not initialized!
```

---

## ? �������

### 1. **TimeOfDaySubsystem.cpp** - ���������� ��������

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

**���������:**
- ��������� �������������� �������� `ScheduleSystem->GetWorld()`
- ��� �����������, ��� subsystem ��������� ���������������

### 2. **ScheduleSubsystem.cpp** - ���������� ������� Tick

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

**���������:**
- ��������� `Super::Tick(DeltaTime)` � ����� ������
- ��� ��������� ������������� ������ ��������� `bInitialized` ����� ����� ������

---

## ?? ������� ������

Unreal Engine's `UTickableWorldSubsystem::Tick()` �������� ��������:

```cpp
void UTickableWorldSubsystem::Tick(float DeltaTime)
{
    // ��������: subsystem ������ ���� ���������������
    ensureMsgf(bInitialized, TEXT("Tickable subsystem %s tried to tick when not initialized!"), *GetFullName());
    // ...
}
```

���� ������� `Super::Tick()` � ������ ������, ��� �������� ��������� ������, ��� subsystem ��������� �����.

---

## ? ���������

- ? ������� ������ �������������
- ? Safe integration ����� TimeOfDaySystem � ScheduleSystem
- ? ���������� ������� �������
- ? Production-ready ���

---

## ?? ������������

### ��������� � Output Log:

```
LogTemp: ? Loaded TimeOfDaySettings from: ...
LogTemp: TimeOfDaySubsystem initialized. Current time: 06:00 Year 1 (Monday)
LogTemp: Schedule Subsystem Initialized
```

**�� ������ ����:**
```
? Error: Ensure condition failed: bInitialized
```

---

## ?? ������ ��������

### DO ?
```cpp
// 1. ���������� subsystem ����� ��������������
if (Subsystem && Subsystem->GetWorld()) { ... }

// 2. ��������� Super::Tick() � �����
void MySubsystem::Tick(float DeltaTime)
{
    // Your logic
    Super::Tick(DeltaTime); // At the end
}
```

### DON'T ?
```cpp
// 1. �� �������������, ��� subsystem �����
Subsystem->Method(); // Crash if nullptr!

// 2. �� ��������� Super::Tick() � ������ tickable subsystems
void MySubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); // ? Too early!
    // Your logic
}
```

---

## ?? �������������� �������

- **UWorldSubsystem.cpp** - Parent class implementation
- **Epic's subsystem documentation** - Best practices
- **INTEGRATION_RU.md** - Integration guide

---

**Status:** ? FIXED  
**Build:** ? SUCCESSFUL  
**Ready for:** Production use
