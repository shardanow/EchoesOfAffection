// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Conditions/ScheduleConditionBase.h"
#include "GameplayTagContainer.h"
#include "ScheduleCondition_GameplayTag.generated.h"

/**
 * Condition that checks for gameplay tags on the actor
 * Useful for checking actor state, abilities, buffs, etc.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Gameplay Tag Condition"))
class SCHEDULERUNTIME_API UScheduleCondition_GameplayTag : public UScheduleConditionBase
{
	GENERATED_BODY()

public:
	/** Tags that must be present */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTagContainer RequiredTags;

	/** If true, actor must have ALL required tags. If false, ANY tag is sufficient */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bRequireAll = true;

protected:
	virtual bool EvaluateConditionInternal_Implementation(const FScheduleEvaluationContext& Context) const override;
	virtual FText GetConditionDescription_Implementation() const override;
};
