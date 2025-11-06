// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/NPCRelationshipComponent.h"
#include "Subsystems/RelationshipSubsystem.h"
#include "DataAssets/RelationshipProfile.h"
#include "RelationshipSystemCore.h"
#include "Engine/World.h"
#include "EngineUtils.h"

UNPCRelationshipComponent::UNPCRelationshipComponent()
	: bAutoCreateRelationships(false)
	, AutoCreateRadius(1000.0f)
{
	PrimaryComponentTick.bCanEverTick = false; // Event-driven, no tick
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UNPCRelationshipComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache subsystem
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			CachedSubsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
		}
	}

	if (!CachedSubsystem)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("RelationshipSubsystem not found for %s"), *GetOwner()->GetName());
		return;
	}

	// Load profile
	if (Profile.IsValid())
	{
		CachedProfile = Profile.LoadSynchronous();
	}

	if (!CachedProfile)
	{
		UE_LOG(LogRelationshipSystem, Warning, TEXT("No profile assigned to %s, using default"), *GetOwner()->GetName());
		// Will use default profile from database
	}

	// Register with subsystem
	CachedSubsystem->RegisterComponent(this);

	// Initialize relationships
	InitializeRelationships();
}

void UNPCRelationshipComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from subsystem
	if (CachedSubsystem)
	{
		CachedSubsystem->UnregisterComponent(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UNPCRelationshipComponent::InitializeRelationships()
{
	if (bAutoCreateRelationships)
	{
		AutoCreateNearbyRelationships();
	}
}

void UNPCRelationshipComponent::AutoCreateNearbyRelationships()
{
	if (!CachedSubsystem)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Find all actors with RelationshipComponent in radius
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* OtherActor = *It;
		if (!OtherActor || OtherActor == Owner)
		{
			continue;
		}

		// Check distance
		float Distance = FVector::Dist(Owner->GetActorLocation(), OtherActor->GetActorLocation());
		if (Distance > AutoCreateRadius)
		{
			continue;
		}

		// Check if has RelationshipComponent
		UNPCRelationshipComponent* OtherComponent = OtherActor->FindComponentByClass<UNPCRelationshipComponent>();
		if (!OtherComponent)
		{
			continue;
		}

		// Check filter tags
		if (AutoCreateFilterTags.Num() > 0)
		{
			bool bMatchesFilter = false;
			for (const FName& Tag : AutoCreateFilterTags)
			{
				if (OtherActor->Tags.Contains(Tag))
				{
					bMatchesFilter = true;
					break;
				}
			}
			if (!bMatchesFilter)
			{
				continue;
			}
		}

		// Create relationship
		CreateRelationshipWith(OtherActor);
	}
}

// Relationship Management

bool UNPCRelationshipComponent::CreateRelationshipWith(AActor* TargetActor)
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	// Get target's profile
	URelationshipProfile* TargetProfile = nullptr;
	if (UNPCRelationshipComponent* TargetComponent = TargetActor->FindComponentByClass<UNPCRelationshipComponent>())
	{
		TargetProfile = TargetComponent->GetProfile();
	}

	return CachedSubsystem->CreateRelationship(Owner, TargetActor, CachedProfile, TargetProfile);
}

bool UNPCRelationshipComponent::RemoveRelationshipWith(AActor* TargetActor)
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->RemoveRelationship(Owner, TargetActor);
}

bool UNPCRelationshipComponent::HasRelationshipWith(AActor* TargetActor) const
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->HasRelationship(Owner, TargetActor);
}

// Dimension Operations

float UNPCRelationshipComponent::GetDimensionValue(AActor* TargetActor, FGameplayTag DimensionTag) const
{
	if (!CachedSubsystem || !TargetActor)
	{
		return 0.0f;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return 0.0f;
	}

	return CachedSubsystem->GetDimensionValue(Owner, TargetActor, DimensionTag);
}

bool UNPCRelationshipComponent::SetDimensionValue(AActor* TargetActor, FGameplayTag DimensionTag, float NewValue)
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->SetDimensionValue(Owner, TargetActor, DimensionTag, NewValue);
}

