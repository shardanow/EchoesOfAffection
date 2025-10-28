// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/ScheduleAction_Composite.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogScheduleComposite, Log, All);

UScheduleAction_Composite::UScheduleAction_Composite()
{
}

void UScheduleAction_Composite::CheckAndAdvanceSequential()
{
	// Check each active composite action
	TArray<FGuid> CompletedActions;

	for (auto& Pair : ChildActionData)
	{
		FGuid CompositeID = Pair.Key;
		FScheduleChildActionData& ActionData = Pair.Value;

		// Check if current child action is still active
		if (ActionData.CurrentActionIndex < ActionData.ChildHandles.Num() && 
			ActionData.CurrentActionIndex < ChildActions.Num())
		{
			UScheduleActionBase* CurrentChild = ChildActions[ActionData.CurrentActionIndex];
			if (CurrentChild)
			{
				bool bIsActive = IScheduleActionInterface::Execute_IsActionActive(
					CurrentChild, 
					ActionData.ChildHandles[ActionData.CurrentActionIndex]
				);

				if (!bIsActive)
				{
					UE_LOG(LogScheduleComposite, Log, TEXT("Child action %d completed, advancing to next"), ActionData.CurrentActionIndex);

					// Current action completed, move to next
					ActionData.CurrentActionIndex++;

					// Check if there are more actions
					if (ActionData.CurrentActionIndex < ChildActions.Num())
					{
						// Start next action
						UScheduleActionBase* NextChild = ChildActions[ActionData.CurrentActionIndex];
						if (NextChild)
						{
							FScheduleExecutionContext* Context = ExecutionContexts.Find(CompositeID);
							if (Context)
							{
								UE_LOG(LogScheduleComposite, Log, TEXT("Starting next child action %d"), ActionData.CurrentActionIndex);
								FScheduleActionHandle NextHandle = IScheduleActionInterface::Execute_ExecuteAction(NextChild, *Context);
								ActionData.ChildHandles.Add(NextHandle);
							}
						}
					}
					else
					{
						// All actions in sequence completed - check if we should loop
						if (bLoopChildActions)
						{
							ActionData.LoopCount++;
							
							// Check if we should continue looping
							bool bShouldContinueLooping = (MaxLoops == 0) || (ActionData.LoopCount < MaxLoops);
							
							if (bShouldContinueLooping)
							{
								UE_LOG(LogScheduleComposite, Log, TEXT("Looping child actions (Loop %d/%d)"), 
									ActionData.LoopCount, MaxLoops == 0 ? -1 : MaxLoops);
								
								// Reset to first action
								ActionData.CurrentActionIndex = 0;
								ActionData.ChildHandles.Empty();
								
								// Start first action again
								if (ChildActions.Num() > 0 && ChildActions[0])
								{
									FScheduleExecutionContext* Context = ExecutionContexts.Find(CompositeID);
									if (Context)
									{
										UE_LOG(LogScheduleComposite, Log, TEXT("Restarting first child action"));
										FScheduleActionHandle FirstHandle = IScheduleActionInterface::Execute_ExecuteAction(ChildActions[0], *Context);
										ActionData.ChildHandles.Add(FirstHandle);
									}
								}
							}
							else
							{
								// Max loops reached
								UE_LOG(LogScheduleComposite, Log, TEXT("Max loops (%d) reached for composite"), MaxLoops);
								CompletedActions.Add(CompositeID);
							}
						}
						else
						{
							// All actions completed, no looping
							UE_LOG(LogScheduleComposite, Log, TEXT("All child actions completed for composite"));
							CompletedActions.Add(CompositeID);
						}
					}
				}
			}
		}
	}

	// Cleanup completed composite actions
	for (const FGuid& CompletedID : CompletedActions)
	{
		// Stop timer
		if (CachedWorld.IsValid())
		{
			if (FTimerHandle* TimerHandle = CheckTimerHandles.Find(CompletedID))
			{
				CachedWorld->GetTimerManager().ClearTimer(*TimerHandle);
			}
		}

		ChildActionData.Remove(CompletedID);
		ExecutionContexts.Remove(CompletedID);
		CheckTimerHandles.Remove(CompletedID);
	}
}

FScheduleActionHandle UScheduleAction_Composite::ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context)
{
	FScheduleActionHandle Handle = Super::ExecuteActionInternal_Implementation(Context);

	if (ChildActions.Num() == 0)
	{
		UE_LOG(LogScheduleComposite, Warning, TEXT("Composite action has no child actions!"));
		return Handle;
	}

	// Cache world for timer management
	CachedWorld = Context.World;

	FScheduleChildActionData& ActionData = ChildActionData.Add(Handle.HandleID);
	ExecutionContexts.Add(Handle.HandleID, Context);

	UE_LOG(LogScheduleComposite, Log, TEXT("Starting composite action with %d children (Sequential: %s)"), 
		ChildActions.Num(), bExecuteSequentially ? TEXT("YES") : TEXT("NO"));

	if (bExecuteSequentially)
	{
		// Start first action
		ActionData.CurrentActionIndex = 0;
		if (ChildActions[0])
		{
			UE_LOG(LogScheduleComposite, Log, TEXT("Starting first child action"));
			FScheduleActionHandle ChildHandle = IScheduleActionInterface::Execute_ExecuteAction(ChildActions[0], Context);
			ActionData.ChildHandles.Add(ChildHandle);

			// Set up timer to check completion
			if (Context.World)
			{
				FTimerHandle& TimerHandle = CheckTimerHandles.Add(Handle.HandleID);
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUObject(this, &UScheduleAction_Composite::CheckAndAdvanceSequential);
				
				Context.World->GetTimerManager().SetTimer(
					TimerHandle,
					TimerDelegate,
					CheckInterval,
					true  // Loop
				);

				UE_LOG(LogScheduleComposite, Log, TEXT("Started timer with interval %.2fs"), CheckInterval);
			}
		}
	}
	else
	{
		// Start all actions in parallel
		UE_LOG(LogScheduleComposite, Log, TEXT("Starting all child actions in parallel"));
		for (UScheduleActionBase* ChildAction : ChildActions)
		{
			if (ChildAction)
			{
				FScheduleActionHandle ChildHandle = IScheduleActionInterface::Execute_ExecuteAction(ChildAction, Context);
				ActionData.ChildHandles.Add(ChildHandle);
			}
		}
	}

	return Handle;
}

