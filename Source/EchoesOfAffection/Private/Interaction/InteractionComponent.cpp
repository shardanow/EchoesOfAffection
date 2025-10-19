// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/InteractionComponent.h"
#include "Interaction/InteractableComponent.h"  // ? Add include for UInteractableComponent
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// ========== Cached Interactable Info ==========
/** Struct to cache interactable information for performance */
struct FCachedInteractable
{
	TWeakObjectPtr<AActor> Actor;
	IInteractableInterface* Interface = nullptr;
	UInteractableComponent* Component = nullptr;
	
	FCachedInteractable() = default;
	
	FCachedInteractable(AActor* InActor)
		: Actor(InActor)
	{
		if (InActor)
		{
			Interface = Cast<IInteractableInterface>(InActor);
			if (!Interface)
			{
				Component = InActor->FindComponentByClass<UInteractableComponent>();
				if (Component)
				{
					Interface = Cast<IInteractableInterface>(Component);
				}
			}
		}
	}
	
	bool IsValid() const
	{
		return Actor.IsValid() && Interface != nullptr;
	}
	
	UObject* GetInterfaceObject() const
	{
		return Component ? Cast<UObject>(Component) : Cast<UObject>(Actor.Get());
	}
};

// ========== Constructor ==========

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// Default settings
	bInteractionEnabled = true;
	TraceDistance = 500.0f;
	TraceRadius = 10.0f;
	TraceTickInterval = 0.1f;
	bDrawDebugTrace = false;
	InteractionTraceChannel = ECC_Visibility;

	// Initialize state
	FocusedActor = nullptr;
	InteractingActor = nullptr;
	bIsInteracting = false;
	CurrentInteractionTime = 0.0f;
	TraceTimer = 0.0f;
	InteractionDuration = 0.0f;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bIsInteracting)
	{
		CancelInteraction();
	}

	Super::EndPlay(EndPlayReason);
}

// ========== Tick ==========

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bInteractionEnabled)
	{
		return;
	}

	// Handle ongoing interaction
	if (bIsInteracting && InteractionDuration > 0.0f)
	{
		CurrentInteractionTime += DeltaTime;

		if (CurrentInteractionTime >= InteractionDuration)
		{
			CompleteInteraction();
		}
	}

	// Update interactable detection
	TraceTimer += DeltaTime;
	if (TraceTimer >= TraceTickInterval)
	{
		TraceTimer = 0.0f;
		FindInteractables();
	}
}

// ========== Interaction Detection ==========

void UInteractionComponent::FindInteractables()
{
	FVector Start, End, Direction;
	if (!GetTraceStartAndEnd(Start, End, Direction))
	{
		UpdateFocusedInteractable(nullptr);
		return;
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	bool bHit = false;

	if (TraceRadius > 0.0f)
	{
		// Sphere trace
		bHit = GetWorld()->SweepSingleByChannel(
			HitResult,
			Start,
			End,
			FQuat::Identity,
			InteractionTraceChannel,
			FCollisionShape::MakeSphere(TraceRadius),
			QueryParams
		);
	}
	else
	{
		// Line trace
		bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			InteractionTraceChannel,
			QueryParams
		);
	}

	// Debug visualization
	if (bDrawDebugTrace)
	{
		if (TraceRadius > 0.0f)
		{
			DrawDebugSphere(GetWorld(), bHit ? HitResult.Location : End, TraceRadius, 12, bHit ? FColor::Green : FColor::Red, false, TraceTickInterval);
		}
		else
		{
			DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, TraceTickInterval, 0, 2.0f);
		}

		if (bHit)
		{
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Yellow, false, TraceTickInterval);
		}
	}

	// Check if hit actor is interactable
	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;
	
	if (HitActor && IsActorInteractable(HitActor))
	{
		// Create cached interactable info
		FCachedInteractable CachedInfo(HitActor);
		
		if (CachedInfo.IsValid())
		{
			// Check if can interact
			bool bCanInteract = CachedInfo.Interface->Execute_CanInteract(CachedInfo.GetInterfaceObject(), GetOwner());
			
			if (bCanInteract)
			{
				UpdateFocusedInteractable(HitActor);
				return;
			}
		}
	}

	UpdateFocusedInteractable(nullptr);
}

