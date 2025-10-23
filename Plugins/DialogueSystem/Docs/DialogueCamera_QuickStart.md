# Dialogue Camera - Quick Start Guide

## Overview

The Dialogue Camera Component provides cinematic camera control during dialogues. It automatically positions the camera to focus on NPCs with professional framing presets.

## Setup

### 1. Add Component to Player

Add `DialogueCameraComponent` to your player character or player controller Blueprint.

### 2. Basic Usage in Blueprint

#### Simple Activation (Recommended)
```
Event Graph:
  [Start Dialogue] -> [Activate Dialogue Camera (Simple)]
                       ?? Target Actor: [NPC Reference]
                       ?? Return Value: [Success Boolean]
```

This uses the default **Medium Shot** preset (Distance: 200cm, AngleOffset: 25°).

#### Advanced Activation with Framing Presets
```
[Activate Dialogue Camera With Framing]
?? Target Actor: [NPC Reference]
?? Framing: [Enum Selection]
    ?? Close-Up (120cm, 15° angle, FOV 60°)
    ?? Medium Shot (200cm, 25° angle, FOV 70°)
    ?? Full Shot (350cm, 30° angle, FOV 80°)
    ?? Over-Shoulder (180cm, 45° angle, FOV 65°)
    ?? Two-Shot (300cm, 0° angle, FOV 75°)
```

#### Custom Settings
```
[Make FDialogueCameraSettings]
?? Distance: 250.0
?? Height Offset: 60.0
?? Angle Offset: 30.0
?? Pitch Adjustment: -5.0
?? FOV Override: 70.0
?? Blend Time: 0.5
?? Blend Mode: Ease In/Out

[Activate Dialogue Camera]
?? Target Actor: [NPC]
?? Settings: [Custom Settings]
```

### 3. Deactivation
```
[Deactivate Dialogue Camera]
```

This automatically restores the original camera view with smooth blending.

## Understanding Parameters

### Distance
- **Definition**: Distance from NPC in centimeters
- **Typical Values**: 
  - Close-Up: 100-150cm
  - Medium: 180-250cm
  - Full Shot: 300-400cm
- **Current Issue**: Values in Blueprint may show incorrect units - use larger values (e.g., 200 instead of 2)

### Height Offset
- **Definition**: Vertical offset above the focus point
- **Default**: 60cm (typical head height)
- **Adjust**: Positive = higher, Negative = lower

### Angle Offset
- **Definition**: Horizontal angle around NPC (in degrees)
- **Values**:
  - 0° = Front view
  - 45° = Three-quarter view
  - 90° = Side view
  - 180° = Back view

### Pitch Adjustment
- **Definition**: Camera tilt (in degrees)
- **Values**:
  - Negative = Look slightly down
  - 0° = Eye level
  - Positive = Look up

### FOV Override
- **Definition**: Field of View in degrees
- **Typical Values**:
  - 50-60° = Close, intimate
  - 70-80° = Standard
  - 90-100° = Wide angle
- **0 = Use default camera FOV**

## Common Issues & Solutions

### Issue: Camera appears too close or under the map
**Cause**: Incorrect Distance value in Blueprint
**Solution**: 
- Use **Activate Dialogue Camera (Simple)** function
- OR set Distance to 200-300 (not 2-3!)

### Issue: Camera looking at wrong NPC
**Cause**: Passing wrong Target Actor reference
**Solution**: 
- Ensure Target Actor points to the NPC, not the player
- Check logs in Output Log window for "Target Actor: [Name]"

### Issue: Camera doesn't restore after dialogue
**Cause**: `DeactivateDialogueCamera` not called
**Solution**: 
- Call `DeactivateDialogueCamera` when dialogue ends
- Check `bRestoreOriginalCamera` is enabled (default: true)

### Issue: Camera conflicts with player camera
**Problem**: Other systems fighting for camera control

**Causes**:
1. **Player Camera has `bUsePawnControlRotation` enabled**
   - This makes the camera follow mouse/controller input
   - Dialogue camera will fight with player camera
   
2. **Other ViewTarget changes happening**
   - Cutscenes, camera shakes, or other camera systems
   - Multiple systems calling `SetViewTarget` simultaneously

**Solutions**:
1. **Disable player camera rotation during dialogue:**
   ```cpp
   // In your character or camera code
   void StartDialogue()
   {
       if (FollowCamera)
       {
           FollowCamera->bUsePawnControlRotation = false;
       }
       // Activate dialogue camera...
   }
   
   void EndDialogue()
   {
       // Deactivate dialogue camera...
       if (FollowCamera)
       {
           FollowCamera->bUsePawnControlRotation = true;
       }
   }
   ```

2. **Coordinate with other camera systems:**
   - Pause camera shakes during dialogues
   - Disable cinematic sequences
   - Use priority system if multiple camera controllers exist

3. **Check ViewTarget logs:**
   ```
   LogDialogueCamera: Stored original view target: bp_MyAnimeCharacter_C_0
   LogDialogueCamera: Restored original view target: bp_MyAnimeCharacter_C_0
   ```

### Issue: Blend modes don't work smoothly
**FIXED in latest version!**

**Previous Problem**: Only Linear blend worked; EaseIn, EaseOut, etc. caused instant cuts.

**Solution (Applied)**: Camera now:
1. Starts at current player camera position
2. Smoothly interpolates to target position using custom easing curves
3. All blend modes (Linear, Cubic, EaseIn, EaseOut, EaseInOut) now work correctly

**Blend Mode Descriptions**:
- **Linear**: Constant speed movement
- **Cubic**: Smooth S-curve (slow start and end, fast middle)
- **Ease In**: Slow start, accelerates
- **Ease Out**: Fast start, decelerates
- **Ease In/Out**: Smooth acceleration and deceleration (recommended)
- **Cut**: Instant (no blend)

## How ViewTarget System Works

### What is ViewTarget?
The **ViewTarget** is the actor that the **PlayerController** is currently viewing through. When you play the game:
- Usually ViewTarget = Your Player Character (or its camera)
- When dialogue starts: ViewTarget = Dialogue Camera Actor
- When dialogue ends: ViewTarget = Restored back to player

### Safe Usage
```cpp
// The system automatically:
1. Saves current ViewTarget (your player/camera)
2. Creates temporary Dialogue Camera Actor
3. Switches ViewTarget to Dialogue Camera
4. On end: Switches back to original ViewTarget
5. Destroys Dialogue Camera Actor
```

### Why This Approach?
? **Non-destructive**: Your player camera remains intact
? **Clean**: No persistent modifications to player
? **Compatible**: Works with any Unreal camera setup
? **Reversible**: Always returns to original state

## Next Steps

- See `DialogueCameraComponent.h` for full API documentation
- Experiment with different framing presets
- Create custom presets for specific scenes
- Integrate with dialogue UI for automatic activation
