# Quest System - API Reference

Complete API documentation for the Quest System plugin.

---

## Table of Contents

- [Core Classes](#core-classes)
- [Subsystems](#subsystems)
- [Components](#components)
- [Blueprint Library](#blueprint-library)
- [Data Assets](#data-assets)
- [Enumerations](#enumerations)
- [Structures](#structures)
- [Delegates](#delegates)

---

## Core Classes

### UQuestSubsystem

**Type**: `UGameInstanceSubsystem`  
**Purpose**: Central quest management system

#### Public Methods

```cpp
bool StartQuest(FName QuestId);
```
Starts a quest by ID. Returns false if quest cannot be started (dependencies not met, already active, etc).

```cpp
bool CompleteQuest(FName QuestId);
```
Marks quest as completed. Grants rewards and broadcasts completion event.

```cpp
bool FailQuest(FName QuestId);
```
Marks quest as failed. Respects failure policy (reset progress, allow retry, etc).

```cpp
bool AbandonQuest(FName QuestId);
```
Player abandons the quest. Quest can potentially be restarted.

```cpp
bool RetryQuest(FName QuestId);
```
Retries a failed quest if allowed by failure policy.

```cpp
EQuestState GetQuestState(FName QuestId) const;
```
Returns current state of quest (NotStarted, Active, Completed, Failed, Abandoned).

```cpp
bool IsQuestActive(FName QuestId) const;
```
Returns true if quest is currently active.

```cpp
bool IsQuestCompleted(FName QuestId) const;
```
Returns true if quest was completed.

```cpp
TArray<FQuestSaveData> GetActiveQuests() const;
```
Returns all currently active quests.

```cpp
TArray<FQuestSaveData> GetCompletedQuests() const;
```
Returns all completed quests.

```cpp
UQuestAsset* GetQuestAsset(FName QuestId) const;
```
Loads and returns quest asset by ID.

```cpp
FQuestSaveData GetQuestSaveData(FName QuestId) const;
```
Returns save data for specific quest.

```cpp
bool UpdateObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount = 1);
```
Increments objective progress. Checks gates and conditions.

```cpp
bool SetObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Progress);
```
Sets objective progress to specific value.

```cpp
bool CompleteObjective(FName QuestId, FName ObjectiveId);
```
Instantly completes objective.

```cpp
void GetObjectiveProgress(FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted) const;
```
Retrieves objective progress data.

```cpp
bool AdvanceToNextPhase(FName QuestId);
```
Advances quest to next phase based on transition rules.

```cpp
bool SetQuestPhase(FName QuestId, FName PhaseId);
```
Manually sets quest to specific phase.

```cpp
FName GetCurrentPhaseId(FName QuestId) const;
```
Returns ID of current active phase.

```cpp
bool CanStartQuest(FName QuestId, TArray<FText>& OutFailReasons) const;
```
Checks if quest can be started. Outputs failure reasons if not.

```cpp
TArray<UQuestAsset*> GetAvailableQuests() const;
```
Returns all quests whose dependencies are met.

```cpp
bool SaveQuestProgress();
```
Saves all quest progress to save game.

```cpp
bool LoadQuestProgress();
```
Loads quest progress from save game.

```cpp
void ResetAllProgress();
```
Resets all quest progress (debug/testing).

```cpp
UQuestEventBus* GetEventBus() const;
```
Returns event bus for emitting events.

```cpp
void EmitQuestEvent(const FQuestEventPayload& Payload);
```
Emits a quest event (delegates to event bus).

#### Delegates

```cpp
FOnQuestStateChanged OnQuestStateChanged;
```
Broadcast when quest state changes.  
**Parameters**: `FName QuestId, EQuestState OldState, EQuestState NewState`

```cpp
FOnQuestStarted OnQuestStarted;
```
Broadcast when quest starts.  
**Parameters**: `FName QuestId, UQuestAsset* QuestAsset`

```cpp
FOnQuestCompleted OnQuestCompleted;
```
Broadcast when quest completes.  
**Parameters**: `FName QuestId, UQuestAsset* QuestAsset`

```cpp
FOnQuestFailed OnQuestFailed;
```
Broadcast when quest fails.  
**Parameters**: `FName QuestId, UQuestAsset* QuestAsset`

```cpp
FOnQuestAbandoned OnQuestAbandoned;
```
Broadcast when quest is abandoned.  
**Parameters**: `FName QuestId, UQuestAsset* QuestAsset`

```cpp
FOnObjectiveUpdated OnObjectiveUpdated;
```
Broadcast when objective progress updates.
**Parameters**: `FName QuestId, FName ObjectiveId, int32 CurrentCount, int32 TargetCount`

```cpp
FOnObjectiveCompleted OnObjectiveCompleted;
```
Broadcast when objective completes.  
**Parameters**: `FName QuestId, FName ObjectiveId`

```cpp
FOnQuestPhaseChanged OnQuestPhaseChanged;
```
Broadcast when quest phase changes.  
**Parameters**: `FName QuestId, FName OldPhaseId, FName NewPhaseId`

---

### UQuestEventBus

**Type**: `UObject`  
**Purpose**: Event dispatcher for quest-related game events

#### Public Methods

```cpp
void EmitEvent(const FQuestEventPayload& Payload);
```
Emits an event to all subscribers.

```cpp
void Subscribe(FGameplayTag EventTag, UObject* Owner, FName FunctionName);
```
Subscribes to events (Blueprint version).

```cpp
void Unsubscribe(FGameplayTag EventTag, UObject* Owner);
```
Unsubscribes from specific event.

```cpp
void UnsubscribeAll(UObject* Owner);
```
Unsubscribes from all events for owner.

```cpp
void SubscribeWithFilter(FGameplayTag EventTag, UObject* Owner, FDelegate Callback, const FGameplayTagContainer& FilterTags, FName FilterStringParam);
```
Subscribes with optional filters (C++ version).

```cpp
bool HasSubscribers(FGameplayTag EventTag) const;
```
Returns true if any subscribers exist for event.

```cpp
int32 GetSubscriptionCount() const;
```
Returns total number of active subscriptions.

```cpp
void ClearAllSubscriptions();
```
Removes all subscriptions (cleanup).

```cpp
void SetEnabled(bool bEnabled);
```
Enables/disables event bus.

```cpp
bool IsEnabled() const;
```
Returns true if event bus is enabled.

#### Event Emission Helpers

```cpp
void EmitItemAcquired(FName ItemId, int32 Count, AActor* Instigator, const FGameplayTagContainer& Tags);
void EmitItemUsed(FName ItemId, AActor* Instigator, AActor* Target, const FGameplayTagContainer& Tags);
void EmitNpcTalked(FName NpcId, AActor* Player, const FGameplayTagContainer& Tags);
void EmitDialogChoice(FName DialogId, FName ChoiceId, const FGameplayTagContainer& Tags);
void EmitEnterArea(FName AreaId, AActor* Actor, const FGameplayTagContainer& Tags);
void EmitActorKilled(AActor* Victim, AActor* Killer, const FGameplayTagContainer& Tags);
void EmitTimeReached(int32 Hour, int32 Minute);
void EmitDayChanged(int32 DayNumber);
void EmitWeatherChanged(const FGameplayTagContainer& WeatherTags);
void EmitNeedChanged(FGameplayTag NeedTag, float NewValue, float OldValue);
void EmitAffectionChanged(FName NpcId, float NewValue, float OldValue);
```

---

### UQuestLogComponent

**Type**: `UActorComponent`  
**Purpose**: Player-side quest tracking

#### Public Methods

```cpp
bool StartQuest(FName QuestId);
bool AbandonQuest(FName QuestId);
bool IsQuestActive(FName QuestId) const;
bool IsQuestCompleted(FName QuestId) const;
EQuestState GetQuestState(FName QuestId) const;
TArray<FQuestSaveData> GetActiveQuests() const;
TArray<FQuestSaveData> GetCompletedQuests() const;
UQuestAsset* GetQuestAsset(FName QuestId) const;
void GetObjectiveProgress(FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted) const;
```

All methods delegate to `UQuestSubsystem`.

```cpp
void SetTrackedQuest(FName QuestId);
```
Sets quest to track on HUD.

```cpp
FName GetTrackedQuest() const;
```
Returns currently tracked quest.

```cpp
void ClearTrackedQuest();
```
Clears tracked quest.

```cpp
FText GetCurrentObjectiveText(FName QuestId) const;
```
Returns formatted text for current incomplete objective.

```cpp
TArray<FText> GetActiveObjectiveTexts(FName QuestId) const;
```
Returns all active objective texts for quest.

```cpp
float GetQuestCompletionPercent(FName QuestId) const;
```
Returns 0.0-1.0 completion percentage.

```cpp
void SubscribeToQuestEvents();
void UnsubscribeFromQuestEvents();
```
Manages event subscriptions.

---

### UQuestBlueprintLibrary

**Type**: `UBlueprintFunctionLibrary`  
**Purpose**: Blueprint-friendly quest functions

All functions are `BlueprintCallable` and require `WorldContextObject`.

#### Quest Management

```cpp
static bool StartQuest(UObject* WorldContextObject, FName QuestId);
static bool CompleteQuest(UObject* WorldContextObject, FName QuestId);
static bool FailQuest(UObject* WorldContextObject, FName QuestId);
static bool AbandonQuest(UObject* WorldContextObject, FName QuestId);
```

#### Quest Queries

```cpp
static EQuestState GetQuestState(UObject* WorldContextObject, FName QuestId);
static bool IsQuestActive(UObject* WorldContextObject, FName QuestId);
static bool IsQuestCompleted(UObject* WorldContextObject, FName QuestId);
static TArray<FQuestSaveData> GetActiveQuests(UObject* WorldContextObject);
static UQuestAsset* GetQuestAsset(UObject* WorldContextObject, FName QuestId);
```

#### Objective Management

```cpp
static bool UpdateObjectiveProgress(UObject* WorldContextObject, FName QuestId, FName ObjectiveId, int32 Amount = 1);
static bool CompleteObjective(UObject* WorldContextObject, FName QuestId, FName ObjectiveId);
static void GetObjectiveProgress(UObject* WorldContextObject, FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted);
```

#### Event Emission

```cpp
static void EmitItemAcquired(UObject* WorldContextObject, FName ItemId, int32 Count, AActor* Instigator, const FGameplayTagContainer& Tags);
static void EmitItemUsed(UObject* WorldContextObject, FName ItemId, AActor* Instigator, AActor* Target, const FGameplayTagContainer& Tags);
static void EmitNpcTalked(UObject* WorldContextObject, FName NpcId, AActor* Player, const FGameplayTagContainer& Tags);
static void EmitDialogChoice(UObject* WorldContextObject, FName DialogId, FName ChoiceId, const FGameplayTagContainer& Tags);
static void EmitEnterArea(UObject* WorldContextObject, FName AreaId, AActor* Actor, const FGameplayTagContainer& Tags);
static void EmitActorKilled(UObject* WorldContextObject, AActor* Victim, AActor* Killer, const FGameplayTagContainer& Tags);
static void EmitCustomEvent(UObject* WorldContextObject, FGameplayTag EventTag, FName StringParam, int32 IntParam, float FloatParam);
```

#### Utility

```cpp
static FText QuestStateToText(EQuestState State);
static FText ObjectiveStyleToText(EObjectiveStyle Style);
static FLinearColor QuestStateToColor(EQuestState State);
```

---

## Data Assets

### UQuestAsset

**Type**: `UPrimaryDataAsset`  
**Purpose**: Complete quest definition

#### Properties

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
FName QuestId;
```
Unique quest identifier.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
FText Title;
FText Description;
FText Summary;
```
Localized quest text.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
FQuestMeta Meta;
```
Quest metadata (categories, difficulty, etc).

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
TArray<FQuestPhase> Phases;
```
Quest phases/stages.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
TArray<FQuestDependency> Dependencies;
```
Prerequisites to start quest.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
FQuestStartPolicyConfig StartPolicy;
```
How quest is started.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
FQuestFailurePolicy FailurePolicy;
```
Failure conditions and handling.

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
FQuestRewardSet RewardsOnComplete;
```
Rewards granted on completion.

#### Methods

```cpp
FQuestPhase GetPhase(FName PhaseId) const;
bool HasPhase(FName PhaseId) const;
FQuestPhase GetFirstPhase() const;
TArray<FName> GetAllObjectiveIds() const;
bool ValidateQuestData(TArray<FText>& OutErrors) const;
```

---

### UQuestSaveGame

**Type**: `USaveGame`  
**Purpose**: Persistent quest progress

#### Properties

```cpp
UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
TArray<FQuestSaveData> QuestProgress;
```
All quest progress data.

```cpp
UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
TSet<FName> GlobalQuestTags;
```
Global tags unlocked by quests.

```cpp
UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save")
TMap<FName, FString> GlobalVariables;
```
Global quest variables.

#### Methods

```cpp
FQuestSaveData GetQuestData(FName QuestId) const;
void SetQuestData(const FQuestSaveData& QuestData);
bool HasQuestData(FName QuestId) const;
void RemoveQuestData(FName QuestId);
TArray<FQuestSaveData> GetQuestsByState(EQuestState State) const;
```

---

## Enumerations

### EQuestState

```cpp
UENUM(BlueprintType)
enum class EQuestState : uint8
{
    NotStarted,
    Active,
    Completed,
  Failed,
    Abandoned
};
```

### EObjectiveStyle

```cpp
UENUM(BlueprintType)
enum class EObjectiveStyle : uint8
{
    Collect,
    Kill,
    Talk,
    Visit,
    Use,
    Deliver,
    Craft,
    Wait,
    Escort,
    Discover,
    Custom
};
```

### EObjectiveLogic

```cpp
UENUM(BlueprintType)
enum class EObjectiveLogic : uint8
{
  And,     // All conditions must be met
    Or,      // Any condition can be met
    Count,   // Reach target count
    Sequence // Conditions in order
};
```

### EQuestStartPolicy

```cpp
UENUM(BlueprintType)
enum class EQuestStartPolicy : uint8
{
    Manual,
    AutoOnCondition,
    OnDialog,
    OnItemPickup,
    OnOverlapTrigger,
    OnScheduleEvent
};
```

### EQuestFailureType

```cpp
UENUM(BlueprintType)
enum class EQuestFailureType : uint8
{
    None,
 TimeLimit,
    OnCondition,
    OnEvent
};
```

### EQuestRewardType

```cpp
UENUM(BlueprintType)
enum class EQuestRewardType : uint8
{
    GiveItem,
    GrantCurrency,
    ModifyStats,
    ModifyNeeds,
    ModifyAffection,
 UnlockDialogBranch,
    UnlockRecipe,
    GrantTag,
    RunBPEvent,
    ExecuteEffect
};
```

### EConditionEventType

```cpp
UENUM(BlueprintType)
enum class EConditionEventType : uint8
{
    OnItemAcquired,
OnItemUsed,
    OnItemGifted,
    OnNpcTalked,
 OnDialogChoice,
    OnDialogNodeReached,
    OnEnterArea,
    OnLeaveArea,
    OnActorKilled,
    OnTimeReached,
    OnDayChanged,
    OnWeatherState,
    OnNeedBelow,
    OnNeedAbove,
    OnAffectionBelow,
    OnAffectionAbove,
    OnItemCrafted,
    OnItemDelivered,
    OnCustomTag,
    OnInteract,
    Custom
};
```

---

## Structures

### FQuestEventPayload

```cpp
USTRUCT(BlueprintType)
struct FQuestEventPayload
{
 UPROPERTY(BlueprintReadWrite, Category = "Quest")
    FGameplayTag EventTag;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    FName StringParam;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    int32 IntParam;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    float FloatParam;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    FGameplayTagContainer TagsParam;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    TWeakObjectPtr<AActor> InstigatorActor;

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    TWeakObjectPtr<AActor> TargetActor;
};
```

### FQuestMeta

```cpp
USTRUCT(BlueprintType)
struct FQuestMeta
{
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FGameplayTagContainer Categories;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    FGameplayTagContainer QuestTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 DifficultyLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bIsRepeatable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
  bool bIsHidden;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    bool bIsMainQuest;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 Priority;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    int32 EstimatedDuration;
};
```

---

For complete structure definitions, see source files:
- `QuestTypes.h` - Enums and basic types
- `QuestStructures.h` - Complex structures
- `QuestObjective.h` - Objective and Phase structures

---

## Usage Examples

### C++ Example

```cpp
#include "QuestSubsystem.h"
#include "QuestBlueprintLibrary.h"

void AMyGameMode::StartTutorialQuest()
{
    UQuestSubsystem* QuestSystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
    
  if (QuestSystem->StartQuest(FName("Q_Tutorial_First")))
    {
        UE_LOG(LogTemp, Log, TEXT("Tutorial quest started"));
    }
}

void AMyInventory::OnItemPickedUp(FName ItemId)
{
    UQuestBlueprintLibrary::EmitItemAcquired(
  this,
        ItemId,
        1,
        GetOwner(),
        FGameplayTagContainer()
    );
}
```

### Blueprint Example

See [Quick Start Guide](QuickStart.md) for visual Blueprint examples.

---

## Version History

**v1.0.0** (Current)
- Initial release
- Full quest system with phases and objectives
- Event-driven architecture
- Save/load system
- Editor validation

---

## Support

For questions and issues:
- Documentation: `/Plugins/QuestSystem/Docs/`
- Examples: `/Plugins/QuestSystem/Docs/Examples.md`
- GitHub Issues: [Link]
