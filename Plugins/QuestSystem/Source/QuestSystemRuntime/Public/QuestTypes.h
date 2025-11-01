// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "QuestTypes.generated.h"

class UQuestAsset;

/**
 * Quest State - Current status of a quest
 */
UENUM(BlueprintType)
enum class EQuestState : uint8
{
	/** Quest is not yet started */
	NotStarted UMETA(DisplayName = "Not Started"),
	
	/** Quest is currently active */
	Active UMETA(DisplayName = "Active"),
	
	/** Quest was completed successfully */
	Completed UMETA(DisplayName = "Completed"),
	
	/** Quest has failed */
	Failed UMETA(DisplayName = "Failed"),
	
	/** Quest was abandoned by player */
	Abandoned UMETA(DisplayName = "Abandoned")
};

/**
 * Objective Style - Type of objective
 */
UENUM(BlueprintType)
enum class EObjectiveStyle : uint8
{
	/** Collect items */
	Collect UMETA(DisplayName = "Collect"),
	
	/** Kill enemies */
	Kill UMETA(DisplayName = "Kill"),
	
	/** Talk to NPCs */
	Talk UMETA(DisplayName = "Talk"),
	
	/** Visit locations */
	Visit UMETA(DisplayName = "Visit"),
	
	/** Use items or interact with objects */
	Use UMETA(DisplayName = "Use"),
	
	/** Deliver items to NPCs */
	Deliver UMETA(DisplayName = "Deliver"),
	
	/** Craft items */
	Craft UMETA(DisplayName = "Craft"),
	
	/** Wait for time or event */
	Wait UMETA(DisplayName = "Wait"),
	
	/** Escort NPC */
	Escort UMETA(DisplayName = "Escort"),
	
	/** Discover location or secret */
	Discover UMETA(DisplayName = "Discover"),
	
	/** Custom objective */
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Objective Logic - How objectives combine
 */
UENUM(BlueprintType)
enum class EObjectiveLogic : uint8
{
	/** All conditions must be met */
	And UMETA(DisplayName = "AND"),
	
	/** Any condition can be met */
	Or UMETA(DisplayName = "OR"),
	
	/** Reach target count */
	Count UMETA(DisplayName = "Count"),
	
	/** Conditions must be met in sequence */
	Sequence UMETA(DisplayName = "Sequence")
};

/**
 * Quest Start Policy - How quest is started
 */
UENUM(BlueprintType)
enum class EQuestStartPolicy : uint8
{
	/** Manual start by player or script */
	Manual UMETA(DisplayName = "Manual"),
	
	/** Auto-start when conditions are met */
	AutoOnCondition UMETA(DisplayName = "Auto on Condition"),
	
	/** Start from dialog */
	OnDialog UMETA(DisplayName = "On Dialog"),
	
	/** Start when item is picked up */
	OnItemPickup UMETA(DisplayName = "On Item Pickup"),
	
	/** Start when entering trigger volume */
	OnOverlapTrigger UMETA(DisplayName = "On Overlap Trigger"),
	
	/** Start on scheduled event */
	OnScheduleEvent UMETA(DisplayName = "On Schedule Event")
};

/**
 * Quest Failure Policy Type
 */
UENUM(BlueprintType)
enum class EQuestFailureType : uint8
{
	/** No failure condition */
	None UMETA(DisplayName = "None"),
	
	/** Fail after time limit */
	TimeLimit UMETA(DisplayName = "Time Limit"),
	
	/** Fail when condition is met */
	OnCondition UMETA(DisplayName = "On Condition"),
	
	/** Fail if certain event occurs */
	OnEvent UMETA(DisplayName = "On Event")
};

/**
 * Reward Type
 */
UENUM(BlueprintType)
enum class EQuestRewardType : uint8
{
	/** Give items to player */
	GiveItem UMETA(DisplayName = "Give Item"),
	
	/** Grant currency */
	GrantCurrency UMETA(DisplayName = "Grant Currency"),
	
	/** Modify player stats */
	ModifyStats UMETA(DisplayName = "Modify Stats"),
	
	/** Modify needs (hunger, sleep, etc) */
	ModifyNeeds UMETA(DisplayName = "Modify Needs"),
	
	/** Modify affection with NPC */
	ModifyAffection UMETA(DisplayName = "Modify Affection"),
	
