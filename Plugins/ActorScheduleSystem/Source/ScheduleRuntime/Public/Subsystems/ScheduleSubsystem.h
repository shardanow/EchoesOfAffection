// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ScheduleTypes.h"
#include "GameplayTagContainer.h"
#include "ScheduleSubsystem.generated.h"

class UScheduleComponent;
class UScheduleData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScheduleEvent, const FScheduleEventData&, EventData);

/**
 * World subsystem managing all scheduled actors in the world
 * Event-driven, no per-frame tick - updates triggered by time system events
 * Handles batch evaluation and execution coordination
 */
UCLASS()
class SCHEDULERUNTIME_API UScheduleSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Called when any schedule event occurs (start, complete, interrupt, etc.) */
	UPROPERTY(BlueprintAssignable, Category = "Schedule Events")
	FOnScheduleEvent OnScheduleEvent;

public:
	UScheduleSubsystem();

	/** USubsystem interface */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual TStatId GetStatId() const override;

	/** UTickableWorldSubsystem interface - only used for minimal maintenance, not per-actor logic */
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override
	{
		// Don't tick until we're initialized
		if (!IsInitialized())
		{
			return ETickableTickType::Never;
		}
		return ETickableTickType::Always;
	}

	/**
	 * Register a schedule component with the subsystem
	 * Called automatically by UScheduleComponent::OnRegister
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void RegisterScheduleComponent(UScheduleComponent* Component);

	/**
	 * Unregister a schedule component
	 * Called automatically by UScheduleComponent::OnUnregister
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void UnregisterScheduleComponent(UScheduleComponent* Component);

	/**
	 * Notify subsystem of time change (called by external time system)
	 * This is the primary trigger for schedule evaluation
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void NotifyTimeChanged(int32 NewHour, int32 NewMinute, int32 NewDayOfWeek);

	/**
	 * Force re-evaluation of all schedules
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void RefreshAllSchedules();

	/**
	 * Get all actors with schedules matching specific tags
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	TArray<AActor*> GetActorsWithScheduleTags(const FGameplayTagContainer& Tags, bool bExactMatch = false) const;

	/**
	 * Pause all schedules (e.g., during cinematic or dialogue)
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void PauseAllSchedules();

	/**
	 * Resume all paused schedules
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void ResumeAllSchedules();

	/**
	 * Get statistics for debugging/profiling
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule")
	void GetScheduleStats(int32& OutTotalActors, int32& OutActiveActions, int32& OutPausedActions) const;

	/**
	 * Get current time for schedule evaluation
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Schedule")
	void GetCurrentTime(int32& OutHour, int32& OutMinute, int32& OutDayOfWeek) const
	{
		OutHour = CurrentHour;
		OutMinute = CurrentMinute;
		OutDayOfWeek = CurrentDayOfWeek;
	}

protected:
	/**
	 * Evaluate all registered schedules
	 */
	void EvaluateSchedules();

	/**
	 * Broadcast a schedule event
	 */
	void BroadcastScheduleEvent(const FGameplayTag& EventType, AActor* Actor, const FScheduleActionHandle& Handle);

private:
	/** All registered schedule components */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UScheduleComponent>> RegisteredComponents;

	/** Current time state */
	int32 CurrentHour = 8;
	int32 CurrentMinute = 0;
	int32 CurrentDayOfWeek = 1;

	/** Last evaluation time to detect changes */
	int32 LastEvaluationHour = -1;
	int32 LastEvaluationMinute = -1;

	/** Whether subsystem is paused */
	bool bIsPaused = false;

	/** Accumulated delta time for statistics */
	float AccumulatedDeltaTime = 0.0f;
};
