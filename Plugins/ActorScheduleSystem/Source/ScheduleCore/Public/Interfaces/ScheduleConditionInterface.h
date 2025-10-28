// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ScheduleTypes.h"
#include "ScheduleConditionInterface.generated.h"

/**
 * Interface for schedule conditions that determine when actions can execute
 * Implement this to create custom condition types (time, weather, gameplay state, etc.)
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UScheduleConditionInterface : public UInterface
{
	GENERATED_BODY()
};

class SCHEDULECORE_API IScheduleConditionInterface
{
	GENERATED_BODY()

public:
	/**
	 * Evaluate if this condition is currently met
	 * @param Context - The evaluation context containing world, actor, and other relevant data
	 * @return True if the condition is satisfied
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	bool EvaluateCondition(const FScheduleEvaluationContext& Context) const;
	virtual bool EvaluateCondition_Implementation(const FScheduleEvaluationContext& Context) const { return true; }

	/**
	 * Get a human-readable description of this condition for editor display
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	FText GetConditionDescription() const;
	virtual FText GetConditionDescription_Implementation() const { return FText::FromString(TEXT("Custom Condition")); }

	/**
	 * Check if this condition requires time-based evaluation
	 * If true, scheduler will re-evaluate on time changes
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	bool IsTimeBased() const;
	virtual bool IsTimeBased_Implementation() const { return false; }
};