	/** Unlock dialog branch */
	UnlockDialogBranch UMETA(DisplayName = "Unlock Dialog Branch"),
	
	/** Unlock crafting recipe */
	UnlockRecipe UMETA(DisplayName = "Unlock Recipe"),
	
	/** Grant gameplay tag */
	GrantTag UMETA(DisplayName = "Grant Tag"),
	
	/** Run blueprint event */
	RunBPEvent UMETA(DisplayName = "Run BP Event"),
	
	/** Execute custom effect */
	ExecuteEffect UMETA(DisplayName = "Execute Effect")
};

/**
 * Condition Event Type
 */
UENUM(BlueprintType)
enum class EConditionEventType : uint8
{
	/** Item was acquired */
	OnItemAcquired UMETA(DisplayName = "Item Acquired"),
	
	/** Item was used */
	OnItemUsed UMETA(DisplayName = "Item Used"),
	
	/** Item was gifted to NPC */
	OnItemGifted UMETA(DisplayName = "Item Gifted"),
	
	/** Talked to NPC */
	OnNpcTalked UMETA(DisplayName = "NPC Talked"),
	
	/** Dialog choice was picked */
	OnDialogChoice UMETA(DisplayName = "Dialog Choice"),
	
	/** Dialog node reached */
	OnDialogNodeReached UMETA(DisplayName = "Dialog Node Reached"),
	
	/** Entered area/location */
	OnEnterArea UMETA(DisplayName = "Enter Area"),
	
	/** Left area/location */
	OnLeaveArea UMETA(DisplayName = "Leave Area"),
	
	/** Actor was killed */
	OnActorKilled UMETA(DisplayName = "Actor Killed"),
	
	/** Time of day reached */
	OnTimeReached UMETA(DisplayName = "Time Reached"),
	
	/** Day changed */
	OnDayChanged UMETA(DisplayName = "Day Changed"),
	
	/** Weather state changed */
	OnWeatherState UMETA(DisplayName = "Weather State"),
	
	/** Need value below threshold */
	OnNeedBelow UMETA(DisplayName = "Need Below"),
	
	/** Need value above threshold */
	OnNeedAbove UMETA(DisplayName = "Need Above"),
	
	/** Affection value below threshold */
	OnAffectionBelow UMETA(DisplayName = "Affection Below"),
	
	/** Affection value above threshold */
	OnAffectionAbove UMETA(DisplayName = "Affection Above"),
	
	/** Item was crafted */
	OnItemCrafted UMETA(DisplayName = "Item Crafted"),
	
	/** Item was delivered */
	OnItemDelivered UMETA(DisplayName = "Item Delivered"),
	
	/** Custom gameplay tag emitted */
	OnCustomTag UMETA(DisplayName = "Custom Tag"),
	
	/** Interacted with object */
	OnInteract UMETA(DisplayName = "Interact"),
	
	/** Custom event */
	Custom UMETA(DisplayName = "Custom")
};

/**
 * Quest Event Payload - Data passed with quest events
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestEventPayload
{
	GENERATED_BODY()

	/** Event tag */
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	FGameplayTag EventTag;

	/** String parameter (ItemId, NpcId, AreaId, etc) */
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	FName StringParam;

	/** Integer parameter (Count, Amount, etc) */
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	int32 IntParam = 0;

	/** Float parameter (Value, Threshold, etc) */
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	float FloatParam = 0.0f;

	/** Tags parameter */
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	FGameplayTagContainer TagsParam;

	/** Instigator actor */
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	TWeakObjectPtr<AActor> InstigatorActor;

	/** Target actor */
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	TWeakObjectPtr<AActor> TargetActor;

	FQuestEventPayload() = default;
};

/**
 * Quest Meta Information
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestMeta
{
	GENERATED_BODY()

	/** Quest categories for filtering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTagContainer Categories;

	/** Quest tags for dependencies and logic */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTagContainer QuestTags;

	/** Difficulty level (1-5) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (ClampMin = 1, ClampMax = 5))
	int32 DifficultyLevel = 1;

	/** Is quest repeatable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsRepeatable = false;

	/** Is quest hidden from UI until started */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsHidden = false;

	/** Is main story quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsMainQuest = false;

	/** Quest priority (higher = more important) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 Priority = 0;

	/** Estimated completion time in minutes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 EstimatedDuration = 10;
};
