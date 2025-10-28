// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/ScheduleActionBase.h"

FScheduleActionHandle UScheduleActionBase::ExecuteAction_Implementation(const FScheduleExecutionContext& Context)
{
	return ExecuteActionInternal(Context);
}

void UScheduleActionBase::CancelAction_Implementation(FScheduleActionHandle ActionHandle)
{
	if (ActiveHandles.Contains(ActionHandle.HandleID))
	{
		CancelActionInternal(ActionHandle);
		ActiveHandles.Remove(ActionHandle.HandleID);
	}
}

void UScheduleActionBase::PauseAction_Implementation(FScheduleActionHandle ActionHandle)
{
	if (ActiveHandles.Contains(ActionHandle.HandleID))
	{
		PauseActionInternal(ActionHandle);
	}
}

void UScheduleActionBase::ResumeAction_Implementation(FScheduleActionHandle ActionHandle)
{
	if (ActiveHandles.Contains(ActionHandle.HandleID))
	{
		ResumeActionInternal(ActionHandle);
	}
}

bool UScheduleActionBase::IsActionActive_Implementation(FScheduleActionHandle ActionHandle) const
{
	if (!ActiveHandles.Contains(ActionHandle.HandleID))
	{
		return false;
	}

	return IsActionActiveInternal(ActionHandle);
}

FText UScheduleActionBase::GetActionDescription_Implementation() const
{
	if (!ActionName.IsEmpty())
	{
		return ActionName;
	}

	return NSLOCTEXT("ScheduleSystem", "UnnamedAction", "Custom Action");
}

FScheduleActionHandle UScheduleActionBase::ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context)
{
	// Default implementation: create a handle and mark as active
	FScheduleActionHandle Handle;
	Handle.OwningActor = Context.ExecutingActor;
	ActiveHandles.Add(Handle.HandleID, Handle);
	return Handle;
}
