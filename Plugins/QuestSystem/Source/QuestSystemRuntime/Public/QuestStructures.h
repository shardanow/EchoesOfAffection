// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "QuestTypes.h"
#include "QuestStructures.generated.h"

/**
 * Objective Condition - Describes what event/state triggers objective progress
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FObjectiveCondition
{
	GENERATED_BODY()

	/** Type of event this condition listens for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	EConditionEventType EventType = EConditionEventType::Custom;

	/** Event tag to listen for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (Categories = "GameEvent"))
	FGameplayTag EventTag;

	/** Item ID (for item-related events) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName ItemId;

	/** NPC ID (for NPC-related events) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName NpcId;

	/** Area/Location ID (for location-related events) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName AreaId;

	/** Dialog node ID */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName DialogNodeId;

	/** Actor class filter (for kill/interact events) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	TSoftClassPtr<AActor> ActorClass;

	/** Required tags (target must have these) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTagContainer RequiredTags;

	/** Count/amount required */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = 1))
	int32 Count = 1;

	/** Threshold value (for needs/affection checks) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	float ThresholdValue = 0.0f;

	/** Time range start (for time-based events) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	int32 TimeRangeStart = 0;

	/** Time range end (for time-based events) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	int32 TimeRangeEnd = 24;

	/** Weather tags (for weather-based events) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (Categories = "Weather"))
	FGameplayTagContainer WeatherTags;

	/** Invert condition (NOT logic) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bInvert = false;

	FObjectiveCondition() = default;
};

/**
 * Objective Progress Gate - Restricts when objective can progress
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FObjectiveProgressGate
{
	GENERATED_BODY()

	/** Time of day requirement (0-24) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	bool bRequireTimeOfDay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate", meta = (EditCondition = "bRequireTimeOfDay"))
	int32 RequiredTimeStart = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate", meta = (EditCondition = "bRequireTimeOfDay"))
	int32 RequiredTimeEnd = 24;

	/** Weather requirement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	bool bRequireWeather = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate", meta = (EditCondition = "bRequireWeather", Categories = "Weather"))
	FGameplayTagContainer RequiredWeather;

	/** Location requirement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	bool bRequireLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate", meta = (EditCondition = "bRequireLocation"))
	FName RequiredLocation;

	/** Affection requirement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	bool bRequireAffection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate", meta = (EditCondition = "bRequireAffection"))
	FName RequiredNpc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate", meta = (EditCondition = "bRequireAffection"))
	float MinAffection = 0.0f;

	/** Tag requirement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
	FGameplayTagContainer RequiredTags;

	FObjectiveProgressGate() = default;
};

/**
 * Objective Counters - Progress tracking
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FObjectiveCounters
{
	GENERATED_BODY()

	/** Target count to complete objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Counter", meta = (ClampMin = 1))
	int32 TargetCount = 1;

	/** Current progress count */
	UPROPERTY(BlueprintReadOnly, Category = "Counter")
	int32 CurrentCount = 0;

	/** Allow counting beyond target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Counter")
	bool bAllowOvercount = false;

	/** Reset counter on objective fail */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Counter")
	bool bResetOnFail = true;

	FObjectiveCounters() = default;
};

/**
 * Quest Reward - Single reward item
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestReward
{
	GENERATED_BODY()

	/** Type of reward */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	EQuestRewardType RewardType = EQuestRewardType::GiveItem;

	/** Item ID (for GiveItem reward) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FName ItemId;

	/** Item count/amount */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward", meta = (ClampMin = 1))
	int32 Amount = 1;

	/** Currency amount (for GrantCurrency) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	int32 CurrencyAmount = 0;

	/** Stat/Need tag to modify */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward", meta = (Categories = "Stat,Need"))
	FGameplayTag StatTag;

	/** Delta value for stat/need */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	float StatDelta = 0.0f;

	/** NPC ID (for affection/dialog rewards) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FName NpcId;

	/** Affection delta */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	float AffectionDelta = 0.0f;

	/** Dialog branch to unlock */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FName DialogBranchId;

	/** Recipe ID to unlock */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FName RecipeId;

	/** Tag to grant */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FGameplayTag GrantedTag;

	/** Blueprint event name to call */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FName BPEventName;

	/** Custom effect class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	TSoftClassPtr<UObject> EffectClass;

	FQuestReward() = default;
};

/**
 * Quest Reward Set - Collection of rewards
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestRewardSet
{
	GENERATED_BODY()

	/** List of rewards */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
	TArray<FQuestReward> Rewards;

	/** Optional gameplay tags to grant */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
	FGameplayTagContainer GrantedTags;

	FQuestRewardSet() = default;
};

