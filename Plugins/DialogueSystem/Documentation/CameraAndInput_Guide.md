# Camera Control & Input Blocking System - User Guide

## Overview

This system provides professional camera control and input blocking for dialogue sequences in Unreal Engine 5, following industry best practices used in AAA games.

## Features

### Camera Control
- **Cinematic Camera Transitions** - Smooth blending between camera positions
- **Multiple Framing Presets** - Close-up, Medium, Full, Over-Shoulder, Two-Shot
- **Automatic Speaker Tracking** - Camera follows active speaker
- **Collision-Aware** - Uses Spring Arm for obstacle avoidance
- **Customizable Settings** - FOV, distance, angles, blend times
- **ViewTarget Integration** - Seamless integration with UE's camera system

### Input Blocking
- **Enhanced Input Support** - Full support for UE5 Enhanced Input System
- **Legacy Input Fallback** - Compatible with traditional input
- **Flexible Blocking Modes** - Block all, movement only, camera only, custom
- **Pawn State Management** - Freeze movement, rotation, hide player
- **Non-Intrusive** - Preserves original input configuration

### Presentation Modes
- **Cinematic** - Full camera control + complete input blocking
- **Interactive** - Minimal blocking, player can move/look around
- **Hybrid** - Camera control with some player input allowed
- **First-Person** - Optimized for first-person games
- **Custom** - Define your own settings

## Quick Start

### Setup (3 Steps)

#### 1. Add Components to Player Character

```cpp
UCLASS()
class AMyPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
    UDialoguePresentationComponent* PresentationComponent;

    AMyPlayerCharacter()
    {
        // Create presentation component (includes camera + input blocker)
        PresentationComponent = CreateDefaultSubobject<UDialoguePresentationComponent>(TEXT("DialoguePresentation"));
    }
};
```

#### 2. Configure in Blueprint

Open your player character blueprint:
1. Select **DialoguePresentation** component
2. Set **Default Presentation Mode** (e.g., "Cinematic")
3. Configure **Camera Settings** as desired
4. Set **Input Blocking** preferences

#### 3. Activate During Dialogue

```cpp
// C++ Usage
void AMyPlayerCharacter::StartDialogueWithNPC(AActor* NPCCharacter)
{
    if (PresentationComponent)
    {
        // Activate with default mode
        PresentationComponent->ActivatePresentation(NPCCharacter, EDialoguePresentationMode::Cinematic);
    }
}
```

```blueprint
// Blueprint Usage
- Get Presentation Component
- Call "Activate Presentation"
  - Target NPC: (NPC Actor Reference)
  - Mode: Cinematic
```

## Advanced Usage

### Custom Camera Settings

```cpp
FDialogueCameraSettings CustomCamera;
CustomCamera.Framing = EDialogueCameraFraming::CloseUp;
CustomCamera.Distance = 150.0f;
CustomCamera.HeightOffset = 75.0f;
CustomCamera.AngleOffset = 20.0f;
CustomCamera.PitchAdjustment = -10.0f;
CustomCamera.FOVOverride = 55.0f;
CustomCamera.BlendTime = 0.8f;
CustomCamera.BlendMode = EDialogueCameraBlendMode::EaseInOut;

// Apply settings
CameraComponent->ActivateDialogueCamera(TargetNPC, CustomCamera);
```

### Custom Input Blocking

```cpp
FDialogueInputBlockingSettings CustomInput;
CustomInput.BlockingMode = EInputBlockingMode::Custom;
CustomInput.bBlockMovement = true;
CustomInput.bBlockCamera = false;  // Allow player to look around
CustomInput.bBlockJump = true;
CustomInput.bBlockSprint = true;
CustomInput.bDisablePawnMovement = true;
CustomInput.bDisablePawnRotation = false;  // Allow rotation
CustomInput.bHidePlayerPawn = false;

// Apply settings
InputBlockerComponent->BlockInput(CustomInput);
```

### Dynamic Camera Transitions

```cpp
// Switch camera target mid-dialogue
void OnSpeakerChanged(AActor* NewSpeaker)
{
    if (PresentationComponent && PresentationComponent->IsPresentationActive())
    {
        // Smoothly transition to new speaker
        PresentationComponent->TransitionToNewTarget(NewSpeaker, 0.5f);
    }
}

// Change camera framing during dialogue
void OnEmotionalMoment()
{
    if (auto* CameraComp = PresentationComponent->GetCameraComponent())
    {
        // Switch to close-up for emotional impact
        CameraComp->UpdateCameraFraming(EDialogueCameraFraming::CloseUp, 0.3f);
    }
}
```

### Presentation Mode Presets

