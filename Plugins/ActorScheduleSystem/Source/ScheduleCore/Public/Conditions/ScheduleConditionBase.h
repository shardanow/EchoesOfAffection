// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/ScheduleConditionInterface.h"
#include "ScheduleConditionBase.generated.h"

/**
 * Base class for all schedule conditions
 * Extend this in C++ or Blueprint to create custom conditions
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class SCHEDULECORE_API UScheduleConditionBase : public UObject, public IScheduleConditionInterface
{
	GENERATED_BODY()

public:
	/** Human-readable name for this condition */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FText ConditionName;

	/** Whether this condition is inverted (NOT logic) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bInvertCondition = false;

public:
	/** IScheduleConditionInterface implementation */
	virtual bool EvaluateCondition_Implementation(const FScheduleEvaluationContext& Context) const override;
	virtual FText GetConditionDescription_Implementation() const override;
	virtual bool IsTimeBased_Implementation() const override { return false; }

protected:
	/**
	 * Override this in child classes to implement condition logic
	 * @return True if condition is met
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Condition", meta = (DisplayName = "Evaluate Condition"))
	bool EvaluateConditionInternal(const FScheduleEvaluationContext& Context) const;
	virtual bool EvaluateConditionInternal_Implementation(const FScheduleEvaluationContext& Context) const { return true; }
};