bool UNPCRelationshipComponent::ModifyDimensionValue(AActor* TargetActor, FGameplayTag DimensionTag, float Delta)
{
	UE_LOG(LogRelationshipSystem, Warning, TEXT("============================================"));
	UE_LOG(LogRelationshipSystem, Warning, TEXT("NPCRelationshipComponent::ModifyDimensionValue CALLED"));
	UE_LOG(LogRelationshipSystem, Warning, TEXT("  Owner: %s"), *GetNameSafe(GetOwner()));
	UE_LOG(LogRelationshipSystem, Warning, TEXT("  TargetActor: %s"), *GetNameSafe(TargetActor));
	UE_LOG(LogRelationshipSystem, Warning, TEXT("  DimensionTag: %s"), *DimensionTag.ToString());
	UE_LOG(LogRelationshipSystem, Warning, TEXT("  Delta: %.2f"), Delta);
	UE_LOG(LogRelationshipSystem, Warning, TEXT("  CachedSubsystem: %s"), CachedSubsystem ? TEXT("Valid") : TEXT("NULL"));

	if (!CachedSubsystem || !TargetActor)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("? ABORTED: CachedSubsystem=%s, TargetActor=%s"), 
			CachedSubsystem ? TEXT("Valid") : TEXT("NULL"),
			TargetActor ? TEXT("Valid") : TEXT("NULL"));
		UE_LOG(LogRelationshipSystem, Warning, TEXT("============================================"));
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("? ABORTED: Owner is NULL!"));
		UE_LOG(LogRelationshipSystem, Warning, TEXT("============================================"));
		return false;
	}

	UE_LOG(LogRelationshipSystem, Warning, TEXT("? Calling Subsystem->ModifyDimensionValue..."));
	bool bResult = CachedSubsystem->ModifyDimensionValue(Owner, TargetActor, DimensionTag, Delta);
	UE_LOG(LogRelationshipSystem, Warning, TEXT("? Subsystem->ModifyDimensionValue returned: %s"), bResult ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogRelationshipSystem, Warning, TEXT("============================================"));
	
	return bResult;
}

bool UNPCRelationshipComponent::ModifyDimensionValues(AActor* TargetActor, const TMap<FGameplayTag, float>& DimensionDeltas)
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->ModifyDimensionValues(Owner, TargetActor, DimensionDeltas);
}

// State Operations

FGameplayTag UNPCRelationshipComponent::GetCurrentState(AActor* TargetActor) const
{
	if (!CachedSubsystem || !TargetActor)
	{
		return FGameplayTag();
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FGameplayTag();
	}

	return CachedSubsystem->GetCurrentState(Owner, TargetActor);
}

bool UNPCRelationshipComponent::ForceStateChange(AActor* TargetActor, FGameplayTag NewState)
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->ForceStateChange(Owner, TargetActor, NewState);
}

// Trait Operations

bool UNPCRelationshipComponent::AddTrait(AActor* TargetActor, FGameplayTag TraitTag)
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->AddTrait(Owner, TargetActor, TraitTag);
}

bool UNPCRelationshipComponent::RemoveTrait(AActor* TargetActor, FGameplayTag TraitTag)
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->RemoveTrait(Owner, TargetActor, TraitTag);
}

bool UNPCRelationshipComponent::HasTrait(AActor* TargetActor, FGameplayTag TraitTag) const
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->HasTrait(Owner, TargetActor, TraitTag);
}

// Action Execution

bool UNPCRelationshipComponent::CanExecuteAction(AActor* TargetActor, FGameplayTag ActionTag) const
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->CanExecuteAction(Owner, TargetActor, ActionTag);
}

bool UNPCRelationshipComponent::ExecuteAction(AActor* TargetActor, FGameplayTag ActionTag)
{
	if (!CachedSubsystem || !TargetActor)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	return CachedSubsystem->ExecuteAction(Owner, TargetActor, ActionTag);
}

TArray<FGameplayTag> UNPCRelationshipComponent::GetAvailableActions(AActor* TargetActor) const
{
	if (!CachedSubsystem || !TargetActor)
	{
		return TArray<FGameplayTag>();
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return TArray<FGameplayTag>();
	}

	return CachedSubsystem->GetAvailableActions(Owner, TargetActor);
}

// Queries