void UInteractionComponent::UpdateFocusedInteractable(AActor* NewFocusedActor)
{
	if (FocusedActor == NewFocusedActor)
	{
		return;
	}

	// ? FIXED: Unfocus previous actor with proper component handling
	if (FocusedActor)
	{
		FCachedInteractable OldCached(FocusedActor);
		
		if (OldCached.IsValid())
		{
			// ? CORRECT: Pass the interface object (component or actor)
			OldCached.Interface->Execute_OnInteractionUnfocused(OldCached.GetInterfaceObject(), GetOwner());
		}
		
		OnInteractableUnfocused.Broadcast(FocusedActor);
	}

	FocusedActor = NewFocusedActor;

	// ? FIXED: Focus new actor with proper component handling
	if (FocusedActor)
	{
		FCachedInteractable NewCached(FocusedActor);
		
		if (NewCached.IsValid())
		{
			// Get interaction data
			FocusedInteractionData = NewCached.Interface->Execute_GetInteractionData(NewCached.GetInterfaceObject(), GetOwner());
			
			// ? CORRECT: Pass the interface object (component or actor)
			NewCached.Interface->Execute_OnInteractionFocused(NewCached.GetInterfaceObject(), GetOwner());
		}
		
		OnInteractableFocused.Broadcast(FocusedActor, FocusedInteractionData);
	}
	else
	{
		FocusedInteractionData = FInteractionData();
	}
}

// ========== Interaction Actions ==========

EInteractionResult UInteractionComponent::TryInteract()
{
	if (!bInteractionEnabled)
	{
		return EInteractionResult::Failed;
	}

	if (bIsInteracting)
	{
		return EInteractionResult::AlreadyInteracting;
	}

	if (!FocusedActor)
	{
		return EInteractionResult::NotInteractable;
	}

	// ? FIXED: Use cached interactable info
	FCachedInteractable CachedInfo(FocusedActor);
	
	if (!CachedInfo.IsValid())
	{
		return EInteractionResult::NotInteractable;
	}

	// Get interaction data
	FInteractionData InteractionData = CachedInfo.Interface->Execute_GetInteractionData(CachedInfo.GetInterfaceObject(), GetOwner());
	
	// Validate interaction
	if (!ValidateInteraction(FocusedActor, InteractionData))
	{
		return EInteractionResult::Failed;
	}

	// Start interaction
	EInteractionResult Result = CachedInfo.Interface->Execute_BeginInteract(CachedInfo.GetInterfaceObject(), GetOwner());

	if (Result == EInteractionResult::Success)
	{
		InteractingActor = FocusedActor;
		bIsInteracting = true;
		CurrentInteractionTime = 0.0f;
		InteractionDuration = InteractionData.InteractionDuration;

		OnInteractionStarted.Broadcast(InteractingActor, InteractionData);

		// If instant interaction, complete immediately
		if (InteractionDuration <= 0.0f)
		{
			CompleteInteraction();
		}
	}

	return Result;
}

