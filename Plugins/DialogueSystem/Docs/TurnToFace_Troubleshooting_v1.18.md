# Turn-To-Face Quick Test Component

## Blueprint Implementation (BP_NPCActor)

### Step 1: Add Event Graph Node

1. Open `BP_NPCActor` (or your NPC blueprint)
2. Go to **Event Graph**
3. Add this logic:

```
Event BeginPlay
  ?
Get World
  ?
Get Subsystem (GameEventBusSubsystem)
  ?
Subscribe To Event Native
  - Event Tag: "GameEvent.Dialogue.TurnToFace"
  - Delegate: Create Event ? OnTurnToFaceReceived
  ?
(Store handle for cleanup)


Custom Event: OnTurnToFaceReceived
  Input: Payload (FGameEventPayload)
  ?
Branch (Payload.InstigatorActor == Self)
  TRUE:
    ?
    Print String: "TURN TO FACE EVENT RECEIVED!"
    ?
    Print String: Target = [Payload.TargetActor.GetName()]
    ?
    Find Look At Rotation
      - Start: GetActorLocation()
      - Target: Payload.TargetActor.GetActorLocation()
    ?
    Set Actor Rotation
      - New Rotation: [Look At Rotation (Yaw only)]
```

### Step 2: Test Log Output

**Expected Output Log:**
```
LogBlueprintUserMessages: TURN TO FACE EVENT RECEIVED!
LogBlueprintUserMessages: Target = bp_MyAnimeCharacter_C_0
```

---

## C++ Test Component (Alternative)

If you prefer C++, create `UTurnToFaceTestComponent`:

```cpp
// TurnToFaceTestComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TurnToFaceTestComponent.generated.h"

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class UTurnToFaceTestComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTurnToFaceTestComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void OnTurnToFaceEvent(const FGameEventPayload& Payload);
    
 FDelegateHandle EventHandle;
};
```

```cpp
// TurnToFaceTestComponent.cpp
#include "TurnToFaceTestComponent.h"
#include "GameEventBusSubsystem.h"
#include "Kismet/KismetMathLibrary.h"

UTurnToFaceTestComponent::UTurnToFaceTestComponent()
{
PrimaryComponentTick.bCanEverTick = false;
}

void UTurnToFaceTestComponent::BeginPlay()
{
    Super::BeginPlay();

#if WITH_GAMEEVENTBUS
    UWorld* World = GetWorld();
    if (!World) return;

    UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
    if (!EventBus) return;

    FGameplayTag TurnToFaceTag = FGameplayTag::RequestGameplayTag("GameEvent.Dialogue.TurnToFace");
    
    auto Lambda = [this](const FGameEventPayload& Payload)
    {
        this->OnTurnToFaceEvent(Payload);
    };
    
    EventHandle = EventBus->SubscribeToEventNative(
        TurnToFaceTag, 
        FGameEventNativeDelegate::FDelegate::CreateLambda(Lambda)
    );
    
  UE_LOG(LogTemp, Warning, TEXT("[TEST] TurnToFaceTestComponent subscribed to events"));
#endif
}

void UTurnToFaceTestComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if WITH_GAMEEVENTBUS
    UWorld* World = GetWorld();
    if (World)
    {
        UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
        if (EventBus && EventHandle.IsValid())
 {
            EventBus->UnsubscribeFromEvent(EventHandle);
        }
    }
#endif

    Super::EndPlay(EndPlayReason);
}

void UTurnToFaceTestComponent::OnTurnToFaceEvent(const FGameEventPayload& Payload)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // Check if I should turn
    if (Payload.InstigatorActor != Owner)
    {
        return; // Not me
  }

    AActor* Target = Payload.TargetActor;
    if (!Target)
    {
        UE_LOG(LogTemp, Error, TEXT("[TEST] No target actor!"));
     return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[TEST] TURN TO FACE EVENT RECEIVED!"));
    UE_LOG(LogTemp, Warning, TEXT("[TEST]   Owner: %s"), *Owner->GetName());
    UE_LOG(LogTemp, Warning, TEXT("[TEST]   Target: %s"), *Target->GetName());
    UE_LOG(LogTemp, Warning, TEXT("[TEST]   Mode: %s"), *Payload.StringParam.ToString());
    UE_LOG(LogTemp, Warning, TEXT("[TEST]   Duration: %.2f"), Payload.FloatParam);

    // Calculate look-at rotation
    FVector OwnerLocation = Owner->GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);

    // Yaw only (don't tilt up/down)
    FRotator CurrentRotation = Owner->GetActorRotation();
    FRotator NewRotation = FRotator(CurrentRotation.Pitch, LookAtRotation.Yaw, CurrentRotation.Roll);

    // Apply rotation (instant for test)
    Owner->SetActorRotation(NewRotation);

    UE_LOG(LogTemp, Warning, TEXT("[TEST] Rotation applied! Old: %.1f, New: %.1f"), 
    CurrentRotation.Yaw, NewRotation.Yaw);
}
```

**Usage:**
1. Add `TurnToFaceTestComponent` to your NPC blueprint
2. Start dialogue
3. Check Output Log for `[TEST]` messages

---

## Checklist for Debugging

### ? Step 1: Verify ListenerId is Set
```
1. Open DialogueDataAsset in editor
2. Select node where NPC speaks
3. Check "ListenerId" field in Details panel
4. Should be "Player" (not empty!)
```

### ? Step 2: Check Logs for Event Emission
```
Output Log ? Search for "[TURN-TO-FACE]"

Expected:
  [TURN-TO-FACE] Auto turn-to-face: 'Lianne' turns to 'Player'
  [TURN-TO-FACE]   SpeakerActor: BP_Lianne_C_1
  [TURN-TO-FACE]   ListenerActor: bp_MyAnimeCharacter_C_0

If MISSING:
  - ListenerId not set correctly
  - DialogueRunner not calling EmitTurnToFaceEvent
  - Actors not resolved
```

### ? Step 3: Check Logs for Event Reception
```
Output Log ? Search for "[TEST]" or "TURN TO FACE EVENT RECEIVED"

If MISSING:
  - No subscribers to event
  - GameEventBus not working
  - Need to add test component
```

### ? Step 4: Verify GameEventBus Works
```
Test with other events:
  - GameEvent.Dialogue.NodeEntered (camera tracking works?)
  - If camera tracking works ? GameEventBus is OK
  - If not ? GameEventBus issue
```

---

## Common Issues

### Issue 1: "ListenerId" field not visible in editor

**Cause:** Editor not reloaded after compilation

**Solution:**
1. Close Unreal Editor
2. Restart Editor
3. Open DialogueDataAsset
4. Check if "ListenerId" field now visible

---

### Issue 2: Logs show event emitted but actor doesn't turn

**Cause:** No subscriber listening to event

**Solution:**
Add test component (Blueprint or C++ above)

---

### Issue 3: "Listener actor not found for 'Player'"

**Cause:** "Player" PersonaId not registered in DialogueParticipants

**Solution:**
Check how Player is registered. Should be:
```cpp
Participants->SetActorForPersona(FName("Player"), PlayerActor);
```

Or in DialogueDataAsset:
```
Primary Persona:
  PersonaId: Player  ? Must match ListenerId!
```

---

## Quick Test Script

Run this in Output Log console:

```
stat game
```

Then start dialogue and watch for:
```
[TURN-TO-FACE] Auto turn-to-face: ...
```

If you see this ? Event is emitting!
If you don't ? Check ListenerId field.

---

**Next Step:** Add test component and report back what you see in logs!