TArray<AActor*> UNPCRelationshipComponent::GetAllRelationshipTargets() const
{
	TArray<AActor*> Result;

	if (!CachedSubsystem)
	{
		return Result;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return Result;
	}

	TArray<FRelationshipData> Relationships = CachedSubsystem->GetRelationshipsAsSubject(Owner);
	for (const FRelationshipData& RelData : Relationships)
	{
		if (AActor* Target = RelData.Target.Get())
		{
			Result.Add(Target);
		}
	}

	return Result;
}

TArray<AActor*> UNPCRelationshipComponent::GetAllRelationshipSubjects() const
{
	TArray<AActor*> Result;

	if (!CachedSubsystem)
	{
		return Result;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return Result;
	}

	TArray<FRelationshipData> Relationships = CachedSubsystem->GetRelationshipsAsTarget(Owner);
	for (const FRelationshipData& RelData : Relationships)
	{
		if (AActor* Subject = RelData.Subject.Get())
		{
			Result.Add(Subject);
		}
	}

	return Result;
}

TArray<AActor*> UNPCRelationshipComponent::FindTargetsByState(FGameplayTag StateTag) const
{
	TArray<AActor*> Result;

	if (!CachedSubsystem)
	{
		return Result;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return Result;
	}

	TArray<FRelationshipData> Relationships = CachedSubsystem->FindRelationshipsByState(Owner, StateTag);
	for (const FRelationshipData& RelData : Relationships)
	{
		if (AActor* Target = RelData.Target.Get())
		{
			Result.Add(Target);
		}
	}

	return Result;
}

TArray<AActor*> UNPCRelationshipComponent::FindTargetsByDimensionRange(FGameplayTag DimensionTag, float MinValue, float MaxValue) const
{
	TArray<AActor*> Result;

	if (!CachedSubsystem)
	{
		return Result;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return Result;
	}

	TArray<FRelationshipData> Relationships = CachedSubsystem->FindRelationshipsByDimensionRange(Owner, DimensionTag, MinValue, MaxValue);
	for (const FRelationshipData& RelData : Relationships)
	{
		if (AActor* Target = RelData.Target.Get())
		{
			Result.Add(Target);
		}
	}

	return Result;
}

AActor* UNPCRelationshipComponent::GetBestTargetByDimension(FGameplayTag DimensionTag) const
{
	if (!CachedSubsystem)
	{
		return nullptr;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	TArray<FRelationshipData> Relationships = CachedSubsystem->GetRelationshipsAsSubject(Owner);
	
	AActor* BestTarget = nullptr;
	float BestValue = -FLT_MAX;

	for (const FRelationshipData& RelData : Relationships)
	{
		float Value = RelData.GetDimensionValue(DimensionTag);
		if (Value > BestValue)
		{
			BestValue = Value;
			BestTarget = RelData.Target.Get();
		}
	}

	return BestTarget;
}

AActor* UNPCRelationshipComponent::GetWorstTargetByDimension(FGameplayTag DimensionTag) const
{
	if (!CachedSubsystem)
	{
		return nullptr;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	TArray<FRelationshipData> Relationships = CachedSubsystem->GetRelationshipsAsSubject(Owner);
	
	AActor* WorstTarget = nullptr;
	float WorstValue = FLT_MAX;

	for (const FRelationshipData& RelData : Relationships)
	{
		float Value = RelData.GetDimensionValue(DimensionTag);
		if (Value < WorstValue)
		{
			WorstValue = Value;
			WorstTarget = RelData.Target.Get();
		}
	}

	return WorstTarget;
}

// Utilities

URelationshipProfile* UNPCRelationshipComponent::GetProfile() const
{
	return CachedProfile;
}

URelationshipSubsystem* UNPCRelationshipComponent::GetRelationshipSubsystem() const
{
	return CachedSubsystem;
}

bool UNPCRelationshipComponent::CanBeSubject() const
{
	if (CachedProfile)
	{
		return CachedProfile->bCanBeSubject;
	}
	return true; // Default: can be subject
}

bool UNPCRelationshipComponent::CanBeTarget() const
{
	if (CachedProfile)
	{
		return CachedProfile->bCanBeTarget;
	}
	return true; // Default: can be target
}
