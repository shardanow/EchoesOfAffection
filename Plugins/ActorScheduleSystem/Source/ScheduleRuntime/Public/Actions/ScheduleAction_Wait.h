// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/ScheduleActionBase.h"
#include "ScheduleAction_Wait.generated.h"

/**
 * Simple action that waits for a specified duration
 * Useful in composite actions for timing control
 */
UCLASS(BlueprintType, meta = (DisplayName = "Wait"))
class SCHEDULERUNTIME_API UScheduleAction_Wait : public UScheduleActionBase
{
	GENERATED_BODY()

public:
	/** Duration to wait in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (ClampMin = "0"))
	float WaitDuration = 5.0f;

	/** Whether to use game time or real time */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool bUseGameTime = true;

protected:
	virtual FScheduleActionHandle ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context) override;
	virtual void CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual bool IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const override;
	virtual FText GetActionDescription_Implementation() const override;

private:
	/** Track wait timers */
	UPROPERTY(Transient)
	TMap<FGuid, float> WaitTimers;

	/** Track start times */
	UPROPERTY(Transient)
	TMap<FGuid, float> StartTimes;
};
