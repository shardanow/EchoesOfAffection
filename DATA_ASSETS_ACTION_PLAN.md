# DATA ASSETS ENCODING FIX - ACTION PLAN

## ?? CRITICAL ISSUE IDENTIFIED

**Problem:** You see **question mark symbols (?)** in Data Asset tooltips and details panels.

**Root Cause:** 
- Data Assets (.uasset files) contain **Russian text saved in binary format**
- C++ code fixes DON'T affect existing .uasset files
- Assets must be fixed **manually in Unreal Editor**

---

## ?? AFFECTED ASSETS

Found **28 Data Assets** that may contain Russian text:

### ?? HIGH PRIORITY (User-Facing):

#### DialogueSystem (1 file):
- `DA_Digalogue_Lianne_Greeting` - Dialogue text visible to player

#### RelationshipSystem (9 files):
- `DA_Dimension_Friendship` - Shows in UI
- `DA_Dimension_Trust` - Shows in UI
- `DA_StateFriend` - Shows in tooltips
- `DA_StateStranger` - Shows in tooltips
- `DA_TraitForgiving` - Shows in tooltips
- `DA_Action_Talk` - Shows in UI
- `DA_ProfileFriendly` - NPC profile
- `DA_ProfilePlayer` - Player profile
- `DA_ReleationshipDatabase` - Database name

#### ScheduleSystem (5 files):
- `DA_AliceSchedule` - Schedule visible in debug
- `DA_Alice_Morning` - Entry names
- `DA_Alice_Afternoon` - Entry names
- `DA_LienneSchedule` - Schedule visible in debug
- `DA_Lienne_Morning` - Entry names

#### QuestSystem (2 files):
- `DA_Quest_Apples` - Quest text visible to player
- `DA_Quest_First` - Quest text visible to player

### ?? MEDIUM PRIORITY (Editor/Debug):

#### Other Systems (11 files):
- `DA_NeedsSettings`, `DA_Need_Energy`, `DA_Need_Hunger`
- `DA_TimeManager_Settings`
- `DA_Apple`, `DA_RestoreHunger`
- `DT_AnimeMaterials`, `DT_SoundScape`
- `DT_ItemsTags`, `DT_NavigationTags`, `DT_QuestTags`

---

## ? TWO SOLUTIONS

### SOLUTION 1: Manual Fix in Editor (RECOMMENDED)

**Time:** 30-60 minutes  
**Risk:** LOW  
**Quality:** HIGH

**Steps:**
```
1. Open EchoesOfAffection.uproject
2. Content Browser ? Game/Core/Subsystems/
3. Double-click each DA_* file
4. Find fields with ? symbols
5. Replace Russian text ? English
6. Save (Ctrl+S)
7. Repeat for all 28 assets
```

**Use:** `FIX_DATA_ASSETS_GUIDE.md` for detailed instructions

---

### SOLUTION 2: Python Script (ADVANCED)

**Time:** 10-15 minutes (if script works)  
**Risk:** MEDIUM (could corrupt assets if not careful)  
**Quality:** HIGH (consistent naming)

**Requirements:**
- Unreal Engine Python Plugin enabled
- Python knowledge for customization

**Steps:**
```
1. Enable Python plugin in Unreal Editor
2. Window ? Developer Tools ? Python Console
3. Run fix_data_assets.py script
4. Review scan results
5. Apply fixes (uncomment apply_known_fixes())
6. Verify all assets
```

**Use:** `fix_data_assets.py` script

---

## ?? QUICK START CHECKLIST

### Before You Start:
- [ ] Backup your Content/ folder
- [ ] Commit current state to Git
- [ ] Open Unreal Editor
- [ ] Close all Data Assets (if any are open)

### Manual Fix Process:
- [ ] Read `FIX_DATA_ASSETS_GUIDE.md`
- [ ] Start with Dialogue assets (user-facing)
- [ ] Fix Relationship assets (UI tooltips)
- [ ] Fix Schedule assets (debug info)
- [ ] Fix Quest assets (player text)
- [ ] Fix remaining system assets
- [ ] Test each asset after fixing

### Verification:
- [ ] Open fixed assets - no ? symbols
- [ ] Check tooltips in Details panel
- [ ] Test in PIE (Play In Editor)
- [ ] Verify UI displays correctly
- [ ] Save all changes (Ctrl+Shift+S)

### Commit to Git:
- [ ] `git add Content/`
- [ ] `git commit -m "Fix: Replace Russian text in Data Assets"`
- [ ] `git push origin master`

---

## ?? PRIORITY ORDER

Fix in this order for maximum impact:

1. **DA_Digalogue_Lianne_Greeting** (player sees this)
2. **DA_Quest_Apples, DA_Quest_First** (player quests)
3. **DA_Dimension_Friendship, DA_Dimension_Trust** (UI visible)
4. **DA_StateFriend, DA_StateStranger** (tooltips)
5. **Schedule assets** (debug/testing)
6. **Remaining assets** (lower priority)

---

## ?? COMMON TRANSLATIONS

| Russian | English | Context |
|---------|---------|---------|
| Дружба | Friendship | Dimension name |
| Доверие | Trust | Dimension name |
| Друг | Friend | State name |
| Незнакомец | Stranger | State name |
| Прощающий | Forgiving | Trait name |
| Поговорить | Talk | Action name |
| Расписание | Schedule | Schedule name |
| Утро | Morning | Time period |
| День | Afternoon | Time period |
| Квест | Quest | Quest name |
| Яблоки | Apples | Item name |
| Голод | Hunger | Need name |
| Энергия | Energy | Need name |

---

## ?? IMPORTANT NOTES

### What This Fixes:
- ? Text visible in Data Assets
- ? Tooltips in Details panel
- ? UI element names
- ? Editor display names

### What This DOESN'T Fix:
- ? Blueprint variable names (fix separately if needed)
- ? Level actor names (rename in World Outliner)
- ? Widget text (fix in Widget Blueprint)
- ? Localization strings (use Localization Dashboard)

---

## ?? AFTER FIXING

### Test Checklist:
```
1. Open Content Browser
2. Navigate to each fixed asset
3. Double-click to open
4. Verify all text fields are English
5. Check tooltip when hovering in Details panel
6. Test in PIE - UI should show correct text
7. Check debug logs - no ? symbols
```

### Expected Result:
- ? All Data Assets display English text
- ? No ? symbols anywhere
- ? Tooltips readable in Editor
- ? UI displays correctly in game
- ? Debug logs show proper names

---

## ?? NEED HELP?

### If Python script fails:
- Check Python plugin is enabled
- Check asset paths are correct
- Try manual fix instead

### If manual fix is tedious:
- Use Python script
- Or create Excel template for batch processing

### If assets become corrupted:
- Restore from Git: `git checkout HEAD -- Content/`
- Restore from backup folder

---

## ?? TIMELINE

**Estimated Time:**
- Manual fix: 30-60 minutes
- Python fix: 10-15 minutes (+ setup time)
- Testing: 10-15 minutes
- **Total: 40-90 minutes**

**Recommended Session:**
1. Backup & commit (5 min)
2. Fix high-priority assets (20 min)
3. Test in Editor (10 min)
4. Fix remaining assets (20 min)
5. Final verification (10 min)
6. Commit to Git (5 min)

---

**Status:** READY TO FIX IN EDITOR  
**Priority:** HIGH  
**Impact:** User Experience + Developer Workflow

**Next Action:** Open Unreal Editor and start with `DA_Digalogue_Lianne_Greeting`

---

Created: 2025-01-03 14:00  
Files: `FIX_DATA_ASSETS_GUIDE.md`, `fix_data_assets.py`
