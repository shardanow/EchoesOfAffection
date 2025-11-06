// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/ScheduleAction_FollowSpline.h"
#include "AIController.h"
#include "Components/SplineComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

FScheduleActionHandle UScheduleAction_FollowSpline::ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context)
{
	FScheduleActionHandle Handle = Super::ExecuteActionInternal_Implementation(Context);

	AActor* Actor = Context.ExecutingActor.Get();
	if (!Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("FollowSpline: No ExecutingActor!"));
		return FScheduleActionHandle();
	}

	if (!SplineActor)
	{
		UE_LOG(LogTemp, Error, TEXT("FollowSpline: No SplineActor assigned!"));
		return FScheduleActionHandle();
	}

	// Get spline component
	USplineComponent* Spline = SplineActor->FindComponentByClass<USplineComponent>();
	if (!Spline)
	{
		UE_LOG(LogTemp, Error, TEXT("FollowSpline: SplineActor has no SplineComponent!"));
		return FScheduleActionHandle();
	}

	// Get AI Controller
	AAIController* AIController = Cast<AAIController>(Actor->GetInstigatorController());
	if (!AIController)
	{
		if (APawn* Pawn = Cast<APawn>(Actor))
		{
			AIController = Cast<AAIController>(Pawn->GetController());
		}
	}

	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("FollowSpline: Actor has no AI Controller!"));
		return FScheduleActionHandle();
	}

	// ? Sample waypoints along spline
	TArray<FVector> Waypoints;
	float SplineLength = Spline->GetSplineLength();
	
	for (int32 i = 0; i <= SamplePoints; i++)
	{
		float Distance = (SplineLength / SamplePoints) * i;
		FVector WorldPos = Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		Waypoints.Add(WorldPos);
	}

	UE_LOG(LogTemp, Log, TEXT("FollowSpline: Generated %d waypoints along spline"), Waypoints.Num());

	// Store data
	ActiveMovements.Add(Handle.HandleID, AIController);
	WaypointPaths.Add(Handle.HandleID, Waypoints);
	CurrentWaypoints.Add(Handle.HandleID, 0);

	// Apply speed multiplier
	if (ACharacter* Character = Cast<ACharacter>(Actor))
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed *= SpeedMultiplier;
		}
	}

	// Start moving to first waypoint
	MoveToNextWaypoint(Handle.HandleID);

	return Handle;
}

void UScheduleAction_FollowSpline::MoveToNextWaypoint(FGuid HandleID)
{
	int32* CurrentIndex = CurrentWaypoints.Find(HandleID);
	TArray<FVector>* Waypoints = WaypointPaths.Find(HandleID);
	TWeakObjectPtr<AAIController>* ControllerPtr = ActiveMovements.Find(HandleID);

	if (!CurrentIndex || !Waypoints || !ControllerPtr)
	{
		return;
	}

	AAIController* Controller = ControllerPtr->Get();
	if (!Controller)
	{
		return;
	}

	// Check if we reached the end
	if (*CurrentIndex >= Waypoints->Num())
	{
		UE_LOG(LogTemp, Log, TEXT("FollowSpline: Reached end of spline path"));
		return;
	}

	// ? Move to next waypoint using AI
	FVector TargetWaypoint = (*Waypoints)[*CurrentIndex];
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetWaypoint);
	MoveRequest.SetAcceptanceRadius(WaypointRadius);
	MoveRequest.SetUsePathfinding(true);

	FPathFollowingRequestResult MoveResult = Controller->MoveTo(MoveRequest);

	if (MoveResult.Code == EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("FollowSpline: Moving to waypoint %d/%d at %s"), 
			*CurrentIndex + 1, Waypoints->Num(), *TargetWaypoint.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FollowSpline: Failed to move to waypoint %d"), *CurrentIndex);
	}
}

void UScheduleAction_FollowSpline::CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	if (TWeakObjectPtr<AAIController>* ControllerPtr = ActiveMovements.Find(ActionHandle.HandleID))
	{
		if (AAIController* Controller = ControllerPtr->Get())
		{
			Controller->StopMovement();

			// Restore speed
			if (ACharacter* Character = Cast<ACharacter>(Controller->GetPawn()))
			{
				if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
				{
					Movement->MaxWalkSpeed /= SpeedMultiplier;
				}
			}
		}

		ActiveMovements.Remove(ActionHandle.HandleID);
		WaypointPaths.Remove(ActionHandle.HandleID);
		CurrentWaypoints.Remove(ActionHandle.HandleID);
	}
}

bool UScheduleAction_FollowSpline::IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const
{
	const int32* CurrentIndex = CurrentWaypoints.Find(ActionHandle.HandleID);
	const TArray<FVector>* Waypoints = WaypointPaths.Find(ActionHandle.HandleID);
	const TWeakObjectPtr<AAIController>* ControllerPtr = ActiveMovements.Find(ActionHandle.HandleID);

	if (!CurrentIndex || !Waypoints || !ControllerPtr)
	{
		return false;
	}

	AAIController* Controller = ControllerPtr->Get();
	if (!Controller)
	{
		return false;
	}

	// Check if we reached current waypoint
	UPathFollowingComponent* PathFollowing = Controller->GetPathFollowingComponent();
	if (PathFollowing && PathFollowing->GetStatus() == EPathFollowingStatus::Idle)
	{
		// Move to next waypoint
		int32 NextIndex = *CurrentIndex + 1;
		
		// ? FIXED: Can't modify CurrentWaypoints in const method
		// Instead, we just check if there are more waypoints
		if (NextIndex < Waypoints->Num())
		{
			// ? Cast away const to modify state
			// This is safe because we're only updating internal tracking
			UScheduleAction_FollowSpline* MutableThis = const_cast<UScheduleAction_FollowSpline*>(this);
			MutableThis->CurrentWaypoints.FindOrAdd(ActionHandle.HandleID) = NextIndex;
			MutableThis->MoveToNextWaypoint(ActionHandle.HandleID);
			return true;
		}
		else
		{
			// Reached end of spline
			return false;
		}
	}

	// Still moving to current waypoint
	return PathFollowing && PathFollowing->GetStatus() != EPathFollowingStatus::Idle;
}