void UScheduleAction_Composite::CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	if (FScheduleChildActionData* ActionData = ChildActionData.Find(ActionHandle.HandleID))
	{
		UE_LOG(LogScheduleComposite, Log, TEXT("Canceling composite action and all %d children"), ActionData->ChildHandles.Num());

		// Stop timer
		if (CachedWorld.IsValid())
		{
			if (FTimerHandle* TimerHandle = CheckTimerHandles.Find(ActionHandle.HandleID))
			{
				CachedWorld->GetTimerManager().ClearTimer(*TimerHandle);
			}
		}

		// Cancel all child actions
		for (int32 i = 0; i < ActionData->ChildHandles.Num() && i < ChildActions.Num(); ++i)
		{
			if (ChildActions[i])
			{
				IScheduleActionInterface::Execute_CancelAction(ChildActions[i], ActionData->ChildHandles[i]);
			}
		}

		ChildActionData.Remove(ActionHandle.HandleID);
		ExecutionContexts.Remove(ActionHandle.HandleID);
		CheckTimerHandles.Remove(ActionHandle.HandleID);
	}
}

void UScheduleAction_Composite::PauseActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	if (FScheduleChildActionData* ActionData = ChildActionData.Find(ActionHandle.HandleID))
	{
		// Pause timer
		if (CachedWorld.IsValid())
		{
			if (FTimerHandle* TimerHandle = CheckTimerHandles.Find(ActionHandle.HandleID))
			{
				CachedWorld->GetTimerManager().PauseTimer(*TimerHandle);
			}
		}

		for (int32 i = 0; i < ActionData->ChildHandles.Num() && i < ChildActions.Num(); ++i)
		{
			if (ChildActions[i])
			{
				IScheduleActionInterface::Execute_PauseAction(ChildActions[i], ActionData->ChildHandles[i]);
			}
		}
	}
}

void UScheduleAction_Composite::ResumeActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	if (FScheduleChildActionData* ActionData = ChildActionData.Find(ActionHandle.HandleID))
	{
		// Resume timer
		if (CachedWorld.IsValid())
		{
			if (FTimerHandle* TimerHandle = CheckTimerHandles.Find(ActionHandle.HandleID))
			{
				CachedWorld->GetTimerManager().UnPauseTimer(*TimerHandle);
			}
		}

		for (int32 i = 0; i < ActionData->ChildHandles.Num() && i < ChildActions.Num(); ++i)
		{
			if (ChildActions[i])
			{
				IScheduleActionInterface::Execute_ResumeAction(ChildActions[i], ActionData->ChildHandles[i]);
			}
		}
	}
}

bool UScheduleAction_Composite::IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const
{
	const FScheduleChildActionData* ActionData = ChildActionData.Find(ActionHandle.HandleID);
	if (!ActionData)
	{
		return false;
	}

	if (bExecuteSequentially)
	{
		// Check current action
		if (ActionData->CurrentActionIndex >= ActionData->ChildHandles.Num() || ActionData->CurrentActionIndex >= ChildActions.Num())
		{
			return false;
		}

		if (ChildActions[ActionData->CurrentActionIndex])
		{
			return IScheduleActionInterface::Execute_IsActionActive(ChildActions[ActionData->CurrentActionIndex], ActionData->ChildHandles[ActionData->CurrentActionIndex]);
		}
	}
	else
	{
		// Check if any action is active
		for (int32 i = 0; i < ActionData->ChildHandles.Num() && i < ChildActions.Num(); ++i)
		{
			if (ChildActions[i] && IScheduleActionInterface::Execute_IsActionActive(ChildActions[i], ActionData->ChildHandles[i]))
			{
				return true;
			}
		}
	}

	return false;
}

FText UScheduleAction_Composite::GetActionDescription_Implementation() const
{
	if (!ActionName.IsEmpty())
	{
		return Super::GetActionDescription_Implementation();
	}

	if (bExecuteSequentially)
	{
		if (bLoopChildActions)
		{
			if (MaxLoops > 0)
			{
				return FText::Format(
					NSLOCTEXT("ScheduleSystem", "LoopingCompositeWithMax", "Loop {0} actions (max {1} times)"),
					FText::AsNumber(ChildActions.Num()),
					FText::AsNumber(MaxLoops)
				);
			}
			else
			{
				return FText::Format(
					NSLOCTEXT("ScheduleSystem", "LoopingCompositeInfinite", "Loop {0} actions (infinite)"),
					FText::AsNumber(ChildActions.Num())
				);
			}
		}
		else
		{
			return FText::Format(
				NSLOCTEXT("ScheduleSystem", "SequentialComposite", "Sequential: {0} actions"),
				FText::AsNumber(ChildActions.Num())
			);
		}
	}
	else
	{
		return FText::Format(
			NSLOCTEXT("ScheduleSystem", "ParallelComposite", "Parallel: {0} actions"),
			FText::AsNumber(ChildActions.Num())
		);
	}
}
