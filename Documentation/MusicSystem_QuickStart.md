# ?? Music System - Quick Start Guide

## 5-Minute Setup

### Step 1: Verify Installation ?

1. Open your project in Unreal Engine
2. Check that these files exist:
   ```
   Source/EchoesOfAffection/Public/Audio/MusicManager.h
   Source/EchoesOfAffection/Private/Audio/MusicManager.cpp
   Source/EchoesOfAffection/Public/Audio/MusicZoneVolume.h
   Source/EchoesOfAffection/Private/Audio/MusicZoneVolume.cpp
   ```

3. **Compile the project:**
   - Visual Studio: Build ? Build Solution (Ctrl+Shift+B)
   - Or click "Compile" in Unreal Editor

### Step 2: Import Music Assets ??

1. Content Browser ? Right Click ? Import
2. Select your music files (.wav, .mp3, .ogg)
3. Recommended structure:
   ```
   Content/Audio/Music/
 ??? Town_Theme.wav
   ??? Forest_Ambient.wav
 ??? Battle_Music.wav
   ```

### Step 3: Create First Music Zone ??

1. **Content Browser:**
   - Right Click ? Blueprint Class
   - Search for: `MusicZoneVolume`
   - Select `AMusicZoneVolume`
   - Name: `BP_MusicZone_Town`

2. **Configure Zone:**
   - Open `BP_MusicZone_Town`
   - Details Panel:
     ```
     Zone Music: Town_Theme
     Priority: 10
     Transition Type: Crossfade
     Crossfade Duration: 2.0
     Volume Multiplier: 1.0
     Is Active: ?
     Player Only: ?
     Show Debug Info: ? (for testing)
   ```

3. **Place in Level:**
   - Drag `BP_MusicZone_Town` into your level
   - Scale to cover desired area (default brush size)
   - Recommended size: 5000 x 5000 x 400 units

### Step 4: Test It! ??

1. **Press Play** (Alt+P)
2. Walk into the zone
3. You should see (if ShowDebugInfo enabled):
   ```
   Entered Music Zone: BP_MusicZone_Town
   Priority: 10
   Music: Town_Theme
   ```
4. Music should start playing with fade-in
5. Walk out of zone - music fades out

### Step 5: Add More Zones ??

**Create Combat Zone (higher priority):**

1. Duplicate `BP_MusicZone_Town` (Ctrl+D)
2. Rename to `BP_MusicZone_Combat`
3. Configure:
   ```
   Zone Music: Battle_Music
   Priority: 50  ? Higher priority!
   Transition Type: Immediate
   Crossfade Duration: 0.5
   Volume Multiplier: 1.2  ? Louder!
   ```

4. Place overlapping with town zone

5. **Test Priority System:**
   - Walk into town ? Town music plays
   - Walk into combat zone ? Battle music takes over immediately!
   - Walk out of combat ? Town music returns
   - Walk out of town ? Music stops

## Common Use Cases

### Use Case 1: Town with Background Music

```
BP_MusicZone_Town
??? Zone Music: Town_Peaceful.wav
??? Priority: 10
??? Transition: Crossfade (3.0s)
??? Covers entire town area
```

### Use Case 2: Combat Override

```
BP_MusicZone_Combat
??? Zone Music: Battle_Intense.wav
??? Priority: 50
??? Transition: Immediate
??? Spawned dynamically when combat starts
```

### Use Case 3: NPC Interaction Music

```
BP_MusicZone_NPC_Vendor
??? Zone Music: Shop_Theme.wav
??? Priority: 25
??? Transition: Crossfade (1.5s)
??? Small zone around NPC (500 units radius)
```

## Blueprint Integration

### Simple Trigger

```cpp
// Blueprint: BP_MusicTrigger

[Event: On Player Overlap]
    ?
[Get Music Manager]
    ?
[Request Music Change]
    ?? Music: MyCustomMusic
  ?? Priority: 30
    ?? Transition: Crossfade
    ?? Duration: 2.0
    ?? Volume: 0.8
```

### C++ Integration

```cpp
// In your Actor class

#include "Audio/MusicManager.h"

void AMyActor::StartCustomMusic()
{
    if (UMusicManager* Manager = UMusicManager::Get(this))
    {
        Manager->RequestMusicChange(
          MyMusicAsset,
         35,  // Priority
       EMusicTransitionType::Crossfade,
    2.0f,  // Duration
       1.0f   // Volume
        );
    }
}

void AMyActor::StopCustomMusic()
{
    if (UMusicManager* Manager = UMusicManager::Get(this))
    {
 Manager->ReleaseZone(35);  // Release by priority
    }
}
```

## Testing & Debug

### Console Commands

Open console (~) and type:

```
Music.Debug    ? Show current music state
Music.Stop     ? Stop all music
```

### Debug Visualization

1. Enable `Show Debug Info` on zones
2. Play game
3. Green boxes = zone entered
4. Yellow boxes = zone exited
5. On-screen messages show current state

### Common Issues

**? Music doesn't play:**
- Check ZoneMusic is assigned
- Verify player is overlapping zone (F8 to eject camera)
- Check Output Log for errors

**? Transitions are choppy:**
- Increase Crossfade Duration (2-3 seconds)
- Check music file format (WAV recommended)

**? Wrong music plays:**
- Check priorities (Music.Debug)
- Higher priority always wins
- Verify IsActive is checked

## Next Steps

? **You're all set!** Your music system is working.

**Want to go deeper?**
- Read full documentation: `Documentation/MusicSystem_Guide.md`
- Explore advanced features:
  - Dynamic music based on emotions
  - Integration with dialogue system
  - Layered music tracks
  - Beat-synchronized transitions

**Need help?**
- Check Output Log (Window ? Developer Tools ? Output Log)
- Use `Music.Debug` console command
- Enable `Show Debug Info` on zones

## Example Project Layout

```
Content/
??? Audio/
?   ??? Music/
?       ??? Town_Theme.wav
?       ??? Forest_Ambient.wav
?       ??? Battle_Music.wav
?       ??? Boss_Theme.wav
?
??? Blueprints/
?   ??? Audio/
?    ??? BP_MusicZone_Town
?     ??? BP_MusicZone_Forest
?       ??? BP_MusicZone_Combat
?       ??? BP_MusicZone_Boss
?
??? Maps/
  ??? TestLevel.umap
        ??? BP_MusicZone_Town (Priority: 10)
        ??? BP_MusicZone_Forest (Priority: 10)
        ??? BP_MusicZone_Combat (Priority: 50)
```

## Quick Reference Card

| Task | Blueprint Node | C++ Function |
|------|----------------|--------------|
| Get Manager | `Get Music Manager` | `UMusicManager::Get(this)` |
| Play Music | `Request Music Change` | `RequestMusicChange(...)` |
| Stop Music | `Stop Music` | `StopMusic(2.0f)` |
| Change Volume | `Set Master Music Volume` | `SetMasterMusicVolume(0.5f)` |
| Release Zone | `Release Zone` | `ReleaseZone(Priority)` |
| Check Status | `Is Music Playing` | `IsMusicPlaying()` |

---

**?? Congratulations!** You now have a professional music system running in your game!
