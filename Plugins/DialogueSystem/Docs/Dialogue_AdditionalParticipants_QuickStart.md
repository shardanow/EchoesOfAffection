# Dialogue Additional Participants - Quick Start

**Version:** 1.13.4  
**Last Updated:** 2025.01.05

---

## Overview

This guide explains how to add **Additional Participants** to dialogues so they also pause their schedules during dialogue.

### Problem

By default, only **Player** (Instigator) and **NPC** (Target) are considered dialogue participants. If other NPCs are nearby (e.g., Alice, Bob), they will **continue their schedules** even while dialogue is active.

### Solution

Use the **`AdditionalParticipants`** system to register extra actors as dialogue participants. These actors will automatically:
- ? Pause their `ScheduleComponent` when dialogue starts
- ? Resume their schedules when dialogue ends

---

## Method 1: Blueprint - Add Participants at Runtime

### Step 1: Get Dialogue Context

In your dialogue setup (e.g., `BeginPlay` or interaction trigger):

```cpp
// C++ Example
UDialogueSessionContext* Context = DialogueRunner->GetCurrentContext();
if (Context && Context->GetParticipants())
{
    // Add Alice as additional participant
    Context->GetParticipants()->AddAdditionalParticipant(AliceActor);
    Context->GetParticipants()->AddAdditionalParticipant(BobActor);
}
```

### Step 2: Start Dialogue

```cpp
TArray<UObject*> Participants;
Participants.Add(PlayerActor);  // Player (Instigator)
Participants.Add(NPCActor);     // NPC (Target)

DialogueRunner->StartDialogue(DialogueAsset, Participants);
```

**Important:** Call `AddAdditionalParticipant()` **BEFORE** starting the dialogue, OR immediately after (before first node is processed).

---

## Method 2: Dialogue Effect - Add Participants Dynamically

### Step 1: Create Custom Dialogue Effect

```cpp
// DialogueEffect_AddParticipant.h
UCLASS()
class DIALOGUESYSTEMCORE_API UDialogueEffect_AddParticipant : public UDialogueEffect
{
    GENERATED_BODY()

public:
    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;

    /** Actor tag to search for */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|Effect")
    FGameplayTag ActorTag;
};

// DialogueEffect_AddParticipant.cpp
void UDialogueEffect_AddParticipant::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (!Context || !Context->GetParticipants())
    {
        return;
    }

    UWorld* World = Context->GetWorld();
    if (!World)
    {
        return;
    }

    // Find actor by gameplay tag
    for (TActorIterator<AActor> It(World); It; ++It)
    {
 AActor* Actor = *It;
        if (Actor->Implements<UGameplayTagAssetInterface>())
        {
            FGameplayTagContainer ActorTags;
     IGameplayTagAssetInterface::Execute_GetOwnedGameplayTags(Actor, ActorTags);
     
            if (ActorTags.HasTag(ActorTag))
            {
           Context->GetParticipants()->AddAdditionalParticipant(Actor);
     UE_LOG(LogDialogue, Log, TEXT("Added additional participant: %s"), *Actor->GetName());
                break;
}
        }
    }
}
```

### Step 2: Add Effect to Dialogue Node

1. Open your dialogue asset (e.g., `DT_Lianne_Greeting`)
2. Select the **Start Node** or first **NPC node**
3. Add effect: **`DialogueEffect_AddParticipant`**
4. Set `ActorTag` = `"NPC.Alice"` (or whatever tag your actor has)

---

## Method 3: Auto-Detect Nearby NPCs (Advanced)

### Create Auto-Discovery Effect

```cpp
// DialogueEffect_AddNearbyNPCs.h
UCLASS()
class DIALOGUESYSTEMCORE_API UDialogueEffect_AddNearbyNPCs : public UDialogueEffect
{
    GENERATED_BODY()

public:
    virtual void Execute_Implementation(UDialogueSessionContext* Context) override;

    /** Search radius (cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|Effect")
    float SearchRadius = 500.0f;

    /** NPC class filter (leave empty for all actors) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|Effect")
    TSubclassOf<AActor> NPCClass;
};

// DialogueEffect_AddNearbyNPCs.cpp
void UDialogueEffect_AddNearbyNPCs::Execute_Implementation(UDialogueSessionContext* Context)
{
    if (!Context || !Context->GetParticipants())
    {
        return;
    }

    AActor* NPC = Context->GetNPC();
    if (!NPC)
{
        return;
    }

    UWorld* World = NPC->GetWorld();
    if (!World)
    {
        return;
    }

    // Find nearby actors
    TArray<AActor*> NearbyActors;
    FVector NPCLocation = NPC->GetActorLocation();

    for (TActorIterator<AActor> It(World, NPCClass); It; ++It)
    {
  AActor* Actor = *It;
    
        // Skip self and player
  if (Actor == NPC || Actor == Context->GetPlayer())
        {
            continue;
   }

        // Check distance
   float Distance = FVector::Distance(NPCLocation, Actor->GetActorLocation());
        if (Distance <= SearchRadius)
        {
       Context->GetParticipants()->AddAdditionalParticipant(Actor);
       UE_LOG(LogDialogue, Log, TEXT("Auto-added nearby NPC: %s (Distance: %.1f)"), 
      *Actor->GetName(), Distance);
    }
    }
}
```

### Usage

