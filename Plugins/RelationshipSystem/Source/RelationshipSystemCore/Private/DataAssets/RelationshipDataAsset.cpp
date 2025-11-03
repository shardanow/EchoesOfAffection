// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/RelationshipDataAsset.h"
#include "RelationshipSystemCore.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

URelationshipDataAsset::URelationshipDataAsset()
	: DisplayColor(FLinearColor::White)
{
}

bool URelationshipDataAsset::Validate(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	// Check that AssetTag is valid
	if (!AssetTag.IsValid())
	{
		OutErrors.Add(FText::FromString(TEXT("Asset Tag is not set or invalid")));
		bIsValid = false;
	}

	// Check for display name
	if (DisplayName.IsEmpty())
	{
		OutErrors.Add(FText::FromString(TEXT("Display Name is empty")));
		// Not critical, just a warning
	}

	// Call child class validation
	if (!ValidateInternal(OutErrors))
	{
		bIsValid = false;
	}

	return bIsValid;
}

FText URelationshipDataAsset::GetDisplayName() const
{
	if (!DisplayName.IsEmpty())
	{
		return DisplayName;
	}

	// Fallback to tag name
	if (AssetTag.IsValid())
	{
		return FText::FromString(AssetTag.ToString());
	}

	return FText::FromString(TEXT("Unnamed Asset"));
}

#if WITH_EDITOR
void URelationshipDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Notify system that data asset changed (for hot-reload)
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		UE_LOG(LogRelationshipSystem, Verbose, TEXT("Data Asset modified: %s"), *GetName());
		// Hot-reload notification will be handled by editor module
	}
}

EDataValidationResult URelationshipDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	TArray<FText> ValidationErrors;
	
	if (!Validate(ValidationErrors))
	{
		for (const FText& Error : ValidationErrors)
		{
			Context.AddError(Error);
		}
		return EDataValidationResult::Invalid;
	}

	if (ValidationErrors.Num() > 0)
	{
		// Had warnings but still valid
		for (const FText& Warning : ValidationErrors)
		{
			Context.AddWarning(Warning);
		}
	}

	return Super::IsDataValid(Context);
}
#endif