```cpp
// Create custom presentation mode
FDialoguePresentationSettings CustomMode;
CustomMode.PresentationMode = EDialoguePresentationMode::Custom;
CustomMode.bEnableCamera = true;
CustomMode.bEnableInputBlocking = true;
CustomMode.CameraSettings.Framing = EDialogueCameraFraming::Medium;
CustomMode.InputSettings.BlockingMode = EInputBlockingMode::BlockMovement;
CustomMode.bPauseGame = false;
CustomMode.TimeDilation = 0.5f;  // Slow-motion dialogue

// Register custom mode
PresentationComponent->SetModeSettings(EDialoguePresentationMode::Custom, CustomMode);

// Use custom mode
PresentationComponent->ActivatePresentation(NPC, EDialoguePresentationMode::Custom);
```

## Camera Framing Presets

### Close-Up
- **Use Case**: Emotional moments, important revelations
- **Distance**: 120 units
- **FOV**: 60°
- **Focus**: Face/eyes

### Medium Shot
- **Use Case**: Standard dialogue (default)
- **Distance**: 200 units
- **FOV**: 70°
- **Focus**: Head and shoulders

### Full Shot
- **Use Case**: Show character actions, environmental context
- **Distance**: 350 units
- **FOV**: 80°
- **Focus**: Entire character

### Over-Shoulder
- **Use Case**: Conversational, perspective shots
- **Distance**: 180 units
- **Angle**: 45° offset
- **Focus**: Other character over shoulder

### Two-Shot
- **Use Case**: Show both participants
- **Distance**: 300 units
- **Angle**: Side view
- **Focus**: Both characters

## Input Blocking Modes

### Block All
- Blocks **all input** except dialogue-specific actions
- Best for: Cinematic sequences

### Block Movement Only
- Blocks: Movement, jump, sprint
- Allows: Camera, interactions, menus
- Best for: Interactive dialogues

### Block Camera Only
- Blocks: Look/turn input
- Allows: Movement, actions
- Best for: Scripted camera sequences

### Block Movement & Camera
- Blocks: Movement + camera input
- Best for: Standard dialogues (default)

### Custom
- Configure exactly what to block
- Full control over each input type

## Best Practices

### 1. Camera Setup
```cpp
? DO:
- Use Medium Shot for standard dialogues
- Blend smoothly (0.3-0.5s typically)
- Enable collision detection
- Track active speaker
- Use appropriate FOV for scene

? DON'T:
- Cut instantly unless intentional
- Use extreme angles without reason
- Forget to restore original camera
- Block camera in interactive modes
```

### 2. Input Blocking
```cpp
? DO:
- Always unblock on dialogue end
- Test with both Enhanced Input and Legacy
- Preserve dialogue-specific actions
- Store and restore pawn state
- Allow escape/cancel actions

? DON'T:
- Block all input permanently
- Forget to handle edge cases (level transitions, etc.)
- Disable input without visual feedback
- Block menu/pause actions
```

### 3. Performance
```cpp
? DO:
- Use object pooling for camera actors
- Disable tick when not active
- Cache component references
- Use timers for tracking updates

? DON'T:
- Create cameras every frame
- Keep tick enabled when inactive
- Perform expensive operations in Tick
```

### 4. User Experience
```cpp
? DO:
- Provide visual feedback when input is blocked
- Allow skipping cinematics
- Make controls intuitive
- Test with different character sizes
- Support both keyboard and gamepad

? DON'T:
- Trap player without escape
- Use jarring camera movements
- Forget accessibility options
- Assume one input device
```

## Integration with Dialogue System

### Automatic Activation

```cpp
UCLASS()
class UMyDialogueComponent : public UDialogueComponent
{
    GENERATED_BODY()

public:
    virtual void OnDialogueStarted_Implementation(UDialogueRunner* Runner) override
    {
        Super::OnDialogueStarted_Implementation(Runner);

        // Auto-activate presentation
        if (AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn())
        {
            if (auto* Presentation = Player->FindComponentByClass<UDialoguePresentationComponent>())
            {
                Presentation->ActivatePresentation(GetOwner(), EDialoguePresentationMode::Cinematic);
            }
        }
    }

    virtual void OnDialogueEnded_Implementation() override
    {
        // Auto-deactivate presentation
        if (AActor* Player = GetWorld()->GetFirstPlayerController()->GetPawn())
        {
            if (auto* Presentation = Player->FindComponentByClass<UDialoguePresentationComponent>())
            {
                Presentation->DeactivatePresentation();
            }
        }

        Super::OnDialogueEnded_Implementation();
    }
};
```

### Speaker Tracking

```cpp
void OnDialogueNodeChanged(UDialogueNode* NewNode)
{
    if (!NewNode) return;

    // Get speaker from node
    AActor* Speaker = GetSpeakerFromNode(NewNode);
    
    if (Speaker && PresentationComponent)
    {
        // Transition camera to new speaker
        PresentationComponent->TransitionToNewTarget(Speaker, 0.4f);
        
        // Optionally adjust framing based on emotion
        if (NewNode->EmotionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Emotion.Angry")))
        {
            // Closer shot for intense moments
            auto* CameraComp = PresentationComponent->GetCameraComponent();
            CameraComp->UpdateCameraFraming(EDialogueCameraFraming::CloseUp, 0.3f);
        }
    }
}
```

