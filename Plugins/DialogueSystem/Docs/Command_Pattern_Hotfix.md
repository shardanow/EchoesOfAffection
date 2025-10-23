# Command Pattern v1.2 - Hotfix: Initialization Issue

> **Date:** 22.01.2025  
> **Issue:** NewObject in constructor crash  
> **Status:** ? FIXED  
> **Severity:** Critical

---

## ?? Problem

Project crashed on startup with error:
```
"NewObject with empty name can't be used to create default subobjects 
(inside of UObject derived class constructor)"
```

**Root Cause:** CommandInvoker was created in `UDialogueRunner` constructor using `NewObject()`.

---

## ? Solution

Implemented **Lazy Initialization Pattern**:

```cpp
// BEFORE (WRONG):
UDialogueRunner::UDialogueRunner()
{
    CommandInvoker = NewObject<UDialogueCommandInvoker>(this); // ? CRASH!
}

// AFTER (CORRECT):
UDialogueRunner::UDialogueRunner()
    : CommandInvoker(nullptr) // ? Initialize to nullptr
{
    // Don't create here
}

void UDialogueRunner::StartDialogue(...)
{
    // Create when needed
    if (!CommandInvoker)
    {
      CommandInvoker = NewObject<UDialogueCommandInvoker>(this); // ? Safe!
    }
}
```

---

## ?? Changes Made

### 1. Constructor
- ? Removed `NewObject()` call
- ? Initialize `CommandInvoker` to `nullptr`

### 2. StartDialogue()
- ? Create CommandInvoker if not exists
- ? Configure recording settings

### 3. Command Methods
- ? All methods check for `nullptr`
- ? Create CommandInvoker on first use
- ? Fail gracefully if creation fails

---

## ?? Impact

| Aspect | Before | After |
|--------|--------|-------|
| Startup | ? Crash | ? Works |
| Memory | Immediate allocation | Lazy allocation |
| Safety | Unsafe | Safe |
| Performance | N/A | Slightly better |

---

## ?? Benefits

**Lazy Initialization:**
- ? Only creates when needed
- ? Safe - no NewObject in constructor
- ? Efficient - no wasted memory
- ? Flexible - can be created anytime

---

## ?? Updated Files

```
Plugins/DialogueSystem/Source/DialogueSystemCore/Private/Core/
??? DialogueRunner.cpp      ?? FIXED

Plugins/DialogueSystem/Docs/
??? Command_Pattern_QuickStart.md      ?? UPDATED
??? Command_Pattern_Hotfix.md    ? NEW (this file)
??? CHANGELOG.md                ?? UPDATED
```

---

## ? Testing

**Before Fix:**
```
> Run Project
? Crash: "NewObject in constructor"
```

**After Fix:**
```
> Run Project
? Success: Project starts normally
? Success: Commands work correctly
? Success: No crashes
```

---

## ?? Future Improvements

This is the correct pattern for Unreal Engine. No further changes needed.

**Best Practice:**
> Never use `NewObject()` in UObject constructors. Always use lazy initialization or create objects in `BeginPlay()` / `PostInitProperties()`.

---

## ?? Related Documentation

- Unreal Engine: [Objects Best Practices](https://docs.unrealengine.com)
- Command Pattern: `Command_Pattern_QuickStart.md`
- Changelog: `CHANGELOG.md` (v1.2.0)

---

**Status:** ? RESOLVED  
**Version:** 1.2.0  
**Build:** SUCCESS ?  
**Date:** 22.01.2025
