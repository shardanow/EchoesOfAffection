// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScheduleTimeIntegrationComponent.generated.h"

class UScheduleSubsystem;

/**
 * Component that integrates external time systems with the schedule system
 * Listens to time system events and forwards them to ScheduleSubsystem
 * Add this to GameState or a persistent actor to enable time-based scheduling
 * 
 * NO DEPENDENCIES on specific time systems - only event-based communication
 */
UCLASS(ClassGroup=(Schedule), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class SCHEDULERUNTIME_API UScheduleTimeIntegrationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Whether integration is currently active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration")
	bool bIntegrationEnabled = true;

	/** Event names to listen for from external time system (leave empty to use default) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Integration")
	TArray<FName> TimeChangeEventNames;

public:
	UScheduleTimeIntegrationComponent();

	/** UActorComponent interface */
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Manually notify schedule system of time change
	 * Call this from your time system's Blueprint or C++ code
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule Integration")
	void NotifyTimeChanged(int32 Hour, int32 Minute, int32 DayOfWeek);

	/**
	 * Subscribe to time system events (Blueprint-friendly)
	 * Use this if your time system broadcasts events
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule Integration")
	void SubscribeToTimeEvents();

	/**
	 * Unsubscribe from time system events
	 */
	UFUNCTION(BlueprintCallable, Category = "Schedule Integration")
	void UnsubscribeFromTimeEvents();

protected:
	/**
	 * Override this in Blueprint to implement custom time fetching logic
	 * Called periodically if no events are available
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule Integration")
	void FetchCurrentTimeFromSystem(int32& OutHour, int32& OutMinute, int32& OutDayOfWeek);
	virtual void FetchCurrentTimeFromSystem_Implementation(int32& OutHour, int32& OutMinute, int32& OutDayOfWeek);

private:
	/** Reference to schedule subsystem */
	UPROPERTY(Transient)
	TObjectPtr<UScheduleSubsystem> ScheduleSubsystem;

	/** Last notified time to prevent duplicate notifications */
	int32 LastNotifiedHour = -1;
	int32 LastNotifiedMinute = -1;
};