## Blueprint Examples

### Example 1: Basic Dialogue with Cinematic Camera

```
Event BeginPlay
  ?
Get Player Character
  ?
Get Component (DialoguePresentation)
  ?
Store as Variable
```

```
On Interact with NPC
  ?
Activate Presentation
  ?? Target NPC: (Interacted Actor)
  ?? Mode: Cinematic
  ?? Return: Success
      ?
  Start Dialogue
```

### Example 2: Custom Camera for Emotional Scenes

```
On Emotional Dialogue Node
  ?
Get Camera Component
  ?
Update Camera Framing
  ?? New Framing: Close-Up
  ?? Blend Time: 0.3
  ?? ?
  Play Camera Shake (optional)
```

### Example 3: Interactive Dialogue with Limited Blocking

```
On Start Interactive Dialogue
  ?
Get Presentation Component
  ?
Activate Presentation
  ?? Target NPC: (NPC Reference)
  ?? Mode: Interactive
      ?
  [Player can still move/look around]
```

## Troubleshooting

### Camera Not Activating
- Check if `bEnableCamera` is true
- Verify target NPC is valid
- Ensure PlayerController exists
- Check for existing view target locks

### Input Still Responsive
- Verify `bEnableInputBlocking` is true
- Check if Enhanced Input is properly configured
- Test both Enhanced and Legacy input paths
- Confirm blocking mode is not "AllowAll"

### Camera Snapping/Jerking
- Increase blend time (0.3-0.5s recommended)
- Enable collision detection
- Check for conflicting camera modifiers
- Verify spring arm settings

### Pawn Not Freezing
- Check `bDisablePawnMovement` setting
- Verify CharacterMovementComponent exists
- Test in PIE (not in editor viewport)
- Check for other movement components

## API Reference

### DialogueCameraComponent

```cpp
// Activation
bool ActivateDialogueCamera(AActor* TargetActor, const FDialogueCameraSettings& Settings);
bool ActivateDialogueCameraWithFraming(AActor* TargetActor, EDialogueCameraFraming Framing);
void DeactivateDialogueCamera();

// Runtime Control
void TransitionToTarget(AActor* NewTarget, float BlendTime);
void UpdateCameraFraming(EDialogueCameraFraming NewFraming, float BlendTime);

// Queries
bool IsDialogueCameraActive() const;
FDialogueCameraSettings GetCurrentCameraSettings() const;
```

### DialogueInputBlockerComponent

```cpp
// Activation
bool BlockInput(const FDialogueInputBlockingSettings& Settings);
bool BlockInputWithDefaults();
void UnblockInput();

// Runtime Control
void UpdateBlockingMode(EInputBlockingMode NewMode);
void AllowInputAction(FName ActionName);

// Queries
bool IsInputBlocked() const;
FDialogueInputBlockingSettings GetCurrentBlockingSettings() const;
```

### DialoguePresentationComponent

```cpp
// Activation
bool ActivatePresentation(AActor* TargetNPC, EDialoguePresentationMode Mode);
bool ActivatePresentationWithSettings(AActor* TargetNPC, const FDialoguePresentationSettings& Settings);
void DeactivatePresentation();

// Runtime Control
void TransitionToNewTarget(AActor* NewTargetNPC, float BlendTime);
void UpdatePresentationMode(EDialoguePresentationMode NewMode);

// Configuration
void SetModeSettings(EDialoguePresentationMode Mode, const FDialoguePresentationSettings& Settings);
FDialoguePresentationSettings GetModeSettings(EDialoguePresentationMode Mode) const;

// Queries
bool IsPresentationActive() const;
UDialogueCameraComponent* GetCameraComponent() const;
UDialogueInputBlockerComponent* GetInputBlockerComponent() const;
```

## Performance Metrics

### Typical Performance Impact
- **Camera System**: < 0.1ms per frame when active
- **Input Blocking**: Negligible (event-based)
- **Memory**: ~1-2KB per active presentation

### Optimization Tips
1. Disable tick when presentation is inactive
2. Use timer-based tracking instead of tick
3. Cache component references
4. Pool camera actors if creating many
5. Use coarser tracking update rates (0.5s instead of every frame)

## See Also

- [Dialogue System Overview](../Documentation/Overview.md)
- [Async Loading Guide](../Documentation/AsyncLoading_Guide.md)
- [UI System Guide](../Documentation/UI_Guide.md)
- [Relationship System Guide](../Documentation/Relationship_Guide.md)
