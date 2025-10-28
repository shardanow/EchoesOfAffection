# Bugfix: Tickable Subsystem Initialization Error

## Issue
```
Ensure condition failed: bInitialized [File:D:\build\++UE5\Sync\Engine\Source\Runtime\Engine\Private\Subsystems\WorldSubsystem.cpp] [Line: 83] 
Tickable subsystem ScheduleSubsystem /Script/ScheduleRuntime.Default__ScheduleSubsystem tried to tick when not initialized! Check for missing Super call
```

## Root Cause

The `UScheduleSubsystem` inherits from `UTickableWorldSubsystem`, which has built-in initialization checking. The subsystem's `GetTickableTickType()` was returning `ETickableTickType::Always`, which allowed the engine to attempt ticking the subsystem before initialization was complete.

### Why This Happens

1. **Engine Tick Registration**: When a `UTickableWorldSubsystem` is created, the Unreal Engine tick system registers it for ticking
2. **Initialization Timing**: The subsystem may be registered for ticking before `Initialize()` is called
3. **Missing Guard**: Without checking `IsInitialized()` in `GetTickableTickType()`, the engine can attempt to tick an uninitialized subsystem
4. **Safety Check**: `UTickableWorldSubsystem::IsAllowedToTick()` checks `bInitialized` and triggers an ensure when it's false

## Solution

Override `GetTickableTickType()` to check initialization status before allowing ticking:

```cpp
virtual ETickableTickType GetTickableTickType() const override
{
    // Don't tick until we're initialized
    if (!IsInitialized())
 {
return ETickableTickType::Never;
    }
    return ETickableTickType::Always;
}
```

### What This Does

1. **Prevents Early Ticking**: Returns `ETickableTickType::Never` if not yet initialized
2. **Enables After Init**: Returns `ETickableTickType::Always` once initialized
3. **Safe Pattern**: Follows Unreal Engine's recommended pattern for tickable subsystems

## Technical Details

### UTickableWorldSubsystem Initialization Flow

1. **Construction**: Subsystem object created
2. **Tick Registration**: Engine registers subsystem as tickable
3. **Initialize Called**: `Initialize()` is called, which sets `bInitialized = true`
4. **GetTickableTickType Checked**: Engine calls this to determine if subsystem should tick
5. **Ticking Begins**: If `GetTickableTickType()` returns anything other than `Never`, ticking starts

### Key Methods Involved

```cpp
// Base class (UTickableWorldSubsystem)
virtual void Initialize(FSubsystemCollectionBase& Collection) override
{
    check(!bInitialized);
    bInitialized = true;  // Sets the flag
    Super::Initialize(Collection);
}

// Base class default implementation
virtual ETickableTickType GetTickableTickType() const override 
{
    // If not initialized, returns Never
    if (IsTemplate() || !bInitialized)
    {
        return ETickableTickType::Never;
    }
    return ETickableTickType::Conditional;
}

// Our override was incorrectly always returning Always
// Now correctly checks initialization
```

## Files Modified

- `Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Public/Subsystems/ScheduleSubsystem.h`

## Testing

After applying this fix:

1. ? No initialization error on subsystem creation
2. ? Subsystem properly waits for initialization before ticking
3. ? Normal ticking resumes after initialization
4. ? All schedule components register and evaluate correctly

## Best Practices for Tickable Subsystems

When creating a `UTickableWorldSubsystem`:

1. **Always Check Initialization**: Override `GetTickableTickType()` to check `IsInitialized()`
2. **Call Super::Initialize First**: Ensure parent class sets up properly
3. **Call Super::Tick Last**: Call parent tick after your logic (we already do this)
4. **Handle Deinitialization**: Clean up in `Deinitialize()` before calling `Super::Deinitialize()`

## References

- Unreal Engine Source: `Engine/Source/Runtime/Engine/Private/Subsystems/WorldSubsystem.cpp`
- UE Documentation: World Subsystems and Tick Dependencies
- Related Fix: `BUGFIX_Subsystem_Init.md` (Super::Tick placement)

## Status

? **FIXED** - Subsystem now properly checks initialization before allowing ticking.
