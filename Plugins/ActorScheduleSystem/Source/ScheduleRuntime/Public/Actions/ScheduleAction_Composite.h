// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/ScheduleActionBase.h"
#include "ScheduleAction_Composite.generated.h"

/** Helper struct to hold child action handles (allows use in TMap) */
USTRUCT()
struct FScheduleChildActionData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FScheduleActionHandle> ChildHandles;

	UPROPERTY()
	int32 CurrentActionIndex = 0;

	UPROPERTY()
	int32 LoopCount = 0;
};

/**
 * Composite action that executes multiple actions in sequence or parallel
 * Useful for complex behaviors like "Move to location, then play animation, then wait"
 */
UCLASS(BlueprintType, meta = (DisplayName = "Composite Action"))
class SCHEDULERUNTIME_API UScheduleAction_Composite : public UScheduleActionBase
{
	GENERATED_BODY()

public:
	UScheduleAction_Composite();

	/** Child actions to execute */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Action")
	TArray<TObjectPtr<UScheduleActionBase>> ChildActions;

	/** Whether to execute actions in sequence (true) or parallel (false) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool bExecuteSequentially = true;

	/** Whether to stop all actions if one fails (only for sequential) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (EditCondition = "bExecuteSequentially"))
	bool bStopOnFailure = true;

	/** Whether to loop child actions indefinitely until cancelled (only for sequential) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (EditCondition = "bExecuteSequentially"))
	bool bLoopChildActions = false;

	/** Maximum number of loops (0 = infinite). Only used if bLoopChildActions is true */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (EditCondition = "bExecuteSequentially && bLoopChildActions", ClampMin = "0"))
	int32 MaxLoops = 0;

	/** How often to check for action completion (in seconds). 0 = check every time update */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (ClampMin = "0.0"))
	float CheckInterval = 0.5f;

protected:
	virtual FScheduleActionHandle ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context) override;
	virtual void CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual void PauseActionInternal_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual void ResumeActionInternal_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual bool IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const override;
	virtual FText GetActionDescription_Implementation() const override;

	/** Check and advance sequential execution - called by timer */
	void CheckAndAdvanceSequential();

private:
	/** Track child action data per composite handle */
	UPROPERTY(Transient)
	TMap<FGuid, FScheduleChildActionData> ChildActionData;

	/** Cached execution contexts for each composite action */
	UPROPERTY(Transient)
	TMap<FGuid, FScheduleExecutionContext> ExecutionContexts;

	/** Timer handles for checking action completion */
	TMap<FGuid, FTimerHandle> CheckTimerHandles;

	/** World context for timer management */
	TWeakObjectPtr<UWorld> CachedWorld;
};
