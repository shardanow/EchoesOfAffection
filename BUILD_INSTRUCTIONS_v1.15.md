# Build Instructions - Dialogue Positioning System v1.15

## ? Pre-Build Checklist

### Files Organization

**Core Module** (DialogueSystemCore):
- ? Actors/DialogueLocationMarker (.h/.cpp) - MOVED from Runtime
- ? Subsystems/DialogueLocationSubsystem (.h/.cpp) - MOVED from Runtime
- ? Data/DialoguePositioningTypes.h
- ? Effects/DialogueEffect_PositionParticipant (.h/.cpp)
- ? Effects/DialogueEffect_PositionAIMoveTo (.h/.cpp)
- ? Effects/DialogueEffect_PositionTeleport (.h/.cpp)
- ? Effects/DialogueEffect_PositionConditionalTeleport (.h/.cpp)
- ? Core/DialogueNode.h (modified - added Positioning property)
- ? Core/DialogueRunner (.h/.cpp) (modified - added ExecuteNodePositioning)

**Runtime Module** (DialogueSystemRuntime):
- ? No changes (LocationMarker moved to Core to avoid circular dependency)

### Module Dependencies

**DialogueSystemCore.Build.cs**:
```csharp
PublicDependencyModuleNames:
- Core
- CoreUObject
- Engine
- GameplayTags
- Json
- JsonUtilities

PrivateDependencyModuleNames:
- LevelSequence
- MovieScene
- AIModule
- NavigationSystem
- Niagara
- GameEventBus (optional)
```

**DialogueSystemRuntime.Build.cs**:
```csharp
PublicDependencyModuleNames:
- Core
- CoreUObject
- Engine
- GameplayTags
- Json
- JsonUtilities
- EnhancedInput
- DialogueSystemCore
- GameEventBus

PrivateDependencyModuleNames:
- AIModule
- NavigationSystem
- Niagara
- QuestSystemRuntime (optional)
```

---

## ?? Build Steps

### Step 1: Clean Solution

```
1. Close Unreal Editor (if open)
2. Visual Studio ? Build ? Clean Solution
3. Wait for clean to complete
```

### Step 2: Delete Intermediate Files (Recommended)

```powershell
# Run in PowerShell from project root:
Remove-Item -Recurse -Force "Binaries\Win64" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "Intermediate" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "Plugins\DialogueSystem\Binaries" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "Plugins\DialogueSystem\Intermediate" -ErrorAction SilentlyContinue

Write-Host "Intermediate files cleaned!" -ForegroundColor Green
```

### Step 3: Regenerate Project Files

```
Right-click EchoesOfAffection.uproject
? "Generate Visual Studio project files"
Wait for completion
```

### Step 4: Rebuild Solution

```
Visual Studio ? Open EchoesOfAffection.sln
Build ? Rebuild Solution (Ctrl+Alt+F7)
Configuration: Development Editor
Platform: Win64
```

### Step 5: Verify Build

**Expected Output:**
```
========== Rebuild All: 2 succeeded, 0 failed, 0 skipped ==========
Build succeeded!
```

**If errors occur**, see Troubleshooting section below.

---

## ?? Troubleshooting

### Error: "Circular dependency between modules"

**Cause**: LocationMarker was initially in Runtime module  
**Solution**: Already fixed - files moved to Core module ?

### Error: "Cannot open include file 'Actors/DialogueLocationMarker.h'"

**Cause**: Old include paths in code  
**Fix**:
```cpp
// Correct (in Core module .cpp files):
#include "Actors/DialogueLocationMarker.h"

// Correct (in Runtime module .cpp files):
#include "Actors/DialogueLocationMarker.h" // Now in Core, accessible via public dependency
```

### Error: "Unresolved external symbol"

**Cause**: Missing module dependency  
**Fix**: Verify Build.cs files have all required modules (see above)

### Error: "DIALOGUESYSTEMRUNTIME_API not defined"

**Cause**: Old API macro after moving files  
**Fix**: Already fixed - changed to DIALOGUESYSTEMCORE_API ?

