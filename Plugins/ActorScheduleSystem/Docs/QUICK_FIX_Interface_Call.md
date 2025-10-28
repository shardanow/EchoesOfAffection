# Quick Fix: "Do not directly call Event functions in Interfaces" Error

## Symptoms
- Game crashes when NPC tries to select a location
- Error message: `"Do not directly call Event functions in Interfaces. Call Execute_SelectLocation instead."`
- Happens when using **Blueprint-based** location selectors

## Immediate Solution

The issue has been **fixed** in the latest version. Update your code or pull the latest changes.

## What Was Wrong

The system was calling interface functions incorrectly for Blueprint implementations.

## What Was Fixed

**File:** `ScheduleComponent.cpp` (line 165)

**Changed from:**
```cpp
Entry->LocationSelector->SelectLocation(LocationContext, SelectedLocation)
```

**Changed to:**
```cpp
IScheduleLocationSelectorInterface::Execute_SelectLocation(Entry->LocationSelector, LocationContext, SelectedLocation)
```

## If You're Using Custom Selectors

### ? Correct Pattern (Use This!)

```cpp
// For interface calls on UObjects
if (SelectorObject->Implements<UScheduleLocationSelectorInterface>())
{
    bool bSuccess = IScheduleLocationSelectorInterface::Execute_SelectLocation(
        SelectorObject, 
        Context, 
        OutLocation
    );
}
```

### ? Incorrect Pattern (Don't Do This!)

```cpp
// This CRASHES with Blueprint implementations
if (IScheduleLocationSelectorInterface* Interface = Cast<IScheduleLocationSelectorInterface>(SelectorObject))
{
    Interface->SelectLocation(Context, OutLocation); // WRONG!
}
```

## Why This Matters

Unreal Engine interfaces work differently for C++ vs Blueprint:

| Implementation Type | Direct Call | Execute_ Wrapper |
|:---------------------|:-------------|:------------------|
| C++ Class | ? Works | ? Works |
| Blueprint Class | ? **CRASHES** | ? Works |

**Always use `Execute_`** to support both C++ and Blueprint implementations.

## Quick Test

1. Update your code with the fix
2. Recompile your project
3. Run the game
4. NPCs should now select locations without crashing

## Need More Info?

See `BUGFIX_Interface_Call.md` for detailed technical explanation.

## Related Interfaces

The same pattern applies to all Blueprint-callable interfaces:
- `IScheduleActionInterface::Execute_ExecuteAction`
- `IScheduleConditionInterface::Execute_EvaluateCondition`
- `IScheduleLocationSelectorInterface::Execute_GetPotentialLocations`
- `IScheduleLocationSelectorInterface::Execute_GetSelectorDescription`

---

**Fixed:** 2025-10-27  
**Version:** ActorScheduleSystem v1.0+
