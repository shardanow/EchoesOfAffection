# Building and Compiling the Time of Day System

## Prerequisites

- Unreal Engine 5.6+
- Visual Studio 2022 (or compatible C++ compiler)
- Windows 10/11 (or Mac/Linux with appropriate tools)
- Project: EchoesOfAffection

## Step 1: Verify Plugin Structure

Ensure all files are in place:

```
? Plugins/TimeOfDaySystem/TimeOfDaySystem.uplugin
? Plugins/TimeOfDaySystem/Source/TimeOfDayCore/TimeOfDayCore.Build.cs
? Plugins/TimeOfDaySystem/Source/TimeOfDayRuntime/TimeOfDayRuntime.Build.cs
? Plugins/TimeOfDaySystem/Source/TimeOfDayUI/TimeOfDayUI.Build.cs
? All .h and .cpp files in respective Public/Private folders
```

## Step 2: Regenerate Project Files

### Option A: Using .uproject file
1. Close Visual Studio if open
2. Close Unreal Editor if open
3. Right-click `EchoesOfAffection.uproject`
4. Select "Generate Visual Studio project files"
5. Wait for completion

### Option B: Using UnrealBuildTool
```batch
cd "D:\Game Dev\Projects\UEProjects\EchoesOfAffection"
"D:\Game Dev\UE\UE_5.6\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="EchoesOfAffection.uproject" -game -rocket -progress
```

## Step 3: Open in Visual Studio

1. Open `EchoesOfAffection.sln`
2. Verify that TimeOfDaySystem modules appear in Solution Explorer:
   - Games/EchoesOfAffection/Plugins/TimeOfDaySystem/TimeOfDayCore
   - Games/EchoesOfAffection/Plugins/TimeOfDaySystem/TimeOfDayRuntime
   - Games/EchoesOfAffection/Plugins/TimeOfDaySystem/TimeOfDayUI

## Step 4: Build the Project

### Configuration: Development Editor

1. Set Configuration to **Development Editor**
2. Set Platform to **Win64**
3. Right-click on **EchoesOfAffection** project
4. Select **Build**

OR use keyboard shortcut: **Ctrl + Shift + B**

### Expected Build Output

```
1>------ Build started: Project: TimeOfDayCore, Configuration: Development_Editor x64 ------
1>  Building TimeOfDayCore...
1>  TimeOfDayCore.cpp
1>  TimeOfDaySettings.cpp
1>  Creating library...
1>  TimeOfDayCore.lib created successfully

2>------ Build started: Project: TimeOfDayRuntime, Configuration: Development_Editor x64 ------
2>  Building TimeOfDayRuntime...
2>  TimeOfDaySubsystem.cpp
2>  TimeOfDayListenerComponent.cpp
2>  TimeOfDayBlueprintLibrary.cpp
2>  TimeDebugActor.cpp
2>  TimeOfDayExampleActor.cpp
2>  Creating library...
2>  TimeOfDayRuntime.lib created successfully

3>------ Build started: Project: TimeOfDayUI, Configuration: Development_Editor x64 ------
3>  Building TimeOfDayUI...
3>  TimeOfDayWidget.cpp
3>  TimeClockWidget.cpp
3>  Creating library...
3>  TimeOfDayUI.lib created successfully

4>------ Build started: Project: EchoesOfAffection, Configuration: Development_Editor x64 ------
4>  Building EchoesOfAffection...
4>  Linking...
4>  EchoesOfAffection-Win64-Development.dll created successfully

========== Build: 4 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========
```

## Step 5: Verify in Unreal Editor

1. Launch Unreal Editor
2. Open EchoesOfAffection project
3. Go to **Edit ? Plugins**
4. Search for "Time of Day"
5. Verify plugin is listed and enabled

## Troubleshooting

### Issue: Module not found

**Error**: `Plugin 'TimeOfDaySystem' failed to load because module 'TimeOfDayCore' could not be found`

**Solution**:
1. Verify `.Build.cs` files are correct
2. Regenerate project files
3. Clean and rebuild solution

### Issue: Unresolved external symbols

**Error**: `LNK2019: unresolved external symbol`

**Solution**:
1. Check module dependencies in `.Build.cs`
2. Verify all `.cpp` files are included in compilation
3. Check `#include` statements

### Issue: Cannot open include file

**Error**: `C1083: Cannot open include file: 'TimeOfDayTypes.h'`

**Solution**:
1. Verify file exists in `Public/Core/` folder
2. Check include paths in `.Build.cs`
3. Regenerate project files

### Issue: Interface compilation errors

**Error**: `Pure virtual function has no implementation`

**Solution**:
1. Verify `_Implementation` suffix on interface methods
2. Check UINTERFACE vs IInterface syntax
3. Ensure `GENERATED_BODY()` is present

### Issue: Blueprint node not appearing

**Problem**: Blueprint functions not visible in editor

**Solution**:
1. Verify `UFUNCTION(BlueprintCallable)` or `UFUNCTION(BlueprintPure)`
2. Ensure `meta=(WorldContext)` is set where needed
3. Hot reload or restart editor

## Build Configurations

