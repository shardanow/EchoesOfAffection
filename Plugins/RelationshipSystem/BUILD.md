# Build Instructions for RelationshipSystem Plugin

## Prerequisites

- Unreal Engine 5.6 installed
- Visual Studio 2022 with C++ workload
- Project: EchoesOfAffection

## Method 1: Build via Unreal Editor (Recommended for first build)

1. **Close Unreal Editor** if it's running

2. **Right-click** on `EchoesOfAffection.uproject`

3. Select **"Generate Visual Studio project files"**

4. Open `EchoesOfAffection.sln` in Visual Studio 2022

5. Set build configuration:
   - Configuration: `Development Editor`
   - Platform: `Win64`

6. **Build** the solution:
   - Right-click on `EchoesOfAffection` project
   - Click **"Build"**

7. **Expected output:**
   ```
   Build succeeded
   RelationshipSystemCore compiled
   RelationshipSystemUI compiled
   RelationshipSystemEditor compiled
   ```

8. **Open** Unreal Editor

9. **Enable plugin:**
   - Edit > Plugins
   - Search "Relationship"
   - Enable "Relationship System"
   - Restart Editor

## Method 2: Build via Command Line

```powershell
# Navigate to project directory
cd "D:\Game Dev\Projects\UEProjects\EchoesOfAffection"

# Generate project files
"D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="EchoesOfAffection.uproject" -game

# Build
"D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\Build.bat" EchoesOfAffectionEditor Win64 Development "D:\Game Dev\Projects\UEProjects\EchoesOfAffection\EchoesOfAffection.uproject"
```

## Method 3: Hot Reload (After first successful build)

If you modify plugin code:

1. Keep Editor open
2. Build in Visual Studio (Ctrl+Shift+B)
3. Editor will detect changes and offer hot reload
4. Click **"Yes"** to hot reload

**Note:** Hot reload doesn't work for:
- New classes
- New UPROPERTY/UFUNCTION
- Module changes

In these cases, close Editor and do full rebuild.

## Troubleshooting

### Error: "Cannot open include file"

**Solution:**
1. Regenerate project files
2. Clean solution (Build > Clean Solution)
3. Rebuild

### Error: "Unresolved external symbol"

**Possible causes:**
1. Missing module in Build.cs dependencies
2. Function not implemented

**Check:**
- All .cpp files are created
- All virtual functions are implemented
- Build.cs has all required modules

### Error: LNK2019 or LNK2001

**Solution:**
```powershell
# Clean intermediate files
Remove-Item -Recurse -Force ".\Intermediate"
Remove-Item -Recurse -Force ".\Binaries"
Remove-Item -Recurse -Force ".\Plugins\RelationshipSystem\Intermediate"
Remove-Item -Recurse -Force ".\Plugins\RelationshipSystem\Binaries"

# Regenerate
"D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="EchoesOfAffection.uproject" -game

# Rebuild
```

### Warning: "No matching implementation for virtual function"

**Solution:** Check that all virtual functions in headers have corresponding .cpp implementation.

### Plugin not showing in Editor

**Solution:**
1. Check `.uplugin` file is valid JSON
2. Verify plugin is in `Plugins/RelationshipSystem/` directory
3. Restart Editor
4. Regenerate project files if needed

## Verification Steps

After successful build, verify:

### 1. Check Binaries Created

```
Plugins/RelationshipSystem/Binaries/Win64/
  ??? UE5-RelationshipSystemCore.dll
  ??? UE5-RelationshipSystemUI.dll
  ??? UE5-RelationshipSystemEditor.dll
```

### 2. Check Plugin Loads

In Editor:
- Edit > Plugins
- Search "Relationship"
- Should show "Relationship System" plugin
- Status: Enabled

### 3. Create Test Data Asset

1. Content Browser > Right-click
2. Miscellaneous > Data Asset
3. Should see:
   - RelationshipDimension
   - RelationshipState
   - RelationshipTrait
   - RelationshipRule
   - RelationshipAction
   - RelationshipProfile
   - RelationshipDatabase

### 4. Check Project Settings

1. Edit > Project Settings
2. Plugins section
3. Should see "Relationship System"

### 5. Add Component to Actor

1. Create or open Blueprint Actor
2. Add Component
3. Search "Relationship"
4. Should find "Relationship Component"

## Expected Build Time

- **First build:** 2-5 minutes (depending on PC)
- **Incremental:** 10-30 seconds
- **Hot reload:** 5-10 seconds

## Build Output

Successful build shows:

```
------ Build started: Project: EchoesOfAffection, Configuration: Development Editor x64 ------
Building EchoesOfAffectionEditor...
Using Visual Studio 2022 14.38.33130 toolchain (D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130) and Windows 10.0.22621.0 SDK (D:\Program Files (x86)\Windows Kits\10)
Building 6 actions with 12 processes...
[1/6] Module.RelationshipSystemCore.cpp
[2/6] Module.RelationshipSystemUI.cpp
[3/6] Module.RelationshipSystemEditor.cpp
[4/6] RelationshipSystemCore.cpp.obj
[5/6] RelationshipSystemUI.cpp.obj
[6/6] EchoesOfAffectionEditor.exe
Total build time: 45.32 seconds
========== Build: 1 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========
```

## Next Steps After Successful Build

1. **Configure Project Settings:**
   - Create Database asset
- Set in Project Settings > Relationship System

2. **Create Example Data Assets:**
- Follow `Content/Examples/README.md`
   - Create dimensions, states, profiles

3. **Add to GameMode:**
   - Subsystem auto-loads (no setup needed)
   - Add RelationshipComponent to player/NPCs

4. **Test in PIE:**
   - Place actors with components
   - Test relationship modifications
   - Check events fire

## Common Build Warnings (Safe to Ignore)

```
warning C4263: member function does not override any base class virtual member function
warning C4264: no override available for virtual member function from base
```

These are UE5 warnings and don't affect functionality.

## Performance Notes

Plugin compiles with:
- `/O2` optimization in Shipping
- `/Od` (debug) in Development
- `/Ox` (full optimization) in Shipping

Expected DLL sizes:
- Development: ~2-4 MB per module
- Shipping: ~500 KB - 1 MB per module

## Integration Testing

After build, test integration:

### Test 1: Component Registration
```cpp
// In any Actor BeginPlay
UNPCRelationshipComponent* RelComp = FindComponentByClass<UNPCRelationshipComponent>();
if (RelComp)
{
    UE_LOG(LogTemp, Log, TEXT("Component found and registered"));
}
```

### Test 2: Subsystem Access
```cpp
// In GameMode or any Actor
URelationshipSubsystem* Subsystem = GetGameInstance()->GetSubsystem<URelationshipSubsystem>();
if (Subsystem)
{
    UE_LOG(LogTemp, Log, TEXT("Subsystem loaded: %d relationships"), Subsystem->GetRelationshipCount());
}
```

### Test 3: Data Asset Creation
1. Create `DA_Dimension_Test`
2. Set Asset Tag: `Relationship.Dimension.Test`
3. Set Min/Max values
4. Should save without errors

## Support

If build fails:
1. Check error log
2. Verify all files created
3. Check Build.cs dependencies
4. Clean and rebuild
5. Regenerate project files

For persistent issues, check:
- UE5 version (must be 5.6)
- Visual Studio version (2022 recommended)
- Windows SDK installed
- C++14 support enabled
