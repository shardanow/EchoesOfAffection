// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/ScheduleAction_Wait.h"
#include "Engine/World.h"

FScheduleActionHandle UScheduleAction_Wait::ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context)
{
	FScheduleActionHandle Handle = Super::ExecuteActionInternal_Implementation(Context);

	if (Context.World)
	{
		float CurrentTime = bUseGameTime ? Context.World->GetTimeSeconds() : Context.World->GetRealTimeSeconds();
		StartTimes.Add(Handle.HandleID, CurrentTime);
		WaitTimers.Add(Handle.HandleID, 0.0f);
	}

	return Handle;
}

void UScheduleAction_Wait::CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	WaitTimers.Remove(ActionHandle.HandleID);
	StartTimes.Remove(ActionHandle.HandleID);
}

bool UScheduleAction_Wait::IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const
{
	const float* StartTime = StartTimes.Find(ActionHandle.HandleID);
	if (!StartTime)
	{
		return false;
	}

	if (AActor* Actor = ActionHandle.OwningActor.Get())
	{
		if (UWorld* World = Actor->GetWorld())
		{
			float CurrentTime = bUseGameTime ? World->GetTimeSeconds() : World->GetRealTimeSeconds();
			float ElapsedTime = CurrentTime - *StartTime;
			return ElapsedTime < WaitDuration;
		}
	}

	return false;
}

FText UScheduleAction_Wait::GetActionDescription_Implementation() const
{
	if (!ActionName.IsEmpty())
	{
		return Super::GetActionDescription_Implementation();
	}

	return FText::Format(
		NSLOCTEXT("ScheduleSystem", "WaitAction", "Wait {0} seconds"),
		FText::AsNumber(WaitDuration)
	);
}
