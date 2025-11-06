# Dialogue Positioning Quick Start

## 5-Minute Setup

### Step 1: Place Marker (1 min)

```
Content Browser ? Search "DialogueLocationMarker"
Drag to level
  Position: Where NPC should stand
  Rotation: Which way NPC should face
  LocationTag: "Location.Dialogue.Lianne.Front"
  bShowDebugInPIE: ? (for testing)
```

### Step 2: Configure Node (2 min)

```
Open DialogueDataAsset
Select dialogue node
Scroll to "Positioning" section:
  
  [?] bEnablePositioning
  
  Primary Persona Positioning:
    Participant ID: Lianne
    Positioning Mode: AI Move To
 Location Source: Marker Tag
    Marker Tag: "Location.Dialogue.Lianne.Front"
    Acceptance Radius: 50
  [?] bWaitForCompletion
    [?] bApplyRotation
```

### Step 3: Test (2 min)

```
PIE (Play In Editor)
Start dialogue
Watch Lianne move to marker
Check console: LogDialoguePositioning
Done! ?
```

## Common Configurations

### Instant Teleport

```
Positioning Mode: Teleport
Location Source: Marker Tag
Marker Tag: "Location.Cutscene.Start"

Use for: Pre-cutscene setup
```

### Smart Movement

```
Positioning Mode: Conditional Teleport
Teleport Distance Threshold: 1000
Location Source: Marker Tag
Marker Tag: "Location.Dialogue.NearPlayer"

Use for: Natural NPC behavior
```

### Manual Coordinates

```
Positioning Mode: Teleport
Location Source: World Coordinates
Target Location: (100, 200, 50)
Target Rotation: (0, 90, 0)

Use for: Quick prototyping
```

### Multiple Participants

```
Primary Persona: Lianne ? "Location.Dialogue.Lianne.Front"
Additional Personas:
  Alice ? "Location.Dialogue.Alice.Side"
  Guard ? "Location.Dialogue.Guard.Back"

Use for: Group conversations
```

## Debug Commands

```
// In Play session, press ` (tilde) key:

stat DialoguePositioning    // Show performance
showdebug Dialogue   // Detailed logs

// Blueprint/C++:
Subsystem->DebugDrawAllMarkers(10.0f);
Subsystem->DebugPrintMarkerInfo();
```

## Troubleshooting Fast

| Problem | Solution |
|---------|----------|
| NPC doesn't move | Check bEnablePositioning ? |
| "Marker not found" | Verify LocationTag matches |
| AI path fails | Add NavMeshBoundsVolume + rebuild (P key) |
| NPC teleports back | Set bRestoreActorTransforms = false on sequence |
| Wrong participant | Match ParticipantId to Persona.PersonaId |

## Tag Convention

```
Location.Category.Character.Position

Examples:
  Location.Dialogue.Lianne.Front
  Location.Dialogue.Lianne.Side
  Location.Dialogue.Alice.Background
  Location.Cutscene.Start
  Location.Shop.Counter
```

## Performance Tips

- **Use markers** (not coordinates): Faster lookup
- **Cache subsystem**: Don't Get() every frame
- **AI movement**: Async (non-blocking)
- **Teleport**: Instant (1 frame)
- **Smart teleport**: Best of both worlds

## Next Steps

1. ? Basic setup working
2. ?? Read full guide: `DialoguePositioning_v1.15_Guide.md`
3. ?? Integrate with sequences: `Sequencer_SUMMARY.md`
4. ?? Add schedules: `ActorSchedule_Architecture.md`
5. ?? Polish with animations and VFX

---

**Time to working system: 5 minutes**  
**Difficulty: Easy ?**  
**AAA Quality: Professional ?**
