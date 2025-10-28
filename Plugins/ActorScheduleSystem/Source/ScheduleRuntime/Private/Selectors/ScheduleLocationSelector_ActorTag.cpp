// Copyright Epic Games, Inc. All Rights Reserved.

#include "Selectors/ScheduleLocationSelector_ActorTag.h"
#include "EngineUtils.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"

TArray<AActor*> UScheduleLocationSelector_ActorTag::FindMatchingActors(const FScheduleLocationContext& Context) const
{
	TArray<AActor*> MatchingActors;

	if (!Context.World)
	{
		UE_LOG(LogTemp, Warning, TEXT("   ? FindMatchingActors: No World in context!"));
		return MatchingActors;
	}

	// Iterate all actors of specified class
	TSubclassOf<AActor> SearchClass = ActorClassFilter.Get() ? ActorClassFilter : TSubclassOf<AActor>(AActor::StaticClass());
	
	UE_LOG(LogTemp, Log, TEXT("   ?? Searching for actors of class: %s"), 
		SearchClass.Get() ? *SearchClass->GetName() : TEXT("AActor"));

	int32 TotalActorsChecked = 0;
	int32 ActorsWithTags = 0;
	
	for (TActorIterator<AActor> It(Context.World, SearchClass); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		TotalActorsChecked++;

		// Check if actor implements gameplay tag interface
		IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Actor);
		if (!TagInterface)
		{
			continue;
		}

		ActorsWithTags++;

		FGameplayTagContainer ActorTags;
		TagInterface->GetOwnedGameplayTags(ActorTags);

		// Check required tags
		if (!Context.RequiredTags.IsEmpty() && !ActorTags.HasAll(Context.RequiredTags))
		{
			UE_LOG(LogTemp, VeryVerbose, TEXT(" Actor '%s' doesn't have required tags. Has: %s"), 
				*Actor->GetName(), *ActorTags.ToString());
			continue;
		}

		// Check distance if specified
		if (Context.MaxDistance > 0.0f && Context.RequestingActor.IsValid())
		{
			float Distance = FVector::Dist(Actor->GetActorLocation(), Context.RequestingActor->GetActorLocation());
			if (Distance > Context.MaxDistance)
			{
				UE_LOG(LogTemp, VeryVerbose, TEXT("      Actor '%s' is too far: %.2f > %.2f"), 
					*Actor->GetName(), Distance, Context.MaxDistance);
				continue;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("      ? Found matching actor: '%s' with tags: %s"), 
			*Actor->GetName(), *ActorTags.ToString());
		MatchingActors.Add(Actor);
	}

	UE_LOG(LogTemp, Log, TEXT("   ?? Search results: Checked %d actors, %d with tag interface, %d matched criteria"), 
		TotalActorsChecked, ActorsWithTags, MatchingActors.Num());

	return MatchingActors;
}

bool UScheduleLocationSelector_ActorTag::SelectLocationInternal_Implementation(const FScheduleLocationContext& Context, FScheduleLocation& OutLocation)
{
	UE_LOG(LogTemp, Log, TEXT("   ?? ActorTag Selector: Starting search..."));
	UE_LOG(LogTemp, Log, TEXT("      RequiredTags: %s"), *Context.RequiredTags.ToString());
	UE_LOG(LogTemp, Log, TEXT("    MaxDistance: %.2f"), Context.MaxDistance);
	
	TArray<AActor*> MatchingActors = FindMatchingActors(Context);

	UE_LOG(LogTemp, Log, TEXT("  Found %d matching actors"), MatchingActors.Num());

	if (MatchingActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("   ? ActorTag Selector: No matching actors found!"));
		return false;
	}

	AActor* SelectedActor = nullptr;

	switch (SelectionMode)
	{
	case ELocationSelectionMode::Closest:
		if (Context.RequestingActor.IsValid())
		{
			float MinDistance = FLT_MAX;
			FVector RequestLocation = Context.RequestingActor->GetActorLocation();
			
			for (AActor* Actor : MatchingActors)
			{
				float Distance = FVector::Dist(Actor->GetActorLocation(), RequestLocation);
				if (Distance < MinDistance)
				{
					MinDistance = Distance;
					SelectedActor = Actor;
				}
			}
			UE_LOG(LogTemp, Log, TEXT("      Selected closest actor at distance: %.2f"), MinDistance);
		}
		else
		{
			SelectedActor = MatchingActors[0];
			UE_LOG(LogTemp, Log, TEXT("      Selected first actor (no RequestingActor)"));
		}
		break;

	case ELocationSelectionMode::Farthest:
		if (Context.RequestingActor.IsValid())
		{
			float MaxDistance = 0.0f;
			FVector RequestLocation = Context.RequestingActor->GetActorLocation();
			
			for (AActor* Actor : MatchingActors)
			{
				float Distance = FVector::Dist(Actor->GetActorLocation(), RequestLocation);
				if (Distance > MaxDistance)
				{
					MaxDistance = Distance;
					SelectedActor = Actor;
				}
			}
			UE_LOG(LogTemp, Log, TEXT("      Selected farthest actor at distance: %.2f"), MaxDistance);
		}
		else
		{
			SelectedActor = MatchingActors.Last();
			UE_LOG(LogTemp, Log, TEXT("   Selected last actor (no RequestingActor)"));
		}
		break;

	case ELocationSelectionMode::Random:
		{
			FRandomStream RandomStream(RandomSeed != 0 ? RandomSeed : FMath::Rand());
			int32 RandomIndex = RandomStream.RandRange(0, MatchingActors.Num() - 1);
			SelectedActor = MatchingActors[RandomIndex];
			UE_LOG(LogTemp, Log, TEXT(" Selected random actor at index: %d"), RandomIndex);
		}
		break;

	case ELocationSelectionMode::First:
	default:
		SelectedActor = MatchingActors[0];
		UE_LOG(LogTemp, Log, TEXT("      Selected first actor"));
		break;
	}

	if (SelectedActor)
	{
		OutLocation.Location = SelectedActor->GetActorLocation();
		OutLocation.Rotation = SelectedActor->GetActorRotation();
		OutLocation.ReferenceActor = SelectedActor;

		// Get tags from actor
		if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(SelectedActor))
		{
			TagInterface->GetOwnedGameplayTags(OutLocation.LocationTags);
		}

		UE_LOG(LogTemp, Log, TEXT("   ? ActorTag Selector: Selected actor '%s' at %s"), 
			*SelectedActor->GetName(), 
			*OutLocation.Location.ToString());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("   ? ActorTag Selector: Failed to select actor!"));
	return false;
}

void UScheduleLocationSelector_ActorTag::GetPotentialLocationsInternal_Implementation(const FScheduleLocationContext& Context, int32 MaxLocations, TArray<FScheduleLocation>& OutLocations)
{
	TArray<AActor*> MatchingActors = FindMatchingActors(Context);

	int32 Count = FMath::Min(MaxLocations, MatchingActors.Num());
	
	for (int32 i = 0; i < Count; ++i)
	{
		FScheduleLocation Location;
		Location.Location = MatchingActors[i]->GetActorLocation();
		Location.Rotation = MatchingActors[i]->GetActorRotation();
		Location.ReferenceActor = MatchingActors[i];

		if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(MatchingActors[i]))
		{
			TagInterface->GetOwnedGameplayTags(Location.LocationTags);
		}

		OutLocations.Add(Location);
	}
}
