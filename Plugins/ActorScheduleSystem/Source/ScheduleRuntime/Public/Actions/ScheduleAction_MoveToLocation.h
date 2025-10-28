// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/ScheduleActionBase.h"
#include "ScheduleAction_MoveToLocation.generated.h"

class AAIController;
class UNavigationSystemV1;

/**
 * Action that moves the actor to a target location using AI navigation
 * Supports NavMesh, NavArea costs for road following
 */
UCLASS(BlueprintType, meta = (DisplayName = "Move To Location"))
class SCHEDULERUNTIME_API UScheduleAction_MoveToLocation : public UScheduleActionBase
{
	GENERATED_BODY()

public:
	/** Acceptance radius (how close is "arrived") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0"))
	float AcceptanceRadius = 100.0f;

	/** Whether to use pathfinding or direct movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bUsePathfinding = true;

	/** Stop on overlap with target actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bStopOnOverlap = true;

	/** Stop on overlap with ANY actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bAcceptPartialPath = true;

	/** Movement speed multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float SpeedMultiplier = 1.0f;

	/** Filter class for navigation (e.g., for road-only navigation) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TSubclassOf<class UNavigationQueryFilter> FilterClass;

	/** Optional location selector - if set, overrides Context.TargetLocation */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Movement")
	TObjectPtr<class UScheduleLocationSelectorBase> LocationSelector;

protected:
	virtual FScheduleActionHandle ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context) override;
	virtual void CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual bool IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const override;
	virtual FText GetActionDescription_Implementation() const override;

private:
	/** Track movement requests */
	UPROPERTY(Transient)
	TMap<FGuid, TWeakObjectPtr<AAIController>> ActiveMovements;
};
