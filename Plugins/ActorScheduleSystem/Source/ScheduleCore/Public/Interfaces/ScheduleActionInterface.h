// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ScheduleTypes.h"
#include "ScheduleActionInterface.generated.h"

/**
 * Interface for schedule actions that can be executed on actors
 * Implement this to create custom action types (AI behavior, animation, dialogue, etc.)
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UScheduleActionInterface : public UInterface
{
	GENERATED_BODY()
};

class SCHEDULECORE_API IScheduleActionInterface
{
	GENERATED_BODY()

public:
	/**
	 * Execute the action on the target actor
	 * @param Context - The execution context containing actor, world, and schedule data
	 * @return Handle to track the action execution
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	FScheduleActionHandle ExecuteAction(const FScheduleExecutionContext& Context);
	virtual FScheduleActionHandle ExecuteAction_Implementation(const FScheduleExecutionContext& Context) { return FScheduleActionHandle(); }

	/**
	 * Cancel an ongoing action
	 * @param ActionHandle - Handle to the action to cancel
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	void CancelAction(FScheduleActionHandle ActionHandle);
	virtual void CancelAction_Implementation(FScheduleActionHandle ActionHandle) {}

	/**
	 * Pause an ongoing action (allows resuming later)
	 * @param ActionHandle - Handle to the action to pause
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	void PauseAction(FScheduleActionHandle ActionHandle);
	virtual void PauseAction_Implementation(FScheduleActionHandle ActionHandle) {}

	/**
	 * Resume a paused action
	 * @param ActionHandle - Handle to the action to resume
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	void ResumeAction(FScheduleActionHandle ActionHandle);
	virtual void ResumeAction_Implementation(FScheduleActionHandle ActionHandle) {}

	/**
	 * Check if an action is still running
	 * @param ActionHandle - Handle to check
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	bool IsActionActive(FScheduleActionHandle ActionHandle) const;
	virtual bool IsActionActive_Implementation(FScheduleActionHandle ActionHandle) const { return false; }

	/**
	 * Get a human-readable description of this action for editor display
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	FText GetActionDescription() const;
	virtual FText GetActionDescription_Implementation() const { return FText::FromString(TEXT("Custom Action")); }

	/**
	 * Check if this action can be interrupted
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schedule")
	bool CanBeInterrupted() const;
	virtual bool CanBeInterrupted_Implementation() const { return true; }
};
