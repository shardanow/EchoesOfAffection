# Version 1.4.5 - Blueprint Library Interface Fix ??

**Дата**: 25 октября 2024, 16:00  
**Версия**: 1.4.5 (Blueprint Library Fix)  
**Статус**: ? **CRITICAL FIX**

---

## ?? Проблема

**Blueprint Library вызывал interface methods напрямую!**

### ? Ошибка:

```cpp
// TimeOfDayBlueprintLibrary.cpp
return Subsystem->GetNormalizedTimeOfDay();  // ? CRASH!
```

**Результат**: 
```
check(0 && "Do not directly call Event functions in Interfaces. 
Call Execute_GetNormalizedTimeOfDay instead.");
```

**Причина**:  
`UTimeOfDaySubsystem` implements `ITimeOfDayManager` interface.  
Interface methods are **Blueprint Native Events** with `_Implementation` suffix.  
Calling them directly triggers assertion!

---

## ? Решение

Использовать **BP_ wrapper methods** вместо прямых вызовов:

### До исправления:

```cpp
// ? НЕПРАВИЛЬНО!
return Subsystem->GetCurrentTime();
return Subsystem->GetCurrentPhase();
return Subsystem->GetNormalizedTimeOfDay();
return Subsystem->GetCurrentSeason();
Subsystem->SetCurrentTime(NewTime);
Subsystem->AdvanceTime(GameSeconds);
```

### После исправления:

```cpp
// ? ПРАВИЛЬНО!
return Subsystem->BP_GetCurrentTime();
return Subsystem->BP_GetCurrentPhase();
return Subsystem->BP_GetNormalizedTimeOfDay();
return Subsystem->BP_GetCurrentSeason();
Subsystem->BP_SetCurrentTime(NewTime);
Subsystem->BP_AdvanceTime(GameSeconds);
```

---

## ?? Изменённые функции

### 1. Quick Access Functions (4 functions):

```cpp
FTimeOfDayTime GetCurrentTime()         ? BP_GetCurrentTime()
ETimeOfDayPhase GetCurrentPhase()       ? BP_GetCurrentPhase()
float GetNormalizedTimeOfDay()          ? BP_GetNormalizedTimeOfDay()
ETimeOfDaySeason GetCurrentSeason()     ? BP_GetCurrentSeason()
```

### 2. Time Control Functions (7 functions):

```cpp
SetCurrentTime()  ? BP_SetCurrentTime()
AdvanceTime() ? BP_AdvanceTime()
SetTimeScale()    ? BP_SetTimeScale()
GetTimeScale()    ? BP_GetTimeScale()
PauseTime()       ? BP_PauseTime()
ResumeTime()      ? BP_ResumeTime()
IsTimePaused()    ? BP_IsTimePaused()
```

---

## ?? Сравнение

| Method Call Type | Old Code | New Code | Works? |
|------------------|----------|----------|--------|
| Interface Direct | `GetCurrentTime()` | - | ? Crash |
| Execute_ Version | `Execute_GetCurrentTime()` | - | ? OK |
| BP_ Wrapper | - | `BP_GetCurrentTime()` | ? OK |

**Решение**: Использовать BP_ wrappers! ?

---

## ?? Compilation

```
Building EchoesOfAffection...
[1/3] Compile TimeOfDayBlueprintLibrary.cpp
[2/3] Link EchoesOfAffection.exe
[3/3] WriteMetadata

Result: Succeeded ?
Total execution time: 7.17 seconds
```

---

## ? Что теперь работает

### Before (v1.4.4):

```cpp
// Blueprint:
Get Normalized Time Of Day (World Context)
  ?
UTimeOfDayBlueprintLibrary::GetNormalizedTimeOfDay()
  ?
Subsystem->GetNormalizedTimeOfDay()  // ? CRASH!
  ?
check(0 && "Do not call Event functions...")
```

### After (v1.4.5):

```cpp
// Blueprint:
Get Normalized Time Of Day (World Context)
  ?
UTimeOfDayBlueprintLibrary::GetNormalizedTimeOfDay()
  ?
Subsystem->BP_GetNormalizedTimeOfDay()  // ? OK!
  ?
GetNormalizedTimeOfDay_Implementation()
  ?
Returns value correctly!
```

---

## ?? File Changed

**File**: `TimeOfDayBlueprintLibrary.cpp`

**Lines changed**: 11 functions  
**Type**: Interface call fix

---

## ?? Breaking Changes

**НЕТ!** Это internal fix.

**For Blueprint users**: Nothing changes!  
**For C++ users**: Use `UTimeOfDayBlueprintLibrary` as before.

---

## ?? Why This Happened

### Unreal Engine Interface Pattern:

1. **Interface declaration**:
```cpp
UFUNCTION(BlueprintNativeEvent)
float GetNormalizedTimeOfDay() const;
```

2. **UHT generates**:
```cpp
// Header:
virtual float GetNormalizedTimeOfDay_Implementation() const;

// Execute wrapper:
static float Execute_GetNormalizedTimeOfDay(UObject* Obj);
```

3. **Implementation**:
```cpp
float UTimeOfDaySubsystem::GetNormalizedTimeOfDay_Implementation()
{
    return CurrentTime.GetNormalizedTimeOfDay();
}
```

4. **Correct calls**:
```cpp
// ? Option 1: Use Execute_
ITimeOfDayManager::Execute_GetNormalizedTimeOfDay(Subsystem);

// ? Option 2: Use wrapper method (our solution)
Subsystem->BP_GetNormalizedTimeOfDay();

// ? Option 3: Call directly (WRONG!)
Subsystem->GetNormalizedTimeOfDay();  // Crashes!
```

---

## ?? Related Fixes

- **v1.2**: Added BP_ wrapper methods to Subsystem
- **v1.4.5**: **Fixed Blueprint Library to use BP_ wrappers** ? Current

---

**Status**: ? **FIXED AND COMPILED**  
**Version**: 1.4.5  
**Priority**: ?? **CRITICAL**  
**Functions Fixed**: 11

**Blueprint Library now works correctly!** ??
