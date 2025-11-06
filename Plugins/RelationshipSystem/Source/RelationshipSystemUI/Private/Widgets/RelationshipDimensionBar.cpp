// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/RelationshipDimensionBar.h"
#include "Components/NPCRelationshipComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Subsystems/RelationshipSubsystem.h"
#include "DataAssets/RelationshipDatabase.h"
#include "DataAssets/RelationshipDimension.h"

float URelationshipDimensionBar::GetDimensionValue() const
{
	if (!Subject || !Target || !DimensionTag.IsValid())
	{
		return 0.0f;
	}

	// ? FIX: Use GameInstance->GetSubsystem for GameInstanceSubsystem
	UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return 0.0f;
	}

	URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
	if (!Subsystem)
	{
		return 0.0f;
	}

	// ? Subsystem works globally, no need for actor relevancy!
	return Subsystem->GetDimensionValue(Subject, Target, DimensionTag);
}

float URelationshipDimensionBar::GetNormalizedValue() const
{
	float RawValue = GetDimensionValue();

	if (!bNormalizeValue)
	{
		return RawValue;
	}

	// ? FIX: Use GameInstance subsystem
	UWorld* World = GetWorld();
	if (!World)
	{
		return 0.5f;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return 0.5f;
	}

	URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
	if (!Subsystem)
	{
		return 0.5f; // Default middle value
	}

	URelationshipDatabase* Database = Subsystem->GetDatabase();
	if (!Database)
	{
		return 0.5f;
	}

	URelationshipDimension* Dimension = Database->FindDimension(DimensionTag);
	if (!Dimension)
	{
		return 0.5f;
	}

	return Dimension->NormalizeValue(RawValue);
}

FText URelationshipDimensionBar::GetDimensionDisplayName() const
{
	if (!DimensionTag.IsValid())
	{
		return FText::FromString(TEXT("Unknown"));
	}

	// ? FIX: Use GameInstance subsystem
	UWorld* World = GetWorld();
	if (World)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>())
			{
				if (URelationshipDatabase* Database = Subsystem->GetDatabase())
				{
					if (URelationshipDimension* Dimension = Database->FindDimension(DimensionTag))
					{
						return Dimension->GetDisplayName();
					}
				}
			}
		}
	}

	// Fallback to tag name
	return FText::FromString(DimensionTag.ToString());
}

void URelationshipDimensionBar::NativeRefreshDisplay()
{
	Super::NativeRefreshDisplay();

	UE_LOG(LogTemp, Log, TEXT("RelationshipDimensionBar::NativeRefreshDisplay - START"));
	UE_LOG(LogTemp, Log, TEXT("  DimensionTag: %s"), *DimensionTag.ToString());
	UE_LOG(LogTemp, Log, TEXT("  ProgressBar: %s"), ProgressBar ? TEXT("Valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  ValueText: %s"), ValueText ? TEXT("Valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  NameText: %s"), NameText ? TEXT("Valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Log, TEXT("  bShowNumericValue: %s"), bShowNumericValue ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Log, TEXT("  bShowDimensionName: %s"), bShowDimensionName ? TEXT("TRUE") : TEXT("FALSE"));

	if (!DimensionTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("  ABORTED: Invalid DimensionTag"));
		return;
	}

	float RawValue = GetDimensionValue();
	float NormalizedValue = GetNormalizedValue();
	
	UE_LOG(LogTemp, Log, TEXT("  RawValue: %.2f"), RawValue);
	UE_LOG(LogTemp, Log, TEXT("  NormalizedValue: %.2f"), NormalizedValue);

	// Update progress bar
	if (ProgressBar)
	{
		// FIXED: Display only normalized value for proper percentage
		float DisplayValue;
		
		if (bNormalizeValue)
		{
			// If value is normalized, display 0%
			// If normalized, display from 0% to 100%
			DisplayValue = FMath::Max(0.0f, NormalizedValue);
		}
		else
		{
			DisplayValue = FMath::Clamp(RawValue / 100.0f, 0.0f, 1.0f);
		}
		
		ProgressBar->SetPercent(DisplayValue);
		UE_LOG(LogTemp, Log, TEXT("  ProgressBar->SetPercent(%.2f)"), DisplayValue);
	}

	// Update value text
	if (ValueText && bShowNumericValue)
	{
		FText ValueTextContent = FText::AsNumber(FMath::RoundToInt(RawValue));
		ValueText->SetText(ValueTextContent);
		ValueText->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("  ValueText->SetText('%s'), Visibility=Visible"), *ValueTextContent.ToString());
	}
	else if (ValueText)
	{
		ValueText->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("  ValueText->SetVisibility(Collapsed)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  ValueText is NULL!"));
	}

	// Update name text
	if (NameText && bShowDimensionName)
	{
		FText DimensionName = GetDimensionDisplayName();
		NameText->SetText(DimensionName);
		NameText->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("  NameText->SetText('%s'), Visibility=Visible"), *DimensionName.ToString());
	}
	else if (NameText)
	{
		NameText->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("  NameText->SetVisibility(Collapsed)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  NameText is NULL!"));
	}

	UE_LOG(LogTemp, Log, TEXT("RelationshipDimensionBar::NativeRefreshDisplay - FINISHED"));
}
