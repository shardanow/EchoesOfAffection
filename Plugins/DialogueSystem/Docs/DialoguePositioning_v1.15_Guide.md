# Dialogue Participant Positioning System v1.15

## Overview

The Dialogue Participant Positioning System automatically positions NPCs and other dialogue participants at designated locations when dialogue nodes execute. This ensures characters are properly positioned for cutscenes, conversations, and cinematic sequences.

## Key Features

? **Three Positioning Modes:**
- **AI Move To**: Pathfinding-based movement (natural navigation)
- **Teleport**: Instant position change (pre-cutscene setup)
- **Smart Teleport**: Distance-based (close = walk, far = teleport)

? **Flexible Location Sources:**
- Manual world coordinates (X, Y, Z)
- Gameplay Tag-based markers (find by tag)
- Actor transform reference (use another actor's position)

? **Multi-Participant Support:**
- Position Primary Persona
- Position Additional Personas
- Independent configuration per participant

? **Integration:**
- Works with DialogueRunner execution
- Integrates with Sequence system (end position preservation)
- Compatible with ActorSchedule system
- Event-driven architecture (no hard dependencies)

## Quick Start

### 1. Place Location Markers

```
1. Add DialogueLocationMarker actor to level
2. Set LocationTag (e.g., "Location.Dialogue.Lianne.Front")
3. Position and rotate to desired spot
4. Enable bShowDebugInPIE for testing
```

### 2. Configure Node Positioning

```
Open DialogueDataAsset ? Edit Node ? Positioning section:

1. Enable Positioning: ?
2. Choose participant (Primary or Additional Persona)
3. Set Positioning Mode:
   - AIMoveTo (natural movement)
   - Teleport (instant)
   - ConditionalTeleport (smart)
4. Set Location Source:
   - MarkerTag: Select gameplay tag
   - WorldCoordinates: Enter X,Y,Z
   - ActorTransform: Reference actor
5. Configure behavior:
   - bWaitForCompletion
   - AcceptanceRadius
   - TeleportDistanceThreshold
```

### 3. Test in Editor

```
1. PIE (Play In Editor)
2. Start dialogue
3. Check positioning logs: LogDialoguePositioning
4. Verify actors move/teleport correctly
5. Adjust settings as needed
```

## Architecture

### Components

**DialogueLocationMarker** (Actor)
- Placed in level
- Tagged with GameplayTag
- Provides position + rotation
- Debug visualization

**DialogueLocationSubsystem** (WorldSubsystem)
- Caches markers by tag
- Fast lookup service
- Level streaming support
- Debug tools

**Positioning Effects** (C++ Classes)
- `UDialogueEffect_PositionAIMoveTo`
- `UDialogueEffect_PositionTeleport`
- `UDialogueEffect_PositionConditionalTeleport`

**Factory** (`UDialoguePositioningEffectFactory`)
- Creates appropriate effect
- Handles multi-participant
- Mode-based instantiation

### Data Flow

```
DialogueNode.Positioning
  ?
DialogueRunner.ExecuteNodePositioning()
  ?
PositioningEffectFactory.CreateEffectsForNode()
  ?
PositionEffect.Execute()
  ?
FindLocationMarker (subsystem) OR Use coordinates
  ?
AIController.MoveTo() OR Actor.SetActorLocation()
```

## Configuration Reference

### FDialogueParticipantPositioning

| Property | Type | Description |
|----------|------|-------------|
| **ParticipantId** | FName | Persona name from DialogueDataAsset |
| **PositioningMode** | Enum | AIMoveTo / Teleport / ConditionalTeleport |
| **LocationSource** | Enum | MarkerTag / WorldCoordinates / ActorTransform |
| **TargetLocation** | FVector | Manual coordinates (if WorldCoordinates) |
| **TargetRotation** | FRotator | Manual rotation (if WorldCoordinates) |
| **MarkerTag** | FGameplayTag | Tag to find LocationMarker (if MarkerTag) |
| **TargetActor** | TSoftObjectPtr | Actor reference (if ActorTransform) |
| **TeleportDistanceThreshold** | float | Distance for ConditionalTeleport (default: 1000) |
| **bWaitForCompletion** | bool | Pause dialogue until positioned |
| **AcceptanceRadius** | float | How close to get (AI movement, default: 50) |
| **bUsePathfinding** | bool | Use navmesh (default: true) |
| **bApplyRotation** | bool | Apply target rotation (default: true) |

### FDialogueNodePositioning

| Property | Type | Description |
|----------|------|-------------|
| **bEnablePositioning** | bool | Enable for this node |
| **PrimaryPersonaPositioning** | Struct | Positioning for primary speaker |
| **AdditionalPersonasPositioning** | TMap | Positioning per persona (by ID) |
| **bPositionBeforeEffects** | bool | Position before/after node effects |

## Use Cases

### Pre-Cutscene Setup

```
Problem: NPCs standing in wrong spots when cutscene starts
Solution: Teleport mode + LocationMarkers
  ? Instant positioning before sequence plays
  ? Characters appear exactly where needed
```

### Natural Conversation

```
Problem: NPC too far away for dialogue camera
Solution: ConditionalTeleport mode
  ? Far distance: teleport (fast)
  ? Close distance: walk naturally (immersive)
```

### Multi-Participant Scenes

```
Problem: 3+ characters need specific positions
Solution: AdditionalPersonasPositioning
  ? Each participant has own positioning config
  ? Lianne: front marker
  ? Alice: side marker
  ? Player: conversation spot
```

### Sequence + Positioning Integration

```
Scenario: Cutscene moves Alice to new location
Setup:
  1. Sequence plays (Alice walks in cutscene)
  2. bRestoreActorTransforms = false
  3. Alice stays at sequence end position ?
  4. Next dialogue node: Alice already in place
```

## Debug Tools

### Location Markers

```cpp
// Find subsystem
UDialogueLocationSubsystem* Subsystem = 
  World->GetSubsystem<UDialogueLocationSubsystem>();

// Debug draw all markers
Subsystem->DebugDrawAllMarkers(10.0f);

// Print marker info
Subsystem->DebugPrintMarkerInfo();

// Rebuild cache (after spawning new markers)
Subsystem->RebuildMarkerCache();
```

### Console Commands

```
showdebug DIALOGUE
  ? Shows positioning logs
  
stat Dialogue
  ? Performance metrics
```

### Log Categories

```
LogDialoguePositioning: Positioning execution
LogDialogueRunner: Node processing
LogDialogueSequence: Sequence integration
```

## Best Practices

### ? DO

- **Use Gameplay Tags** for marker identification
  - Tag hierarchy: `Location.Dialogue.CharacterName.Position`
  - Example: `Location.Dialogue.Lianne.Front`
  
- **Place multiple markers** per conversation area
  - Front, Side, Back positions
  - Entry, Exit points
  - Conversation circle formation

- **Test both modes** for each scenario
  - Close distance: AI movement
  - Far distance: teleport
  - Find optimal ConditionalTeleport threshold

- **Enable debug visualization** during development
  - bShowDebugInPIE on markers
  - Check positioning logs
  - Verify navigation works

### ? DON'T

- **Don't hardcode coordinates** unless necessary
  - Use markers instead (easier to adjust)
  - Designers can move markers without code changes

- **Don't teleport short distances**
  - Use AI movement for < 500 units
  - Teleport breaks immersion for close moves

- **Don't forget navmesh**
  - AI movement requires NavMeshBoundsVolume
  - Test pathfinding accessibility
  - Fallback to teleport if path fails

## Integration Examples

### With Sequences

```
Node 1: Position participants
  ? Lianne: Move to cutscene start marker
  ? Alice: Teleport to background position
  
Node 2: Play sequence
  ? Sequence moves both characters
  ? bRestoreActorTransforms = false
  ? Characters stay at sequence end positions
  
Node 3: Continue dialogue
  ? Characters already positioned ?
  ? No repositioning needed
```

### With ActorSchedule

```
NPC has active schedule (walking patrol)

Dialogue starts:
  1. Schedule pauses (via GameEventBus)
2. Positioning effect runs (move to marker)
  3. Dialogue plays
  4. Dialogue ends
  5. Schedule resumes FROM CURRENT POSITION ?
  
Result: NPC continues patrol from dialogue location
```

## Troubleshooting

### "Participant not found"

**Cause**: ParticipantId doesn't match actor name
**Fix**: 
- Check DialogueDataAsset ? Personas ? PersonaId
- Match actor name in level
- Use exact FName match

### "Marker not found by tag"

**Cause**: Tag mismatch or marker not in level
**Fix**:
- Verify LocationTag on marker actor
- Check tag spelling (case-sensitive)
- Rebuild marker cache: `Subsystem->RebuildMarkerCache()`
- Ensure marker in streamed level (not unloaded)

### "AI movement failed"

**Cause**: No navmesh or path blocked
**Fix**:
- Add NavMeshBoundsVolume to level
- Rebuild navigation (P key in editor)
- Check AcceptanceRadius (lower if stuck)
- Enable bUsePathfinding = false for direct move
- Fallback: use Teleport mode

### "Positioning not executing"

**Cause**: bEnablePositioning = false or empty config
**Fix**:
- Check Node ? Positioning ? bEnablePositioning ?
- Verify at least one participant configured
- Check positioning logs: `LogDialoguePositioning`

## Performance

**Marker Lookup**: O(1) via cached TMap
**AI Pathfinding**: Async (non-blocking)
**Teleport**: Instant (single frame)

**Recommended Limits**:
- Max markers: 100-200 per level
- Max positioning effects per node: 3-5
- Rebuild cache: Only on level streaming

## Future Enhancements (v1.16+)

- [ ] Animation blending on position change
- [ ] Smooth rotation interpolation
- [ ] Formation positioning (circle, line, etc.)
- [ ] Dynamic marker spawning
- [ ] Blueprint positioning effects
- [ ] Timeline-based positioning
- [ ] Multi-step waypoint movement

## Version History

- **v1.15**: Initial positioning system release
  - Three positioning modes
  - Location markers with tags
  - Multi-participant support
  - Sequence integration

---

**See Also:**
- `Sequencer_SUMMARY.md` - Sequence integration
- `ActorSchedule_Architecture.md` - Schedule system
- `DialogueCamera_QuickStart.md` - Camera control

**Project**: Echoes of Affection  
**Plugin**: Dialogue System v1.15  
**Author**: Shardanow  
**Engine**: Unreal Engine 5.6
