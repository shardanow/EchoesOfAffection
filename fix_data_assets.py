"""
Unreal Engine Python Script: Fix Russian Text in Data Assets

This script finds all Data Assets and reports which ones might have Russian text.
You can extend it to automatically replace text, but be careful with .uasset files!

REQUIREMENTS:
- Unreal Engine Python Plugin enabled
- Run from Unreal Editor Python console or as Editor Utility

USAGE:
1. Open Unreal Editor
2. Window ? Developer Tools ? Python Console
3. Copy-paste this script OR
4. File ? Execute Python Script ? select this file
"""

import unreal
import re

# Russian Cyrillic detection pattern
RUSSIAN_PATTERN = re.compile(r'[Р-пр-џЈИ]')

def has_russian_text(text):
    """Check if string contains Russian characters"""
    if not text:
        return False
    return bool(RUSSIAN_PATTERN.search(str(text)))

def check_data_asset(asset_path):
    """Check a single Data Asset for Russian text"""
    try:
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
          return None
  
    issues = []
        
 # Get all properties
    properties = dir(asset)
        
    # Common text properties to check
     text_properties = [
      'display_name',
  'description',
       'tooltip',
  'category',
        'name',
   'title',
            'text'
        ]
        
        for prop_name in text_properties:
         if hasattr(asset, prop_name):
      try:
     value = asset.get_editor_property(prop_name)
         if has_russian_text(str(value)):
            issues.append(f"{prop_name}: {value}")
       except:
         pass
  
return issues if issues else None
        
    except Exception as e:
 unreal.log_warning(f"Error checking {asset_path}: {e}")
    return None

def scan_all_data_assets():
    """Scan all Data Assets for Russian text"""
    unreal.log("=== SCANNING DATA ASSETS FOR RUSSIAN TEXT ===")
    unreal.log("")
    
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
    
    # Find all Data Assets
    filter = unreal.ARFilter(
  class_names=["DataAsset"],
        recursive_paths=True
    )
    
    all_assets = asset_registry.get_assets(filter)
    
    unreal.log(f"Found {len(all_assets)} Data Assets")
    unreal.log("")
    
    problematic_assets = []
    
    for asset_data in all_assets:
 asset_path = str(asset_data.package_name)
        asset_name = str(asset_data.asset_name)
        
        issues = check_data_asset(asset_path)
    
        if issues:
            problematic_assets.append({
         'path': asset_path,
        'name': asset_name,
     'issues': issues
  })
            
            unreal.log_warning(f"[RUSSIAN TEXT] {asset_name}")
        unreal.log_warning(f"  Path: {asset_path}")
   for issue in issues:
           unreal.log_warning(f"    - {issue}")
 unreal.log("")
    
    # Summary
    unreal.log("=== SUMMARY ===")
    unreal.log(f"Total Data Assets: {len(all_assets)}")
    unreal.log(f"Assets with Russian text: {len(problematic_assets)}")
    unreal.log("")
    
    if problematic_assets:
        unreal.log_warning("Assets needing manual fix:")
        for asset in problematic_assets:
            unreal.log_warning(f"  - {asset['name']}")
    else:
     unreal.log("? No Russian text found!")
    
    return problematic_assets

def fix_data_asset_text(asset_path, replacements):
    """
    Fix Russian text in a Data Asset
    
    Args:
        asset_path: Path to asset (e.g., "/Game/Core/DA_MyAsset")
        replacements: Dict of {"property_name": "new_value"}
    
    Example:
  fix_data_asset_text(
            "/Game/Core/DA_Dimension_Friendship",
    {
  "display_name": "Friendship",
  "description": "Friendship dimension"
            }
        )
    """
    try:
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
   unreal.log_error(f"Asset not found: {asset_path}")
       return False
        
  for prop_name, new_value in replacements.items():
         if hasattr(asset, prop_name):
        try:
                  asset.set_editor_property(prop_name, new_value)
     unreal.log(f"? Updated {prop_name} in {asset_path}")
    except Exception as e:
               unreal.log_error(f"Failed to update {prop_name}: {e}")
  
        # Save the asset
        if unreal.EditorAssetLibrary.save_asset(asset_path):
   unreal.log(f"? Saved {asset_path}")
      return True
        else:
   unreal.log_error(f"Failed to save {asset_path}")
            return False
          
    except Exception as e:
        unreal.log_error(f"Error fixing {asset_path}: {e}")
        return False

# Example usage for specific assets
KNOWN_FIXES = {
"/Game/Core/Subsystems/RelationshipSystem/DA_Dimension_Friendship": {
    "display_name": "Friendship",
        "description": "Measures friendly relationship between characters"
    },
    "/Game/Core/Subsystems/RelationshipSystem/DA_Dimension_Trust": {
        "display_name": "Trust",
        "description": "Measures trust level in relationship"
  },
    "/Game/Core/Subsystems/RelationshipSystem/DA_StateFriend": {
        "display_name": "Friend",
        "description": "Friendly relationship state"
    },
    "/Game/Core/Subsystems/RelationshipSystem/DA_StateStranger": {
     "display_name": "Stranger",
        "description": "Unknown relationship state"
    },
    # Add more as needed...
}

def apply_known_fixes():
    """Apply predefined fixes to known assets"""
    unreal.log("=== APPLYING KNOWN FIXES ===")
    unreal.log("")
    
    success_count = 0
    
    for asset_path, replacements in KNOWN_FIXES.items():
     unreal.log(f"Fixing: {asset_path}")
        if fix_data_asset_text(asset_path, replacements):
  success_count += 1
        unreal.log("")
    
    unreal.log(f"? Fixed {success_count}/{len(KNOWN_FIXES)} assets")

# Main execution
if __name__ == "__main__":
    # Run scan
    problematic = scan_all_data_assets()
    
    # Optionally apply known fixes (uncomment to use)
    # apply_known_fixes()
    
    unreal.log("Script completed!")