void UInteractionComponent::CompleteInteraction()
{
	if (!bIsInteracting || !InteractingActor)
	{
		return;
	}

	// ? FIXED: Use cached interactable info
	FCachedInteractable CachedInfo(InteractingActor);
	
	if (CachedInfo.IsValid())
	{
		CachedInfo.Interface->Execute_EndInteract(CachedInfo.GetInterfaceObject(), GetOwner());
	}

	OnInteractionCompleted.Broadcast(InteractingActor);

	AActor* CompletedActor = InteractingActor;
	InteractingActor = nullptr;
	bIsInteracting = false;
	CurrentInteractionTime = 0.0f;
	InteractionDuration = 0.0f;

	// Check if we should unfocus after single-use interaction
	if (FocusedActor == CompletedActor)
	{
		FCachedInteractable FocusedCached(FocusedActor);
		
		if (FocusedCached.IsValid())
		{
			FInteractionData Data = FocusedCached.Interface->Execute_GetInteractionData(FocusedCached.GetInterfaceObject(), GetOwner());
			
			if (!Data.bCanInteractMultipleTimes)
			{
				UpdateFocusedInteractable(nullptr);
			}
		}
	}
}

void UInteractionComponent::CancelInteraction()
{
	if (!bIsInteracting || !InteractingActor)
	{
		return;
	}

	// ? FIXED: Use cached interactable info
	FCachedInteractable CachedInfo(InteractingActor);
	
	if (CachedInfo.IsValid())
	{
		CachedInfo.Interface->Execute_CancelInteract(CachedInfo.GetInterfaceObject(), GetOwner());
	}

	OnInteractionCancelled.Broadcast(InteractingActor);

	InteractingActor = nullptr;
	bIsInteracting = false;
	CurrentInteractionTime = 0.0f;
	InteractionDuration = 0.0f;
}

float UInteractionComponent::GetInteractionProgress() const
{
	if (!bIsInteracting || InteractionDuration <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(CurrentInteractionTime / InteractionDuration, 0.0f, 1.0f);
}

// ========== Helper Methods ==========

bool UInteractionComponent::GetTraceStartAndEnd(FVector& OutStart, FVector& OutEnd, FVector& OutDirection)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	// Try to find camera component
	UCameraComponent* CameraComp = Owner->FindComponentByClass<UCameraComponent>();
	
	if (CameraComp)
	{
		OutStart = CameraComp->GetComponentLocation();
		OutDirection = CameraComp->GetForwardVector();
	}
	else if (APawn* Pawn = Cast<APawn>(Owner))
	{
		// Try to get controller's view point
		APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
		if (PC)
		{
			FRotator ViewRotation;
			PC->GetPlayerViewPoint(OutStart, ViewRotation);
			OutDirection = ViewRotation.Vector();
		}
		else
		{
			// Fallback to pawn's view
			OutStart = Pawn->GetPawnViewLocation();
			OutDirection = Pawn->GetViewRotation().Vector();
		}
	}
	else
	{
		// Fallback to actor forward
		OutStart = Owner->GetActorLocation();
		OutDirection = Owner->GetActorForwardVector();
	}

	OutEnd = OutStart + (OutDirection * TraceDistance);
	return true;
}

bool UInteractionComponent::IsActorInteractable(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	// Check if actor implements the interface directly
	if (Actor->Implements<UInteractableInterface>())
	{
		return true;
	}

	// Check if actor has an InteractableComponent
	UInteractableComponent* InteractableComp = Actor->FindComponentByClass<UInteractableComponent>();
	return InteractableComp != nullptr;
}

bool UInteractionComponent::ValidateInteraction(AActor* InteractableActor, const FInteractionData& InteractionData)
{
	if (!InteractableActor || !GetOwner())
	{
		return false;
	}

	// Check distance
	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), InteractableActor->GetActorLocation());
	if (Distance > InteractionData.MaxInteractionDistance)
	{
		return false;
	}

	// ? IMPROVED: Better line of sight check
	if (InteractionData.bRequiresLineOfSight)
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());
		QueryParams.AddIgnoredActor(InteractableActor);

		// ? Use bounds center instead of actor origin for better accuracy
		FVector Start = GetOwner()->GetActorLocation();
		FVector TargetLocation = InteractableActor->GetComponentsBoundingBox().GetCenter();

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, TargetLocation, ECC_Visibility, QueryParams))
		{
			// Something is blocking the view
			return false;
		}
	}

	return true;
}
