// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/RelationshipNPCListWidget.h"
#include "Widgets/RelationshipNPCEntryWidget.h"
#include "Widgets/RelationshipPanel.h"
#include "Components/ScrollBox.h"
#include "Subsystems/RelationshipSubsystem.h"
#include "DataAssets/RelationshipDatabase.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Conditional include for GameEventBus
#if __has_include("GameEventBusSubsystem.h")
	#include "GameEventBusSubsystem.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

void URelationshipNPCListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SubscribeToEvents();
	PopulateList();
}

void URelationshipNPCListWidget::NativeDestruct()
{
	UnsubscribeFromEvents();

	Super::NativeDestruct();
}

void URelationshipNPCListWidget::PopulateList()
{
	ClearList();

	if (!NPCContainer || !NPCEntryClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("RelationshipNPCListWidget: Missing required widgets or classes"));
		return;
	}

	// Get player actor
	AActor* Player = GetPlayerActor();
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("RelationshipNPCListWidget: No player actor found"));
		return;
	}

	// Get RelationshipSubsystem
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("RelationshipNPCListWidget: RelationshipSubsystem not found"));
		return;
	}

	// Get all relationships where player is subject
	TArray<FRelationshipData> AllRelationships = Subsystem->GetRelationshipsAsSubject(Player);

	// Filter relationships
	TArray<FRelationshipData> FilteredRelationships;
	for (const FRelationshipData& RelData : AllRelationships)
	{
		if (PassesFilters(RelData))
		{
			FilteredRelationships.Add(RelData);
		}
	}

	// Sort if needed
	if (SortByDimension.IsValid())
	{
		SortRelationshipData(FilteredRelationships);
	}

	// Create entry widget for each relationship
	for (const FRelationshipData& RelData : FilteredRelationships)
	{
		AActor* NPC = RelData.Target.Get();
		if (!NPC)
		{
			continue;
		}

		URelationshipNPCEntryWidget* Entry = CreateWidget<URelationshipNPCEntryWidget>(this, NPCEntryClass);
		if (Entry)
		{
			Entry->SetNPCActor(NPC);
			Entry->SetPlayerActor(Player);
			Entry->OnNPCClicked.AddDynamic(this, &URelationshipNPCListWidget::OnNPCSelected);

			NPCContainer->AddChild(Entry);
			SpawnedEntries.Add(Entry);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("RelationshipNPCListWidget: Populated with %d NPCs"), SpawnedEntries.Num());
}

void URelationshipNPCListWidget::ClearList()
{
	if (NPCContainer)
	{
		NPCContainer->ClearChildren();
	}

	SpawnedEntries.Empty();
}

void URelationshipNPCListWidget::ApplyFilters()
{
	PopulateList(); // Re-populate with current filters
}

void URelationshipNPCListWidget::OnNPCSelected(AActor* NPC)
{
	if (!NPC)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("RelationshipNPCListWidget: NPC selected: %s"), *NPC->GetName());

	// Broadcast event
	OnNPCSelectedEvent.Broadcast(NPC);

	// Show detail panel
	if (DetailPanelClass)
	{
		// Remove old panel
		if (CurrentDetailPanel)
		{
			CurrentDetailPanel->RemoveFromParent();
			CurrentDetailPanel = nullptr;
		}

		// Create new panel
		CurrentDetailPanel = CreateWidget<URelationshipPanel>(this, DetailPanelClass);
		if (CurrentDetailPanel)
		{
			AActor* Player = GetPlayerActor();
			CurrentDetailPanel->SetActors(Player, NPC);
			CurrentDetailPanel->bAutoHideWhenNoDialogue = false; // Manual control
			CurrentDetailPanel->RefreshDisplay();
			CurrentDetailPanel->AddToViewport();
		}
	}
}

AActor* URelationshipNPCListWidget::GetPlayerActor() const
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		return PC->GetPawn();
	}
	return nullptr;
}

void URelationshipNPCListWidget::SubscribeToEvents()
{
#if GAMEEVENTBUS_AVAILABLE
	if (bAutoRefresh)
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();
			if (EventBus)
			{
				FGameplayTag RelationshipChangedTag = FGameplayTag::RequestGameplayTag(FName("Relationship.DimensionChanged"), false);
				if (RelationshipChangedTag.IsValid())
				{
					RelationshipChangedHandle = EventBus->SubscribeToEventNative(
						RelationshipChangedTag,
						FGameEventNativeDelegate::FDelegate::CreateUObject(this, &URelationshipNPCListWidget::OnRelationshipChanged)
					);

					UE_LOG(LogTemp, Log, TEXT("? RelationshipNPCListWidget: Subscribed to relationship change events"));
				}
			}
		}
	}
#endif
}

void URelationshipNPCListWidget::UnsubscribeFromEvents()
{
#if GAMEEVENTBUS_AVAILABLE
	if (RelationshipChangedHandle.IsValid())
	{
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();
			if (EventBus)
			{
				FGameplayTag RelationshipChangedTag = FGameplayTag::RequestGameplayTag(FName("Relationship.DimensionChanged"), false);
				if (RelationshipChangedTag.IsValid())
				{
					EventBus->UnsubscribeNative(RelationshipChangedTag, RelationshipChangedHandle);
				}
				RelationshipChangedHandle.Reset();
			}
		}
	}
#endif
}

void URelationshipNPCListWidget::OnRelationshipChanged(const FGameEventPayload& Payload)
{
	UE_LOG(LogTemp, Verbose, TEXT("RelationshipNPCListWidget: Relationship changed, refreshing list..."));
	PopulateList();
}

bool URelationshipNPCListWidget::PassesFilters(const FRelationshipData& RelData) const
{
	// Filter by state
	if (FilterByState.IsValid())
	{
		if (RelData.State.CurrentState != FilterByState)
		{
			return false;
		}
	}

	// Filter by dimension value
	if (FilterDimension.IsValid())
	{
		float DimValue = RelData.GetDimensionValue(FilterDimension);
		if (DimValue < MinDimensionValue)
		{
			return false;
		}
	}

	return true;
}

void URelationshipNPCListWidget::SortRelationshipData(TArray<FRelationshipData>& RelationshipList) const
{
	if (!SortByDimension.IsValid())
	{
		return;
	}

	RelationshipList.Sort([this](const FRelationshipData& A, const FRelationshipData& B)
	{
		float ValueA = A.GetDimensionValue(SortByDimension);
		float ValueB = B.GetDimensionValue(SortByDimension);

		return bSortDescending ? (ValueA > ValueB) : (ValueA < ValueB);
	});
}
