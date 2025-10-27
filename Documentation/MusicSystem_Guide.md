# ?? Music System Documentation

## Overview

Professional music management system with zone-based priority system and smooth transitions.

## Architecture

```
MusicManager (GameInstance Subsystem)
??? Audio Components (Primary + Secondary for crossfade)
??? Active Zones (TMap<Priority, MusicRequest>)
??? Crossfade State Machine

MusicZoneVolume (AVolume)
??? Zone Properties (Music, Priority, Transition Type)
??? Overlap Detection (Player enter/exit)
```

## Components

### 1. UMusicManager

Centralized music controller (Subsystem).

**Key Features:**
- Priority-based zone system
- Smooth crossfades
- Master volume control
- Pause/Resume support
- Console commands for debugging

**Blueprint Functions:**
```cpp
// Get singleton
UMusicManager* Manager = UMusicManager::Get(WorldContext);

// Request music change
Manager->RequestMusicChange(Music, Priority, TransitionType, Duration, Volume);

// Release zone
Manager->ReleaseZone(Priority);

// Stop music
Manager->StopMusic(FadeOutDuration);

// Volume control
Manager->SetMasterMusicVolume(0.5f); // 50%
float Volume = Manager->GetMasterMusicVolume();

// Pause/Resume
Manager->PauseMusic();
Manager->ResumeMusic();

// Check state
bool bPlaying = Manager->IsMusicPlaying();
USoundBase* Current = Manager->GetCurrentMusic();
```

### 2. AMusicZoneVolume

Volume actor for zone-based music.

**Properties:**
- `ZoneMusic` (USoundBase*) - Music to play
- `Priority` (int32, 0-100) - Zone priority
- `TransitionType` (EMusicTransitionType) - Transition style
- `CrossfadeDuration` (float) - Fade duration
- `VolumeMultiplier` (float, 0.0-1.0) - Volume modifier
- `bIsActive` (bool) - Enable/disable zone
- `bPlayerOnly` (bool) - Only trigger for player
- `bShowDebugInfo` (bool) - Show debug messages

**Transition Types:**
- `Immediate` - Instant switch
- `Crossfade` - Smooth fade between tracks
- `WaitForEnd` - Wait for current track to finish
- `Layer` - Additive layering (future feature)

## Usage

### Basic Setup

1. **Place Music Zones in Level:**

```cpp
Content Browser ? Right Click ? Blueprint Class ? AMusicZoneVolume
Name: BP_MusicZone_Town

Details Panel:
??? Zone Music: SFX_Town_Theme
??? Priority: 10
??? Transition Type: Crossfade
??? Crossfade Duration: 2.0
??? Volume Multiplier: 1.0
```

2. **Create Multiple Zones with Priorities:**

```cpp
BP_MusicZone_Town (Priority: 10)
??? Music: Town_Ambient.wav

BP_MusicZone_Combat (Priority: 50)
??? Music: Battle_Theme.wav

BP_MusicZone_Boss (Priority: 100)
??? Music: Boss_Music.wav
```

### Priority System

Music plays based on **highest active priority**:

```cpp
Scenario:
1. Player spawns in Town (Priority: 10) ? Town_Ambient plays
2. Player enters Combat zone (Priority: 50) ? Battle_Theme plays (higher priority!)
3. Combat ends, player exits zone ? Town_Ambient resumes (next highest)
4. Player leaves all zones ? Music fades out
```

**Priority Guidelines:**
- **0-20**: Ambient zones (towns, forests, dungeons)
- **20-40**: Special events (NPC encounters, cutscenes)
- **40-60**: Combat situations (regular fights)
- **60-80**: Important battles (mini-bosses)
- **80-100**: Critical moments (main bosses, story climax)

### Advanced: Blueprint Integration

**Custom Music Trigger:**

```cpp
// Blueprint: BP_CustomMusicTrigger

Event BeginPlay
?? Get Music Manager
?? Request Music Change
    ?? New Music: MyCustomMusic
    ?? Priority: 35
    ?? Transition Type: Crossfade
    ?? Crossfade Duration: 1.5
    ?? Volume Multiplier: 0.8

Event Trigger Deactivated
?? Get Music Manager
?? Release Zone (Priority: 35)
```

**Dynamic Music based on Game State:**

```cpp
// Blueprint: BP_GameStateManager

Function UpdateMusicForState(GameState)
?? Branch (GameState == Battle)
?   ?? Get Music Manager
?   ?? Request Music Change (BattleMusic, 50, Immediate, 0, 1.0)
?
?? Branch (GameState == Peaceful)
    ?? Get Music Manager
    ?? Request Music Change (AmbientMusic, 10, Crossfade, 3.0, 0.7)
```

### Integration with Dialogue System

**Automatic Music Ducking:**

DialogueWidget automatically reduces music volume to 30% during dialogues:

```cpp
// In DialogueWidget.cpp (already implemented)

void UDialogueWidget::NativeConstruct()
{
    // Music volume reduced to 30% when dialogue opens
    MusicManager->SetMasterMusicVolume(0.3f);
}

void UDialogueWidget::NativeDestruct()
{
    // Music volume restored when dialogue closes
    MusicManager->SetMasterMusicVolume(1.0f);
}
```

**Custom NPC Music:**

