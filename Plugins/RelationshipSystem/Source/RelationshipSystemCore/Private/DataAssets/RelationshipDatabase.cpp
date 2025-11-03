// Copyright Epic Games, Inc. All Rights Reserved.

#include "DataAssets/RelationshipDatabase.h"
#include "DataAssets/RelationshipDimension.h"
#include "DataAssets/RelationshipState.h"
#include "DataAssets/RelationshipTrait.h"
#include "DataAssets/RelationshipRule.h"
#include "DataAssets/RelationshipAction.h"
#include "DataAssets/RelationshipProfile.h"
#include "RelationshipSystemCore.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

URelationshipDatabase::URelationshipDatabase()
{
}

void URelationshipDatabase::BuildIndices()
{
	ClearIndices();

	UE_LOG(LogRelationshipSystem, Log, TEXT("Building Relationship Database indices..."));

	BuildIndexForType(Dimensions, DimensionMap);
	BuildIndexForType(States, StateMap);
	BuildIndexForType(Traits, TraitMap);
	BuildIndexForType(Rules, RuleMap);
	BuildIndexForType(Actions, ActionMap);
	BuildIndexForType(Profiles, ProfileMap);

	UE_LOG(LogRelationshipSystem, Log, TEXT("Database indexed: %d dimensions, %d states, %d traits, %d rules, %d actions, %d profiles"),
		DimensionMap.Num(), StateMap.Num(), TraitMap.Num(), RuleMap.Num(), ActionMap.Num(), ProfileMap.Num());
}

void URelationshipDatabase::ClearIndices()
{
	DimensionMap.Empty();
	StateMap.Empty();
	TraitMap.Empty();
	RuleMap.Empty();
	ActionMap.Empty();
	ProfileMap.Empty();
}

template<typename T>
void URelationshipDatabase::BuildIndexForType(const TArray<TSoftObjectPtr<T>>& Assets, TMap<FGameplayTag, TObjectPtr<T>>& OutMap)
{
	for (const TSoftObjectPtr<T>& AssetPtr : Assets)
	{
		if (T* Asset = AssetPtr.LoadSynchronous())
		{
			FGameplayTag Tag = Asset->GetAssetTag();
			if (Tag.IsValid())
			{
				if (OutMap.Contains(Tag))
				{
					UE_LOG(LogRelationshipSystem, Warning, TEXT("Duplicate asset tag found: %s"), *Tag.ToString());
				}
				OutMap.Add(Tag, Asset);
			}
			else
			{
				UE_LOG(LogRelationshipSystem, Warning, TEXT("Asset has invalid tag: %s"), *Asset->GetName());
			}
		}
	}
}

URelationshipDimension* URelationshipDatabase::FindDimension(FGameplayTag Tag) const
{
	if (const TObjectPtr<URelationshipDimension>* Found = DimensionMap.Find(Tag))
	{
		return *Found;
	}
	return nullptr;
}

URelationshipState* URelationshipDatabase::FindState(FGameplayTag Tag) const
{
	if (const TObjectPtr<URelationshipState>* Found = StateMap.Find(Tag))
	{
		return *Found;
	}
	return nullptr;
}

URelationshipTrait* URelationshipDatabase::FindTrait(FGameplayTag Tag) const
{
	if (const TObjectPtr<URelationshipTrait>* Found = TraitMap.Find(Tag))
	{
		return *Found;
	}
	return nullptr;
}

URelationshipRule* URelationshipDatabase::FindRule(FGameplayTag Tag) const
{
	if (const TObjectPtr<URelationshipRule>* Found = RuleMap.Find(Tag))
	{
		return *Found;
	}
	return nullptr;
}

URelationshipAction* URelationshipDatabase::FindAction(FGameplayTag Tag) const
{
	if (const TObjectPtr<URelationshipAction>* Found = ActionMap.Find(Tag))
	{
		return *Found;
	}
	return nullptr;
}

URelationshipProfile* URelationshipDatabase::FindProfile(FGameplayTag Tag) const
{
	if (const TObjectPtr<URelationshipProfile>* Found = ProfileMap.Find(Tag))
	{
		return *Found;
	}
	return nullptr;
}

bool URelationshipDatabase::ValidateDatabase(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	// Validate all dimensions
	for (const TSoftObjectPtr<URelationshipDimension>& DimPtr : Dimensions)
	{
		if (URelationshipDimension* Dim = DimPtr.LoadSynchronous())
		{
			if (!Dim->Validate(OutErrors))
			{
				bIsValid = false;
			}
		}
	}

	// Validate all states
	for (const TSoftObjectPtr<URelationshipState>& StatePtr : States)
	{
		if (URelationshipState* State = StatePtr.LoadSynchronous())
		{
			if (!State->Validate(OutErrors))
			{
				bIsValid = false;
			}
		}
	}

	// Validate all traits
	for (const TSoftObjectPtr<URelationshipTrait>& TraitPtr : Traits)
	{
		if (URelationshipTrait* Trait = TraitPtr.LoadSynchronous())
		{
			if (!Trait->Validate(OutErrors))
			{
				bIsValid = false;
			}
		}
	}

	// Check for default profile
	if (!DefaultProfile.IsValid())
	{
		OutErrors.Add(FText::FromString(TEXT("No default profile set")));
		bIsValid = false;
	}

	return bIsValid;
}

TArray<URelationshipDimension*> URelationshipDatabase::GetAllDimensions() const
{
	TArray<URelationshipDimension*> Result;
	Result.Reserve(DimensionMap.Num());
	for (const auto& Pair : DimensionMap)
	{
		Result.Add(Pair.Value);
	}
	return Result;
}

TArray<URelationshipState*> URelationshipDatabase::GetAllStates() const
{
	TArray<URelationshipState*> Result;
	Result.Reserve(StateMap.Num());
	for (const auto& Pair : StateMap)
	{
		Result.Add(Pair.Value);
	}
	return Result;
}

#if WITH_EDITOR
void URelationshipDatabase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Rebuild indices when database changes
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		BuildIndices();
	}
}

EDataValidationResult URelationshipDatabase::IsDataValid(FDataValidationContext& Context) const
{
	TArray<FText> ValidationErrors;
	
	if (!ValidateDatabase(ValidationErrors))
	{
		for (const FText& Error : ValidationErrors)
		{
			Context.AddError(Error);
		}
		return EDataValidationResult::Invalid;
	}

	return Super::IsDataValid(Context);
}
#endif
