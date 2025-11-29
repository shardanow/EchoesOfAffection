# TurnToFace Component - Blueprint Implementation

## ? GOOD NEWS: Events Are Working!

Your logs show:
```
[GameEventBus] EmitEvent: 'GameEvent.Dialogue.TurnToFace'
SpeakerActor: BP_Lianne_C_1
ListenerActor: bp_MyAnimeCharacter_C_0
```

**Problem:** No one is listening to the event!

---

## ?? Solution: Add Event Handler to BP_Lianne

### Option 1: Quick Blueprint Implementation (5 minutes)

#### Step 1: Open BP_Lianne (or BP_NPCActor base class)

1. Content Browser ? Find `BP_Lianne`
2. Double-click to open
3. Go to **Event Graph**

#### Step 2: Add BeginPlay Subscription

```
Event BeginPlay
  ?
Sequence  (for multiple outputs)
  ? Then 0
  [Existing BeginPlay logic...]
  
  ? Then 1  (NEW)
Get World
  ?
Get Subsystem (Class: GameEventBusSubsystem)
  ?
Branch (Is Valid?)
  TRUE:
    ?
    Subscribe To Event Native
      Event Tag: "GameEvent.Dialogue.TurnToFace" (type as string)
      Delegate: Create Event ? "OnTurnToFaceEvent"
    ?
    Print String: "TurnToFace listener registered for [Actor Name]"
```

#### Step 3: Add Custom Event Handler

```
Custom Event: OnTurnToFaceEvent
  Input: Payload (type: FGameEventPayload)
  
  ?
Branch (Payload.InstigatorActor == Self?)
  FALSE: Return (not me, ignore)
  
  TRUE:
    ?
    Print String: "I should turn to: [Payload.TargetActor.GetName()]"
    ?
    Branch (Is Valid? Payload.TargetActor)
      FALSE: Return
      
   TRUE:
        ?
        Get Actor Location (Self)
        ?
      Get Actor Location (Payload.TargetActor)
    ?
        Find Look At Rotation
   Start: Self Location
          Target: Target Location
        ?
      Break Rotator (Look At Rotation)
          Get: Yaw only
      ?
        Get Actor Rotation (Self)
        ?
        Break Rotator (Current Rotation)
          Get: Pitch, Roll
        ?
      Make Rotator
 Pitch: Current Pitch
  Yaw: Look At Yaw  ? NEW rotation
     Roll: Current Roll
        ?
Set Actor Rotation (New Rotation)
  ?
        Print String: "Turned to face [Target Name]"
```

---

### Option 2: Use Timeline for Smooth Rotation (10 minutes)

For smooth interpolation:

```
Custom Event: OnTurnToFaceEvent
  Input: Payload (FGameEventPayload)
  
  ?
[Same validation as above...]
  ?
Store Target Rotation in variable (Find Look At Rotation)
  ?
Store Start Rotation (Get Actor Rotation)
  ?
Get Duration from Payload.FloatParam (default 0.5)
  ?
Create Timeline
  Name: "TurnTimeline"
  Length: Duration
  Curve: Float curve (0 to 1, linear or ease in/out)
  
  On Update:
    ?
    Lerp (Rotator)
 A: Start Rotation
      B: Target Rotation
      Alpha: Timeline Output (0-1)
    ?
    Set Actor Rotation (Lerped Rotation)
  
  On Finished:
    ?
    Print String: "Turn completed!"
```

---

## ?? Screenshot Guide

### BeginPlay Node:
```
????????????????????
?  Event BeginPlay ?
????????????????????
         ?
 ?
????????????????????
?    Sequence      ?
????????????????????
? Then 0 ? Then 1  ?  ? Add new pin
????????????????????
     ?        ?
 [Existing] [NEW: Subscribe]
```

### Subscribe Node:
```
??????????????????????????????
? Get Subsystem    ?
? (GameEventBusSubsystem)    ?
??????????????????????????????
?
??????????????????????????????
? Subscribe To Event Native  ?
??????????????????????????????
? Event Tag: GameEvent.      ?
?     Dialogue.        ?
?           TurnToFace       ?
? Delegate: OnTurnToFaceEvent?
??????????????????????????????
```

### Handler Node:
```
?????????????????????????????
? Custom Event:          ?
? OnTurnToFaceEvent         ?
? Payload: FGameEventPayload?
?????????????????????????????
          ?
?????????????????????????????
? Branch          ?
? Condition:                ?
? Payload.InstigatorActor?
? == Self ?
?????????????????????????????
  FALSE?      TRUE?
  Return       Continue...
```

---

## ?? Testing After Implementation

### Expected Output Log:

**On Game Start:**
```
LogBlueprintUserMessages: TurnToFace listener registered for BP_Lianne_C_1
```

**On Dialogue Start:**
```
[GameEventBus] EmitEvent: 'GameEvent.Dialogue.TurnToFace'
LogBlueprintUserMessages: I should turn to: bp_MyAnimeCharacter_C_0
LogBlueprintUserMessages: Turned to face bp_MyAnimeCharacter_C_0
```

**Visual Result:**
- BP_Lianne rotates to face player
- Smooth or instant (depending on implementation)

---

## ?? Quick Checklist

- [ ] BP_Lianne opened in editor
- [ ] Event Graph opened
- [ ] BeginPlay ? Subscribe To Event Native added
- [ ] Custom Event "OnTurnToFaceEvent" created
- [ ] Branch: InstigatorActor == Self added
- [ ] Find Look At Rotation added
- [ ] Set Actor Rotation added
- [ ] Compile blueprint (green checkmark)
- [ ] Save blueprint
- [ ] Test in PIE
- [ ] Check Output Log for "I should turn to..."

---

## ? Super Quick Test (30 seconds)

If you just want to see if it works WITHOUT smooth rotation:

```
Custom Event: OnTurnToFaceEvent
  ?
Branch (Payload.InstigatorActor == Self)
  TRUE:
    ?
    Find Look At Rotation (Self ? Target)
    ?
    Set Actor Rotation (Look At Rotation)
```

That's it! No lerp, no timeline, just instant snap.

---

## ?? Troubleshooting

### Issue: "Subscribe To Event Native" node not found

**Solution:** 
1. Right-click in Event Graph
2. Search: "Subscribe To Event Native"
3. If not found:
   - Uncheck "Context Sensitive"
   - Search again
   - Select from list

---

### Issue: FGameEventPayload not in dropdown

**Solution:**
1. Drag from Payload pin
2. Search for "Break Game Event Payload"
3. This gives you access to all fields:
   - InstigatorActor
   - TargetActor
   - StringParam (mode)
   - FloatParam (duration)
   - IntParam (bHeadOnly)

---

### Issue: Actor doesn't rotate smoothly

**Solution:**
Use Timeline (Option 2 above) or:
```
RInterp To (Rotator Interpolation)
  Current: Get Actor Rotation
  Target: Look At Rotation
  Delta Time: Get World Delta Seconds
  Interp Speed: 5.0
```

Call this in **Event Tick** while rotating.

---

## ?? Additional Resources

### Full C++ Component (if you prefer):

I can create a full `UTurnToFaceComponent` in C++ that:
- Subscribes automatically
- Handles smooth rotation
- Configurable speed
- Just drop on actor

Let me know if you want this!

---

## ?? Summary

**Current Status:**
- ? Turn-to-face events WORKING
- ? GameEventBus WORKING
- ? Actor resolution WORKING
- ? No listener in BP_Lianne

**Next Step:**
Add Blueprint handler (5-10 minutes)

**After that:**
Actor will turn automatically! ??

---

**Let me know when you add the Blueprint handler and I'll help test!** ??
