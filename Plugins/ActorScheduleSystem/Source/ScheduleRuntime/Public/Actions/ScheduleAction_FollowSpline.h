// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/ScheduleActionBase.h"
#include "ScheduleAction_FollowSpline.generated.h"

class AAIController;
class USplineComponent;

/**
 * Action that makes actor follow a spline path using AI movement
 * Combines AI pathfinding with predefined spline routes
 */
UCLASS(BlueprintType, meta = (DisplayName = "Follow Spline Path"))
class SCHEDULERUNTIME_API UScheduleAction_FollowSpline : public UScheduleActionBase
{
	GENERATED_BODY()

public:
	/** Spline actor to follow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<AActor> SplineActor;

	/** How many points to sample along spline */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "2", ClampMax = "50"))
	int32 SamplePoints = 10;

	/** Movement speed multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float SpeedMultiplier = 1.0f;

	/** Acceptance radius for each waypoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0"))
	float WaypointRadius = 100.0f;

protected:
	virtual FScheduleActionHandle ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context) override;
	virtual void CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual bool IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const override;

private:
	/** Current waypoint index for each active movement */
	UPROPERTY(Transient)
	TMap<FGuid, int32> CurrentWaypoints;

	/** AI Controllers for active movements */
	UPROPERTY(Transient)
	TMap<FGuid, TWeakObjectPtr<AAIController>> ActiveMovements;

	/** Waypoint paths for each movement - stored separately to avoid TMap<TArray> issue */
	TMap<FGuid, TArray<FVector>> WaypointPaths;  // ? Removed UPROPERTY

	/** Move to next waypoint along spline */
	void MoveToNextWaypoint(FGuid HandleID);
};
