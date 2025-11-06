// Copyright Epic Games, Inc. All Rights Reserved.

#include "Subsystems/DialogueLocationSubsystem.h"
#include "Actors/DialogueLocationMarker.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

void UDialogueLocationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("DialogueLocationSubsystem: Initialized"));

	// Build initial marker cache
	BuildMarkerCache();
}

void UDialogueLocationSubsystem::Deinitialize()
{
	ClearMarkerCache();

	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("DialogueLocationSubsystem: Deinitialized"));
}

ADialogueLocationMarker* UDialogueLocationSubsystem::FindMarkerByTag(FGameplayTag MarkerTag) const
{
	if (!MarkerTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("FindMarkerByTag: Invalid tag"));
		return nullptr;
	}

	// Try cache first
	if (bMarkerCacheValid)
	{
		if (const TWeakObjectPtr<ADialogueLocationMarker>* Found = MarkerCacheByTag.Find(MarkerTag))
		{
			if (Found->IsValid())
			{
				return Found->Get();
			}
		}
	}

	// Fallback: scan world if cache invalid
	UE_LOG(LogTemp, Warning, TEXT("FindMarkerByTag: Cache invalid, scanning world for tag '%s'"), *MarkerTag.ToString());

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<ADialogueLocationMarker> It(World); It; ++It)
	{
		ADialogueLocationMarker* Marker = *It;
		if (Marker && Marker->LocationTag == MarkerTag)
		{
			return Marker;
		}
	}

	return nullptr;
}

TArray<ADialogueLocationMarker*> UDialogueLocationSubsystem::FindMarkersWithTag(FGameplayTag MarkerTag, bool bExactMatch) const
{
	TArray<ADialogueLocationMarker*> Result;

	if (!MarkerTag.IsValid())
	{
		return Result;
	}

	// Scan all cached markers
	for (const TWeakObjectPtr<ADialogueLocationMarker>& WeakMarker : AllMarkers)
	{
		if (!WeakMarker.IsValid())
		{
			continue;
		}

		ADialogueLocationMarker* Marker = WeakMarker.Get();
		if (!Marker || !Marker->LocationTag.IsValid())
		{
			continue;
		}

		// Check tag match
		bool bMatch = false;
		if (bExactMatch)
		{
			bMatch = (Marker->LocationTag == MarkerTag);
		}
		else
		{
			bMatch = Marker->LocationTag.MatchesTag(MarkerTag);
		}

		if (bMatch)
		{
			Result.Add(Marker);
		}
	}

	return Result;
}

TArray<ADialogueLocationMarker*> UDialogueLocationSubsystem::GetAllMarkers() const
{
	TArray<ADialogueLocationMarker*> Result;

	for (const TWeakObjectPtr<ADialogueLocationMarker>& WeakMarker : AllMarkers)
	{
		if (WeakMarker.IsValid())
		{
			Result.Add(WeakMarker.Get());
		}
	}

	return Result;
}

void UDialogueLocationSubsystem::RebuildMarkerCache()
{
	ClearMarkerCache();
	BuildMarkerCache();

	UE_LOG(LogTemp, Log, TEXT("RebuildMarkerCache: Cache rebuilt, %d markers registered"), AllMarkers.Num());
}

void UDialogueLocationSubsystem::DebugDrawAllMarkers(float Duration, float Thickness) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (const TWeakObjectPtr<ADialogueLocationMarker>& WeakMarker : AllMarkers)
	{
		if (!WeakMarker.IsValid())
		{
			continue;
		}

		ADialogueLocationMarker* Marker = WeakMarker.Get();
		if (!Marker)
		{
			continue;
		}

		const FVector Location = Marker->GetActorLocation();
		const FRotator Rotation = Marker->GetActorRotation();
		const FVector Forward = Rotation.Vector();

		// Draw sphere
		DrawDebugSphere(World, Location, Marker->DebugRadius, 16, Marker->DebugColor, false, Duration, 0, Thickness);

		// Draw arrow
		DrawDebugDirectionalArrow(
			World,
			Location,
			Location + Forward * 100.0f,
			50.0f,
			Marker->DebugColor,
			false,
			Duration,
			0,
			Thickness
		);

		// Draw tag name
		if (Marker->LocationTag.IsValid())
		{
			DrawDebugString(
				World,
				Location + FVector(0, 0, Marker->DebugRadius + 20.0f),
				Marker->LocationTag.ToString(),
				nullptr,
				Marker->DebugColor,
				Duration,
				true
			);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("DebugDrawAllMarkers: Drew %d markers"), AllMarkers.Num());
}

void UDialogueLocationSubsystem::DebugPrintMarkerInfo() const
{
	UE_LOG(LogTemp, Log, TEXT("=== Dialogue Location Markers ==="));
	UE_LOG(LogTemp, Log, TEXT("Total markers: %d"), AllMarkers.Num());
	UE_LOG(LogTemp, Log, TEXT("Cache valid: %s"), bMarkerCacheValid ? TEXT("YES") : TEXT("NO"));

	int32 Index = 0;
	for (const TWeakObjectPtr<ADialogueLocationMarker>& WeakMarker : AllMarkers)
	{
		if (!WeakMarker.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("  [%d] INVALID"), Index);
			Index++;
			continue;
		}

		ADialogueLocationMarker* Marker = WeakMarker.Get();
		if (!Marker)
		{
			UE_LOG(LogTemp, Log, TEXT("  [%d] NULL"), Index);
			Index++;
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("  [%d] %s"), Index, *Marker->GetName());
		UE_LOG(LogTemp, Log, TEXT("      Tag: %s"), *Marker->LocationTag.ToString());
		UE_LOG(LogTemp, Log, TEXT("      Location: %s"), *Marker->GetActorLocation().ToString());
		UE_LOG(LogTemp, Log, TEXT("      Rotation: %s"), *Marker->GetActorRotation().ToString());

		Index++;
	}

	UE_LOG(LogTemp, Log, TEXT("================================"));
}

void UDialogueLocationSubsystem::BuildMarkerCache()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	MarkerCacheByTag.Empty();
	AllMarkers.Empty();

	// Find all location markers in world
	for (TActorIterator<ADialogueLocationMarker> It(World); It; ++It)
	{
		ADialogueLocationMarker* Marker = *It;
		if (!Marker)
		{
			continue;
		}

		// Add to all markers list
		AllMarkers.Add(Marker);

		// Add to tag cache if tag is valid
		if (Marker->LocationTag.IsValid())
		{
			MarkerCacheByTag.Add(Marker->LocationTag, Marker);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BuildMarkerCache: Marker '%s' has no tag set"), *Marker->GetName());
		}
	}

	bMarkerCacheValid = true;

	UE_LOG(LogTemp, Log, TEXT("BuildMarkerCache: Found %d markers"), AllMarkers.Num());
}

void UDialogueLocationSubsystem::ClearMarkerCache()
{
	MarkerCacheByTag.Empty();
	AllMarkers.Empty();
	bMarkerCacheValid = false;
}
