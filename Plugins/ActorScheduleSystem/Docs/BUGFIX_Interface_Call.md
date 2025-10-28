# BUGFIX: Interface Function Call for Blueprint Compatibility

## Issue
**Error Message:**
```
Assertion failed: 0 && "Do not directly call Event functions in Interfaces. Call Execute_SelectLocation instead."
File: ScheduleLocationSelectorInterface.gen.cpp [Line: 199]
```

## Root Cause
The `ScheduleComponent` was calling the interface function `SelectLocation()` directly on the selector object:

```cpp
if (!Entry->LocationSelector->SelectLocation(LocationContext, SelectedLocation))
```

When calling interface functions on UObjects that might be implemented in **Blueprint**, you must use the `Execute_` wrapper function. This is because:

1. Blueprint implementations don't have direct C++ vtable entries
2. Unreal's reflection system needs to route the call through `ProcessEvent`
3. The `Execute_` wrapper handles both C++ and Blueprint implementations correctly

## Solution
Changed the call to use the `Execute_SelectLocation` static wrapper:

```cpp
if (!IScheduleLocationSelectorInterface::Execute_SelectLocation(Entry->LocationSelector, LocationContext, SelectedLocation))
```

## Technical Details

### How Unreal Interface Calls Work

**For C++ implementations:**
- Direct function calls work because of C++ vtable
- `Execute_` wrapper also works (checks for Blueprint first, then calls native)

**For Blueprint implementations:**
- Direct calls **FAIL** with assertion error
- `Execute_` wrapper **REQUIRED** - uses `ProcessEvent` to call Blueprint function

### The Execute_ Wrapper Pattern

The generated code creates a static `Execute_` function for each `BlueprintNativeEvent`:

```cpp
// Generated in .gen.cpp
bool IScheduleLocationSelectorInterface::Execute_SelectLocation(
    UObject* O, 
    const FScheduleLocationContext& Context, 
    FScheduleLocation& OutLocation)
{
    check(O != NULL);
    check(O->GetClass()->ImplementsInterface(UScheduleLocationSelectorInterface::StaticClass()));
    
    // Try to find Blueprint implementation
    UFunction* const Func = O->FindFunction(NAME_UScheduleLocationSelectorInterface_SelectLocation);
    if (Func)
    {
        // Blueprint implementation exists - use ProcessEvent
        ScheduleLocationSelectorInterface_eventSelectLocation_Parms Parms;
        Parms.Context = Context;
        Parms.OutLocation = OutLocation;
        O->ProcessEvent(Func, &Parms);
 OutLocation = Parms.OutLocation;
        return Parms.ReturnValue;
    }
    else if (auto I = (IScheduleLocationSelectorInterface*)(O->GetNativeInterfaceAddress(...)))
    {
        // C++ implementation - call directly
        return I->SelectLocation_Implementation(Context, OutLocation);
  }
}
```

## Files Changed

### Plugins/ActorScheduleSystem/Source/ScheduleRuntime/Private/Components/ScheduleComponent.cpp

**Before:**
```cpp
if (!Entry->LocationSelector->SelectLocation(LocationContext, SelectedLocation))
```

**After:**
```cpp
if (!IScheduleLocationSelectorInterface::Execute_SelectLocation(Entry->LocationSelector, LocationContext, SelectedLocation))
```

## Testing

1. **Compile** the project - should compile without errors
2. **Test with Blueprint selector** - should no longer crash
3. **Test with C++ selector** - should still work correctly
4. **Check logs** - should see location selection working properly

## Best Practices

### When to Use Execute_ Wrapper

? **ALWAYS use Execute_ when:**
- Calling interface functions on UObject pointers
- The object might be a Blueprint class
- You're not sure if it's C++ or Blueprint

? **Direct calls are OK when:**
- You KNOW it's a C++ object (rare)
- You're calling from within the same C++ class
- You're calling the `_Implementation` function directly

### Pattern to Follow

```cpp
// ? WRONG - Will crash with Blueprint implementations
if (IMyInterface* Interface = Cast<IMyInterface>(SomeObject))
{
  Interface->MyFunction(Args);
}

// ? CORRECT - Works with both C++ and Blueprint
if (SomeObject->GetClass()->ImplementsInterface(UMyInterface::StaticClass()))
{
    IMyInterface::Execute_MyFunction(SomeObject, Args);
}

// ? ALSO CORRECT - More concise
if (SomeObject->Implements<UMyInterface>())
{
    IMyInterface::Execute_MyFunction(SomeObject, Args);
}
```

## Related Issues

This is a common Unreal Engine pitfall. Similar fixes may be needed for:
- `IScheduleActionInterface::Execute_ExecuteAction`
- `IScheduleConditionInterface::Execute_EvaluateCondition`
- Any other Blueprint-callable interface functions

## References

- [Unreal Engine Interfaces Documentation](https://docs.unrealengine.com/5.3/en-US/interfaces-in-unreal-engine/)
- [BlueprintNativeEvent Best Practices](https://docs.unrealengine.com/5.3/en-US/blueprint-native-events-in-unreal-engine/)

## Version
- **Fixed in:** 2025-10-27
- **Affects:** ActorScheduleSystem v1.0+
- **Unreal Version:** 5.6 (applies to 5.0+)
