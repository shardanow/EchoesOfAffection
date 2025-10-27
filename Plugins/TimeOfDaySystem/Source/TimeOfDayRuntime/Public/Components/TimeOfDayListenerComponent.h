// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/TimeOfDayInterface.h"
#include "TimeOfDayListenerComponent.generated.h"

/**
 * Component that automatically registers/unregisters with TimeOfDaySubsystem
 * Provides a simple way for actors to receive time events
 * 
 * Usage:
 * - Add this component to any actor that needs to respond to time changes
 * - Implement the desired event handlers in Blueprint or C++
 * - The component handles registration/unregistration automatically
 */
UCLASS(ClassGroup = (TimeOfDay), meta = (BlueprintSpawnableComponent))
class TIMEOFDAYRUNTIME_API UTimeOfDayListenerComponent : public UActorComponent, public ITimeOfDayListener
{
	GENERATED_BODY()

public:
	UTimeOfDayListenerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// ========== ITimeOfDayListener Interface ==========
	
	virtual void OnMinuteChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnHourChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnDayChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnDayPhaseChanged_Implementation(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime) override;
	virtual void OnSeasonChanged_Implementation(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime) override;
	virtual void OnYearChanged_Implementation(const FTimeOfDayTime& CurrentTime) override;
	virtual void OnTimeScaleChanged_Implementation(float NewTimeScale) override;

	// ========== Blueprint Events ==========

	/** Called when a new minute starts */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Minute Changed"))
	void BP_OnMinuteChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when a new hour starts */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Hour Changed"))
	void BP_OnHourChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when a new day starts */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Day Changed"))
	void BP_OnDayChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when the day phase changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Day Phase Changed"))
	void BP_OnDayPhaseChanged(ETimeOfDayPhase NewPhase, const FTimeOfDayTime& CurrentTime);

	/** Called when a new season starts */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Season Changed"))
	void BP_OnSeasonChanged(ETimeOfDaySeason NewSeason, const FTimeOfDayTime& CurrentTime);

	/** Called when a new year starts */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Year Changed"))
	void BP_OnYearChanged(const FTimeOfDayTime& CurrentTime);

	/** Called when time scale changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Time of Day", meta = (DisplayName = "On Time Scale Changed"))
	void BP_OnTimeScaleChanged(float NewTimeScale);

	// ========== Configuration ==========

	/** Whether to receive minute change events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Events")
	bool bReceiveMinuteEvents = false;

	/** Whether to receive hour change events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Events")
	bool bReceiveHourEvents = true;

	/** Whether to receive day change events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Events")
	bool bReceiveDayEvents = true;

	/** Whether to receive phase change events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Events")
	bool bReceivePhaseEvents = true;

	/** Whether to receive season change events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Events")
	bool bReceiveSeasonEvents = true;

	/** Whether to receive year change events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Events")
	bool bReceiveYearEvents = false;

	/** Whether to receive time scale change events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time of Day|Events")
	bool bReceiveTimeScaleEvents = false;
};

