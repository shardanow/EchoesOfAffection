# Relationship System Troubleshooting Guide

## Problem: "Dimension Bar показывает 50% при значении 0"

### Symptoms
- ProgressBar визуально заполнен наполовину
- Текст показывает `"Trust: 0"` или `"Friendship: 0"`
- Ожидалось 0%, но показывается 50%

### Root Cause
DataAssets используют диапазон **-100 до +100**, где **0 = середина = 50%**

### Quick Fix (1 минута)

1. Откройте DataAssets:
   - `Content/RelationshipSystem/DataAssets/DA_Dimension_Trust`
   - `Content/RelationshipSystem/DataAssets/DA_Dimension_Friendship`

2. Измените параметры:
   ```
   Min Value: 0 (было: -100)
   Max Value: 100
   Default Value: 0
   ```

3. Сохраните и запустите игру

### Результат
? **0** ? **0%** (пустой bar)
? **50** ? **50%** (половина)
? **100** ? **100%** (полный bar)

?? **Подробности:** см. `Documentation/DIMENSION_BAR_50_PERCENT_ISSUE.md`

---

## Problem: "No relationships visible in UI"

### Symptoms
- Relationship panel shows no dimensions
- `GetCurrentState()` returns invalid tag
- `GetDimensionValue()` returns 0

### Root Cause
**Relationship does not exist** between Player and NPC.

### Solution Steps

#### 1?? **Check if NPCs have RelationshipComponent**

In Blueprint editor (BP_Lianne, BP_Alice, etc.):
- Open Components panel
- Look for `NPCRelationshipComponent`
- If missing ? Add it!

#### 2?? **Assign RelationshipProfile to NPCs**

In NPC Blueprint Details panel:
- Select `NPCRelationshipComponent`
- Set **Profile** to a valid `RelationshipProfile` DataAsset
- Example: `DA_Profile_Friendly` or `DA_Profile_Neutral`

#### 3?? **Check Player has RelationshipComponent**

Player Character Blueprint (`bp_MyAnimeCharacter`):
- Add `NPCRelationshipComponent`
- Assign Profile (e.g., `DA_Profile_Player`)

#### 4?? **Create relationship programmatically**

Option A - Auto-create on dialogue start:

```cpp
// In DialogueSubsystem::StartDialogue_Implementation
if (UNPCRelationshipComponent* PlayerComp = Player->FindComponentByClass<UNPCRelationshipComponent>())
{
    if (UNPCRelationshipComponent* NPCComp = NPC->FindComponentByClass<UNPCRelationshipComponent>())
    {
        if (!PlayerComp->HasRelationshipWith(NPC))
        {
   PlayerComp->CreateRelationshipWith(NPC);
UE_LOG(LogDialogueSubsystem, Log, TEXT("Created relationship: %s <-> %s"), 
             *Player->GetName(), *NPC->GetName());
        }
    }
}
```

Option B - Use `bAutoCreateRelationships`:

In Player/NPC RelationshipComponent:
- Set `bAutoCreateRelationships = true`
- Set `AutoCreateRadius` (e.g., `5000.0`)
- Optionally set `AutoCreateFilterTags`

#### 5?? **Verify relationship creation**

Run game, check Output Log for:
```
[LogRelationshipSystem] Created relationship between Player <-> NPC_Lianne
```

If missing ? relationship was never created!

#### 6?? **Check RelationshipDatabase**

Project Settings ? Plugins ? Relationship System:
- Ensure **Relationship Database** is set
- Database must contain:
  - At least 1 Dimension (e.g., Trust, Friendship)
  - At least 1 State (e.g., Stranger, Friend)
  - At least 1 Profile (default profile)

---

## Problem: "Warning: No profile assigned to BP_Lianne_C_1"

### Fix
1. Open `BP_Lianne` Blueprint
2. Select `NPCRelationshipComponent`
3. In Details ? **Profile** ? Select a `RelationshipProfile` DataAsset
4. Save Blueprint

---

## Problem: "Dimensions show 0 value"

### Check
1. Relationship exists? ? Use `HasRelationshipWith(NPC)`
2. Dimension exists in Database? ? Check `RelationshipDatabase` DataAsset
3. Initial value set? ? Check `RelationshipProfile` ? `InitialDimensions`

---

## Problem: "Panel not showing during dialogue"

### Check Event Integration

1. **GameEventBus enabled?**
   - Check logs for: `? RelationshipPanel: Subscribed to GameEventBus events`
   - If showing `?? GameEventBus not available` ? Check `.Build.cs` dependencies

2. **Events firing?**
   - Look for: `[LogQuestBridge] ?? Received game event: 'GameEvent.Dialogue.Started'`

3. **SetActors called?**
   - Add debug log in `RelationshipPanel::SubscribeToEvents` lambda:
   ```cpp
   UE_LOG(LogTemp, Log, TEXT("RelationshipPanel: SetActors called - Player=%s, NPC=%s"),
       *GetNameSafe(Player), *GetNameSafe(NPC));
   ```

---

## Debug Commands

### Check active relationships
Blueprint:
```
GetAllRelationshipTargets() ? Print count
```

C++:
```cpp
URelationshipSubsystem* Subsystem = GetGameInstance()->GetSubsystem<URelationshipSubsystem>();
int32 Count = Subsystem->GetRelationshipsAsSubject(PlayerActor).Num();
UE_LOG(LogTemp, Warning, TEXT("Player has %d relationships"), Count);
```

### Force create relationship
```cpp
PlayerComponent->CreateRelationshipWith(NPC);
```

### Check dimension value
```cpp
float Value = PlayerComponent->GetDimensionValue(NPC, FGameplayTag::RequestGameplayTag("Dimension.Trust"));
UE_LOG(LogTemp, Warning, TEXT("Trust value: %.2f"), Value);
```

---

## Common Mistakes

? **Forgetting to assign Profile** ? Component initializes with default/empty profile
? **No Database configured** ? System can't function
? **Missing GameEventBus dependency** ? Event-driven updates won't work
? **Never creating relationship** ? Components exist but no relationship data
? **Wrong Subject/Target order** ? Relationships are directional!

---

## Success Checklist

? Both actors have `NPCRelationshipComponent`
? Both components have `Profile` assigned
? `RelationshipDatabase` configured in Project Settings
? Relationship created (auto or manual)
? GameEventBus integrated and subscribed
? Panel receives `SetActors(Player, NPC)` on dialogue start

---

## Still Not Working?

Enable verbose logging:
```
LogRelationshipSystem VeryVerbose
LogRelationshipSystemUI VeryVerbose
```

Then check Output Log during gameplay for detailed execution flow.