### Error: Niagara module not found

**Cause**: Missing Niagara dependency  
**Fix**: Already added to Build.cs files ?

### Warning: "PCH not found"

**Cause**: Precompiled header issue  
**Fix**:
```
Visual Studio ? Project Properties
? C/C++ ? Precompiled Headers
? Set to "Use Precompiled Header"
? PCH File: "CoreMinimal.h"
```

---

## ? Post-Build Verification

### 1. Launch Unreal Editor

```
Double-click EchoesOfAffection.uproject
Wait for editor to load
Check Output Log for errors
```

### 2. Verify Classes Available

```
Content Browser ? C++ Classes ? DialogueSystemCore
Should see:
  ? ADialogueLocationMarker
  ? UDialogueLocationSubsystem
  ? FDialoguePositioningTypes
  ? UDialogueEffect_PositionParticipant
  ? UDialogueEffect_PositionAIMoveTo
  ? UDialogueEffect_PositionTeleport
  ? UDialogueEffect_PositionConditionalTeleport
```

### 3. Test in Editor

```
1. Create new level or open existing
2. Place Actors ? All Classes ? Search "DialogueLocationMarker"
3. Drag DialogueLocationMarker into level
4. Set properties:
 - LocationTag: "Location.Test.Marker"
   - bShowDebugInPIE: ?
5. PIE (Play In Editor)
6. Check console: Should see marker cached in subsystem
```

### 4. Test Positioning

```
1. Open DialogueDataAsset
2. Select any dialogue node
3. Scroll to "Positioning" section
4. Should see:
   ? bEnablePositioning checkbox
   ? PrimaryPersonaPositioning struct
 ? AdditionalPersonasPositioning map
   ? All enum options (AIMoveTo, Teleport, ConditionalTeleport)
```

---

## ?? Build Statistics

**Estimated Build Time**: 2-5 minutes (depending on PC)

**Files Compiled**:
- Core Module: ~15 new files
- Modified: 3 existing files
- Total LOC: ~2000 lines

**Memory Usage**: ~2GB during compilation

---

## ?? If Build Fails

### Nuclear Option (Full Clean)

```powershell
# Close Editor and Visual Studio first!

# Delete everything
Remove-Item -Recurse -Force "Binaries" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "Intermediate" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "DerivedDataCache" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "Saved" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "Plugins\DialogueSystem\Binaries" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "Plugins\DialogueSystem\Intermediate" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "*.sln" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "*.vcxproj*" -ErrorAction SilentlyContinue

Write-Host "Full clean complete!" -ForegroundColor Green

# Regenerate
Right-click EchoesOfAffection.uproject
? Generate Visual Studio project files

# Rebuild
Open .sln ? Rebuild All
```

### Still Failing?

1. Check UE5 version: Should be 5.6
2. Check Visual Studio version: Should be 2022
3. Check Windows SDK installed
4. Verify GameplayTags plugin enabled
5. Verify all plugin dependencies exist

---

## ?? Build Configuration

**Recommended Settings**:
```
Configuration: Development Editor
Platform: Win64
Target: EchoesOfAffectionEditor
```

**For Shipping Build**:
```
Configuration: Shipping
Platform: Win64
Target: EchoesOfAffection
Note: Debug features disabled in shipping
```

---

## ? Success Criteria

Build is successful when:

? No compilation errors in Output window  
? Editor launches without crashes  
? DialogueLocationMarker appears in C++ Classes  
? Positioning section visible in DialogueNode  
? Subsystem initializes (check Output Log)  
? Test marker placement works in PIE  

---

## ?? Next Steps After Successful Build

1. ? Build complete
2. ?? Read quick start: `DialoguePositioning_QuickStart.md`
3. ?? Test system in project
4. ?? Create gameplay tags for locations
5. ?? Set up markers in levels
6. ?? Configure positioning in dialogues

---

**Build Version**: v1.15  
**Last Updated**: 2025  
**Status**: Ready to build ?

**If build succeeds - система готова к использованию!** ??