```cpp
// Blueprint: BP_NPCActor

Event Begin Dialogue
?? Get Music Manager
?? Request Music Change
    ?? New Music: NPC_Theme
    ?? Priority: 30
    ?? Transition Type: Crossfade
    ?? Duration: 2.0

Event End Dialogue
?? Get Music Manager
?? Release Zone (Priority: 30)
```

## Console Commands

**Debug Music State:**
```
Music.Debug
```
Output:
```
=== MUSIC MANAGER DEBUG ===
Current Music: Town_Theme
Master Volume: 1.00
Is Playing: Yes
Active Zones: 2
```

**Stop Music:**
```
Music.Stop
```

## Common Patterns

### 1. Nested Zones (NPC in Town)

```cpp
Town Zone (Priority: 10)
??? Vendor NPC Zone (Priority: 25) - inside town

Result:
- Walk into town ? Town music
- Approach vendor ? Vendor music (higher priority)
- Leave vendor ? Town music returns
- Leave town ? Music fades out
```

### 2. Combat Override

```cpp
BP_CombatManager:

Event Combat Started
?? Spawn BP_MusicZone_Combat at Player Location
?? Set Zone Radius to 5000 units

Event Combat Ended
?? Destroy Combat Zone (auto-releases priority)
```

### 3. Emotional Music (Dialogue Integration)

```cpp
// C++ in DialogueWidget::UpdateEmotion()

void UDialogueWidget::UpdateEmotion(const FGameplayTag& EmotionTag)
{
    if (UMusicManager* Manager = UMusicManager::Get(this))
    {
      if (EmotionTag == "Emotion.Romantic")
        {
            Manager->RequestMusicChange(RomanticMusic, 40, Crossfade, 2.0f, 0.8f);
        }
     else if (EmotionTag == "Emotion.Tense")
        {
        Manager->RequestMusicChange(TenseMusic, 45, Immediate, 0.0f, 1.0f);
        }
    }
}
```

## Performance

- **Low overhead**: Only 2 audio components active
- **Efficient updates**: Tick only during crossfade
- **Memory friendly**: Zones use references, not copies

## Troubleshooting

**Music doesn't play:**
1. Check `Music.Debug` console command
2. Verify ZoneMusic is assigned in zone
3. Check player is overlapping zone (use ShowDebugInfo)
4. Verify AudioComponent is created (check logs)

**Transitions are choppy:**
1. Increase CrossfadeDuration (2-3 seconds recommended)
2. Check CPU performance (use `stat game`)
3. Verify music files are streamed correctly

**Multiple zones conflict:**
1. Check priorities with `Music.Debug`
2. Use bShowDebugInfo on zones
3. Draw debug boxes to visualize overlaps

## Future Enhancements

- [ ] Layered music system (percussion, melody, harmony)
- [ ] Beat-synchronized transitions
- [ ] Music intensity system (dynamic mix)
- [ ] Save/load music state
- [ ] Music event callbacks
- [ ] Integration with Wwise/FMOD

## Example Project Structure

```
Content/
??? Audio/
?   ??? Music/
?   ?   ??? Ambient/
?   ?   ?   ??? Town_Theme.wav
?   ?   ?   ??? Forest_Ambient.wav
?   ?   ?   ??? Cave_Echo.wav
?   ?   ??? Combat/
?   ?   ?   ??? Battle_01.wav
?   ?   ?   ??? Battle_02.wav
?   ?   ?   ??? Boss_Theme.wav
?   ?   ??? Dialogue/
?   ?       ??? NPC_Friendly.wav
?   ?       ??? NPC_Romantic.wav
?   ? ??? NPC_Tense.wav
?   ??? SFX/
?    ??? UI/
?           ??? Music_Transitions/
?
??? Blueprints/
    ??? Audio/
        ??? BP_MusicZone_Town
        ??? BP_MusicZone_Combat
     ??? BP_MusicZone_Boss
        ??? BP_MusicZone_NPC_Base
```

## API Reference

### UMusicManager

| Function | Description | Parameters |
|----------|-------------|------------|
| `Get(WorldContext)` | Get singleton instance | WorldContextObject |
| `RequestMusicChange()` | Request music transition | Music, Priority, Type, Duration, Volume |
| `ReleaseZone()` | Release zone by priority | Priority |
| `StopMusic()` | Stop all music | FadeOutDuration |
| `SetMasterMusicVolume()` | Set global volume | Volume (0-1) |
| `GetMasterMusicVolume()` | Get global volume | Returns float |
| `PauseMusic()` | Pause playback | None |
| `ResumeMusic()` | Resume playback | None |
| `IsMusicPlaying()` | Check if playing | Returns bool |
| `GetCurrentMusic()` | Get current track | Returns USoundBase* |

### AMusicZoneVolume

| Function | Description | Parameters |
|----------|-------------|------------|
| `GetZoneMusic()` | Get zone music | Returns USoundBase* |
| `SetZoneMusic()` | Set zone music | NewMusic |
| `SetZoneActive()` | Enable/disable zone | bActive |
| `IsZoneActive()` | Check if active | Returns bool |
| `OnPlayerEntered()` | BP event: player entered | PlayerController |
| `OnPlayerExited()` | BP event: player exited | PlayerController |

## Credits

System designed for Echoes of Affection
Implements professional game audio patterns
Compatible with Unreal Engine 5.x
