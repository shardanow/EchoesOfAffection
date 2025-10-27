# Needs System - Quick Start Guide

## 5-Minute Setup

### Step 1: Create Needs (2 min)

1. Right-click in Content Browser ? **Miscellaneous ? Data Asset**
2. Choose **Need Definition Asset**
3. Create needs:

**Hunger:**
- Name: `DA_Need_Hunger`
- Tag: `Need.Hunger`
- Initial Value: `100`
- Decay Rate: `5.0` (points per game hour)
- Warning: `30`, Critical: `15`, Recovered: `50`

**Energy:**
- Name: `DA_Need_Energy`
- Tag: `Need.Energy`
- Initial Value: `100`
- Decay Rate: `10.0`
- Warning: `25`, Critical: `10`, Recovered: `40`

**Repeat for:** Social, Comfort, Stress, Appearance (optional)

---

### Step 2: Create Settings (1 min)

1. Create **Needs Settings Asset**: `DA_NeedsSettings`
2. Add your needs to "Available Needs" array
3. Configure:
   - Enable Mood: ?
   - Update Frequency: `10.0`
   - LOD Far Distance: `2000.0`
   - LOD Parked Distance: `5000.0`
   - **Use Game Time:** ? or ? (see below)
   - **Real Time Scale:** `1.0` (1 game hour = 1 real hour)
   - **Game Time Fallback Scale:** `1.0`

4. Save to: `Content/NeedsSystem/DA_NeedsSettings`

**?? IMPORTANT - Time Scale Settings:**

The time scale controls how fast needs decay:

- **Real Time Scale = 1.0** (DEFAULT, RECOMMENDED):
  - 1 game hour = 1 real hour
  - Realistic decay over hours of gameplay
  - Example: Decay rate 5.0/hour = loses 5 points every real hour

- **Real Time Scale = 0.1** (SLOWER):
  - 10 real hours = 1 game hour
  - Very slow decay for testing
  - Example: Decay rate 5.0/hour = loses 5 points every 10 real hours

- **Real Time Scale = 24.0** (FASTER):
  - 1 real hour = 24 game hours
  - Fast decay for quick testing
  - Example: Decay rate 5.0/hour = loses 5 points every 2.5 real minutes

**If you see needs dropping instantly:**
- Your `Real Time Scale` or `Game Time Fallback Scale` is probably set to 24.0
- Change it to **1.0** for normal speed
- Change it to **0.1** for slower testing

---

### Step 3: Add Component (30 sec)

**Blueprint:**
Open your Character Blueprint ? **Add Component** ? **Character Needs Component**

**C++:**
```cpp
// YourCharacter.h
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Needs")
UCharacterNeedsComponent* NeedsComponent;

// YourCharacter.cpp (constructor)
NeedsComponent = CreateDefaultSubobject<UCharacterNeedsComponent>(TEXT("NeedsComponent"));
```

---

### Step 4: Create Food Effect (1 min)

1. Create **Need Effect Asset**: `DA_Effect_EatBread`
2. Configure:
   - Tag: `Need.Effect.EatBread`
   - Effect Type: **Instant**
   - Modifications ? Add:
     - Target: `Need.Hunger`
     - Value: `30.0`

---

### Step 5: Test! (30 sec)

**Blueprint:**
```blueprint
// On "E" key pressed
Event E Pressed
  ?? Get Needs Component
     ?? Apply Effect (DA_Effect_EatBread, 1)
```

**C++:**
```cpp
void AMyCharacter::Eat()
{
    if (NeedsComponent && BreadEffect)
    {
        NeedsComponent->ApplyEffect(BreadEffect, 1);
    }
}
```

**Debug:**
- Open Console: `needs.debug 1`
- See needs above character head!

---

## 10-Minute Advanced Setup

### Add Sleep System

**Sleep Effect** (`DA_Effect_Sleep`):
- Effect Type: **Over Time**
- Duration: `8.0` hours
- Modifications:
  - Energy: `+100.0`
- Granted Tags: `State.Sleeping`

**Well Rested Modifier** (`DA_Modifier_WellRested`):
- Priority: `10`
- Modifications:
  - Energy Decay Rate: **Multiply** by `0.5`

**Blueprint Sleep Function:**
```blueprint
Function: Start Sleep
?? Apply Effect (DA_Effect_Sleep, 1)
?? Suppress Need (Need.Hunger, true)
?? On Effect Expired:
   ?? Add Modifier (DA_Modifier_WellRested)
   ?? Suppress Need (Need.Hunger, false)
```

---

### Add UI

1. Create Widget Blueprint: `WBP_NeedsPanel`
2. Parent Class: **Needs Panel Widget**
3. Add Hierarchy:
   ```
   VerticalBox [NeedsContainer]
   TextBlock [MoodTextBlock]
   ```
4. Details:
   - Need Widget Class: **Need Display Widget**
   - Auto Update: ?
   - Show Mood: ?

5. Add to viewport:
   ```blueprint
   Event Begin Play
   ?? Create Widget (WBP_NeedsPanel)
      ?? Set Target Component (Get Needs Component)
         ?? Add to Viewport
   ```

---

## Common Patterns

### Pattern 1: Check Before Action

```blueprint
Function: Can Perform Action
?? Get Current Mood ? Mood
?? Return (Mood > -20.0)
```

### Pattern 2: React to Critical State

```blueprint
Event: On Need Threshold Changed
?? IF NeedTag == Need.Hunger AND NewLevel == Critical
?  ?? Start Damage Timer
?? ELIF NewLevel == Recovered
   ?? Stop Damage Timer
```

### Pattern 3: Environmental Effects

```blueprint
Function: Update Environment
?? Get All Characters in Radius
?? FOR EACH Character:
   ?? Get Needs Component
   ?? IF In Hot Zone:
      ?? Add Modifier (HeatStress)
```

### Pattern 4: Item Consumption

```blueprint
Function: Use Item
?? Get Item Effect (from data table)
?? Apply Effect (ItemEffect, 1)
?? Remove Item from Inventory
```

---

## Troubleshooting

### Needs not decaying?
- Check Settings path: `/NeedsSystem/DA_NeedsSettings`
- Verify `Update Frequency > 0`
- Check Time Integration settings

### UI not updating?
- Verify widget parent class: `NeedsPanelWidget`
- Check `Auto Update` is enabled
- Confirm `Target Component` is set

### Effects not applying?
- Check Gameplay Tags are correct
- Verify effect is not blocked by tags
- Check console for errors

### Low performance?
- Increase `Update Frequency` (lower = faster updates)
- Adjust LOD distances
- Reduce `Batch Size`

---

## Next Steps

1. **Read Full Docs:** `README.md`
2. **Study Examples:** `EXAMPLES.md`
3. **Check API:** `API.md`
4. **Integration Guide:** `INTEGRATION.md`

---

## Quick Reference

### Console Commands

```
needs.debug 1// Enable debug visualization
needs.debug 0 // Disable debug visualization
```

### Common Blueprint Nodes

```
Get Needs Component
Apply Effect
Add Modifier
Get Need Value
Get Current Mood
Set Need Value
Suppress Need
Freeze Need
```

### Common C++ Functions

```cpp
ApplyEffect(Effect, StackCount)
AddModifier(Modifier)
GetNeedValue(Tag)
SetNeedValue(Tag, Value)
GetCurrentMood()
RegisterListener(Listener)
```

---

**You're ready! ??**

Start with basic needs, add effects as you go, and expand with modifiers and UI!
