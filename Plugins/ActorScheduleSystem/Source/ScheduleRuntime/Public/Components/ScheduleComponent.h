// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScheduleTypes.h"
#include "GameplayTagContainer.h"
#include "ScheduleComponent.generated.h"

class UScheduleData;
class UScheduleEntryData;
class UScheduleSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScheduleActionStarted, FScheduleActionHandle, ActionHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScheduleActionCompleted, FScheduleActionHandle, ActionHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScheduleActionInterrupted, FScheduleActionHandle, ActionHandle);

/**
 * Component that enables an actor to follow a schedule
 * Add this to any actor and assign a ScheduleData asset
 * No Tick - evaluation triggered by ScheduleSubsystem via time events
 */
UCLASS(ClassGroup=(Schedule), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class SCHEDULERUNTIME_API UScheduleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** The schedule data asset defining this actor's routine */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schedule")
	TObjectPtr<UScheduleData> ScheduleData;

	/** Whether the schedule is currently active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schedule")
	bool bScheduleEnabled = true;

	/** Priority offset for this specific actor (added to entry priorities) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schedule", meta = (ClampMin = "-50", ClampMax = "50"))
	int32 PriorityOffset = 0;

	/** Custom tags for this scheduled actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schedule")
	FGameplayTagContainer ActorScheduleTags;

	/** Called when an action starts */
	UPROPERTY(BlueprintAssignable, Category = "Schedule Events")
	FOnScheduleActionStarted OnActionStarted;

	/** Called when an action completes naturally */
	UPROPERTY(BlueprintAssignable, Category = "Schedule Events")
	FOnScheduleActionCompleted OnActionCompleted;

	/** Called when an action is interrupted */
	UPROPERTY(BlueprintAssignable, Category = "Schedule Events")
	FOnScheduleActionInterrupted OnActionInterrupted;

public:
	UScheduleComponent();

	/** UActorComponent interface */
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	/**
	 * Evaluate the schedule and execute appropriate actions
	 * Called by ScheduleSubsystem when time changes
	 */
	void EvaluateSchedule();

	/**
	 * Interrupt the current action (e.g., for player interaction)
	 * @param bCanResume - Whether the action can be resumed later
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void InterruptCurrentAction(bool bCanResume = true);

	/**
	 * Resume the interrupted action
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void ResumeInterruptedAction();

	/**
	 * Cancel the current action completely
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void CancelCurrentAction();

	/**
	 * Pause the schedule (stops evaluation but doesn't cancel current action)
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void PauseSchedule();

	/**
	 * Resume the schedule
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void ResumeSchedule();

	/**
	 * Check if schedule is currently paused
	 */
	UFUNCTION(BlueprintPure, Category = "Schedule")
	bool IsSchedulePaused() const { return bIsPaused; }

	/**
	 * Check if an action is currently active
	 */
	UFUNCTION(BlueprintPure, Category = "Schedule")
	bool HasActiveAction() const { return CurrentActionHandle.IsValid(); }

	/**
	 * Get the current action handle
	 */
	UFUNCTION(BlueprintPure, Category = "Schedule")
	FScheduleActionHandle GetCurrentActionHandle() const { return CurrentActionHandle; }

	/**
	 * Get the current schedule entry being executed
	 */
	UFUNCTION(BlueprintPure, Category = "Schedule")
	UScheduleEntryData* GetCurrentEntry() const { return CurrentEntry; }

	/**
	 * Get the schedule data
	 */
	UFUNCTION(BlueprintPure, Category = "Schedule")
	UScheduleData* GetScheduleData() const { return ScheduleData; }

	/**
	 * Set a new schedule data asset at runtime
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void SetScheduleData(UScheduleData* NewScheduleData, bool bCancelCurrent = true);

protected:
	/**
	 * Find the highest priority entry that matches current conditions
	 */
	UScheduleEntryData* FindMatchingEntry(const FScheduleEvaluationContext& Context) const;

	/**
	 * Execute a schedule entry
	 */
	void ExecuteEntry(UScheduleEntryData* Entry);

	/**
	 * Build evaluation context
	 */
	FScheduleEvaluationContext BuildEvaluationContext() const;

	/**
	 * Build execution context
	 */
	FScheduleExecutionContext BuildExecutionContext(const FScheduleLocation& Location) const;

private:
	/** Reference to schedule subsystem */
	UPROPERTY(Transient)
	TObjectPtr<UScheduleSubsystem> ScheduleSubsystem;

	/** Currently executing entry */
	UPROPERTY(Transient)
	TObjectPtr<UScheduleEntryData> CurrentEntry;

	/** Handle to current action */
	FScheduleActionHandle CurrentActionHandle;

	/** Interrupted action that can be resumed */
	UPROPERTY(Transient)
	TObjectPtr<UScheduleEntryData> InterruptedEntry;

	FScheduleActionHandle InterruptedActionHandle;

	/** Whether schedule is paused */
	bool bIsPaused = false;

	/** Last evaluation time to prevent duplicate evaluations */
	float LastEvaluationTime = -1.0f;
};