### Development Editor (Recommended for development)
- Full debugging symbols
- Editor functionality
- Moderate optimization
```
Configuration: Development Editor
Platform: Win64
```

### Debug Editor (For debugging)
- Maximum debugging symbols
- No optimization
- Slowest performance
```
Configuration: DebugGame Editor
Platform: Win64
```

### Shipping (For final builds)
- Full optimization
- No editor
- No debugging symbols
```
Configuration: Shipping
Platform: Win64
```

## Hot Reload

After making code changes, you can hot reload without closing editor:

1. Make changes in Visual Studio
2. Build solution (Ctrl + Shift + B)
3. In Unreal Editor, click **Compile** button
4. OR: Go to **File ? Refresh Visual Studio Project**

**Note**: Hot reload works for most changes but may require full restart for:
- New classes
- Interface changes
- Module dependencies changes

## Performance Validation

After successful build, run these checks:

### 1. Launch Editor Performance
```
1. Open editor
2. Enable: Window ? Developer Tools ? Output Log
3. Search for "TimeOfDay"
4. Verify initialization logs
5. Check for warnings/errors
```

### 2. PIE (Play In Editor) Test
```
1. Create new level
2. Place BP_TimeDebugActor
3. Hit Play (Alt + P)
4. Verify time progression
5. Check console for errors
```

### 3. Blueprint Compilation
```
1. Create new Blueprint
2. Add TimeOfDayListenerComponent
3. Compile blueprint
4. Verify no errors
```

### 4. Packaging Test (Optional)
```
1. File ? Package Project ? Windows ? Windows (64-bit)
2. Wait for packaging
3. Run packaged game
4. Verify time system works
```

## Continuous Integration (CI) Setup

For automated builds, add to your CI pipeline:

```yaml
# Example GitHub Actions workflow
name: Build Time of Day System

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v2
  
      - name: Setup UE5
        # Setup Unreal Engine 5
        
      - name: Generate Project Files
        run: |
UnrealBuildTool.exe -projectfiles -project="EchoesOfAffection.uproject"
          
      - name: Build Plugin
   run: |
    MSBuild.exe EchoesOfAffection.sln /t:TimeOfDayCore;TimeOfDayRuntime;TimeOfDayUI /p:Configuration="Development Editor"

      - name: Run Tests
     # Run automated tests
```

## Build Script (Batch File)

Create `BuildTimeOfDaySystem.bat`:

```batch
@echo off
echo ========================================
echo Building Time of Day System Plugin
echo ========================================

set UE_ROOT=D:\Game Dev\UE\UE_5.6
set PROJECT_ROOT=D:\Game Dev\Projects\UEProjects\EchoesOfAffection
set PROJECT_FILE=%PROJECT_ROOT%\EchoesOfAffection.uproject

echo.
echo Step 1: Generating project files...
"%UE_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="%PROJECT_FILE%" -game -rocket -progress

echo.
echo Step 2: Building plugin modules...
"%UE_ROOT%\Engine\Build\BatchFiles\Build.bat" EchoesOfAffection Win64 Development "%PROJECT_FILE%" -waitmutex

echo.
echo ========================================
echo Build Complete!
echo ========================================
pause
```

## Version Control Integration

### .gitignore additions
```gitignore
# Time of Day System binaries
Plugins/TimeOfDaySystem/Binaries/
Plugins/TimeOfDaySystem/Intermediate/

# Keep source files
!Plugins/TimeOfDaySystem/Source/
```

### Files to commit
```
? All .h and .cpp files
? All .Build.cs files
? TimeOfDaySystem.uplugin
? All documentation (*.md)
? Binaries folder
? Intermediate folder
? .vs folder
```

## Build Metrics

### Expected Build Times
- Clean build: 30-60 seconds
- Incremental build: 5-15 seconds
- Hot reload: 2-5 seconds

### Output Size
- TimeOfDayCore.lib: ~500 KB
- TimeOfDayRuntime.lib: ~1.5 MB
- TimeOfDayUI.lib: ~800 KB
- Total: ~3 MB

## Next Steps After Successful Build

1. ? Create DataAsset: `DA_TimeOfDaySettings`
2. ? Follow QUICKSTART.md for setup
3. ? Test in Play In Editor (PIE)
4. ? Create example actors
5. ? Integrate with your game systems

## Getting Help

If you encounter build errors:

1. **Check Output Log**: Look for specific error messages
2. **Verify Files**: Ensure all files are present and correct
3. **Clean Build**: Delete Binaries and Intermediate folders, rebuild
4. **Check Documentation**: Review README.md and ARCHITECTURE.md
5. **Community**: Search UE5 forums for similar issues

## Useful Visual Studio Shortcuts

- **Ctrl + Shift + B**: Build solution
- **F7**: Build current project
- **Ctrl + Break**: Cancel build
- **F5**: Start debugging
- **Ctrl + F5**: Start without debugging
- **Ctrl + Shift + F**: Find in all files

---

**Build Status**: Ready to compile ?
**Estimated Build Time**: 30-60 seconds (clean build)
**Compatibility**: UE 5.6+

Happy building! ??
