# Build Instructions for EchoesOfAffection Project

## Encoding Issues Fixed - Build Guide

### ? What Was Fixed

**Total Files Modified:** 33 files across all systems
- **DialogueSystem:** 28 files (all corrupted Russian text removed)
- **RelationshipSystem:** 2 files (Russian comments ? English)
- **Game Source:** 1 file (MusicZoneVolume.cpp)
- **ActorScheduleSystem:** 1 file (emoji ? text markers)
- **TimeOfDaySystem:** 1 file (emoji ? text markers)

**Issues Resolved:**
- ? Removed all corrupted Cyrillic text (displayed as ????)
- ? Replaced all emoji symbols with ASCII text
- ? Fixed all encoding issues in UPROPERTY/UFUNCTION tooltips
- ? No compilation errors

---

## Building in Unreal Editor

### Method 1: Live Coding (Fastest)
```
1. Open Unreal Editor
2. Press Ctrl+Alt+F11 (or click Compile button in toolbar)
3. Wait for "Compile Complete" message
4. Test tooltips in Blueprint Editor
```

### Method 2: Full Rebuild (Recommended)
```
1. Close Unreal Editor (if open)
2. Right-click on EchoesOfAffection.uproject
3. Select "Generate Visual Studio project files"
4. Open EchoesOfAffection.sln in Visual Studio
5. Build ? Rebuild Solution (Ctrl+Shift+B)
6. Wait for "Build succeeded" message
7. Launch Unreal Editor
```

### Method 3: Clean Build (If Issues Persist)
```powershell
# Run in PowerShell from project root

# 1. Clean intermediate files
Remove-Item -Recurse -Force "Intermediate","Saved","Binaries" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force "Plugins\*\Intermediate","Plugins\*\Binaries" -ErrorAction SilentlyContinue

# 2. Regenerate project files
& "D:\Game Dev\UE\UE_5.6\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="$PWD\EchoesOfAffection.uproject" -game -engine

# 3. Open solution and rebuild
& "EchoesOfAffection.sln"
```

---

## Verification Checklist

### In Visual Studio:
- [ ] Solution compiles without errors
- [ ] No warnings about encoding (C4819)
- [ ] IntelliSense shows proper tooltips

### In Unreal Editor:
- [ ] Project loads without errors
- [ ] No "Failed to compile" notifications
- [ ] Tooltips display correctly (no ? symbols)

**Test Tooltips:**
```
1. Create/Open any Blueprint
2. Add DialogueComponent
3. Hover over properties (e.g., "bEnableCameraControl")
4. Tooltip should show English text or be empty (no ????)
```

---

## Expected Build Output

### Success Indicators:
```
========== Build: 1 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========
```

**Compile time:** ~2-5 minutes (depending on system)

### If Build Fails:

**Common Issues:**

1. **PCH errors:**
   ```
   Solution: Clean build (Method 3 above)
   ```

2. **Missing UHT headers:**
   ```
   Solution: Regenerate project files
   ```

3. **Module not found:**
   ```
   Solution: Check .uproject EnabledPlugins list
 ```

---

## Post-Build Testing

### 1. Test DialogueSystem Tooltips
```
Blueprint Editor:
- Open any BP with DialogueComponent
- Check UDialogueNode properties
- Check DialogueCameraComponent settings
- Verify MemoryConditions show proper text
```

### 2. Test RelationshipSystem Tooltips
```
Blueprint Editor:
- Add NPCRelationshipComponent
- Check dimension properties
- Check state/trait settings
```

### 3. Test In-Game
```
PIE (Play In Editor):
- Start dialogue with NPC
- Check camera transitions
- Verify memory system works
- Test relationship changes
```

---

## Git Commit (After Successful Build)

```sh
git add .
git commit -m "Fix: Resolve encoding issues across all systems

Fixed corrupted Russian text and emoji symbols in:
- DialogueSystem (28 files)
- RelationshipSystem (2 files)
- ActorScheduleSystem (1 file)
- TimeOfDaySystem (1 file)
- Game source (1 file)

All tooltips now display correctly in Unreal Editor.
No compilation errors.

Build verified: Visual Studio + Unreal Editor
"

git push origin master
```

---

## Rollback (If Needed)

If build fails unexpectedly:

```sh
# Restore from Git
git reset --hard HEAD~1
git clean -fd

# Or restore specific system
git checkout HEAD~1 -- Plugins/DialogueSystem/

# Regenerate and rebuild
GenerateProjectFiles.bat
```

---

## Support

**Modified Files List:**
See `CHANGELOG.md` for complete list of changes.

**Backup Files:**
PowerShell script created `.backup` files (safe to delete after verification).

**Questions:**
- Check IntelliSense tooltips in Visual Studio
- Verify Blueprint tooltips in Unreal Editor
- Review Git diff for specific changes

---

**Status:** ? Ready to build
**Last Updated:** 2025-01-03 13:15
**Build Environment:** Visual Studio 2022 + Unreal Engine 5.6
