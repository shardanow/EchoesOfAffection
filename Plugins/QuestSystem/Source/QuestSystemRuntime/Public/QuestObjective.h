// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "QuestTypes.h"
#include "QuestStructures.h"
#include "QuestObjective.generated.h"

/**
 * Quest Objective - Individual goal within a quest phase
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestObjective
{
	GENERATED_BODY()

	/** Unique objective ID within the quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FName ObjectiveId;

	/** Display title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText Title;

	/** Detailed description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText Description;

	/** Objective style/type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	EObjectiveStyle Style = EObjectiveStyle::Custom;

	/** Objective completion logic */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	EObjectiveLogic Logic = EObjectiveLogic::And;

	/** Conditions that trigger objective progress */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	TArray<FObjectiveCondition> Conditions;

	/** Gates that restrict when objective can progress */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	TArray<FObjectiveProgressGate> Gates;

	/** Progress counters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FObjectiveCounters Counters;

	/** Signals/events for lifecycle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FObjectiveSignals Signals;

	/** Is objective optional */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bIsOptional = false;

	/** Is objective hidden from UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bIsHidden = false;

	/** Track on HUD/compass */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bTrackOnHUD = true;

	/** Location to track (for navigation) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FVector TrackLocation = FVector::ZeroVector;

	/** Actor to track */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	TSoftObjectPtr<AActor> TrackActor;

	FQuestObjective() = default;
};

/**
 * Quest Phase - Stage/chapter within a quest
 */
USTRUCT(BlueprintType)
struct QUESTSYSTEMRUNTIME_API FQuestPhase
{
	GENERATED_BODY()

	/** Unique phase ID within the quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	FName PhaseId;

	/** Display title */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	FText Title;

	/** Detailed description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	FText Description;

	/** Objectives in this phase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	TArray<FQuestObjective> Objectives;

	/** Transition rules when phase completes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	FQuestPhaseTransition Transition;

	/** Phase requires ALL objectives complete (false = ANY) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	bool bRequireAllObjectives = true;

	/** Rewards given when phase completes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	FQuestRewardSet PhaseRewards;

	/** Is phase hidden from UI until activated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	bool bIsHidden = false;

	FQuestPhase() = default;
};
