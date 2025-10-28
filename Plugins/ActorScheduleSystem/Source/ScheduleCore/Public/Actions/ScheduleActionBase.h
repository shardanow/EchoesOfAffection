// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/ScheduleActionInterface.h"
#include "ScheduleActionBase.generated.h"

/**
 * Base class for all schedule actions
 * Extend this in C++ or Blueprint to create custom actions (AI behavior, animation, dialogue, etc.)
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class SCHEDULECORE_API UScheduleActionBase : public UObject, public IScheduleActionInterface
{
	GENERATED_BODY()

public:
	/** Human-readable name for this action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	FText ActionName;

	/** Detailed description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action", meta = (MultiLine = true))
	FText ActionDescription;

	/** Whether this action can be interrupted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool bCanBeInterruptedOverride = true;

	/** Gameplay tags for this action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	FGameplayTagContainer ActionTags;

public:
	/** IScheduleActionInterface implementation */
	virtual FScheduleActionHandle ExecuteAction_Implementation(const FScheduleExecutionContext& Context) override;
	virtual void CancelAction_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual void PauseAction_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual void ResumeAction_Implementation(FScheduleActionHandle ActionHandle) override;
	virtual bool IsActionActive_Implementation(FScheduleActionHandle ActionHandle) const override;
	virtual FText GetActionDescription_Implementation() const override;
	virtual bool CanBeInterrupted_Implementation() const override { return bCanBeInterruptedOverride; }

protected:
	/**
	 * Override in child classes to implement action execution logic
	 * @return Handle to track this action instance
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Action", meta = (DisplayName = "Execute Action"))
	FScheduleActionHandle ExecuteActionInternal(const FScheduleExecutionContext& Context);
	virtual FScheduleActionHandle ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context);

	/**
	 * Override to implement cancel logic
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Action", meta = (DisplayName = "Cancel Action"))
	void CancelActionInternal(FScheduleActionHandle ActionHandle);
	virtual void CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle) {}

	/**
	 * Override to implement pause logic
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Action", meta = (DisplayName = "Pause Action"))
	void PauseActionInternal(FScheduleActionHandle ActionHandle);
	virtual void PauseActionInternal_Implementation(FScheduleActionHandle ActionHandle) {}

	/**
	 * Override to implement resume logic
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Action", meta = (DisplayName = "Resume Action"))
	void ResumeActionInternal(FScheduleActionHandle ActionHandle);
	virtual void ResumeActionInternal_Implementation(FScheduleActionHandle ActionHandle) {}

	/**
	 * Override to check if action is still active
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Action", meta = (DisplayName = "Is Action Active"))
	bool IsActionActiveInternal(FScheduleActionHandle ActionHandle) const;
	virtual bool IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const { return false; }

protected:
	/** Active action handles */
	UPROPERTY(Transient)
	TMap<FGuid, FScheduleActionHandle> ActiveHandles;
};
