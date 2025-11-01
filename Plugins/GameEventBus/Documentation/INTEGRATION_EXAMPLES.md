# GameEventBus Integration Examples

**Complete copy-paste examples for integrating GameEventBus into your systems**

---

## Table of Contents

1. [Inventory System Integration](#inventory-system-integration)
2. [Dialogue System Integration](#dialogue-system-integration)
3. [Time System Integration](#time-system-integration)
4. [Quest System (Already Done!)](#quest-system-already-done)
5. [Custom System Integration](#custom-system-integration)

---

## Inventory System Integration

### Step 1: Add Include at Top of `InventoryComponent.cpp`

```cpp
#include "InventoryComponent.h"
#include "InventoryItem.h"
#include "ItemDataAsset.h"
// ... other includes ...

// ?? ADD THIS:
#if __has_include("Integration/InventoryGameEventBridge.h")
    #include "Integration/InventoryGameEventBridge.h"
    #define GAMEEVENTBUS_AVAILABLE 1
#else
    #define GAMEEVENTBUS_AVAILABLE 0
#endif
```

### Step 2: Emit Event When Item is Added

Find your `AddItem` method and add event emission at the end:

```cpp
UInventoryItem* UInventoryComponent::AddItem(UItemDataAsset* ItemData, int32 Quantity, EInventoryOperationResult& OutResult)
{
    // ... your existing code ...
  
  if (FirstCreatedItem)
    {
    OutResult = EInventoryOperationResult::Success;
        NotifyInventoryChanged();
        
     // ?? ADD THIS:
  #if GAMEEVENTBUS_AVAILABLE
      // Get ItemId as FName (adjust based on your ItemData structure)
     FName ItemId = FName(*ItemData->ItemName.ToString()); // or ItemData->ItemId if you have it
            UInventoryGameEventBridge::EmitItemAcquiredEvent(this, ItemId, Quantity, GetOwner());
        #endif
    }

    return FirstCreatedItem;
}
```

### Step 3: Emit Event When Item is Used

Find your `UseItem` method:

```cpp
bool UInventoryComponent::UseItem(FInventoryItemId ItemId, AActor* User, EInventoryOperationResult& OutResult)
{
    // ... your existing code ...

    if (Item->ItemData->bConsumeOnUse)
    {
 EInventoryOperationResult RemoveResult;
        RemoveItemById(ItemId, 1, RemoveResult);
    }

    OutResult = EInventoryOperationResult::Success;
    OnItemUsed.Broadcast(Item, User, true);
    
    // ?? ADD THIS:
    #if GAMEEVENTBUS_AVAILABLE
        FName ItemIdName = FName(*Item->ItemData->ItemName.ToString());
UInventoryGameEventBridge::EmitItemUsedEvent(this, ItemIdName, 1, User);
    #endif
    
    return true;
}
```

### Step 4: Emit Event When Item is Gifted

Find your `GiftItem` method:

```cpp
bool UInventoryComponent::GiftItem(FInventoryItemId ItemId, AActor* Receiver, int32 Quantity, EInventoryOperationResult& OutResult)
{
    // ... your existing code ...

    if (AddResult == EInventoryOperationResult::Success && GiftedItem)
    {
        EInventoryOperationResult RemoveResult;
 RemoveItemById(ItemId, QuantityToGift, RemoveResult);

   OutResult = EInventoryOperationResult::Success;
        OnItemGifted.Broadcast(Item, GetOwner(), Receiver, true);
        
        // ?? ADD THIS:
        #if GAMEEVENTBUS_AVAILABLE
            FName ItemIdName = FName(*Item->ItemData->ItemName.ToString());
   UInventoryGameEventBridge::EmitItemGiftedEvent(this, ItemIdName, QuantityToGift, GetOwner(), Receiver);
        #endif
      
        return true;
    }
    
    // ... rest of code ...
}
```

**That's it!** Inventory events now work automatically with Quest System!

---

## Dialogue System Integration

### Already Implemented! ?

Dialogue System already has `UDialogueEffect_EmitGameEvent` which you can use in Data Assets.

**Example in Dialogue Node:**

```cpp
// In your Dialogue Data Asset:
Node: "Help Me!"
  Effects:
    - Dialogue Effect: Emit Game Event
      Event Tag: Quest.Event.Started
      String Param: "HelpFarmerQuest"
```

When player reaches this node ? Quest auto-starts!

---

## Time System Integration

### If You Have Time/Day System

```cpp
// TimeOfDaySubsystem.cpp (or similar)

#if __has_include("Integration/TimeSystemGameEventEmitter.h")
 #include "Integration/TimeSystemGameEventEmitter.h"
    #define GAMEEVENTBUS_AVAILABLE 1
#else
    #define GAMEEVENTBUS_AVAILABLE 0
#endif

void UTimeOfDaySubsystem::OnHourChanged(int32 NewHour)
{
    // Your existing logic...
    
    // ?? ADD THIS:
    #if GAMEEVENTBUS_AVAILABLE
        UTimeSystemGameEventEmitter* Emitter = GetTimeEmitter(); // Create if needed
        if (Emitter)
        {
            Emitter->OnHourChanged(NewHour);
  }
    #endif
}

void UTimeOfDaySubsystem::OnDayChanged(int32 NewDay)
{
    // Your existing logic...
    
    // ?? ADD THIS:
 #if GAMEEVENTBUS_AVAILABLE
        UTimeSystemGameEventEmitter* Emitter = GetTimeEmitter();
   if (Emitter)
   {
            Emitter->OnDayChanged(NewDay);
 }
    #endif
}
```

---

## Quest System (Already Done!)

? **Quest System is fully integrated!**

- Automatically subscribes to `Inventory.Event.ItemAcquired`
- Automatically subscribes to `Dialogue.Event.*`
- Quest objectives update automatically when events are received

**No additional work needed!**

---

## Custom System Integration

### Example: Crafting System

```cpp
// CraftingComponent.cpp

#if __has_include("GameEventBusSubsystem.h")
    #include "GameEventBusSubsystem.h"
    #define GAMEEVENTBUS_AVAILABLE 1
#else
    #define GAMEEVENTBUS_AVAILABLE 0
#endif

void UCraftingComponent::OnItemCrafted(UItemDataAsset* CraftedItem, int32 Quantity)
{
    // Your crafting logic...
    
    // ?? EMIT EVENT:
    #if GAMEEVENTBUS_AVAILABLE
        UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(this);
        if (EventBus)
        {
  FGameEventPayload Payload;
 Payload.EventTag = FGameplayTag::RequestGameplayTag("Crafting.Event.ItemCrafted");
  Payload.StringParam = FName(*CraftedItem->ItemName.ToString());
     Payload.IntParam = Quantity;
     Payload.InstigatorActor = GetOwner();
            
            EventBus->EmitEvent(Payload, true); // true = log to console
        }
    #endif
}
```

Now create a Quest that listens to `"Crafting.Event.ItemCrafted"`!

---

## Blueprint Integration

### Emit Event from Blueprint

**Example: Custom Quest Event**

1. Get `Game Event Bus Subsystem`
2. Call `Emit Simple Event` node
3. Set Event Tag: `"MyCustomEvent"`
4. Set parameters as needed

**Blueprint Nodes Available:**

- `Emit Simple Event` - Just tag and basic params
- `Emit Item Event` - For inventory-related events
- `Emit Dialogue Event` - For dialogue-related events
- `Emit Custom Event` - Full control over all parameters

---

## Testing Your Integration

### Console Commands

```
// Enable event history
GameEventBus.EnableHistory 1

// View recent events
GameEventBus.PrintHistory

// View statistics
GameEventBus.PrintStats
```

### Check Logs

Look for:
```
LogGameEventBus: Event [Inventory.Event.ItemAcquired] | String: Apple | Int: 5
LogQuest: Quest objective updated: CollectApples (5/10)
```

---

## Common Patterns

### Pattern 1: Item Interaction

```cpp
#if GAMEEVENTBUS_AVAILABLE
    FGameEventPayload Payload;
  Payload.EventTag = FGameplayTag::RequestGameplayTag("Inventory.Event.ItemAcquired");
    Payload.StringParam = ItemId;
    Payload.IntParam = Quantity;
    Payload.InstigatorActor = Player;
    EventBus->EmitEvent(Payload);
#endif
```

### Pattern 2: Location-Based Events

```cpp
#if GAMEEVENTBUS_AVAILABLE
    FGameEventPayload Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag("World.Event.LocationReached");
    Payload.StringParam = LocationId;
    Payload.TargetActor = TriggerVolume;
    Payload.AdditionalTags.AddTag(FGameplayTag::RequestGameplayTag("Location.Farm"));
    EventBus->EmitEvent(Payload);
#endif
```

### Pattern 3: NPC Interaction

```cpp
#if GAMEEVENTBUS_AVAILABLE
    FGameEventPayload Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag("NPC.Event.TalkStarted");
    Payload.StringParam = NPCId;
    Payload.InstigatorActor = Player;
    Payload.TargetActor = NPC;
    EventBus->EmitEvent(Payload);
#endif
```

---

## FAQ

**Q: Will my code break if GameEventBus is removed?**  
A: No! The `#if GAMEEVENTBUS_AVAILABLE` check ensures code only compiles when the plugin exists.

**Q: Do I need to subscribe manually?**  
A: No! Quest System already subscribes to all common events.

**Q: Can I emit custom events?**  
A: Yes! Just use `UGameEventBusSubsystem->EmitEvent()` with your own GameplayTag.

**Q: How do I create new GameplayTags?**  
A: See `Documentation/GameplayTags.md` for the full list and how to add custom tags.

---

## Next Steps

1. ? Add includes to your systems (copy-paste from above)
2. ? Add emission calls (copy-paste from above)
3. ? Create Quest Data Assets that listen to events
4. ? Test in-game!

**You're done!** ??

Event emissions happen automatically, Quest System reacts automatically, no BP setup needed!
