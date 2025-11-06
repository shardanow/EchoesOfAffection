# Fixing Question Mark Symbols (?) in Data Assets

## Problem

You see **question mark symbols (?)** in Data Asset tooltips and details because:

1. **Data Assets were created with Russian text** in Unreal Editor
2. **This text is saved in .uasset binary files** (not in C++ code)
3. **C++ fixes don't affect existing assets** - only new code tooltips

## Solution

You must **manually fix each Data Asset** in Unreal Editor.

---

## Step-by-Step Guide

### Method 1: Find and Fix in Content Browser

```
1. Open Unreal Editor
2. Open Content Browser (Ctrl+Space)
3. Navigate to: Content/Game/Core/Subsystems/
4. Find Data Assets with ? symbols
5. Double-click to open
6. Fix text fields (DisplayName, Description, Tooltip)
7. Save (Ctrl+S)
8. Repeat for each asset
```

### Method 2: Use Search Filter

```
1. In Content Browser, click "Filters"
2. Select "Data Asset"
3. Browse all DA_* files
4. Open each one
5. Replace Russian text ? English
6. Save all changes
```

---

## List of Data Assets to Check

### DialogueSystem:
- **DA_Digalogue_Lianne_Greeting** (Content/Game/Data/Dialogues/)
  - Check: Node text, choice text, speaker names

### RelationshipSystem (7 files):
Location: `Content/Game/Core/Subsystems/RelationshipSystem/`

1. **DA_Action_Talk**
   - Fields: DisplayName, Description, Tooltip
   
2. **DA_Dimension_Friendship**
   - Fields: DisplayName, Min/Max labels, Tooltip
   
3. **DA_Dimension_Trust**
   - Fields: DisplayName, Tooltip
   
4. **DA_ProfileFriendly**
   - Fields: ProfileName, Description
   
5. **DA_ProfilePlayer**
   - Fields: ProfileName, Description
   
6. **DA_StateFriend**
   - Fields: StateName, Description, Tooltip
   
7. **DA_StateStranger**
   - Fields: StateName, Description, Tooltip
   
8. **DA_TraitForgiving**
   - Fields: TraitName, Description, Tooltip
   
9. **DA_ReleationshipDatabase**
   - Fields: DatabaseName, Description

### ScheduleSystem (5 files):
Location: `Content/Game/Core/Subsystems/ScheduleManager/`

1. **DA_AliceSchedule**
2. **DA_Alice_Afternoon**
3. **DA_Alice_Morning**
4. **DA_LienneSchedule**
5. **DA_Lienne_Morning**

Fields to check:
- Schedule Name
- Entry descriptions
- Action names
- Location descriptions

### QuestSystem (2 files):
Location: `Content/Game/Core/Subsystems/QuestSystem/`

1. **DA_Quest_Apples**
2. **DA_Quest_First**

Fields to check:
- Quest Name
- Quest Description
- Objective Text
- Reward Text

### Other Systems:
- **DA_NeedsSettings** - Needs system config
- **DA_Need_Energy** - Energy need
- **DA_Need_Hunger** - Hunger need
- **DA_TimeManager_Settings** - Time of day settings
- **DA_Apple** - Item asset
- **DA_RestoreHunger** - Effect asset

---

## Common Fields with Russian Text

When you open a Data Asset, look for these fields:

### UDataAsset Common:
- **DisplayName** (shown in editor)
- **Description** (tooltip)
- **Category** (organization)

### RelationshipDimension:
- **Dimension Name**
- **Min Value Label** (e.g., "Враг" ? "Enemy")
- **Max Value Label** (e.g., "Друг" ? "Friend")
- **Description**

### RelationshipState:
- **State Name** (e.g., "Друг" ? "Friend")
- **Description** (e.g., "Дружеские отношения" ? "Friendly relationship")

### RelationshipAction:
- **Action Name** (e.g., "Поговорить" ? "Talk")
- **Description**

### ScheduleEntry:
- **Entry Name**
- **Display Text** (shown in UI)
- **Action Description**

### Dialogue Node:
- **Dialogue Text** (the actual dialogue)
- **Choice Text** (player choice button)
- **Speaker Name**

---

## Quick Fix Template

For each Data Asset:

### Before (Russian):
```
DisplayName: "Дружба"
Description: "Измерение дружеских отношений"
```

### After (English):
```
DisplayName: "Friendship"
Description: "Dimension measuring friendly relationships"
```

---

## Translation Guide

Common Russian ? English translations:

| Russian | English |
|---------|---------|
| Дружба | Friendship |
| Доверие | Trust |
| Друг | Friend |
| Враг | Enemy |
| Незнакомец | Stranger |
| Поговорить | Talk |
| Расписание | Schedule |
| Утро | Morning |
| День | Afternoon/Day |
| Вечер | Evening |
| Ночь | Night |
| Квест | Quest |
| Задание | Objective |
| Награда | Reward |
| Голод | Hunger |
| Энергия | Energy |
| Предмет | Item |

---

## Automated Approach (Advanced)

**WARNING:** .uasset files are binary and can't be easily edited with scripts!

If you have MANY assets to fix, you can:

1. **Export to JSON** (if you set up serialization)
2. **Use Python Editor Utility**
3. **Mass edit in Unreal Editor**

### Python Script Example:

```python
import unreal

# Get all Data Assets
asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
all_assets = asset_registry.get_assets_by_class("DataAsset", True)

for asset_data in all_assets:
    asset_path = str(asset_data.package_name)
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    
    # Check if has DisplayName property (example)
    if hasattr(asset, 'display_name'):
  current_name = asset.get_editor_property('display_name')
  # Replace Russian text here
      # asset.set_editor_property('display_name', new_name)
   # unreal.EditorAssetLibrary.save_asset(asset_path)
```

**NOTE:** This requires Unreal Engine Python plugin enabled.

---

## Best Practice for Future

To prevent this issue:

1. **Always use English** in Data Assets
2. **Use Localization system** for translated text
3. **Set Editor language to English** (Edit ? Editor Preferences ? Region & Language)
4. **Check tooltips before committing** assets to Git

---

## After Fixing

1. **Save all assets** (Ctrl+Shift+S)
2. **Test in Editor** - open a few assets and verify no ?
3. **Commit to Git**:
   ```sh
   git add Content/
   git commit -m "Fix: Replace Russian text with English in Data Assets"
   git push
   ```

---

## Quick Checklist

- [ ] Open Unreal Editor
- [ ] Navigate to Content/Game/Core/Subsystems/
- [ ] Open each DA_* file
- [ ] Check all text fields for ? symbols
- [ ] Replace with English text
- [ ] Save each asset
- [ ] Test tooltips
- [ ] Commit changes

---

**Estimated Time:** 30-60 minutes (depending on amount of text)

**Status:** Ready to fix in Unreal Editor
**Priority:** HIGH (affects user experience and readability)

---

Created: 2025-01-03
Last Updated: 13:55