/**
 * Quest Dependency - Required conditions to start quest
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestDependency
{
	GENERATED_BODY()

	/** Required quest ID and its state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency")
	FName RequiredQuestId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency")
	EQuestState RequiredQuestState = EQuestState::Completed;

	/** Required player level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency")
	int32 RequiredLevel = 0;

	/** Required gameplay tags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency")
	FGameplayTagContainer RequiredTags;

	/** Forbidden gameplay tags (player must NOT have these) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency")
	FGameplayTagContainer ForbiddenTags;

	/** Time range requirement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency")
	bool bRequireTimeRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency", meta = (EditCondition = "bRequireTimeRange"))
	int32 TimeStart = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency", meta = (EditCondition = "bRequireTimeRange"))
	int32 TimeEnd = 24;

	/** Weather requirement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency", meta = (Categories = "Weather"))
	FGameplayTagContainer RequiredWeather;

	/** Location requirement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency")
	FName RequiredLocation;

	/** Minimum affection with NPC */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency")
	bool bRequireAffection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency", meta = (EditCondition = "bRequireAffection"))
	FName AffectionNpcId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dependency", meta = (EditCondition = "bRequireAffection"))
	float MinAffectionValue = 0.0f;

	FQuestDependency() = default;
};

/**
 * Quest Start Policy Configuration
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestStartPolicyConfig
{
	GENERATED_BODY()

	/** Start policy type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Policy")
	EQuestStartPolicy PolicyType = EQuestStartPolicy::Manual;

	/** Condition for auto-start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Policy", meta = (EditCondition = "PolicyType == EQuestStartPolicy::AutoOnCondition"))
	FObjectiveCondition AutoStartCondition;

	/** Dialog node that triggers start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Policy", meta = (EditCondition = "PolicyType == EQuestStartPolicy::OnDialog"))
	FName DialogNodeId;

	/** Item that triggers start when picked up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Policy", meta = (EditCondition = "PolicyType == EQuestStartPolicy::OnItemPickup"))
	FName TriggerItemId;

	/** Trigger volume tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Policy", meta = (EditCondition = "PolicyType == EQuestStartPolicy::OnOverlapTrigger", Categories = "Location"))
	FGameplayTag TriggerVolumeTag;

	/** Schedule event tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Policy", meta = (EditCondition = "PolicyType == EQuestStartPolicy::OnScheduleEvent", Categories = "Schedule"))
	FGameplayTag ScheduleEventTag;

	FQuestStartPolicyConfig() = default;
};

/**
 * Quest Failure Policy Configuration
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestFailurePolicy
{
	GENERATED_BODY()

	/** Failure type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failure")
	EQuestFailureType FailureType = EQuestFailureType::None;

	/** Time limit in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failure", meta = (EditCondition = "FailureType == EQuestFailureType::TimeLimit"))
	float TimeLimit = 0.0f;

	/** Time of day after which quest fails (0-24) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failure", meta = (EditCondition = "FailureType == EQuestFailureType::TimeLimit"))
	int32 FailAfterTimeOfDay = 24;

	/** Condition that triggers failure */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failure", meta = (EditCondition = "FailureType == EQuestFailureType::OnCondition"))
	FObjectiveCondition FailureCondition;

	/** Event tag that triggers failure */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failure", meta = (EditCondition = "FailureType == EQuestFailureType::OnEvent", Categories = "Quest.Event"))
	FGameplayTag FailureEventTag;

	/** Reset progress on failure */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failure")
	bool bResetProgressOnFail = true;

	/** Allow retry after failure */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Failure")
	bool bAllowRetry = true;

	FQuestFailurePolicy() = default;
};

/**
 * Quest Phase Transition - Controls phase flow
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestPhaseTransition
{
	GENERATED_BODY()

	/** Next phase ID on completion (empty = END) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	FName NextPhaseId;

	/** Conditional branches (for branching quests) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	TMap<FName, FName> ConditionalBranches; // ConditionTag -> PhaseId

	/** Auto-advance to next phase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	bool bAutoAdvance = true;

	/** Delay before auto-advance (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition", meta = (EditCondition = "bAutoAdvance"))
	float AutoAdvanceDelay = 0.0f;

	FQuestPhaseTransition() = default;
};

/**
 * Objective Signals - Events fired during objective lifecycle
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FObjectiveSignals
{
	GENERATED_BODY()

	/** Rewards on objective start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signals")
	FQuestRewardSet OnStartRewards;

	/** Rewards on objective update */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signals")
	FQuestRewardSet OnUpdateRewards;

	/** Rewards on objective completion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signals")
	FQuestRewardSet OnCompleteRewards;

	/** Gameplay events to emit on start */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signals", meta = (Categories = "Quest.Event"))
	FGameplayTagContainer OnStartEvents;

	/** Gameplay events to emit on update */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signals", meta = (Categories = "Quest.Event"))
	FGameplayTagContainer OnUpdateEvents;

	/** Gameplay events to emit on complete */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Signals", meta = (Categories = "Quest.Event"))
	FGameplayTagContainer OnCompleteEvents;

	FObjectiveSignals() = default;
};
