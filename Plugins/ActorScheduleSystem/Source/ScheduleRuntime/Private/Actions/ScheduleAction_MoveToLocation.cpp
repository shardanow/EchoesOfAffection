// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actions/ScheduleAction_MoveToLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/ScheduleLocationSelectorInterface.h"
#include "Selectors/ScheduleLocationSelectorBase.h"

FScheduleActionHandle UScheduleAction_MoveToLocation::ExecuteActionInternal_Implementation(const FScheduleExecutionContext& Context)
{
	FScheduleActionHandle Handle = Super::ExecuteActionInternal_Implementation(Context);

	AActor* Actor = Context.ExecutingActor.Get();
	if (!Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("MoveToLocation: No ExecutingActor in context!"));
		return FScheduleActionHandle(); // Return invalid handle
	}

	// Determine target location - use own LocationSelector if available, otherwise use Context
	FScheduleLocation TargetLocation = Context.TargetLocation;
	
	if (LocationSelector)
	{
		UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Using own LocationSelector: %s"), *LocationSelector->GetName());
		
		FScheduleLocationContext LocationContext;
		LocationContext.World = Context.World;
		LocationContext.RequestingActor = Actor;
		
		FScheduleLocation SelectedLocation;
		bool bLocationSelected = IScheduleLocationSelectorInterface::Execute_SelectLocation(
			LocationSelector, 
			LocationContext, 
			SelectedLocation
		);
		
		if (bLocationSelected)
		{
			TargetLocation = SelectedLocation;
			UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Selected location: %s"), *TargetLocation.Location.ToString());
			if (TargetLocation.ReferenceActor.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("   ReferenceActor: %s"), *TargetLocation.ReferenceActor->GetName());
			}
			if (TargetLocation.LocationTags.Num() > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("   LocationTags: %s"), *TargetLocation.LocationTags.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveToLocation: LocationSelector failed to select location, using Context location"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Using location from Context"));
		if (TargetLocation.LocationTags.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("   Context LocationTags: %s"), *TargetLocation.LocationTags.ToString());
		}
	}

	if (!TargetLocation.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("MoveToLocation: Target location is invalid for actor %s"), *Actor->GetName());
		return FScheduleActionHandle(); // Return invalid handle
	}

	UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Starting movement for %s to %s"), 
		*Actor->GetName(), *TargetLocation.Location.ToString());

	// Get or create AI controller
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
		UE_LOG(LogTemp, Error, TEXT("MoveToLocation: Actor %s has no AI Controller"), *Actor->GetName());
		UE_LOG(LogTemp, Error, TEXT("   Make sure: 1) Actor is a Pawn, 2) AI Controller is assigned, 3) Auto Possess AI is set"));
		return FScheduleActionHandle(); // Return invalid handle
	}

	UE_LOG(LogTemp, Log, TEXT("MoveToLocation: AI Controller found: %s"), *AIController->GetName());

	// Apply speed multiplier if actor is a character
	if (ACharacter* Character = Cast<ACharacter>(Actor))
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed *= SpeedMultiplier;
			UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Speed set to %.2f"), Movement->MaxWalkSpeed);
		}
	}

	// Execute movement
	FVector TargetPos = TargetLocation.Location;
	AActor* TargetActor = TargetLocation.ReferenceActor.Get();

	// ? FIX: Save target BEFORE starting movement (for pause/resume)
	PausedTargetLocations.Add(Handle.HandleID, TargetPos);
	if (TargetActor)
	{
		PausedTargetActors.Add(Handle.HandleID, TargetActor);
		UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Saved target ACTOR for pause/resume: %s"), *TargetActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Saved target LOCATION for pause/resume: %s"), *TargetPos.ToString());
	}

	EPathFollowingRequestResult::Type Result = EPathFollowingRequestResult::Failed;
	
	// Use FAIMoveRequest for more control
	FAIMoveRequest MoveRequest;
	if (TargetActor)
	{
		MoveRequest.SetGoalActor(TargetActor);
		UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Moving to actor %s"), *TargetActor->GetName());
	}
	else
	{
		MoveRequest.SetGoalLocation(TargetPos);
		UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Moving to location %s"), *TargetPos.ToString());
	}
	
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	MoveRequest.SetCanStrafe(bUsePathfinding);
	MoveRequest.SetAllowPartialPath(bAcceptPartialPath);
	MoveRequest.SetUsePathfinding(bUsePathfinding);
	
	if (FilterClass)
	{
		MoveRequest.SetNavigationFilter(FilterClass);
	}
	
	FPathFollowingRequestResult MoveResult = AIController->MoveTo(MoveRequest);
	Result = MoveResult.Code;

	UE_LOG(LogTemp, Log, TEXT("MoveToLocation: MoveTo result: %d"), (int32)Result);

	// Both RequestSuccessful and AlreadyAtGoal should be treated as success
	if (Result == EPathFollowingRequestResult::RequestSuccessful || Result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		// Always add to ActiveMovements for tracking, even if already at goal
		ActiveMovements.Add(Handle.HandleID, AIController);
		
		if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Already at goal location"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("MoveToLocation: Movement started successfully"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MoveToLocation: Movement request FAILED for actor %s"), *Actor->GetName());
		UE_LOG(LogTemp, Error, TEXT("   Result code: %d"), (int32)Result);
		UE_LOG(LogTemp, Error, TEXT("   Check: 1) NavMesh exists (press P), 2) Target is on NavMesh, 3) Path is reachable"));
		
		// Cleanup saved data on failure
		PausedTargetLocations.Remove(Handle.HandleID);
		PausedTargetActors.Remove(Handle.HandleID);
		
		// Return invalid handle since movement failed
		return FScheduleActionHandle();
	}

	return Handle;
}