1. Add **`DialogueEffect_AddNearbyNPCs`** to your dialogue's **Start Node**
2. Set **`SearchRadius`** = `500.0` (or desired range)
3. Set **`NPCClass`** = `BP_NPC_Base` (your NPC base class)

**Result:** All NPCs within 500cm will pause their schedules during dialogue.

---

## Verification

### Check Logs

When dialogue starts, you should see:

```
LogDialogueRunner: [GAMEEVENTBUS] EMITTING: GameEvent.Dialogue.Started
LogDialogueRunner: [GAMEEVENTBUS]   DialogueId: lianne_greeting
LogDialogueRunner: [GAMEEVENTBUS]   Player (Instigator): bp_MyAnimeCharacter_C_0
LogDialogueRunner: [GAMEEVENTBUS]   NPC (Target): BP_Lianne_C_1
LogDialogueRunner: [GAMEEVENTBUS]   AdditionalParticipants: 1  <-- Alice should appear here
```

Then for **each** additional participant:

```
LogScheduleComponent: [EVENT] OnDialogueStarted received for actor: BP_Alice_C_1
LogScheduleComponent: [EVENT]   bIsInvolvedInDialogue: YES  <-- Fixed!
LogScheduleComponent: [EVENT] Dialogue started - pausing schedule for actor: BP_Alice_C_1
LogScheduleComponent: [PAUSE] Pausing schedule for actor: BP_Alice_C_1
```

---

## API Reference

### UDialogueParticipants

```cpp
// Add participant
UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
void AddAdditionalParticipant(UObject* Participant);

// Remove participant
UFUNCTION(BlueprintCallable, Category = "Dialogue|Participants")
void RemoveAdditionalParticipant(UObject* Participant);

// Get all additional participants
UFUNCTION(BlueprintPure, Category = "Dialogue|Participants")
TArray<UObject*> GetAdditionalParticipants() const;
```

### UDialogueSessionContext

```cpp
// Get participants manager
UFUNCTION(BlueprintPure, Category = "Context|Components")
UDialogueParticipants* GetParticipants() const;
```

---

## Best Practices

### ? DO

- Add participants **before** starting dialogue OR in the **Start Node effect**
- Use `SearchRadius` to avoid affecting NPCs far away
- Log added participants for debugging
- Clean up participants after dialogue ends (automatic with `EndDialogue()`)

### ? DON'T

- Add participants **after** first node processes (event already emitted)
- Add the **Player** or **NPC** as additional participants (they're already tracked)
- Forget to check for `nullptr` when adding actors
- Add non-Actor objects (will be filtered out)

---

## Troubleshooting

### Issue: Alice still moving during dialogue

**Check:**
1. ? Alice added to `AdditionalParticipants` BEFORE dialogue starts?
2. ? Log shows `AdditionalParticipants: 1` or more?
3. ? Alice's `ScheduleComponent` subscribed to `GameEvent.Dialogue.Started`?

**Fix:**
```cpp
// C++ - Check subscription
UScheduleComponent* AliceSchedule = Alice->FindComponentByClass<UScheduleComponent>();
if (AliceSchedule)
{
    UE_LOG(LogTemp, Log, TEXT("Alice has ScheduleComponent: Subscribed = %s"),
        AliceSchedule->IsSubscribedToDialogueEvents() ? TEXT("YES") : TEXT("NO"));
}
```

### Issue: AdditionalParticipants always 0

**Cause:** Participants added **too late** (after `EmitDialogueStartedEvent()` called).

**Fix:** Move `AddAdditionalParticipant()` call to **dialogue start node effect** OR call it **immediately after** `StartDialogue()` but **before** first tick.

---

## Example: Complete Integration

```cpp
// In your Interactable component or dialogue trigger
void UMyDialogueTrigger::BeginInteract_Implementation(AActor* InteractingActor)
{
    UDialogueSubsystem* DialogueSys = UGameInstance::GetSubsystem<UDialogueSubsystem>(GetWorld());
 if (!DialogueSys)
    {
        return;
    }

    TArray<UObject*> Participants;
    Participants.Add(InteractingActor); // Player
    Participants.Add(GetOwner()); // NPC (Lianne)

    DialogueSys->StartDialogue(DialogueAsset, Participants, 
  [this, DialogueSys](bool bSuccess, FName DialogueId)
        {
            if (bSuccess)
            {
   // Add Alice as additional participant IMMEDIATELY
     UDialogueRunner* Runner = DialogueSys->GetActiveRunner();
       if (Runner && Runner->GetCurrentContext())
   {
        UDialogueParticipants* Parts = Runner->GetCurrentContext()->GetParticipants();
        if (Parts)
  {
      // Find Alice by tag
  AActor* Alice = FindActorWithTag(FName("NPC.Alice"));
      if (Alice)
            {
           Parts->AddAdditionalParticipant(Alice);
             UE_LOG(LogTemp, Log, TEXT("Added Alice to dialogue participants"));
     }
        }
  }
       }
        });
}
```

---

## See Also

- **`Sequence_Participants_QuickStart.md`** - Adding participants to Sequencer effects
- **`Sequence_Schedule_Integration_Complete.md`** - Full schedule integration guide
- **`ScheduleComponent.h`** - Schedule pause/resume API

---

**Last Updated:** 2025.01.05  
**Version:** DialogueSystem v1.13.4
