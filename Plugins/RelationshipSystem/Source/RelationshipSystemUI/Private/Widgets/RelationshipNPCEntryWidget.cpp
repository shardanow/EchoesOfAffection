// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/RelationshipNPCEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/NPCRelationshipComponent.h"
#include "Subsystems/RelationshipSubsystem.h"
#include "DataAssets/RelationshipDatabase.h"
#include "DataAssets/RelationshipState.h"
#include "DataAssets/RelationshipDimension.h"

void URelationshipNPCEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button click
	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &URelationshipNPCEntryWidget::OnButtonClicked);
	}

	RefreshDisplay();
}

void URelationshipNPCEntryWidget::SetNPCActor(AActor* InNPC)
{
	NPCActor = InNPC;
	RefreshDisplay();
}

void URelationshipNPCEntryWidget::SetPlayerActor(AActor* InPlayer)
{
	PlayerActor = InPlayer;
	RefreshDisplay();
}

void URelationshipNPCEntryWidget::RefreshDisplay()
{
	if (!NPCActor)
	{
		return;
	}

	// Update name
	if (NPCNameText)
	{
		NPCNameText->SetText(GetNPCName());
	}

	// Update state
	if (StateText)
	{
		StateText->SetText(GetStateDisplayName());
	}

	// Update preview dimension
	if (PreviewDimensionBar)
	{
		PreviewDimensionBar->SetPercent(GetPreviewDimensionNormalized());
	}

	if (PreviewDimensionText)
	{
		float Value = GetPreviewDimensionValue();
		FText ValueText = FText::Format(
			FText::FromString(TEXT("{0}/100")),
			FText::AsNumber(FMath::RoundToInt(Value))
		);
		PreviewDimensionText->SetText(ValueText);
	}

	// TODO: Set state icon based on current state
}

FText URelationshipNPCEntryWidget::GetNPCName() const
{
	if (!NPCActor)
	{
		return FText::FromString(TEXT("Unknown NPC"));
	}

	// Try to get friendly name from actor
	FString ActorName;
	
#if WITH_EDITOR
	ActorName = NPCActor->GetActorLabel();
#else
	ActorName = NPCActor->GetName();
#endif

	if (ActorName.IsEmpty())
	{
		ActorName = NPCActor->GetName();
	}

	return FText::FromString(ActorName);
}

FGameplayTag URelationshipNPCEntryWidget::GetCurrentState() const
{
	if (!NPCActor || !PlayerActor)
	{
		return FGameplayTag();
	}

	// Get relationship component from player
	UNPCRelationshipComponent* RelComp = PlayerActor->FindComponentByClass<UNPCRelationshipComponent>();
	if (!RelComp)
	{
		return FGameplayTag();
	}

	return RelComp->GetCurrentState(NPCActor);
}

FText URelationshipNPCEntryWidget::GetStateDisplayName() const
{
	FGameplayTag StateTag = GetCurrentState();
	
	if (!StateTag.IsValid())
	{
		return FText::FromString(TEXT("Stranger"));
	}

	// Try to get display name from database
	if (NPCActor && PlayerActor)
	{
		if (UNPCRelationshipComponent* RelComp = PlayerActor->FindComponentByClass<UNPCRelationshipComponent>())
		{
			if (URelationshipSubsystem* Subsystem = RelComp->GetRelationshipSubsystem())
			{
				if (URelationshipDatabase* Database = Subsystem->GetDatabase())
				{
					if (URelationshipState* State = Database->FindState(StateTag))
					{
						return State->GetDisplayName();
					}
				}
			}
		}
	}

	// Fallback to tag name
	return FText::FromString(StateTag.ToString());
}

float URelationshipNPCEntryWidget::GetPreviewDimensionValue() const
{
	if (!NPCActor || !PlayerActor || !PreviewDimension.IsValid())
	{
		return 0.0f;
	}

	UNPCRelationshipComponent* RelComp = PlayerActor->FindComponentByClass<UNPCRelationshipComponent>();
	if (!RelComp)
	{
		return 0.0f;
	}

	return RelComp->GetDimensionValue(NPCActor, PreviewDimension);
}

float URelationshipNPCEntryWidget::GetPreviewDimensionNormalized() const
{
	float Value = GetPreviewDimensionValue();
	
	// Assume dimension range is 0-100
	// TODO: Get actual range from dimension asset
	return FMath::Clamp(Value / 100.0f, 0.0f, 1.0f);
}

void URelationshipNPCEntryWidget::OnButtonClicked()
{
	if (NPCActor)
	{
		UE_LOG(LogTemp, Log, TEXT("RelationshipNPCEntryWidget: Button clicked for NPC: %s"), *NPCActor->GetName());
		OnNPCClicked.Broadcast(NPCActor);
	}
}