void UScheduleAction_MoveToLocation::CancelActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	if (TWeakObjectPtr<AAIController>* ControllerPtr = ActiveMovements.Find(ActionHandle.HandleID))
	{
		if (AAIController* Controller = ControllerPtr->Get())
		{
			Controller->StopMovement();
			
			// Restore speed if modified
			if (ACharacter* Character = Cast<ACharacter>(Controller->GetPawn()))
			{
				if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
				{
					Movement->MaxWalkSpeed /= SpeedMultiplier;
				}
			}
		}

		ActiveMovements.Remove(ActionHandle.HandleID);
		
		// ? Cleanup saved pause data
		PausedTargetLocations.Remove(ActionHandle.HandleID);
		PausedTargetActors.Remove(ActionHandle.HandleID);
	}
}

void UScheduleAction_MoveToLocation::PauseActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("[MOVE] PauseAction called for handle: %s"), *ActionHandle.HandleID.ToString());

	if (TWeakObjectPtr<AAIController>* ControllerPtr = ActiveMovements.Find(ActionHandle.HandleID))
	{
		if (AAIController* Controller = ControllerPtr->Get())
		{
			UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Pausing AI movement for: %s"), 
				Controller->GetPawn() ? *Controller->GetPawn()->GetName() : TEXT("NULL"));

			// ? FIX: Target is already saved in ExecuteActionInternal, just verify it exists
			FVector* SavedLocation = PausedTargetLocations.Find(ActionHandle.HandleID);
			TWeakObjectPtr<AActor>* SavedActor = PausedTargetActors.Find(ActionHandle.HandleID);
			
			if (SavedLocation || SavedActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Using saved target: %s (Actor: %s)"),
					SavedLocation ? *SavedLocation->ToString() : TEXT("None"),
					(SavedActor && SavedActor->IsValid()) ? *SavedActor->Get()->GetName() : TEXT("None"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[MOVE]   ERROR: No saved target found! Movement may not resume correctly."));
			}

			// Stop AI movement
			Controller->StopMovement();
			
			// Also stop physical movement
			if (APawn* Pawn = Controller->GetPawn())
			{
				if (ACharacter* Character = Cast<ACharacter>(Pawn))
				{
					if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
					{
						Movement->StopMovementImmediately();
						UE_LOG(LogTemp, Warning, TEXT("[MOVE] Stopped CharacterMovementComponent"));
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Controller is NULL!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Handle not found in ActiveMovements!"));
	}
}

void UScheduleAction_MoveToLocation::ResumeActionInternal_Implementation(FScheduleActionHandle ActionHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("[MOVE] ResumeAction called for handle: %s"), *ActionHandle.HandleID.ToString());

	if (TWeakObjectPtr<AAIController>* ControllerPtr = ActiveMovements.Find(ActionHandle.HandleID))
	{
		if (AAIController* Controller = ControllerPtr->Get())
		{
			UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Resuming AI movement for: %s"), 
				Controller->GetPawn() ? *Controller->GetPawn()->GetName() : TEXT("NULL"));

			// ? RESTORE target from saved data
			FVector* SavedLocationPtr = PausedTargetLocations.Find(ActionHandle.HandleID);
			TWeakObjectPtr<AActor>* SavedActorPtr = PausedTargetActors.Find(ActionHandle.HandleID);

			AActor* TargetActor = SavedActorPtr ? SavedActorPtr->Get() : nullptr;
			FVector TargetLocation;

			// ? FIX: If target is an actor, use CURRENT actor position!
			if (TargetActor)
			{
				TargetLocation = TargetActor->GetActorLocation();
				UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Restored target ACTOR: %s at position: %s"), 
					*TargetActor->GetName(),
					*TargetLocation.ToString());
			}
			else if (SavedLocationPtr)
			{
				TargetLocation = *SavedLocationPtr;
				UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Restored target LOCATION: %s"), 
					*TargetLocation.ToString());

				// ? SAFETY CHECK: Don't try to move to (0,0,0)
				if (TargetLocation.IsNearlyZero())
				{
					UE_LOG(LogTemp, Error, TEXT("[MOVE]   ERROR: Cannot resume movement to (0,0,0)!"));
					UE_LOG(LogTemp, Error, TEXT("[MOVE]   This action was configured incorrectly in ScheduleData!"));
					UE_LOG(LogTemp, Error, TEXT("[MOVE]   Skipping resume for this action."));
					
					// Cleanup saved data
					PausedTargetLocations.Remove(ActionHandle.HandleID);
					PausedTargetActors.Remove(ActionHandle.HandleID);
					return;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[MOVE]   No saved target! Cannot resume movement."));
				return;
			}

			// ? RESTART movement to the target
			FAIMoveRequest MoveRequest;
			if (TargetActor)
			{
				MoveRequest.SetGoalActor(TargetActor);
			}
			else
			{
				MoveRequest.SetGoalLocation(TargetLocation);
			}

			MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
			MoveRequest.SetCanStrafe(bUsePathfinding);
			MoveRequest.SetAllowPartialPath(bAcceptPartialPath);
			MoveRequest.SetUsePathfinding(bUsePathfinding);

			if (FilterClass)
			{
				MoveRequest.SetNavigationFilter(FilterClass);
			}

			FPathFollowingRequestResult MoveResult = Controller->MoveTo(MoveRequest);

			if (MoveResult.Code == EPathFollowingRequestResult::RequestSuccessful || 
				MoveResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				UE_LOG(LogTemp, Warning, TEXT("[MOVE]   ? Movement resumed successfully to %s"), 
					TargetActor ? *TargetActor->GetName() : *TargetLocation.ToString());
				
				// ? Cleanup saved data
				PausedTargetLocations.Remove(ActionHandle.HandleID);
				PausedTargetActors.Remove(ActionHandle.HandleID);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[MOVE]   ? Failed to resume movement! Result: %d"), (int32)MoveResult.Code);
				UE_LOG(LogTemp, Error, TEXT("[MOVE]   Target: %s"), TargetActor ? *TargetActor->GetName() : *TargetLocation.ToString());
				UE_LOG(LogTemp, Error, TEXT("[MOVE]   Pawn: %s"), Controller->GetPawn() ? *Controller->GetPawn()->GetName() : TEXT("NULL"));
				UE_LOG(LogTemp, Error, TEXT("[MOVE]   Check: 1) NavMesh exists, 2) Target is reachable, 3) Pawn is on NavMesh"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Controller is NULL!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[MOVE]   Handle not found in ActiveMovements!"));
	}
}

bool UScheduleAction_MoveToLocation::IsActionActiveInternal_Implementation(FScheduleActionHandle ActionHandle) const
{
	if (const TWeakObjectPtr<AAIController>* ControllerPtr = ActiveMovements.Find(ActionHandle.HandleID))
	{
		if (AAIController* Controller = ControllerPtr->Get())
		{
			UPathFollowingComponent* PathFollowing = Controller->GetPathFollowingComponent();
			return PathFollowing && PathFollowing->GetStatus() != EPathFollowingStatus::Idle;
		}
	}

	return false;
}

FText UScheduleAction_MoveToLocation::GetActionDescription_Implementation() const
{
	if (!ActionName.IsEmpty())
	{
		return Super::GetActionDescription_Implementation();
	}

	return NSLOCTEXT("ScheduleSystem", "MoveToLocationAction", "Move To Location");
}
